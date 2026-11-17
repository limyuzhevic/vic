// Agent brain utility functions implementation
// Extracted from AgentBrain.cpp for better maintainability

#include "AgentBrainUtilities.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

namespace nlm {

// Motor group implementation
void AgentMotorGroups::distributeMotorNeurons(
    const std::vector<Neuron*>& motorNeurons,
    DistributionStrategy strategy) {
    
    clearGroups();
    size_t groupCount = static_cast<size_t>(MotorGroup::Count);
    
    if (motorNeurons.empty()) return;
    
    switch (strategy) {
        case DistributionStrategy::RoundRobin:
            for (size_t i = 0; i < motorNeurons.size(); ++i) {
                MotorGroup group = static_cast<MotorGroup>(i % groupCount);
                motorGroups_[static_cast<size_t>(group)].push_back(motorNeurons[i]);
            }
            break;
            
        case DistributionStrategy::Weighted:
            // Simple weighted distribution based on action frequency
            std::vector<float> weights = {1.2f, 1.0f, 0.8f, 0.8f, 1.5f, 0.5f}; // Heuristic weights
            float totalWeight = std::accumulate(weights.begin(), weights.end(), 0.0f);
            
            std::random_device rd;
            std::mt19937 gen(rd());
            std::discrete_distribution<> dist(weights.begin(), weights.end());
            
            for (Neuron* neuron : motorNeurons) {
                MotorGroup group = static_cast<MotorGroup>(dist(gen));
                motorGroups_[static_cast<size_t>(group)].push_back(neuron);
            }
            break;
            
        case DistributionStrategy::Balanced:
            // Distribute evenly across all groups
            for (size_t i = 0; i < motorNeurons.size(); ++i) {
                MotorGroup group = static_cast<MotorGroup>(i % groupCount);
                motorGroups_[static_cast<size_t>(group)].push_back(motorNeurons[i]);
            }
            break;
            
        case DistributionStrategy::Random:
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, groupCount - 1);
            
            for (Neuron* neuron : motorNeurons) {
                MotorGroup group = static_cast<MotorGroup>(dist(gen));
                motorGroups_[static_cast<size_t>(group)].push_back(neuron);
            }
            break;
    }
    
    // Build group index
    for (size_t i = 0; i < groupCount; ++i) {
        groupIndex_[static_cast<MotorGroup>(i)] = i;
    }
    
    initialized_ = true;
}

const std::vector<Neuron*>& AgentMotorGroups::getMotorGroup(MotorGroup group) const {
    size_t groupIndex = static_cast<size_t>(group);
    if (groupIndex < motorGroups_.size()) {
        return motorGroups_[groupIndex];
    }
    static const std::vector<Neuron*> empty;
    return empty;
}

const std::vector<std::vector<Neuron*>>& AgentMotorGroups::getAllMotorGroups() const {
    return motorGroups_;
}

MotorGroups::MotorGroup AgentMotorGroups::motorCommandToGroup(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return MotorGroup::Forward;
        case MotorCommand::MoveBackward: return MotorGroup::Backward;
        case MotorCommand::TurnLeft: return MotorGroup::TurnLeft;
        case MotorCommand::TurnRight: return MotorGroup::TurnRight;
        case MotorCommand::Interact: return MotorGroup::Interact;
        case MotorCommand::Wait: return MotorGroup::Wait;
        default: return MotorGroup::Wait;
    }
}

AgentMotorGroups::GroupStats AgentMotorGroups::getGroupStats(MotorGroup group) const {
    GroupStats stats;
    const auto& neurons = getMotorGroup(group);
    
    stats.neuronCount = neurons.size();
    
    if (!neurons.empty()) {
        float totalRate = 0.0f;
        float totalActivity = 0.0f;
        float maxRate = 0.0f;
        
        for (const Neuron* neuron : neurons) {
            if (neuron) {
                float rate = neuron->getFiringRate();
                totalRate += rate;
                totalActivity += std::max(0.0f, rate - 10.0f); // Activity above 10 Hz
                if (rate > maxRate) maxRate = rate;
            }
        }
        
        stats.averageFiringRate = totalRate / neurons.size();
        stats.totalActivity = totalActivity;
        stats.dominanceScore = maxRate / (stats.averageFiringRate + 0.1f);
    }
    
    return stats;
}

