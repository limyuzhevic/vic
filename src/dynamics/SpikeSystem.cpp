#include "SpikeSystem.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <limits>
#include <random>
#include <unordered_map>
#include <vector>
#include <memory>
#include <queue>
#include <deque>
#include <cmath>
#include <utility>

namespace nlm {

// Implementation details for SpikeEventPool
struct SpikeEventPool::Impl {
    std::deque<DetailedSpikeEvent> eventBuffer;
    std::vector<DetailedSpikeEvent*> freeSlots;
    size_t maxSize;
    size_t allocatedCount;
    
    Impl(size_t initialSize, size_t maxSize) 
        : maxSize(maxSize), allocatedCount(0) {
        eventBuffer.reserve(maxSize);
        eventBuffer.resize(initialSize);
        freeSlots.reserve(maxSize);
        for (size_t i = 0; i < initialSize; ++i) {
            freeSlots.push_back(&eventBuffer[i]);
        }
    }
};

SpikeEventPool::SpikeEventPool(size_t initialSize, size_t maxSize)
    : pImpl(new Impl(initialSize, maxSize)) {}

SpikeEventPool::~SpikeEventPool() = default;

DetailedSpikeEvent* SpikeEventPool::acquire() {
    if (pImpl->freeSlots.empty()) {
        return nullptr; // Pool exhausted
    }
    DetailedSpikeEvent* event = pImpl->freeSlots.back();
    pImpl->freeSlots.pop_back();
    pImpl->allocatedCount++;
    return event;
}

void SpikeEventPool::release(DetailedSpikeEvent* event) {
    if (event) {
        // Find the event in buffer (simple linear search)
        auto it = std::find(&pImpl->eventBuffer[0],
                           &pImpl->eventBuffer[0] + pImpl->eventBuffer.size(), event);
        if (it != &pImpl->eventBuffer[0] + pImpl->eventBuffer.size()) {
            pImpl->freeSlots.push_back(event);
            pImpl->allocatedCount--;
        }
    }
}

void SpikeEventPool::clear() {
    pImpl->freeSlots.clear();
    for (size_t i = 0; i < pImpl->eventBuffer.size(); ++i) {
        pImpl->freeSlots.push_back(&pImpl->eventBuffer[i]);
    }
    pImpl->allocatedCount = 0;
}

size_t SpikeEventPool::getAllocatedCount() const {
    return pImpl->allocatedCount;
}

size_t SpikeEventPool::getFreeCount() const {
    return pImpl->freeSlots.size();
}

// Implementation details for SpikeSystem
struct SpikeSystem::Impl {
    // Optimized event queues using priority queues for delayed events
    std::priority_queue<DetailedSpikeEvent, std::vector<DetailedSpikeEvent>, 
                       std::greater<DetailedSpikeEvent>> pendingSpikes;
    
    // Delayed spikes grouped by delivery time for efficient processing
    std::priority_queue<DetailedSpikeEvent, std::vector<DetailedSpikeEvent>, 
                       std::greater<DetailedSpikeEvent>> pendingDelayedSpikes;
    
    // Event history with vector for efficiency
    std::vector<DetailedSpikeEvent> spikeHistory;
    size_t maxHistorySize;
    
    // Memory pool for event allocation
    std::unique_ptr<SpikeEventPool> eventPool;
    
    // Handler callbacks for event processing
    std::vector<SpikeHandler> spikeHandlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    std::vector<STDPUpdateHandler> stdpHandlers;
    std::vector<HebbianUpdateHandler> hebbianHandlers;
    
    // Legacy handler callbacks for backward compatibility
    std::vector<LegacySpikeHandler> legacySpikeHandlers;
    std::vector<LegacyDelayedSpikeHandler> legacyDelayedHandlers;
    
    // Statistics and metrics
    size_t totalEventsProcessed;
    size_t totalHandlersExecuted;
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;
    std::vector<float> delaySamples;
    size_t maxDelaySamples;
    
    // Realistic propagation configuration
    PropagationConfig propagationConfig;
    OptimizationConfig optimizationConfig;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Batching for performance
    std::vector<DetailedSpikeEvent> batchBuffer;
    bool batchReady;
    
    Impl() : maxHistorySize(10000), totalEventsProcessed(0), totalHandlersExecuted(0),
             maxDelaySamples(1000), batchReady(false) {
        // Initialize random generator
        std::random_device rd;
        rng.seed(rd());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 1.0f);
        
        // Default configurations
        propagationConfig = {1.0f, 0.1f, 0.1f, 10.0f, false, 1.0f, 0.3f};
        optimizationConfig = {true, 100, true, 1024, 16384, true};
        
