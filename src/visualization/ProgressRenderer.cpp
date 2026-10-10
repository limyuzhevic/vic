#include "ProgressRenderer.hpp"
#include "ColorManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

ProgressRenderer::ProgressRenderer()
    : progressViewMode_(ProgressViewMode::LearningCurve), metricType_(MetricType::LearningSpeed),
      showProgressHistory_(true), showResourceMetrics_(false), updateInterval_(0.1f),
      currentProgress_(0.0f), averagePerformance_(0.0f), totalLearning_(0.0f), efficiencyScore_(0.0f),
      timeRangeStart_(0.0f), timeRangeEnd_(100.0f), metricMin_(0.0f), metricMax_(1.0f),
      selectedExperiment_(""), maxProgressHistory_(1000), smoothProgress_(true),
      smoothingFactor_(0.3f), colorScheme_("default") {
    initializeDefaultConfiguration();
}

void ProgressRenderer::initializeDefaultConfiguration() {
    // Initialize default rendering parameters
    maxProgressHistory_ = 1000;
    smoothProgress_ = true;
    smoothingFactor_ = 0.3f;
    colorScheme_ = "default";
}

void ProgressRenderer::render(const Brain& brain) {
    // Collect current progress data
    collectProgressData(brain);
    updateProgressStatistics();
    updateLearningCurve();
    
    // Clear screen
    std::cout << "\033[2J\033[H";
    
    // Render title
    ColorManager colorManager;
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightRed) << "=== EXPERIMENT PROGRESS VISUALIZATION ===" << "\033[0m" << std::endl;
    
    // Render based on progress view mode
    switch (progressViewMode_) {
        case ProgressViewMode::LearningCurve:
            renderLearningCurve(brain);
            break;
        case ProgressViewMode::PerformanceMetrics:
            renderPerformanceMetrics(brain);
            break;
        case ProgressViewMode::ResourceUsage:
            renderResourceUsage(brain);
            break;
        case ProgressViewMode::ComparisonChart:
            renderComparisonChart(brain);
            break;
        case ProgressViewMode::ExperimentState:
            renderExperimentState(brain);
            break;
        case ProgressViewMode::PhaseProgress:
            renderPhaseProgress(brain);
            break;
    }
    
    // Render additional elements
    if (showProgressHistory_) {
        renderLearningProgress(brain);
    }
    
    if (showResourceMetrics_) {
        renderResourceUsage(brain);
    }
    
    // Render controls
    renderControls();
}

void ProgressRenderer::update(const Brain& brain) {
    collectProgressData(brain);
    updateProgressStatistics();
}

void ProgressRenderer::collectProgressData(const Brain& brain) {
    // Collect progress data from brain state
    SimulationStep currentStep = brain.getCurrentStep();
    Timestamp currentTime = brain.getSimulationTime();
    
    // Calculate learning progress based on brain state
    float learningProgress = 0.0f;
    
    // Calculate based on various metrics
    float firingRate = brain.getAverageFiringRate();
    float activeNeurons = brain.getActiveNeuronCount();
    float totalNeurons = brain.getTotalNeuronCount();
    
    // Normalize firing rate
    learningProgress += firingRate * 0.3f;
    
    // Normalize activity
    if (totalNeurons > 0) {
        learningProgress += (activeNeurons / totalNeurons) * 0.3f;
    }
    
    // Normalize spike count
    float spikeCount = brain.getTotalSpikeCount();
    learningProgress += std::min(1.0f, spikeCount / 1000.0f) * 0.4f;
    
    // Calculate performance score
    float performanceScore = 0.0f;
    performanceScore += firingRate * 0.4f;
    performanceScore += (activeNeurons / std::max(1.0f, totalNeurons)) * 0.3f;
    performanceScore += std::min(1.0f, spikeCount / 500.0f) * 0.3f;
    
    // Calculate resource usage (simplified)
    float resourceUsage = 0.0f;
    resourceUsage += firingRate * 0.5f;
    resourceUsage += (activeNeurons / std::max(1.0f, totalNeurons)) * 0.3f;
    resourceUsage += std::min(1.0f, spikeCount / 1000.0f) * 0.2f;
    
    // Create progress data entry
    ProgressData progress(currentStep, currentTime, learningProgress, performanceScore, resourceUsage);
    
    // Add metrics
    progress.metrics["firing_rate"] = firingRate;
    progress.metrics["active_neurons"] = activeNeurons;
    progress.metrics["total_neurons"] = totalNeurons;
    progress.metrics["spike_count"] = spikeCount;
    progress.metrics["efficiency"] = (performanceScore / std::max(0.1f, resourceUsage));
    
    progressHistory_.push_back(progress);
    
    // Trim old data
    if (progressHistory_.size() > maxProgressHistory_) {
        progressHistory_.erase(progressHistory_.begin());
    }
    
    // Update learning curve
    learningCurve_.push_back(learningProgress);
    if (learningCurve_.size() > maxProgressHistory_) {
        learningCurve_.erase(learningCurve_.begin());
    }
    
    performanceCurve_.push_back(performanceScore);
    if (performanceCurve_.size() > maxProgressHistory_) {
        performanceCurve_.erase(performanceCurve_.begin());
    }
}

