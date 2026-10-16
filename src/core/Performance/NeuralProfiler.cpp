#include "NeuralProfiler.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

NeuralProfiler::NeuralProfiler() : isRunning_(false), totalNeurons_(0), totalSynapses_(0),
    activeNeurons_(0), firingNeurons_(0), averageNetworkActivity_(0.0f) {
    lastMonitoringTime_ = std::chrono::system_clock::now();
}

NeuralProfiler::~NeuralProfiler() {
    stop();
}

void NeuralProfiler::start() {
    if (!isRunning_) {
        isRunning_ = true;
        updateNeuralMetrics(0, 0, 0, 0, 0.0f);
        
        // Start monitoring thread
        monitoringThread_ = std::thread(&NeuralProfiler::monitorNeuralSystem, this);
        
        NLM_LOG_INFO("Neural profiler started");
    }
}

void NeuralProfiler::stop() {
    if (isRunning_) {
        isRunning_ = false;
        
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        NLM_LOG_INFO("Neural profiler stopped");
    }
}

bool NeuralProfiler::isRunning() const {
    return isRunning_.load();
}

void NeuralProfiler::recordNeuronPerformance(const std::string& neuronId, const std::string& region,
                                            const std::string& population, const std::chrono::nanoseconds& duration,
                                            size_t spikeCount, float firingRate, float membranePotential) {
    std::lock_guard<std::mutex> lock(neuronMutex_);
    
    if (neuronPerformances_.find(neuronId) == neuronPerformances_.end()) {
        neuronPerformances_[neuronId] = NeuronPerformance(neuronId, region, population);
    }
    
    auto& neuron = neuronPerformances_[neuronId];
    neuron.totalExecutionTime += duration;
    neuron.spikeProcessingTime += duration * spikeCount; // Approximate spike processing time
    neuron.synapticInputTime += duration * 0.1; // Estimate synaptic input time
    neuron.membranePotentialTime += duration * 0.2; // Estimate membrane potential time
    neuron.refractoryTime += duration * 0.05; // Estimate refractory time
    neuron.spikeCount += spikeCount;
    
    // Update firing rate statistics
    if (neuron.spikeCount > 0) {
        neuron.averageFiringRate = neuron.averageFiringRate + (firingRate - neuron.averageFiringRate) / neuron.spikeCount;
    }
    neuron.maxFiringRate = std::max(neuron.maxFiringRate, firingRate);
    
    // Update membrane potential statistics
    neuron.averageMembranePotential = neuron.averageMembranePotential + (membranePotential - neuron.averageMembranePotential) / (neuron.spikeCount + 1);
    neuron.maxMembranePotential = std::max(neuron.maxMembranePotential, membranePotential);
    neuron.minMembranePotential = std::min(neuron.minMembranePotential, membranePotential);
    
    neuron.lastUpdateTime = std::chrono::system_clock::now();
    
    // Clean up old neuron profiles if needed
    if (neuronPerformances_.size() > config_.maxNeuronProfiles) {
        // Remove least recently updated neurons
        auto it = neuronPerformances_.begin();
        std::advance(it, neuronPerformances_.size() - config_.maxNeuronProfiles);
        neuronPerformances_.erase(it, neuronPerformances_.end());
    }
}

