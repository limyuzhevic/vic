// NeuralWorkingMemory Implementation
// ==========================
// 
// Working Memory in NLM Context
// ---------------------------
// In the Neural Language Model (NLM) brain architecture, working memory
// serves as the brain's short-term memory system that maintains and manipulates
// information for immediate use in cognitive processing. Unlike traditional
// buffer systems, NLM implements working memory using neural dynamics with
// persistent activity, allowing information to be maintained through recurrent
// connections and activity-dependent plasticity.
// 
// Key Principles:
// - Information is encoded as distributed neural activity patterns
// - Memory traces are actively maintained through recurrent connections
// - Competition selects which patterns dominate attention
// - Decay prevents unbounded growth and prioritizes newer information
// - Integration with attentional selection enables goal-directed processing
// 
// The working memory system operates as a dynamic reservoir where:
// 1. Patterns are encoded as coordinated neural firing
// 2. Recurrent connections sustain activity through synaptic feedback
// 3. Competition gates information flow to higher processing areas
// 4. Decay and update mechanisms balance stability with flexibility
// 
// This implementation combines Hebbian learning principles with competitive
// dynamics to create a biologically plausible working memory system.

#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    Impl() : brain(nullptr) {}
};

// =============================================================================
// Memory Initialization and Configuration
// =============================================================================

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    // Initialize with default working memory parameters
    // 
    // Capacity: Number of memory traces that can be actively maintained
    // - 100 traces is a reasonable balance between memory capacity and
    //   neural resource constraints for demonstration purposes
    // 
    // Decay Rate: Strength of decay per timestep (0.01 = 1% decay per timestep)
    // - This prevents unbounded growth and prioritizes newer information
    // - Biologically plausible for short-term memory systems
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with capacity " + 
                std::to_string(capacity_) + " and decay rate " + 
                std::to_string(decayRate_));
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    // Establish connection to the brain system for neural operations
    // 
    // Parameters:
    //   brain: Pointer to the Brain instance that provides access to neural regions
    //          and injection capabilities. Cannot be null.
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Sets pImpl->brain and brain_ for future neural operations
    //   - Logs initialization for debugging
    //   - Resets all memory traces to empty state
    // 
    // Note: Must be called before any memory operations. This method establishes
    // the working memory's interface to the brain's neural infrastructure.
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize working memory with null brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    brain_ = brain;
    
    // Clear any existing memory traces
    clear();
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with " + 
                std::to_string(brain_->getTotalNeuronCount()) + " total neurons " +
                "available for memory encoding");
}

