#include "NeuralWorkingMemory.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Logger/Logger.hpp"
#include <limits>

namespace nlm {

// Helper function to find nearest neuron in population
Neuron* findNearestNeuron(const std::vector<Neuron*>& neurons, NeuronId target) {
    Neuron* closest = nullptr;
    float minDist = std::numeric_limits<float>::max();
    
    for (Neuron* neuron : neurons) {
        if (neuron && neuron->getId() != INVALID_NEURON_ID) {
            // Simple distance calculation based on neuron ID (as proxy for location)
            float dist = std::abs(static_cast<float>(neuron->getId().value) - static_cast<float>(target.value));
            if (dist < minDist) {
                minDist = dist;
                closest = neuron;
            }
        }
    }
    return closest;
}

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Synaptic efficacy for maintenance connections
    std::vector<float> maintenanceEfficacy;
    
    // Neural population for pattern storage
    PopulationId patternPopulation;
    
    // Random generator for pattern completion
    std::mt19937 rng;
    std::uniform_real_distribution<float> activationDist;
    std::bernoulli_distribution patternDist;
    
    Impl() : brain(nullptr), patternPopulation(0), rng(std::random_device{}()), 
             activationDist(0.1f, 1.0f), patternDist(0.8f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    // Initialize random number generator
    std::random_device rd;
    pImpl->rng.seed(rd());
    pImpl->activationDist = std::uniform_real_distribution<float>(0.1f, 1.0f);
    pImpl->patternDist = std::bernoulli_distribution(0.8f);
    
    NLM_LOG_DEBUG("NeuralWorkingMemory constructed with capacity " + std::to_string(capacity_));
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize pattern storage population
    pImpl->patternPopulation = brain->addRegion("pattern_storage");
    
    // Create recurrent network for pattern maintenance
    setupRecurrentNetwork();
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with brain reference");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Get or create pattern storage neurons
    auto storageNeurons = getPatternStorageNeurons();
    size_t neuronsAvailable = storageNeurons.size();
    
    if (neuronsAvailable == 0) {
        NLM_LOG_WARNING("No pattern storage neurons available for working memory");
        return;
    }
    
    // Store pattern with competitive allocation
    storePatternCompetitively(pattern, strength);
    
    // Establish recurrent connections for maintenance
    establishRecurrentConnections(strength);
    
    // Initiate pattern completion through attractor dynamics
    initiatePatternCompletion(pattern, strength);
    
    NLM_LOG_DEBUG("Pattern stored in working memory with strength " + std::to_string(strength));
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        activeTraces_.push_back(idx);
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        activeTraces_.push_back(memoryNeurons_.size() - 1);
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
    
    // Create maintenance connection for this neuron
    createMaintenanceConnection(neuron);
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
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
    
    // Update maintenance - reinforce active memory neurons through attractor dynamics
    updateAttractorDynamics(dt);
    
    // Apply synaptic plasticity for maintenance connections
    updateMaintenancePlasticity(dt);
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
    
    // Update recurrent connections for maintenance
    updateRecurrentConnections();
}

void NeuralWorkingMemory::updateAttractorDynamics(TimestepDuration dt) {
    // Implement attractor dynamics for pattern completion
    // This creates stable persistent activity patterns
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        // Apply attractor dynamics - reinforce self-excitation
        if (activation > 0.1f) {
            // Self-excitation to maintain activation
            brain_->injectCurrent(neuron, activation * 1.5f);
            
            // Cross-talk from other neurons in the pattern
            for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
                if (i == j) continue;
                
                float otherActivation = memoryActivations_[j];
                if (otherActivation > 0.1f) {
                    // Mutual reinforcement
                    brain_->injectCurrent(neuron, otherActivation * 0.3f);
                }
            }
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old - apply decay
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
}

void NeuralWorkingMemory::updateMaintenancePlasticity(TimestepDuration dt) {
    // Apply STDP to maintenance connections to strengthen useful patterns
    if (brain_ && brain_->getSTDP()) {
        for (const auto& conn : pImpl->maintenanceSynapses) {
            // Find corresponding synapse in brain (simplified lookup)
            // In a real implementation, this would use proper synapse lookup
            
            // Apply Hebbian-like plasticity to maintenance connections
            // This strengthens connections between co-activated neurons
            if (memoryActivations_.size() > 1) {
                float avgActivation = 0.0f;
                for (float act : memoryActivations_) {
                    avgActivation += act;
                }
                avgActivation /= memoryActivations_.size();
                
                // Strengthen connections for neurons above average activation
                auto it1 = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
                auto it2 = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.second);
                
                if (it1 != memoryNeurons_.end() && it2 != memoryNeurons_.end()) {
                    size_t idx1 = std::distance(memoryNeurons_.begin(), it1);
                    size_t idx2 = std::distance(memoryNeurons_.begin(), it2);
                    
                    if (memoryActivations_[idx1] > avgActivation && 
                        memoryActivations_[idx2] > avgActivation) {
                        // This connection should be strengthened
                        // In real implementation, this would use plasticity rules
                    }
                }
            }
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        // Also decay based on trace age
        float ageDecay = std::min(1.0f, memoryTimestamps_[i] / 500.0f);
        memoryActivations_[i] *= (1.0f - ageDecay * decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Also remove corresponding connections
        for (auto connIt = pImpl->maintenanceSynapses.begin(); 
             connIt != pImpl->maintenanceSynapses.end(); ++connIt) {
            if ((connIt->first == memoryNeurons_[idx] && 
                 connIt->second == (idx > 0 ? memoryNeurons_[idx-1] : INVALID_NEURON_ID)) ||
                (connIt->first == (idx < memoryNeurons_.size()-1 ? memoryNeurons_[idx+1] : INVALID_NEURON_ID) && 
                 connIt->second == memoryNeurons_[idx])) {
                connIt = pImpl->maintenanceSynapses.erase(connIt);
                break;
            }
        }
        
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        
        // Remove from active traces
        auto traceIt = std::find(activeTraces_.begin(), activeTraces_.end(), idx);
        if (traceIt != activeTraces_.end()) {
            activeTraces_.erase(traceIt);
        }
    }
}

void NeuralWorkingMemory::setupRecurrentNetwork() {
    // Create recurrent network for pattern maintenance
    if (!brain_) return;
    
    // Get or create neural region for pattern storage
    NeuralRegion* region = brain_->getRegion(pImpl->patternPopulation);
    if (!region) return;
    
    // Initialize storage neurons
    auto& neurons = region->getAllNeurons();
    if (neurons) {
        for (Neuron* neuron : *neurons) {
            if (neuron) {
                // Configure neurons for working memory function
                neuron->setType(NeuronType::Internal);
                neuron->setRefractoryPeriod(10);  // Short refractory period for persistence
                neuron->enablePlasticity(true, true, false);  // Enable STDP and Hebbian
            }
        }
    }
}

std::vector<Neuron*> NeuralWorkingMemory::getPatternStorageNeurons() {
    std::vector<Neuron*> neurons;
    
    if (!brain_) return neurons;
    
    // Get pattern storage region
    NeuralRegion* region = brain_->getRegion(pImpl->patternPopulation);
    if (region) {
        auto& regionNeurons = region->getAllNeurons();
        if (regionNeurons) {
            neurons = *regionNeurons;
        }
    }
    
    return neurons;
}

void NeuralWorkingMemory::storePatternCompetitively(const std::vector<float>& pattern, float strength) {
    // Store pattern with competitive allocation and sparse coding
    
    // Get available storage neurons
    auto storageNeurons = getPatternStorageNeurons();
    if (storageNeurons.empty()) {
        NLM_LOG_WARNING("No pattern storage neurons available");
        return;
    }
    
    // Find neurons with highest activation levels
    std::vector<std::pair<NeuronId, float>> neuronActivations;
    
    for (size_t i = 0; i < std::min(pattern.size(), storageNeurons.size()); ++i) {
        Neuron* neuron = storageNeurons[i];
        if (neuron) {
            float activation = pattern[i] * strength;
            
            // Apply competition - neurons must exceed threshold to be stored
            float threshold = 0.2f;  // Sparse coding threshold
            if (activation > threshold) {
                NeuronId neuronId = neuron->getId();
                neuronActivations.push_back({neuronId, activation});
                
                // Store neuron and activation
                storeToNeuron(neuronId, activation);
            }
        }
    }
    
    // Sort by activation (strongest first) for competitive storage
    std::sort(neuronActivations.begin(), neuronActivations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Apply capacity constraints - keep top activated neurons
    if (neuronActivations.size() > capacity_) {
        neuronActivations.resize(capacity_);
    }
}

void NeuralWorkingMemory::establishRecurrentConnections(float strength) {
    // Establish recurrent connections for pattern maintenance
    
    // Create ring connections between stored neurons for attractor dynamics
    if (memoryNeurons_.size() < 2) return;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        size_t next = (i + 1) % memoryNeurons_.size();
        
        // Create forward connection
        createRecurrentConnection(memoryNeurons_[i], memoryNeurons_[next], 
                                 strength * 0.3f);
        
        // Create backward connection for bidirectional attraction
        createRecurrentConnection(memoryNeurons_[next], memoryNeurons_[i], 
                                 strength * 0.2f);
    }
}

void NeuralWorkingMemory::initiatePatternCompletion(const std::vector<float>& pattern, float strength) {
    // Initiate pattern completion through attractor dynamics
    // This uses partial patterns to trigger full pattern retrieval
    
    // Get current brain state
    auto storageNeurons = getPatternStorageNeurons();
    if (storageNeurons.empty()) return;
    
    // Create partial pattern for completion
    std::vector<float> partialPattern;
    size_t patternSize = std::min(pattern.size(), storageNeurons.size());
    
    // Apply probabilistic partial activation (simulating noisy retrieval)
    for (size_t i = 0; i < patternSize; ++i) {
        if (pImpl->patternDist()) {  // Bernoulli trial
            partialPattern.push_back(pattern[i] * strength * 0.5f);
        }
    }
    
    // If we have partial pattern, initiate completion
    if (!partialPattern.empty()) {
        // Inject partial activation to trigger attractor dynamics
        for (size_t i = 0; i < std::min(partialPattern.size(), storageNeurons.size()); ++i) {
            Neuron* neuron = storageNeurons[i];
            if (neuron) {
                float partialActivation = partialPattern[i];
                brain_->injectCurrent(neuron->getId(), partialActivation * 8.0f);
            }
        }
        
        NLM_LOG_DEBUG("Pattern completion initiated with partial pattern of size " + 
                     std::to_string(partialPattern.size()));
    }
}

void NeuralWorkingMemory::createMaintenanceConnection(NeuronId neuron) {
    // Create maintenance connection for a single neuron
    
    // Find neurons that could provide recurrent input
    auto storageNeurons = getPatternStorageNeurons();
    std::vector<NeuronId> neuronIds;
    
    for (Neuron* n : storageNeurons) {
        if (n) neuronIds.push_back(n->getId());
    }
    
    // Connect to a few random neurons in the population
    if (neuronIds.size() > 1) {
        std::shuffle(neuronIds.begin(), neuronIds.end(), pImpl->rng);
        
        // Create connections to 1-2 random neurons
        size_t numConns = std::min(size_t(2), neuronIds.size() - 1);
        
        for (size_t i = 0; i < numConns; ++i) {
            NeuronId target = neuronIds[i];
            if (target != neuron) {
                createRecurrentConnection(target, neuron, 0.5f);
            }
        }
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Multi-level competition
    // 1. First level: strong vs weak neurons
    std::vector<size_t> strongNeurons;
    std::vector<size_t> weakNeurons;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= 0.3f) {
            strongNeurons.push_back(i);
        } else if (memoryActivations_[i] > 0.1f) {
            weakNeurons.push_back(i);
        }
    }
    
    // 2. Strong vs weak competition
    if (!strongNeurons.empty() && !weakNeurons.empty()) {
        float avgStrong = 0.0f;
        for (size_t idx : strongNeurons) {
            avgStrong += memoryActivations_[idx];
        }
        avgStrong /= strongNeurons.size();
        
        for (size_t idx : weakNeurons) {
            // Inhibit weak neurons based on strong activity
            if (brain_) {
                brain_->injectCurrent(memoryNeurons_[idx], -avgStrong * 2.0f);
            }
        }
    }
    
    // 3. Strong neuron selection
    float threshold = 0.4f;
    for (size_t idx : strongNeurons) {
        if (memoryActivations_[idx] >= threshold) {
            winners_.push_back(memoryNeurons_[idx]);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(memoryNeurons_[idx], 2.0f);
            }
        }
    }
    
    // 4. If no strong neurons, promote strongest weak neuron
    if (winners_.empty() && !weakNeurons.empty()) {
        size_t bestIdx = weakNeurons[0];
        float bestAct = memoryActivations_[bestIdx];
        
        for (size_t idx : weakNeurons) {
            if (memoryActivations_[idx] > bestAct) {
                bestIdx = idx;
                bestAct = memoryActivations_[idx];
            }
        }
        
        winners_.push_back(memoryNeurons_[bestIdx]);
        if (brain_) {
            brain_->injectCurrent(memoryNeurons_[bestIdx], 1.0f);
        }
    }
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
    pImpl->maintenanceEfficacy.clear();
    
    // Reset attractor state
    winners_.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::isWinning(NeuronId neuron) const {
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
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

} // namespace nlm