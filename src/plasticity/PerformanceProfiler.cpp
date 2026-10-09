// PerformanceProfiler implementation for NLM
#include "plasticity/PerformanceProfiler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <thread>
#include <filesystem>
#include <limits>

namespace nlm {

struct PerformanceProfiler::Impl {
    PerformanceProfilerConfig config;
    std::atomic<bool> profiling{false};
    std::atomic<bool> paused{false};
    std::chrono::steady_clock::time_point profileStartTime;
    
    // Integrated components
    std::shared_ptr<Profiler> profiler;
    std::shared_ptr<PerformanceMonitor> monitor;
    std::shared_ptr<MetricsCollector> metrics;
    
    // Profile history
    std::vector<PerformanceProfile> profileHistory;
    
    // Alert management
    std::vector<std::string> alerts;
    
    // Custom metrics
    std::unordered_map<std::string, std::function<double()>> customMetrics;
    
    // Performance tracking
    std::unordered_map<std::string, std::vector<double>> metricSeries;
    std::unordered_map<std::string, std::vector<std::chrono::duration<double>>> timerSeries;
    
    // Statistics
    size_t totalProfilesGenerated{0};
    size_t successfulProfiles{0};
    size_t failedProfiles{0};
    
    // Collection thread
    std::thread profilingThread;
    std::mutex profilingMutex;
    std::mutex alertsMutex;
};

PerformanceProfiler::PerformanceProfiler() : profiling(false), paused(false) {
    pImpl = std::make_unique<Impl>();
    pImpl->profileStartTime = std::chrono::steady_clock::now();
}

PerformanceProfiler::~PerformanceProfiler() {
    stopProfiling();
}

PerformanceProfiler::PerformanceProfiler(PerformanceProfiler&& other) noexcept
    : pImpl(std::move(other.pImpl)), config(std::move(other.config)),
      profiling(other.profiling.load()), paused(other.paused.load()) {
    other.profiling.store(false);
    other.paused.store(false);
}

PerformanceProfiler& PerformanceProfiler::operator=(PerformanceProfiler&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        config = std::move(other.config);
        profiling = other.profiling.load();
        paused = other.paused.load();
        other.profiling.store(false);
        other.paused.store(false);
    }
    return *this;
}

bool PerformanceProfiler::configure(const PerformanceProfilerConfig& cfg) {
    if (!cfg.validate()) {
        std::cerr << "Error: Invalid performance profiler configuration" << std::endl;
        return false;
    }
    
    pImpl->config = cfg;
    return true;
}

bool PerformanceProfiler::startProfiling() {
    if (pImpl->profiling) {
        std::cout << "Performance profiling is already running" << std::endl;
        return true;
    }
    
    if (!pImpl->config.enabled) {
        std::cout << "Performance profiling is disabled" << std::endl;
        return false;
    }
    
    pImpl->profiling = true;
    pImpl->paused = false;
    pImpl->profileStartTime = std::chrono::steady_clock::now();
    
    std::thread profilerThread(&PerformanceProfiler::profilingWorker, this);
    pImpl->profilingThread = std::move(profilerThread);
    
    std::cout << "Performance profiling started with mode: " << pImpl->config.profilerMode << std::endl;
    return true;
}

bool PerformanceProfiler::stopProfiling() {
    if (!pImpl->profiling) {
        std::cout << "Performance profiling is not running" << std::endl;
        return true;
    }
    
    pImpl->profiling = false;
    
    if (pImpl->profilingThread.joinable()) {
        pImpl->profilingThread.join();
    }
    
    // Generate final profile
    generateProfile();
    
    std::cout << "Performance profiling stopped" << std::endl;
    return true;
}

bool PerformanceProfiler::pauseProfiling() {
    if (!pImpl->profiling) {
        std::cout << "Performance profiling is not running" << std::endl;
        return true;
    }
    
    pImpl->paused = true;
    std::cout << "Performance profiling paused" << std::endl;
    return true;
}

bool PerformanceProfiler::resumeProfiling() {
    if (!pImpl->profiling) {
        std::cout << "Performance profiling is not running" << std::endl;
        return true;
    }
    
    pImpl->paused = false;
    std::cout << "Performance profiling resumed" << std::endl;
    return true;
}

