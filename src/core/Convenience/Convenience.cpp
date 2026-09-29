#include "Convenience.hpp"
#include <iostream>

namespace nlm {
namespace Convenience {

// Implementation of convenience functions

std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Core simulation settings
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Neural properties
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("neuron_threshold", -55.0f, ConfigSource::Default);
    config->set("neuron_leak_conductance", 10.0f, ConfigSource::Default);
    
    // Plasticity settings
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Development settings
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation settings
    config->set("dopamine_baseline", 0.0f, ConfigSource::Default);
    
    // Memory settings
    config->set("working_memory_enabled", true, ConfigSource::Default);
    config->set("episodic_memory_enabled", true, ConfigSource::Default);
    config->set("associative_memory_enabled", true, ConfigSource::Default);
    
    // Prediction system
    config->set("prediction_system_enabled", true, ConfigSource::Default);
    
    // Cognitive systems
    config->set("attention_enabled", true, ConfigSource::Default);
    config->set("planning_enabled", true, ConfigSource::Default);
    config->set("concept_formation_enabled", true, ConfigSource::Default);
    
    // Development
    config->set("development_enabled", true, ConfigSource::Default);
    config->set("critical_period_enabled", true, ConfigSource::Default);
    
    // Output
    config->set("log_level", "INFO", ConfigSource::Default);
    
    return config;
}

std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
createCompleteAgentSystem(int neuronCount, int worldWidth, int worldHeight) {
    auto config = createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
    
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Initialize world
    world->configure(worldWidth, worldHeight, 8, 8);
    world->reset();
    
    // Initialize agent
    agent->initialize(world);
    
    return std::make_tuple(brain, world, agent);
}

void runSimulationEpisode(
    const std::string& episodeName,
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    int numSteps,
    bool enableLearning,
    bool enableDevelopment) {
    
    std::cout << "=== Episode: " << episodeName << " ===" << std::endl;
    
    // Enable/disable systems based on flags
    if (enableLearning) {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
    }
    
    if (enableDevelopment) {
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
    }
    
    agent->enableRewardModulation(enableLearning);
    agent->enableStructuralPlasticity(enableLearning);
    agent->enableDevelopment(enableDevelopment);
    agent->enableCuriosity(!enableDevelopment);
    
    // Store initial state
    size_t initialNeurons = brain->getTotalNeuronCount();
    size_t initialSynapses = brain->getTotalSynapseCount();
    float initialFiringRate = brain->getAverageFiringRate();
    
    // Run episode
    for (int step = 0; step < numSteps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step, step * 0.001);
        
        // Generate action
        auto action = agent->decodeMotorCommand();
        
        // Apply action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        agent->applyRewardModulation(0.1f, 0.5f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Log progress periodically
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " neurons firing, "
                     << brain->getTotalSpikeCount() << " total spikes, "
                     << brain->getAverageFiringRate() << " Hz firing rate" << std::endl;
        }
    }
    
    // Log episode results
    std::cout << "\n=== Episode Results ===" << std::endl;
    std::cout << "Initial neurons: " << initialNeurons << std::endl;
    std::cout << "Final neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapse change: " << (brain->getTotalSynapseCount() - initialSynapses) << std::endl;
    std::cout << "Learning: " << (brain->getAverageFiringRate() - initialFiringRate > 0.1f ? "Yes" : "No") << std::endl;
    std::cout << "Episode completed successfully!" << std::endl;
}

void trainNeuralNetwork(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    int trainingSteps,
    const std::vector<std::vector<float>>& inputPatterns) {
    
    std::cout << "Starting neural network training..." << std::endl;
    
    // Enable all learning systems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Store initial weights
    std::vector<float> initialWeights;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Training loop
    for (int step = 0; step < trainingSteps; ++step) {
        // Select input pattern
        const auto& pattern = inputPatterns[step % inputPatterns.size()];
        
        // Create sensory input from pattern
        auto vision = std::make_shared<Vision>(8, 8, 3);
        
        // Create percept
        SensoryPercept percept;
        percept.setVision(vision);
        
        // Process input
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step, step * 0.001);
        
        // Apply learning signal
        float reward = 0.5f;
        agent->applyRewardModulation(reward, 0.5f);
        
        // Update curiosity
        agent->applyCuriosity(0.3f);
        
        // Log progress
        if (step % 100 == 0) {
            std::cout << "Training step " << step << "/" << trainingSteps << std::endl;
        }
    }
    
    // Calculate learning metrics
    float finalFiringRate = brain->getAverageFiringRate();
    
    std::cout << "Training completed!" << std::endl;
    std::cout << "Final firing rate: " << finalFiringRate << " Hz" << std::endl;
}

