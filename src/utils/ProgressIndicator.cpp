#include "ProgressIndicator.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cmath>
#include <mutex>

namespace nlm {

struct ProgressIndicator::Impl {
    Options options;
    bool active = false;
    size_t current = 0;
    size_t total = 100;
    std::string label;
    std::string status;
    std::chrono::system_clock::time_point startTime;
    std::atomic<bool> running{false};
    std::mutex mutex;
    size_t spinnerIndex = 0;
    double lastPercent = -1.0;
};

ProgressIndicator::ProgressIndicator(const Options& options) : pImpl(std::make_unique<Impl>()) {
    pImpl->options = options;
    if (pImpl->options.spinnerFrames.empty()) {
        pImpl->options.spinnerFrames = {"-", "\\", "|", "/"};
    }
}

ProgressIndicator::~ProgressIndicator() {
    if (pImpl->active) {
        finish();
    }
}

void ProgressIndicator::start(const std::string& label) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->active = true;
    pImpl->label = label;
    pImpl->current = 0;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->spinnerIndex = 0;
    pImpl->lastPercent = -1.0;
    
    std::cout << "\r" << std::string(pImpl->options.width + pImpl->options.prefix.size() + 20, ' ') << "\r";
    std::cout.flush();
}

void ProgressIndicator::update(double progress, const std::string& status) {
    update(static_cast<size_t>(progress * pImpl->total), pImpl->total, status);
}

void ProgressIndicator::update(size_t current, size_t total, const std::string& status) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->current = current;
    pImpl->total = total;
    pImpl->status = status;
    
    if (!pImpl->active) return;
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->startTime);
    
    std::cout << "\r";
    
    if (!pImpl->options.prefix.empty()) {
        std::cout << pImpl->options.prefix;
    }
    
    double percent = (total > 0) ? (static_cast<double>(current) / total) * 100.0 : 0.0;
    
    switch (pImpl->options.style) {
        case Style::Bar:
            if (pImpl->options.showPercentage) {
                std::cout << "[" << std::string(static_cast<size_t>(pImpl->options.width * percent / 100.0), pImpl->options.fillChar) << std::string(pImpl->options.width - static_cast<size_t>(pImpl->options.width * percent / 100.0), pImpl->options.emptyChar) << "] ";
                std::cout << std::fixed << std::setprecision(1) << percent << "%";
            } else {
                std::cout << "[" << std::string(static_cast<size_t>(pImpl->options.width * percent / 100.0), pImpl->options.fillChar) << std::string(pImpl->options.width - static_cast<size_t>(pImpl->options.width * percent / 100.0), pImpl->options.emptyChar) << "]";
            }
            break;
            
        case Style::Spinner:
            std::cout << pImpl->options.spinnerFrames[pImpl->spinnerIndex % pImpl->options.spinnerFrames.size()] << " ";
            pImpl->spinnerIndex++;
            if (pImpl->options.showPercentage) {
                std::cout << std::fixed << std::setprecision(1) << percent << "%";
            }
            break;
            
        case Style::Dots:
            for (size_t i = 0; i < static_cast<size_t>(percent / 10); ++i) {
                std::cout << ".";
            }
            break;
            
        case Style::Percentage:
            std::cout << std::fixed << std::setprecision(1) << percent << "%";
            break;
            
        case Style::Custom:
            break;
    }
    
    if (!pImpl->status.empty()) {
        std::cout << " " << pImpl->status;
    }
    
    if (pImpl->options.showElapsed) {
        auto hours = elapsed.count() / 3600000;
        auto minutes = (elapsed.count() % 3600000) / 60000;
        auto seconds = (elapsed.count() % 60000) / 1000;
        auto ms = elapsed.count() % 1000;
        
        std::cout << " [";
        if (hours > 0) std::cout << hours << "h ";
        std::cout << minutes << "m " << seconds << "s " << ms << "ms";
        std::cout << "]";
    }
    
    if (pImpl->options.showRemaining && percent > 0 && percent < 100) {
        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::duration<double>(elapsed.count() * (100.0 / percent - 1.0))
        );
        auto remHours = remaining.count() / 3600000;
        auto remMinutes = (remaining.count() % 3600000) / 60000;
        auto remSeconds = (remaining.count() % 60000) / 1000;
        
        std::cout << " ETA:";
        if (remHours > 0) std::cout << " " << remHours << "h";
        std::cout << " " << remMinutes << "m " << remSeconds << "s";
    }
    
    if (!pImpl->label.empty()) {
        std::cout << " " << pImpl->label;
    }
    
    std::cout.flush();
}

void ProgressIndicator::setLabel(const std::string& label) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->label = label;
}

void ProgressIndicator::setStatus(const std::string& status) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->status = status;
}

void ProgressIndicator::increment() {
    update(pImpl->current + 1, pImpl->total);
}

void ProgressIndicator::finish() {
    finish("");
}

void ProgressIndicator::finish(const std::string& finalMessage) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->active) return;
    
    std::cout << "\r" << std::string(pImpl->options.width + pImpl->options.prefix.size() + 50, ' ');
    std::cout << "\r";
    
    if (!finalMessage.empty()) {
        std::cout << finalMessage;
    }
    
    std::cout << std::endl;
    std::cout.flush();
    
    pImpl->active = false;
}

bool ProgressIndicator::isActive() const {
    return pImpl->active;
}

void ProgressIndicator::setActive(bool active) {
    pImpl->active = active;
}

void ProgressIndicator::setTotal(size_t total) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->total = total;
}

size_t ProgressIndicator::getCurrent() const {
    return pImpl->current;
}

size_t ProgressIndicator::getTotal() const {
    return pImpl->total;
}

double ProgressIndicator::getProgress() const {
    if (pImpl->total == 0) return 0.0;
    return static_cast<double>(pImpl->current) / pImpl->total;
}

std::chrono::milliseconds ProgressIndicator::getElapsedTime() const {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->startTime);
    return elapsed;
}

std::chrono::milliseconds ProgressIndicator::getEstimatedRemainingTime() const {
    double progress = getProgress();
    if (progress <= 0.0 || progress >= 1.0) return std::chrono::milliseconds(0);
    
    auto elapsed = getElapsedTime();
    auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double>(elapsed.count() * (1.0 / progress - 1.0))
    );
    return remaining;
}

} // namespace nlm