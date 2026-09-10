#include "ProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct ProceduralMemory::Impl {
    Brain* brain;
    
    // Skill execution feedback
    std::vector<float> executionScores;
    
    Impl() : brain(nullptr) {}
};

ProceduralMemory::ProceduralMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , decayRate_(0.01f) {
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ProceduralMemory initialized");
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : skills_) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = 0.0f;
            skill.successRate = 0.5f;
            skill.practiceCount = 0;
            return;
        }
    }
    
    Skill newSkill;
    newSkill.name = name;
    newSkill.neuralPattern = pattern;
    newSkill.proficiency = 0.0f;
    newSkill.successRate = 0.5f;
    newSkill.practiceCount = 0;
    skills_.push_back(newSkill);
    pImpl->executionScores.push_back(0.0f);
}

bool ProceduralMemory::executeSkill(const std::string& name, const std::vector<NeuronId>& inputPattern) const {
    Skill* skill = getSkill(name);
    if (!skill) return false;
    
    float similarity = computePatternSimilarity(skill->neuralPattern, inputPattern);
    float threshold = 0.3f + skill->proficiency * 0.5f;
    
    return similarity >= threshold;
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : skills_) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : skills_) {
        result.push_back(&skill);
    }
    return result;
}

std::vector<Skill*> ProceduralMemory::getSkillsByProficiency(float minProficiency, float maxProficiency) const {
    std::vector<Skill*> result;
    for (auto& skill : skills_) {
        if (skill.proficiency >= minProficiency && skill.proficiency <= maxProficiency) {
            result.push_back(&skill);
        }
    }
    return result;
}

std::vector<Skill*> ProceduralMemory::getAllSkillsSortedByProficiency() const {
    std::vector<Skill*> result;
    for (auto& skill : skills_) {
        result.push_back(&skill);
    }
    
    std::sort(result.begin(), result.end(),
              [](const Skill* a, const Skill* b) {
                  return a->proficiency > b->proficiency;
              });
    
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    Skill* skill = getSkill(name);
    if (skill) {
        skill->proficiency = std::clamp(skill->proficiency + delta, 0.0f, 1.0f);
    }
}

void ProceduralMemory::recordSuccess(const std::string& name, float score) {
    Skill* skill = getSkill(name);
    if (skill) {
        skill->practiceCount++;
        skill->successRate = (skill->successRate * (skill->practiceCount - 1) + score) / skill->practiceCount;
        skill->proficiency = std::min(skill->proficiency + score * 0.1f, 1.0f);
        
        auto it = std::find_if(pImpl->executionScores.begin(), pImpl->executionScores.end(),
                               [&name](const float& s) {
                                   // Need to map to skill index
                                   return true;  // Simplified
                               });
        if (it != pImpl->executionScores.end()) {
            *it = score;
        }
    }
}

void ProceduralMemory::practiceSkill(const std::string& name, TimestepDuration dt) {
    Skill* skill = getSkill(name);
    if (!skill) return;
    
    skill->lastPracticed = dt;
    skill->proficiency = std::min(skill->proficiency + decayRate_ * dt * 0.5f, 1.0f);
}

void ProceduralMemory::clear() {
    skills_.clear();
    pImpl->executionScores.clear();
}

float ProceduralMemory::getTotalProficiency() const {
    float total = 0.0f;
    for (const auto& skill : skills_) {
        total += skill.proficiency;
    }
    return total;
}

std::string ProceduralMemory::getMostPracticedSkill() const {
    if (skills_.empty()) return "";
    
    size_t maxCount = 0;
    std::string bestSkill;
    
    for (const auto& skill : skills_) {
        if (skill.practiceCount > maxCount) {
            maxCount = skill.practiceCount;
            bestSkill = skill.name;
        }
    }
    
    return bestSkill;
}

void ProceduralMemory::decayUnusedSkills(TimestepDuration dt) {
    float decayThreshold = dt * 0.1f;
    
    for (auto& skill : skills_) {
        if (dt - skill.lastPracticed > decayThreshold) {
            skill.proficiency *= (1.0f - decayRate_ * 0.5f);
        }
    }
}

float ProceduralMemory::computePatternSimilarity(const std::vector<NeuronId>& pattern1, const std::vector<NeuronId>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    size_t matches = 0;
    for (const auto& n1 : pattern1) {
        for (const auto& n2 : pattern2) {
            if (n1 == n2) {
                matches++;
                break;
            }
        }
    }
    
    float maxMatches = std::max(pattern1.size(), pattern2.size());
    return matches / maxMatches;
}

} // namespace nlm