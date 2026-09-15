#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    std::unordered_map<std::string, std::vector<NeuronId>> skillPatterns;
    std::unordered_map<std::string, float> skillProficiencies;
    std::unordered_map<std::string, std::vector<NeuronId>> skillSequences;
    std::unordered_map<std::string, float> skillTimestamps;
    
    Impl() : brain(nullptr) {}
};

NeuralProceduralMemory::NeuralProceduralMemory()
    : pImpl(new Impl)
{
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::learnSkill(const std::string& skillName, const std::vector<NeuronId>& sequence, float proficiency) {
    if (sequence.empty()) return;
    
    // Store skill pattern and sequence
    skillPatterns[skillName] = sequence;
    skillSequences[skillName] = sequence;
    skillProficiencies[skillName] = std::clamp(proficiency, 0.0f, 1.0f);
    skillTimestamps[skillName] = 0.0f;
    
    // Create neural pathways for this skill
    if (brain_) {
        for (size_t i = 0; i < sequence.size() - 1; ++i) {
            // Create sequential connections
            // In a real implementation, this would establish synaptic connections
        }
    }
}

std::vector<NeuronId> NeuralProceduralMemory::executeSkill(const std::string& skillName) const {
    auto it = skillPatterns.find(skillName);
    if (it != skillPatterns.end()) {
        return it->second;
    }
    return std::vector<NeuronId>();
}

bool NeuralProceduralMemory::hasSkill(const std::string& skillName) const {
    return skillPatterns.find(skillName) != skillPatterns.end();
}

std::vector<std::string> NeuralProceduralMemory::getAllSkills() const {
    std::vector<std::string> skills;
    skills.reserve(skillPatterns.size());
    for (const auto& pair : skillPatterns) {
        skills.push_back(pair.first);
    }
    return skills;
}

float NeuralProceduralMemory::getSkillProficiency(const std::string& skillName) const {
    auto it = skillProficiencies.find(skillName);
    if (it != skillProficiencies.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralProceduralMemory::updateSkillProficiency(const std::string& skillName, float delta) {
    auto it = skillProficiencies.find(skillName);
    if (it != skillProficiencies.end()) {
        it->second = std::clamp(it->second + delta, 0.0f, 1.0f);
        skillTimestamps[skillName] = 0.0f;
        
        if (brain_ && it->second > 0.5f) {
            NLM_LOG_INFO("Skill '" + skillName + "' proficiency increased to " + std::to_string(it->second));
        }
    }
}

std::vector<NeuronId> NeuralProceduralMemory::getSkillPattern(const std::string& skillName) const {
    auto it = skillSequences.find(skillName);
    if (it != skillSequences.end()) {
        return it->second;
    }
    return std::vector<NeuronId>();
}

void NeuralProceduralMemory::clear() {
    skillPatterns.clear();
    skillProficiencies.clear();
    skillSequences.clear();
    skillTimestamps.clear();
}

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::update(TimestepDuration dt) {
    float timeDelta = dt.count();
    
    // Skills decay if not practiced
    for (auto& pair : skillProficiencies) {
        if (pair.second > 0.1f) {
            pair.second *= (1.0f - 0.005f * timeDelta);  // Slow decay
            skillTimestamps[pair.first] += timeDelta;
        }
    }
}

} // namespace nlm
