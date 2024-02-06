#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__
#include <libcamera/camera_manager.h>
#include <libcamera/framebuffer.h>
#include <libcamera/geometry.h>
#include <libcamera/libcamera.h>
#include <libcamera/pixel_format.h>
#include <libcamera/stream.h>
#include <memory>

#include <csetjmp>
#include <jpeglib.h>
#include <vector>

#include <sys/mman.h>

#include <spdlog/spdlog.h>

#include "../../loop/loop.hpp"

extern std::shared_ptr<spdlog::logger> logger;

using namespace libcamera;

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
  my_error_mgr *myerr = (my_error_mgr *)cinfo->err;
  (*cinfo->err->output_message)(cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

static void convertMJPEGToRGB(const uint8_t *src, std::vector<uint8_t> &dst,
                              int width, int height) {
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    return;
  }

  jpeg_create_decompress(&cinfo);

  jpeg_mem_src(&cinfo, src, width * height * 3);

  (void)jpeg_read_header(&cinfo, TRUE);
  (void)jpeg_start_decompress(&cinfo);

  int row_stride = cinfo.output_width * cinfo.output_components;

  dst.resize(cinfo.output_width * cinfo.output_height *
             cinfo.output_components);

  while (cinfo.output_scanline < cinfo.output_height) {
    uint8_t *buffer_array[1];
    buffer_array[0] = &dst[cinfo.output_scanline * row_stride];
    jpeg_read_scanlines(&cinfo, buffer_array, 1);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
}

static void convertXRGB8888ToRGB(const uint8_t *src, std::vector<uint8_t> &dst,
                                 int width, int height) {
  dst.clear();
  dst.reserve(width * height * 3);

  for (int i = 0; i < width * height; ++i) {
    src++;                 // Skip the padding byte
    dst.push_back(*src++); // Red
    dst.push_back(*src++); // Green
    dst.push_back(*src++); // Blue
  }
}

static std::string cameraName(Camera *camera) {
  const ControlList &props = camera->properties();
  std::string name;

  const auto &location = props.get(properties::Location);
  if (location) {
    switch (*location) {
    case properties::CameraLocationFront:
      name = "Internal front camera";
      break;
    case properties::CameraLocationBack:
      name = "Internal back camera";
      break;
    case properties::CameraLocationExternal:
      name = "External camera";
      const auto &model = props.get(properties::Model);
      if (model)
        name = " '" + *model + "'";
      break;
    }
  }

  name += " (" + camera->id() + ")";

  return name;
}

class CameraClient {

  std::unique_ptr<CameraManager> cameraManager_;
  std::shared_ptr<Camera> camera_;
  std::unique_ptr<CameraConfiguration> config_;
  FrameBufferAllocator *allocator_;

  Stream *stream_; // Stream for our use case
  std::vector<std::unique_ptr<Request>> requests_;
  Size size_;

  std::mutex imageLock_;
  std::vector<uint8_t> rgbData_;

  Loop loop_;

public:
  // Initialization Done
  CameraClient();
  void processFrameBuffer(const Stream *stream, const FrameBuffer *frameBuffer);
  std::vector<uint8_t> GetImage();
  std::vector<uint8_t> GetImageMqtt();
};
#endif // __CAMERA_HPP__
