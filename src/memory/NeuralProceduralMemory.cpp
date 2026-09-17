#include "NeuralProceduralMemory.hpp"

namespace nlm {

// NeuralProceduralMemory Implementation
struct NeuralProceduralMemory::Impl {
    // Skill storage with detailed information
    std::unordered_map<std::string, Skill> skills;
    
    // Recurrent connections for skill maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections;
    
    // Practice history for adaptation
    std::vector<std::pair<std::string, SimulationStep>> practiceHistory;
    
    // Skill similarity for interference management
    std::vector<std::pair<std::string, std::string>> interferencePairs;
    
    // Skill activation counts for consolidation
    std::unordered_map<std::string, size_t> activationCounts;
};

NeuralProceduralMemory::NeuralProceduralMemory() : pImpl(new Impl), brain_(nullptr), capacity_(100) {}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    brain_ = brain;
}

void NeuralProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Check if skill already exists
    if (skills_.find(name) != skills_.end()) {
        return;
    }
    
    // Create neural pattern for the skill
    createSkillNeuralPattern(name, pattern);
    
    // Set initial proficiency
    Skill& skill = skills_[name];
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.patternActivations.resize(pattern.size(), 0.0f);
    skill.proficiency = 0.05f;  // Start with low proficiency
    skill.lastPracticed = SimulationStep(0);
    skill.successRate = 0.5f;
    
    // Create recurrent connections for maintenance
    createSkillRecurrentConnection(name);
}

void NeuralProceduralMemory::createSkillNeuralPattern(const std::string& name, const std::vector<NeuronId>& pattern) {
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.patternActivations.resize(pattern.size(), 0.0f);
    skill.proficiency = 0.0f;
    skill.lastPracticed = SimulationStep(0);
    skill.successRate = 0.0f;
    
    skills_[name] = skill;
}

std::vector<float> NeuralProceduralMemory::retrieveSkill(const std::string& name) const {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        return it->second.patternActivations;
    }
    return std::vector<float>();
}

bool NeuralProceduralMemory::hasSkill(const std::string& name) const {
    return skills_.find(name) != skills_.end();
}

std::vector<Skill> NeuralProceduralMemory::getAllSkills() const {
    std::vector<Skill> result;
    result.reserve(skills_.size());
    for (const auto& skill : skills_) {
        result.push_back(skill.second);
    }
    return result;
}

void NeuralProceduralMemory::practiceSkill(const std::string& name, bool success) {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        Skill& skill = it->second;
        skill.lastPracticed = SimulationStep(0);
        skill.successRate = (skill.successRate * 0.9f) + (success ? 1.0f : 0.0f) * 0.1f;
        skill.proficiency = std::min(1.0f, skill.proficiency + 0.01f);
        
        // Update pattern activations based on success
        if (success) {
            for (float& activation : skill.patternActivations) {
                activation = std::min(1.0f, activation + 0.05f);
            }
        }
        
        // Record practice
        pImpl->practiceHistory.emplace_back(name, SimulationStep(0));
        pImpl->activationCounts[name]++;
    }
}

float NeuralProceduralMemory::getSkillProficiency(const std::string& name) const {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        return it->second.proficiency;
    }
    return 0.0f;
}

std::vector<NeuronId> NeuralProceduralMemory::getSkillNeurons(const std::string& name) const {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        return it->second.neuralPattern;
    }
    return std::vector<NeuronId>();
}

void NeuralProceduralMemory::deleteSkill(const std::string& name) {
    skills_.erase(name);
    pImpl->activationCounts.erase(name);
}

const Skill* NeuralProceduralMemory::getSkill(const std::string& name) const {
    auto it = skills_.find(name);
    if (it != skills_.end()) {
        return &it->second;
    }
    return nullptr;
}

void NeuralProceduralMemory::createSkillRecurrentConnection(const std::string& name) {
    auto it = skills_.find(name);
    if (it != skills_.end() && !it->second.neuralPattern.empty()) {
        // Create a circular recurrent connection for skill maintenance
        for (size_t i = 0; i < it->second.neuralPattern.size(); ++i) {
            NeuronId from = it->second.neuralPattern[i];
            NeuronId to = it->second.neuralPattern[(i + 1) % it->second.neuralPattern.size()];
            pImpl->recurrentConnections.emplace_back(from, to);
        }
    }
}

