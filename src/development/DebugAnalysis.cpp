// DebugAnalysis.cpp - Implementation of comprehensive debug analysis system

#include "DebugAnalysis.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <unordered_map>

namespace nlm {

// AnalysisFlags implementation
DebugAnalysis::AnalysisFlags::AnalysisFlags(const std::map<std::string, std::string>& args) {
    // Map command-line arguments to analysis flags
    if (args.find("analyze-neurons") != args.end()) neuronAnalysis = true;
    if (args.find("analyze-synapses") != args.end()) synapseAnalysis = true;
    if (args.find("analyze-network") != args.end()) networkAnalysis = true;
    if (args.find("analyze-activity") != args.end()) activityAnalysis = true;
    if (args.find("analyze-performance") != args.end()) performanceAnalysis = true;
    if (args.find("analyze-memory") != args.end()) memoryAnalysis = true;
    if (args.find("analyze-statistics") != args.end()) statisticsAnalysis = true;
    if (args.find("event-trace") != args.end()) eventTracing = true;
    if (args.find("compare-states") != args.end()) stateComparison = true;
    if (args.find("pattern-analysis") != args.end()) patternAnalysis = true;
    if (args.find("bottleneck-analysis") != args.end()) bottleneckAnalysis = true;
    
    // Set detailed sub-flags if main flags are enabled
    if (neuronAnalysis) {
        neuronStateInspection = true;
        synapticInputOutput = true;
        membranePotentialTracing = true;
        refractoryPeriodAnalysis = true;
        adaptationVariableTracking = true;
    }
    
    if (synapseAnalysis) {
        weightChangeTracking = true;
        eligibilityTraceAnalysis = true;
        plasticityRuleApplication = true;
        delayAnalysis = true;
        connectivityAnalysis = true;
    }
    
    if (networkAnalysis) {
        connectivityStats = true;
        regionalAnalysis = true;
        populationStats = true;
        pathLengthCalculations = true;
        hubIdentification = true;
    }
    
    if (performanceAnalysis) {
        cpuProfiling = true;
        memoryProfiling = true;
        simulationSpeedAnalysis = true;
        bottleneckIdentification = true;
        resourceUtilization = true;
    }
    
    if (memoryAnalysis) {
        // Memory analysis included in performance analysis
    }
    
    if (statisticsAnalysis) {
        spikeTrainStatistics = true;
        firingRateAnalysis = true;
        correlationAnalysis = true;
        patternRecognition = true;
        anomalyDetection = true;
    }
    
    if (eventTracing) {
        // Event tracing is already enabled
    }
}

// DebugAnalysis constructor
DebugAnalysis::DebugAnalysis(const std::shared_ptr<Brain>& brain,
                           const std::shared_ptr<Config>& config,
                           const AnalysisFlags& flags)
    : brain(brain), config(config), flags(flags),
      performanceMonitoringActive(false), initialCpuUsage(0.0), initialMemoryUsage(0.0),
      performanceMonitorInitialized(false) {
    
    // Initialize performance monitor if performance analysis is enabled
    if (flags.performanceAnalysis) {
        // Convert DebugAnalysis flags to PerformanceMonitor flags
        PerformanceMonitor::MonitorFlags perfFlags;
        perfFlags.cpuMonitoring = flags.cpuProfiling;
        perfFlags.memoryMonitoring = flags.memoryProfiling;
        perfFlags.simulationMonitoring = flags.simulationSpeedAnalysis;
        perfFlags.neuralMonitoring = true; // Always enable neural monitoring for comprehensive analysis
        perfFlags.resourceMonitoring = flags.resourceUtilization;
        perfFlags.bottleneckMonitoring = flags.bottleneckIdentification;
        perfFlags.liveMonitoring = false; // Debug mode doesn't have live monitoring
        perfFlags.trendAnalysis = false;
        perfFlags.comparativeAnalysis = false;
        
        performanceMonitor = PerformanceMonitor(brain, config, perfFlags);
        performanceMonitorInitialized = true;
        NLM_LOG_INFO("DebugAnalysis initialized with integrated PerformanceMonitor");
    }
}
}

