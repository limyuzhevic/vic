#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

// Forward declarations
class Tool;

// Tool base class
class Tool {
public:
    virtual ~Tool() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getUsage() const = 0;
    
    virtual bool initialize(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual void shutdown() = 0;
    
    virtual bool isInteractive() const = 0;
    virtual void setInteractive(bool interactive) = 0;
    
    // Common tool functionality
    virtual void printHelp() const;
    virtual void printVersion() const;
    virtual bool validateArguments(const std::vector<std::string>& args) const;
    virtual void log(const std::string& message, bool error = false) const;
    
protected:
    std::string trim(const std::string& str) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
    std::string toLower(const std::string& str) const;
    std::string toUpper(const std::string& str) const;
};

// Progress reporting
class ProgressReporter {
public:
    ProgressReporter(size_t total, const std::string& label = "Progress")
        : total_(total), completed_(0), label_(label) {}
    
    void update(size_t completed) {
        std::lock_guard<std::mutex> lock(mutex_);
        completed_ = completed;
        printProgress();
    }
    
    void increment() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++completed_;
        printProgress();
    }
    
private:
    void printProgress() {
        float percentage = (static_cast<float>(completed_) / total_) * 100.0f;
        std::cout << "\r" << label_ << ": [";
        size_t barWidth = 50;
        size_t pos = (percentage * barWidth) / 100;
        for (size_t i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << percentage 
                  << "% (" << completed_ << "/" << total_ << ")";
        std::cout.flush();
        
        if (completed_ >= total_) {
            std::cout << std::endl;
        }
    }
    
    size_t total_;
    std::atomic<size_t> completed_;
    std::string label_;
    mutable std::mutex mutex_;
};

// Tool context
struct ToolContext {
    std::string toolName;
    std::string version;
    std::shared_ptr<nlm::Config> config;
    bool interactive;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    ToolContext() : interactive(true), startTime(std::chrono::steady_clock::now()) {}
    
    double getElapsedTime() const {
        auto end = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(end - startTime).count();
    }
    
    void finish() {
        endTime = std::chrono::steady_clock::now();
    }
    
    double getTotalTime() const {
        if (endTime.time_since_epoch().count() == 0) {
            return getElapsedTime();
        }
        return std::chrono::duration<double>(endTime - startTime).count();
    }
};

} // namespace nlm