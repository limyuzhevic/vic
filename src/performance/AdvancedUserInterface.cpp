// Advanced user interface implementation
#include "AdvancedUserInterface.hpp"

namespace nlm {

// ProgressReporter implementation
ProgressReporter::ProgressReporter() : lastReportTime_(0), totalSteps_(0) {}

void ProgressReporter::reportProgress(int currentStep, int totalSteps, const std::string& message, const ProgressOptions& options) {
    if (!options.showProgress) return;
    
    auto currentTime = std::chrono::system_clock::now();
    auto timeSinceLastReport = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastReportTime_).count();
    
    // Update every second or every 10% progress
    if (timeSinceLastReport >= 1 || (totalSteps_ > 0 && currentStep % std::max(1, totalSteps_ / 10) == 0)) {
        displayProgress(currentStep, totalSteps, message, options);
        lastReportTime_ = currentTime;
    }
}

void ProgressReporter::reportCompletion(const std::string& message, const ProgressOptions& options) {
    if (options.completionMessage.empty()) {
        std::cout << "Complete!" << std::endl;
    } else {
        std::cout << options.completionMessage << std::endl;
    }
    
    if (options.showCompletionPercentage) {
        std::cout << "100% complete" << std::endl;
    }
}

void ProgressReporter::setTotalSteps(int totalSteps) {
    totalSteps_ = totalSteps;
}

void ProgressReporter::displayProgress(int currentStep, int totalSteps, const std::string& message, const ProgressOptions& options) {
    std::cout << "\r";
    
    if (options.showCompletionPercentage) {
        float percentage = (totalSteps > 0) ? (static_cast<float>(currentStep) / totalSteps) * 100.0f : 0.0f;
        std::cout << std::fixed << std::setprecision(1) << percentage << "%";
    }
    
    std::cout << " [";
    int barWidth = 30;
    int progressWidth = (totalSteps > 0) ? (currentStep * barWidth) / totalSteps : 0;
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < progressWidth) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }
    
    std::cout << "]";
    
    if (currentStep > 0 && totalSteps > 0) {
        std::cout << " " << currentStep << "/" << totalSteps;
        
        if (options.showEstimatedTime && totalSteps_ > 0) {
            double estimatedTotalSeconds = (durationEstimate_.count() > 0) ? 
                (durationEstimate_.count() * static_cast<double>(totalSteps)) / currentStep : 0.0;
            double remainingSeconds = estimatedTotalSeconds - durationEstimate_.count();
            
            std::chrono::seconds remaining(remainingSeconds);
            if (remaining.count() > 60) {
                int minutes = remaining.count() / 60;
                int seconds = remaining.count() % 60;
                std::cout << " (~" << minutes << "m " << seconds << "s remaining)";
            } else {
                std::cout << " (~" << remaining.count() << "s remaining)";
            }
        }
    }
    
    if (!message.empty()) {
        std::cout << " - " << message;
    }
    
    std::cout << std::flush;
    
    // Start timing if this is the first report
    if (durationEstimate_.count() == 0 && currentStep > 0) {
        durationEstimate_ = std::chrono::steady_clock::now() - startTime_;
    }
    
    startTime_ = std::chrono::steady_clock::now();
}

} // namespace nlm