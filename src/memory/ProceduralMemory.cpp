#include "ProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <random>

namespace nlm {

struct ProceduralMemory::Impl {
    std::priority_queue<std::pair<float, std::string>> proficientSkills;
    std::unordered_map<size_t, std::vector<NeuronId>> normalizedPatterns;
    std::mt19937 rng;
};

ProceduralMemory::ProceduralMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextSkillId_(0)
{
    // Initialize random number generator
    std::random_device rd;
    pImpl->rng.seed(rd());
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    nextSkillId_ = 0;
    
    std::random_device rd;
    pImpl->rng.seed(rd());
    
    NLM_LOG_INFO("ProceduralMemory initialized");
}

void ProceduralMemory::learnSkill(const std::string& name,
                                  const std::vector<NeuronId>& neuralPattern,
                                  float initialProficiency) {
    if (name.empty()) return;
    
    Skill skill;
    skill.name = name;
    skill.neuralPattern = neuralPattern;
    skill.proficiency = std::max(0.0f, std::min(1.0f, initialProficiency));
    
    skills_[name] = skill;
    SkillStats& stats = skillStats_[name];
    stats.executions = 0;
    stats.lastExecuted = 0;
    stats.totalPracticeQuality = 0.0f;
    
    // Store normalized pattern
    pImpl->normalizedPatterns[name] = normalizePattern(neuralPattern);
    
    // Add to priority queue if proficient enough
    if (skill.proficiency > 0.5f) {
        pImpl->proficientSkills.push({skill.proficiency, name});
    }
    
    NLM_LOG_INFO("Learned skill: " + name + " (pattern size: " + std::to_string(neuralPattern.size()) + ", proficiency: " + std::to_string(skill.proficiency) + ")");
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> skillPtrs;
    skillPtrs.reserve(skills_.size());
    
    for (auto& pair : skills_) {
        skillPtrs.push_back(&pair.second);
    }
    
    return skillPtrs;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        it->second.proficiency = std::max(0.0f, std::min(1.0f, it->second.proficiency + delta));
        
        // Update priority queue
        pImpl->proficientSkills = std::priority_queue<std::pair<float, std::string>>();
        for (const auto& pair : skills_) {
            if (pair.second.proficiency > 0.5f) {
                pImpl->proficientSkills.push({pair.second.proficiency, pair.first});
            }
        }
    }
}

std::vector<NeuronId> ProceduralMemory::executeSkill(const std::string& name) {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        // Update execution stats
        SkillStats& stats = skillStats_[name];
        stats.executions++;
        stats.lastExecuted = nextSkillId_++;
        
        NLM_LOG_INFO("Executing skill: " + name + " (execution count: " + std::to_string(stats.executions) + ")");
        
        return it->second.neuralPattern;
    }
    return std::vector<NeuronId>();
}

bool ProceduralMemory::hasSkill(const std::string& name) const {
    return skills_.find(name) != skills_.end();
}

void ProceduralMemory::clear() {
    skills_.clear();
    skillStats_.clear();
    pImpl->normalizedPatterns.clear();
    pImpl->proficientSkills = std::priority_queue<std::pair<float, std::string>>();
    NLM_LOG_INFO("ProceduralMemory cleared");
}

float ProceduralMemory::getAverageProficiency() const {
    if (skills_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& pair : skills_) {
        total += pair.second.proficiency;
    }
    
    return total / skills_.size();
}

std::vector<std::string> ProceduralMemory::findSimilarSkills(const std::vector<NeuronId>& pattern,
                                                              size_t maxResults) const {
    std::vector<std::pair<std::string, float>> similarities;
    similarities.reserve(skills_.size());
    
    std::vector<float> normalizedPattern = normalizePattern(pattern);
    
    for (const auto& pair : skills_) {
        auto normIt = pImpl->normalizedPatterns.find(pair.first);
        if (normIt != pImpl->normalizedPatterns.end()) {
            float similarity = computePatternSimilarity(normalizedPattern, normIt->second);
            similarities.push_back({pair.first, similarity});
        }
    }
    
    std::partial_sort(similarities.begin(),
                     std::min(similarities.begin() + maxResults, similarities.end()),
                     similarities.end(),
                     [](const auto& a, const auto& b) { return a.second > b.second; });
    
    similarities.resize(std::min(similarities.size(), maxResults));
    
    std::vector<std::string> similarSkills;
    similarSkills.reserve(similarities.size());
    
    for (const auto& pair : similarities) {
        similarSkills.push_back(pair.first);
    }
    
    return similarSkills;
}

