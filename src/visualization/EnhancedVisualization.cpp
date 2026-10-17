#include "EnhancedVisualization.hpp"
#include "../brain/Brain.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <chrono>

namespace nlm {

// RealTimeDataCollector implementation
RealTimeDataCollector::RealTimeDataCollector() : currentTime_(0.0) {
}

RealTimeDataCollector::~RealTimeDataCollector() = default;

void RealTimeDataCollector::setBrain(std::shared_ptr<Brain> brain) {
    brain_ = brain;
}

void RealTimeDataCollector::setSimulationClock(std::shared_ptr<SimulationClock> clock) {
    clock_ = clock;
}

void RealTimeDataCollector::update(float deltaTime) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    currentTime_ += deltaTime;
    
    collectTimeSeriesData();
    collectNetworkActivity();
    collectPlasticityMetrics();
    collectMemoryMetrics();
    
    if (dataUpdateCallback_) {
        dataUpdateCallback_();
    }
}

void RealTimeDataCollector::collectTimeSeriesData() {
    if (!brain_ || !clock_) return;
    
    double timestamp = clock_->getCurrentTime();
    
    // Collect firing rate data
    size_t totalSpikeCount = brain_->getTotalSpikeCount();
    size_t firingNeuronCount = brain_->getFiringNeuronCount();
    
    double firingRate = (firingNeuronCount > 0) ? 
        (static_cast<double>(totalSpikeCount) / firingNeuronCount) : 0.0;
    
    timeSeriesData_["firing_rate"].addPoint(timestamp, firingRate);
    timeSeriesData_["firing_rate"].name = "Firing Rate";
    timeSeriesData_["firing_rate"].units = "Hz";
    timeSeriesData_["firing_rate"].color = "#00FF00";
    
    // Collect membrane potential data (from first region if available)
    if (auto* region = brain_->getRegion(RegionId(1))) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            double totalMembranePotential = 0.0;
            size_t validNeurons = 0;
            
            for (auto* neuron : neurons) {
                if (neuron && neuron->getMembranePotential() > -100.0f) {
                    totalMembranePotential += neuron->getMembranePotential();
                    validNeurons++;
                }
            }
            
            if (validNeurons > 0) {
                double avgMembranePotential = totalMembranePotential / validNeurons;
                timeSeriesData_["membrane_potential"].addPoint(timestamp, avgMembranePotential);
                timeSeriesData_["membrane_potential"].name = "Membrane Potential";
                timeSeriesData_["membrane_potential"].units = "mV";
                timeSeriesData_["membrane_potential"].color = "#FF00FF";
            }
        }
    }
    
    // Collect synaptic weight data
    if (auto* region = brain_->getRegion(RegionId(1))) {
        double totalWeight = 0.0;
        size_t synapseCount = 0;
        
        for (auto& syn : region->getSynapses()) {
            totalWeight += syn->getWeight();
            synapseCount++;
        }
        
        if (synapseCount > 0) {
            double avgWeight = totalWeight / synapseCount;
            timeSeriesData_["synaptic_weight"].addPoint(timestamp, avgWeight);
            timeSeriesData_["synaptic_weight"].name = "Average Synaptic Weight";
            timeSeriesData_["synaptic_weight"].units = "normalized";
            timeSeriesData_["synaptic_weight"].color = "#FFFF00";
        }
    }
}

void RealTimeDataCollector::collectNetworkActivity() {
    if (!brain_) return;
    
    networkActivity_.firingRates.clear();
    networkActivity_.membranePotentials.clear();
    networkActivity_.synapticWeights.clear();
    networkActivity_.neuronTypes.clear();
    networkActivity_.regionIds.clear();
    
    for (auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        for (auto* neuron : neurons) {
            if (neuron) {
                networkActivity_.firingRates.push_back(1.0f); // Placeholder - would get actual firing rate
                networkActivity_.membranePotentials.push_back(neuron->getMembranePotential());
                networkActivity_.neuronTypes.push_back("Internal"); // Placeholder
                networkActivity_.regionIds.push_back(region->getId());
            }
        }
    }
}

void RealTimeDataCollector::collectPlasticityMetrics() {
    plasticityMetrics_.hebbianWeightChange = 0.0;
    plasticityMetrics_.stdpWeightChange = 0.0;
    plasticityMetrics_.structuralPlasticityRate = 0.0;
    plasticityMetrics_.learningEfficiency = 0.0;
    plasticityMetrics_.weightStability = 0.0;
    
    // Collect plasticity metrics from brain components
    // This would be implemented based on available brain components
}

