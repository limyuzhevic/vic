// Neuromodulation classes

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"), "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection for exploration)pbdoc")
        .def(py::init<>());

    py::class_<PredictionErrorSignal>(m, "PredictionErrorSignal", R"pbdoc(Computes and broadcasts prediction error)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionErrorSignal::computeError, py::arg("predicted"), py::arg("actual"), "Compute error between predicted and actual")
        .def("getModulationSignal", &PredictionErrorSignal::getModulationSignal, "Get neuromodulation signal strength")
        .def("recordError", &PredictionErrorSignal::recordError, py::arg("error"), py::arg("step"), "Record error for analysis")
        .def("isSurprising", &PredictionErrorSignal::isSurprising, "Is this a surprising event");

    // ========== FACTORY FUNCTIONS FOR NEUROMODULATION ==========
    m.def("createDopamine", []() -> std::shared_ptr<Dopamine> {
        return std::make_shared<Dopamine>();
    }, "Create dopamine neuromodulator");

    m.def("createCuriosity", []() -> std::shared_ptr<Curiosity> {
        return std::make_shared<Curiosity>();
    }, "Create curiosity neuromodulator");

    m.def("createNovelty", []() -> std::shared_ptr<Novelty> {
        return std::make_shared<Novelty>();
    }, "Create novelty neuromodulator");

    m.def("createPredictionError", []() -> std::shared_ptr<PredictionErrorSignal> {
        return std::make_shared<PredictionErrorSignal>();
    }, "Create prediction error neuromodulator");

    // ========== SENSORY INPUTS ==========
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory data packet - what the agent perceives)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // ========== MEMORY SYSTEMS ==========
    // Note: Memory system bindings would go here
    // This includes NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory

    // ========== COGNITIVE SYSTEMS ==========
    // Note: Cognitive system bindings would go here
    // This includes NeuralPlanner, ConceptFormation, AttentionalSelection

    // ========== PREDICTION SYSTEM ==========
    // Note: Prediction system bindings would go here
    // This includes PredictionSystem, NeuralPrediction

    // ========== DEVELOPMENT SYSTEM ==========
    // Note: Development system bindings would go here
    // This includes DevelopmentSystem, Synaptogenesis, Pruning, Maturation

    // ========== PERFORMANCE OPTIMIZATIONS ==========
    // Note: Performance optimization bindings would go here
    // This includes OptimizedBrain, MemoryPool, EventQueue

    // ========== EXPERIMENTS ==========
    // Note: Experiment bindings would go here
    // This includes Experiment, ExperimentRunner, Metrics

    // ========== PHASE-SPECIFIC EXPERIMENTS ==========
    // Note: Phase experiment bindings would go here
    // This includes Phase3Experiment, Phase4Experiment, Phase5Experiment, Phase6IntegratedExperiment

    // ========== VISUALIZATION ==========
    // Note: Visualization bindings would go here
    // This includes VisualizationInterface
}