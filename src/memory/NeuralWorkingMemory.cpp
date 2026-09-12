#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Neural dynamics state
    std::vector<float> membranePotentials;
    std::vector<float> firingRates;
    std::vector<float> synapticWeights;
    
    // Random generator for stochastic dynamics
    std::mt19937 rng;
    
    Impl() : brain(nullptr), rng(std::random_device{}()) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    // Initialize with random seed for neural dynamics
    std::random_device rd;
    pImpl->rng.seed(rd());
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize neural populations with realistic dynamics
    clear();
    
    // Set up neural dynamics parameters
    pImpl->membranePotentials.resize(capacity_, -70.0f);  // Resting potential
    pImpl->firingRates.resize(capacity_, 0.0f);
    pImpl->synapticWeights.resize(capacity_, 0.5f);
    
    // Create initial population of memory neurons
    if (brain_ && capacity_ > 0) {
        // Get a region to place memory neurons
        auto* region = brain_->getRegion(RegionId(1));
        if (!region && brain_->getRegionCount() > 0) {
            region = brain_->getRegion(RegionId(1));
        }
        
        if (region) {
            // Create memory neurons with appropriate properties
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= capacity_) {
                // Use existing neurons
                auto it = neurons.begin();
                for (size_t i = 0; i < capacity_; ++i, ++it) {
                    memoryNeurons_.push_back((*it)->getId());
                }
            } else {
                // Create additional neurons if needed
                for (size_t i = memoryNeurons_.size(); i < capacity_; ++i) {
                    memoryNeurons_.push_back(NeuronId(i + 1000));
                }
            }
        } else {
            // Fallback: create dummy neuron IDs
            for (size_t i = 0; i < capacity_; ++i) {
                memoryNeurons_.push_back(NeuronId(i + 1000));
            }
        }
    }
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with " + std::to_string(capacity_) + " memory neurons");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation through recurrent dynamics
        if (brain_) {
            // Inject current to set the desired activation
            brain_->injectCurrent(neuron, activation * 5.0f);
            
            // Establish or strengthen recurrent connections
            if (i > 0) {
                createRecurrentConnection(memoryNeurons_[(i-1) % memoryNeurons_.size()], 
                                        neuron, strength * 0.5f);
            }
        }
        
        // Update stored activation with neural dynamics
        if (i < memoryActivations_.size()) {
            // Neural dynamics: activation doesn't instantly change
            float neuralChange = activation - memoryActivations_[i];
            memoryActivations_[i] += neuralChange * 0.1f;  // Time constant
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            if (i < memoryNeurons_.size()) {
                memoryNeurons_[i] = neuron;
            }
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < std::min<size_t>(memoryNeurons_.size(), 10); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        
        // Neural dynamics: gradual activation change
        float currentActivation = memoryActivations_[idx];
        float delta = activation - currentActivation;
        memoryActivations_[idx] += delta * 0.2f;  // Time constant
        
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
    }
    
    // Inject current to maintain activation - this creates persistent neural activity
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 3.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    // Apply neural dynamics - retrieve with smoothing
    for (float activation : memoryActivations_) {
        // Neural activation has noise and decay
        float noise = ((std::mt19937(std::random_device{}())() % 1000) / 1000.0f - 0.5f) * 0.05f;
        result.push_back(activation * 0.9f + noise);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current to sustain activity
            brain_->injectCurrent(neuron, activation * 1.5f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Neural dynamics: activation can change over time
            float neuralIntegration = brain_->getRegion(RegionId(neuron.getId() / 1000))->getAllNeurons();
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_ * dt);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Decay weak traces with neural dynamics
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
    
    // Reset neural dynamics state
    pImpl->membranePotentials.clear();
    pImpl->firingRates.clear();
    pImpl->synapticWeights.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply strong inhibition to enforce winner-take-all
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 2.5f);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    return total / memoryActivations_.size();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
    
    // Apply connection strength through neural dynamics
    if (brain_) {
        brain_->injectCurrent(to, strength * 2.0f);
    }
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal through recurrent connection
            brain_->injectCurrent(conn.second, fromActivation * 1.5f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        // Neural decay with time constant
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
        
        // Remove associated maintenance synapses
        pImpl->maintenanceSynapses.erase(
            std::remove_if(pImpl->maintenanceSynapses.begin(), pImpl->maintenanceSynapses.end(),
                         [this, idx = *it](const auto& conn) {
                             return conn.first == memoryNeurons_[idx] || 
                                    conn.second == memoryNeurons_[idx];
                         }),
            pImpl->maintenanceSynapses.end()
        );
    }
}

} // namespace nlm