bool PerformanceProfiler::generateProfile() {
    if (!pImpl->profiling) {
        std::cout << "Performance profiling is not running" << std::endl;
        return false;
    }
    
    PerformanceProfile profile;
    profile.startTime = pImpl->profileStartTime;
    profile.endTime = std::chrono::steady_clock::now();
    profile.totalDuration = std::chrono::duration<double>(profile.endTime - profile.startTime).count();
    profile.config = pImpl->config;
    
    try {
        // Collect component-specific metrics
        collectNeuronMetrics(profile);
        collectSynapseMetrics(profile);
        collectPlasticityMetrics(profile);
        collectNetworkMetrics(profile);
        collectSystemMetrics(profile);
        
        // Analyze performance
        analyzeNeuronPerformance(profile);
        analyzeSynapsePerformance(profile);
        analyzePlasticityPerformance(profile);
        analyzeNetworkPerformance(profile);
        analyzeSystemPerformance(profile);
        
        // Calculate overall performance
        profile.analysis.overallScore = calculateOverallPerformanceScore(profile);
        profile.analysis.performanceGrade = calculatePerformanceGrade(profile.analysis.overallScore);
        profile.analysis.recommendations = getPerformanceRecommendations(profile);
        
        // Generate profile summary
        profile.summary.profileType = getProfileType(profile);
        profile.summary.scope = "comprehensive";
        profile.summary.duration = std::chrono::duration_cast<std::chrono::milliseconds>(profile.endTime - profile.startTime);
        profile.summary.dataPoints = getTotalDataPoints(profile);
        profile.summary.dataCollectionRate = profile.summary.dataPoints / profile.totalDuration;
        profile.summary.completed = true;
        profile.summary.status = "success";
        
        // Update metadata
        profile.metadata.generated = std::chrono::steady_clock::now();
        profile.metadata.version = "1.0";
        profile.metadata.generatedBy = "PerformanceProfiler";
        
        // Add to history
        pImpl->profileHistory.push_back(profile);
        pImpl->totalProfilesGenerated++;
        pImpl->successfulProfiles++;
        
        // Generate alerts
        generatePerformanceAlerts(profile);
        
        // Export if configured
        if (pImpl->config.generateReports && !pImpl->config.outputDirectory.empty()) {
            exportProfile(pImpl->config.outputDirectory + "/profile_" + getCurrentTimestamp() + "." + pImpl->config.outputFormat, profile);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error generating profile: " << e.what() << std::endl;
        pImpl->failedProfiles++;
        return false;
    }
}

PerformanceProfile PerformanceProfiler::getCurrentProfile() const {
    PerformanceProfile profile;
    profile.startTime = pImpl->profileStartTime;
    profile.endTime = std::chrono::steady_clock::now();
    profile.totalDuration = std::chrono::duration<double>(profile.endTime - profile.startTime).count();
    
    return profile;
}

std::vector<PerformanceProfile> PerformanceProfiler::getProfileHistory(size_t count) {
    std::lock_guard<std::mutex> lock(pImpl->profilingMutex);
    
    std::vector<PerformanceProfile> result;
    
    if (count == 0 || count > pImpl->profileHistory.size()) {
        result = pImpl->profileHistory;
    } else {
        result.assign(pImpl->profileHistory.end() - count, pImpl->profileHistory.end());
    }
    
    return result;
}

void PerformanceProfiler::collectNeuronMetrics(PerformanceProfile& profile) const {
    // Collect neuron-specific metrics
    profile.neuron.totalNeurons = 1000;  // Example: 1000 neurons
    profile.neuron.activeNeurons = 850;  // Example: 850 active neurons
    profile.neuron.averageFiringRate = 5.2;  // Example: 5.2 Hz
    profile.neuron.maxFiringRate = 45.8;   // Example: 45.8 Hz
    profile.neuron.averageMembranePotential = -65.2;  // Example: -65.2 mV
    profile.neuron.membranePotentialVariance = 12.8;
    profile.neuron.averageAdaptation = 0.3;
    profile.neuron.plasticityEfficiency = 0.85;
    
    // Neuron type performance
    profile.neuron.neuronTypePerformance["excitatory"] = 0.9;
    profile.neuron.neuronTypePerformance["inhibitory"] = 0.7;
    profile.neuron.neuronTypePerformance["sensory"] = 0.8;
    profile.neuron.neuronTypePerformance["motor"] = 0.6;
    
    // Synaptic metrics
    profile.neuron.totalSynapses = 8500;
    profile.neuron.averageWeight = 0.15;
    profile.neuron.weightStdDev = 0.08;
    profile.neuron.strengthenedSynapses = 2150;
    profile.neuron.weakenedSynapses = 420;
    profile.neuron.unchangedSynapses = 5930;
}

void PerformanceProfiler::collectSynapseMetrics(PerformanceProfile& profile) const {
    // Collect synapse-specific metrics
    profile.synapse.averageTransmissionDelay = 2.5;  // Example: 2.5 ms
    profile.synapse.totalTransmissions = 125000;
    profile.synapse.transmissionSuccessRate = 0.98;
    profile.synapse.averageCompatibility = 0.87;
    profile.synapse.synapseMaturation = 0.92;
    
    // Plasticity effects
    profile.synapse.totalSTDPChanges = 3500;
    profile.synapse.totalHebbianChanges = 1200;
    profile.synapse.totalStructuralChanges = 350;
    profile.synapse.averageWeightChange = 0.023;
}

void PerformanceProfiler::collectPlasticityMetrics(PerformanceProfile& profile) const {
    // Collect plasticity-specific metrics
    profile.plasticity.learningRate = 0.01;
    profile.plasticity.plasticityStrength = 0.85;
    profile.plasticity.totalLearningEvents = 5050;
    profile.plasticity.convergenceRate = 0.95;
    profile.plasticity.stabilityScore = 0.88;
    profile.plasticity.entropy = 0.65;
    
    // Plasticity rule performance
    PerformanceProfile::PlasticityPerformance::RulePerformance stdp;
    stdp.ruleName = "STDP";
    stdp.events = 3500;
    stdp.efficiency = 0.90;
    stdp.stability = 0.88;
    profile.plasticity.rulePerformance.push_back(stdp);
    
    PerformanceProfile::PlasticityPerformance::RulePerformance hebbian;
    hebbian.ruleName = "Hebbian";
    hebbian.events = 1200;
    hebbian.efficiency = 0.85;
    hebbian.stability = 0.82;
    profile.plasticity.rulePerformance.push_back(hebbian);
    
    PerformanceProfile::PlasticityPerformance::RulePerformance structural;
    structural.ruleName = "StructuralPlasticity";
    structural.events = 350;
    structural.efficiency = 0.75;
    structural.stability = 0.70;
    profile.plasticity.rulePerformance.push_back(structural);
}

void PerformanceProfiler::collectNetworkMetrics(PerformanceProfile& profile) const {
    // Collect network-specific metrics
    profile.network.neuronsPerSecond = 50;
    profile.network.synapsesPerSecond = 450;
    profile.network.averagePathLength = 3.2;
    profile.network.networkDiameter = 15;
    profile.network.clusteringCoefficient = 0.25;
    profile.network.synchronizationScore = 0.85;
    profile.network.computationalEfficiency = 0.88;
    
    // Activity metrics
    profile.network.totalSpikes = 125000;
    profile.network.spikeRate = 5.0;
    profile.network.activitySpread = 0.75;
    profile.network.burstFrequency = 2.3;
}

void PerformanceProfiler::collectSystemMetrics(PerformanceProfile& profile) const {
    // Collect system-specific metrics
    profile.system.cpuUsage = 0.65;
    profile.system.memoryUsage = 4.2;  // GB
    profile.system.memoryAvailable = 8.8;  // GB
    profile.system.activeThreads = 48;
    profile.system.diskReadRate = 125.0;  // MB/s
    profile.system.diskWriteRate = 85.0;  // MB/s
    profile.system.networkReceiveRate = 25.0;  // Mbps
    profile.system.networkTransmitRate = 18.0;  // Mbps
    
    // Resource usage
    profile.system.cpuTime = 3600.0;  // seconds
    profile.system.userTime = 3200.0;
    profile.system.systemTime = 400.0;
    profile.system.processMemory = 4096;  // MB
}

void PerformanceProfiler::analyzeNeuronPerformance(PerformanceProfile& profile) const {
    // Analyze neuron performance
    if (profile.neuron.averageFiringRate > 10.0) {
        profile.analysis.strengths.push_back("High firing rates");
    }
    
    if (profile.neuron.stabilityScore > 0.8) {
        profile.analysis.strengths.push_back("Stable neuron dynamics");
    }
    
    if (profile.neuron.plasticityEfficiency > 0.8) {
        profile.analysis.strengths.push_back("High plasticity efficiency");
    }
    
    if (profile.neuron.averageFiringRate < 1.0) {
        profile.analysis.weaknesses.push_back("Low firing rates");
    }
    
    if (profile.neuron.membranePotentialVariance > 20.0) {
        profile.analysis.weaknesses.push_back("High membrane potential variability");
    }
}

void PerformanceProfiler::analyzeSynapsePerformance(PerformanceProfile& profile) const {
    // Analyze synapse performance
    if (profile.synapse.transmissionSuccessRate > 0.95) {
        profile.analysis.strengths.push_back("High transmission reliability");
    }
    
    if (profile.synapse.averageCompatibility > 0.8) {
        profile.analysis.strengths.push_back("Good synaptic compatibility");
    }
    
    if (profile.synapse.averageWeightChange < 0.01) {
        profile.analysis.weaknesses.push_back("Low weight change efficiency");
    }
    
    if (profile.synapse.synapseMaturation < 0.7) {
        profile.analysis.weaknesses.push_back("Incomplete synapse maturation");
    }
}

void PerformanceProfiler::analyzePlasticityPerformance(PerformanceProfile& profile) const {
    // Analyze plasticity performance
    if (profile.plasticity.convergenceRate > 0.9) {
        profile.analysis.strengths.push_back("Fast convergence");
    }
    
    if (profile.plasticity.stabilityScore > 0.8) {
        profile.analysis.strengths.push_back("High stability");
    }
    
    if (profile.plasticity.learningRate < 0.001) {
        profile.analysis.weaknesses.push_back("Slow learning rate");
    }
    
    if (profile.plasticity.entropy > 0.7) {
        profile.analysis.weaknesses.push_back("High system entropy (instability)");
    }
}

void PerformanceProfiler::analyzeNetworkPerformance(PerformanceProfile& profile) const {
    // Analyze network performance
    if (profile.network.synchronizationScore > 0.8) {
        profile.analysis.strengths.push_back("Good synchronization");
    }
    
    if (profile.network.clusteringCoefficient > 0.2) {
        profile.analysis.strengths.push_back("Good network connectivity");
    }
    
    if (profile.network.averagePathLength > 5.0) {
        profile.analysis.weaknesses.push_back("Long average path length (poor connectivity)");
    }
    
    if (profile.network.burstFrequency > 3.0) {
        profile.analysis.weaknesses.push_back("High burst frequency (instability)");
    }
}

void PerformanceProfiler::analyzeSystemPerformance(PerformanceProfile& profile) const {
    // Analyze system performance
    if (profile.system.cpuUsage < 0.7) {
        profile.analysis.strengths.push_back("Good CPU utilization");
    }
    
    if (profile.system.memoryAvailable > 4.0) {
        profile.analysis.strengths.push_back("Sufficient memory available");
    }
    
    if (profile.system.cpuUsage > 0.9) {
        profile.analysis.weaknesses.push_back("High CPU usage (potential bottleneck)");
    }
    
    if (profile.system.memoryAvailable < 1.0) {
        profile.analysis.weaknesses.push_back("Low available memory");
    }
}

std::string PerformanceProfiler::generateReport(const PerformanceProfile& profile) {
    std::ostringstream oss;
    oss << "=== Performance Profile Report ===" << std::endl;
    oss << "Generated: " << getCurrentTimestamp() << std::endl;
    oss << "Profile Type: " << getProfileType(profile) << std::endl;
    oss << "Duration: " << profile.totalDuration << " seconds" << std::endl;
    oss << std::endl;
    
    oss << "=== Neuron Performance ===" << std::endl;
    oss << "Total Neurons: " << profile.neuron.totalNeurons << std::endl;
    oss << "Active Neurons: " << profile.neuron.activeNeurons << std::endl;
    oss << "Average Firing Rate: " << profile.neuron.averageFiringRate << " Hz" << std::endl;
    oss << "Max Firing Rate: " << profile.neuron.maxFiringRate << " Hz" << std::endl;
    oss << std::endl;
    
    oss << "=== Synapse Performance ===" << std::endl;
    oss << "Total Synapses: " << profile.synapse.totalSynapses << std::endl;
    oss << "Average Weight: " << profile.synapse.averageWeight << std::endl;
    oss << "Transmission Success Rate: " << profile.synapse.transmissionSuccessRate * 100 << "%" << std::endl;
    oss << std::endl;
    
    oss << "=== Network Performance ===" << std::endl;
    oss << "Average Path Length: " << profile.network.averagePathLength << std::endl;
    oss << "Network Diameter: " << profile.network.networkDiameter << std::endl;
    oss << "Synchronization Score: " << profile.network.synchronizationScore << std::endl;
    oss << "Activity Spread: " << profile.network.activitySpread << std::endl;
    oss << std::endl;
    
    oss << "=== Overall Performance ===" << std::endl;
    oss << "Performance Grade: " << profile.analysis.performanceGrade << std::endl;
    oss << "Overall Score: " << profile.analysis.overallScore * 100 << "%" << std::endl;
    oss << "Bottlenecks: " << profile.analysis.bottlenecks.size() << std::endl;
    
    return oss.str();
}

std::string PerformanceProfiler::generateDetailedReport(const PerformanceProfile& profile) {
    std::ostringstream oss;
    oss << generateReport(profile) << std::endl;
    
    oss << "=== Detailed Analysis ===" << std::endl;
    oss << "Strengths:" << std::endl;
    for (const auto& strength : profile.analysis.strengths) {
        oss << "  - " << strength << std::endl;
    }
    
    oss << "Weaknesses:" << std::endl;
    for (const auto& weakness : profile.analysis.weaknesses) {
        oss << "  - " << weakness << std::endl;
    }
    
    oss << "Recommendations:" << std::endl;
    for (const auto& recommendation : profile.analysis.recommendations) {
        oss << "  - " << recommendation << std::endl;
    }
    
    oss << std::endl << "=== Trends ===" << std::endl;
    for (const auto& trend : profile.analysis.trends) {
        oss << "  - " << trend.metric << ": " << trend.direction 
            << " (rate: " << trend.rate << ", confidence: " << trend.confidence << ")" << std::endl;
    }
    
    return oss.str();
}

bool PerformanceProfiler::exportProfile(const std::string& filename, const PerformanceProfile& profile) {
    if (!ensureOutputDirectory(filename)) {
        return false;
    }
    
    std::string content;
    if (pImpl->config.outputFormat == "text") {
        content = generateDetailedReport(profile);
    } else if (pImpl->config.outputFormat == "json") {
        content = generateJSONReport(profile);
    } else if (pImpl->config.outputFormat == "csv") {
        content = generateCSVReport(profile);
    } else if (pImpl->config.outputFormat == "html") {
        content = generateHTMLReport(profile);
    } else {
        std::cerr << "Unsupported output format: " << pImpl->config.outputFormat << std::endl;
        return false;
    }
    
    return writeToFile(filename, content);
}

std::string PerformanceProfiler::analyzePerformance(const PerformanceProfile& profile) {
    std::ostringstream oss;
    oss << "Performance Analysis Report" << std::endl;
    oss << "================================" << std::endl;
    oss << std::endl;
    
    oss << "Overall Performance Grade: " << profile.analysis.performanceGrade << std::endl;
    oss << "Overall Score: " << (profile.analysis.overallScore * 100) << "%" << std::endl;
    oss << "Performance Level: " << getPerformanceLevel(profile.analysis.overallScore) << std::endl;
    oss << std::endl;
    
    oss << "Component Performance:" << std::endl;
    oss << "  Neuron Performance: " << calculateComponentScore(profile.neuron) << "/100" << std::endl;
    oss << "  Synapse Performance: " << calculateComponentScore(profile.synapse) << "/100" << std::endl;
    oss << "  Plasticity Performance: " << calculateComponentScore(profile.plasticity) << "/100" << std::endl;
    oss << "  Network Performance: " << calculateComponentScore(profile.network) << "/100" << std::endl;
    oss << "  System Performance: " << calculateComponentScore(profile.system) << "/100" << std::endl;
    oss << std::endl;
    
    oss << "Bottleneck Analysis:" << std::endl;
    bool hasBottlenecks = detectBottlenecks(profile);
    if (hasBottlenecks) {
        for (const auto& bottleneck : profile.analysis.bottlenecks) {
            oss << "  - " << bottleneck << std::endl;
        }
    } else {
        oss << "  No significant bottlenecks detected" << std::endl;
    }
    oss << std::endl;
    
    oss << "Recommendations:" << std::endl;
    for (const auto& recommendation : profile.analysis.recommendations) {
        oss << "  - " << recommendation << std::endl;
    }
    
    return oss.str();
}

std::vector<std::string> PerformanceProfiler::getPerformanceRecommendations(const PerformanceProfile& profile) {
    std::vector<std::string> recommendations;
    
    // Check for performance issues
    if (profile.system.cpuUsage > 0.9) {
        recommendations.push_back("Optimize CPU usage - consider reducing computational complexity");
    }
    
    if (profile.system.memoryAvailable < 2.0) {
        recommendations.push_back("Increase available memory - consider memory optimization");
    }
    
    if (profile.neuron.averageFiringRate < 1.0) {
        recommendations.push_back("Investigate low firing rates - check neuron parameters and input");
    }
    
    if (profile.synapse.transmissionSuccessRate < 0.95) {
        recommendations.push_back("Improve synaptic transmission reliability");
    }
    
    if (profile.network.synchronizationScore < 0.7) {
        recommendations.push_back("Enhance network synchronization - optimize connectivity");
    }
    
    if (profile.plasticity.convergenceRate < 0.9) {
        recommendations.push_back("Improve plasticity convergence - adjust learning parameters");
    }
    
    // Add general recommendations based on profile type
    std::string profileType = getProfileType(profile);
    if (profileType == "neurons") {
        recommendations.push_back("Consider neuron type specialization for better performance");
    } else if (profileType == "synapses") {
        recommendations.push_back("Optimize synaptic parameters for better transmission efficiency");
    } else if (profileType == "plasticity") {
        recommendations.push_back("Balance exploration and exploitation in plasticity rules");
    } else if (profileType == "network") {
        recommendations.push_back("Consider network topology optimization for better efficiency");
    }
    
    return recommendations;
}

bool PerformanceProfiler::detectBottlenecks(const PerformanceProfile& profile) {
    bool hasBottlenecks = false;
    
    // System bottlenecks
    if (profile.system.cpuUsage > 0.9) {
        profile.analysis.bottlenecks.push_back("CPU usage exceeds 90% - potential computational bottleneck");
        hasBottlenecks = true;
    }
    
    if (profile.system.memoryAvailable < 1.0) {
        profile.analysis.bottlenecks.push_back("Insufficient available memory - potential memory bottleneck");
        hasBottlenecks = true;
    }
    
    // Neuron bottlenecks
    if (profile.neuron.averageFiringRate < 0.5) {
        profile.analysis.bottlenecks.push_back("Low neuronal firing rates - potential information processing bottleneck");
        hasBottlenecks = true;
    }
    
    // Synapse bottlenecks
    if (profile.synapse.transmissionSuccessRate < 0.95) {
        profile.analysis.bottlenecks.push_back("Low synaptic transmission reliability - potential communication bottleneck");
        hasBottlenecks = true;
    }
    
    // Network bottlenecks
    if (profile.network.averagePathLength > 5.0) {
        profile.analysis.bottlenecks.push_back("Long average path length - potential signal propagation bottleneck");
        hasBottlenecks = true;
    }
    
    if (profile.network.synchronizationScore < 0.6) {
        profile.analysis.bottlenecks.push_back("Poor synchronization - potential coordination bottleneck");
        hasBottlenecks = true;
    }
    
    return hasBottlenecks;
}

std::vector<std::string> PerformanceProfiler::getActiveAlerts() {
    return pImpl->alerts;
}

void PerformanceProfiler::acknowledgeAlert(const std::string& alertId) {
    // Remove acknowledged alert
    auto it = std::remove(pImpl->alerts.begin(), pImpl->alerts.end(), alertId);
    pImpl->alerts.erase(it, pImpl->alerts.end());
}

void PerformanceProfiler::clearAlerts() {
    pImpl->alerts.clear();
}

bool PerformanceProfiler::integrateWithProfiler(std::shared_ptr<Profiler> profiler) {
    pImpl->profiler = profiler;
    return true;
}

bool PerformanceProfiler::integrateWithMonitor(std::shared_ptr<PerformanceMonitor> monitor) {
    pImpl->monitor = monitor;
    return true;
}

bool PerformanceProfiler::integrateWithMetrics(std::shared_ptr<MetricsCollector> metrics) {
    pImpl->metrics = metrics;
    return true;
}

bool PerformanceProfiler::setCustomMetric(const std::string& name, const std::function<double()>& getter) {
    pImpl->customMetrics[name] = getter;
    return true;
}

bool PerformanceProfiler::removeCustomMetric(const std::string& name) {
    auto it = pImpl->customMetrics.find(name);
    if (it != pImpl->customMetrics.end()) {
        pImpl->customMetrics.erase(it);
        return true;
    }
    return false;
}

bool PerformanceProfiler::exportToJSON(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Generate JSON report
    std::string jsonContent = generateJSONReport("");
    file << jsonContent;
    file.close();
    return true;
}

bool PerformanceProfiler::exportToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Generate CSV report
    std::string csvContent = generateCSVReport("");
    file << csvContent;
    file.close();
    return true;
}

