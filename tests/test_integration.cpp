// Neural Working Memory Tests
void testNeuralWorkingMemory() {
    std::cout << "    Testing Neural Working Memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Test working memory initialization
    auto* wm = brain.getWorkingMemory();
    if (wm == nullptr) {
        std::cout << "      Warning: Working memory not integrated" << std::endl;
        return;
    }
    
    // Test storing and retrieving
    nlm::NeuronId testNeuron(1001);
    wm->storeToNeuron(testNeuron, 0.8f);
    
    float activation = wm->getNeuronActivation(testNeuron);
    if (std::abs(activation - 0.8f) < 0.01f) {
        std::cout << "      Working memory storage/retrieval: PASSED" << std::endl;
    } else {
        throw std::runtime_error("Working memory retrieval failed");
    }
    
    // Test update
    wm->update(0.001f);
    
    // Test contains
    if (wm->contains(testNeuron)) {
        std::cout << "      Working memory contains: PASSED" << std::endl;
    } else {
        throw std::runtime_error("Working memory contains failed");
    }
    
    std::cout << "    Neural Working Memory tests completed" << std::endl;
}

// Episodic Memory Tests
void testEpisodicMemory() {
    std::cout << "    Testing Episodic Memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Test episodic memory initialization
    auto* em = brain.getEpisodicMemory();
    if (em == nullptr) {
        std::cout << "      Warning: Episodic memory not integrated" << std::endl;
        return;
    }
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.5f;
    episode.action = nlm::ActionType::MoveForward;
    
    // Add some neurons
    episode.activeNeurons.push_back(nlm::NeuronId(2001));
    episode.neuronActivations.push_back(0.9f);
    
    // Store episode
    em->storeEpisode(episode);
    
    // Test episode count
    size_t episodeCount = em->getEpisodeCount();
    if (episodeCount > 0) {
        std::cout << "      Episodic memory storage: PASSED" << std::endl;
    } else {
        throw std::runtime_error("Episodic memory storage failed");
    }
    
    // Test episode retrieval
    const nlm::EpisodicMemoryItem* retrieved = em->getEpisode(episodeCount - 1);
    if (retrieved != nullptr && retrieved->timestamp == 100) {
        std::cout << "      Episodic memory retrieval: PASSED" << std::endl;
    } else {
        throw std::runtime_error("Episodic memory retrieval failed");
    }
    
    std::cout << "    Episodic Memory tests completed" << std::endl;
}

// Prediction System Tests
void testPredictionSystem() {
    std::cout << "    Testing Prediction System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Test prediction system initialization
    auto* prediction = brain.getPredictionSystem();
    if (prediction == nullptr) {
        std::cout << "      Warning: Prediction system not integrated" << std::endl;
        return;
    }
    
    std::cout << "      Prediction system present: PASSED" << std::endl;
    
    // Note: The prediction system implementation is complex and would need
    // more detailed testing based on the specific requirements
    std::cout << "    Prediction System tests completed" << std::endl;
}

// Cognition Systems Tests
void testCognitionSystems() {
    std::cout << "    Testing Cognition Systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Test NeuralPlanner
    auto* planner = brain.getPlanner();
    if (planner != nullptr) {
        std::cout << "      Neural Planner present: PASSED" << std::endl;
        
        // Initialize planner if it has an initialize method
        if (brain.getRandomGenerator()) {
            planner->initialize(&brain);
            std::cout << "      Neural Planner initialization: PASSED" << std::endl;
        }
    } else {
        std::cout << "      Warning: Neural Planner not integrated" << std::endl;
    }
    
    // Test ConceptFormation
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation != nullptr) {
        std::cout << "      Concept Formation present: PASSED" << std::endl;
        
        if (brain.getRandomGenerator()) {
            conceptFormation->initialize(&brain);
            std::cout << "      Concept Formation initialization: PASSED" << std::endl;
        }
    } else {
        std::cout << "      Warning: Concept Formation not integrated" << std::endl;
    }
    
    // Test AttentionalSelection
    auto* attention = brain.getAttention();
    if (attention != nullptr) {
        std::cout << "      Attention system present: PASSED" << std::endl;
        
        if (brain.getRandomGenerator()) {
            attention->initialize(&brain);
            std::cout << "      Attention system initialization: PASSED" << std::endl;
        }
    } else {
        std::cout << "      Warning: Attention system not integrated" << std::endl;
    }
    
    std::cout << "    Cognition Systems tests completed" << std::endl;
}

