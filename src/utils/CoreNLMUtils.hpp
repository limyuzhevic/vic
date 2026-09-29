// Core NLM Utilities - Common functions and patterns used throughout the codebase
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace utils {

// String manipulation utilities
class StringUtils {
public:
    // Convert string to lowercase
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    // Convert string to uppercase
    static std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }
    
    // Trim whitespace from both ends
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Trim whitespace from left only
    static std::string trimLeft(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string&npos) return "";
        return str.substr(start);
    }
    
    // Trim whitespace from right only
    static std::string trimRight(const std::string& str) {
        size_t end = str.find_last_not_of(" \t\r\n");
        if (end == std::string::npos) return str;
        return str.substr(0, end + 1);
    }
    
    // Check if string starts with prefix
    static bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && 
               str.compare(0, prefix.size(), prefix) == 0;
    }
    
    // Check if string ends with suffix
    static bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    // Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            result.push_back(trim(token));
        }
        return result;
    }
    
    // Join vector of strings with delimiter
    static std::string join(const std::vector<std::string>& strings, char delimiter) {
        std::stringstream ss;
        for (size_t i = 0; i < strings.size(); ++i) {
            if (i > 0) ss << delimiter;
            ss << strings[i];
        }
        return ss.str();
    }
    
    // Replace all occurrences of substring
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }
    
    // String to number conversions
    static int stringToInt(const std::string& str, int defaultValue = 0) {
        try {
            return std::stoi(str);
        } catch (...) {
            return defaultValue;
        }
    }
    
    static int64_t stringToInt64(const std::string& str, int64_t defaultValue = 0) {
        try {
            return std::stoll(str);
        } catch (...) {
            return defaultValue;
        }
    }
    
    static double stringToDouble(const std::string& str, double defaultValue = 0.0) {
        try {
            return std::stod(str);
        } catch (...) {
            return defaultValue;
        }
    }
    
    static float stringToFloat(const std::string& str, float defaultValue = 0.0f) {
        try {
            return static_cast<float>(std::stod(str));
        } catch (...) {
            return defaultValue;
        }
    }
    
    static bool stringToBool(const std::string& str) {
        std::string lower = toLower(trim(str));
        return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
    }
    
    // Number to string conversions
    static std::string intToString(int value) {
        return std::to_string(value);
    }
    
    static std::string int64ToString(int64_t value) {
        return std::to_string(value);
    }
    
    static std::string doubleToString(double value, int precision = 6) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    static std::string floatToString(float value, int precision = 6) {
        return doubleToString(static_cast<double>(value), precision);
    }
    
    // Format string with arguments (similar to printf)
    template<typename... Args>
    static std::string format(const std::string& formatStr, Args... args) {
        std::stringstream ss;
        formatInternal(ss, formatStr, args...);
        return ss.str();
    }
    
private:
    template<typename T, typename... Args>
    static void formatInternal(std::stringstream& ss, const std::string& formatStr, T value, Args... args) {
        size_t pos = formatStr.find("{}");
        if (pos != std::string::npos) {
            ss << formatStr.substr(0, pos);
            ss << value;
            formatInternal(ss, formatStr.substr(pos + 2), args...);
        } else {
            ss << formatStr;
            if (sizeof...(args) > 0) {
                formatInternal(ss, "", args...);
            }
        }
    }
    
    static void formatInternal(std::stringstream& ss, const std::string& formatStr) {
        ss << formatStr;
    }
};

