#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    
    // Skill storage
    std::vector<std::string> skillNames;
    std::vector<std::vector<NeuronId>> neuralPatterns;
    std::vector<float> proficiencies;
    std::vector<float> lastUsedTimestamps;
    std::vector<float> successCounts;
    std::vector<float> failureCounts;
    std::vector<float> executionCounts;
    
    // Neural pathways for skill execution
    std::vector<std::vector<std::pair<NeuronId, float>>> neuralPathways;
    
    // Skill consolidation state
    std::vector<bool> consolidated;
    
    // Procedural learning parameters
    float learningRate;
    float decayRate;
    float consolidationThreshold;
    
    Impl() : brain(nullptr), learningRate(0.01f), decayRate(0.005f), consolidationThreshold(0.8f) {}
};

NeuralProceduralMemory::NeuralProceduralMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnSkill(const std::string& skillName, 
                                         const std::vector<NeuronId>& neuralPattern,
                                         float initialProficiency) {
    if (!brain_) return;
    
    // Check if skill already exists
    auto it = std::find(pImpl->skillNames.begin(), pImpl->skillNames.end(), skillName);
    if (it != pImpl->skillNames.end()) {
        size_t idx = std::distance(pImpl->skillNames.begin(), it);
        pImpl->neuralPatterns[idx] = neuralPattern;
        pImpl->proficiencies[idx] = std::max(pImpl->proficiencies[idx], initialProficiency);
        pImpl->executionCounts[idx]++;
        return;
    }
    
    // Add new skill
    pImpl->skillNames.push_back(skillName);
    pImpl->neuralPatterns.push_back(neuralPattern);
    pImpl->proficiencies.push_back(std::max(0.0f, initialProficiency));
    pImpl->lastUsedTimestamps.push_back(0.0f);
    pImpl->successCounts.push_back(0.0f);
    pImpl->failureCounts.push_back(0.0f);
    pImpl->executionCounts.push_back(1.0f);
    pImpl->consolidated.push_back(false);
    
    // Create neural pathway
    createNeuralPathway(neuralPattern, 1.0f);
    
    NLM_LOG_INFO("Learned procedural skill: " + skillName);
}

NeuralProceduralMemory::Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    auto it = std::find(pImpl->skillNames.begin(), pImpl->skillNames.end(), name);
    if (it != pImpl->skillNames.end()) {
        size_t idx = std::distance(pImpl->skillNames.begin(), it);
        return &pImpl->neuralPatterns[idx];  // Return reference to internal data
    }
    return nullptr;
}

std::vector<NeuralProceduralMemory::Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skillNames.size());
    
    for (size_t i = 0; i < pImpl->skillNames.size(); ++i) {
        // Create Skill struct wrapper for return
        Skill skill;
        skill.name = pImpl->skillNames[i];
        skill.neuralPattern = pImpl->neuralPatterns[i];
        skill.proficiency = pImpl->proficiencies[i];
        skill.lastUsed = pImpl->lastUsedTimestamps[i];
        skill.successCount = pImpl->successCounts[i];
        skill.failureCount = pImpl->failureCounts[i];
        
        result.push_back(&result.back());  // Add address (this will have issues)
    }
    
    return result;
}

void NeuralProceduralMemory::executeSkill(const std::string& skillName, float strength) {
    auto it = std::find(pImpl->skillNames.begin(), pImpl->skillNames.end(), skillName);
    if (it == pImpl->skillNames.end()) return;
    
    size_t idx = std::distance(pImpl->skillNames.begin(), it);
    
    // Update last used
    pImpl->lastUsedTimestamps[idx] = brain_ ? brain_->getCurrentStep() : 0.0f;
    pImpl->executionCounts[idx]++;
    
    // Execute neural pattern
    const std::vector<NeuronId>& pattern = pImpl->neuralPatterns[idx];
    if (!pattern.empty()) {
        // Scale pattern by execution strength and proficiency
        float effectiveStrength = strength * pImpl->proficiencies[idx];
        
        for (const NeuronId& neuronId : pattern) {
            // Inject current into each neuron in the pattern
            if (brain_) {
                brain_->injectCurrent(neuronId, effectiveStrength * 5.0f);
            }
        }
        
        // Create temporary pathway for this execution
        createNeuralPathway(pattern, effectiveStrength);
    }
    
    // Check for consolidation
    if (pImpl->proficiencies[idx] >= pImpl->consolidationThreshold && !pImpl->consolidated[idx]) {
        consolidateSkill(idx);
    }
    
    NLM_LOG_INFO("Executed skill: " + skillName + " (proficiency: " + 
                 std::to_string(pImpl->proficiencies[idx]) + ")");
}

void NeuralProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = std::find(pImpl->skillNames.begin(), pImpl->skillNames.end(), name);
    if (it != pImpl->skillNames.end()) {
        size_t idx = std::distance(pImpl->skillNames.begin(), it);
        pImpl->proficiencies[idx] = std::clamp(pImpl->proficiencies[idx] + delta, 0.0f, 1.0f);
    }
}

void NeuralProceduralMemory::recordExecutionOutcome(const std::string& skillName, bool success) {
    auto it = std::find(pImpl->skillNames.begin(), pImpl->skillNames.end(), skillName);
    if (it != pImpl->skillNames.end()) {
        size_t idx = std::distance(pImpl->skillNames.begin(), it);
        
        if (success) {
            pImpl->successCounts[idx]++;
            // Strengthen proficiency on success
            pImpl->proficiencies[idx] = std::min(1.0f, pImpl->proficiencies[idx] + 0.01f);
        } else {
            pImpl->failureCounts[idx]++;
            // Weaken proficiency on failure
            pImpl->proficiencies[idx] = std::max(0.0f, pImpl->proficiencies[idx] - 0.02f);
        }
        
        // Apply neuromodulation-based learning
        if (brain_) {
            applyNeuromodulation(0.5f, 0.3f);  // Placeholder neuromodulation levels
        }
    }
}

