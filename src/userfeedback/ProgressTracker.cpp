#include "userfeedback/ProgressTracker.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace nlm;

void ProgressTracker::updateProgressDisplay() {
    std::string progressLine;
    
    if (options_.showProgressBar) {
        int barWidth = options_.barWidth;
        int filled = static_cast<int>(getProgressPercentage() * barWidth);
        
        progressLine = "[";
        for (int i = 0; i < filled; ++i) progressLine += "=";
        if (filled < barWidth) progressLine += ">";
        for (int i = filled + 1; i < barWidth; ++i) progressLine += " ";
        progressLine += "]";
    }
    
    std::string display = description_;
    if (!currentMessage_.empty() && currentMessage_ != "Starting...") {
        display += " - " + currentMessage_;
    }
    
    if (options_.showPercentage) {
        display += " (" + std::to_string(static_cast<int>(getProgressPercentage() * 100)) + "%)";
    }
    
    if (options_.showElapsedTime) {
        display += " [" + getElapsedTime() + "]";
    }
    
    if (options_.showEstimatedTime && getProgressPercentage() > 0.0f) {
        display += " [ETA: " + getEstimatedTimeRemaining() + "]";
    }
    
    if (options_.showSpeed && currentStep_ > 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
        if (elapsed > 0) {
            double speed = currentStep_ / elapsed;
            display += " [" + std::to_string(static_cast<int>(speed)) + " steps/s]";
        }
    }
    
    showProgress(display, getProgressPercentage(), true);
}

std::string ProgressTracker::formatTime(std::chrono::system_clock::duration duration) const {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    
    if (seconds < 60) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600) {
        int minutes = seconds / 60;
        int secs = seconds % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

} // namespace nlm