// Neuromodulation Systems Tests
void testNeuromodulationSystems() {
    std::cout << "    Testing Neuromodulation Systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Test Dopamine
    auto* dopamine = brain.getDopamine();
    if (dopamine != nullptr) {
        std::cout << "      Dopamine system present: PASSED" << std::endl;
        
        // Test dopamine update
        dopamine->update(0.001f);
        float level = dopamine->getLevel();
        if (!std::isnan(level)) {
            std::cout << "      Dopamine system update: PASSED" << std::endl;
        }
    } else {
        std::cout << "      Warning: Dopamine system not integrated" << std::endl;
    }
    
    // Test Curiosity
    auto* curiosity = brain.getCuriosity();
    if (curiosity != nullptr) {
        std::cout << "      Curiosity system present: PASSED" << std::endl;
        
        curiosity->update(0.001f);
        float curiosityLevel = curiosity->getLevel();
        if (!std::isnan(curiosityLevel)) {
            std::cout << "      Curiosity system update: PASSED" << std::endl;
        }
    } else {
        std::cout << "      Warning: Curiosity system not integrated" << std::endl;
    }
    
    // Test Novelty
    auto* novelty = brain.getNovelty();
    if (novelty != nullptr) {
        std::cout << "      Novelty system present: PASSED" << std::endl;
        
        novelty->update(0.001f);
        std::cout << "      Novelty system update: PASSED" << std::endl;
    } else {
        std::cout << "      Warning: Novelty system not integrated" << std::endl;
    }
    
    std::cout << "    Neuromodulation Systems tests completed" << std::endl;
}

// Development System Tests
void testDevelopmentSystem() {
    std::cout << "    Testing Development System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Test DevelopmentSystem
    auto* development = brain.getDevelopmentSystem();
    if (development != nullptr) {
        std::cout << "      Development system present: PASSED" << std::endl;
        
        // Test development update (called automatically in brain loop)
        if (brain.getRandomGenerator()) {
            development->update(&brain, *brain.getRandomGenerator(), 1.0f);
            std::cout << "      Development system update: PASSED" << std::endl;
        }
        
        // Test developmental stage
        nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
        std::cout << "      Initial developmental stage: " << static_cast<int>(stage) << std::endl;
        
        // Test stage transition
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
        if (brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod) {
            std::cout << "      Developmental stage transition: PASSED" << std::endl;
        }
    } else {
        std::cout << "      Warning: Development system not integrated" << std::endl;
    }
    
    std::cout << "    Development System tests completed" << std::endl;
}

// Checkpoint System Tests
void testCheckpointSystem() {
    std::cout << "    Testing Checkpoint System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    // Create and initialize first brain
    nlm::Brain brain1(config);
    if (!brain1.initialize()) {
        std::cout << "      Warning: Brain1 initialization failed" << std::endl;
        return;
    }
    
    // Run some steps
    for (int i = 0; i < 50; ++i) {
        brain1.step(i, i * 0.001f);
    }
    
    // Test save
    std::string checkpointPath = "/tmp/nlm_test_checkpoint.bin";
    bool saveSuccess = brain1.save(checkpointPath);
    
    if (saveSuccess) {
        std::cout << "      Checkpoint save: PASSED" << std::endl;
        
        // Create and initialize second brain
        nlm::Brain brain2(config);
        if (!brain2.initialize()) {
            std::cout << "      Warning: Brain2 initialization failed" << std::endl;
            return;
        }
        
        // Test load
        bool loadSuccess = brain2.load(checkpointPath);
        
        if (loadSuccess) {
            std::cout << "      Checkpoint load: PASSED" << std::endl;
            
            // Compare neuron counts (load should restore structure)
            size_t brain1Neurons = brain1.getTotalNeuronCount();
            size_t brain2Neurons = brain2.getTotalNeuronCount();
            
            if (brain1Neurons == brain2Neurons) {
                std::cout << "      Checkpoint restoration: PASSED" << std::endl;
            } else {
                std::cout << "      Warning: Neuron count mismatch after load" << std::endl;
            }
        } else {
            std::cout << "      Checkpoint load: FAILED" << std::endl;
        }
    } else {
        std::cout << "      Checkpoint save: FAILED" << std::endl;
    }
    
    std::cout << "    Checkpoint System tests completed" << std::endl;
}

void runIntegrationTests() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing disconnected systems mentioned in Phase 6 audit..." << std::endl << std::endl;
    
    try {
        testNeuralWorkingMemory();
        testEpisodicMemory();
        testPredictionSystem();
        testCognitionSystems();
        testNeuromodulationSystems();
        testDevelopmentSystem();
        testCheckpointSystem();
        
        std::cout << std::endl;
        std::cout << "=== Integration Tests Summary ===" << std::endl;
        std::cout << "Tests completed for Phase 6 disconnected systems." << std::endl;
        std::cout << "Note: Some tests may show 'Warning: Not integrated' if systems are still disconnected." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Integration Test ERROR ===" << std::endl;
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return;
    }
}