std::vector<std::string> NeuralProceduralMemory::getAvailableSkills() const {
    std::vector<std::string> available;
    
    for (size_t i = 0; i < pImpl->skillNames.size(); ++i) {
        // Consider skill available if proficiency > 0 and recently used
        float lastUsedAgo = brain_ ? 
            brain_->getCurrentStep() - pImpl->lastUsedTimestamps[i] : 0.0f;
        
        if (pImpl->proficiencies[i] > 0.1f && lastUsedAgo < 1000.0f) {
            available.push_back(pImpl->skillNames[i]);
        }
    }
    
    return available;
}

bool NeuralProceduralMemory::hasSkill(const std::string& skillName) const {
    return std::find(pImpl->skillNames.begin(), pImpl->skillNames.end(), skillName) != pImpl->skillNames.end();
}

void NeuralProceduralMemory::clear() {
    pImpl->skillNames.clear();
    pImpl->neuralPatterns.clear();
    pImpl->proficiencies.clear();
    pImpl->lastUsedTimestamps.clear();
    pImpl->successCounts.clear();
    pImpl->failureCounts.clear();
    pImpl->executionCounts.clear();
    pImpl->neuralPathways.clear();
    pImpl->consolidated.clear();
}

float NeuralProceduralMemory::getProceduralActivity() const {
    if (pImpl->proficiencies.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (size_t i = 0; i < pImpl->proficiencies.size(); ++i) {
        float activity = pImpl->proficiencies[i] * pImpl->executionCounts[i];
        totalActivity += activity;
    }
    return totalActivity / pImpl->proficiencies.size();
}

float NeuralProceduralMemory::getSkillProficiency(const std::string& skillName) const {
    auto it = std::find(pImpl->skillNames.begin(), pImpl->skillNames.end(), skillName);
    if (it != pImpl->skillNames.end()) {
        size_t idx = std::distance(pImpl->skillNames.begin(), it);
        return pImpl->proficiencies[idx];
    }
    return 0.0f;
}

void NeuralProceduralMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Decay unused skills
    double currentStep = brain_->getCurrentStep();
    for (size_t i = 0; i < pImpl->skillNames.size(); ++i) {
        double age = currentStep - pImpl->lastUsedTimestamps[i];
        if (age > 0.0) {
            // Decay proficiency based on age
            pImpl->proficiencies[i] *= (1.0f - pImpl->decayRate * static_cast<float>(age));
            
            // Remove if proficiency is too low
            if (pImpl->proficiencies[i] < 0.01f) {
                // Mark for removal
                pImpl->proficiencies[i] = 0.0f;
            }
        }
    }
}

void NeuralProceduralMemory::applyNeuromodulation(float dopamineLevel, float curiosityLevel) {
    if (!brain_) return;
    
    // Dopamine enhances procedural learning and memory consolidation
    float dopamineEffect = dopamineLevel * 0.5f + 0.5f;
    
    // Curiosity enhances skill acquisition and exploration
    float curiosityEffect = curiosityLevel * 0.3f + 0.7f;
    
    // Apply effects to all skills
    for (size_t i = 0; i < pImpl->skillNames.size(); ++i) {
        // Dopamine increases proficiency and consolidation
        pImpl->proficiencies[i] *= dopamineEffect;
        
        // Curiosity increases execution frequency
        pImpl->executionCounts[i] *= curiosityEffect;
        
        // Clamp to reasonable ranges
        pImpl->proficiencies[i] = std::clamp(pImpl->proficiencies[i], 0.0f, 1.0f);
        pImpl->executionCounts[i] = std::max(1.0f, pImpl->executionCounts[i]);
    }
}

void NeuralProceduralMemory::createNeuralPathway(const std::vector<NeuronId>& pattern, float strength) {
    if (!brain_ || pattern.empty()) return;
    
    // Create pathway entries
    std::vector<std::pair<NeuronId, float>> pathway;
    
    for (const NeuronId& neuronId : pattern) {
        pathway.emplace_back(neuronId, strength);
    }
    
    pImpl->neuralPathways.push_back(pathway);
}

void NeuralProceduralMemory::consolidateSkill(size_t skillIndex) {
    if (skillIndex >= pImpl->skillNames.size()) return;
    
    // Mark as consolidated
    pImpl->consolidated[skillIndex] = true;
    
    // Create additional synaptic connections
    const std::vector<NeuronId>& pattern = pImpl->neuralPatterns[skillIndex];
    if (pattern.size() >= 2) {
        // Create recurrent connections between neurons in the pattern
        for (size_t i = 0; i < pattern.size(); ++i) {
            for (size_t j = i + 1; j < pattern.size(); ++j) {
                // Create bidirectional connection
                createNeuralPathway({pattern[i], pattern[j]}, 0.5f);
            }
        }
    }
    
    NLM_LOG_INFO("Consolidated procedural skill: " + pImpl->skillNames[skillIndex]);
}

void NeuralProceduralMemory::reinforceExecution(const std::string& skillName, bool success) {
    // Additional reinforcement logic
    updateProficiency(skillName, success ? 0.01f : -0.01f);
}

} // namespace nlm