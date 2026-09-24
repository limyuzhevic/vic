// Visualization and Debugging Tools for NLM Phase 6
// Provides console-based visualization and debugging capabilities for the integrated brain

#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <chrono>

namespace nlm {

// Simple console-based visualization
class NLMVisualizer {
public:
    NLMVisualizer() = default;
    ~NLMVisualizer() = default;
    
    // Initialize visualizer with experiment
    void initialize(std::shared_ptr<Phase6IntegratedExperiment> experiment) {
        experiment_ = experiment;
    }
    
    // Create neuron activity heatmap visualization
    std::string createNeuronHeatmap(const std::vector<NeuronId>& neuronIds) {
        if (!experiment_ || !experiment_->getBrain()) return "No data available";
        
        auto* region = experiment_->getBrain()->getRegion(RegionId(1));
        if (!region) return "No regions available";
        
        std::stringstream ss;
        ss << "=== Neuron Activity Heatmap ===" << std::endl;
        
        // Calculate activity levels
        std::vector<float> activities;
        for (NeuronId neuronId : neuronIds) {
            auto* neuron = region->getNeuron(NeuronId(1), neuronId);  // Simplified
            if (neuron) {
                activities.push_back(std::abs(neuron->getState().membranePotential) / 100.0f);
            }
        }
        
        if (activities.empty()) return "No neuron activity data";
        
        // Create heatmap visualization
        float maxActivity = *std::max_element(activities.begin(), activities.end());
        int heatmapWidth = 60;
        
        ss << std::setw(heatmapWidth) << std::left << "Activity Scale: 0.0";
        ss << std::setfill('=') << std::setw(maxActivity * heatmapWidth) << "";
        ss << std::setfill(' ') << std::setw(maxActivity * heatmapWidth) << "1.0" << std::endl;
        ss << std::setw(heatmapWidth) << std::left << "";  // Reset
        
        ss << "Neuron activations: [";
        for (size_t i = 0; i < std::min(size_t(10), activities.size()); ++i) {
            ss << std::fixed << std::setprecision(2) << activities[i];
            if (i < activities.size() - 1 && i < 9) ss << ", ";
        }
        if (activities.size() > 10) ss << " ... (" << activities.size() - 10 << " more)";
        ss << "]" << std::endl;
        
        return ss.str();
    }
    
    // Create memory activity timeline visualization
    std::string createMemoryTimeline(const std::vector<EpisodicMemoryItem>& episodes) {
        if (episodes.empty()) return "No memory episodes available";
        
        std::stringstream ss;
        ss << "=== Memory Timeline ===" << std::endl;
        ss << "Episode Count: " << episodes.size() << std::endl << std::endl;
        
        ss << "Recent episodes:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), episodes.size()); ++i) {
            const auto& episode = episodes[i];
            ss << "  Episode " << (i + 1) << ": timestamp=" 
               << episode.timestamp << ", active_neurons=" 
               << episode.activeNeurons.size() << ", reward=" 
               << std::fixed << std::setprecision(2) << episode.reward << std::endl;
        }
        
        if (episodes.size() > 5) {
            ss << "  ... and " << (episodes.size() - 5) << " more episodes" << std::endl;
        }
        
        return ss.str();
    }
    
