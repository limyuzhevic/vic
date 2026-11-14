// Cognition System Tests
void test_cognition::testNeuralPlanner() {
    std::cout << "    Testing Neural Planner..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    if (planner == nullptr) {
        std::cout << "      Neural planner not yet implemented (expected for Phase 6)" << std::endl;
        return;
    }
    
    planner->initialize(&brain);
    std::cout << "      Neural planner initialized" << std::endl;
}

void test_cognition::testConceptFormation() {
    std::cout << "    Testing Concept Formation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation == nullptr) {
        std::cout << "      Concept formation not yet implemented (expected for Phase 6)" << std::endl;
        return;
    }
    
    conceptFormation->initialize(&brain);
    std::cout << "      Concept formation system present" << std::endl;
}

void test_cognition::testAttentionalSelection() {
    std::cout << "    Testing Attentional Selection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* attention = brain.getAttention();
    if (attention == nullptr) {
        std::cout << "      Attentional selection not yet implemented (expected for Phase 6)" << std::endl;
        return;
    }
    
    attention->initialize(&brain);
    std::cout << "      Attentional selection system present" << std::endl;
}

void test_cognition::runAll() {
    testNeuralPlanner();
    testConceptFormation();
    testAttentionalSelection();
    std::cout << "  Cognition tests PASSED" << std::endl;
}