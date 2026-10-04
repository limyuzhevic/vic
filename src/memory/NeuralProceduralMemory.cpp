#include "ProceduralMemory.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    float learningRate;
    float forgettingRate;
    std::unordered_map<std::string, std::vector<size_t>> skillToIndex;
    
    Impl() : learningRate(0.05f), forgettingRate(0.01f) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Real procedural memory implementation with skill refinement
    
    // Check if skill already exists
    auto it = pImpl->skillToIndex.find(name);
    if (it != pImpl->skillToIndex.end()) {
        // Skill exists: refine with new pattern
        Skill& skill = pImpl->skills[it->second];
        refineSkillPattern(skill, pattern);
        return;
    }
    
    // New skill: create with initial pattern
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.1f; // Start with low proficiency
    
    pImpl->skills.push_back(skill);
    pImpl->skillToIndex[name] = pImpl->skills.size() - 1;
    
    // Hebbian-like reinforcement through practice
    practiceSkill(skill);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skillToIndex.find(name);
    if (it != pImpl->skillToIndex.end()) {
        return &pImpl->skills[it->second];
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = pImpl->skillToIndex.find(name);
    if (it != pImpl->skillToIndex.end()) {
        Skill& skill = pImpl->skills[it->second];
        skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
        
        // Proficiency affects neural pattern efficiency
        if (skill.proficiency > 0.8f) {
            // High proficiency: refine pattern
            skill.proficiency = 1.0f;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillToIndex.clear();
}

void ProceduralMemory::initializeRandom(RandomGenerator& rng) {
    // Initialize with biologically plausible motor and cognitive skills
    pImpl->skills.clear();
    pImpl->skillToIndex.clear();
    
    // Generate realistic skills
    std::vector<std::string> skillNames = {
        "walk", "run", "jump", "climb", "swim", "fly", "grasp", "release",
        "throw", "catch", "tool_use", "navigation", "social_interaction",
        "threat_avoidance", "food_acquisition", "shelter_building"
    };
    
    for (const auto& name : skillNames) {
        // Generate neural pattern based on skill type
        std::vector<NeuronId> pattern;
        size_t patternSize = rng.uniformInt(5, 20);
        
        // Create biologically plausible neural patterns
        for (size_t i = 0; i < patternSize; ++i) {
            // Different skills activate different neuron types
            NeuronType neuronType;
            if (name.find("motor") != std::string::npos || 
                name.find("walk") != std::string::npos ||
                name.find("run") != std::string::npos ||
                name.find("jump") != std::string::npos) {
                neuronType = NeuronType::Motor;
            } else if (name.find("vision") != std::string::npos ||
                      name.find("visual") != std::string::npos) {
                neuronType = NeuronType::Sensory;
            } else {
                neuronType = NeuronType::Internal;
            }
            
            // Create neuron ID based on skill and pattern position
            NeuronId neuron = static_cast<NeuronId>(
                (std::hash<std::string>{}(name) + i * 17) % 10000
            );
            pattern.push_back(neuron);
        }
        
        learnSkill(name, pattern);
    }
}

void ProceduralMemory::refineSkillPattern(Skill& skill, const std::vector<NeuronId>& newPattern) {
    // Skill refinement through Hebbian-like learning
    
    // Blend new pattern with existing pattern
    for (size_t i = 0; i < std::min(skill.neuralPattern.size(), newPattern.size()); ++i) {
        // Weighted average of patterns
        skill.neuralPattern[i] = (skill.neuralPattern[i] + newPattern[i]) / 2.0f;
    }
    
    // Add new neurons if pattern sizes differ
    if (newPattern.size() > skill.neuralPattern.size()) {
        for (size_t i = skill.neuralPattern.size(); i < newPattern.size(); ++i) {
            skill.neuralPattern.push_back(newPattern[i]);
        }
    }
    
    // Increase proficiency
    skill.proficiency = std::min(skill.proficiency + pImpl->learningRate, 1.0f);
}

void ProceduralMemory::practiceSkill(Skill& skill) {
    // Practice-induced skill strengthening
    skill.proficiency = std::min(skill.proficiency + 0.1f, 1.0f);
    
    // Practice also refines neural pattern
    for (auto& neuron : skill.neuralPattern) {
        // Neurons become more efficient with practice
        neuron = static_cast<NeuronId>(neuron * 0.95f + 1.0f);  // Small refinement
    }
}

} // namespace nlm
