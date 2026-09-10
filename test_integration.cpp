int main(int argc, char* argv[]) {
    NLM_LOG_INFO("NLM - Neural Learning Machine (Phase 6: Final Integration)");
    NLM_LOG_INFO("Building the first artificial developmental brain...");

    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15f);

    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return 1;
    }

    // Create simple world
    SimpleWorld world;
    world.initialize(20, 20);
    world.setAgentStart(10.0f, 10.0f);

    // Create agent brain interface
    AgentBrain agent(brain);
    agent.initialize(world);

    // Enable learning systems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);

    NLM_LOG_INFO("Starting Phase 6 Integration Simulation");

    // Run simulation for 500 steps
    for (uint64_t step = 0; step < 500; ++step) {
        // Update world (simple movement simulation)
        world.update(0.1f);

        // Get sensory percept
        auto percept = world.getSensoryPercept();

        // Process sensory input
        agent.processSensoryInput(percept);

        // Brain step
        brain->step(step, step * 0.1f);

        // Get motor command
        auto action = agent.decodeMotorCommand();

        // Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime());

        // Apply reward modulation
        agent.applyRewardModulation(0.1f, 0.0f);

        // Update development
        agent.updateDevelopment(0.1f);

        // Log status every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO(std::to_string(step) + ": Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                ", Working memory traces: " + std::to_string(brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }

    NLM_LOG_INFO("=== Phase 6 Integration Complete ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));

    // Test save/load
    std::string checkpointPath = "/tmp/nlm_phase6_test.bin";
    NLM_LOG_INFO("Testing checkpoint save/load...");
    if (brain->save(checkpointPath)) {
        NLM_LOG_INFO("Checkpoint saved successfully");
        
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        
        if (brain2->load(checkpointPath)) {
            NLM_LOG_INFO("Checkpoint loaded successfully");
            NLM_LOG_INFO("Integration test PASSED");
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint");
        }
    } else {
        NLM_LOG_ERROR("Failed to save checkpoint");
    }

    return 0;
}
