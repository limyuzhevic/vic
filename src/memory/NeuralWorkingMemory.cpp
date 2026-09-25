#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHierarchy.hpp"
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
    try {
        // Initialize with error handling
        reset();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize NeuralWorkingMemory: " + std::string(e.what()));
        // Continue with default initialization
    }
}

NeuralWorkingMemory::~NeuralWorkingMemory() {
    delete pImpl;
}

void NeuralWorkingMemory::initialize(Brain* brain) {
    try {
        if (!brain) {
            throw NLMMemoryError("Brain pointer is null",
                                NLMMemoryError::ErrorType::InvalidReference,
                                "NeuralWorkingMemory::initialize");
        }
        
        pImpl->brain = brain;
        brain_ = brain;
        NLM_LOG_INFO("NeuralWorkingMemory initialized");
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in initialize: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error initializing working memory: " + std::string(e.what()));
        throw NLMMemoryError("Failed to initialize working memory",
                            NLMMemoryError::ErrorType::InvalidReference,
                            "NeuralWorkingMemory::initialize");
    }
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    try {
        // Validate input parameters
        if (pattern.empty()) {
            NLM_LOG_ERROR("Empty pattern provided to store");
            return;
        }
        
        if (!brain_) {
            NLM_LOG_ERROR("Working memory not initialized - brain pointer is null");
            return;
        }
        
        if (strength == 0.0f) {
            NLM_LOG_INFO("Zero strength provided to store - ignoring");
            return;
        }
        
        if (std::isnan(strength) || std::isinf(strength)) {
            throw NLMMemoryError("Invalid strength value: " + std::to_string(strength),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "NeuralWorkingMemory::store");
        }
        
        // Find neurons to encode this pattern
        size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
        
        for (size_t i = 0; i < neuronsNeeded; ++i) {
            NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
            float activation = pattern[i] * strength;
            
            // Validate activation
            if (std::isnan(activation) || std::isinf(activation)) {
                throw NLMMemoryError("Invalid activation value: " + std::to_string(activation),
                                    NLMMemoryError::ErrorType::ConsistencyViolation,
                                    "NeuralWorkingMemory::store");
            }
            
            // Set neuron activation
            try {
                if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
                    for (auto* nn : *n) {
                        if (nn->getId() == neuron) {
                            nn->injectCurrent(activation * 5.0f);
                            break;
                        }
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error setting neuron activation for neuron " + std::to_string(neuron.getId()) + ": " + std::string(e.what()));
                // Continue with next neuron
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
                NLM_LOG_ERROR("Error creating recurrent connection: " + std::string(e.what()));
                // Continue with other connections
            }
        }
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in store: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error storing to working memory: " + std::string(e.what()));
        // Continue with degraded functionality
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    try {
        // Validate input parameters
        if (std::isnan(activation) || std::isinf(activation)) {
            throw NLMMemoryError("Invalid activation value: " + std::to_string(activation),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "NeuralWorkingMemory::storeToNeuron");
        }
        
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
        } else {
            NLM_LOG_WARNING("Memory capacity reached - cannot store new neuron");
            // Continue with existing neurons
        }
        
        // Inject current to maintain activation
        if (brain_) {
            try {
                brain_->injectCurrent(neuron, activation * 5.0f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error injecting current for neuron " + std::to_string(neuron.getId()) + ": " + std::string(e.what()));
            }
        }
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in storeToNeuron: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error storing to neuron in working memory: " + std::string(e.what()));
        // Continue with degraded functionality
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    try {
        std::vector<float> result;
        result.reserve(memoryActivations_.size());
        
        for (float activation : memoryActivations_) {
            // Validate activation
            if (std::isnan(activation) || std::isinf(activation)) {
                NLM_LOG_ERROR("Invalid activation value found during retrieval");
                continue; // Skip invalid values
            }
            
            result.push_back(activation);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error retrieving from working memory: " + std::string(e.what()));
        return {};  // Return empty vector on error
    }
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    try {
        return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error checking if neuron is in working memory: " + std::string(e.what()));
        return false;
    }
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    try {
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            float activation = memoryActivations_[idx];
            
            // Validate activation
            if (std::isnan(activation) || std::isinf(activation)) {
                NLM_LOG_ERROR("Invalid activation value found for neuron " + std::to_string(neuron.getId()));
                return 0.0f;
            }
            
            return activation;
        }
        return 0.0f;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting neuron activation from working memory: " + std::string(e.what()));
        return 0.0f;
    }
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    try {
        // Validate timestep
        if (dt <= 0.0f) {
            NLM_LOG_WARNING("Invalid timestep for working memory update: " + std::to_string(dt));
            return;
        }
        
        if (!brain_) {
            NLM_LOG_ERROR("Working memory not initialized - brain pointer is null");
            return;
        }
        
        // Update maintenance - reinforce active memory neurons
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            NeuronId neuron = memoryNeurons_[i];
            float activation = memoryActivations_[i];
            
            try {
                if (activation > 0.1f) {
                    // Inject maintenance current
                    brain_->injectCurrent(neuron, activation * 2.0f);
                    
                    // Age the trace
                    memoryTimestamps_[i]++;
                    
                    // Check if trace is too old
                    if (memoryTimestamps_[i] > 1000) {
                        activation *= (1.0f - decayRate_);
                    }
                    
                    // Validate updated activation
                    if (std::isnan(activation) || std::isinf(activation)) {
                        throw NLMMemoryError("Invalid activation after aging",
                                            NLMMemoryError::ErrorType::ConsistencyViolation,
                                            "NeuralWorkingMemory::update");
                    }
                    
                    memoryActivations_[i] = activation;
                }
            } catch (const NLMMemoryError& e) {
                NLM_LOG_ERROR("Memory error updating neuron " + std::to_string(neuron.getId()) + ": " + std::string(e.what()));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error updating neuron " + std::to_string(neuron.getId()) + ": " + std::string(e.what()));
                // Continue with next neuron
            }
        }
        
        // Decay weak traces
        try {
            decayWeakTraces();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error during decay weak traces: " + std::string(e.what()));
        }
        
        // Run competition to select winners
        try {
            runCompetition();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error during competition: " + std::string(e.what()));
        }
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in working memory update: " + std::string(e.what()));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error updating working memory: " + std::string(e.what()));
        // Continue with degraded functionality
    }
}

void NeuralWorkingMemory::clear() {
    try {
        memoryNeurons_.clear();
        memoryActivations_.clear();
        memoryTimestamps_.clear();
        activeTraces_.clear();
        pImpl->maintenanceSynapses.clear();
        NLM_LOG_INFO("Working memory cleared");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error clearing working memory: " + std::string(e.what()));
    }
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    try {
        // Validate factor
        if (std::isnan(factor) || std::isinf(factor)) {
            throw NLMMemoryError("Invalid strengthen factor: " + std::to_string(factor),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "NeuralWorkingMemory::strengthenMemory");
        }
        
        if (factor <= 0.0f) {
            NLM_LOG_ERROR("Factor must be positive for strengthening");
            return;
        }
        
        for (size_t i = 0; i < memoryActivations_.size(); ++i) {
            float activation = memoryActivations_[i];
            
            // Strengthen activation (but don't exceed 1.0)
            float newActivation = std::min(1.0f, activation * factor);
            
            // Validate new activation
            if (std::isnan(newActivation) || std::isinf(newActivation)) {
                throw NLMMemoryError("Invalid new activation value",
                                    NLMMemoryError::ErrorType::ConsistencyViolation,
                                    "NeuralWorkingMemory::strengthenMemory");
            }
            
            memoryActivations_[i] = newActivation;
        }
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in strengthenMemory: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error strengthening memory: " + std::string(e.what()));
        // Continue with degraded functionality
    }
}

void NeuralWorkingMemory::runCompetition() {
    try {
        winners_.clear();
        
        if (memoryActivations_.empty()) return;
        
        // Find neurons with above-threshold activation
        float threshold = 0.3f;
        
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            float activation = memoryActivations_[i];
            
            // Validate activation
            if (std::isnan(activation) || std::isinf(activation)) {
                NLM_LOG_ERROR("Invalid activation value during competition");
                continue;  // Skip invalid values
            }
            
            if (activation >= threshold) {
                winners_.push_back(memoryNeurons_[i]);
            }
        }
        
        // Inhibitory competition - suppress non-winners
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
            
            if (!isWinner && brain_) {
                try {
                    // Apply strong inhibition
                    brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error applying inhibition for neuron " + std::to_string(memoryNeurons_[i].getId()) + ": " + std::string(e.what()));
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during memory competition: " + std::string(e.what()));
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    try {
        return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error checking if neuron is winner: " + std::string(e.what()));
        return false;
    }
}

float NeuralWorkingMemory::getMemoryActivity() const {
    try {
        if (memoryActivations_.empty()) return 0.0f;
        
        float total = 0.0f;
        for (size_t i = 0; i < memoryActivations_.size(); ++i) {
            float activation = memoryActivations_[i];
            
            // Validate activation
            if (std::isnan(activation) || std::isinf(activation)) {
                NLM_LOG_ERROR("Invalid activation value found during activity calculation");
                continue;  // Skip invalid values
            }
            
            total += activation;
        }
        
        return total / memoryActivations_.size();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting memory activity: " + std::string(e.what()));
        return 0.0f;
    }
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    try {
        // Validate inputs
        if (strength == 0.0f) {
            NLM_LOG_ERROR("Zero strength for recurrent connection");
            return;
        }
        
        if (std::isnan(strength) || std::isinf(strength)) {
            throw NLMMemoryError("Invalid connection strength: " + std::to_string(strength),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "NeuralWorkingMemory::createRecurrentConnection");
        }
        
        // Check if connection already exists
        for (const auto& conn : pImpl->maintenanceSynapses) {
            if (conn.first == from && conn.second == to) {
                NLM_LOG_DEBUG("Recurrent connection already exists between neurons");
                return;
            }
        }
        
        pImpl->maintenanceSynapses.emplace_back(from, to);
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error creating recurrent connection: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error creating recurrent connection: " + std::string(e.what()));
        // Continue with other connections
    }
}

void NeuralWorkingMemory::updateRecurrentConnections() {
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
            
            // Validate activation
            if (std::isnan(fromActivation) || std::isinf(fromActivation)) {
                NLM_LOG_ERROR("Invalid activation for recurrent connection source");
                continue;  // Skip invalid connections
            }
            
            if (fromActivation > 0.1f && brain_) {
                try {
                    // Send maintenance signal
                    brain_->injectCurrent(conn.second, fromActivation * 2.0f);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error sending recurrent signal: " + std::string(e.what()));
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error updating recurrent connections: " + std::string(e.what()));
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    try {
        std::vector<size_t> toRemove;
        
        for (size_t i = 0; i < memoryActivations_.size(); ++i) {
            float activation = memoryActivations_[i];
            
            // Apply decay
            float newActivation = activation * (1.0f - decayRate_);
            
            // Validate new activation
            if (std::isnan(newActivation) || std::isinf(newActivation)) {
                throw NLMMemoryError("Invalid activation after decay",
                                    NLMMemoryError::ErrorType::ConsistencyViolation,
                                    "NeuralWorkingMemory::decayWeakTraces");
            }
            
            memoryActivations_[i] = newActivation;
            
            if (newActivation < 0.01f) {
                toRemove.push_back(i);
            }
        }
        
        // Remove weak traces (in reverse order to maintain indices)
        for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
            memoryNeurons_.erase(memoryNeurons_.begin() + *it);
            memoryActivations_.erase(memoryActivations_.begin() + *it);
            memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
        }
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error during decay weak traces: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during decay weak traces: " + std::string(e.what()));
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
    try {
        // Initialize with error handling
        reset();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize AttentionalSelection: " + std::string(e.what()));
        // Continue with default initialization
    }
}

AttentionalSelection::~AttentionalSelection() {
    delete pImpl;
}

void AttentionalSelection::initialize(Brain* brain) {
    try {
        if (!brain) {
            throw NLMMemoryError("Brain pointer is null",
                                NLMMemoryError::ErrorType::InvalidReference,
                                "AttentionalSelection::initialize");
        }
        
        pImpl = std::make_unique<Impl>();
        brain_ = brain;
        NLM_LOG_INFO("AttentionalSelection initialized");
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in initialize: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error initializing attentional selection: " + std::string(e.what()));
        throw NLMMemoryError("Failed to initialize attentional selection",
                            NLMMemoryError::ErrorType::InvalidReference,
                            "AttentionalSelection::initialize");
    }
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    try {
        winners_.clear();
        
        if (competitors.empty()) return winners_;
        
        // Validate global inhibition
        if (std::isnan(globalInhibition) || std::isinf(globalInhibition) || globalInhibition < 0.0f) {
            throw NLMMemoryError("Invalid global inhibition value: " + std::to_string(globalInhibition),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "AttentionalSelection::processCompetition");
        }
        
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
            
            // Validate salience and bias
            if (std::isnan(salience) || std::isinf(salience) || std::isnan(bias) || std::isinf(bias)) {
                throw NLMMemoryError("Invalid salience or bias value",
                                    NLMMemoryError::ErrorType::ConsistencyViolation,
                                    "AttentionalSelection::processCompetition");
            }
            
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
                
                // Validate relative activity
                if (std::isnan(relativeActivity) || std::isinf(relativeActivity)) {
                    throw NLMMemoryError("Invalid relative activity calculation",
                                        NLMMemoryError::ErrorType::ConsistencyViolation,
                                        "AttentionalSelection::processCompetition");
                }
                
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
        
        return winners_;
        
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in processCompetition: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in attentional selection competition: " + std::string(e.what()));
        // Continue with degraded functionality
        return winners_;
    }
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    try {
        if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
            attendedRegions_.push_back(region);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error focusing on region: " + std::string(e.what()));
    }
}

void AttentionalSelection::releaseAttention() {
    try {
        attendedRegions_.clear();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error releasing attention: " + std::string(e.what()));
    }
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    try {
        return attendedRegions_;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting attended regions: " + std::string(e.what()));
        return {};
    }
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    try {
        if (std::isnan(strength) || std::isinf(strength) || strength < 0.0f) {
            throw NLMMemoryError("Invalid inhibition strength: " + std::to_string(strength),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "AttentionalSelection::setInhibitionStrength");
        }
        
        inhibitionStrength_ = strength;
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in setInhibitionStrength: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error setting inhibition strength: " + std::string(e.what()));
        throw NLMMemoryError("Failed to set inhibition strength",
                            NLMMemoryError::ErrorType::ConsistencyViolation,
                            "AttentionalSelection::setInhibitionStrength");
    }
}

void AttentionalSelection::setExcitationStrength(float strength) {
    try {
        if (std::isnan(strength) || std::isinf(strength) || strength < 0.0f) {
            throw NLMMemoryError("Invalid excitation strength: " + std::to_string(strength),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "AttentionalSelection::setExcitationStrength");
        }
        
        excitationStrength_ = strength;
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in setExcitationStrength: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error setting excitation strength: " + std::string(e.what()));
        throw NLMMemoryError("Failed to set excitation strength",
                            NLMMemoryError::ErrorType::ConsistencyViolation,
                            "AttentionalSelection::setExcitationStrength");
    }
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    try {
        if (std::isnan(threshold) || std::isinf(threshold) || threshold < 0.0f) {
            throw NLMMemoryError("Invalid competition threshold: " + std::to_string(threshold),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "AttentionalSelection::setCompetitionThreshold");
        }
        
        competitionThreshold_ = threshold;
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error in setCompetitionThreshold: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error setting competition threshold: " + std::string(e.what()));
        throw NLMMemoryError("Failed to set competition threshold",
                            NLMMemoryError::ErrorType::ConsistencyViolation,
                            "AttentionalSelection::setCompetitionThreshold");
    }
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    try {
        auto it = std::find(winners_.begin(), winners_.end(), neuron);
        if (it != winners_.end()) {
            return 0.0f;  // Winners don't receive inhibition
        }
        return inhibitionStrength_;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting inhibition for neuron: " + std::string(e.what()));
        return 0.0f;
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
        NLM_LOG_ERROR("Error getting excitation for neuron: " + std::string(e.what()));
        return 0.0f;
    }
}

void AttentionalSelection::update(TimestepDuration dt) {
    try {
        // Validate timestep
        if (dt <= 0.0f) {
            NLM_LOG_WARNING("Invalid timestep for attentional selection update: " + std::to_string(dt));
            return;
        }
        
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
        NLM_LOG_ERROR("Error updating attentional selection: " + std::string(e.what()));
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    try {
        return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error checking if neuron is attended: " + std::string(e.what()));
        return false;
    }
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    try {
        if (std::isnan(biasStrength) || std::isinf(biasStrength) || biasStrength < 0.0f) {
            throw NLMMemoryError("Invalid top-down bias strength: " + std::to_string(biasStrength),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "AttentionalSelection::applyTopDownBias");
        }
        
        // Store bias in map keyed by NeuronId
        auto it = topDownBias_.find(neuron.value);
        if (it != topDownBias_.end()) {
            it->second += biasStrength;
        } else {
            topDownBias_[neuron.value] = biasStrength;
        }
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error applying top-down bias: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error applying top-down bias: " + std::string(e.what()));
        throw NLMMemoryError("Failed to apply top-down bias",
                            NLMMemoryError::ErrorType::ConsistencyViolation,
                            "AttentionalSelection::applyTopDownBias");
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    try {
        if (std::isnan(salienceStrength) || std::isinf(salienceStrength) || salienceStrength < 0.0f) {
            throw NLMMemoryError("Invalid bottom-up salience strength: " + std::to_string(salienceStrength),
                                NLMMemoryError::ErrorType::ConsistencyViolation,
                                "AttentionalSelection::applyBottomUpSalience");
        }
        
        // Store salience in map keyed by NeuronId
        auto it = bottomUpSalience_.find(neuron.value);
        if (it != bottomUpSalience_.end()) {
            it->second += salienceStrength;
        } else {
            bottomUpSalience_[neuron.value] = salienceStrength;
        }
    } catch (const NLMMemoryError& e) {
        NLM_LOG_ERROR("Memory error applying bottom-up salience: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error applying bottom-up salience: " + std::string(e.what()));
        throw NLMMemoryError("Failed to apply bottom-up salience",
                            NLMMemoryError::ErrorType::ConsistencyViolation,
                            "AttentionalSelection::applyBottomUpSalience");
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
        NLM_LOG_ERROR("Error resetting attentional selection: " + std::string(e.what()));
    }
}

} // namespace nlm