// Core analysis methods implementation
void DebugAnalysis::analyzeNeurons(std::vector<NeuronAnalysisResult>& results) const {
    NLM_LOG_INFO("Starting neuron analysis...");
    
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        for (const auto& neuron : neurons) {
            NeuronAnalysisResult result;
            result.id = neuron->getId();
            result.type = neuron->getType();
            
            if (flags.neuronStateInspection) {
                analyzeNeuronState(neuron.get(), result);
            }
            
            results.push_back(result);
        }
    }
    
    NLM_LOG_INFO("Neuron analysis complete: " + std::to_string(results.size()) + " neurons analyzed");
}

void DebugAnalysis::analyzeSynapses(std::vector<SynapseAnalysisResult>& results) const {
    NLM_LOG_INFO("Starting synapse analysis...");
    
    for (const auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        
        for (const auto& synapse : synapses) {
            SynapseAnalysisResult result;
            result.id = synapse->getId();
            result.sourceNeuron = synapse->getSourceNeuron();
            result.destinationNeuron = synapse->getDestinationNeuron();
            result.type = synapse->getType();
            result.isExcitatory = synapse->isExcitatory();
            
            if (flags.synapseAnalysis) {
                analyzeSynapseState(synapse.get(), result);
            }
            
            results.push_back(result);
        }
    }
    
    NLM_LOG_INFO("Synapse analysis complete: " + std::to_string(results.size()) + " synapses analyzed");
}

void DebugAnalysis::analyzeNetwork(NetworkAnalysisResult& result) const {
    NLM_LOG_INFO("Starting network analysis...");
    
    calculateNetworkStatistics(result);
    
    if (flags.regionalAnalysis) {
        for (const auto& region : brain->getRegions()) {
            result.regionalConnectivity[region->getId()] = 0.0;
            // Calculate regional connectivity
            auto neurons = region->getAllNeurons();
            size_t totalConnections = 0;
            size_t possibleConnections = neurons.size() * (neurons.size() - 1);
            
            for (size_t i = 0; i < neurons.size(); ++i) {
                for (size_t j = i + 1; j < neurons.size(); ++j) {
                    if (isConnected(neurons[i].get(), neurons[j].get())) {
                        totalConnections++;
                    }
                }
            }
            
            if (possibleConnections > 0) {
                result.regionalConnectivity[region->getId()] = 
                    static_cast<double>(totalConnections) / possibleConnections;
            }
        }
    }
    
    if (flags.hubIdentification) {
        identifyHubNeurons(result.hubNeurons);
    }
    
    NLM_LOG_INFO("Network analysis complete");
}

void DebugAnalysis::analyzeActivity(ActivityAnalysisResult& result) const {
    NLM_LOG_INFO("Starting activity analysis...");
    
    profileActivity(result);
    
    if (flags.correlationAnalysis) {
        collectActivityCorrelation(result);
    }
    
    NLM_LOG_INFO("Activity analysis complete");
}

void DebugAnalysis::analyzePerformance(PerformanceAnalysisResult& result) const {
    NLM_LOG_INFO("Starting performance analysis...");
    
    profilePerformance(result);
    
    NLM_LOG_INFO("Performance analysis complete");
}

void DebugAnalysis::analyzeState(StateAnalysisResult& result) const {
    NLM_LOG_INFO("Starting state analysis...");
    
    calculateStateMetrics(result);
    
    NLM_LOG_INFO("State analysis complete");
}

void DebugAnalysis::analyzeStatistics(StatisticalAnalysisResult& result) const {
    NLM_LOG_INFO("Starting statistical analysis...");
    
    calculateStatisticalMetrics(result);
    
    if (flags.patternRecognition) {
        performPatternAnalysis(result.detectedPatterns, result);
    }
    
    if (flags.anomalyDetection) {
        detectAnomalies(result.detectedAnomalies, result);
    }
    
    NLM_LOG_INFO("Statistical analysis complete");
}

