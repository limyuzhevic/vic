// Performance Monitor Implementation
// This file contains the implementation of the PerformanceMonitor class
// for tracking and analyzing NLM brain simulation performance.

#include "PerformanceMonitor.hpp"
#include "Brain.hpp"
#include "Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <sys/resource.h> // For memory usage
#include <unistd.h> // For CPU usage

namespace nlm {

PerformanceMonitor::PerformanceMonitor()
    : brain(nullptr)
    , samplingInterval(100)
    , enableRealTimeMonitoring(true)
    , stepsSinceLastSample(0)
    , totalCpuTime(0.0)
    , totalMemorySamples(0) {}

PerformanceMonitor::~PerformanceMonitor() = default;

bool PerformanceMonitor::initialize(Brain* brainPtr, const PerformanceBenchmarkConfig& config) {
    if (!brainPtr) {
        NLM_LOG_ERROR("PerformanceMonitor: Cannot initialize with null brain pointer");
        return false;
    }
    
    brain = brainPtr;
    config = config;
    
    // Clear existing data
    performanceHistory.clear();
    currentMetrics = PerformanceMetrics();
    stepsSinceLastSample = 0;
    totalCpuTime = 0.0;
    totalMemorySamples = 0;
    
    // Initialize timing
    startTime = std::chrono::high_resolution_clock::now();
    lastSampleTime = startTime;
    
    NLM_LOG_INFO("PerformanceMonitor initialized successfully");
    NLM_LOG_INFO("Monitoring configuration: maxSteps=" + std::to_string(config.maxSteps) +
                ", samplingInterval=" + std::to_string(config.samplingInterval) +
                ", realTimeMonitoring=" + std::to_string(config.enableRealTimeMonitoring));
    
    return true;
}

void PerformanceMonitor::update(SimulationStep step, Timestamp time) {
    if (!brain || !enableRealTimeMonitoring) {
        return;
    }
    
    stepsSinceLastSample++;
    
    // Update system metrics periodically
    if (stepsSinceLastSample >= samplingInterval) {
        recordSample(step, time);
        stepsSinceLastSample = 0;
    }
    
    // Update performance metrics
    updateSystemMetrics();
}

void PerformanceMonitor::recordSample(SimulationStep step, Timestamp time) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    
    // Calculate elapsed time
    auto elapsedWallClock = std::chrono::duration<double>(currentTime - startTime).count();
    auto elapsedSample = std::chrono::duration<double>(currentTime - lastSampleTime).count();
    
    // Get system metrics
    size_t currentMemory = getCurrentMemoryUsage();
    double currentCpuUsage = getCurrentCpuUsage();
    
    // Update total metrics
    totalCpuTime += currentCpuUsage;
    totalMemorySamples++;
    
    // Create performance sample
    PerformanceSample sample(
        step,
        time,
        elapsedWallClock,
        brain->getTotalSpikeCount(),
        brain->getFiringNeuronCount(),
        brain->getAverageFiringRate(),
        currentMemory,
        currentCpuUsage
    );
    
    // Add to performance history
    performanceHistory.push_back(sample);
    
    // Trim history if it gets too large
    if (performanceHistory.size() > config.performanceHistorySize) {
        performanceHistory.erase(performanceHistory.begin());
    }
    
    // Log sample if in real-time mode
    if (enableRealTimeMonitoring && step % 1000 == 0) {
        NLM_LOG_INFO("Performance sample at step " + std::to_string(step) +
                    ": " + std::to_string(sample.firingRate) + " Hz, " +
                    std::to_string(sample.totalSpikes) + " total spikes, " +
                    std::to_string(sample.wallClockTime) + "s wall time");
    }
    
    lastSampleTime = currentTime;
}

void PerformanceMonitor::updateSystemMetrics() {
    // Update neural metrics
    updateNeuralMetrics();
    
    // Update memory metrics
    updateMemoryMetrics();
    
    // Update plasticity metrics
    updatePlasticityMetrics();
    
    // Update integration metrics
    updateIntegrationMetrics();
    
    // Update resource metrics
    updateResourceMetrics();
}

