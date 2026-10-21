#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <numeric>

namespace nlm {

// Mathematical utility functions
namespace Math {
    // Clamp value between min and max
    template<typename T>
    constexpr T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }
    
    // Normalize value from [inMin, inMax] to [outMin, outMax]
    template<typename T>
    constexpr T normalize(T value, T inMin, T inMax, T outMin, T outMax) {
        return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
    
    // Exponential decay
    template<typename T>
    constexpr T exponentialDecay(T value, T factor) {
        return value * factor;
    }
    
    // Linear interpolation
    template<typename T>
    constexpr T lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }
    
    // Smoothstep function (Hermite interpolation)
    template<typename T>
    constexpr T smoothstep(T edge0, T edge1, T x) {
        T t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
        return t * t * (3.0 - 2.0 * t);
    }
    
    // Sigmoid function
    template<typename T>
    constexpr T sigmoid(T x) {
        return 1.0 / (1.0 + std::exp(-x));
    }
    
    // Gaussian function
    template<typename T>
    constexpr T gaussian(T x, T mean, T stddev) {
        return std::exp(-0.5 * std::pow((x - mean) / stddev, 2.0)) / (stddev * std::sqrt(2.0 * 3.14159265359));
    }
    
    // Mean of values
    template<typename T>
    T mean(const std::vector<T>& values) {
        if (values.empty()) return T(0);
        T sum = std::accumulate(values.begin(), values.end(), T(0));
        return sum / values.size();
    }
    
    // Standard deviation of values
    template<typename T>
    T standardDeviation(const std::vector<T>& values, T mean) {
        if (values.empty()) return T(0);
        T sum = 0;
        for (const auto& value : values) {
            sum += (value - mean) * (value - mean);
        }
        return std::sqrt(sum / values.size());
    }
    
    // L2 norm (Euclidean distance)
    template<typename T>
    T norm(const std::vector<T>& values) {
        T sum = std::accumulate(values.begin(), values.end(), T(0));
        for (const auto& value : values) {
            sum += value * value;
        }
        return std::sqrt(sum);
    }
}

// Collection utility functions
namespace Collection {
    // Find element in vector using predicate
    template<typename T, typename Predicate>
    std::optional<T> find(const std::vector<T>& collection, Predicate predicate) {
        auto it = std::find_if(collection.begin(), collection.end(), predicate);
        if (it != collection.end()) {
            return *it;
        }
        return std::nullopt;
    }
    
    // Find element in vector by value (with comparison)
    template<typename T>
    std::optional<T> find(const std::vector<T>& collection, const T& value) {
        auto it = std::find(collection.begin(), collection.end(), value);
        if (it != collection.end()) {
            return *it;
        }
        return std::nullopt;
    }
    
    // Filter collection using predicate
    template<typename T, typename Predicate>
    std::vector<T> filter(const std::vector<T>& collection, Predicate predicate) {
        std::vector<T> result;
        result.reserve(collection.size());
        for (const auto& item : collection) {
            if (predicate(item)) {
                result.push_back(item);
            }
        }
        return result;
    }
    
    // Map collection using function
    template<typename T, typename U, typename Func>
    std::vector<U> map(const std::vector<T>& collection, Func func) {
        std::vector<U> result;
        result.reserve(collection.size());
        for (const auto& item : collection) {
            result.push_back(func(item));
        }
        return result;
    }
    
    // Reduce collection using operation
    template<typename T, typename U, typename Operation>
    U reduce(const std::vector<T>& collection, U initial, Operation operation) {
        return std::accumulate(collection.begin(), collection.end(), initial, operation);
    }
    
    // Check if all elements satisfy predicate
    template<typename T, typename Predicate>
    bool all(const std::vector<T>& collection, Predicate predicate) {
        return std::all_of(collection.begin(), collection.end(), predicate);
    }
    
    // Check if any element satisfies predicate
    template<typename T, typename Predicate>
    bool any(const std::vector<T>& collection, Predicate predicate) {
        return std::any_of(collection.begin(), collection.end(), predicate);
    }
    
