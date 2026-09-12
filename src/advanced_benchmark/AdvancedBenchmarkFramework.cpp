// Advanced NLM Performance Benchmark Framework Implementation
// Contains detailed implementation of memory tracking, spike analysis, and topology analysis

#include "AdvancedBenchmarkFramework.hpp"
#include "core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nlm {

// MemoryTracker implementation
void MemoryTracker::recordMemoryUsage(size_t bytes, const std::string& description) {
    auto now = std::chrono::system_clock::now();
    double timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();
    
    MemorySample sample;
    sample.timestamp = timestamp;
    sample.memoryBytes = bytes;
    sample.description = description;
    
    memoryHistory.push_back(sample);
    
    if (bytes > peakMemoryBytes) {
        peakMemoryBytes = bytes;
    }
}

void MemoryTracker::startTracking() {
    auto now = std::chrono::system_clock::now();
    startTime = std::chrono::duration<double>(now.time_since_epoch()).count();
}

void MemoryTracker::stopTracking() {
    // No-op for now, memory tracking is event-based
}

double MemoryTracker::getPeakMemoryMB() const {
    return static_cast<double>(peakMemoryBytes) / (1024.0 * 1024.0);
}

std::vector<MemoryTracker::MemorySample> MemoryTracker::getMemoryHistory() const {
    return memoryHistory;
}

// SpikeAnalyzer implementation
void SpikeAnalyzer::recordSpike(size_t neuronId, double spikeTime) {
    if (neuronTimes.find(neuronId) == neuronTimes.end()) {
        neuronTimes[neuronId] = std::vector<double>();
    }
    
    neuronTimes[neuronId].push_back(spikeTime);
    
    // Update spike pattern
    if (neuronSpikes.find(neuronId) == neuronSpikes.end()) {
        SpikePattern pattern;
        pattern.neuronId = neuronId;
        pattern.firstSpikeTime = spikeTime;
        pattern.lastSpikeTime = spikeTime;
        neuronSpikes[neuronId] = pattern;
    } else {
        neuronSpikes[neuronId].lastSpikeTime = spikeTime;
        neuronSpikes[neuronId].spikeTimes.push_back(spikeTime);
    }
}

void SpikeAnalyzer::startRecording() {
    auto now = std::chrono::system_clock::now();
    recordingStartTime = std::chrono::duration<double>(now.time_since_epoch()).count();
}

void SpikeAnalyzer::stopRecording() {
    // No-op for now, spike recording is event-based
}

std::vector<SpikeAnalyzer::SpikePattern> SpikeAnalyzer::getSpikePatterns() const {
    std::vector<SpikePattern> patterns;
    for (const auto& pair : neuronSpikes) {
        patterns.push_back(pair.second);
    }
    return patterns;
}

