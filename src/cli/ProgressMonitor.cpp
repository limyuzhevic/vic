// NLM (熙然) - Neural Learning Machine CLI Progress Monitor Implementation
//
// This file provides the implementation for CLI progress monitoring.

#include "cli/ProgressMonitor.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

namespace nlm {
namespace cli {

ProgressMonitor::ProgressMonitor() : 
    enabled(true),
    active(false),
    updateInterval(10),
    totalSteps(0),
    currentStep(0),
    currentOperation("") {
}

void ProgressMonitor::start(const std::string& operation, int totalSteps) {
    if (!enabled) return;
    
    active = true;
    currentOperation = operation;
    this->totalSteps = totalSteps;
    currentStep = 0;
    startTime = std::chrono::steady_clock::now();
    
    std::cout << "\n";
    std::cout << "Progress: " << currentOperation << " (0 / " << totalSteps << ")" << std::endl;
    displayProgressBar();
}

void ProgressMonitor::update(int currentStep, const std::string& message) {
    if (!enabled || !active) return;
    
    this->currentStep = currentStep;
    
    if (currentStep % updateInterval == 0 || currentStep == totalSteps) {
        clearProgressLine();
        std::cout << "\rProgress: " << currentOperation << " (" 
                  << currentStep << " / " << totalSteps << ")";
        
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        
        displayProgressBar();
        
        if (currentStep == totalSteps) {
            std::cout << std::endl;
            active = false;
        }
    }
}

void ProgressMonitor::stop() {
    if (active) {
        clearProgressLine();
        std::cout << std::endl;
        active = false;
    }
}

void ProgressMonitor::setUpdateInterval(int interval) {
    updateInterval = interval;
}

void ProgressMonitor::enable(bool enable) {
    this->enabled = enable;
}

std::chrono::seconds ProgressMonitor::getEstimatedTimeRemaining() const {
    if (currentStep == 0 || totalSteps == 0) {
        return std::chrono::seconds(0);
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    
    if (elapsed.count() == 0) {
        return std::chrono::seconds(0);
    }
    
    double progress = static_cast<double>(currentStep) / totalSteps;
    auto estimatedTotal = static_cast<int>(elapsed.count() / progress);
    auto remaining = estimatedTotal - elapsed.count();
    
    return std::chrono::seconds(std::max(0, remaining));
}

void ProgressMonitor::displayProgressBar() const {
    if (totalSteps == 0) return;
    
    double progress = static_cast<double>(currentStep) / totalSteps;
    int barWidth = 50;
    int pos = static_cast<int>(barWidth * progress);
    
    std::cout << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "]";
    
    // Display estimated time remaining
    auto remaining = getEstimatedTimeRemaining();
    std::cout << "  " << remaining.count() << "s remaining";
    
    std::cout << std::flush;
}

void ProgressMonitor::clearProgressLine() const {
    std::cout << "\r" << std::string(100, ' ') << "\r" << std::flush;
}

} // namespace cli
} // namespace nlm