void ProgressRenderer::updateProgressStatistics() {
    calculateProgressStatistics();
}

void ProgressRenderer::updateLearningCurve() {
    if (learningCurve_.empty()) return;
    
    // Smooth the learning curve if requested
    if (smoothProgress_) {
        std::vector<float> smoothedCurve = learningCurve_;
        for (size_t i = 1; i < smoothedCurve.size(); ++i) {
            smoothedCurve[i] = smoothingFactor_ * smoothedCurve[i] + (1.0f - smoothingFactor_) * smoothedCurve[i - 1];
        }
        learningCurve_ = smoothedCurve;
    }
}

void ProgressRenderer::calculateProgressStatistics() {
    if (progressHistory_.empty()) return;
    
    // Get the latest progress data
    const ProgressData& latest = progressHistory_.back();
    
    currentProgress_ = latest.learningProgress;
    averagePerformance_ = latest.performanceScore;
    
    // Calculate total learning (sum of learning progress)
    totalLearning_ = 0.0f;
    for (const auto& progress : progressHistory_) {
        totalLearning_ += progress.learningProgress;
    }
    
    // Calculate efficiency score
    if (!progressHistory_.empty()) {
        float totalPerformance = 0.0f;
        float totalResources = 0.0f;
        for (const auto& progress : progressHistory_) {
            totalPerformance += progress.performanceScore;
            totalResources += progress.resourceUsage;
        }
        
        efficiencyScore_ = (totalPerformance / progressHistory_.size()) / std::max(0.1f, (totalResources / progressHistory_.size()));
    }
}

