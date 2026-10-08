// Implementation file for VisualizationSystem.hpp - Core implementation

#include "VisualizationSystem.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// BrainActivityMonitor implementation
BrainActivityMonitor::BrainActivityMonitor(std::shared_ptr<AgentBrain> agent)
    : agent_(agent)
    , advancedAgent_(std::dynamic_pointer_cast<AdvancedAgentBrain>(agent))
    , monitoring_(false)
    , monitoringInterval_(0.1)
    , maxHistorySize_(10000) {
    
    if (!agent_) {
        throw AgentBrainException("Cannot create BrainActivityMonitor with null agent");
    }
    
    lastUpdateTime_ = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("BrainActivityMonitor initialized");
}

BrainActivityMonitor::~BrainActivityMonitor() {
    stopMonitoring();
    NLM_LOG_INFO("BrainActivityMonitor destroyed");
}

void BrainActivityMonitor::startMonitoring(double intervalSeconds) {
    if (monitoring_) {
        NLM_LOG_WARNING("Monitoring already started");
        return;
    }
    
    monitoringInterval_ = intervalSeconds;
    monitoring_ = true;
    monitoringThread_ = std::thread(&BrainActivityMonitor::monitoringLoop, this);
    
    NLM_LOG_INFO("BrainActivityMonitor started with interval: " + std::to_string(intervalSeconds) + "s");
}

void BrainActivityMonitor::stopMonitoring() {
    if (!monitoring_) {
        return;
    }
    
    monitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    NLM_LOG_INFO("BrainActivityMonitor stopped");
}

bool BrainActivityMonitor::isMonitoring() const {
    return monitoring_;
}

NeuralActivityData BrainActivityMonitor::getCurrentNeuralActivity() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    NeuralActivityData data;
    if (!neuralActivityHistory_.empty()) {
        data = neuralActivityHistory_.back();
    }
    
    // Extract current neural activity from agent
    if (agent_) {
        // In a real implementation, this would extract data from the agent's brain
        data.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now() - lastUpdateTime_).count();
        data.stepNumber = agent_->getBrain()->getTotalStepCount();
        
        // Get neuron activities
        size_t totalNeurons = agent_->getBrain()->getTotalNeuronCount();
        data.neuronActivities.resize(totalNeurons, 0.0f);
        
        for (size_t i = 0; i < totalNeurons; ++i) {
            // This is a simplified extraction - in real implementation would query brain state
            data.neuronActivities[i] = static_cast<float>(i % 100) / 100.0f;
        }
        
        // Get synaptic weights
        for (const auto& region : agent_->getBrain()->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto& synapse : pop->getSynapses()) {
                    data.synapticWeights.push_back(synapse->getWeight());
                }
            }
        }
        
        // Get connections (simplified)
        data.connections.resize(data.neuronActivities.size(), std::vector<float>(data.neuronActivities.size(), 0.0f));
    }
    
    return data;
}

SimulationMetrics BrainActivityMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    SimulationMetrics metrics;
    if (!metricsHistory_.empty()) {
        metrics = metricsHistory_.back();
    }
    
    // Extract current metrics from agent
    if (advancedAgent_) {
        metrics.elapsedTime = advancedAgent_->getVariable("simulation_time");
        metrics.stepsCompleted = advancedAgent_->getVariable("simulation_steps");
        metrics.averageFiringRate = advancedAgent_->getVariable("average_firing_rate");
        metrics.totalReward = advancedAgent_->getVariable("total_reward");
        metrics.predictionError = advancedAgent_->getVariable("prediction_error");
        metrics.curiosityLevel = advancedAgent_->getVariable("curiosity_level");
        metrics.noveltyLevel = advancedAgent_->getVariable("novelty_level");
        metrics.dopamineLevel = advancedAgent_->getVariable("dopamine_level");
        metrics.energyLevel = advancedAgent_->getVariable("energy_level");
        metrics.healthLevel = advancedAgent_->getVariable("health_level");
    }
    
    return metrics;
}

std::vector<NeuralActivityData> BrainActivityMonitor::getNeuralActivityHistory(size_t maxPoints) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    size_t count = std::min(neuralActivityHistory_.size(), maxPoints);
    return std::vector<NeuralActivityData>(neuralActivityHistory_.end() - count, neuralActivityHistory_.end());
}

std::vector<SimulationMetrics> BrainActivityMonitor::getMetricsHistory(size_t maxPoints) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    size_t count = std::min(metricsHistory_.size(), maxPoints);
    return std::vector<SimulationMetrics>(metricsHistory_.end() - count, metricsHistory_.end());
}

