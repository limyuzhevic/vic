#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "../Validation/ValidationSystem.hpp"

namespace nlm {

// Enhanced log severity levels with validation status
enum class LogLevel {
  Debug = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Critical = 4,
  Validation = 5  // Special level for validation events
};

// Enhanced log entry with context information
struct LogEntry {
  LogLevel level;
  std::string message;
  std::string file;
  int line;
  std::string function;
  std::chrono::system_clock::time_point timestamp;
  std::string component; // Component that generated the log
  std::string category; // Category for filtering (validation, state, error, etc.)
  std::unordered_map<std::string, std::string> metadata; // Additional context data
  bool isValidationFailure = false; // Flag for validation failures
  std::string validationParameter; // Parameter name for validation failures
  std::string stackTrace; // Optional stack trace for errors
  uint64_t threadId; // Thread ID for concurrency tracking
  
  LogEntry()
      : level(LogLevel::Info), message(), file(), line(0), function(), 
        timestamp(), component(), category(), metadata(), isValidationFailure(false),
        validationParameter(), stackTrace(), threadId(0) {}
  
  LogEntry(LogLevel lvl, const std::string &msg, const std::string &file = "", 
           int line = 0, const std::string &function = "",
           const std::string &component = "",
           const std::string &category = "general")
      : level(lvl), message(msg), file(file), line(line), function(function),
        timestamp(std::chrono::system_clock::now()), component(component),
        category(category), isValidationFailure(false), validationParameter(), 
        stackTrace(), threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {}
};

// Performance monitoring data
struct PerformanceMetrics {
  uint64_t validationCount = 0;
  uint64_t validationFailureCount = 0;
  double validationFailureRate = 0.0;
  std::chrono::system_clock::time_point lastValidationTime;
  std::chrono::system_clock::time_point lastValidationFailureTime;
  uint64_t errorCount = 0;
  uint64_t warningCount = 0;
  std::unordered_map<std::string, uint64_t> componentErrorCounts;
  std::unordered_map<std::string, uint64_t> componentWarningCounts;
  
  PerformanceMetrics() : lastValidationTime(), lastValidationFailureTime() {}
};

// Enhanced ILogger interface with validation tracking
class ILogger {
public:
  virtual ~ILogger() = default;
  virtual void log(const LogEntry &entry) = 0;
  virtual void setLevel(LogLevel level) = 0;
  virtual LogLevel getLevel() const = 0;
  virtual void setCategoryFilter(const std::unordered_set<std::string> &categories) = 0;
  virtual std::unordered_set<std::string> getCategoryFilter() const = 0;
  virtual void enableValidationTracking() = 0;
  virtual void disableValidationTracking() = 0;
  virtual bool isValidationTrackingEnabled() const = 0;
  virtual PerformanceMetrics getPerformanceMetrics() const = 0;
  virtual void resetPerformanceMetrics() = 0;
};

// Enhanced console logger with validation tracking
class ConsoleLogger : public ILogger {
public:
  ConsoleLogger();
  explicit ConsoleLogger(LogLevel level);
  ~ConsoleLogger() override;

  void log(const LogEntry &entry) override;
  void setLevel(LogLevel level) override;
  LogLevel getLevel() const override;
  void setCategoryFilter(const std::unordered_set<std::string> &categories) override;
  std::unordered_set<std::string> getCategoryFilter() const override;
  void enableValidationTracking() override;
  void disableValidationTracking() override;
  bool isValidationTrackingEnabled() const override;
  PerformanceMetrics getPerformanceMetrics() const override;
  void resetPerformanceMetrics() override;

  // Enable/disable colors
  void setUseColors(bool useColors);
  bool getUseColors() const;

  // Set output file (in addition to console)
  void setOutputFile(const std::string &filepath);
  void closeOutputFile();

  // Format log entry for output
  std::string formatLogEntry(const LogEntry &entry) const;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;

  std::string levelToString(LogLevel level) const;
  std::string levelToColor(LogLevel level) const;
  std::string formatMetadata(const std::unordered_map<std::string, std::string> &metadata) const;
};

// Enhanced file logger for persistent logging
class FileLogger : public ILogger {
public:
  FileLogger(const std::string &filepath, bool append = true);
  ~FileLogger() override;

  void log(const LogEntry &entry) override;
  void setLevel(LogLevel level) override;
  LogLevel getLevel() const override;
  void setCategoryFilter(const std::unordered_set<std::string> &categories) override;
  std::unordered_set<std::string> getCategoryFilter() const override;
  void enableValidationTracking() override;
  void disableValidationTracking() override;
  bool isValidationTrackingEnabled() const override;
  PerformanceMetrics getPerformanceMetrics() const override;
  void resetPerformanceMetrics() override;

  // Flush log file
  void flush();

  // Get current file size
  size_t getFileSize() const;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

// Main enhanced Logger class with validation integration
class Logger {
public:
  Logger();
  ~Logger();

  // Disable copying
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  // Add logger sink
  void addLogger(std::shared_ptr<ILogger> logger);

  // Remove all loggers
  void clearLoggers();