    // Remove duplicates from sorted collection
    template<typename T>
    std::vector<T> uniqueSorted(const std::vector<T>& collection) {
        std::vector<T> result = collection;
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());
        return result;
    }
    
    // Partition collection based on predicate
    template<typename T, typename Predicate>
    std::pair<std::vector<T>, std::vector<T>> partition(const std::vector<T>& collection, Predicate predicate) {
        std::vector<T> truePart, falsePart;
        truePart.reserve(collection.size());
        falsePart.reserve(collection.size());
        
        for (const auto& item : collection) {
            if (predicate(item)) {
                truePart.push_back(item);
            } else {
                falsePart.push_back(item);
            }
        }
        
        return {truePart, falsePart};
    }
    
    // Split collection into chunks of given size
    template<typename T>
    std::vector<std::vector<T>> chunk(const std::vector<T>& collection, size_t chunkSize) {
        std::vector<std::vector<T>> chunks;
        if (chunkSize == 0) return chunks;
        
        for (size_t i = 0; i < collection.size(); i += chunkSize) {
            size_t end = std::min(i + chunkSize, collection.size());
            chunks.emplace_back(collection.begin() + i, collection.begin() + end);
        }
        
        return chunks;
    }
}

// String utility functions
namespace String {
    // Trim whitespace from start and end
    std::string trim(const std::string& str);
    
    // Convert string to lowercase
    std::string toLower(const std::string& str);
    
    // Convert string to uppercase
    std::string toUpper(const std::string& str);
    
    // Check if string starts with prefix
    bool startsWith(const std::string& str, const std::string& prefix);
    
    // Check if string ends with suffix
    bool endsWith(const std::string& str, const std::string& suffix);
    
    // Split string by delimiter
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Join vector of strings with delimiter
    template<typename T>
    std::string join(const std::vector<T>& strings, const std::string& delimiter = ", ") {
        if (strings.empty()) return "";
        
        std::stringstream ss;
        ss << strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            ss << delimiter << strings[i];
        }
        return ss.str();
    }
    
    // Format string with placeholders (similar to Python's str.format)
    std::string format(const std::string& formatStr, const std::vector<std::string>& args);
}

// Memory utility functions
namespace Memory {
    // Safe delete for pointers
    template<typename T>
    void deletePtr(T* ptr) {
        delete ptr;
    }
    
    // Safe delete for arrays
    template<typename T>
    void deleteArray(T* ptr) {
        delete[] ptr;
    }
    
    // Safe free for memory allocated with new and used with delete
    template<typename T>
    void freePtr(T* ptr) {
        delete ptr;
    }
    
    // Check if pointer is null
    template<typename T>
    bool isNull(T* ptr) {
        return ptr == nullptr;
    }
    
    // Check if pointer is not null
    template<typename T>
    bool isNotNull(T* ptr) {
        return ptr != nullptr;
    }
}

// Time utility functions
namespace Time {
    // Convert seconds to milliseconds
    constexpr double toMilliseconds(double seconds) {
        return seconds * 1000.0;
    }
    
    // Convert milliseconds to seconds
    constexpr double toSeconds(double milliseconds) {
        return milliseconds / 1000.0;
    }
    
    // Convert seconds to microseconds
    constexpr double toMicroseconds(double seconds) {
        return seconds * 1000000.0;
    }
    
    // Get current time in seconds
    double now();
    
    // Sleep for duration in seconds
    void sleep(double seconds);
}

// Random utility functions
namespace Random {
    // Generate random integer in range [min, max]
    template<typename T>
    T uniformInt(T min, T max) {
        static std::mt19937 generator;
        std::uniform_int_distribution<T> distribution(min, max);
        return distribution(generator);
    }
    
    // Generate random floating point in range [min, max]
    template<typename T>
    T uniformReal(T min, T max) {
        static std::mt19937 generator;
        std::uniform_real_distribution<T> distribution(min, max);
        return distribution(generator);
    }
    
    // Generate random Gaussian with given mean and standard deviation
    template<typename T>
    T gaussian(T mean, T stddev) {
        static std::mt19937 generator;
        std::normal_distribution<T> distribution(mean, stddev);
        return distribution(generator);
    }
    
    // Randomly choose one element from collection
    template<typename T>
    T choice(const std::vector<T>& collection) {
        if (collection.empty()) return T();
        return collection[uniformInt<size_t>(0, collection.size() - 1)];
    }
}

// Event-driven simulation utilities
namespace Simulation {
    // Calculate next spike time using exponential distribution
    template<typename T>
    T nextExponential(T rate) {
        static std::mt19937 generator;
        std::exponential_distribution<T> distribution(rate);
        return distribution(generator);
    }
    
    // Calculate next Poisson event time
    template<typename T>
    T nextPoisson(T rate) {
        return nextExponential(rate);
    }
    
    // Calculate time constant for RC circuit
    template<typename T>
    T rcTimeConstant(T resistance, T capacitance) {
        return resistance * capacitance;
    }
}

