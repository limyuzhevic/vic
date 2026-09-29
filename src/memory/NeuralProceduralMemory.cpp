#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    
    // Skill storage
    std::unordered_map<std::string, Skill> skills;
    std::unordered_map<std::string, float> skillProficiency;
    std::unordered_map<std::string, std::vector<NeuronId>> skillNeuralPatterns;
    std::unordered_map<std::string, std::vector<NeuronId>> skillExecutingNeurons;
    
    // Motor mapping for skills
    std::unordered_map<std::string, std::vector<ActionType>> skillActions;
    
    // Execution state
    std::unordered_map<std::string, float> skillExecutionStrength;
    std::unordered_map<std::string, SimulationStep> lastExecutionTime;
    
    // Skill statistics
    std::unordered_map<std::string, size_t> executionCounts;
    std::unordered_map<std::string, float> totalPerformance;
    
    Impl() : brain(nullptr) {}
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
    
    // Initialize motor mapping for common skills
    pImpl->skillActions["move_forward"] = {ActionType::MoveForward};
    pImpl->skillActions["move_backward"] = {ActionType::MoveBackward};
    pImpl->skillActions["turn_left"] = {ActionType::TurnLeft};
    pImpl->skillActions["turn_right"] = {ActionType::TurnRight};
    pImpl->skillActions["interact"] = {ActionType::Interact};
    pImpl->skillActions["wait"] = {ActionType::Wait};
    pImpl->skillActions["look_left"] = {ActionType::LookLeft};
    pImpl->skillActions["look_right"] = {ActionType::LookRight};
    
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& neuralPattern, float initialProficiency) {
    if (!brain_ || neuralPattern.empty()) return;
    
    Skill skill;
    skill.name = name;
    skill.neuralPattern = neuralPattern;
    skill.proficiency = initialProficiency;
    
    pImpl->skills[name] = skill;
    pImpl->skillProficiency[name] = initialProficiency;
    pImpl->skillNeuralPatterns[name] = neuralPattern;
    pImpl->skillExecutionStrength[name] = 0.0f;
    pImpl->lastExecutionTime[name] = 0;
    pImpl->executionCounts[name] = 0;
    pImpl->totalPerformance[name] = 0.0f;
    
    // Create motor neuron groups for this skill
    createSkillMotorMapping(name, neuralPattern);
    
    NLM_LOG_INFO("Learned skill: " + name + " with pattern size " + std::to_string(neuralPattern.size()) + 
                " proficiency " + std::to_string(initialProficiency));
}

Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skills.size());
    
    for (auto& kv : pImpl->skills) {
        result.push_back(&kv.second);
    }
    
    return result;
}

void NeuralProceduralMemory::updateProficiency(const std::string& name, float delta, NeuronId performingNeuron) {
    auto skillIt = pImpl->skills.find(name);
    if (skillIt == pImpl->skills.end()) return;
    
    Skill& skill = skillIt->second;
    
    // Update proficiency
    skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 100.0f);
    pImpl->skillProficiency[name] = skill.proficiency;
    
    // Record performance
    pImpl->totalPerformance[name] += delta;
    pImpl->executionCounts[name]++;
    
    // Update neural pattern based on successful execution
    if (delta > 0.1f) {
        // Strengthen successful patterns
        strengthenSkillPattern(name, performingNeuron, 0.1f);
    }
    
    // Smooth the execution strength
    pImpl->skillExecutionStrength[name] = 0.9f * pImpl->skillExecutionStrength[name] + 0.1f * skill.proficiency;
    
    // Update last execution time
    pImpl->lastExecutionTime[name] = pImpl->brain ? pImpl->brain->getCurrentStep() : 0;
    
    NLM_LOG_INFO("Updated skill proficiency: " + name + " new proficiency " + std::to_string(skill.proficiency));
}

