// Implementation of ProgressReporter for NLM

#include "ProgressReporter.hpp"
#include <thread>
#include <algorithm>
#include <sstream>
#include <cmath>

namespace nlm {

struct ProgressReporter::Impl {
    ProgressBarStyle style;
    bool showPercentage;
    bool showTimeRemaining;
    bool useColors;
    std::chrono::milliseconds updateInterval;
    
    // Current progress state
    std::atomic<double> currentProgress{0.0};
    std::atomic<int> currentStep{0};
    std::atomic<int> totalSteps{0};
    std::string currentOperation;
    std::string currentMessage;
    std::chrono::system_clock::time_point startTime;
    bool isActive{false};
    bool monitoring{false};
    
    // Callbacks
    ProgressCallback progressCallback;
    
    // Monitoring state
    std::string monitoringOperation;
    std::chrono::system_clock::time_point monitoringStartTime;
    std::chrono::seconds monitoringEstimatedDuration{0};
    
    // Experiment progress
    std::string experimentName;
    int currentPhase{0};
    int totalPhases{0};
    
    // Simulation progress
    std::string simulationName;
    SimulationStep currentSimStep{0};
    SimulationStep totalSimSteps{0};
    
    // File operation progress
    std::string fileOperationName;
    std::string currentFilename;
    size_t currentBytesTransferred{0};
    size_t totalBytes{0};
    
    // Initialization progress
    std::string initComponentName;
    std::vector<std::pair<std::string, double>> initSteps;
    
    // Thread for periodic updates
    std::thread updateThread;
    std::atomic<bool> updateThreadRunning{false};
    
    // Progress history for export
    std::vector<std::pair<double, std::chrono::system_clock::time_point>> progressHistory;
};

ProgressReporter::ProgressReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->style = ProgressBarStyle::Fancy;
    pImpl->showPercentage = true;
    pImpl->showTimeRemaining = true;
    pImpl->useColors = true;
    pImpl->updateInterval = std::chrono::milliseconds(100);
}

ProgressReporter::~ProgressReporter() {
    stopProgress();
    stopMonitoring();
}

void ProgressReporter::setProgressBarStyle(ProgressBarStyle style) {
    pImpl->style = style;
}

void ProgressReporter::setShowPercentage(bool show) {
    pImpl->showPercentage = show;
}

void ProgressReporter::setShowTimeRemaining(bool show) {
    pImpl->showTimeRemaining = show;
}

void ProgressReporter::setUpdateInterval(std::chrono::milliseconds interval) {
    pImpl->updateInterval = interval;
}

void ProgressReporter::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

void ProgressReporter::startProgress(const std::string& operationName, int totalSteps) {
    std::lock_guard<std::mutex> lock(std::mutex{});
    
    pImpl->currentOperation = operationName;
    pImpl->totalSteps = totalSteps;
    pImpl->currentProgress = 0.0;
    pImpl->currentStep = 0;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->isActive = true;
    pImpl->currentMessage.clear();
    
    // Start periodic update thread if interval is set
    if (pImpl->updateInterval.count() > 0) {
        pImpl->updateThreadRunning = true;
        pImpl->updateThread = std::thread([this]() {
            while (pImpl->updateThreadRunning && pImpl->isActive) {
                std::this_thread::sleep_for(pImpl->updateInterval);
                if (pImpl->progressCallback) {
                    pImpl->progressCallback(pImpl->currentProgress, pImpl->currentOperation, pImpl->currentMessage);
                }
            }
        });
    }
    
    // Initial display
    displayProgressBar(pImpl->currentProgress, pImpl->currentOperation, pImpl->currentMessage);
}

void ProgressReporter::updateProgress(int currentStep, const std::string& message) {
    if (!pImpl->isActive) return;
    
    std::lock_guard<std::mutex> lock(std::mutex{});
    
    pImpl->currentStep = currentStep;
    pImpl->currentProgress = (totalSteps > 0) ? (static_cast<double>(currentStep) / totalSteps) * 100.0 : 0.0;
    pImpl->currentMessage = message;
    pImpl->progressHistory.emplace_back(pImpl->currentProgress, std::chrono::system_clock::now());
    
    displayProgressBar(pImpl->currentProgress, pImpl->currentOperation, pImpl->currentMessage);
}