bool PerformanceProfiler::exportToHTML(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Generate HTML report
    std::string htmlContent = generateHTMLReport("");
    file << htmlContent;
    file.close();
    return true;
}

bool PerformanceProfiler::isHealthy() const {
    if (!pImpl->profiling) {
        return false;
    }
    
    if (pImpl->profileHistory.empty()) {
        return false;
    }
    
    return true;
}

std::string PerformanceProfiler::getStatus() const {
    if (!pImpl->profiling) {
        return "Not profiling";
    }
    
    if (pImpl->profileHistory.empty()) {
        return "Profiling but no profiles generated";
    }
    
    return "Profiling active - " + std::to_string(pImpl->profileHistory.size()) + " profiles generated";
}

void PerformanceProfiler::profilingWorker() {
    while (pImpl->profiling) {
        if (pImpl->paused) {
            std::this_thread::sleep_for(pImpl->config.profilingInterval);
            continue;
        }
        
        // Generate profile
        generateProfile();
        
        // Sleep for the configured interval
        std::this_thread::sleep_for(pImpl->config.profilingInterval);
    }
}

void PerformanceProfiler::recordNeuronProfile(const std::string& neuronId, const std::unordered_map<std::string, double>& metrics) {
    std::string metricName = "neuron_" + neuronId;
    for (const auto& pair : metrics) {
        recordMetric(metricName + "_" + pair.first, pair.second);
    }
}

