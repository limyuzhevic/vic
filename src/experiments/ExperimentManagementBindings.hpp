// Add Python bindings for the new experiment management framework classes

    // ============= EXPERIMENTAL CONFIGURATION =============
    
    py::class_<ExperimentConfig>(m, "ExperimentConfig", R"pbdoc(Experiment configuration for neural learning machine experiments)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def("getName", &ExperimentConfig::getName)
        .def("setName", &ExperimentConfig::setName, py::arg("name"))
        .def("getMaxSteps", &ExperimentConfig::getMaxSteps)
        .def("setMaxSteps", &ExperimentConfig::setMaxSteps, py::arg("steps"))
        .def("getNeuronCount", &ExperimentConfig::getNeuronCount)
        .def("setNeuronCount", &ExperimentConfig::setNeuronCount, py::arg("count"))
        .def("getRegionCount", &ExperimentConfig::getRegionCount)
        .def("setRegionCount", &ExperimentConfig::setRegionCount, py::arg("count"))
        .def("getConnectionProbability", &ExperimentConfig::getConnectionProbability)
        .def("setConnectionProbability", &ExperimentConfig::setConnectionProbability, py::arg("probability"))
        .def("getEnableCheckpointing", &ExperimentConfig::getEnableCheckpointing)
        .def("setEnableCheckpointing", &ExperimentConfig::setEnableCheckpointing, py::arg("enable"))
        .def("getEnableReplay", &ExperimentConfig::getEnableReplay)
        .def("setEnableReplay", &ExperimentConfig::setEnableReplay, py::arg("enable"))
        .def("getEnableDevelopment", &ExperimentConfig::getEnableDevelopment)
        .def("setEnableDevelopment", &ExperimentConfig::setEnableDevelopment, py::arg("enable"))
        .def("getCheckpointPath", &ExperimentConfig::getCheckpointPath)
        .def("setCheckpointPath", &ExperimentConfig::setCheckpointPath, py::arg("path"))
        .def("getSeed", &ExperimentConfig::getSeed)
        .def("setSeed", &ExperimentConfig::setSeed, py::arg("seed"))
        .def("validate", &ExperimentConfig::validate)
        .def("validateAndGetError", &ExperimentConfig::validateAndGetError)
        .def("saveToFile", &ExperimentConfig::saveToFile, py::arg("filepath"))
        .def("loadFromFile", &ExperimentConfig::loadFromFile, py::arg("filepath"))
        .def("mergeWith", &ExperimentConfig::mergeWith, py::arg("other"))
        .def("equals", &ExperimentConfig::equals, py::arg("other"))
        // Factory methods
        .def_static("createQuickTestConfig", &ExperimentConfig::createQuickTestConfig)
        .def_static("createTrainingConfig", &ExperimentConfig::createTrainingConfig)
        .def_static("createBenchmarkConfig", &ExperimentConfig::createBenchmarkConfig)
        .def_static("createAblationConfig", &ExperimentConfig::createAblationConfig)
        .def("__repr__", [](const ExperimentConfig& cfg) {
            return "<ExperimentConfig: " + cfg.getName() + ", " + std::to_string(cfg.getMaxSteps()) + " steps>";
        });

    // ============= EXPERIMENTAL METRICS =============
    
    py::class_<ExperimentMetrics>(m, "ExperimentMetrics", R"pbdoc(Comprehensive metrics tracking for neural learning machine experiments)pbdoc")
        .def(py::init<>())
        .def("recordStep", &ExperimentMetrics::recordStep,
             py::arg("step"), py::arg("reward"), py::arg("firingRate"), py::arg("activeNeurons"),
             py::arg("firingNeurons"), py::arg("totalSpikes"), py::arg("curiosityLevel") = 0.0f,
             py::arg("noveltyLevel") = 0.0f, py::arg("dopamineLevel") = 0.0f)
        .def("recordReward", &ExperimentMetrics::recordReward, py::arg("reward"))
        .def("recordFiringRate", &ExperimentMetrics::recordFiringRate, py::arg("firingRate"))
        .def("recordActiveNeurons", &ExperimentMetrics::recordActiveNeurons, py::arg("count"))
        .def("recordFiringNeurons", &ExperimentMetrics::recordFiringNeurons, py::arg("count"))
        .def("recordTotalSpikes", &ExperimentMetrics::recordTotalSpikes, py::arg("spikes"))
        .def("recordNeuromodulation", &ExperimentMetrics::recordNeuromodulation,
             py::arg("curiosity"), py::arg("novelty"), py::arg("dopamine"))
        .def("recordSteps", &ExperimentMetrics::recordSteps, py::arg("steps"))
        .def("recordEpisode", &ExperimentMetrics::recordEpisode,
             py::arg("totalReward"), py::arg("avgFiringRate"), py::arg("totalActiveNeurons"),
             py::arg("totalFiringNeurons"), py::arg("totalSpikes"))
        .def("getTotalReward", &ExperimentMetrics::getTotalReward)
        .def("getAverageFiringRate", &ExperimentMetrics::getAverageFiringRate)
        .def("getAverageRewardPerStep", &ExperimentMetrics::getAverageRewardPerStep)
        .def("getTotalSteps", &ExperimentMetrics::getTotalSteps)
        .def("getTotalActiveNeurons", &ExperimentMetrics::getTotalActiveNeurons)
        .def("getTotalFiringNeurons", &ExperimentMetrics::getTotalFiringNeurons)
        .def("getTotalSpikes", &ExperimentMetrics::getTotalSpikes)
        .def("getCuriosityLevel", &ExperimentMetrics::getCuriosityLevel)
        .def("getNoveltyLevel", &ExperimentMetrics::getNoveltyLevel)
        .def("getDopamineLevel", &ExperimentMetrics::getDopamineLevel)
        .def("getStepMetrics", &ExperimentMetrics::getStepMetrics, py::arg("step"))
        .def("getAllStepMetrics", &ExperimentMetrics::getAllStepMetrics)
        .def("getRewardMean", &ExperimentMetrics::getRewardMean)
        .def("getRewardStdDev", &ExperimentMetrics::getRewardStdDev)
        .def("getFiringRateMean", &ExperimentMetrics::getFiringRateMean)
        .def("getFiringRateStdDev", &ExperimentMetrics::getFiringRateStdDev)
        .def("getRewardPerStepMean", &ExperimentMetrics::getRewardPerStepMean)
        .def("getAverageStepDuration", &ExperimentMetrics::getAverageStepDuration)
        .def("getTotalSimulationTime", &ExperimentMetrics::getTotalSimulationTime)
        .def("getStepsPerSecond", &ExperimentMetrics::getStepsPerSecond)
        .def("getMemoryUsage", &ExperimentMetrics::getMemoryUsage)
        .def("getMetricCount", &ExperimentMetrics::getMetricCount)
        .def("createSnapshot", &ExperimentMetrics::createSnapshot)
        .def("getSummaryReport", &ExperimentMetrics::getSummaryReport)
        .def("compareWith", &ExperimentMetrics::compareWith, py::arg("other"), py::arg("metricName"))
        .def("reset", &ExperimentMetrics::reset)
        .def("clear", &ExperimentMetrics::clear)
        .def("exportToFile", &ExperimentMetrics::exportToFile, py::arg("filepath"))
        .def("importFromFile", &ExperimentMetrics::importFromFile, py::arg("filepath"))
        .def("isValid", &ExperimentMetrics::isValid)
        .def("validateAndGetError", &ExperimentMetrics::validateAndGetError)
        // Iterator support
        .def("__iter__", [](const ExperimentMetrics& metrics) {
            return py::make_iterator(metrics.begin(), metrics.end());
        }, py::keep_alive<0, 1>())
        .def("__len__", &ExperimentMetrics::getMetricCount);

    // ============= ENHANCED EXPERIMENT RUNNER =============
    
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Core experiment running framework for neural learning machine experiments)pbdoc")
        .def(py::init<>())
        .def("setup", &ExperimentRunner::setup,
             py::arg("config"), py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("environment"))
        .def("runEpisode", &ExperimentRunner::runEpisode)
        .def("runExperiment", &ExperimentRunner::runExperiment, py::arg("maxEpisodes") = -1)
        .def("initializeComponents", &ExperimentRunner::initializeComponents)
        .def("resetComponents", &ExperimentRunner::resetComponents)
        .def("cleanupComponents", &ExperimentRunner::cleanupComponents)
        .def("saveCheckpoint", &ExperimentRunner::saveCheckpoint, py::arg("filepath"))
        .def("loadCheckpoint", &ExperimentRunner::loadCheckpoint, py::arg("filepath"))
        .def("pause", &ExperimentRunner::pause)
        .def("resume", &ExperimentRunner::resume)
        .def("isRunning", &ExperimentRunner::isRunning)
        .def("getExperimentMetrics", &ExperimentRunner::getExperimentMetrics)
        .def("getCurrentMetrics", &ExperimentRunner::getCurrentMetrics)
        .def("getResultsSummary", &ExperimentRunner::getResultsSummary)
        .def("getConfig", &ExperimentRunner::getConfig)
        .def("setConfig", &ExperimentRunner::setConfig, py::arg("config"))
        .def("getBrain", &ExperimentRunner::getBrain)
        .def("getAgent", &ExperimentRunner::getAgent)
        .def("getWorld", &ExperimentRunner::getWorld)
        .def("getEnvironment", &ExperimentRunner::getEnvironment)
        .def("getProgress", &ExperimentRunner::getProgress)
        .def("getCurrentEpisode", &ExperimentRunner::getCurrentEpisode)
        .def("getTotalEpisodes", &ExperimentRunner::getTotalEpisodes)
        .def("validateSetup", &ExperimentRunner::validateSetup)
        .def("validateAndGetError", &ExperimentRunner::validateAndGetError)
        .def("lock", &ExperimentRunner::lock)
        .def("unlock", &ExperimentRunner::unlock)
        .def("isThreadSafe", &ExperimentRunner::isThreadSafe)
        .def("runQuickTest", &ExperimentRunner::runQuickTest)
        .def("runTrainingSession", &ExperimentRunner::runTrainingSession)
        .def("runBenchmark", &ExperimentRunner::runBenchmark)
        .def("getPerformanceStats", &ExperimentRunner::getPerformanceStats)
        .def("runMultipleExperiments", &ExperimentRunner::runMultipleExperiments,
             py::arg("configs"), py::arg("episodesPerConfig") = 1)
        .def("compareWith", &ExperimentRunner::compareWith, py::arg("other"), py::arg("metricName"))
        .def("exportResults", &ExperimentRunner::exportResults, py::arg("filepath"))
        .def("importResults", &ExperimentRunner::importResults, py::arg("filepath"))
        .def("getDebugInfo", &ExperimentRunner::getDebugInfo);

    // ============= PHASE 6 EXPERIMENT =============
    
    py::class_<Phase6Experiment>(m, "Phase6Experiment", R"pbdoc(Phase 6 integration test experiment inheriting from ExperimentRunner)pbdoc")
        .def(py::init<>())
        .def("verifyIntegration", &Phase6Experiment::verifyIntegration)
        .def("testMemoryIntegration", &Phase6Experiment::testMemoryIntegration)
        .def("testNeuromodulationIntegration", &Phase6Experiment::testNeuromodulationIntegration)
        .def("testCheckpointing", &Phase6Experiment::testCheckpointing)
        .def("testReplay", &Phase6Experiment::testReplay)
        .def("runPhase6IntegrationTest", &Phase6Experiment::runPhase6IntegrationTest, py::arg("config"))
        .def("runComprehensiveTestSuite", &Phase6Experiment::runComprehensiveTestSuite)
        .def("getIntegrationResult", &Phase6Experiment::getIntegrationResult)
        .def("runMemoryIntegrationExperiment", &Phase6Experiment::runMemoryIntegrationExperiment)
        .def("runNeuromodulationIntegrationExperiment", &Phase6Experiment::runNeuromodulationIntegrationExperiment)
        .def("runDevelopmentIntegrationExperiment", &Phase6Experiment::runDevelopmentIntegrationExperiment)
        .def("runPredictionIntegrationExperiment", &Phase6Experiment::runPredictionIntegrationExperiment)
        .def("runCheckpointingIntegrationExperiment", &Phase6Experiment::runCheckpointingIntegrationExperiment)
        .def("runReplayIntegrationExperiment", &Phase6Experiment::runReplayIntegrationExperiment)
        .def("runAblationStudy", &Phase6Experiment::runAblationStudy, py::arg("ablationType"))
        .def("runHyperparameterSweep", &Phase6Experiment::runHyperparameterSweep, py::arg("parameterName"))
        .def("runBenchmarkSuite", &Phase6Experiment::runBenchmarkSuite)
        .def("validatePhase6Setup", &Phase6Experiment::validatePhase6Setup)
        .def("validateAndGetPhase6Error", &Phase6Experiment::validateAndGetPhase6Error)
        .def("exportPhase6Results", &Phase6Experiment::exportPhase6Results, py::arg("filepath"))
        .def("importPhase6Results", &Phase6Experiment::importPhase6Results, py::arg("filepath"))
        .def("getPhase6Progress", &Phase6Experiment::getPhase6Progress)
        .def("getCurrentPhase6Test", &Phase6Experiment::getCurrentPhase6Test);

    // ============= MODULE-LEVEL CONVENIENCE FUNCTIONS =============
    
    // Factory function to create ExperimentRunner
    m.def("createExperimentRunner", [](std::shared_ptr<ExperimentConfig> config) {
        return std::make_shared<ExperimentRunner>();
    }, py::arg("config"), "Factory function to create ExperimentRunner with configuration");
    
    // Run Phase 6 experiment and return results
    m.def("runPhase6Experiment", [](std::shared_ptr<ExperimentConfig> config) {
        Phase6Experiment experiment;
        bool success = experiment.runPhase6IntegrationTest(config);
        return std::make_tuple(success, experiment.getIntegrationResult());
    }, py::arg("config"), "Run Phase 6 experiment and return success status and results");
    
    // Compare multiple experiment results
    m.def("compareExperiments", [](const std::vector<std::shared_ptr<ExperimentRunner>>& experiments, const std::string& metricName) {
        std::vector<double> comparisons;
        if (!experiments.empty()) {
            auto& first = *experiments[0];
            for (size_t i = 0; i < experiments.size(); ++i) {
                comparisons.push_back(first.compareWith(*experiments[i], metricName));
            }
        }
        return comparisons;
    }, py::arg("experiments"), py::arg("metricName"), "Compare multiple experiment results");

    // ============= DOCUMENTATION =============
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Experiment Management Framework
        ---------------------------------------------------------------------------------
        
        This enhanced module provides comprehensive Python bindings for the NLM neural simulation
        framework with a complete experiment management framework including:
        
        1. ExperimentConfig - Configuration for experiments with validation and serialization
        2. ExperimentMetrics - Performance tracking and analysis
        3. ExperimentRunner - Core experiment running framework
        4. Phase6Experiment - Phase 6 integration test class
        
        The framework is designed for:
        - Simulation research - Easy setup of different experimental conditions
        - Hyperparameter tuning - Support for parameter sweeps
        - Benchmarking - Performance measurement
        - Reproducible research - Checkpoint saving/loading
        - Ablation studies - System component testing
        
        Usage Examples:
        >>> import pynlm
        >>> 
        >>> # Create experiment configuration
        >>> config = pynlm.ExperimentConfig.createTrainingConfig()
        >>> config.setNeuronCount(500)
        >>> config.setMaxSteps(10000)
        >>> 
        >>> # Create experiment runner
        >>> runner = pynlm.createExperimentRunner(config)
        >>> 
        >>> # Setup and run experiment
        >>> # runner.setup(config, brain, agent, world, environment)  # Would need actual components
        >>> # results = runner.runExperiment(5)
        >>> 
        >>> # Create Phase 6 experiment
        >>> phase6 = pynlm.Phase6Experiment()
        >>> success, result = pynlm.runPhase6Experiment(config)
        >>> print(result.getIntegrationSummary())
        >>> 
        >>> # Compare experiments
        >>> experiments = [runner1, runner2, runner3]  # List of ExperimentRunner objects
        >>> comparisons = pynlm.compareExperiments(experiments, "total_reward")
    )pbdoc";
}