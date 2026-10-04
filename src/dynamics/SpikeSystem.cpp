#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>

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
    
    Impl() : maxHistorySize(10000) {
        try {
            // Initialize collections safely
            spikeHistory.reserve(maxHistorySize);
            spikeCountPerNeuron.reserve(10000);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize SpikeSystem collections: " + std::string(e.what()));
        }
    }
};

SpikeSystem::SpikeSystem() : pImpl(new Impl) {
    try {
        // Initialize additional safety mechanisms
        if (!pImpl) {
            throw std::runtime_error("Failed to allocate SpikeSystem implementation");
        }
        NLM_LOG_INFO("SpikeSystem initialized successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize SpikeSystem: " + std::string(e.what()));
        throw;
    }
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

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    try {
        if (!pImpl) {
            throw std::runtime_error("SpikeSystem not properly initialized");
        }
        pImpl->pendingSpikes.push(event);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error queuing spike: ") + e.what());
        throw;
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    try {
        if (!pImpl) {
            throw std::runtime_error("SpikeSystem not properly initialized");
        }
        pImpl->pendingSpikes.emplace(neuron, timestamp, step);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error queuing spike with parameters: ") + e.what());
        throw;
    }
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    try {
        if (!pImpl) {
            throw std::runtime_error("SpikeSystem not properly initialized");
        }
        // Store by delivery step for efficient processing
        pImpl->delayedSpikes[event.delivery_step].push_back(event);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error queuing delayed spike: ") + e.what());
        throw;
    }
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    try {
        if (!pImpl) {
            throw std::runtime_error("SpikeSystem not properly initialized");
        }
        
        // Process all pending immediate spikes
        while (!pImpl->pendingSpikes.empty()) {
            SpikeEvent event = pImpl->pendingSpikes.front();
            pImpl->pendingSpikes.pop();
            
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
                if (handler) {
                    handler(detailed);
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error processing spikes: ") + e.what());
        throw;
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    try {
        if (!pImpl) {
            throw std::runtime_error("SpikeSystem not properly initialized");
        }
        
        // Find and process all delayed spikes scheduled for this step
        auto it = pImpl->delayedSpikes.find(currentStep);
        if (it != pImpl->delayedSpikes.end()) {
            // Process all spikes scheduled for this step
            for (const auto& delayedEvent : it->second) {
                // Call delayed spike handlers (these will deliver synaptic input)
                for (auto& handler : pImpl->delayedHandlers) {
                    if (handler) {
                        handler(delayedEvent);
                    }
                }
            }
            // Remove processed spikes
            pImpl->delayedSpikes.erase(it);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error processing delayed spikes: ") + e.what());
        throw;
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    pImpl->handlers.push_back(handler);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    pImpl->delayedHandlers.push_back(handler);
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
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
