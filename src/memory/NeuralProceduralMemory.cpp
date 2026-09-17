#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    struct SkillData {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        float effectiveness;  // Recent performance metric
        float lastExecuted;
        size_t executionCount;
        
        SkillData() : proficiency(0.0f), effectiveness(0.5f), lastExecuted(0), executionCount(0) {}
    };
    
    std::vector<SkillData> skills;
    float decayRate;
    
    Impl() : decayRate(0.01f) {}
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

void NeuralProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& neuralPattern) {
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = neuralPattern;
            skill.proficiency = 0.3f;  // Initial learning
            NLM_LOG_INFO("Updated procedural skill: " + name);
            return;
        }
    }
    
    // Add new skill
    Impl::SkillData skill;
    skill.name = name;
    skill.neuralPattern = neuralPattern;
    skill.proficiency = 0.3f;  // Initial learning
    pImpl->skills.push_back(skill);
    
    NLM_LOG_INFO("Learned procedural skill: " + name);
}

void NeuralProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            skill.effectiveness = std::clamp(skill.effectiveness + delta * 0.5f, 0.0f, 1.0f);
            NLM_LOG_INFO("Updated proficiency of skill: " + name + " to " + std::to_string(skill.proficiency));
            return;
        }
    }
}

void NeuralProceduralMemory::updateSkillExecution(const std::string& name, float effectiveness) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.effectiveness = effectiveness;
            skill.lastExecuted = 0;  // Will be updated by brain
            skill.executionCount++;
            
            // Increase proficiency based on execution effectiveness
            skill.proficiency = std::min(1.0f, skill.proficiency + effectiveness * 0.1f);
            break;
        }
    }
}

Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Return as pointer to underlying Skill structure
            static Skill tempSkill;
            tempSkill.name = skill.name;
            tempSkill.neuralPattern = skill.neuralPattern;
            tempSkill.proficiency = skill.proficiency;
            return &tempSkill;
        }
    }
    return nullptr;
}

std::vector<Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    
    for (auto& skill : pImpl->skills) {
        Skill* s = getSkill(skill.name);
        if (s) {
            result.push_back(s);
        }
    }
    
    return result;
}

std::vector<Skill*> NeuralProceduralMemory::getSkillsOrderedByProficiency() const {
    std::vector<Skill*> result;
    
    for (const auto& skill : pImpl->skills) {
        Skill* s = getSkill(skill.name);
        if (s) {
            result.push_back(s);
        }
    }
    
    // Sort by proficiency (highest first)
    std::sort(result.begin(), result.end(), 
        [](const Skill* a, const Skill* b) {
            return a->proficiency > b->proficiency;
        });
    
    return result;
}

void NeuralProceduralMemory::clear() {
    pImpl->skills.clear();
    NLM_LOG_INFO("NeuralProceduralMemory cleared");
}

} // namespace nlm
