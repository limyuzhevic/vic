    // Simulation monitoring and observation functions
    m.def("run_simulation_with_monitoring", [](
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        int numSteps,
        const std::function<void(int step, float reward, float novelty, float curiosity)>& metricsCallback,
        const std::function<void(int step, const std::vector<float>& neuralActivity)>& activityCallback,
        const std::function<bool(int step, const std::string& status)>& checkpointCallback,
        const std::function<void(const std::string& message)>& logCallback
    ) {
        checkNotNull(brain, "Brain cannot be null");
        checkNotNull(world, "World cannot be null");
        checkNotNull(agent, "Agent cannot be null");
        
        float totalReward = 0.0f;
        float maxNovelty = 0.0f;
        float maxCuriosity = 0.0f;
        
        for (int step = 0; step < numSteps; ++step) {
            // Get pre-step metrics
            float novelty = agent->getNoveltyLevel();
            float curiosity = agent->getCuriosityLevel();
            float reward = 0.0f; // Will be updated after action
            
            // Update world and get percept
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            
            // Get neural activity for monitoring
            std::vector<float> neuralActivity;
            if (auto workingMem = brain->getWorkingMemory()) {
                neuralActivity = workingMem->getWorkingMemory();
            }
            
            // Call activity monitoring callback
            if (activityCallback) {
                activityCallback(step, neuralActivity);
            }
            
            // Process input and step brain
            agent->processSensoryInput(percept);
            brain->step(step);
            
            // Decode action and execute
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            reward = result.reward;
            totalReward += reward;
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Call metrics callback
            if (metricsCallback) {
                metricsCallback(step, reward, novelty, curiosity);
            }
            
            // Check for checkpoints
            if (checkpointCallback && checkpointCallback(step, "simulating")) {
                // Save checkpoint if requested
                std::string checkpointFile = "checkpoint_step_" + std::to_string(step) + ".nlm";
                brain->save(checkpointFile);
            }
            
            // Log progress periodically
            if (logCallback && step % 100 == 0) {
                logCallback("Step " + std::to_string(step) + ": Reward=" + std::to_string(reward) + 
                           ", Novelty=" + std::to_string(novelty) + ", Curiosity=" + std::to_string(curiosity));
            }
            
            // Update max values
            maxNovelty = std::max(maxNovelty, novelty);
            maxCuriosity = std::max(maxCuriosity, curiosity);
        }
        
        // Log final summary
        if (logCallback) {
            logCallback("Simulation completed: Total Reward=" + std::to_string(totalReward) + 
                       ", Max Novelty=" + std::to_string(maxNovelty) + 
                       ", Max Curiosity=" + std::to_string(maxCuriosity));
        }
        
        return std::make_tuple(totalReward, maxNovelty, maxCuriosity);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
          py::arg("metricsCallback"), py::arg("activityCallback"), py::arg("checkpointCallback"), py::arg("logCallback"),
          R"pbdoc(
        Run a simulation with comprehensive monitoring callbacks
        
        Args:
            brain: The NLM brain to simulate
            world: The simulation world
            agent: Agent brain interface
            numSteps: Number of simulation steps
            metricsCallback: Called with (step, reward, novelty, curiosity)
            activityCallback: Called with (step, neuralActivity) for detailed monitoring
            checkpointCallback: Called with (step, status), returns true to save checkpoint
            logCallback: Called with log messages for custom logging
        
        Returns:
            Tuple of (total_reward, max_novelty, max_curiosity)
        )pbdoc");

    m.def("brain_observe", [](std::shared_ptr<Brain> brain) {
        checkNotNull(brain, "Brain cannot be null");
        
        // Get comprehensive brain state for observation
        auto state = std::make_shared<py::dict>();
        
        // Basic statistics
        (*state)["total_neurons"] = brain->getTotalNeuronCount();
        (*state)["total_synapses"] = brain->getTotalSynapseCount();
        (*state)["active_neurons"] = brain->getActiveNeuronCount();
        (*state)["firing_neurons"] = brain->getFiringNeuronCount();
        (*state)["total_spikes"] = brain->getTotalSpikeCount();
        (*state)["avg_firing_rate"] = brain->getAverageFiringRate();
        (*state)["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        (*state)["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Get region information
        auto regions = brain->getRegions();
        auto region_list = std::make_shared<py::list>();
        for (const auto& region : regions) {
            auto region_info = std::make_shared<py::dict>();
            (*region_info)["id"] = region->getId();
            (*region_info)["neuron_count"] = region->getNeuronCount();
            (*region_info)["synapse_count"] = region->getSynapseCount();
            region_list->append(*region_info);
        }
        (*state)["regions"] = *region_list;
        
        // Get working memory if available
        if (auto workingMem = brain->getWorkingMemory()) {
            (*state)["working_memory_size"] = workingMem->getWorkingMemory().size();
        }
        
        return *state;
    }, py::arg("brain"),
          "Get comprehensive brain state observation for debugging and monitoring");

    m.def("create_simulation_logger", []() {
        auto logger = std::make_shared<py::dict>();
        
        // Initialize logger with standard metrics
        (*logger)["step_count"] = 0;
        (*logger)["total_reward"] = 0.0f;
        (*logger)["max_reward"] = -1000000.0f;
        (*logger)["min_reward"] = 1000000.0f;
        (*logger)["avg_reward"] = 0.0f;
        (*logger)["reward_variance"] = 0.0f;
        (*logger)["max_novelty"] = 0.0f;
        (*logger)["max_curiosity"] = 0.0f;
        (*logger)["firing_rate_history"] = std::make_shared<py::list>();
        (*logger)["action_distribution"] = std::make_shared<py::dict>();
        
        return *logger;
    },
          "Create a simulation logger for tracking metrics");

    m.def("update_simulation_logger", [](std::shared_ptr<py::dict> logger, 
                                        int step,
                                        float reward,
                                        float novelty,
                                        float curiosity,
                                        float firingRate) {
        if (!logger) return;
        
        (*logger)["step_count"] = step;
        
        // Update reward statistics
        float total_reward = (*logger)["total_reward"].cast<float>() + reward;
        (*logger)["total_reward"] = total_reward;
        
        float max_reward = (*logger)["max_reward"].cast<float>();
        if (reward > max_reward) (*logger)["max_reward"] = reward;
        
        float min_reward = (*logger)["min_reward"].cast<float>();
        if (reward < min_reward) (*logger)["min_reward"] = reward;
        
        // Update firing rate history
        auto history = (*logger)["firing_rate_history"].cast<std::shared_ptr<py::list>>();
        history->append(firingRate);
        
        // Keep only last 1000 entries
        if (history->size() > 1000) {
            history->erase(history->begin());
        }
        
        // Update novelty and curiosity maxima
        float max_novelty = (*logger)["max_novelty"].cast<float>();
        if (novelty > max_novelty) (*logger)["max_novelty"] = novelty;
        
        float max_curiosity = (*logger)["max_curiosity"].cast<float>();
        if (curiosity > max_curiosity) (*logger)["max_curiosity"] = curiosity;
        
    }, py::arg("logger"), py::arg("step"), py::arg("reward"), py::arg("novelty"), py::arg("curiosity"), py::arg("firingRate"),
          "Update simulation logger with new metrics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}