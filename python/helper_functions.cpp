// Helper functions implementation for the Python bindings
namespace helper_functions {

// Simulation helpers
SimulationStats run_simulation_with_stats(std::shared_ptr<Brain> brain, 
                                          std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent,
                                          const helper_utils::SimulationOptions& options) {
    SimulationStats stats;
    stats.total_steps = options.num_steps;
    
    if (!brain || !world || !agent) {
        throw ResourceError("One or more simulation components is null");
    }
    
    agent->initialize(*world);
    
    if (options.enable_learning) {
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
    }
    if (options.enable_development) {
        agent->enableDevelopment(true);
    }
    
    for (int step = 0; step < options.num_steps; ++step) {
        world->update(options.timestep);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step, step * options.timestep * 1000.0f);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (options.log_stats && step % 100 == 0) {
            stats.firing_rates.push_back(brain->getAverageFiringRate());
            stats.spike_counts.push_back(brain->getTotalSpikeCount());
            stats.e_i_ratios.push_back(brain->getExcitationInhibitionRatio());
            stats.curiosity_levels.push_back(agent->getCuriosityLevel());
            stats.novelty_levels.push_back(agent->getNoveltyLevel());
            stats.prediction_errors.push_back(agent->getPredictionError());
        }
    }
    
    stats.compute_statistics();
    return stats;
}

// World setup helpers
std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
create_agent_simulation(const helper_utils::WorldConfig& world_config,
                         const helper_utils::LearningConfig& learning_config,
                         const std::string& brain_mode = "default") {
    auto config = std::make_shared<Config>();
    
    // Set brain parameters based on mode
    if (brain_mode == "compact") {
        config->set("brain.neuron_count", 100);
        config->set("brain.connection_probability", 0.05);
    } else if (brain_mode == "large") {
        config->set("brain.neuron_count", 5000);
        config->set("brain.connection_probability", 0.15);
    } else {
        config->set("brain.neuron_count", 1000);
        config->set("brain.connection_probability", 0.1);
    }
    
    config->set("simulation.mode", brain_mode);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(world_config.width, world_config.height,
                     world_config.vision_width, world_config.vision_height);
    world->reset();
    world->setMaxEnergy(world_config.max_energy);
    world->setEnergyDecayRate(world_config.energy_decay_rate);
    world->setAgentStart(world_config.agent_start_x, world_config.agent_start_y);
    world->setRandomSeed(std::chrono::system_clock::now().time_since_epoch().count());
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    if (learning_config.enable_reward_modulation) {
        agent->enableRewardModulation(true);
    }
    if (learning_config.enable_structural_plasticity) {
        agent->enableStructuralPlasticity(true);
    }
    if (learning_config.enable_development) {
        agent->enableDevelopment(true);
    }
    if (learning_config.enable_curiosity) {
        agent->enableCuriosity(true);
    }
    
    return std::make_tuple(brain, world, agent);
}

// Visualization and analysis helpers
std::string generate_network_summary(const Brain& brain) {
    std::stringstream ss;
    ss << "=== Neural Network Summary ===\n";
    ss << "Total neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "Total synapses: " << brain.getTotalSynapseCount() << "\n";
    ss << "Active neurons: " << brain.getActiveNeuronCount() << "\n";
    ss << "Firing neurons: " << brain.getFiringNeuronCount() << "\n";
    ss << "Average firing rate: " << brain.getAverageFiringRate() << " Hz\n";
    ss << "Excitation/Inhibition ratio: " << brain.getExcitationInhibitionRatio() << "\n";
    ss << "Total spikes: " << brain.getTotalSpikeCount() << "\n";
    ss << "Developmental stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    ss << "Regions: " << brain.getRegionCount() << "\n";
    return ss.str();
}

std::vector<float> run_spiking_analysis(std::shared_ptr<Brain> brain, int duration_steps) {
    if (!brain) {
        throw ResourceError("Brain pointer is null");
    }
    
    brain->initialize();
    std::vector<float> firing_rates;
    
    for (int step = 0; step < duration_steps; ++step) {
        brain->step(step);
        firing_rates.push_back(brain->getAverageFiringRate());
    }
    
    return firing_rates;
}

// Configuration validation and loading
bool validate_simulation_setup(const Brain& brain, const SimpleWorld& world) {
    bool brain_valid = (brain.getConfig() != nullptr) && 
                       brain.getTotalNeuronCount() > 0;
    bool world_valid = (world.getWidth() > 0) && (world.getHeight() > 0);
    return brain_valid && world_valid;
}

std::string create_experiment_description(const std::string& experiment_name,
                                           const helper_utils::SimulationOptions& options,
                                           const helper_utils::WorldConfig& world_config,
                                           const helper_utils::LearningConfig& learning_config) {
    std::stringstream ss;
    ss << "=== Experiment: " << experiment_name << " ===\n";
    ss << "Simulation steps: " << options.num_steps << "\n";
    ss << "Time step: " << options.timestep << " s\n";
    ss << "World size: " << world_config.width << "x" << world_config.height << "\n";
    ss << "Vision: " << world_config.vision_width << "x" << world_config.vision_height << "\n";
    ss << "Learning enabled: Reward=" << (options.enable_learning ? "yes" : "no")
       << ", Curiosity=" << (options.enable_curiosity ? "yes" : "no")
       << ", Development=" << (options.enable_development ? "yes" : "no") << "\n";
    return ss.str();
}

// Data export and import helpers
bool export_simulation_data(const SimulationStats& stats, const std::string& filename) {
    // This would typically serialize to JSON, CSV, or another format
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "firing_rate_mean," << stats.mean_firing_rate << "\n";
    file << "firing_rate_std," << stats.std_firing_rate << "\n";
    file << "firing_rate_max," << stats.max_firing_rate << "\n";
    file << "firing_rate_min," << stats.min_firing_rate << "\n";
    file << "curiosity_mean," << stats.mean_curiosity << "\n";
    file << "novelty_mean," << stats.mean_novelty << "\n";
    file << "prediction_error_mean," << stats.mean_prediction_error << "\n";
    file << "total_steps," << stats.total_steps << "\n";
    
    file.close();
    return true;
}

} // namespace helper_functions