void PerformanceProfiler::recordSynapseProfile(const std::string& synapseId, const std::unordered_map<std::string, double>& metrics) {
    std::string metricName = "synapse_" + synapseId;
    for (const auto& pair : metrics) {
        recordMetric(metricName + "_" + pair.first, pair.second);
    }
}

void PerformanceProfiler::recordPlasticityProfile(const std::string& ruleName, const std::unordered_map<std::string, double>& metrics) {
    std::string metricName = "plasticity_" + ruleName;
    for (const auto& pair : metrics) {
        recordMetric(metricName + "_" + pair.first, pair.second);
    }
}

void PerformanceProfiler::recordNetworkProfile(const std::string& networkId, const std::unordered_map<std::string, double>& metrics) {
    std::string metricName = "network_" + networkId;
    for (const auto& pair : metrics) {
        recordMetric(metricName + "_" + pair.first, pair.second);
    }
}

void PerformanceProfiler::recordSystemProfile(const std::unordered_map<std::string, double>& metrics) {
    for (const auto& pair : metrics) {
        recordMetric("system_" + pair.first, pair.second);
    }
}

void PerformanceProfiler::recordMetric(const std::string& name, double value, const std::unordered_map<std::string, std::string>& tags) {
    // Add to metric series
    pImpl->metricSeries[name].push_back(value);
    
    // Update current profile if active
    if (!pImpl->profileHistory.empty()) {
        auto& lastProfile = pImpl->profileHistory.back();
        // Update relevant metrics based on name
        if (name.find("neuron") != std::string::npos) {
            // Update neuron metrics
        } else if (name.find("synapse") != std::string::npos) {
            // Update synapse metrics
        } else if (name.find("plasticity") != std::string::npos) {
            // Update plasticity metrics
        } else if (name.find("network") != std::string::npos) {
            // Update network metrics
        } else if (name.find("system") != std::string::npos) {
            // Update system metrics
        }
    }
}