void RealTimeDataCollector::collectMemoryMetrics() {
    memoryMetrics_.workingMemorySize = 0;
    memoryMetrics_.episodicMemoryCount = 0;
    memoryMetrics_.semanticMemoryCount = 0;
    memoryMetrics_.proceduralMemoryCount = 0;
    memoryMetrics_.memoryConsolidationRate = 0.0;
    memoryMetrics_.retrievalAccuracy = 0.0;
    
    // Collect memory metrics from brain components
    // This would be implemented based on available brain components
}

const TimeSeriesData& RealTimeDataCollector::getTimeSeriesData(const std::string& name) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto it = timeSeriesData_.find(name);
    static TimeSeriesData emptyData;
    return (it != timeSeriesData_.end()) ? it->second : emptyData;
}

const NetworkActivity& RealTimeDataCollector::getNetworkActivity() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return networkActivity_;
}

const PlasticityMetrics& RealTimeDataCollector::getPlasticityMetrics() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return plasticityMetrics_;
}

const MemoryMetrics& RealTimeDataCollector::getMemoryMetrics() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return memoryMetrics_;
}

void RealTimeDataCollector::clearData() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    timeSeriesData_.clear();
    networkActivity_ = NetworkActivity();
    plasticityMetrics_ = PlasticityMetrics();
    memoryMetrics_ = MemoryMetrics();
}

void RealTimeDataCollector::saveData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return;
    
    file << "TimeSeriesData:" << std::endl;
    for (const auto& pair : timeSeriesData_) {
        file << "  " << pair.first << ":" << std::endl;
        file << "    name: " << pair.second.name << std::endl;
        file << "    units: " << pair.second.units << std::endl;
        file << "    color: " << pair.second.color << std::endl;
        file << "    data:" << std::endl;
        for (size_t i = 0; i < pair.second.timestamps.size(); ++i) {
            file << "      " << pair.second.timestamps[i] << ", " 
                 << pair.second.values[i] << std::endl;
        }
    }
    
    file.close();
}

bool RealTimeDataCollector::loadData(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    // Implementation would load data from file
    // For now, return false
    
    file.close();
    return true;
}

void RealTimeDataCollector::setDataUpdateCallback(std::function<void()> callback) {
    dataUpdateCallback_ = callback;
}

// RealTimeDataVisualizer implementation
RealTimeDataVisualizer::RealTimeDataVisualizer() : 
    windowWidth_(1024), windowHeight_(768), updateFrequency_(60.0) {
    showTimeSeriesView_ = true;
    showNetworkView_ = true;
    showMetricsView_ = true;
    showControlsView_ = true;
    
    lastUpdateTime_ = std::chrono::steady_clock::now();
}

RealTimeDataVisualizer::~RealTimeDataVisualizer() = default;

void RealTimeDataVisualizer::initialize() {
    // Initialize visualization system
    // This would set up windows, renderers, etc.
}

void RealTimeDataVisualizer::shutdown() {
    // Shutdown visualization system
}

void RealTimeDataVisualizer::update(float deltaTime) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime_);
    
    if (elapsed.count() >= 1000.0 / updateFrequency_) {
        // Update visualization based on current data
        if (dataCollector_) {
            dataCollector_->update(deltaTime);
        }
        
        lastUpdateTime_ = currentTime;
    }
}

void RealTimeDataVisualizer::plotTimeSeries(const std::string& title, const TimeSeriesData& data) {
    // Implementation would plot time series data
}

void RealTimeDataVisualizer::plotMultipleTimeSeries(const std::string& title, 
                                                   const std::vector<TimeSeriesData>& datasets) {
    // Implementation would plot multiple time series
}

void RealTimeDataVisualizer::renderNetworkGraph(const NetworkActivity& activity) {
    // Implementation would render network graph
}

void RealTimeDataVisualizer::highlightActiveNeurons(const std::vector<size_t>& neuronIds) {
    // Implementation would highlight specific neurons
}

void RealTimeDataVisualizer::showSynapseWeights(const NetworkActivity& activity) {
    // Implementation would show synapse weights
}

void RealTimeDataVisualizer::updateMetricsDashboard(const PlasticityMetrics& plasticity,
                                                    const MemoryMetrics& memory) {
    // Implementation would update metrics dashboard
}

void RealTimeDataVisualizer::showPerformanceMetrics(const std::string& title,
                                                    double fps, double memoryUsage, 
                                                    double cpuUsage) {
    // Implementation would show performance metrics
}

void RealTimeDataVisualizer::showSimulationControls(bool& running, float& timeScale) {
    // Implementation would show simulation controls
}

void RealTimeDataVisualizer::showBrainParameters(const std::map<std::string, std::string>& parameters) {
    // Implementation would show brain parameters
}

void RealTimeDataVisualizer::setDataCollector(std::shared_ptr<RealTimeDataCollector> collector) {
    dataCollector_ = collector;
}

