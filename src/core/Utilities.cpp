// NLM Utilities Implementation
// Implementation of utility classes and functions

#include "core/Utilities.hpp"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <random>

namespace nlm {

// StringUtils implementation
std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";
    
    std::stringstream result;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) result << delimiter;
        result << strings[i];
    }
    
    return result.str();
}

std::string StringUtils::format(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // Buffer size (adjust as needed)
    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    
    va_end(args);
    
    return std::string(buffer);
}

std::string StringUtils::timeToString(time_t time) {
    char buffer[64];
    struct tm tm;
    localtime_r(&time, &tm);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buffer);
}

std::string StringUtils::durationToString(double seconds) {
    if (seconds < 60.0) {
        return StringUtils::format("%.3f seconds", seconds);
    } else if (seconds < 3600.0) {
        double minutes = seconds / 60.0;
        if (minutes < 60.0) {
            return StringUtils::format("%.1f minutes", minutes);
        } else {
            double hours = minutes / 60.0;
            return StringUtils::format("%.2f hours", hours);
        }
    } else {
        double hours = seconds / 3600.0;
        return StringUtils::format("%.2f hours", hours);
    }
}

// MathUtils implementation
float MathUtils::clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

float MathUtils::lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float MathUtils::map(float value, float inMin, float inMax, float outMin, float outMax) {
    return outMin + (outMax - outMin) * ((value - inMin) / (inMax - inMin));
}

float MathUtils::sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

float MathUtils::tanh(float x) {
    return std::tanh(x);
}

float MathUtils::normalize(float value, float min, float max) {
    return (value - min) / (max - min);
}

float MathUtils::sqrtApprox(float x) {
    return std::sqrt(x);
}

float MathUtils::smoothStep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

bool MathUtils::isPowerOfTwo(size_t x) {
    return x > 0 && (x & (x - 1)) == 0;
}

size_t MathUtils::nextPowerOfTwo(size_t x) {
    size_t result = 1;
    while (result < x) result <<= 1;
    return result;
}

// PerformanceMonitor implementation
PerformanceMonitor::PerformanceMonitor() : elapsedTime_(0.0), cpuTime_(0.0) {}

void PerformanceMonitor::startTiming() {
    startTime_ = std::chrono::high_resolution_clock::now();
    cpuStartTime_ = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::stopTiming() {
    auto endTime = std::chrono::high_resolution_clock::now();
    elapsedTime_ = std::chrono::duration<double>(endTime - startTime_).count();
    cpuTime_ = std::chrono::duration<double>(endTime - cpuStartTime_).count();
}

double PerformanceMonitor::getElapsedTime() const {
    return elapsedTime_;
}

double PerformanceMonitor::getCPUTime() const {
    return cpuTime_;
}

PerformanceMonitor::Metrics PerformanceMonitor::getMetrics() const {
    Metrics metrics;
    metrics.cpuTime = cpuTime_;
    metrics.wallTime = elapsedTime_;
    // Note: Getting actual memory usage would require platform-specific code
    metrics.memoryUsage = 0;
    metrics.allocatedObjects = 0;
    metrics.lastFrameTime = 0.0;
    metrics.fps = 1.0 / elapsedTime_;
    return metrics;
}

void PerformanceMonitor::reset() {
    startTime_ = std::chrono::high_resolution_clock::now();
    cpuStartTime_ = std::chrono::high_resolution_clock::now();
    elapsedTime_ = 0.0;
    cpuTime_ = 0.0;
}

// ConfigParser implementation
std::map<std::string, std::string> ConfigParser::parseConfigFile(const std::string& filename) {
    std::map<std::string, std::string> config;
    if (!fileExists(filename)) return config;
    
    std::vector<std::string> lines = readLines(filename);
    for (const auto& line : lines) {
        std::string key, value;
        if (parseKeyValue(line, key, value)) {
            config[key] = value;
        }
    }
    
    return config;
}

bool ConfigParser::parseKeyValue(const std::string& line, std::string& key, std::string& value) {
    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos) {
        // Remove comment
        std::string lineWithoutComment = line.substr(0, commentPos);
        // Remove trailing whitespace
        lineWithoutComment.erase(lineWithoutComment.find_last_not_of(" \t\r\n") + 1);
    }
    
    size_t equalPos = line.find('=');
    if (equalPos == std::string::npos) return false;
    
    key = trim(line.substr(0, equalPos));
    value = trim(line.substr(equalPos + 1));
    
    return !key.empty();
}

std::string ConfigParser::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string ConfigParser::unescapeString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                case '\\': result += '\\'; i++; break;
                case '"': result += '"'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

std::vector<std::string> ConfigParser::readLines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    if (!file.is_open()) return lines;
    
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

bool ConfigParser::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// ProgressReporter implementation
ProgressReporter::ProgressReporter(size_t totalSteps, size_t reportInterval)
    : currentStep_(0), totalSteps_(totalSteps), reportInterval_(reportInterval), startTime_(std::chrono::high_resolution_clock::now()) {}