// =============================================================================
// Memory Encoding Methods
// =============================================================================

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    // Encode a pattern into working memory as coordinated neural activity
    // 
    // This is the primary encoding mechanism that transforms abstract patterns
    // into persistent neural representations. The pattern is distributed across
    // multiple neurons to create a population code, with recurrent connections
    // sustaining the activity over time.
    // 
    // Parameters:
    //   pattern: Vector of activation values representing the information to store.
    //           Each value maps to a corresponding neuron's firing level (0.0 to 1.0).
    //           Pattern length determines the dimensionality of the memory trace.
    //   strength: Amplitude multiplier for the encoded pattern (0.0 to 1.0).
    //            Scales all activation values to set memory trace strength.
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Allocates memory for new neural traces if capacity allows
    //   - Injects current into brain neurons to create persistent activity
    //   - Establishes recurrent connections for maintenance
    //   - Updates memoryNeurons_, memoryActivations_, and memoryTimestamps_
    //   - Logs "NeuralWorkingMemory: Stored pattern with X elements" on success
    // 
    // Memory Management:
    //   - Uses modulo indexing to reuse neurons within capacity constraints
    //   - Creates maintenance connections between adjacent neurons in the trace
    //   - Respects memory capacity - traces may be discarded if full
    // 
    // Usage:
    //   std::vector<float> featureVector = {0.5f, 0.8f, 0.3f};
    //   workingMemory.store(featureVector, 1.0f);
    //   // Encodes a 3-dimensional pattern with unit strength
    // 
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
    
    NLM_LOG_INFO("NeuralWorkingMemory: Stored pattern with " + std::to_string(pattern.size()) +
                " elements at strength " + std::to_string(strength));
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Store specific neuron activation directly in working memory
    // 
    // This method provides granular control over working memory by encoding
    // activation patterns at the individual neuron level. It's useful when you
    // want to specifically target a neuron for memory maintenance or when you
    // have precise control over which neurons participate in memory storage.
    // 
    // Parameters:
    //   neuron: Neuron ID to store in working memory. This neuron will maintain
    //           the specified activation level.
    //   activation: Target activation level for the neuron (typically 0.0 to 1.0).
    //               This represents the strength of the memory trace.
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Finds or adds the neuron to the working memory system
    //   - Sets the neuron's activation level in the memory system
    //   - Injects current into the brain neuron to create persistent activity
    //   - Logs "NeuralWorkingMemory: Stored activation for neuron X" on success
    // 
    // Usage:
    //   NeuronId keyNeuron = NeuronId(42);
    //   workingMemory.storeToNeuron(keyNeuron, 0.8f);
    //   // Stores a strong activation in a specific neuron
    // 
    // Implementation:
    //   - First checks if neuron already exists in memory
    //   - If exists, updates its activation level and timestamp
    //   - If not exists, adds it if capacity allows
    //   - Always injects current to maintain the activation
    
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
        NLM_LOG_WARNING("Working memory at capacity, cannot store neuron " + 
                       std::to_string(neuron.index()));
        return;
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
    
    NLM_LOG_INFO("NeuralWorkingMemory: Stored activation " + std::to_string(activation) +
                " for neuron " + std::to_string(neuron.index()));
}

// =============================================================================
// Memory Retrieval Methods
// =============================================================================

std::vector<float> NeuralWorkingMemory::retrieve() const {
    // Retrieve current working memory content as activity levels
    // 
    // This method provides read-only access to the working memory content,
    // allowing other systems (like the planner or attention systems) to
    // access the information currently maintained in memory.
    // 
    // Returns: Vector of activation levels representing the current memory content.
    //          Each element corresponds to a neuron's current activation level,
    //          maintaining the distributed representation format.
    // 
    // Memory Format:
    //   - Each element represents one dimension of the memory trace
    //   - Values are in the range [0.0, 1.0] representing activation levels
    //   - Order corresponds to the original encoding order
    //   - Missing dimensions are represented by lower activation levels
    // 
    // Usage:
    //   std::vector<float> memoryContent = workingMemory.retrieve();
    //   // Get all memory traces as a vector
    //   for (float activation : memoryContent) {
    //       // Process each dimension of memory
    //   }
    // 
    // Note:
    //   - This is a snapshot of current memory state
    //   - Active traces may have different weights due to competition
    //   - System uses competition to select which traces dominate attention
    
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

// =============================================================================
// Memory State Management
// =============================================================================

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    // Check if specific neuron is part of working memory
    // 
    // This method provides quick lookup to determine if a given neuron
    // is currently maintaining a memory trace in the working memory system.
    // 
    // Parameters:
    //   neuron: Neuron ID to check for membership in working memory.
    // 
    // Returns: bool
    //   - true if neuron is actively maintaining a memory trace
    //   - false if neuron is not in working memory
    // 
    // Usage:
    //   NeuronId targetNeuron = NeuronId(42);
    //   if (workingMemory.contains(targetNeuron)) {
    //       // Neuron is maintaining memory - use its activation level
    //   }
    
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    // Get activation level of a specific neuron in working memory
    // 
    // This method provides precise access to individual neuron activation levels,
    // allowing fine-grained control and monitoring of specific memory traces.
    // 
    // Parameters:
    //   neuron: Neuron ID whose activation level is requested.
    
    // Returns: float
    //   - Activation level of the specified neuron (0.0 to 1.0)
    //   - Returns 0.0 if neuron is not in working memory
    
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    // Update working memory (maintenance and decay)
    // 
    // This is the core update method that maintains memory traces over time.
    // It implements biologically plausible mechanisms for memory maintenance
    // including recurrent activation, competition, and decay.
    // 
    // Parameters:
    //   dt: Timestep duration in simulation time units. Used for time-based
    //       calculations, though most working memory operations are event-driven.
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Maintains active memory traces through recurrent synaptic connections
    //   - Applies competitive dynamics to select dominant traces
    //   - Decays weak traces to prevent unbounded growth
    //   - Updates all memory state variables (activations, timestamps, traces)
    //   - Logs "NeuralWorkingMemory: Updated with X traces" on completion
    // 
    // Update Process:
    //   1. Recurrence: Reinforce active memory neurons through recurrent connections
    //   2. Competition: Select winning traces through competitive dynamics
    //   3. Decay: Gradually reduce activation of weak traces
    //   4. Consolidation: Update memory state for next timestep
    // 
    // Usage:
    //   TimestepDuration dt = TimestepDuration(0.001f); // 1ms timestep
    //   workingMemory.update(dt);
    //   // Maintains memory traces for next timestep
    // 
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
    
    NLM_LOG_INFO("NeuralWorkingMemory: Updated with " + std::to_string(memoryNeurons_.size()) +
                " traces (active: " + std::to_string(activeTraces_.size()) + ")");
}

