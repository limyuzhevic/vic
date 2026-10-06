#include "MemoryAnalysisTools.hpp"
#include "core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations
class Brain;

MemoryAnalysisTools::MemoryAnalysisTools()
    : isMonitoring_(false)
    , brain_(nullptr)
    , currentStep_(0) {
    pImpl = std::make_unique<struct Impl>();
    startTime_ = std::chrono::steady_clock::now();
}

MemoryAnalysisTools::~MemoryAnalysisTools() {
    stopRealTimeMonitoring();
}

void MemoryAnalysisTools::integrateWithBrain(Brain* brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    brain_ = brain;
}

void MemoryAnalysisTools::updateAnalysisData() {
    std::lock_guard<std::mutex> lock(mutex_);
    currentStep_++;
}

MemoryStats MemoryAnalysisTools::analyzeWorkingMemory(const NeuralWorkingMemory* workingMemory) const {
    MemoryStats stats{};
    
    if (!workingMemory) {
        return stats;
    }

    stats.workingMemoryUsage = workingMemory->getActiveTraces() * sizeof(size_t);
    stats.capacity = workingMemory->getCapacity();
    stats.utilizationRatio = calculateUtilizationRatio(workingMemory->getActiveTraces(), 
                                                       workingMemory->getCapacity());

    return stats;
}

MemoryStats MemoryAnalysisTools::analyzeEpisodicMemory(const NeuralEpisodicMemory* episodicMemory) const {
    MemoryStats stats{};
    
    if (!episodicMemory) {
        return stats;
    }

    stats.episodicMemoryUsage = episodicMemory->getEpisodeCount() * sizeof(EpisodicMemoryItem);
    stats.totalMemoryUsage = stats.workingMemoryUsage + stats.episodicMemoryUsage;

    return stats;
}

ConsolidationStats MemoryAnalysisTools::analyzeConsolidation(const std::vector<double>& consolidationMetrics) const {
    ConsolidationStats stats{};

    if (consolidationMetrics.empty()) {
        return stats;
    }

    stats.peakConsolidation = *std::max_element(consolidationMetrics.begin(), consolidationMetrics.end());

    auto maxIt = std::max_element(consolidationMetrics.begin(), consolidationMetrics.end());
    stats.timeToPeak = std::distance(consolidationMetrics.begin(), maxIt);

    double initial = consolidationMetrics.front();
    double final = consolidationMetrics.back();
    
    if (final > 0 && initial > 0) {
        stats.halfLife = std::log(0.5) / std::log(final / initial);
    }

    stats.finalConsolidation = final;

    return stats;
}

ReplayStats MemoryAnalysisTools::analyzeReplay(const std::vector<const EpisodicMemoryItem*>& episodes) const {
    ReplayStats stats{};

    if (episodes.empty()) {
        return stats;
    }

    double totalEffectiveness = 0.0;
    double totalPreservation = 0.0;
    double totalIntegration = 0.0;

    for (const auto* episode : episodes) {
        // Simple effectiveness calculation based on episode characteristics
        double effectiveness = (episode->novelty + episode->reward) / 2.0;
        double preservation = std::min(1.0, episode->age / 100.0);
        double integration = episode->resultingReward / (episode->reward + 0.001);

        totalEffectiveness += effectiveness;
        totalPreservation += preservation;
        totalIntegration += integration;
    }

    size_t count = episodes.size();
    stats.effectiveness = totalEffectiveness / count;
    stats.patternPreservation = totalPreservation / count;
    stats.integrationScore = totalIntegration / count;
    stats.replayCount = count;

    return stats;
}

MemoryProfile MemoryAnalysisTools::profileMemoryAllocation(const std::string& name,
                                                          const std::function<void()>& allocationFunc) const {
    return performMemoryProfile(name, allocationFunc);
}

LeakAnalysis MemoryAnalysisTools::detectMemoryLeaks(const std::vector<uint64_t>& createdObjects,
                                                  const std::vector<uint64_t>& deletedObjects) const {
    return performLeakAnalysis(createdObjects, deletedObjects);
}

FragmentationAnalysis MemoryAnalysisTools::analyzeFragmentation(const NeuralWorkingMemory* workingMemory) const {
    return performFragmentationAnalysis(workingMemory);
}

