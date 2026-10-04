#include "ProgressReporter.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <thread>

namespace nlm {

struct ProgressReporter::Impl {
    // Configuration
    ProgressConfig config;
    
    // Current state
    std::string currentOperationName;
    double currentProgress{0.0};
    size_t currentStep{0};
    size_t totalSteps{0};
    double totalWork{100.0};
    ProgressMetrics currentMetrics;
    ETAInfo etaInfo;
    bool isPaused{false};
    bool isActive{false};
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastUpdateTime;
    
    // Callbacks and milestones
    std::vector<ProgressCallback> callbacks;
    std::vector<ProgressMilestone> milestones;
    std::vector<ProgressMilestone> completedMilestones;
    
    // Progress history
    std::vector<std::pair<double, ProgressMetrics>> progressHistory;
    
    // Output stream
    std::ofstream logFileStream;
    
    // Constructor
    Impl() : currentProgress(0.0), isPaused(false), isActive(false) {}
    
    // Destructor
    ~Impl() {
        if (logFileStream.is_open()) {
            logFileStream.close();
        }
    }
};

ProgressReporter::ProgressReporter(const ProgressConfig& config) : pImpl(std::make_unique<Impl>()) {
    configure(config);
    
    // Open log file if specified
    if (!config.progressLogFile.empty()) {
        pImpl->logFileStream.open(config.progressLogFile, std::ios::app);
    }
}

ProgressReporter::~ProgressReporter() = default;

void ProgressReporter::configure(const ProgressConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->config = config;
}

void ProgressReporter::startProgress(const std::string& operationName,
                                    size_t totalSteps,
                                    double totalWork) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    pImpl->currentOperationName = operationName;
    pImpl->totalSteps = totalSteps;
    pImpl->totalWork = totalWork;
    pImpl->currentProgress = 0.0;
    pImpl->currentStep = 0;
    pImpl->currentMetrics = ProgressMetrics();
    pImpl->completedMilestones.clear();
    pImpl->isPaused = false;
    pImpl->isActive = true;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
    pImpl->progressHistory.clear();
    
    // Clear ETA info
    pImpl->etaInfo = ETAInfo();
    
    // Output starting message
    if (pImpl->config.enableLogging) {
        logProgress("Starting progress for operation: " + operationName, LogLevel::Info);
    }
}

void ProgressReporter::updateProgress(double progressPercentage,
                                     const std::string& message,
                                     const ProgressMetrics& metrics) {
    if (!pImpl->isActive || pImpl->isPaused) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Clamp progress to valid range
    double oldProgress = pImpl->currentProgress;
    pImpl->currentProgress = std::max(0.0, std::min(100.0, progressPercentage));
    pImpl->currentMetrics = metrics;
    pImpl->lastUpdateTime = std::chrono::system_clock::now();
    
    // Update progress history
    updateProgressHistory(pImpl->currentProgress, metrics);
    
    // Check milestones
    checkMilestones(pImpl->currentProgress);
    
    // Update ETA
    updateETA(pImpl->currentProgress);
    
    // Output progress
    outputProgress(pImpl->currentProgress, message, metrics);
    
    // Invoke callbacks
    invokeCallbacks(pImpl->currentProgress, message, metrics);
}

void ProgressReporter::updateProgress(size_t currentStep,
                                     size_t totalSteps,
                                     const std::string& message,
                                     const ProgressMetrics& metrics) {
    if (totalSteps == 0) {
        updateProgress(100.0, message, metrics);
        return;
    }
    
    double progressPercentage = (static_cast<double>(currentStep) / totalSteps) * 100.0;
    updateProgress(progressPercentage, message, metrics);
}

void ProgressReporter::setOperation(const std::string& operationName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->currentOperationName = operationName;
}

void ProgressReporter::setTotalWork(size_t totalSteps, double totalWork) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->totalSteps = totalSteps;
    pImpl->totalWork = totalWork;
}

void ProgressReporter::addProgressCallback(const ProgressCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->callbacks.push_back(callback);
}

void ProgressReporter::removeProgressCallback(const ProgressCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->callbacks.erase(std::remove(pImpl->callbacks.begin(), pImpl->callbacks.end(), callback),
                          pImpl->callbacks.end());
}

