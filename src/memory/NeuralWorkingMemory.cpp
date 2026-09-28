#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Neuromodulator levels (dopamine, curiosity, novelty)
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Development stage (0.0 to 1.0, 0=child, 1=adult)
    float developmentalStage;
    
    // Critical period modulation
    bool criticalPeriodActive;
    SimulationStep criticalPeriodEnd;
    
    // Prediction error signals
    float predictionError;
    
    Impl() : brain(nullptr), dopamineLevel(0.5f), curiosityLevel(0.3f), noveltyLevel(0.2f),
              developmentalStage(0.5f), criticalPeriodActive(true), criticalPeriodEnd(10000),
              predictionError(0.0f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , neuromodulatorBaseStrength_(0.5f)
    , developmentalPlasticity_(1.0f)
{
    // Initialize with biologically realistic neuromodulator levels
    pImpl->dopamineLevel = neuromodulatorBaseStrength_;
    pImpl->curiosityLevel = neuromodulatorBaseStrength_ * 0.6f;
    pImpl->noveltyLevel = neuromodulatorBaseStrength_ * 0.4f;
    pImpl->developmentalStage = 0.5f;
    pImpl->criticalPeriodActive = true;
    pImpl->criticalPeriodEnd = 10000;
    pImpl->predictionError = 0.0f;
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Set development stage based on brain age
    pImpl->developmentalStage = brain_->getSimulationStep() / 1000.0f;
    if (pImpl->developmentalStage > 1.0f) pImpl->developmentalStage = 1.0f;
    
    // Critical period ends around developmental stage 0.8
    pImpl->criticalPeriodActive = pImpl->developmentalStage < 0.8f;
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Check for novelty - increase neuromodulator levels for novel patterns
    bool isNovel = checkNovelty(pattern);
    if (isNovel) {
        pImpl->noveltyLevel = std::min(1.0f, pImpl->noveltyLevel + 0.1f);
    }
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Apply neuromodulatory effects
        activation *= (1.0f + pImpl->dopamineLevel * 0.5f); // Dopamine enhances activation
        activation *= (1.0f + pImpl->curiosityLevel * 0.3f); // Curiosity boosts encoding
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation with developmental modulation
        float developmentalFactor = 1.0f + (1.0f - pImpl->developmentalStage) * 0.5f;
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation * developmentalFactor;
        } else {
            memoryActivations_.push_back(activation * developmentalFactor);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
    
    // Apply STDP-like plasticity through neuromodulation
    applyNeuromodulatoryPlasticity(pattern);
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        
        // Apply neuromodulatory effects
        float neuromodFactor = 1.0f + pImpl->dopamineLevel * 0.3f + pImpl->curiosityLevel * 0.2f;
        memoryActivations_[idx] = activation * neuromodFactor;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        // New trace with neuromodulatory enhancement
        float neuromodFactor = 1.0f + pImpl->dopamineLevel * 0.3f + pImpl->curiosityLevel * 0.2f;
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation * neuromodFactor);
        memoryTimestamps_.push_back(0);
    }
    
    // Apply maintenance without constant external injection
    // Use persistent activity through recurrent connections
    createMaintenanceTrace(neuron, activation);
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
    
    // Update neuromodulators based on memory activity and prediction errors
    updateNeuromodulators();
    
    // Update maintenance with enhanced neuromodulatory effects
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Apply maintenance through enhanced recurrent connections
            // and neuromodulatory support
            float maintenanceStrength = 1.0f;
            
            // Dopamine supports trace maintenance
            maintenanceStrength += pImpl->dopamineLevel * 0.5f;
            
            // Curiosity boosts trace stability
            maintenanceStrength += pImpl->curiosityLevel * 0.3f;
            
            // Novelty enhances memory consolidation
            if (pImpl->noveltyLevel > 0.5f) {
                maintenanceStrength *= 1.2f;
            }
            
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f * maintenanceStrength);
            
            // Age the trace (neuromodulator-dependent decay)
            float decayRate = decayRate_;
            if (pImpl->criticalPeriodActive) {
                // Critical period: faster decay for flexibility
                decayRate *= 2.0f;
            }
            
            // Development-dependent decay
            decayRate *= (1.0f + 0.5f * (1.0f - pImpl->developmentalStage));
            
            memoryTimestamps_[i]++;
            
            // Check if trace should be consolidated or decayed
            if (memoryTimestamps_[i] > 1000) {
                // Trace is old - may need consolidation
                if (activation > 0.3f) {
                    // Strong enough to be consolidated
                    memoryActivations_[i] *= (1.0f - decayRate * 0.5f);
                } else {
                    // Weak trace - faster decay
                    memoryActivations_[i] *= (1.0f - decayRate * 2.0f);
                }
            } else {
                // Recent trace - normal decay
                memoryActivations_[i] *= (1.0f - decayRate);
            }
            
            memoryActivations_[i] = std::max(0.0f, memoryActivations_[i]);
        }
    }
    
    // Update recurrent connections with neuromodulatory effects
    updateRecurrentConnectionsWithNeuromodulation();
    
    // Decay weak traces with neuromodulation
    decayWeakTraces();
    
    // Run competition with enhanced neuromodulatory influence
    runCompetitionWithNeuromodulation();
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
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
            // Apply strong inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