std::vector<AgentMotorGroups::GroupStats> AgentMotorGroups::getAllGroupStats() const {
    std::vector<GroupStats> allStats;
    allStats.reserve(motorGroups_.size());
    
    for (const auto& group : motorGroups_) {
        if (!group.empty()) {
            GroupStats stats;
            size_t neuronCount = group.size();
            
            float totalRate = 0.0f;
            float totalActivity = 0.0f;
            
            for (const Neuron* neuron : group) {
                if (neuron) {
                    float rate = neuron->getFiringRate();
                    totalRate += rate;
                    totalActivity += std::max(0.0f, rate - 10.0f);
                }
            }
            
            stats.neuronCount = neuronCount;
            stats.averageFiringRate = totalRate / neuronCount;
            stats.totalActivity = totalActivity;
            stats.dominanceScore = (totalRate / neuronCount) / 10.0f; // Normalized
            
            allStats.push_back(stats);
        }
    }
    
    return allStats;
}

void AgentMotorGroups::clearGroups() {
    for (auto& group : motorGroups_) {
        group.clear();
    }
    groupIndex_.clear();
    initialized_ = false;
}

size_t AgentMotorGroups::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& group : motorGroups_) {
        total += group.size();
    }
    return total;
}

bool AgentMotorGroups::isInitialized() const {
    return initialized_ && !motorGroups_.empty();
}

// Sensory groups implementation
void AgentSensoryGroups::distributeSensoryNeurons(
    const std::vector<Neuron*>& sensoryNeurons,
    const std::string& distribution) {
    
    clearGroups();
    size_t groupCount = static_cast<size_t>(SensoryGroup::Count);
    
    if (sensoryNeurons.empty()) return;
    
    if (distribution == "balanced") {
        // Distribute evenly across sensory modalities
        for (size_t i = 0; i < sensoryNeurons.size(); ++i) {
            SensoryGroup group = static_cast<SensoryGroup>(i % groupCount);
            sensoryGroups_[static_cast<size_t>(group)].push_back(sensoryNeurons[i]);
        }
    } else if (distribution == "vision-heavy") {
        // Give more weight to vision neurons
        std::vector<float> weights = {2.0f, 0.8f, 0.6f, 0.4f}; // Vision, Touch, Internal, Proprioception
        float totalWeight = std::accumulate(weights.begin(), weights.end(), 0.0f);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<> dist(weights.begin(), weights.end());
        
        for (Neuron* neuron : sensoryNeurons) {
            SensoryGroup group = static_cast<SensoryGroup>(dist(gen));
            sensoryGroups_[static_cast<size_t>(group)].push_back(neuron);
        }
    } else {
        // Default balanced distribution
        for (size_t i = 0; i < sensoryNeurons.size(); ++i) {
            SensoryGroup group = static_cast<SensoryGroup>(i % groupCount);
            sensoryGroups_[static_cast<size_t>(group)].push_back(sensoryNeurons[i]);
        }
    }
    
    // Build group index
    for (size_t i = 0; i < groupCount; ++i) {
        groupIndex_[static_cast<SensoryGroup>(i)] = i;
    }
    
    initialized_ = true;
}

const std::vector<Neuron*>& AgentSensoryGroups::getSensoryGroup(SensoryGroup group) const {
    size_t groupIndex = static_cast<size_t>(group);
    if (groupIndex < sensoryGroups_.size()) {
        return sensoryGroups_[groupIndex];
    }
    static const std::vector<Neuron*> empty;
    return empty;
}

const std::vector<std::vector<Neuron*>>& AgentSensoryGroups::getAllSensoryGroups() const {
    return sensoryGroups_;
}

const char* AgentSensoryGroups::sensoryGroupName(SensoryGroup group) {
    switch (group) {
        case SensoryGroup::Vision: return "Vision";
        case SensoryGroup::Touch: return "Touch";
        case SensoryGroup::Internal: return "Internal";
        case SensoryGroup::Proprioception: return "Proprioception";
        default: return "Unknown";
    }
}