void ProgressReporter::addMilestone(const ProgressMilestone& milestone) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->milestones.push_back(milestone);
}

void ProgressReporter::removeMilestone(const ProgressMilestone& milestone) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->milestones.erase(std::remove(pImpl->milestones.begin(), pImpl->milestones.end(), milestone),
                           pImpl->milestones.end());
}

void ProgressReporter::pauseProgress() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    if (pImpl->isActive && !pImpl->isPaused) {
        pImpl->isPaused = true;
        if (pImpl->config.enableLogging) {
            logProgress("Progress reporting paused", LogLevel::Info);
        }
    }
}

void ProgressReporter::resumeProgress() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    if (pImpl->isActive && pImpl->isPaused) {
        pImpl->isPaused = false;
        pImpl->lastUpdateTime = std::chrono::system_clock::now();
        if (pImpl->config.enableLogging) {
            logProgress("Progress reporting resumed", LogLevel::Info);
        }
    }
}

void ProgressReporter::finishProgress(const std::string& finalMessage) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (!pImpl->isActive) {
        return;
    }
    
    bool completed = std::abs(pImpl->currentProgress - 100.0) < 1.0;
    pImpl->currentProgress = 100.0;
    pImpl->isActive = false;
    
    // Log completion
    std::string message = finalMessage.empty() ? 
        (completed ? "Operation completed successfully" : "Operation finished") : 
        finalMessage;
    
    if (pImpl->config.enableLogging) {
        logProgress(message, completed ? LogLevel::Info : LogLevel::Warning);
    }
    
    outputProgress(100.0, message, pImpl->currentMetrics);
    invokeCallbacks(100.0, message, pImpl->currentMetrics);
}

std::string ProgressReporter::formatProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    std::stringstream ss;
    
    if (pImpl->currentOperationName.empty()) {
        ss << "Progress: " << std::fixed << std::setprecision(1) 
           << pImpl->currentProgress << "%";
        return ss.str();
    }
    
    ss << pImpl->currentOperationName << ": "
       << std::fixed << std::setprecision(1) << pImpl->currentProgress << "%";
    
    if (!pImpl->config.enableProgressBars) {
        return ss.str();
    }
    
    // Add progress bar
    switch (pImpl->config.progressBarStyle) {
        case ProgressConfig::ProgressBarStyle::Simple:
            ss << " ["; 
            for (int i = 0; i < 50; ++i) {
                if (i < static_cast<int>(pImpl->currentProgress / 2.0)) {
                    ss << "=";
                } else if (i == static_cast<int>(pImpl->currentProgress / 2.0)) {
                    ss << ">";
                } else {
                    ss << " ";
                }
            }
            ss << "]";
            break;
            
        case ProgressConfig::ProgressBarStyle::Unicode:
            ss << " ["; 
            for (int i = 0; i < 50; ++i) {
                double pos = static_cast<double>(i) / 50.0;
                if (pos < pImpl->currentProgress / 100.0) {
                    ss << "█";
                } else if (pos == pImpl->currentProgress / 100.0) {
                    ss << "▓";
                } else {
                    ss << "░";
                }
            }
            ss << "]";
            break;
            
        case ProgressConfig::ProgressBarStyle::ASCII:
            ss << " ["; 
            for (int i = 0; i < 50; ++i) {
                double pos = static_cast<double>(i) / 50.0;
                if (pos < pImpl->currentProgress / 100.0) {
                    ss << "#";
                } else if (pos == pImpl->currentProgress / 100.0) {
                    ss << ">";
                } else {
                    ss << "-";
                }
            }
            ss << "]";
            break;
            
        case ProgressConfig::ProgressBarStyle::Colored:
            // Use ANSI color codes (simplified)
            ss << " [\033[32m"; 
            for (int i = 0; i < 50; ++i) {
                double pos = static_cast<double>(i) / 50.0;
                if (pos < pImpl->currentProgress / 100.0) {
                    ss << "█";
                } else if (pos == pImpl->currentProgress / 100.0) {
                    ss << "▓";
                } else {
                    ss << "░";
                }
            }
            ss << "\033[0m]";
            break;
            
        default:
            // Simple style
            ss << " ["; 
            for (int i = 0; i < 50; ++i) {
                if (i < static_cast<int>(pImpl->currentProgress / 2.0)) {
                    ss << "=";
                } else if (i == static_cast<int>(pImpl->currentProgress / 2.0)) {
                    ss << ">";
                } else {
                    ss << " ";
                }
            }
            ss << "]";
            break;
    }
    
    // Add ETA if available
    if (pImpl->etaInfo.isEstimated && pImpl->config.enableETACalculation) {
        ss << " " << formatETA(pImpl->etaInfo);
    }
    
    // Add completion percentage
    ss << " (" << pImpl->currentProgress << "%)";
    
    return ss.str();
}

