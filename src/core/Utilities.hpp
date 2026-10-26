#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <numeric>
#include <random>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <variant>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace nlm {

// Common type aliases
using String = std::string;
using Bytes = std::vector<uint8_t>;
using Timestamp = std::chrono::system_clock::time_point;
using Duration = std::chrono::duration<double>;

// Forward declarations
class UUID;

// Enhanced string utilities
namespace string_utils {
    
    // String manipulation utilities
    String trim(const String& str);
    String toLower(const String& str);
    String toUpper(const String& str);
    String capitalize(const String& str);
    String capitalizeFirst(const String& str);
    bool startsWith(const String& str, const String& prefix);
    bool endsWith(const String& str, const String& suffix);
    String removeExtension(const String& filename);
    String getFileExtension(const String& filename);
    String getFileNameWithoutExtension(const String& filename);
    String escapeHtml(const String& str);
    String replaceAll(const String& str, const String& from, const String& to);
    std::vector<String> split(const String& str, char delimiter);
    std::vector<String> split(const String& str, const String& delimiter);
    String join(const std::vector<String>& strings, const String& delimiter);
    
    // Case-insensitive comparison
    bool iequals(const String& a, const String& b);
    
    // UUID generation
    String generateUUID();
    
    // String formatting
    String format(const String& format, ...);
    String formatv(const String& format, std::vector<String> args);
    
    // Check if string contains only digits
    bool isDigits(const String& str);
    
    // Convert string to numeric types
    int toInt(const String& str, int defaultValue = 0);
    double toDouble(const String& str, double defaultValue = 0.0);
    bool toBool(const String& str, bool defaultValue = false);
}

// Memory utilities
namespace memory_utils {
    
    // Safe memory allocation with checking
    template<typename T>
    std::unique_ptr<T> make_unique_safe(size_t count = 1);
    
    // Memory leak detection
    void enableMemoryLeakDetection();
    void reportMemoryLeaks();
    
    // Memory usage monitoring
    size_t getCurrentMemoryUsage();
    size_t getPeakMemoryUsage();
}

// Time utilities
namespace time_utils {
    
    // High precision timing
    double getTimeSeconds();
    uint64_t getTimeTicks();
    Timestamp now();
    Duration elapsedSince(Timestamp start);
    
    // Sleep utilities
    void sleepMilliseconds(uint32_t ms);
    void sleepSeconds(uint32_t sec);
    
    // Performance monitoring
    class Profiler {
    public:
        Profiler(const String& name);
        ~Profiler();
        
        void start();
        void stop();
        double getElapsedSeconds() const;
        uint64_t getElapsedTicks() const;
        
        static void printReport();
        static void clearReport();
        
    private:
        String name_;
        Timestamp startTime_;
        bool isRunning_;
    };
    
    // Timer utility
    class Timer {
    public:
        Timer();
        void reset();
        double elapsed() const;
        bool elapsed(double seconds) const;
        
    private:
        Timestamp start_;
    };
}

// File utilities
namespace file_utils {
    
    // File operations with error handling
    bool fileExists(const String& path);
    bool directoryExists(const String& path);
    size_t fileSize(const String& path);
    String readFile(const String& path);
    bool writeFile(const String& path, const String& content, bool overwrite = false);
    bool appendFile(const String& path, const String& content);
    
    // Directory operations
    bool createDirectory(const String& path);
    bool removeFile(const String& path);
    bool removeDirectory(const String& path, bool recursive = false);
    std::vector<String> listFiles(const String& directory, const String& pattern = "*");
    
    // Path utilities
    String getExecutablePath();
    String getCurrentWorkingDirectory();
    String getTempDirectory();
    
    // Safe file reading/writing with error handling
    class FileReader {
    public:
        FileReader(const String& path);
        ~FileReader();
        
        bool isOpen() const;
        bool eof() const;
        String readLine();
        String readAll();
        bool readAllLines(std::vector<String>& lines);
        bool isGood() const;
        String getError() const;
        
    private:
        std::ifstream file_;
        String error_;
    };
    