AgentSensoryGroups::GroupCharacteristics AgentSensoryGroups::getGroupCharacteristics(SensoryGroup group) const {
    GroupCharacteristics chars;
    const auto& neurons = getSensoryGroup(group);
    
    if (!neurons.empty()) {
        float totalSensitivity = 0.0f;
        float totalTuning = 0.0f;
        float totalBaseline = 0.0f;
        
        for (const Neuron* neuron : neurons) {
            if (neuron) {
                float rate = neuron->getFiringRate();
                // Simple heuristics for characteristics
                totalSensitivity += 1.0f / (1.0f + std::abs(rate - 5.0f)); // Peak sensitivity around 5 Hz
                totalTuning += std::max(0.0f, rate - 2.0f); // Tuning strength
                totalBaseline += std::min(2.0f, rate); // Baseline activity
            }
        }
        
        size_t count = neurons.size();
        chars.sensitivity = totalSensitivity / count;
        chars.tuningWidth = 10.0f - chars.sensitivity; // Inverse relationship
        chars.baselineActivity = totalBaseline / count;
        chars.dynamicRange = chars.sensitivity * chars.tuningWidth;
    }
    
    return chars;
}

std::vector<AgentSensoryGroups::GroupCharacteristics> AgentSensoryGroups::getAllCharacteristics() const {
    std::vector<GroupCharacteristics> allChars;
    allChars.reserve(sensoryGroups_.size());
    
    for (const auto& group : sensoryGroups_) {
        if (!group.empty()) {
            GroupCharacteristics chars;
            size_t count = group.size();
            
            float totalSensitivity = 0.0f;
            float totalTuning = 0.0f;
            float totalBaseline = 0.0f;
            
            for (const Neuron* neuron : group) {
                if (neuron) {
                    float rate = neuron->getFiringRate();
                    totalSensitivity += 1.0f / (1.0f + std::abs(rate - 5.0f));
                    totalTuning += std::max(0.0f, rate - 2.0f);
                    totalBaseline += std::min(2.0f, rate);
                }
            }
            
            chars.sensitivity = totalSensitivity / count;
            chars.tuningWidth = 10.0f - chars.sensitivity;
            chars.baselineActivity = totalBaseline / count;
            chars.dynamicRange = chars.sensitivity * chars.tuningWidth;
            
            allChars.push_back(chars);
        }
    }
    
    return allChars;
}

void AgentSensoryGroups::clearGroups() {
    for (auto& group : sensoryGroups_) {
        group.clear();
    }
    groupIndex_.clear();
    initialized_ = false;
}

size_t AgentSensoryGroups::getTotalSensoryCount() const {
    size_t total = 0;
    for (const auto& group : sensoryGroups_) {
        total += group.size();
    }
    return total;
}

bool AgentSensoryGroups::isInitialized() const {
    return initialized_ && !sensoryGroups_.empty();
}

// Lateral inhibition implementation
void AgentLateralInhibition::configure(const InhibitionParams& params) {
    params_ = params;
    inhibitionState_.clear();
    active_ = false;
    decayTimer_ = 0.0f;
}

void AgentLateralInhibition::applyInhibition(
    const std::vector<float>& groupActivities,
    const std::vector<AgentMotorGroups::MotorGroup>& groupOrder) {
    
    if (groupActivities.empty() || groupOrder.empty()) return;
    
    // Determine if inhibition should be active based on conditions
    bool shouldInhibit = false;
    
    if (groupActivities.size() < 5) return; // Need at least 5 groups for meaningful inhibition
    
    // Find the winning group (highest activity)
    float maxActivity = *std::max_element(groupActivities.begin(), groupActivities.end());
    size_t winnerIndex = std::max_element(groupActivities.begin(), groupActivities.end()) - groupActivities.begin();
    
    // Check if winner has clear dominance (at least 2x stronger than next)
    if (groupActivities.size() > 1) {
        auto secondMax = *std::max_element(groupActivities.begin(), groupActivities.end() - 1);
        if (maxActivity < params_.threshold * 2.0f) {
            shouldInhibit = false;
        } else {
            shouldInhibit = true;
        }
    }
    
    // Apply inhibition to all groups except winner
    inhibitionState_.resize(groupActivities.size(), 0.0f);
    
    if (shouldInhibit && winnerIndex < inhibitionState_.size()) {
        active_ = true;
        
        for (size_t i = 0; i < inhibitionState_.size(); ++i) {
            if (i != winnerIndex) {
                // Inhibition strength based on winner's dominance
                float dominance = maxActivity / (secondMax + 0.01f);
                inhibitionState_[i] = params_.inhibitionStrength * dominance * 
                                    std::min(1.0f, params_.noveltySensitivity);
            }
        }
    } else {
        active_ = false;
        // Decay existing inhibition
        for (float& inhibition : inhibitionState_) {
            inhibition *= (1.0f - params_.recoveryTime);
        }
    }
    
    decayTimer_ += 0.001f; // Small timestep
}

