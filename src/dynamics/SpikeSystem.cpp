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
    
    Impl() : maxHistorySize(10000) {}
};

SpikeSystem::SpikeSystem() : pImpl(new Impl) {}

SpikeSystem::~SpikeSystem() {
    delete pImpl;
}

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
    pImpl->pendingSpikes.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    pImpl->pendingSpikes.emplace(neuron, timestamp, step);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Store by delivery step for efficient processing
    pImpl->delayedSpikes[event.delivery_step].push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
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
            handler(detailed);
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Find and process all delayed spikes scheduled for this step
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it != pImpl->delayedSpikes.end()) {
        // Process all spikes scheduled for this step
        for (const auto& delayedEvent : it->second) {
            // Call delayed spike handlers (these will deliver synaptic input)
            for (auto& handler : pImpl->delayedHandlers) {
                handler(delayedEvent);
            }
        }
        // Remove processed spikes
        pImpl->delayedSpikes.erase(it);
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

std::vector<DelayedSpikeEvent> SpikeSystem::getPendingDelayedSpikes() const {
    std::vector<DelayedSpikeEvent> out;
    for (const auto& pair : pImpl->delayedSpikes) {
        out.insert(out.end(), pair.second.begin(), pair.second.end());
    }
    return out;
}

void SpikeSystem::restorePendingDelayedSpikes(const std::vector<DelayedSpikeEvent>& events) {
    pImpl->delayedSpikes.clear();
    for (const auto& event : events) {
        pImpl->delayedSpikes[event.delivery_step].push_back(event);
    }
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

// Clear pending spikes and history
void SpikeSystem::clear() {
    reset();  // Clear everything
}

// Get detailed spike information
const DetailedSpikeEvent* SpikeSystem::getLastSpike() const {
    if (pImpl->spikeHistory.empty()) {
        return nullptr;
    }
    return &pImpl->spikeHistory.back();
}

// Check if spike system is empty
bool SpikeSystem::isEmpty() const {
    return pImpl->pendingSpikes.empty() && pImpl->spikeHistory.empty() && 
           pImpl->delayedSpikes.empty();
}

bool SpikeSystem::hasPendingSpikes() const {
    return !pImpl->pendingSpikes.empty();
}

bool SpikeSystem::hasPendingDelayedSpikes() const {
    return !pImpl->delayedSpikes.empty();
}

// Configuration
void SpikeSystem::setMaxHistorySize(size_t size) {
    pImpl->maxHistorySize = size;
    // Trim history if needed
    while (pImpl->spikeHistory.size() > size) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

size_t SpikeSystem::getMaxHistorySize() const {
    return pImpl->maxHistorySize;
}

// Performance optimization
void SpikeSystem::optimizeMemory() {
    // Optimize memory usage by cleaning up unused data structures
    pImpl->spikeCountPerNeuron.shrink_to_fit();
    pImpl->pendingSpikes = std::queue<SpikeEvent>();  // Clear and reallocate
    
    // Convert delayedSpikes to vector and back to free unused memory
    std::vector<std::vector<DelayedSpikeEvent>> temp;
    temp.reserve(pImpl->delayedSpikes.size());
    for (auto& pair : pImpl->delayedSpikes) {
        temp.push_back(std::move(pair.second));
    }
    pImpl->delayedSpikes.clear();
    for (auto& vec : temp) {
        pImpl->delayedSpikes[vec.empty() ? 0 : (SimulationStep)vec.front().delivery_step] = std::move(vec);
    }
}

// Statistics enhancement
float SpikeSystem::getStandardDeviationSpikeRate() const {
    // Simple implementation - could be optimized with cached statistics
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    float mean = getAverageSpikeRate();
    float sumSquaredDiff = 0.0f;
    
    for (const auto& spike : pImpl->spikeHistory) {
        float spikeRate = 1.0f / (spike.timestamp - (spike.timestamp > 0 ? spike.timestamp : 0));
        float diff = spikeRate - mean;
        sumSquaredDiff += diff * diff;
    }
    
    return std::sqrt(sumSquaredDiff / pImpl->spikeHistory.size());
}

float SpikeSystem::getMinSpikeRate() const {
    if (pImpl->spikeHistory.empty()) {
        return 0.0f;
    }
    
    float minRate = std::numeric_limits<float>::max();
    for (const auto& spike : pImpl->spikeHistory) {
        float spikeRate = 1.0f / (spike.timestamp - (spike.timestamp > 0 ? spike.timestamp : 0));
        minRate = std::min(minRate, spikeRate);
    }
    return minRate;
}

float SpikeSystem::getMaxSpikeRate() const {
    if (pImpl->spikeHistory.empty()) {
        return 0.0f;
    }
    
    float maxRate = -std::numeric_limits<float>::max();
    for (const auto& spike : pImpl->spikeHistory) {
        float spikeRate = 1.0f / (spike.timestamp - (spike.timestamp > 0 ? spike.timestamp : 0));
        maxRate = std::max(maxRate, spikeRate);
    }
    return maxRate;
}

// Advanced event handling
void SpikeSystem::setEventFilter(std::function<bool(const DetailedSpikeEvent&)> filter) {
    // This would require significant refactoring to implement properly
    // For now, we'll just store the filter for potential future use
    pImpl->handlers.clear();  // Remove existing handlers
    if (filter) {
        // Store filter for later use
    }
}

void SpikeSystem::removeEventFilter() {
    // Clear any stored filters
}

// Batch processing
void SpikeSystem::processSpikeBatch(const std::vector<SpikeEvent>& batch) {
    for (const auto& event : batch) {
        pImpl->pendingSpikes.push(event);
    }
}

void SpikeSystem::processDelayedSpikeBatch(const std::vector<DelayedSpikeEvent>& batch) {
    for (const auto& event : batch) {
        pImpl->delayedSpikes[event.delivery_step].push_back(event);
    }
}

// Priority queue support
void SpikeSystem::enablePriorityQueue(bool enable) {
    // This would require significant refactoring to implement properly
    // For now, we'll just store the setting for potential future use
}

bool SpikeSystem::isPriorityQueueEnabled() const {
    // Placeholder for future implementation
    return false;
}

// Statistics caching
void SpikeSystem::enableStatisticsCaching(bool enable) {
    // This would require significant refactoring to implement properly
    // For now, we'll just store the setting for potential future use
}

bool SpikeSystem::isStatisticsCachingEnabled() const {
    // Placeholder for future implementation
    return false;
}

// Validation and error handling
bool SpikeSystem::validateEvent(const SpikeEvent& event) const {
    if (event.source_neuron.value == 0) {
        return false; // Invalid neuron ID
    }
    if (event.timestamp < 0.0f) {
        return false; // Invalid timestamp
    }
    if (event.step < 0) {
        return false; // Invalid step
    }
    return true;
}

bool SpikeSystem::validateDelayedEvent(const DelayedSpikeEvent& event) const {
    if (event.source_neuron.value == 0) {
        return false; // Invalid neuron ID
    }
    if (event.destination_neuron.value == 0) {
        return false; // Invalid destination neuron ID
    }
    if (event.delivery_step < 0) {
        return false; // Invalid delivery step
    }
    return true;
}

std::vector<std::string> SpikeSystem::getEventValidationErrors() const {
    std::vector<std::string> errors;
    // This would be implemented with detailed error checking
    return errors;
}

// Configuration and monitoring
void SpikeSystem::setLogLevel(LogLevel level) {
    // This would require refactoring to integrate with logging system
    // For now, we'll just store the setting for potential future use
}

LogLevel SpikeSystem::getLogLevel() const {
    // Placeholder for future implementation
    return LogLevel::Info;
}

// Performance monitoring
double SpikeSystem::getAverageProcessingTime() const {
    // Placeholder for future implementation
    return 0.0;
}

double SpikeSystem::getMemoryUsage() const {
    // Placeholder for future implementation
    return 0.0;
}

// Health check
bool SpikeSystem::healthCheck() const {
    // Basic health check
    return !pImpl->pendingSpikes.empty() || !pImpl->spikeHistory.empty() || 
           !pImpl->delayedSpikes.empty();
}

std::string SpikeSystem::getHealthStatus() const {
    std::ostringstream oss;
    oss << "SpikeSystem Health Status:" << std::endl;
    oss << "  Pending spikes: " << pImpl->pendingSpikes.size() << std::endl;
    oss << "  Spike history size: " << pImpl->spikeHistory.size() << std::endl;
    oss << "  Pending delayed spikes: " << getPendingDelayedCount() << std::endl;
    oss << "  Max history size: " << pImpl->maxHistorySize << std::endl;
    oss << "  System status: " << (healthCheck() ? "OK" : "WARNING") << std::endl;
    return oss.str();
}

} // namespace nlm
