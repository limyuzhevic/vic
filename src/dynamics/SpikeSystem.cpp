#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <numeric>
#include <cmath>

namespace nlm {

// SpikeSystem implementation
struct SpikeSystem::Impl {
    // Configuration
    SpikeSystemConfig config;
    
    // Performance tracking
    SpikeSystemPerformanceStats perfStats;
    Stats stats;
    
    // Spike event management
    std::queue<OptimizedSpikeEvent> pendingSpikes;
    std::vector<DetailedSpikeEvent> spikeHistory;
    
    // Delayed spike management
    std::unordered_map<SimulationStep, std::vector<OptimizedDelayedSpikeEvent>> delayedSpikes;
    
    // Handler management
    std::vector<SpikeHandler> handlers;
    std::vector<OptimizedSpikeHandler> optimizedHandlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;
    
    // Performance monitoring
    std::chrono::steady_clock::time_point lastUpdateTime;
    size_t spikesProcessedThisUpdate{0};
    size_t delayedSpikesProcessedThisUpdate{0};
    
    // Memory management
    size_t memoryUsage;
    bool historyCompressed{false};
    
    Impl(const SpikeSystemConfig& cfg) : config(cfg), memoryUsage(0), historyCompressed(false) {
        lastUpdateTime = std::chrono::steady_clock::now();
    }
};

SpikeSystem::SpikeSystem() : pImpl(new Impl(SpikeSystemConfig{})) {
    pImpl->memoryUsage = sizeof(*pImpl);
}

SpikeSystem::SpikeSystem(const SpikeSystemConfig& config) : pImpl(new Impl(config)) {
    pImpl->memoryUsage = sizeof(*pImpl);
}

SpikeSystem::~SpikeSystem() = default;

SpikeSystem::SpikeSystem(SpikeSystem&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

SpikeSystem& SpikeSystem::operator=(SpikeSystem&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void SpikeSystem::queueSpike(const OptimizedSpikeEvent& event) {
    if (pImpl->pendingSpikes.size() >= pImpl->config.maxPendingSpikes) {
        // Overflow protection
        auto temp = std::move(pImpl->pendingSpikes.front());
        pImpl->pendingSpikes.pop();
        NLM_LOG_WARNING("Spike queue overflow - dropped oldest spike");
    }
    
    pImpl->pendingSpikes.push(event);
    pImpl->stats.totalSpikesProcessed++;
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    OptimizedSpikeEvent event(neuron, timestamp, step);
    queueSpike(event);
}

void SpikeSystem::queueDelayedSpike(const OptimizedDelayedSpikeEvent& event) {
    if (pImpl->delayedSpikes.size() >= pImpl->config.maxDelayedSpikes) {
        // Find and remove oldest spike in that step
        for (auto& pair : pImpl->delayedSpikes) {
            if (!pair.second.empty()) {
                pair.second.erase(pair.second.begin());
                break;
            }
        }
    }
    
    pImpl->delayedSpikes[event.delivery_step].push_back(event);
    pImpl->stats.totalDelayedSpikesProcessed++;
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    if (pImpl->config.enableBatchProcessing) {
        processSpikesBatch(currentStep);
    } else {
        processSpikesImpl(currentStep);
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->config.enableBatchProcessing) {
        processDelayedSpikesBatch(currentStep, currentTime);
    } else {
        processDelayedSpikesImpl(currentStep, currentTime);
    }
}

void SpikeSystem::processSpikesBatch(SimulationStep currentStep) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Collect all spikes to process
    std::vector<OptimizedSpikeEvent> batch;
    batch.reserve(pImpl->config.batchSize);
    
    while (!pImpl->pendingSpikes.empty() && batch.size() < pImpl->config.batchSize) {
        batch.push_back(std::move(pImpl->pendingSpikes.front()));
        pImpl->pendingSpikes.pop();
    }
    
    // Process batch
    if (!batch.empty()) {
        processSpikeBatch(batch, currentStep);
    }
    
    // Update statistics
    auto endTime = std::chrono::steady_clock::now();
    pImpl->perfStats.totalProcessingTime += (endTime - startTime);
    pImpl->perfStats.spikesProcessed += batch.size();
    pImpl->perfStats.handlerCalls += batch.size() * (pImpl->optimizedHandlers.size() + pImpl->handlers.size());
}

void SpikeSystem::processDelayedSpikesBatch(SimulationStep currentStep, Timestamp currentTime) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Find spikes scheduled for this step
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it == pImpl->delayedSpikes.end()) {
        return;
    }
    