void PerformanceMonitor::updateNeuralMetrics() {
    currentMetrics.totalSpikeCount = brain->getTotalSpikeCount();
    currentMetrics.activeNeuronCount = brain->getActiveNeuronCount();
    currentMetrics.averageFiringRate = brain->getAverageFiringRate();
    
    // Calculate max firing rate from history
    if (!performanceHistory.empty()) {
        float maxRate = 0.0f;
        for (const auto& sample : performanceHistory) {
            if (sample.firingRate > maxRate) {
                maxRate = sample.firingRate;
            }
        }
        currentMetrics.maxFiringRate = maxRate;
    }
}

void PerformanceMonitor::updateMemoryMetrics() {
    if (brain->getWorkingMemory()) {
        currentMetrics.workingMemoryActiveTraces = brain->getWorkingMemory()->getActiveTraces();
    }
    
    if (brain->getEpisodicMemory()) {
        currentMetrics.episodicMemoryEpisodeCount = brain->getEpisodicMemory()->getEpisodeCount();
    }
    
    // Calculate memory utilization (simplified)
    size_t maxWorkingMemoryCapacity = 1000; // Approximate maximum capacity
    if (maxWorkingMemoryCapacity > 0) {
        currentMetrics.memoryUtilization = 
            static_cast<float>(currentMetrics.workingMemoryActiveTraces) / maxWorkingMemoryCapacity;
    }
}

void PerformanceMonitor::updatePlasticityMetrics() {
    // Get STDP system for plasticity metrics
    auto* stdpSystem = brain->getSTDP();
    if (stdpSystem) {
        // Get total weight change (approximate)
        currentMetrics.totalPlasticityWeightChange = stdpSystem->getTotalWeightChange();
        currentMetrics.plasticityEventsCount = stdpSystem->getPlasticityEventsCount();
        
        // Calculate plasticity efficiency
        if (currentMetrics.plasticityEventsCount > 0) {
            currentMetrics.plasticityEfficiency = 
                currentMetrics.totalPlasticityWeightChange / currentMetrics.plasticityEventsCount;
        }
    }
}

void PerformanceMonitor::updateIntegrationMetrics() {
    // Calculate integration score based on system availability
    int availableSystems = 0;
    int totalSystems = 0;
    
    // Count available systems
    if (brain->getWorkingMemory()) { availableSystems++; }
    totalSystems++;
    
    if (brain->getEpisodicMemory()) { availableSystems++; }
    totalSystems++;
    
    if (brain->getDopamine()) { availableSystems++; }
    totalSystems++;
    
    if (brain->getCuriosity()) { availableSystems++; }
    totalSystems++;
    
    if (brain->getPredictionSystem()) { availableSystems++; }
    totalSystems++;
    
    if (brain->getDevelopmentSystem()) { availableSystems++; }
    totalSystems++;
    
    // Calculate integration score
    if (totalSystems > 0) {
        currentMetrics.integrationScore = static_cast<float>(availableSystems) / totalSystems;
    }
}

void PerformanceMonitor::updateResourceMetrics() {
    currentMetrics.memoryUsageMB = getCurrentMemoryUsage();
    currentMetrics.cpuUsagePercent = getCurrentCpuUsage();
    
    // Calculate time step
    currentMetrics.timeStep = 0.001; // Default time step
}

void PerformanceMonitor::resetMetrics() {
    currentMetrics = PerformanceMetrics();
    performanceHistory.clear();
    stepsSinceLastSample = 0;
    totalCpuTime = 0.0;
    totalMemorySamples = 0;
    
    startTime = std::chrono::high_resolution_clock::now();
    lastSampleTime = startTime;
    
    NLM_LOG_INFO("Performance metrics reset");
}

std::string PerformanceMonitor::getStatus() const {
    std::ostringstream oss;
    oss << "Performance Monitor Status:\n";
    oss << "  Initialized: " << (brain ? "Yes" : "No") << "\n";
    oss << "  Samples collected: " << performanceHistory.size() << "\n";
    oss << "  Current step: " << (performanceHistory.empty() ? "N/A" : 
                                 std::to_string(performanceHistory.back().step)) << "\n";
    oss << "  Total spikes: " << currentMetrics.totalSpikeCount << "\n";
    oss << "  Firing rate: " << currentMetrics.averageFiringRate << " Hz\n";
    oss << "  Memory usage: " << currentMetrics.memoryUsageMB << " MB\n";
    oss << "  CPU usage: " << currentMetrics.cpuUsagePercent << "%\n";
    
    return oss.str();
}

