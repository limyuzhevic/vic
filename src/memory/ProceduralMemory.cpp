#include "ProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct ProceduralMemory::Impl {
    float consolidationThreshold = 0.8f;
    size_t maxSkills = 100;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Check if skill already exists
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        // Update existing skill
        it->second.neuralPattern = pattern;
        it->second.proficiency = std::min(1.0f, it->second.proficiency + 0.1f);
    } else {
        // Create new skill
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = 0.5f;  // Initial proficiency
        
        skills_[name] = skill;
    }
    
    NLM_LOG_INFO("ProceduralMemory learned skill: " + name);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(skills_.size());
    for (auto& pair : skills_) {
        result.push_back(&pair.second);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        it->second.proficiency = std::clamp(it->second.proficiency + delta, 0.0f, 1.0f);
    }
}

void ProceduralMemory::clear() {
    skills_.clear();
    NLM_LOG_INFO("ProceduralMemory cleared");
}

Skill* ProceduralMemory::findBestSkill(const std::vector<NeuronId>& pattern) const {
    if (skills_.empty()) return nullptr;
    
    Skill* bestSkill = nullptr;
    float bestSimilarity = -1.0f;
    
    for (const auto& pair : skills_) {
        const Skill& skill = pair.second;
        float similarity = computePatternSimilarity(pattern, skill.neuralPattern);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestSkill = const_cast<Skill*>(&skill);
        }
    }
    
    return bestSkill;
}

void ProceduralMemory::executeSkill(const std::string& name, Brain* brain) {
    Skill* skill = getSkill(name);
    if (!skill || skill->proficiency < 0.1f) {
        NLM_LOG_INFO("Cannot execute skill " + name + ": insufficient proficiency");
        return;
    }
    
    NLM_LOG_INFO("Executing skill: " + name);
    
    // Activate neural pattern for skill execution
    for (size_t i = 0; i < std::min(skill->neuralPattern.size(), 
                                   static_cast<size_t>(1000)); ++i) {
        NeuronId neuron = skill->neuralPattern[i];
        brain->injectCurrent(neuron, skill->proficiency * 0.1f);
    }
}

float ProceduralMemory::computePatternSimilarity(const std::vector<NeuronId>& patternA,
                                                 const std::vector<NeuronId>& patternB) const {
    if (patternA.empty() || patternB.empty()) return 0.0f;
    
    // Simple Jaccard-like similarity for neuron patterns
    std::unordered_set<NeuronId> setA(patternA.begin(), patternA.end());
    std::unordered_set<NeuronId> setB(patternB.begin(), patternB.end());
    
    size_t intersection = 0;
    for (const auto& neuron : setA) {
        if (setB.find(neuron) != setB.end()) {
            intersection++;
        }
    }
    
    size_t unionSize = setA.size() + setB.size() - intersection;
    return unionSize > 0 ? static_cast<float>(intersection) / unionSize : 0.0f;
}

} // namespace nlm