UsagePattern MemoryAnalysisTools::analyzeUsagePatterns(const NeuralWorkingMemory* workingMemory,
                                                     const NeuralEpisodicMemory* episodicMemory) const {
    return performUsagePatternAnalysis(workingMemory, episodicMemory);
}

MemoryPrediction MemoryAnalysisTools::predictMemoryUsage(size_t neurons, size_t regions,
                                                        float connectionProbability) const {
    return performMemoryPrediction(neurons, regions, connectionProbability);
}

MemoryStats MemoryAnalysisTools::analyzeMemoryUsage(const Config& config) const {
    return performConfigAnalysis(config);
}

BenchmarkResult MemoryAnalysisTools::benchmarkMemoryAllocation(const Config& config) const {
    return performBenchmark(config);
}

void MemoryAnalysisTools::exportAnalysis(const std::string& filepath) const {
    performExport(filepath);
}

std::string MemoryAnalysisTools::generateOptimizationReport() const {
    return performReportGeneration();
}

void MemoryAnalysisTools::startRealTimeMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (isMonitoring_) {
        return;
    }

    isMonitoring_ = true;
    monitoringThread_ = std::thread(&MemoryAnalysisTools::runMonitoringLoop, this);
}

void MemoryAnalysisTools::stopRealTimeMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    isMonitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

MemoryProfile MemoryAnalysisTools::performMemoryProfile(const std::string& name,
                                                       const std::function<void()>& allocationFunc) const {
    MemoryProfile profile{name};

    auto startTime = std::chrono::steady_clock::now();
    
    std::vector<MemoryAllocation> allocations;
    size_t peakMemory = 0;

    // Record allocations before
    for (const auto& alloc : pImpl->allocationHistory) {
        allocations.push_back(alloc);
        peakMemory = std::max(peakMemory, alloc.size);
    }

    // Execute allocation function
    allocationFunc();

    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    // Calculate statistics
    profile.allocationCount = pImpl->allocationHistory.size();
    profile.averageAllocationTime = profile.allocationCount > 0 ? duration / profile.allocationCount : 0.0;
    profile.peakMemoryUsage = peakMemory;
    profile.memoryGrowthRate = calculateMemoryGrowthRate(pImpl->allocationHistory);
    profile.allocationHistory = pImpl->allocationHistory;

    return profile;
}

LeakAnalysis MemoryAnalysisTools::performLeakAnalysis(const std::vector<uint64_t>& createdObjects,
                                                     const std::vector<uint64_t>& deletedObjects) const {
    LeakAnalysis analysis{};

    size_t createdCount = createdObjects.size();
    size_t deletedCount = deletedObjects.size();
    size_t netIncrease = createdCount - deletedCount;

    analysis.leakRate = calculateLeakRate(createdCount, deletedCount, createdCount);
    analysis.leakDetected = (netIncrease > 0) && (analysis.leakRate > 10.0); // Threshold for leak detection
    analysis.growthTrend = determineGrowthTrend(pImpl->leakHistory);

    if (analysis.leakDetected) {
        analysis.suggestedCleanup = "Investigate unreleased traces in working memory";
    }

    return analysis;
}

FragmentationAnalysis MemoryAnalysisTools::performFragmentationAnalysis(const NeuralWorkingMemory* workingMemory) const {
    FragmentationAnalysis analysis{};

    if (!workingMemory) {
        return analysis;
    }

    // Simple fragmentation analysis based on active traces
    size_t totalCapacity = workingMemory->getCapacity();
    size_t activeTraces = workingMemory->getActiveTraces();
    
    if (totalCapacity > 0) {
        analysis.fragmentationLevel = 1.0 - static_cast<double>(activeTraces) / totalCapacity;
        analysis.needsDefragmentation = (analysis.fragmentationLevel > 0.3); // Threshold
        analysis.wastedMemory = static_cast<size_t>(analysis.fragmentationLevel * totalCapacity);
        analysis.optimizationPotential = std::min(100.0, analysis.fragmentationLevel * 100.0);
    }

    return analysis;
}

