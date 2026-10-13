// Progress Monitor Implementation
//
// This file provides the implementation for the ProgressMonitor class,
// which handles progress reporting and timing for NLM experiments.

#include "ProgressMonitor.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {

struct ProgressMonitor::Impl {
    int totalSteps;
    int currentStep;
    std::string operation;
    bool enabled;
    std::ostream* outputStream;
    int lastDisplayStep;
    int updateFrequency;
    bool timingStarted;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
};

ProgressMonitor::ProgressMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->totalSteps = 0;
    pImpl->currentStep = 0;
    pImpl->operation = "Unknown operation";
    pImpl->enabled = true;
    pImpl->outputStream = &std::cout;
    pImpl->lastDisplayStep = -1;
    pImpl->updateFrequency = 100;
    pImpl->timingStarted = false;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
}

ProgressMonitor::~ProgressMonitor() = default;

void ProgressMonitor::setTotalSteps(int totalSteps) {
    pImpl->totalSteps = totalSteps;
}

void ProgressMonitor::setCurrentStep(int currentStep) {
    pImpl->currentStep = currentStep;
    if (pImpl->timingStarted) {
        pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    }
}

void ProgressMonitor::setOperation(const std::string& operation) {
    pImpl->operation = operation;
}

void ProgressMonitor::updateProgress(int step, const std::string& message) {
    if (!pImpl->enabled) return;
    
    pImpl->currentStep = step;
    if (pImpl->timingStarted) {
        pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    }
    
    if (shouldUpdateDisplay(step)) {
        *pImpl->outputStream << "\r[" << std::setw(3) << getProgressPercentage() << "%] " 
                           << message << " (Step " << step << "/" << pImpl->totalSteps << ")...";
        *pImpl->outputStream << std::flush;
        pImpl->lastDisplayStep = step;
    }
}

void ProgressMonitor::complete(const std::string& finalMessage) {
    if (!pImpl->enabled) return;
    
    stopTiming();
    *pImpl->outputStream << "\r[100%] " << finalMessage << " (Step " << pImpl->totalSteps << "/" << pImpl->totalSteps << ")..." << std::endl;
    *pImpl->outputStream << "Completed in " << getElapsedTime() << " seconds." << std::endl;
}

float ProgressMonitor::getProgressPercentage() const {
    if (pImpl->totalSteps <= 0) return 0.0f;
    return static_cast<float>(pImpl->currentStep) / pImpl->totalSteps * 100.0f;
}

int ProgressMonitor::getCurrentStep() const {
    return pImpl->currentStep;
}

int ProgressMonitor::getTotalSteps() const {
    return pImpl->totalSteps;
}

const std::string& ProgressMonitor::getOperation() const {
    return pImpl->operation;
}

bool ProgressMonitor::isEnabled() const {
    return pImpl->enabled;
}

void ProgressMonitor::enable(bool enabled) {
    pImpl->enabled = enabled;
}

void ProgressMonitor::setOutputStream(std::ostream& outputStream) {
    pImpl->outputStream = &outputStream;
}

double ProgressMonitor::getElapsedTime() const {
    if (!pImpl->timingStarted) return 0.0;
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->startTime).count();
    return duration / 1000.0;
}

double ProgressMonitor::getEstimatedTimeRemaining() const {
    if (pImpl->totalSteps <= 0 || pImpl->currentStep <= 0) return 0.0;
    
    double elapsedTime = getElapsedTime();
    float progress = getProgressPercentage() / 100.0f;
    
    if (progress <= 0.0f) return 0.0;
    
    double estimatedTotalTime = elapsedTime / progress;
    double remainingTime = estimatedTotalTime - elapsedTime;
    
    return remainingTime;
}

void ProgressMonitor::startTiming() {
    pImpl->timingStarted = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
}

void ProgressMonitor::stopTiming() {
    pImpl->timingStarted = false;
}

void ProgressMonitor::setUpdateFrequency(int frequency) {
    pImpl->updateFrequency = frequency;
}

bool ProgressMonitor::shouldUpdateDisplay(int currentStep) const {
    if (!pImpl->enabled) return false;
    if (pImpl->totalSteps <= 0) return false;
    
    if (currentStep == pImpl->totalSteps) return true; // Always update on completion
    
    return (currentStep % pImpl->updateFrequency == 0) || (currentStep - pImpl->lastDisplayStep >= 100);
}

