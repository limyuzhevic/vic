// Implementation of utility functions

#include "Utilities.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

bool hasCommandLineFlag(int argc, char** argv, const std::string& flag) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == flag) {
            return true;
        }
        // Check for --flag=value format
        std::string arg = argv[i];
        if (arg.size() >= flag.size() && arg.compare(0, flag.size(), flag) == 0 && arg[flag.size()] == '=') {
            return true;
        }
    }
    return false;
}

template<typename T>
T getCommandLineValue(int argc, char** argv, const std::string& flag, const T& defaultValue) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.size() >= flag.size() + 2 && 
            arg.compare(0, flag.size(), flag) == 0 && 
            arg[flag.size()] == '=') {
            
            std::string valueStr = arg.substr(flag.size() + 1);
            
            // Parse based on type
            if constexpr (std::is_same_v<T, int>) {
                return std::stoi(valueStr);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::stof(valueStr);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::stod(valueStr);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return valueStr;
            } else if constexpr (std::is_same_v<T, bool>) {
                return (valueStr == "true" || valueStr == "1" || valueStr == "yes");
            } else {
                return defaultValue;
            }
        }
    }
    return defaultValue;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 
           str.compare(0, prefix.size(), prefix) == 0;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string formatFloat(float value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

bool fileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

bool ensureDirectoryExists(const std::string& path) {
    try {
        std::filesystem::path dir(path);
        if (!std::filesystem::exists(dir)) {
            return std::filesystem::create_directories(dir);
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        NLM_LOG_ERROR("Failed to create directory " + path + ": " + e.what());
        return false;
    }
}

std::string getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filepath.substr(pos);
}

std::string generateUniqueFilename(const std::string& baseName, const std::string& extension) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    std::stringstream ss;
    ss << baseName << "_" << timestamp << extension;
    return ss.str();
}

template<typename T>
T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

template<typename T>
T lerp(T a, T b, float t) {
    return a + (b - a) * t;
}

template<typename T>
T mean(const std::vector<T>& values) {
    if (values.empty()) return T();
    T sum = std::accumulate(values.begin(), values.end(), T());
    return sum / values.size();
}

template<typename T>
T standardDeviation(const std::vector<T>& values, T mean) {
    if (values.empty()) return T();
    T sum = std::accumulate(values.begin(), values.end(), T());
    T variance = (sum / values.size()) - mean * mean;
    return std::sqrt(std::abs(variance));
}

template<typename T>
T safeDivide(T numerator, T denominator, T defaultValue) {
    if (denominator == T(0)) {
        return defaultValue;
    }
    return numerator / denominator;
}

} // namespace nlm