  // Set minimum level for all loggers
  void setLevel(LogLevel level);

  // Set category filter for all loggers
  void setCategoryFilter(const std::unordered_set<std::string> &categories);

  // Enable/disable validation tracking
  void enableValidationTracking();
  void disableValidationTracking();
  bool isValidationTrackingEnabled() const;

  // Get global performance metrics
  PerformanceMetrics getGlobalPerformanceMetrics() const;

  // Reset global performance metrics
  void resetGlobalPerformanceMetrics();

  // Global logging functions with enhanced context
  void log(LogLevel level, const std::string &message,
           const std::string &file = "", int line = 0,
           const std::string &function = "",
           const std::string &component = "",
           const std::string &category = "general",
           const std::unordered_map<std::string, std::string> &metadata = {});

  // Convenience methods with component/category
  void debug(const std::string &message, const std::string &file = "",
             int line = 0, const std::string &function = "",
             const std::string &component = "",
             const std::string &category = "debug");
  void info(const std::string &message, const std::string &file = "",
            int line = 0, const std::string &function = "",
            const std::string &component = "",
            const std::string &category = "info");
  void warning(const std::string &message, const std::string &file = "",
               int line = 0, const std::string &function = "",
               const std::string &component = "",
               const std::string &category = "warning");
  void error(const std::string &message, const std::string &file = "",
             int line = 0, const std::string &function = "",
             const std::string &component = "",
             const std::string &category = "error");
  void critical(const std::string &message, const std::string &file = "",
                int line = 0, const std::string &function = "",
                const std::string &component = "",
                const std::string &category = "critical");

  // Validation-specific logging
  void logValidationFailure(const std::string &message, const std::string &parameter,
                           const std::string &component = "",
                           const std::string &file = "", int line = 0,
                           const std::string &function = "");
  void logValidationSuccess(const std::string &message,
                           const std::string &component = "",
                           const std::string &file = "", int line = 0,
                           const std::string &function = "");

  // Check if level is enabled
  bool isEnabled(LogLevel level) const;

  // Get global logger instance
  static Logger &getGlobal();

  // Set global logger
  static void setGlobal(std::shared_ptr<Logger> logger);

  // Get validation system reference
  static ValidationSystem &getValidationSystem();

  // Convenience macro for file/line/function info
  #define NLM_LOG_DEBUG(msg)                                                     \
    nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

  #define NLM_LOG_INFO(msg)                                                      \
    nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

  #define NLM_LOG_WARNING(msg)                                                   \
    nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

  #define NLM_LOG_ERROR(msg)                                                     \
    nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

  #define NLM_LOG_CRITICAL(msg)                                                  \
    nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

  // Enhanced macros with component and category
  #define NLM_LOG_DEBUG_C(msg, comp, cat)                                         \
    nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, comp, cat)

  #define NLM_LOG_INFO_C(msg, comp, cat)                                          \
    nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, comp, cat)

  #define NLM_LOG_WARNING_C(msg, comp, cat)                                       \
    nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, comp, cat)

  #define NLM_LOG_ERROR_C(msg, comp, cat)                                         \
    nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, comp, cat)

  #define NLM_LOG_CRITICAL_C(msg, comp, cat)                                      \
    nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, comp, cat)

  #define NLM_LOG_VALIDATION_FAILURE(msg, param, comp)                           \
    nlm::Logger::getGlobal().logValidationFailure(msg, param, comp, __FILE__, __LINE__, __func__)

  #define NLM_LOG_VALIDATION_SUCCESS(msg, comp)                                  \
    nlm::Logger::getGlobal().logValidationSuccess(msg, comp, __FILE__, __LINE__, __func__)

  // Stream-style logging helper with enhanced context
  class LogStream {
  public:
    LogStream(LogLevel level, const std::string &file, int line,
              const std::string &function, const std::string &component = "",
              const std::string &category = "general");
    ~LogStream();

    template <typename T> LogStream &operator<<(const T &value) {
      oss_ << value;
      return *this;
    }

    // Add metadata to the log entry
    LogStream &withMetadata(const std::string &key, const std::string &value) {
      metadata_[key] = value;
      return *this;
    }

    // Add component and category
    LogStream &withComponent(const std::string &component) {
      component_ = component;
      return *this;
    }

    LogStream &withCategory(const std::string &category) {
      category_ = category;
      return *this;
    }

  private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string component_;
    std::string category_;
    std::unordered_map<std::string, std::string> metadata_;
  };

  #define NLM_LOG_STREAM(level)                                                  \
    nlm::LogStream(level, __FILE__, __LINE__, __func__)

  #define NLM_LOG_STREAM_C(level, comp, cat)                                      \
    nlm::LogStream(level, __FILE__, __LINE__, __func__, comp, cat)

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
  
  // Get current thread ID
  static uint64_t getCurrentThreadId() {
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
  }
};

// Global performance metrics function
inline PerformanceMetrics &getGlobalPerformanceMetrics() {
  static PerformanceMetrics metrics;
  return metrics;
}

} // namespace nlm