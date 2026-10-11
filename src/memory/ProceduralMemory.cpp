#include "ProceduralMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct ProceduralMemory::Impl {
    struct Skill {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        SimulationStep lastExecuted;
        size_t executionCount;
        float executionProbability;
        
        Skill() : proficiency(0.0f), lastExecuted(0), executionCount(0), executionProbability(0.1f) {}
        Skill(const std::string& n, const std::vector<NeuronId>& pattern)
            : name(n), neuralPattern(pattern), proficiency(0.0f), lastExecuted(0), executionCount(0), executionProbability(0.1f) {}
    };
    
    std::unordered_map<std::string, Skill> skills;
    SimulationStep nextTimestamp;
    
    Impl() : nextTimestamp(0) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl), brain_(nullptr) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ProceduralMemory initialized");
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    if (name.empty() || pattern.empty()) return;
    
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        // Update existing skill
        it->second.neuralPattern = pattern;
        it->second.proficiency = std::min(1.0f, it->second.proficiency + 0.2f);
        it->second.lastExecuted = pImpl->nextTimestamp++;
        it->second.executionProbability = 0.1f; // Reset probability
    } else {
        // Create new skill
        pImpl->skills[name] = Impl::Skill(name, pattern);
        pImpl->skills[name].lastExecuted = pImpl->nextTimestamp++;
    }
    
    NLM_LOG_INFO("Learned skill: " + name + " with " + std::to_string(pattern.size()) + " neurons");
}

const Skill* ProceduralMemory::getSkill(const std::string& name) const {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skills.size());
    
    for (auto& pair : pImpl->skills) {
        result.push_back(&pair.second);
    }
    
    return result;
}

bool ProceduralMemory::executeSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it == pImpl->skills.end()) {
        return false; // Skill not found
    }
    
    Skill& skill = it->second;
    
    // Check if skill has sufficient proficiency
    if (skill.proficiency < 0.1f) {
        NLM_LOG_INFO("Skill " + name + " has insufficient proficiency: " + std::to_string(skill.proficiency));
        return false;
    }
    
    // Execute skill by activating associated neurons
    if (brain_ && !skill.neuralPattern.empty()) {
        NLM_LOG_INFO("Executing skill: " + name + " with " + std::to_string(skill.neuralPattern.size()) + " neurons");
        
        // Record execution
        skill.executionCount++;
        skill.lastExecuted = pImpl->nextTimestamp++;
        skill.executionProbability = std::min(1.0f, skill.executionProbability + 0.05f);
        
        // Note: In a real implementation, we would activate the neurons
        // For now, we just log the execution
        
        return true;
    }
    
    return false;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.proficiency = std::clamp(it->second.proficiency + delta, 0.0f, 1.0f);
    }
}

bool ProceduralMemory::hasSkill(const std::string& name) const {
    return pImpl->skills.find(name) != pImpl->skills.end();
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    NLM_LOG_INFO("ProceduralMemory cleared");
}

size_t ProceduralMemory::getSkillCount() const {
    return pImpl->skills.size();
}

std::vector<std::pair<std::string, float>> ProceduralMemory::getMostPracticedSkills(size_t count) const {
    std::vector<std::pair<std::string, float>> result;
    result.reserve(pImpl->skills.size());
    
    for (const auto& pair : pImpl->skills) {
        result.push_back(std::make_pair(pair.first, static_cast<float>(pair.second.executionCount)));
    }
    
    // Sort by execution count (descending)
    std::sort(result.begin(), result.end(),
        [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
            return a.second > b.second;
        });
    
    // Limit to requested count
    if (count < result.size()) {
        result.resize(count);
    }
    
    return result;
}

void ProceduralMemory::associateNeuronsToSkill(const std::string& name, const std::vector<NeuronId>& neurons) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.neuralPattern = neurons;
    }
}

std::vector<NeuronId> ProceduralMemory::getSkillNeurons(const std::string& name) const {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return it->second.neuralPattern;
    }
    return std::vector<NeuronId>();
}

void ProceduralMemory::updateSkillState(float dt) {
    for (auto& pair : pImpl->skills) {
        Skill& skill = pair.second;
        
        // Decay execution probability over time
        skill.executionProbability = std::max(0.01f, skill.executionProbability * (1.0f - dt * 0.01f));
        
        // Natural decay of proficiency if not practiced
        if (skill.lastExecuted > 0) {
            SimulationStep timeSinceExecution = pImpl->nextTimestamp - skill.lastExecuted;
            if (timeSinceExecution > 1000) { // Decay after 1000 steps
                skill.proficiency *= (1.0f - dt * 0.001f);
            }
        }
    }
}

float ProceduralMemory::getSkillExecutionProbability(const std::string& name) const {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return it->second.executionProbability;
    }
    return 0.0f;
}

void ProceduralMemory::recordSkillExecution(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.lastExecuted = pImpl->nextTimestamp++;
        it->second.executionCount++;
        it->second.executionProbability = std::min(1.0f, it->second.executionProbability + 0.1f);
    }
}

} // namespace nlm