// Status Reporter Implementation

struct StatusReporter::Impl {
    int verbosityLevel;
    bool colorOutput;
    std::vector<std::pair<std::string, std::string>> statusEntries;
    std::vector<std::pair<std::string, double>> performanceMetrics;
    std::vector<std::pair<std::string, double>> systemMetrics;
    std::chrono::steady_clock::time_point startTime;
    std::vector<std::pair<std::string, std::chrono::steady_clock::time_point>> timingCheckpoints;
};

StatusReporter::StatusReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->verbosityLevel = 1;
    pImpl->colorOutput = true;
    pImpl->startTime = std::chrono::steady_clock::now();
}

StatusReporter::~StatusReporter() = default;

void StatusReporter::addStatusEntry(const std::string& category, const std::string& message) {
    if (pImpl->verbosityLevel >= 1) {
        pImpl->statusEntries.emplace_back(category, message);
    }
}

void StatusReporter::addPerformanceMetric(const std::string& name, double value, const std::string& unit) {
    pImpl->performanceMetrics.emplace_back(name, value);
}

void StatusReporter::addSystemMetric(const std::string& name, double value, const std::string& unit) {
    pImpl->systemMetrics.emplace_back(name, value);
}

std::string StatusReporter::generateReport() const {
    std::stringstream report;
    
    report << "=== NLM Status Report ===" << std::endl;
    report << "Generated at: " << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - pImpl->startTime).count() << " ms ago" << std::endl;
    report << "" << std::endl;
    
    if (pImpl->verbosityLevel >= 1) {
        report << "--- Status Entries ---" << std::endl;
        for (const auto& entry : pImpl->statusEntries) {
            report << entry.first << ": " << entry.second << std::endl;
        }
        report << "" << std::endl;
    }
    
    if (pImpl->verbosityLevel >= 2) {
        report << "--- Performance Metrics ---" << std::endl;
        for (const auto& metric : pImpl->performanceMetrics) {
            report << metric.first << ": " << std::fixed << std::setprecision(4) << metric.second << std::endl;
        }
        report << "" << std::endl;
        
        report << "--- System Metrics ---" << std::endl;
        for (const auto& metric : pImpl->systemMetrics) {
            report << metric.first << ": " << std::fixed << std::setprecision(4) << metric.second << std::endl;
        }
        report << "" << std::endl;
    }
    
    // Add timing information
    report << "--- Timing Information ---" << std::endl;
    double totalTime = getTotalTime();
    report << "Total time: " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    
    for (const auto& checkpoint : pImpl->timingCheckpoints) {
        auto checkpointTime = getCheckpointTime(checkpoint.first);
        report << checkpoint.first << ": " << std::fixed << std::setprecision(2) << checkpointTime << " seconds" << std::endl;
    }
    
    return report.str();
}

void StatusReporter::printReport(std::ostream& outputStream) const {
    outputStream << generateReport();
}

void StatusReporter::clear() {
    pImpl->statusEntries.clear();
    pImpl->performanceMetrics.clear();
    pImpl->systemMetrics.clear();
    pImpl->timingCheckpoints.clear();
    pImpl->startTime = std::chrono::steady_clock::now();
}

void StatusReporter::setVerbosityLevel(int level) {
    pImpl->verbosityLevel = level;
}

int StatusReporter::getVerbosityLevel() const {
    return pImpl->verbosityLevel;
}

void StatusReporter::setColorOutput(bool enabled) {
    pImpl->colorOutput = enabled;
}

bool StatusReporter::getColorOutput() const {
    return pImpl->colorOutput;
}

void StatusReporter::addTimingCheckpoint(const std::string& checkpointName) {
    pImpl->timingCheckpoints.emplace_back(checkpointName, std::chrono::steady_clock::now());
}

double StatusReporter::getCheckpointTime(const std::string& checkpointName) const {
    for (const auto& checkpoint : pImpl->timingCheckpoints) {
        if (checkpoint.first == checkpointName) {
            auto checkpointTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - checkpoint.second).count();
            return checkpointTime / 1000.0;
        }
    }
    return 0.0;
}

double StatusReporter::getTotalTime() const {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - pImpl->startTime).count();
    return duration / 1000.0;
}

// Experiment Progress Tracker Implementation

