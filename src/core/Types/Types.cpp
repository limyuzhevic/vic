#include "Types.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Initialize static color constants
const Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);
const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::Gray(0.5f, 0.5f, 0.5f, 1.0f);
const Color Color::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
const Color Color::Cyan(0.0f, 1.0f, 1.0f, 1.0f);
const Color Color::Magenta(1.0f, 0.0f, 1.0f, 1.0f);

// Vector2D implementations
float Vector2D::normalize() {
    float mag = magnitude();
    if (mag > 0.0f) {
        x /= mag;
        y /= mag;
    }
    return mag;
}

Vector2D Vector2D::normalized() const {
    Vector2D result = *this;
    float mag = result.magnitude();
    if (mag > 0.0f) {
        result.x /= mag;
        result.y /= mag;
    }
    return result;
}

// Utility function implementations
namespace utils {

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

std::string capitalize(const std::string& str) {
    if (str.empty()) return str;
    return toUpper(str.substr(0, 1)) + toLower(str.substr(1));
}

std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";
    std::string result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += delimiter + strings[i];
    }
    return result;
}

template<typename T>
T clamp(T value, T min, T max) {
    return std::max(min, std::min(value, max));
}

template<typename T>
bool inRange(T value, T min, T max) {
    return (value >= min && value <= max);
}

template<typename T>
size_t safeIndex(size_t index, size_t maxSize, size_t defaultIndex) {
    if (index >= maxSize) {
        return defaultIndex;
    }
    return index;
}

RandomGenerator::RandomGenerator() : seed_(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {
    std::mt19937::seed(seed_);
}

RandomGenerator::RandomGenerator(uint64_t seed) : seed_(seed) {
    std::mt19937::seed(seed_);
}

float RandomGenerator::uniformReal(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(std::mt19937(seed_));
}

int RandomGenerator::uniformInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(std::mt19937(seed_));
}

double RandomGenerator::normalDistribution(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(std::mt19937(seed_));
}

bool fileExists(const std::string& path) {
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
#endif
}

bool directoryExists(const std::string& path) {
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
#endif
}

std::string getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == 0) {
        return "";
    }
    return filename.substr(dotPos + 1);
}

std::string getFileNameWithoutExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) {
        return filename;
    }
    return filename.substr(0, dotPos);
}

template<typename T>
T sigmoid(T x) {
    return static_cast<T>(1.0 / (1.0 + std::exp(-static_cast<double>(x))));
}

template<typename T>
T relu(T x) {
    return std::max(static_cast<T>(0), x);
}

template<typename T>
T leakyRelu(T x, T alpha) {
    return (x > 0) ? x : alpha * x;
}

template<typename T>
T deriveSigmoid(T x) {
    T s = sigmoid(x);
    return s * (static_cast<T>(1.0) - s);
}

template<typename T>
T deriveReLU(T x) {
    return (x > 0) ? static_cast<T>(1.0) : static_cast<T>(0.0);
}

template<typename T>
T gaussian(T x, T mean, T sigma) {
    T exponent = static_cast<T>(-0.5) * std::pow((x - mean) / sigma, 2);
    return (1.0 / (sigma * std::sqrt(2.0 * M_PI))) * std::exp(static_cast<double>(exponent));
}

// Explicit template instantiations for common types
template float clamp<float>(float, float, float);
template double clamp<double>(double, double, double);

template float inRange<float>(float, float, float);
template double inRange<double>(double, double, double);

template size_t safeIndex<size_t>(size_t, size_t, size_t);

template float sigmoid<float>(float);
template double sigmoid<double>(double);

template float relu<float>(float);
template double relu<double>(double);

template float leakyRelu<float>(float, float);
template double leakyRelu<double>(double, double);

template float deriveSigmoid<float>(float);
template double deriveSigmoid<double>(double);

template float deriveReLU<float>(float);
template double deriveReLU<double>(double);

template float gaussian<float>(float, float, float);
template double gaussian<double>(double, double, double);

} // namespace utils

} // namespace nlm