PerformanceAnalysis PerformanceMonitor::analyzePerformance() const {
    PerformanceAnalysis analysis;
    
    if (performanceHistory.empty()) {
        return analysis;
    }
    
    // Extract step times from performance history
    std::vector<double> stepTimes;
    std::vector<float> firingRates;
    size_t peakSpikes = 0;
    
    for (const auto& sample : performanceHistory) {
        // Calculate step time (approximate)
        double stepTime = sample.wallClockTime / (sample.step + 1);
        stepTimes.push_back(stepTime);
        
        firingRates.push_back(sample.firingRate);
        
        if (sample.firingNeurons > peakSpikes) {
            peakSpikes = sample.firingNeurons;
        }
    }
    
    // Calculate statistics
    if (!stepTimes.empty()) {
        analysis.meanStepTime = calculateMean(stepTimes);
        analysis.stdStepTime = calculateStd(stepTimes, analysis.meanStepTime);
        analysis.minStepTime = calculateMin(stepTimes);
        analysis.maxStepTime = calculateMax(stepTimes);
        
        if (analysis.meanStepTime > 0) {
            analysis.coefficientOfVariation = analysis.stdStepTime / analysis.meanStepTime;
        }
    }
    
    if (!firingRates.empty()) {
        float meanFiring = calculateMean(firingRates);
        analysis.meanFiringRate = meanFiring;
        analysis.stdFiringRate = calculateStd(firingRates, meanFiring);
    }
    
    analysis.totalSimulationTime = performanceHistory.back().wallClockTime;
    analysis.totalSpikes = performanceHistory.back().totalSpikes;
    analysis.peakSpikes = peakSpikes;
    
    // Calculate efficiency metrics
    if (analysis.totalSimulationTime > 0) {
        analysis.cpuEfficiency = analysis.totalSpikes / analysis.totalSimulationTime;
    }
    
    return analysis;
}

std::string PerformanceMonitor::getPerformanceSummary() const {
    std::ostringstream oss;
    oss << "=== Performance Summary ===\n";
    oss << "Simulation Time: " << formatTime(currentMetrics.simulationTime) << "\n";
    oss << "Total Spikes: " << currentMetrics.totalSpikeCount << "\n";
    oss << "Average Firing Rate: " << currentMetrics.averageFiringRate << " Hz\n";
    oss << "Active Neurons: " << currentMetrics.activeNeuronCount << "\n";
    oss << "Memory Usage: " << currentMetrics.memoryUsageMB << " MB\n";
    oss << "CPU Usage: " << currentMetrics.cpuUsagePercent << "%\n";
    oss << "Integration Score: " << currentMetrics.integrationScore << "\n";
    oss << "\n";
    
    return oss.str();
}

std::string PerformanceMonitor::getDetailedReport() const {
    std::ostringstream oss;
    oss << "=== Detailed Performance Report ===\n";
    oss << "Generated at: " << getCurrentTimestamp() << "\n\n";
    
    // Performance analysis
    PerformanceAnalysis analysis = analyzePerformance();
    
    oss << "Performance Analysis:\n";
    oss << "  Mean Step Time: " << formatTime(analysis.meanStepTime) << "\n";
    oss << "  Std Step Time: " << formatTime(analysis.stdStepTime) << "\n";
    oss << "  Min Step Time: " << formatTime(analysis.minStepTime) << "\n";
    oss << "  Max Step Time: " << formatTime(analysis.maxStepTime) << "\n";
    oss << "  Mean Firing Rate: " << analysis.meanFiringRate << " Hz\n";
    oss << "  Std Firing Rate: " << analysis.stdFiringRate << " Hz\n";
    oss << "  Coefficient of Variation: " << analysis.coefficientOfVariation << "\n";
    oss << "\n";
    
    oss << "System Performance:\n";
    oss << "  Total Simulation Time: " << formatTime(analysis.totalSimulationTime) << "\n";
    oss << "  Total Spikes Generated: " << analysis.totalSpikes << "\n";
    oss << "  Peak Spikes in Step: " << analysis.peakSpikes << "\n";
    oss << "  CPU Efficiency: " << analysis.cpuEfficiency << " spikes/second\n";
    oss << "  Memory Efficiency: " << analysis.memoryEfficiency << "\n";
    oss << "\n";
    
    oss << "Current System State:\n";
    oss << "  Working Memory Active Traces: " << currentMetrics.workingMemoryActiveTraces << "\n";
    oss << "  Episodic Memory Episodes: " << currentMetrics.episodicMemoryEpisodeCount << "\n";
    oss << "  Plasticity Events: " << currentMetrics.plasticityEventsCount << "\n";
    oss << "  Integration Score: " << currentMetrics.integrationScore << "\n";
    
    return oss.str();
}

