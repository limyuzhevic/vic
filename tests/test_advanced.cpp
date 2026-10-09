// Memory System Tests
namespace test_memory {

void testWorkingMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Test storing and recalling patterns
        uint64_t patternId = 12345;
        workingMemory->store(patternId);
        
        // Verify pattern is stored
        auto activePatterns = workingMemory->getActivePatterns();
        assert(activePatterns.size() == 1);
        
        // Test getting pattern strength
        float strength = workingMemory->getPatternStrength(patternId);
        assert(strength > 0.0f);
        
        // Test getting recall probability
        float prob = workingMemory->getRecallProbability(patternId);
        assert(prob >= 0.0f && prob <= 1.0f);
        
        // Test memory statistics
        auto stats = workingMemory->getMemoryStats();
        assert(stats.patternCount == 1);
        
        // Clear memory
        workingMemory->clear();
        assert(workingMemory->getActivePatterns().size() == 0);
    }
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        // Test storing experiences
        uint64_t timestamp = 1000;
        uint64_t experienceId = 99;
        episodicMemory->store(experienceId);
        
        // Test recalling experiences
        auto experiences = episodicMemory->recall(timestamp);
        assert(experiences.size() == 1);
        
        // Test getting temporal range
        auto timeRange = episodicMemory->getTemporalRange();
        assert(timeRange.first == 0);
        assert(timeRange.second == 1000);
        
        // Test getting experiences by pattern
        auto patternExperiences = episodicMemory->getExperiencesByPattern(12345);
        assert(patternExperiences.size() == 0);  // Not associated yet
        
        // Test getting consolidation candidates
        auto candidates = episodicMemory->getConsolidationCandidates();
        assert(candidates.size() >= 0);
        
        // Test getting consolidation statistics
        auto consolidationStats = episodicMemory->getConsolidationStats();
        assert(consolidationStats.consolidatedExperiences == 0);
        
        // Test replay functionality
        episodicMemory->replay(500, 1500);  // Replay between these timestamps
    }
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testAssociativeMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory) {
        // Test storing associations
        uint64_t keyPattern = 123;
        uint64_t valuePattern = 456;
        float strength = 0.8f;
        
        associativeMemory->store(keyPattern, valuePattern);
        
        // Test recalling associations
        uint64_t recalledValue = associativeMemory->recall(keyPattern);
        assert(recalledValue == valuePattern);
        
        // Test getting association strength
        float retrievedStrength = associativeMemory->getAssociationStrength(keyPattern, valuePattern);
        assert(retrievedStrength > 0.0f);
        
        // Test removing associations
        associativeMemory->removeAssociation(keyPattern);
        
        uint64_t emptyRecall = associativeMemory->recall(keyPattern);
        assert(emptyRecall == 0); // Invalid ID for no association
        
        // Test getting all associations for pattern
        auto associations = associativeMemory->getPatternAssociations(keyPattern);
        assert(associations.size() == 0); // Cleared above
        
        // Test getting all keys and values
        auto keys = associativeMemory->getAllKeys();
        auto values = associativeMemory->getAllValues();
        
        // Test getting memory statistics
        auto stats = associativeMemory->getMemoryStats();
        assert(stats.associationCount == 0); // Cleared above
    }
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void testConceptFormation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        // Test processing patterns to form concepts
        uint64_t patternId = 789;
        conceptFormation->processPattern(patternId);
        
        // Test getting concepts
        auto concepts = conceptFormation->getActiveConcepts();
        assert(concepts.size() == 1);
        
        // Test getting concept stability
        float stability = conceptFormation->getConceptStability(concepts[0]);
        assert(stability >= 0.0f && stability <= 1.0f);
        
        // Test getting concept prototype
        auto prototype = conceptFormation->getConceptPrototype(concepts[0]);
        assert(prototype == patternId);
        
        // Test getting concept count
        size_t count = conceptFormation->getConceptCount();
        assert(count == 1);
        
        // Test getting concept statistics
        auto stats = conceptFormation->getConceptStats();
        assert(stats.conceptCount == 1);
        
        // Test reset functionality
        conceptFormation->reset();
        assert(conceptFormation->getConceptCount() == 0);
    }
    
    std::cout << "    testConceptFormation passed" << std::endl;
}

void testPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        // Test making predictions
        uint64_t patternId = 111;
        predictionSystem->predict(patternId);
        
        // Test getting prediction error
        float error = predictionSystem->getPredictionError(patternId);
        assert(error >= 0.0f);
        
        // Test updating prediction with actual result
        predictionSystem->updatePrediction(patternId, patternId);  // Perfect prediction
        
        // Test getting prediction accuracy
        float accuracy = predictionSystem->getPredictionAccuracy();
        assert(accuracy >= 0.0f && accuracy <= 1.0f);
        
        // Test getting prediction count
        size_t count = predictionSystem->getPredictionCount();
        assert(count > 0);
        
        // Test getting prediction statistics
        auto stats = predictionSystem->getPredictionStats();
        assert(stats.totalPredictions > 0);
        
        // Test reset functionality
        predictionSystem->reset();
        assert(predictionSystem->getPredictionCount() == 0);
    }
    
    std::cout << "    testPredictionSystem passed" << std::endl;
}

void testCognitionSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Neural Planner
    auto planner = brain.getPlanner();
    if (planner) {
        nlm::ActionType action = nlm::ActionType::MoveForward;
        planner->evaluateAction(action);
        
        auto bestAction = planner->getBestAction();
        assert(bestAction.has_value());
        
        planner->addActionPlan(1); // Plan ID
        auto activePlans = planner->getActivePlans();
        assert(activePlans.size() == 1);
        
        auto planStats = planner->getPlanStats();
        assert(planStats.totalPlans > 0);
        
        planner->reset();
        assert(planner->getPlanCount() == 0);
    }
    
    // Test Attentional Selection
    auto attention = brain.getAttention();
    if (attention) {
        std::vector<uint64_t> inputPatterns = {222, 333, 444};
        attention->select(inputPatterns);
        
        auto selected = attention->getSelectedPattern();
        assert(selected.has_value());
        
        float strength = attention->getSelectionStrength();
        assert(strength >= 0.0f && strength <= 1.0f);
        
        float competitionScore = attention->getCompetitionScore(selected.value());
        assert(competitionScore >= 0.0f);
        
        auto attentionStats = attention->getAttentionStats();
        assert(attentionStats.selectionCount > 0);
        
        attention->reset();
    }
    
    std::cout << "    testCognitionSystems passed" << std::endl;
}

void testDevelopmentSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        // Test updating development system
        developmentSystem->update(0.1);
        
        auto stage = developmentSystem->getDevelopmentalStage();
        assert(stage != nlm::DevelopmentalStage::Invalid);
        
        developmentSystem->setTargetStage(stage);
        
        float progress = developmentSystem->getDevelopmentProgress();
        assert(progress >= 0.0f && progress <= 1.0f);
        
        float duration = developmentSystem->getStageDuration(stage);
        assert(duration > 0.0f);
        
        bool criticalPeriod = developmentSystem->getCriticalPeriodActive();
        assert(criticalPeriod == (stage == nlm::DevelopmentalStage::CriticalPeriod));
        
        auto devStats = developmentSystem->getDevelopmentStats();
        assert(devStats.stageChanges > 0);
        
        developmentSystem->reset();
    }
    
    std::cout << "    testDevelopmentSystem passed" << std::endl;
}

void testNeuromodulationSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test Dopamine
    auto dopamine = brain.getDopamine();
    if (dopamine) {
        dopamine->update(1.0f, 0.1f); // reward, prediction_error
        
        float level = dopamine->getLevel();
        assert(level >= 0.0f);
        
        float strength = dopamine->getModulationStrength();
        assert(strength >= 0.0f);
        
        float actionModulation = dopamine->getActionValueModulation();
        assert(actionModulation >= 0.0f);
        
        float learningModulation = dopamine->getLearningRateModulation();
        assert(learningModulation >= 0.0f);
        
        auto stats = dopamine->getStats();
        assert(stats.totalUpdates > 0);
    }
    
    // Test Novelty
    auto novelty = brain.getNovelty();
    if (novelty) {
        novelty->update(555); // input pattern
        
        float level = novelty->getLevel();
        assert(level >= 0.0f);
        
        float surprise = novelty->getSurpriseScore();
        assert(surprise >= 0.0f);
        
        float threshold = novelty->getNoveltyThreshold();
        assert(threshold >= 0.0f);
        
        novelty->setNoveltyThreshold(0.5f);
        
        auto stats = novelty->getStats();
        assert(stats.detectionCount > 0);
    }
    
    // Test Curiosity
    auto curiosity = brain.getCuriosity();
    if (curiosity) {
        uint64_t envState = 666;
        curiosity->update(envState);
        
        float level = curiosity->getLevel();
        assert(level >= 0.0f);
        
        float exploration = curiosity->getExplorationDrive();
        assert(exploration >= 0.0f);
        
        float motivation = curiosity->getIntrinsicMotivation();
        assert(motivation >= 0.0f);
        
        bool shouldExplore = curiosity->shouldExplore();
        assert(shouldExplore == (level > 0.5f));
        
        auto stats = curiosity->getStats();
        assert(stats.explorationEvents > 0);
    }
    
    // Test PredictionError
    auto predError = brain.getPredictionErrorSignal();
    if (predError) {
        predError->update(777, 888); // predicted, actual
        
        float level = predError->getLevel();
        assert(level >= 0.0f);
        
        float magnitude = predError->getErrorMagnitude();
        assert(magnitude >= 0.0f);
        
        float derivative = predError->getTemporalDerivative();
        assert(derivative >= 0.0f);
        
        float learningModulation = predError->getLearningRateModulation();
        assert(learningModulation >= 0.0f);
        
        auto stats = predError->getStats();
        assert(stats.errorCount > 0);
    }
    
    std::cout << "    testNeuromodulationSystems passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory System tests..." << std::endl;
    try {
        testWorkingMemory();
        testEpisodicMemory();
        testAssociativeMemory();
        testConceptFormation();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << "Running Prediction System tests..." << std::endl;
    try {
        testPredictionSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << "Running Cognition System tests..." << std::endl;
    try {
        testCognitionSystems();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << "Running Development System tests..." << std::endl;
    try {
        testDevelopmentSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << "Running Neuromodulation System tests..." << std::endl;
    try {
        testNeuromodulationSystems();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_memory

// Integration Tests
namespace test_integration {

void testAdvancedBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Test advanced brain creation
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Verify all advanced systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Test that all systems can be used
    auto workingMem = brain.getWorkingMemory();
    workingMem->store(1);
    
    auto episodicMem = brain.getEpisodicMemory();
    episodicMem->store(2);
    
    auto assocMem = brain.getAssociativeMemory();
    assocMem->store(3, 4);
    
    auto predSys = brain.getPredictionSystem();
    predSys->predict(5);
    
    auto planner = brain.getPlanner();
    planner->evaluateAction(nlm::ActionType::MoveForward);
    
    auto attention = brain.getAttention();
    std::vector<uint64_t> patterns = {6, 7, 8};
    attention->select(patterns);
    
    auto conceptForm = brain.getConceptFormation();
    conceptForm->processPattern(9);
    
    auto devSys = brain.getDevelopmentSystem();
    devSys->update(0.1);
    
    auto dopamine = brain.getDopamine();
    dopamine->update(1.0f, 0.0f);
    
    auto novelty = brain.getNovelty();
    novelty->update(10);
    
    auto curiosity = brain.getCuriosity();
    curiosity->update(11);
    
    auto predError = brain.getPredictionErrorSignal();
    predError->update(12, 13);
    
    std::cout << "    testAdvancedBrainCreation passed" << std::endl;
}

void testResearchConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Test that research configuration sets appropriate defaults
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    config->set("neuromod.dopamine.scale", 2.0);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("prediction.system.enable", true);
    config->set("development.enable", true);
    config->set("memory.working.enable", true);
    config->set("memory.episodic.enable", true);
    config->set("memory.associative.enable", true);
    config->set("memory.concept.enable", true);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    std::cout << "    testResearchConfiguration passed" << std::endl;
}

void testMemoryUsage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Perform operations that use memory
    auto workingMem = brain.getWorkingMemory();
    for (size_t i = 0; i < 100; ++i) {
        workingMem->store(i);
    }
    
    auto episodicMem = brain.getEpisodicMemory();
    for (size_t i = 0; i < 50; ++i) {
        episodicMem->store(i + 1000);
    }
    
    auto assocMem = brain.getAssociativeMemory();
    for (size_t i = 0; i < 75; ++i) {
        assocMem->store(i, i + 2000);
    }
    
    auto conceptForm = brain.getConceptFormation();
    for (size_t i = 0; i < 25; ++i) {
        conceptForm->processPattern(i + 3000);
    }
    
    // Verify memory is being used
    assert(workingMem->getPatternCount() == 100);
    assert(episodicMem->getExperienceCount() == 50);
    assert(assocMem->getAllKeys().size() == 75);
    assert(conceptForm->getConceptCount() == 25);
    
    std::cout << "    testMemoryUsage passed" << std::endl;
}

void testSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test integration between systems
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(50, 50, 8, 8);
    world->reset();
    
    agentBrain->initialize(*world);
    
    // Run a few steps of integrated simulation
    for (size_t step = 0; step < 10; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agentBrain->processSensoryInput(percept);
        
        brain.step(step);
        
        auto action = agentBrain->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        agentBrain->applyRewardModulation(0.1f, 0.0f);
        agentBrain->updateDevelopment(0.1);
    }
    
    // Verify everything still works
    assert(brain.getFiringNeuronCount() >= 0);
    assert(world->getSimulationTime() > 0.0);
    
    std::cout << "    testSystemIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Integration tests..." << std::endl;
    try {
        testAdvancedBrainCreation();
        testResearchConfiguration();
        testMemoryUsage();
        testSystemIntegration();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_integration

// Main test runner
int main() {
    std::cout << "=== NLM Advanced System Tests ===" << std::endl;
    std::cout << "Testing Advanced Memory Systems:" << std::endl;
    std::cout << "  - Working Memory" << std::endl;
    std::cout << "  - Episodic Memory" << std::endl;
    std::cout << "  - Associative Memory" << std::endl;
    std::cout << "  - Concept Formation" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Running Memory tests..." << std::endl;
    try {
        test_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "Running Integration tests..." << std::endl;
    try {
        test_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== All Advanced Tests PASSED ===" << std::endl;
    return 0;
}

} // namespace nlm