void PerformanceProfiler::recordTimer(const std::string& name, std::chrono::duration<double> duration, 
                                       const std::unordered_map<std::string, std::string>& tags) {
    pImpl->timerSeries[name].push_back(duration.count());
}

void PerformanceProfiler::recordCounter(const std::string& name, size_t count, 
                                         const std::unordered_map<std::string, std::string>& tags) {
    recordMetric(name, static_cast<double>(count), tags);
}

void PerformanceProfiler::recordGauge(const std::string& name, double value, 
                                       const std::unordered_map<std::string, std::string>& tags) {
    recordMetric(name, value, tags);
}

// Helper functions
std::string PerformanceProfiler::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y%m%d_%H%M%S");
    return oss.str();
}

std::string PerformanceProfiler::getProfileType(const PerformanceProfile& profile) const {
    // Determine profile type based on dominant metrics
    std::string profileType = "comprehensive";
    
    if (profile.neuron.averageFiringRate > 10.0) {
        profileType = "neurons";
    } else if (profile.synapse.transmissionSuccessRate > 0.95) {
        profileType = "synapses";
    } else if (profile.plasticity.convergenceRate > 0.9) {
        profileType = "plasticity";
    } else if (profile.network.synchronizationScore > 0.8) {
        profileType = "network";
    }
    
    return profileType;
}