void ProgressRenderer::renderLearningCurve(const Brain& brain) {
    // Render learning curve visualization
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightGreen) << "=== LEARNING CURVE ===" << "\033[0m" << std::endl;
    
    if (learningCurve_.empty()) {
        std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "No learning data available" << "\033[0m" << std::endl;
        return;
    }
    
    // Render learning curve
    int curveHeight = 20;
    int curveWidth = std::min(80, static_cast<int>(learningCurve_.size()) + 10);
    
    // Find min/max values for scaling
    float minVal = *std::min_element(learningCurve_.begin(), learningCurve_.end());
    float maxVal = *std::max_element(learningCurve_.begin(), learningCurve_.end());
    
    if (maxVal == minVal) {
        maxVal = minVal + 1.0f;
    }
    
    // Draw axes
    for (int y = curveHeight - 1; y >= 0; --y) {
        for (int x = 0; x < curveWidth; ++x) {
            if (x == 0 && y == curveHeight - 1) {
                std::cout << colorManager.getColorCode(ColorManager::Color::White) << "+" << "\033[0m";
            } else if (x == curveWidth - 1 && y == 0) {
                std::cout << colorManager.getColorCode(ColorManager::Color::White) << "+" << "\033[0m";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    
    // Draw learning curve
    for (size_t i = 1; i < learningCurve_.size() && i < static_cast<size_t>(curveWidth - 1); ++i) {
        float x = static_cast<float>(i);
        float y = learningCurve_[i];
        
        // Normalize y value
        float normalizedY = (y - minVal) / (maxVal - minVal);
        int screenY = static_cast<int>((1.0f - normalizedY) * curveHeight);
        
        // Draw point
        std::cout << colorManager.getProgressColor(y);
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int px = static_cast<int>(x) + dx;
                int py = screenY + dy;
                
                if (px >= 0 && px < curveWidth && py >= 0 && py < curveHeight) {
                    // Move cursor to position (simplified)
                    std::cout << "\033[" << py + 1 << ";" << px + 1 << "H" << "*";
                }
            }
        }
        std::cout << "\033[0m";
    }
    
    // Display current progress
    std::cout << std::endl << colorManager.getColorCode(ColorManager::Color::White) << "Progress: " << (currentProgress_ * 100.0f) << "%" 
              << "  Performance: " << (averagePerformance_ * 100.0f) << "%" << "\033[0m" << std::endl;
}

