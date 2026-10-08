// Additional high-level convenience functions for NLM Python API
// These functions make it easier for users to get started with NLM

namespace nlm {

// High-level factory functions for common use cases

// Create a simple brain with default configuration
std::shared_ptr<Brain> createSimpleBrain(size_t neuronCount = 1000) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neuronCount);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

// Create a default configuration with sensible values
std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    // Set sensible defaults for common use cases
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42u);
    return config;
}

// Create a simple world with default settings
std::shared_ptr<SimpleWorld> createQuickWorld(size_t width = 20, 
                                              size_t height = 20,
                                              size_t visionWidth = 8,
                                              size_t visionHeight = 8) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, visionWidth, visionHeight);
    return world;
}

// Run a quick simulation with default settings
std::tuple<size_t, size_t, double> runQuickSimulation(size_t numSteps = 100) {
    // Quick one-step simulation using high-level API
    auto config = createDefaultConfig();
    auto brain = createSimpleBrain(500);
    auto world = createQuickWorld();
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    for (size_t step = 0; step < numSteps; ++step) {
        world->update(0.1);
        agentBrain.processSensoryInput(world->getSensoryPercept());
        brain->step(step);
        auto cmd = agentBrain.decodeMotorCommand();
        world->applyMotorCommand(cmd, world->getSimulationTime());
    }
    
    return std::make_tuple(brain->getTotalNeuronCount(), 
                          brain->getTotalSynapseCount(),
                          brain->getAverageFiringRate());
}

// Get preset configurations for common use cases
std::shared_ptr<Config> getPresetConfig(const std::string& presetName) {
    auto config = std::make_shared<Config>();
    
    if (presetName == "fast") {
        // Fast, small brain for real-time applications
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.05f);
        config->set("simulation_timestep", 0.0001);
        config->set("random_seed", 42u);
    } else if (presetName == "accurate") {
        // Large, accurate brain for research
        config->set("neuron_count", 5000);
        config->set("region_count", 4);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 123u);
    } else if (presetName == "balanced") {
        // Balanced configuration
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42u);
    } else {
        throw std::invalid_argument("Unknown preset name: " + presetName);
    }
    
    return config;
}

// Educational examples for getting started
std::string getEducationalExampleCode() {
    return R"example(
# Simple Brain Simulation Example
import pynlm

# Method 1: Using simple factory functions
brain = pynlm.createSimpleBrain(1000)
world = pynlm.createQuickWorld()
agent = pynlm.createAgentSetup(brain, world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Method 2: Using configuration presets
config = pynlm.getPresetConfig("balanced")
brain2 = pynlm.createBrain(config)

print(f"Brain created: {brain2.getTotalNeuronCount()} neurons")

example
        ";
}

// Benchmark brain performance
pybind11::dict benchmarkSetup(int numNeurons = 1000, int numSteps = 1000) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", numNeurons);
    config->set("region_count", std::max(1, numNeurons / 500));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int step = 0; step < numSteps; ++step) {
        brain->step(step);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration<double>(end - start).count();
    double stepsPerSecond = numSteps / duration;
    
    pybind11::dict results;
    results["neurons"] = numNeurons;
    results["steps"] = numSteps;
    results["duration_seconds"] = duration;
    results["steps_per_second"] = stepsPerSecond;
    results["spikes_per_second"] = brain->getTotalSpikes() / duration;
    
    return results;
}

} // namespace nlm