float NeuralWorkingMemory::getCuriosityLevel() const {
    return pImpl->curiosityLevel;
}

float NeuralWorkingMemory::getNoveltyLevel() const {
    return pImpl->noveltyLevel;
}

float NeuralWorkingMemory::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

float NeuralWorkingMemory::getPredictionError() const {
    return pImpl->predictionError;
}

void NeuralWorkingMemory::updateNeuromodulators() {
    // Update neuromodulators based on memory activity
    float totalActivity = getMemoryActivity();
    
    // Dopamine tracks prediction error - strengthens successful traces
    pImpl->predictionError = totalActivity - targetMemoryActivity_;
    pImpl->dopamineLevel += pImpl->predictionError * 0.001f;
    if (pImpl->dopamineLevel < 0.1f) pImpl->dopamineLevel = 0.1f;
    if (pImpl->dopamineLevel > 2.0f) pImpl->dopamineLevel = 2.0f;
    
    // Curiosity increases for novel or unexpected patterns
    pImpl->curiosityLevel += (fabsf(pImpl->predictionError) * 0.0005f) * neuromodulatorBaseStrength_;
    if (pImpl->curiosityLevel > 1.0f) pImpl->curiosityLevel = 1.0f;
    
    // Novelty decays slowly
    pImpl->noveltyLevel *= 0.995f;
    
    // Developmental modulation
    developmentalPlasticity_ = 1.0f - 0.5f * (1.0f - pImpl->developmentalStage);
}

// Integration with prediction system
void NeuralWorkingMemory::integratePrediction(const std::vector<float>& prediction) {
    // Use prediction to update working memory
    if (prediction.empty() || !brain_) return;
    
    // Store prediction as working memory trace
    // Prediction represents expected sensory input
    float predictionStrength = 1.0f + fabsf(pImpl->predictionError);
    
    for (size_t i = 0; i < std::min(prediction.size(), memoryNeurons_.size()); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float predActivation = prediction[i] * predictionStrength;
        
        // Update with prediction error
        predActivation *= (1.0f + pImpl->predictionError * 0.5f);
        
        storeToNeuron(neuron, predActivation);
        
        // Apply prediction-based plasticity
        pImpl->dopamineLevel = std::min(2.0f, pImpl->dopamineLevel + fabsf(pImpl->predictionError) * 0.01f);
    }
    
    // Novelty signal for prediction mismatch
    if (fabsf(pImpl->predictionError) > 0.5f) {
        pImpl->noveltyLevel = std::min(1.0f, pImpl->noveltyLevel + 0.2f);
    }
}

