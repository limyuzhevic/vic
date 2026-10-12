// NLM Modern C++ Utilities
// Modern C++ patterns and utilities for improved code quality

#ifndef NLM_MODERN_UTILS_HPP
#define NLM_MODERN_UTILS_HPP

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>
#include <expected>
#include <format>
#include <threadpool>
#include <filesystem>
#include <span>

namespace nlm::utils {

// Type aliases for common patterns
using Timestamp = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::duration<double>;
using String = std::string;
using Bytes = std::vector<uint8_t>;

template<typename T>
using Optional = std::optional<T>;

template<typename... Args>
using Expected = std::expected<Args...>;

// Result type for error handling
enum class ResultCode {
    Success,
    InvalidArgument,
    NotFound,
    PermissionDenied,
    Timeout,
    InternalError,
    NotImplemented
};

// Modern smart pointer factory
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Thread pool wrapper for parallel processing
class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_ = false;
    
public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex_);
                        this->condition_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });
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
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread& worker : workers_) {
            worker.join();
        }
    }
    
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        }
        condition_.notify_one();
    }
    
    bool isEmpty() {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        return tasks_.empty();
    }
};

// Performance timer utilities
class Timer {
private:
    Timestamp start_time_;
    Timestamp end_time_;
    bool running_ = false;
    
public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}
    
    void start() {
        start_time_ = std::chrono::high_resolution_clock::now();
        running_ = true;
    }
    
    void stop() {
        end_time_ = std::chrono::high_resolution_clock::now();
        running_ = false;
    }
    
    Duration elapsed() const {
        auto end = running_ ? std::chrono::high_resolution_clock::now() : end_time_;
        return std::chrono::duration_cast<Duration>(end - start_time_);
    }
    
    double elapsedSeconds() const {
        return elapsed().count();n    }
    
    // For convenience - measure duration of a lambda
    template<class F>
    static Duration measure(F&& f) {
        Timer timer;
        f();
        return timer.elapsed();
    }
};

// String formatting utilities
inline std::string formatString(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::string result;
    try {
        result = std::vformat(fmt, std::make_format_args(args));
    } catch (const std::format_error&) {
        // Fallback for older C++ versions
        char buffer[1024];
        std::vsnprintf(buffer, sizeof(buffer), fmt, args);
        result = buffer;
    }
    va_end(args);
    return result;
}

// Safe file operations
inline bool fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

inline std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

inline bool writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::out | std::ios::binary);
    if (!file) {
        return false;
    }
    file << content;
    return true;
}

// Mathematical utilities
template<typename T>
T clamp(T value, T min, T max) {
    return std::max(min, std::min(value, max));
}

template<typename T>
T lerp(T a, T b, float t) {
    return a + (b - a) * t;
}

// Progress reporting utilities
class ProgressReporter {
private:
    size_t total_;
    size_t current_ = 0;
    std::string description_;
    std::chrono::steady_clock::time_point start_time_;
    bool show_progress_ = true;
    
public:
    ProgressReporter(size_t total, const std::string& description = "Progress")
        : total_(total), description_(description), start_time_(std::chrono::steady_clock::now()) {}
    
    void increment(size_t amount = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        current_ += amount;
        if (show_progress_ && current_ % 100 == 0) {
            double percentage = (static_cast<double>(current_) / total_) * 100.0;
            auto elapsed = std::chrono::steady_clock::now() - start_time_;
            double eta = (elapsed.count() / current_) * (total_ - current_);
            
            std::cout << formatString("\r%s: %d/%d (%.1f%%) ETA: %.1fs",
                                     description_.c_str(), current_, total_, percentage, eta.count())
                     << std::flush;
            
            if (current_ >= total_) {
                std::cout << "\n" << description_ << " complete!" << std::endl;
                show_progress_ = false;
            }
        }
    }
    