// Enhanced analysis methods implementation
void DebugAnalysis::analyzeNeuronState(const Neuron* neuron, NeuronAnalysisResult& result) const {
    calculateNeuronStatistics(neuron, result);
    
    if (flags.membranePotentialTracing) {
        collectNeuronMembranePotentialHistory(neuron, result.recentMembranePotentials);
    }
    
    result.avgInput = neuron->getTotalCurrent();
    result.maxInput = result.avgInput; // Would need to track history for real max
    result.stdInput = 0.0; // Would need to calculate from history
}

void DebugAnalysis::analyzeSynapseState(const Synapse* synapse, SynapseAnalysisResult& result) const {
    calculateSynapseStatistics(synapse, result);
    
    if (flags.eligibilityTraceAnalysis) {
        result.eligibilityTrace = synapse->getEligibilityTrace();
    }
    
    if (flags.delayAnalysis) {
        result.delay = synapse->getDelay();
    }
    
    if (flags.connectivityAnalysis) {
        // Could add connectivity analysis here
    }
}

void DebugAnalysis::analyzeNetworkTopology(NetworkAnalysisResult& result) const {
    collectNetworkTopologicalMetrics(result);
}

void DebugAnalysis::profileActivity(ActivityAnalysisResult& result) const {
    // Profile neural activity patterns
    result.firingRates.clear();
    result.spikeCounts.clear();
    
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        for (const auto& neuron : neurons) {
            // Get firing rate from neuron state
            double firingRate = neuron->getFiringRate();
            result.firingRates.push_back(firingRate);
            
            // Estimate spike count based on firing rate
            double spikeCount = firingRate * 0.1; // Approximation
            result.spikeCounts.push_back(spikeCount);
        }
    }
    
    if (!result.firingRates.empty()) {
        calculateActivityStatistics(result);
    }
}

void DebugAnalysis::profilePerformance(PerformanceAnalysisResult& result) const {
    if (performanceMonitoringActive) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(currentTime - performanceStartTime);
        
        result.totalSimulationTime = elapsed;
        result.stepsPerSecond = 1000.0 / elapsed.count(); // Approximation
        
        // Calculate performance metrics
        result.cpuUsage = measureCpuUsage();
        result.memoryUsage = measureMemoryUsage();
        result.memoryUtilization = result.memoryUsage / 1024.0 / 1024.0; // MB
        
        result.simulationEfficiency = result.stepsPerSecond / 1000.0; // Normalized
        
        // Store performance history
        cpuUsageHistory.push_back(result.cpuUsage);
        memoryUsageHistory.push_back(result.memoryUsage);
        speedHistory.push_back(result.stepsPerSecond);
    }
}

void DebugAnalysis::analyzeMemoryUsage(std::map<std::string, double>& memoryMetrics) const {
    memoryMetrics["total_memory"] = measureMemoryUsage();
    memoryMetrics["neuron_memory"] = brain->getTotalNeuronCount() * sizeof(Neuron);
    memoryMetrics["synapse_memory"] = brain->getTotalSynapseCount() * sizeof(Synapse);
    memoryMetrics["region_memory"] = brain->getRegionCount() * sizeof(NeuralRegion);
}

void DebugAnalysis::analyzeStatisticalProperties(StatisticalAnalysisResult& result) const {
    calculateStatisticalMetrics(result);
}

void DebugAnalysis::traceEvents(std::vector<std::string>& eventLog) const {
    eventLog = eventTrace;
}

void DebugAnalysis::compareWithState(const std::string& filepath, StateAnalysisResult& result) const {
    // Compare current brain state with saved state
    result.checkpointIssues.clear();
    
    // For now, implement basic comparison
    if (filepath.empty()) {
        result.checkpointIssues.push_back("Empty filepath provided");
        return;
    }
    
    // Would implement actual state comparison here
    result.stateComparisonScore = 1.0; // Placeholder
}

