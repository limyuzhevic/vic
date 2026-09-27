#include "NeuralProceduralMemory.hpp"
#include <algorithm>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    std::vector<Skill> skills;
    Brain* brain;
};

NeuralProceduralMemory::NeuralProceduralMemory() : pImpl(std::make_unique<Impl>()) {}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void NeuralProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Check if skill already exists
    auto it = std::find_if(pImpl->skills.begin(), pImpl->skills.end(),
                          [&name](const Skill& skill) { return skill.name == name; });
    
    if (it != pImpl->skills.end()) {
        it->neuralPattern = pattern;
        it->proficiency = 0.0f; // Reset proficiency when relearned
    } else {
        pImpl->skills.emplace_back(name, pattern);
    }
}

Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    auto it = std::find_if(pImpl->skills.begin(), pImpl->skills.end(),
                          [&name](const Skill& skill) { return skill.name == name; });
    return (it != pImpl->skills.end()) ? &(*it) : nullptr;
}

std::vector<Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skills.size());
    
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    
    return result;
}

void NeuralProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = std::find_if(pImpl->skills.begin(), pImpl->skills.end(),
                          [&name](const Skill& skill) { return skill.name == name; });
    
    if (it != pImpl->skills.end()) {
        it->proficiency += delta;
        // Clamp proficiency to [0, 1] range
        it->proficiency = std::clamp(it->proficiency, 0.0f, 1.0f);
    }
}

void NeuralProceduralMemory::clear() {
    pImpl->skills.clear();
}

} // namespace nlm