void NeuralProfiler::recordSynapsePerformance(const std::string& synapseId, const std::string& sourceNeuron,
                                             const std::string& destinationNeuron, const std::chrono::nanoseconds& duration,
                                             float weightChange, float efficacy) {
    std::lock_guard<std::mutex> lock(synapseMutex_);
    
    if (synapsePerformances_.find(synapseId) == synapsePerformances_.end()) {
        synapsePerformances_[synapseId] = SynapsePerformance(synapseId, sourceNeuron, destinationNeuron);
    }
    
    auto& synapse = synapsePerformances_[synapseId];
    synapse.totalExecutionTime += duration;
    synapse.transmissionTime += duration * 0.6; // Estimate transmission time
    synapse.plasticityUpdateTime += duration * 0.3; // Estimate plasticity update time
    synapse.eligibilityTraceTime += duration * 0.1; // Estimate eligibility trace time
    synapse.spikeTransmissionCount++;
    
    // Update weight change statistics
    synapse.averageWeightChange = synapse.averageWeightChange + (weightChange - synapse.averageWeightChange) / synapse.spikeTransmissionCount;
    synapse.maxWeightChange = std::max(synapse.maxWeightChange, weightChange);
    synapse.minWeightChange = std::min(synapse.minWeightChange, weightChange);
    
    // Update efficacy statistics
    synapse.averageEfficacy = (synapse.averageEfficacy + efficacy) / 2;
    
    synapse.lastUpdateTime = std::chrono::system_clock::now();
    
    // Clean up old synapse profiles if needed
    if (synapsePerformances_.size() > config_.maxSynapseProfiles) {
        // Remove least recently updated synapses
        auto it = synapsePerformances_.begin();
        std::advance(it, synapsePerformances_.size() - config_.maxSynapseProfiles);
        synapsePerformances_.erase(it, synapsePerformances_.end());
    }
}

void NeuralProfiler::recordNeuralNetworkPerformance(const std::string& componentName, const std::string& operationName,
                                                   const std::chrono::nanoseconds& totalTime,
                                                   const std::chrono::nanoseconds& forwardPassTime,
                                                   const std::chrono::nanoseconds& backwardPassTime,
                                                   float accuracy, size_t inputSize, size_t outputSize) {
    std::lock_guard<std::mutex> lock(neuralMutex_);
    
    std::string key = componentName + "::" + operationName;
    if (neuralNetworkPerformances_.find(key) == neuralNetworkPerformances_.end()) {
        neuralNetworkPerformances_[key] = NeuralNetworkPerformance(componentName, operationName);
    }
    
    auto& performance = neuralNetworkPerformances_[key];
    performance.executionTime += totalTime;
    performance.neuralForwardPassTime += forwardPassTime;
    performance.neuralBackwardPassTime += backwardPassTime;
    performance.forwardPassAccuracy = accuracy;
    performance.inputSize = inputSize;
    performance.outputSize = outputSize;
    performance.timestamp = std::chrono::system_clock::now();
    
    // Calculate gradient and weight norms (simplified)
    performance.gradientNorm = calculateGradientNorm(performance);
    performance.weightNorm = calculateWeightNorm(performance);
    
    // Clean up old neural profiles if needed
    if (neuralNetworkPerformances_.size() > config_.maxNeuralProfiles) {
        // Remove oldest neural profiles
        auto it = neuralNetworkPerformances_.begin();
        std::advance(it, neuralNetworkPerformances_.size() - config_.maxNeuralProfiles);
        neuralNetworkPerformances_.erase(it, neuralNetworkPerformances_.end());
    }
}

std::vector<NeuralNetworkPerformance> NeuralProfiler::getNeuralNetworkPerformances() const {
    std::lock_guard<std::mutex> lock(neuralMutex_);
    std::vector<NeuralNetworkPerformance> result;
    result.reserve(neuralNetworkPerformances_.size());
    for (const auto& [key, performance] : neuralNetworkPerformances_) {
        result.push_back(performance);
    }
    return result;
}

std::vector<NeuronPerformance> NeuralProfiler::getNeuronPerformances() const {
    std::lock_guard<std::mutex> lock(neuronMutex_);
    std::vector<NeuronPerformance> result;
    result.reserve(neuronPerformances_.size());
    for (const auto& [key, neuron] : neuronPerformances_) {
        result.push_back(neuron);
    }
    return result;
}