    // Extract batch
    std::vector<OptimizedDelayedSpikeEvent> batch = std::move(it->second);
    
    // Process batch
    processDelayedBatch(batch, currentStep);
    
    // Remove processed spikes
    pImpl->delayedSpikes.erase(it);
    
    // Update statistics
    auto endTime = std::chrono::steady_clock::now();
    pImpl->perfStats.totalProcessingTime += (endTime - startTime);
    pImpl->perfStats.delayedSpikesProcessed += batch.size();
    pImpl->perfStats.handlerCalls += batch.size() * pImpl->delayedHandlers.size();
}

void SpikeSystem::processSpikeBatch(const std::vector<OptimizedSpikeEvent>& batch, SimulationStep step) {
    // Convert to DetailedSpikeEvent for handlers
    std::vector<DetailedSpikeEvent> detailedBatch;
    detailedBatch.reserve(batch.size());
    
    for (const auto& spike : batch) {
        DetailedSpikeEvent detailed;
        detailed.source = spike.source_neuron;
        detailed.timestamp = spike.timestamp;
        detailed.step = spike.step;
        detailed.regionId = RegionId();
        detailed.populationId = PopulationId();
        
        detailedBatch.push_back(detailed);
        
        // Update spike count per neuron
        pImpl->spikeCountPerNeuron[spike.source_neuron.value]++;
        
        // Add to history if optimization is enabled
        if (pImpl->config.enableOptimizedHistory) {
            pImpl->spikeHistory.push_back(detailed);
        }
    }
    
    // Update performance stats
    pImpl->perfStats.update(batch.size(), 0, batch.size() * (pImpl->optimizedHandlers.size() + pImpl->handlers.size()));
    
    // Call optimized handlers first
    if (pImpl->config.optimizeHandlerCalls) {
        for (const auto& handler : pImpl->optimizedHandlers) {
            for (const auto& spike : batch) {
                handler(spike);
            }
        }
    }
    
    // Call regular handlers
    for (const auto& handler : pImpl->handlers) {
        for (const auto& detailed : detailedBatch) {
            handler(detailed);
        }
    }
    
    // History management
    if (pImpl->config.enableOptimizedHistory) {
        compressHistoryIfNeeded();
    }
}

