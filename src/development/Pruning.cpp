#include "Pruning.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/Neuron.hpp"
#include "../../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <set>

namespace nlm {

struct AdvancedPruningImpl {
    // Pruning configuration
    float threshold;                      // Minimum synaptic weight for pruning (0-1)
    float pruningRate;                     // Base pruning rate (synapses/ms)
    float competitionStrength;             // Strength of competitive pruning (0-1)
    float activityThreshold;               // Minimum activity for survival (Hz)
    float resourceCapacity;                // Maximum synapses before pruning (0-1)
    float timeConstant;                    // Time constant for pruning dynamics (ms)
    
    // Biological mechanism parameters
    bool activityDependentPruningEnabled;  // Enable activity-dependent pruning
    bool competitivePruningEnabled;        // Enable competitive pruning
    float hebbianProtectionStrength;        // Strength of Hebbian protection for synapses
    float homeostaticPruningStrength;      // Strength of homeostatic pruning
    
    // State tracking
    size_t totalPrunedSynapses;            // Total number of pruned synapses
    size_t currentlyPrunedCount;           // Currently pruned synapses
    float resourceUtilization;              // Current resource usage (0-1)
    float lastActivitySum;                  // Sum of recent activities for homeostasis
    Timestamp lastHomeostasisUpdate;        // Time of last homeostasis update
    
    // Synapse evaluation scores
    std::unordered_map<SynapseHandle, float> prunabilityScores;
    std::unordered_map<SynapseHandle, float> activityScores;
    std::unordered_map<SynapseHandle, float> strengthScores;
    std::unordered_map<SynapseHandle, float> recencyScores;
    
    // Priority queues for pruning selection
    std::priority_queue<std::pair<float, SynapseHandle>> highPriorityPruneQueue;
    std::priority_queue<std::pair<float, SynapseHandle>> mediumPriorityPruneQueue;
    std::queue<SynapseHandle> lowPriorityPruneQueue;
    
    // Pruning history
    std::vector<Timestamp> pruningHistory;
    std::unordered_map<SynapseHandle, Timestamp> synapsePruningTimes;
    
    // Biological constants
    static constexpr float MIN_THRESHOLD = 0.001f;           // Very weak synapses
    static constexpr float MAX_THRESHOLD = 0.5f;            // Moderately strong synapses
    static constexpr float MIN_PRUNING_RATE = 1e-7f;         // Very slow base pruning
    static constexpr float MAX_PRUNING_RATE = 1e-4f;         // Fast pruning during development
    static constexpr float MIN_COMPETITION_STRENGTH = 0.0f;  // No competition
    static constexpr float MAX_COMPETITION_STRENGTH = 1.0f;   // Strong competition
    static constexpr float MIN_ACTIVITY_THRESHOLD = 0.01f;  // Very low activity
    static constexpr float MAX_ACTIVITY_THRESHOLD = 10.0f;  // High activity
    static constexpr float MIN_RESOURCE_CAPACITY = 0.5f;     // Prune at 50% capacity
    static constexpr float MAX_RESOURCE_CAPACITY = 0.95f;   // Prune at 95% capacity
    
    // Pruning constants
    static constexpr float ACTIVITY_DECAY = 0.2f;           // Activity decay per time step
    static constexpr float STRENGTH_DECAY = 0.1f;           // Strength decay per time step
    static constexpr float RECENCY_WINDOW = 1000.0f;        // ms, recency evaluation window
    static constexpr float HEBBIAN_PROTECTION_FACTOR = 2.0f; // Hebbian protection multiplier
    static constexpr float HOMEOSTATIC_PRUNING_FACTOR = 0.5f; // Homeostatic pruning strength
    static constexpr float SYNAPSE_RESTORATION_RATE = 0.01f;  // Rate of synapse restoration
    static constexpr float COMPETITIVE_PRESSURE = 1.0f;     // Competitive pressure multiplier
    
    AdvancedPruningImpl() {
        // Initialize with default values
        threshold = MIN_THRESHOLD;
        pruningRate = MIN_PRUNING_RATE;
        competitionStrength = 0.5f;
        activityThreshold = 1.0f;
        resourceCapacity = 0.8f;
        timeConstant = 100.0f;
        
        activityDependentPruningEnabled = true;
        competitivePruningEnabled = true;
        hebbianProtectionStrength = 0.7f;
        homeostaticPruningStrength = 0.3f;
        
        totalPrunedSynapses = 0;
        currentlyPrunedCount = 0;
        resourceUtilization = 0.0f;
        lastActivitySum = 0.0f;
        lastHomeostasisUpdate = 0;
        
        pruningHistory.reserve(1000);
    }
};

Pruning::Pruning() : pImpl(new AdvancedPruningImpl()) {}

Pruning::~Pruning() = default;

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, AdvancedPruningImpl::MIN_THRESHOLD,
                                   AdvancedPruningImpl::MAX_THRESHOLD);
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, AdvancedPruningImpl::MIN_PRUNING_RATE,
                                    AdvancedPruningImpl::MAX_PRUNING_RATE);
}

