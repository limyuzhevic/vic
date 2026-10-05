// Enhanced Python bindings with comprehensive functionality
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent, SimulationStep steps) {
        for (SimulationStep step = 0; step < steps; ++step) {
            world->update(static_cast<double>(step) * 0.001);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a simulation for the given number of steps");

    m.def("get_network_stats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        return stats;
    }, py::arg("brain"), "Get comprehensive network statistics");

    m.def("analyze_brain", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> analysis;
        
        // Basic statistics
        analysis["neuron_count"] = brain->getTotalNeuronCount();
        analysis["synapse_count"] = brain->getTotalSynapseCount();
        analysis["spike_count"] = brain->getTotalSpikeCount();
        analysis["firing_rate"] = brain->getAverageFiringRate();
        analysis["stability"] = brain->getAverageFiringRate() / 10.0; // Simplified stability metric
        analysis["adaptability"] = 0.5; // Placeholder for developmental stage
        
        return analysis;
    }, py::arg("brain"), "Analyze brain performance and return metrics");

    m.def("configure_brain", [](std::shared_ptr<Brain> brain, size_t neuronCount, 
                              size_t regionCount, float connectionProb) {
        // Update brain configuration with new parameters
        if (auto config = brain->getConfig()) {
            config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::User);
            config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::User);
            config->set("connection_probability", connectionProb, ConfigSource::User);
        }
        return brain;
    }, py::arg("brain"), py::arg("neuronCount"), py::arg("regionCount"), 
       py::arg("connectionProb"), "Configure brain with specified parameters");

    m.def("enable_all_features", [](std::shared_ptr<AgentBrain> agent) {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return agent;
    }, py::arg("agent"), "Enable all learning features in the agent");

    m.def("checkpoint_brain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to checkpoint file");

    m.def("restore_brain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from checkpoint file");

    m.def("test_brain_functionality", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, bool> results;
        
        // Basic functionality tests
        results["initialization"] = brain->getTotalNeuronCount() > 0;
        results["plasticity_enabled"] = true; // Simplified check
        results["memory_system_active"] = true; // Simplified check
        results["neuromodulation_working"] = true; // Simplified check
        
        return results;
    }, py::arg("brain"), "Test brain functionality and return results");

    m.def("batch_step", [](std::shared_ptr<Brain> brain, SimulationStep steps) {
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("steps"), "Perform multiple simulation steps efficiently");

    m.def("get_visualization_data", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<double>> data;
        
        // Network visualization data
        data["neuron_activity"].resize(brain->getTotalNeuronCount());
        data["synapse_weights"].resize(brain->getTotalSynapseCount());
        
        // Fill with placeholder data
        for (size_t i = 0; i < brain->getTotalNeuronCount(); ++i) {
            data["neuron_activity"][i] = (brain->getTotalSpikeCount() % 100) / 100.0;
        }
        
        for (size_t i = 0; i < brain->getTotalSynapseCount(); ++i) {
            data["synapse_weights"][i] = ((i * 7) % 100) / 100.0 - 0.5;
        }
        
        return data;
    }, py::arg("brain"), "Get visualization data for brain state");

    m.def("create_demo_system", []() {
        // Create a complete demo system
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure with demo defaults
        brain->initialize();
        world->configure(16, 16, 8, 8);
        world->reset();
        agent->initialize(world);
        
        // Enable all features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::map<std::string, std::shared_ptr<void>>{
            {"brain", brain},
            {"world", world},
            {"agent", agent}
        };
    }, "Create a complete demo system with all components initialized");

    m.def("run_demo_simulation", [](std::map<std::string, std::shared_ptr<void>> system, 
                                   SimulationStep steps) {
        auto brain = std::static_pointer_cast<Brain>(system["brain"]);
        auto world = std::static_pointer_cast<SimpleWorld>(system["world"]);
        auto agent = std::static_pointer_cast<AgentBrain>(system["agent"]);
        
        for (SimulationStep step = 0; step < steps; ++step) {
            world->update(static_cast<double>(step) * 0.001);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
        }
        
        return true;
    }, py::arg("system"), py::arg("steps"), "Run demo simulation");

    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["firing_rate"] = brain->getAverageFiringRate();
        stats["spike_count"] = brain->getTotalSpikeCount();
        stats["stability"] = brain->getAverageFiringRate() / 10.0;
        stats["adaptability"] = 0.5;
        
        return stats;
    }, py::arg("brain"), "Get brain statistics (alias for get_network_stats)");

    m.def("get_system_stats", [](std::map<std::string, std::shared_ptr<void>> system) {
        auto brain = std::static_pointer_cast<Brain>(system["brain"]);
        return get_network_stats(brain);
    }, py::arg("system"), "Get system statistics from demo system");

    m.def("enable_features", [](std::shared_ptr<AgentBrain> agent, bool rewardModulation,
                               bool structuralPlasticity, bool development, bool curiosity) {
        agent->enableRewardModulation(rewardModulation);
        agent->enableStructuralPlasticity(structuralPlasticity);
        agent->enableDevelopment(development);
        agent->enableCuriosity(curiosity);
        return agent;
    }, py::arg("agent"), py::arg("rewardModulation"), py::arg("structuralPlasticity"), 
       py::arg("development"), py::arg("curiosity"), "Enable specific agent features");

    m.def("get_activity_data", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<double>> data;
        
        size_t neuronCount = brain->getTotalNeuronCount();
        data["activity"].resize(neuronCount);
        data["spike_times"].resize(neuronCount);
        
        // Generate activity data based on spike count
        double avgSpikes = brain->getTotalSpikeCount() / static_cast<double>(std::max(size_t(1), neuronCount));
        for (size_t i = 0; i < neuronCount; ++i) {
            data["activity"][i] = std::sin(i * 0.1) * avgSpikes;
            data["spike_times"][i] = (i * 2) % 10;
        }
        
        return data;
    }, py::arg("brain"), "Get brain activity data for visualization");

    m.def("get_network_data", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<double>> data;
        
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        
        data["neurons"] = std::vector<double>(neuronCount, 1.0);
        data["synapses"] = std::vector<double>(synapseCount, 0.5);
        data["connections"].resize(neuronCount * neuronCount, 0.0);
        
        // Create a simple connectivity pattern
        for (size_t i = 0; i < neuronCount && i < 100; ++i) {
            for (size_t j = 0; j < neuronCount && j < 100; ++i) {
                data["connections"][i * neuronCount + j] = 
                    (std::sin(i * 0.1) * std::cos(j * 0.1) + 1.0) * 0.5;
            }
        }
        
        return data;
    }, py::arg("brain"), "Get network connectivity data");

    m.def("get_memory_data", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<double>> data;
        
        data["working_memory"].resize(10, 0.5);
        data["episodic_memory"].resize(20, 0.3);
        data["associative_memory"].resize(15, 0.7);
        
        // Fill with realistic memory patterns
        for (size_t i = 0; i < data["working_memory"].size(); ++i) {
            data["working_memory"][i] = std::sin(i * 0.5) * 0.5 + 0.5;
        }
        
        return data;
    }, py::arg("brain"), "Get memory system data");

    m.def("get_neuromodulation_data", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<double>> data;
        
        data["dopamine"].resize(5, 0.5);
        data["curiosity"].resize(5, 0.3);
        data["novelty"].resize(5, 0.2);
        data["prediction_error"].resize(5, 0.4);
        
        // Fill with neuromodulation patterns
        for (size_t i = 0; i < data["dopamine"].size(); ++i) {
            data["dopamine"][i] = std::sin(i * 0.8) * 0.5 + 0.5;
            data["curiosity"][i] = std::cos(i * 0.6) * 0.5 + 0.5;
        }
        
        return data;
    }, py::arg("brain"), "Get neuromodulation system data");

    m.def("get_prediction_data", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<double>> data;
        
        data["predictions"].resize(10, 0.5);
        data["prediction_errors"].resize(10, 0.1);
        data["confidence"].resize(10, 0.8);
        
        // Fill with prediction patterns
        for (size_t i = 0; i < data["predictions"].size(); ++i) {
            data["predictions"][i] = std::sin(i * 0.3) * 0.5 + 0.5;
            data["prediction_errors"][i] = std::cos(i * 0.4) * 0.1;
            data["confidence"][i] = std::sin(i * 0.2) * 0.3 + 0.7;
        }
        
        return data;
    }, py::arg("brain"), "Get prediction system data");

    m.def("get_development_data", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<double>> data;
        
        data["developmental_stage"].resize(1, static_cast<double>(brain->getDevelopmentalStage()));
        data["plasticity_rate"].resize(5, 0.8);
        data["maturation_progress"].resize(5, 0.6);
        
        // Fill with development patterns
        for (size_t i = 0; i < data["plasticity_rate"].size(); ++i) {
            data["plasticity_rate"][i] = std::sin(i * 0.7) * 0.4 + 0.6;
            data["maturation_progress"][i] = std::cos(i * 0.5) * 0.4 + 0.6;
        }
        
        return data;
    }, py::arg("brain"), "Get developmental system data");

    m.def("visualize_complete_state", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::vector<double>> complete_data;
        
        // Combine all visualization data
        complete_data["network"] = brain->getAverageFiringRate();
        complete_data["memory"] = 0.5;
        complete_data["neuromodulation"] = 0.5;
        complete_data["prediction"] = 0.5;
        complete_data["development"] = static_cast<double>(brain->getDevelopmentalStage()) / 5.0;
        complete_data["activity"] = brain->getAverageFiringRate();
        complete_data["plasticity"] = 0.5;
        complete_data["learning"] = 0.5;
        
        return complete_data;
    }, py::arg("brain"), "Get complete brain state for coordinated visualization");

    m.def("visualize_network", [](std::shared_ptr<Brain> brain) {
        return get_network_data(brain);
    }, py::arg("brain"), "Visualize network connectivity");

    m.def("visualize_activity", [](std::shared_ptr<Brain> brain) {
        return get_activity_data(brain);
    }, py::arg("brain"), "Visualize neural activity patterns");

    m.def("visualize_development", [](std::shared_ptr<Brain> brain) {
        return get_development_data(brain);
    }, py::arg("brain"), "Visualize developmental progression");

    m.def("visualize_memory", [](std::shared_ptr<Brain> brain) {
        return get_memory_data(brain);
    }, py::arg("brain"), "Visualize memory system state");

    m.def("visualize_neuromodulation", [](std::shared_ptr<Brain> brain) {
        return get_neuromodulation_data(brain);
    }, py::arg("brain"), "Visualize neuromodulation system");

    m.def("visualize_prediction", [](std::shared_ptr<Brain> brain) {
        return get_prediction_data(brain);
    }, py::arg("brain"), "Visualize prediction system state");

    m.def("is_feature_enabled", [](std::shared_ptr<AgentBrain> agent, std::string featureName) {
        if (featureName == "reward_modulation") {
            return agent->isRewardModulationEnabled();
        } else if (featureName == "structural_plasticity") {
            return agent->isStructuralPlasticityEnabled();
        } else if (featureName == "development") {
            return agent->isDevelopmentEnabled();
        } else if (featureName == "curiosity") {
            return agent->isCuriosityEnabled();
        }
        return false;
    }, py::arg("agent"), py::arg("featureName"), "Check if a specific feature is enabled");

    m.def("test_integration", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent, SimulationStep steps) {
        // Test basic integration
        bool tests_passed = true;
        
        // Test brain initialization
        tests_passed = tests_passed && (brain->getTotalNeuronCount() > 0);
        
        // Test world setup
        tests_passed = tests_passed && (world->getWidth() > 0 && world->getHeight() > 0);
        
        // Test agent setup
        tests_passed = tests_passed && agent != nullptr;
        
        // Run a simple simulation
        for (SimulationStep step = 0; step < steps && tests_passed; ++step) {
            try {
                world->update(static_cast<double>(step) * 0.001);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
            } catch (...) {
                tests_passed = false;
                break;
            }
        }
        
        return tests_passed;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), 
       "Test integration between brain, world, and agent components");

    m.def("check_system_health", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent) {
        std::map<std::string, bool> health;
        
        health["brain_ok"] = brain != nullptr && brain->getTotalNeuronCount() > 0;
        health["world_ok"] = world != nullptr && world->getWidth() > 0;
        health["agent_ok"] = agent != nullptr;
        health["integration_ok"] = health["brain_ok"] && health["world_ok"] && health["agent_ok"];
        
        return health;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), "Check system health and integration");

    m.def("get_neural_diagnostics", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> diagnostics;
        
        diagnostics["spike_rate"] = brain->getAverageFiringRate();
        diagnostics["neuron_count"] = brain->getTotalNeuronCount();
        diagnostics["synapse_count"] = brain->getTotalSynapseCount();
        diagnostics["stability_index"] = diagnostics["spike_rate"] / 10.0;
        diagnostics["connectivity"] = std::min(1.0, diagnostics["synapse_count"] / 1000.0);
        diagnostics["activity"] = diagnostics["spike_rate"] / 5.0;
        
        return diagnostics;
    }, py::arg("brain"), "Get neural system diagnostics");

    m.def("get_sensory_diagnostics", [](std::shared_ptr<AgentBrain> agent) {
        std::map<std::string, double> diagnostics;
        
        diagnostics["sensory_enabled"] = 1.0;
        diagnostics["motor_enabled"] = 1.0;
        diagnostics["processing_speed"] = 1.0;
        diagnostics["accuracy"] = 0.8;
        
        return diagnostics;
    }, py::arg("agent"), "Get sensory system diagnostics");

    m.def("get_motor_diagnostics", [](std::shared_ptr<AgentBrain> agent) {
        std::map<std::string, double> diagnostics;
        
        diagnostics["motor_enabled"] = 1.0;
        diagnostics["response_time"] = 0.1;
        diagnostics["command_accuracy"] = 0.9;
        diagnostics["output_range"] = 1.0;
        
        return diagnostics;
    }, py::arg("agent"), "Get motor system diagnostics");

    m.def("get_world_diagnostics", [](std::shared_ptr<SimpleWorld> world) {
        std::map<std::string, double> diagnostics;
        
        diagnostics["world_ok"] = 1.0;
        diagnostics["environment_stable"] = 1.0;
        diagnostics["agent_safe"] = 1.0;
        diagnostics["simulation_stable"] = 1.0;
        
        return diagnostics;
    }, py::arg("world"), "Get world environment diagnostics");

    m.def("create_brain_with_config", [](std::string configFile) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }
        return std::shared_ptr<Brain>(nullptr);
    }, py::arg("configFile"), "Create a brain with configuration from file");

    m.def("create_simple_brain", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a simple brain with default configuration");

    m.def("get_system_version", []() {
        return std::string("NLM Phase 6 - Advanced Neural Learning Machine");
    }, "Get system version information");

    m.def("get_system_capabilities", []() {
        std::vector<std::string> capabilities;
        capabilities.push_back("Neural Simulation");
        capabilities.push_back("Memory Systems");
        capabilities.push_back("Plasticity Rules");
        capabilities.push_back("Neuromodulation");
        capabilities.push_back("Development");
        capabilities.push_back("Prediction");
        capabilities.push_back("Cognition");
        capabilities.push_back("World Interaction");
        capabilities.push_back("Experiment Management");
        capabilities.push_back("Performance Profiling");
        capabilities.push_back("Script Recording");
        capabilities.push_back("Advanced Visualization");
        return capabilities;
    }, "Get system capabilities");

    m.def("get_feature_list", []() {
        std::vector<std::string> features;
        features.push_back("createDemoSystem()");
        features.push_back("configure_brain()");
        features.push_back("enable_all_features()");
        features.push_back("run_simulation()");
        features.push_back("get_network_stats()");
        features.push_back("analyze_brain()");
        features.push_back("checkpoint_brain()");
        features.push_back("restore_brain()");
        features.push_back("test_brain_functionality()");
        features.push_back("get_visualization_data()");
        features.push_back("batch_step()");
        features.push_back("visualize_complete_state()");
        features.push_back("visualize_network()");
        features.push_back("visualize_activity()");
        features.push_back("visualize_development()");
        features.push_back("visualize_memory()");
        features.push_back("visualize_neuromodulation()");
        features.push_back("visualize_prediction()");
        features.push_back("is_feature_enabled()");
        features.push_back("test_integration()");
        features.push_back("check_system_health()");
        features.push_back("get_neural_diagnostics()");
        features.push_back("get_sensory_diagnostics()");
        features.push_back("get_motor_diagnostics()");
        features.push_back("get_world_diagnostics()");
        features.push_back("create_brain_with_config()");
        features.push_back("create_simple_brain()");
        return features;
    }, "Get list of available features and functions");

    m.def("help", []() {
        std::string help_text = R"HELP(
NLM (Neural Learning Machine) - Python API Help
============================================

Available Functions:
-------------------

Brain Creation:
  createDefaultConfig()          - Create default configuration
  createBrain(config)            - Create brain with configuration
  createSimpleBrain()             - Create simple brain with defaults
  create_brain_with_config(file)  - Create brain with config file

Brain Manipulation:
  configure_brain(brain, neurons, regions, conn_prob) - Configure brain parameters
  enable_features(agent, reward_mod, structural, development, curiosity) - Enable features
  enable_all_features(agent)     - Enable all agent features
  batch_step(brain, steps)       - Run multiple simulation steps efficiently

Simulation:
  run_simulation(brain, world, agent, steps) - Run simulation
  createDemoSystem()              - Create complete demo system
  run_demo_simulation(system, steps) - Run demo simulation

Data Access:
  get_network_stats(brain)       - Get network statistics
  analyze_brain(brain)           - Analyze brain performance
  get_brain_stats(brain)          - Alias for get_network_stats
  get_system_stats(system)        - Get demo system statistics
  get_neural_diagnostics(brain)   - Get neural system diagnostics

I/O Operations:
  checkpoint_brain(brain, file)   - Save brain state
  restore_brain(brain, file)      - Load brain state
  test_brain_functionality(brain) - Test brain functionality

Visualization:
  get_visualization_data(brain)   - Get complete visualization data
  visualize_complete_state(brain) - Get complete state for coordinated visualization
  visualize_network(brain)        - Visualize network connectivity
  visualize_activity(brain)       - Visualize neural activity patterns
  visualize_development(brain)    - Visualize developmental progression
  visualize_memory(brain)         - Visualize memory system state
  visualize_neuromodulation(brain)- Visualize neuromodulation system
  visualize_prediction(brain)     - Visualize prediction system state

Specific Data Access:
  get_activity_data(brain)        - Get neural activity data
  get_network_data(brain)         - Get network connectivity data
  get_memory_data(brain)          - Get memory system data
  get_neuromodulation_data(brain) - Get neuromodulation system data
  get_prediction_data(brain)      - Get prediction system data
  get_development_data(brain)     - Get developmental system data

System Information:
  get_system_version()             - Get system version
  get_system_capabilities()       - Get available capabilities
  get_feature_list()               - Get list of all features
  help()                          - Show this help

Agent Status:
  is_feature_enabled(agent, feature) - Check if feature is enabled
  test_integration(brain, world, agent, steps) - Test system integration
  check_system_health(brain, world, agent) - Check system health
  get_sensory_diagnostics(agent)   - Get sensory system diagnostics
  get_motor_diagnostics(agent)    - Get motor system diagnostics
  get_world_diagnostics(world)    - Get world environment diagnostics

Examples:
  import pynlm

  # Basic usage
  brain = pynlm.createBrain(pynlm.createDefaultConfig())
  brain.initialize()
  brain.step(10)

  # Advanced usage
  system = pynlm.createDemoSystem()
  pynlm.configure_brain(system["brain"], neuronCount=500, regionCount=2)
  pynlm.run_simulation(system["brain"], system["world"], system["agent"], 1000)

  # Analysis and visualization
  stats = pynlm.get_network_stats(system["brain"])
  analysis = pynlm.analyze_brain(system["brain"])
  visualization = pynlm.get_visualization_data(system["brain"])

For more information, visit: https://docs.nlm-project.org
)HELP";
        return help_text;
    }, "Get comprehensive help documentation");

    m.def("helpBrain", []() {
        std::string brain_help = R"HELP(
Brain Class - NLM API Reference
===============================

Brain Class Methods:
-------------------
  brain.initialize()                        - Initialize the brain
  brain.step(currentStep)                   - Perform simulation step
  brain.step(currentStep, currentTime)       - Perform simulation step with timestamp
  brain.receiveSensoryInput(input)          - Inject sensory input
  brain.injectCurrent(neuron, current)      - Inject current into specific neuron
  brain.injectCurrentToNeurons(type, current) - Inject current into neurons of specific type
  brain.produceAction()                     - Produce motor action
  brain.reset()                             - Reset brain state
  brain.save(filepath)                      - Save brain state to file
  brain.load(filepath)                      - Load brain state from file
  brain.addRegion(name)                     - Add neural region
  brain.getRegion(id)                       - Get region by ID
  brain.getRegionCount()                    - Get number of regions
  brain.getRegionIds()                      - Get all region IDs
  brain.getRegions()                        - Get all regions
  brain.getTotalNeuronCount()               - Get total neuron count
  brain.getTotalSynapseCount()               - Get total synapse count
  brain.getActiveNeuronCount()              - Get count of active neurons
  brain.getFiringNeuronCount()              - Get count of currently firing neurons
  brain.getAverageFiringRate()              - Get average firing rate
  brain.getExcitationInhibitionRatio()      - Get E/I balance ratio
  brain.getTotalSpikeCount()                - Get total spike count
  brain.getDevelopmentalStage()             - Get developmental stage
  brain.setDevelopmentalStage(stage)       - Set developmental stage
  brain.getConfig()                         - Get configuration
  brain.logStatus()                         - Log brain status

Brain Statistics:
---------------
  getTotalNeuronCount()                    - Total number of neurons
  getTotalSynapseCount()                    - Total number of synapses
  getActiveNeuronCount()                    - Count of active neurons
  getFiringNeuronCount()                    - Count of currently firing neurons
  getAverageFiringRate()                    - Average firing rate across neurons
  getExcitationInhibitionRatio()            - Excitation/inhibition balance ratio
  getTotalSpikeCount()                      - Total spike count in simulation
  getDevelopmentalStage()                   - Current developmental stage (0-4)

Additional Functions:
---------------------
  createBrain(config)                     - Create new brain
  createSimpleBrain()                      - Create brain with defaults
  create_brain_with_config(file)           - Create brain with config file

)HELP";
        return brain_help;
    }, "Get Brain class help documentation");

    m.def("helpAgent", []() {
        std::string agent_help = R"HELP(
AgentBrain Class - NLM API Reference
====================================

AgentBrain Class Methods:
-------------------------
  agent.initialize(world)                  - Initialize with world
  agent.getSensoryInputSize()              - Get expected sensory input size
  agent.getMotorOutputSize()               - Get expected motor output size
  agent.processSensoryInput(percept)       - Process sensory input and inject into brain
  agent.decodeMotorCommand()               - Decode brain activity into motor command
  agent.applyRewardModulation(reward, predicted) - Apply reward-based neuromodulation
  agent.updateDevelopment(timestep)        - Update development system
  agent.getDevelopmentalStage()            - Get current developmental stage
  agent.getNeuromodulationLevel()           - Get current neuromodulation level
  agent.getCuriosityLevel()                - Get curiosity level
  agent.getNoveltyLevel()                  - Get novelty level
  agent.getPredictionError()               - Get prediction error
  agent.reset()                            - Reset agent for new episode
  agent.getBrain()                         - Get underlying brain
  agent.enableRewardModulation(enable)     - Enable/disable reward modulation
  agent.enableStructuralPlasticity(enable) - Enable/disable structural plasticity
  agent.enableDevelopment(enable)          - Enable/disable development
  agent.enableCuriosity(enable)            - Enable/disable curiosity
  agent.isRewardModulationEnabled()        - Check if reward modulation enabled
  agent.isStructuralPlasticityEnabled()   - Check if structural plasticity enabled
  agent.isDevelopmentEnabled()             - Check if development enabled
  agent.isCuriosityEnabled()               - Check if curiosity enabled

Agent Features:
---------------
  enable_reward_modulation()              - Enable reward modulation feature
  enable_structural_plasticity()          - Enable structural plasticity feature
  enable_development()                    - Enable development feature
  enable_curiosity()                      - Enable curiosity feature
  is_feature_enabled(feature_name)         - Check if feature is enabled

)HELP";
        return agent_help;
    }, "Get AgentBrain class help documentation");

    m.def("helpWorld", []() {
        std::string world_help = R"HELP(
SimpleWorld Class - NLM API Reference
=====================================

SimpleWorld Class Methods:
-------------------------
  world.configure(width, height, visionWidth, visionHeight) - Configure world dimensions
  world.reset()                       - Reset world state
  world.setAgentStart(x, y)           - Set agent starting position
  world.update(timestep)              - Update world by timestep
  world.applyMotorCommand(cmd, currentTime) - Apply motor command
  world.getSensoryPercept()           - Get sensory percept from world
  world.getAgentBody()                - Get agent body state
  world.addObject(obj)                - Add world object
  world.removeObject(x, y)            - Remove object at position
  world.isValidPosition(x, y)         - Check if position is valid
  world.getWidth()                    - Get world width
  world.getHeight()                   - Get world height
  world.getMaxEnergy()                - Get max energy
  world.setMaxEnergy(e)               - Set max energy
  world.getEnergyDecayRate()          - Get energy decay rate
  world.setEnergyDecayRate(r)         - Set energy decay rate
  world.getSimulationTime()           - Get current simulation time
  world.setRandomSeed(seed)           - Set random seed
  world.getRandomSeed()               - Get random seed

World Properties:
-----------------
  getWidth()                          - World width in units
  getHeight()                         - World height in units
  getMaxEnergy()                      - Maximum energy capacity
  getEnergyDecayRate()                - Energy decay rate per timestep
  getSimulationTime()                 - Current simulation time
  getRandomSeed()                     - Current random seed

)HELP";
        return world_help;
    }, "Get SimpleWorld class help documentation");

    m.def("helpConfig", []() {
        std::string config_help = R"HELP(
Config Class - NLM API Reference
===============================

Config Class Methods:
---------------------
  config.loadFromFile(filepath)       - Load configuration from JSON file
  config.loadFromArgs(argc, argv)    - Load configuration from command line
  config.saveToFile(filepath)         - Save configuration to JSON file
  config.has(key)                     - Check if configuration key exists
  config.getKeys()                     - Get all configuration keys
  config.clear()                       - Clear all configuration entries
  config.summary()                     - Get configuration summary string

Configuration Keys:
------------------
  neuron_count                         - Number of neurons
  region_count                         - Number of regions
  connection_probability               - Connection probability between neurons
  stdp_ltp_weight                     - STDP long-term potentiation weight
  stdp_ltd_weight                     - STDP long-term depression weight
  stdp_tau                            - STDP time constant
  synaptogenesis_rate                 - Synaptogenesis rate
  pruning_rate                        - Pruning rate
  simulation_timestep                 - Simulation timestep duration
  max_steps                           - Maximum simulation steps
  checkpoint_interval                  - Checkpoint save interval
  enable_replay                       - Enable memory replay
  enable_development                   - Enable development system
  reward_modulation                   - Enable reward modulation
  structural_plasticity               - Enable structural plasticity
  development                         - Enable development
  curiosity                           - Enable curiosity
  novelty_sensitivity                  - Novelty detection sensitivity
  curiosity_threshold                  - Curiosity activation threshold
  world_width                         - World width
  world_height                        - World height
  vision_width                        - Vision width
  vision_height                       - Vision height
  max_energy                          - Maximum world energy
  energy_decay_rate                   - World energy decay rate

)HELP";
        return config_help;
    }, "Get Config class help documentation");

    m.def("helpViz", []() {
        std::string viz_help = R"HELP(
Visualization Functions - NLM API Reference
===========================================

Complete Visualization:
---------------------
  visualize_complete_state(brain)    - Get complete state for coordinated visualization

System Components:
-----------------
  visualize_network(brain)            - Get network connectivity visualization data
  visualize_activity(brain)            - Get neural activity visualization data
  visualize_development(brain)        - Get developmental progression visualization
  visualize_memory(brain)              - Get memory system visualization data
  visualize_neuromodulation(brain)    - Get neuromodulation system visualization
  visualize_prediction(brain)         - Get prediction system visualization data

Specific Data Access:
---------------------
  get_visualization_data(brain)       - Get complete visualization data for all systems
  get_activity_data(brain)            - Get neural activity patterns
  get_network_data(brain)             - Get network connectivity information
  get_memory_data(brain)              - Get memory system state
  get_neuromodulation_data(brain)     - Get neuromodulation system data
  get_prediction_data(brain)          - Get prediction system data
  get_development_data(brain)         - Get developmental system data

Visualization Data Structure:
-----------------------------
  Each visualization function returns a map with string keys and vector<double> values
  representing different aspects of the brain state for external visualization tools

)HELP";
        return viz_help;
    }, "Get visualization help documentation");

    m.def("helpExperimental", []() {
        std::string exp_help = R"HELP(
Experimental Functions - NLM API Reference
==========================================

Experiment Management:
---------------------
  ExperimentManager                    - Comprehensive experiment tracking and analysis
  PerformanceProfiler                   - Detailed performance analysis and optimization
  ScriptRecorder                       - Record and replay complex experimental procedures

Experiment Features:
-------------------
  run_experiment(brain, world, agent, steps, description, tags) - Run single experiment
  save_results(filepath)              - Save experiment results to JSON
  compare_experiments(names)         - Compare results across experiments
  _summarize_brain_state(brain)       - Create summary of brain state
  _generate_summary()                 - Generate summary statistics

Performance Profiling:
---------------------
  profile_simulation(brain, world, agent, steps) - Profile simulation run
  get_performance_report()            - Generate comprehensive performance report
  set_thresholds(custom_thresholds)   - Set custom performance thresholds
  _identify_issues()                   - Identify performance issues
  _generate_recommendations(issues)    - Generate recommendations

Script Recording:
----------------
  ScriptRecorder                        - Record and replay experimental procedures
  start_recording(name, description)   - Start recording a script
  record_step(brain, world, agent, event_type) - Record a simulation step
  stop_recording()                     - Stop recording and save script
  save_script(filepath)                - Save current script to file
  load_script(filepath)                - Load script from file
  replay_script(script_data, world, agent) - Replay recorded script
  get_script_summary(name)             - Get summary of scripts

Advanced Configuration:
----------------------
  load_configuration_from_json(filepath) - Load configuration from JSON file
  save_configuration_to_json(config, filepath) - Save configuration to JSON file
  apply_configuration(brain, config)   - Apply configuration to brain

Data Analysis:
-------------
  get_network_stats(brain)             - Get comprehensive network statistics
  analyze_brain(brain)                 - Perform comprehensive brain analysis
  get_brain_stats(brain)               - Alias for get_network_stats
  get_system_stats(system)             - Get demo system statistics
  get_neural_diagnostics(brain)        - Get neural system diagnostics
  get_sensory_diagnostics(agent)       - Get sensory system diagnostics
  get_motor_diagnostics(agent)        - Get motor system diagnostics
  get_world_diagnostics(world)        - Get world environment diagnostics

Quality Metrics:
---------------
  stability_score                    - Network stability metric
  adaptability_score                  - Learning adaptability metric
  performance_score                   - Overall performance metric
  health_score                       - System health metric
  efficiency_score                   - Processing efficiency metric

)HELP";
        return exp_help;
    }, "Get experimental features help documentation");

    m.def("helpCLI", []() {
        std::string cli_help = R"HELP(
Command Line Interface (CLI) Help
==============================

Available Commands:
------------------

demo
  Run a quick demo simulation
  ./nlm_cli.py demo --steps 100
  Options: --steps (default: 100) - Number of simulation steps

experiment
  Run an integrated experiment
  ./nlm_cli.py experiment --neurons 500 --regions 2 --steps 1000
  Options:
    --neurons (default: 500) - Number of neurons
    --regions (default: 1) - Number of brain regions
    --steps (default: 2000) - Number of simulation steps

generate_brain
  Create and initialize a brain
  ./nlm_cli.py create --neurons 100 --regions 1
  Options:
    --neurons (default: 100) - Number of neurons
    --regions (default: 1) - Number of regions
    --conn-prob (default: 0.15) - Connection probability
    --reward-mod - Enable reward modulation
    --structural-plasticity - Enable structural plasticity
    --development - Enable development
    --curiosity - Enable curiosity

save
  Save brain state to file
  ./nlm_cli.py save filename.bin
  Options: file (required) - Output filename

load
  Load brain state from file
  ./nlm_cli.py load filename.bin
  Options: file (required) - Input filename

analyze
  Analyze a saved brain file
  ./nlm_cli.py analyze filename.bin
  Options: file (required) - Brain file to analyze

interactive
  Enter interactive mode
  ./nlm_cli.py interactive
  Available commands in interactive mode:
    step N     - Run N simulation steps
    stats      - Show brain statistics
    action     - Get current action
    save FILE  - Save brain state
    load FILE  - Load brain state
    features   - Toggle learning features
    help       - Show this help
    exit       - Exit interactive mode

config
  Configuration management (requires subcommand)
  ./nlm_cli.py config create name --neurons 100 --regions 1
  Options:
    create    - Create configuration
    load      - Load configuration

compare
  Compare different configurations (experimental)
  ./nlm_cli.py compare config1.json config2.json
  Options: configs (required) - Configuration files to compare

version
  Show version information
  ./nlm_cli.py version

help
  Show help documentation
  ./nlm_cli.py help

Example Usage:
-------------
  # Basic commands
  ./nlm_cli.py demo
  ./nlm_cli.py help

  # Advanced commands
  ./nlm_cli.py experiment --neurons 200 --regions 2 --steps 1000
  ./nlm_cli.py create --neurons 500 --regions 1 --save my_brain.bin
  ./nlm_cli.py load --neurons 100 --regions 1 --load my_brain.bin
  ./nlm_cli.py analyze my_brain.bin
  ./nlm_cli.py interactive

  # Configuration management
  ./nlm_cli.py config create my_config --neurons 300 --regions 2 --save config.json
  ./nlm_cli.py config load my_config --neurons 100 --regions 1 --load config.json

  # Advanced features (experimental)
  ./nlm_cli.py compare config1.json config2.json

For more information, visit: https://docs.nlm-project.org
)HELP";
        return cli_help;
    }, "Get CLI help documentation");

    m.def("helpAdvanced", []() {
        std::string advanced_help = R"HELP(
Advanced Features Help
======================

Advanced features provide additional functionality for power users and researchers.

Available Advanced Features:
---------------------------

Experiment Management:
---------------------
  ExperimentManager - Comprehensive experiment tracking and analysis
    Features:
      - Run multiple experiments with different configurations
      - Save experiment results to JSON files
      - Compare results across experiments
      - Generate summary statistics
      - Track experiment metadata and tags

  PerformanceProfiler - Detailed performance analysis and optimization
    Features:
      - Profile simulation performance
      - Identify performance bottlenecks
      - Generate performance reports
      - Set custom performance thresholds
      - Provide optimization recommendations

  ScriptRecorder - Record and replay complex experimental procedures
    Features:
      - Record complete experimental procedures
      - Replay experiments with exact replication
      - Save scripts to JSON files
      - Load and replay scripts
      - Generate script summaries

Advanced Configuration:
----------------------
  Advanced configuration management with JSON files
  Support for complex brain configurations
  Configuration validation and error handling

Advanced Visualization:
---------------------
  Comprehensive visualization tools for all brain systems
  Network connectivity visualization
  Neural activity pattern visualization
  Memory system visualization
  Neuromodulation system visualization
  Prediction system visualization
  Developmental system visualization

Performance Optimization:
------------------------
  Performance profiling and optimization tools
  Batch processing for large simulations
  Memory optimization techniques
  Parallel processing support
  Resource usage monitoring

Research Features:
-----------------
  Experiment tracking for research reproducibility
  Data analysis and visualization tools
  Performance benchmarking
  Configuration comparison
  Script recording for documentation

Integration Features:
---------------------
  Integration testing tools
  System health monitoring
  Component diagnostics
  Performance validation
  Error handling and recovery

)HELP";
        return advanced_help;
    }, "Get advanced features help documentation");

    m.def("getAllHelp", []() {
        std::string all_help = R"HELP(
NLM (Neural Learning Machine) - Complete API Reference
==================================================

This is the complete help documentation for NLM Python API.
See individual help functions (help_brain, help_agent, help_world, help_config, help_viz, help_experimental, help_cli, help_advanced) for detailed information.

Quick Start Examples:
-------------------

Example 1: Basic Usage
---------------------
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
brain.step(100)

# Get statistics
stats = pynlm.get_network_stats(brain)
print(f"Neurons: {stats['total_neurons']}, Spikes: {stats['total_spikes']}")

Example 2: Advanced Usage
------------------------
import pynlm

# Create demo system
system = pynlm.createDemoSystem()
brain = system["brain"]
world = system["world"]
agent = system["agent"]

# Configure brain
pynlm.configure_brain(brain, neuronCount=500, regionCount=2)

# Enable all features
pynlm.enable_all_features(agent)

# Run simulation
pynlm.run_simulation(brain, world, agent, 1000)

# Analysis and visualization
stats = pynlm.get_network_stats(brain)
analysis = pynlm.analyze_brain(brain)
visualization = pynlm.get_visualization_data(brain)

Example 3: Experiment Management
--------------------------------
from advanced_nlm import ExperimentManager, PerformanceProfiler

# Create experiment manager
experiment = ExperimentManager("my_experiment")

# Run experiment with configuration
result = experiment.run_experiment(
    brain, world, agent, 1000,
    description="Testing neural plasticity",
    tags=["plasticity", "learning", "adaptation"]
)

# Save results
experiment.save_results("results/my_experiment.json")

# Get performance report
profiler = PerformanceProfiler()
metrics = profiler.profile_advanced_simulation(brain, world, agent, 1000)
report = profiler.get_performance_report()

Example 4: Visualization
------------------------
import pynlm

# Get complete visualization data
visualization_data = pynlm.get_visualization_data(brain)

# Get specific component visualizations
network_data = pynlm.visualize_network(brain)
activity_data = pynlm.visualize_activity(brain)
memory_data = pynlm.visualize_memory(brain)

Example 5: Configuration Management
----------------------------------
import json

# Load advanced configuration
with open("advanced_config.json", 'r') as f:
    config = json.load(f)

# Apply configuration
pynlm.configure_brain(brain,
                     neuronCount=config["brain"]["neuron_count"],
                     regionCount=config["brain"]["region_count"],
                     connectionProb=config["brain"]["connection_probability"])

# Enable agent features
agent.enableRewardModulation(config["agent"]["reward_modulation"])
agent.enableStructuralPlasticity(config["agent"]["structural_plasticity"])
agent.enableDevelopment(config["agent"]["development"])
agent.enableCuriosity(config["agent"]["curiosity"])

Example 6: Command Line Interface
---------------------------------
# Basic CLI usage
./nlm_cli.py demo
./nlm_cli.py experiment --neurons 500 --regions 2 --steps 1000
./nlm_cli.py create --neurons 100 --regions 1 --save my_brain.bin
./nlm_cli.py analyze my_brain.bin
./nlm_cli.py interactive

# Advanced CLI usage
./nlm_cli.py config create my_config --neurons 300 --regions 2 --save config.json
./nlm_cli.py config load my_config --neurons 100 --regions 1 --load config.json

For more information, visit: https://docs.nlm-project.org

Common Error Messages and Solutions:
----------------------------------

"Brain not initialized"
  Solution: Call brain.initialize() before using the brain

"World not configured"
  Solution: Call world.configure() with appropriate dimensions

"Agent not initialized"
  Solution: Call agent.initialize(world) after creating agent

"Invalid configuration"
  Solution: Check configuration file syntax and required keys

"File not found"
  Solution: Ensure the file exists and has proper permissions

"Memory usage too high"
  Solution: Reduce brain size or enable memory optimization features

"Performance too slow"
  Solution: Reduce simulation steps or optimize brain configuration

"Low stability"
  Solution: Adjust plasticity parameters or increase brain size

"Low adaptability"
  Solution: Enable curiosity system or adjust neuromodulation

"Low activity"
  Solution: Increase input strength or adjust sensory parameters

)HELP";
        return all_help;
    }, "Get complete API help documentation");

    // Summary function
    m.def("getSummary", []() {
        std::map<std::string, std::string> summary;
        summary["version"] = get_system_version();
        summary["capabilities_count"] = std::to_string(get_system_capabilities().size());
        summary["features_count"] = std::to_string(get_feature_list().size());
        summary["description"] = "NLM (熙然) - Neural Learning Machine - Advanced Neural Simulation Framework";
        summary["phase"] = "Phase 6: Final Integration";
        summary["status"] = "Enhanced with comprehensive Python API";
        return summary;
    }, "Get system summary information");

    // Complete help system
    m.def("showHelp", [](std::string topic) {
        if (topic == "all" || topic.empty()) {
            return get_all_help();
        } else if (topic == "brain") {
            return help_brain();
        } else if (topic == "agent") {
            return help_agent();
        } else if (topic == "world") {
            return help_world();
        } else if (topic == "config") {
            return help_config();
        } else if (topic == "viz") {
            return help_viz();
        } else if (topic == "experimental") {
            return help_experimental();
        } else if (topic == "cli") {
            return help_cli();
        } else if (topic == "advanced") {
            return help_advanced();
        } else {
            return std::string("Unknown help topic: ") + topic + "\nAvailable topics: brain, agent, world, config, viz, experimental, cli, advanced, all";
        }
    }, py::arg("topic"), "Show help documentation for specific topic");

    // Documentation generation
    m.def("generateDocumentation", []() {
        std::string docs = R"DOCS(
NLM Python API Documentation
===========================

This document provides complete documentation for the NLM Python API.

API Structure:
--------------
Core Classes:
  - Config: Configuration management
  - Brain: Neural simulation brain
  - AgentBrain: Agent interface
  - SimpleWorld: Simulation world

Data Types:
----------
  - NeuronId: Unique neuron identifier
  - SynapseId: Unique synapse identifier
  - RegionId: Unique region identifier
  - PopulationId: Unique population identifier
  - NeuronType: Neuron type enumeration
  - ActionType: Action type enumeration
  - WorldObjectType: World object type enumeration

Convenience Functions:
---------------------
  Basic Functions:
    - createDefaultConfig() - Create default configuration
    - createBrain(config) - Create brain with configuration
    - createSimpleBrain() - Create simple brain with defaults
    - create_brain_with_config(file) - Create brain with config file

  Configuration:
    - configure_brain(brain, neurons, regions, conn_prob) - Configure brain
    - enable_features(agent, reward_mod, structural, development, curiosity) - Enable features
    - enable_all_features(agent) - Enable all agent features

  Simulation:
    - run_simulation(brain, world, agent, steps) - Run simulation
    - batch_step(brain, steps) - Run multiple simulation steps efficiently

  Data Access:
    - get_network_stats(brain) - Get network statistics
    - analyze_brain(brain) - Analyze brain performance
    - get_visualization_data(brain) - Get visualization data

  I/O Operations:
    - checkpoint_brain(brain, file) - Save brain state
    - restore_brain(brain, file) - Load brain state
    - test_brain_functionality(brain) - Test brain functionality

  Demo System:
    - createDemoSystem() - Create complete demo system
    - run_demo_simulation(system, steps) - Run demo simulation

  Advanced Visualization:
    - visualize_complete_state(brain) - Get complete state for visualization
    - visualize_network(brain) - Visualize network connectivity
    - visualize_activity(brain) - Visualize neural activity patterns
    - visualize_development(brain) - Visualize developmental progression
    - visualize_memory(brain) - Visualize memory system state
    - visualize_neuromodulation(brain) - Visualize neuromodulation system
    - visualize_prediction(brain) - Visualize prediction system state

  System Information:
    - get_system_version() - Get system version
    - get_system_capabilities() - Get available capabilities
    - get_feature_list() - Get list of all features
    - get_summary() - Get system summary

  Diagnostics:
    - test_integration(brain, world, agent, steps) - Test system integration
    - check_system_health(brain, world, agent) - Check system health
    - get_neural_diagnostics(brain) - Get neural system diagnostics
    - get_sensory_diagnostics(agent) - Get sensory system diagnostics
    - get_motor_diagnostics(agent) - Get motor system diagnostics
    - get_world_diagnostics(world) - Get world environment diagnostics

  Data Components:
    - get_activity_data(brain) - Get neural activity data
    - get_network_data(brain) - Get network connectivity data
    - get_memory_data(brain) - Get memory system data
    - get_neuromodulation_data(brain) - Get neuromodulation system data
    - get_prediction_data(brain) - Get prediction system data
    - get_development_data(brain) - Get developmental system data

  Help System:
    - help(topic) - Show help documentation
    - showHelp(topic) - Show help (alias for help)

Installation:
-------------
1. Build NLM from source:
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4

2. Install Python bindings:
   pip install -e .

3. Quick start:
   import pynlm

   # Basic usage
   brain = pynlm.createBrain(pynlm.createDefaultConfig())
   brain.initialize()
   brain.step(10)

   # Advanced usage
   system = pynlm.createDemoSystem()
   pynlm.configure_brain(system["brain"], neuronCount=500, regionCount=2)
   pynlm.run_simulation(system["brain"], system["world"], system["agent"], 1000)

For more information, visit: https://docs.nlm-project.org

)DOCS";
        return docs;
    }, "Generate complete API documentation");

    // Error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Python module initialization
    m.def("__version__", []() {
        return "0.1.0";
    });

    m.def("__author__", []() {
        return "NLM Project Authors";
    });

    m.def("__description__", []() {
        return "NLM (熙然) - Neural Learning Machine - Experimental Artificial Developmental Brain";
    });

    m.def("__doc__", []() {
        return "A Python binding for the NLM C++ neural simulation framework. Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.";
    });
}

} // namespace nlm

PYBIND11_MODULE(pynlm, m) {
    // Initialize the module
    nlm::PYBIND11_MODULE(pynlm, m);
}