#pragma once

#include "AdvancedProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <set>

namespace nlm {

AdvancedProceduralMemory::AdvancedProceduralMemory()
    : brain_(nullptr)
    , averageProficiency_(0.0f)
{
    NLM_LOG_INFO("AdvancedProceduralMemory initialized");
}

AdvancedProceduralMemory::~AdvancedProceduralMemory() {
    NLM_LOG_INFO("AdvancedProceduralMemory destroyed");
}

void AdvancedProceduralMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize AdvancedProceduralMemory with null brain");
        return;
    }
    brain_ = brain;
    NLM_LOG_INFO("AdvancedProceduralMemory initialized with brain reference");
}

void AdvancedProceduralMemory::learnSkill(const std::string& skillName,
                                         const std::vector<NeuronId>& neuralPattern,
                                         float initialProficiency) {
    if (concepts_.find(skillName) != concepts_.end()) {
        NLM_LOG_DEBUG("Skill already exists: " + skillName);
        return;
    }

    ProceduralSkill skill(skillName, neuralPattern, initialProficiency);

    skills_[skillName] = skill;
    neuralToSkillMap_[neuralPattern[0]] = skillName; // Map first neuron to skill

    NLM_LOG_DEBUG("Learned skill: " + skillName + " with proficiency " + std::to_string(initialProficiency));
}

std::vector<float> AdvancedProceduralMemory::retrieveSkill(const std::string& skillName) const {
    auto it = skills_.find(skillName);
    if (it == skills_.end()) {
        return std::vector<float>();
    }

    const ProceduralSkill& skill = it->second;
    return skill.neuralPattern; // Simplified - return neural pattern as activation
}

std::vector<std::string> AdvancedProceduralMemory::getAllSkills() const {
    std::vector<std::string> skillNames;
    for (const auto& pair : skills_) {
        skillNames.push_back(pair.first);
    }
    return skillNames;
}

void AdvancedProceduralMemory::updateProficiency(const std::string& skillName, float delta,
                                                 const std::vector<float>& performanceContext) {
    auto it = skills_.find(skillName);
    if (it == skills_.end()) {
        return;
    }

    ProceduralSkill& skill = it->second;

    // Update proficiency based on performance
    skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);

    // Adapt neural pattern based on performance
    adaptSkillPattern(skill, performanceContext);

    // Update related skills if this is a transfer skill
    for (const auto& relatedSkill : skill.relatedSkills) {
        if (skills_.find(relatedSkill) != skills_.end()) {
            // Transfer some proficiency
            float transferAmount = delta * 0.3f;
            skills_[relatedSkill].proficiency = std::clamp(
                skills_[relatedSkill].proficiency + transferAmount, 0.0f, 1.0f);
        }
    }

    // Update average proficiency
    float totalProficiency = 0.0f;
    for (const auto& pair : skills_) {
        totalProficiency += pair.second.proficiency;
    }
    averageProficiency_ = totalProficiency / skills_.size();

    NLM_LOG_DEBUG("Updated proficiency for skill: " + skillName + " to " + std::to_string(skill.proficiency));
}

void AdvancedProceduralMemory::combineSkills(const std::string& skillA,
                                             const std::string& skillB,
                                             const std::string& hybridName) {
    if (skills_.find(skillA) == skills_.end() || skills_.find(skillB) == skills_.end()) {
        NLM_LOG_ERROR("One or both skills do not exist for combination");
        return;
    }

    const ProceduralSkill& skillAObj = skills_[skillA];
    const ProceduralSkill& skillBObj = skills_[skillB];

    // Create hybrid skill
    SkillHybrid hybrid;
    hybrid.hybridName = hybridName;
    hybrid.parentSkills = {skillA, skillB};
    hybrid.combinedPattern = skillAObj.neuralPattern;
    hybrid.combinedPattern.insert(hybrid.combinedPattern.end(), skillBObj.neuralPattern.begin(), skillBObj.neuralPattern.end());
    hybrid.baseProficiency = (skillAObj.proficiency + skillBObj.proficiency) * 0.5f;
    hybrid.createdTime = 0; // Would be set by simulation

    skillHybrids_.push_back(hybrid);

    // Add hybrid to skills
    ProceduralSkill hybridSkill;
    hybridSkill.name = hybridName;
    hybridSkill.neuralPattern = hybrid.combinedPattern;
    hybridSkill.proficiency = hybrid.baseProficiency;
    hybridSkill.stability = 0.5f;
    hybridSkill.transferPotential = 0.3f;
    hybridSkill.lastPracticed = 0;

    skills_[hybridName] = hybridSkill;

    NLM_LOG_DEBUG("Combined skills " + skillA + " and " + skillB + " into hybrid: " + hybridName);
}

void AdvancedProceduralMemory::hybridizeSkill(const std::string& sourceSkill,
                                             const std::vector<float>& adaptationParameters) {
    auto it = skills_.find(sourceSkill);
    if (it == skills_.end()) {
        return;
    }

    ProceduralSkill& sourceSkillObj = it->second;

    // Create hybridized version with modified parameters
    std::string hybridizedName = "hyb_" + sourceSkill;

    ProceduralSkill hybridizedSkill = sourceSkillObj;
    hybridizedSkill.name = hybridizedName;

    // Adapt neural pattern based on parameters
    for (size_t i = 0; i < std::min(adaptationParameters.size(), hybridizedSkill.neuralPattern.size()); ++i) {
        // Scale neural activations based on adaptation
        hybridizedSkill.neuralPattern[i].value *= (1.0f + adaptationParameters[i] * 0.1f);
    }

    hybridizedSkill.proficiency *= (1.0f + adaptationParameters.size() * 0.05f);
    hybridizedSkill.stability *= 0.8f; // Reduced stability for hybrid

    skills_[hybridizedName] = hybridizedSkill;

    NLM_LOG_DEBUG("Hybridized skill: " + sourceSkill + " into " + hybridizedName);
}