UsagePattern MemoryAnalysisTools::performUsagePatternAnalysis(const NeuralWorkingMemory* workingMemory,
                                                             const NeuralEpisodicMemory* episodicMemory) const {
    UsagePattern pattern;
    
    std::vector<double> usageHistory;
    
    if (workingMemory) {
        usageHistory.push_back(static_cast<double>(workingMemory->getActiveTraces()) / 
                              workingMemory->getCapacity());
    }
    
    if (episodicMemory) {
        usageHistory.push_back(static_cast<double>(episodicMemory->getEpisodeCount()) / 100.0);
    }

    if (!usageHistory.empty()) {
        pattern.peakUsageTime = static_cast<SimulationStep>(usageHistory.size() - 1);
        pattern.periodicity = calculatePeriodicity(usageHistory);
        pattern.predictiveCompressionRatio = calculateCompressionRatio(usageHistory);
        pattern.compressionEfficiency = calculateCompressionEfficiency(usageHistory);
        pattern.usageHistory = usageHistory;
    }

    return pattern;
}

std::vector<OptimizationRecommendation> MemoryAnalysisTools::performOptimizationRecommendations() const {
    std::vector<OptimizationRecommendation> recommendations;

    double currentUtilization = pImpl->currentUtilization;
    double currentFragmentation = pImpl->currentFragmentation;
    size_t currentCapacity = pImpl->currentCapacity;

    if (currentUtilization > 0.8) {
        OptimizationRecommendation rec;
        rec.area = "Capacity Scaling";
        rec.suggestion = "Increase working memory capacity to prevent overflow";
        rec.potentialImprovement = (currentUtilization - 0.8) * 50.0;
        rec.implementationComplexity = "Medium";
        recommendations.push_back(rec);
    }

    if (currentFragmentation > 0.3) {
        OptimizationRecommendation rec;
        rec.area = "Memory Defragmentation";
        rec.suggestion = "Implement periodic memory consolidation to reduce fragmentation";
        rec.potentialImprovement = currentFragmentation * 30.0;
        rec.implementationComplexity = "High";
        recommendations.push_back(rec);
    }

    if (pImpl->currentLeakRate > 5.0) {
        OptimizationRecommendation rec;
        rec.area = "Leak Prevention";
        rec.suggestion = "Implement automatic trace cleanup for unused working memory entries";
        rec.potentialImprovement = pImpl->currentLeakRate * 10.0;
        rec.implementationComplexity = "Medium";
        recommendations.push_back(rec);
    }

    return recommendations;
}

MemoryPrediction MemoryAnalysisTools::performMemoryPrediction(size_t neurons, size_t regions,
                                                            float connectionProbability) const {
    MemoryPrediction prediction{};

    // Simple prediction model based on neuron count
    double baseMemoryPerNeuron = 1.5; // KB per neuron
    double baseMemoryPerRegion = 10.0; // KB per region
    double connectionOverhead = neurons * regions * connectionProbability * 0.8;

    prediction.predictedMemoryUsage = static_cast<size_t>(
        neurons * baseMemoryPerNeuron + 
        regions * baseMemoryPerRegion + 
        connectionOverhead
    );

    prediction.confidence = std::min(1.0, 1.0 - (connectionProbability / 2.0)); // Higher confidence for lower connectivity
    prediction.uncertainty = prediction.predictedMemoryUsage * 0.1; // 10% uncertainty

    // Generate trend predictions
    for (int i = 1; i <= 5; ++i) {
        size_t trendValue = prediction.predictedMemoryUsage * (1 + i * 0.1); // 10% growth per period
        prediction.predictedTrends.push_back(trendValue);
    }

    return prediction;
}

MemoryStats MemoryAnalysisTools::performConfigAnalysis(const Config& config) const {
    MemoryStats stats{};

    // Extract configuration parameters
    int neuronCount = config.get<int>("neuron_count", 1000);
    int regionCount = config.get<int>("region_count", 4);
    float connectionProb = config.get<float>("connection_probability", 0.15f);

    // Estimate memory usage based on configuration
    double memoryPerNeuron = 1.5; // KB
    double memoryPerRegion = 10.0; // KB
    double connectionMemory = neuronCount * regionCount * connectionProb * 0.8; // KB

    stats.totalMemoryUsage = static_cast<size_t>(
        neuronCount * memoryPerNeuron + 
        regionCount * memoryPerRegion + 
        connectionMemory
    );

    stats.workingMemoryUsage = static_cast<size_t>(stats.totalMemoryUsage * 0.3); // Estimate 30% for working memory
    stats.episodicMemoryUsage = static_cast<size_t>(stats.totalMemoryUsage * 0.5); // Estimate 50% for episodic memory
    stats.associativeMemoryUsage = stats.totalMemoryUsage - stats.workingMemoryUsage - stats.episodicMemoryUsage;

    stats.memoryEfficiency = 1.0 - (connectionMemory / stats.totalMemoryUsage); // Efficiency based on connection overhead
    stats.utilizationRatio = 0.7; // Estimate 70% utilization
    stats.capacity = neuronCount;

    return stats;
}

