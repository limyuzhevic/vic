#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    
    // For skill execution
    std::mt19937 rng;
    
    Impl(Brain* brainPtr = nullptr) : brain(brainPtr) {
        if (brainPtr && brainPtr->getRandomGenerator()) {
            rng = *brainPtr->getRandomGenerator();
        } else {
            std::random_device rd;
            rng.seed(rd());
        }
    }
};

NeuralProceduralMemory::NeuralProceduralMemory()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , skillLimit_(100)
{
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->rng = std::mt19937();  // Reset with brain's seed if available
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    if (name.empty() || pattern.empty() || !brain_) return;
    
    // Check if skill already exists
    auto it = std::find_if(skills_.begin(), skills_.end(),
                         [&name](const Skill& s) { return s.name == name; });
    
    if (it != skills_.end()) {
        // Update existing skill
        it->neuralPattern = pattern;
        it->proficiency = std::min(1.0f, it->proficiency + 0.1f);
    } else {
        // Add new skill
        if (skills_.size() >= skillLimit_) {
            // Remove least proficient skill
            auto minIt = std::min_element(skills_.begin(), skills_.end(),
                [](const Skill& a, const Skill& b) { return a.proficiency < b.proficiency; });
            skills_.erase(minIt);
        }
        
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = 1.0f;
        skills_.push_back(skill);
        
        // Create procedural connections
        // In real implementation, this would create forward connections
    }
}

Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    auto it = std::find_if(skills_.begin(), skills_.end(),
                         [&name](const Skill& s) { return s.name == name; });
    return (it != skills_.end()) ? &(*it) : nullptr;
}

std::vector<Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(skills_.size());
    
    for (auto& skill : skills_) {
        result.push_back(&skill);
    }
    
    return result;
}

void NeuralProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = std::find_if(skills_.begin(), skills_.end(),
                         [&name](const Skill& s) { return s.name == name; });
    
    if (it != skills_.end()) {
        it->proficiency = std::clamp(it->proficiency + delta, 0.0f, 1.0f);
    }
}

std::vector<float> NeuralProceduralMemory::executeSkill(const std::string& name, const std::vector<float>& input) {
    std::vector<float> output(input.size(), 0.0f);
    
    auto skill = getSkill(name);
    if (!skill || skill->proficiency < 0.1f) return output;
    
    // Simple skill execution based on pattern
    size_t patternSize = std::min(skill->neuralPattern.size(), input.size());
    for (size_t i = 0; i < patternSize; ++i) {
        output[i] = input[i] * skill->proficiency * 0.5f;
    }
    
    // Add noise based on pattern
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    for (size_t i = 0; i < output.size(); ++i) {
        output[i] += dist(pImpl->rng) * skill->proficiency;
    }
    
    return output;
}

void NeuralProceduralMemory::clear() {
    skills_.clear();
}

} // namespace nlm
