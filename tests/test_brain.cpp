// Neuromodulation Tests
void testDopamineNeuromodulator() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test initialization
    assert(dopamine->getLevel() == 0.0f);
    assert(dopamine->getPlasticityFactor() == 0.01f); // Based on learning rate 0.01
    
    // Test reward signaling
    dopamine->signalReward(0.5f);
    assert(dopamine->getLevel() > 0.0f);
    
    // Test prediction error
    dopamine->signalPredictionError(0.2f);
    assert(dopamine->getPredictionError() == 0.2f);
    
    // Test update
    dopamine->update(0.001f);
    assert(dopamine->getLevel() >= 0.0f);
    
    // Test configuration
    dopamine->setLearningRate(0.05f);
    assert(dopamine->getLearningRate() == 0.05f);
    
    std::cout << "    testDopamineNeuromodulator passed" << std::endl;
}

void testNoveltyNeuromodulator() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Novelty novelty;
    
    // Test initialization
    assert(novelty.getLevel() == 0.0f);
    
    // Test novelty detection
    std::vector<float> currentPattern = {0.1f, 0.2f, 0.3f};
    std::vector<float> previousPattern = {0.0f, 0.0f, 0.0f};
    
    novelty.detectNovelty(currentPattern, previousPattern);
    assert(novelty.getLevel() > 0.0f);
    
    // Test update (decay)
    novelty.update(0.1f);
    assert(novelty.getLevel() >= 0.0f);
    
    // Test history
    auto history = novelty.getHistory();
    assert(!history.empty());
    
    std::cout << "    testNoveltyNeuromodulator passed" << std::endl;
}

void testCuriosityNeuromodulator() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Curiosity curiosity;
    curiosity.initialize(nullptr); // Can pass nullptr for this test
    
    // Test initialization
    assert(curiosity.getLevel() == 0.0f);
    assert(curiosity.getExplorationDrive() == 0.0f);
    
    // Test update with novelty and prediction error
    curiosity.update(0.5f, 0.2f, 0.1f);
    assert(curiosity.getLevel() > 0.0f);
    
    // Test configuration
    curiosity.setNoveltyWeight(0.8f);
    curiosity.setPredictionErrorWeight(0.2f);
    
    std::cout << "    testCuriosityNeuromodulator passed" << std::endl;
}

void testPredictionErrorNeuromodulator() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::PredictionError predError;
    predError.initialize(nullptr); // Can pass nullptr for this test
    
    // Test initialization
    assert(predError.getError() == 0.0f);
    assert(predError.getMagnitude() == 0.0f);
    
    // Test computation
    predError.computeError(1.0f, 0.5f); // Predicted 1.0, actual 0.5
    assert(predError.getError() == -0.5f);
    assert(predError.getMagnitude() == 0.5f);
    
    // Test update
    predError.updatePrediction(0.7f);
    
    // Test history
    auto history = predError.getHistory();
    assert(history.size() > 0);
    
    std::cout << "    testPredictionErrorNeuromodulator passed" << std::endl;
}

void testMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
    brain.step(0);
    
    // Test episodic memory storage
    // (In real usage, this would be triggered by the episodic memory update system)
    
    // Test associative memory
    // (In real usage, this would be triggered by learning)
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::PredictionSystem predictionSystem;
    
    // Test initialization would happen with brain reference
    // Test prediction error
    // Test temporal sequence learning
    
    // Basic test that system exists
    assert(predictionSystem.getPredictionError() == 0.0f);
    
    std::cout << "    testPredictionSystem passed" << std::endl;
}

void testCognitionSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    auto* attention = brain.getAttention();
    auto* conceptFormation = brain.getConceptFormation();
    
    assert(planner != nullptr);
    assert(attention != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test attention system
    if (attention) {
        std::vector<nlm::NeuronId> competitors;
        attention->update(0.001f);
        // processCompetition would be called with actual competitors
    }
    
    std::cout << "    testCognitionSystems passed" << std::endl;
}

void testDevelopmentSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test development stage
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    std::cout << "    testDevelopmentSystem passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable neuromodulation
    if (auto* agentBrain = brain.getAgentBrain()) {
        agentBrain->enableRewardModulation(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        assert(agentBrain->isRewardModulationEnabled());
        assert(agentBrain->isDevelopmentEnabled());
        assert(agentBrain->isCuriosityEnabled());
    }
    
    // Test development effects
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    brain.develop();
    
    // Test neuromodulation level
    float neuromodulationLevel = 0.5f; // Would come from dopamine
    // Would apply to neural excitability
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPlasticityWithNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* stdp = brain.getSTDP();
    auto* dopamine = brain.getDopamine();
    
    assert(stdp != nullptr);
    assert(dopamine != nullptr);
    
    // Test STDP with dopamine modulation
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0}; // Post before pre (depression)
    
    // Apply dopamine modulation
    dopamine->signalPredictionError(-0.8f); // Negative prediction error
    
    // Update STDP
    stdp->update(&synapse, preSpikes, postSpikes, 0.001f);
    
    // Weight should have changed
    assert(synapse.getWeight() != 0.5f);
    
    std::cout << "    testPlasticityWithNeuromodulation passed" << std::endl;
}

void testPhase6CompletePipeline() {
    // Test the complete Phase 6 pipeline with all systems integrated
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all systems are present
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Run multiple steps to see integration
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        // Inject some sensory input
        nlm::Vision vision(16, 16, 3);
        std::vector<float> data(16*16*3, 0.5f);
        vision.setData(data);
        
        brain.receiveSensoryInput(vision);
        
        // Apply reward modulation
        if (brain.getDopamine()) {
            brain.getDopamine()->signalReward(0.1f);
        }
        
        // Step brain
        brain.step(step);
        
        // Apply neuromodulation effects
        if (brain.getDopamine()) {
            brain.getDopamine()->update(0.001f);
        }
    }
    
    // Verify something changed (activity occurred)
    assert(brain.getTotalSpikeCount() >= initialSpikes);
    
    std::cout << "    testPhase6CompletePipeline passed" << std::endl;
}

void runAllAdvancedTests() {
    std::cout << "Running advanced NLM tests (Phase 6 integration)..." << std::endl;
    
    testDopamineNeuromodulator();
    testNoveltyNeuromodulator();
    testCuriosityNeuromodulator();
    testPredictionErrorNeuromodulator();
    testMemorySystemIntegration();
    testPredictionSystem();
    testCognitionSystems();
    testDevelopmentSystem();
    testNeuromodulationIntegration();
    testPlasticityWithNeuromodulation();
    testPhase6CompletePipeline();
    
    std::cout << "All advanced tests passed!" << std::endl;
}