void NeuralProceduralMemory::createSkillRecurrentConnections() {
    for (const auto& skill : skills_) {
        createSkillRecurrentConnection(skill.first);
    }
}

void NeuralProceduralMemory::update(TimestepDuration dt) {
    // Decay unused skills
    decayUnusedSkills();
    
    // Adapt skills based on practice outcomes
    adaptSkillsBasedOnPractice();
    
    // Consolidate skills if needed
    if (pImpl->activationCounts.size() > 50) {
        consolidate();
    }
}

void NeuralProceduralMemory::decayUnusedSkills() {
    // Decay skills that haven't been practiced recently
    SimulationStep currentTime = SimulationStep(0);
    
    for (auto it = skills_.begin(); it != skills_.end(); ) {
        if (it->second.lastPracticed > SimulationStep(100)) {
            it = skills_.erase(it);
        } else {
            ++it;
        }
    }
}

void NeuralProceduralMemory::adaptSkillsBasedOnPractice() {
    // Adapt skill patterns based on practice history
    for (auto& skillPair : skills_) {
        Skill& skill = skillPair.second;
        
        // Adjust pattern activations based on success rate
        if (skill.successRate > 0.7f) {
            for (float& activation : skill.patternActivations) {
                activation = std::min(1.0f, activation + 0.01f);
            }
        } else if (skill.successRate < 0.3f) {
            for (float& activation : skill.patternActivations) {
                activation = std::max(0.0f, activation - 0.02f);
            }
        }
    }
}

void NeuralProceduralMemory::consolidate() {
    // Move frequently used skills to permanent storage
    // This is a placeholder implementation
    std::vector<std::string> skillsToKeep;
    
    for (const auto& skill : skills_) {
        auto it = pImpl->activationCounts.find(skill.first);
        if (it != pImpl->activationCounts.end() && it->second > 5) {
            skillsToKeep.push_back(skill.first);
        }
    }
    
    // Filter skills
    std::unordered_map<std::string, Skill> filteredSkills;
    for (const auto& name : skillsToKeep) {
        filteredSkills[name] = skills_[name];
    }
    
    skills_ = filteredSkills;
    
    // Clear activation counts for consolidated skills
    std::vector<std::string> toRemove;
    for (const auto& pair : pImpl->activationCounts) {
        if (std::find(skillsToKeep.begin(), skillsToKeep.end(), pair.first) == skillsToKeep.end()) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& name : toRemove) {
        pImpl->activationCounts.erase(name);
    }
}

void NeuralProceduralMemory::clear() {
    skills_.clear();
    pImpl->recurrentConnections.clear();
    pImpl->practiceHistory.clear();
    pImpl->activationCounts.clear();
}

std::vector<std::string> NeuralProceduralMemory::getRecentlyPracticedSkills(size_t count) const {
    std::vector<std::pair<std::string, SimulationStep>> sortedHistory = pImpl->practiceHistory;
    std::sort(sortedHistory.begin(), sortedHistory.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    result.reserve(std::min(count, sortedHistory.size()));
    for (size_t i = 0; i < std::min(count, sortedHistory.size()); ++i) {
        result.push_back(sortedHistory[i].first);
    }
    
    return result;
}

std::vector<std::string> NeuralProceduralMemory::getSkillsNeedingPractice(float minProficiency) const {
    std::vector<std::string> result;
    
    for (const auto& skill : skills_) {
        if (skill.second.proficiency < minProficiency) {
            result.push_back(skill.first);
        }
    }
    
    return result;
}

float NeuralProceduralMemory::getAverageProficiency() const {
    if (skills_.empty()) {
        return 0.0f;
    }
    
    float total = 0.0f;
    for (const auto& skill : skills_) {
        total += skill.second.proficiency;
    }
    
    return total / static_cast<float>(skills_.size());
}

} // namespace nlm