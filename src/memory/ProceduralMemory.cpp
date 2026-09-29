#include "ProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

namespace nlm {

struct ProceduralMemory::Impl {
    // Skills database - learned behaviors and capabilities
    std::unordered_map<std::string, Skill> skills_;
    
    // Skill organization
    std::vector<std::string> skillCategories_;
    std::vector<std::string> learnedSequences_;
    
    // Learning mechanisms
    float learningRate_;
    float consolidationThreshold_;
    size_t maxSkills_;
    float skillDecayRate_;
    
    // Integration with brain
    Brain* brain_;
    
    Impl() : learningRate_(0.2f), consolidationThreshold_(0.6f), maxSkills_(50), skillDecayRate_(0.98f), brain_(nullptr) {}
    
    ~Impl() {
        clear();
    }
    
    // Learn new skill from neural pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = 0.0f;
        
        // Extract category from skill name
        std::string category = extractCategory(name);
        skillCategories_.push_back(category);
        
        // Store skill
        skills_[name] = skill;
        
        // Create sequence representation
        learnedSequences_.push_back(createSequenceRepresentation(pattern));
        
        NLM_LOG_INFO("ProceduralMemory: Learned skill '" + name + "' with pattern size " + std::to_string(pattern.size()));
    }
    
    // Retrieve skill by name
    Skill* getSkill(const std::string& name) {
        auto it = skills_.find(name);
        if (it != skills_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    // Get all skills (non-const for ability to modify)
    std::vector<Skill*> getAllSkills() {
        std::vector<Skill*> result;
        result.reserve(skills_.size());
        for (auto& pair : skills_) {
            result.push_back(&pair.second);
        }
        return result;
    }
    
    // Get all skills (const version)
    std::vector<const Skill*> getAllSkills() const {
        std::vector<const Skill*> result;
        result.reserve(skills_.size());
        for (const auto& pair : skills_) {
            result.push_back(&pair.second);
        }
        return result;
    }
    
    // Get all skills (const version)
    std::vector<const Skill*> getAllSkills() const {
        std::vector<const Skill*> result;
        result.reserve(skills_.size());
        for (const auto& pair : skills_) {
            result.push_back(&pair.second);
        }
        return result;
    }
    
    // Update skill proficiency based on practice/performance
    void updateProficiency(const std::string& name, float delta) {
        auto it = skills_.find(name);
        if (it != skills_.end()) {
            // Apply learning rate
            delta *= learningRate_;
            
            // Update proficiency with bounds
            it->second.proficiency = std::clamp(it->second.proficiency + delta, 0.0f, 1.0f);
            
            // Auto-consolidate if proficiency is high
            if (it->second.proficiency > consolidationThreshold_) {
                consolidate(consolidationThreshold_);
            }
            
            NLM_LOG_DEBUG("ProceduralMemory: Updated proficiency for skill '" + name + "' to " + std::to_string(it->second.proficiency));
        }
    }
    
    // Clear all skills
    void clear() {
        skills_.clear();
        skillCategories_.clear();
        learnedSequences_.clear();
        NLM_LOG_DEBUG("ProceduralMemory: Cleared all skills");
    }
    
    // Get skill by category
    std::vector<Skill*> getSkillsByCategory(const std::string& category) {
        std::vector<Skill*> result;
        
        for (auto& pair : skills_) {
            if (extractCategory(pair.first) == category) {
                result.push_back(&pair.second);
            }
        }
        
        return result;
    }
    
    // Get skills above proficiency threshold
    std::vector<Skill*> getProficientSkills(float threshold) {
        std::vector<Skill*> result;
        
        for (auto& pair : skills_) {
            if (pair.second.proficiency >= threshold) {
                result.push_back(&pair.second);
            }
        }
        
        // Sort by proficiency (descending)
        std::sort(result.begin(), result.end(),
            [](Skill* a, Skill* b) {
                return a->proficiency > b->proficiency;
            });
        
        return result;
    }
    
    // Decay skills over time (use it or lose it)
    void decaySkills() {
        for (auto& pair : skills_) {
            pair.second.proficiency *= skillDecayRate_;
        }
        
        // Remove poorly mastered skills
        consolidate(skillDecayRate_ * 0.5f);
    }
    
    // Execute skill - return neural pattern for brain to use
    std::vector<NeuronId> executeSkill(const std::string& name, float activationLevel = 1.0f) {
        auto it = skills_.find(name);
        if (it == skills_.end()) {
            NLM_LOG_WARN("ProceduralMemory: Cannot execute unknown skill '" + name + "'");
            return {};
        }
        
        // Modify pattern based on proficiency and activation
        std::vector<NeuronId> pattern = it->second.neuralPattern;
        
        if (activationLevel < 1.0f) {
            // Scale pattern strength based on activation
            // (Implementation depends on how patterns are used)
        }
        
        return pattern;
    }
    
    // Learn from successful execution
    void learnFromExecution(const std::string& name, float success, float learningRate = -1.0f) {
        if (learningRate < 0.0f) {
            learningRate = this->learningRate_ * 0.5f;
        }
        
        Skill* skill = getSkill(name);
        if (skill) {
            float proficiencyGain = success * learningRate * skill->proficiency * (1.0f - skill->proficiency);
            updateProficiency(name, proficiencyGain);
            
            NLM_LOG_DEBUG("ProceduralMemory: Learned from execution of skill '" + name + "' with success " + std::to_string(success));
        }
    }
    
    // Get skill statistics
    float getAverageProficiency() const {
        if (skills_.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& pair : skills_) {
            sum += pair.second.proficiency;
        }
        return sum / skills_.size();
    }
    
    size_t getSkillCount() const {
        return skills_.size();
    }
    
    // Consolidation (remove poor skills, reinforce good ones)
    void consolidate(float threshold) {
        if (skills_.size() <= maxSkills_ / 2) return;
        
        std::vector<std::pair<std::string, float>> skillScores;
        for (const auto& pair : skills_) {
            skillScores.emplace_back(pair.first, pair.second.proficiency);
        }
        
        // Sort by proficiency (ascending)
        std::sort(skillScores.begin(), skillScores.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            });
        
        // Remove lowest proficiency skills
        size_t toRemove = skills_.size() - maxSkills_ / 2;
        for (size_t i = 0; i < toRemove && i < skillScores.size(); ++i) {
            const auto& skillToRemove = skillScores[i].first;
            skills_.erase(skillToRemove);
            
            // Clean up from other data structures
            eraseFromVector(skillCategories_, extractCategory(skillToRemove));
            eraseFromVector(learnedSequences_, skillToRemove);
        }
        
        NLM_LOG_DEBUG("ProceduralMemory: Consolidated skills, removed " + std::to_string(toRemove) + " skills");
    }
    
    // Configuration
    void setLearningRate(float rate) {
        learningRate_ = std::max(0.0f, std::min(rate, 1.0f));
    }
    
    void setConsolidationThreshold(float threshold) {
        consolidationThreshold_ = std::max(0.0f, std::min(threshold, 1.0f));
    }
    
    void setMaxSkills(size_t max) {
        maxSkills_ = max;
    }
    
    void setSkillDecayRate(float rate) {
        skillDecayRate_ = std::max(0.0f, std::min(rate, 1.0f));
    }
    
    // Integration with brain
    void setBrain(Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("ProceduralMemory: Connected to brain");
    }
    
    Brain* getBrain() const {
        return brain_;
    }
    
    // Internal helpers
    std::string extractCategory(const std::string& name) const {
        // Extract action/verb from skill name
        std::istringstream iss(name);
        std::string word;
        if (iss >> word) {
            // Convert to lowercase for consistency
            std::string result = word;
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::tolower(c); });
            return result;
        }
        return "general";
    }
    
    std::string createSequenceRepresentation(const std::vector<NeuronId>& pattern) const {
        // Create string representation of neural pattern for storage
        std::ostringstream oss;
        oss << "pattern:";
        for (size_t i = 0; i < pattern.size(); ++i) {
            oss << pattern[i];
            if (i < pattern.size() - 1) oss << ",";
        }
        return oss.str();
    }
    
    template<typename T>
    void eraseFromVector(std::vector<T>& vec, const T& value) {
        auto it = std::find(vec.begin(), vec.end(), value);
        if (it != vec.end()) {
            vec.erase(it);
        }
    }
    
    // Analyze and suggest new skills to learn
    std::vector<std::string> getSkillsForLearning(const std::vector<std::string>& currentSkills, float minSimilarity = 0.3f) const {
        std::vector<std::string> suggestions;
        
        for (const auto& pair : skills_) {
            if (currentSkills.empty() || findSkillOverlap(pair.first, currentSkills) > minSimilarity) {
                if (pair.second.proficiency < 0.5f) {  // Only suggest low-proficiency skills
                    suggestions.push_back(pair.first);
                }
            }
        }
        
        return suggestions;
    }
    
    // Calculate skill overlap/similarity
    float findSkillOverlap(const std::string& skill1, const std::vector<std::string>& otherSkills) const {
        if (otherSkills.empty()) return 0.0f;
        
        // Simple string similarity based on category overlap
        std::string cat1 = extractCategory(skill1);
        
        int overlap = 0;
        for (const auto& other : otherSkills) {
            std::string cat2 = extractCategory(other);
            if (cat1 == cat2) overlap++;
        }
        
        return static_cast<float>(overlap) / static_cast<float>(otherSkills.size());
    }
};