// Mathematical utilities
class MathUtils {
public:
    // Clamp value between min and max
    template<typename T>
    static T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }
    
    // Linear interpolation
    template<typename T>
    static T lerp(T start, T end, float t) {
        return start + (end - start) * t;
    }
    
    // Map value from one range to another
    template<typename T>
    static T mapRange(T value, T fromMin, T fromMax, T toMin, T toMax) {
        if (fromMax == fromMin) return toMin;
        return toMin + (value - fromMin) * (toMax - toMin) / (fromMax - fromMin);
    }
    
    // Calculate exponential moving average
    template<typename T>
    static T ema(T current, T previous, float alpha) {
        return previous * (1.0f - alpha) + current * alpha;
    }
    
    // Calculate moving average
    template<typename T>
    static T movingAverage(const std::vector<T>& values, size_t windowSize) {
        if (values.empty()) return T(0);
        size_t actualWindow = std::min(windowSize, values.size());
        T sum = T(0);
        for (size_t i = 0; i < actualWindow; ++i) {
            sum += values[values.size() - actualWindow + i];
        }
        return sum / static_cast<T>(actualWindow);
    }
    
    // Calculate standard deviation
    template<typename T>
    static T standardDeviation(const std::vector<T>& values) {
        if (values.size() < 2) return T(0);
        
        T mean = std::accumulate(values.begin(), values.end(), T(0)) / static_cast<T>(values.size());
        T sumSq = T(0);
        
        for (const T& val : values) {
            T diff = val - mean;
            sumSq += diff * diff;
        }
        
        return std::sqrt(sumSq / static_cast<T>(values.size() - 1));
    }
    
    // Generate random number in range
    template<typename T>
    static T randomRange(T min, T max) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<double> dist(min, max);
        return static_cast<T>(dist(rng));
    }
    
    // Check if value is approximately equal to another
    template<typename T>
    static bool approximatelyEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) {
        return std::abs(a - b) < epsilon;
    }
    
    // Normalize value to range [0, 1]
    template<typename T>
    static T normalize(T value, T min, T max) {
        return (value - min) / (max - min);
    }
    
    // Denormalize value from range [0, 1] to target range
    template<typename T>
    static T denormalize(float normalized, T min, T max) {
        return min + normalized * (max - min);
    }
    
    // Calculate Euclidean distance between two points
    static double euclideanDistance(double x1, double y1, double x2, double y2) {
        double dx = x1 - x2;
        double dy = y1 - y2;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    // Calculate dot product of two vectors
    template<typename T>
    static T dotProduct(const std::vector<T>& a, const std::vector<T>& b) {
        T result = T(0);
        size_t size = std::min(a.size(), b.size());
        for (size_t i = 0; i < size; ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    // Calculate cross product of two 3D vectors
    static std::array<double, 3> crossProduct(const std::array<double, 3>& a, const std::array<double, 3>& b) {
        return {
            a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]
        };
    }
    
    // Calculate magnitude of vector
    template<typename T>
    static T magnitude(const std::vector<T>& vec) {
        return std::sqrt(dotProduct(vec, vec));
    }
};

// Time utilities
class TimeUtils {
public:
    // Get current time as milliseconds since epoch
    static int64_t getCurrentTimeMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    // Get current time as microseconds since epoch
    static int64_t getCurrentTimeUs() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    // Get current time as seconds since epoch
    static double getCurrentTimeSeconds() {
        return std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    // Format duration as human readable string
    static std::string formatDuration(std::chrono::duration<double> duration) {
        double totalSeconds = duration.count();
        double hours = std::floor(totalSeconds / 3600);
        double minutes = std::floor((totalSeconds - hours * 3600) / 60);
        double seconds = totalSeconds - hours * 3600 - minutes * 60;
        
        std::stringstream ss;
        if (hours > 0) {
            ss << static_cast<int>(hours) << "h ";
        }
        if (minutes > 0 || hours > 0) {
            ss << static_cast<int>(minutes) << "m ";
        }
        ss << std::fixed << std::setprecision(3) << seconds << "s";
        return ss.str();
    }
    
    // Get uptime
    static std::chrono::duration<double> getUptime() {
        return std::chrono::system_clock::now() - startupTime_;
    }
    
    // Get time since last operation
    static std::chrono::duration<double> timeSince(std::chrono::time_point<std::chrono::system_clock> timePoint) {
        return std::chrono::system_clock::now() - timePoint;
    }
    
    // Sleep for specified duration
    static void sleepFor(std::chrono::duration<double> duration) {
        std::this_thread::sleep_for(duration);
    }
    
private:
    static const std::chrono::time_point<std::chrono::system_clock> startupTime_;
};

// File utilities
class FileUtils {
public:
    // Check if file exists
    static bool fileExists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
    
    // Check if directory exists
    static bool directoryExists(const std::string& path) {
        struct stat info;
        return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR) != 0;
    }
    
    // Create directory recursively
    static bool createDirectory(const std::string& path) {
        if (directoryExists(path)) return true;
        
        size_t pos = 0;
        while ((pos = path.find('/', pos)) != std::string::npos) {
            std::string dir = path.substr(0, pos++);
            if (!directoryExists(dir) && !mkdir(dir.c_str(), 0777)) {
                return false;
            }
        }
        
        return mkdir(path.c_str(), 0777) == 0;
    }
    
    // Get file size
    static uint64_t getFileSize(const std::string& path) {
        std::ifstream file(path, std::ifstream::ate | std::ifstream::binary);
        return file.tellg();
    }
    
    // Read file content
    static std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + path);
        }
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    
    // Write content to file
    static bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return true;
    }
    
    // List files in directory
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& extension = "") {
        std::vector<std::string> files;
        if (!directoryExists(directory)) return files;
        
        // Implementation would use platform-specific file listing
        // For simplicity, return empty vector
        return files;
    }
    
    // Get file extension
    static std::string getFileExtension(const std::string& filename) {
        size_t pos = filename.rfind('.');
        if (pos == std::string::npos) return "";
        return filename.substr(pos + 1);
    }
    
    // Get filename without extension
    static std::string getFileNameWithoutExtension(const std::string& filename) {
        size_t pos = filename.rfind('.');
        if (pos == std::string::npos) return filename;
        return filename.substr(0, pos);
    }
    
    // Get full path
    static std::string getFullPath(const std::string& filename) {
        char buffer[1024];
        if (realpath(filename.c_str(), buffer)) {
            return std::string(buffer);
        }
        return filename;
    }
};

