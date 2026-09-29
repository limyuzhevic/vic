#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

namespace nlm {

class HighResTimer {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    
    TimePoint startTime_;
    TimePoint lastIntervalStart_;
    bool running_ = false;
    std::string name_;

public:
    HighResTimer() : HighResTimer("") {}
    
    explicit HighResTimer(const std::string& name) : name_(name) {
        reset();
    }
    
    void reset() {
        startTime_ = Clock::now();
        lastIntervalStart_ = startTime_;
        running_ = true;
    }
    
    void start() {
        if (!running_) {
            startTime_ = Clock::now();
            lastIntervalStart_ = startTime_;
            running_ = true;
        }
    }
    
    void stop() {
        running_ = false;
    }
    
    void pause() {
        if (running_) {
            auto now = Clock::now();
            startTime_ += (now - lastIntervalStart_);
            running_ = false;
        }
    }
    
    void resume() {
        if (!running_) {
            lastIntervalStart_ = Clock::now();
            running_ = true;
        }
    }
    
    double elapsedSeconds() const {
        auto end = Clock::now();
        return std::chrono::duration<double>(end - startTime_).count();
    }
    
    double elapsedMilliseconds() const {
        return elapsedSeconds() * 1000.0;
    }
    
    double elapsedMicroseconds() const {
        return elapsedSeconds() * 1000000.0;
    }
    
    double elapsedNanoseconds() const {
        return elapsedSeconds() * 1000000000.0;
    }
    
    bool isRunning() const {
        return running_;
    }
    
    std::string getName() const {
        return name_;
    }
    
    void setName(const std::string& name) {
        name_ = name;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Timer: " << name_ << "\n";
        ss << "  Status: " << (running_ ? "Running" : "Stopped") << "\n";
        ss << "  Elapsed: " << std::fixed << std::setprecision(6) 
           << elapsedSeconds() << " seconds\n";
        ss << "  Milliseconds: " << elapsedMilliseconds() << " ms\n";
        ss << "  Microseconds: " << elapsedMicroseconds() << " μs\n";
        ss << "  Nanoseconds: " << elapsedNanoseconds() << " ns\n";
        return ss.str();
    }
};

class ManualTimer : public HighResTimer {
public:
    ManualTimer(const std::string& name = "") : HighResTimer(name) {}
    
    double measure( std::function<void()> func) {
        this->start();
        func();
        this->stop();
        return this->elapsedSeconds();
    }
    
    double measure(std::function<void()> func, const std::string& name) {
        setName(name);
        return measure(func);
    }
};

} // namespace nlm
