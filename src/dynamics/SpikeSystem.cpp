#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace nlm {

struct SpikeSystem::Impl {
    std::queue<SpikeEvent> pendingSpikes;
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;  // Grouped by delivery step
    size_t maxHistorySize;
    
    // Spike counting for statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;  // neuron_id -> count
    
    // Performance optimization: Pre-allocate common containers
    static constexpr size_t PENDING_SPIKES_RESERVE = 1000;
    static constexpr size_t HANDLERS_RESERVE = 20;
    static constexpr size_t DELAYED_HANDLERS_RESERVE = 10;
    
    Impl() : maxHistorySize(10000) {
        // Pre-allocate for common usage patterns
        pendingSpikes.reserve(PENDING_SPIKES_RESERVE);
        handlers.reserve(HANDLERS_RESERVE);
        delayedHandlers.reserve(DELAYED_HANDLERS_RESERVE);
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

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    // Validate input before processing
    if (!event.source_neuron) {
        NLM_LOG_ERROR("Attempt to queue spike with invalid neuron ID");
        return;
    }
    
    if (std::isnan(event.timestamp) || event.timestamp < 0) {
        NLM_LOG_ERROR("Attempt to queue spike with invalid timestamp: " + std::to_string(event.timestamp));
        return;
    }
    
    pImpl->pendingSpikes.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    // Validate input parameters
    if (!neuron) {
        NLM_LOG_ERROR("Attempt to queue spike with invalid neuron ID");
        return;
    }
    
    if (std::isnan(timestamp) || timestamp < 0) {
        NLM_LOG_ERROR("Attempt to queue spike with invalid timestamp: " + std::to_string(timestamp));
        return;
    }
    
    if (std::isnan(step) || step < 0) {
        NLM_LOG_ERROR("Attempt to queue spike with invalid step: " + std::to_string(step));
        return;
    }
    
    pImpl->pendingSpikes.emplace(neuron, timestamp, step);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Validate input
    if (!event.source_neuron) {
        NLM_LOG_ERROR("Attempt to queue delayed spike with invalid neuron ID");
        return;
    }
    
    if (std::isnan(event.timestamp) || event.timestamp < 0) {
        NLM_LOG_ERROR("Attempt to queue delayed spike with invalid timestamp: " + std::to_string(event.timestamp));
        return;
    }
    
    if (std::isnan(event.delivery_step) || event.delivery_step < 0) {
        NLM_LOG_ERROR("Attempt to queue delayed spike with invalid delivery step: " + std::to_string(event.delivery_step));
        return;
    }
    
    // Store by delivery step for efficient processing
    pImpl->delayedSpikes[event.delivery_step].push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Validate current step
    if (std::isnan(currentStep) || currentStep < 0) {
        NLM_LOG_ERROR("Attempt to process spikes with invalid current step: " + std::to_string(currentStep));
        return;
    }
    
    // Process all pending immediate spikes
    while (!pImpl->pendingSpikes.empty()) {
        SpikeEvent event = pImpl->pendingSpikes.front();
        pImpl->pendingSpikes.pop();
        
        // Validate event
        if (!event.source_neuron) {
            NLM_LOG_ERROR("Skipping spike event with invalid neuron ID");
            continue;
        }
        
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        // Trim history if needed
        if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
        
        // Call handlers
        for (auto& handler : pImpl->handlers) {
            try {
                handler(detailed);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in spike handler: " + std::string(e.what()));
            }
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Validate input parameters
    if (std::isnan(currentStep) || currentStep < 0) {
        NLM_LOG_ERROR("Attempt to process delayed spikes with invalid current step: " + std::to_string(currentStep));
        return;
    }
    
    if (std::isnan(currentTime) || currentTime < 0) {
        NLM_LOG_ERROR("Attempt to process delayed spikes with invalid current time: " + std::to_string(currentTime));
        return;
    }
    
    // Find and process all delayed spikes scheduled for this step
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it != pImpl->delayedSpikes.end()) {
        // Process all spikes scheduled for this step
        for (const auto& delayedEvent : it->second) {
            // Call delayed spike handlers (these will deliver synaptic input)
            for (auto& handler : pImpl->delayedHandlers) {
                try {
                    handler(delayedEvent);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error in delayed spike handler: " + std::string(e.what()));
                }
            }
        }
        // Remove processed spikes
        pImpl->delayedSpikes.erase(it);
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    if (!handler) {
        NLM_LOG_ERROR("Attempt to register null spike handler");
        return;
    }
    
    pImpl->handlers.push_back(handler);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    if (!handler) {
        NLM_LOG_ERROR("Attempt to register null delayed spike handler");
        return;
    }
    
    pImpl->delayedHandlers.push_back(handler);
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
}

void SpikeSystem::setMaxHistorySize(size_t size) {
    pImpl->maxHistorySize = size;
    
    // Trim history if necessary
    while (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void SpikeSystem::getStatistics(SpikeStatistics& stats) const {
    // Calculate comprehensive spike statistics
    stats.totalSpikes = pImpl->spikeHistory.size();
    
    if (stats.totalSpikes < 2) {
        stats.averageISI = 0.0f;
        stats.firingRate = 0.0f;
        stats.coefficientOfVariation = 0.0f;
        return;
    }
    
    // Find time range and calculate statistics
    float minTime = std::numeric_limits<float>::max();
    float maxTime = -std::numeric_limits<float>::max();
    std::vector<float> intervals;
    
    for (const auto& spike : pImpl->spikeHistory) {
        minTime = std::min(minTime, static_cast<float>(spike.timestamp));
        maxTime = std::max(maxTime, static_cast<float>(spike.timestamp));
    }
    
    float duration = maxTime - minTime;
    if (duration <= 0.0f) {
        stats.averageISI = 0.0f;
        stats.firingRate = 0.0f;
        stats.coefficientOfVariation = 0.0f;
        return;
    }
    
    stats.firingRate = static_cast<float>(pImpl->spikeHistory.size()) / duration;
    
    // Calculate inter-spike intervals
    if (pImpl->spikeHistory.size() > 1) {
        for (size_t i = 1; i < pImpl->spikeHistory.size(); ++i) {
            float interval = pImpl->spikeHistory[i].timestamp - pImpl->spikeHistory[i-1].timestamp;
            if (interval > 0) {
                intervals.push_back(interval);
            }
        }
        
        if (!intervals.empty()) {
            float sum = std::accumulate(intervals.begin(), intervals.end(), 0.0f);
            stats.averageISI = sum / intervals.size();
            
            // Calculate coefficient of variation
            if (stats.averageISI > 0) {
                float varianceSum = 0.0f;
                for (float interval : intervals) {
                    float diff = interval - stats.averageISI;
                    varianceSum += diff * diff;
                }
                float variance = varianceSum / intervals.size();
                stats.coefficientOfVariation = std::sqrt(variance) / stats.averageISI;
            }
        }
    }
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    size_t count = 0;
    for (const auto& pair : pImpl->delayedSpikes) {
        count += pair.second.size();
    }
    return count;
}

float SpikeSystem::getAverageSpikeRate() const {
    // Calculate average spike rate based on history
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
    if (count == 0) {
        return {};
    }
    
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

void SpikeSystem::reset() {
    while (!pImpl->pendingSpikes.empty()) {
        pImpl->pendingSpikes.pop();
    }
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikes.clear();
}

} // namespace nlm