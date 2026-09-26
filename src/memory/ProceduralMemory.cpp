#include "ProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    size_t maxSkills;
    
    Impl(size_t max) : maxSkills(max) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl(100)) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // TODO PHASE 2: Implement skill learning with neural pattern extraction
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = std::max(skill.proficiency, 0.0f);
            return;
        }
    }
    
    if (pImpl->skills.size() < pImpl->maxSkills) {
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = 0.0f;
        pImpl->skills.push_back(skill);
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> skillPtrs;
    for (auto& skill : pImpl->skills) {
        skillPtrs.push_back(&skill);
    }
    return skillPtrs;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency += delta;
            skill.proficiency = std::max(0.0f, std::min(skill.proficiency, 1.0f));
            break;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

} // namespace nlm