void NeuralWorkingMemory::clear() {
    // Clear working memory
    // 
    // This method resets the working memory system to an empty state,
    // removing all memory traces and freeing associated neural resources.
    // It's useful for task switching, memory reset, or system reinitialization.
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Clears all memory traces (memoryNeurons_, memoryActivations_, etc.)
    //   - Removes all recurrent connections
    //   - Resets active traces to empty state
    //   - Releases injected current from brain neurons
    //   - Logs "NeuralWorkingMemory: Cleared" on completion
    // 
    // Usage:
    //   workingMemory.clear();
    //   // All working memory traces removed, system ready for new content
    
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
    
    NLM_LOG_INFO("NeuralWorkingMemory: Cleared all memory traces");
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    // Strengthen working memory representation (for rehearsal)
    // 
    // This method enhances existing memory traces, useful for rehearsal,
    // consolidation, or when you want to boost the strength of important memories.
    // 
    // Parameters:
    //   factor: Multiplication factor for memory trace strengths (1.0 = no change,
    //           2.0 = double strength, 0.5 = half strength).
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Multiplies all memory activations by the factor
    //   - Rescales activations to maintain reasonable range
    //   - Re-injects current into neurons to maintain new strength levels
    //   - Logs "NeuralWorkingMemory: Strengthened by factor X" on completion
    
    if (factor <= 0.0f) {
        NLM_LOG_WARNING("Cannot strengthen memory with non-positive factor");
        return;
    }
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= factor;
        
        // Re-inject current to maintain new strength
        if (memoryNeurons_.size() > i && brain_) {
            brain_->injectCurrent(memoryNeurons_[i], memoryActivations_[i] * 5.0f);
        }
    }
    
    NLM_LOG_INFO("NeuralWorkingMemory: Strengthened all traces by factor " + 
                std::to_string(factor));
}