void ProgressReporter::increment() {
    ++currentStep_;
    if (currentStep_ % reportInterval_ == 0 || isComplete()) {
        report();
    }
}

void ProgressReporter::report(const std::string& message) {
    double progress = getProgress();
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime_).count();
    
    std::cout << "\rProgress: " << std::fixed << std::setprecision(1) << (progress * 100.0) << "% ";
    std::cout << "(" << currentStep_ << "/" << totalSteps_ << ") ";
    std::cout << "ETA: " << (progress > 0.0 ? (elapsed / progress - elapsed) : 0.0) << "s ";
    if (!message.empty()) {
        std::cout << "- " << message;
    }
    std::cout << std::flush;
}

void ProgressReporter::setStatus(const std::string& status) {
    status_ = status;
}

bool ProgressReporter::isComplete() const {
    return currentStep_ >= totalSteps_;
}

double ProgressReporter::getProgress() const {
    return static_cast<double>(currentStep_) / totalSteps_;
}

// RandomUtils implementation
static std::mt19937 rngEngine;

void RandomUtils::seedRandom(unsigned int seed) {
    rngEngine.seed(seed);
}

float RandomUtils::randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rngEngine);
}

double RandomUtils::randomDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rngEngine);
}

int RandomUtils::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rngEngine);
}

bool RandomUtils::randomBool(float probability) {
    return randomFloat(0.0f, 1.0f) < probability;
}

// ColorUtils implementation
void ColorUtils::setColor(Color color) {
    // ANSI color codes
    const char* codes[] = {
        "\033[31m", // RED
        "\033[32m", // GREEN
        "\033[33m", // YELLOW
        "\033[34m", // BLUE
        "\033[35m", // MAGENTA
        "\033[36m", // CYAN
        "\033[37m", // WHITE
        "\033[90m", // GRAY
        "\033[0m"   // RESET
    };
    
    if (color >= 0 && color < RESET) {
        std::cout << codes[color];
    }
}

std::string ColorUtils::getColorCode(Color color) {
    const char* codes[] = {
        "\033[31m", // RED
        "\033[32m", // GREEN
        "\033[33m", // YELLOW
        "\033[34m", // BLUE
        "\033[35m", // MAGENTA
        "\033[36m", // CYAN
        "\033[37m", // WHITE
        "\033[90m", // GRAY
        "\033[0m"   // RESET
    };
    
    if (color >= 0 && color < RESET) {
        return std::string(codes[color]);
    }
    return "";
}

std::string ColorUtils::formatWithColor(const std::string& text, Color color) {
    return getColorCode(color) + text + getColorCode(RESET);
}

std::string ColorUtils::getSuccessPrefix() {
    return formatWithColor("[SUCCESS] ", GREEN);
}

std::string ColorUtils::getErrorPrefix() {
    return formatWithColor("[ERROR] ", RED);
}

std::string ColorUtils::getWarningPrefix() {
    return formatWithColor("[WARNING] ", YELLOW);
}

std::string ColorUtils::getInfoPrefix() {
    return formatWithColor("[INFO] ", BLUE);
}

// TimeUtils implementation
std::string TimeUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto inTimeT = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    struct tm tm;
    localtime_r(&inTimeT, &tm);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buffer);
}

std::string TimeUtils::getRelativeTime(double seconds) {
    if (seconds < 60.0) {
        return StringUtils::format("%.1f seconds", seconds);
    } else if (seconds < 3600.0) {
        double minutes = seconds / 60.0;
        if (minutes < 60.0) {
            return StringUtils::format("%.1f minutes", minutes);
        } else {
            double hours = minutes / 60.0;
            return StringUtils::format("%.2f hours", hours);
        }
    } else {
        double hours = seconds / 3600.0;
        double minutes = (hours - (int)hours) * 60.0;
        return StringUtils::format("%d hours %.1f minutes", (int)hours, minutes);
    }
}

double TimeUtils::getCurrentTime() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(now.time_since_epoch()).count();
}

void TimeUtils::sleep(double seconds) {
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

std::chrono::high_resolution_clock::time_point TimeUtils::now() {
    return std::chrono::high_resolution_clock::now();
}

double TimeUtils::toMilliseconds(const std::chrono::high_resolution_clock::time_point& start) {
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double TimeUtils::toSeconds(const std::chrono::high_resolution_clock::time_point& start) {
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

// StreamUtils implementation
std::string StreamUtils::streamToString(std::istream& stream) {
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

void StreamUtils::writeToStream(std::ostream& stream, const std::string& data) {
    stream << data;
}

bool StreamUtils::isStreamGood(std::istream& stream) {
    return stream.good();
}

bool StreamUtils::isStreamGood(std::ostream& stream) {
    return stream.good();
}

void StreamUtils::safeClose(std::ofstream& file) {
    if (file.is_open()) {
        file.close();
    }
}

void StreamUtils::safeClose(std::ifstream& file) {
    if (file.is_open()) {
        file.close();
    }
}

} // namespace nlm