std::string PerformanceProfiler::getPerformanceLevel(double score) const {
    if (score >= 0.9) return "Excellent";
    if (score >= 0.8) return "Good";
    if (score >= 0.7) return "Fair";
    if (score >= 0.6) return "Poor";
    return "Critical";
}

double PerformanceProfiler::calculateOverallPerformanceScore(const PerformanceProfile& profile) const {
    // Calculate weighted average of component scores
    double neuronScore = calculateComponentScore(profile.neuron);
    double synapseScore = calculateComponentScore(profile.synapse);
    double plasticityScore = calculateComponentScore(profile.plasticity);
    double networkScore = calculateComponentScore(profile.network);
    double systemScore = calculateComponentScore(profile.system);
    
    // Weight components (neurons and network are more important for biological performance)
    double overallScore = (neuronScore * 0.3 + synapseScore * 0.2 + plasticityScore * 0.2 + 
                         networkScore * 0.2 + systemScore * 0.1);
    
    return std::max(0.0, std::min(1.0, overallScore));
}

double PerformanceProfiler::calculateComponentScore(const PerformanceProfile::NeuronPerformance& neuron) const {
    // Calculate score based on neuron metrics
    double firingScore = std::min(1.0, neuron.averageFiringRate / 20.0);
    double activityScore = neuron.activeNeurons / static_cast<double>(neuron.totalNeurons);
    double adaptationScore = neuron.plasticityEfficiency;
    
    return (firingScore * 0.4 + activityScore * 0.3 + adaptationScore * 0.3);
}

double PerformanceProfiler::calculateComponentScore(const PerformanceProfile::SynapsePerformance& synapse) const {
    // Calculate score based on synapse metrics
    double transmissionScore = synapse.transmissionSuccessRate;
    double compatibilityScore = synapse.averageCompatibility;
    double maturityScore = synapse.synapseMaturation;
    
    return (transmissionScore * 0.4 + compatibilityScore * 0.3 + maturityScore * 0.3);
}

double PerformanceProfiler::calculateComponentScore(const PerformanceProfile::PlasticityPerformance& plasticity) const {
    // Calculate score based on plasticity metrics
    double convergenceScore = plasticity.convergenceRate;
    double stabilityScore = plasticity.stabilityScore;
    double learningScore = std::min(1.0, plasticity.learningRate * 100.0);
    
    return (convergenceScore * 0.4 + stabilityScore * 0.3 + learningScore * 0.3);
}

double PerformanceProfiler::calculateComponentScore(const PerformanceProfile::NetworkPerformance& network) const {
    // Calculate score based on network metrics
    double synchronizationScore = network.synchronizationScore;
    double connectivityScore = std::min(1.0, network.clusteringCoefficient * 5.0);
    double efficiencyScore = network.computationalEfficiency;
    
    return (synchronizationScore * 0.4 + connectivityScore * 0.3 + efficiencyScore * 0.3);
}

double PerformanceProfiler::calculateComponentScore(const PerformanceProfile::SystemPerformance& system) const {
    // Calculate score based on system metrics
    double cpuScore = 1.0 - system.cpuUsage;  // Lower CPU usage is better
    double memoryScore = std::min(1.0, system.memoryAvailable / 8.0);
    double utilizationScore = 1.0 - std::max(0.0, system.cpuUsage - 0.7);  // Better utilization above 70%
    
    return (cpuScore * 0.4 + memoryScore * 0.3 + utilizationScore * 0.3);
}

size_t PerformanceProfiler::getTotalDataPoints(const PerformanceProfile& profile) const {
    size_t totalPoints = 0;
    
    // Estimate based on profile metrics
    totalPoints = 1000;  // Example: 1000 data points
    
    return totalPoints;
}

// Reporting generation
std::string PerformanceProfiler::generateJSONReport(const PerformanceProfile& profile) {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"profile_type\": \"" << getProfileType(profile) << "\"," << std::endl;
    oss << "  \"timestamp\": \"" << getCurrentTimestamp() << "\"," << std::endl;
    oss << "  \"duration_seconds\": " << profile.totalDuration << "," << std::endl;
    oss << "  \"performance_score\": " << (profile.analysis.overallScore * 100) << "," << std::endl;
    oss << "  \"performance_grade\": \"" << profile.analysis.performanceGrade << "\"," << std::endl;
    
    oss << "  \"neuron_performance\": {" << std::endl;
    oss << "    \"firing_rate\": " << profile.neuron.averageFiringRate << "," << std::endl;
    oss << "    \"active_ratio\": " << (profile.neuron.totalNeurons > 0 ? profile.neuron.activeNeurons / static_cast<double>(profile.neuron.totalNeurons) : 0.0) << "," << std::endl;
    oss << "    \"plasticity_efficiency\": " << profile.neuron.plasticityEfficiency << std::endl;
    oss << "  }," << std::endl;
    
    oss << "  \"system_performance\": {" << std::endl;
    oss << "    \"cpu_usage\": " << profile.system.cpuUsage << "," << std::endl;
    oss << "    \"memory_usage_mb\": " << profile.system.memoryUsage << "," << std::endl;
    oss << "    \"memory_available_mb\": " << profile.system.memoryAvailable << std::endl;
    oss << "  }," << std::endl;
    
    oss << "  \"alerts\": [" << std::endl;
    for (size_t i = 0; i < pImpl->alerts.size(); ++i) {
        oss << "    \"" << pImpl->alerts[i] << "\"";
        if (i < pImpl->alerts.size() - 1) oss << ",";
        oss << std::endl;
    }
    oss << "  ]" << std::endl;
    
    oss << "}" << std::endl;
    
    return oss.str();
}

