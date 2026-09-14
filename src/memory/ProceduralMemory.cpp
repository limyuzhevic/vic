#include "ProceduralMemory.hpp"

namespace nlm {

struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    std::unordered_map<std::string, size_t> skillIndex;
    
    Impl() {
        // Pre-populate with some basic skills
        Skill basicLearning;
        basicLearning.name = "basic_learning";
        basicLearning.proficiency = 0.5f;
        skills.push_back(basicLearning);
        skillIndex["basic_learning"] = 0;
    }
};

ProceduralMemory::ProceduralMemory() : pImpl(std::make_unique<Impl>()) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    if (pImpl->skillIndex.find(name) == pImpl->skillIndex.end()) {
        Skill newSkill;
        newSkill.name = name;
        newSkill.neuralPattern = pattern;
        newSkill.proficiency = 0.0f;
        pImpl->skills.push_back(newSkill);
        pImpl->skillIndex[name] = pImpl->skills.size() - 1;
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skillIndex.find(name);
    if (it != pImpl->skillIndex.end() && it->second < pImpl->skills.size()) {
        return &pImpl->skills[it->second];
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = pImpl->skillIndex.find(name);
    if (it != pImpl->skillIndex.end() && it->second < pImpl->skills.size()) {
        Skill& skill = pImpl->skills[it->second];
        skill.proficiency += delta;
        skill.proficiency = std::max(0.0f, std::min(1.0f, skill.proficiency));
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillIndex.clear();
}

} // namespace nlm