// ProceduralMemory implementation

ProceduralMemory::ProceduralMemory() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("ProceduralMemory initialized with max skills " + std::to_string(pImpl->getMaxSkills()));
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    pImpl->learnSkill(name, pattern);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    return pImpl->getSkill(name);
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    return pImpl->getAllSkills();
}

const std::vector<Skill*> ProceduralMemory::getAllSkills() const {
    return pImpl->getAllSkills();
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    pImpl->updateProficiency(name, delta);
}

void ProceduralMemory::clear() {
    pImpl->clear();
}

std::vector<Skill*> ProceduralMemory::getSkillsByCategory(const std::string& category) {
    return pImpl->getSkillsByCategory(category);
}

std::vector<Skill*> ProceduralMemory::getProficientSkills(float threshold) {
    return pImpl->getProficientSkills(threshold);
}

void ProceduralMemory::decaySkills() {
    pImpl->decaySkills();
}

std::vector<NeuronId> ProceduralMemory::executeSkill(const std::string& name, float activationLevel) {
    return pImpl->executeSkill(name, activationLevel);
}

void ProceduralMemory::learnFromExecution(const std::string& name, float success, float learningRate) {
    pImpl->learnFromExecution(name, success, learningRate);
}

void ProceduralMemory::consolidate(float threshold) {
    pImpl->consolidate(threshold);
}

void ProceduralMemory::setLearningRate(float rate) {
    pImpl->setLearningRate(rate);
}

void ProceduralMemory::setConsolidationThreshold(float threshold) {
    pImpl->setConsolidationThreshold(threshold);
}

void ProceduralMemory::setMaxSkills(size_t max) {
    pImpl->setMaxSkills(max);
}

void ProceduralMemory::setSkillDecayRate(float rate) {
    pImpl->setSkillDecayRate(rate);
}

void ProceduralMemory::initialize(Brain* brain) {
    if (pImpl) {
        pImpl->setBrain(brain);
    }
}

float ProceduralMemory::getAverageProficiency() const {
    return pImpl ? pImpl->getAverageProficiency() : 0.0f;
}

size_t ProceduralMemory::getSkillCount() const {
    return pImpl ? pImpl->getSkillCount() : 0;
}

} // namespace nlm
