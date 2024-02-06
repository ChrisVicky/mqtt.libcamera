#include "camera.hpp"
CameraClient::CameraClient() {

  // 0. Mute Log
  logSetTarget(libcamera::LoggingTargetFile);
  logSetFile("libcamera.log", false);

  // 1. Start Sub threading
  loop_.start();

  // 2. Start Camera
  cameraManager_ = std::make_unique<libcamera::CameraManager>();
  cameraManager_->start();
  for (auto const &camera : cameraManager_->cameras())
    std::cout << " - " << cameraName(camera.get()) << std::endl;

  if (cameraManager_->cameras().empty()) {
    logger->error("No cameras were identified on the system.");
    cameraManager_->stop();
    exit(EXIT_FAILURE);
  }
  camera_ = cameraManager_->cameras()[0];
  camera_->acquire();

  config_ = camera_->generateConfiguration({StreamRole::Viewfinder});
  config_->validate();

  camera_->configure(config_.get());

  allocator_ = new FrameBufferAllocator(camera_);
  for (StreamConfiguration &cfg : *config_) {
    int ret = allocator_->allocate(cfg.stream());
    if (ret < 0) {
      std::cerr << "Can't allocate buffers" << std::endl;
      exit(EXIT_FAILURE);
    }
    size_t allocated = allocator_->buffers(cfg.stream()).size();
    logger->info("Allocated {} buffers for stream", allocated);
  }

  StreamConfiguration streamConfig = config_->at(0);
  stream_ = streamConfig.stream();
  const std::vector<std::unique_ptr<FrameBuffer>> &buffers =
      allocator_->buffers(stream_);

  for (unsigned int i = 0; i < buffers.size(); ++i) {
    std::unique_ptr<Request> request = camera_->createRequest();
    if (!request) {
      logger->error("Can't create request");
      exit(EXIT_FAILURE);
    }

    const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
    int ret = request->addBuffer(stream_, buffer.get());
    if (ret < 0) {
      logger->error("Can't set buffer for request");
      exit(EXIT_FAILURE);
    }

    ControlList &controls = request->controls();
    controls.set(controls::Brightness, 0.5);

    requests_.push_back(std::move(request));
  }

  camera_->requestCompleted.connect(this, [this](Request *r) {
    if (r->status() == Request::RequestCancelled)
      return;

    loop_.callLater([=]() {
      logger->trace("Request Completed: {}", r->toString());

      const ControlList &requestMetadata = r->metadata();
      for (const auto &ctrl : requestMetadata) {
        const ControlId *id = controls::controls.at(ctrl.first);
        const ControlValue &value = ctrl.second;
        logger->trace("\t{} = {}", id->name(), value.toString());
      }

      // Genrally, we assign only one stream at a time but multiple is the
      // model-defined
      const Request::BufferMap &buffers = r->buffers();
      for (auto bufferPair : buffers) {
        Stream const *stream = bufferPair.first;
        FrameBuffer const *buffer = bufferPair.second;

        processFrameBuffer(stream, buffer);
      }

      /* Re-queue the Request to the camera. */
      r->reuse(Request::ReuseBuffers);
      camera_->queueRequest(r);
    });
  });

  camera_->start();
  for (std::unique_ptr<Request> &request : requests_)
    camera_->queueRequest(request.get());
}

void CameraClient::processFrameBuffer(const Stream *stream,
                                      const FrameBuffer *frameBuffer) {
  const StreamConfiguration &streamConfig = stream->configuration();

  // Pixel Format shall later determine the transformation function
  const PixelFormat &pixelFormat = streamConfig.pixelFormat;

  int width = streamConfig.size.width;
  int height = streamConfig.size.height;

  // Map FrameBuffer For access
  FrameBuffer::Plane plane = frameBuffer->planes().front();
  void *memory =
      mmap(nullptr, plane.length, PROT_READ, MAP_SHARED, plane.fd.get(), 0);

  if (memory == MAP_FAILED) {
    std::cerr << "Failed to map memory" << std::endl;
    return;
  }

  const uint8_t *src = static_cast<uint8_t *>(memory);

  {
    std::unique_lock<std::mutex> locker(imageLock_);
    rgbData_.clear();
    logger->debug("format {}", pixelFormat);
    if (pixelFormat == PixelFormat::fromString("XRGB8888")) {
      convertXRGB8888ToRGB(src, rgbData_, width, height);
    } else if (pixelFormat == PixelFormat::fromString("MJPEG")) {
      convertMJPEGToRGB(src, rgbData_, width, height);
    }
    logger->debug("Convert {} to RGB ({}byte)", pixelFormat.toString(),
                  rgbData_.size());
    size_ = stream->configuration().size;
  }

  munmap(memory, plane.length);
}

std::vector<uint8_t> CameraClient::GetImage() {
  std::unique_lock<std::mutex> locker(imageLock_);
  return rgbData_;
}

std::vector<uint8_t> CameraClient::GetImageMqtt() {
  auto ret = GetImage();
  logger->trace("Size: {} x {} (hxw)", size_.height, size_.width);
  int width = size_.width;
  int height = size_.height;
  ret.push_back(width & 0xFF);         // 宽度的低字节
  ret.push_back((width >> 8) & 0xFF);  // 宽度的高字节
  ret.push_back(height & 0xFF);        // 高度的低字节
  ret.push_back((height >> 8) & 0xFF); // 高度的高字节
  return ret;
}