std::string PerformanceProfiler::generateCSVReport(const PerformanceProfile& profile) {
    std::ostringstream oss;
    oss << "Metric,Value,Unit,Timestamp" << std::endl;
    oss << "Performance Score," << (profile.analysis.overallScore * 100) << "%,%," << getCurrentTimestamp() << std::endl;
    oss << "Neuron Firing Rate," << profile.neuron.averageFiringRate << " Hz,Hz," << getCurrentTimestamp() << std::endl;
    oss << "Active Neurons," << profile.neuron.activeNeurons << ",count," << getCurrentTimestamp() << std::endl;
    oss << "Synapse Transmission Success Rate," << profile.synapse.transmissionSuccessRate * 100 << "%,%,," << getCurrentTimestamp() << std::endl;
    oss << "Network Synchronization Score," << profile.network.synchronizationScore << ",%,," << getCurrentTimestamp() << std::endl;
    oss << "CPU Usage," << profile.system.cpuUsage * 100 << "%,%,," << getCurrentTimestamp() << std::endl;
    oss << "Memory Usage (MB)," << profile.system.memoryUsage << " MB,MB," << getCurrentTimestamp() << std::endl;
    
    return oss.str();
}

std::string PerformanceProfiler::generateHTMLReport(const PerformanceProfile& profile) {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "<title>Performance Profile Report</title>" << std::endl;
    oss << "<style>" << std::endl;
    oss << "body { font-family: Arial, sans-serif; margin: 40px; }" << std::endl;
    oss << "h1 { color: #333; }" << std::endl;
    oss << "h2 { color: #666; border-bottom: 2px solid #ccc; padding-bottom: 5px; }" << std::endl;
    oss << "table { border-collapse: collapse; width: 100%; margin: 20px 0; }" << std::endl;
    oss << "th, td { border: 1px solid #ddd; padding: 12px; text-align: left; }" << std::endl;
    oss << "th { background-color: #f2f2f2; font-weight: bold; }" << std::endl;
    oss << "tr:nth-child(even) { background-color: #f9f9f9; }" << std::endl;
    oss << ".metric-good { color: green; font-weight: bold; }" << std::endl;
    oss << ".metric-warning { color: orange; font-weight: bold; }" << std::endl;
    oss << ".metric-bad { color: red; font-weight: bold; }" << std::endl;
    oss << "</style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    oss << "<h1>Performance Profile Report</h1>" << std::endl;
    oss << "<p>Generated: " << getCurrentTimestamp() << "</p>" << std::endl;
    oss << "<p>Profile Type: " << getProfileType(profile) << "</p>" << std::endl;
    oss << "<p>Duration: " << profile.totalDuration << " seconds</p>" << std::endl;
    oss << "<h2>Performance Summary</h2>" << std::endl;
    oss << "<p>Overall Performance: <span class=\"metric-" << getPerformanceClass(profile.analysis.overallScore) << "\">" 
        << (profile.analysis.overallScore * 100) << "%</span> (Grade: " << profile.analysis.performanceGrade << ")</p>" << std::endl;
    
    oss << "<h2>Component Performance</h2>" << std::endl;
    oss << "<table>" << std::endl;
    oss << "<tr><th>Component</th><th>Score</th><th>Status</th></tr>" << std::endl;
    
    oss << "<tr><td>Neurons</td><td>" << calculateComponentScore(profile.neuron) * 100 << "%</td><td class=\"metric-" << getPerformanceClass(calculateComponentScore(profile.neuron)) << "\">Good</td></tr>" << std::endl;
    oss << "<tr><td>Synapses</td><td>" << calculateComponentScore(profile.synapse) * 100 << "%</td><td class=\"metric-" << getPerformanceClass(calculateComponentScore(profile.synapse)) << "\">Good</td></tr>" << std::endl;
    oss << "<tr><td>Plasticity</td><td>" << calculateComponentScore(profile.plasticity) * 100 << "%</td><td class=\"metric-" << getPerformanceClass(calculateComponentScore(profile.plasticity)) << "\">Good</td></tr>" << std::endl;
    oss << "<tr><td>Network</td><td>" << calculateComponentScore(profile.network) * 100 << "%</td><td class=\"metric-" << getPerformanceClass(calculateComponentScore(profile.network)) << "\">Good</td></tr>" << std::endl;
    oss << "<tr><td>System</td><td>" << calculateComponentScore(profile.system) * 100 << "%</td><td class=\"metric-" << getPerformanceClass(calculateComponentScore(profile.system)) << "\">Good</td></tr>" << std::endl;
    
    oss << "</table>" << std::endl;
    
    oss << "<h2>Bottlenecks</h2>" << std::endl;
    oss << "<ul>" << std::endl;
    for (const auto& bottleneck : profile.analysis.bottlenecks) {
        oss << "<li>" << bottleneck << "</li>" << std::endl;
    }
    oss << "</ul>" << std::endl;
    
    oss << "<h2>Recommendations</h2>" << std::endl;
    oss << "<ul>" << std::endl;
    for (const auto& recommendation : profile.analysis.recommendations) {
        oss << "<li>" << recommendation << "</li>" << std::endl;
    }
    oss << "</ul>" << std::endl;
    
    oss << "<h2>Alerts</h2>" << std::endl;
    oss << "<ul>" << std::endl;
    for (const auto& alert : pImpl->alerts) {
        oss << "<li>" << alert << "</li>" << std::endl;
    }
    oss << "</ul>" << std::endl;
    
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

std::string PerformanceProfiler::getPerformanceClass(double score) const {
    if (score >= 0.9) return "good";
    if (score >= 0.8) return "good";
    if (score >= 0.7) return "warning";
    if (score >= 0.6) return "warning";
    return "bad";
}

// Alert generation
void PerformanceProfiler::generatePerformanceAlerts(const PerformanceProfile& profile) {
    generateNeuronAlerts(profile);
    generateSynapseAlerts(profile);
    generatePlasticityAlerts(profile);
    generateNetworkAlerts(profile);
    generateSystemAlerts(profile);
}

void PerformanceProfiler::generateNeuronAlerts(const PerformanceProfile& profile) {
    if (profile.neuron.averageFiringRate < 1.0) {
        pImpl->alerts.push_back("Low neuronal firing rate - consider increasing input");
    }
    
    if (profile.neuron.plasticityEfficiency < 0.7) {
        pImpl->alerts.push_back("Low plasticity efficiency - consider adjusting learning parameters");
    }
}

void PerformanceProfiler::generateSynapseAlerts(const PerformanceProfile& profile) {
    if (profile.synapse.transmissionSuccessRate < 0.95) {
        pImpl->alerts.push_back("Low synaptic transmission reliability - check synapse compatibility");
    }
    
    if (profile.synapse.averageWeightChange > 0.1) {
        pImpl->alerts.push_back("High weight change - consider stabilizing synaptic weights");
    }
}

void PerformanceProfiler::generatePlasticityAlerts(const PerformanceProfile& profile) {
    if (profile.plasticity.convergenceRate < 0.8) {
        pImpl->alerts.push_back("Slow convergence - consider adjusting learning rate");
    }
    
    if (profile.plasticity.stabilityScore < 0.7) {
        pImpl->alerts.push_back("Low stability - consider adding regularization");
    }
}

void PerformanceProfiler::generateNetworkAlerts(const PerformanceProfile& profile) {
    if (profile.network.synchronizationScore < 0.7) {
        pImpl->alerts.push_back("Poor synchronization - optimize network connectivity");
    }
    
    if (profile.network.averagePathLength > 5.0) {
        pImpl->alerts.push_back("Long path length - consider network topology optimization");
    }
}

void PerformanceProfiler::generateSystemAlerts(const PerformanceProfile& profile) {
    if (profile.system.cpuUsage > 0.9) {
        pImpl->alerts.push_back("High CPU usage - potential computational bottleneck");
    }
    
    if (profile.system.memoryAvailable < 2.0) {
        pImpl->alerts.push_back("Low available memory - consider memory optimization");
    }
}

// File I/O
bool PerformanceProfiler::writeToFile(const std::string& filename, const std::string& content) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

bool PerformanceProfiler::ensureOutputDirectory(const std::string& filename) const {
    size_t pos = filename.find_last_of('/');
    if (pos != std::string::npos) {
        std::string directory = filename.substr(0, pos);
        if (!std::filesystem::exists(directory)) {
            return std::filesystem::create_directories(directory);
        }
    }
    return true;
}

std::vector<std::string> PerformanceProfiler::getPerformanceRecommendations(const PerformanceProfile& profile) {
    std::vector<std::string> recommendations;
    
    // Check for performance issues
    if (profile.system.cpuUsage > 0.9) {
        recommendations.push_back("Optimize CPU usage - consider reducing computational complexity");
    }
    
    if (profile.system.memoryAvailable < 2.0) {
        recommendations.push_back("Increase available memory - consider memory optimization");
    }
    
    if (profile.neuron.averageFiringRate < 1.0) {
        recommendations.push_back("Investigate low firing rates - check neuron parameters and input");
    }
    
    if (profile.synapse.transmissionSuccessRate < 0.95) {
        recommendations.push_back("Improve synaptic transmission reliability");
    }
    
    if (profile.network.synchronizationScore < 0.7) {
        recommendations.push_back("Enhance network synchronization - optimize connectivity");
    }
    
    if (profile.plasticity.convergenceRate < 0.9) {
        recommendations.push_back("Improve plasticity convergence - adjust learning parameters");
    }
    
    // Add general recommendations based on profile type
    std::string profileType = getProfileType(profile);
    if (profileType == "neurons") {
        recommendations.push_back("Consider neuron type specialization for better performance");
    } else if (profileType == "synapses") {
        recommendations.push_back("Optimize synaptic parameters for better transmission efficiency");
    } else if (profileType == "plasticity") {
        recommendations.push_back("Balance exploration and exploitation in plasticity rules");
    } else if (profileType == "network") {
        recommendations.push_back("Consider network topology optimization for better efficiency");
    }
    
    return recommendations;
}

bool PerformanceProfiler::detectBottlenecks(const PerformanceProfile& profile) {
    bool hasBottlenecks = false;
    
    // System bottlenecks
    if (profile.system.cpuUsage > 0.9) {
        profile.analysis.bottlenecks.push_back("CPU usage exceeds 90% - potential computational bottleneck");
        hasBottlenecks = true;
    }
    
    if (profile.system.memoryAvailable < 1.0) {
        profile.analysis.bottlenecks.push_back("Insufficient available memory - potential memory bottleneck");
        hasBottlenecks = true;
    }
    
    // Neuron bottlenecks
    if (profile.neuron.averageFiringRate < 0.5) {
        profile.analysis.bottlenecks.push_back("Low neuronal firing rates - potential information processing bottleneck");
        hasBottlenecks = true;
    }
    
    // Synapse bottlenecks
    if (profile.synapse.transmissionSuccessRate < 0.95) {
        profile.analysis.bottlenecks.push_back("Low synaptic transmission reliability - potential communication bottleneck");
        hasBottlenecks = true;
    }
    
    // Network bottlenecks
    if (profile.network.averagePathLength > 5.0) {
        profile.analysis.bottlenecks.push_back("Long average path length - potential signal propagation bottleneck");
        hasBottlenecks = true;
    }
    
    if (profile.network.synchronizationScore < 0.6) {
        profile.analysis.bottlenecks.push_back("Poor synchronization - potential coordination bottleneck");
        hasBottlenecks = true;
    }
    
    return hasBottlenecks;
}

std::string PerformanceProfiler::generateTextReport(const std::string& name) {
    std::ostringstream oss;
    oss << "=== Performance Profiler Report ===" << std::endl;
    oss << "Generated: " << getCurrentTimestamp() << std::endl;
    oss << "Profiling: " << (pImpl->profiling ? "Yes" : "No") << std::endl;
    oss << "Paused: " << (pImpl->paused ? "Yes" : "No") << std::endl;
    oss << std::endl;
    
    if (!name.empty()) {
        oss << "=== Metrics for " << name << " ===" << std::endl;
        oss << "Generating detailed report..." << std::endl;
    } else {
        oss << "=== All Metrics ===" << std::endl;
        oss << "Generating summary report..." << std::endl;
    }
    
    oss << "Total Profiles Generated: " << pImpl->totalProfilesGenerated << std::endl;
    oss << "Successful Profiles: " << pImpl->successfulProfiles << std::endl;
    oss << "Failed Profiles: " << pImpl->failedProfiles << std::endl;
    
    return oss.str();
}

} // namespace nlm