void SpikeSystem::processDelayedBatch(const std::vector<OptimizedDelayedSpikeEvent>& batch, SimulationStep step) {
    for (const auto& handler : pImpl->delayedHandlers) {
        for (const auto& spike : batch) {
            handler(spike);
        }
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    pImpl->handlers.push_back(std::move(handler));
}

void SpikeSystem::registerOptimizedHandler(OptimizedSpikeHandler handler) {
    pImpl->optimizedHandlers.push_back(std::move(handler));
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    pImpl->delayedHandlers.push_back(std::move(handler));
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->historyCompressed = false;
}

float SpikeSystem::getAverageSpikeRate() const {
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    // Find time range
    float minTime = std::numeric_limits<float>::max();
    float maxTime = -std::numeric_limits<float>::max();
    
    for (const auto& spike : pImpl->spikeHistory) {
        minTime = std::min(minTime, static_cast<float>(spike.timestamp));
        maxTime = std::max(maxTime, static_cast<float>(spike.timestamp));
    }
    
    float duration = maxTime - minTime;
    if (duration <= 0.0f) {
        return 0.0f;
    }
    
    return static_cast<float>(pImpl->spikeHistory.size()) / duration;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    for (const auto& pair : pImpl->spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    // Sort by count descending
    std::sort(neuronCounts.begin(), neuronCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.emplace_back(neuronCounts[i].first);
    }
    
    return result;
}

void SpikeSystem::configure(const SpikeSystemConfig& config) {
    pImpl->config = config;
}

SpikeSystemConfig SpikeSystem::getConfig() const {
    return pImpl->config;
}

SpikeSystemPerformanceStats SpikeSystem::getPerformanceStats() const {
    return pImpl->perfStats;
}

void SpikeSystem::resetPerformanceStats() {
    pImpl->perfStats.reset();
}

void SpikeSystem::optimizeMemoryUsage() {
    compressHistoryIfNeeded();
    optimizeHandlerVector();
    pImpl->memoryUsage = sizeof(*pImpl) + pImpl->spikeHistory.size() * sizeof(DetailedSpikeEvent);
}

size_t SpikeSystem::getMemoryUsage() const {
    return pImpl->memoryUsage;
}

SpikeSystem::Stats SpikeSystem::getStats() const {
    return pImpl->stats;
}

void SpikeSystem::updateStats() {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - pImpl->lastUpdateTime);
    
    pImpl->stats.uptime += elapsed.count();
    
    // Update max pending counts
    if (pImpl->pendingSpikes.size() > pImpl->stats.maxPendingSpikes) {
        pImpl->stats.maxPendingSpikes = pImpl->pendingSpikes.size();
    }
    
    size_t totalDelayed = 0;
    for (const auto& pair : pImpl->delayedSpikes) {
        totalDelayed += pair.second.size();
    }
    if (totalDelayed > pImpl->stats.maxPendingDelayed) {
        pImpl->stats.maxPendingDelayed = totalDelayed;
    }
    
    // Update average spike rate
    pImpl->stats.averageSpikeRate = getAverageSpikeRate();
    
    pImpl->lastUpdateTime = currentTime;
}

void SpikeSystem::reset() {
    while (!pImpl->pendingSpikes.empty()) {
        pImpl->pendingSpikes.pop();
    }
    
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikes.clear();
    pImpl->historyCompressed = false;
    
    // Reset statistics
    pImpl->perfStats.reset();
    pImpl->stats.reset();
}

void SpikeSystem::forceMemoryCleanup() {
    // Force history compression
    if (!pImpl->historyCompressed) {
        compressHistoryIfNeeded();
    }
    
    // Clear unused delayed spikes
    for (auto it = pImpl->delayedSpikes.begin(); it != pImpl->delayedSpikes.end(); ) {
        if (it->second.empty()) {
            it = pImpl->delayedSpikes.erase(it);
        } else {
            ++it;
        }
    }
}

void SpikeSystem::tuneForPerformance(const std::string& workloadProfile) {
    if (workloadProfile == "high_frequency") {
        // Optimize for high-frequency spike processing
        pImpl->config.enableBatchProcessing = true;
        pImpl->config.batchSize = 1000;
        pImpl->config.enableOptimizedHistory = false;
        pImpl->config.maxHistorySize = 5000;
    } else if (workloadProfile == "memory_constrained") {
        // Optimize for memory usage
        pImpl->config.enableMemoryPool = true;
        pImpl->config.enableBatchProcessing = false;
        pImpl->config.enableOptimizedHistory = true;
        pImpl->config.maxHistorySize = 20000;
    } else if (workloadProfile == "balanced") {
        // Balanced optimization
        pImpl->config.enableMemoryPool = true;
        pImpl->config.enableBatchProcessing = true;
        pImpl->config.batchSize = 500;
        pImpl->config.enableOptimizedHistory = true;
    }
}

void SpikeSystem::optimizeForSpeed() {
    pImpl->config.enableMemoryPool = true;
    pImpl->config.enableBatchProcessing = true;
    pImpl->config.batchSize = 1000;
    pImpl->config.optimizeHandlerCalls = true;
    pImpl->config.enableOptimizedHistory = false;
    pImpl->config.enableSIMDProcessing = true;
}

void SpikeSystem::optimizeForMemory() {
    pImpl->config.enableMemoryPool = true;
    pImpl->config.enableBatchProcessing = false;
    pImpl->config.enableOptimizedHistory = true;
    pImpl->config.maxHistorySize = 50000;
    pImpl->config.compressHistory = true;
    pImpl->config.enableSIMDProcessing = false;
}

void SpikeSystem::compressHistoryIfNeeded() {
    if (!pImpl->config.compressHistory || pImpl->historyCompressed) {
        return;
    }
    
    if (pImpl->spikeHistory.size() > pImpl->config.maxHistorySize) {
        size_t targetSize = pImpl->config.maxHistorySize;
        size_t compressionRatio = pImpl->config.historyCompressionRatio;
        
        std::vector<DetailedSpikeEvent> compressedHistory;
        compressedHistory.reserve(targetSize);
        
        size_t step = compressionRatio;
        for (size_t i = 0; i < pImpl->spikeHistory.size(); i += step) {
            if (compressedHistory.size() < targetSize) {
                compressedHistory.push_back(pImpl->spikeHistory[i]);
            }
        }
        
        pImpl->spikeHistory = std::move(compressedHistory);
        pImpl->historyCompressed = true;
        
        NLM_LOG_INFO("Spike history compressed from " + std::to_string(pImpl->spikeHistory.size() + step) + 
                     " to " + std::to_string(pImpl->spikeHistory.size()) + " entries");
    }
}

void SpikeSystem::optimizeHandlerVector() {
    std::sort(pImpl->optimizedHandlers.begin(), pImpl->optimizedHandlers.end());
    std::sort(pImpl->handlers.begin(), pImpl->handlers.end());
    
    pImpl->optimizedHandlers.erase(
        std::unique(pImpl->optimizedHandlers.begin(), pImpl->optimizedHandlers.end()),
        pImpl->optimizedHandlers.end()
    );
    
    pImpl->handlers.erase(
        std::unique(pImpl->handlers.begin(), pImpl->handlers.end()),
        pImpl->handlers.end()
    );
}

void SpikeSystem::processSpikesImpl(SimulationStep currentStep) {
    auto startTime = std::chrono::steady_clock::now();
    
    while (!pImpl->pendingSpikes.empty()) {
        OptimizedSpikeEvent event = std::move(pImpl->pendingSpikes.front());
        pImpl->pendingSpikes.pop();
        
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        detailed.regionId = RegionId();
        detailed.populationId = PopulationId();
        
        pImpl->spikeHistory.push_back(detailed);
        
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        for (auto& handler : pImpl->optimizedHandlers) {
            handler(event);
        }
        
        for (auto& handler : pImpl->handlers) {
            handler(detailed);
        }
        
        pImpl->perfStats.spikesProcessed++;
        pImpl->perfStats.handlerCalls += pImpl->optimizedHandlers.size() + pImpl->handlers.size();
        
        if (pImpl->spikeHistory.size() > pImpl->config.maxHistorySize) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    pImpl->perfStats.totalProcessingTime += (endTime - startTime);
}

void SpikeSystem::processDelayedSpikesImpl(SimulationStep currentStep, Timestamp currentTime) {
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it == pImpl->delayedSpikes.end()) {
        return;
    }
    
    for (const auto& delayedEvent : it->second) {
        for (auto& handler : pImpl->delayedHandlers) {
            handler(delayedEvent);
        }
    }
    
    pImpl->delayedSpikes.erase(it);
}

// Memory optimizer implementation
void SpikeSystemMemoryOptimizer::optimizeMemory(SpikeSystem& spikeSystem) {
    spikeSystem.optimizeMemoryUsage();
}

void SpikeSystemMemoryOptimizer::monitorMemoryUsage(SpikeSystem& spikeSystem) {
    size_t usage = spikeSystem.getMemoryUsage();
    NLM_LOG_INFO("SpikeSystem memory usage: " + std::to_string(usage) + " bytes");
}

void SpikeSystemMemoryOptimizer::tuneForPerformance(SpikeSystem& spikeSystem, const std::string& workloadProfile) {
    spikeSystem.tuneForPerformance(workloadProfile);
}

void SpikeSystemMemoryOptimizer::profileProcessing(SpikeSystem& spikeSystem, const std::string& testName) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t i = 0; i < 1000; ++i) {
        OptimizedSpikeEvent spike(NeuronId(i), static_cast<Timestamp>(i), i);
        spikeSystem.queueSpike(spike);
    }
    
    spikeSystem.processSpikes(0);
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    NLM_LOG_INFO("Profile " + testName + " completed in " + std::to_string(duration.count()) + " microseconds");
}

