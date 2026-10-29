#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <queue>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    
    // Skill storage: name -> skill
    std::unordered_map<std::string, Skill> skills;
    
    // Skill execution statistics
    std::unordered_map<std::string, SkillStats> skillStats;
    
    // Neural pattern to skill mapping
    std::unordered_map<size_t, std::string> patternToSkill;
    
    // Skill similarity matrix
    std::unordered_map<std::string, std::unordered_map<std::string, float>> skillSimilarity;
    
    // Motor system integration
    const MotorSystem* motorSystem;
    const Environment* environment;
    
    // Learning parameters
    float learningRate = 0.1f;
    float explorationRate = 0.2f;
    float successThreshold = 0.7f;
    size_t maxSkillSequenceLength = 10;
    
    // Execution tracking
    std::queue<std::string> recentSkills;
    size_t maxRecentSkills = 10;
    
    // Max skills to store
    size_t maxSkills = 100;
    
    Impl() : brain(nullptr), motorSystem(nullptr), environment(nullptr) {}
};

NeuralProceduralMemory::NeuralProceduralMemory() : pImpl(new Impl) {}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

bool NeuralProceduralMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize ProceduralMemory without brain reference");
        return false;
    }
    
    pImpl->brain = brain;
    
    NLM_LOG_INFO("NeuralProceduralMemory initialized with brain reference");
    return true;
}

void NeuralProceduralMemory::integrateWithMotorSystem(const MotorSystem* motorSystem) {
    pImpl->motorSystem = motorSystem;
}

void NeuralProceduralMemory::integrateWithEnvironment(const Environment* environment) {
    pImpl->environment = environment;
}

void NeuralProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& neuralPattern, 
                                       const std::vector<MotorCommand>& actionSequence) {
    if (pImpl->skills.size() >= pImpl->maxSkills) {
        // Remove least used skill
        std::string leastUsedSkill;
        float minUsage = FLT_MAX;
        
        for (const auto& pair : pImpl->skillStats) {
            if (pair.second.totalExecutions < minUsage) {
                minUsage = pair.second.totalExecutions;
                leastUsedSkill = pair.first;
            }
        }
        
        if (!leastUsedSkill.empty()) {
            pImpl->skills.erase(leastUsedSkill);
            pImpl->skillStats.erase(leastUsedSkill);
            for (auto& inner : pImpl->skillSimilarity) {
                inner.erase(leastUsedSkill);
            }
        }
    }
    
    Skill skill;
    skill.name = name;
    skill.neuralPattern = neuralPattern;
    skill.proficiency = 0.5f; // Start with moderate proficiency
    
    pImpl->skills[name] = skill;
    
    // Initialize statistics
    SkillStats& stats = pImpl->skillStats[name];
    stats.name = name;
    stats.proficiency = 0.5f;
    stats.successRate = 0.0f;
    stats.totalExecutions = 0;
    stats.successfulExecutions = 0;
    stats.averageOutcome = 0.0f;
    stats.lastOutcome = 0.0f;
    stats.dominantPattern = neuralPattern;
    
    // Convert action sequence to MotorCommand format
    std::vector<MotorCommand> motorSequence;
    for (ActionType action : actionSequence) {
        motorSequence.emplace_back(action);
    }
    
    // Store sequence in skill
    // (For now, simplified - in real implementation, store full sequence)
}

std::pair<std::string, std::vector<MotorCommand>> NeuralProceduralMemory::executeSkill(
    const std::vector<float>& currentState) const {
    
    std::string bestSkill;
    float bestScore = 0.0f;
    
    for (const auto& pair : pImpl->skills) {
        float score = evaluateSkillForState(pair.first, currentState);
        if (score > bestScore) {
            bestScore = score;
            bestSkill = pair.first;
        }
    }
    
    if (bestSkill.empty() || bestScore < 0.1f) {
        return {"", std::vector<MotorCommand>()};
    }
    
    // Get skill sequence (simplified for now)
    std::vector<MotorCommand> sequence;
    
    return {bestSkill, sequence};
}

void NeuralProceduralMemory::updateSkillProficiency(const std::string& name, float successValue) {
    auto skillIt = pImpl->skills.find(name);
    if (skillIt == pImpl->skills.end()) {
        return;
    }
    
    Skill& skill = skillIt->second;
    
    // Update proficiency based on success value
    skill.proficiency = std::clamp(skill.proficiency + (successValue * pImpl->learningRate), 0.0f, 1.0f);
    
    // Update statistics
    auto& stats = pImpl->skillStats[name];
    stats.proficiency = skill.proficiency;
    stats.lastOutcome = successValue;
    stats.averageOutcome = (stats.averageOutcome * stats.totalExecutions + successValue) / 
                          std::max<size_t>(stats.totalExecutions + 1, 1);
    
    if (successValue > pImpl->successThreshold) {
        stats.successfulExecutions++;
    }
}

void NeuralProceduralMemory::learnFromOutcome(const std::vector<NeuronId>& neuralPattern, 
                                             const std::vector<MotorCommand>& actions,
                                             float outcomeValue) {
    // Find existing skill with similar pattern
    std::string matchingSkill;
    float bestSimilarity = 0.0f;
    
    for (const auto& pair : pImpl->skills) {
        float similarity = computePatternSimilarity(neuralPattern, pair.second.neuralPattern);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            matchingSkill = pair.first;
        }
    }
    
    if (!matchingSkill.empty() && bestSimilarity > 0.5f) {
        // Update existing skill
        updateSkillProficiency(matchingSkill, outcomeValue);
    } else {
        // Create new skill
        std::string skillName = generateSkillName(neuralPattern);
        learnSkill(skillName, neuralPattern, actions);
        updateSkillProficiency(skillName, outcomeValue);
    }
}

Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& pair : pImpl->skills) {
        result.push_back(&pair.second);
    }
    return result;
}

