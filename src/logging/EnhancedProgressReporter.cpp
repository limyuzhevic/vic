#include "EnhancedProgressReporter.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <cmath>
#include <set>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct EnhancedProgressReporter::Impl {
    std::shared_ptr<Logger> logger;
    
    std::map<std::string, EnhancedProgressReport> activeProgress;
    std::map<std::string, ProgressStage> activeStages;
    std::map<std::string, std::vector<ProgressMilestone>> milestones;
    
    // Configuration
    ProgressBarStyle progressBarStyle;
    bool enableProgressBar;
    bool enableTimeEstimates;
    bool enableColors;
    bool enableAnimations;
    bool enableNotifications;
    bool hierarchicalProgress;
    bool parallelProgress;
    bool adaptiveThrottling;
    size_t displayInterval;
    double updateFrequency;
    double progressUpdateThreshold;
    double speedWarningThreshold;
    int progressBarWidth;
    bool showPercentage;
    bool showSpeed;
    bool showETA;
    bool showCustomMetrics;
    size_t maxMemoryUsage;
    
    // Output
    std::ostream* outputStream;
    
    // Callbacks
    std::function<void(const EnhancedProgressReport&)> progressCallback;
    std::function<void(const ProgressNotification&)> notificationCallback;
    std::function<void(double)> progressThresholdCallback;
    
    // Performance tracking
    std::map<std::string, std::vector<double>> progressSpeeds;
    std::map<std::string, std::chrono::steady_clock::time_point> startTimes;
    std::map<std::string, std::chrono::steady_clock::time_point> lastUpdateTimes;
    
    // Statistics
    std::chrono::steady_clock::time_point sessionStartTime;
    std::atomic<size_t> totalUpdates{0};
    std::atomic<size_t> completedOperations{0};
    mutable std::mutex dataMutex;
    
    Impl(std::shared_ptr<Logger> logg)
        : logger(logg), progressBarStyle(ProgressBarStyle::Sparkle), enableProgressBar(true),
          enableTimeEstimates(true), enableColors(true), enableAnimations(true),
          enableNotifications(true), hierarchicalProgress(false), parallelProgress(false),
          adaptiveThrottling(false), displayInterval(100), updateFrequency(1.0),
          progressUpdateThreshold(0.01), speedWarningThreshold(1.0), progressBarWidth(40),
          showPercentage(true), showSpeed(true), showETA(true), showCustomMetrics(true),
          maxMemoryUsage(100 * 1024 * 1024), // 100MB
          outputStream(&std::cout) {
        sessionStartTime = std::chrono::steady_clock::now();
    }
};

EnhancedProgressReporter::EnhancedProgressReporter(std::shared_ptr<Logger> logger)
    : pImpl(std::make_unique<Impl>(logger)) {
}

EnhancedProgressReporter::~EnhancedProgressReporter() = default;

void EnhancedProgressReporter::startProgress(const std::string& testName, size_t totalSteps,
                                            const std::string& description,
                                            const std::string& stage) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    EnhancedProgressReport report(testName, 0, totalSteps);
    report.description = description;
    report.stage = stage;
    
    pImpl->activeProgress[testName] = report;
    pImpl->startTimes[testName] = std::chrono::steady_clock::now();
    pImpl->lastUpdateTimes[testName] = pImpl->startTimes[testName];
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(report);
    }
    
    // Initial display
    if (pImpl->enableProgressBar) {
        displayProgress(report);
    }
    
    logProgressEvent("progress_started", testName, {{ "total_steps", std::to_string(totalSteps) }});
}

void EnhancedProgressReporter::updateProgress(const std::string& testName, size_t currentStep,
                                             const std::string& status,
                                             std::map<std::string, double> customMetrics) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    updateProgressInternal(testName, currentStep, status, customMetrics);
}

