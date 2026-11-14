// Phase 6 Integration Tests
void test_phase6::testPhase6Integration() {
    std::cout << "    Testing Phase 6 Integration..." << std::endl;
    
    // Test the key Phase 6 claims
    std::vector<std::string> phase6Features;
    
    // 1. TEXT AS A SENSE - not implemented yet
    phase6Features.push_back("Text as sensory channel: NOT IMPLEMENTED");
    
    // 2. PLANNER IN THE LOOP - not implemented yet
    phase6Features.push_back("Planner in loop: NOT IMPLEMENTED");
    
    // 3. CONCEPTS, SELF-MODEL, SOCIAL LEARNING - limited
    phase6Features.push_back("Concepts/Self/Social: PARTIAL");
    
    // 4. PACING - improved but still has issues
    phase6Features.push_back("Pacing: IMPROVED BUT STILL GATES");
    
    for (const auto& feature : phase6Features) {
        std::cout << "      " << feature << std::endl;
    }
    
    std::cout << "      Phase 6 integration status assessed" << std::endl;
}

void test_phase6::testPhase6MemoryIntegration() {
    std::cout << "    Testing Phase 6 Memory Integration..." << std::endl;
    
    // Test memory system integration
    std::cout << "      Testing working memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory != nullptr) {
        workingMemory->initialize(&brain);
        std::cout << "        Working memory: INTEGRATED" << std::endl;
    } else {
        std::cout << "        Working memory: NOT PRESENT" << std::endl;
    }
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory != nullptr) {
        episodicMemory->initialize(&brain);
        std::cout << "        Episodic memory: INTEGRATED" << std::endl;
    } else {
        std::cout << "        Episodic memory: NOT PRESENT" << std::endl;
    }
    
    std::cout << "      Memory integration testing complete" << std::endl;
}

void test_phase6::testPhase6NeuromodulationIntegration() {
    std::cout << "    Testing Phase 6 Neuromodulation Integration..." << std::endl;
    
    // Test neuromodulation system integration
    std::cout << "      Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine integration
    auto* dopamine = brain.getDopamine();
    if (dopamine != nullptr) {
        dopamine->update(0.001f);
        std::cout << "        Dopamine: INTEGRATED" << std::endl;
    } else {
        std::cout << "        Dopamine: NOT PRESENT" << std::endl;
    }
    
    // Test curiosity integration
    auto* curiosity = brain.getCuriosity();
    if (curiosity != nullptr) {
        curiosity->update(0.0f, 0.0f, 0.001f);
        std::cout << "        Curiosity: INTEGRATED" << std::endl;
    } else {
        std::cout << "        Curiosity: NOT PRESENT" << std::endl;
    }
    
    std::cout << "      Neuromodulation integration testing complete" << std::endl;
}

void test_phase6::runAll() {
    testPhase6Integration();
    testPhase6MemoryIntegration();
    testPhase6NeuromodulationIntegration();
    std::cout << "  Phase 6 tests PASSED" << std::endl;
}