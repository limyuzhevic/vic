#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <functional>

namespace nlm {
namespace cli {

// Forward declarations
class Command;
class CommandLine;

// CLI utility types
enum class LogLevel {
    Quiet,
    Error,
    Warning,
    Info,
    Debug,
    Verbose
};

struct ProgressInfo {
    std::string description;
    double progress; // 0.0 to 1.0
    std::chrono::system_clock::time_point startTime;
    bool showETA;
};

struct CLIResult {
    bool success;
    std::string message;
    int exitCode;
};

// Utility functions
namespace utils {

// String utilities
std::string trim(const std::string& str);
std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
std::string capitalize(const std::string& str);
std::string leftPad(const std::string& str, size_t width);
std::string centerText(const std::string& text, size_t width);
std::string rightPad(const std::string& str, size_t width);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

// Color output utilities
bool supportsColors();
void printColored(const std::string& text, const std::string& colorCode = "");
void printError(const std::string& message);
void printWarning(const std::string& message);
void printInfo(const std::string& message);
void printSuccess(const std::string& message);
void printDebug(const std::string& message);

// Progress and timing utilities
std::string formatDuration(std::chrono::duration<double> duration);
std::string formatMemorySize(size_t bytes);
void printProgress(const ProgressInfo& info);

// Configuration utilities
double parseFloat(const std::string& str);
int64_t parseInt(const std::string& str);
bool parseBool(const std::string& str);
std::string currentTimestamp();

// File utilities
bool fileExists(const std::string& path);
bool directoryExists(const std::string& path);
std::string getFileExtension(const std::string& filename);
std::string getFileName(const std::string& path);

} // namespace utils

} // namespace cli
} // namespace nlm