void ProceduralMemory::transferSkill(const std::string& sourceSkill,
                                     const std::string& targetSkill,
                                     float transferStrength) {
    if (!hasSkill(sourceSkill)) return;
    
    // Create blended pattern
    auto sourceIt = skills_.find(sourceSkill);
    if (sourceIt == skills_.end()) return;
    
    std::vector<NeuronId> sourcePattern = sourceIt->second.neuralPattern;
    
    // Add some noise/random variation to make it different but related
    std::vector<NeuronId> targetPattern = sourcePattern;
    for (size_t i = 0; i < targetPattern.size(); ++i) {
        if (pImpl->rng() % 100 < static_cast<int>(transferStrength * 100)) {
            // Modify neuron slightly (within bounds)
            NeuronId original = targetPattern[i];
            NeuronId newId = static_cast<NeuronId>((original + pImpl->rng() % 100) % 10000);
            targetPattern[i] = newId;
        }
    }
    
    // Learn the target skill with high proficiency (transfer makes it easier)
    float proficiency = std::max(0.5f, sourceIt->second.proficiency * transferStrength * 2.0f);
    learnSkill(targetSkill, targetPattern, proficiency);
    
    NLM_LOG_INFO("Transferred skill: " + sourceSkill + " -> " + targetSkill + " (strength: " + std::to_string(transferStrength) + ")");
}

std::vector<std::string> ProceduralMemory::getSkillsByProficiency(float minProficiency) const {
    std::vector<std::string> proficientSkills;
    
    for (const auto& pair : skills_) {
        if (pair.second.proficiency >= minProficiency) {
            proficientSkills.push_back(pair.first);
        }
    }
    
    // Sort by proficiency (descending)
    std::sort(proficientSkills.begin(), proficientSkills.end(),
              [this](const std::string& a, const std::string& b) {
                  return skills_.at(a).proficiency > skills_.at(b).proficiency;
              });
    
    return proficientSkills;
}

void ProceduralMemory::practiceSkill(const std::string& name, float practiceQuality) {
    auto it = skills_.find(name);
    if (it == skills_.end()) return;
    
    // Increase proficiency based on practice quality
    float proficiencyGain = practiceQuality * 0.1f; // 10% of practice quality per practice session
    updateProficiency(name, proficiencyGain);
    
    // Update practice stats
    SkillStats& stats = skillStats_[name];
    stats.totalPracticeQuality += practiceQuality;
    
    NLM_LOG_INFO("Practiced skill: " + name + " (quality: " + std::to_string(practiceQuality) + ", new proficiency: " + std::to_string(it->second.proficiency) + ")");
}

size_t ProceduralMemory::getSkillExecutions(const std::string& name) const {
    auto it = skillStats_.find(name);
    if (it != skillStats_.end()) {
        return it->second.executions;
    }
    return 0;
}

float ProceduralMemory::computePatternSimilarity(const std::vector<NeuronId>& patternA,
                                                 const std::vector<NeuronId>& patternB) const {
    if (patternA.size() != patternB.size()) {
        // Different sizes - normalize to min size
        size_t minSize = std::min(patternA.size(), patternB.size());
        float matches = 0.0f;
        
        for (size_t i = 0; i < minSize; ++i) {
            if (patternA[i] == patternB[i]) {
                matches++;
            }
        }
        
        return matches / static_cast<float>(minSize);
    } else {
        // Same size - direct comparison
        float matches = 0.0f;
        for (size_t i = 0; i < patternA.size(); ++i) {
            if (patternA[i] == patternB[i]) {
                matches++;
            }
        }
        return matches / static_cast<float>(patternA.size());
    }
}

std::vector<float> ProceduralMemory::normalizePattern(const std::vector<NeuronId>& pattern) const {
    std::vector<float> normalized;
    normalized.reserve(pattern.size());
    
    // Convert neuron IDs to normalized values (0-1 range)
    float maxId = 10000.0f; // Assuming neuron IDs are less than 10000
    for (const auto& neuronId : pattern) {
        normalized.push_back(static_cast<float>(neuronId) / maxId);
    }
    
    return normalized;
}

} // namespace nlm
