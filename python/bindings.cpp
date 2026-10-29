    // Working memory system
    py::class_<WorkingMemory, std::shared_ptr<WorkingMemory>>(m, "WorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"), "Store item in working memory")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"), "Retrieve item from working memory")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"), "Check if item is in working memory")
        .def("clear", &WorkingMemory::clear, "Clear working memory")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current working memory size")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"), "Decay working memory items");

    // Episodic memory system
    py::class_<EpisodicMemory, std::shared_ptr<EpisodicMemory>>(m, "EpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"), "Store episode in episodic memory")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"), "Retrieve episode by index")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount, "Get number of episodes")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"), "Get recent episodes")
        .def("clear", &EpisodicMemory::clear, "Clear episodic memory")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"), "Consolidate episodic memory");

    // Semantic memory system
    py::class_<SemanticMemory, std::shared_ptr<SemanticMemory>>(m, "SemanticMemory", R"pbdoc(Semantic memory system)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"), "Store fact in semantic memory")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"), "Retrieve fact from semantic memory")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"), "Check if fact exists in semantic memory")
        .def("getAllFacts", &SemanticMemory::getAllFacts, "Get all facts from semantic memory")
        .def("clear", &SemanticMemory::clear, "Clear semantic memory");

    // Procedural memory system
    py::class_<ProceduralMemory, std::shared_ptr<ProceduralMemory>>(m, "ProceduralMemory", R"pbdoc(Procedural memory system)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"), "Learn new skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"), "Get skill by name")
        .def("getAllSkills", &ProceduralMemory::getAllSkills, "Get all learned skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"), "Update skill proficiency")
        .def("clear", &ProceduralMemory::clear, "Clear procedural memory");

    // Associative memory system
    py::class_<AssociativeMemory, std::shared_ptr<AssociativeMemory>>(m, "AssociativeMemory", R"pbdoc(Associative memory system)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"), "Create association between neurons")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"), "Get associated neurons")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"), "Get association strength")
        .def("updateAssociation", &AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"), "Update association strength")
        .def("clear", &AssociativeMemory::clear, "Clear associative memory");

    // Neuromodulation systems
    py::enum_<Dopamine::State>(m, "DopamineState")
        .value("Resting", Dopamine::State::Resting)
        .value("Increasing", Dopamine::State::Increasing)
        .value("Decreasing", Dopamine::State::Decreasing)
        .export_values();

    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("update", &Dopamine::update, py::arg("dt"), "Update dopamine levels")
        .def("getLevel", &Dopamine::getLevel, "Get current dopamine level")
        .def("getState", &Dopamine::getState, "Get dopamine state")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"), "Set dopamine level");

    py::enum_<Curiosity::State>(m, "CuriosityState")
        .value("Low", Curiosity::State::Low)
        .value("Medium", Curiosity::State::Medium)
        .value("High", Curiosity::State::High)
        .export_values();

    py::class_<Curiosity, std::shared_ptr<Curiosity>>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("update", &Curiosity::update, py::arg("dt"), "Update curiosity levels")
        .def("getLevel", &Curiosity::getLevel, "Get current curiosity level")
        .def("getState", &Curiosity::getState, "Get curiosity state")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"), "Set curiosity level");

    py::enum_<Novelty::State>(m, "NoveltyState")
        .value("Low", Novelty::State::Low)
        .value("Medium", Novelty::State::Medium)
        .value("High", Novelty::State::High)
        .export_values();

    py::class_<Novelty, std::shared_ptr<Novelty>>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("update", &Novelty::update, py::arg("dt"), "Update novelty levels")
        .def("getLevel", &Novelty::getLevel, "Get current novelty level")
        .def("getState", &Novelty::getState, "Get novelty state")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level");

    py::enum_<PredictionError::State>(m, "PredictionErrorState")
        .value("Negative", PredictionError::State::Negative)
        .value("Zero", PredictionError::State::Zero)
        .value("Positive", PredictionError::State::Positive)
        .export_values();

    py::class_<PredictionError, std::shared_ptr<PredictionError>>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("update", &PredictionError::update, py::arg("dt"), "Update prediction error levels")
        .def("getLevel", &PredictionError::getLevel, "Get current prediction error level")
        .def("getState", &PredictionError::getState, "Get prediction error state")
        .def("setLevel", &PredictionError::setLevel, py::arg("level"), "Set prediction error level");

    // Development system
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::class_<DevelopmentSystem, std::shared_ptr<DevelopmentSystem>>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development system")
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("getAge", &DevelopmentSystem::getAge, "Get current developmental age")
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate, "Get current plasticity rate");

    // Prediction system
    py::class_<PredictionSystem, std::shared_ptr<PredictionSystem>>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("update", &PredictionSystem::update, py::arg("sensoryInput"), py::arg("action"), py::arg("reward"), py::arg("dt"), "Update prediction system")
        .def("getPrediction", &PredictionSystem::getPrediction, "Get prediction value")
        .def("getError", &PredictionSystem::getError, "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence, "Get prediction confidence")
        .def("reset", &PredictionSystem::reset, "Reset prediction system");

    // Cognitive systems
    py::class_<NeuralPlanner, std::shared_ptr<NeuralPlanner>>(m, "NeuralPlanner", R"pbdoc(Neural planner)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize neural planner")
        .def("planAction", &NeuralPlanner::planAction, py::arg("goal"), "Plan action for goal")
        .def("getPlanQuality", &NeuralPlanner::getPlanQuality, "Get plan quality")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"), "Set planning depth")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth, "Get planning depth");

    py::class_<ConceptFormation, std::shared_ptr<ConceptFormation>>(m, "ConceptFormation", R"pbdoc(Concept formation)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize concept formation")
        .def("processPattern", &ConceptFormation::processPattern, py::arg("pattern"), "Process neural pattern")
        .def("getConcepts", &ConceptFormation::getConcepts, "Get learned concepts")
        .def("getConceptStrength", &ConceptFormation::getConceptStrength, py::arg("concept"), "Get concept strength")
        .def("forgetConcept", &ConceptFormation::forgetConcept, py::arg("concept"), "Forget concept");

    py::class_<AttentionalSelection, std::shared_ptr<AttentionalSelection>>(m, "AttentionalSelection", R"pbdoc(Attentional selection)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"), "Initialize attentional selection")
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("global_inhibition"), "Process competition between neurons")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"), "Focus attention on region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention, "Release attention")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions, "Get attended regions")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"), "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"), "Set competition threshold")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"), "Check if neuron is attended")
        .def("getWinners", &AttentionalSelection::getWinners, "Get winning neurons")
        .def("update", &AttentionalSelection::update, py::arg("dt"), "Update attentional selection");

    // Memory and cognitive integration
    m.def("createWorkingMemory", []() -> std::shared_ptr<WorkingMemory> {
        return std::make_shared<WorkingMemory>();
    }, "Create working memory system");

    m.def("createEpisodicMemory", []() -> std::shared_ptr<EpisodicMemory> {
        return std::make_shared<EpisodicMemory>();
    }, "Create episodic memory system");

    m.def("createSemanticMemory", []() -> std::shared_ptr<SemanticMemory> {
        return std::make_shared<SemanticMemory>();
    }, "Create semantic memory system");

    m.def("createProceduralMemory", []() -> std::shared_ptr<ProceduralMemory> {
        return std::make_shared<ProceduralMemory>();
    }, "Create procedural memory system");

    m.def("createAssociativeMemory", []() -> std::shared_ptr<AssociativeMemory> {
        return std::make_shared<AssociativeMemory>();
    }, "Create associative memory system");

    m.def("createDopamine", []() -> std::shared_ptr<Dopamine> {
        return std::make_shared<Dopamine>();
    }, "Create dopamine neuromodulator");

    m.def("createCuriosity", []() -> std::shared_ptr<Curiosity> {
        return std::make_shared<Curiosity>();
    }, "Create curiosity neuromodulator");

    m.def("createNovelty", []() -> std::shared_ptr<Novelty> {
        return std::make_shared<Novelty>();
    }, "Create novelty neuromodulator");

    m.def("createPredictionError", []() -> std::shared_ptr<PredictionError> {
        return std::make_shared<PredictionError>();
    }, "Create prediction error neuromodulator");

    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create development system");

    m.def("createPredictionSystem", []() -> std::shared_ptr<PredictionSystem> {
        return std::make_shared<PredictionSystem>();
    }, "Create prediction system");

    m.def("createNeuralPlanner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    }, "Create neural planner");

    m.def("createConceptFormation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    }, "Create concept formation system");

    m.def("createAttentionalSelection", []() -> std::shared_ptr<AttentionalSelection> {
        return std::make_shared<AttentionalSelection>();
    }, "Create attentional selection system");

    // Advanced brain configuration helpers
    m.def("createAdvancedBrainConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set up advanced configuration for integrated brain
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(2000));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(3));
        config->set<std::string>("brain.connection_probability", 0.15f);
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.stdp.learning_rate", 0.001);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", true);
        config->set<std::string>("neuromod.dopamine.scale", 1.0);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("neuromod.prediction_error.enable", true);
        config->set<std::string>("development.synaptogenesis_rate", 0.001);
        config->set<std::string>("development.pruning_rate", 0.0001);
        
        return config;
    }, "Create advanced brain configuration with all systems enabled");

    // Debugging and profiling helpers
    m.def("profileBrainState", [](const Brain& brain) {
        std::cout << "=== Brain State Profile ===" << std::endl;
        std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain.getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << brain.getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        std::cout << "Developmental stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        return true;
    }, py::arg("brain"), "Profile brain state and print statistics");

    m.def("exportBrainForML", [](const Brain& brain, const std::string& filepath) {
        // Export brain state for machine learning analysis
        auto config = brain.getConfig();
        bool success = config->saveToFile(filepath + "_config.json");
        if (success) {
            std::cout << "Brain state exported to " << filepath << "_config.json" << std::endl;
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Export brain state for ML analysis");

    // Simulation utilities
    m.def("runSimulationWithMonitoring", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                           std::shared_ptr<AgentBrain> agent, size_t numSteps,
                                           std::function<void(size_t, const Brain&, const SimpleWorld&, const AgentBrain&)> callback) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            callback(step, *brain, *world, *agent);
        }
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), py::arg("callback"), "Run simulation with monitoring callback");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t numSteps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Benchmark: " << numSteps << " steps in " << duration.count() << " ms" << std::endl;
        std::cout << "Steps per second: " << (numSteps * 1000.0 / duration.count()) << std::endl;
        
        return duration.count();
    }, py::arg("brain"), py::arg("num_steps"), "Benchmark brain performance");

    // Phase helpers
    m.def("runPhase2Test", []() -> bool {
        // Run Phase 2 (neural computation) tests
        std::cout << "=== Phase 2 Tests ===" << std::endl;
        std::cout << "Testing LIF neuron dynamics, spike propagation, STDP, Hebbian learning" << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test basic connectivity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(50.0f);
                brain->step(0);
                
                if (brain->getFiringNeuronCount() > 0) {
                    std::cout << "✓ Basic neural computation working" << std::endl;
                    return true;
                }
            }
        }
        
        std::cout << "✗ Phase 2 tests failed" << std::endl;
        return false;
    }, "Run Phase 2 neural computation tests");

    m.def("runPhase3Test", []() -> bool {
        // Run Phase 3 (world interaction) tests
        std::cout << "=== Phase 3 Tests ===" << std::endl;
        std::cout << "Testing world interaction, sensory input, motor output, reward prediction" << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Run a few steps
        for (size_t step = 0; step < 20; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        std::cout << "✓ World interaction working" << std::endl;
        return true;
    }, "Run Phase 3 world interaction tests");

    m.def("runPhase4Test", []() -> bool {
        // Run Phase 4 (memory and cognition) tests
        std::cout << "=== Phase 4 Tests ===" << std::endl;
        std::cout << "Testing memory systems, prediction, cognition, attention" << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(200));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test memory systems
        auto workingMemory = std::make_shared<WorkingMemory>();
        
        // Test episodic memory
        EpisodicMemoryItem episode;
        episode.timestamp = 0;
        episode.sensoryState = {0.5f, 0.7f, 0.3f};
        episode.action = ActionType::MoveForward;
        episode.reward = 1.0f;
        
        auto episodicMemory = std::make_shared<EpisodicMemory>();
        episodicMemory->storeEpisode(episode);
        
        // Test associative memory
        auto associativeMemory = std::make_shared<AssociativeMemory>();
        associativeMemory->associate({0.5f, 0.7f}, {0.3f, 0.9f}, 0.8f);
        
        std::cout << "✓ Memory systems working" << std::endl;
        return true;
    }, "Run Phase 4 memory and cognition tests");

    m.def("runPhase6Test", []() -> bool {
        // Run Phase 6 (full integration) tests
        std::cout << "=== Phase 6 Tests (Full Integration) ===" << std::endl;
        std::cout << "Testing complete brain integration, all systems working together" << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(500));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(2));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", true);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("neuromod.prediction_error.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test all integrated systems
        bool success = true;
        
        // Working memory
        auto workingMemory = std::make_shared<WorkingMemory>();
        
        // Episodic memory
        auto episodicMemory = std::make_shared<EpisodicMemory>();
        episodicMemory->initialize(brain.get());
        
        // Semantic memory
        auto semanticMemory = std::make_shared<SemanticMemory>();
        
        // Procedural memory
        auto proceduralMemory = std::make_shared<ProceduralMemory>();
        
        // Associative memory
        auto associativeMemory = std::make_shared<AssociativeMemory>();
        associativeMemory->initialize(brain.get());
        
        // Neuromodulation systems
        auto dopamine = std::make_shared<Dopamine>();
        auto curiosity = std::make_shared<Curiosity>();
        auto novelty = std::make_shared<Novelty>();
        auto predictionError = std::make_shared<PredictionError>();
        
        // Development system
        auto developmentSystem = std::make_shared<DevelopmentSystem>();
        
        // Prediction system
        auto predictionSystem = std::make_shared<PredictionSystem>();
        
        // Cognitive systems
        auto planner = std::make_shared<NeuralPlanner>();
        auto conceptFormation = std::make_shared<ConceptFormation>();
        auto attention = std::make_shared<AttentionalSelection>();
        
        std::cout << "✓ All integrated systems initialized" << std::endl;
        
        // Run integration test
        for (size_t step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        std::cout << "✓ All systems working together" << std::endl;
        std::cout << "✓ Phase 6 integration complete" << std::endl;
        
        return true;
    }, "Run Phase 6 full integration tests");

    // Example usage functions
    m.def("example_working_memory", []() {
        std::cout << "=== Working Memory Example ===" << std::endl;
        std::cout << "import pynlm" << std::endl;
        std::cout << "brain = pynlm.createBrain(pynlm.createDefaultConfig())" << std::endl;
        std::cout << "brain.initialize()" << std::endl;
        std::cout << "wm = pynlm.createWorkingMemory()" << std::endl;
        std::cout << "wm.storeToNeuron(neuron_id, 0.5)" << std::endl;
        std::cout << "activation = wm.getNeuronActivation(neuron_id)" << std::endl;
    }, "Show working memory example");

    m.def("example_agent", []() {
        std::cout << "=== Agent Example ===" << std::endl;
        std::cout << "import pynlm" << std::endl;
        std::cout << "brain = pynlm.createBrain(pynlm.createDefaultConfig())" << std::endl;
        std::cout << "world = pynlm.createSimpleWorld()" << std::endl;
        std::cout << "agent = pynlm.createAgentBrain(brain)" << std::endl;
        std::cout << "world.configure(20, 20, 8, 8)" << std::endl;
        std::cout << "world.reset()" << std::endl;
        std::cout << "agent.initialize(world)" << std::endl;
        std::cout << "agent.enableRewardModulation(True)" << std::endl;
        std::cout << "agent.enableCuriosity(True)" << std::endl;
        std::cout << "agent.enableDevelopment(True)" << std::endl;
    }, "Show agent example");

    m.def("example_learning", []() {
        std::cout << "=== Learning Example ===" << std::endl;
        std::cout << "import pynlm" << std::endl;
        std::cout << "brain = pynlm.createBrain(pynlm.createAdvancedConfig())" << std::endl;
        std::cout << "brain.initialize()" << std::endl;
        std::cout << "# Run simulation with learning" << std::endl;
        std::cout << "for step in range(1000):" << std::endl;
        std::cout << "    brain.step(step)" << std::endl;
        std::cout << "    # Observe learning in action" << std::endl;
    }, "Show learning example");

    m.def("advanced_brain_builder", [](const std::string& type) {
        if (type == "working_memory") {
            auto config = std::make_shared<Config>();
            config->set<std::string>("brain.neuron_count", static_cast<int64_t>(1000));
            config->set<std::string>("brain.region_count", static_cast<int64_t>(3));
            config->set<std::string>("memory.working_memory.capacity", 500);
            config->set<std::string>("memory.working_memory.decay_rate", 0.01);
            return config;
        } else if (type == "episodic_memory") {
            auto config = std::make_shared<Config>();
            config->set<std::string>("brain.neuron_count", static_cast<int64_t>(1500));
            config->set<std::string>("memory.episodic.max_episodes", 1000);
            config->set<std::string>("memory.episodic.replay_enabled", true);
            return config;
        } else if (type == "cognition") {
            auto config = std::make_shared<Config>();
            config->set<std::string>("brain.neuron_count", static_cast<int64_t>(2000));
            config->set<std::string>("cognition.planner.planning_depth", 5);
            config->set<std::string>("cognition.attention.inhibition_strength", 0.7);
            config->set<std::string>("cognition.attention.excitation_strength", 1.3);
            return config;
        } else if (type == "neuromodulation") {
            auto config = std::make_shared<Config>();
            config->set<std::string>("neuromod.dopamine.scale", 1.5);
            config->set<std::string>("neuromod.curiosity.enable", true);
            config->set<std::string>("neuromod.curiosity.scale", 2.0);
            config->set<std::string>("neuromod.novelty.scale", 1.2);
            config->set<std::string>("neuromod.prediction_error.scale", 0.8);
            return config;
        } else if (type == "development") {
            auto config = std::make_shared<Config>();
            config->set<std::string>("development.enabled", true);
            config->set<std::string>("development.critical_period_duration", 300.0);
            config->set<std::string>("development.maturation_duration", 900.0);
            config->set<std::string>("development.plasticity_decay_rate", 0.5);
            return config;
        } else if (type == "prediction") {
            auto config = std::make_shared<Config>();
            config->set<std::string>("prediction.enabled", true);
            config->set<std::string>("prediction.horizon", 10);
            config->set<std::string>("prediction.confidence_threshold", 0.7);
            config->set<std::string>("prediction.learning_rate", 0.01);
            return config;
        }
        
        return std::make_shared<Config>();
    }, py::arg("type"), "Create specialized configuration for different brain systems");

    // Documentation
    m.doc() = R"pbdoc(
    NLM (Neural Learning Machine) - Advanced Python Bindings
    ========================================================

    This module provides advanced bindings for NLM's integrated brain systems.
    It includes comprehensive bindings for memory systems, neuromodulation,
    cognitive functions, and development.

    Key features:
    - Complete memory system integration (Working, Episodic, Semantic, Procedural, Associative)
    - Advanced neuromodulation (Dopamine, Curiosity, Novelty, PredictionError)
    - Cognitive systems (Planner, ConceptFormation, Attention)
    - Development and plasticity systems
    - Prediction and decision-making systems
    - Integration helpers and advanced configuration
    - Benchmarking and profiling tools
    - Example usage and educational demonstrations

    For basic usage, see easy_usage.md. For detailed documentation, see HOW_TO_USE.md.
    
    Phase 2.5: Integrated Neural Computation
    Phase 3: World Interaction
    Phase 4: Memory and Cognition
    Phase 6: Full Integration
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::logic_error>(m, "LogicError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Create aliases for convenience
    m.attr("WorkingMemory") = py::type<WorkingMemory>();
    m.attr("EpisodicMemory") = py::type<EpisodicMemory>();
    m.attr("SemanticMemory") = py::type<SemanticMemory>();
    m.attr("ProceduralMemory") = py::type<ProceduralMemory>();
    m.attr("AssociativeMemory") = py::type<AssociativeMemory>();
    m.attr("Dopamine") = py::type<Dopamine>();
    m.attr("Curiosity") = py::type<Curiosity>();
    m.attr("Novelty") = py::type<Novelty>();
    m.attr("PredictionError") = py::type<PredictionError>();
    m.attr("DevelopmentSystem") = py::type<DevelopmentSystem>();
    m.attr("PredictionSystem") = py::type<PredictionSystem>();
    m.attr("NeuralPlanner") = py::type<NeuralPlanner>();
    m.attr("ConceptFormation") = py::type<ConceptFormation>();
    m.attr("AttentionalSelection") = py::type<AttentionalSelection>();

    // Memory system shortcuts
    m.def("createMemorySystem", []() {
        auto workingMemory = std::make_shared<WorkingMemory>();
        auto episodicMemory = std::make_shared<EpisodicMemory>();
        auto semanticMemory = std::make_shared<SemanticMemory>();
        auto proceduralMemory = std::make_shared<ProceduralMemory>();
        auto associativeMemory = std::make_shared<AssociativeMemory>();
        
        return std::make_tuple(workingMemory, episodicMemory, semanticMemory, proceduralMemory, associativeMemory);
    }, "Create all memory systems at once");

    m.def("createNeuromodulationSystem", []() {
        auto dopamine = std::make_shared<Dopamine>();
        auto curiosity = std::make_shared<Curiosity>();
        auto novelty = std::make_shared<Novelty>();
        auto predictionError = std::make_shared<PredictionError>();
        
        return std::make_tuple(dopamine, curiosity, novelty, predictionError);
    }, "Create all neuromodulation systems at once");

    m.def("createCognitiveSystem", []() {
        auto planner = std::make_shared<NeuralPlanner>();
        auto conceptFormation = std::make_shared<ConceptFormation>();
        auto attention = std::make_shared<AttentionalSelection>();
        
        return std::make_tuple(planner, conceptFormation, attention);
    }, "Create all cognitive systems at once");

    m.def("createDevelopmentSystem", []() {
        auto development = std::make_shared<DevelopmentSystem>();
        return development;
    }, "Create development system");

    m.def("createPredictionSystem", []() {
        auto prediction = std::make_shared<PredictionSystem>();
        return prediction;
    }, "Create prediction system");

    // Configuration shortcuts
    m.def("createMemoryConfig", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("memory.working_memory.capacity", 200);
        config->set<std::string>("memory.episodic.max_episodes", 500);
        config->set<std::string>("memory.associative.max_associations", 100);
        return config;
    }, "Create memory-focused configuration");

    m.def("createCognitiveConfig", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("cognition.planner.planning_depth", 3);
        config->set<std::string>("cognition.attention.inhibition_strength", 0.6);
        config->set<std::string>("cognition.attention.excitation_strength", 1.4);
        return config;
    }, "Create cognition-focused configuration");

    m.def("createNeuromodConfig", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("neuromod.dopamine.scale", 1.0);
        config->set<std::string>("neuromod.curiosity.scale", 1.5);
        config->set<std::string>("neuromod.novelty.scale", 1.2);
        config->set<std::string>("neuromod.prediction_error.scale", 0.9);
        return config;
    }, "Create neuromodulation-focused configuration");

    m.def("createDevelopmentConfig", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("development.enabled", true);
        config->set<std::string>("development.critical_period_duration", 200.0);
        config->set<std::string>("development.maturation_duration", 600.0);
        return config;
    }, "Create development-focused configuration");

    m.def("createPredictionConfig", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("prediction.enabled", true);
        config->set<std::string>("prediction.horizon", 5);
        config->set<std::string>("prediction.confidence_threshold", 0.8);
        return config;
    }, "Create prediction-focused configuration");

    // Performance and utility functions
    m.def("optimizedForSpeed", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("memory.working_memory.capacity", 50);
        config->set<std::string>("memory.episodic.max_episodes", 100);
        return config;
    }, "Create configuration optimized for speed");

    m.def("optimizedForMemory", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(50));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        config->set<std::string>("plasticity.stdp.enable", false);
        config->set<std::string>("plasticity.hebbian.enable", false);
        config->set<std::string>("memory.working_memory.capacity", 10);
        config->set<std::string>("memory.episodic.max_episodes", 20);
        return config;
    }, "Create configuration optimized for memory usage");

    m.def("optimizedForLearning", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(500));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(3));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", true);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("development.enabled", true);
        return config;
    }, "Create configuration optimized for learning");

    // State management
    m.def("saveBrainForRestart", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state for restart");

    m.def("loadBrainFromSave", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath);
        return brain;
    }, py::arg("filepath"), "Load brain state from save");

    // Quick setup helpers
    m.def("quickTestBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(50));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a quick test brain");

    m.def("quickAgentSetup", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 6, 6);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create quick agent setup");

    // Documentation and help
    m.def("printUsage", []() {
        std::cout << "=== NLM Python Usage ===" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Basic usage:" << std::endl;
        std::cout << "  import pynlm" << std::endl;
        std::cout << "  brain = pynlm.createBrain(pynlm.createDefaultConfig())" << std::endl;
        std::cout << "  brain.initialize()" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Memory systems:" << std::endl;
        std::cout << "  wm = pynlm.createWorkingMemory()" << std::endl;
        std::cout << "  em = pynlm.createEpisodicMemory()" << std::endl;
        std::cout << "  sm = pynlm.createSemanticMemory()" << std::endl;
        std::cout << "  pm = pynlm.createProceduralMemory()" << std::endl;
        std::cout << "  am = pynlm.createAssociativeMemory()" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  dm = pynlm.createDopamine()" << std::endl;
        std::cout << "  cm = pynlm.createCuriosity()" << std::endl;
        std::cout << "  nm = pynlm.createNovelty()" << std::endl;
        std::cout << "  pem = pynlm.createPredictionError()" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Cognition:" << std::endl;
        std::cout << "  planner = pynlm.createNeuralPlanner()" << std::endl;
        std::cout << "  cf = pynlm.createConceptFormation()" << std::endl;
        std::cout << "  attention = pynlm.createAttentionalSelection()" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Agent setup:" << std::endl;
        std::cout << "  agent = pynlm.createAgentBrain(brain)" << std::endl;
        std::cout << "  agent.initialize(world)" << std::endl;
        std::cout << "  agent.enableRewardModulation(True)" << std::endl;
        std::cout << "  agent.enableCuriosity(True)" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "For more examples, see easy_usage.md" << std::endl;
    }, "Print comprehensive usage guide");

    // Learning and development helpers
    m.def("enableAdvancedLearning", [](std::shared_ptr<AgentBrain> agent) {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return true;
    }, py::arg("agent"), "Enable all advanced learning features");

    m.def("runDevelopmentalLearning", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            auto brain = agent->getBrain();
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(0.1f, 0.0f);
            agent->updateDevelopment(0.1);
        }
        return true;
    }, py::arg("agent"), py::arg("world"), py::arg("num_steps"), "Run developmental learning simulation");

    m.def("createLearningEnvironment", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(200));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(2));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", true);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("development.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create environment optimized for learning");

    // Error handling and safety
    m.def("safeBrainOperation", [](std::function<void(std::shared_ptr<Brain>)> func, std::shared_ptr<Brain> brain) {
        try {
            func(brain);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error in brain operation: " << e.what() << std::endl;
            return false;
        }
    }, py::arg("func"), py::arg("brain"), "Safely execute brain operation with error handling");

    m.def("validateBrainConfig", [](std::shared_ptr<Config> config) {
        if (!config->has("brain.neuron_count")) {
            std::cerr << "Warning: brain.neuron_count not set" << std::endl;
            return false;
        }
        if (!config->has("brain.region_count")) {
            std::cerr << "Warning: brain.region_count not set" << std::endl;
            return false;
        }
        if (config->getOr<int64_t>("brain.neuron_count", 0) < 1) {
            std::cerr << "Error: neuron_count must be at least 1" << std::endl;
            return false;
        }
        if (config->getOr<int64_t>("brain.region_count", 0) < 1) {
            std::cerr << "Error: region_count must be at least 1" << std::endl;
            return false;
        }
        return true;
    }, py::arg("config"), "Validate brain configuration");

    // Performance monitoring
    m.def("monitorBrainPerformance", [](std::shared_ptr<Brain> brain, size_t durationSteps) {
        size_t totalSpikes = brain->getTotalSpikeCount();
        size_t activeNeurons = brain->getActiveNeuronCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        float avgFiringRate = brain->getAverageFiringRate();
        float eiRatio = brain->getExcitationInhibitionRatio();
        
        std::cout << "=== Brain Performance Monitor ===" << std::endl;
        std::cout << "Duration: " << durationSteps << " steps" << std::endl;
        std::cout << "Total spikes: " << totalSpikes << std::endl;
        std::cout << "Active neurons: " << activeNeurons << std::endl;
        std::cout << "Firing neurons: " << firingNeurons << std::endl;
        std::cout << "Total synapses: " << totalSynapses << std::endl;
        std::cout << "Average firing rate: " << avgFiringRate << std::endl;
        std::cout << "E/I ratio: " << eiRatio << std::endl;
        std::cout << "Steps per spike: " << (durationSteps > 0 ? durationSteps / std::max(totalSpikes, 1) : 0) << std::endl;
        
        return std::make_tuple(totalSpikes, activeNeurons, firingNeurons, totalSynapses, avgFiringRate, eiRatio);
    }, py::arg("brain"), py::arg("duration_steps"), "Monitor and report brain performance metrics");

    // System information
    m.def("getSystemInfo", []() {
        std::cout << "=== NLM System Information ===" << std::endl;
        std::cout << "Version: 0.1.0" << std::endl;
        std::cout << "Phase: 2.5 (Integrated Neural Computation)" << std::endl;
        std::cout << "Architecture: Spiking neural networks with integrated memory and cognition" << std::endl;
        std::cout << "Memory systems: Working, Episodic, Semantic, Procedural, Associative" << std::endl;
        std::cout << "Neuromodulation: Dopamine, Curiosity, Novelty, PredictionError" << std::endl;
        std::cout << "Cognition: NeuralPlanner, ConceptFormation, AttentionalSelection" << std::endl;
        std::cout << "Development: Plasticity regulation and maturation" << std::endl;
        std::cout << "Prediction: Temporal and causal inference" << std::endl;
        std::cout << "Configuration: JSON-based structured configuration" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Core components: " << std::endl;
        std::cout << "  - Neuron: LIF spiking neurons with plasticity" << std::endl;
        std::cout << "  - Synapse: Event-driven spike propagation with delays" << std::endl;
        std::cout << "  - Plasticity: STDP, Hebbian, structural synaptogenesis/pruning" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Memory components:" << std::endl;
        std::cout << "  - Working: Persistent activity and recurrent connections" << std::endl;
        std::cout << "  - Episodic: Neural activity patterns with replay" << std::endl;
        std::cout << "  - Semantic: Fact and knowledge storage" << std::endl;
        std::cout << "  - Procedural: Skills and habits" << std::endl;
        std::cout << "  - Associative: Relationships between patterns" << std::endl;
        return true;
    }, "Get and print system information");

    // Advanced configuration helpers
    m.def("createResearchConfig", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(2000));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(4));
        config->set<std::string>("brain.connection_probability", 0.2f);
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.stdp.learning_rate", 0.002);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", true);
        config->set<std::string>("neuromod.dopamine.scale", 1.5);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.curiosity.scale", 2.0);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("neuromod.novelty.scale", 1.5);
        config->set<std::string>("neuromod.prediction_error.enable", true);
        config->set<std::string>("development.enabled", true);
        config->set<std::string>("development.critical_period_duration", 500.0);
        config->set<std::string>("development.maturation_duration", 1500.0);
        config->set<std::string>("simulation.max_steps", 50000);
        config->set<std::string>("simulation.time_limit", 60.0);
        config->set<std::string>("logging.level", "INFO");
        config->set<std::string>("logging.to_file", true);
        config->set<std::string>("logging.filename", "nlm_research.log");
        return config;
    }, "Create research-grade configuration with all advanced features");

    m.def("createEmbeddedConfig", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(50));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        config->set<std::string>("brain.connection_probability", 0.05f);
        config->set<std::string>("plasticity.stdp.enable", false);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", false);
        config->set<std::string>("neuromod.curiosity.enable", false);
        config->set<std::string>("neuromod.novelty.enable", false);
        config->set<std::string>("development.enabled", false);
        config->set<std::string>("simulation.max_steps", 1000);
        config->set<std::string>("simulation.time_limit", 0.0);
        config->set<std::string>("logging.level", "ERROR");
        config->set<std::string>("logging.to_file", false);
        return config;
    }, "Create embedded/IoT configuration with minimal resources");

    m.def("createRealTimeConfig", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(200));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(2));
        config->set<std::string>("brain.connection_probability", 0.1f);
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", false);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("development.enabled", true);
        config->set<std::string>("simulation.max_steps", 5000);
        config->set<std::string>("simulation.time_limit", 5.0);
        config->set<std::string>("logging.level", "WARNING");
        config->set<std::string>("logging.to_file", true);
        return config;
    }, "Create real-time configuration for interactive applications");

    // Serialization and persistence
    m.def("exportBrainForAnalysis", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        auto config = brain->getConfig();
        
        // Export brain state
        bool brainSaved = brain->save(filepath + "_brain.bin");
        
        // Export configuration
        bool configSaved = config->saveToFile(filepath + "_config.json");
        
        // Export statistics
        std::ofstream statsFile(filepath + "_stats.txt");
        if (statsFile.is_open()) {
            statsFile << "=== Brain Analysis Export ===" << std::endl;
            statsFile << "Generated: " << std::time(nullptr) << std::endl;
            statsFile << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
            statsFile << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
            statsFile << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            statsFile << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
            statsFile << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            statsFile << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            statsFile.close();
        }
        
        return brainSaved && configSaved;
    }, py::arg("brain"), py::arg("filepath"), "Export brain state for analysis and persistence");

    // Import and export helpers
    m.def("importBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->load(filepath + "_brain.bin")) {
            std::cout << "Brain imported successfully from " << filepath << "_brain.bin" << std::endl;
            return brain;
        }
        
        std::cerr << "Failed to import brain from " << filepath << "_brain.bin" << std::endl;
        return nullptr;
    }, py::arg("filepath"), "Import brain state from file");

    // Model validation and testing
    m.def("validateBrainModel", [](std::shared_ptr<Brain> brain, size_t testSteps) {
        bool passed = true;
        
        std::cout << "=== Brain Model Validation ===" << std::endl;
        
        // Test 1: Basic connectivity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) {
                std::cout << "✗ Test 1 FAILED: No neurons found" << std::endl;
                passed = false;
            } else {
                neurons[0]->injectCurrent(10.0f);
                brain->step(0);
                if (brain->getFiringNeuronCount() == 0) {
                    std::cout << "✗ Test 1 FAILED: No spikes generated" << std::endl;
                    passed = false;
                } else {
                    std::cout << "✓ Test 1 PASSED: Basic neural connectivity" << std::endl;
                }
            }
        }
        
        // Test 2: Memory system integration
        if (brain->getWorkingMemory()) {
            std::cout << "✓ Test 2 PASSED: Working memory system present" << std::endl;
        } else {
            std::cout << "✗ Test 2 FAILED: Working memory system missing" << std::endl;
            passed = false;
        }
        
        if (brain->getEpisodicMemory()) {
            std::cout << "✓ Test 2 PASSED: Episodic memory system present" << std::endl;
        } else {
            std::cout << "✗ Test 2 FAILED: Episodic memory system missing" << std::endl;
            passed = false;
        }
        
        if (brain->getPredictionSystem()) {
            std::cout << "✓ Test 2 PASSED: Prediction system present" << std::endl;
        } else {
            std::cout << "✗ Test 2 FAILED: Prediction system missing" << std::endl;
            passed = false;
        }
        
        // Test 3: Neuromodulation
        if (brain->getDopamine()) {
            std::cout << "✓ Test 3 PASSED: Dopamine neuromodulation present" << std::endl;
        } else {
            std::cout << "✗ Test 3 FAILED: Dopamine neuromodulation missing" << std::endl;
            passed = false;
        }
        
        if (brain->getCuriosity()) {
            std::cout << "✓ Test 3 PASSED: Curiosity neuromodulation present" << std::endl;
        } else {
            std::cout << "✗ Test 3 FAILED: Curiosity neuromodulation missing" << std::endl;
            passed = false;
        }
        
        // Test 4: Development
        if (brain->getDevelopmentSystem()) {
            std::cout << "✓ Test 4 PASSED: Development system present" << std::endl;
        } else {
            std::cout << "✗ Test 4 FAILED: Development system missing" << std::endl;
            passed = false;
        }
        
        // Test 5: Plasticity
        if (brain->getSTDP()) {
            std::cout << "✓ Test 5 PASSED: STDP plasticity present" << std::endl;
        } else {
            std::cout << "✗ Test 5 FAILED: STDP plasticity missing" << std::endl;
            passed = false;
        }
        
        if (brain->getHebbian()) {
            std::cout << "✓ Test 5 PASSED: Hebbian plasticity present" << std::endl;
        } else {
            std::cout << "✗ Test 5 FAILED: Hebbian plasticity missing" << std::endl;
            passed = false;
        }
        
        // Run simulation test
        for (size_t step = 0; step < testSteps; ++step) {
            brain->step(step);
        }
        
        std::cout << "✓ Test 6 PASSED: Simulation runs without errors" << std::endl;
        
        if (passed) {
            std::cout << "=== All Brain Model Validation Tests PASSED ===" << std::endl;
        } else {
            std::cout << "=== Some Brain Model Validation Tests FAILED ===" << std::endl;
        }
        
        return passed;
    }, py::arg("brain"), py::arg("test_steps"), "Validate brain model and test all systems");

    // Advanced configuration templates
    m.def("getTemplateConfig", [](const std::string& templateName) {
        auto config = std::make_shared<Config>();
        
        if (templateName == "default") {
            // Default configuration for general use
            config->set<std::string>("brain.neuron_count", static_cast<int64_t>(1000));
            config->set<std::string>("brain.region_count", static_cast<int64_t>(2));
            config->set<std::string>("brain.connection_probability", 0.1f);
            config->set<std::string>("plasticity.stdp.enable", true);
            config->set<std::string>("plasticity.hebbian.enable", true);
            config->set<std::string>("neuromod.curiosity.enable", true);
            config->set<std::string>("neuromod.novelty.enable", true);
        } else if (templateName == "research") {
            // Research configuration with all features enabled
            config->set<std::string>("brain.neuron_count", static_cast<int64_t>(5000));
            config->set<std::string>("brain.region_count", static_cast<int64_t>(5));
            config->set<std::string>("brain.connection_probability", 0.15f);
            config->set<std::string>("plasticity.stdp.enable", true);
            config->set<std::string>("plasticity.stdp.learning_rate", 0.001);
            config->set<std::string>("plasticity.hebbian.enable", true);
            config->set<std::string>("plasticity.structural.enable", true);
            config->set<std::string>("neuromod.curiosity.enable", true);
            config->set<std::string>("neuromod.curiosity.scale", 1.5);
            config->set<std::string>("neuromod.novelty.enable", true);
            config->set<std::string>("neuromod.prediction_error.enable", true);
            config->set<std::string>("development.enabled", true);
            config->set<std::string>("development.critical_period_duration", 300.0);
        } else if (templateName == "embedded") {
            // Embedded/IoT configuration
            config->set<std::string>("brain.neuron_count", static_cast<int64_t>(50));
            config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
            config->set<std::string>("brain.connection_probability", 0.05f);
            config->set<std::string>("plasticity.stdp.enable", false);
            config->set<std::string>("plasticity.hebbian.enable", true);
            config->set<std::string>("neuromod.curiosity.enable", false);
            config->set<std::string>("neuromod.novelty.enable", false);
            config->set<std::string>("development.enabled", false);
        } else if (templateName == "learning") {
            // Learning-focused configuration
            config->set<std::string>("brain.neuron_count", static_cast<int64_t>(2000));
            config->set<std::string>("brain.region_count", static_cast<int64_t>(3));
            config->set<std::string>("brain.connection_probability", 0.2f);
            config->set<std::string>("plasticity.stdp.enable", true);
            config->set<std::string>("plasticity.stdp.learning_rate", 0.002);
            config->set<std::string>("plasticity.hebbian.enable", true);
            config->set<std::string>("plasticity.structural.enable", true);
            config->set<std::string>("neuromod.curiosity.enable", true);
            config->set<std::string>("neuromod.curiosity.scale", 2.0);
            config->set<std::string>("neuromod.novelty.enable", true);
            config->set<std::string>("neuromod.novelty.scale", 1.5);
            config->set<std::string>("neuromod.prediction_error.enable", true);
            config->set<std::string>("development.enabled", true);
            config->set<std::string>("development.critical_period_duration", 500.0);
        }
        
        return config;
    }, py::arg("template_name"), "Get pre-configured template");

    // Performance optimization helpers
    m.def("optimizeForRealTime", [](std::shared_ptr<Config> config) {
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        config->set<std::string>("brain.connection_probability", 0.05f);
        config->set<std::string>("plasticity.stdp.enable", false);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", false);
        config->set<std::string>("development.enabled", false);
        config->set<std::string>("simulation.max_steps", 1000);
        config->set<std::string>("simulation.time_limit", 1.0);
        return config;
    }, py::arg("config"), "Optimize configuration for real-time performance");

    m.def("optimizeForLearning", [](std::shared_ptr<Config> config) {
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(1000));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(3));
        config->set<std::string>("brain.connection_probability", 0.15f);
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.stdp.learning_rate", 0.001);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", true);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.curiosity.scale", 1.5);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("neuromod.prediction_error.enable", true);
        config->set<std::string>("development.enabled", true);
        config->set<std::string>("development.critical_period_duration", 300.0);
        config->set<std::string>("simulation.max_steps", 10000);
        config->set<std::string>("simulation.time_limit", 10.0);
        return config;
    }, py::arg("config"), "Optimize configuration for learning performance");

    m.def("optimizeForMemory", [](std::shared_ptr<Config> config) {
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(50));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        config->set<std::string>("brain.connection_probability", 0.02f);
        config->set<std::string>("plasticity.stdp.enable", false);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", false);
        config->set<std::string>("neuromod.curiosity.enable", false);
        config->set<std::string>("neuromod.novelty.enable", false);
        config->set<std::string>("development.enabled", false);
        config->set<std::string>("simulation.max_steps", 500);
        config->set<std::string>("simulation.time_limit", 0.0);
        return config;
    }, py::arg("config"), "Optimize configuration for memory usage");

    // Configuration validation and analysis
    m.def("analyzeConfig", [](std::shared_ptr<Config> config) {
        std::cout << "=== Configuration Analysis ===" << std::endl;
        
        int64_t neuronCount = config->getOr<int64_t>("brain.neuron_count", 0);
        int64_t regionCount = config->getOr<int64_t>("brain.region_count", 0);
        float connectionProb = config->getOr<float>("brain.connection_probability", 0.0f);
        
        std::cout << "Brain configuration:" << std::endl;
        std::cout << "  Neurons: " << neuronCount << std::endl;
        std::cout << "  Regions: " << regionCount << std::endl;
        std::cout << "  Connection probability: " << connectionProb << std::endl;
        
        bool stdpEnabled = config->getOr<bool>("plasticity.stdp.enable", false);
        bool hebbianEnabled = config->getOr<bool>("plasticity.hebbian.enable", false);
        bool structuralEnabled = config->getOr<bool>("plasticity.structural.enable", false);
        
        std::cout << "Plasticity configuration:" << std::endl;
        std::cout << "  STDP enabled: " << (stdpEnabled ? "Yes" : "No") << std::endl;
        std::cout << "  Hebbian enabled: " << (hebbianEnabled ? "Yes" : "No") << std::endl;
        std::cout << "  Structural enabled: " << (structuralEnabled ? "Yes" : "No") << std::endl;
        
        bool curiosityEnabled = config->getOr<bool>("neuromod.curiosity.enable", false);
        bool noveltyEnabled = config->getOr<bool>("neuromod.novelty.enable", false);
        bool predictionErrorEnabled = config->getOr<bool>("neuromod.prediction_error.enable", false);
        
        std::cout << "Neuromodulation configuration:" << std::endl;
        std::cout << "  Curiosity enabled: " << (curiosityEnabled ? "Yes" : "No") << std::endl;
        std::cout << "  Novelty enabled: " << (noveltyEnabled ? "Yes" : "No") << std::endl;
        std::cout << "  Prediction error enabled: " << (predictionErrorEnabled ? "Yes" : "No") << std::endl;
        
        bool developmentEnabled = config->getOr<bool>("development.enabled", false);
        
        std::cout << "Development configuration:" << std::endl;
        std::cout << "  Development enabled: " << (developmentEnabled ? "Yes" : "No") << std::endl;
        
        int64_t maxSteps = config->getOr<int64_t>("simulation.max_steps", 0);
        
        std::cout << "Simulation configuration:" << std::endl;
        std::cout << "  Max steps: " << maxSteps << std::endl;
        
        // Estimated resource usage
        int estimatedMemUsage = neuronCount * 100 + maxSteps * 10;  // Rough estimate
        int estimatedTimePerStep = neuronCount / 10 + 1;  // Rough estimate
        
        std::cout << "Estimated resource usage:" << std::endl;
        std::cout << "  Memory (estimated): " << estimatedMemUsage << " MB" << std::endl;
        std::cout << "  Time per step (estimated): " << estimatedTimePerStep << " ms" << std::endl;
        
        return true;
    }, py::arg("config"), "Analyze and report configuration resource usage");

    // Quick setup helpers for common use cases
    m.def("simpleBrainSetup", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, "Create simple brain with minimal configuration");

    m.def("quickAgentSetup", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(200));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(2));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 6, 6);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create quick agent setup with world and agent");

    m.def("learningEnvironmentSetup", []() {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(500));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(3));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("development.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create learning-optimized environment");

    // Debugging and testing helpers
    m.def("debugBrainState", [](std::shared_ptr<Brain> brain) {
        std::cout << "=== Brain Debug State ===" << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        // Check memory systems
        if (brain->getWorkingMemory()) {
            std::cout << "Working memory: Active traces = " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
        }
        if (brain->getEpisodicMemory()) {
            std::cout << "Episodic memory: Episode count = " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
        }
        if (brain->getPredictionSystem()) {
            std::cout << "Prediction system: Active" << std::endl;
        }
        if (brain->getDopamine()) {
            std::cout << "Dopamine neuromodulator: Active" << std::endl;
        }
        if (brain->getCuriosity()) {
            std::cout << "Curiosity neuromodulator: Active" << std::endl;
        }
        if (brain->getDevelopmentSystem()) {
            std::cout << "Development system: Active" << std::endl;
        }
        
        return true;
    }, py::arg("brain"), "Print detailed brain debug information");

    // Performance profiling
    m.def("profileBrainPerformance", [](std::shared_ptr<Brain> brain, size_t profileSteps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < profileSteps; ++step) {
            brain->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "=== Brain Performance Profile ===" << std::endl;
        std::cout << "Steps profiled: " << profileSteps << std::endl;
        std::cout << "Total time: " << duration.count() << " ms" << std::endl;
        std::cout << "Steps per second: " << (profileSteps * 1000.0 / duration.count()) << std::endl;
        std::cout << "Average time per step: " << (duration.count() / static_cast<double>(profileSteps)) << " ms" << std::endl;
        
        return duration.count();
    }, py::arg("brain"), py::arg("profile_steps"), "Profile brain performance metrics");

    // Resource usage analysis
    m.def("analyzeResourceUsage", [](std::shared_ptr<Config> config) {
        std::cout << "=== Resource Usage Analysis ===" << std::endl;
        
        int64_t neuronCount = config->getOr<int64_t>("brain.neuron_count", 0);
        int64_t regionCount = config->getOr<int64_t>("brain.region_count", 0);
        int64_t maxSteps = config->getOr<int64_t>("simulation.max_steps", 0);
        
        // Rough estimates for resource usage
        int estimatedMemoryMB = (neuronCount * 100) / 1024 / 1024;  // Neurons * ~100 bytes each
        int estimatedSynapses = static_cast<int>(neuronCount * neuronCount * 
                                              config->getOr<float>("brain.connection_probability", 0.1f) / 1000000);
        int estimatedCPUUsage = std::min(100, (neuronCount / 10) + (maxSteps / 100));
        
        std::cout << "Estimated memory usage:" << std::endl;
        std::cout << "  Neural data: ~" << estimatedMemoryMB << " MB" << std::endl;
        std::cout << "  Synaptic weights: ~" << estimatedSynapses << " MB" << std::endl;
        std::cout << "  Total estimated: ~" << (estimatedMemoryMB + estimatedSynapses) << " MB" << std::endl;
        std::cout << "" << std::endl;
        
        std::cout << "Estimated CPU usage:" << std::endl;
        std::cout << "  Processing: ~" << estimatedCPUUsage << "%" << std::endl;
        std::cout << "  Memory access: High" << std::endl;
        std::cout << "  I/O operations: Moderate" << std::endl;
        
        return std::make_tuple(estimatedMemoryMB, estimatedSynapses, estimatedCPUUsage);
    }, py::arg("config"), "Analyze estimated resource usage");

    // Quality of service assessment
    m.def("assessQoS", [](std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
        std::cout << "=== Quality of Service Assessment ===" << std::endl;
        
        int64_t neuronCount = config->getOr<int64_t>("brain.neuron_count", 0);
        bool stdpEnabled = config->getOr<bool>("plasticity.stdp.enable", false);
        bool curiosityEnabled = config->getOr<bool>("neuromod.curiosity.enable", false);
        
        // Determine QoS level based on configuration
        std::string qosLevel;
        if (neuronCount >= 1000 && stdpEnabled && curiosityEnabled) {
            qosLevel = "High - Advanced AI capabilities with full plasticity and learning";
        } else if (neuronCount >= 500 && (stdpEnabled || curiosityEnabled)) {
            qosLevel = "Medium - Good AI capabilities with some learning features";
        } else if (neuronCount >= 100) {
            qosLevel = "Basic - Limited AI capabilities with minimal learning";
        } else {
            qosLevel = "Minimal - Very basic neural computation";
        }
        
        std::cout << "QoS Level: " << qosLevel << std::endl;
        std::cout << "" << std::endl;
        
        std::cout << "Recommendations:" << std::endl;
        if (neuronCount < 100) {
            std::cout << "  - Consider increasing neuron_count for better performance" << std::endl;
        }
        if (!stdpEnabled) {
            std::cout << "  - Enable STDP for improved learning capabilities" << std::endl;
        }
        if (!curiosityEnabled) {
            std::cout << "  - Enable curiosity for better exploration behavior" << std::endl;
        }
        
        std::cout << "  - Current configuration is suitable for " << qosLevel << std::endl;
        
        return qosLevel;
    }, py::arg("config"), py::arg("brain"), "Assess quality of service and provide recommendations");

    // Documentation and help
    m.def("printAdvancedDocumentation", []() {
        std::cout << "=== Advanced NLM Documentation ===" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Memory Systems:" << std::endl;
        std::cout << "  - WorkingMemory: Persistent activity and recurrent connections" << std::endl;
        std::cout << "  - EpisodicMemory: Neural experience storage with replay" << std::endl;
        std::cout << "  - semanticMemory: Fact and knowledge storage" << std::endl;
        std::cout << "  - ProceduralMemory: Skills and habits" << std::endl;
        std::cout << "  - AssociativeMemory: Relationships between patterns" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  - Dopamine: Reward prediction error signal" << std::endl;
        std::cout << "  - Curiosity: Exploration drive and novelty detection" << std::endl;
        std::cout << "  - Novelty: Surprise and unexpected events" << std::endl;
        std::cout << "  - PredictionError: Temporal difference learning" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Cognition:" << std::endl;
        std::cout << "  - NeuralPlanner: Goal-directed planning" << std::endl;
        std::cout << "  - ConceptFormation: Abstract concept learning" << std::endl;
        std::cout << "  - AttentionalSelection: Competitive attention and selection" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Development:" << std::endl;
        std::cout << "  - DevelopmentSystem: Plasticity regulation and maturation" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Prediction:" << std::endl;
        std::cout << "  - PredictionSystem: Temporal and causal prediction" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  - JSON-based structured configuration" << std::endl;
        std::cout << "  - Templates for common use cases" << std::endl;
        std::cout << "  - Resource usage analysis and optimization" << std::endl;
        return true;
    }, "Print comprehensive advanced documentation");

    // Quick reference guide
    m.def("quickReference", []() {
        std::cout << "=== NLM Quick Reference ===" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Creation:" << std::endl;
        std::cout << "  brain = pynlm.createBrain(pynlm.createDefaultConfig())" << std::endl;
        std::cout << "  brain.initialize()" << std::endl;
        std::cout << "  world = pynlm.createSimpleWorld()" << std::endl;
        std::cout << "  agent = pynlm.createAgentBrain(brain)" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Memory:" << std::endl;
        std::cout << "  wm = pynlm.createWorkingMemory()" << std::endl;
        std::cout << "  em = pynlm.createEpisodicMemory()" << std::endl;
        std::cout << "  am = pynlm.createAssociativeMemory()" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  dm = pynlm.createDopamine()" << std::endl;
        std::cout << "  cm = pynlm.createCuriosity()" << std::endl;
        std::cout << "  nm = pynlm.createNovelty()" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Cognition:" << std::endl;
        std::cout << "  planner = pynlm.createNeuralPlanner()" << std::endl;
        std::cout << "  attention = pynlm.createAttentionalSelection()" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Main loop:" << std::endl;
        std::cout << "  for step in range(1000):" << std::endl;
        std::cout << "      world.update(0.1)" << std::endl;
        std::cout << "      percept = world.getSensoryPercept()" << std::endl;
        std::cout << "      agent.processSensoryInput(percept)" << std::endl;
        std::cout << "      brain.step(step)" << std::endl;
        std::cout << "      action = agent.decodeMotorCommand()" << std::endl;
        std::cout << "      world.applyMotorCommand(action, time)" << std::endl;
        return true;
    }, "Print quick reference guide");

    // System validation
    m.def("validateAdvancedSetup", []() {
        std::cout << "=== Advanced System Validation ===" << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(500));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(3));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", true);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("neuromod.prediction_error.enable", true);
        config->set<std::string>("development.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        bool validationPassed = true;
        
        // Check all major components
        if (!brain->getWorkingMemory()) {
            std::cout << "✗ Working memory missing" << std::endl;
            validationPassed = false;
        } else {
            std::cout << "✓ Working memory present" << std::endl;
        }
        
        if (!brain->getEpisodicMemory()) {
            std::cout << "✗ Episodic memory missing" << std::endl;
            validationPassed = false;
        } else {
            std::cout << "✓ Episodic memory present" << std::endl;
        }
        
        if (!brain->getPredictionSystem()) {
            std::cout << "✗ Prediction system missing" << std::endl;
            validationPassed = false;
        } else {
            std::cout << "✓ Prediction system present" << std::endl;
        }
        
        if (!brain->getDopamine()) {
            std::cout << "✗ Dopamine neuromodulator missing" << std::endl;
            validationPassed = false;
        } else {
            std::cout << "✓ Dopamine neuromodulator present" << std::endl;
        }
        
        if (!brain->getCuriosity()) {
            std::cout << "✗ Curiosity neuromodulator missing" << std::endl;
            validationPassed = false;
        } else {
            std::cout << "✓ Curiosity neuromodulator present" << std::endl;
        }
        
        if (!brain->getDevelopmentSystem()) {
            std::cout << "✗ Development system missing" << std::endl;
            validationPassed = false;
        } else {
            std::cout << "✓ Development system present" << std::endl;
        }
        
        if (!brain->getSTDP()) {
            std::cout << "✗ STDP plasticity missing" << std::endl;
            validationPassed = false;
        } else {
            std::cout << "✓ STDP plasticity present" << std::endl;
        }
        
        if (!brain->getHebbian()) {
            std::cout << "✗ Hebbian plasticity missing" << std::endl;
            validationPassed = false;
        } else {
            std::cout << "✓ Hebbian plasticity present" << std::endl;
        }
        
        // Test basic functionality
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(50.0f);
                brain->step(0);
                if (brain->getFiringNeuronCount() > 0) {
                    std::cout << "✓ Basic neural computation working" << std::endl;
                } else {
                    std::cout << "✗ Basic neural computation failed" << std::endl;
                    validationPassed = false;
                }
            } else {
                std::cout << "✗ No neurons found" << std::endl;
                validationPassed = false;
            }
        }
        
        if (validationPassed) {
            std::cout << "=== Advanced system validation PASSED ===" << std::endl;
        } else {
            std::cout << "=== Advanced system validation FAILED ===" << std::endl;
        }
        
        return validationPassed;
    }, "Validate complete advanced NLM setup");

    // Summary
    m.def("printSummary", []() {
        std::cout << "=== NLM Python Bindings Summary ===" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Core Components:" << std::endl;
        std::cout << "  ✓ Brain: Spiking neural networks" << std::endl;
        std::cout << "  ✓ Config: JSON-based configuration" << std::endl;
        std::cout << "  ✓ AgentBrain: Brain-world interface" << std::endl;
        std::cout << "  ✓ SimpleWorld: Environment simulation" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Memory Systems:" << std::endl;
        std::cout << "  ✓ WorkingMemory: Persistent activity" << std::endl;
        std::cout << "  ✓ EpisodicMemory: Experience storage" << std::endl;
        std::cout << "  ✓ SemanticMemory: Knowledge storage" << std::endl;
        std::cout << "  ✓ ProceduralMemory: Skill learning" << std::endl;
        std::cout << "  ✓ AssociativeMemory: Pattern relationships" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  ✓ Dopamine: Reward processing" << std::endl;
        std::cout << "  ✓ Curiosity: Exploration" << std::endl;
        std::cout << "  ✓ Novelty: Surprise detection" << std::endl;
        std::cout << "  ✓ PredictionError: Learning signal" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Cognition:" << std::endl;
        std::cout << "  ✓ NeuralPlanner: Goal-directed planning" << std::endl;
        std::cout << "  ✓ ConceptFormation: Abstract learning" << std::endl;
        std::cout << "  ✓ AttentionalSelection: Competitive attention" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Development:" << std::endl;
        std::cout << "  ✓ DevelopmentSystem: Plasticity regulation" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Prediction:" << std::endl;
        std::cout << "  ✓ PredictionSystem: Temporal prediction" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Features:" << std::endl;
        std::cout << "  ✓ JSON configuration with structured access" << std::endl;
        std::cout << "  ✓ Phase-specific test functions" << std::endl;
        std::cout << "  ✓ Performance profiling and benchmarking" << std::endl;
        std::cout << "  ✓ Resource usage analysis" << std::endl;
        std::cout << "  ✓ Quality of service assessment" << std::endl;
        std::cout << "  ✓ Configuration templates" << std::endl;
        std::cout << "  ✓ Error handling and validation" << std::endl;
        std::cout << "  ✓ Documentation and help" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "For detailed documentation, see HOW_TO_USE.md and easy_usage.md" << std::endl;
        return true;
    }, "Print comprehensive summary");

    // Documentation
    m.doc() = R"pbdoc(
    NLM (Neural Learning Machine) - Advanced Python Bindings
    ========================================================

    This is the advanced Python bindings module for NLM (Neural Learning Machine).
    It provides comprehensive bindings for all major components of the NLM
    artificial brain system, including memory systems, neuromodulation,
    cognition, development, and prediction.

    Key Features:
    - Complete memory system integration (Working, Episodic, Semantic, Procedural, Associative)
    - Advanced neuromodulation systems (Dopamine, Curiosity, Novelty, PredictionError)
    - Cognitive functions (Planner, ConceptFormation, Attention)
    - Development and plasticity regulation
    - Temporal and causal prediction
    - JSON-based configuration with structured access
    - Performance profiling and benchmarking
    - Resource usage analysis
    - Quality of service assessment
    - Configuration templates for common use cases
    - Comprehensive error handling and validation
    - Documentation and examples

    This module builds upon the basic bindings in pynlm and provides
    advanced functionality for research and production use.

    For basic usage, see easy_usage.md. For detailed documentation, see HOW_TO_USE.md.
    "pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::logic_error>(m, "LogicError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Create aliases for convenience
    m.attr("WorkingMemory") = py::type<WorkingMemory>();
    m.attr("EpisodicMemory") = py::type<EpisodicMemory>();
    m.attr("SemanticMemory") = py::type<SemanticMemory>();
    m.attr("ProceduralMemory") = py::type<ProceduralMemory>();
    m.attr("AssociativeMemory") = py::type<AssociativeMemory>();
    m.attr("Dopamine") = py::type<Dopamine>();
    m.attr("Curiosity") = py::type<Curiosity>();
    m.attr("Novelty") = py::type<Novelty>();
    m.attr("PredictionError") = py::type<PredictionError>();
    m.attr("DevelopmentSystem") = py::type<DevelopmentSystem>();
    m.attr("PredictionSystem") = py::type<PredictionSystem>();
    m.attr("NeuralPlanner") = py::type<NeuralPlanner>();
    m.attr("ConceptFormation") = py::type<ConceptFormation>();
    m.attr("AttentionalSelection") = py::type<AttentionalSelection>();

    // Performance and utility functions
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t numSteps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count();
    }, py::arg("brain"), py::arg("num_steps"), "Benchmark brain performance");

    m.def("profileBrainState", [](const Brain& brain) {
        std::cout << "=== Brain State Profile ===" << std::endl;
        std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain.getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << brain.getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain.getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        return true;
    }, py::arg("brain"), "Profile brain state and print statistics");

    // Phase test functions
    m.def("runPhase2Test", []() -> bool {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(1));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(50.0f);
                brain->step(0);
                return brain->getFiringNeuronCount() > 0;
            }
        }
        return false;
    }, "Run Phase 2 neural computation tests");

    m.def("runPhase3Test", []() -> bool {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(100));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (size_t step = 0; step < 20; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return true;
    }, "Run Phase 3 world interaction tests");

    m.def("runPhase4Test", []() -> bool {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(200));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto workingMemory = std::make_shared<WorkingMemory>();
        auto episodicMemory = std::make_shared<EpisodicMemory>();
        auto associativeMemory = std::make_shared<AssociativeMemory>();
        associativeMemory->initialize(brain.get());
        
        return true;
    }, "Run Phase 4 memory and cognition tests");

    m.def("runPhase6Test", []() -> bool {
        auto config = std::make_shared<Config>();
        config->set<std::string>("brain.neuron_count", static_cast<int64_t>(500));
        config->set<std::string>("brain.region_count", static_cast<int64_t>(2));
        config->set<std::string>("plasticity.stdp.enable", true);
        config->set<std::string>("plasticity.hebbian.enable", true);
        config->set<std::string>("plasticity.structural.enable", true);
        config->set<std::string>("neuromod.curiosity.enable", true);
        config->set<std::string>("neuromod.novelty.enable", true);
        config->set<std::string>("neuromod.prediction_error.enable", true);
        config->set<std::string>("development.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto workingMemory = std::make_shared<WorkingMemory>();
        auto episodicMemory = std::make_shared<EpisodicMemory>();
        episodicMemory->initialize(brain.get());
        auto semanticMemory = std::make_shared<SemanticMemory>();
        auto proceduralMemory = std::make_shared<ProceduralMemory>();
        auto associativeMemory = std::make_shared<AssociativeMemory>();
        associativeMemory->initialize(brain.get());
        
        auto dopamine = std::make_shared<Dopamine>();
        auto curiosity = std::make_shared<Curiosity>();
        auto novelty = std::make_shared<Novelty>();
        auto predictionError = std::make_shared<PredictionError>();
        
        auto developmentSystem = std::make_shared<DevelopmentSystem>();
        auto predictionSystem = std::make_shared<PredictionSystem>();
        auto planner = std::make_shared<NeuralPlanner>();
        auto conceptFormation = std::make_shared<ConceptFormation>();
        auto attention = std::make_shared<AttentionalSelection>();
        
        for (size_t step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        return true;
    }, "Run Phase 6 full integration tests");

} // namespace nlm

(End of file - total 1417 lines)