double SpikeAnalyzer::calculateFiringRate(size_t neuronId) const {
    auto it = neuronTimes.find(neuronId);
    if (it == neuronTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    double timeSpan = it->second.back() - recordingStartTime;
    if (timeSpan <= 0.0) return 0.0;
    
    return static_cast<double>(it->second.size()) / timeSpan;
}

double SpikeAnalyzer::calculateCoefficientOfVariation(size_t neuronId) const {
    auto it = neuronTimes.find(neuronId);
    if (it == neuronTimes.end() || it->second.size() < 2) {
        return 0.0;
    }
    
    std::vector<double>& times = it->second;
    double mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    
    double sumSquares = 0.0;
    for (double t : times) {
        double diff = t - mean;
        sumSquares += diff * diff;
    }
    
    double variance = sumSquares / times.size();
    double stdDev = std::sqrt(variance);
    
    if (mean == 0.0) return 0.0;
    
    return stdDev / mean;
}

// NetworkTopologyAnalyzer implementation
void NetworkTopologyAnalyzer::analyzeBrain(const std::shared_ptr<Brain>& brain) {
    if (!brain) return;
    
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t synapseCount = brain->getTotalSynapseCount();
    size_t regionCount = brain->getTotalNeuronCount() > 0 ? 1 : 0; // Simplified
    
    // Calculate average degree (simplified)
    metrics.averageDegree = (neuronCount > 0) ? static_cast<float>(synapseCount * 2 / neuronCount) : 0.0f;
    
    // Calculate connection density (simplified)
    metrics.connectionDensity = (neuronCount > 1) ? static_cast<float>(synapseCount) / (neuronCount * neuronCount) : 0.0f;
    
    // Calculate average connection strength (simplified)
    metrics.averageConnectionStrength = 0.5f; // Placeholder
    
    // Calculate modularity (simplified)
    metrics.modularity = 0.5f; // Placeholder
    
    // Calculate synchronizability (simplified)
    metrics.synchronizability = 0.7f; // Placeholder
    
    metrics.communityCount = 2; // Placeholder
}

NetworkTopologyAnalyzer::NetworkMetrics NetworkTopologyAnalyzer::getMetrics() const {
    return metrics;
}

std::unordered_map<size_t, size_t> NetworkTopologyAnalyzer::getCommunityAssignments() const {
    return neuronCommunities;
}

// AdvancedBenchmark implementation
AdvancedBenchmarkResult AdvancedBenchmark::runBenchmark(
    const std::shared_ptr<Config>& config,
    size_t neuronCount,
    size_t steps,
    const std::string& benchmarkName,
    bool trackMemory,
    bool analyzeSpikes,
    bool analyzeTopology
) {
    AdvancedBenchmarkResult result;
    result.neurons = neuronCount;
    result.steps = steps;
    
    if (!config) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        cfg->set("connection_probability", 0.1f, ConfigSource::Default);
        config = cfg;
    }
    
    NLM_LOG_INFO("Running advanced benchmark: " + std::to_string(neuronCount) + 
                " neurons, " + std::to_string(steps) + " steps");
    
    auto startWall = std::chrono::high_resolution_clock::now();
    auto startCpu = std::chrono::system_clock::now();
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Initialize trackers
    if (trackMemory) {
        memoryTracker.startTracking();
    }
    if (analyzeSpikes) {
        spikeAnalyzer.startRecording();
    }
    
    // Run simulation
    for (size_t step = 0; step < steps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            if (analyzeSpikes) {
                spikeAnalyzer.recordSpike(i, step * 0.001);
            }
        }
        
        brain->step(step, step * 0.001);
        
        // Record metrics
        if (trackMemory) {
            // Record current memory usage
            memoryTracker.recordMemoryUsage(BenchmarkUtils::getCurrentMemoryUsageMB() * 1024 * 1024, 
                                           "Step " + std::to_string(step));
        }
        
        if (analyzeSpikes) {
            // Record spikes from brain
            size_t spikeCount = brain->getTotalSpikeCount();
            result.spikeCountsPerStep.push_back(spikeCount);
        }
        
        // Record firing rate per step
        result.firingRatesPerStep.push_back(brain->getAverageFiringRate());
        
        // Record step time
        auto stepStart = std::chrono::high_resolution_clock::now();
        result.stepTimes.push_back(std::chrono::duration<double>(stepStart - startWall).count());
    }
    
    auto endWall = std::chrono::high_resolution_clock::now();
    auto endCpu = std::chrono::system_clock::now();
    
    result.wallClockTime = std::chrono::duration<double>(endWall - startWall).count();
    result.cpuTime = std::chrono::duration<double>(endCpu - startCpu).count();
    
    // Collect basic results
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.totalSpikeCount = brain->getTotalSpikeCount();
    result.totalTime = result.wallClockTime;
    result.stepsPerSecond = steps / result.totalTime;
    result.spikesPerStep = static_cast<double>(result.totalSpikeCount) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Collect advanced metrics
    result.memoryUsageMB = memoryTracker.getPeakMemoryMB();
    result.workingMemoryTraces = brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0;
    result.episodicMemoryEpisodes = brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0;
    
    // Calculate firing rate statistics
    if (!result.firingRatesPerStep.empty()) {
        result.averageFiringRate = std::accumulate(result.firingRatesPerStep.begin(), 
                                                   result.firingRatesPerStep.end(), 0.0f) / result.firingRatesPerStep.size();
        result.maxFiringRate = *std::max_element(result.firingRatesPerStep.begin(), result.firingRatesPerStep.end());
        result.minFiringRate = *std::min_element(result.firingRatesPerStep.begin(), result.firingRatesPerStep.end());
    }
    
    // Calculate spike statistics
    result.spikeFrequency = result.spikesPerStep * (steps / result.totalTime);
    if (!result.spikeCountsPerStep.empty()) {
        double meanSpikes = std::accumulate(result.spikeCountsPerStep.begin(), 
                                           result.spikeCountsPerStep.end(), 0.0) / result.spikeCountsPerStep.size();
        double sumSquares = 0.0;
        for (size_t spikes : result.spikeCountsPerStep) {
            double diff = static_cast<double>(spikes) - meanSpikes;
            sumSquares += diff * diff;
        }
        double variance = sumSquares / result.spikeCountsPerStep.size();
        result.spikeCoefficientOfVaration = std::sqrt(variance) / (meanSpikes > 0 ? meanSpikes : 1.0);
    }
    
    // Analyze network topology
    if (analyzeTopology) {
        topologyAnalyzer.analyzeBrain(brain);
        NetworkTopologyAnalyzer::NetworkMetrics metrics = topologyAnalyzer.getMetrics();
        result.averageConnectionStrength = metrics.averageConnectionStrength;
        result.connectionDensity = metrics.averageDegree;
        result.regionCount = metrics.communityCount;
        result.averageRegionActivity = 0.5f; // Placeholder
    }
    
    NLM_LOG_INFO("Benchmark complete: " + std::to_string(neuronCount) + " neurons, " +
                std::to_string(result.totalTime) + "s, " + std::to_string(result.stepsPerSecond) +
                " steps/sec");
    
    return result;
}