// Collection utilities
class CollectionUtils {
public:
    // Find element in vector
    template<typename T, typename Predicate>
    static bool contains(const std::vector<T>& collection, Predicate predicate) {
        return std::find_if(collection.begin(), collection.end(), predicate) != collection.end();
    }
    
    // Find index of element in vector
    template<typename T, typename Predicate>
    static size_t findIndex(const std::vector<T>& collection, Predicate predicate) {
        auto it = std::find_if(collection.begin(), collection.end(), predicate);
        return (it != collection.end()) ? (it - collection.begin()) : static_cast<size_t>(-1);
    }
    
    // Remove element from vector
    template<typename T, typename Predicate>
    static bool removeIf(std::vector<T>& collection, Predicate predicate) {
        auto it = std::remove_if(collection.begin(), collection.end(), predicate);
        bool removed = (it != collection.end());
        if (removed) {
            collection.erase(it, collection.end());
        }
        return removed;
    }
    
    // Add unique element to vector
    template<typename T>
    static bool addUnique(std::vector<T>& collection, const T& value) {
        if (!contains(collection, [&value](const T& item) { return item == value; })) {
            collection.push_back(value);
            return true;
        }
        return false;
    }
    
    // Shuffle vector
    template<typename T>
    static void shuffle(std::vector<T>& collection) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(collection.begin(), collection.end(), gen);
    }
    
    // Reverse vector
    template<typename T>
    static void reverse(std::vector<T>& collection) {
        std::reverse(collection.begin(), collection.end());
    }
    
    // Remove duplicates from vector
    template<typename T>
    static void removeDuplicates(std::vector<T>& collection) {
        std::sort(collection.begin(), collection.end());
        auto last = std::unique(collection.begin(), collection.end());
        collection.erase(last, collection.end());
    }
    
    // Merge two sorted vectors
    template<typename T>
    static std::vector<T> mergeSorted(const std::vector<T>& a, const std::vector<T>& b) {
        std::vector<T> result;
        result.reserve(a.size() + b.size());
        std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
        return result;
    }
    
    // Rotate vector left
    template<typename T>
    static void rotateLeft(std::vector<T>& collection, size_t positions) {
        if (collection.empty()) return;
        positions = positions % collection.size();
        std::rotate(collection.begin(), collection.begin() + positions, collection.end());
    }
    
    // Rotate vector right
    template<typename T>
    static void rotateRight(std::vector<T>& collection, size_t positions) {
        if (collection.empty()) return;
        rotateLeft(collection, collection.size() - (positions % collection.size()));
    }
    
    // Get maximum element
    template<typename T>
    static T max(const std::vector<T>& collection) {
        return *std::max_element(collection.begin(), collection.end());
    }
    
    // Get minimum element
    template<typename T>
    static T min(const std::vector<T>& collection) {
        return *std::min_element(collection.begin(), collection.end());
    }
    
    // Calculate sum
    template<typename T>
    static T sum(const std::vector<T>& collection) {
        return std::accumulate(collection.begin(), collection.end(), T(0));
    }
    
    // Calculate product
    template<typename T>
    static T product(const std::vector<T>& collection) {
        T result = T(1);
        for (const T& value : collection) {
            result *= value;
        }
        return result;
    }
    
    // Check if all elements satisfy predicate
    template<typename T, typename Predicate>
    static bool all(const std::vector<T>& collection, Predicate predicate) {
        return std::all_of(collection.begin(), collection.end(), predicate);
    }
    
    // Check if any element satisfies predicate
    template<typename T, typename Predicate>
    static bool any(const std::vector<T>& collection, Predicate predicate) {
        return std::any_of(collection.begin(), collection.end(), predicate);
    }
    
    // Count elements that satisfy predicate
    template<typename T, typename Predicate>
    static size_t countIf(const std::vector<T>& collection, Predicate predicate) {
        return static_cast<size_t>(std::count_if(collection.begin(), collection.end(), predicate));
    }
    
    // Find minimum and maximum
    template<typename T>
    static std::pair<T, T> minMax(const std::vector<T>& collection) {
        if (collection.empty()) {
            return std::make_pair(T(0), T(0));
        }
        T minVal = *std::min_element(collection.begin(), collection.end());
        T maxVal = *std::max_element(collection.begin(), collection.end());
        return std::make_pair(minVal, maxVal);
    }
    
    // Get median
    template<typename T>
    static T median(std::vector<T> values) {
        if (values.empty()) return T(0);
        std::sort(values.begin(), values.end());
        size_t size = values.size();
        if (size % 2 == 0) {
            return (values[size / 2 - 1] + values[size / 2]) / 2;
        } else {
            return values[size / 2];
        }
    }
    
    // Get mode (most frequent element)
    template<typename T>
    static T mode(const std::vector<T>& collection) {
        if (collection.empty()) return T(0);
        std::unordered_map<T, int> frequencyMap;
        for (const T& value : collection) {
            frequencyMap[value]++;
        }
        
        T modeValue;
        int maxFrequency = 0;
        for (const auto& pair : frequencyMap) {
            if (pair.second > maxFrequency) {
                maxFrequency = pair.second;
                modeValue = pair.first;
            }
        }
        return modeValue;
    }
};