std::string ProgressReporter::formatMetrics(const ProgressMetrics& metrics) const {
    std::stringstream ss;
    
    switch (pImpl->config.statusVerbosity) {
        case ProgressConfig::StatusVerbosity::Quiet:
            return "";
            
        case ProgressConfig::StatusVerbosity::Summary:
            ss << "[Spike:" << metrics.spikeCount 
               << ", Firing:" << std::fixed << std::setprecision(2) << metrics.firingRate
               << "%, Neurons:" << metrics.neuronCount
               << ", Synapses:" << metrics.synapseCount << "]";
            break;
            
        case ProgressConfig::StatusVerbosity::Detailed:
            ss << "[Spike:" << metrics.spikeCount 
               << ", Firing:" << std::fixed << std::setprecision(2) << metrics.firingRate
               << "%, Neurons:" << metrics.neuronCount
               << ", Synapses:" << metrics.synapseCount
               << ", Act:" << metrics.firingNeuronCount
               << ", Ratio:" << std::fixed << std::setprecision(3) << metrics.excitationInhibitionRatio
               << ", Memory:" << metrics.memoryUsage << "B]";
            break;
            
        case ProgressConfig::StatusVerbosity::Debug:
            ss << "[Spike:" << metrics.spikeCount 
               << ", Firing:" << std::fixed << std::setprecision(2) << metrics.firingRate
               << "%, Neurons:" << metrics.neuronCount
               << ", Synapses:" << metrics.synapseCount
               << ", Act:" << metrics.firingNeuronCount
               << ", Ratio:" << std::fixed << std::setprecision(3) << metrics.excitationInhibitionRatio
               << ", Memory:" << metrics.memoryUsage << "B, CPU:" << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%, ";
            
            // Add custom metrics
            for (const auto& pair : metrics.customMetrics) {
                ss << pair.first << ":" << std::fixed << std::setprecision(2) << pair.second << " ";
            }
            break;
    }
    
    return ss.str();
}

std::string ProgressReporter::formatETA(const ETAInfo& eta) const {
    if (!eta.isEstimated) {
        return "ETA: ?";
    }
    
    std::stringstream ss;
    ss << "ETA:";
    
    if (eta.etaPercentage > 0.0) {
        ss << std::fixed << std::setprecision(1) << eta.etaPercentage << "%";
    }
    
    if (eta.etaSeconds > 0.0) {
        int hours = static_cast<int>(eta.etaSeconds / 3600);
        int minutes = static_cast<int>((eta.etaSeconds - hours * 3600) / 60);
        double seconds = eta.etaSeconds - hours * 3600 - minutes * 60;
        
        if (hours > 0) {
            ss << hours << "h";
        }
        ss << minutes << "m" << std::setprecision(1) << seconds << "s";
    }
    
    return ss.str();
}