std::vector<SynapsePerformance> NeuralProfiler::getSynapsePerformances() const {
    std::lock_guard<std::mutex> lock(synapseMutex_);
    std::vector<SynapsePerformance> result;
    result.reserve(synapsePerformances_.size());
    for (const auto& [key, synapse] : synapsePerformances_) {
        result.push_back(synapse);
    }
    return result;
}

void NeuralProfiler::setConfig(const NeuralProfilingConfig& config) {
    config_ = config;
}

NeuralProfilingConfig NeuralProfiler::getConfig() const {
    return config_;
}

void NeuralProfiler::updateNeuralMetrics(size_t totalNeurons, size_t totalSynapses, size_t activeNeurons,
                                        size_t firingNeurons, float averageActivity) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    totalNeurons_ = totalNeurons;
    totalSynapses_ = totalSynapses;
    activeNeurons_ = activeNeurons;
    firingNeurons_ = firingNeurons;
    averageNetworkActivity_ = averageActivity;
}

void NeuralProfiler::analyzeNeuronPerformance(NeuronPerformance& neuron) const {
    // Calculate performance score based on firing rate, membrane potential, etc.
    double performanceScore = calculatePerformanceScore(neuron);
    
    // Check if neuron performance is below threshold
    if (performanceScore < config_.neuronPerformanceThreshold) {
        // Log warning or create alert
        NLM_LOG_WARNING("Neuron " + neuron.neuronId + " performance below threshold: " + std::to_string(performanceScore));
    }
}

void NeuralProfiler::analyzeSynapsePerformance(SynapsePerformance& synapse) const {
    // Calculate synapse efficiency
    double efficiency = calculateSynapseEfficiency(synapse);
    
    // Check if synapse efficiency is below threshold
    if (efficiency < config_.synapsePerformanceThreshold) {
        // Log warning or create alert
        NLM_LOG_WARNING("Synapse " + synapse.synapseId + " efficiency below threshold: " + std::to_string(efficiency));
    }
}

void NeuralProfiler::analyzeNeuralNetworkPerformance(NeuralNetworkPerformance& performance) const {
    // Analyze forward pass vs backward pass time ratio
    double forwardPassRatio = (performance.executionTime.count() > 0)
        ? (performance.neuralForwardPassTime.count() * 1.0 / performance.executionTime.count())
        : 0.0;
    
    // Check for bottlenecks
    if (forwardPassRatio < 0.5) {
        // Forward pass is too slow relative to total time
        NLM_LOG_WARNING("Neural network " + performance.componentName + "::" + performance.operationName +
                       " has slow forward pass (ratio: " + std::to_string(forwardPassRatio) + ")");
    }
}

