// Utility functions for NLM (Neural Learning Machine)
// Common operations, helper functions, and utilities for the brain simulation

#ifndef NLM_UTILITIES_HPP
#define NLM_UTILITIES_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <memory>
#include <stdexcept>
#include <filesystem>
#include <cstdint>
#include <limits>
#include <random>
#include <iomanip>
#include <map>

namespace nlm {
namespace utils {

// ===== STRING UTILITIES =====

// Convert string to lowercase
inline std::string toLowercase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Convert string to uppercase
inline std::string toUppercase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

// Trim whitespace from both ends of a string
inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return str;
    
    size_t last = str.find_last_not_of(" \t\n\r");
    if (last == std::string::npos) return str;
    
    return str.substr(first, (last - first + 1));
}

// Split string by delimiter
inline std::vector<std::string> split(const std::string& str, char delimiter = ' ') {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(trim(token));
        }
    }
    return tokens;
}

// Join vector of strings with delimiter
inline std::string join(const std::vector<std::string>& strings, char delimiter = ' ') {
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) ss << delimiter;
        ss << strings[i];
    }
    return ss.str();
}

// Check if string starts with prefix
inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

// Check if string ends with suffix
inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
}

// ===== FILE UTILITIES =====

// Check if file exists
inline bool fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

// Get file extension
inline std::string getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

// Create directory if it doesn't exist
inline bool ensureDirectory(const std::string& path) {
    try {
        if (!path.empty()) {
            std::filesystem::path dir(path);
            if (!dir.empty() && !std::filesystem::exists(dir)) {
                return std::filesystem::create_directories(dir);
            }
        }
    } catch (const std::exception&) {
        return false;
    }
    return true;
}

// ===== VALIDATION UTILITIES =====

// Validate numeric range
inline bool inRange(double value, double min, double max, bool inclusive = true) {
    if (inclusive) {
        return value >= min && value <= max;
    } else {
        return value > min && value < max;
    }
}

// Parse integer with error handling
inline bool parseInt(const std::string& str, int& value, int min = INT_MIN, int max = INT_MAX) {
    try {
        size_t pos;
        value = std::stoi(str, &pos);
        if (pos != str.size()) return false;
        return inRange(value, min, max);
    } catch (const std::exception&) {
        return false;
    }
}

// Parse floating point with error handling
inline bool parseFloat(const std::string& str, float& value, float min = -std::numeric_limits<float>::max(), 
                      float max = std::numeric_limits<float>::max()) {
    try {
        size_t pos;
        value = std::stof(str, &pos);
        if (pos != str.size()) return false;
        return inRange(value, min, max);
    } catch (const std::exception&) {
        return false;
    }
}

// Parse double with error handling
inline bool parseDouble(const std::string& str, double& value, double min = -std::numeric_limits<double>::max(),
                       double max = std::numeric_limits<double>::max()) {
    try {
        size_t pos;
        value = std::stod(str, &pos);
        if (pos != str.size()) return false;
        return inRange(value, min, max);
    } catch (const std::exception&) {
        return false;
    }
}

// ===== STRING FORMAT UTILITIES =====

// Format float with specified precision
inline std::string formatFloat(float value, int precision = 6) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

// Format double with specified precision
inline std::string formatDouble(double value, int precision = 6) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

// Format integer with thousands separators
inline std::string formatNumber(int64_t number) {
    std::stringstream ss;
    ss << number;
    std::string str = ss.str();
    int insertPos = static_cast<int>(str.length() - 3);
    while (insertPos > 0) {
        str.insert(insertPos, ",");
        insertPos -= 3;
    }
    return str;
}

// ===== VECTOR UTILITIES =====

// Calculate average of vector elements
inline double vectorAverage(const std::vector<double>& vec) {
    if (vec.empty()) return 0.0;
    double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
    return sum / vec.size();
}

// Calculate standard deviation of vector elements
inline double vectorStandardDeviation(const std::vector<double>& vec) {
    if (vec.size() < 2) return 0.0;
    double mean = vectorAverage(vec);
    double variance = 0.0;
    for (double value : vec) {
        variance += (value - mean) * (value - mean);
    }
    return std::sqrt(variance / (vec.size() - 1));
}

// Clamp vector values to range
inline std::vector<double> clampVector(const std::vector<double>& vec, double min, double max) {
    std::vector<double> result = vec;
    for (auto& value : result) {
        if (value < min) value = min;
        if (value > max) value = max;
    }
    return result;
}

