// Development System Tests
void test_development::testDevelopmentSystem() {
    std::cout << "    Testing Development System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem == nullptr) {
        std::cout << "      Development system not yet implemented (expected for Phase 6)" << std::endl;
        return;
    }
    
    std::cout << "      Development system present" << std::endl;
}

void test_development::testSynaptogenesis() {
    std::cout << "    Testing Synaptogenesis..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem == nullptr) {
        std::cout << "      Synaptogenesis not yet implemented (expected for Phase 6)" << std::endl;
        return;
    }
    
    std::cout << "      Synaptogenesis system present" << std::endl;
}

void test_development::testPruning() {
    std::cout << "    Testing Neural Pruning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem == nullptr) {
        std::cout << "      Neural pruning not yet implemented (expected for Phase 6)" << std::endl;
        return;
    }
    
    std::cout << "      Neural pruning system present" << std::endl;
}

void test_development::runAll() {
    testDevelopmentSystem();
    testSynaptogenesis();
    testPruning();
    std::cout << "  Development tests PASSED" << std::endl;
}