std::string NeuralProfiler::generateHtmlReport() const {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "    <title>NLM Neural Performance Report</title>" << std::endl;
    oss << "    <style>" << std::endl;
    oss << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    oss << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }" << std::endl;
    oss << "        .metrics { margin: 20px 0; padding: 20px; background-color: #f9f9f9; border-radius: 5px; }" << std::endl;
    oss << "        table { border-collapse: collapse; width: 100%; margin: 10px 0; }" << std::endl;
    oss << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    oss << "        th { background-color: #f2f2f2; }" << std::endl;
    oss << "        .good { color: green; }" << std::endl;
    oss << "        .warning { color: orange; }" << std::endl;
    oss << "        .poor { color: red; }" << std::endl;
    oss << "    </style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    
    oss << "    <div class=\"header\">" << std::endl;
    oss << "        <h1>NLM Neural Performance Report</h1>" << std::endl;
    oss << "        <p>Generated on: " << getCurrentTimestamp() << "</p>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <div class=\"metrics\">" << std::endl;
    oss << "        <h2>Neural System Metrics</h2>" << std::endl;
    oss << "        <p>Total Neurons: " << totalNeurons_ << "</p>" << std::endl;
    oss << "        <p>Total Synapses: " << totalSynapses_ << "</p>" << std::endl;
    oss << "        <p>Active Neurons: " << activeNeurons_ << "</p>" << std::endl;
    oss << "        <p>Firing Neurons: " << firingNeurons_ << "</p>" << std::endl;
    oss << "        <p>Average Network Activity: " << averageNetworkActivity_ << "</p>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <h2>Neuron Performance</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Neuron ID</th><th>Region</th><th>Population</th><th>Avg Firing Rate</th><th>Max Firing Rate</th><th>Avg Membrane Potential</th><th>Spike Count</th><th>Performance Score</th></tr>" << std::endl;
    
    for (const auto& [key, neuron] : neuronPerformances_) {
        double score = calculatePerformanceScore(neuron);
        std::string scoreClass = "good";
        if (score < config_.neuronPerformanceThreshold * 0.5) {
            scoreClass = "poor";
        } else if (score < config_.neuronPerformanceThreshold * 0.8) {
            scoreClass = "warning";
        }
        
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << neuron.neuronId << "</td>" << std::endl;
        oss << "            <td>" << neuron.regionName << "</td>" << std::endl;
        oss << "            <td>" << neuron.populationName << "</td>" << std::endl;
        oss << "            <td>" << neuron.averageFiringRate << "</td>" << std::endl;
        oss << "            <td>" << neuron.maxFiringRate << "</td>" << std::endl;
        oss << "            <td>" << neuron.averageMembranePotential << "</td>" << std::endl;
        oss << "            <td>" << neuron.spikeCount << "</td>" << std::endl;
        oss << "            <td class=\"" << scoreClass << "\">" << score << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "    <h2>Synapse Performance</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Synapse ID</th><th>Source</th><th>Destination</th><th>Avg Weight Change</th><th>Efficacy</th><th>Efficiency</th></tr>" << std::endl;
    
    for (const auto& [key, synapse] : synapsePerformances_) {
        double efficiency = calculateSynapseEfficiency(synapse);
        std::string efficiencyClass = "good";
        if (efficiency < config_.synapsePerformanceThreshold * 0.5) {
            efficiencyClass = "poor";
        } else if (efficiency < config_.synapsePerformanceThreshold * 0.8) {
            efficiencyClass = "warning";
        }
        
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << synapse.synapseId << "</td>" << std::endl;
        oss << "            <td>" << synapse.sourceNeuronId << "</td>" << std::endl;
        oss << "            <td>" << synapse.destinationNeuronId << "</td>" << std::endl;
        oss << "            <td>" << synapse.averageWeightChange << "</td>" << std::endl;
        oss << "            <td>" << synapse.averageEfficacy << "</td>" << std::endl;
        oss << "            <td class=\"" << efficiencyClass << "\">" << efficiency << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "    <h2>Neural Network Performance</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Component</th><th>Operation</th><th>Execution Time (ms)</th><th>Forward Pass (ms)</th><th>Backward Pass (ms)</th><th>Accuracy</th><th>Forward Pass Ratio</th></tr>" << std::endl;
    
    for (const auto& [key, performance] : neuralNetworkPerformances_) {
        double forwardPassRatio = (performance.executionTime.count() > 0)
            ? (performance.neuralForwardPassTime.count() * 1.0 / performance.executionTime.count())
            : 0.0;
        
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << performance.componentName << "</td>" << std::endl;
        oss << "            <td>" << performance.operationName << "</td>" << std::endl;
        oss << "            <td>" << (performance.executionTime.count() / 1000000.0) << "</td>" << std::endl;
        oss << "            <td>" << (performance.neuralForwardPassTime.count() / 1000000.0) << "</td>" << std::endl;
        oss << "            <td>" << (performance.neuralBackwardPassTime.count() / 1000000.0) << "</td>" << std::endl;
        oss << "            <td>" << performance.forwardPassAccuracy << "</td>" << std::endl;
        oss << "            <td>" << forwardPassRatio << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

std::string NeuralProfiler::generateJsonReport() const {
    std::ostringstream oss;
    
    oss << "{
    \"neuralReport\": {
        \"timestamp\": \"" << getCurrentTimestamp() << "\",
        \"totalNeurons\": " << totalNeurons_ << ",
        \"totalSynapses\": " << totalSynapses_ << ",
        \"activeNeurons\": " << activeNeurons_ << ",
        \"firingNeurons\": " << firingNeurons_ << ",
        \"averageNetworkActivity\": " << averageNetworkActivity_ << ",
        \"neuronPerformances\": [";
    
    for (size_t i = 0; i < neuronPerformances_.size(); ++i) {
        const auto& neuron = neuronPerformances_.begin()->second;
        oss << "{
            \"neuronId\": \"" << neuron.neuronId << "\",
            \"region\": \"" << neuron.regionName << "\",
            \"population\": \"" << neuron.populationName << "\",
            \"averageFiringRate\": " << neuron.averageFiringRate << ",
            \"maxFiringRate\": " << neuron.maxFiringRate << ",
            \"averageMembranePotential\": " << neuron.averageMembranePotential << ",
            \"spikeCount\": " << neuron.spikeCount
        }";
        
        if (i < neuronPerformances_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "],
        \"synapsePerformances\": [";
    
    for (size_t i = 0; i < synapsePerformances_.size(); ++i) {
        const auto& synapse = synapsePerformances_.begin()->second;
        oss << "{
            \"synapseId\": \"" << synapse.synapseId << "\",
            \"sourceNeuron\": \"" << synapse.sourceNeuronId << "\",
            \"destinationNeuron\": \"" << synapse.destinationNeuronId << "\",
            \"averageWeightChange\": " << synapse.averageWeightChange << ",
            \"efficacy\": " << synapse.averageEfficacy << ",
            \"spikeTransmissionCount\": " << synapse.spikeTransmissionCount
        }";
        
        if (i < synapsePerformances_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "],
        \"neuralNetworkPerformances\": [";
    
    for (size_t i = 0; i < neuralNetworkPerformances_.size(); ++i) {
        const auto& performance = neuralNetworkPerformances_.begin()->second;
        oss << "{
            \"componentName\": \"" << performance.componentName << "\",
            \"operationName\": \"" << performance.operationName << "\",
            \"executionTime\": " << (performance.executionTime.count() / 1000000.0) << ",
            \"forwardPassTime\": " << (performance.neuralForwardPassTime.count() / 1000000.0) << ",
            \"backwardPassTime\": " << (performance.neuralBackwardPassTime.count() / 1000000.0) << ",
            \"accuracy\": " << performance.forwardPassAccuracy << ",
            \"inputSize\": " << performance.inputSize << ",
            \"outputSize\": " << performance.outputSize
        }";
        
        if (i < neuralNetworkPerformances_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "]
    }
}";
    
    return oss.str();
}

std::string NeuralProfiler::generateTextReport() const {
    std::ostringstream oss;
    
    oss << "NLM Neural Performance Report" << std::endl;
    oss << std::string(50, '=') << std::endl;
    oss << "Generated on: " << getCurrentTimestamp() << std::endl;
    oss << "Total Neurons: " << totalNeurons_ << std::endl;
    oss << "Total Synapses: " << totalSynapses_ << std::endl;
    oss << "Active Neurons: " << activeNeurons_ << std::endl;
    oss << "Firing Neurons: " << firingNeurons_ << std::endl;
    oss << "Average Network Activity: " << averageNetworkActivity_ << std::endl;
    oss << std::endl;
    
    oss << "Neuron Performance Summary:" << std::endl;
    for (const auto& [key, neuron] : neuronPerformances_) {
        double score = calculatePerformanceScore(neuron);
        oss << "  - " << neuron.neuronId << " (" << neuron.regionName << "/" << neuron.populationName << "):" << std::endl;
        oss << "    Firing Rate: " << neuron.averageFiringRate << "/" << neuron.maxFiringRate << std::endl;
        oss << "    Membrane Potential: " << neuron.averageMembranePotential << std::endl;
        oss << "    Spike Count: " << neuron.spikeCount << std::endl;
        oss << "    Performance Score: " << score << std::endl;
        oss << std::endl;
    }
    
    oss << "Synapse Performance Summary:" << std::endl;
    for (const auto& [key, synapse] : synapsePerformances_) {
        double efficiency = calculateSynapseEfficiency(synapse);
        oss << "  - " << synapse.synapseId << " (" << synapse.sourceNeuronId << " -> " << synapse.destinationNeuronId << "):" << std::endl;
        oss << "    Average Weight Change: " << synapse.averageWeightChange << std::endl;
        oss << "    Efficacy: " << synapse.averageEfficacy << std::endl;
        oss << "    Efficiency: " << efficiency << std::endl;
        oss << "    Spike Transmissions: " << synapse.spikeTransmissionCount << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

double NeuralProfiler::calculatePerformanceScore(const NeuronPerformance& neuron) const {
    // Calculate performance score based on firing rate, membrane potential, etc.
    double firingScore = std::min(1.0, neuron.averageFiringRate / config_.neuronPerformanceThreshold);
    double membraneScore = 0.5 + (neuron.averageMembranePotential + 80.0) / 200.0; // Normalize to 0-1
    double activityScore = std::min(1.0, static_cast<double>(neuron.spikeCount) / 1000.0); // Assume 1000 spikes is max
    
    return (firingScore * 0.4 + membraneScore * 0.3 + activityScore * 0.3);
}

double NeuralProfiler::calculateSynapseEfficiency(const SynapsePerformance& synapse) const {
    // Calculate efficiency based on weight change and efficacy
    double weightScore = std::min(1.0, std::abs(synapse.averageWeightChange) / config_.synapsePerformanceThreshold);
    double efficacyScore = synapse.averageEfficacy;
    double stabilityScore = 1.0 - std::min(1.0, std::abs(synapse.averageWeightChange) / 0.5); // Avoid large weight changes
    
    return (weightScore * 0.4 + efficacyScore * 0.4 + stabilityScore * 0.2);
}

double NeuralProfiler::calculateGradientNorm(const NeuralNetworkPerformance& performance) const {
    // Simulate gradient norm calculation
    return std::min(10.0, performance.forwardPassAccuracy * 5.0);
}

double NeuralProfiler::calculateWeightNorm(const NeuralNetworkPerformance& performance) const {
    // Simulate weight norm calculation
    return std::min(5.0, performance.inputSize * performance.outputSize * 0.01);
}

void NeuralProfiler::monitorNeuralSystem() {
    while (isRunning_) {
        // Simulate neural metrics update
        static size_t neuronCount = 0;
        static size_t synapseCount = 0;
        static size_t activeCount = 0;
        static size_t firingCount = 0;
        static float activity = 0.0f;
        
        // Simulate changing metrics
        neuronCount = 1000 + (rand() % 5000);
        synapseCount = 10000 + (rand() % 50000);
        activeCount = neuronCount / 4 + (rand() % neuronCount / 4);
        firingCount = activeCount / 8 + (rand() % activeCount / 4);
        activity = static_cast<float>(firingCount) / static_cast<float>(activeCount);
        
        updateNeuralMetrics(neuronCount, synapseCount, activeCount, firingCount, activity);
        
        // Analyze performance
        for (auto& [key, neuron] : neuronPerformances_) {
            analyzeNeuronPerformance(neuron);
        }
        
        for (auto& [key, synapse] : synapsePerformances_) {
            analyzeSynapsePerformance(synapse);
        }
        
        for (auto& [key, performance] : neuralNetworkPerformances_) {
            analyzeNeuralNetworkPerformance(performance);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.profilingIntervalMs));
    }
}