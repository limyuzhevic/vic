#include "NeuralProceduralMemory.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <queue>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    float proficiencyDecayRate;
    float learningRate;
    size_t maxSkills;
    
    Impl() : brain(nullptr), proficiencyDecayRate(0.01f), learningRate(0.1f), maxSkills(500) {}
};

NeuralProceduralMemory::NeuralProceduralMemory() : pImpl(new Impl) {}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    this->brain_ = brain;
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnSkill(const std::string& name,
                                        const std::vector<float>& pattern,
                                        float initialProficiency) {
    // Limit total skills
    if (skills_.size() >= pImpl->maxSkills) {
        // Remove least proficient skill
        std::string leastUsedSkill;
        float minProficiency = std::numeric_limits<float>::max();
        for (const auto& pair : proficiency_) {
            if (pair.second < minProficiency) {
                minProficiency = pair.second;
                leastUsedSkill = pair.first;
            }
        }
        if (!leastUsedSkill.empty()) {
            skills_.erase(leastUsedSkill);
            proficiency_.erase(leastUsedSkill);
            skillNeurons_.erase(leastUsedSkill);
        }
    }
    
    // Store skill
    skills_[name] = pattern;
    proficiency_[name] = initialProficiency;
    skillNeurons_[name] = findSkillNeuron(pattern);
    
    NLM_LOG_INFO("Learned procedural skill: " + name + " (proficiency: " + 
                 std::to_string(initialProficiency) + ")");
}

const std::vector<float>* NeuralProceduralMemory::getSkill(const std::string& name) const {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<float> NeuralProceduralMemory::executeSkill(const std::string& name,
                                                         const std::vector<float>& inputState) {
    const std::vector<float>* skill = getSkill(name);
    if (!skill) {
        return std::vector<float>();  // Skill not found
    }
    
    // Execute skill by combining input and skill pattern
    std::vector<float> result(inputState.size());
    float proficiency = getSkillProficiency(name);
    
    for (size_t i = 0; i < inputState.size(); ++i) {
        float skillPattern = i < skill->size() ? (*skill)[i] : 0.0f;
        // Blend input with skill pattern based on proficiency
        result[i] = inputState[i] * (1.0f - proficiency) + skillPattern * proficiency;
    }
    
    // Skill execution increases proficiency slightly
    updateProficiency(name, 0.01f);
    
    return result;
}

float NeuralProceduralMemory::getSkillProficiency(const std::string& name) const {
    auto it = proficiency_.find(name);
    if (it != proficiency_.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = proficiency_.find(name);
    if (it != proficiency_.end()) {
        // Decay proficiency over time, then apply learning
        it->second = std::max(0.0f, it->second * (1.0f - pImpl->proficiencyDecayRate) + delta);
    }
}

std::vector<std::string> NeuralProceduralMemory::getAllSkills() const {
    std::vector<std::string> result;
    result.reserve(skills_.size());
    for (const auto& pair : skills_) {
        result.push_back(pair.first);
    }
    return result;
}

float NeuralProceduralMemory::getSkillSimilarity(const std::string& name,
                                                 const std::vector<float>& currentState) const {
    const std::vector<float>* skill = getSkill(name);
    if (!skill) {
        return 0.0f;
    }
    
    return computeSimilarity(*skill, currentState);
}

void NeuralProceduralMemory::prepareSkill(const std::string& name) {
    // Prepare skill by activating associated neurons
    auto it = skillNeurons_.find(name);
    if (it != skillNeurons_.end()) {
        // In a full implementation, this would activate the neurons
        // For now, just log the preparation
        NLM_LOG_INFO("Prepared procedural skill: " + name);
    }
}

std::vector<std::pair<std::string, float>> NeuralProceduralMemory::getSkillsByProficiency() const {
    std::vector<std::pair<std::string, float>> result;
    result.reserve(skills_.size());
    for (const auto& pair : skills_) {
        auto profIt = proficiency_.find(pair.first);
        if (profIt != proficiency_.end()) {
            result.emplace_back(pair.first, profIt->second);
        }
    }
    
    // Sort by proficiency (descending)
    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return result;
}

void NeuralProceduralMemory::clear() {
    skills_.clear();
    proficiency_.clear();
    skillNeurons_.clear();
    NLM_LOG_INFO("NeuralProceduralMemory cleared");
}

std::vector<std::pair<std::string, std::vector<float>>> NeuralProceduralMemory::exportSkills() const {
    std::vector<std::pair<std::string, std::vector<float>>> result;
    result.reserve(skills_.size());
    for (const auto& pair : skills_) {
        result.emplace_back(pair.first, pair.second);
    }
    return result;
}

void NeuralProceduralMemory::importSkills(const std::vector<std::pair<std::string, std::vector<float>>>& skills) {
    skills_.clear();
    proficiency_.clear();
    skillNeurons_.clear();
    
    for (const auto& pair : skills) {
        skills_[pair.first] = pair.second;
        proficiency_[pair.first] = 0.5f;  // Default proficiency
        skillNeurons_[pair.first] = findSkillNeuron(pair.second);
    }
    
    NLM_LOG_INFO("Imported " + std::to_string(skills.size()) + " procedural skills");
}

NeuronId NeuralProceduralMemory::findSkillNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pair : skillNeurons_) {
        if (computeSimilarity(pattern, skills_.at(pair.first)) > 0.8f) {
            return pair.second;
        }
    }
    
    // Create new neuron for this skill
    NeuronId newId(INVALID_NEURON_ID.index() + skillNeurons_.size() + 1);
    skillNeurons_[newId.index()] = newId;
    skills_[newId.index() + "_temp"] = pattern;  // Temp entry for consistency
    
    return newId;
}

float NeuralProceduralMemory::computeSimilarity(const std::vector<float>& a,
                                              const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
    }
    
    float magnitudeA = 0.0f;
    float magnitudeB = 0.0f;
    for (const auto& val : a) magnitudeA += val * val;
    for (const auto& val : b) magnitudeB += val * val;
    
    magnitudeA = std::sqrt(magnitudeA);
    magnitudeB = std::sqrt(magnitudeB);
    
    if (magnitudeA == 0.0f || magnitudeB == 0.0f) return 0.0f;
    
    return dotProduct / (magnitudeA * magnitudeB);
}

} // namespace nlm
