#include "logger.h"
#include "test_framework.h"
#include <sstream>
#include <string>

TEST(logger_info_message_is_emitted_at_default_level) {
  std::ostringstream oss;
  logger::Logger::instance().setOutput(&oss);
  logger::Logger::instance().setLevel(logger::Level::Info);

  LOG_INFO("hello logger");

  std::string out = oss.str();
  ASSERT_TRUE(out.find("[INFO]") != std::string::npos);
  ASSERT_TRUE(out.find("hello logger") != std::string::npos);

  logger::Logger::instance().setOutput(nullptr);
}

TEST(logger_debug_message_is_filtered_at_info_level) {
  std::ostringstream oss;
  logger::Logger::instance().setOutput(&oss);
  logger::Logger::instance().setLevel(logger::Level::Info);

  LOG_DEBUG("debug detail");

  ASSERT_EQ(oss.str().find("debug detail"), std::string::npos);

  logger::Logger::instance().setOutput(nullptr);
}

TEST(logger_debug_message_is_emitted_at_debug_level) {
  std::ostringstream oss;
  logger::Logger::instance().setOutput(&oss);
  logger::Logger::instance().setLevel(logger::Level::Debug);

  LOG_DEBUG("debug detail");

  ASSERT_TRUE(oss.str().find("[DEBUG]") != std::string::npos);
  ASSERT_TRUE(oss.str().find("debug detail") != std::string::npos);

  logger::Logger::instance().setOutput(nullptr);
  logger::Logger::instance().setLevel(logger::Level::Info);
}

TEST(logger_error_message_is_emitted_at_info_level) {
  std::ostringstream oss;
  logger::Logger::instance().setOutput(&oss);
  logger::Logger::instance().setLevel(logger::Level::Info);

  LOG_ERROR("something failed");

  ASSERT_TRUE(oss.str().find("[ERROR]") != std::string::npos);
  ASSERT_TRUE(oss.str().find("something failed") != std::string::npos);

  logger::Logger::instance().setOutput(nullptr);
}