void EnhancedProgressReporter::updateProgressInternal(const std::string& testName, size_t currentStep,
                                                     const std::string& status,
                                                     std::map<std::string, double> customMetrics) {
    auto it = pImpl->activeProgress.find(testName);
    if (it == pImpl->activeProgress.end()) return;
    
    EnhancedProgressReport& report = it->second;
    
    // Check if progress update is needed based on threshold
    double progressRatio = static_cast<double>(currentStep) / report.totalSteps;
    if (progressRatio < pImpl->progressUpdateThreshold && currentStep > 0) {
        return;
    }
    
    report.currentStep = currentStep;
    report.statusMessage = status;
    report.customMetrics = customMetrics;
    
    // Calculate progress speed
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->startTimes[testName]);
    
    if (elapsed.count() > 0) {
        report.progressSpeed = (static_cast<double>(currentStep) / elapsed.count()) * 1000.0; // steps per second
        pImpl->progressSpeeds[testName].push_back(report.progressSpeed);
        
        // Calculate advanced ETA
        calculateAdvancedETA(report);
        
        // Speed warning
        if (report.progressSpeed > 0 && report.progressSpeed < pImpl->speedWarningThreshold) {
            ProgressNotification warning(NotificationType::SpeedWarning, 
                                        "Progress speed warning: " + std::to_string(report.progressSpeed) + " steps/s",
                                        testName);
            if (pImpl->notificationCallback) {
                pImpl->notificationCallback(warning);
            }
        }
        
        // Check if we should display
        auto timeSinceLastDisplay = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - pImpl->lastUpdateTimes[testName]);
        
        if (timeSinceLastDisplay.count() >= 1000 / static_cast<int>(pImpl->updateFrequency) ||
            currentStep == report.totalSteps) {
            
            if (pImpl->enableProgressBar) {
                displayProgress(report);
            }
            
            pImpl->lastUpdateTimes[testName] = now;
        }
    }
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(report);
    }
    
    // Check milestones
    checkMilestones(testName, report);
    
    // Trigger threshold callback
    if (pImpl->progressThresholdCallback && progressRatio > 0) {
        pImpl->progressThresholdCallback(progressRatio);
    }
}

void EnhancedProgressReporter::calculateAdvancedETA(EnhancedProgressReport& report) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - report.startTime);
    
    if (elapsed.count() > 0 && report.currentStep > 0 && report.totalSteps > 0) {
        double completionRatio = static_cast<double>(report.currentStep) / report.totalSteps;
        auto estimatedTotal = (elapsed.count() / completionRatio);
        report.estimatedTimeRemaining = std::chrono::milliseconds(
            static_cast<long>(estimatedTotal - elapsed.count()));
        
        // Add confidence interval based on recent speed variations
        if (pImpl->progressSpeeds.find(report.testName) != pImpl->progressSpeeds.end()) {
            const auto& speeds = pImpl->progressSpeeds[report.testName];
            if (speeds.size() >= 3) {
                // Calculate standard deviation for confidence interval
                double mean = std::accumulate(speeds.begin(), speeds.end(), 0.0) / speeds.size();
                double sumSquares = 0.0;
                for (double speed : speeds) {
                    double diff = speed - mean;
                    sumSquares += diff * diff;
                }
                double stdDev = std::sqrt(sumSquares / speeds.size());
                
                // Add +/- 2*stdDev to ETA calculation
                double confidenceFactor = 1.0 + (stdDev / mean);
                report.estimatedTimeRemaining = std::chrono::milliseconds(
                    static_cast<long>(report.estimatedTimeRemaining.count() * confidenceFactor));
            }
        }
    }
}