void ProgressReporter::updateProgress(double percentage, const std::string& message) {
    if (!pImpl->isActive) return;
    
    std::lock_guard<std::mutex> lock(std::mutex{});
    
    pImpl->currentProgress = std::min(100.0, std::max(0.0, percentage));
    pImpl->currentMessage = message;
    pImpl->progressHistory.emplace_back(pImpl->currentProgress, std::chrono::system_clock::now());
    
    displayProgressBar(pImpl->currentProgress, pImpl->currentOperation, pImpl->currentMessage);
}

void ProgressReporter::stopProgress() {
    if (!pImpl->isActive) return;
    
    pImpl->isActive = false;
    
    // Wait for update thread to finish
    if (pImpl->updateThreadRunning) {
        pImpl->updateThreadRunning = false;
        if (pImpl->updateThread.joinable()) {
            pImpl->updateThread.join();
        }
    }
    
    // Display completion message
    std::cout << "\r" << std::string(80, ' ') << "\r"; // Clear line
    std::cout << "✓ Completed: " << pImpl->currentOperation << " (" 
              << formatProgressPercentage(pImpl->currentProgress) << ")" << std::endl;
}

void ProgressReporter::startMonitoring(const std::string& operationName, const std::chrono::seconds& estimatedDuration) {
    pImpl->monitoringOperation = operationName;
    pImpl->monitoringStartTime = std::chrono::system_clock::now();
    pImpl->monitoringEstimatedDuration = estimatedDuration;
    pImpl->monitoring = true;
    pImpl->currentOperation = operationName;
}

void ProgressReporter::updateMonitoring(double progress, const std::string& message) {
    if (!pImpl->monitoring) return;
    
    std::lock_guard<std::mutex> lock(std::mutex{});
    
    pImpl->currentProgress = progress;
    pImpl->currentMessage = message;
    
    // Display monitoring update
    std::cout << "MONITOR: " << pImpl->monitoringOperation << " - " 
              << formatProgressPercentage(progress) << " - " << message << std::endl;
}

void ProgressReporter::stopMonitoring() {
    if (!pImpl->monitoring) return;
    
    pImpl->monitoring = false;
    std::cout << "MONITOR STOP: " << pImpl->monitoringOperation << " completed" << std::endl;
}

void ProgressReporter::startExperimentProgress(const std::string& experimentName, int totalPhases) {
    pImpl->experimentName = experimentName;
    pImpl->totalPhases = totalPhases;
    pImpl->currentPhase = 0;
    
    std::cout << "EXPERIMENT: Starting experiment: " << experimentName << std::endl;
    std::cout << "EXPERIMENT: Total phases: " << totalPhases << std::endl;
}

void ProgressReporter::updateExperimentPhase(int phaseNumber, const std::string& phaseName, double progress) {
    std::lock_guard<std::mutex> lock(std::mutex{});
    
    pImpl->currentPhase = phaseNumber;
    std::cout << "PHASE " << phaseNumber << "/" << totalPhases << ": " << phaseName 
              << " - " << formatProgressPercentage(progress) << std::endl;
}

void ProgressReporter::updateExperimentStep(int stepNumber, const std::string& stepDescription) {
    std::cout << "  STEP " << stepNumber << ": " << stepDescription << std::endl;
}

void ProgressReporter::stopExperimentProgress() {
    std::cout << "EXPERIMENT: Experiment completed: " << pImpl->experimentName << std::endl;
}

void ProgressReporter::startSimulationProgress(const std::string& simulationName, SimulationStep totalSteps) {
    pImpl->simulationName = simulationName;
    pImpl->totalSimSteps = totalSteps;
    pImpl->currentSimStep = 0;
    
    std::cout << "SIMULATION: Starting simulation: " << simulationName 
              << " (total steps: " << totalSteps << ")" << std::endl;
}

void ProgressReporter::updateSimulationStep(SimulationStep currentStep, const std::string& statusMessage) {
    std::lock_guard<std::mutex> lock(std::mutex{});
    
    pImpl->currentSimStep = currentStep;
    double progress = (totalSimSteps > 0) ? static_cast<double>(currentStep) / totalSimSteps * 100.0 : 0.0;
    
    std::cout << "STEP " << currentStep << "/" << totalSimSteps << " (" 
              << formatProgressPercentage(progress) << ")";
    if (!statusMessage.empty()) {
        std::cout << " - " << statusMessage;
    }
    std::cout << "\r" << std::flush;
}

