#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Safe access macros for pImpl members
#define pImpl_safe pImpl_safe_impl

// Helper function for safe pImpl access
Impl* safe_pimpl_access(Impl* ptr) {
    if (!ptr) {
        NLM_LOG_ERROR("Null pImpl access in NeuralWorkingMemory.cpp");
        return nullptr;
    }
    return ptr;
}

// Helper function for safe brain access
Brain* safe_brain_access(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Null Brain pointer in NeuralWorkingMemory.cpp");
        return nullptr;
    }
    return brain;
}

// Helper function for parameter validation
bool validate_pattern_parameters(const std::vector<float>& pattern, float strength) {
    if (pattern.empty()) {
        NLM_LOG_ERROR("NeuralWorkingMemory: pattern cannot be empty");
        return false;
    }
    
    if (!validate_positive(strength, "strength", "NeuralWorkingMemory::store")) {
        NLM_LOG_ERROR("NeuralWorkingMemory: invalid strength value");
        return false;
    }
    
    return true;
}

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
    auto* safe_brain = safe_brain_access(brain);
    if (!safe_brain) {
        NLM_LOG_ERROR("NeuralWorkingMemory::initialize failed: null brain pointer");
        return;
    }
    
    // Biological inspiration: Working memory maintains information through persistent
    // neural activity, similar to prefrontal cortical working memory mechanisms.
    // Persistent activity is achieved through recurrent connections that maintain
    // activation states even after input ceases. This implementation implements
    // activity-dependent memory traces that stabilize through reinforcement.
    
    pImpl->brain = safe_brain;
    brain_ = safe_brain;
    
    // Initialize working memory with biologically plausible parameters
    // Capacity reflects the limited number of items humans can hold in working memory
    // (Miller's Law: 7±2 items). Decay rate models forgetting due to interference.
    capacity_ = std::max(10, brain_->getTotalNeuronCount() / 50); // Scale with brain size
    decayRate_ = 0.01f; // 1% decay per timestep - matches biological forgetting rates
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with capacity " + std::to_string(capacity_));
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    // Validate parameters
    if (!validate_pattern_parameters(pattern, strength)) {
        return;
    }
    
    // Validate brain access
    auto* safe_brain = safe_brain_access(brain_);
    if (!safe_brain) {
        NLM_LOG_ERROR("NeuralWorkingMemory::store failed: null brain pointer");
        return;
    }
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        auto* safe_brain = safe_brain_access(brain_);
    if (!safe_brain) {
        NLM_LOG_ERROR("NeuralWorkingMemory::store failed: null brain pointer for neuron access");
        continue;
    }
    
    // Biological inspiration: Working memory storage uses neuronal activation patterns
    // to encode information. The activation is proportional to input strength, mimicking
    // how neurons in prefrontal cortex maintain information through persistent activity.
    // This implementation uses current injection to maintain activation levels.
        
        try {
            // Set neuron activation with error handling
            if (auto* region = brain->getRegion(neuron.getId() / 1000)) {
                auto neurons = region->getAllNeurons();
                for (auto* nn : *neurons) {
                    if (nn->getId() == neuron) {
                        nn->injectCurrent(activation * 5.0f);
                        break;
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("NeuralWorkingMemory::store failed: exception injecting current to neuron - " + std::string(e.what()));
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
        try {
            createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("NeuralWorkingMemory::store failed: exception creating recurrent connection - " + std::string(e.what()));
        }
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Validate parameters
    if (!validate_positive(activation, "activation", "NeuralWorkingMemory::storeToNeuron")) {
        NLM_LOG_ERROR("NeuralWorkingMemory::storeToNeuron failed: invalid activation value");
        return;
    }
    
    // Validate brain access
    auto* safe_brain = safe_brain_access(brain_);
    if (!safe_brain) {
        NLM_LOG_ERROR("NeuralWorkingMemory::storeToNeuron failed: null brain pointer");
        return;
    }
    
    try {
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
        safe_brain->injectCurrent(neuron, activation * 5.0f);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::storeToNeuron failed: exception - " + std::string(e.what()));
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
    try {
        return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::contains failed: exception - " + std::string(e.what()));
        return false;
    }
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    try {
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            return memoryActivations_[idx];
        }
        return 0.0f;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::getNeuronActivation failed: exception - " + std::string(e.what()));
        return 0.0f;
    }
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    // Validate brain access
    auto* safe_brain = safe_brain_access(brain_);
    if (!safe_brain) {
        NLM_LOG_ERROR("NeuralWorkingMemory::update failed: null brain pointer");
        return;
    }
    
    // Validate timestep parameter
    if (!validate_positive(dt, "timestep", "NeuralWorkingMemory::update")) {
        NLM_LOG_ERROR("NeuralWorkingMemory::update failed: invalid timestep value");
        return;
    }
    
    try {
        // Update maintenance - reinforce active memory neurons
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            NeuronId neuron = memoryNeurons_[i];
            float activation = memoryActivations_[i];
            
            if (activation > 0.1f) {
                // Inject maintenance current
                safe_brain->injectCurrent(neuron, activation * 2.0f);
                
                // Age the trace
                memoryTimestamps_[i]++;
                
                // Check if trace is too old
                if (memoryTimestamps_[i] > 1000) {
                    activation *= (1.0f - decayRate_);
                }
                
                memoryActivations_[i] = activation;
            }
        }
        
        // Decay weak traces
        decayWeakTraces();
        
        // Run competition to select winners
        runCompetition();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::update failed: exception - " + std::string(e.what()));
    }
}

void NeuralWorkingMemory::clear() {
    try {
        memoryNeurons_.clear();
        memoryActivations_.clear();
        memoryTimestamps_.clear();
        activeTraces_.clear();
        pImpl->maintenanceSynapses.clear();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::clear failed: exception - " + std::string(e.what()));
    }
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    // Validate parameter
    if (!validate_positive(factor, "factor", "NeuralWorkingMemory::strengthenMemory")) {
        NLM_LOG_ERROR("NeuralWorkingMemory::strengthenMemory failed: invalid factor value");
        return;
    }
    
    try {
        for (auto& activation : memoryActivations_) {
            activation = std::min(1.0f, activation * factor);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::strengthenMemory failed: exception - " + std::string(e.what()));
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
        
    try {
        // Biological inspiration: Working memory retrieval involves competitive dynamics
        // among stored items. The competition mechanism implements the neural equivalent
        // of selective attention and choice, where the most activated item wins through
        // mutual inhibition. This is modeled by injecting inhibitory currents into
        // non-winning neurons, suppressing their activity.
        brain_->injectCurrent(neuron, -memoryActivations_[i] * 3.0f);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::runCompetition failed: exception applying inhibition - " + std::string(e.what()));
    }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    try {
        return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::isWinning failed: exception - " + std::string(e.what()));
        return false;
    }
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
    // Validate parameter
    if (!validate_positive(strength, "strength", "NeuralWorkingMemory::createRecurrentConnection")) {
        NLM_LOG_ERROR("NeuralWorkingMemory::createRecurrentConnection failed: invalid strength value");
        return;
    }
    
    try {
        // Check if connection already exists
        for (const auto& conn : pImpl->maintenanceSynapses) {
            if (conn.first == from && conn.second == to) return;
        }
        
        pImpl->maintenanceSynapses.emplace_back(from, to);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::createRecurrentConnection failed: exception - " + std::string(e.what()));
    }
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    auto* safe_brain = safe_brain_access(brain_);
    if (!safe_brain) {
        NLM_LOG_ERROR("NeuralWorkingMemory::updateRecurrentConnections failed: null brain pointer");
        return;
    }
    
    try {
        // Apply maintenance currents through recurrent connections
        for (const auto& conn : pImpl->maintenanceSynapses) {
            float fromActivation = 0.0f;
            
            // Find from neuron activation
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                fromActivation = memoryActivations_[idx];
            }
            
            if (fromActivation > 0.1f && safe_brain) {
                // Send maintenance signal
                safe_brain->injectCurrent(conn.second, fromActivation * 2.0f);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::updateRecurrentConnections failed: exception - " + std::string(e.what()));
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuralWorkingMemory::decayWeakTraces failed: exception - " + std::string(e.what()));
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
    auto* safe_brain = safe_brain_access(brain);
    if (!safe_brain) {
        NLM_LOG_ERROR("AttentionalSelection::initialize failed: null brain pointer");
        return;
    }
    
    pImpl = std::make_unique<Impl>();
    brain_ = safe_brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Validate parameters
    if (!validate_positive(globalInhibition, "globalInhibition", "AttentionalSelection::processCompetition")) {
        NLM_LOG_ERROR("AttentionalSelection::processCompetition failed: invalid globalInhibition value");
        return winners_;
    }
    
    try {
        // Compute activity levels for competitors
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
        
        // Competition: neurons inhibit each other based on relative activity
        for (size_t i = 0; i < competitors.size(); ++i) {
            for (size_t j = 0; j < competitors.size(); ++j) {
                if (i == j) continue;
                
                float relativeActivity = activities[i] / (activities[j] + 0.001f);
                
                if (relativeActivity > 1.5f) {
                    // i is much stronger than j - apply inhibition to j
                    if (brain_) {
                        brain_->injectCurrent(competitors[j], -globalInhibition * inhibitionStrength_);
                    }
                    pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
                }
            }
        }
        
        // Winners are neurons with above-threshold activity
        float threshold = competitionThreshold_ * totalActivity / competitors.size();
        
        for (size_t i = 0; i < competitors.size(); ++i) {
            if (activities[i] >= threshold) {
                winners_.push_back(competitors[i]);
                
                // Apply excitation to winners
                if (brain_) {
                    brain_->injectCurrent(competitors[i], excitationStrength_ * 2.0f);
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::processCompetition failed: exception - " + std::string(e.what()));
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    try {
        if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
            attendedRegions_.push_back(region);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::focusOnRegion failed: exception - " + std::string(e.what()));
    }
}

void AttentionalSelection::releaseAttention() {
    try {
        attendedRegions_.clear();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::releaseAttention failed: exception - " + std::string(e.what()));
    }
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    try {
        return attendedRegions_;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::getAttendedRegions failed: exception - " + std::string(e.what()));
        return std::vector<RegionId>();
    }
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
    try {
        auto it = std::find(winners_.begin(), winners_.end(), neuron);
        if (it != winners_.end()) {
            return 0.0f;  // Winners don't receive inhibition
        }
        return inhibitionStrength_;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::getInhibitionFor failed: exception - " + std::string(e.what()));
        return inhibitionStrength_;
    }
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    try {
        auto it = std::find(winners_.begin(), winners_.end(), neuron);
        if (it != winners_.end()) {
            return excitationStrength_;
        }
        return 0.0f;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::getExcitationFor failed: exception - " + std::string(e.what()));
        return 0.0f;
    }
}

void AttentionalSelection::update(TimestepDuration dt) {
    try {
        // Decay salience and bias over time
        for (auto& s : bottomUpSalience_) {
            s *= 0.95f;
        }
        
        for (auto& b : topDownBias_) {
            b *= 0.98f;
        }
        
        // Decay inhibition
        for (auto& i : pImpl->inhibitionLevel) {
            i *= 0.9f;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::update failed: exception - " + std::string(e.what()));
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    try {
        return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::isAttended failed: exception - " + std::string(e.what()));
        return false;
    }
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    try {
        // Store bias in map keyed by NeuronId
        auto it = topDownBias_.find(neuron.value);
        if (it != topDownBias_.end()) {
            it->second += biasStrength;
        } else {
            topDownBias_[neuron.value] = biasStrength;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::applyTopDownBias failed: exception - " + std::string(e.what()));
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    try {
        // Store salience in map keyed by NeuronId
        auto it = bottomUpSalience_.find(neuron.value);
        if (it != bottomUpSalience_.end()) {
            it->second += salienceStrength;
        } else {
            bottomUpSalience_[neuron.value] = salienceStrength;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::applyBottomUpSalience failed: exception - " + std::string(e.what()));
    }
}

void AttentionalSelection::reset() {
    try {
        winners_.clear();
        attendedRegions_.clear();
        neuronSalience_.clear();
        topDownBias_.clear();
        bottomUpSalience_.clear();
        pImpl->inhibitionLevel.clear();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AttentionalSelection::reset failed: exception - " + std::string(e.what()));
    }
}

} // namespace nlm