void NeuralProceduralMemory::executeSkill(const std::string& name, float executionStrength) {
    auto skillIt = pImpl->skills.find(name);
    if (skillIt == pImpl->skills.end()) {
        NLM_LOG_WARNING("Cannot execute unknown skill: " + name);
        return;
    }
    
    const Skill& skill = skillIt->second;
    
    // Check if skill is learned enough
    float currentExecutionStrength = pImpl->skillExecutionStrength[name];
    if (currentExecutionStrength < 0.3f * skill.proficiency) {
        NLM_LOG_INFO("Skill not ready yet: " + name + " proficiency " + std::to_string(skill.proficiency));
        return;
    }
    
    // Get actions for this skill
    auto actionsIt = pImpl->skillActions.find(name);
    if (actionsIt == pImpl->skillActions.end()) {
        NLM_LOG_INFO("No motor mapping for skill: " + name);
        return;
    }
    
    const std::vector<ActionType>& actions = actionsIt->second;
    
    // Execute the skill
    for (ActionType action : actions) {
        executeAction(action, executionStrength);
    }
    
    // Update execution statistics
    pImpl->executionCounts[name]++;
    pImpl->skillExecutionStrength[name] = executionStrength;
    
    NLM_LOG_INFO("Executed skill: " + name + " with strength " + std::to_string(executionStrength) + " actions " + std::to_string(actions.size()));
}

void NeuralProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillProficiency.clear();
    pImpl->skillNeuralPatterns.clear();
    pImpl->skillExecutingNeurons.clear();
    pImpl->skillActions.clear();
    pImpl->skillExecutionStrength.clear();
    pImpl->lastExecutionTime.clear();
    pImpl->executionCounts.clear();
    pImpl->totalPerformance.clear();
}

float NeuralProceduralMemory::getSkillProficiency(const std::string& name) const {
    auto it = pImpl->skillProficiency.find(name);
    if (it != pImpl->skillProficiency.end()) {
        return it->second;
    }
    return 0.0f;
}

Skill* NeuralProceduralMemory::getSkillByActivation(const std::vector<NeuronId>& activeNeurons, float threshold) {
    // Find skills whose neural patterns match active neurons
    for (auto& kv : pImpl->skillNeuralPatterns) {
        const std::vector<NeuronId>& pattern = kv.second;
        
        // Compute pattern match score
        float matchScore = 0.0f;
        size_t matchedNeurons = 0;
        
        for (const NeuronId& patternNeuron : pattern) {
            for (const NeuronId& activeNeuron : activeNeurons) {
                if (patternNeuron == activeNeuron) {
                    matchScore += 1.0f;
                    matchedNeurons++;
                    break;
                }
            }
        }
        
        if (matchScore / pattern.size() >= threshold) {
            return &pImpl->skills[kv.first];
        }
    }
    
    return nullptr;
}

void NeuralProceduralMemory::createSkillMotorMapping(const std::string& name, const std::vector<NeuronId>& neuralPattern) {
    if (!brain_ || neuralPattern.empty()) return;
    
    // Find motor neurons that can be associated with this skill
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                // Check if this population might be relevant for the skill
                // In a full implementation, we would analyze motor output patterns
                // For now, just log the potential mapping
                NLM_LOG_INFO("Found motor population " + std::to_string(pop->getId().index()) + 
                           " potentially relevant for skill: " + name);
            }
        }
    }
}

void NeuralProceduralMemory::strengthenSkillPattern(const std::string& name, NeuronId performingNeuron, float strength) {
    // Find and strengthen the neural pattern for this skill
    auto patternIt = pImpl->skillNeuralPatterns.find(name);
    if (patternIt == pImpl->skillNeuralPatterns.end()) return;
    
    std::vector<NeuronId>& pattern = patternIt->second;
    
    // Find the neuron in the pattern and increase its contribution
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == performingNeuron) {
            // This neuron was successful, strengthen its role
            // In a full implementation, this would involve synaptic strengthening
            break;
        }
    }
}

void NeuralProceduralMemory::executeAction(ActionType action, float strength) {
    if (!brain_) return;
    
    // Create an action based on the action type
    // In a full implementation, this would activate the appropriate motor neurons
    
    // For now, just log the action execution
    NLM_LOG_INFO("Executing action: " + std::to_string(static_cast<int>(action)) + " strength " + std::to_string(strength));
}

// ProceduralMemory Implementation
struct ProceduralMemory::Impl {
    std::unordered_map<std::string, Skill> skills;
    
    Impl() {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}
ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.0f;
    
    pImpl->skills[name] = skill;
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skills.size());
    
    for (auto& kv : pImpl->skills) {
        result.push_back(&kv.second);
    }
    
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.proficiency = std::clamp(it->second.proficiency + delta, 0.0f, 100.0f);
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

} // namespace nlm