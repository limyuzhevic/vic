// Utility function implementations for NLM (Neural Learning Machine)

#include "Utilities.hpp"
#include <cstdarg>  // For va_list
#include <numeric>  // For std::accumulate

namespace nlm {

// String utilities implementation
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Vector utilities implementation
float vectorMagnitude(const std::vector<float>& vec) {
    float sum = 0.0f;
    for (float v : vec) {
        sum += v * v;
    }
    return std::sqrt(sum);
}

float vectorDot(const std::vector<float>& a, const std::vector<float>& b) {
    float result = 0.0f;
    size_t size = std::min(a.size(), b.size());
    for (size_t i = 0; i < size; ++i) {
        result += a[i] * b[i];
    }
    return result;
}

std::vector<float> vectorNormalize(const std::vector<float>& vec) {
    float mag = vectorMagnitude(vec);
    if (mag == 0.0f) return vec;
    std::vector<float> result = vec;
    for (float& v : result) {
        v /= mag;
    }
    return result;
}

// Array utilities implementation
float arraySum(const float* array, size_t size) {
    float sum = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        sum += array[i];
    }
    return sum;
}

float arrayMean(const float* array, size_t size) {
    if (size == 0) return 0.0f;
    return arraySum(array, size) / size;
}

float arrayVariance(const float* array, size_t size) {
    if (size < 2) return 0.0f;
    
    float mean = arrayMean(array, size);
    float sumSq = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        float diff = array[i] - mean;
        sumSq += diff * diff;
    }
    return sumSq / (size - 1);
}

// Debugging utilities implementation
std::string debugFormat(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // Use snprintf for safe formatting
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    va_end(args);
    
    return std::string(buffer);
}

// Performance utilities implementation
double getCurrentTime() {
    using clock = std::chrono::high_resolution_clock;
    auto now = clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

} // namespace nlm