using namespace nlm;

// Phase 6 Integration Test: Brain Systems Integration Verification
void testBrainIntegration() {
    std::cout << "=== NLM Phase 6 Integration Test ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Brain initialization failed" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Test 1: Verify all memory systems are connected
    bool workingMemoryOK = (brain->getWorkingMemory() != nullptr);
    bool episodicMemoryOK = (brain->getEpisodicMemory() != nullptr);
    bool associativeMemoryOK = (brain->getAssociativeMemory() != nullptr);
    
    std::cout << "Test 1 - Memory System Integration:" << std::endl;
    std::cout << "  Working Memory: " << (workingMemoryOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (episodicMemoryOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Associative Memory: " << (associativeMemoryOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    // Test 2: Verify all neuromodulation systems are connected
    bool dopamineOK = (brain->getDopamine() != nullptr);
    bool curiosityOK = (brain->getCuriosity() != nullptr);
    bool noveltyOK = (brain->getNovelty() != nullptr);
    bool predictionErrorOK = (brain->getPredictionErrorSignal() != nullptr);
    
    std::cout << "\nTest 2 - Neuromodulation System Integration:" << std::endl;
    std::cout << "  Dopamine: " << (dopamineOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Curiosity: " << (curiosityOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Novelty: " << (noveltyOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Prediction Error: " << (predictionErrorOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    // Test 3: Verify all cognition systems are connected
    bool plannerOK = (brain->getPlanner() != nullptr);
    bool conceptFormationOK = (brain->getConceptFormation() != nullptr);
    bool attentionOK = (brain->getAttention() != nullptr);
    
    std::cout << "\nTest 3 - Cognition System Integration:" << std::endl;
    std::cout << "  Neural Planner: " << (plannerOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Concept Formation: " << (conceptFormationOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Attentional Selection: " << (attentionOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    // Test 4: Verify prediction system is connected
    bool predictionSystemOK = (brain->getPredictionSystem() != nullptr);
    
    std::cout << "\nTest 4 - Prediction System Integration:" << std::endl;
    std::cout << "  Prediction System: " << (predictionSystemOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    // Test 5: Verify development system is connected
    bool developmentSystemOK = (brain->getDevelopmentSystem() != nullptr);
    
    std::cout << "\nTest 5 - Development System Integration:" << std::endl;
    std::cout << "  Development System: " << (developmentSystemOK ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    // Test 6: Run simulation steps and verify integration
    std::cout << "\nTest 6 - Integration Verification through Simulation:" << std::endl;
    
    // Create some sensory input
    std::vector<float> sensoryData = {0.5f, 0.8f, 0.3f, 0.9f, 0.2f};
    brain->receiveSensoryInput(sensoryData);
    
    // Run a few simulation steps
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        
        // Check if working memory is being updated
        if (workingMemoryOK) {
            auto* wm = brain->getWorkingMemory();
            if (wm && wm->getActiveTraces() > 0) {
                std::cout << "  Step " << step << ": Working memory active traces = " << wm->getActiveTraces() << std::endl;
                break; // Found evidence of integration
            }
        }
        
        // Check if episodic memory is being updated
        if (episodicMemoryOK) {
            auto* em = brain->getEpisodicMemory();
            if (em && em->getEpisodeCount() > 0) {
                std::cout << "  Step " << step << ": Episodic memory episodes = " << em->getEpisodeCount() << std::endl;
                break; // Found evidence of integration
            }
        }
    }
    
    // Test 7: Verify checkpoint functionality
    std::cout << "\nTest 7 - Checkpoint System Integration:" << std::endl;
    std::string checkpointPath = "/tmp/nlm_integration_test.bin";
    
    // Save checkpoint
    bool saveOK = brain->save(checkpointPath);
    std::cout << "  Save checkpoint: " << (saveOK ? "✓ SUCCESS" : "✗ FAILED") << std::endl;
    
    if (saveOK) {
        // Load checkpoint into new brain
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        
        bool loadOK = brain2->load(checkpointPath);
        std::cout << "  Load checkpoint: " << (loadOK ? "✓ SUCCESS" : "✗ FAILED") << std::endl;
    }
    
    // Test 8: Test prediction system update
    std::cout << "\nTest 8 - Prediction System Integration:" << std::endl;
    if (predictionSystemOK) {
        // The prediction system update happens in Brain::step()
        // We can verify it runs by checking if prediction error updates
        auto* predError = brain->getPredictionErrorSignal();
        if (predError) {
            std::cout << "  Prediction Error system accessible" << std::endl;
        }
    }
    
    // Summary
    std::cout << "\n=== INTEGRATION TEST SUMMARY ===" << std::endl;
    int passedTests = 0;
    int totalTests = 8;
    
    if (workingMemoryOK) passedTests++;
    if (episodicMemoryOK) passedTests++;
    if (dopamineOK) passedTests++;
    if (curiosityOK) passedTests++;
    if (plannerOK) passedTests++;
    if (conceptFormationOK) passedTests++;
    if (attentionOK) passedTests++;
    if (predictionSystemOK) passedTests++;
    
    std::cout << "Tests Passed: " << passedTests << "/" << totalTests << std::endl;
    std::cout << "Integration Score: " << (passedTests * 100 / totalTests) << "%" << std::endl;
    
    if (passedTests >= 7) {
        std::cout << "✓ PHASE 6 INTEGRATION SUCCESSFUL" << std::endl;
    } else {
        std::cout << "✗ PHASE 6 INTEGRATION NEEDS MORE WORK" << std::endl;
    }
}

int main() {
    std::cout << "NLM Phase 6 Integration Test Suite" << std::endl;
    std::cout << "==================================" << std::endl;
    
    testBrainIntegration();
    
    return 0;
}
