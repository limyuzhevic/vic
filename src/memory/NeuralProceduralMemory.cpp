#pragma once

#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct NeuralSemanticMemory::Impl {
    class Brain* brain;
    std::vector<Skill> skills;
    std::vector<float> lastUsedTimes;
    std::vector<float> decayRates;
    
    Impl() : brain(nullptr) {}
};

NeuralProceduralMemory::NeuralProceduralMemory() : pImpl(new Impl) {}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Procedural memory initialized");
}

void NeuralProceduralMemory::storeSkill(const std::string& name, const std::vector<NeuronId>& pattern, float initialProficiency) {
    // Find existing skill
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = initialProficiency;
            skill.lastUsedTime = 0.0f;
            return;
        }
    }
    
    // Add new skill
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = initialProficiency;
    skill.lastUsedTime = 0.0f;
    skill.decayRate = 0.0001f;
    
    pImpl->skills.push_back(skill);
}

NeuralProceduralMemory::Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<NeuralProceduralMemory::Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void NeuralProceduralMemory::practiceSkill(const std::string& name, float improvementDelta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency += improvementDelta;
            skill.lastUsedTime = 0.0f;  // Reset decay
            break;
        }
    }
}

void NeuralProceduralMemory::decayUnused(float currentTime, float decayRate) {
    for (auto& skill : pImpl->skills) {
        if (skill.lastUsedTime > 0.0f) {
            float timeSinceUsed = currentTime - skill.lastUsedTime;
            if (timeSinceUsed > 0.0f) {
                skill.proficiency *= std::pow(1.0f - decayRate, timeSinceUsed);
                if (skill.proficiency < 0.01f) {
                    // Remove weak skills
                    skill.proficiency = 0.0f;
                }
            }
        }
    }
}

} // namespace nlm