// Normalize vector to range [0, 1]
inline std::vector<double> normalizeVector(const std::vector<double>& vec) {
    if (vec.empty()) return vec;
    double minVal = *std::min_element(vec.begin(), vec.end());
    double maxVal = *std::max_element(vec.begin(), vec.end());
    double range = maxVal - minVal;
    
    if (range == 0.0) return std::vector<double>(vec.size(), 0.5);
    
    std::vector<double> result;
    for (double value : vec) {
        result.push_back((value - minVal) / range);
    }
    return result;
}

// ===== MAP UTILITIES =====

// Get value from map with default if not found
inline double getMapValue(const std::unordered_map<std::string, double>& map, const std::string& key, double defaultValue = 0.0) {
    auto it = map.find(key);
    return it != map.end() ? it->second : defaultValue;
}

// Find key in map by value
inline std::string findKeyByValue(const std::unordered_map<std::string, double>& map, double value, double tolerance = 1e-9) {
    for (const auto& pair : map) {
        if (std::abs(pair.second - value) < tolerance) {
            return pair.first;
        }
    }
    return "";
}

// ===== STRING STREAM UTILITIES =====

// Safe string stream parsing
class SafeStringStream {
private:
    std::stringstream stream_;
    std::string remaining_;
public:
    SafeStringStream(const std::string& str) : stream_(str) {}
    
    bool parseInt(int& value) {
        std::string token;
        if (std::getline(stream_, token, ' ')) {
            try {
                value = std::stoi(token);
                return true;
            } catch (const std::exception&) {
                return false;
            }
        }
        return false;
    }
    
    bool parseFloat(float& value) {
        std::string token;
        if (std::getline(stream_, token, ' ')) {
            try {
                value = std::stof(token);
                return true;
            } catch (const std::exception&) {
                return false;
            }
        }
        return false;
    }
    
    std::string getRemaining() const {
        return stream_.str();
    }
    
    bool eof() const {
        return stream_.eof();
    }
};

// ===== TIMING UTILITIES =====

// High-resolution timer class
class HighResolutionTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point endTime_;
    bool running_ = false;
public:
    HighResolutionTimer() : running_(false) {}
    
    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
        running_ = true;
    }
    
    void stop() {
        endTime_ = std::chrono::high_resolution_clock::now();
        running_ = false;
    }
    
    double elapsedSeconds() const {
        auto end = running_ ? std::chrono::high_resolution_clock::now() : endTime_;
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime_);
        return duration.count() / 1000000.0;
    }
    
    double elapsedMilliseconds() const {
        auto end = running_ ? std::chrono::high_resolution_clock::now() : endTime_;
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime_);
        return duration.count() / 1000.0;
    }
    
    bool isRunning() const { return running_; }
};

// ===== RANDOM UTILITIES =====

// Thread-safe random number generation
class ThreadSafeRandom {
private:
    std::mt19937 rng_;
    std::mutex mutex_;
    static thread_local std::mt19937 threadLocalRng_;
    static thread_local bool initialized_;
    
    static void initializeThreadLocal() {
        if (!initialized_) {
            std::random_device rd;
            threadLocalRng_.seed(rd());
            initialized_ = true;
        }
    }
public:
    ThreadSafeRandom() : rng_(std::random_device{}()) {}
    
    ThreadSafeRandom(uint64_t seed) : rng_(seed) {}
    
    int uniformInt(int min, int max) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng_);
    }
    
    float uniformFloat(float min = 0.0f, float max = 1.0f) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng_);
    }
    
    double uniformDouble(double min = 0.0, double max = 1.0) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::uniform_real_distribution<double> dist(min, max);
        return dist(rng_);
    }
    
    // Thread-local version
    static int threadLocalUniformInt(int min, int max) {
        initializeThreadLocal();
        std::uniform_int_distribution<int> dist(min, max);
        return dist(threadLocalRng_);
    }
    
    static float threadLocalUniformFloat(float min = 0.0f, float max = 1.0f) {
        initializeThreadLocal();
        std::uniform_real_distribution<float> dist(min, max);
        return dist(threadLocalRng_);
    }
    
    static double threadLocalUniformDouble(double min = 0.0, double max = 1.0) {
        initializeThreadLocal();
        std::uniform_real_distribution<double> dist(min, max);
        return dist(threadLocalRng_);
    }
};

// ===== ERROR HANDLING UTILITIES =====

// Custom exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    enum class Type {
        ConfigurationError,
        FileError,
        MemoryError,
        SimulationError,
        ValidationError,
        UnknownError
    };
    
    NLMException(const std::string& message, Type type = Type::UnknownError)
        : std::runtime_error(message), type_(type) {}
    
    Type getType() const { return type_; }
    
private:
    Type type_;
};

// ===== COLOR AND TERMINAL UTILITIES =====

