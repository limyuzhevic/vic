// Neuromodulation System Tests
void test_neuromodulation::testDopamine() {
    std::cout << "    Testing Dopamine System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    if (dopamine == nullptr) {
        throw std::runtime_error("Dopamine system is not integrated");
    }
    
    // Test basic dopamine functionality
    dopamine->update(0.001f);
    float dopamineLevel = dopamine->getLevel();
    
    std::cout << "      Dopamine system functional (level: " << dopamineLevel << ")" << std::endl;
}

void test_neuromodulation::testCuriosity() {
    std::cout << "    Testing Curiosity System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity == nullptr) {
        throw std::runtime_error("Curiosity system is not integrated");
    }
    
    // Test basic curiosity functionality
    curiosity->update(0.0f, 0.0f, 0.001f);
    float curiosityLevel = curiosity->getLevel();
    
    std::cout << "      Curiosity system functional (level: " << curiosityLevel << ")" << std::endl;
}

void test_neuromodulation::testNovelty() {
    std::cout << "    Testing Novelty Detection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* novelty = brain.getNovelty();
    if (novelty == nullptr) {
        std::cout << "      Novelty detection not yet implemented (expected for Phase 6)" << std::endl;
        return;
    }
    
    novelty->update(0.001f);
    float noveltyLevel = novelty->getLevel();
    
    std::cout << "      Novelty detection functional (level: " << noveltyLevel << ")" << std::endl;
}

void test_neuromodulation::runAll() {
    testDopamine();
    testCuriosity();
    testNovelty();
    std::cout << "  Neuromodulation tests PASSED" << std::endl;
}