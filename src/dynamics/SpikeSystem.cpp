#include "SpikeSystem.hpp"
#include "../core/Errors/Errors.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <cmath>

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
        if (maxHistorySize > MAX_HISTORY_SIZE) {
            throw SimulationException("Requested history size too large", __FILE__, __LINE__);
        }
    }
};

SpikeSystem::SpikeSystem() throw(SimulationException) : pImpl(new Impl) {
    try {
        pImpl = std::make_unique<Impl>();
    } catch (const std::bad_alloc& e) {
        throw SimulationException("Failed to allocate SpikeSystem resources", __FILE__, __LINE__);
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

void SpikeSystem::queueSpike(const SpikeEvent& event) throw(SimulationException, ValidationException) {
    try {
        validateSpikeEvent(event);
        pImpl->pendingSpikes.push(event);
    } catch (const ValidationException& e) {
        throw;
    } catch (const std::exception& e) {
        throw SimulationException(std::string("Failed to queue spike: ") + e.what(), __FILE__, __LINE__);
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) throw(SimulationException, ValidationException) {
    try {
        validateSpikeEvent(SpikeEvent(neuron, timestamp, step));
        pImpl->pendingSpikes.emplace(neuron, timestamp, step);
    } catch (const ValidationException& e) {
        throw;
    } catch (const std::exception& e) {
        throw SimulationException(std::string("Failed to queue spike: ") + e.what(), __FILE__, __LINE__);
    }
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) throw(SimulationException, ValidationException) {
    try {
        validateDelayedSpikeEvent(event);
        pImpl->delayedSpikes[event.delivery_step].push_back(event);
    } catch (const ValidationException& e) {
        throw;
    } catch (const std::exception& e) {
        throw SimulationException(std::string("Failed to queue delayed spike: ") + e.what(), __FILE__, __LINE__);
    }
}

void SpikeSystem::processSpikes(SimulationStep currentStep) throw(SimulationException) {
    try {
        validateStepParameters(currentStep, 0.0);
        validateHistoryLimits();
        
        // Process all pending immediate spikes
        while (!pImpl->pendingSpikes.empty()) {
            SpikeEvent event = pImpl->pendingSpikes.front();
            pImpl->pendingSpikes.pop();
            
            // Validate spike event parameters
            if (event.step > MAX_STEP) {
                throw SimulationException("Spike event step exceeds maximum value", __FILE__, __LINE__);
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
                    // Log handler error but continue processing
                    Logger::getGlobal().error(std::string("Spike handler error: ") + e.what(), __FILE__, __LINE__);
                }
            }
        }
    } catch (const ValidationException& e) {
        throw;
    } catch (const std::exception& e) {
        throw SimulationException(std::string("Failed to process spikes: ") + e.what(), __FILE__, __LINE__);
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) throw(SimulationException) {
    try {
        validateStepParameters(currentStep, currentTime);
        
        // Find and process all delayed spikes scheduled for this step
        auto it = pImpl->delayedSpikes.find(currentStep);
        if (it != pImpl->delayedSpikes.end()) {
            // Process all spikes scheduled for this step
            for (const auto& delayedEvent : it->second) {
                // Validate delayed event parameters
                if (delayedEvent.delivery_time < currentTime) {
                    throw SimulationException("Delayed spike delivery time in the past", __FILE__, __LINE__);
                }
                
                // Call delayed spike handlers (these will deliver synaptic input)
                for (auto& handler : pImpl->delayedHandlers) {
                    try {
                        handler(delayedEvent);
                    } catch (const std::exception& e) {
                        // Log handler error but continue processing
                        Logger::getGlobal().error(std::string("Delayed spike handler error: ") + e.what(), __FILE__, __LINE__);
                    }
                }
            }
            // Remove processed spikes
            pImpl->delayedSpikes.erase(it);
        }
    } catch (const ValidationException& e) {
        throw;
    } catch (const std::exception& e) {
        throw SimulationException(std::string("Failed to process delayed spikes: ") + e.what(), __FILE__, __LINE__);
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) throw(MemoryException) {
    if (!handler) {
        throw MemoryException("Cannot register null spike handler", __FILE__, __LINE__);
    }
    pImpl->handlers.push_back(handler);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) throw(MemoryException) {
    if (!handler) {
        throw MemoryException("Cannot register null delayed spike handler", __FILE__, __LINE__);
    }
    pImpl->delayedHandlers.push_back(handler);
}

void SpikeSystem::clearHistory() throw(SimulationException) {
    try {
        pImpl->spikeHistory.clear();
        pImpl->pendingSpikes = std::queue<SpikeEvent>();
        pImpl->delayedSpikes.clear();
        pImpl->spikeCountPerNeuron.clear();
    } catch (const std::exception& e) {
        throw SimulationException(std::string("Failed to clear spike history: ") + e.what(), __FILE__, __LINE__);
    }
}

void SpikeSystem::reset() throw(SimulationException) {
    try {
        clearHistory();
    } catch (const SimulationException& e) {
        throw;
    } catch (const std::exception& e) {
        throw SimulationException(std::string("Failed to reset spike system: ") + e.what(), __FILE__, __LINE__);
    }
}

// Validation implementations
void SpikeSystem::validateSpikeEvent(const SpikeEvent& event) const throw(ValidationException) {
    if (std::isnan(event.timestamp) || std::isinf(event.timestamp)) {
        throw ValidationException("Invalid spike timestamp (NaN or infinite)", __FILE__, __LINE__);
    }
    
    if (event.timestamp < 0.0) {
        throw ValidationException("Spike timestamp cannot be negative", __FILE__, __LINE__);
    }
    
    if (event.source_neuron.value == UINT64_MAX) {
        throw ValidationException("Invalid spike source neuron ID", __FILE__, __LINE__);
    }
    
    if (event.step > MAX_STEP) {
        throw RangeValidationException("Spike step", static_cast<double>(event.step), 0.0, static_cast<double>(MAX_STEP), __FILE__, __LINE__);
    }
}

void SpikeSystem::validateDelayedSpikeEvent(const DelayedSpikeEvent& event) const throw(ValidationException) {
    validateSpikeEvent(SpikeEvent(event.source_neuron, event.timestamp, event.step));
    
    if (event.destination_neuron.value == UINT64_MAX) {
        throw ValidationException("Invalid spike destination neuron ID", __FILE__, __LINE__);
    }
    
    if (event.synapse_id.value == UINT64_MAX) {
        throw ValidationException("Invalid spike synapse ID", __FILE__, __LINE__);
    }
    
    if (event.delivery_step > MAX_STEP) {
        throw RangeValidationException("Delayed spike delivery step", static_cast<double>(event.delivery_step), 0.0, static_cast<double>(MAX_STEP), __FILE__, __LINE__);
    }
    
    if (event.delivery_time < event.timestamp) {
        throw ValidationException("Delayed spike delivery time cannot be before spike time", __FILE__, __LINE__);
    }
    
    if (event.weight < MIN_WEIGHT || event.weight > MAX_WEIGHT) {
        throw RangeValidationException("Delayed spike weight", event.weight, MIN_WEIGHT, MAX_WEIGHT, __FILE__, __LINE__);
    }
}

void SpikeSystem::validateStepParameters(SimulationStep currentStep, Timestamp currentTime) const throw(ValidationException) {
    if (currentStep > MAX_STEP) {
        throw RangeValidationException("Current simulation step", static_cast<double>(currentStep), 0.0, static_cast<double>(MAX_STEP), __FILE__, __LINE__);
    }
    
    if (std::isnan(currentTime) || std::isinf(currentTime)) {
        throw ValidationException("Invalid current timestamp (NaN or infinite)", __FILE__, __LINE__);
    }
}

void SpikeSystem::validateHistoryLimits() const throw(ValidationException) {
    if (pImpl->maxHistorySize == 0) {
        throw ValidationException("History size cannot be zero", __FILE__, __LINE__);
    }
    
    if (pImpl->maxHistorySize > MAX_HISTORY_SIZE) {
        throw RangeValidationException("History size", static_cast<double>(pImpl->maxHistorySize), 0.0, static_cast<double>(MAX_HISTORY_SIZE), __FILE__, __LINE__);
    }
}

// Implement remaining methods
void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    registerDelayedHandler(std::move(handler));
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    size_t count = 0;
    for (const auto& entry : pImpl->delayedSpikes) {
        count += entry.second.size();
    }
    return count;
}

float SpikeSystem::getAverageSpikeRate() const {
    if (pImpl->spikeHistory.empty()) {
        return 0.0f;
    }
    
    if (pImpl->spikeHistory.size() < 2) {
        return static_cast<float>(pImpl->spikeHistory.size());
    }
    
    float totalTime = pImpl->spikeHistory.back().timestamp - pImpl->spikeHistory.front().timestamp;
    if (totalTime <= 0.0f) {
        return static_cast<float>(pImpl->spikeHistory.size());
    }
    
    return static_cast<float>(pImpl->spikeHistory.size()) / totalTime;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<uint64_t, size_t>> counts;
    for (const auto& entry : pImpl->spikeCountPerNeuron) {
        counts.emplace_back(entry.first, entry.second);
    }
    
    std::sort(counts.begin(), counts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NeuronId> result;
    result.reserve(std::min(count, counts.size()));
    
    for (size_t i = 0; i < std::min(count, counts.size()); ++i) {
        result.emplace_back(counts[i].first);
    }
    
    return result;
}

} // namespace nlm

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