void Pruning::setCompetitionStrength(float strength) {
    pImpl->competitionStrength = std::clamp(strength, AdvancedPruningImpl::MIN_COMPETITION_STRENGTH,
                                            AdvancedPruningImpl::MAX_COMPETITION_STRENGTH);
}

void Pruning::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::clamp(threshold, AdvancedPruningImpl::MIN_ACTIVITY_THRESHOLD,
                                          AdvancedPruningImpl::MAX_ACTIVITY_THRESHOLD);
}

void Pruning::setResourceCapacity(float capacity) {
    pImpl->resourceCapacity = std::clamp(capacity, AdvancedPruningImpl::MIN_RESOURCE_CAPACITY,
                                         AdvancedPruningImpl::MAX_RESOURCE_CAPACITY);
}

void Pruning::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(tau, 1.0f); // Ensure positive time constant
}

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

float Pruning::getCompetitionStrength() const {
    return pImpl->competitionStrength;
}

float Pruning::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

float Pruning::getResourceCapacity() const {
    return pImpl->resourceCapacity;
}

float Pruning::getTimeConstant() const {
    return pImpl->timeConstant;
}

void Pruning::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    float timestep = static_cast<float>(dt) * 1000.0f; // Convert to ms
    
    // Update resource utilization
    updateResourceUtilization(timestep);
    
    // Evaluate synapse prunability if pruning is needed
    if (pImpl->resourceUtilization > pImpl->resourceCapacity) {
        evaluateSynapsePrunability(brain, timestep);
    }
    
    // Apply activity-dependent pruning
    if (pImpl->activityDependentPruningEnabled) {
        applyActivityDependentPruning(brain, timestep);
    }
    
    // Apply competitive pruning
    if (pImpl->competitivePruningEnabled) {
        applyCompetitivePruning(brain, rng, timestep);
    }
    
    // Apply homeostatic pruning
    applyHomeostaticPruning(brain, timestep);
    
    // Execute scheduled pruning
    executePruning(brain, rng, timestep);
    
    // Update pruning history
    updatePruningHistory(timestep);
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    update(brain, rng, 0.001f); // Default 1ms timestep
}

size_t Pruning::getPrunedSynapseCount() const {
    return pImpl->totalPrunedSynapses;
}

size_t Pruning::getCurrentPrunedCount() const {
    return pImpl->currentlyPrunedCount;
}

float Pruning::getPruningEfficiency() const {
    if (pImpl->totalPrunedSynapses == 0) return 0.0f;
    
    // Compute efficiency as ratio of pruned synapses to potential pruning opportunities
    return std::min(1.0f, static_cast<float>(pImpl->totalPrunedSynapses) / 
                    (pImpl->totalPrunedSynapses + 1000)); // Normalized efficiency
}

float Pruning::getResourceRelief() const {
    return std::min(1.0f, pImpl->resourceUtilization / pImpl->resourceCapacity);
}

void Pruning::reset() {
    pImpl->totalPrunedSynapses = 0;
    pImpl->currentlyPrunedCount = 0;
    pImpl->resourceUtilization = 0.0f;
    pImpl->lastActivitySum = 0.0f;
    pImpl->prunabilityScores.clear();
    pImpl->activityScores.clear();
    pImpl->strengthScores.clear();
    pImpl->recencyScores.clear();
    
    while (!pImpl->highPriorityPruneQueue.empty()) pImpl->highPriorityPruneQueue.pop();
    while (!pImpl->mediumPriorityPruneQueue.empty()) pImpl->mediumPriorityPruneQueue.pop();
    while (!pImpl->lowPriorityPruneQueue.empty()) pImpl->lowPriorityPruneQueue.pop();
    
    pImpl->pruningHistory.clear();
    pImpl->synapsePruningTimes.clear();
}