void SpikeSystemMemoryOptimizer::analyzeMemoryPatterns(SpikeSystem& spikeSystem) {
    size_t memoryUsage = spikeSystem.getMemoryUsage();
    NLM_LOG_INFO("Memory analysis: " + std::to_string(memoryUsage) + " bytes used");
}

void SpikeSystemMemoryOptimizer::generatePerformanceReport(SpikeSystem& spikeSystem, const std::string& outputFile) {
    auto perfStats = spikeSystem.getPerformanceStats();
    auto stats = spikeSystem.getStats();
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open performance report file: " + outputFile);
        return;
    }
    
    file << "SpikeSystem Performance Report" << std::endl;
    file << "===============================" << std::endl;
    file << "Spikes Processed: " << perfStats.spikesProcessed << std::endl;
    file << "Delayed Spikes Processed: " << perfStats.delayedSpikesProcessed << std::endl;
    file << "Handler Calls: " << perfStats.handlerCalls << std::endl;
    file << "Average Processing Time per Spike: " << perfStats.avgProcessingTimePerSpike << " microseconds" << std::endl;
    file << "Average Processing Time per Delayed: " << perfStats.avgProcessingTimePerDelayed << " microseconds" << std::endl;
    file << "Memory Allocations: " << perfStats.memoryPoolAllocations << std::endl;
    file << "Memory Deallocations: " << perfStats.memoryPoolDeallocations << std::endl;
    
    file << std::endl;
    file << "System Statistics:" << std::endl;
    file << "Total Spikes Processed: " << stats.totalSpikesProcessed << std::endl;
    file << "Total Delayed Spikes Processed: " << stats.totalDelayedSpikesProcessed << std::endl;
    file << "Total Handler Calls: " << stats.totalHandlerCalls << std::endl;
    file << "Max Pending Spikes: " << stats.maxPendingSpikes << std::endl;
    file << "Max Pending Delayed: " << stats.maxPendingDelayed << std::endl;
    file << "Average Spike Rate: " << stats.averageSpikeRate << " spikes/second" << std::endl;
    file << "Uptime: " << stats.uptime << " seconds" << std::endl;
    
    file.close();
    
    NLM_LOG_INFO("Performance report generated: " + outputFile);
}

// SpikeSystem memory pool implementation
OptimizedSpikeEvent* SpikeEventPool::allocate(const OptimizedSpikeEvent& event) {
    return SpikeEventPool::getInstance().allocate(event);
}

void SpikeEventPool::deallocate(OptimizedSpikeEvent* event) {
    SpikeEventPool::getInstance().deallocate(event);
}

void SpikeEventPool::clear() {
    SpikeEventPool::getInstance().clear();
}

} // namespace nlm
