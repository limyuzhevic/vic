#pragma once

#include <string>
#include <chrono>
#include <functional>
#include <memory>

namespace nlm {

class LoggerHelper;

class TimerUtils {
public:
    // High-resolution timer
    class Timer {
    public:
        Timer();
        ~Timer();
        
        void start();
        void stop();
        void reset();
        
        // Get elapsed time in various units
        double elapsedSeconds() const;
        double elapsedMilliseconds() const;
        double elapsedMicroseconds() const;
        std::chrono::milliseconds elapsedMs() const;
        std::chrono::microseconds elapsedUs() const;
        
        // Check if timer is running
        bool isRunning() const;
        
        // Start-stop wrapper for single-shot timing
        double measure(std::function<void()> func);

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

    // Benchmark utilities
    template<typename Func, typename... Args>
    static double benchmark(Func&& func, Args&&... args);

    static std::string formatTime(double seconds);
    static std::string formatTime(const std::chrono::steady_clock::duration& duration);
    
    // Scope timing for automatic logging
    class ScopedTimer {
    public:
        ScopedTimer(const std::string& label, LoggerHelper::LogLevel level = LoggerHelper::LogLevel::INFO);
        ~ScopedTimer();
        
        double elapsed() const;

    private:
        std::string label_;
        LoggerHelper::LogLevel level_;
        Timer timer_;
    };
};

} // namespace nlm