BenchmarkResult MemoryAnalysisTools::performBenchmark(const Config& config) const {
    BenchmarkResult result{};

    auto startTime = std::chrono::steady_clock::now();
    size_t allocationsCompleted = 0;

    // Simulate memory allocations
    const size_t allocationCount = 1000;
    std::vector<std::vector<float>> allocatedData;

    for (size_t i = 0; i < allocationCount; ++i) {
        allocatedData.emplace_back(config.get<int>("neuron_count", 1000) / 100, 0.0f);
        ++allocationsCompleted;
    }

    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    // Calculate results
    result.allocationsPerSecond = allocationsCompleted / (duration / 1000.0); // allocations per second
    result.averageAllocationTime = duration / allocationsCompleted; // ms per allocation
    result.peakThroughput = allocationsCompleted / (duration / 1000.0) / 1000.0; // allocations per ms
    result.efficiencyScore = std::min(100.0, 100.0 * (1.0 - (duration / 10000.0))); // Higher for faster times

    return result;
}

void MemoryAnalysisTools::performExport(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }

    file << "{\n";
    file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    file << "  \"workingMemoryUsage\": " << pImpl->currentWorkingMemoryUsage << ",\n";
    file << "  \"episodicMemoryUsage\": " << pImpl->currentEpisodicMemoryUsage << ",\n";
    file << "  \"leakRate\": " << pImpl->currentLeakRate << ",\n";
    file << "  \"fragmentationLevel\": " << pImpl->currentFragmentation << ",\n";
    file << "  \"utilizationRatio\": " << pImpl->currentUtilization << ",\n";
    file << "  \"capacity\": " << pImpl->currentCapacity << ",\n";
    file << "  \"optimizationRecommendations\": [\n";

    auto recommendations = performOptimizationRecommendations();
    for (size_t i = 0; i < recommendations.size(); ++i) {
        const auto& rec = recommendations[i];
        file << "    {\n";
        file << "      \"area\": \"" << rec.area << "\",\n";
        file << "      \"suggestion\": \"" << rec.suggestion << "\",\n";
        file << "      \"potentialImprovement\": " << rec.potentialImprovement << ",\n";
        file << "      \"implementationComplexity\": \"" << rec.implementationComplexity << "\"\n";
        if (i < recommendations.size() - 1) {
            file << "    },\n";
        } else {
            file << "    }\n";
        }
    }

    file << "  ]\n";
    file << "}\n";
    file.close();
}

std::string MemoryAnalysisTools::performReportGeneration() const {
    std::stringstream report;

    report << "MEMORY OPTIMIZATION REPORT\n";
    report << "=============================\n\n";

    report << "Current Status:\n";
    report << "  Working Memory Usage: " << pImpl->currentWorkingMemoryUsage << "KB\n";
    report << "  Episodic Memory Usage: " << pImpl->currentEpisodicMemoryUsage << "KB\n";
    report << "  Memory Leak Rate: " << pImpl->currentLeakRate << " leaks/1000 ops\n";
    report << "  Fragmentation Level: " << (pImpl->currentFragmentation * 100) << "%\n";
    report << "  Utilization Ratio: " << (pImpl->currentUtilization * 100) << "%\n";
    report << "  Total Capacity: " << pImpl->currentCapacity << " traces\n\n";

    auto recommendations = performOptimizationRecommendations();
    if (!recommendations.empty()) {
        report << "Optimization Recommendations:\n";
        for (const auto& rec : recommendations) {
            report << "  • [" << rec.area << "] " << rec.suggestion << "\n";
            report << "    Potential improvement: " << rec.potentialImprovement << "%\n";
            report << "    Implementation complexity: " << rec.implementationComplexity << "\n\n";
        }
    } else {
        report << "No optimization recommendations at this time.\n\n";
    }

    report << "Generated: " << getCurrentTimestamp() << "\n";

    return report.str();
}