double EnhancedProgressReporter::calculateEfficiencyScore(const EnhancedProgressReport& report) const {
    if (report.totalSteps == 0) return 0.0;
    
    // Simple efficiency calculation based on progress per unit time
    double timeEfficiency = report.progressSpeed > 0 ? 1.0 : 0.0;
    
    // Check if we're on schedule based on milestone progress
    double milestoneProgress = 0.0;
    if (pImpl->milestones.find(report.testName) != pImpl->milestones.end()) {
        const auto& testMilestones = pImpl->milestones[report.testName];
        for (const auto& milestone : testMilestones) {
            double milestoneRatio = milestone.percentage / 100.0;
            if (milestoneRatio <= (static_cast<double>(report.currentStep) / report.totalSteps)) {
                milestoneProgress += 1.0;
            }
        }
        if (!testMilestones.empty()) {
            milestoneProgress /= testMilestones.size();
        }
    }
    
    // Combined efficiency score
    return (timeEfficiency * 0.6) + (milestoneProgress * 0.4);
}

void EnhancedProgressReporter::displayProgress(const EnhancedProgressReport& report) {
    if (!pImpl->enableProgressBar) return;
    
    std::stringstream ss;
    
    // Test name and stage
    ss << "[" << report.testName << "]";
    if (!report.stage.empty()) {
        ss << " [Stage: " << report.stage << "]";
    }
    ss << " ";
    
    // Progress bar
    ss << formatProgressBar(report, pImpl->progressBarStyle) << " ";
    
    // Percentage
    if (pImpl->showPercentage) {
        ss << std::fixed << std::setprecision(1) << report.percentage << "% ";
    }
    
    // Current/Total
    ss << report.currentStep << "/" << report.totalSteps << " ";
    
    // Status
    ss << "- " << report.statusMessage << " ";
    
    // Speed
    if (pImpl->showSpeed && report.progressSpeed > 0) {
        ss << "[" << std::fixed << std::setprecision(1) << report.progressSpeed << " steps/s" << "; ";
    }
    
    // ETA
    if (pImpl->showETA && report.estimatedTimeRemaining.count() > 0) {
        auto eta = report.estimatedTimeRemaining;
        ss << "ETA: " << (eta.count() / 1000) << "s";
    }
    
    if (pImpl->showSpeed && report.progressSpeed > 0 && report.estimatedTimeRemaining.count() > 0) {
        ss << "]";
    }
    
    // Custom metrics
    if (pImpl->showCustomMetrics && !report.customMetrics.empty()) {
        ss << " | " << formatCustomMetrics(report.customMetrics);
    }
    
    ss << std::flush;
    
    // Output
    if (pImpl->outputStream) {
        *pImpl->outputStream << ss.str() << std::endl;
    }
}

std::string EnhancedProgressReporter::formatProgressBar(const EnhancedProgressReport& report, ProgressBarStyle style) const {
    std::stringstream ss;
    
    int barWidth = pImpl->progressBarWidth;
    int progressPos = static_cast<int>((static_cast<double>(report.currentStep) / report.totalSteps) * barWidth);
    
    switch (style) {
        case ProgressBarStyle::Classic:
            ss << "[";
            for (int i = 0; i < barWidth; ++i) {
                if (i < progressPos) {
                    ss << "=";
                } else if (i == progressPos) {
                    ss << ">";
                } else {
                    ss << " ";
                }
            }
            ss << "]";
            break;
            
        case ProgressBarStyle::Sparkle:
            ss << "["; 
            for (int i = 0; i < barWidth; ++i) {
                if (i < progressPos) {
                    // Animated sparkle effect (simple implementation)
                    if (i % 3 == 0) ss << "*";
                    else ss << "~";
                } else if (i == progressPos) {
                    ss << "@";
                } else {
                    ss << ".";
                }
            }
            ss << "]";
            break;
            
        case ProgressBarStyle::Blocks:
            ss << "[";
            for (int i = 0; i < barWidth; ++i) {
                if (i < progressPos) {
                    // Use Unicode block characters
                    if (i % 4 == 0) ss << "█";
                    else ss << "▓";
                } else if (i == progressPos) {
                    ss << "▌";
                } else {
                    ss << "░";
                }
            }
            ss << "]";
            break;
            
        case ProgressBarStyle::ASCII:
            ss << "[";
            for (int i = 0; i < barWidth; ++i) {
                if (i < progressPos) {
                    ss << "#";
                } else if (i == progressPos) {
                    ss << ">";
                } else {
                    ss << "-";
                }
            }
            ss << "]";
            break;
            
        case ProgressBarStyle::Custom1:
            ss << "[";
            for (int i = 0; i < barWidth; ++i) {
                if (i < progressPos) {
                    if (report.completed) ss << "■";
                    else if (report.failed) ss << "✗";
                    else ss << "◉";
                } else if (i == progressPos) {
                    ss << "▶";
                } else {
                    ss << "○";
                }
            }
            ss << "]";
            break;
            
        case ProgressBarStyle::Custom2:
            ss << "[";
            for (int i = 0; i < barWidth; ++i) {
                if (i < progressPos) {
                    ss << "⬛";
                } else if (i == progressPos) {
                    ss << "⬜";
                } else {
                    ss << "⬜";
                }
            }
            ss << "]";
            break;
    }
    
    return ss.str();
}