// Integration with attention system
void NeuralWorkingMemory::integrateAttention(const std::vector<NeuronId>& attendedNeurons) {
    // Apply attentional focus to working memory
    for (const NeuronId& attendedNeuron : attendedNeurons) {
        bool isInMemory = contains(attendedNeuron);
        
        if (isInMemory) {
            // Amplify attended memory trace
            size_t idx = 0;
            for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
                if (memoryNeurons_[i] == attendedNeuron) {
                    idx = i;
                    memoryActivations_[idx] *= 1.5f; // Attention boost
                    break;
                }
            }
        }
    }
    
    // Curiosity modulation based on attention
    pImpl->curiosityLevel += (attendedNeurons.size() > 0) ? 0.01f : 0.0f;
}

// Create maintenance trace without requiring constant external firing
void NeuralWorkingMemory::createMaintenanceTrace(NeuronId neuron, float activation) {
    // Create a persistent trace through recurrent connections
    // This simulates the biological mechanism of persistent activity
    // without requiring constant external stimulation
    
    // Find or create maintenance connections
    NeuronId anchorNeuron = neuron; // Self-maintenance loop
    createRecurrentConnection(anchorNeuron, anchorNeuron, 0.8f * activation);
    
    // Store in active traces if not already present
    auto it = std::find(activeTraces_.begin(), activeTraces_.end(), neuron);
    if (it == activeTraces_.end()) {
        activeTraces_.push_back(neuron);
    }
}

bool NeuralWorkingMemory::checkNovelty(const std::vector<float>& pattern) const {
    if (memoryNeurons_.empty()) return true;
    
    // Check if pattern is novel by comparing to stored activations
    for (size_t i = 0; i < std::min(pattern.size(), memoryActivations_.size()); ++i) {
        if (fabsf(pattern[i] - memoryActivations_[i]) < 0.01f) {
            return false;
        }
    }
    
    return true;
}

void NeuralWorkingMemory::applyNeuromodulatoryPlasticity(const std::vector<float>& pattern) {
    // Apply spike-timing dependent plasticity-like changes
    // Based on dopamine and curiosity levels
    
    for (size_t i = 0; i < std::min(pattern.size(), memoryNeurons_.size()); ++i) {
        if (pImpl->dopamineLevel > 0.5f) {
            // Dopamine enables LTP/LTD based on timing
            // Stronger dopamine = more plastic synapses
            float weightChange = pattern[i] * pImpl->dopamineLevel * 0.01f;
            
            if (i < memoryActivations_.size()) {
                // Synaptic modification based on pattern
                memoryActivations_[i] += weightChange * developmentalPlasticity_;
            }
        }
        
        if (pImpl->curiosityLevel > 0.3f) {
            // Curiosity promotes exploration and new associations
            // Increase trace excitability
            if (i < memoryActivations_.size()) {
                memoryActivations_[i] *= (1.0f + pImpl->curiosityLevel * 0.05f);
            }
        }
    }
}

void NeuralWorkingMemory::updateRecurrentConnectionsWithNeuromodulation() {
    // Enhanced recurrent connection updates with neuromodulatory effects
    
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Enhanced maintenance signal based on neuromodulators
            float neuromodFactor = 1.0f;
            neuromodFactor += pImpl->dopamineLevel * 0.3f;
            neuromodFactor += pImpl->curiosityLevel * 0.2f;
            
            brain_->injectCurrent(conn.second, fromActivation * 2.0f * neuromodFactor);
        }
    }
}

void NeuralWorkingMemory::runCompetitionWithNeuromodulation() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f * (1.0f + pImpl->dopamineLevel * 0.5f);
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Enhanced inhibitory competition with neuromodulatory effects
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply inhibition modulated by neuromodulators
            float inhibitionStrength = memoryActivations_[i] * 3.0f;
            
            // Curiosity reduces inhibition to promote exploration
            inhibitionStrength *= (1.0f - pImpl->curiosityLevel * 0.5f);
            
            // Novelty enhances competition
            if (pImpl->noveltyLevel > 0.5f) {
                inhibitionStrength *= 1.5f;
            }
            
            brain_->injectCurrent(memoryNeurons_[i], -inhibitionStrength);
        }
    }
}