void ProgressReporter::stopSimulationProgress() {
    std::cout << std::endl << "SIMULATION: Simulation completed: " << pImpl->simulationName << std::endl;
}

void ProgressReporter::startFileOperationProgress(const std::string& operation, const std::string& filename, size_t totalBytes) {
    pImpl->fileOperationName = operation;
    pImpl->currentFilename = filename;
    pImpl->totalBytes = totalBytes;
    pImpl->currentBytesTransferred = 0;
    
    std::cout << "FILE: " << operation << " - " << filename;
    if (totalBytes > 0) {
        std::cout << " (0 bytes / " << totalBytes << " bytes)";
    }
    std::cout << std::endl;
}

void ProgressReporter::updateFileOperationProgress(size_t bytesTransferred, const std::string& filename) {
    std::lock_guard<std::mutex> lock(std::mutex{});
    
    pImpl->currentBytesTransferred = bytesTransferred;
    
    double progress = (totalBytes > 0) ? static_cast<double>(bytesTransferred) / totalBytes * 100.0 : 0.0;
    
    std::cout << "\rFILE: " << pImpl->fileOperationName << " - " << filename 
              << " - " << formatProgressPercentage(progress) << " - " 
              << formatElapsedTime(calculateElapsedTime(pImpl->startTime));
    if (pImpl->showTimeRemaining && totalBytes > 0) {
        auto remaining = calculateEstimatedTimeRemaining(progress, pImpl->startTime);
        std::cout << " - ETA: " << formatTimeRemaining(remaining);
    }
    std::cout << std::flush;
}

void ProgressReporter::stopFileOperationProgress() {
    std::cout << std::endl << "FILE: Operation completed: " << pImpl->fileOperationName 
              << " - " << pImpl->currentFilename << std::endl;
}

void ProgressReporter::startInitializationProgress(const std::string& componentName) {
    pImpl->initComponentName = componentName;
    std::cout << "INIT: Starting initialization of component: " << componentName << std::endl;
}

void ProgressReporter::updateInitializationStep(const std::string& stepName, double progress) {
    std::lock_guard<std::mutex> lock(std::mutex{});
    
    // Store step progress
    bool found = false;
    for (auto& step : pImpl->initSteps) {
        if (step.first == stepName) {
            step.second = progress;
            found = true;
            break;
        }
    }
    if (!found) {
        pImpl->initSteps.emplace_back(stepName, progress);
    }
    
    std::cout << "INIT: " << stepName << " - " << formatProgressPercentage(progress) << std::endl;
}

void ProgressReporter::stopInitializationProgress() {
    std::cout << "INIT: Initialization completed for component: " << pImpl->initComponentName << std::endl;
}

void ProgressReporter::setProgressCallback(const ProgressCallback& callback) {
    pImpl->progressCallback = callback;
}

double ProgressReporter::getCurrentProgress() const {
    return pImpl->currentProgress;
}

std::string ProgressReporter::getCurrentOperation() const {
    return pImpl->currentOperation;
}

std::chrono::system_clock::time_point ProgressReporter::getStartTime() const {
    return pImpl->startTime;
}

std::chrono::milliseconds ProgressReporter::getElapsedTime() const {
    return calculateElapsedTime(pImpl->startTime);
}

std::chrono::milliseconds ProgressReporter::getEstimatedTimeRemaining() const {
    return calculateEstimatedTimeRemaining(pImpl->currentProgress, pImpl->startTime);
}

std::string ProgressReporter::generateProgressReport() const {
    std::stringstream report;
    report << "Progress Report" << std::endl;
    report << std::string(50, '=') << std::endl;
    report << "Operation: " << pImpl->currentOperation << std::endl;
    report << "Progress: " << formatProgressPercentage(pImpl->currentProgress) << std::endl;
    report << "Elapsed Time: " << formatElapsedTime(getElapsedTime()) << std::endl;
    report << "Estimated Time Remaining: " << formatTimeRemaining(getEstimatedTimeRemaining()) << std::endl;
    report << "Current Step: " << pImpl->currentStep << "/" << pImpl->totalSteps << std::endl;
    report << "Message: " << pImpl->currentMessage << std::endl;
    
    return report.str();
}

