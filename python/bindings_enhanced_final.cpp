// Performance monitoring and metrics APIs
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics", R"pbdoc(Performance metrics for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("getNeuronsProcessed", &PerformanceMetrics::getNeuronsProcessed)
        .def("getSynapsesProcessed", &PerformanceMetrics::getSynapsesProcessed)
        .def("getTotalSpikes", &PerformanceMetrics::getTotalSpikes)
        .def("getAverageFiringRate", &PerformanceMetrics::getAverageFiringRate)
        .def("getEnergyConsumption", &PerformanceMetrics::getEnergyConsumption)
        .def("getMemoryUsage", &PerformanceMetrics::getMemoryUsage)
        .def("getExecutionTime", &PerformanceMetrics::getExecutionTime)
        .def("reset", &PerformanceMetrics::reset)
        .def("__repr__", [](const PerformanceMetrics& metrics) {
            std::stringstream ss;
            ss << "<PerformanceMetrics: "
               << " neurons=" << metrics.getNeuronsProcessed()
               << " synapses=" << metrics.getSynapsesProcessed()
               << " spikes=" << metrics.getTotalSpikes()
               << " energy=" << metrics.getEnergyConsumption()
               << " time=" << metrics.getExecutionTime() << "s";
            return ss.str();
        });

    // Benchmarking utilities
    m.def("benchmarkSimulation", [](const std::shared_ptr<Brain>& brain, size_t numSteps, 
                                    const std::string& description = "") {
        auto metrics = std::make_shared<PerformanceMetrics>();
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
            metrics->recordNeuron(brain->getTotalNeuronCount());
            metrics->recordSynapse(brain->getTotalSynapseCount());
            metrics->recordSpike(brain->getTotalSpikeCount());
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        metrics->setExecutionTime(duration.count() / 1000.0);
        
        if (!description.empty()) {
            std::cout << "Benchmark: " << description << "\n";
            std::cout << "  Steps: " << numSteps << "\n";
            std::cout << "  Duration: " << metrics->getExecutionTime() << " seconds\n";
            std::cout << "  Neurons processed: " << metrics->getNeuronsProcessed() << " per step\n";
            std::cout << "  Synapses processed: " << metrics->getSynapsesProcessed() << " per step\n";
            std::cout << "  Total spikes: " << metrics->getTotalSpikes() << "\n";
            std::cout << "  Average firing rate: " << metrics->getAverageFiringRate() << " Hz\n";
        }
        
        return metrics;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("description") = "",
         "Benchmark simulation performance and return metrics");

    m.def("profileSimulation", [](const std::shared_ptr<Brain>& brain, size_t numSteps) {
        auto metrics = std::make_shared<PerformanceMetrics>();
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
            metrics->recordNeuron(brain->getTotalNeuronCount());
            metrics->recordSynapse(brain->getTotalSynapseCount());
            metrics->recordSpike(brain->getTotalSpikeCount());
            
            if (step % 100 == 0) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
                std::cout << "Step " << step << ": " << metrics->getAverageFiringRate() 
                         << " Hz average, " << elapsed.count() << " ms elapsed\n";
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        metrics->setExecutionTime(duration.count() / 1000.0);
        
        return metrics;
    }, py::arg("brain"), py::arg("numSteps"),
         "Profile simulation with detailed progress output");

    m.def("createProfiler", []() {
        return std::make_shared<PerformanceProfiler>();
    }, "Create a performance profiler");

    // Simulation utilities
    m.def("runLearningExperiment", [](const std::shared_ptr<Brain>& brain, 
                                      const std::shared_ptr<SimpleWorld>& world,
                                      const std::shared_ptr<AgentBrain>& agent,
                                      size_t numEpisodes, float timestep = 0.1) {
        std::vector<py::dict> episodeResults;
        
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            world->reset();
            agent->getBrain()->reset();
            agent->initialize(world.get());
            
            py::dict episodeStats;
            episodeStats["episode"] = static_cast<int>(episode);
            episodeStats["total_spikes"] = 0;
            episodeStats["total_actions"] = 0;
            
            for (size_t step = 0; step < 100; ++step) {
                world->update(timestep);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                agent->getBrain()->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                episodeStats["total_spikes"] += agent->getBrain()->getTotalSpikeCount();
                episodeStats["total_actions"]++;
            }
            
            episodeResults.push_back(episodeStats);
        }
        
        return episodeResults;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
         py::arg("numEpisodes"), py::arg("timestep") = 0.1f,
         "Run multiple learning episodes and return results");

    m.def("createExperimentSuite", []() {
        return std::make_shared<ExperimentSuite>();
    }, "Create an experiment suite for systematic testing");

    // Utility functions
    m.def("getSystemInfo", []() {
        py::dict info;
        info["version"] = "1.0.0";
        info["cpp_standard"] = "C++20";
        info["python_module"] = "pynlm";
        info["description"] = "Neural Learning Machine - Experimental Artificial Developmental Brain";
        return info;
    }, "Get system information and version details");

    m.def("validateConfiguration", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        if (!config->has("random_seed")) {
            errors.push_back("Missing required parameter: random_seed");
        }
        
        if (!config->has("simulation_timestep")) {
            errors.push_back("Missing required parameter: simulation_timestep");
        }
        
        if (!config->has("neuron_count")) {
            errors.push_back("Missing required parameter: neuron_count");
        }
        
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw ConfigurationError(errorMsg);
        }
        
        return true;
    }, py::arg("config"), "Validate configuration and throw exception if invalid");

    m.def("createRandomConfig", [](uint64_t seed, size_t neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", seed, ConfigSource::Runtime);
        config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config->set("connection_probability", 0.1, ConfigSource::Runtime);
        return config;
    }, py::arg("seed") = 42, py::arg("neuronCount") = 1000,
         "Create a randomized configuration");

    m.def("saveModelCheckpoint", [](const std::shared_ptr<Brain>& brain, 
                                   const std::string& filepath,
                                   const std::string& description = "") {
        if (!brain->save(filepath)) {
            throw SimulationError("Failed to save brain checkpoint to: " + filepath);
        }
        
        if (!description.empty()) {
            std::cout << "Model checkpoint saved to: " << filepath << "\n";
            std::cout << "Description: " << description << "\n";
        }
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), py::arg("description") = "",
         "Save brain model as a checkpoint file");

    m.def("loadModelCheckpoint", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(filepath)) {
            throw SimulationError("Failed to load brain checkpoint from: " + filepath);
        }
        
        return brain;
    }, py::arg("filepath"), "Load brain model from checkpoint file");

    // Neuroscience-specific utilities
    m.def("createSensoryStimulation", [](const std::shared_ptr<Brain>& brain, 
                                         const std::vector<float>& pattern,
                                         float current = 50.0f) {
        if (brain->getTotalNeuronCount() < 10) {
            throw InitializationError("Brain too small for sensory stimulation");
        }
        
        size_t maxSensoryNeurons = brain->getTotalNeuronCount() / 10;
        for (size_t i = 0; i < std::min(pattern.size(), maxSensoryNeurons); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, current * pattern[i]);
        }
        
        return true;
    }, py::arg("brain"), py::arg("pattern"), py::arg("current") = 50.0f,
         "Create sensory stimulation pattern");

    m.def("measureNeuralSynchronization", [](const std::shared_ptr<Brain>& brain, 
                                            size_t durationSteps = 100) {
        if (brain->getTotalNeuronCount() < 2) {
            throw SimulationError("Need at least 2 neurons for synchronization measurement");
        }
        
        std::vector<size_t> spikeCounts;
        size_t totalSpikes = 0;
        
        for (size_t step = 0; step < durationSteps; ++step) {
            brain->step(step);
            size_t spikesThisStep = brain->getTotalSpikeCount() - totalSpikes;
            spikeCounts.push_back(spikesThisStep);
            totalSpikes += spikesThisStep;
        }
        
        // Calculate synchronization index (simplified)
        float meanSpikes = static_cast<float>(totalSpikes) / durationSteps;
        float variance = 0.0f;
        for (size_t count : spikeCounts) {
            float diff = static_cast<float>(count) - meanSpikes;
            variance += diff * diff;
        }
        variance /= durationSteps;
        float stdDev = std::sqrt(variance);
        float syncIndex = (meanSpikes > 0) ? (1.0f - stdDev / meanSpikes) : 0.0f;
        
        py::dict results;
        results["synchronization_index"] = syncIndex;
        results["mean_spikes_per_step"] = meanSpikes;
        results["std_deviation"] = stdDev;
        results["total_spikes"] = totalSpikes;
        
        return results;
    }, py::arg("brain"), py::arg("durationSteps") = 100,
         "Measure neural synchronization and return statistics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm