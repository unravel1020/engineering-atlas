// OpenMachineLab demo entry point.
//
// Usage:
//   oml_demo <registry.json> <model_name> <input_path> [output_path]
//
// Supports both image files and video files. For video, an optional output_path
// can be provided to write the processed frames back to disk.

#include "core/Application.h"
#include "pipeline/FrameData.h"
#include "pipeline/SequentialPipeline.h"
#include <cctype>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

namespace {

void printUsage(const char *program) {
  std::cerr << "Usage: " << program
            << " <registry.json> <model_name> <input_path> [output_path]\n";
}

bool isImageFile(const std::string &path) {
  std::string ext;
  size_t dot = path.find_last_of('.');
  if (dot != std::string::npos) {
    ext = path.substr(dot + 1);
  }
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp" ||
         ext == "tiff" || ext == "webp";
}

bool isVideoFile(const std::string &path) {
  std::string ext;
  size_t dot = path.find_last_of('.');
  if (dot != std::string::npos) {
    ext = path.substr(dot + 1);
  }
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return ext == "mp4" || ext == "avi" || ext == "mkv" || ext == "mov" ||
         ext == "webm" || ext == "flv";
}

void runImage(oml::core::Application &app, oml::pipeline::SequentialPipeline &pipeline,
              const std::string &input_path) {
  cv::Mat image = cv::imread(input_path);
  if (image.empty()) {
    std::cerr << "Failed to load image: " << input_path << "\n";
    std::exit(1);
  }

  auto frame = std::make_shared<oml::pipeline::FrameData>();
  frame->frame_id = 0;
  frame->source = input_path;
  frame->original_image = image;

  auto result = pipeline.run(frame);
  if (!result) {
    std::cerr << "Pipeline returned null result\n";
    std::exit(1);
  }

  std::cout << result->result_json << "\n";
}

void runVideo(oml::core::Application &app, oml::pipeline::SequentialPipeline &pipeline,
              const std::string &input_path, const std::string &output_path) {
  cv::VideoCapture capture(input_path);
  if (!capture.isOpened()) {
    std::cerr << "Failed to open video: " << input_path << "\n";
    std::exit(1);
  }

  const int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
  const double fps = capture.get(cv::CAP_PROP_FPS);
  const int frame_width = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
  const int frame_height = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));

  cv::VideoWriter writer;
  if (!output_path.empty()) {
    writer.open(output_path, fourcc, fps > 0 ? fps : 30.0,
                cv::Size(frame_width, frame_height));
    if (!writer.isOpened()) {
      std::cerr << "Failed to open video writer: " << output_path << "\n";
      std::exit(1);
    }
  }

  cv::Mat image;
  uint64_t frame_id = 0;
  uint64_t processed = 0;

  while (capture.read(image)) {
    auto frame = std::make_shared<oml::pipeline::FrameData>();
    frame->frame_id = frame_id++;
    frame->source = input_path;
    frame->original_image = image.clone();

    auto result = pipeline.run(frame);
    if (result && !result->result_json.empty()) {
      std::cout << "frame " << result->frame_id << ": " << result->result_json
                << "\n";
      ++processed;
    }

    if (writer.isOpened()) {
      writer.write(image);
    }
  }

  std::cerr << "Processed " << processed << " frames from " << input_path
            << "\n";
  if (!output_path.empty()) {
    std::cerr << "Wrote output video to " << output_path << "\n";
  }
}

} // namespace

int main(int argc, char **argv) {
  if (argc < 4) {
    printUsage(argv[0]);
    return 1;
  }

  std::string registry_path = argv[1];
  std::string model_name = argv[2];
  std::string input_path = argv[3];
  std::string output_path = argc > 4 ? argv[4] : "";

  try {
    auto app = oml::core::Application::create(registry_path);
    if (!app->initialize()) {
      std::cerr << "Failed to initialize application\n";
      return 1;
    }

    auto pipeline = app->createPipeline(model_name);

    if (isImageFile(input_path)) {
      runImage(*app, *pipeline, input_path);
    } else if (isVideoFile(input_path)) {
      runVideo(*app, *pipeline, input_path, output_path);
    } else {
      std::cerr << "Unsupported input type: " << input_path << "\n";
      return 1;
    }

    app->shutdown();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