bool ProgressReporter::exportProgressData(const std::string& filepath, const std::string& format) const {
    // Implementation would serialize progress data based on format
    // For now, just write a simple CSV
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    file << "Timestamp,Progress,Operation,Message" << std::endl;
    for (const auto& entry : pImpl->progressHistory) {
        auto time = std::chrono::system_clock::to_time_t(entry.second);
        file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
             << "," << entry.first 
             << "," << pImpl->currentOperation 
             << "," << pImpl->currentMessage << std::endl;
    }
    
    return true;
}

void ProgressReporter::displayProgressBar(double progress, const std::string& operation, const std::string& message) const {
    switch (pImpl->style) {
        case ProgressBarStyle::Fancy:
            displayFancyProgressBar(progress, operation, message);
            break;
        case ProgressBarStyle::ProgressLine:
            displayProgressLine(progress, operation, message);
            break;
        case ProgressBarStyle::Simple:
        default:
            displaySimpleProgress(progress, operation, message);
            break;
    }
}

void ProgressReporter::displayFancyProgressBar(double progress, const std::string& operation, const std::string& message) const {
    const int barWidth = 50;
    int pos = static_cast<int>(barWidth * progress / 100.0);
    
    std::cout << "\r[";
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    
    std::cout << "] " << formatProgressPercentage(progress) << "%";
    
    if (pImpl->showTimeRemaining) {
        std::cout << " (" << formatTimeRemaining(calculateEstimatedTimeRemaining(progress, pImpl->startTime)) << ")";
    }
    
    if (!operation.empty()) {
        std::cout << " - " << operation;
    }
    
    if (!message.empty()) {
        std::cout << " - " << message;
    }
    
    std::cout << std::flush;
}

void ProgressReporter::displayProgressLine(double progress, const std::string& operation, const std::string& message) const {
    std::cout << "\r";
    std::cout << "Progress: " << formatProgressPercentage(progress) << " ";
    
    if (pImpl->useColors) {
        std::cout << "\033[32m"; // Green
    }
    
    int barWidth = 30;
    int pos = static_cast<int>(barWidth * progress / 100.0);
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "#";
        else std::cout << "-";
    }
    
    if (pImpl->useColors) {
        std::cout << "\033[0m"; // Reset
    }
    
    std::cout << " " << operation;
    if (!message.empty()) {
        std::cout << " - " << message;
    }
    
    std::cout << std::flush;
}

void ProgressReporter::displaySimpleProgress(double progress, const std::string& operation, const std::string& message) const {
    std::cout << "\rProgress: " << formatProgressPercentage(progress) << "% - " << operation;
    if (!message.empty()) {
        std::cout << " - " << message;
    }
    std::cout << std::flush;
}

std::chrono::milliseconds ProgressReporter::calculateElapsedTime(std::chrono::system_clock::time_point startTime) const {
    auto elapsed = std::chrono::system_clock::now() - startTime;
    return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
}

std::chrono::milliseconds ProgressReporter::calculateEstimatedTimeRemaining(double progress, std::chrono::system_clock::time_point startTime) const {
    if (progress <= 0.0 || progress >= 100.0) {
        return std::chrono::milliseconds(0);
    }
    
    auto elapsed = calculateElapsedTime(startTime);
    double remainingRatio = (100.0 - progress) / progress;
    auto estimatedRemaining = std::chrono::milliseconds(static_cast<long>(elapsed.count() * remainingRatio));
    
    return estimatedRemaining;
}

std::string ProgressReporter::formatProgressPercentage(double progress) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << progress << "%";
    return ss.str();
}

std::string ProgressReporter::formatElapsedTime(std::chrono::milliseconds ms) const {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(hours);
    
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(minutes);
    
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(seconds);
    
    std::stringstream ss;
    ss << hours.count() << "h " << minutes.count() << "m " << seconds.count() << "s";
    return ss.str();
}

std::string ProgressReporter::formatTimeRemaining(std::chrono::milliseconds ms) const {
    if (ms.count() <= 0) {
        return "Unknown";
    }
    
    auto hours = std::chrono::duration_cast<std::chrono::hours>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(hours);
    
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(minutes);
    
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(ms);
    
    std::stringstream ss;
    if (hours.count() > 0) {
        ss << hours.count() << "h " << minutes.count() << "m";
    } else if (minutes.count() > 0) {
        ss << minutes.count() << "m " << seconds.count() << "s";
    } else {
        ss << seconds.count() << "s";
    }
    
    return ss.str();
}

} // namespace nlm