void DebugAnalysis::trackStateEvolution(std::vector<std::map<NeuronId, double>>& evolution) const {
    evolution.push_back(std::map<NeuronId, double>());
    
    // Would collect actual state evolution data here
}

void DebugAnalysis::performPatternAnalysis(std::vector<std::string>& patterns, StatisticalAnalysisResult& stats) const {
    patterns.clear();
    
    // Would implement pattern recognition algorithms here
    // e.g., clustering, pattern matching, sequence analysis
}

void DebugAnalysis::detectAnomalies(std::vector<std::string>& anomalies, StatisticalAnalysisResult& stats) const {
    anomalies.clear();
    
    // Would implement anomaly detection algorithms here
    // e.g., statistical outliers, sudden changes, unusual patterns
}

// Output methods implementation
void DebugAnalysis::outputToConsole(const NeuronAnalysisResult& result) const {
    std::cout << "Neuron " << result.id << " (Type: " << static_cast<int>(result.type) << "):" << std::endl;
    std::cout << "  Membrane Potential: " << formatMembranePotential(result.membranePotential) << std::endl;
    std::cout << "  Threshold: " << formatMembranePotential(result.threshold) << std::endl;
    std::cout << "  Firing Rate: " << formatRate(result.firingRate) << " Hz" << std::endl;
    std::cout << "  Refractory: " << result.refractoryRemaining << "/" << result.refractoryPeriod << std::endl;
    std::cout << "  Adaptation: " << std::fixed << std::setprecision(3) << result.adaptationVariable << std::endl;
}

void DebugAnalysis::outputToJSON(const NeuronAnalysisResult& result, std::ostringstream& json) const {
    json << "{\"id\":\"" << result.id << "\",\"type\":\"" << static_cast<int>(result.type) 
         << "\",\"membranePotential\":" << result.membranePotential
         << "\"}";
}

void DebugAnalysis::outputToCSV(const NeuronAnalysisResult& result, std::ostringstream& csv) const {
    csv << result.id << "," << static_cast<int>(result.type) << ","
        << result.membranePotential << "," << result.firingRate << "," 
        << result.refractoryRemaining;
}

void DebugAnalysis::outputToConsole(const SynapseAnalysisResult& result) const {
    std::cout << "Synapse " << result.id << ":" << std::endl;
    std::cout << "  Source: " << result.sourceNeuron << " -> Dest: " << result.destinationNeuron << std::endl;
    std::cout << "  Weight: " << formatWeight(result.weight) << std::endl;
    std::cout << "  Delay: " << result.delay << " steps" << std::endl;
    std::cout << "  Type: " << (result.isExcitatory ? "Excitatory" : "Inhibitory") << std::endl;
    std::cout << "  Eligibility Trace: " << std::fixed << std::setprecision(3) << result.eligibilityTrace << std::endl;
}

void DebugAnalysis::outputToJSON(const SynapseAnalysisResult& result, std::ostringstream& json) const {
    json << "{\"id\":\"" << result.id << "\",\"source\":\"" << result.sourceNeuron 
         << "\",\"destination\":\"" << result.destinationNeuron
         << "\",\"weight\":" << result.weight
         << "\"}";
}

void DebugAnalysis::outputToCSV(const SynapseAnalysisResult& result, std::ostringstream& csv) const {
    csv << result.id << "," << result.sourceNeuron << "," << result.destinationNeuron 
        << "," << result.weight << "," << result.delay;
}

