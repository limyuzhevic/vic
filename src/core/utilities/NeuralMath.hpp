// Mathematical and Neural Utility Functions
// Reusable neural calculations, math utilities, and string utilities

#pragma once

#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <limits>
#include <memory>
#include <type_traits>

namespace nlm {

// Neural activation functions
class NeuralMath {
public:
    // Basic activation functions
    static float sigmoid(float x) { return 1.0f / (1.0f + std::exp(-x)); }
    static float sigmoidDerivative(float x) {
        float s = sigmoid(x);
        return s * (1.0f - s);
    }
    
    static float tanh(float x) { return std::tanh(x); }
    static float tanhDerivative(float x) {
        float t = tanh(x);
        return 1.0f - t * t;
    }
    
    static float relu(float x) { return std::max(0.0f, x); }
    static float reluDerivative(float x) { return x > 0.0f ? 1.0f : 0.0f; }
    
    static float leakyRelu(float x, float alpha = 0.01f) { return x > 0.0f ? x : alpha * x; }
    static float leakyReluDerivative(float x, float alpha = 0.01f) { 
        return x > 0.0f ? 1.0f : alpha; 
    }
    
    static float elu(float x, float alpha = 1.0f) {
        return x > 0.0f ? x : alpha * (std::exp(x) - 1.0f);
    }
    static float eluDerivative(float x, float alpha = 1.0f) {
        return x > 0.0f ? 1.0f : x + alpha;
    }
    
    static float swish(float x) { return x * sigmoid(x); }
    static float swishDerivative(float x) {
        float s = sigmoid(x);
        return s + x * s * (1.0f - s);
    }

    // Normalization functions
    static float normalize(float value, float min, float max) {
        if (max == min) return 0.5f;
        return (value - min) / (max - min);
    }
    
    static float denormalize(float normalized, float min, float max) {
        return normalized * (max - min) + min;
    }
    
    static float normalizeToRange(float value, float newMin, float newMax,
                                  float oldMin = 0.0f, float oldMax = 1.0f) {
        if (oldMax == oldMin) return newMin;
        float normalized = (value - oldMin) / (oldMax - oldMin);
        return normalized * (newMax - newMin) + newMin;
    }

    // Distance and similarity metrics
    static float euclideanDistance(float a, float b) { return std::abs(a - b); }
    static float squaredDistance(float a, float b) { return (a - b) * (a - b); }
    static float cosineSimilarity(float a, float b) { 
        float normA = std::abs(a);
        float normB = std::abs(b);
        if (normA == 0.0f || normB == 0.0f) return 0.0f;
        return a * b / (normA * normB);
    }

    // Statistical functions
    static float mean(const std::vector<float>& values) {
        return std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
    }
    
    static float variance(const std::vector<float>& values, float mean = 0.0f) {
        float sum = 0.0f;
        for (float value : values) {
            float diff = value - mean;
            sum += diff * diff;
        }
        return sum / values.size();
    }
    
    static float standardDeviation(const std::vector<float>& values, float mean = 0.0f) {
        return std::sqrt(variance(values, mean));
    }
    
    static float median(std::vector<float> values) {
        size_t n = values.size();
        std::sort(values.begin(), values.end());
        if (n % 2 == 0) {
            return (values[n/2 - 1] + values[n/2]) / 2.0f;
        } else {
            return values[n/2];
        }
    }

    // Probability and statistics
    static float gaussian(float x, float mean = 0.0f, float stddev = 1.0f) {
        float exponent = -(x - mean) * (x - mean) / (2.0f * stddev * stddev);
        return std::exp(exponent) / (stddev * std::sqrt(2.0f * 3.14159f));
    }
    
    static float poisson(float lambda, size_t k) {
        float result = std::exp(-lambda);
        for (size_t i = 1; i <= k; ++i) {
            result *= lambda / i;
        }
        return result;
    }
};

// Interpolation and curve functions
class Interpolation {
public:
    // Linear interpolation
    static float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
    // Smoothstep interpolation
    static float smoothstep(float edge0, float edge1, float x) {
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
    
    // Cubic interpolation
    static float cubic(float v0, float v1, float v2, float v3, float t) {
        float t2 = t * t;
        float t3 = t2 * t;
        return (1.0f - 3.0f * t + 3.0f * t2 - t3) * v0 +
               (3.0f * t - 6.0f * t2 + 3.0f * t3) * v1 +
               (-3.0f * t + 3.0f * t2) * v2 +
               t3 * v3;
    }
    
    // Spline interpolation
    static std::vector<float> cubicSpline(const std::vector<float>& x, const std::vector<float>& y, float xQuery);
    
    // Weighted average
    static float weightedAverage(const std::vector<float>& values, const std::vector<float>& weights);
    
    // Moving average
    static std::vector<float> movingAverage(const std::vector<float>& values, size_t windowSize);

    // Curve fitting (simplified)
    static std::pair<float, float> linearRegression(const std::vector<float>& x, const std::vector<float>& y);
};

// String and path utilities
class StringUtils {
public:
    // String manipulation
    static std::string toLowerCase(const std::string& str);
    static std::string toUpperCase(const std::string& str);
    static std::string trim(const std::string& str);
    static std::string ltrim(const std::string& str);
    static std::string rtrim(const std::string& str);
    static std::string replaceAll(std::string str, const std::string& from, const std::string& to);
    static std::string format(const std::string& format, ...);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // File path utilities
    static std::string extractFileName(const std::string& path);
    static std::string extractFileExtension(const std::string& path);
    static std::string extractDirectory(const std::string& path);
    static std::string normalizePath(const std::string& path);
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static std::string getTempDirectory();
    
