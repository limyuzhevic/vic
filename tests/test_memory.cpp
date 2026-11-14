// Memory System Tests
void test_memory::testWorkingMemory() {
    std::cout << "    Testing Working Memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        throw std::runtime_error("Brain initialization failed");
    }
    
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory == nullptr) {
        throw std::runtime_error("Working memory is not integrated");
    }
    
    std::cout << "      Working memory initialized successfully" << std::endl;
}

void test_memory::testEpisodicMemory() {
    std::cout << "    Testing Episodic Memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory == nullptr) {
        throw std::runtime_error("Episodic memory is not integrated");
    }
    
    // Test basic episodic memory functionality
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.positionX = 0.5f;
    episode.positionY = 0.5f;
    episode.orientation = 1.0f;
    episode.reward = 0.1f;
    
    episodicMemory->storeEpisode(episode);
    
    if (episodicMemory->getEpisodeCount() != 1) {
        throw std::runtime_error("Episodic memory episode count mismatch");
    }
    
    std::cout << "      Episodic memory functional (1 episode stored)" << std::endl;
}

void test_memory::testAssociativeMemory() {
    std::cout << "    Testing Associative Memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory == nullptr) {
        std::cout << "      Associative memory not yet implemented (expected for Phase 6)" << std::endl;
        return;
    }
    
    associativeMemory->initialize(&brain);
    
    std::cout << "      Associative memory system present" << std::endl;
}

void test_memory::runAll() {
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    std::cout << "  Memory tests PASSED" << std::endl;
}