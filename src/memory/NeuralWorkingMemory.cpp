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

/**
 * @brief Stores a pattern in working memory by encoding it as neural activity
 * 
 * This method implements the core working memory storage mechanism. It takes
 * an input pattern and encodes it using a subset of available neurons,
 * maintaining the activation levels and creating recurrent connections for
 * memory persistence.
 * 
 * @param pattern Input pattern to store (normalized float values)
 * @param strength Multiplicative factor for pattern encoding strength
 * @post Pattern is stored with neural activations in the brain's neuron pool
 * @pre Brain must be initialized and pattern must not be empty
 * 
 * @complexity O(N_pattern + N_connections) where N is pattern length and
 *            current number of memory neurons
 */
void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) {
        return;
    }
    
    // Calculate how many neurons we need for this pattern
    const size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    // Encode each element of the pattern using available neurons
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        const NeuronId targetNeuron = memoryNeurons_[i % memoryNeurons_.size()];
        const float patternValue = pattern[i];
        const float activation = patternValue * strength;
        
        // Set neuron activation through current injection
        injectPatternActivation(targetNeuron, activation);
        
        // Update stored activation values
        updateMemoryActivation(i, activation, targetNeuron);
    }
    
    // Create recurrent connections between consecutive neurons for maintenance
    establishMaintenanceConnections(strength);
}

/**
 * @brief Injects current into a specific neuron to achieve target activation
 * 
 * @param neuron Target neuron for activation
 * @param activation Target activation level to achieve
 */
void NeuralWorkingMemory::injectPatternActivation(NeuronId neuron, float activation) {
    // Get all neurons in the neuron's region
    if (auto* region = brain_->getRegion(neuron.getId() / 1000)) {
        if (auto* allNeurons = region->getAllNeurons()) {
            // Find and activate the target neuron
            for (auto* currentNeuron : *allNeurons) {
                if (currentNeuron->getId() == neuron) {
                    currentNeuron->injectCurrent(activation * 5.0f);
                    return;  // Found and activated, exit
                }
            }
        }
    }
}

/**
 * @brief Updates memory activation storage for a specific neuron
 * 
 * @param index Index of the memory slot
 * @param activation New activation value
 * @param neuron Neuron associated with this memory slot
 */
void NeuralWorkingMemory::updateMemoryActivation(size_t index, float activation, 
                                                 const NeuronId& neuron) {
    if (index < memoryActivations_.size()) {
        // Update existing activation
        memoryActivations_[index] = activation;
    } else {
        // Create new memory entry
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        memoryNeurons_.push_back(neuron);
    }
}

/**
 * @brief Creates recurrent connections between neurons for memory maintenance
 * 
 * Establishes feedforward recurrent connections between consecutive neurons
 * in the memory sequence to create persistent activity patterns.
 * 
 * @param strength Base connection strength for recurrent synapses
 */
void NeuralWorkingMemory::establishMaintenanceConnections(float strength) {
    // Create connections between consecutive neurons
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        const NeuronId fromNeuron = memoryNeurons_[i-1];
        const NeuronId toNeuron = memoryNeurons_[i];
        const float connectionStrength = strength * 0.5f;
        
        createRecurrentConnection(fromNeuron, toNeuron, connectionStrength);
    }
}

/**
 * @brief Stores specific neuron activation in working memory
 * 
 * This method allows direct storage of neuron activations without pattern encoding,
 * useful for incorporating specific neural states into working memory.
 * 
 * @param neuron Neuron ID to store
 * @param activation Activation level to store
 * @post Neuron is added to or updated in working memory with given activation
 * @pre Brain must be initialized
 * 
 * @complexity O(N_memory_neutrons) for search operation
 */
void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Search for existing neuron in memory
    auto memoryIt = findMemoryNeuron(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (memoryIt != memoryNeurons_.end()) {
        // Update existing memory entry
        const size_t memoryIndex = getMemoryIndex(memoryIt);
        updateMemoryEntry(memoryIndex, activation);
    } else if (memoryNeurons_.size() < capacity_) {
        // Add new memory entry if capacity allows
        addNewMemoryEntry(neuron, activation);
    }
    
    // Maintain activation through current injection
    maintainMemoryActivation(neuron, activation);
}

/**
 * @brief Finds a neuron in the memory neuron list using iterative search
 * 
 * @param begin Iterator to start of memory neuron list
 * @param end Iterator to end of memory neuron list
 * @param neuron Target neuron to find
 * @return Iterator to found neuron or end if not found
 */
template<typename Iterator>
Iterator NeuralWorkingMemory::findMemoryNeuron(Iterator begin, Iterator end, const NeuronId& neuron) {
    return std::find(begin, end, neuron);
}

/**
 * @brief Gets index of neuron in memory from iterator
 * 
 * @param iterator Iterator to neuron in memory list
 * @return Index position of neuron in memory lists
 */
size_t NeuralWorkingMemory::getMemoryIndex(std::vector<NeuronId>::iterator iterator) {
    return std::distance(memoryNeurons_.begin(), iterator);
}

/**
 * @brief Updates activation of existing memory entry
 * 
 * @param index Index of memory entry to update
 * @param activation New activation value
 */
void NeuralWorkingMemory::updateMemoryEntry(size_t index, float activation) {
    memoryActivations_[index] = activation;
    memoryTimestamps_[index] = 0;  // Reset timestamp for fresh activation
}

/**
 * @brief Adds new neuron activation to working memory
 * 
 * @param neuron Neuron to add
 * @param activation Activation level to store
 */
void NeuralWorkingMemory::addNewMemoryEntry(NeuronId neuron, float activation) {
    memoryNeurons_.push_back(neuron);
    memoryActivations_.push_back(activation);
    memoryTimestamps_.push_back(0);
}

/**
 * @brief Maintains neuron activation through current injection
 * 
 * @param neuron Neuron to maintain
 * @param activation Current activation level
 */
void NeuralWorkingMemory::maintainMemoryActivation(NeuronId neuron, float activation) {
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

/**
 * @brief Retrieves all current memory activations
 * 
 * @return Vector of all memory activation levels, in order of storage
 * @post Returns current state of working memory without modification
 * 
 * @complexity O(N_memory_entries) - linear with number of stored memories
 */
std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> memoryState;
    memoryState.reserve(memoryActivations_.size());
    
    // Copy all activation values
    for (const float activation : memoryActivations_) {
        memoryState.push_back(activation);
    }
    
    return memoryState;
}

/**
 * @brief Checks if a specific neuron is currently stored in working memory
 * 
 * @param neuron Neuron ID to check for
 * @return true if neuron is in working memory, false otherwise
 * 
 * @complexity O(N_memory_neutrons) for search operation
 */
bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return findMemoryNeuron(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

/**
 * @brief Gets activation level of specific neuron in working memory
 * 
 * @param neuron Neuron ID to query
 * @return Activation level if neuron in memory, 0.0f otherwise
 * 
 * @complexity O(N_memory_neutrons) for search operation
 */
float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto memoryIt = findMemoryNeuron(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (memoryIt != memoryNeurons_.end()) {
        const size_t memoryIndex = getMemoryIndex(memoryIt);
        return memoryActivations_[memoryIndex];
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
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f);
            
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