    class FileWriter {
    public:
        FileWriter(const String& path, bool append = false);
        ~FileWriter();
        
        bool isOpen() const;
        bool writeLine(const String& line);
        bool write(const String& content);
        bool write(const char* data, size_t size);
        bool isGood() const;
        String getError() const;
        
    private:
        std::ofstream file_;
        String error_;
    };
}

// Collection utilities
namespace collection_utils {
    
    // Mathematical operations on collections
    template<typename T>
    T sum(const std::vector<T>& values);
    
    template<typename T>
    T average(const std::vector<T>& values);
    
    template<typename T>
    T median(std::vector<T> values);
    
    template<typename T>
    T min(const std::vector<T>& values);
    
    template<typename T>
    T max(const std::vector<T>& values);
    
    // Statistical operations
    double standardDeviation(const std::vector<double>& values);
    double variance(const std::vector<double>& values);
    
    // Collection operations
    template<typename T, typename U>
    std::vector<U> map(const std::vector<T>& collection, std::function<U(const T&)> func);
    
    template<typename T>
    std::vector<T> filter(const std::vector<T>& collection, std::function<bool(const T&)> predicate);
    
    template<typename T>
    bool contains(const std::vector<T>& collection, const T& value);
    
    template<typename T>
    size_t count(const std::vector<T>& collection, const T& value);
    
    // Unique and distinct operations
    template<typename T>
    std::vector<T> unique(const std::vector<T>& collection);
    
    // Conversion utilities
    template<typename T>
    std::vector<T> toVector(const std::set<T>& set);
    
    template<typename T>
    std::set<T> toSet(const std::vector<T>& vector);
    
    // Safe access functions
    template<typename Container, typename Key>
    bool safeAccess(const Container& container, const Key& key, typename Container::mapped_type& value);
}

// Threading utilities
namespace thread_utils {
    
    // Thread pooling
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads);
        ~ThreadPool();
        
        template<typename F, typename... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;
        
        void waitForCompletion();
        bool isBusy() const;
        
    private:
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;
        std::mutex queueMutex_;
        std::condition_variable condition_;
        bool stop_;
    };
    
    // Lock utilities
    class LockGuard {
    public:
        LockGuard(std::mutex& mutex);
        ~LockGuard();
        
        LockGuard(const LockGuard&) = delete;
        LockGuard& operator=(const LockGuard&) = delete;
        
    private:
        std::mutex& mutex_;
    };
    
    class UniqueLock {
    public:
        UniqueLock(std::mutex& mutex);
        ~UniqueLock();
        
        bool tryLock();
        void unlock();
        
    private:
        std::mutex& mutex_;
        bool locked_;
    };
}

// Smart pointer utilities
namespace smart_ptr_utils {
    
    // Custom deleters
    template<typename T>
    struct DefaultDeleter {
        void operator()(T* ptr) const {
            delete ptr;
        }
    };
    
    template<typename T>
    struct ArrayDeleter {
        void operator()(T* ptr) const {
            delete[] ptr;
        }
    };
    
    // Make smart pointers with custom deleters
    template<typename T, typename Deleter, typename... Args>
    std::unique_ptr<T, Deleter> makeUnique(Deleter&& deleter, Args&&... args);
}

// UUID utilities
class UUID {
public:
    UUID();
    explicit UUID(const String& uuid);
    
    String toString() const;
    bool isValid() const;
    
    bool operator==(const UUID& other) const;
    bool operator!=(const UUID& other) const;
    bool operator<(const UUID& other) const;
    
private:
    String uuid_;
};

// Error handling utilities
namespace error_utils {
    
    // Exception types
    class NLMException : public std::runtime_error {
    public:
        NLMException(const String& message);
        NLMException(const String& message, const String& file, int line);
        
        const String& getFile() const;
        int getLine() const;
        const String& getFunction() const;
        
    private:
        String file_;
        int line_;
        String function_;
    };
    
    class ConfigurationError : public NLMException {
    public:
        ConfigurationError(const String& message);
    };
    