// Terminal color codes for output formatting
namespace Color {
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string RESET = "\033[0m";
    const std::string CLEAR_LINE = "\033[2K";
    const std::string CURSOR_UP = "\033[A";
    const std::string ERASE_LINE = "\033[2K";
    const std::string MOVE_CURSOR = "\033[%d;%dH";
}

// Check if terminal supports colors
inline bool terminalSupportsColors() {
    // Check for common terminal environment variables
    const char* term = std::getenv("TERM");
    if (!term) return false;
    
    std::string termStr = toLowercase(term);
    return termStr.find("color") != std::string::npos || 
           termStr.find("truecolor") != std::string::npos ||
           termStr.find("256color") != std::string::npos;
}

// ===== CONCURRENT UTILITIES =====

// Thread pool for parallel operations
class SimpleThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_ = false;
    size_t numWorkers_;
public:
    SimpleThreadPool(size_t numWorkers = std::thread::hardware_concurrency())
        : numWorkers_(numWorkers) {
        for (size_t i = 0; i < numWorkers_; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex_);
                        this->condition_.wait(lock, [this] {
                            return this->stop_ || !this->tasks_.empty();
                        });
                        if (this->stop_ && this->tasks_.empty()) {
                            return;
                        }
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    task();
                }
            });
        }
    }
    
    ~SimpleThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
    }
    
    template<class F>
    void enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            tasks_.emplace(std::forward<F>(task));
        }
        condition_.notify_one();
    }
    
    size_t getNumWorkers() const { return numWorkers_; }
    
    size_t getQueueSize() {
        std::unique_lock<std::mutex> lock(queueMutex_);
        return tasks_.size();
    }
};

// ===== PROGRESS UTILITIES =====

// Simple progress bar utility
class ProgressBar {
private:
    size_t total_;
    size_t current_ = 0;
    size_t width_ = 50;
    std::string label_;
    bool showPercentage_ = true;
    bool showEstimatedTime_ = true;
    bool showSpeed_ = true;
    
    HighResolutionTimer timer_;
    std::chrono::high_resolution_clock::time_point startTime_;
public:
    ProgressBar(size_t total, const std::string& label = "Progress")
        : total_(total), label_(label) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void increment(size_t amount = 1) {
        current_ += amount;
        printProgress();
    }
    
    void setCurrent(size_t current) {
        current_ = current;
        printProgress();
    }
    
    void setWidth(size_t width) {
        width_ = width;
    }
    
    void setShowPercentage(bool show) {
        showPercentage_ = show;
    }
    
    void setShowEstimatedTime(bool show) {
        showEstimatedTime_ = show;
    }
    
    void setShowSpeed(bool show) {
        showSpeed_ = show;
    }
    
    void printProgress() {
        if (total_ == 0) return;
        
        // Clear line and move cursor to beginning
        std::cout << "\r" << Color::CLEAR_LINE;
        
        // Print label
        std::cout << label_ << ": ";
        
        // Print progress bar
        float progress = static_cast<float>(current_) / total_;
        size_t filled = static_cast<size_t>(width_ * progress);
        
        std::cout << "[";
        for (size_t i = 0; i < filled; ++i) {
            std::cout << "=";
        }
        if (filled < width_) {
            std::cout << ">";
            for (size_t i = filled + 1; i < width_; ++i) {
                std::cout << " ";
            }
        }
        std::cout << "]";
        
        // Print percentage
        if (showPercentage_) {
            std::cout << " " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
        }
        
        // Print estimated time remaining
        if (showEstimatedTime_ && current_ > 0) {
            auto elapsed = timer_.elapsedSeconds();
            auto remaining = (elapsed / current_) * (total_ - current_);
            auto hours = static_cast<size_t>(remaining / 3600);
            auto minutes = static_cast<size_t>((remaining - hours * 3600) / 60);
            auto seconds = static_cast<size_t>(remaining - hours * 3600 - minutes * 60);
            
            std::cout << "  ETA: ";
            if (hours > 0) std::cout << hours << "h ";
            if (hours > 0 || minutes > 0) std::cout << minutes << "m ";
            std::cout << seconds << "s";
        }
        
        // Print speed
        if (showSpeed_ && current_ > 0) {
            auto elapsed = timer_.elapsedSeconds();
            float speed = current_ / elapsed;
            std::cout << "  Speed: " << std::fixed << std::setprecision(1) << speed << " items/sec";
        }
        
        // Flush output
        std::cout << std::flush;
        
        if (current_ >= total_) {
            std::cout << "\n" << Color::CLEAR_LINE;
        }
    }
    
    void finish() {
        if (current_ < total_) {
            setCurrent(total_);
            std::cout << "\n";
        }
    }
};

} // namespace utils
} // namespace nlm

#endif // NLM_UTILITIES_HPP
