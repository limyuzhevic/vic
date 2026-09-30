    // Advanced simulation control
    m.def("runAdvancedSimulation", [](std::shared_ptr<Brain>& brain, std::shared_ptr<AgentBrain>& agent,
                                      std::shared_ptr<SimpleWorld>& world, size_t steps,
                                      const std::map<std::string, float>& parameters) {
        // Run advanced simulation with custom parameters
        float totalReward = 0.0f;
        size_t successfulActions = 0;
        
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get perception
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get motor command
            auto cmd = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(cmd, world->getSimulationTime());
            
            // Check if action was successful
            if (cmd.getType() != ActionType::Wait && cmd.getType() != ActionType::Rest) {
                successfulActions++;
            }
            
            // Optional: Apply custom neuromodulation based on parameters
            if (parameters.count("dopamine_boost")) {
                auto dopamine = brain->getDopamine();
                if (dopamine) {
                    float boost = parameters.at("dopamine_boost");
                    dopamine->setLevel(std::min(1.0f, dopamine->getLevel() + boost));
                }
            }
        }
        
        return std::make_tuple(totalReward / steps, 
                               successfulActions / (float)steps,
                               brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
         py::arg("parameters") = std::map<std::string, float>(),
         "Run advanced simulation with custom parameters");
    
    // Performance monitoring
    m.def("getPerformanceMetrics", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> metrics;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["firing_rate"] = brain->getAverageFiringRate();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        metrics["active_neurons"] = brain->getActiveNeuronCount();
        
        if (brain->getWorkingMemory()) {
            metrics["memory_activity"] = brain->getWorkingMemory()->getMemoryActivity();
            metrics["memory_traces"] = brain->getWorkingMemory()->getActiveTraces();
        }
        
        // Add development metrics
        metrics["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return metrics;
    }, py::arg("brain"), "Get comprehensive performance metrics");
    
    // Advanced memory management
    m.def("triggerMemoryConsolidation", [](std::shared_ptr<Brain>& brain, float strength = 0.5f) {
        if (brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->consolidate(strength);
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("strength") = 0.5f,
         "Trigger memory consolidation process");
    
    m.def("triggerReplay", [](std::shared_ptr<Brain>& brain, size_t episodeCount = 5) {
        if (brain->getEpisodicMemory()) {
            auto episodes = brain->getEpisodicMemory()->getEpisodesForReplay(episodeCount);
            for (const auto* episode : episodes) {
                brain->getEpisodicMemory()->replayEpisode(episode);
            }
            return episodes.size();
        }
        return size_t(0);
    }, py::arg("brain"), py::arg("episodeCount") = 5,
         "Trigger episodic memory replay with specified number of episodes");
    
    // Agent behavior customization
    m.def("setAgentBehaviorProfile", [](std::shared_ptr<AgentBrain>& agent, const std::string& profile) {
        if (profile == "exploratory") {
            agent->enableCuriosity(true);
            agent->enableRewardModulation(false);
            agent->enableDevelopment(true);
        } else if (profile == "exploitative") {
            agent->enableCuriosity(true);
            agent->enableRewardModulation(true);
            agent->enableDevelopment(false);
        } else if (profile == "balanced") {
            agent->enableCuriosity(true);
            agent->enableRewardModulation(true);
            agent->enableDevelopment(true);
        }
        return agent;
    }, py::arg("agent"), py::arg("profile"),
         "Set agent behavior profile (exploratory, exploitative, balanced)");
    
    // Simulation debugging tools
    m.def("debugSimulationStep", [](std::shared_ptr<Brain>& brain, size_t step) {
        std::map<std::string, double> debugInfo;
        debugInfo["step"] = step;
        debugInfo["firing_rate"] = brain->getAverageFiringRate();
        debugInfo["spike_count"] = brain->getTotalSpikeCount();
        debugInfo["active_neurons"] = brain->getActiveNeuronCount();
        debugInfo["region_count"] = brain->getRegionCount();
        
        if (brain->getWorkingMemory()) {
            debugInfo["memory_traces"] = brain->getWorkingMemory()->getActiveTraces();
            debugInfo["memory_activity"] = brain->getWorkingMemory()->getMemoryActivity();
        }
        
        if (brain->getDopamine()) {
            debugInfo["dopamine_level"] = brain->getDopamine()->getLevel();
        }
        
        return debugInfo;
    }, py::arg("brain"), py::arg("step"),
         "Get debug information for a specific simulation step");