std::string EnhancedProgressReporter::formatCustomMetrics(const std::map<std::string, double>& metrics) const {
    std::stringstream ss;
    bool first = true;
    for (const auto& pair : metrics) {
        if (!first) ss << " | ";
        first = false;
        ss << pair.first << ": " << std::fixed << std::setprecision(3) << pair.second;
    }
    return ss.str();
}

void EnhancedProgressReporter::checkMilestones(const std::string& testName, const EnhancedProgressReport& report) {
    auto it = pImpl->milestones.find(testName);
    if (it == pImpl->milestones.end()) return;
    
    const auto& testMilestones = it->second;
    for (const auto& milestone : testMilestones) {
        double progressRatio = static_cast<double>(report.currentStep) / report.totalSteps;
        if (progressRatio >= milestone.percentage / 100.0 &&
            (report.currentStep < milestone.step || milestone.step == report.totalSteps)) {
            
            // Trigger milestone callback
            if (milestone.callback) {
                milestone.callback(report);
            }
            
            // Send notification
            ProgressNotification notification(NotificationType::Milestone,
                                             "Milestone reached: " + milestone.name,
                                             testName);
            notification.metadata["milestone_name"] = milestone.name;
            notification.metadata["milestone_percentage"] = std::to_string(milestone.percentage);
            
            if (pImpl->notificationCallback) {
                pImpl->notificationCallback(notification);
            }
            
            // Remove milestone if one-time
            if (milestone.step > 0) {
                // Check if we've passed this milestone
                if (report.currentStep >= milestone.step) {
                    logProgressEvent("milestone_completed", testName,
                                    {{ "milestone_name", milestone.name }});
                }
            }
        }
    }
}

void EnhancedProgressReporter::triggerNotifications(const std::string& testName, const EnhancedProgressReport& report) {
    // Stage change notification
    if (!report.stage.empty()) {
        ProgressNotification notification(NotificationType::StageChange,
                                         "Stage change: " + report.stage,
                                         testName);
        if (pImpl->notificationCallback) {
            pImpl->notificationCallback(notification);
        }
    }
    
    // Completion notification
    if (report.currentStep >= report.totalSteps) {
        ProgressNotification notification(NotificationType::EstimatedCompletion,
                                         "Progress completed: " + report.testName,
                                         testName);
        notification.metadata["final_status"] = report.failed ? "failed" : "completed";
        
        if (pImpl->notificationCallback) {
            pImpl->notificationCallback(notification);
        }
    }
}

void EnhancedProgressReporter::logProgressEvent(const std::string& event, const std::string& testName,
                                               const std::map<std::string, std::string>& metadata) {
    if (pImpl->logger) {
        std::stringstream ss;
        ss << "Progress event: " << event << " - " << testName;
        
        for (const auto& pair : metadata) {
            ss << " [" << pair.first << "=" << pair.second << "]";
        }
        
        pImpl->logger->info(ss.str());
    }
}