bool ProgressReporter::exportStatus(const std::string& filepath, const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json" || format == "JSON") {
        // Export as JSON (simplified)
        file << "{\n";
        file << "  \"operation\": \"" << pImpl->currentOperationName << "\",\n";
        file << "  \"progress\": " << std::fixed << std::setprecision(2) << pImpl->currentProgress << ",\n";
        file << "  \"step\": " << pImpl->currentStep << "/" << pImpl->totalSteps << ",\n";
        file << "  \"metrics\": {\n";
        file << "    \"spikeCount\": " << pImpl->currentMetrics.spikeCount << ",\n";
        file << "    \"firingRate\": " << std::fixed << std::setprecision(4) << pImpl->currentMetrics.firingRate << ",\n";
        file << "    \"neuronCount\": " << pImpl->currentMetrics.neuronCount << ",\n";
        file << "    \"synapseCount\": " << pImpl->currentMetrics.synapseCount << ",\n";
        file << "    \"firingNeuronCount\": " << pImpl->currentMetrics.firingNeuronCount << "\n";
        file << "  }\n";
        file << "}\n";
    } else {
        // Export as text
        file << "Progress Report\n";
        file << "==============\n";
        file << "Operation: " << pImpl->currentOperationName << "\n";
        file << "Progress: " << std::fixed << std::setprecision(2) << pImpl->currentProgress << "%\n";
        file << "Step: " << pImpl->currentStep << "/" << pImpl->totalSteps << "\n";
        file << "Metrics:\n";
        file << "  Spike Count: " << pImpl->currentMetrics.spikeCount << "\n";
        file << "  Firing Rate: " << std::fixed << std::setprecision(4) << pImpl->currentMetrics.firingRate << "%\n";
        file << "  Neuron Count: " << pImpl->currentMetrics.neuronCount << "\n";
        file << "  Synapse Count: " << pImpl->currentMetrics.synapseCount << "\n";
        file << "  Firing Neuron Count: " << pImpl->currentMetrics.firingNeuronCount << "\n";
        file << "  Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(4) << pImpl->currentMetrics.excitationInhibitionRatio << "\n";
        file << "  Memory Usage: " << pImpl->currentMetrics.memoryUsage << " bytes\n";
        file << "  CPU Usage: " << std::fixed << std::setprecision(1) << pImpl->currentMetrics.cpuUsage << "%\n";
        
        if (!pImpl->currentMetrics.customMetrics.empty()) {
            file << "  Custom Metrics:\n";
            for (const auto& pair : pImpl->currentMetrics.customMetrics) {
                file << "    " << pair.first << ": " << std::fixed << std::setprecision(4) << pair.second << "\n";
            }
        }
        
        file << "\nETA: " << formatETA(pImpl->etaInfo) << "\n";
        file << "Elapsed Time: " << getElapsedTime() << " seconds\n";
    }
    
    file.close();
    return true;
}

bool ProgressReporter::logProgress(const std::string& message, LogLevel level) {
    if (!pImpl->config.enableLogging) {
        return false;
    }
    
    // Log to global logger
    Logger::getGlobal().log(level, message);
    
    // Log to file if specified
    if (pImpl->logFileStream.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
           << " [" << pImpl->currentOperationName << "] "
           << message << "\n";
        
        pImpl->logFileStream << ss.str();
        pImpl->logFileStream.flush();
    }
    
    return true;
}

void ProgressReporter::updateETA(double progressPercentage) {
    if (!pImpl->config.enableETACalculation) {
        return;
    }
    
    double elapsedSeconds = getElapsedTime();
    
    if (elapsedSeconds <= 0.0 || progressPercentage <= 0.0) {
        return;
    }
    
    double etaPercentage = 100.0 - progressPercentage;
    double etaSeconds = (etaPercentage / progressPercentage) * elapsedSeconds;
    
    pImpl->etaInfo.etaPercentage = etaPercentage;
    pImpl->etaInfo.etaSeconds = etaSeconds;
    pImpl->etaInfo.isEstimated = true;
    pImpl->etaInfo.lastUpdate = std::chrono::system_clock::now();
}

void ProgressReporter::checkMilestones(double progressPercentage) {
    if (!pImpl->config.enableMilestones) {
        return;
    }
    
    for (auto& milestone : pImpl->milestones) {
        if (!milestone.reached && progressPercentage >= milestone.percentage) {
            milestone.reached = true;
            milestone.reachedTime = std::chrono::system_clock::now();
            pImpl->completedMilestones.push_back(milestone);
            
            // Log milestone
            std::stringstream ss;
            ss << "Milestone reached: " << milestone.name << " (" << std::fixed << std::setprecision(1)
               << milestone.percentage << "% ) - " << milestone.description;
            
            if (pImpl->config.enableLogging) {
                logProgress(ss.str(), LogLevel::Info);
            }
        }
    }
}

