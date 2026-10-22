#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    Impl() : brain(nullptr) {}
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
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
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
    
    // Update maintenance - reinforce active memory neurons with neuromodulation
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current (base level)
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old and apply decay
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            // Apply neuromodulation effects
            if (brain_->getAcetylcholine()) {
                float achLevel = brain_->getAcetylcholine()->getLevel();
                // ACh enhances memory maintenance and attention
                brain_->injectCurrent(neuron, activation * achLevel * 0.5f);
            }
            
            if (brain_->getNorepinephrine()) {
                float neLevel = brain_->getNorepinephrine()->getLevel();
                // NE increases arousal and vigilance - enhances memory salience
                brain_->injectCurrent(neuron, activation * neLevel * 0.3f);
            }
            
            if (brain_->getDopamine()) {
                float daLevel = brain_->getDopamine()->getLevel();
                // DA modulates plasticity - affects memory strength
                if (daLevel > 0.0f) {
                    activation *= (1.0f + daLevel * 0.2f);
                }
            }
            
            // Apply attention effects
            auto* attention = brain_->getAttention();
            if (attention && attention->isAttended(neuron)) {
                // Attended neurons get stronger maintenance
                brain_->injectCurrent(neuron, activation * 2.0f);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Update recurrent connections for maintenance
    updateRecurrentConnections();
    
    // Decay weak traces with neuromodulation
    decayWeakTraces();
    
    // Run competition with neuromodulation
    runCompetition();
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
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
            
            // Create stronger recurrent connections for winners (rehearsal)
            for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
                if (i != j) {
                    createRecurrentConnection(memoryNeurons_[i], memoryNeurons_[j], memoryActivations_[i] * 0.3f);
                }
            }
        }
    }
    
    // Inhibit non-winners more strongly when attention is high
    float attentionLevel = 1.0f;
    if (brain_) {
        auto* attention = brain_->getAttention();
        if (attention) {
            attentionLevel = attention->getInhibitionStrength();
        }
    }
    
    // Inhibitory competition - suppress non-winners with attention modulation
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply stronger inhibition based on memory activation and attention
            float inhibitionStrength = memoryActivations_[i] * 3.0f * attentionLevel;
            brain_->injectCurrent(memoryNeurons_[i], -inhibitionStrength);
            
            // Also apply neuromodulation effects
            if (brain_->getAcetylcholine()) {
                float achLevel = brain_->getAcetylcholine()->getLevel();
                // ACh enhances attention - increase inhibition for better competition
                brain_->injectCurrent(memoryNeurons_[i], -inhibitionStrength * achLevel * 0.5f);
            }
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
    
    // Apply neuromodulation effects to the new connection
    if (brain_) {
        // Acetylcholine enhances synaptic plasticity
        if (brain_->getAcetylcholine()) {
            float achLevel = brain_->getAcetylcholine()->getLevel();
            if (achLevel > 0.5f) {
                // Strong ACh promotes learning and memory
                strength *= (1.0f + achLevel * 0.5f);
            }
        }
        
        // Dopamine modulates connection strength based on reward prediction error
        if (brain_->getDopamine()) {
            float daLevel = brain_->getDopamine()->getLevel();
            if (daLevel > 0.0f) {
                // Positive dopamine strengthens connections
                strength *= (1.0f + daLevel * 0.3f);
            } else if (daLevel < 0.0f) {
                // Negative dopamine weakens connections
                strength *= std::max(0.1f, 1.0f + daLevel * 0.5f);
            }
        }
        
        // Norepinephrine increases signal detection for connections
        if (brain_->getNorepinephrine()) {
            float neLevel = brain_->getNorepinephrine()->getLevel();
            strength *= (1.0f + neLevel * 0.2f);
        }
        
        // Serotonin stabilizes connections
        if (brain_->getSerotonin()) {
            float serotoninLevel = brain_->getSerotonin()->getLevel();
            // Moderate serotonin stabilizes, high serotonin reduces excessive strengthening
            if (serotoninLevel > 0.7f) {
                strength *= 0.8f;  // Reduce strengthening at high serotonin
            }
        }
    }
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections with neuromodulation
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Base maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
            
            // Apply neuromodulation effects
            if (brain_->getAcetylcholine()) {
                float achLevel = brain_->getAcetylcholine()->getLevel();
                // ACh enhances synaptic efficacy
                brain_->injectCurrent(conn.second, fromActivation * achLevel * 0.5f);
            }
            
            if (brain_->getNorepinephrine()) {
                float neLevel = brain_->getNorepinephrine()->getLevel();
                // NE increases signal detection and arousal
                brain_->injectCurrent(conn.second, fromActivation * neLevel * 0.3f);
            }
            
            if (brain_->getDopamine()) {
                float daLevel = brain_->getDopamine()->getLevel();
                // DA modulates plasticity based on prediction error
                if (daLevel > 0.0f) {
                    // Positive DA increases connection efficacy
                    brain_->injectCurrent(conn.second, fromActivation * daLevel * 0.2f);
                }
            }
            
            if (brain_->getSerotonin()) {
                float serotoninLevel = brain_->getSerotonin()->getLevel();
                // Serotonin modulates behavioral flexibility
                if (serotoninLevel < 0.5f) {
                    // Low serotonin increases plasticity
                    brain_->injectCurrent(conn.second, fromActivation * serotoninLevel * 0.3f);
                } else {
                    // High serotonin stabilizes connections
                    brain_->injectCurrent(conn.second, fromActivation * 0.5f * (1.0f - serotoninLevel));
                }
            }
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
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
    }
}

// AttentionalSelection Implementation
struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    
    Impl() {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Compute activity levels for competitors with neuromodulation
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        activities[i] = salience + bias;
        totalActivity += activities[i];
    }
    
    if (totalActivity < 0.001f) {
        // No strong competitors - all equal
        return competitors;
    }
    
    // Apply neuromodulation effects to competition
    if (brain_) {
        // Acetylcholine enhances competition focus and attention
        if (brain_->getAcetylcholine()) {
            float achLevel = brain_->getAcetylcholine()->getLevel();
            if (achLevel > 0.5f) {
                // Strong ACh increases competition sharpness
                for (size_t i = 0; i < competitors.size(); ++i) {
                    if (activities[i] > 0.1f) {
                        activities[i] *= (1.0f + achLevel * 0.3f);
                    }
                }
            }
        }
        
        // Norepinephrine increases arousal and vigilance - enhances competition sensitivity
        if (brain_->getNorepinephrine()) {
            float neLevel = brain_->getNorepinephrine()->getLevel();
            if (neLevel > 0.5f) {
                // Strong NE increases competition sensitivity
                for (size_t i = 0; i < competitors.size(); ++i) {
                    activities[i] *= (1.0f + neLevel * 0.2f);
                }
            }
        }
        
        // Dopamine modulates competition based on prediction error
        if (brain_->getDopamine()) {
            float daLevel = brain_->getDopamine()->getLevel();
            if (daLevel > 0.0f) {
                // Positive DA enhances winning bias
                float winningBias = 1.0f + daLevel * 0.2f;
                for (size_t i = 0; i < competitors.size(); ++i) {
                    if (activities[i] > 0.1f) {
                        activities[i] *= winningBias;
                    }
                }
            }
        }
        
        // Serotonin modulates competition stability
        if (brain_->getSerotonin()) {
            float serotoninLevel = brain_->getSerotonin()->getLevel();
            if (serotoninLevel > 0.7f) {
                // High serotonin reduces competition volatility
                for (size_t i = 0; i < competitors.size(); ++i) {
                    activities[i] *= (1.0f - serotoninLevel * 0.3f);
                }
            }
        }
    }
    
    // Recompute total activity after neuromodulation
    totalActivity = 0.0f;
    for (size_t i = 0; i < competitors.size(); ++i) {
        totalActivity += activities[i];
    }
    
    // Competition: neurons inhibit each other based on relative activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relativeActivity = activities[i] / (activities[j] + 0.001f);
            
            if (relativeActivity > 1.5f) {
                // i is much stronger than j - apply inhibition to j
                if (brain_) {
                    // Apply neuromodulation to inhibition
                    float inhibition = globalInhibition * inhibitionStrength_;
                    
                    if (brain_->getAcetylcholine()) {
                        float achLevel = brain_->getAcetylcholine()->getLevel();
                        inhibition *= (1.0f + achLevel * 0.5f);
                    }
                    
                    if (brain_->getNorepinephrine()) {
                        float neLevel = brain_->getNorepinephrine()->getLevel();
                        inhibition *= (1.0f + neLevel * 0.3f);
                    }
                    
                    brain_->injectCurrent(competitors[j], -inhibition);
                }
                pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
            }
        }
    }
    
    // Winners are neurons with above-threshold activity (adjusted for neuromodulation)
    float threshold = competitionThreshold_ * totalActivity / competitors.size();
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners with neuromodulation
            if (brain_) {
                float excitation = excitationStrength_ * 2.0f;
                
                if (brain_->getAcetylcholine()) {
                    float achLevel = brain_->getAcetylcholine()->getLevel();
                    excitation *= (1.0f + achLevel * 0.5f);
                }
                
                if (brain_->getNorepinephrine()) {
                    float neLevel = brain_->getNorepinephrine()->getLevel();
                    excitation *= (1.0f + neLevel * 0.2f);
                }
                
                if (brain_->getDopamine()) {
                    float daLevel = brain_->getDopamine()->getLevel();
                    excitation *= (1.0f + daLevel * 0.3f);
                }
                
                brain_->injectCurrent(competitors[i], excitation);
            }
        }
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = strength;
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = strength;
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = threshold;
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return excitationStrength_;
    }
    return 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Decay salience and bias over time with neuromodulation effects
    for (auto& s : bottomUpSalience_) {
        s *= 0.95f;
        
        // Neuromodulation effects on salience decay
        if (brain_->getAcetylcholine()) {
            float achLevel = brain_->getAcetylcholine()->getLevel();
            if (achLevel > 0.5f) {
                // ACh enhances salience maintenance
                s *= (1.0f - achLevel * 0.1f);
            }
        }
        
        if (brain_->getNorepinephrine()) {
            float neLevel = brain_->getNorepinephrine()->getLevel();
            if (neLevel > 0.5f) {
                // NE enhances arousal - salience maintenance
                s *= (1.0f - neLevel * 0.1f);
            }
        }
    }
    
    for (auto& b : topDownBias_) {
        b *= 0.98f;
        
        // Neuromodulation effects on bias decay
        if (brain_->getDopamine()) {
            float daLevel = brain_->getDopamine()->getLevel();
            if (daLevel > 0.0f) {
                // Positive DA maintains bias longer
                b *= (1.0f + daLevel * 0.02f);
            }
        }
        
        if (brain_->getSerotonin()) {
            float serotoninLevel = brain_->getSerotonin()->getLevel();
            if (serotoninLevel > 0.7f) {
                // High serotonin stabilizes bias
                b *= 0.99f;
            }
        }
    }
    
    // Decay inhibition with neuromodulation
    for (auto& i : pImpl->inhibitionLevel) {
        i *= 0.9f;
        
        // Neuromodulation effects on inhibition decay
        if (brain_->getAcetylcholine()) {
            float achLevel = brain_->getAcetylcholine()->getLevel();
            i *= (1.0f - achLevel * 0.05f);
        }
        
        if (brain_->getNorepinephrine()) {
            float neLevel = brain_->getNorepinephrine()->getLevel();
            i *= (1.0f - neLevel * 0.03f);
        }
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    // Store bias in map keyed by NeuronId
    auto it = topDownBias_.find(neuron.value);
    if (it != topDownBias_.end()) {
        it->second += biasStrength;
    } else {
        topDownBias_[neuron.value] = biasStrength;
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    // Store salience in map keyed by NeuronId
    auto it = bottomUpSalience_.find(neuron.value);
    if (it != bottomUpSalience_.end()) {
        it->second += salienceStrength;
    } else {
        bottomUpSalience_[neuron.value] = salienceStrength;
    }
}

void AttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
}

} // namespace nlm