void RealTimeDataVisualizer::setWindowTitle(const std::string& title) {
    windowTitle_ = title;
}

void RealTimeDataVisualizer::setWindowSize(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;
}

void RealTimeDataVisualizer::setUpdateFrequency(double frequency) {
    updateFrequency_ = frequency;
}

void RealTimeDataVisualizer::exportData(const std::string& filepath) {
    if (dataCollector_) {
        dataCollector_->saveData(filepath);
    }
}

void RealTimeDataVisualizer::generateReport(const std::string& filepath) {
    if (dataCollector_) {
        // Generate analysis report
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "NLM Real-Time Visualization Report" << std::endl;
            file << "=================================" << std::endl;
            file << std::endl;
            
            // Add report data
            file.close();
        }
    }
}

// AnalysisTools implementation
AnalysisTools::AnalysisTools() = default;

AnalysisTools::~AnalysisTools() = default;

void AnalysisTools::setDataCollector(std::shared_ptr<RealTimeDataCollector> collector) {
    dataCollector_ = collector;
}

double AnalysisTools::calculateMean(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    double sum = 0.0;
    for (double value : data) {
        sum += value;
    }
    return sum / data.size();
}

double AnalysisTools::calculateStandardDeviation(const std::vector<double>& data) const {
    if (data.size() <= 1) return 0.0;
    
    double mean = calculateMean(data);
    double sumOfSquares = 0.0;
    for (double value : data) {
        sumOfSquares += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sumOfSquares / (data.size() - 1));
}

double AnalysisTools::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() <= 1) return 0.0;
    
    double xMean = calculateMean(x);
    double yMean = calculateMean(y);
    
    double numerator = 0.0;
    double denominatorX = 0.0;
    double denominatorY = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        double xDiff = x[i] - xMean;
        double yDiff = y[i] - yMean;
        numerator += xDiff * yDiff;
        denominatorX += xDiff * xDiff;
        denominatorY += yDiff * yDiff;
    }
    
    if (denominatorX == 0.0 || denominatorY == 0.0) return 0.0;
    
    return numerator / std::sqrt(denominatorX * denominatorY);
}

std::vector<size_t> AnalysisTools::detectActivityPatterns(const NetworkActivity& activity) const {
    std::vector<size_t> patternIndices;
    
    if (activity.firingRates.empty()) return patternIndices;
    
    double meanFiringRate = calculateMean(activity.firingRates);
    double stdDevFiringRate = calculateStandardDeviation(activity.firingRates);
    
    // Detect neurons with above-average firing rates
    for (size_t i = 0; i < activity.firingRates.size(); ++i) {
        if (activity.firingRates[i] > meanFiringRate + stdDevFiringRate) {
            patternIndices.push_back(i);
        }
    }
    
    return patternIndices;
}

double AnalysisTools::detectLearningTrends(const TimeSeriesData& weightData) const {
    if (weightData.values.size() < 2) return 0.0;
    
    // Simple linear regression to detect trends
    double xMean = calculateMean(weightData.timestamps);
    double yMean = calculateMean(weightData.values);
    
    double numerator = 0.0;
    double denominator = 0.0;
    
    for (size_t i = 0; i < weightData.timestamps.size(); ++i) {
        double xDiff = weightData.timestamps[i] - xMean;
        double yDiff = weightData.values[i] - yMean;
        numerator += xDiff * yDiff;
        denominator += xDiff * xDiff;
    }
    
    if (denominator == 0.0) return 0.0;
    
    return numerator / denominator; // Slope
}

std::vector<std::string> AnalysisTools::detectAnomalies(const std::vector<double>& metrics) const {
    std::vector<std::string> anomalies;
    
    if (metrics.empty()) return anomalies;
    
    double mean = calculateMean(metrics);
    double stdDev = calculateStandardDeviation(metrics);
    
    for (size_t i = 0; i < metrics.size(); ++i) {
        if (std::abs(metrics[i] - mean) > 2.0 * stdDev) {
            anomalies.push_back("Metric " + std::to_string(i) + " is anomalous: " + 
                             std::to_string(metrics[i]));
        }
    }
    
    return anomalies;
}

double AnalysisTools::calculatePerformanceScore(const PlasticityMetrics& plasticity,
                                                const MemoryMetrics& memory) const {
    double score = 0.0;
    
    // Normalize and combine metrics
    score += plasticity.learningEfficiency * 0.3;
    score += (1.0 - plasticity.weightStability) * 0.2; // Lower stability is worse
    score += memory.memoryConsolidationRate * 0.3;
    score += memory.retrievalAccuracy * 0.2;
    
    return std::min(1.0, score);
}