std::string NeuralProceduralMemory::getBestSkill(const std::vector<float>& currentState) const {
    std::string bestSkill;
    float bestScore = 0.0f;
    
    for (const auto& pair : pImpl->skills) {
        float score = evaluateSkillForState(pair.first, currentState);
        if (score > bestScore) {
            bestScore = score;
            bestSkill = pair.first;
        }
    }
    
    return bestScore > 0.1f ? bestSkill : "";
}

float NeuralProceduralMemory::getSkillSimilarity(const std::string& skillA, const std::string& skillB) const {
    auto itA = pImpl->skillSimilarity.find(skillA);
    if (itA != pImpl->skillSimilarity.end()) {
        auto itB = itA->second.find(skillB);
        if (itB != itA->second.end()) {
            return itB->second;
        }
    }
    return 0.0f;
}

std::vector<NeuralProceduralMemory::SkillStats> NeuralProceduralMemory::getSkillStats() const {
    std::vector<SkillStats> result;
    for (const auto& pair : pImpl->skillStats) {
        result.push_back(pair.second);
    }
    return result;
}

void NeuralProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillStats.clear();
    pImpl->patternToSkill.clear();
    pImpl->skillSimilarity.clear();
    recentSkills = std::queue<std::string>();
}

size_t NeuralProceduralMemory::getMaxSkills() const {
    return pImpl->maxSkills;
}

size_t NeuralProceduralMemory::getCurrentSkillCount() const {
    return pImpl->skills.size();
}

// Helper methods
float NeuralProceduralMemory::Impl::computePatternSimilarity(const std::vector<NeuronId>& a, const std::vector<NeuronId>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    size_t dim = std::min(a.size(), b.size());
    size_t matches = 0;
    
    std::unordered_set<NeuronId> setA(a.begin(), a.begin() + dim);
    
    for (size_t i = 0; i < dim && i < b.size(); ++i) {
        if (setA.find(b[i]) != setA.end()) {
            matches++;
        }
    }
    
    return static_cast<float>(matches) / static_cast<float>(std::max(dim, 1));
}

std::string NeuralProceduralMemory::Impl::generateSkillName(const std::vector<NeuronId>& pattern) const {
    std::stringstream ss;
    ss << "Skill_";
    
    if (!pattern.empty()) {
        ss << "N" << pattern[0];
        for (size_t i = 1; i < std::min<size_t>(pattern.size(), 3); ++i) {
            ss << "_" << pattern[i];
        }
    }
    
    ss << "_" << skills.size();
    
    return ss.str();
}

std::vector<MotorCommand> NeuralProceduralMemory::createSkillSequence(const std::vector<NeuronId>& pattern) const {
    // Create action sequence based on neural pattern
    std::vector<MotorCommand> sequence;
    
    for (size_t i = 0; i < pattern.size() && i < 10; ++i) {
        NeuronId neuron = pattern[i];
        
        // Convert neuron ID to action (simplified mapping)
        ActionType action;
        if (neuron % 6 == 0) action = ActionType::MoveForward;
        else if (neuron % 6 == 1) action = ActionType::MoveBackward;
        else if (neuron % 6 == 2) action = ActionType::TurnLeft;
        else if (neuron % 6 == 3) action = ActionType::TurnRight;
        else if (neuron % 6 == 4) action = ActionType::LookLeft;
        else if (neuron % 6 == 5) action = ActionType::LookRight;
        else action = ActionType::Interact;
        
        sequence.emplace_back(action);
    }
    
    return sequence;
}

float NeuralProceduralMemory::evaluateSkillForState(const std::string& skillName, const std::vector<float>& currentState) const {
    auto skillIt = pImpl->skills.find(skillName);
    if (skillIt == pImpl->skills.end()) {
        return 0.0f;
    }
    
    const Skill& skill = skillIt->second;
    const auto& stats = pImpl->skillStats.at(skillName);
    
    // Multiple factors for skill selection
    float proficiencyWeight = 0.5f;
    float similarityWeight = 0.3f;
    float recencyWeight = 0.2f;
    
    float proficiencyScore = skill.proficiency;
    float similarityScore = computePatternSimilarity(skill.neuralPattern, convertStateToPattern(currentState));
    
    // Recency score based on recent usage
    float recencyScore = 0.0f;
    std::queue<std::string> tempQueue = recentSkills;
    size_t position = 0;
    while (!tempQueue.empty()) {
        if (tempQueue.front() == skillName) {
            recencyScore = 1.0f - (position / static_cast<float>(maxRecentSkills));
            break;
        }
        tempQueue.pop();
        position++;
    }
    
    float totalScore = proficiencyScore * proficiencyWeight + 
                      similarityScore * similarityWeight + 
                      recencyScore * recencyWeight;
    
    return totalScore;
}

std::vector<NeuronId> NeuralProceduralMemory::Impl::convertStateToPattern(const std::vector<float>& state) const {
    // Convert state vector to neural pattern (simplified)
    std::vector<NeuronId> pattern;
    pattern.reserve(state.size() / 10); // Downsample
    
    for (size_t i = 0; i < state.size(); i += 10) {
        if (i < state.size()) {
            NeuronId neuron = static_cast<NeuronId>(state[i] * 1000.0f) % 10000; // Scale and modulo
            pattern.push_back(neuron);
        }
    }
    
    return pattern;
}

} // namespace nlm