    // Create system status dashboard
    std::string createSystemStatusDashboard() {
        if (!experiment_) return "No experiment data available";
        
        std::stringstream ss;
        ss << "=== NLM Phase 6 System Status Dashboard ===" << std::endl;
        ss << std::endl;
        
        // Get experiment results
        const Phase6ExperimentResults& results = experiment_->getResults();
        
        // System status indicators
        auto printIndicator = [](const std::string& name, bool status, const std::string& icon = "") {
            std::string statusStr = status ? "ONLINE" : "OFFLINE";
            std::string color = status ? "✓" : "✗";
            ss << "  " << color << " " << name << ": " << statusStr << std::endl;
            if (!icon.empty()) ss << "     " << icon << std::endl;
        };
        
        // Memory systems
        ss << "Memory Systems:" << std::endl;
        printIndicator("Working Memory", results.memoryWorkingMemoryIntegrated,
                      (results.workingMemoryTraces > 0 ? 
                       "Active traces: " + std::to_string(results.workingMemoryTraces) : "Idle"));
        printIndicator("Episodic Memory", results.memoryEpisodicMemoryIntegrated,
                      "Episodes: " + std::to_string(results.memoryEpisodesStored));
        ss << std::endl;
        
        // Neuromodulation
        ss << "Neuromodulation Systems:" << std::endl;
        printIndicator("Dopamine", results.neuromodulationIntegrated,
                      "Level: " + std::to_string(results.dopamineLevel));
        printIndicator("Curiosity", results.neuromodulationIntegrated,
                      "Level: " + std::to_string(results.curiosityLevel));
        printIndicator("Prediction Error", results.neuromodulationIntegrated,
                      "Errors: " + std::to_string(results.predictionErrors));
        ss << std::endl;
        
        // Cognitive systems
        ss << "Cognitive Systems:" << std::endl;
        printIndicator("Attention", results.attentionIntegrated,
                      "Focus: " + std::to_string(static_cast<int>(results.developmentalStage)));
        printIndicator("Planning", results.planningIntegrated);
        printIndicator("Concept Formation", results.conceptFormationIntegrated);
        ss << std::endl;
        
        // Development
        ss << "Development System:" << std::endl;
        printIndicator("Development Active", results.developmentIntegrated,
                      "Stage: " + std::to_string(results.developmentalStage));
        ss << std::endl;
        
        // Prediction
        ss << "Prediction System:" << std::endl;
        printIndicator("Prediction Active", results.predictionIntegrated,
                      "Accuracy: " + std::to_string(results.predictionAccuracy * 100.0f) + "%");
        ss << std::endl;
        
        // Performance
        ss << "Performance Metrics:" << std::endl;
        ss << "  Learning Rate: " << std::fixed << std::setprecision(3) << results.learningRate << std::endl;
        ss << "  System Stability: " << std::fixed << std::setprecision(3) << results.stability << std::endl;
        ss << "  Adaptability: " << std::fixed << std::setprecision(3) << results.adaptability << std::endl;
        ss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << results.averageFiringRate << " Hz" << std::endl;
        ss << std::endl;
        
        return ss.str();
    }
    
    // Create debug log summary
    std::string createDebugLogSummary(const std::vector<std::string>& logEntries) {
        if (logEntries.empty()) return "No log entries available";
        
        std::stringstream ss;
        ss << "=== Debug Log Summary ===" << std::endl;
        ss << "Total log entries: " << logEntries.size() << std::endl;
        ss << std::endl;
        
        // Count log levels
        int infoCount = 0, warningCount = 0, errorCount = 0;
        for (const auto& entry : logEntries) {
            if (entry.find("[INFO]") != std::string::npos) infoCount++;
            else if (entry.find("[WARNING]") != std::string::npos) warningCount++;
            else if (entry.find("[ERROR]") != std::string::npos) errorCount++;
        }
        
        ss << "Log distribution:" << std::endl;
        ss << "  INFO: " << infoCount << std::endl;
        ss << "  WARNING: " << warningCount << std::endl;
        ss << "  ERROR: " << errorCount << std::endl;
        ss << std::endl;
        
        // Show recent entries
        ss << "Recent log entries:" << std::endl;
        int showCount = std::min(5, static_cast<int>(logEntries.size()));
        for (int i = 0; i < showCount; ++i) {
            ss << "  " << logEntries[logEntries.size() - 1 - i] << std::endl;
        }
        if (logEntries.size() > 5) {
            ss << "  ... and " << (logEntries.size() - 5) << " more entries" << std::endl;
        }
        
        return ss.str();
    }
    
    // Run all visualizations
    std::string runAllVisualizations() {
        if (!experiment_) {
            return "No experiment data available for visualization";
        }
        
        std::stringstream ss;
        ss << "=== NLM Phase 6 Visualization Suite ===" << std::endl << std::endl;
        
        // Get neuron IDs from experiment brain
        std::vector<NeuronId> neuronIds;
        if (experiment_->getBrain()) {
            auto* region = experiment_->getBrain()->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    neuronIds.push_back(neuron->getId());
                }
            }
        }
        
        // Get episodic memory episodes
        std::vector<EpisodicMemoryItem> episodes;
        if (experiment_->getBrain()) {
            auto* episodicMemory = experiment_->getBrain()->getEpisodicMemory();
            if (episodicMemory) {
                // Get episodes (implementation would need to be provided)
                episodes = {};  // Placeholder - actual implementation would retrieve episodes
            }
        }
        
        // Run all visualizations
        ss << createNeuronHeatmap(neuronIds) << std::endl;
        ss << createMemoryTimeline(episodes) << std::endl;
        ss << createSystemStatusDashboard() << std::endl;
        
        // Add debug log summary if available
        ss << createDebugLogSummary({}) << std::endl;
        
        return ss.str();
    }

private:
    std::shared_ptr<Phase6IntegratedExperiment> experiment_;
};

// Performance profiler for NLM
class NLMProfiler {
public:
    NLMProfiler() : startTime_(std::chrono::steady_clock::now()), 
                   stepCount_(0), totalTime_(0.0), peakMemoryUsage_(0) {
    }
    
    ~NLMProfiler() {
    }
    
    // Record start of measurement
    void startMeasurement() {
        startTime_ = std::chrono::steady_clock::now();
    }
    
    // Record end of measurement and get duration
    double endMeasurement() {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
        return duration.count();
    }
    
    // Record a simulation step
    void recordStep() {
        stepCount_++;
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime_);
        totalTime_ += duration.count();
        startTime_ = currentTime;  // Reset for next measurement
    }
    
    // Get average time per step
    double getAverageStepTime() const {
        return stepCount_ > 0 ? totalTime_ / stepCount_ : 0.0;
    }
    
    // Get total steps recorded
    size_t getStepCount() const {
        return stepCount_;
    }
    
    // Get total elapsed time
    double getTotalTime() const {
        return totalTime_;
    }
    
    // Generate performance report
    std::string generatePerformanceReport() const {
        std::stringstream ss;
        ss << "=== NLM Performance Profile ===" << std::endl;
        ss << "Total simulation steps: " << stepCount_ << std::endl;
        ss << std::fixed << std::setprecision(3);
        ss << "Average time per step: " << getAverageStepTime() << " seconds" << std::endl;
        ss << "Total elapsed time: " << totalTime_ << " seconds" << std::endl;
        ss << "Steps per second: " << (stepCount_ / std::max(totalTime_, 0.001)) << std::endl;
        ss << std::endl;
        
        if (stepCount_ > 0) {
            ss << "Performance Metrics:" << std::endl;
            double stepsPerSecond = stepCount_ / std::max(totalTime_, 0.001);
            
            if (stepsPerSecond > 100.0) {
                ss << "  ✓ Excellent performance ( >100 steps/sec )" << std::endl;
            } else if (stepsPerSecond > 10.0) {
                ss << "  ✓ Good performance (10-100 steps/sec)" << std::endl;
            } else if (stepsPerSecond > 1.0) {
                ss << "  ⚠ Moderate performance (1-10 steps/sec)" << std::endl;
            } else {
                ss << "  ✗ Low performance (<1 step/sec)" << std::endl;
            }
        }
        
        return ss.str();
    }
    
    // Reset profiler
    void reset() {
        startTime_ = std::chrono::steady_clock::now();
        stepCount_ = 0;
        totalTime_ = 0.0;
        peakMemoryUsage_ = 0;
    }

private:
    std::chrono::steady_clock::time_point startTime_;
    size_t stepCount_;
    double totalTime_;
    size_t peakMemoryUsage_;
};

} // namespace nlm