void MemoryAnalysisTools::runMonitoringLoop() {
    while (isMonitoring_) {
        collectMemoryMetrics();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MemoryAnalysisTools::collectMemoryMetrics() {
    std::lock_guard<std::mutex> lock(mutex_);

    updateAnalysisData();

    // Collect metrics from brain if available
    if (brain_) {
        if (brain_->getWorkingMemory()) {
            auto wmStats = analyzeWorkingMemory(brain_->getWorkingMemory());
            pImpl->currentWorkingMemoryUsage = wmStats.workingMemoryUsage;
            pImpl->currentUtilization = wmStats.utilizationRatio;
        }

        if (brain_->getEpisodicMemory()) {
            auto emStats = analyzeEpisodicMemory(brain_->getEpisodicMemory());
            pImpl->currentEpisodicMemoryUsage = emStats.episodicMemoryUsage;
        }
    }

    // Calculate derived metrics
    pImpl->currentFragmentation = calculateCurrentFragmentation();
    pImpl->currentLeakRate = calculateCurrentLeakRate();
    pImpl->currentCapacity = calculateCurrentCapacity();
}

double MemoryAnalysisTools::calculateUtilizationRatio(size_t current, size_t capacity) const {
    if (capacity == 0) return 0.0;
    return static_cast<double>(current) / capacity;
}

double MemoryAnalysisTools::calculateFragmentation(const std::vector<size_t>& holeSizes) const {
    if (holeSizes.empty()) return 0.0;

    size_t totalHoles = std::accumulate(holeSizes.begin(), holeSizes.end(), 0);
    double totalSize = std::accumulate(holeSizes.begin(), holeSizes.end(), 0.0);

    return static_cast<double>(totalHoles) / totalSize;
}

double MemoryAnalysisTools::calculateLeakRate(size_t created, size_t deleted, size_t total) const {
    if (total == 0) return 0.0;
    return static_cast<double>(created - deleted) / total * 1000.0; // leaks per 1000 operations
}

std::string MemoryAnalysisTools::determineGrowthTrend(const std::vector<size_t>& values) const {
    if (values.size() < 2) return "stable";

    size_t first = values.front();
    size_t last = values.back();

    if (last > first * 1.1) return "increasing";
    if (last < first * 0.9) return "decreasing";
    return "stable";
}

double MemoryAnalysisTools::calculateMemoryGrowthRate(const std::vector<MemoryAllocation>& allocations) const {
    if (allocations.size() < 2) return 0.0;

    size_t initial = allocations.front().size;
    size_t final = allocations.back().size;

    if (initial == 0) return final;

    return static_cast<double>(final - initial) / allocations.size();
}

double MemoryAnalysisTools::calculateCurrentFragmentation() const {
    return pImpl->currentFragmentation;
}

double MemoryAnalysisTools::calculateCurrentLeakRate() const {
    return pImpl->currentLeakRate;
}

size_t MemoryAnalysisTools::calculateCurrentCapacity() const {
    return pImpl->currentCapacity;
}

std::string MemoryAnalysisTools::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

double MemoryAnalysisTools::calculatePeriodicity(const std::vector<double>& values) const {
    if (values.size() < 3) return 1.0;

    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();

    double variance = 0.0;
    for (const auto& val : values) {
        variance += (val - mean) * (val - mean);
    }
    variance /= values.size();

    double stddev = std::sqrt(variance);

    return stddev / (mean > 0 ? mean : 1.0);
}

double MemoryAnalysisTools::calculateCompressionRatio(const std::vector<double>& values) const {
    if (values.empty()) return 1.0;

    double maxVal = *std::max_element(values.begin(), values.end());
    double minVal = *std::min_element(values.begin(), values.end());

    if (maxVal - minVal < 0.001) return 1.0;

    return (maxVal - minVal) / maxVal;
}

double MemoryAnalysisTools::calculateCompressionEfficiency(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;

    double maxVal = *std::max_element(values.begin(), values.end());
    double minVal = *std::min_element(values.begin(), values.end());

    if (maxVal - minVal < 0.001) return 100.0;

    return (1.0 - (maxVal - minVal) / maxVal) * 100.0;
}

} // namespace nlm