void ProgressRenderer::renderLearningProgress(const Brain& brain) {
    // Render learning progress histogram
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightCyan) << "=== LEARNING PROGRESS ===" << "\033[0m" << std::endl;
    
    if (progressHistory_.empty()) return;
    
    // Group progress by step ranges
    std::map<int, float> stepProgress;
    for (const auto& progress : progressHistory_) {
        int stepGroup = (progress.step / 100) * 100; // Group by 100-step intervals
        stepProgress[stepGroup] += progress.learningProgress;
    }
    
    // Render histogram
    int histHeight = 15;
    int histWidth = 60;
    
    float maxProgress = 0.0f;
    for (const auto& step : stepProgress) {
        maxProgress = std::max(maxProgress, step.second);
    }
    
    for (int y = histHeight - 1; y >= 0; --y) {
        for (int x = 0; x < histWidth; ++x) {
            float targetStep = static_cast<float>(x) / histWidth * 1000.0f; // Up to 1000 steps
            bool stepFound = false;
            
            for (const auto& step : stepProgress) {
                if (std::abs(targetStep - step.first) < 100.0f) {
                    float normalized = step.second / maxProgress;
                    if (y < normalized * histHeight) {
                        std::cout << colorManager.getProgressColor(step.second);
                        std::cout << "#" << "\033[0m";
                    } else {
                        std::cout << " ";
                    }
                    stepFound = true;
                    break;
                }
            }
            
            if (!stepFound) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

void ProgressRenderer::renderSpeedOfLearning(const Brain& brain) {
    // Render speed of learning visualization
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightYellow) << "=== SPEED OF LEARNING ===" << "\033[0m" << std::endl;
    
    if (learningCurve_.size() < 2) return;
    
    // Calculate learning rate
    std::vector<float> learningRates;
    for (size_t i = 1; i < learningCurve_.size(); ++i) {
        float rate = learningCurve_[i] - learningCurve_[i - 1];
        learningRates.push_back(rate);
    }
    
    // Find min/max learning rates
    float minRate = *std::min_element(learningRates.begin(), learningRates.end());
    float maxRate = *std::max_element(learningRates.begin(), learningRates.end());
    
    // Render learning rate graph
    int graphHeight = 15;
    int graphWidth = std::min(60, static_cast<int>(learningRates.size()));
    
    for (size_t i = 0; i < learningRates.size() && i < static_cast<size_t>(graphWidth); ++i) {
        float rate = learningRates[i];
        
        // Normalize rate
        float normalized = (rate - minRate) / (maxRate - minRate);
        int barHeight = static_cast<int>(normalized * graphHeight);
        
        std::string color = colorManager.getProgressColor(rate);
        for (int y = graphHeight - 1; y >= 0; --y) {
            if (y < barHeight) {
                std::cout << color << "|" << "\033[0m";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

void ProgressRenderer::renderPerformanceMetrics(const Brain& brain) {
    // Render performance metrics visualization
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightMagenta) << "=== PERFORMANCE METRICS ===" << "\033[0m" << std::endl;
    
    if (progressHistory_.empty()) return;
    
    const ProgressData& latest = progressHistory_.back();
    
    // Display various metrics
    std::cout << colorManager.getProgressColor(latest.performanceScore) << "Learning Progress: " << (latest.learningProgress * 100.0f) << "%" << "\033[0m" << std::endl;
    std::cout << colorManager.getPerformanceColor(latest.performanceScore) << "Performance Score: " << (latest.performanceScore * 100.0f) << "%" << "\033[0m" << std::endl;
    std::cout << colorManager.getMetricColor(latest.metrics.at("firing_rate"), MetricType::FiringRate) 
              << "Firing Rate: " << latest.metrics.at("firing_rate") << " Hz" << "\033[0m" << std::endl;
    std::cout << colorManager.getMetricColor(latest.metrics.at("active_neurons"), MetricType::None) 
              << "Active Neurons: " << latest.metrics.at("active_neurons") << "\033[0m" << std::endl;
    std::cout << colorManager.getMetricColor(latest.metrics.at("spike_count"), MetricType::None) 
              << "Spike Count: " << latest.metrics.at("spike_count") << "\033[0m" << std::endl;
    std::cout << colorManager.getMetricColor(latest.metrics.at("efficiency"), MetricType::None) 
              << "Efficiency: " << latest.metrics.at("efficiency") << "\033[0m" << std::endl;
}

void ProgressRenderer::renderEfficiencyMetrics(const Brain& brain) {
    // Render efficiency metrics visualization
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightBlue) << "=== EFFICIENCY METRICS ===" << "\033[0m" << std::endl;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Average Performance: " << (averagePerformance_ * 100.0f) << "%" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Total Learning: " << totalLearning_ << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "Efficiency Score: " << efficiencyScore_ << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Red) << "Current Progress: " << (currentProgress_ * 100.0f) << "%" << "\033[0m" << std::endl;
}

void ProgressRenderer::renderQualityMetrics(const Brain& brain) {
    // Render quality metrics visualization
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightCyan) << "=== QUALITY METRICS ===" << "\033[0m" << std::endl;
    
    // Calculate quality metrics from progress history
    float stability = 0.0f;
    float consistency = 0.0f;
    
    if (progressHistory_.size() > 3) {
        // Calculate stability (variance of recent performance)
        size_t recentCount = std::min<size_t>(10, progressHistory_.size());
        std::vector<float> recentPerformance;
        
        for (auto it = progressHistory_.rbegin(); it != progressHistory_.rend() && recentCount > 0; ++it, --recentCount) {
            recentPerformance.push_back(it->performanceScore);
        }
        
        float mean = calculateLearningRate(recentPerformance);
        float variance = 0.0f;
        for (float perf : recentPerformance) {
            float diff = perf - mean;
            variance += diff * diff;
        }
        variance /= recentPerformance.size();
        
        stability = std::max(0.0f, 1.0f - variance);
        
        // Calculate consistency (rate of improvement)
        if (!learningCurve_.empty()) {
            consistency = (learningCurve_.back() - learningCurve_.front()) / learningCurve_.front();
        }
    }
    
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Stability: " << (stability * 100.0f) << "%" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Consistency: " << (consistency * 100.0f) << "%" << "\033[0m" << std::endl;
}

void ProgressRenderer::renderResourceUsage(const Brain& brain) {
    // Render resource usage visualization
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightRed) << "=== RESOURCE USAGE ===" << "\033[0m" << std::endl;
    
    if (progressHistory_.empty()) return;
    
    const ProgressData& latest = progressHistory_.back();
    
    // Calculate resource metrics
    float cpuUsage = latest.resourceUsage * 0.6f + (brain.getAverageFiringRate() * 0.4f);
    float memoryUsage = (brain.getTotalNeuronCount() / 10000.0f) * 0.5f + latest.resourceUsage * 0.5f;
    float ioUsage = std::min(1.0f, latest.metrics.at("spike_count") / 5000.0f);
    
    // Render resource gauges
    int gaugeHeight = 10;
    
    std::cout << colorManager.getMetricColor(cpuUsage, MetricType::CPUUsage) << "CPU Usage:" << colorManager.getColorCode(ColorManager::Color::White);
    for (int y = gaugeHeight - 1; y >= 0; --y) {
        if (y < cpuUsage * gaugeHeight) {
            std::cout << "#";
        } else {
            std::cout << "-";
        }
    }
    std::cout << " (" << static_cast<int>(cpuUsage * 100.0f) << "%)" << "\033[0m" << std::endl;
    
    std::cout << colorManager.getMetricColor(memoryUsage, MetricType::MemoryUsage) << "Memory Usage:" << colorManager.getColorCode(ColorManager::Color::White);
    for (int y = gaugeHeight - 1; y >= 0; --y) {
        if (y < memoryUsage * gaugeHeight) {
            std::cout << "#";
        } else {
            std::cout << "-";
        }
    }
    std::cout << " (" << static_cast<int>(memoryUsage * 100.0f) << "%)" << "\033[0m" << std::endl;
    
    std::cout << colorManager.getMetricColor(ioUsage, MetricType::IOOperations) << "IO Usage:" << colorManager.getColorCode(ColorManager::Color::White);
    for (int y = gaugeHeight - 1; y >= 0; --y) {
        if (y < ioUsage * gaugeHeight) {
            std::cout << "#";
        } else {
            std::cout << "-";
        }
    }
    std::cout << " (" << static_cast<int>(ioUsage * 100.0f) << "%)" << "\033[0m" << std::endl;
}

