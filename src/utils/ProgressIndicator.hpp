#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace nlm {

class ProgressIndicator {
public:
    enum class Style {
        Bar,
        Spinner,
        Dots,
        Percentage,
        Custom
    };
    
    struct Options {
        Style style = Style::Bar;
        size_t width = 50;
        std::string prefix;
        std::string suffix;
        bool showPercentage = true;
        bool showElapsed = true;
        bool showRemaining = true;
        char fillChar = '=';
        char emptyChar = ' ';
        char completeChar = '✓';
        std::vector<std::string> spinnerFrames;
        std::chrono::milliseconds updateInterval{100};
        bool showTimeEstimate = true;
    };
    
    ProgressIndicator(const Options& options = Options());
    ~ProgressIndicator();
    
    void start(const std::string& label = "Progress");
    void update(double progress, const std::string& status = "");
    void update(size_t current, size_t total, const std::string& status = "");
    void setLabel(const std::string& label);
    void setStatus(const std::string& status);
    void increment();
    void finish();
    void finish(const std::string& finalMessage);
    bool isActive() const;
    void setActive(bool active);
    void setTotal(size_t total);
    size_t getCurrent() const;
    size_t getTotal() const;
    double getProgress() const;
    std::chrono::milliseconds getElapsedTime() const;
    std::chrono::milliseconds getEstimatedRemainingTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm