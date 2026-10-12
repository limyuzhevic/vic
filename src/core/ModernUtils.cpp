// NLM Modern C++ Utilities Implementation
// Implementation of modern C++ patterns and utilities

#include "ModernUtils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>

namespace nlm::utils {

// ThreadPool implementation
void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    for (std::thread& worker : workers_) {
        worker.join();
    }
}

// JSONConfig implementation requires nlohmann::json
// This would need to be added as a dependency

// ProgressReporter implementation
void ProgressReporter::increment(size_t amount) {
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

// LRUCache implementation
void LRUCache::printDebug() const {
    std::cout << "Cache contents: ";
    for (auto current = head_.get(); current != nullptr; current = current->next.get()) {
        std::cout << current->key << " ";
    }
    std::cout << std::endl;
}

} // namespace nlm::utils

// Global utility functions for backward compatibility
namespace nlm {
    // Safe file operations
    bool fileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    std::string readFile(const std::string& path) {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + path);
        }
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }
    
    bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::out | std::ios::binary);
        if (!file) {
            return false;
        }
        file << content;
        return true;
    }
    
    // String formatting with fallback
    std::string formatString(const char* fmt, ...) {
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
}