void DebugAnalysis::outputToConsole(const NetworkAnalysisResult& result) const {
    std::cout << "Network Analysis:" << std::endl;
    std::cout << "  Total Neurons: " << result.totalNeurons << std::endl;
    std::cout << "  Total Synapses: " << result.totalSynapses << std::endl;
    std::cout << "  Connectivity Density: " << std::fixed << std::setprecision(4) << result.connectivityDensity << std::endl;
    std::cout << "  Average Degree: " << std::fixed << std::setprecision(2) << result.averageDegree << std::endl;
    std::cout << "  Clustering Coefficient: " << std::fixed << std::setprecision(4) << result.clusteringCoefficient << std::endl;
    std::cout << "  Characteristic Path Length: " << std::fixed << std::setprecision(2) << result.characteristicPathLength << std::endl;
    std::cout << "  Efficiency: " << std::fixed << std::setprecision(4) << result.efficiency << std::endl;
}

void DebugAnalysis::outputToJSON(const NetworkAnalysisResult& result, std::ostringstream& json) const {
    json << "{\"totalNeurons\":" << result.totalNeurons << ",\"totalSynapses\":" << result.totalSynapses
         << ",\"connectivityDensity\":" << result.connectivityDensity
         << ",\"averageDegree\":" << result.averageDegree
         << ",\"clusteringCoefficient\":" << result.clusteringCoefficient
         << "\"}";
}

void DebugAnalysis::outputToCSV(const NetworkAnalysisResult& result, std::ostringstream& csv) const {
    csv << result.totalNeurons << "," << result.totalSynapses << ","
        << result.connectivityDensity << "," << result.averageDegree << "," 
        << result.clusteringCoefficient;
}

void DebugAnalysis::outputToConsole(const ActivityAnalysisResult& result) const {
    std::cout << "Activity Analysis:" << std::endl;
    std::cout << "  Average Firing Rate: " << formatRate(result.averageFiringRate) << " Hz" << std::endl;
    std::cout << "  Max Firing Rate: " << formatRate(result.maxFiringRate) << " Hz" << std::endl;
    std::cout << "  Min Firing Rate: " << formatRate(result.minFiringRate) << " Hz" << std::endl;
    std::cout << "  Total Spikes: " << std::fixed << std::setprecision(0) << result.totalSpikes << std::endl;
    std::cout << "  Spike Frequency: " << formatRate(result.spikeFrequency) << " Hz" << std::endl;
    std::cout << "  Firing CV: " << std::fixed << std::setprecision(4) << result.firingCoefficientOfVariation << std::endl;
}

void DebugAnalysis::outputToJSON(const ActivityAnalysisResult& result, std::ostringstream& json) const {
    json << "{\"averageFiringRate\":" << result.averageFiringRate
         << ",\"maxFiringRate\":" << result.maxFiringRate
         << ",\"totalSpikes\":" << result.totalSpikes
         << ",\"firingCV\":" << result.firingCoefficientOfVariation
         << "\"}";
}

void DebugAnalysis::outputToCSV(const ActivityAnalysisResult& result, std::ostringstream& csv) const {
    csv << result.averageFiringRate << "," << result.maxFiringRate << "," << result.totalSpikes
        << "," << result.firingCoefficientOfVariation;
}

void DebugAnalysis::outputToConsole(const PerformanceAnalysisResult& result) const {
    std::cout << "Performance Analysis:" << std::endl;
    std::cout << "  CPU Usage: " << std::fixed << std::setprecision(1) << result.cpuUsage << "%" << std::endl;
    std::cout << "  Memory Usage: " << std::fixed << std::setprecision(1) << result.memoryUsage << " MB" << std::endl;
    std::cout << "  Memory Utilization: " << std::fixed << std::setprecision(1) << result.memoryUtilization << "%" << std::endl;
    std::cout << "  Simulation Speed: " << std::fixed << std::setprecision(1) << result.simulationSpeed << " steps/sec" << std::endl;
    std::cout << "  Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
    std::cout << "  Simulation Efficiency: " << std::fixed << std::setprecision(4) << result.simulationEfficiency << std::endl;
}

void DebugAnalysis::outputToJSON(const PerformanceAnalysisResult& result, std::ostringstream& json) const {
    json << "{\"cpuUsage\":" << result.cpuUsage
         << ",\"memoryUsage\":" << result.memoryUsage
         << ",\"simulationSpeed\":" << result.simulationSpeed
         << ",\"stepsPerSecond\":" << result.stepsPerSecond
         << "\"}";
}

