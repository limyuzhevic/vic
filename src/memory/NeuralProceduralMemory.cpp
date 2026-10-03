#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    
    // Skills stored as neurons
    std::vector<NeuronId> skillNeurons;
    std::vector<std::string> skillNames;
    
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
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern, float proficiency) {
    // Find or create skill neuron
    NeuronId neuron = findOrCreateSkillNeuron(name);
    
    // Store pattern for skill
    // In a full implementation, this would store the motor pattern
    // For now, we just log the learning
    
    skillProficiency_[name] = std::clamp(proficiency, 0.0f, 1.0f);
    
    // Store pattern in neurons (simplified)
    if (brain_ && neuron != INVALID_NEURON_ID) {
        // Create motor pattern connections
        createMotorPattern(name, pattern);
    }
    
    NLM_LOG_INFO("NeuralProceduralMemory learned skill: " + name + " (proficiency: " + std::to_string(proficiency) + ")");
}

Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    // Create a Skill struct from stored data
    static Skill tempSkill;
    
    // Find skill
    auto it = skillProficiency_.find(name);
    if (it != skillProficiency_.end()) {
        tempSkill.name = name;
        tempSkill.proficiency = it->second;
        return &tempSkill;
    }
    
    return nullptr;
}

std::vector<Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (const auto& kv : skillProficiency_) {
        Skill* skill = getSkill(kv.first);
        if (skill) {
            result.push_back(skill);
        }
    }
    return result;
}

void NeuralProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = skillProficiency_.find(name);
    if (it != skillProficiency_.end()) {
        it->second = std::clamp(it->second + delta, 0.0f, 1.0f);
    }
}

void NeuralProceduralMemory::executeSkill(const std::string& name, float intensity) {
    if (!brain_) return;
    
    // Get skill
    Skill* skill = getSkill(name);
    if (!skill) return;
    
    // Execute skill (simplified)
    // In a full implementation, this would activate the motor pattern
    NLM_LOG_INFO("NeuralProceduralMemory executing skill: " + name + " (intensity: " + std::to_string(intensity) + ")");
}

bool NeuralProceduralMemory::hasSkill(const std::string& name) const {
    return skillProficiency_.find(name) != skillProficiency_.end();
}

float NeuralProceduralMemory::getProficiency(const std::string& name) const {
    auto it = skillProficiency_.find(name);
    if (it != skillProficiency_.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralProceduralMemory::clearSkill(const std::string& name) {
    skillProficiency_.erase(name);
}

void NeuralProceduralMemory::clear() {
    skillProficiency_.clear();
}

size_t NeuralProceduralMemory::getSkillCount() const {
    return skillProficiency_.size();
}

void NeuralProceduralMemory::learnFromTrial(const std::string& skillName, float outcome, float learningRate) {
    if (!hasSkill(skillName)) return;
    
    // Update proficiency based on outcome
    float delta = learningRate * (outcome - 0.5f);
    updateProficiency(skillName, delta);
    
    NLM_LOG_INFO("NeuralProceduralMemory learned from trial: " + skillName + " (outcome: " + std::to_string(outcome) + ")");
}

void NeuralProceduralMemory::composeSkills(const std::string& primary, const std::string& secondary, const std::string& composite) {
    if (!hasSkill(primary) || !hasSkill(secondary)) return;
    
    // Create composite skill
    float compositeProficiency = (getProficiency(primary) + getProficiency(secondary)) * 0.5f;
    skillProficiency_[composite] = std::clamp(compositeProficiency, 0.0f, 1.0f);
    
    NLM_LOG_INFO("NeuralProceduralMemory composed skills: " + composite + " from " + primary + " + " + secondary);
}

std::vector<std::string> NeuralProceduralMemory::decomposeSkill(const std::string& name) const {
    std::vector<std::string> result;
    
    if (!hasSkill(name)) return result;
    
    // In a full implementation, this would decompose the motor pattern into primitives
    result.push_back(name);
    return result;
}

float NeuralProceduralMemory::getSkillSimilarity(const std::string& skill1, const std::string& skill2) const {
    if (skill1 == skill2) return 1.0f;
    if (!hasSkill(skill1) || !hasSkill(skill2)) return 0.0f;
    
    // Simple similarity based on proficiency difference
    return 1.0f - std::abs(getProficiency(skill1) - getProficiency(skill2));
}

void NeuralProceduralMemory::consolidate(float relevanceThreshold) {
    // Remove skills with low proficiency
    auto it = skillProficiency_.begin();
    while (it != skillProficiency_.end()) {
        if (it->second < relevanceThreshold) {
            it = skillProficiency_.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<Skill*> NeuralProceduralMemory::getSkillsByType(const std::string& type) const {
    // In a full implementation, skills would have types
    // For now, return all skills
    return const_cast<NeuralProceduralMemory*>(this)->getAllSkills();
}

std::vector<float> NeuralProceduralMemory::getMotorPattern(const std::string& name) const {
    // Return a simplified motor pattern
    std::vector<float> pattern(10, 0.0f);
    
    float proficiency = getProficiency(name);
    for (size_t i = 0; i < pattern.size(); ++i) {
        pattern[i] = proficiency * 0.1f;
    }
    
    return pattern;
}

void NeuralProceduralMemory::addMotorAdaptation(const std::string& skillName, const std::vector<float>& adaptation) {
    // Store adaptation for skill
    // In a full implementation, this would modify the motor pattern
}

float NeuralProceduralMemory::getMotorMemory(const std::string& skillName) const {
    // Motor memory is the product of proficiency
    return getProficiency(skillName);
}

bool NeuralProceduralMemory::isMotorPatternReady(const std::string& name) const {
    return getProficiency(name) > 0.3f;
}

NeuronId NeuralProceduralMemory::findOrCreateSkillNeuron(const std::string& name) {
    // Check if we already have a neuron for this skill
    for (size_t i = 0; i < skillNames.size(); ++i) {
        if (skillNames[i] == name) {
            return skillNeurons[i];
        }
    }
    
    // Create new skill neuron
    NeuronId newId(20000 + skillNames.size());  // Reserve IDs for skills
    skillNeurons.push_back(newId);
    skillNames.push_back(name);
    
    return newId;
}

NeuronId NeuralProceduralMemory::createMotorPattern(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Create connections for the motor pattern
    // In a full implementation, this would establish synapses
    if (!pattern.empty()) {
        return pattern[0];
    }
    return INVALID_NEURON_ID;
}

} // namespace nlm
