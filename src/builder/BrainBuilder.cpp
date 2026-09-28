// BrainBuilder.cpp - Implementation of the BrainBuilder factory pattern
#include "BrainBuilder.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/core/Logger/Logger.hpp"

namespace nlm {

BrainBuilder& BrainBuilder::withConfig(std::shared_ptr<Config> config) {
    config_ = config;
    return *this;
}

BrainBuilder& BrainBuilder::withNeuronCount(size_t neuronCount) {
    neuronCount_ = neuronCount;
    return *this;
}

BrainBuilder& BrainBuilder::withRegionCount(size_t regionCount) {
    regionCount_ = regionCount;
    return *this;
}

BrainBuilder& BrainBuilder::withConnectionProbability(float probability) {
    connectionProbability_ = probability;
    return *this;
}

BrainBuilder& BrainBuilder::addCustomConfig(const std::string& key, const std::string& value) {
    customConfigs_[key] = value;
    return *this;
}

BrainBuilder& BrainBuilder::addCustomConfig(const std::string& key, int value) {
    customConfigs_[key] = std::to_string(value);
    return *this;
}

BrainBuilder& BrainBuilder::addCustomConfig(const std::string& key, float value) {
    customConfigs_[key] = std::to_string(value);
    return *this;
}

BrainBuilder& BrainBuilder::addCustomConfig(const std::string& key, double value) {
    customConfigs_[key] = std::to_string(value);
    return *this;
}

BrainBuilder& BrainBuilder::withWorldSize(size_t width, size_t height) {
    worldWidth_ = width;
    worldHeight_ = height;
    return *this;
}

BrainBuilder& BrainBuilder::withVisionSize(size_t visionWidth, size_t visionHeight) {
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    return *this;
}

BrainBuilder& BrainBuilder::withDevelopmentStage(const std::string& stage) {
    customConfigs_["development_stage"] = stage;
    return *this;
}

BrainBuilder& BrainBuilder::enableRewardModulation(bool enable) {
    rewardModulationEnabled_ = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableStructuralPlasticity(bool enable) {
    structuralPlasticityEnabled_ = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableDevelopment(bool enable) {
    developmentEnabled_ = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableCuriosity(bool enable) {
    curiosityEnabled_ = enable;
    return *this;
}

std::shared_ptr<Brain> BrainBuilder::buildBrain() {
    // Create default config if none provided
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    
    // Set basic configuration
    config_->set<size_t>("neuron_count", neuronCount_);
    config_->set<size_t>("region_count", regionCount_);
    config_->set<float>("connection_probability", connectionProbability_);
    
    // Add custom configurations
    for (const auto& pair : customConfigs_) {
        // Try to parse as different types
        try {
            // Try integer
            size_t intVal = std::stoul(pair.second);
            config_->set<size_t>(pair.first, intVal);
        } catch (...) {
            try {
                // Try float
                float floatVal = std::stof(pair.second);
                config_->set<float>(pair.first, floatVal);
            } catch (...) {
                // Try double
                try {
                    double doubleVal = std::stod(pair.second);
                    config_->set<double>(pair.first, doubleVal);
                } catch (...) {
                    // Fall back to string
                    config_->set<std::string>(pair.first, pair.second);
                }
            }
        }
    }
    
    // Set world configurations
    config_->set<size_t>("world_width", worldWidth_);
    config_->set<size_t>("world_height", worldHeight_);
    config_->set<size_t>("vision_width", visionWidth_);
    config_->set<size_t>("vision_height", visionHeight_);
    
    // Set development flags
    config_->set<bool>("reward_modulation_enabled", rewardModulationEnabled_);
    config_->set<bool>("structural_plasticity_enabled", structuralPlasticityEnabled_);
    config_->set<bool>("development_enabled", developmentEnabled_);
    config_->set<bool>("curiosity_enabled", curiosityEnabled_);
    
    NLM_LOG_INFO("Building Brain with configuration: " + getConfigSummary());
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config_);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize Brain");
        return nullptr;
    }
    
    return brain;
}

std::shared_ptr<AgentBrain> BrainBuilder::buildAgentBrain(std::shared_ptr<SimpleWorld> world) {
    auto brain = buildBrain();
    if (!brain) {
        return nullptr;
    }
    
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    if (world) {
        agentBrain->initialize(*world);
    }
    
    return agentBrain;
}

void BrainBuilder::reset() {
    config_.reset();
    customConfigs_.clear();
    neuronCount_ = 1000;
    regionCount_ = 1;
    connectionProbability_ = 0.1f;
    worldWidth_ = 100;
    worldHeight_ = 100;
    visionWidth_ = 16;
    visionHeight_ = 16;
    rewardModulationEnabled_ = true;
    structuralPlasticityEnabled_ = true;
    developmentEnabled_ = true;
    curiosityEnabled_ = true;
}

std::string BrainBuilder::getConfigSummary() const {
    std::stringstream ss;
    ss << "Brain Configuration: "
       << "neurons=" << neuronCount_ << ", "
       << "regions=" << regionCount_ << ", "
       << "connection_prob=" << connectionProbability_ << ", "
       << "world_size=" << worldWidth_ << "x" << worldHeight_ << ", "
       << "vision_size=" << visionWidth_ << "x" << visionHeight_;
    if (!customConfigs_.empty()) {
        ss << ", custom=" << customConfigs_.size();
    }
    return ss.str();
}

// Convenience functions
std::shared_ptr<Brain> createDefaultBrain() {
    return BrainBuilder()
        .withNeuronCount(1000)
        .withRegionCount(1)
        .withConnectionProbability(0.1f)
        .enableRewardModulation(true)
        .enableStructuralPlasticity(true)
        .enableDevelopment(true)
        .enableCuriosity(true)
        .buildBrain();
}

std::shared_ptr<Brain> createMinimalBrain(size_t neuronCount) {
    return BrainBuilder()
        .withNeuronCount(neuronCount)
        .withRegionCount(1)
        .withConnectionProbability(0.05f)
        .enableDevelopment(false)
        .enableCuriosity(false)
        .buildBrain();
}

std::shared_ptr<Brain> createDevelopmentBrain() {
    return BrainBuilder()
        .withNeuronCount(2000)
        .withRegionCount(2)
        .withConnectionProbability(0.15f)
        .withDevelopmentStage("CriticalPeriod")
        .enableRewardModulation(true)
        .enableStructuralPlasticity(true)
        .enableDevelopment(true)
        .enableCuriosity(true)
        .withWorldSize(200, 200)
        .withVisionSize(32, 32)
        .buildBrain();
}

std::shared_ptr<Brain> createReinforcementBrain() {
    return BrainBuilder()
        .withNeuronCount(1500)
        .withRegionCount(2)
        .withConnectionProbability(0.2f)
        .enableRewardModulation(true)
        .enableStructuralPlasticity(true)
        .enableDevelopment(false)
        .enableCuriosity(true)
        .withDevelopmentStage("Adult")
        .buildBrain();
}