void BrainActivityMonitor::setActivityCallback(std::function<void(const NeuralActivityData&)> callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    activityCallback_ = std::move(callback);
}

void BrainActivityMonitor::setMetricsCallback(std::function<void(const SimulationMetrics&)> callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    metricsCallback_ = std::move(callback);
}

void BrainActivityMonitor::setActivityFilter(std::function<bool(const NeuralActivityData&)> filter) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    activityFilter_ = std::move(filter);
}

void BrainActivityMonitor::setMetricsFilter(std::function<bool(const SimulationMetrics&)> filter) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    metricsFilter_ = std::move(filter);
}

std::map<std::string, double> BrainActivityMonitor::computeStatistics(const NeuralActivityData& data) const {
    std::map<std::string, double> stats;
    
    if (data.neuronActivities.empty()) {
        return stats;
    }
    
    double sum = std::accumulate(data.neuronActivities.begin(), data.neuronActivities.end(), 0.0);
    double mean = sum / data.neuronActivities.size();
    
    double sqSum = 0.0;
    for (float val : data.neuronActivities) {
        double diff = val - mean;
        sqSum += diff * diff;
    }
    
    double stddev = std::sqrt(sqSum / data.neuronActivities.size());
    
    double min = *std::min_element(data.neuronActivities.begin(), data.neuronActivities.end());
    double max = *std::max_element(data.neuronActivities.begin(), data.neuronActivities.end());
    
    stats["mean"] = mean;
    stats["stddev"] = stddev;
    stats["min"] = min;
    stats["max"] = max;
    stats["range"] = max - min;
    
    return stats;
}

std::vector<double> BrainActivityMonitor::detectPatterns(const NeuralActivityData& data) const {
    std::vector<double> patterns;
    
    if (data.neuronActivities.size() < 10) {
        return patterns;
    }
    
    // Simple pattern detection - in real implementation would use more sophisticated algorithms
    for (size_t i = 0; i < data.neuronActivities.size() - 9; ++i) {
        std::vector<double> window(data.neuronActivities.begin() + i,
                                  data.neuronActivities.begin() + i + 10);
        
        // Compute some simple statistics
        double mean = std::accumulate(window.begin(), window.end(), 0.0) / window.size();
        double variance = 0.0;
        for (double val : window) {
            double diff = val - mean;
            variance += diff * diff;
        }
        variance /= window.size();
        
        patterns.push_back(mean);
        patterns.push_back(variance);
    }
    
    return patterns;
}

AnalysisResult BrainActivityMonitor::performDeepAnalysis(size_t windowSize) const {
    AnalysisResult result;
    result.analysisType = "deep_analysis";
    result.description = "Deep analysis of neural activity patterns";
    result.confidence = 0.0;
    
    if (neuralActivityHistory_.size() < windowSize) {
        result.insights.push_back("Insufficient data for deep analysis");
        return result;
    }
    
    // Get recent data window
    size_t startIdx = neuralActivityHistory_.size() - windowSize;
    std::vector<NeuralActivityData> recentData(
        neuralActivityHistory_.begin() + startIdx, neuralActivityHistory_.end());
    
    // Analyze neural dynamics
    std::map<std::string, double> dynamicsMetrics;
    double totalActivity = 0.0;
    
    for (const auto& data : recentData) {
        auto stats = computeStatistics(data);
        totalActivity += stats["mean"];
        
        dynamicsMetrics["avg_mean"] += stats["mean"];
        dynamicsMetrics["avg_stddev"] += stats["stddev"];
        dynamicsMetrics["avg_range"] += stats["range"];
    }
    
    size_t count = recentData.size();
    for (auto& pair : dynamicsMetrics) {
        pair.second /= count;
    }
    
    result.metrics["neural_dynamics"] = dynamicsMetrics;
    result.metrics["total_activity"] = totalActivity;
    result.metrics["activity_variance"] = 0.0; // Would compute variance across time
    
    // Analyze learning patterns
    std::vector<double> learningTrend;
    for (size_t i = 1; i < recentData.size(); ++i) {
        double trend = recentData[i].neuronActivities.size() > 0 ? 
                      recentData[i].neuronActivities.back() - recentData[i-1].neuronActivities.back() : 0.0;
        learningTrend.push_back(trend);
    }
    
    if (!learningTrend.empty()) {
        double avgTrend = std::accumulate(learningTrend.begin(), learningTrend.end(), 0.0) / learningTrend.size();
        result.metrics["learning_trend"] = avgTrend;
        
        if (avgTrend > 0.01) {
            result.insights.push_back("Neural activity showing increasing trend - learning progressing");
        } else if (avgTrend < -0.01) {
            result.insights.push_back("Neural activity showing decreasing trend - potential forgetting or stabilization");
        } else {
            result.insights.push_back("Neural activity stable - system in equilibrium");
        }
    }
    
    // Detect bursts of activity
    std::vector<size_t> burstIndices;
    for (size_t i = 1; i < recentData.size(); ++i) {
        if (recentData[i].neuronActivities.size() > 0 &&
            recentData[i].neuronActivities.back() > 0.5) {
            burstIndices.push_back(i);
        }
    }
    
    if (!burstIndices.empty()) {
        result.metrics["burst_count"] = static_cast<double>(burstIndices.size());
        result.insights.push_back("Detected " + std::to_string(burstIndices.size()) + 
                                 " bursts of high neural activity");
    }
    
    // Generate recommendations
    if (result.metrics["learning_trend"] > 0.01) {
        result.recommendations.push_back("Increase difficulty to challenge learning further");
    } else if (result.metrics["learning_trend"] < -0.01) {
        result.recommendations.push_back("Provide reinforcement to stabilize learning");
    }
    
    result.recommendations.push_back("Continue monitoring for longer-term patterns");
    
    // Calculate confidence based on data quality
    result.confidence = std::min(1.0, static_cast<double>(count) / 100.0);
    
    return result;
}