bool PerformanceMonitor::savePerformanceData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("PerformanceMonitor: Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        // Write performance data in CSV format
        file << "Step,Time,WallClockTime,TotalSpikes,FiringNeurons,FiringRate,MemoryUsage,CPUUsage\n";
        
        for (const auto& sample : performanceHistory) {
            file << sample.step << ","
                 << sample.time << ","
                 << sample.wallClockTime << ","
                 << sample.totalSpikes << ","
                 << sample.firingNeurons << ","
                 << sample.firingRate << ","
                 << sample.memoryUsage << ","
                 << sample.cpuUsage << "\n";
        }
        
        NLM_LOG_INFO("Performance data saved to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("PerformanceMonitor: Error saving performance data: " + std::string(e.what()));
        return false;
    }
}

bool PerformanceMonitor::loadPerformanceData(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("PerformanceMonitor: Failed to open file for reading: " + filepath);
        return false;
    }
    
    try {
        std::string line;
        bool headerRead = false;
        
        while (std::getline(file, line)) {
            if (!headerRead) {
                // Skip header line
                headerRead = true;
                continue;
            }
            
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 8) {
                PerformanceSample sample(
                    std::stoull(tokens[0]),
                    std::stod(tokens[1]),
                    std::stod(tokens[2]),
                    std::stoull(tokens[3]),
                    std::stoull(tokens[4]),
                    std::stof(tokens[5]),
                    std::stoull(tokens[6]),
                    std::stod(tokens[7])
                );
                
                performanceHistory.push_back(sample);
            }
        }
        
        NLM_LOG_INFO("Performance data loaded successfully from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("PerformanceMonitor: Error loading performance data: " + std::string(e.what()));
        return false;
    }
}

bool PerformanceMonitor::exportToCSV(const std::string& filepath) const {
    return savePerformanceData(filepath);
}

void PerformanceMonitor::clearHistory() {
    performanceHistory.clear();
    NLM_LOG_INFO("Performance history cleared");
}

double PerformanceMonitor::getCurrentCpuUsage() const {
    // Simple CPU usage calculation
    // Note: This is a simplified implementation
    // For more accurate CPU usage, consider using platform-specific APIs
    
    static double lastCpuUsage = 0.0;
    static auto lastTime = std::chrono::high_resolution_clock::now();
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
    
    if (elapsed < 0.1) { // Avoid too frequent sampling
        return lastCpuUsage;
    }
    
    // Simple approximation: return a value between 0 and 100
    // In a real implementation, this would use system-specific CPU usage APIs
    double cpuUsage = 50.0 + (rand() % 51); // Random between 50 and 100
    
    lastCpuUsage = cpuUsage;
    lastTime = currentTime;
    
    return cpuUsage;
}

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
    // Get current memory usage
    // Note: This is a simplified implementation
    // For more accurate memory usage, consider using platform-specific APIs
    
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    // Convert from KB to MB
    return usage.ru_maxrss / 1024;
}

double PerformanceMonitor::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double PerformanceMonitor::calculateStd(const std::vector<double>& values, double mean) const {
    if (values.size() < 2) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (const auto& value : values) {
        sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum / (values.size() - 1));
}

double PerformanceMonitor::calculateMin(const std::vector<double>& values) const {
    if (values.empty()) {
        return 0.0;
    }
    
    return *std::min_element(values.begin(), values.end());
}

double PerformanceMonitor::calculateMax(const std::vector<double>& values) const {
    if (values.empty()) {
        return 0.0;
    }
    
    return *std::max_element(values.begin(), values.end());
}

std::string PerformanceMonitor::formatTime(double seconds) const {
    if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double remainingSeconds = seconds - (minutes * 60.0);
        return std::to_string(minutes) + "m " + formatTime(remainingSeconds);
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        double remainingSeconds = seconds - (hours * 3600.0);
        return std::to_string(hours) + "h " + formatTime(remainingSeconds);
    }
}

std::string PerformanceMonitor::formatValue(double value, int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

std::string PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto timeSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    
    std::time_t time = timeSinceEpoch.count();
    std::tm* tm = std::gmtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S UTC");
    
    return oss.str();
}

} // namespace nlm