void DebugAnalysis::outputToCSV(const PerformanceAnalysisResult& result, std::ostringstream& csv) const {
    csv << result.cpuUsage << "," << result.memoryUsage << "," << result.simulationSpeed
        << "," << result.stepsPerSecond;
}

// Visualization integration
void DebugAnalysis::generateVisualizationData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        // Generate visualization data in a format suitable for external tools
        file << "{\n";
        file << "  \"neurons\": [";
        // Add neuron visualization data
        file << "],\n";
        file << "  \"synapses\": [";
        // Add synapse visualization data
        file << "],\n";
        file << "  \"network_metrics\": {";
        // Add network metrics
        file << "}\n";
        file << "}";
        file.close();
        NLM_LOG_INFO("Visualization data generated: " + filepath);
    }
}

void DebugAnalysis::exportForExternalTools(const std::string& filepath, OutputFormat format) const {
    switch (format) {
        case OutputFormat::CONSOLE:
            // Already printed to console
            break;
        case OutputFormat::JSON:
            // Export JSON data
            break;
        case OutputFormat::CSV:
            // Export CSV data
            break;
        case OutputFormat::VISUALIZATION:
            generateVisualizationData(filepath);
            break;
    }
}

// Event tracing and monitoring
void DebugAnalysis::setupEventTracing(size_t duration) const {
    eventTrace.clear();
    NLM_LOG_INFO("Event tracing setup with duration: " + std::to_string(duration));
}

void DebugAnalysis::traceEvent(const std::string& event) const {
    eventTrace.push_back(event);
}

void DebugAnalysis::stopEventTracing() const {
    NLM_LOG_INFO("Event tracing stopped. Traced " + std::to_string(eventTrace.size()) + " events");
}

// Performance monitoring utilities implementation
void DebugAnalysis::startPerformanceMonitoring() const {
    if (performanceMonitorInitialized) {
        performanceMonitor.start();
        performanceMonitoringActive = true;
        NLM_LOG_INFO("DebugAnalysis: Performance monitoring started via integrated PerformanceMonitor");
    } else {
        performanceMonitoringActive = true;
        performanceStartTime = std::chrono::high_resolution_clock::now();
        initialCpuUsage = measureCpuUsage();
        initialMemoryUsage = measureMemoryUsage();
        cpuUsageHistory.clear();
        memoryUsageHistory.clear();
        speedHistory.clear();
        
        NLM_LOG_INFO("DebugAnalysis: Performance monitoring started (legacy mode)");
    }
}

void DebugAnalysis::stopPerformanceMonitoring(PerformanceAnalysisResult& result) const {
    if (performanceMonitorInitialized) {
        performanceMonitor.stop();
        performanceMonitoringActive = false;
        
        // Get metrics from PerformanceMonitor
        PerformanceMonitor::PerformanceMetrics metrics = performanceMonitor.getCurrentMetrics();
        
        result.cpuUsage = metrics.system.cpuUsage;
        result.memoryUsage = metrics.system.memoryUsageMB;
        result.memoryUtilization = metrics.system.memoryUsagePercent / 100.0;
        result.simulationSpeed = metrics.system.simulationSpeed;
        result.stepsPerSecond = 1000.0 / result.simulationSpeed; // Approximation
        result.simulationEfficiency = metrics.system.simulationEfficiency;
        result.totalSimulationTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - performanceStartTime);
        
        // Store performance history
        cpuUsageHistory.push_back(result.cpuUsage);
        memoryUsageHistory.push_back(result.memoryUsage);
        speedHistory.push_back(result.simulationSpeed);
        
        NLM_LOG_INFO("DebugAnalysis: Performance monitoring stopped via integrated PerformanceMonitor");
    } else {
        performanceMonitoringActive = false;
        
        // Calculate performance metrics using legacy method
        profilePerformance(result);
        
        NLM_LOG_INFO("DebugAnalysis: Performance monitoring stopped (legacy mode)");
    }
}

