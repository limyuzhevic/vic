    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Add convenience factory methods for Python users
    m.def("createAdvancedConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set up a balanced brain configuration by default
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("enable_reward_modulation", true);
        config->set("curiosity_threshold", 0.5f);
        config->set("novelty_threshold", 0.3f);
        config->set("temperature", 1.0f);
        
        return config;
    }, "Create an advanced configuration with balanced settings");
    
    m.def("createSmallBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a small brain for testing");
    
    m.def("createExplorationWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 20, 20);  // width, height, visionWidth, visionHeight
        world->setAgentStart(50, 50);
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.01f);
        return world;
    }, "Create an exploration world for agent testing");
    
    m.def("createAgentWithBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain with world");
    
    m.def("runSimpleExperiment", [](std::shared_ptr<Brain> brain, 
                                   size_t steps, 
                                   float duration = 0.1f,
                                   bool logProgress = true) {
        
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, duration * (i + 1));
            
            if (logProgress && (i % 100 == 0)) {
                size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
                double firingRate = brain->getAverageFiringRate();
                std::cout << "Step " << i << ": " 
                         << "Firing rate: " << firingRate 
                         << " Hz, " 
                         << "Active neurons: " << brain->getActiveNeuronCount()
                         << " / " << neuronCount << std::endl;
            }
        }
        
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"), py::arg("duration") = 0.1f,
       py::arg("logProgress") = true, "Run a simple simulation experiment");
    
    m.def("createLearningConfig", [](float learningRate, 
                                   float curiosity, 
                                   bool enableDevelopment = true,
                                   bool enableReplay = true) {
        auto config = std::make_shared<Config>();
        
        config->set("learning_rate", learningRate);
        config->set("curiosity_threshold", curiosity);
        config->set("enable_development", enableDevelopment);
        config->set("enable_replay", enableReplay);
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        
        return config;
    }, py::arg("learningRate") = 0.01f, py::arg("curiosity") = 0.5f,
       py::arg("enableDevelopment") = true, py::arg("enableReplay") = true,
       "Create a learning-focused configuration");
    
    m.def("createCheckpointConfig", [](const std::string& checkpointPath) {
        auto config = std::make_shared<Config>();
        
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 2000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.05f);
        config->set("checkpoint_path", checkpointPath);
        
        return config;
    }, py::arg("checkpointPath"), "Create a checkpoint-compatible configuration");
    
    m.def("createVisualizationConfig", []() {
        auto config = std::make_shared<Config>();
        
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 300);
        config->set("enable_visualization", true);
        config->set("max_spikes_per_step", 10000);
        
        return config;
    }, "Create a visualization-optimized configuration");
}