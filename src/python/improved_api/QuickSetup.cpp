#include "NLMImprovedAPI.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <chrono>

namespace nlm {
namespace QuickSetup {

std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
create_agent_world_setup(int width, int height, int vision_width, int vision_height,
                         bool enable_learning, bool enable_development, bool enable_curiosity) {
    auto config = create_research_config(width, height);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, vision_width, vision_height);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    if (enable_learning) {
        agent->enableRewardModulation(true);
        agent->enableCuriosity(enable_curiosity);
    }
    
    if (enable_development) {
        agent->enableDevelopment(true);
    }
    
    return std::make_tuple(brain, world, agent);
}

std::shared_ptr<Config> create_research_config(int width, int height) {
    return ConfigBuilder{}
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

std::shared_ptr<Config> create_rl_config(float learning_rate) {
    return ConfigBuilder{}
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

std::shared_ptr<Config> create_developmental_config(double lifespan) {
    return ConfigBuilder{}
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

void run_research_simulation(size_t steps) {
    auto config = create_research_config(20, 20);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    run_research_pattern(brain, steps);
}

void run_rl_simulation(size_t steps) {
    auto config = create_rl_config(0.01f);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(30, 30, 12, 12);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    run_rl_pattern(agent, world, steps);
}

void run_development_simulation(size_t steps) {
    auto config = create_developmental_config(60.0);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    run_development_pattern(brain, steps, 60.0);
}

float analyze_brain_efficiency(std::shared_ptr<Brain> brain, size_t steps) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    size_t initial_spikes = brain->getTotalSpikeCount();
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(static_cast<SimulationStep>(i));
    }
    
    size_t final_spikes = brain->getTotalSpikeCount();
    size_t total_spikes = final_spikes - initial_spikes;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double wall_time = std::chrono::duration<double>(end_time - start_time).count();
    
    if (wall_time > 0.0) {
        return static_cast<float>(total_spikes / wall_time); // spikes per second
    }
    return 0.0f;
}

float analyze_learning_efficiency(std::shared_ptr<AgentBrain> agent,
                                 std::shared_ptr<SimpleWorld> world,
                                 size_t steps) {
    SimulationResult result = run_learning_episode(agent, world, steps);
    
    if (result.wall_clock_time > 0.0) {
        return result.final_reward / result.wall_clock_time; // reward per second
    }
    return 0.0f;
}

} // namespace QuickSetup

} // namespace nlm