#include "NeuralWorkingMemory.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <chrono>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Enhanced memory traces with neural connections
    struct MemoryTrace {
        std::vector<float> pattern;
        float strength;
        SimulationStep age;
        float decayRate;
        bool persistent;
        
        MemoryTrace() : strength(0.0f), age(0), decayRate(0.95f), persistent(false) {}
    };
    
    std::deque<MemoryTrace> traces;
    
    // Neural connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections;
    
    // Performance optimizations
    size_t maxTraces;
    float decayRate;
    float interferenceThreshold;
    bool usePriorityQueue;
    
    // Statistics
    size_t totalTracesCreated;
    size_t totalTracesExpired;
    size_t activeTraces;
    float averageTraceStrength;
    
    Impl() : brain(nullptr), maxTraces(1000), decayRate(0.95f), 
             interferenceThreshold(0.1f), usePriorityQueue(true),
             totalTracesCreated(0), totalTracesExpired(0),
             activeTraces(0), averageTraceStrength(0.0f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Check for interference with existing traces
    bool shouldStore = true;
    if (pImpl->usePriorityQueue) {
        for (const auto& trace : pImpl->traces) {
            float interference = calculateInterference(pattern, trace.pattern);
            if (interference < pImpl->interferenceThreshold) {
                shouldStore = false;
                break;
            }
        }
    }
    
    if (shouldStore) {
        MemoryTrace newTrace;
        newTrace.pattern = pattern;
        newTrace.strength = strength;
        newTrace.age = 0;
        newTrace.decayRate = pImpl->decayRate;
        newTrace.persistent = false;
        
        // Remove lowest strength traces if at capacity
        if (pImpl->traces.size() >= pImpl->maxTraces) {
            std::partial_sort(pImpl->traces.begin(), 
                            pImpl->traces.begin() + pImpl->maxTraces / 2,
                            pImpl->traces.end(),
                            [](const MemoryTrace& a, const MemoryTrace& b) {
                                return a.strength > b.strength;
                            });
            pImpl->traces.resize(pImpl->maxTraces / 2);
            pImpl->totalTracesExpired += pImpl->maxTraces / 2;
        }
        
        pImpl->traces.push_back(newTrace);
        pImpl->totalTracesCreated++;
        
        // Create neural representation
        createNeuralRepresentation(newTrace);
        
        // Create maintenance connections
        createMaintenanceConnections();
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

void NeuralWorkingMemory::update(float dt) {
    // Update all traces
    for (auto& trace : pImpl->traces) {
        trace.age += dt;
        trace.strength *= trace.decayRate;
        
        if (trace.strength < 0.001f) {
            trace.strength = 0.0f;
        }
    }
    
    // Remove expired traces
    auto it = std::remove_if(pImpl->traces.begin(), pImpl->traces.end(),
        [](const MemoryTrace& trace) {
            return trace.strength < 0.001f || 
                   trace.age > 10000.0f;  // 10 second duration limit
        });
    
    pImpl->totalTracesExpired += std::distance(it, pImpl->traces.end());
    pImpl->traces.erase(it, pImpl->traces.end());
    
    updateStatistics();
}

void NeuralWorkingMemory::clear() {
    pImpl->traces.clear();
    pImpl->maintenanceSynapses.clear();
    pImpl->recurrentConnections.clear();
    
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    
    pImpl->totalTracesCreated = 0;
    pImpl->totalTracesExpired = 0;
    pImpl->activeTraces = 0;
    pImpl->averageTraceStrength = 0.0f;
}

void NeuralWorkingMemory::createNeuralRepresentation(const MemoryTrace& trace) {
    if (!brain_ || trace.pattern.empty()) return;
    
    // Find suitable neurons for this pattern
    size_t neuronsNeeded = std::min(trace.pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = trace.pattern[i] * trace.strength;
        
        // Set neuron activation
        storeToNeuron(neuron, activation);
    }
}

void NeuralWorkingMemory::createMaintenanceConnections() {
    if (!brain_ || pImpl->traces.empty()) return;
    
    // Create recurrent connections between neurons
    for (size_t i = 0; i < memoryNeurons_.size() - 1; ++i) {
        NeuronId source = memoryNeurons_[i];
        NeuronId target = memoryNeurons_[i + 1];
        
        // Create maintenance synapse
        createRecurrentConnection(source, target, 0.5f);
    }
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId source, NeuronId target, float strength) {
    if (!brain_) return;
    
    // Find source and target neurons
    auto* sourceRegion = brain_->getRegion(source.getId() / 1000);
    auto* targetRegion = brain_->getRegion(target.getId() / 1000);
    
    if (sourceRegion && targetRegion) {
        auto sourceNeurons = sourceRegion->getAllNeurons();
        auto targetNeurons = targetRegion->getAllNeurons();
        
        for (auto* srcN : *sourceNeurons) {
            if (srcN->getId() == source) {
                for (auto* tgtN : *targetNeurons) {
                    if (tgtN->getId() == target) {
                        // Create synapse (simplified)
                        pImpl->maintenanceSynapses.emplace_back(source, target);
                        pImpl->recurrentConnections.emplace_back(source, target);
                        
                        // Set connection strength
                        break;
                    }
                }
                break;
            }
        }
    }
}

float NeuralWorkingMemory::calculateInterference(const std::vector<float>& pattern1, 
                                               const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) {
        return 1.0f; // No interference if patterns don't match
    }
    
    float interference = 0.0f;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        interference += std::abs(pattern1[i] - pattern2[i]);
    }
    
    return interference / pattern1.size();
}

void NeuralWorkingMemory::updateStatistics() {
    if (pImpl->traces.empty()) {
        pImpl->activeTraces = 0;
        pImpl->averageTraceStrength = 0.0f;
        return;
    }
    
    // Count active traces (strength > threshold)
    size_t activeCount = 0;
    float totalStrength = 0.0f;
    
    for (const auto& trace : pImpl->traces) {
        if (trace.strength > 0.001f) {
            activeCount++;
        }
        totalStrength += trace.strength;
    }
    
    pImpl->activeTraces = activeCount;
    pImpl->averageTraceStrength = totalStrength / pImpl->traces.size();
}

std::string NeuralWorkingMemory::getMemoryStats() const {
    std::ostringstream oss;
    oss << "Neural Working Memory Statistics:" << std::endl;
    oss << "  Total traces created: " << pImpl->totalTracesCreated << std::endl;
    oss << "  Total traces expired: " << pImpl->totalTracesExpired << std::endl;
    oss << "  Active traces: " << pImpl->activeTraces << std::endl;
    oss << "  Maximum traces: " << pImpl->maxTraces << std::endl;
    oss << "  Decay rate: " << pImpl->decayRate << std::endl;
    oss << "  Average trace strength: " << pImpl->averageTraceStrength << std::endl;
    oss << "  Trace capacity usage: " << (pImpl->traces.size() / (float)pImpl->maxTraces * 100.0f) << "%" << std::endl;
    oss << "  Memory neurons: " << memoryNeurons_.size() << std::endl;
    oss << "  Maintenance synapses: " << pImpl->maintenanceSynapses.size() << std::endl;
    oss << "  Recurrent connections: " << pImpl->recurrentConnections.size() << std::endl;
    
    return oss.str();
}

} // namespace nlm