const std::vector<float>& AgentLateralInhibition::getInhibitionState() const {
    return inhibitionState_;
}

void AgentLateralInhibition::resetInhibition() {
    inhibitionState_.clear();
    active_ = false;
    decayTimer_ = 0.0f;
}

bool AgentLateralInhibition::isInhibitionActive() const {
    return active_ && !inhibitionState_.empty();
}

float AgentLateralInhibition::getInhibitionStrength(size_t groupIndex) const {
    if (groupIndex < inhibitionState_.size()) {
        return inhibitionState_[groupIndex];
    }
    return 0.0f;
}

// Internal neurons implementation
void AgentInternalNeurons::classifyInternalNeurons(const std::vector<Neuron*>& internalNeurons) {
    clearGroups();
    size_t groupCount = static_cast<size_t>(InternalNeuronType::Count);
    
    if (internalNeurons.empty()) return;
    
    // Simple classification based on neuron properties
    // In a real implementation, this would be more sophisticated
    for (size_t i = 0; i < internalNeurons.size(); ++i) {
        InternalNeuronType type;
        
        // Heuristic classification based on neuron index
        size_t groupIndex = i % groupCount;
        type = static_cast<InternalNeuronType>(groupIndex);
        
        internalGroups_[static_cast<size_t>(type)].push_back(internalNeurons[i]);
    }
    
    // Build group index
    for (size_t i = 0; i < groupCount; ++i) {
        groupIndex_[static_cast<InternalNeuronType>(i)] = i;
    }
    
    initialized_ = true;
}

const std::vector<Neuron*>& AgentInternalNeurons::getInternalGroup(InternalNeuronType type) const {
    size_t groupIndex = static_cast<size_t>(type);
    if (groupIndex < internalGroups_.size()) {
        return internalGroups_[groupIndex];
    }
    static const std::vector<Neuron*> empty;
    return empty;
}

const std::vector<std::vector<Neuron*>>& AgentInternalNeurons::getAllInternalGroups() const {
    return internalGroups_;
}

AgentInternalNeurons::GroupDynamics AgentInternalNeurons::getGroupDynamics(InternalNeuronType type) const {
    GroupDynamics dynamics;
    const auto& neurons = getInternalGroup(type);
    
    if (!neurons.empty()) {
        float totalExcitation = 0.0f;
        float totalAdaptation = 0.0f;
        float totalRefractory = 0.0f;
        float totalPlasticity = 0.0f;
        
        for (const Neuron* neuron : neurons) {
            if (neuron) {
                float rate = neuron->getFiringRate();
                // Simple heuristics based on firing rate
                totalExcitation += std::max(0.0f, rate - 10.0f) / 10.0f;
                totalAdaptation += std::min(1.0f, rate / 50.0f); // Adaptation increases with rate
                totalRefractory += neuron->getRefractoryPeriod();
                totalPlasticity += 1.0f - std::min(1.0f, rate / 100.0f); // Plasticity decreases with rate
            }
        }
        
        size_t count = neurons.size();
        dynamics.excitationLevel = totalExcitation / count;
        dynamics.adaptationRate = totalAdaptation / count;
        dynamics.refractoryPeriod = totalRefractory / count;
        dynamics.plasticityRate = totalPlasticity / count;
    }
    
    return dynamics;
}