void ProgressReporter::invokeCallbacks(double progressPercentage,
                                       const std::string& message,
                                       const ProgressMetrics& metrics) {
    if (!pImpl->config.enableCallbacks) {
        return;
    }
    
    auto timestamp = std::chrono::system_clock::now();
    
    // Prepare metrics map for callbacks
    std::unordered_map<std::string, double> metricsMap;
    metricsMap["progress"] = progressPercentage;
    metricsMap["spike_count"] = static_cast<double>(metrics.spikeCount);
    metricsMap["firing_rate"] = metrics.firingRate;
    metricsMap["neuron_count"] = static_cast<double>(metrics.neuronCount);
    metricsMap["synapse_count"] = static_cast<double>(metrics.synapseCount);
    metricsMap["memory_usage"] = static_cast<double>(metrics.memoryUsage);
    metricsMap["cpu_usage"] = metrics.cpuUsage;
    
    for (const auto& callback : pImpl->callbacks) {
        try {
            callback(progressPercentage, message, metricsMap, timestamp);
        } catch (const std::exception& e) {
            // Catch and log callback exceptions to avoid crashing
            logProgress(std::string("Callback exception: ") + e.what(), LogLevel::Error);
        }
    }
}

void ProgressReporter::updateProgressHistory(double progressPercentage,
                                             const ProgressMetrics& metrics) {
    if (pImpl->progressHistory.size() >= pImpl->config.maxProgressHistory) {
        pImpl->progressHistory.erase(pImpl->progressHistory.begin());
    }
    
    pImpl->progressHistory.emplace_back(progressPercentage, metrics);
}

void ProgressReporter::outputProgress(double progressPercentage,
                                      const std::string& message,
                                      const ProgressMetrics& metrics) {
    if (!pImpl->isActive) {
        return;
    }
    
    std::stringstream ss;
    
    // Format timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    
    // Add operation name if configured
    if (!pImpl->currentOperationName.empty() &&
        (pImpl->config.verbosityLevel >= ProgressLevel::Normal ||
         pImpl->config.statusVerbosity >= ProgressConfig::StatusVerbosity::Summary)) {
        ss << " [" << pImpl->currentOperationName << "]";
    }
    
    // Add progress bar and metrics
    ss << " " << formatProgress();
    
    if (!message.empty()) {
        ss << " - " << message;
    }
    
    // Add metrics based on verbosity
    std::string metricsStr = formatMetrics(metrics);
    if (!metricsStr.empty()) {
        ss << " " << metricsStr;
    }
    
    // Output to stream
    std::ostream& out = (pImpl->config.outputStream ? *pImpl->config.outputStream : std::cout);
    
    if (pImpl->config.verbosityLevel >= ProgressLevel::Normal) {
        out << ss.str() << std::endl;
    }
    
    // For verbose levels, also log to logger
    if (pImpl->config.verbosityLevel >= ProgressLevel::Verbose ||
        pImpl->config.statusVerbosity >= ProgressConfig::StatusVerbosity::Detailed) {
        if (pImpl->config.enableLogging) {
            logProgress(ss.str(), LogLevel::Info);
        }
    }
}

double ProgressReporter::getElapsedTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (!pImpl->isActive) {
        return 0.0;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
    return static_cast<double>(duration.count());
}

std::vector<std::pair<double, ProgressMetrics>> ProgressReporter::getProgressHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->progressHistory;
}

std::vector<ProgressMilestone> ProgressReporter::getCompletedMilestones() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->completedMilestones;
}

bool ProgressReporter::isMilestoneReached(const std::string& milestoneName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (const auto& milestone : pImpl->completedMilestones) {
        if (milestone.name == milestoneName) {
            return true;
        }
    }
    return false;
}

void ProgressReporter::reset() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    pImpl->currentProgress = 0.0;
    pImpl->currentStep = 0;
    pImpl->completedMilestones.clear();
    pImpl->progressHistory.clear();
    pImpl->etaInfo = ETAInfo();
    pImpl->isActive = false;
    
    if (pImpl->config.enableLogging) {
        logProgress("Progress reporter reset", LogLevel::Info);
    }
}

ProgressReporter& ProgressReporter::getGlobalReporter() {
    static std::shared_ptr<ProgressReporter> globalReporter;
    if (!globalReporter) {
        globalReporter = std::make_shared<ProgressReporter>();
    }
    return *globalReporter;
}

void ProgressReporter::setGlobalReporter(std::shared_ptr<ProgressReporter> reporter) {
    static std::shared_ptr<ProgressReporter> globalReporter;
    globalReporter = reporter;
}

} // namespace nlm