std::string AnalysisTools::analyzeMemoryEfficiency(const MemoryMetrics& memory) const {
    std::stringstream ss;
    ss << "Memory Efficiency Analysis:" << std::endl;
    ss << "  Working Memory Usage: " << memory.workingMemorySize << std::endl;
    ss << "  Episodic Memory: " << memory.episodicMemoryCount << " entries" << std::endl;
    ss << "  Semantic Memory: " << memory.semanticMemoryCount << " facts" << std::endl;
    ss << "  Procedural Memory: " << memory.proceduralMemoryCount << " skills" << std::endl;
    ss << "  Consolidation Rate: " << memory.memoryConsolidationRate << std::endl;
    ss << "  Retrieval Accuracy: " << memory.retrievalAccuracy << std::endl;
    
    return ss.str();
}

std::string AnalysisTools::analyzePlasticityStability(const PlasticityMetrics& plasticity) const {
    std::stringstream ss;
    ss << "Plasticity Stability Analysis:" << std::endl;
    ss << "  Hebbian Weight Change: " << plasticity.hebbianWeightChange << std::endl;
    ss << "  STDP Weight Change: " << plasticity.stdpWeightChange << std::endl;
    ss << "  Structural Plasticity Rate: " << plasticity.structuralPlasticityRate << std::endl;
    ss << "  Learning Efficiency: " << plasticity.learningEfficiency << std::endl;
    ss << "  Weight Stability: " << plasticity.weightStability << std::endl;
    
    return ss.str();
}

void AnalysisTools::exportAnalysisResults(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return;
    
    // Export analysis results
    // This would include all analysis data
    
    file.close();
}

std::string AnalysisTools::generateAnalysisReport() const {
    std::stringstream ss;
    ss << "NLM Analysis Report" << std::endl;
    ss << "====================" << std::endl;
    ss << std::endl;
    ss << "This report contains analysis results from the NLM simulation." << std::endl;
    ss << "Analysis includes performance metrics, memory efficiency, and plasticity stability." << std::endl;
    
    return ss.str();
}

// VisualizationUtils implementation
std::map<std::string, std::string> VisualizationUtils::neuronTypeColors_ = {
    {"Internal", "#FF0000"},
    {"Sensory", "#00FF00"},
    {"Motor", "#0000FF"},
    {"Interneuron", "#FFFF00"},
    {"Glial", "#FF00FF"}
};

std::string VisualizationUtils::interpolateColor(const std::string& color1, const std::string& color2, 
                                                  double factor) {
    // Simple color interpolation
    return color1; // Placeholder
}

std::string VisualizationUtils::getColorForValue(double value, double min, double max) {
    if (max == min) return "#808080";
    
    double normalized = (value - min) / (max - min);
    normalized = std::max(0.0, std::min(1.0, normalized));
    
    // Map normalized value to a color gradient (red -> yellow -> green)
    if (normalized < 0.5) {
        return "#FF" + std::string((int)(510 * normalized)) + "00"; // Red to Yellow
    } else {
        return "#" + std::string((int)(510 * (1 - normalized))) + "FF00"; // Yellow to Green
    }
}

std::string VisualizationUtils::getColorForNeuronType(const std::string& neuronType) {
    auto it = neuronTypeColors_.find(neuronType);
    if (it != neuronTypeColors_.end()) {
        return it->second;
    }
    return "#808080"; // Default gray
}

std::string VisualizationUtils::formatTime(double seconds) {
    std::stringstream ss;
    int hours = static_cast<int>(seconds / 3600);
    int minutes = static_cast<int>((seconds - hours * 3600) / 60);
    int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);
    
    ss << hours << ":" << std::setfill('0') << std::setw(2) << minutes << ":" 
       << std::setfill('0') << std::setw(2) << secs;
    
    return ss.str();
}

std::string VisualizationUtils::formatValue(double value, const std::string& units) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << value << " " << units;
    return ss.str();
}

std::string VisualizationUtils::formatPercentage(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value << "%";
    return ss.str();
}

double VisualizationUtils::normalizeValue(double value, double min, double max) {
    if (max == min) return 0.5;
    return (value - min) / (max - min);
}

double VisualizationUtils::clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
}

std::vector<std::pair<size_t, size_t>> VisualizationUtils::computeNetworkLayout(const NetworkActivity& activity) {
    std::vector<std::pair<size_t, size_t>> connections;
    
    // Compute network layout based on activity
    // This would implement graph layout algorithms
    
    return connections;
}

double VisualizationUtils::calculateNodePosition(size_t nodeId, const NetworkActivity& activity) {
    // Calculate node position for visualization
    return 0.0; // Placeholder
}

} // namespace nlm