// Neural network specific utilities
namespace Neural {
    // Calculate synaptic current
    template<typename T>
    T calculateSynapticCurrent(T conductance, T reversalPotential) {
        return conductance * reversalPotential;
    }
    
    // Calculate membrane potential change
    template<typename T>
    T calculateMembranePotentialChange(T current, T capacitance, T dt) {
        return current / capacitance * dt;
    }
    
    // Calculate spike frequency adaptation
    template<typename T>
    T calculateAdaptation(T firingRate, T adaptationFactor) {
        return firingRate * adaptationFactor;
    }
    
    // Calculate STDP weight change
    template<typename T>
    T calculateSTDPChange(T preSpikeTime, T postSpikeTime, T ltpWeight, T ltdWeight, T tau) {
        T delta = postSpikeTime - preSpikeTime;
        if (delta > 0) {
            return ltpWeight * std::exp(-delta / tau);
        } else {
            return -ltdWeight * std::exp(delta / tau);
        }
    }
}

// Logging utility functions
namespace Log {
    // Format log message with timestamp
    std::string formatWithTimestamp(const std::string& level, const std::string& message);
    
    // Format log message with file and line info
    std::string formatWithLocation(const std::string& level, const std::string& message, 
                                   const std::string& file, int line);
    
    // Format log message with function name
    std::string formatWithFunction(const std::string& level, const std::string& message,
                                   const std::string& function);
}

// Mathematical constants
namespace Constants {
    constexpr double PI = 3.14159265358979323846;
    constexpr double E = 2.71828182845904523536;
    constexpr double EULER = 2.71828182845904523536;
    constexpr double SQRT2 = 1.41421356237309504880;
    constexpr double SQRT3 = 1.73205080756887729353;
    constexpr double GOLDEN_RATIO = 1.61803398874989484820;
    
    // Neuron-related constants
    constexpr double RESTING_POTENTIAL = -70.0;
    constexpr double THRESHOLD = -55.0;
    constexpr double RESET_POTENTIAL = -70.0;
    constexpr double MEMBRANE_CAPACITANCE = 1.0;  // nF
    constexpr double TIME_CONSTANT = 20.0;      // ms
    constexpr double REFRACTORY_PERIOD = 5.0;   // ms
    
    // Synaptic constants
    constexpr double EXCITATORY_WEIGHT_MIN = 0.0;
    constexpr double EXCITATORY_WEIGHT_MAX = 1.0;
    constexpr double INHIBITORY_WEIGHT_MIN = -1.0;
    constexpr double INHIBITORY_WEIGHT_MAX = 0.0;
    constexpr double SYNAPTIC_DELAY_MIN = 1;
    constexpr double SYNAPTIC_DELAY_MAX = 10;
}

// Convenience aliases for common operations
using Vector = std::vector;
using Map = std::unordered_map;
using Set = std::unordered_set;
using Pair = std::pair;

} // namespace nlm

// Common utility functions (inline implementation for performance)
inline std::string nlm::String::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::string nlm::String::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline std::string nlm::String::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

inline bool nlm::String::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

inline bool nlm::String::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::vector<std::string> nlm::String::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

inline std::string nlm::String::format(const std::string& formatStr, const std::vector<std::string>& args) {
    std::string result = formatStr;
    size_t pos = 0;
    size_t argIndex = 0;
    
    while ((pos = result.find("{}", pos)) != std::string::npos && argIndex < args.size()) {
        result.replace(pos, 2, args[argIndex]);
        pos += args[argIndex].size();
        argIndex++;
    }
    
    return result;
}

inline double nlm::Time::now() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
}

inline void nlm::Time::sleep(double seconds) {
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

inline std::string nlm::Log::formatWithTimestamp(const std::string& level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "]";
    ss << " [" << level << "]";
    ss << " " << message;
    return ss.str();
}

inline std::string nlm::Log::formatWithLocation(const std::string& level, const std::string& message,
                                                const std::string& file, int line) {
    std::stringstream ss;
    ss << "[" << level << "]";
    ss << " " << message;
    if (!file.empty() && line > 0) {
        ss << " (" << file << ":" << line << ")";
    }
    return ss.str();
}

inline std::string nlm::Log::formatWithFunction(const std::string& level, const std::string& message,
                                                const std::string& function) {
    std::stringstream ss;
    ss << "[" << level << "]";
    ss << " " << message;
    if (!function.empty()) {
        ss << " in " << function;
    }
    return ss.str();
}
