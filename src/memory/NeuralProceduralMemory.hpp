#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <memory>
#include <queue>

namespace nlm {

// Enhanced Procedural Memory: Neural skill learning and motor pattern storage
// Replaces placeholder ProceduralMemory with real neural implementation

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    // Initialize with brain reference for neural integration
    bool initialize(Brain* brain);
    
    // Learn skill from successful action
    void learnSkill(const std::string& name, const std::vector<NeuronId>& neuralPattern, 
                   const std::vector<MotorCommand>& actionSequence);
    
    // Execute best learned skill for current state
    std::pair<std::string, std::vector<MotorCommand>> executeSkill(const std::vector<float>& currentState) const;
    
    // Update skill proficiency based on success
    void updateSkillProficiency(const std::string& name, float successValue);
    
    // Get skill by name
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Get best skill for current state
    std::string getBestSkill(const std::vector<float>& currentState) const;
    
    // Get skill neural similarity
    float getSkillSimilarity(const std::string& skillA, const std::string& skillB) const;
    
    // Create skill sequence from neural patterns
    std::vector<MotorCommand> createSkillSequence(const std::vector<NeuronId>& pattern) const;
    
    // Learn from action outcome (reinforcement learning)
    void learnFromOutcome(const std::vector<NeuronId>& neuralPattern, 
                         const std::vector<MotorCommand>& actions,
                         float outcomeValue);
    
    // Get skill statistics
    struct SkillStats {
        std::string name;
        float proficiency;
        float successRate;
        size_t totalExecutions;
        size_t successfulExecutions;
        float averageOutcome;
        float lastOutcome;
        std::vector<NeuronId> dominantPattern;
    };
    
    std::vector<SkillStats> getSkillStats() const;
    
    // Clear memory and reset
    void clear();
    
    // Get capacity limits
    size_t getMaxSkills() const;
    size_t getCurrentSkillCount() const;
    
    // Integration with motor system
    void integrateWithMotorSystem(const MotorSystem* motorSystem);
    void integrateWithEnvironment(const Environment* environment);

private:
    struct Impl;
    Impl* pImpl;
};

// Motor command structure for skill sequences
struct MotorCommand {
    ActionType action;
    float parameters[3];  // e.g., intensity, duration, timing
    float confidence;
    
    MotorCommand() : action(ActionType::Wait), confidence(0.0f) {
        for (int i = 0; i < 3; ++i) parameters[i] = 0.0f;
    }
    
    MotorCommand(ActionType act, float intensity = 1.0f, float duration = 1.0f, float timing = 0.0f)
        : action(act), confidence(1.0f) {
        parameters[0] = intensity;
        parameters[1] = duration;
        parameters[2] = timing;
    }
};

} // namespace nlm