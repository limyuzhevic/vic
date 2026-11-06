// NLM Utility Functions
// Collection of helper functions for common operations

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <set>
#include <map>
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

namespace nlm {

// Forward declarations for types that might be referenced
struct NeuronId;
struct SynapseId;
struct RegionId;
struct PopulationId;
enum class NeuronType;
enum class SynapseType;
enum class DevelopmentalStage;
enum class FiringState;
enum class ActionType;
enum class MotorCommand;
enum class WorldObjectType;
struct SimulationStep;
struct Timestamp;
struct LogEntry;
struct Config;
struct ConfigValue;
enum class ConfigSource;
struct ConfigEntry;

// String utility functions
inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline bool stringContains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

// Numeric utility functions
inline double clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
}

inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

inline int clamp(int value, int min, int max) {
    return std::max(min, std::min(max, value));
}

inline bool isEven(int value) {
    return value % 2 == 0;
}

inline bool isOdd(int value) {
    return value % 2 != 0;
}

inline std::string formatTime(std::chrono::system_clock::time_point tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_time;
    gmtime_r(&time, &tm_time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Vector utility functions
inline std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

inline std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) oss << delimiter;
        oss << strings[i];
    }
    return oss.str();
}

inline float calculateMean(const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / values.size();
}

inline float calculateVariance(const std::vector<float>& values) {
    if (values.size() < 2) return 0.0f;
    float mean = calculateMean(values);
    float sum = 0.0f;
    for (float value : values) {
        sum += (value - mean) * (value - mean);
    }
    return sum / values.size();
}

inline float calculateStandardDeviation(const std::vector<float>& values) {
    return std::sqrt(calculateVariance(values));
}

// Color utility functions
inline std::string colorToHex(int r, int g, int b) {
    std::ostringstream oss;
    oss << "#" << std::hex << std::uppercase << std::setfill('0')
        << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b;
    return oss.str();
}

inline std::string colorToName(int r, int g, int b) {
    if (r > 200 && g > 200 && b > 200) return "white";
    if (r < 50 && g < 50 && b < 50) return "black";
    if (r > 150 && g < 100 && b < 100) return "red";
    if (g > 150 && r < 100 && b < 100) return "green";
    if (b > 150 && r < 100 && g < 100) return "blue";
    if (r > 200 && g > 200 && b < 100) return "yellow";
    if (r > 200 && b > 200 && g < 100) return "cyan";
    if (g > 200 && b > 200 && r < 100) return "magenta";
    return "unknown";
}

// Simulation utility functions
inline double calculateTimestamp(SimulationStep step, double timestep) {
    return static_cast<double>(step) * timestep;
}

inline SimulationStep calculateStep(double timestamp, double timestep) {
    return static_cast<SimulationStep>(timestamp / timestep);
}

// Performance monitoring utilities
class PerformanceMonitor {
public:
    PerformanceMonitor(const std::string& name) : name_(name), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~PerformanceMonitor() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        NLM_LOG_INFO("Performance " + name_ + ": " + std::to_string(duration.count()) + " ms");
    }
    
    void logCurrentTime(const std::string& message = "") {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
        NLM_LOG_INFO(message + ": " + std::to_string(duration.count()) + " ms elapsed");
    }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Memory management utilities
inline size_t countNonZeroElements(const std::vector<float>& values) {
    return std::count_if(values.begin(), values.end(), [](float v) { return v != 0.0f; });
}

inline float getMinValue(const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    return *std::min_element(values.begin(), values.end());
}

inline float getMaxValue(const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    return *std::max_element(values.begin(), values.end());
}

// Logging utilities
inline void logSection(const std::string& sectionName) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== " + sectionName + " ===");
    NLM_LOG_INFO("");
}

inline void logSubSection(const std::string& subSectionName) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("--- " + subSectionName + " ---");
    NLM_LOG_INFO("");
}

} // namespace nlm

