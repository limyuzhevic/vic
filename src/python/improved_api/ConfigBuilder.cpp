#include "NLMImprovedAPI.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

struct ConfigBuilder::Impl {
    // Configuration values that will be set
    int neuron_count;
    float gain;
    float threshold;
    float synapse_density;
    float weight_mean;
    float weight_std;
    bool stdp_enabled;
    bool hebbian_enabled;
    bool structural_enabled;
    float stdp_learning_rate;
    float hebbian_strength;
    std::string development_stage;
    double development_age_rate;
    bool critical_period_enabled;
    bool curiosity_enabled;
    bool novelty_enabled;
    bool reward_enabled;
    float dopamine_scale;
    bool working_memory_enabled;
    bool episodic_memory_enabled;
    float memory_capacity;
    bool prediction_enabled;
    float prediction_horizon;
    float prediction_error_weight;
    int world_width;
    int world_height;
    int vision_width;
    int vision_height;
    float vision_range;
    bool agent_learning_enabled;
    bool agent_planning_enabled;
    bool agent_social_enabled;
    
    Impl() : neuron_count(1000), gain(1.0f), threshold(-50.0f),
             synapse_density(0.1f), weight_mean(0.5f), weight_std(0.1f),
             stdp_enabled(true), hebbian_enabled(true), structural_enabled(true),
             stdp_learning_rate(0.001f), hebbian_strength(0.5f),
             development_stage("Research"), development_age_rate(1.0), critical_period_enabled(true),
             curiosity_enabled(true), novelty_enabled(true), reward_enabled(true), dopamine_scale(1.0f),
             working_memory_enabled(true), episodic_memory_enabled(true), memory_capacity(1000.0f),
             prediction_enabled(true), prediction_horizon(0.1f), prediction_error_weight(1.0f),
             world_width(20), world_height(20), vision_width(8), vision_height(8), vision_range(1.0f),
             agent_learning_enabled(true), agent_planning_enabled(true), agent_social_enabled(false) {}
};

ConfigBuilder::ConfigBuilder() : pImpl(std::make_unique<Impl>()) {}

ConfigBuilder& ConfigBuilder::with_neural_params(int neuron_count, float gain, float threshold) {
    pImpl->neuron_count = neuron_count;
    pImpl->gain = gain;
    pImpl->threshold = threshold;
    return *this;
}

ConfigBuilder& ConfigBuilder::with_synapse_params(float density, float weight_mean, float weight_std) {
    pImpl->synapse_density = density;
    pImpl->weight_mean = weight_mean;
    pImpl->weight_std = weight_std;
    return *this;
}

ConfigBuilder& ConfigBuilder::with_plasticity(bool enable_stdp, bool enable_hebbian,
                                               bool enable_structural, float stdp_learning_rate,
                                               float hebbian_strength) {
    pImpl->stdp_enabled = enable_stdp;
    pImpl->hebbian_enabled = enable_hebbian;
    pImpl->structural_enabled = enable_structural;
    pImpl->stdp_learning_rate = stdp_learning_rate;
    pImpl->hebbian_strength = hebbian_strength;
    return *this;
}

ConfigBuilder& ConfigBuilder::with_development(const std::string& stage, double age_rate,
                                               bool enable_critical_period) {
    pImpl->development_stage = stage;
    pImpl->development_age_rate = age_rate;
    pImpl->critical_period_enabled = enable_critical_period;
    return *this;
}

ConfigBuilder& ConfigBuilder::with_neuromodulation(bool enable_curiosity, bool enable_novelty,
                                                    bool enable_reward, float dopamine_scale) {
    pImpl->curiosity_enabled = enable_curiosity;
    pImpl->novelty_enabled = enable_novelty;
    pImpl->reward_enabled = enable_reward;
    pImpl->dopamine_scale = dopamine_scale;
    return *this;
}

ConfigBuilder& ConfigBuilder::with_memory(bool enable_working_memory, bool enable_episodic_memory,
                                          float memory_capacity) {
    pImpl->working_memory_enabled = enable_working_memory;
    pImpl->episodic_memory_enabled = enable_episodic_memory;
    pImpl->memory_capacity = memory_capacity;
    return *this;
}

ConfigBuilder& ConfigBuilder::with_prediction(bool enable_prediction, float prediction_horizon,
                                              float prediction_error_weight) {
    pImpl->prediction_enabled = enable_prediction;
    pImpl->prediction_horizon = prediction_horizon;
    pImpl->prediction_error_weight = prediction_error_weight;
    return *this;
}

ConfigBuilder& ConfigBuilder::with_world(int width, int height, int vision_width, int vision_height,
                                         float vision_range) {
    pImpl->world_width = width;
    pImpl->world_height = height;
    pImpl->vision_width = vision_width;
    pImpl->vision_height = vision_height;
    pImpl->vision_range = vision_range;
    return *this;
}

ConfigBuilder& ConfigBuilder::with_agent(bool enable_learning, bool enable_planning,
                                         bool enable_social) {
    pImpl->agent_learning_enabled = enable_learning;
    pImpl->agent_planning_enabled = enable_planning;
    pImpl->agent_social_enabled = enable_social;
    return *this;
}

