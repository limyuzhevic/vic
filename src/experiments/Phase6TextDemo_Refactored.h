// Refactored Phase 6 Text Demo - Modular Implementation
// Part 1: Core infrastructure and common utilities
// This file contains the main structure and helper functions
// 
// Original file: Phase6TextDemo.cpp (426 lines)
// Refactored into: 5 focused modules for better maintainability

#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace nlm {
namespace text_demo {

// Constants for configuration (replacing magic numbers)
constexpr float TEXT_ENCODING_GAIN = 0.8f;
constexpr float TEXT_SIMILARITY_THRESHOLD = 0.05f;
constexpr float RESOURCE_SCENE_BRIGHTNESS = 0.7f;
constexpr float HAZARD_SCENE_DARKNESS = 0.75f;
constexpr float MIN_INTERACTION_THRESHOLD = 0.05f;
constexpr float CONCEPT_FORMATION_REWARD_STRENGTH = 10.0f;
constexpr float CONCEPT_FORMATION_PENALTY_STRENGTH = -15.0f;
constexpr int TEACHER_DEMO_ROUNDS = 5;
constexpr int TRAINING_ROUNDS = 300;
constexpr int SANDBOX_STEPS = 100;
constexpr float SANDBOX_WORLD_STEP = 0.1f;

// Helper function to get command name from motor command
const char* getCommandName(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        default: return "Wait";
    }
}

// Helper function to convert developmental stage to string
const char* getStageName(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

// Create default configuration
std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    return config;
}

// Create sensory percept with vision, touch, and optional text
SensoryPercept makePercept(const std::vector<float>& vision,
                           const std::vector<float>& touch,
                           const std::string& text = "") {
    SensoryPercept p;
    p.setVision(vision);
    p.setTouch(touch);
    p.setInternal({1.0f, 1.0f, 0.0f, 0.0f});
    p.setProprioception({0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f});
    p.setAudio({});
    p.setText(text);
    return p;
}

// Present a percept for specified actions and return internal firing rates
std::vector<float> presentAndRead(AgentBrain& agent,
                                   const std::shared_ptr<Brain>& brain,
                                   const SensoryPercept& percept,
                                   int actions, uint64_t& step) {
    for (int a = 0; a < actions; ++a) {
        agent.processSensoryInput(percept);
        for (int i = 0; i < 10; ++i) brain->step(step++);
    }
    return agent.getInternalFiringRates();
}

// Compute cosine similarity between two vectors
float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    size_t n = std::min(a.size(), b.size());
    float dot = 0.0f, na = 0.0f, nb = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        dot += a[i] * b[i];
        na += a[i] * a[i];
        nb += b[i] * b[i];
    }
    if (na < 1e-9f || nb < 1e-9f) return 0.0f;
    return dot / (std::sqrt(na) * std::sqrt(nb));
}

// Subtract vector b from vector a
std::vector<float> vectorSubtract(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> out;
    size_t n = std::min(a.size(), b.size());
    out.reserve(n);
    for (size_t i = 0; i < n; ++i) out.push_back(a[i] - b[i]);
    return out;
}

// Check if text learning was successful
bool checkTextLearning(float simTextAfter, float simTextBefore,
                      float simControlAfter, float simControlBefore) {
    return (simTextAfter > simTextBefore + TEXT_SIMILARITY_THRESHOLD) &&
           (simTextAfter > simControlAfter + TEXT_SIMILARITY_THRESHOLD);
}

// Check if differential learning was specific (word -> scene)
bool checkDifferentialLearning(float appleResource, float appleHazard,
                              float zebraHazard, float zebraResource) {
    return (appleResource > appleHazard + TEXT_SIMILARITY_THRESHOLD) &&
           (zebraHazard > zebraResource + TEXT_SIMILARITY_THRESHOLD);
}

// Get similarity between text pattern and scene pattern
float getTextSceneSimilarity(AgentBrain& agent, const std::shared_ptr<Brain>& brain,
                            const std::vector<float>& textPattern,
                            const std::vector<float>& scenePattern,
                            const std::vector<float>& emptyBaseline,
                            uint64_t& step) {
    auto darkVision = std::vector<float>(16, 0.0f);
    auto textPercept = makePercept(darkVision, {}, "");
    auto scenePercept = makePercept(scenePattern, {});
    
    auto textPatternVec = presentAndRead(agent, brain, textPercept, 50, step);
    auto scenePatternVec = presentAndRead(agent, brain, scenePercept, 50, step);
    
    float textDiff = cosineSimilarity(vectorSubtract(textPatternVec, vectorSubtract(emptyBaseline, vectorSubtract)), vectorSubtract(scenePatternVec, vectorSubtract(emptyBaseline, vectorSubtract)));
    return textDiff;
}

} // namespace text_demo
} // namespace nlm