void quickDemo() {
    std::cout << "=== NLM Quick Demo ===" << std::endl;
    std::cout << "Creating minimal neural network..." << std::endl;
    
    // Create minimal brain
    auto config = createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    
    // Run quick computation
    for (int step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 10 == 0) {
            std::cout << "Step " << step << ": " 
                     << brain->getFiringNeuronCount() << " neurons firing, "
                     << brain->getTotalSpikeCount() << " spikes" << std::endl;
        }
    }
    
    std::cout << "\nDemo completed successfully!" << std::endl;
    std::cout << "Your neural network is thinking!" << std::endl;
}

std::shared_ptr<Config> createResearchConfig() {
    auto config = createDefaultConfig();
    
    // Research-oriented settings
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
    config->set("connection_probability", 0.05f, ConfigSource::Runtime);
    config->set("simulation_timestep", 0.0005, ConfigSource::Runtime);
    
    // Enable all systems for comprehensive research
    config->set("working_memory_enabled", true, ConfigSource::Runtime);
    config->set("episodic_memory_enabled", true, ConfigSource::Runtime);
    config->set("associative_memory_enabled", true, ConfigSource::Runtime);
    config->set("prediction_system_enabled", true, ConfigSource::Runtime);
    config->set("attention_enabled", true, ConfigSource::Runtime);
    config->set("planning_enabled", true, ConfigSource::Runtime);
    config->set("concept_formation_enabled", true, ConfigSource::Runtime);
    config->set("development_enabled", true, ConfigSource::Runtime);
    config->set("synaptogenesis_enabled", true, ConfigSource::Runtime);
    
    return config;
}

std::shared_ptr<Config> createRealTimeConfig() {
    auto config = createDefaultConfig();
    
    // Real-time processing settings
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
    config->set("connection_probability", 0.2f, ConfigSource::Runtime);
    
    // Lightweight settings
    config->set("working_memory_enabled", true, ConfigSource::Runtime);
    config->set("episodic_memory_enabled", false, ConfigSource::Runtime);
    config->set("associative_memory_enabled", false, ConfigSource::Runtime);
    config->set("prediction_system_enabled", false, ConfigSource::Runtime);
    config->set("attention_enabled", true, ConfigSource::Runtime);
    config->set("planning_enabled", false, ConfigSource::Runtime);
    
    // Focus on fast response
    config->set("synaptogenesis_enabled", false, ConfigSource::Runtime);
    config->set("pruning_enabled", false, ConfigSource::Runtime);
    
    return config;
}

std::shared_ptr<Config> createEducationalConfig() {
    auto config = createDefaultConfig();
    
    // Educational settings
    config->set("neuron_count", static_cast<int64_t>(300), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
    config->set("simulation_timestep", 0.01, ConfigSource::Runtime);
    config->set("connection_probability", 0.1f, ConfigSource::Runtime);
    
    // Enable debugging and logging
    config->set("log_level", "DEBUG", ConfigSource::Runtime);
    config->set("log_to_file", true, ConfigSource::Runtime);
    config->set("log_filename", "nlm_educational.log", ConfigSource::Runtime);
    
    // Enable all learning for educational purposes
    config->set("working_memory_enabled", true, ConfigSource::Runtime);
    config->set("episodic_memory_enabled", true, ConfigSource::Runtime);
    config->set("associative_memory_enabled", true, ConfigSource::Runtime);
    config->set("prediction_system_enabled", true, ConfigSource::Runtime);
    config->set("attention_enabled", true, ConfigSource::Runtime);
    config->set("planning_enabled", true, ConfigSource::Runtime);
    config->set("concept_formation_enabled", true, ConfigSource::Runtime);
    config->set("development_enabled", true, ConfigSource::Runtime);
    
    return config;
}

} // namespace Convenience
} // namespace nlm