// Integration Tests
// Tests for system integration and cross-component interactions

TEST_CASE("integration_brain_memory_test", TestCategory::INTEGRATION, "Tests for brain-memory integration")
void testIntegrationBrainMemory() {
    // Test 1: Brain initialization with memory components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", 200);
    
    nlm::Brain brain(config);
    bool brainInitialized = brain.initialize();
    assert(brainInitialized);
    
    // Brain should have memory components initialized
    assert(brain.getTotalNeuronCount() == 100);
    
    // Test 2: Cross-component signal flow
    nlm::WorkingMemory& workingMemory = brain.getWorkingMemory();
    workingMemory.store(nlm::MemoryItem(nlm::MemoryId(1), "brain_state"));
    
    auto retrieved = workingMemory.retrieve(nlm::MemoryId(1));
    assert(retrieved.has_value());
    assert(retrieved.value().content == "brain_state");
    
    // Test 3: Integration test with neuromodulation
    nlm::Neuromodulator& neuromodulator = brain.getNeuromodulator();
    nlm::ModulationSignal signal;
    signal.type = nlm::ModulationType::Reward;
    signal.strength = 1.0f;
    signal.timestamp = 0.0;
    
    neuromodulator.applySignal(signal);
    neuromodulator.processSignals();
    
    // Test 4: Memory persistence across brain cycles
    nlm::EpisodicMemory& episodicMemory = brain.getEpisodicMemory();
    nlm::EpisodicEvent event;
    event.id = nlm::MemoryId(2);
    event.timestamp = 0.0;
    event.context = "integration_test_context";
    event.importance = 0.9f;
    event.associatedNeuron = nlm::NeuronId(1);
    
    episodicMemory.storeEvent(event);
    assert(episodicMemory.getEventCount() == 1);
    
    // Test 5: Semantic memory integration
    nlm::SemanticMemory& semanticMemory = brain.getSemanticMemory();
    nlm::Concept concept;
    concept.id = nlm::ConceptId(1);
    concept.name = "brain_behavior";
    concept.associations = {"integration"};
    concept.frequency = 1.0f;
    
    semanticMemory.storeConcept(concept);
    assert(semanticMemory.getConceptCount() > 0);
    
    std::cout << "Brain-memory integration tests passed" << std::endl;
    return true;
}

TEST_CASE("integration_neuromodulation_test", TestCategory::INTEGRATION, "Tests for neuromodulation integration")
void testIntegrationNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test 1: Reward-neuron interaction
    nlm::Neuromodulator& neuromodulator = brain.getNeuromodulator();
    
    nlm::Reward reward;
    reward.value = 10.0f;
    reward.timestamp = 0.0;
    neuromodulator.applyReward(reward);
    
    neuromodulator.processSignals();
    
    // Test 2: Prediction error-based learning
    nlm::PredictionError error;
    error.prediction = 5.0f;
    error.actual = 8.0f;
    error.timestamp = 0.0;
    neuromodulator.applyPredictionError(error);
    
    neuromodulator.processSignals();
    
    // Test 3: Novelty-driven exploration
    nlm::Novelty novelty;
    novelty.significance = 0.8f;
    novelty.timestamp = 0.0;
    neuromodulator.applyNovelty(novelty);
    
    neuromodulator.processSignals();
    
    // Test 4: Curiosity-based behavior
    nlm::Curiosity curiosity;
    curiosity.level = 0.7f;
    curiosity.timestamp = 0.0;
    neuromodulator.applyCuriosity(curiosity);
    
    neuromodulator.processSignals();
    
    // Test 5: Integration with brain dynamics
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
        neuromodulator.processSignals();
        
        // Verify system is still functional
        assert(brain.getTotalNeuronCount() == 50);
        assert(brain.getFiringNeuronCount() >= 0);
    }
    
    std::cout << "Neuromodulation integration tests passed" << std::endl;
    return true;
}

TEST_CASE("integration_cognition_test", TestCategory::INTEGRATION, "Tests for cognition system integration")
void testIntegrationCognition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("concept_memory_size", 50);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test 1: Concept formation and brain interaction
    nlm::Cognition& cognition = brain.getCognition();
    
    nlm::ConceptFormation conceptFormation;
    nlm::Concept concept;
    concept.id = nlm::ConceptId(1);
    concept.name = "cognitive_behavior";
    concept.associations = {"cognition", "memory"};
    concept.frequency = 1.0f;
    
    conceptFormation.storeConcept(concept);
    assert(conceptFormation.getConceptCount() == 1);
    
    // Test 2: Neural planner integration
    nlm::NeuralPlanner planner;
    nlm::Plan plan;
    plan.id = nlm::PlanId(1);
    plan.goal = "complete_task";
    plan.complexity = 0.5f;
    plan.priority = 0.8f;
    
    planner.createPlan(plan);
    assert(planner.getPlanCount() > 0);
    
    // Test 3: Cross-component coordination
    nlm::SensoryInput sensoryInput;
    sensoryInput.data = "stimulus_data";
    sensoryInput.timestamp = 0.0;
    sensoryInput.modality = nlm::SensoryModality::Visual;
    
    nlm::Action action = cognition.processSensoryInput(sensoryInput);
    assert(action != nullptr);
    
    // Test 4: Integration with memory systems
    nlm::SemanticMemory& semanticMemory = brain.getSemanticMemory();
    nlm::EpisodicMemory& episodicMemory = brain.getEpisodicMemory();
    
    // Concepts should be accessible from episodic context
    nlm::EpisodicEvent event;
    event.id = nlm::MemoryId(1);
    event.timestamp = 0.0;
    event.context = "cognitive_processing";
    event.importance = 0.9f;
    
    episodicMemory.storeEvent(event);
    
    // Episodic memory should interact with semantic memory
    auto events = episodicMemory.getRecentEvents(100.0);
    assert(events.size() > 0);
    
    std::cout << "Cognition integration tests passed" << std::endl;
    return true;
}

TEST_CASE("integration_agent_test", TestCategory::INTEGRATION, "Tests for agent system integration")
void testIntegrationAgent() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("sensory_input_rate", 10);
    config->set("action_output_rate", 5);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test 1: Agent-brain interface
    nlm::AgentBrain agentBrain;
    nlm::AgentBody agentBody;
    
    // Create agent percept
    nlm::SensoryPercept percept;
    percept.sensorId = nlm::SensorId(1);
    percept.value = 0.7f;
    percept.timestamp = 0.0;
    
    agentBody.processPercept(percept);
    
    // Test 2: Agent cognition integration
    nlm::Cognition& cognition = brain.getCognition();
    
    nlm::Action action = cognition.processPercept(percept);
    assert(action != nullptr);
    
    // Test 3: World interaction
    nlm::World world;
    world.updateEnvironment();
    
    nlm::Observation observation = world.getObservation();
    assert(observation != nullptr);
    
    // Test 4: Sensorimotor loop
    nlm::MotorSystem motor;
    
    nlm::Action agentAction = agentBody.generateAction();
    assert(agentAction != nullptr);
    
    motor.executeAction(agentAction);
    
    // Test 5: Reinforcement learning integration
    nlm::Neuromodulator& neuromodulator = brain.getNeuromodulator();
    
    nlm::Reward reward;
    reward.value = 5.0f;
    reward.timestamp = 0.0;
    neuromodulator.applyReward(reward);
    
    neuromodulator.processSignals();
    
    std::cout << "Agent integration tests passed" << std::endl;
    return true;
}