    class FileError : public NLMException {
    public:
        FileError(const String& message);
    };
    
    class MemoryError : public NLMException {
    public:
        MemoryError(const String& message);
    };
    
    // Error reporting
    void reportError(const std::exception& e);
    void reportError(const String& message);
    
    // Error context
    class ErrorContext {
    public:
        ErrorContext& addContext(const String& key, const String& value);
        const String& getContext(const String& key) const;
        
    private:
        std::map<String, String> context_;
    };
}

// Linear algebra utilities
namespace math_utils {
    
    // Vector operations
    template<typename T>
    std::vector<T> add(const std::vector<T>& a, const std::vector<T>& b);
    
    template<typename T>
    std::vector<T> subtract(const std::vector<T>& a, const std::vector<T>& b);
    
    template<typename T>
    T dotProduct(const std::vector<T>& a, const std::vector<T>& b);
    
    template<typename T>
    double magnitude(const std::vector<T>& v);
    
    template<typename T>
    std::vector<T> normalize(const std::vector<T>& v);
    
    // Matrix operations (basic)
    template<typename T>
    class Matrix {
    public:
        Matrix(size_t rows, size_t cols);
        Matrix(const std::vector<std::vector<T>>& data);
        
        T& at(size_t row, size_t col);
        const T& at(size_t row, size_t col) const;
        
        size_t rows() const;
        size_t cols() const;
        
        Matrix multiply(const Matrix& other) const;
        Matrix transpose() const;
        
    private:
        std::vector<std::vector<T>> data_;
    };
    
    // Interpolation
    template<typename T>
    T lerp(const T& a, const T& b, double t);
    
    // Random utilities
    class Random {
    public:
        Random();
        explicit Random(uint32_t seed);
        
        int nextInt(int min, int max);
        double nextDouble(double min = 0.0, double max = 1.0);
        float nextFloat(float min = 0.0f, float max = 1.0f);
        bool nextBool();
        
        // Distributions
        int nextNormal(double mean, double stddev);
        int nextPoisson(double lambda);
        
    private:
        std::mt19937 generator_;
        std::uniform_int_distribution<int> intDist_;
        std::uniform_real_distribution<double> doubleDist_;
    };
}

// Safe conversion utilities
namespace convert_utils {
    
    // Safe type conversion with error handling
    template<typename To, typename From>
    std::optional<To> safeConvert(const From& value);
    
    // String to numeric conversions
    template<typename T>
    T stringToNumber(const String& str);
    
    // Numeric to string conversions
    template<typename T>
    String numberToString(const T& value);
    
    // Binary conversions
    String toHex(const Bytes& bytes);
    Bytes fromHex(const String& hex);
}

// Progress tracking utilities
class ProgressTracker {
public:
    ProgressTracker(const String& description, size_t total);
    ~ProgressTracker();
    
    void increment(size_t count = 1);
    void incrementTo(size_t value);
    void setDescription(const String& description);
    double getProgress() const;
    bool isComplete() const;
    
    // Display utilities
    void display(bool forceUpdate = false);
    void hide();
    
private:
    String description_;
    size_t total_;
    size_t current_;
    Timestamp startTime_;
    bool displayActive_;
    mutable std::mutex mutex_;
};

// Assertion utilities
namespace assert_utils {
    
    // Standard assertions
    [[noreturn]] void assert(bool condition, const String& message);
    [[noreturn]] void assertNotNull(const void* ptr, const String& message);
    
    // Debug assertions (only in debug builds)
    #ifndef NDEBUG
        #define NLM_ASSERT(condition, message) assert(condition, message)
        #define NLM_ASSERT_NOT_NULL(ptr, message) assertNotNull(ptr, message)
    #else
        #define NLM_ASSERT(condition, message) (void)0
        #define NLM_ASSERT_NOT_NULL(ptr, message) (void)0
    #endif
    
    // Conditional assertions
    void assertEqual(const String& actual, const String& expected, const String& message);
    void assertGreaterThan(double actual, double expected, const String& message);
    void assertLessThan(double actual, double expected, const String& message);
}

} // namespace nlm