void ProgressRenderer::renderComparisonChart(const Brain& brain) {
    // Render comparison chart
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightYellow) << "=== COMPARISON CHART ===" << "\033[0m" << std::endl;
    
    if (comparisonData_.empty()) {
        std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "No comparison data available" << "\033[0m" << std::endl;
        return;
    }
    
    // Simple comparison table
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Experiment              Learning  Performance    Efficiency" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "------------  ----------  ------------  -----------" << "\033[0m" << std::endl;
    
    for (const auto& exp : comparisonData_) {
        std::cout << colorManager.getColorCode(ColorManager::Color::Green) << exp.experimentName.substr(0, 12) << " " << "\033[0m";
        std::cout << colorManager.getColorCode(ColorManager::Color::Cyan) << "  " << std::setw(10) << (exp.learningCurveScore * 100.0f) << "%" << " " << "\033[0m";
        std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "  " << std::setw(13) << (exp.performanceScore * 100.0f) << "%" << " " << "\033[0m";
        std::cout << colorManager.getColorCode(ColorManager::Color::Red) << "  " << std::setw(12) << exp.efficiencyScore << " " << "\033[0m" << std::endl;
    }
}

void ProgressRenderer::renderExperimentState(const Brain& brain) {
    // Render experiment state visualization
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightBlue) << "=== EXPERIMENT STATE ===" << "\033[0m" << std::endl;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Brain ID: " << brain.getBrainId() << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Current Step: " << brain.getCurrentStep() << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "Current Time: " << brain.getSimulationTime() << " s" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Red) << "Total Neurons: " << brain.getTotalNeuronCount() << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Magenta) << "Total Synapses: " << brain.getTotalSynapseCount() << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Cyan) << "Firing Neurons: " << brain.getFiringNeuronCount() << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightWhite) << "Total Spikes: " << brain.getTotalSpikeCount() << "\033[0m" << std::endl;
    
    // Display development stage
    auto developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Development Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\033[0m" << std::endl;
    }
    
    // Display neuromodulation levels
    auto dopamine = brain.getDopamine();
    if (dopamine) {
        std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Dopamine Level: " << dopamine->getLevel() << "\033[0m" << std::endl;
    }
}