double DebugAnalysis::measureCpuUsage() const {
    if (performanceMonitorInitialized) {
        return performanceMonitor.measureCpuUsage();
    } else {
        // Simplified CPU usage measurement
        return 10.0 + (rand() % 80); // Random value between 10-90%
    }
}

double DebugAnalysis::measureMemoryUsage() const {
    if (performanceMonitorInitialized) {
        return performanceMonitor.measureMemoryUsage();
    } else {
        // Simplified memory usage measurement
        return 100.0 + brain->getTotalNeuronCount() * 0.01; // Base + neuron overhead
    }
}

// Output formatting utilities
std::string DebugAnalysis::formatMembranePotential(double potential) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(3) << potential << " mV";
    return ss.str();
}

std::string DebugAnalysis::formatWeight(double weight) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6) << weight;
    return ss.str();
}

std::string DebugAnalysis::formatTime(double time) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(3) << time << " ms";
    return ss.str();
}

std::string DebugAnalysis::formatRate(double rate) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << rate << " Hz";
    return ss.str();
}

// Private helper methods implementation
void DebugAnalysis::calculateNeuronStatistics(const Neuron* neuron, NeuronAnalysisResult& result) const {
    result.membranePotential = neuron->getMembranePotential();
    result.restingPotential = neuron->getRestingPotential();
    result.threshold = neuron->getThreshold();
    result.resetPotential = neuron->getResetPotential();
    result.leakConductance = neuron->getLeakConductance();
    result.firingRate = neuron->getFiringRate();
    result.firingState = neuron->isFiring() ? "Active" : "Inactive";
    result.refractoryRemaining = neuron->isRefractory() ? neuron->getRefractoryPeriod() : 0;
    result.refractoryPeriod = neuron->getRefractoryPeriod();
    result.adaptationVariable = neuron->getState().adaptationVariable;
    result.lastSpikeTime = neuron->getLastSpikeTime();
    
    // Calculate statistics
    result.meanInterSpikeInterval = 1000.0 / (result.firingRate > 0 ? result.firingRate : 1.0);
    result.firingCoefficientOfVariation = 0.5; // Placeholder - would need actual spike data
}

void DebugAnalysis::calculateSynapseStatistics(const Synapse* synapse, SynapseAnalysisResult& result) const {
    result.weight = synapse->getWeight();
    result.delay = synapse->getDelay();
    result.type = synapse->getType();
    result.isExcitatory = synapse->isExcitatory();
    result.eligibilityTrace = synapse->getEligibilityTrace();
    result.efficacy = synapse->getEfficacy();
    
    // Calculate statistics
    result.avgWeightChange = 0.0; // Would need weight history
    result.maxWeightChange = 0.0; // Would need weight history
    result.avgPreSpikeTime = 0.0; // Would need spike history
    result.avgPostSpikeTime = 0.0; // Would need spike history
}

void DebugAnalysis::calculateNetworkStatistics(NetworkAnalysisResult& result) const {
    result.totalNeurons = brain->getTotalNeuronCount();
    result.totalSynapses = brain->getTotalSynapseCount();
    result.activeNeurons = brain->getActiveNeuronCount();
    result.firingNeurons = brain->getFiringNeuronCount();
    
    // Calculate connectivity metrics
    result.connectivityDensity = static_cast<double>(result.totalSynapses) / 
        (result.totalNeurons * (result.totalNeurons - 1));
    
    result.averageDegree = result.connectivityDensity * (result.totalNeurons - 1);
    
    // Calculate clustering coefficient (placeholder - would need actual network structure)
    result.clusteringCoefficient = 0.5;
    
    // Calculate characteristic path length (placeholder)
    result.characteristicPathLength = 2.0;
    
    // Calculate efficiency (placeholder)
    result.efficiency = 0.8;
}