std::shared_ptr<Config> ConfigBuilder::build() {
    auto config = std::make_shared<Config>();
    
    // Set neural parameters
    config->set("brain.neuron_count", static_cast<int64_t>(pImpl->neuron_count));
    config->set("brain.gain", pImpl->gain);
    config->set("brain.threshold", pImpl->threshold);
    
    // Set synapse parameters
    config->set("brain.synapse_density", pImpl->synapse_density);
    config->set("brain.initial_weight_mean", pImpl->weight_mean);
    config->set("brain.initial_weight_std", pImpl->weight_std);
    
    // Set plasticity parameters
    config->set("plasticity.stdp.enable", pImpl->stdp_enabled);
    config->set("plasticity.stdp.learning_rate", pImpl->stdp_learning_rate);
    config->set("plasticity.hebbian.enable", pImpl->hebbian_enabled);
    config->set("plasticity.structural.enable", pImpl->structural_enabled);
    
    // Set development parameters
    config->set("development.stage", pImpl->development_stage);
    config->set("development.age_rate", pImpl->development_age_rate);
    config->set("development.critical_period.enabled", pImpl->critical_period_enabled);
    
    // Set neuromodulation parameters
    config->set("neuromod.dopamine.scale", pImpl->dopamine_scale);
    config->set("neuromod.curiosity.enable", pImpl->curiosity_enabled);
    config->set("neuromod.novelty.enable", pImpl->novelty_enabled);
    config->set("neuromod.reward.enable", pImpl->reward_enabled);
    
    // Set memory parameters
    config->set("memory.working.enable", pImpl->working_memory_enabled);
    config->set("memory.episodic.enable", pImpl->episodic_memory_enabled);
    config->set("memory.capacity", pImpl->memory_capacity);
    
    // Set prediction parameters
    config->set("prediction.enabled", pImpl->prediction_enabled);
    config->set("prediction.horizon", pImpl->prediction_horizon);
    config->set("prediction.error_weight", pImpl->prediction_error_weight);
    
    // Set world parameters
    config->set("world.width", pImpl->world_width);
    config->set("world.height", pImpl->world_height);
    config->set("sensory.vision.width", pImpl->vision_width);
    config->set("sensory.vision.height", pImpl->vision_height);
    config->set("sensory.vision.range", pImpl->vision_range);
    
    // Set agent parameters
    config->set("agent.learning.enable", pImpl->agent_learning_enabled);
    config->set("agent.planning.enable", pImpl->agent_planning_enabled);
    config->set("agent.social.enable", pImpl->agent_social_enabled);
    
    return config;
}

std::shared_ptr<Config> ConfigBuilder::create_research_config(int width, int height) {
    ConfigBuilder builder;
    return builder
        .with_neural_params(1500, 1.2f, -45.0f)
        .with_synapse_params(0.15f, 0.6f, 0.15f)
        .with_plasticity(true, true, true, 0.002f, 0.6f)
        .with_development("Research", 1.5, true)
        .with_neuromodulation(true, true, true, 1.2f)
        .with_memory(true, true, 1500.0f)
        .with_prediction(true, 0.15f, 1.5f)
        .with_world(width, height, 16, 16, 1.5f)
        .with_agent(true, true, false)
        .build();
}

std::shared_ptr<Config> ConfigBuilder::create_rl_config(float learning_rate) {
    ConfigBuilder builder;
    return builder
        .with_neural_params(2000, 1.5f, -40.0f)
        .with_synapse_params(0.2f, 0.7f, 0.2f)
        .with_plasticity(true, false, false, learning_rate, 0.3f)
        .with_development("Adult", 0.8, false)
        .with_neuromodulation(true, false, true, 0.8f)
        .with_memory(true, false, 2000.0f)
        .with_prediction(true, 0.2f, 2.0f)
        .with_world(30, 30, 12, 12, 2.0f)
        .with_agent(true, false, false)
        .build();
}

std::shared_ptr<Config> ConfigBuilder::create_developmental_config(double lifespan) {
    ConfigBuilder builder;
    return builder
        .with_neural_params(1200, 1.0f, -55.0f)
        .with_synapse_params(0.12f, 0.5f, 0.12f)
        .with_plasticity(true, true, true, 0.0015f, 0.5f)
        .with_development("Developmental", lifespan / 60.0, true)
        .with_neuromodulation(true, true, true, 1.0f)
        .with_memory(true, true, 1200.0f)
        .with_prediction(true, 0.1f, 1.0f)
        .with_world(25, 25, 10, 10, 1.2f)
        .with_agent(true, true, false)
        .build();
}

std::shared_ptr<Config> ConfigBuilder::create_reproduction_config() {
    ConfigBuilder builder;
    return builder
        .with_neural_params(800, 0.8f, -60.0f)
        .with_synapse_params(0.08f, 0.4f, 0.08f)
        .with_plasticity(false, false, false, 0.0005f, 0.2f)
        .with_development("Initial", 0.5, false)
        .with_neuromodulation(false, false, false, 0.5f)
        .with_memory(true, false, 800.0f)
        .with_prediction(false, 0.05f, 0.5f)
        .with_world(15, 15, 6, 6, 0.8f)
        .with_agent(false, false, false)
        .build();
}

std::string ConfigBuilder::get_config_summary() const {
    std::stringstream ss;
    ss << "NLM Configuration Summary:" << std::endl;
    ss << "  Neurons: " << pImpl->neuron_count << std::endl;
    ss << "  Synapse density: " << pImpl->synapse_density << std::endl;
    ss << "  Development stage: " << pImpl->development_stage << std::endl;
    ss << "  Learning enabled: " << (pImpl->stdp_enabled ? "Yes" : "No") << std::endl;
    ss << "  Curiosity enabled: " << (pImpl->curiosity_enabled ? "Yes" : "No") << std::endl;
    ss << "  World size: " << pImpl->world_width << "x" << pImpl->world_height << std::endl;
    return ss.str();
}

void ConfigBuilder::print_config() const {
    std::cout << get_config_summary();
}

} // namespace nlm