        // Initialize memory pool
        eventPool = std::make_unique<SpikeEventPool>(optimizationConfig.memoryPoolInitialSize,
                                                    optimizationConfig.memoryPoolMaxSize);
    }
};

SpikeSystem::SpikeSystem() : pImpl(new Impl) {}

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

void SpikeSystem::queueSpike(const DetailedSpikeEvent& event) {
    if (optimizationConfig.enableMemoryPool && eventPool) {
        DetailedSpikeEvent* pooledEvent = eventPool->acquire();
        if (pooledEvent) {
            *pooledEvent = event;
            pendingSpikes.push(*pooledEvent);
            return;
        }
    }
    pendingSpikes.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step,
                            RegionId regionId, PopulationId populationId,
                            float delay, bool excitatory) {
    // Generate realistic delay with biological variability
    float actualDelay = delay;
    if (propagationConfig.useGaussianDistribution) {
        actualDelay = propagationConfig.meanDelay + 
                     normalDist(rng) * propagationConfig.stdDevDelay;
    } else {
        actualDelay = propagationConfig.minDelay + 
                     uniformDist(rng) * (propagationConfig.maxDelay - 
                                      propagationConfig.minDelay);
    }
    
    // Clamp to valid range
    actualDelay = std::clamp(actualDelay, propagationConfig.minDelay,
                            propagationConfig.maxDelay);
    
    DetailedSpikeEvent event(neuron, timestamp, step, regionId, populationId,
                           actualDelay, excitatory);
    
    // Store in pending spikes queue
    if (optimizationConfig.enableMemoryPool && eventPool) {
        DetailedSpikeEvent* pooledEvent = eventPool->acquire();
        if (pooledEvent) {
            *pooledEvent = event;
            pendingDelayedSpikes.push(*pooledEvent);
            return;
        }
    }
    pendingDelayedSpikes.push(event);
}

