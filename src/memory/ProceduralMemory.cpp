#include "ProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct ProceduralMemory::Impl {
    // Skills indexed by name for quick access
    std::unordered_map<std::string, Skill> skills;
    // Neural pattern for quick skill matching
    std::vector<std::vector<NeuronId>> patternDatabase;
    // Skill usage statistics for refinement
    std::unordered_map<std::string, size_t> usageCount;
    // Skill proficiency decay over time
    std::unordered_map<std::string, float> lastPracticeTime;
    
    Impl() : currentTime_(0.0f) {}
};

ProceduralMemory::ProceduralMemory()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("ProceduralMemory initialized");
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    if (pattern.empty()) return;
    
    // Check if skill already exists
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        // Update existing skill with new pattern
        Skill& skill = it->second;
        skill.neuralPattern = pattern;
        skill.proficiency = std::min(1.0f, skill.proficiency + 0.1f); // Incremental learning
        NLM_LOG_INFO("ProceduralMemory: Enhanced skill " + name);
    } else {
        // Create new skill
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = 0.3f; // Initial proficiency
        pImpl->skills[name] = skill;
        pImpl->patternDatabase.push_back(pattern);
        
        NLM_LOG_INFO("ProceduralMemory: Learned new skill " + name);
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> skills;
    skills.reserve(pImpl->skills.size());
    for (auto& pair : pImpl->skills) {
        skills.push_back(&pair.second);
    }
    return skills;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        Skill& skill = it->second;
        skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
        pImpl->usageCount[name]++;
        pImpl->lastPracticeTime[name] = currentTime_;
        
        // Decay proficiency over time if not practiced recently
        if (currentTime_ - pImpl->lastPracticeTime[name] > 1000.0f) {
            float decayFactor = 0.1f * (currentTime_ - pImpl->lastPracticeTime[name]) / 1000.0f;
            skill.proficiency = std::max(0.0f, skill.proficiency - decayFactor);
            NLM_LOG_INFO("ProceduralMemory: Proficient skill " + name + " decayed");
        }
        
        NLM_LOG_INFO("ProceduralMemory: Updated skill " + name + " proficiency to " + std::to_string(skill.proficiency));
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->patternDatabase.clear();
    pImpl->usageCount.clear();
    pImpl->lastPracticeTime.clear();
    NLM_LOG_INFO("ProceduralMemory: Cleared all skills");
}

} // namespace nlm