std::vector<AgentInternalNeurons::GroupDynamics> AgentInternalNeurons::getAllDynamics() const {
    std::vector<GroupDynamics> allDynamics;
    allDynamics.reserve(internalGroups_.size());
    
    for (const auto& group : internalGroups_) {
        if (!group.empty()) {
            GroupDynamics dynamics;
            size_t count = group.size();
            
            float totalExcitation = 0.0f;
            float totalAdaptation = 0.0f;
            float totalRefractory = 0.0f;
            float totalPlasticity = 0.0f;
            
            for (const Neuron* neuron : group) {
                if (neuron) {
                    float rate = neuron->getFiringRate();
                    totalExcitation += std::max(0.0f, rate - 10.0f) / 10.0f;
                    totalAdaptation += std::min(1.0f, rate / 50.0f);
                    totalRefractory += neuron->getRefractoryPeriod();
                    totalPlasticity += 1.0f - std::min(1.0f, rate / 100.0f);
                }
            }
            
            dynamics.excitationLevel = totalExcitation / count;
            dynamics.adaptationRate = totalAdaptation / count;
            dynamics.refractoryPeriod = totalRefractory / count;
            dynamics.plasticityRate = totalPlasticity / count;
            
            allDynamics.push_back(dynamics);
        }
    }
    
    return allDynamics;
}

void AgentInternalNeurons::clearGroups() {
    for (auto& group : internalGroups_) {
        group.clear();
    }
    groupIndex_.clear();
    initialized_ = false;
}

size_t AgentInternalNeurons::getTotalInternalCount() const {
    size_t total = 0;
    for (const auto& group : internalGroups_) {
        total += group.size();
    }
    return total;
}

bool AgentInternalNeurons::isInitialized() const {
    return initialized_ && !internalGroups_.empty();
}

// Agent brain systems implementation
void AgentBrainSystems::initialize(
    const std::vector<Neuron*>& motorNeurons,
    const std::vector<Neuron*>& sensoryNeurons,
    const std::vector<Neuron*>& internalNeurons) {
    
    motorGroups_.distributeMotorNeurons(motorNeurons);
    sensoryGroups_.distributeSensoryNeurons(sensoryNeurons, "balanced");
    internalGroups_.classifyInternalNeurons(internalNeurons);
    
    ready_ = motorGroups_.isInitialized() && 
             sensoryGroups_.isInitialized() && 
             internalGroups_.isInitialized();
}

void AgentBrainSystems::update(float timestep) {
    // Update lateral inhibition based on current system state
    // This would be called from AgentBrain with real-time data
    if (ready_) {
        // In a real implementation, this would get current group activities
        // and update inhibition accordingly
    }
}

AgentBrainSystems::SystemStatus AgentBrainSystems::getStatus() const {
    SystemStatus status;
    status.motorInitialized = motorGroups_.isInitialized();
    status.sensoryInitialized = sensoryGroups_.isInitialized();
    status.internalInitialized = internalGroups_.isInitialized();
    status.lateralInhibitionActive = false; // Would need to check from lateral inhibition
    status.overallActivity = 0.5f; // Would need to calculate from all systems
    status.systemStability = 0.8f; // Would need to calculate based on system health
    
    return status;
}

AgentBrainSystems::SystemParameters AgentBrainSystems::getParameters() const {
    SystemParameters params;
    // Return current system parameters
    params.motorDistribution = AgentMotorGroups::DistributionStrategy::RoundRobin;
    params.sensoryDistribution = "balanced";
    params.inhibitionParams = AgentLateralInhibition::InhibitionParams();
    params.updateInterval = 0.001f;
    
    return params;
}

void AgentBrainSystems::configure(const SystemParameters& params) {
    // Configure system parameters
    // This would update the various system components
}

void AgentBrainSystems::reset() {
    motorGroups_.clearGroups();
    sensoryGroups_.clearGroups();
    internalGroups_.clearGroups();
    ready_ = false;
}

bool AgentBrainSystems::isReady() const {
    return ready_;
}

} // namespace nlm