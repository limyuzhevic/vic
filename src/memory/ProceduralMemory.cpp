#include "ProceduralMemory.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <algorithm>

namespace nlm {

struct ProceduralMemory::Impl {
    Brain* brain;
    
    // Skills storage
    std::vector<Skill> skills;
    
    // Skill chaining
    std::unordered_map<std::string, std::vector<std::string>> skillChains;
    std::unordered_map<std::string, float> chainTriggers;
    
    // Learning parameters
    std::unordered_map<std::string, float> automaticity;
    std::unordered_map<std::string, float> learningRates;
    std::unordered_map<std::string, SimulationStep> lastPractice;
    std::unordered_map<std::string, float> predictedValues;
    
    // Context patterns
    std::unordered_map<std::string, std::vector<NeuronId>> contextPatterns;
    
    // Execution timing
    std::unordered_map<std::string, SimulationStep> lastExecution;
    std::unordered_map<std::string, float> minIntervals;
    
    // Outcome history for learning
    std::unordered_map<std::string, std::vector<std::pair<SimulationStep, bool>>> outcomeHistory;
    
    Impl() : brain(nullptr) {}
    
    // Helper methods
    float calculateLearningRate(const Skill& skill, bool success) const {
        float baseRate = 0.1f;
        if (success) {
            baseRate *= skill.proficiency + 0.1f;  // Better skills learn faster
        } else {
            baseRate *= 0.5f;  // Slower from errors
        }
        return std::clamp(baseRate, 0.01f, 0.5f);
    }
    
    void updateAutomaticity(const std::string& skillName, float delta) {
        automaticity[skillName] = std::clamp(
            automaticity[skillName] + delta, 0.0f, 1.0f);
    }
    
    bool matchesContext(const std::vector<float>& context, const std::vector<NeuronId>& pattern) const {
        if (context.size() != pattern.size()) return false;
        
        for (size_t i = 0; i < context.size(); ++i) {
            // For now, simple match on neuron IDs converted to float
            float neuronValue = static_cast<float>(pattern[i].getId()) / 1000.0f;
            if (std::abs(context[i] - neuronValue) > 0.1f) {
                return false;
            }
        }
        return true;
    }
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Check if skill exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            pImpl->learningRates[name] = 0.1f; // Reset learning rate
            pImpl->lastPractice[name] = pImpl->brain->getClock()->getStep();
            return;
        }
    }
    
    // Create new skill
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.0f; // Start with zero proficiency
    pImpl->skills.push_back(skill);
    
    // Initialize learning parameters
    pImpl->automaticity[name] = 0.0f;
    pImpl->learningRates[name] = 0.1f;
    pImpl->predictedValues[name] = 0.0f;
    pImpl->minIntervals[name] = 100.0f; // Default minimum interval
}

const Skill* ProceduralMemory::getSkill(const std::string& name) const {
    for (const auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<const Skill*> ProceduralMemory::getAllSkills() const {
    std::vector<const Skill*> result;
    for (const auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta, bool success) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            float oldProficiency = skill.proficiency;
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            
            // Update learning rate based on outcome
            float learningRate = pImpl->calculateLearningRate(skill, success);
            pImpl->learningRates[name] = learningRate;
            
            // Update automaticity (more proficiency = more automatic)
            pImpl->updateAutomaticity(name, skill.proficiency - oldProficiency);
            
            // Record outcome
            pImpl->outcomeHistory[name].push_back(
                std::make_pair(pImpl->brain->getClock()->getStep(), success));
            
            // Update predicted value
            float prediction = success ? 1.0f : 0.0f;
            pImpl->predictedValues[name] = 
                pImpl->predictedValues[name] * 0.9f + prediction * 0.1f;
            
            pImpl->lastPractice[name] = pImpl->brain->getClock()->getStep();
            break;
        }
    }
}

bool ProceduralMemory::executeSkill(const std::string& name, const std::vector<float>& context) {
    // Check if skill exists
    const Skill* skill = getSkill(name);
    if (!skill) {
        return false;
    }
    
    // Check cooldown
    if (pImpl->brain) {
        auto clock = pImpl->brain->getClock();
        SimulationStep currentStep = clock->getStep();
        SimulationStep lastExec = pImpl->lastExecution[name];
        
        if (currentStep - lastExec < pImpl->minIntervals[name]) {
            return false;
        }
    }
    
    // Check context match if context patterns are defined
    auto contextIt = pImpl->contextPatterns.find(name);
    if (contextIt != pImpl->contextPatterns.end()) {
        const std::vector<NeuronId>& requiredPattern = contextIt->second;
        
        if (!pImpl->matchesContext(context, requiredPattern)) {
            return false;
        }
    }
    
    // Execute skill
    if (pImpl->brain) {
        pImpl->lastExecution[name] = pImpl->brain->getClock()->getStep();
    }
    
    return true;
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillChains.clear();
    pImpl->chainTriggers.clear();
    pImpl->automaticity.clear();
    pImpl->learningRates.clear();
    pImpl->lastPractice.clear();
    pImpl->predictedValues.clear();
    pImpl->contextPatterns.clear();
    pImpl->lastExecution.clear();
    pImpl->minIntervals.clear();
    pImpl->outcomeHistory.clear();
}