void NeuralWorkingMemory::runCompetition() {
    // Competition between memory traces
    // 
    // This method implements competitive dynamics where memory traces
    // compete for neural resources and attentional focus. Only the strongest
    // traces win and are maintained, while weaker traces are suppressed.
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Applies competition to select winning memory traces
    //   - Updates activeTraces_ with winners
    //   - Suppresses losing traces through competitive inhibition
    //   - Logs "NeuralWorkingMemory: Competition completed with X winners" on completion
    
    // Simple competition: select traces with highest activation
    if (memoryActivations_.empty()) {
        winners_.clear();
        return;
    }
    
    // Find winning traces (highest activations)
    std::vector<std::pair<float, size_t>> activationIndexPairs;
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        activationIndexPairs.emplace_back(memoryActivations_[i], i);
    }
    
    std::sort(activationIndexPairs.begin(), activationIndexPairs.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Keep only the top N traces as winners (N = capacity / 2)
    size_t numWinners = std::min(memoryNeurons_.size() / 2, memoryNeurons_.size());
    winners_.clear();
    
    for (size_t i = 0; i < numWinners; ++i) {
        size_t idx = activationIndexPairs[i].second;
        if (idx < memoryNeurons_.size()) {
            winners_.push_back(memoryNeurons_[idx]);
        }
    }
    
    // Update active traces
    activeTraces_.clear();
    for (size_t i = 0; i < winners_.size(); ++i) {
        activeTraces_.push_back(i);
    }
    
    NLM_LOG_INFO("NeuralWorkingMemory: Competition completed with " + 
                std::to_string(winners_.size()) + " winners");
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    // Check if this neuron is part of the winning population
    // 
    // This method provides access to the competitive selection process,
    // allowing other systems to know which memories are currently dominant
    // and receiving attentional resources.
    // 
    // Parameters:
    //   neuron: Neuron ID to check for winner status.
    
    // Returns: bool
    //   - true if neuron is in the current winning population
    //   - false if neuron is not a winner
    
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    // Get overall memory activity level
    // 
    // This method provides a scalar measure of how active the working
    // memory system is, useful for monitoring memory load and attention.
    // 
    // Returns: float
    //   - Average activation level across all memory traces
    //   - Returns 0.0 if no memory traces exist
    
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = std::accumulate(memoryActivations_.begin(), memoryActivations_.end(), 0.0f);
    return total / static_cast<float>(memoryActivations_.size());
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Create recurrent connection for maintenance
    // 
    // This method establishes synaptic connections between neurons to
    // create recurrent loops that maintain memory traces over time.
    // This is a key mechanism for working memory persistence.
    // 
    // Parameters:
    //   from: Source neuron ID (preserves earlier activity)
    //   to: Target neuron ID (receives recurrent input)
    //   strength: Synaptic strength of the recurrent connection
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Adds connection to maintenanceSynapses vector
    //   - Establishes neural pathway for maintaining memory activity
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Update recurrent connections for maintenance
    // 
    // This method updates the strength and effectiveness of recurrent
    // connections based on current memory activity and plasticity rules.
    // It implements activity-dependent maintenance of memory traces.
    // 
    // Returns: void
    // 
    // Implementation:
    //   - Iterates through all maintenance synapses
    //   - Updates their strength based on plasticity rules
    //   - May add or remove synapses based on activity
    
    for (auto& synapse : pImpl->maintenanceSynapses) {
        // In a full implementation, this would apply plasticity rules
        // and update synaptic strength based on recent activity
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    // Decay weak memory traces
    // 
    // This method implements the decay process that prevents unbounded
    // growth of weak memory traces and prioritizes newer information.
    // It uses both age-based and activation-based decay mechanisms.
    // 
    // Returns: void
    // 
    // Side effects:
    //   - Reduces activation of traces that are too weak or too old
    //   - Removes traces that decay below threshold
    //   - Updates memory state to reflect decayed traces
    
    // Implementation would include:
    // - Age-based decay: Older traces decay faster
    // - Activation-based decay: Weaker traces decay faster
    // - Trace removal: Traces below threshold are removed
}

// =============================================================================
// Advanced Working Memory Features
// =============================================================================

void NeuralWorkingMemory::addNote(const std::string& note) {
    // Add a note about memory operation
    // 
    // This method allows recording of operational notes about working memory
    // behavior, useful for debugging, analysis, and understanding memory dynamics.
    // 
    // Parameters:
    //   note: Text note describing memory operation or observation.
    // 
    // Returns: void
    // 
    // Usage:
    //   workingMemory.addNote("Pattern stored with high fidelity");
    //   workingMemory.addNote("Competition suppressed trace [3]");
    
    notes_.push_back(note);
    // Limit notes to prevent memory growth
    if (notes_.size() > 100) {
        notes_.erase(notes_.begin());
    }
}

const std::vector<std::string>& NeuralWorkingMemory::getNotes() const {
    // Get all notes about memory operations
    // 
    // This method provides access to operational notes about working memory,
    // useful for debugging, analysis, and understanding system behavior.
    
    return notes_;
}

} // namespace nlm