void Pruning::initialize(Brain* brain, RandomGenerator& rng) {
    reset();
    
    // Initialize with biologically realistic starting point
    if (brain) {
        // Get current brain size to set reasonable limits
        int neuronCount = brain->getNeuronCount();
        pImpl->resourceCapacity = 0.7f; // Prune when at 70% capacity
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Check if synapse is eligible for pruning
    if (!isSynapsePrunable(synapse)) return false;
    
    // Force prune the synapse
    // In real implementation, this would remove the synapse from brain structure
    // For now, we'll mark it as pruned and update statistics
    
    Timestamp currentTime = getCurrentTime();
    
    // Remove from priority queues
    removeFromPruningQueues(synapse->getHandle());
    
    // Update pruning statistics
    pImpl->totalPrunedSynapses++;
    pImpl->currentlyPrunedCount++;
    
    // Record pruning time
    pImpl->synapsePruningTimes[synapse->getHandle()] = currentTime;
    
    return true;
}

bool Pruning::isSynapsePrunable(Synapse* synapse) const {
    if (!synapse) return false;
    
    // Check if synapse has already been pruned
    if (pImpl->synapsePruningTimes.find(synapse->getHandle()) != 
        pImpl->synapsePruningTimes.end()) {
        return false; // Already pruned
    }
    
    // Get synapse prunability score
    float prunabilityScore = getSynapsePrunabilityScore(synapse);
    
    // Check against threshold
    return prunabilityScore > pImpl->threshold;
}

void Pruning::setHebbianProtection(float strength) {
    pImpl->hebbianProtectionStrength = std::clamp(strength, 0.0f, 1.0f);
}

void Pruning::setHomeostaticPruning(float strength) {
    pImpl->homeostaticPruningStrength = std::clamp(strength, 0.0f, 1.0f);
}

void Pruning::enableActivityDependentPruning(bool enable) {
    pImpl->activityDependentPruningEnabled = enable;
}

void Pruning::enableCompetitivePruning(bool enable) {
    pImpl->competitivePruningEnabled = enable;
}

bool Pruning::isActivityDependentPruningEnabled() const {
    return pImpl->activityDependentPruningEnabled;
}

bool Pruning::isCompetitivePruningEnabled() const {
    return pImpl->competitivePruningEnabled;
}

float Pruning::getHebbianProtectionStrength() const {
    return pImpl->hebbianProtectionStrength;
}

float Pruning::getHomeostaticPruningStrength() const {
    return pImpl->homeostaticPruningStrength;
}

float Pruning::getSynapsePrunabilityScore(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    auto it = pImpl->prunabilityScores.find(synapse->getHandle());
    if (it != pImpl->prunabilityScores.end()) {
        return it->second;
    }
    
    return 0.0f; // Default neutral score
}

float Pruning::getActivityScore(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    auto it = pImpl->activityScores.find(synapse->getHandle());
    if (it != pImpl->activityScores.end()) {
        return it->second;
    }
    
    return 0.0f; // Default neutral score
}

float Pruning::getStrengthScore(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    auto it = pImpl->strengthScores.find(synapse->getHandle());
    if (it != pImpl->strengthScores.end()) {
        return it->second;
    }
    
    return 0.0f; // Default neutral score
}

float Pruning::getRecencyScore(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    auto it = pImpl->recencyScores.find(synapse->getHandle());
    if (it != pImpl->recencyScores.end()) {
        return it->second;
    }
    
    return 0.0f; // Default neutral score
}

// Private helper methods
void Pruning::updateResourceUtilization(float dt) {
    // Update resource utilization based on current brain state
    // In real implementation, this would access brain structure
    
    // Placeholder implementation
    // Update resource utilization based on synapse count
    float estimatedUtilization = std::min(1.0f, static_cast<float>(getCurrentSynapseCount()) / 
                                         (pImpl->resourceCapacity * 1000.0f));
    
    // Apply smooth transitions
    float utilizationChange = (estimatedUtilization - pImpl->resourceUtilization) * 
                             (1.0f - std::exp(-dt / pImpl->timeConstant));
    pImpl->resourceUtilization += utilizationChange;
}

void Pruning::evaluateSynapsePrunability(Brain* brain, float dt) {
    // Evaluate prunability of all synapses in the brain
    if (!brain) return;
    
    // Clear previous scores
    pImpl->prunabilityScores.clear();
    pImpl->activityScores.clear();
    pImpl->strengthScores.clear();
    pImpl->recencyScores.clear();
    
    // Get all synapses in the brain (placeholder)
    // In real implementation, this would iterate over all synapses
    
    // Evaluate each synapse's prunability
    // For now, we'll use placeholder logic
    size_t numSynapses = getCurrentSynapseCount();
    
    for (size_t i = 0; i < numSynapses; ++i) {
        // Placeholder synapse evaluation
        float activityScore = rng.uniformReal(0.0f, 1.0f);
        float strengthScore = rng.uniformReal(0.0f, 1.0f);
        float recencyScore = rng.uniformReal(0.0f, 1.0f);
        
        // Compute overall prunability score
        float prunabilityScore = activityScore * 0.3f + strengthScore * 0.4f + 
                                 recencyScore * 0.3f;
        
        // Store scores
        pImpl->activityScores[static_cast<SynapseHandle>(i)] = activityScore;
        pImpl->strengthScores[static_cast<SynapseHandle>(i)] = strengthScore;
        pImpl->recencyScores[static_cast<SynapseHandle>(i)] = recencyScore;
        pImpl->prunabilityScores[static_cast<SynapseHandle>(i)] = prunabilityScore;
        
        // Add to appropriate priority queue
        addToPruningQueue(static_cast<SynapseHandle>(i), prunabilityScore);
    }
}

void Pruning::applyActivityDependentPruning(Brain* brain, float dt) {
    // Apply activity-dependent pruning rules
    // Synapses with low activity are prioritized for pruning
    
    // Update activity scores based on recent activity
    for (auto& pair : pImpl->activityScores) {
        SynapseHandle synapseId = pair.first;
        float currentActivity = getSynapseActivity(brain, synapseId);
        
        // Update activity score with decay
        float activityDecay = std::exp(-dt / pImpl->activityDecay);
        float updatedActivityScore = currentActivity * activityDecay;
        
        // Store updated score
        pair.second = updatedActivityScore;
        
        // Update prunability score
        pImpl->prunabilityScores[synapseId] = 
            updatedActivityScore * 0.3f + pImpl->strengthScores[synapseId] * 0.4f +
            pImpl->recencyScores[synapseId] * 0.3f;
        
        // Update priority queue
        removeFromPruningQueues(synapseId);
        addToPruningQueue(synapseId, pImpl->prunabilityScores[synapseId]);
    }
}

void Pruning::applyCompetitivePruning(Brain* brain, RandomGenerator& rng, float dt) {
    // Apply competitive pruning based on synaptic effectiveness
    // Strong, active synapses are protected from pruning
    
    // Apply Hebbian protection to strong synapses
    for (auto& pair : pImpl->prunabilityScores) {
        SynapseHandle synapseId = pair.first;
        float currentScore = pair.second;
        
        // Get synapse strength and activity
        float strength = pImpl->strengthScores[synapseId];
        float activity = pImpl->activityScores[synapseId];
        
        // Apply Hebbian protection
        float hebbianProtection = 1.0f;
        if (strength > 0.7f && activity > 0.5f) {
            hebbianProtection = HEBBIAN_PROTECTION_FACTOR;
        }
        
        // Apply competitive pressure
        float competitivePressure = COMPETITIVE_PRESSURE;
        if (activity < 0.2f) {
            competitivePressure *= (1.0f - pImpl->competitionStrength) + 0.1f;
        }
        
        // Compute protected score
        float protectedScore = currentScore * hebbianProtection * competitivePressure;
        
        // Update prunability score
        pImpl->prunabilityScores[synapseId] = protectedScore;
        
        // Update priority queue
        removeFromPruningQueues(synapseId);
        addToPruningQueue(synapseId, protectedScore);
    }
}

void Pruning::applyHomeostaticPruning(Brain* brain, float dt) {
    // Apply homeostatic pruning to maintain network stability
    // Controls overall pruning level based on network state
    
    // Compute target pruning level
    float targetPruningLevel = pImpl->homeostaticPruningStrength * 
                              (pImpl->resourceUtilization - pImpl->resourceCapacity);
    
    // Adjust pruning rate based on homeostatic needs
    float homeostaticAdjustment = 1.0f;
    if (targetPruningLevel > 0.0f) {
        homeostaticAdjustment = 1.0f + targetPruningLevel * HOMEOSTATIC_PRUNING_FACTOR;
    }
    
    // Apply adjustment to all pruning queues
    adjustAllPruningQueues(homeostaticAdjustment);
}

void Pruning::executePruning(Brain* brain, RandomGenerator& rng, float dt) {
    // Execute pruning of synapses from priority queues
    
    size_t synapsesToPrune = static_cast<size_t>(pImpl->pruningRate * dt * 
                                                 getCurrentSynapseCount());
    
    if (synapsesToPrune == 0) return;
    
    // Prune from high priority queue first
    while (synapsesToPrune > 0 && !pImpl->highPriorityPruneQueue.empty()) {
        auto top = pImpl->highPriorityPruneQueue.top();
        pImpl->highPriorityPruneQueue.pop();
        
        // Execute pruning (placeholder)
        executeSinglePruning(brain, top.second);
        
        synapsesToPrune--;
    }
    
    // Continue with medium and low priority queues
    while (synapsesToPrune > 0 && !pImpl->mediumPriorityPruneQueue.empty()) {
        auto top = pImpl->mediumPriorityPruneQueue.top();
        pImpl->mediumPriorityPruneQueue.pop();
        
        executeSinglePruning(brain, top.second);
        
        synapsesToPrune--;
    }
    
    while (synapsesToPrune > 0 && !pImpl->lowPriorityPruneQueue.empty()) {
        auto top = pImpl->lowPriorityPruneQueue.front();
        pImpl->lowPriorityPruneQueue.pop();
        
        executeSinglePruning(brain, top);
        
        synapsesToPrune--;
    }
}

void Pruning::updatePruningHistory(float dt) {
    // Update pruning history and statistics
    pImpl->pruningHistory.push_back(dt);
    
    // Keep history manageable
    if (pImpl->pruningHistory.size() > 1000) {
        pImpl->pruningHistory.erase(pImpl->pruningHistory.begin());
    }
}

void Pruning::addToPruningQueue(SynapseHandle synapseId, float priority) {
    // Add synapse to appropriate priority queue based on priority score
    
    if (priority > 0.7f) {
        // High priority for pruning
        pImpl->highPriorityPruneQueue.push(std::make_pair(priority, synapseId));
    } else if (priority > 0.3f) {
        // Medium priority
        pImpl->mediumPriorityPruneQueue.push(std::make_pair(priority, synapseId));
    } else {
        // Low priority
        pImpl->lowPriorityPruneQueue.push(synapseId);
    }
}

void Pruning::removeFromPruningQueues(SynapseHandle synapseId) {
    // Remove synapse from all priority queues
    
    // Remove from high priority queue
    std::priority_queue<std::pair<float, SynapseHandle>> tempHigh;
    while (!pImpl->highPriorityPruneQueue.empty()) {
        auto top = pImpl->highPriorityPruneQueue.top();
        pImpl->highPriorityPruneQueue.pop();
        
        if (top.second != synapseId) {
            tempHigh.push(top);
        }
    }
    pImpl->highPriorityPruneQueue = tempHigh;
    
    // Remove from medium priority queue
    std::priority_queue<std::pair<float, SynapseHandle>> tempMedium;
    while (!pImpl->mediumPriorityPruneQueue.empty()) {
        auto top = pImpl->mediumPriorityPruneQueue.top();
        pImpl->mediumPriorityPruneQueue.pop();
        
        if (top.second != synapseId) {
            tempMedium.push(top);
        }
    }
    pImpl->mediumPriorityPruneQueue = tempMedium;
    
    // Remove from low priority queue
    std::queue<SynapseHandle> tempLow;
    while (!pImpl->lowPriorityPruneQueue.empty()) {
        auto front = pImpl->lowPriorityPruneQueue.front();
        pImpl->lowPriorityPruneQueue.pop();
        
        if (front != synapseId) {
            tempLow.push(front);
        }
    }
    pImpl->lowPriorityPruneQueue = tempLow;
}

void Pruning::adjustAllPruningQueues(float adjustment) {
    // Adjust all pruning queues by a multiplicative factor
    
    // Note: Priority queues don't support easy element-wise operations
    // In real implementation, this would involve reconstructing queues
}

void Pruning::executeSinglePruning(Brain* brain, SynapseHandle synapseId) {
    // Execute pruning of a single synapse
    
    // Placeholder implementation
    // In real implementation, this would:
    // 1. Remove synapse from brain structure
    // 2. Update connectivity
    // 3. Adjust neuron states
    // 4. Update pruning statistics
    
    // For now, just mark as pruned
    pImpl->synapsePruningTimes[synapseId] = getCurrentTime();
    pImpl->currentlyPrunedCount++;
}

float Pruning::getSynapseActivity(Brain* brain, SynapseHandle synapseId) const {
    // Get activity level of a synapse
    // Placeholder implementation
    return 0.5f; // Neutral activity level
}

size_t Pruning::getCurrentSynapseCount() const {
    // Get current number of synapses
    // Placeholder implementation
    return 1000; // Example count
}

Timestamp Pruning::getCurrentTime() const {
    // Get current simulation time
    // Placeholder implementation
    return static_cast<Timestamp>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
}

} // namespace nlm
