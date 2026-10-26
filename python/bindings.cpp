// Advanced Brain Control
    m.def("enableRewardModulation", [](std::shared_ptr<AgentBrain> agent, bool enable) {
        agent->enableRewardModulation(enable);
        // For backward compatibility, also access through brain
        if (auto brain = agent->getBrain()) {
            brain->enableRewardModulation(enable);
        }
    }, py::arg("agent"), py::arg("enable"), "Enable/disable reward modulation");

    m.def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
         py::arg("enable"), "Enable structural plasticity for connection growth");

    m.def("enableDevelopment", &AgentBrain::enableDevelopment,
         py::arg("enable"), "Enable developmental system for brain maturation");

    m.def("enableCuriosity", &AgentBrain::enableCuriosity,
         py::arg("enable"), "Enable curiosity-driven exploration");

    // Memory system access
    m.def("getWorkingMemory", &Brain::getWorkingMemory,
         py::return_value_policy::reference_internal,
         "Get working memory system");

    m.def("getEpisodicMemory", &Brain::getEpisodicMemory,
         py::return_value_policy::reference_internal,
         "Get episodic memory system");

    m.def("getAssociativeMemory", &Brain::getAssociativeMemory,
         py::return_value_policy::reference_internal,
         "Get associative memory system");

    m.def("getPredictionSystem", &Brain::getPredictionSystem,
         py::return_value_policy::reference_internal,
         "Get prediction system");

    m.def("getPlanner", &Brain::getPlanner,
         py::return_value_policy::reference_internal,
         "Get neural planner");

    m.def("getConceptFormation", &Brain::getConceptFormation,
         py::return_value_policy::reference_internal,
         "Get concept formation system");

    m.def("getAttention", &Brain::getAttention,
         py::return_value_policy::reference_internal,
         "Get attentional selection system");

    // Neuromodulation system access
    m.def("getDopamine", &Brain::getDopamine,
         py::return_value_policy::reference_internal,
         "Get dopamine neuromodulator");

    m.def("getCuriosity", &Brain::getCuriosity,
         py::return_value_policy::reference_internal,
         "Get curiosity neuromodulator");

    m.def("getNovelty", &Brain::getNovelty,
         py::return_value_policy::reference_internal,
         "Get novelty neuromodulator");

    m.def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
         py::return_value_policy::reference_internal,
         "Get prediction error signal");

    // Development system access
    m.def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
         py::return_value_policy::reference_internal,
         "Get development system");

    // Statistics and monitoring
    m.def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
         "Get total spike count");

    m.def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
         "Get count of currently firing neurons");

    m.def("getAverageFiringRate", &Brain::getAverageFiringRate,
         "Get average firing rate across all neurons");

    m.def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
         "Get excitation/inhibition balance ratio");

    m.def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
         "Get count of pending spike events");

    m.def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
         "Get count of active neurons");

    // Simulation control
    m.def("resetSimulation", &Brain::reset,
         "Reset brain state and start fresh simulation");

    m.def("saveBrainState", &Brain::save, py::arg("filepath"),
         "Save brain state to file (checkpoint)");

    m.def("loadBrainState", &Brain::load, py::arg("filepath"),
         "Load brain state from file");

    // Configuration utilities
    m.def("getConfigSummary", &Config::summary,
         "Get configuration summary string");

    m.def("hasConfigKey", &Config::has, py::arg("key"),
         "Check if configuration key exists");

    m.def("getConfigKeys", &Config::getKeys,
         "Get all configuration keys");

    // World simulation control
    m.def("resetWorld", &SimpleWorld::reset,
         "Reset world to initial state");

    m.def("setWorldRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
         "Set random seed for world simulation");

    m.def("getWorldSimulationTime", &SimpleWorld::getSimulationTime,
         "Get current simulation time");

    // Agent control
    m.def("resetAgent", &AgentBrain::reset,
         "Reset agent for new episode");

    m.def("getAgentCuriosityLevel", &AgentBrain::getCuriosityLevel,
         "Get current curiosity level");

    m.def("getAgentNoveltyLevel", &AgentBrain::getNoveltyLevel,
         "Get current novelty level");

    m.def("getAgentPredictionError", &AgentBrain::getPredictionError,
         "Get current prediction error");

    m.def("getAgentNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
         "Get current neuromodulation level");

    m.def("getAgentDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
         "Get current developmental stage");

    // Advanced simulation utilities
    m.def("runSimulationSteps", [](std::shared_ptr<Brain> brain, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
    }, py::arg("brain"), py::arg("steps"),
         "Run simulation for specified number of steps");

    m.def("runAgentEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
         "Run complete agent episode with brain, agent, and world");

    // Memory manipulation
    m.def("clearWorkingMemory", [](std::shared_ptr<Brain> brain) {
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->clear();
        }
    }, py::arg("brain"),
         "Clear working memory");

    m.def("clearEpisodicMemory", [](std::shared_ptr<Brain> brain) {
        if (brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->clear();
        }
    }, py::arg("brain"),
         "Clear episodic memory");

    // Performance monitoring
    m.def("getBrainStatistics", &Brain::logStatus,
         "Get comprehensive brain statistics (logs to console)");

    // Configuration modification
    m.def("setConfigValue", [](std::shared_ptr<Config> config, std::string key, std::string value) {
        config->set(key, value, ConfigSource::CommandLine);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
         "Set configuration value");

    m.def("setConfigInt", [](std::shared_ptr<Config> config, std::string key, int value) {
        config->set(key, value, ConfigSource::CommandLine);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
         "Set configuration integer value");

    m.def("setConfigFloat", [](std::shared_ptr<Config> config, std::string key, float value) {
        config->set(key, value, ConfigSource::CommandLine);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
         "Set configuration float value");

    m.def("setConfigBool", [](std::shared_ptr<Config> config, std::string key, bool value) {
        config->set(key, value, ConfigSource::CommandLine);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
         "Set configuration boolean value");

    // Error handling and validation
    m.def("validateBrainConfiguration", [](std::shared_ptr<Brain> brain) {
        return brain->validateConfiguration();
    }, py::arg("brain"),
         "Validate brain configuration");

    // Utility functions
    m.def("createBrainWithConfig", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
         "Create brain with specific configuration");

    m.def("createSimpleWorldWithConfig", [](size_t width, size_t height,
                                           size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
         "Create and configure simple world");

    // Version and metadata
    m.attr("version") = "0.1.0";
    m.attr("description") = "NLM (Neural Learning Machine) - Advanced Brain Simulator";
    m.attr("authors") = "NLM Research Team";
    m.attr("features") = py::dict(
        "neurons", "Leaky Integrate-and-Fire spiking neurons",
        "plasticity", "STDP and Hebbian learning",
        "memory", "Working and episodic memory systems",
        "prediction", "Temporal sequence prediction",
        "cognition", "Planning and concept formation",
        "neuromodulation", "Dopamine, curiosity, novelty",
        "development", "Biologically-inspired maturation",
        "curiosity", "Exploration motivation",
        "structural_plasticity", "Connection growth and pruning"
    );