// Memory utilities
class MemoryUtils {
public:
    // Get available memory
    static uint64_t getAvailableMemory() {
        // Platform-specific implementation
        return static_cast<uint64_t>(-1); // Placeholder
    }
    
    // Get used memory
    static uint64_t getUsedMemory() {
        // Platform-specific implementation
        return static_cast<uint64_t>(-1); // Placeholder
    }
    
    // Get total memory
    static uint64_t getTotalMemory() {
        // Platform-specific implementation
        return static_cast<uint64_t>(-1); // Placeholder
    }
    
    // Check if memory limit is exceeded
    static bool isMemoryOverLimit(uint64_t limitBytes) {
        return getUsedMemory() > limitBytes;
    }
    
    // Optimize memory usage
    static void optimizeMemory() {
        // Platform-specific memory optimization
    }
    
    // Allocate memory with checking
    static void* allocateMemory(size_t size) {
        void* ptr = std::malloc(size);
        if (!ptr && size > 0) {
            throw std::bad_alloc();
        }
        return ptr;
    }
    
    // Free memory
    static void freeMemory(void* ptr) {
        if (ptr) {
            std::free(ptr);
        }
    }
    
    // Memory leak detection (placeholder)
    static void detectMemoryLeaks() {
        // Implement memory leak detection
    }
};

// Concurrency utilities
class ConcurrencyUtils {
public:
    // Get number of CPU cores
    static int getCpuCoreCount() {
        return std::thread::hardware_concurrency();
    }
    
    // Execute function in parallel
    template<typename F>
    static void parallelFor(int start, int end, F func, int numThreads = 0) {
        if (numThreads <= 0) {
            numThreads = getCpuCoreCount();
        }
        
        int chunkSize = (end - start + numThreads - 1) / numThreads;
        std::vector<std::thread> threads;
        
        for (int i = 0; i < numThreads; ++i) {
            int threadStart = start + i * chunkSize;
            int threadEnd = std::min(threadStart + chunkSize, end);
            
            if (threadStart < threadEnd) {
                threads.emplace_back([func, threadStart, threadEnd]() {
                    for (int j = threadStart; j < threadEnd; ++j) {
                        func(j);
                    }
                });
            }
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    // Execute function in parallel with results
    template<typename F, typename T>
    static std::vector<T> parallelMap(int start, int end, F func, int numThreads = 0) {
        if (numThreads <= 0) {
            numThreads = getCpuCoreCount();
        }
        
        int chunkSize = (end - start + numThreads - 1) / numThreads;
        std::vector<std::thread> threads;
        std::vector<T> results(end - start);
        
        for (int i = 0; i < numThreads; ++i) {
            int threadStart = start + i * chunkSize;
            int threadEnd = std::min(threadStart + chunkSize, end);
            
            if (threadStart < threadEnd) {
                threads.emplace_back([func, threadStart, threadEnd, &results]() {
                    for (int j = threadStart; j < threadEnd; ++j) {
                        results[j - start] = func(j);
                    }
                });
            }
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        return results;
    }
    
    // Thread pool implementation (simplified)
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads) {
            for (size_t i = 0; i < numThreads; ++i) {
                workers_.emplace_back([this]() {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(queueMutex_);
                            condition.wait(lock, [this]() { return !tasks_.empty() || stop_; });
                            if (stop_ && tasks_.empty()) {
                                return;
                            }
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        task();
                    }
                });
            }
        }
        
        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                stop_ = true;
            }
            condition.notify_all();
            for (auto& worker : workers_) {
                worker.join();
            }
        }
        
        // Submit task
        template<typename F>
        void submit(F&& task) {
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                tasks_.emplace(std::forward<F>(task));
            }
            condition.notify_one();
        }
        
        // Wait for all tasks to complete
        void wait() {
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition.wait(lock, [this]() { return tasks_.empty(); });
        }
        
    private:
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;
        std::mutex queueMutex_;
        std::condition_variable condition;
        bool stop_ = false;
    };
};

} // namespace utils
} // namespace nlm