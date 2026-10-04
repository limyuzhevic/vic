#pragma once

#include <algorithm>
#include <numeric>
#include <type_traits>
#include <optional>
#include <vector>
#include <numeric>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <any>

namespace nlm {

// Forward declarations
class Timer;

// Container utilities
namespace container {
    template<typename Container, typename T>
    bool contains(const Container& container, const T& value) {
        return std::find(container.begin(), container.end(), value) != container.end();
    }
    
    template<typename Container, typename Func>
    auto filter(const Container& container, Func predicate) -> std::vector<typename Container::value_type> {
        std::vector<typename Container::value_type> result;
        for (const auto& item : container) {
            if (predicate(item)) {
                result.push_back(item);
            }
        }
        return result;
    }
    
    template<typename Container, typename Func>
    auto transform(const Container& container, Func func) -> std::vector<decltype(func(*container.begin()))> {
        std::vector<decltype(func(*container.begin()))> result;
        result.reserve(container.size());
        for (const auto& item : container) {
            result.push_back(func(item));
        }
        return result;
    }
    
    template<typename Container, typename Func>
    auto map(const Container& container, Func func) -> std::unordered_map<typename Container::value_type, decltype(func(*container.begin()))> {
        std::unordered_map<typename Container::value_type, decltype(func(*container.begin()))> result;
        for (const auto& item : container) {
            result[item] = func(item);
        }
        return result;
    }
}

// Mathematical utilities
namespace math {
    template<typename T>
    T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }
    
    template<typename T>
    T lerp(T start, T end, T t) {
        return start + (end - start) * t;
    }
    
    template<typename T>
    T easeInOut(T t) {
        if (t < 0.5) return 2 * t * t;
        return -1 + (4 - 2 * t) * t;
    }
    
    // Statistical utilities
    template<typename Container>
    auto mean(const Container& container) {
        if (container.empty()) return typename Container::value_type{};
        typename Container::value_type sum = std::accumulate(container.begin(), container.end(), typename Container::value_type{});
        return sum / container.size();
    }
    
    template<typename Container>
    auto variance(const Container& container) {
        if (container.size() < 2) return typename Container::value_type{};
        auto mn = mean(container);
        typename Container::value_type sum = 0;
        for (const auto& val : container) {
            sum += (val - mn) * (val - mn);
        }
        return sum / (container.size() - 1);
    }
    
    template<typename Container>
    auto standardDeviation(const Container& container) {
        return std::sqrt(variance(container));
    }
}

// Hash utilities for custom types
namespace hash {
    template<typename T>
    struct Hash {
        size_t operator()(const T& value) const {
            return std::hash<T>{}(value);
        }
    };
    
    // Specialized hash for ID types
    template<typename T>
    struct IDHash {
        size_t operator()(const T& id) const {
            return static_cast<size_t>(id.index());
        }
    };
}

// Callback utilities
template<typename... Args>
using Callback = std::function<void(Args...)>;

template<typename... Args>
using Predicate = std::function<bool(Args...)>;

// Timer utilities
class Timer {
public:
    Timer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~Timer() = default;
    
    // Disable copying
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    
    // Enable moving
    Timer(Timer&&) noexcept = default;
    Timer& operator=(Timer&&) noexcept = default;
    
    // Get elapsed time
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
        return duration.count();
    }
    
    // Reset timer
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // Get current time point
    static std::chrono::high_resolution_clock::time_point now() {
        return std::chrono::high_resolution_clock::now();
    }
    
    // Sleep for specified duration
    static void sleep(double seconds) {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Memory utilities
template<typename T>
class MemoryPool {
public:
    MemoryPool() = default;
    ~MemoryPool() = default;
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // Enable moving
    MemoryPool(MemoryPool&&) noexcept = default;
    MemoryPool& operator=(MemoryPool&&) noexcept = default;
    
    T* allocate() {
        return static_cast<T*>(std::malloc(sizeof(T)));
    }
    
    void deallocate(T* ptr) {
        std::free(ptr);
    }
    
    template<typename... Args>
    T* create(Args&&... args) {
        T* ptr = allocate();
        if (ptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
        return ptr;
    }
    
    void destroy(T* ptr) {
        if (ptr) {
            ptr->~T();
            deallocate(ptr);
        }
    }
};

// Smart pointer factory functions
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
std::shared_ptr<T> make_shared() {
    return std::make_shared<T>();
}

// String utilities
namespace string {
    // Case conversion
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    
    // String formatting (simple implementation)
    template<typename... Args>
    std::string format(const std::string& format, Args&&... args) {
        // Simple implementation - in production use std::format from C++20
        std::string result = format;
        // TODO: Implement proper formatting
        return result;
    }
    
    // Trimming
    std::string trim(const std::string& str);
    
    // Split
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Join
    template<typename Container>
    std::string join(const Container& container, char delimiter) {
        if (container.empty()) return "";
        std::string result;
        auto it = container.begin();
        result += *it;
        for (++it; it != container.end(); ++it) {
            result += delimiter;
            result += *it;
        }
        return result;
    }
}

// Filesystem utilities
namespace fs {
    // Check if file exists
    bool exists(const std::string& path);
    
    // Get file size
    uint64_t fileSize(const std::string& path);
    
    // Read file content
    std::string readFile(const std::string& path);
    
    // Write file content
    bool writeFile(const std::string& path, const std::string& content);
}

// Concurrency utilities
namespace concurrency {
    // Thread pool
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads);
        ~ThreadPool();
        
        // Disable copying
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        
        // Enable moving
        ThreadPool(ThreadPool&&) noexcept = default;
        ThreadPool& operator=(ThreadPool&&) noexcept = default;
        
        // Submit task
        template<typename F>
        void submit(F&& task);
        
        // Wait for all tasks to complete
        void wait();
        
        // Get number of threads
        size_t threadCount() const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}

} // namespace nlm
