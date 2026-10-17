#include "TimerUtils.hpp"
#include "LoggerHelper.hpp"
#include <chrono>
#include <cmath>

namespace nlm {

// TimerUtils implementation

struct TimerUtils::Timer::Impl {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point stopTime;
    bool isRunning;
};

TimerUtils::Timer::Timer() : pImpl(std::make_unique<Impl>()) {
    reset();
}

TimerUtils::Timer::~Timer() = default;

void TimerUtils::Timer::start() {
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->isRunning = true;
}

void TimerUtils::Timer::stop() {
    pImpl->stopTime = std::chrono::steady_clock::now();
    pImpl->isRunning = false;
}

void TimerUtils::Timer::reset() {
    pImpl->isRunning = false;
}

double TimerUtils::Timer::elapsedSeconds() const {
    auto endTime = pImpl->isRunning ? std::chrono::steady_clock::now() : pImpl->stopTime;
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - pImpl->startTime);
    return duration.count();
}

double TimerUtils::Timer::elapsedMilliseconds() const {
    auto endTime = pImpl->isRunning ? std::chrono::steady_clock::now() : pImpl->stopTime;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - pImpl->startTime);
    return duration.count();
}

double TimerUtils::Timer::elapsedMicroseconds() const {
    auto endTime = pImpl->isRunning ? std::chrono::steady_clock::now() : pImpl->stopTime;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - pImpl->startTime);
    return duration.count();
}

std::chrono::milliseconds TimerUtils::Timer::elapsedMs() const {
    auto endTime = pImpl->isRunning ? std::chrono::steady_clock::now() : pImpl->stopTime;
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - pImpl->startTime);
}

std::chrono::microseconds TimerUtils::Timer::elapsedUs() const {
    auto endTime = pImpl->isRunning ? std::chrono::steady_clock::now() : pImpl->stopTime;
    return std::chrono::duration_cast<std::chrono::microseconds>(endTime - pImpl->startTime);
}

bool TimerUtils::Timer::isRunning() const {
    return pImpl->isRunning;
}

double TimerUtils::Timer::measure(std::function<void()> func) {
    start();
    func();
    stop();
    return elapsedSeconds();
}

template<typename Func, typename... Args>
double TimerUtils::benchmark(Func&& func, Args&&... args) {
    Timer timer;
    timer.start();
    func(std::forward<Args>(args)...);
    timer.stop();
    return timer.elapsedSeconds();
}

std::string TimerUtils::formatTime(double seconds) {
    if (seconds >= 3600) {
        auto hours = static_cast<int>(seconds / 3600);
        auto minutes = static_cast<int>((seconds - hours * 3600) / 60);
        auto secs = seconds - hours * 3600 - minutes * 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else if (seconds >= 60) {
        auto minutes = static_cast<int>(seconds / 60);
        auto secs = seconds - minutes * 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else if (seconds >= 1) {
        auto secs = static_cast<int>(seconds);
        auto ms = (seconds - secs) * 1000;
        return std::to_string(secs) + "." + std::to_string(static_cast<int>(ms)) + "s";
    } else if (seconds >= 0.001) {
        auto ms = seconds * 1000;
        return std::to_string(ms) + "ms";
    } else {
        auto us = seconds * 1000000;
        return std::to_string(us) + "µs";
    }
}

std::string TimerUtils::formatTime(const std::chrono::steady_clock::duration& duration) {
    return formatTime(std::chrono::duration_cast<std::chrono::duration<double>>(duration).count());
}

// TimerUtils::ScopedTimer implementation

TimerUtils::ScopedTimer::ScopedTimer(const std::string& label, LoggerHelper::LogLevel level)
    : label_(label), level_(level) {
    timer_.start();
}

TimerUtils::ScopedTimer::~ScopedTimer() {
    timer_.stop();
    double elapsed = timer_.elapsedSeconds();
    
    // Log the timing result
    std::ostringstream oss;
    oss << label_ << " took " << formatTime(elapsed);
    
    switch (level_) {
        case LoggerHelper::LogLevel::DEBUG:
            LoggerHelper::debug(oss.str());
            break;
        case LoggerHelper::LogLevel::INFO:
            LoggerHelper::info(oss.str());
            break;
        case LoggerHelper::LogLevel::WARNING:
            LoggerHelper::warning(oss.str());
            break;
        case LoggerHelper::LogLevel::ERROR:
            LoggerHelper::error(oss.str());
            break;
        case LoggerHelper::LogLevel::CRITICAL:
            LoggerHelper::critical(oss.str());
            break;
    }
}

double TimerUtils::ScopedTimer::elapsed() const {
    return timer_.elapsedSeconds();
}

} // namespace nlm