    void finish() {
        if (show_progress_) {
            double percentage = (static_cast<double>(current_) / total_) * 100.0;
            std::cout << formatString("\r%s: %d/%d (%.1f%%)",
                                     description_.c_str(), current_, total_, percentage)
                     << std::endl;
            show_progress_ = false;
        }
    }
};

// JSON configuration utilities (Phase 3 enhancement)
class JSONConfig {
private:
    std::unordered_map<std::string, std::variant<std::string, int, double, bool, std::vector<std::variant<std::string, int, double, bool>>>> data_;
    
public:
    bool loadFromFile(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json json_data = nlohmann::json::parse(file);
            data_ = json_data.get<decltype(data_)>();
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    template<typename T>
    Optional<T> get(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    void set(const std::string& key, const std::string& value) {
        data_[key] = value;
    }
    
    void set(const std::string& key, int value) {
        data_[key] = value;
    }
    
    void set(const std::string& key, double value) {
        data_[key] = value;
    }
    
    void set(const std::string& key, bool value) {
        data_[key] = value;
    }
    
    std::string summary() const {
        std::ostringstream oss;
        oss << "JSON Configuration (" << data_.size() << " entries):\n";
        for (const auto& [key, value] : data_) {
            oss << "  " << key << " = ";
            std::visit([&oss](const auto& v) { oss << v; }, value);
            oss << "\n";
        }
        return oss.str();
    }
};

// Cache utilities for frequently accessed data
class LRUCache {
private:
    struct CacheNode {
        std::string key;
        std::any value;
        Timestamp timestamp;
        std::unique_ptr<CacheNode> next;
        
        CacheNode(const std::string& k, const std::any& v) : key(k), value(v), timestamp(std::chrono::high_resolution_clock::now()) {}
    };
    
    std::unique_ptr<CacheNode> head_;
    size_t capacity_;
    std::unordered_map<std::string, std::unique_ptr<CacheNode>> nodes_;
    
public:
    LRUCache(size_t capacity) : capacity_(capacity) {}
    
    bool contains(const std::string& key) const {
        return nodes_.find(key) != nodes_.end();
    }
    
    std::any get(const std::string& key) {
        auto it = nodes_.find(key);
        if (it == nodes_.end()) {
            throw std::runtime_error("Key not found: " + key);
        }
        
        // Move to front (LRU)
        auto& node = it->second;
        if (head_.get() != node.get()) {
            node->next = std::move(head_);
            head_ = std::move(it->second);
            nodes_[key] = std::move(head_);
        }
        
        return node->value;
    }
    
    void put(const std::string& key, const std::any& value) {
        auto it = nodes_.find(key);
        
        if (it != nodes_.end()) {
            // Update existing
            it->second->value = value;
            it->second->timestamp = std::chrono::high_resolution_clock::now();
            
            // Move to front
            if (head_.get() != it->second.get()) {
                auto& node = it->second;
                node->next = std::move(head_);
                head_ = std::move(it->second);
                nodes_[key] = std::move(head_);
            }
            return;
        }
        
        // Add new node
        auto newNode = std::make_unique<CacheNode>(key, value);
        newNode->next = std::move(head_);
        head_ = std::move(newNode);
        nodes_[key] = head_.get();
        
        // Remove oldest if over capacity
        if (nodes_.size() > capacity_) {
            std::string oldestKey;
            {
                auto current = head_.get();
                for (auto node = current; node != nullptr; node = node->next.get()) {
                    if (node->next) {
                        oldestKey = node->next->key;
                        break;
                    }
                }
            }
            
            if (!oldestKey.empty()) {
                nodes_.erase(oldestKey);
            }
        }
    }
    
    void clear() {
        head_.reset();
        nodes_.clear();
    }
    
    size_t size() const {
        return nodes_.size();
    }
    
    bool isEmpty() const {
        return nodes_.empty();
    }
};

} // namespace nlm::utils

#endif // NLM_MODERN_UTILS_HPP