void AdvancedProceduralMemory::adaptSkill(const std::string& skillName,
                                         const std::vector<float>& performanceMetrics,
                                         float learningRate) {
    auto it = skills_.find(skillName);
    if (it == skills_.end()) {
        return;
    }

    ProceduralSkill& skill = it->second;

    // Compute optimal adaptations based on performance
    std::vector<float> optimalAdaptations;
    for (float metric : performanceMetrics) {
        // Simple adaptation: increase or decrease based on metric
        float adaptation = (metric - 0.5f) * learningRate;
        optimalAdaptations.push_back(adaptation);
    }

    // Apply adaptations to neural pattern
    adaptSkillPattern(skill, optimalAdaptations);

    NLM_LOG_DEBUG("Adapted skill: " + skillName + " based on performance metrics");
}

void AdvancedProceduralMemory::applySkillDecay(float decayRate) {
    for (auto& pair : skills_) {
        ProceduralSkill& skill = pair.second;

        // Decay proficiency
        skill.proficiency *= (1.0f - decayRate);

        // Decay stability
        skill.stability *= (1.0f - decayRate * 0.5f);

        // Decay transfer potential
        skill.transferPotential *= (1.0f - decayRate);
    }

    NLM_LOG_DEBUG("Applied skill decay with rate: " + std::to_string(decayRate));
}

void AdvancedProceduralMemory::forgetUnusedSkills(float threshold) {
    std::vector<std::string> skillsToRemove;

    for (const auto& pair : skills_) {
        const ProceduralSkill& skill = pair.second;

        // Check if skill is rarely used (based on last practiced time and stability)
        if (skill.stability < threshold || skill.proficiency < threshold) {
            skillsToRemove.push_back(pair.first);
        }
    }

    // Remove unused skills
    for (const auto& skillName : skillsToRemove) {
        skills_.erase(skillName);

        // Remove from neural map
        for (auto it = neuralToSkillMap_.begin(); it != neuralToSkillMap_.end(); ) {
            if (it->second == skillName) {
                it = neuralToSkillMap_.erase(it);
            } else {
                ++it;
            }
        }
    }

    NLM_LOG_DEBUG("Forgot " + std::to_string(skillsToRemove.size()) + " unused skills");
}

void AdvancedProceduralMemory::transferSkill(const std::string& sourceSkill,
                                            const std::string& targetSkill,
                                            float transferStrength) {
    if (skills_.find(sourceSkill) == skills_.end() || skills_.find(targetSkill) == skills_.end()) {
        return;
    }

    ProceduralSkill& sourceSkillObj = skills_[sourceSkill];
    ProceduralSkill& targetSkillObj = skills_[targetSkill];

    // Transfer proficiency
    float transferredAmount = sourceSkillObj.proficiency * transferStrength;

    targetSkillObj.proficiency = std::min(1.0f, targetSkillObj.proficiency + transferredAmount);

    // Update related skills for the target
    for (const auto& relatedSkill : targetSkillObj.relatedSkills) {
        if (skills_.find(relatedSkill) != skills_.end()) {
            skills_[relatedSkill].proficiency *= (1.0f + transferredAmount * 0.2f);
        }
    }

    NLM_LOG_DEBUG("Transferred skill: " + sourceSkill + " to " + targetSkill +
                  " with strength " + std::to_string(transferStrength));
}

void AdvancedProceduralMemory::generalizeSkill(const std::string& sourceSkill,
                                               const std::vector<std::string>& relatedSkills) {
    auto it = skills_.find(sourceSkill);
    if (it == skills_.end()) {
        return;
    }

    const ProceduralSkill& sourceSkillObj = it->second;

    for (const auto& relatedSkillName : relatedSkills) {
        if (skills_.find(relatedSkillName) != skills_.end()) {
            ProceduralSkill& relatedSkillObj = skills_[relatedSkillName];

            // Generalize by averaging proficiency
            float newProficiency = (sourceSkillObj.proficiency + relatedSkillObj.proficiency) * 0.5f;
            relatedSkillObj.proficiency = newProficiency;

            // Update neural pattern by averaging (simplified)
            relatedSkillObj.neuralPattern.clear();
            relatedSkillObj.neuralPattern.insert(relatedSkillObj.neuralPattern.begin(),
                                               sourceSkillObj.neuralPattern.begin(),
                                               sourceSkillObj.neuralPattern.end());
        }
    }

    NLM_LOG_DEBUG("Generalized skill: " + sourceSkill + " to related skills");
}

void AdvancedProceduralMemory::clear() {
    skills_.clear();
    skillHybrids_.clear();
    neuralToSkillMap_.clear();

    averageProficiency_ = 0.0f;

    NLM_LOG_INFO("AdvancedProceduralMemory cleared");
}

void AdvancedProceduralMemory::adaptSkillPattern(ProceduralSkill& skill,
                                                const std::vector<float>& adaptations) {
    for (size_t i = 0; i < std::min(adaptations.size(), skill.neuralPattern.size()); ++i) {
        // Adapt based on adaptation parameters
        skill.neuralPattern[i].value *= (1.0f + adaptations[i] * 0.1f);
    }
}

} // namespace nlm