struct ExperimentProgressTracker::Impl {
    std::string experimentName;
    bool running;
    int totalSteps;
    int currentStep;
    bool detailedTracking;
    std::vector<std::pair<std::string, int>> milestones;
    std::vector<std::pair<std::string, std::string>> events;
    std::vector<std::pair<std::string, double>> performanceEvents;
    std::chrono::steady_clock::time_point startTime;
};

ExperimentProgressTracker::ExperimentProgressTracker() : pImpl(std::make_unique<Impl>()) {
    pImpl->experimentName = "Unnamed Experiment";
    pImpl->running = false;
    pImpl->totalSteps = 0;
    pImpl->currentStep = 0;
    pImpl->detailedTracking = false;
    pImpl->startTime = std::chrono::steady_clock::now();
}

ExperimentProgressTracker::~ExperimentProgressTracker() = default;

void ExperimentProgressTracker::startExperiment(const std::string& experimentName) {
    pImpl->experimentName = experimentName;
    pImpl->running = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->milestones.clear();
    pImpl->events.clear();
    pImpl->performanceEvents.clear();
}

void ExperimentProgressTracker::endExperiment() {
    pImpl->running = false;
}

void ExperimentProgressTracker::recordMilestone(const std::string& milestone, int step, const std::string& description) {
    pImpl->milestones.emplace_back(milestone, step);
}

void ExperimentProgressTracker::recordEvent(const std::string& event, const std::string& details) {
    pImpl->events.emplace_back(event, details);
}

void ExperimentProgressTracker::recordPerformance(const std::string& event, double value, const std::string& unit) {
    pImpl->performanceEvents.emplace_back(event + " (" + unit + ")", value);
}

const std::string& ExperimentProgressTracker::getExperimentName() const {
    return pImpl->experimentName;
}

int ExperimentProgressTracker::getTotalSteps() const {
    return pImpl->totalSteps;
}

int ExperimentProgressTracker::getCurrentStep() const {
    return pImpl->currentStep;
}

void ExperimentProgressTracker::setTotalSteps(int totalSteps) {
    pImpl->totalSteps = totalSteps;
}

void ExperimentProgressTracker::setCurrentStep(int step) {
    pImpl->currentStep = step;
}

double ExperimentProgressTracker::getElapsedTime() const {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - pImpl->startTime).count();
    return duration / 1000.0;
}

std::string ExperimentProgressTracker::generateProgressReport() const {
    std::stringstream report;
    
    report << "=== Experiment Progress Report ===" << std::endl;
    report << "Experiment: " << pImpl->experimentName << std::endl;
    report << "Status: " << (pImpl->running ? "RUNNING" : "COMPLETED") << std::endl;
    report << "Progress: " << pImpl->currentStep << "/" << pImpl->totalSteps << " steps ("
           << (pImpl->totalSteps > 0 ? (pImpl->currentStep * 100.0 / pImpl->totalSteps) : 0) << "%)" << std::endl;
    report << "Elapsed Time: " << std::fixed << std::setprecision(2) << getElapsedTime() << " seconds" << std::endl;
    report << "" << std::endl;
    
    if (!pImpl->milestones.empty()) {
        report << "--- Milestones ---" << std::endl;
        for (const auto& milestone : pImpl->milestones) {
            report << "Step " << milestone.second << ": " << milestone.first << std::endl;
        }
        report << "" << std::endl;
    }
    
    if (!pImpl->events.empty()) {
        report << "--- Events ---" << std::endl;
        for (const auto& event : pImpl->events) {
            report << event.first << ": " << event.second << std::endl;
        }
        report << "" << std::endl;
    }
    
    if (!pImpl->performanceEvents.empty()) {
        report << "--- Performance Events ---" << std::endl;
        for (const auto& perfEvent : pImpl->performanceEvents) {
            report << perfEvent.first << ": " << std::fixed << std::setprecision(4) << perfEvent.second << std::endl;
        }
        report << "" << std::endl;
    }
    
    return report.str();
}

void ExperimentProgressTracker::printProgressReport(std::ostream& outputStream) const {
    outputStream << generateProgressReport();
}

bool ExperimentProgressTracker::isRunning() const {
    return pImpl->running;
}

void ExperimentProgressTracker::enableDetailedTracking(bool enabled) {
    pImpl->detailedTracking = enabled;
}

bool ExperimentProgressTracker::getDetailedTracking() const {
    return pImpl->detailedTracking;
}

} // namespace nlm