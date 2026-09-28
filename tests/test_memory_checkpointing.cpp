// Test memory system checkpointing
void testMemoryCheckpointing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps to populate memory systems
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Save the brain state
    std::string savePath = "./test_checkpoint.nckpt";
    bool saveSuccess = brain.save(savePath);
    assert(saveSuccess);
    
    // Reset brain to empty state
    brain.reset();
    
    // Load the brain state back
    bool loadSuccess = brain.load(savePath);
    assert(loadSuccess);
    
    // Verify the brain still has neurons
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 1);
    
    // Clean up
    std::remove(savePath.c_str());
    
    std::cout << "    testMemoryCheckpointing passed" << std::endl;
}