void BrainActivityMonitor::monitoringLoop() {
    NLM_LOG_INFO("BrainActivityMonitor monitoring loop started");
    
    while (monitoring_) {
        try {
            auto startTime = std::chrono::steady_clock::now();
            
            // Update neural activity data
            updateNeuralActivity();
            
            // Update metrics
            updateMetrics();
            
            // Invoke callbacks
            {
                std::lock_guard<std::mutex> lock(dataMutex_);
                if (activityCallback_ && !neuralActivityHistory_.empty()) {
                    activityCallback_(neuralActivityHistory_.back());
                }
                if (metricsCallback_ && !metricsHistory_.empty()) {
                    metricsCallback_(metricsHistory_.back());
                }
            }
            
            // Apply filters
            {
                std::lock_guard<std::mutex> lock(dataMutex_);
                if (activityFilter_) {
                    // Filter neural activity history
                    auto it = std::remove_if(neuralActivityHistory_.begin(), neuralActivityHistory_.end(),
                                           [&](const NeuralActivityData& data) { return !activityFilter_(data); });
                    neuralActivityHistory_.erase(it, neuralActivityHistory_.end());
                }
                
                if (metricsFilter_) {
                    // Filter metrics history
                    auto it = std::remove_if(metricsHistory_.begin(), metricsHistory_.end(),
                                           [&](const SimulationMetrics& metrics) { return !metricsFilter_(metrics); });
                    metricsHistory_.erase(it, metricsHistory_.end());
                }
                
                // Trim history if too large
                if (neuralActivityHistory_.size() > maxHistorySize_) {
                    neuralActivityHistory_.erase(neuralActivityHistory_.begin(),
                                               neuralActivityHistory_.begin() + 
                                               (neuralActivityHistory_.size() - maxHistorySize_));
                }
                
                if (metricsHistory_.size() > maxHistorySize_) {
                    metricsHistory_.erase(metricsHistory_.begin(),
                                        metricsHistory_.begin() + 
                                        (metricsHistory_.size() - maxHistorySize_));
                }
            }
            
            auto endTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            // Sleep for the remaining time
            auto sleepTime = monitoringInterval_ - elapsed;
            if (sleepTime > 0.0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error in monitoring loop: ") + e.what());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    NLM_LOG_INFO("BrainActivityMonitor monitoring loop stopped");
}

void BrainActivityMonitor::updateNeuralActivity() {
    NeuralActivityData data = getCurrentNeuralActivity();
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    neuralActivityHistory_.push_back(data);
}

void BrainActivityMonitor::updateMetrics() {
    SimulationMetrics metrics = getCurrentMetrics();
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    metricsHistory_.push_back(metrics);
}

// AdvancedDataPlotter implementation (simplified)
AdvancedDataPlotter::AdvancedDataPlotter() : zoomEnabled_(false), panEnabled_(false), selectionEnabled_(false) {}

AdvancedDataPlotter::~AdvancedDataPlotter() {}

bool AdvancedDataPlotter::initialize(const std::string& backend) {
    // Simplified initialization - would set up actual plotting backend
    NLM_LOG_INFO("AdvancedDataPlotter initialized with backend: " + backend);
    return true;
}

void AdvancedDataPlotter::plotNeuronsActivity(const std::vector<float>& activities, const std::string& title) {
    plotData_[title + "_neurons"] = std::vector<std::pair<double, double>>();
    for (size_t i = 0; i < activities.size(); ++i) {
        plotData_[title + "_neurons"].push_back(std::make_pair(static_cast<double>(i), activities[i]));
    }
    
    NLM_LOG_INFO("Plotted neurons activity with " + std::to_string(activities.size()) + " points");
}

void AdvancedDataPlotter::plotSynapticWeights(const std::vector<float>& weights, const std::string& title) {
    plotData_[title + "_weights"] = std::vector<std::pair<double, double>>();
    for (size_t i = 0; i < weights.size(); ++i) {
        plotData_[title + "_weights"].push_back(std::make_pair(static_cast<double>(i), weights[i]));
    }
    
    NLM_LOG_INFO("Plotted synaptic weights with " + std::to_string(weights.size()) + " points");
}

void AdvancedDataPlotter::plotConnections(const std::vector<std::vector<float>>& connections, const std::string& title) {
    // Simplified connection plotting
    plotData_[title + "_connections"] = std::vector<std::pair<double, double>>();
    NLM_LOG_INFO("Plotted connections with " + std::to_string(connections.size()) + " connection matrices");
}

void AdvancedDataPlotter::plotMetrics(const SimulationMetrics& metrics, const std::string& title) {
    plotData_[title + "_metrics"] = std::vector<std::pair<double, double>>();
    plotData_[title + "_metrics"].push_back(std::make_pair(0.0, metrics.averageFiringRate));
    plotData_[title + "_metrics"].push_back(std::make_pair(1.0, metrics.totalReward));
    plotData_[title + "_metrics"].push_back(std::make_pair(2.0, metrics.predictionError));
    plotData_[title + "_metrics"].push_back(std::make_pair(3.0, metrics.curiosityLevel));
    
    NLM_LOG_INFO("Plotted metrics with " + std::to_string(plotData_[title + "_metrics"].size()) + " data points");
}

void AdvancedDataPlotter::plotTimeSeries(const std::map<std::string, std::vector<double>>& timeSeries, const std::string& title) {
    for (const auto& series : timeSeries) {
        plotData_[title + "_" + series.first] = std::vector<std::pair<double, double>>();
        for (size_t i = 0; i < series.second.size(); ++i) {
            plotData_[title + "_" + series.first].push_back(std::make_pair(static_cast<double>(i), series.second[i]));
        }
    }
    
    NLM_LOG_INFO("Plotted time series with " + std::to_string(timeSeries.size()) + " series");
}

void AdvancedDataPlotter::plot3DNeuralNetwork(const NeuralActivityData& neuralData, const std::string& title) {
    // Simplified 3D plotting
    NLM_LOG_INFO("Plotted 3D neural network with " + std::to_string(neuralData.neuronActivities.size()) + " neurons");
}

void AdvancedDataPlotter::createDashboard(const std::map<std::string, std::vector<std::pair<double, double>>>& plots) {
    plotData_.insert(plots.begin(), plots.end());
    NLM_LOG_INFO("Created dashboard with " + std::to_string(plots.size()) + " plots");
}

void AdvancedDataPlotter::createTimeSeriesDashboard(const std::vector<std::string>& metrics, double duration) {
    NLM_LOG_INFO("Created time series dashboard with " + std::to_string(metrics.size()) + " metrics over " + std::to_string(duration) + "s");
}

bool AdvancedDataPlotter::exportPlots(const std::string& directory, const std::string& format) {
    NLM_LOG_INFO("Exporting plots to " + directory + " in format: " + format);
    return true; // Simplified - would actually export
}

bool AdvancedDataPlotter::exportDashboard(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("Exporting dashboard to " + filename + " in format: " + format);
    return true; // Simplified - would actually export
}

void AdvancedDataPlotter::enableZoom(bool enable) {
    zoomEnabled_ = enable;
}

void AdvancedDataPlotter::enablePan(bool enable) {
    panEnabled_ = enable;
}

void AdvancedDataPlotter::enableSelection(bool enable) {
    selectionEnabled_ = enable;
}

void AdvancedDataPlotter::setPlotClickCallback(std::function<void(const std::string&, size_t)> callback) {
    clickCallback_ = std::move(callback);
}

void AdvancedDataPlotter::setPlotHoverCallback(std::function<void(const std::string&, double)> callback) {
    hoverCallback_ = std::move(callback);
}

} // namespace nlm