    // Formatting utilities
    static std::string formatNumber(float value, int precision = 2);
    static std::string formatTime(float seconds);
    static std::string formatBytes(size_t bytes);
    static std::string formatMemory(size_t bytes);

    // Validation utilities
    static bool isNumber(const std::string& str);
    static bool isInteger(const std::string& str);
    static bool isFloat(const std::string& str);
    static bool containsOnlyLetters(const std::string& str);
    static bool containsOnlyDigits(const std::string& str);
};

// Memory utilities
class MemoryUtils {
public:
    // Memory alignment
    static void* alignPointer(void* ptr, size_t alignment);
    static size_t alignSize(size_t size, size_t alignment);
    static bool isAligned(const void* ptr, size_t alignment);
    
    // Memory allocation tracking
    static void enableMemoryTracking(bool enable = true);
    static size_t getTotalAllocatedMemory();
    static size_t getCurrentAllocatedMemory();
    static size_t getPeakAllocatedMemory();
    static void resetMemoryStats();
    
    // Memory pools (simplified)
    static void* allocateFromPool(size_t size, size_t alignment = 16);
    static void deallocateToPool(void* ptr);
    static void cleanupMemoryPools();
    
    // Memory utilities for neural networks
    static float* allocateNeuralArray(size_t size);
    static void deallocateNeuralArray(float* ptr);
    static void initializeNeuralArray(float* ptr, float value, size_t size);
    static void fillRandomNeuralArray(float* ptr, float min, float max, size_t size);

    // Memory management
    static void enablePooling(bool enable = true);
    static bool isPoolingEnabled();
    static void setPoolSize(size_t size);
    static size_t getPoolSize();

private:
    static bool memoryTrackingEnabled_;
    static size_t totalAllocated_;
    static size_t currentAllocated_;
    static size_t peakAllocated_;
    static bool poolingEnabled_;
    static size_t poolSize_;
};

// Configuration and type utilities
class TypeUtils {
public:
    // Type checking and conversion
    static bool isIntegralType(const std::type_info& type);
    static bool isFloatingPointType(const std::type_info& type);
    static bool isStringType(const std::type_info& type);
    static bool isVectorType(const std::type_info& type);
    
    // Template type information
    template<typename T>
    static std::string getTypeName();
    
    // Compile-time validation
    template<typename T>
    static bool validateType();
    
    // Type mapping
    static std::string typeToString(const std::type_info& type);
    
    // Configuration type validation
    static bool isValidConfigType(const std::type_info& type);
    static bool isNumericType(const std::type_info& type);
    static bool isStringLikeType(const std::type_info& type);
};

// Implementation details for Interpolation
inline std::vector<float> Interpolation::cubicSpline(const std::vector<float>& x, const std::vector<float>& y, float xQuery) {
    if (x.size() != y.size() || x.size() < 2) {
        return std::vector<float>(1, y.empty() ? 0.0f : y[0]);
    }
    
    if (xQuery <= x[0]) {
        return std::vector<float>(1, y[0]);
    }
    if (xQuery >= x.back()) {
        return std::vector<float>(1, y.back());
    }
    
    size_t idx = 0;
    for (size_t i = 0; i < x.size() - 1; ++i) {
        if (x[i] <= xQuery && xQuery <= x[i + 1]) {
            idx = i;
            break;
        }
    }
    
    return std::vector<float>(1, cubic(y[idx], y[idx + 1], y[idx + 2], y[idx + 3], 
                                       (xQuery - x[idx]) / (x[idx + 1] - x[idx])));
}

inline float Interpolation::weightedAverage(const std::vector<float>& values, const std::vector<float>& weights) {
    if (values.size() != weights.size() || values.empty()) {
        return 0.0f;
    }
    
    float sumValue = 0.0f;
    float sumWeight = 0.0f;
    
    for (size_t i = 0; i < values.size(); ++i) {
        sumValue += values[i] * weights[i];
        sumWeight += weights[i];
    }
    
    return sumWeight > 0.0f ? sumValue / sumWeight : 0.0f;
}

inline std::vector<float> Interpolation::movingAverage(const std::vector<float>& values, size_t windowSize) {
    if (values.empty() || windowSize == 0) return std::vector<float>();
    
    size_t actualWindow = std::min(windowSize, values.size());
    std::vector<float> result;
    result.reserve(values.size() - actualWindow + 1);
    
    for (size_t i = 0; i <= values.size() - actualWindow; ++i) {
        float sum = 0.0f;
        for (size_t j = 0; j < actualWindow; ++j) {
            sum += values[i + j];
        }
        result.push_back(sum / actualWindow);
    }
    
    return result;
}

inline std::pair<float, float> Interpolation::linearRegression(const std::vector<float>& x, const std::vector<float>& y) {
    if (x.size() != y.size() || x.size() < 2) {
        return {0.0f, 0.0f};
    }
    
    float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f, sumY2 = 0.0f;
    size_t n = x.size();
    
    for (size_t i = 0; i < n; ++i) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
        sumY2 += y[i] * y[i];
    }
    
