# pragma once

#include "Memory.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

namespace nlm {

/**
 * @brief Base memory system providing common functionality
 * 
 * This class serves as the foundation for all specialized memory systems in NLM.
 * It provides common operations like clearing, accessing traces, and basic memory management.
 */

Memory::Memory() : capacity(0), maxTraceValue(0.0f), currentTraceCount(0), 
                   maxTraceThreshold(1000), cleanupInterval(100), 
                   enableReplay(true), enableConsolidation(false) {}

Memory::~Memory() = default;

void Memory::clear() {
    traces.clear();
    currentTraceCount = 0;
    maxTraceValue = 0.0f;
    NLM_LOG_DEBUG("Memory cleared");
}

void Memory::setCapacity(size_t cap) {
    capacity = cap;
    // If current traces exceed new capacity, keep the strongest ones
    if (traces.size() > capacity) {
        std::partial_sort(traces.begin(), traces.begin() + capacity, traces.end(),
                         [](const MemoryTrace& a, const MemoryTrace& b) {
                             return a.strength > b.strength;
                         });
        traces.resize(capacity);
        currentTraceCount = traces.size();
    }
}

void Memory::addTrace(const MemoryTrace& trace) {
    if (capacity == 0) {
        return; // No capacity for traces
    }
    
    // Check if we already have a trace for this neuron
    auto it = std::find_if(traces.begin(), traces.end(),
                           [&trace](const MemoryTrace& existing) {
                               return existing.neuronId == trace.neuronId;
                           });
    
    if (it != traces.end()) {
        // Update existing trace with higher strength
        if (trace.strength > it->strength) {
            *it = trace;
            updateTraceMax();
        }
    } else {
        // Add new trace
        if (traces.size() < capacity) {
            traces.push_back(trace);
            currentTraceCount++;
            if (trace.strength > maxTraceValue) {
                maxTraceValue = trace.strength;
            }
        } else {
            // Trace capacity full, replace weakest if new trace is stronger
            auto weakest = std::min_element(traces.begin(), traces.end(),
                                           [](const MemoryTrace& a, const MemoryTrace& b) {
                                               return a.strength < b.strength;
                                           });
            if (trace.strength > weakest->strength) {
                *weakest = trace;
                updateTraceMax();
            }
        }
    }
}

const MemoryTrace* Memory::getTrace(NeuronId neuronId) const {
    auto it = std::find_if(traces.begin(), traces.end(),
                           [&neuronId](const MemoryTrace& trace) {
                               return trace.neuronId == neuronId;
                           });
    
    if (it != traces.end()) {
        return &*it;
    }
    return nullptr;
}

std::vector<const MemoryTrace*> Memory::getTraces() const {
    std::vector<const MemoryTrace*> result;
    result.reserve(traces.size());
    for (const auto& trace : traces) {
        result.push_back(&trace);
    }
    return result;
}

void Memory::updateTraceMax() {
    if (traces.empty()) {
        maxTraceValue = 0.0f;
        return;
    }
    
    maxTraceValue = 0.0f;
    for (const auto& trace : traces) {
        if (trace.strength > maxTraceValue) {
            maxTraceValue = trace.strength;
        }
    }
}

float Memory::getMaxTraceValue() const {
    return maxTraceValue;
}

void Memory::setMaxTraceThreshold(size_t threshold) {
    maxTraceThreshold = threshold;
}

void Memory::setCleanupInterval(size_t interval) {
    cleanupInterval = interval;
}

void Memory::setEnableReplay(bool enable) {
    enableReplay = enable;
}

void Memory::setEnableConsolidation(bool enable) {
    enableConsolidation = enable;
}

void Memory::pruneWeakTraces(float strengthThreshold) {
    auto initialSize = traces.size();
    traces.erase(std::remove_if(traces.begin(), traces.end(),
                                [&strengthThreshold](const MemoryTrace& trace) {
                                    return trace.strength < strengthThreshold;
                                }),
                 traces.end());
    
    currentTraceCount = traces.size();
    
    if (initialSize != traces.size()) {
        NLM_LOG_DEBUG("Pruned " + std::to_string(initialSize - traces.size()) + 
                     " weak memory traces (threshold: " + std::to_string(strengthThreshold) + ")");
    }
}

void Memory::cleanup() {
    if (enableConsolidation) {
        pruneWeakTraces(maxTraceValue * 0.1f); // Remove traces weaker than 10% of max
    }
    
    // Reset trace counters periodically
    if (currentTraceCount > 0 && currentTraceCount % cleanupInterval == 0) {
        // Apply decay to all traces to simulate memory fading
        for (auto& trace : traces) {
            trace.strength *= 0.95f; // 5% decay per cleanup cycle
            trace.timeToLive = std::max(0.0f, trace.timeToLive - 1.0f);
        }
        
        removeExpiredTraces();
    }
}

void Memory::removeExpiredTraces() {
    auto initialSize = traces.size();
    traces.erase(std::remove_if(traces.begin(), traces.end(),
                                [](const MemoryTrace& trace) {
                                    return trace.timeToLive <= 0.0f;
                                }),
                 traces.end());
    
    currentTraceCount = traces.size();
}

void Memory::exportToStream(std::ostream& os) const {
    os << "Memory System Status:" << std::endl;
    os << "  Total traces: " << traces.size() << std::endl;
    os << "  Capacity: " << capacity << std::endl;
    os << "  Max trace strength: " << maxTraceValue << std::endl;
    os << "  Current traces: " << currentTraceCount << std::endl;
    os << "  Replay enabled: " << (enableReplay ? "Yes" : "No") << std::endl;
    os << "  Consolidation enabled: " << (enableConsolidation ? "Yes" : "No") << std::endl;
}

} // namespace nlm
