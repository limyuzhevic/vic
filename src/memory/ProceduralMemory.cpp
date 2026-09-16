#include "ProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <sstream>

namespace nlm {

struct ProceduralMemory::Impl {
    // Skills stored with their neural patterns and proficiency levels
    std::unordered_map<std::string, Skill> skills;
    
    // Neural pathways between skills (prerequisites)
    std::vector<std::pair<std::string, std::string>> skillDependencies;
    
    // Motor program sequences for complex actions
    std::unordered_map<std::string, std::vector<ActionType>> skillSequences;
    
    // Skill execution statistics for refinement
    std::unordered_map<std::string, float> skillUsageCounts;
    
    // Automaticity levels for skills (0.0 to 1.0)
    std::unordered_map<std::string, float> skillAutomaticity;
    
    Impl() {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {
    // Initialize with basic procedural skills
    initializeBasicSkills();
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::initializeBasicSkills() {
    // Basic motor skills essential for NLM operation
    std::vector<NeuronId> moveForwardPattern;
    moveForwardPattern.push_back(NeuronId(50001));
    moveForwardPattern.push_back(NeuronId(50002));
    learnSkill("move_forward", moveForwardPattern);
    
    std::vector<NeuronId> turnLeftPattern;
    turnLeftPattern.push_back(NeuronId(50003));
    turnLeftPattern.push_back(NeuronId(50004));
    learnSkill("turn_left", turnLeftPattern);
    
    std::vector<NeuronId> turnRightPattern;
    turnRightPattern.push_back(NeuronId(50005));
    turnRightPattern.push_back(NeuronId(50006));
    learnSkill("turn_right", turnRightPattern);
    
    std::vector<NeuronId> lookPattern;
    lookPattern.push_back(NeuronId(50007));
    lookPattern.push_back(NeuronId(50008));
    learnSkill("look", lookPattern);
    
    std::vector<NeuronId> eatPattern;
    eatPattern.push_back(NeuronId(50009));
    eatPattern.push_back(NeuronId(50010));
    learnSkill("eat", eatPattern);
    
    // Add dependencies
    addSkillDependency("turn_left", "move_forward");
    addSkillDependency("turn_right", "move_forward");
    addSkillDependency("eat", "move_forward");
    
    NLM_LOG_INFO("Procedural memory initialized with basic skills");
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.0f;
    
    // Set sequence for known skills
    if (name == "move_forward") {
        std::vector<ActionType> seq;
        seq.push_back(ActionType::MoveForward);
        skillSequences[name] = seq;
    } else if (name == "turn_left") {
        std::vector<ActionType> seq;
        seq.push_back(ActionType::TurnLeft);
        skillSequences[name] = seq;
    } else if (name == "turn_right") {
        std::vector<ActionType> seq;
        seq.push_back(ActionType::TurnRight);
        skillSequences[name] = seq;
    } else if (name == "look") {
        std::vector<ActionType> seq;
        seq.push_back(ActionType::Look);
        skillSequences[name] = seq;
    } else if (name == "eat") {
        std::vector<ActionType> seq;
        seq.push_back(ActionType::Eat);
        skillSequences[name] = seq;
    }
    
    pImpl->skills[name] = skill;
    pImpl->skillUsageCounts[name] = 0.0f;
    pImpl->skillAutomaticity[name] = 0.0f;
    
    NLM_LOG_DEBUG("Procedural memory learned skill: " + name);
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
    for (auto& pair : pImpl->skills) {
        result.push_back(&pair.second);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    Skill* skill = getSkill(name);
    if (skill) {
        skill->proficiency = std::clamp(skill->proficiency + delta, 0.0f, 1.0f);
        pImpl->skillUsageCounts[name] += 1.0f;
        
        // Increase automaticity with proficiency
        pImpl->skillAutomaticity[name] = skill->proficiency * 0.5f + 
                                        (pImpl->skillUsageCounts[name] / 100.0f) * 0.5f;
        
        NLM_LOG_DEBUG("Updated proficiency for skill " + name + ": " + 
                     std::to_string(skill->proficiency));
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillDependencies.clear();
    pImpl->skillSequences.clear();
    pImpl->skillUsageCounts.clear();
    pImpl->skillAutomaticity.clear();
    NLM_LOG_INFO("Procedural memory cleared");
}

void ProceduralMemory::addSkillDependency(const std::string& dependent, const std::string& prerequisite) {
    // Check if already exists
    for (const auto& dep : pImpl->skillDependencies) {
        if ((dep.first == dependent && dep.second == prerequisite) ||
            (dep.first == prerequisite && dep.second == dependent)) {
            return;
        }
    }
    
    pImpl->skillDependencies.emplace_back(dependent, prerequisite);
    
    NLM_LOG_DEBUG("Added skill dependency: " + dependent + " requires " + prerequisite);
}

std::vector<std::string> ProceduralMemory::getSkillPrerequisites(const std::string& skill) const {
    std::vector<std::string> prerequisites;
    for (const auto& dep : pImpl->skillDependencies) {
        if (dep.first == skill) {
            prerequisites.push_back(dep.second);
        }
    }
    return prerequisites;
}

std::vector<std::string> ProceduralMemory::getSkillsThatRequire(const std::string& skill) const {
    std::vector<std::string> dependents;
    for (const auto& dep : pImpl->skillDependencies) {
        if (dep.second == skill) {
            dependents.push_back(dep.first);
        }
    }
    return dependents;
}

float ProceduralMemory::getSkillAutomaticity(const std::string& name) const {
    auto it = pImpl->skillAutomaticity.find(name);
    return (it != pImpl->skillAutomaticity.end()) ? it->second : 0.0f;
}

bool ProceduralMemory::canExecuteSkill(const std::string& skillName) const {
    Skill* skill = const_cast<ProceduralMemory*>(this)->getSkill(skillName);
    if (!skill) return false;
    
    // Check proficiency
    if (skill->proficiency < 0.1f) return false;
    
    // Check prerequisites
    std::vector<std::string> prerequisites = getSkillPrerequisites(skillName);
    for (const auto& prereq : prerequisites) {
        if (getSkill(prereq) == nullptr || 
            getSkill(prereq)->proficiency < 0.5f) {
            return false;
        }
    }
    
    return true;
}

std::vector<ActionType> ProceduralMemory::executeSkill(const std::string& skillName) {
    auto it = skillSequences.find(skillName);
    if (it != skillSequences.end()) {
        return it->second;
    }
    
    // Default action if no sequence defined
    std::vector<ActionType> defaultSeq;
    defaultSeq.push_back(ActionType::Wait);
    return defaultSeq;
}

void ProceduralMemory::addSkillSequence(const std::string& skillName, const std::vector<ActionType>& sequence) {
    skillSequences[skillName] = sequence;
    NLM_LOG_DEBUG("Added sequence for skill: " + skillName);
}

float ProceduralMemory::getSkillUsageCount(const std::string& name) const {
    auto it = pImpl->skillUsageCounts.find(name);
    return (it != pImpl->skillUsageCounts.end()) ? it->second : 0.0f;
}

std::string ProceduralMemory::getMostUsedSkill() const {
    if (pImpl->skillUsageCounts.empty()) return "";
    
    float maxUsage = 0.0f;
    std::string mostUsed;
    
    for (const auto& pair : pImpl->skillUsageCounts) {
        if (pair.second > maxUsage) {
            maxUsage = pair.second;
            mostUsed = pair.first;
        }
    }
    
    return mostUsed;
}

std::vector<std::string> ProceduralMemory::getAllSkillNames() const {
    std::vector<std::string> result;
    for (const auto& pair : pImpl->skills) {
        result.push_back(pair.first);
    }
    return result;
}

} // namespace nlm