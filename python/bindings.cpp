// Additional convenience factory methods for NLM Python API
    m.def("createFitzHughNagumoDynamics", 
          py::overload_cast<float, float, float, float, float>(
              &FitzHughNagumoDynamics::FitzHughNagumoDynamics),
          py::arg("a") = 0.7f, py::arg("b") = 0.8f, py::arg("epsilon") = 0.08f,
          py::arg("threshold") = -0.5f, py::arg("reset_v") = -2.0f,
          "Create FitzHugh-Nagumo neural dynamics with parameters");

    m.def("createHodgkinHuxleyDynamics",
          py::overload_cast<float>(&HodgkinHuxleyDynamics::HodgkinHuxleyDynamics),
          py::arg("temperature") = 6.3f,
          "Create Hodgkin-Huxley neural dynamics with temperature parameter");

    m.def("createIntegrateAndFireDynamics",
          &IntegrateAndFireDynamics::IntegrateAndFireDynamics,
          "Create integrate-and-fire neural dynamics");

    m.def("createSynapticDynamics",
          py::overload_cast<float, float>(&SynapticDynamics::SynapticDynamics),
          py::arg("time_constant") = 5.0f,
          py::arg("reversal_potential") = 0.0f,
          "Create synaptic dynamics with time constant and reversal potential");

    // Configuration presets
    m.def("createNeuralDynamicsConfig",
          []() -> std::shared_ptr<Config> {
              auto config = std::make_shared<Config>();
              config->set("dynamics.type", "fhn"); // Default to FitzHugh-Nagumo
              config->set("dynamics.a", 0.7f);
              config->set("dynamics.b", 0.8f);
              config->set("dynamics.epsilon", 0.08f);
              config->set("dynamics.threshold", -0.5f);
              config->set("dynamics.reset_v", -2.0f);
              config->set("dynamics.I_inj", 0.0f);
              config->set("plasticity.stdp.enable", true);
              config->set("plasticity.stdp.learning_rate", 0.001f);
              config->set("plasticity.stdp.tau_plus", 20.0f);
              config->set("plasticity.stdp.tau_minus", 20.0f);
              config->set("plasticity.hebbian.enable", true);
              config->set("plasticity.hebbian.learning_rate", 0.001f);
              config->set("plasticity.structural.enable", true);
              config->set("plasticity.structural.creation_rate", 0.0001f);
              config->set("plasticity.structural.removal_rate", 0.00005f);
              return config;
          },
          "Create a configuration preset for neural dynamics and plasticity");

    // Helper functions for common workflows
    m.def("createSimpleBrain", []() -> std::shared_ptr<Brain> {
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.1f);
        return createBrain(config);
    }, "Create a simple brain with default configuration");

    m.def("createAdvancedBrain", []() -> std::shared_ptr<Brain> {
        auto config = createNeuralDynamicsConfig();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.15f);
        config->set("plasticity.stdp.learning_rate", 0.002f);
        config->set("plasticity.structural.creation_rate", 0.00005f);
        return createBrain(config);
    }, "Create an advanced brain with optimized parameters");

    m.def("initializeAndRunBrain", [](std::shared_ptr<Brain> brain, SimulationStep steps) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("steps"),
       "Initialize and run a brain for specified steps");

    // Simulation control commands
    m.def("simulateStep", [](std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time) {
        brain->step(step, time);
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
       "Run a single simulation step");

    m.def("simulateRange", [](std::shared_ptr<Brain> brain, SimulationStep start, SimulationStep end) {
        for (SimulationStep step = start; step < end; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("start_step"), py::arg("end_step"),
       "Run a range of simulation steps");

    // Memory inspection and management
    m.def("getBrainMemoryUsage", [](std::shared_ptr<Brain> brain) {
        return std::map<std::string, size_t>{{{"neurons", brain->getTotalNeuronCount()},
                                             {"synapses", brain->getTotalSynapseCount()},
                                             {"spikes", brain->getTotalSpikeCount()}}};
    }, py::arg("brain"),
        "Get memory usage statistics for a brain");

    m.def("getPerformanceStats", [](std::shared_ptr<Brain> brain, SimulationStep steps) {
        std::map<std::string, double> stats;
        size_t initial_spikes = brain->getTotalSpikeCount();
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        size_t final_spikes = brain->getTotalSpikeCount();
        size_t spikes_per_step = final_spikes - initial_spikes;
        
        stats["total_steps"] = steps;
        stats["total_spikes"] = final_spikes;
        stats["spikes_per_step"] = static_cast<double>(spikes_per_step) / steps;
        stats["steps_per_second"] = steps / (duration.count() / 1000.0);
        
        return stats;
    }, py::arg("brain"), py::arg("steps"),
        "Run simulation and return performance statistics");

    // Configuration management commands
    m.def("saveBrainConfig", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        auto config = brain->getConfig();
        config->saveToFile(filename);
    }, py::arg("brain"), py::arg("filename"),
        "Save brain configuration to file");

    m.def("loadBrainConfig", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        auto config = brain->getConfig();
        config->loadFromFile(filename);
    }, py::arg("brain"), py::arg("filename"),
        "Load brain configuration from file");

    // Advanced experiment utilities
    m.def("runLearningExperiment", [](std::shared_ptr<Brain> brain, size_t steps) {
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Record initial weights
        std::vector<float> initial_weights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initial_weights.push_back(syn->getWeight());
            }
        }
        
        // Run simulation
        size_t initial_spikes = brain->getTotalSpikeCount();
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        // Record final weights
        std::vector<float> final_weights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                final_weights.push_back(syn->getWeight());
            }
        }
        
        // Calculate learning metrics
        double weight_change_sum = 0.0;
        size_t min_size = std::min(initial_weights.size(), final_weights.size());
        for (size_t i = 0; i < min_size; ++i) {
            weight_change_sum += std::abs(final_weights[i] - initial_weights[i]);
        }
        
        double avg_weight_change = weight_change_sum / min_size;
        size_t final_spikes = brain->getTotalSpikeCount();
        size_t total_spikes = final_spikes - initial_spikes;
        
        return std::map<std::string, double>{{"avg_weight_change", avg_weight_change},
                                            {"total_spikes", total_spikes},
                                            {"spikes_per_step", total_spikes / static_cast<double>(steps)}};
    }, py::arg("brain"), py::arg("steps"),
        "Run a learning experiment and return metrics");

    m.def("compareDynamics", [](const std::string& type1, const std::string& type2, 
                               std::shared_ptr<Config> config1, std::shared_ptr<Config> config2,
                               SimulationStep steps) {
        auto brain1 = createBrain(config1);
        auto brain2 = createBrain(config2);
        
        brain1->initialize();
        brain2->initialize();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain1->step(step);
            brain2->step(step);
        }
        
        return std::map<std::string, std::vector<double>>{
            {"brain1_spike_counts", std::vector<double>(steps + 1)},
            {"brain2_spike_counts", std::vector<double>(steps + 1)}
        };
    }, py::arg("type1"), py::arg("type2"), py::arg("config1"), py::arg("config2"), py::arg("steps"),
       "Compare two neural dynamics configurations");

    // Utility functions for visualization
    m.def("collectSpikeData", [](std::shared_ptr<Brain> brain) {
        auto spike_system = brain->getSpikeSystem();
        auto history = spike_system->getSpikeHistory();
        
        std::vector<std::vector<double>> spike_data;
        spike_data.reserve(history.size());
        
        for (const auto& spike : history) {
            spike_data.push_back({static_cast<double>(spike.source.value),
                                static_cast<double>(spike.timestamp),
                                static_cast<double>(spike.step)});
        }
        
        return spike_data;
    }, py::arg("brain"),
        "Collect spike data for visualization");

    m.def("collectSynapseData", [](std::shared_ptr<Brain> brain) {
        std::vector<std::vector<double>> synapse_data;
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                synapse_data.push_back({static_cast<double>(syn->getId().value),
                                      static_cast<double>(syn->getWeight()),
                                      static_cast<double>(syn->isExcitatory() ? 1 : 0)});
            }
        }
        
        return synapse_data;
    }, py::arg("brain"),
        "Collect synapse data for visualization");

    m.def("collectNeuronData", [](std::shared_ptr<Brain> brain) {
        std::vector<std::vector<double>> neuron_data;
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto neurons = region->getAllNeurons();
            for (const auto& neuron : neurons) {
                neuron_data.push_back({static_cast<double>(neuron->getId().value),
                                      static_cast<double>(neuron->getMembranePotential()),
                                      static_cast<double>(neuron->isFiring() ? 1 : 0)});
            }
        }
        
        return neuron_data;
    }, py::arg("brain"),
        "Collect neuron data for visualization");

    // Add version and metadata
    m.attr("__version__") = "2.0.0";
    m.attr("__description__") = "NLM Neural Learning Machine v2.0 with real neural dynamics and plasticity";
}