void ProceduralMemory::consolidate() {
    auto clock = pImpl->brain->getClock();
    SimulationStep currentStep = clock->getStep();
    
    for (auto& skill : pImpl->skills) {
        // Age-based consolidation
        auto lastPracticeIt = pImpl->lastPractice.find(skill.name);
        if (lastPracticeIt != pImpl->lastPractice.end()) {
            float age = static_cast<float>(currentStep - lastPracticeIt->second);
            
            // Consolidate well-practiced skills
            if (skill.proficiency > 0.8f && age > 100.0f) {
                // Skills become more automatic over time
                float consolidationBoost = std::min(0.1f, age * 0.001f);
                skill.proficiency = std::min(1.0f, skill.proficiency + consolidationBoost);
                pImpl->updateAutomaticity(skill.name, consolidationBoost);
            }
        }
        
        // Inter-skill interference management
        float interference = 0.0f;
        for (const auto& other : pImpl->skills) {
            if (other.name != skill.name) {
                // Check for pattern similarity
                if (!skill.neuralPattern.empty() && !other.neuralPattern.empty()) {
                    // Simple similarity check (dot product)
                    float similarity = 0.0f;
                    size_t minSize = std::min(skill.neuralPattern.size(), other.neuralPattern.size());
                    
                    for (size_t i = 0; i < minSize; ++i) {
                        float val1 = static_cast<float>(skill.neuralPattern[i].getId()) / 1000.0f;
                        float val2 = static_cast<float>(other.neuralPattern[i].getId()) / 1000.0f;
                        similarity += val1 * val2;
                    }
                    
                    if (similarity > 0.5f) {
                        interference += similarity * 0.01f;
                    }
                }
            }
        }
        
        if (interference > 0.0f) {
            skill.proficiency = std::max(0.0f, skill.proficiency - interference * 0.1f);
        }
    }
}

std::vector<const Skill*> ProceduralMemory::getAvailableSkills(const std::vector<float>& context) const {
    std::vector<const Skill*> result;
    
    for (const auto& skill : pImpl->skills) {
        // Check if skill has context requirement
        auto contextIt = pImpl->contextPatterns.find(skill.name);
        
        bool contextMatches = true;
        if (contextIt != pImpl->contextPatterns.end()) {
            // Check context match
            if (!pImpl->matchesContext(context, contextIt->second)) {
                contextMatches = false;
            }
        }
        
        // Check proficiency threshold
        bool proficiencyThreshold = skill.proficiency > 0.5f; // Only highly proficient skills
        
        if (contextMatches && proficiencyThreshold) {
            result.push_back(&skill);
        }
    }
    
    return result;
}

void ProceduralMemory::setSkillContext(const std::string& skillName, const std::vector<NeuronId>& context) {
    pImpl->contextPatterns[skillName] = context;
}

void ProceduralMemory::clearSkillContext(const std::string& skillName) {
    auto it = pImpl->contextPatterns.find(skillName);
    if (it != pImpl->contextPatterns.end()) {
        pImpl->contextPatterns.erase(it);
    }
}

// Chain skills together
void ProceduralMemory::chainSkills(const std::string& precursor, const std::string& successor, float triggerThreshold) {
    // Add to chain
    pImpl->skillChains[precursor].push_back(successor);
    pImpl->chainTriggers[precursor] = triggerThreshold;
}

// Get skill chains
const std::vector<std::string>& ProceduralMemory::getSkillChain(const std::string& skillName) const {
    static const std::vector<std::string> emptyChain;
    auto it = pImpl->skillChains.find(skillName);
    if (it != pImpl->skillChains.end()) {
        return it->second;
    }
    return emptyChain;
}

// Get automaticity level for skill
float ProceduralMemory::getAutomaticity(const std::string& skillName) const {
    auto it = pImpl->automaticity.find(skillName);
    if (it != pImpl->automaticity.end()) {
        return it->second;
    }
    return 0.0f;
}

// Get learning rate for skill
float ProceduralMemory::getLearningRate(const std::string& skillName) const {
    auto it = pImpl->learningRates.find(skillName);
    if (it != pImpl->learningRates.end()) {
        return it->second;
    }
    return 0.0f;
}

// Get predicted value (expected reward) for skill
float ProceduralMemory::getPredictedValue(const std::string& skillName) const {
    auto it = pImpl->predictedValues.find(skillName);
    if (it != pImpl->predictedValues.end()) {
        return it->second;
    }
    return 0.0f;
}

// Get time since last practice (for consolidation)
SimulationStep ProceduralMemory::getTimeSinceLastPractice(const std::string& skillName) const {
    auto it = pImpl->lastPractice.find(skillName);
    if (it != pImpl->lastPractice.end()) {
        return pImpl->brain->getClock()->getStep() - it->second;
    }
    return 0;
}

void ProceduralMemory::learnFromOutcome(const std::string& skillName, const std::vector<float>& outcome, bool success) {
    // Update proficiency based on outcome
    float error = success ? 0.0f : 1.0f;
    
    // Learn from error
    updateProficiency(skillName, error * 0.05f, success);
    
    // Update predicted value
    pImpl->predictedValues[skillName] = 
        pImpl->predictedValues[skillName] * 0.9f + 
        (success ? 1.0f : 0.0f) * 0.1f;
}

} // namespace nlm