    float denominator = n * sumX2 - sumX * sumX;
    if (std::abs(denominator) < 1e-10f) {
        return {0.0f, 0.0f};
    }
    
    float slope = (n * sumXY - sumX * sumY) / denominator;
    float intercept = (sumY - slope * sumX) / n;
    
    return {slope, intercept};
}

inline std::string StringUtils::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline std::string StringUtils::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

inline std::string StringUtils::trim(const std::string& str) {
    return ltrim(rtrim(str));
}

inline std::string StringUtils::ltrim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : str.substr(start);
}

inline std::string StringUtils::rtrim(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\r\n");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

inline std::string StringUtils::replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

inline std::string StringUtils::format(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    va_list argsCopy;
    va_copy(argsCopy, args);
    int size = std::vsnprintf(nullptr, 0, format.c_str(), argsCopy);
    va_end(argsCopy);
    
    if (size <= 0) return "";
    
    std::vector<char> buffer(size + 1);
    std::vsnprintf(buffer.data(), buffer.size(), format.c_str(), args);
    va_end(args);
    
    return std::string(buffer.data());
}

inline std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) ss << delimiter;
        ss << strings[i];
    }
    return ss.str();
}

inline std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

inline std::string StringUtils::extractFileName(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

inline std::string StringUtils::extractFileExtension(const std::string& path) {
    return std::filesystem::path(path).extension().string();
}

inline std::string StringUtils::extractDirectory(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

inline std::string StringUtils::normalizePath(const std::string& path) {
    return std::filesystem::path(path).string();
}

inline bool StringUtils::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

inline bool StringUtils::directoryExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

inline std::string StringUtils::getTempDirectory() {
    return std::filesystem::temp_directory_path().string();
}

inline std::string StringUtils::formatNumber(float value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

inline std::string StringUtils::formatTime(float seconds) {
    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    int secs = static_cast<int>(seconds) % 60;
    int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
    
    std::stringstream ss;
    if (hours > 0) ss << hours << "h ";
    ss << minutes << "m " << secs << "." << std::setfill('0') << std::setw(3) << ms << "s";
    return ss.str();
}

inline std::string StringUtils::formatBytes(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return ss.str();
}

inline std::string StringUtils::formatMemory(size_t bytes) {
    return formatBytes(bytes);
}

inline bool StringUtils::isNumber(const std::string& str) {
    std::istringstream iss(str);
    float val;
    iss >> val;
    return !iss.fail() && iss.eof();
}

inline bool StringUtils::isInteger(const std::string& str) {
    std::istringstream iss(str);
    int val;
    iss >> val;
    return !iss.fail() && iss.eof();
}

inline bool StringUtils::isFloat(const std::string& str) {
    std::istringstream iss(str);
    float val;
    iss >> val;
    return !iss.fail() && iss.eof();
}

inline bool StringUtils::containsOnlyLetters(const std::string& str) {
    return std::all_of(str.begin(), str.end(), ::isalpha);
}

inline bool StringUtils::containsOnlyDigits(const std::string& str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

inline void* MemoryUtils::alignPointer(void* ptr, size_t alignment) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    return reinterpret_cast<void*>(aligned);
}

inline size_t MemoryUtils::alignSize(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

inline bool MemoryUtils::isAligned(const void* ptr, size_t alignment) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    return (addr % alignment) == 0;
}

inline void MemoryUtils::enableMemoryTracking(bool enable) {
    memoryTrackingEnabled_ = enable;
}

inline size_t MemoryUtils::getTotalAllocatedMemory() {
    return totalAllocated_;
}

inline size_t MemoryUtils::getCurrentAllocatedMemory() {
    return currentAllocated_;
}

inline size_t MemoryUtils::getPeakAllocatedMemory() {
    return peakAllocated_;
}

inline void MemoryUtils::resetMemoryStats() {
    totalAllocated_ = 0;
    currentAllocated_ = 0;
    peakAllocated_ = 0;
}

inline void* MemoryUtils::allocateFromPool(size_t size, size_t alignment) {
    return std::aligned_alloc(alignment, size);
}

inline void MemoryUtils::deallocateToPool(void* ptr) {
    std::free(ptr);
}

inline void MemoryUtils::cleanupMemoryPools() {
}

inline float* MemoryUtils::allocateNeuralArray(size_t size) {
    float* ptr = static_cast<float*>(std::aligned_alloc(32, size * sizeof(float)));
    if (memoryTrackingEnabled_) {
        totalAllocated_ += size * sizeof(float);
        currentAllocated_ += size * sizeof(float);
        if (currentAllocated_ > peakAllocated_) {
            peakAllocated_ = currentAllocated_;
        }
    }
    return ptr;
}

inline void MemoryUtils::deallocateNeuralArray(float* ptr) {
    if (ptr) {
        std::free(ptr);
    }
}

inline void MemoryUtils::initializeNeuralArray(float* ptr, float value, size_t size) {
    if (ptr) {
        for (size_t i = 0; i < size; ++i) {
            ptr[i] = value;
        }
    }
}

inline void MemoryUtils::fillRandomNeuralArray(float* ptr, float min, float max, size_t size) {
    if (ptr) {
        for (size_t i = 0; i < size; ++i) {
            ptr[i] = min + static_cast<float>(rand()) / RAND_MAX * (max - min);
        }
    }
}

inline void MemoryUtils::enablePooling(bool enable) {
    poolingEnabled_ = enable;
}

inline bool MemoryUtils::isPoolingEnabled() {
    return poolingEnabled_;
}

inline void MemoryUtils::setPoolSize(size_t size) {
    poolSize_ = size;
}

inline size_t MemoryUtils::getPoolSize() {
    return poolSize_;
}

namespace {
    template<typename T>
    std::string getTypeNameImpl() {
        return "unknown";
    }

    template<typename T>
    bool validateTypeImpl() {
        return true;
    }
}

inline bool TypeUtils::isIntegralType(const std::type_info& type) {
    const std::type_info& intType = typeid(int);
    const std::type_info& int64Type = typeid(int64_t);
    const std::type_info& sizeTType = typeid(size_t);
    
    return type == intType || type == int64Type || type == sizeTType;
}

inline bool TypeUtils::isFloatingPointType(const std::type_info& type) {
    const std::type_info& floatType = typeid(float);
    const std::type_info& doubleType = typeid(double);
    
    return type == floatType || type == doubleType;
}

inline bool TypeUtils::isStringType(const std::type_info& type) {
    return type == typeid(std::string);
}

inline bool TypeUtils::isVectorType(const std::type_info& type) {
    return type == typeid(std::vector<float>);
}

template<typename T>
inline std::string TypeUtils::getTypeName() {
    return getTypeNameImpl<T>();
}

template<typename T>
inline bool TypeUtils::validateType() {
    return validateTypeImpl<T>();
}

inline std::string TypeUtils::typeToString(const std::type_info& type) {
    return type.name();
}

inline bool TypeUtils::isValidConfigType(const std::type_info& type) {
    const std::type_info& intType = typeid(int);
    const std::type_info& int64Type = typeid(int64_t);
    const std::type_info& floatType = typeid(float);
    const std::type_info& doubleType = typeid(double);
    const std::type_info& boolType = typeid(bool);
    const std::type_info& stringType = typeid(std::string);
    
    return type == intType || type == int64Type || type == floatType ||
           type == doubleType || type == boolType || type == stringType;
}

inline bool TypeUtils::isNumericType(const std::type_info& type) {
    return isIntegralType(type) || isFloatingPointType(type);
}

inline bool TypeUtils::isStringLikeType(const std::type_info& type) {
    return isStringType(type);
}

// MemoryUtils static member initialization
bool MemoryUtils::memoryTrackingEnabled_ = false;
size_t MemoryUtils::totalAllocated_ = 0;
size_t MemoryUtils::currentAllocated_ = 0;
size_t MemoryUtils::peakAllocated_ = 0;
bool MemoryUtils::poolingEnabled_ = false;
size_t MemoryUtils::poolSize_ = 1024;

} // namespace nlm