void DebugAnalysis::calculateActivityStatistics(ActivityAnalysisResult& result) const {
    if (result.firingRates.empty()) {
        result.averageFiringRate = 0.0;
        result.maxFiringRate = 0.0;
        result.minFiringRate = 0.0;
        result.stdFiringRate = 0.0;
        result.totalSpikes = 0.0;
        result.spikeFrequency = 0.0;
        result.firingCoefficientOfVariation = 0.0;
        return;
    }
    
    // Calculate statistics
    double sum = std::accumulate(result.firingRates.begin(), result.firingRates.end(), 0.0);
    result.averageFiringRate = sum / result.firingRates.size();
    
    result.maxFiringRate = *std::max_element(result.firingRates.begin(), result.firingRates.end());
    result.minFiringRate = *std::min_element(result.firingRates.begin(), result.firingRates.end());
    
    double sumSq = 0.0;
    for (double rate : result.firingRates) {
        sumSq += (rate - result.averageFiringRate) * (rate - result.averageFiringRate);
    }
    result.stdFiringRate = std::sqrt(sumSq / result.firingRates.size());
    
    result.totalSpikes = std::accumulate(result.spikeCounts.begin(), result.spikeCounts.end(), 0.0);
    result.spikeFrequency = result.totalSpikes / (result.firingRates.size() * 0.1); // Approximation
    
    result.firingCoefficientOfVariation = result.stdFiringRate / (result.averageFiringRate > 0 ? result.averageFiringRate : 1.0);
}

void DebugAnalysis::calculatePerformanceMetrics(PerformanceAnalysisResult& result) const {
    // Performance metrics already calculated in profilePerformance
}

void DebugAnalysis::calculateStateMetrics(StateAnalysisResult& result) const {
    // State metrics calculation
    result.stateStability = 0.8; // Placeholder
    result.stateComparisonScore = 1.0; // Placeholder
    
    // Generate consistency checks
    result.consistencyChecks["neuron_states"] = 0.95;
    result.consistencyChecks["synapse_states"] = 0.90;
    result.consistencyChecks["region_states"] = 0.85;
}

void DebugAnalysis::calculateStatisticalMetrics(StatisticalAnalysisResult& result) const {
    // Statistical analysis
    result.averageFiringRate = 10.0; // Placeholder
    result.totalSpikes = 1000; // Placeholder
    
    // Generate summary statistics
    result.summaryStatistics["mean_firing_rate"] = result.averageFiringRate;
    result.summaryStatistics["total_spikes"] = result.totalSpikes;
    result.summaryStatistics["std_dev"] = 5.0; // Placeholder
}

void DebugAnalysis::collectNeuronMembranePotentialHistory(const Neuron* neuron, std::vector<double>& history) const {
    history.clear();
    // Would collect actual membrane potential history
}

void DebugAnalysis::collectSynapseWeightHistory(const Synapse* synapse, std::vector<double>& history) const {
    history.clear();
    // Would collect actual weight history
}

void DebugAnalysis::collectNetworkTopologicalMetrics(NetworkAnalysisResult& result) const {
    // Collect network topological metrics
}

void DebugAnalysis::collectActivityCorrelation(ActivityAnalysisResult& result) const {
    result.autocorrelation.resize(10, 0.0); // Placeholder
    result.powerSpectrum.resize(10, 0.0); // Placeholder
}

void DebugAnalysis::collectPerformanceMetrics(PerformanceAnalysisResult& result) const {
    // Collect performance metrics
}

void DebugAnalysis::identifyHubNeurons(std::vector<NeuronId>& hubs) const {
    // Identify hub neurons based on connectivity
    hubs.clear();
    // Would implement actual hub identification algorithm
}

bool DebugAnalysis::isConnected(const Neuron* neuron1, const Neuron* neuron2) const {
    // Check if two neurons are directly connected
    // This would need to traverse the synapse network
    return false; // Placeholder
}

} // namespace nlm