void SpikeSystem::queueDelayedSpike(const DetailedSpikeEvent& event) {
    if (optimizationConfig.enableMemoryPool && eventPool) {
        DetailedSpikeEvent* pooledEvent = eventPool->acquire();
        if (pooledEvent) {
            *pooledEvent = event;
            pendingDelayedSpikes.push(*pooledEvent);
            return;
        }
    }
    pendingDelayedSpikes.push(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep, Timestamp currentTime) {
    totalEventsProcessed += pendingSpikes.size();
    
    // Process immediate spikes
    while (!pendingSpikes.empty()) {
        const DetailedSpikeEvent& event = pendingSpikes.top();
        
        if (optimizationConfig.enableBatchedHandlerCalls) {
            // Buffer event for batched processing
            batchBuffer.push_back(event);
        } else {
            // Process immediately
            for (auto& handler : spikeHandlers) {
                handler(event);
                totalHandlersExecuted++;
            }
            
            // Process legacy handlers for backward compatibility
            for (auto& handler : legacySpikeHandlers) {
                LegacyDetailedSpikeEvent legacyEvent;
                legacyEvent.source = event.source;
                legacyEvent.timestamp = event.timestamp;
                legacyEvent.step = event.step;
                legacyEvent.regionId = event.regionId;
                legacyEvent.populationId = event.populationId;
                handler(legacyEvent);
                totalHandlersExecuted++;
            }
        }
        
        // Add to history
        spikeHistory.push_back(event);
        spikeCountPerNeuron[event.source.value]++;
        
        // Record delay sample for statistics
        if (event.delay > 0.0f) {
            delaySamples.push_back(event.delay);
            if (delaySamples.size() > maxDelaySamples) {
                delaySamples.erase(delaySamples.begin());
            }
        }
        
        // Trim history if needed
        if (spikeHistory.size() > maxHistorySize) {
            spikeHistory.erase(spikeHistory.begin());
        }
        
        pendingSpikes.pop();
    }
    
    // Flush batch if ready
    if (optimizationConfig.enableBatchedHandlerCalls && !batchBuffer.empty()) {
        batchReady = true;
        for (const auto& event : batchBuffer) {
            for (auto& handler : spikeHandlers) {
                handler(event);
                totalHandlersExecuted++;
            }
            
            // Process legacy handlers for backward compatibility
            for (auto& handler : legacySpikeHandlers) {
                LegacyDetailedSpikeEvent legacyEvent;
                legacyEvent.source = event.source;
                legacyEvent.timestamp = event.timestamp;
                legacyEvent.step = event.step;
                legacyEvent.regionId = event.regionId;
                legacyEvent.populationId = event.populationId;
                handler(legacyEvent);
                totalHandlersExecuted++;
            }
        }
        batchBuffer.clear();
        batchReady = false;
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Process all delayed spikes scheduled for delivery now
    while (!pendingDelayedSpikes.empty()) {
        const DetailedSpikeEvent& event = pendingDelayedSpikes.top();
        
        // Check if spike should be delivered now (within tolerance)
        if (event.deliveryTime <= currentTime + 1e-6f) {
            if (optimizationConfig.enableBatchedHandlerCalls) {
                batchBuffer.push_back(event);
            } else {
                for (auto& handler : delayedHandlers) {
                    handler(event);
                    totalHandlersExecuted++;
                }
                
                // Process legacy handlers for backward compatibility
                for (auto& handler : legacyDelayedHandlers) {
                    LegacyDetailedSpikeEvent legacyEvent;
                    legacyEvent.source = event.source;
                    legacyEvent.timestamp = event.timestamp;
                    legacyEvent.step = event.step;
                    legacyEvent.regionId = event.regionId;
                    legacyEvent.populationId = event.populationId;
                    handler(legacyEvent);
                    totalHandlersExecuted++;
                }
            }
            
            pendingDelayedSpikes.pop();
        } else {
            break; // Remaining spikes are still pending
        }
    }
    
    // Flush delayed spike batch
    if (optimizationConfig.enableBatchedHandlerCalls && !batchBuffer.empty()) {
        batchReady = true;
        for (const auto& event : batchBuffer) {
            for (auto& handler : delayedHandlers) {
                handler(event);
                totalHandlersExecuted++;
            }
            
            // Process legacy handlers for backward compatibility
            for (auto& handler : legacyDelayedHandlers) {
                LegacyDetailedSpikeEvent legacyEvent;
                legacyEvent.source = event.source;
                legacyEvent.timestamp = event.timestamp;
                legacyEvent.step = event.step;
                legacyEvent.regionId = event.regionId;
                legacyEvent.populationId = event.populationId;
                handler(legacyEvent);
                totalHandlersExecuted++;
            }
        }
        batchBuffer.clear();
        batchReady = false;
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    spikeHandlers.push_back(handler);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    delayedHandlers.push_back(handler);
}

void SpikeSystem::registerSTDPHandler(STDPUpdateHandler handler) {
    stdpHandlers.push_back(handler);
}

void SpikeSystem::registerHebbianHandler(HebbianUpdateHandler handler) {
    hebbianHandlers.push_back(handler);
}

void SpikeSystem::registerLegacyHandler(LegacySpikeHandler handler) {
    legacySpikeHandlers.push_back(handler);
}

void SpikeSystem::registerLegacyDelayedHandler(LegacyDelayedSpikeHandler handler) {
    legacyDelayedHandlers.push_back(handler);
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return spikeHistory;
}

void SpikeSystem::clearHistory() {
    spikeHistory.clear();
    spikeCountPerNeuron.clear();
    delaySamples.clear();
    
    // Clear memory pool if enabled
    if (optimizationConfig.enableMemoryPool && eventPool) {
        eventPool->clear();
    }
}

size_t SpikeSystem::getSpikeCount() const {
    return spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    return pendingDelayedSpikes.size();
}

float SpikeSystem::getAverageSpikeRate() const {
    if (spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    float minTime = std::numeric_limits<float>::max();
    float maxTime = -std::numeric_limits<float>::max();
    
    for (const auto& spike : spikeHistory) {
        minTime = std::min(minTime, static_cast<float>(spike.timestamp));
        maxTime = std::max(maxTime, static_cast<float>(spike.timestamp));
    }
    
    float duration = maxTime - minTime;
    if (duration <= 0.0f) {
        return 0.0f;
    }
    
    return static_cast<float>(spikeHistory.size()) / duration;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(spikeCountPerNeuron.size());
    
    for (const auto& pair : spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    std::sort(neuronCounts.begin(), neuronCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.emplace_back(neuronCounts[i].first);
    }
    
    return result;
}

float SpikeSystem::getAverageDelay() const {
    if (delaySamples.empty()) {
        return propagationConfig.baseDelay;
    }
    
    float sum = 0.0f;
    for (float delay : delaySamples) {
        sum += delay;
    }
    return sum / static_cast<float>(delaySamples.size());
}

float SpikeSystem::getDelayVariance() const {
    if (delaySamples.size() < 2) {
        return 0.0f;
    }
    
    float mean = getAverageDelay();
    float sumSqDiff = 0.0f;
    for (float delay : delaySamples) {
        float diff = delay - mean;
        sumSqDiff += diff * diff;
    }
    
    return sumSqDiff / static_cast<float>(delaySamples.size());
}

size_t SpikeSystem::getTotalEventsProcessed() const {
    return totalEventsProcessed;
}

size_t SpikeSystem::getHandlersExecuted() const {
    return totalHandlersExecuted;
}

float SpikeSystem::getProcessingEfficiency() const {
    if (totalEventsProcessed == 0) {
        return 0.0f;
    }
    
    return static_cast<float>(totalHandlersExecuted) / 
           static_cast<float>(totalEventsProcessed);
}

void SpikeSystem::reset() {
    while (!pendingSpikes.empty()) {
        pendingSpikes.pop();
    }
    while (!pendingDelayedSpikes.empty()) {
        pendingDelayedSpikes.pop();
    }
    spikeHistory.clear();
    spikeCountPerNeuron.clear();
    delaySamples.clear();
    
    // Clear memory pool if enabled
    if (optimizationConfig.enableMemoryPool && eventPool) {
        eventPool->clear();
    }
}

void SpikeSystem::setHistorySize(size_t maxHistory) {
    maxHistorySize = maxHistory;
}

void SpikeSystem::setMemoryPoolSize(size_t initialSize, size_t maxSize) {
    if (optimizationConfig.enableMemoryPool && !eventPool) {
        eventPool.reset();
        eventPool = std::make_unique<SpikeEventPool>(initialSize, maxSize);
    }
}

void SpikeSystem::configurePropagation(const PropagationConfig& config) {
    propagationConfig = config;
    
    // Reinitialize random distributions
    std::random_device rd;
    pImpl->rng.seed(rd());
    uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    normalDist = std::normal_distribution<float>(0.0f, 1.0f);
}

void SpikeSystem::configureOptimizations(const OptimizationConfig& config) {
    optimizationConfig = config;
    
    // Reinitialize memory pool if needed
    if (config.enableMemoryPool && !eventPool) {
        eventPool.reset();
        eventPool = std::make_unique<SpikeEventPool>(config.memoryPoolInitialSize,
                                                    config.memoryPoolMaxSize);
    }
}

void SpikeSystem::queueLegacySpike(const SpikeEvent& event) {
    DetailedSpikeEvent enhancedEvent(
        event.source_neuron,
        event.timestamp,
        event.step,
        RegionId(),
        PopulationId(),
        1.0f, // default delay
        true  // default excitatory
    );
    enhancedEvent.synapticWeight = 1.0f;
    enhancedEvent.efficacy = 1.0f;
    enhancedEvent.delivered = false;
    
    if (optimizationConfig.enableMemoryPool && eventPool) {
        DetailedSpikeEvent* pooledEvent = eventPool->acquire();
        if (pooledEvent) {
            *pooledEvent = enhancedEvent;
            pendingSpikes.push(*pooledEvent);
            return;
        }
    }
    pendingSpikes.push(enhancedEvent);
}

void SpikeSystem::queueLegacySpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    DetailedSpikeEvent enhancedEvent(neuron, timestamp, step, RegionId(), PopulationId());
    enhancedEvent.synapticWeight = 1.0f;
    enhancedEvent.efficacy = 1.0f;
    enhancedEvent.delivered = false;
    
    if (optimizationConfig.enableMemoryPool && eventPool) {
        DetailedSpikeEvent* pooledEvent = eventPool->acquire();
        if (pooledEvent) {
            *pooledEvent = enhancedEvent;
            pendingDelayedSpikes.push(*pooledEvent);
            return;
        }
    }
    pendingDelayedSpikes.push(enhancedEvent);
}

void SpikeSystem::queueLegacyDelayedSpike(const DelayedSpikeEvent& event) {
    DetailedSpikeEvent enhancedEvent(
        event.source_neuron,
        event.timestamp,
        event.step,
        RegionId(),
        PopulationId(),
        (event.delivery_time - event.timestamp) * 1000.0f,  // Convert to ms
        event.is_excitatory
    );
    enhancedEvent.synapticWeight = event.weight;
    enhancedEvent.efficacy = std::abs(event.weight);
    enhancedEvent.delivered = false;
    
    if (optimizationConfig.enableMemoryPool && eventPool) {
        DetailedSpikeEvent* pooledEvent = eventPool->acquire();
        if (pooledEvent) {
            *pooledEvent = enhancedEvent;
            pendingDelayedSpikes.push(*pooledEvent);
            return;
        }
    }
    pendingDelayedSpikes.push(enhancedEvent);
}

} // namespace nlm