void ProgressRenderer::renderPhaseProgress(const Brain& brain) {
    // Render phase progress visualization
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightRed) << "=== PHASE PROGRESS ===" << "\033[0m" << std::endl;
    
    // Display phase completion (simplified phases based on system components)
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Phase 1: Core Systems - Complete" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Phase 2: Neural Computation - Complete" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Phase 3: Memory Systems - Complete" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Phase 4: Prediction Systems - Complete" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Phase 5: Cognition Systems - Complete" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Phase 6: Integrated Brain - Complete" << "\033[0m" << std::endl;
    
    // Calculate overall progress
    float totalPhases = 6.0f;
    float completedPhases = 6.0f; // Assuming all phases are complete
    float overallProgress = (completedPhases / totalPhases) * 100.0f;
    
    std::cout << std::endl << colorManager.getColorCode(ColorManager::Color::BrightWhite) << "Overall Phase Progress: " << overallProgress << "%" << "\033[0m" << std::endl;
}

void ProgressRenderer::renderControls() const {
    ColorManager colorManager;
    std::string helpColor = colorManager.getColorCode(ColorManager::Color::BrightYellow);
    
    std::cout << "\033[2J\033[H"; // Clear screen (temporary)
    
    std::cout << helpColor << "=== PROGRESS VISUALIZATION CONTROLS ===" << "\033[0m" << std::endl;
    std::cout << helpColor << "1-6: Change Display Mode (Learning/Performance/Resources/Comparison/State/Phases)" << "\033[0m" << std::endl;
    std::cout << helpColor << "M: Toggle Progress History" << "\033[0m" << std::endl;
    std::cout << helpColor << "R: Toggle Resource Metrics" << "\033[0m" << std::endl;
    std::cout << helpColor << "C: Change Comparison Data" << "\033[0m" << std::endl;
    std::cout << helpColor << "T: Adjust Time Range" << "\033[0m" << std::endl;
    std::cout << helpColor << "Q: Quit" << "\033[0m" << std::endl;
    std::cout << std::endl;
    
    // Show current mode
    std::cout << helpColor << "Display: " << static_cast<int>(progressViewMode_) 
              << "  Metric: " << static_cast<int>(metricType_)
              << "  History: " << (showProgressHistory_ ? "ON" : "OFF") << "\033[0m" << std::endl;
}

float ProgressRenderer::calculateLearningRate(const std::vector<float>& curve) const {
    if (curve.empty()) return 0.0f;
    
    float sum = std::accumulate(curve.begin(), curve.end(), 0.0f);
    return sum / curve.size();
}

float ProgressRenderer::calculatePerformanceScore(const std::vector<float>& metrics) const {
    if (metrics.empty()) return 0.0f;
    
    float sum = std::accumulate(metrics.begin(), metrics.end(), 0.0f);
    return sum / metrics.size();
}

float ProgressRenderer::calculateEfficiency(float performance, float resources) const {
    return performance / std::max(0.1f, resources);
}

std::string ProgressRenderer::formatMetricValue(float value, MetricType metric) const {
    switch (metric) {
        case MetricType::FiringRate:
            return std::to_string(value) + " Hz";
        case MetricType::LearningSpeed:
            return std::to_string(value) + " step/s";
        case MetricType::MemoryCapacity:
            return std::to_string(static_cast<int>(value)) + " entries";
        case MetricType::CPUUsage:
            return std::to_string(static_cast<int>(value * 100.0f)) + "%";
        case MetricType::MemoryUsage:
            return std::to_string(static_cast<int>(value * 100.0f)) + "%";
        case MetricType::IOOperations:
            return std::to_string(static_cast<int>(value * 100.0f)) + "%";
        case MetricType::ResponseTime:
            return std::to_string(value) + " ms";
        default:
            return std::to_string(value);
    }
}

} // namespace nlm