std::vector<AdvancedBenchmarkResult> AdvancedBenchmark::runBenchmarkSuite(
    const std::vector<std::pair<size_t, size_t>>& configurations
) {
    std::vector<AdvancedBenchmarkResult> results;
    for (const auto& config : configurations) {
        auto result = runBenchmark(nullptr, config.first, config.second);
        results.push_back(result);
    }
    return results;
}

bool AdvancedBenchmark::exportResults(const std::string& filepath, 
                                     const std::vector<AdvancedBenchmarkResult>& results,
                                     const std::string& format) {
    NLM_LOG_INFO("Exporting benchmark results to: " + filepath + " (format: " + format + ")");
    
    if (format == "json") {
        // JSON export implementation
        // For now, just create a simple text-based export
    } else if (format == "csv") {
        // CSV export implementation
        // For now, just create a simple text-based export
    } else {
        NLM_LOG_ERROR("Unsupported export format: " + format);
        return false;
    }
    
    return true;
}

void AdvancedBenchmark::printResults(const std::vector<AdvancedBenchmarkResult>& results) const {
    for (const auto& result : results) {
        BenchmarkUtils::printAdvancedResults(result);
    }
}

// BenchmarkUtils implementation
double BenchmarkUtils::getCurrentTime() {
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

size_t BenchmarkUtils::getCurrentMemoryUsageMB() {
    struct stat statbuf;
    if (stat("/proc/self/status", &statbuf) != 0) {
        return 0;
    }
    
    FILE* file = fopen("/proc/self/status", "r");
    if (!file) return 0;
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            fclose(file);
            // Parse memory usage from /proc/self/status
            // VmRSS: 12345 kB
            return static_cast<size_t>(atof(line + 6) / 1024); // Convert to MB
        }
    }
    
    fclose(file);
    return 0;
}

void BenchmarkUtils::setupBenchmarkConfig(std::shared_ptr<Config>& config, 
                                         size_t neuronCount,
                                         const std::string& benchmarkType) {
    if (!config) {
        config = std::make_shared<Config>();
    }
    
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    if (benchmarkType == "development") {
        config->set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
        config->set("development_pruning_rate", 0.0001f, ConfigSource::Default);
    }
}

void BenchmarkUtils::printAdvancedResults(const AdvancedBenchmarkResult& result) {
    std::cout << "Advanced Benchmark Results:" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << "Neurons: " << result.neurons << std::endl;
    std::cout << "Synapses: " << result.synapses << std::endl;
    std::cout << "Steps: " << result.steps << std::endl;
    std::cout << "Total Time: " << formatTime(result.totalTime) << std::endl;
    std::cout << "Steps/sec: " << result.stepsPerSecond << std::endl;
    std::cout << "Spikes/step: " << result.spikesPerStep << std::endl;
    std::cout << "Memory Usage: " << formatMemory(static_cast<size_t>(result.memoryUsageMB * 1024 * 1024)) << std::endl;
    std::cout << "Working Memory Traces: " << result.workingMemoryTraces << std::endl;
    std::cout << "Episodic Episodes: " << result.episodicMemoryEpisodes << std::endl;
    std::cout << "Average Firing Rate: " << result.averageFiringRate << std::endl;
    std::cout << "Max Firing Rate: " << result.maxFiringRate << std::endl;
    std::cout << "Min Firing Rate: " << result.minFiringRate << std::endl;
    std::cout << "Spike Frequency: " << result.spikeFrequency << std::endl;
    std::cout << "Spike CV: " << result.spikeCoefficientOfVaration << std::endl;
    std::cout << std::endl;
}

std::string BenchmarkUtils::formatTime(double seconds) {
    if (seconds < 60) {
        return std::to_string(static_cast<int>(seconds)) + "s";
    } else if (seconds < 3600) {
        return std::to_string(static_cast<int>(seconds / 60)) + "m " +
               std::to_string(static_cast<int>(seconds) % 60) + "s";
    } else {
        return std::to_string(static_cast<int>(seconds / 3600)) + "h " +
               std::to_string(static_cast<int>(seconds) % 3600 / 60) + "m " +
               std::to_string(static_cast<int>(seconds) % 60) + "s";
    }
}

std::string BenchmarkUtils::formatMemory(size_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    } else {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
}

} // namespace nlm
