    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test developmental stage progression
    auto stage = brain.getDevelopmentalStage();
    
    // Test development system access
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test setting and getting developmental stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    std::cout << "    testBrainDevelopment passed" << std::endl;
}

void testBrainPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system access
    auto* predSystem = brain.getPredictionSystem();
    assert(predSystem != nullptr);
    
    // Test prediction error signal
    auto* predError = brain.getPredictionErrorSignal();
    assert(predError != nullptr);
    
    std::cout << "    testBrainPredictionSystem passed" << std::endl;
}

void testBrainMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory access
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Test episodic memory access
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Test associative memory access
    auto* assocMem = brain.getAssociativeMemory();
    assert(assocMem != nullptr);
    
    // Test concept formation
    auto* conceptForm = brain.getConceptFormation();
    assert(conceptForm != nullptr);
    
    // Test spatial representation
    auto* spatialRep = brain.getSpatialRepresentation();
    assert(spatialRep != nullptr);
    
    // Test temporal relations
    auto* temporalRel = brain.getTemporalRelation();
    assert(temporalRel != nullptr);
    
    std::cout << "    testBrainMemorySystems passed" << std::endl;
}

void testBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation system access
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test neuromodulation level access
    float neuromodLevel = brain.getNeuromodulationLevel();
    assert(neuromodLevel >= 0.0f);
    
    float curiosityLevel = brain.getCuriosityLevel();
    assert(curiosityLevel >= 0.0f);
    
    float noveltyLevel = brain.getNoveltyLevel();
    assert(noveltyLevel >= 0.0f);
    
    std::cout << "    testBrainNeuromodulation passed" << std::endl;
}

void testBrainCognitionSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test attentional selection
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Test neural planner
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test self-model
    auto* selfModel = brain.getSelfModel();
    assert(selfModel != nullptr);
    
    // Test social learning
    auto* socialLearning = brain.getSocialLearning();
    assert(socialLearning != nullptr);
    
    std::cout << "    testBrainCognitionSystems passed" << std::endl;
}

void testBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create and inject sensory input
    auto visionInput = std::make_shared<nlm::Vision>(10, 10, 3);
    std::vector<float> testData(100, 0.5f);  // Simple uniform data
    visionInput->setData(testData);
    
    brain.receiveSensoryInput(*visionInput);
    
    // Verify that sensory input was processed
    size_t sensoryNeurons = 0;
    auto* spikeSystem = brain.getSpikeSystem();
    if (spikeSystem) {
        // Count sensory-related neurons
        for (size_t i = 0; i < brain.getTotalNeuronCount(); ++i) {
            // Check if neuron is sensory type (this requires neuron inspection)
            // For now, just verify no crash occurred
        }
    }
    
    std::cout << "    testBrainSensoryProcessing passed" << std::endl;
}

void testBrainActionComplexity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test action properties
    assert(action->getType() != nlm::ActionType::Custom || 
           !action->getParameters().empty());
    
    // Test different action types through motor system
    auto motorAction = brain.produceAction();
    assert(motorAction != nullptr);
    
    std::cout << "    testBrainActionComplexity passed" << std::endl;
}

void testBrainPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity
    auto* region = brain.getRegion(nlm::RegionId(0));
    if (region) {
        for (auto& synapse : region->getSynapses()) {
            synapse->enablePlasticity(true, true, false);
        }
    }
    
    // Test plasticity system access
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structural = brain.getStructuralPlasticity();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    assert(structural != nullptr);
    
    // Test plasticity updates
    brain.updatePlasticity();
    
    std::cout << "    testBrainPlasticityIntegration passed" << std::endl;
}

void testBrainCheckpointSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run some steps to generate activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointFile = "test_checkpoint.nlm";
    assert(brain1.save(checkpointFile));
    
    // Create new brain and load state
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    assert(brain2.load(checkpointFile));
    
    // Verify key properties match (approximately)
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount());
    
    // Cleanup
    std::remove(checkpointFile.c_str());
    
    std::cout << "    testBrainCheckpointSaveLoad passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive Brain tests..." << std::endl;
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainDevelopment();
    testBrainPredictionSystem();
    testBrainMemorySystems();
    testBrainNeuromodulation();
    testBrainCognitionSystems();
    testBrainSensoryProcessing();
    testBrainActionComplexity();
    testBrainPlasticityIntegration();
    testBrainCheckpointSaveLoad();
}