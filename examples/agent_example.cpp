// agent_example.cpp
// Complete agent with world interaction example
// Demonstrates how NLM agents interact with their environment

#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/neuromodulation/Neuromodulator.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <iostream>

// AgentBrain: The interface between brain and world
class AgentBrain {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    bool rewardModulationEnabled;
    bool curiosityEnabled;
    bool structuralPlasticityEnabled;
    bool developmentEnabled;
    
public:
    AgentBrain(std::shared_ptr<Brain> b) : brain(b), rewardModulationEnabled(false),
                                           curiosityEnabled(false), structuralPlasticityEnabled(false),
                                           developmentEnabled(false) {}
    
    void initialize(std::shared_ptr<SimpleWorld> w) {
        world = w;
        std::cout << "AgentBrain initialized with world" << std::endl;
    }
    
    void processSensoryInput(const std::vector<float>& percept) {
        // Convert world percept to brain input
        brain->receiveSensoryInput(percept);
    }
    
    Action decodeMotorCommand() {
        // Decode brain's motor command
        return brain->produceAction();
    }
    
    void applyRewardModulation(float reward, float predicted) {
        if (rewardModulationEnabled) {
            // Apply reward-based learning
            // This would interface with neuromodulators in full implementation
            std::cout << "  Reward modulation: " << reward << " (predicted: " << predicted << ")" << std::endl;
        }
    }
    
    void updateDevelopment(double dt) {
        if (developmentEnabled) {
            // Update developmental stage
            brain->updateDevelopmentalStage(dt);
        }
    }
    
    void enableRewardModulation(bool enable) { rewardModulationEnabled = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled = enable; }
    void enableDevelopment(bool enable) { developmentEnabled = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled = enable; }
    
    float getCuriosityLevel() const { return curiosityEnabled ? 0.5f : 0.0f; }
    float getNoveltyLevel() const { return noveltyEnabled ? 0.3f : 0.0f; }
    
private:
    bool noveltyEnabled = false;
};

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Agent Example ===" << std::endl;
    
    // Create configuration optimized for agent tasks
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 500, ConfigSource::Default);
    
    // Enable learning features for agent
    config->set("plasticity.stdp.enable", True, ConfigSource::File);
    config->set("plasticity.hebbian.enable", True, ConfigSource::File);
    config->set("neuromod.curiosity.enable", True, ConfigSource::File);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    std::cout << "Creating brain with " << brain->getTotalNeuronCount() << " neurons..." << std::endl;
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create and configure world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    );
    world->reset();
    
    std::cout << "World configured: 20x20 cells with 8x8 vision" << std::endl;
    
    // Create agent interface
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    // Enable all learning capabilities for agent
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    std::cout << "Enabling learning features: Reward modulation, Curiosity, Development" << std::endl;
    
    // Run simulation loop
    std::cout << "\nRunning simulation loop..." << std::endl;
    std::cout << "Step | Firing | Curiosity | Novelty | Dev Stage | Action" << std::endl;
    std::cout << "-----|--------|----------|---------|-----------|-------" << std::endl;
    
    for (SimulationStep step = 0; step < 100; ++step) {
        // 1. Update world
        world->update(0.1);  // 0.1 second timestep
        
        // 2. Get what the agent sees
        auto percept = world->getSensoryPercept();
        
        // 3. Tell the brain
        agent->processSensoryInput(percept);
        
        // 4. Brain thinks
        brain->step(step);
        
        // 5. Get action from brain
        auto action = agent->decodeMotorCommand();
        
        // 6. Do action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // 7. Learn from results (simplified)
        auto currentPercept = world->getSensoryPercept();
        float reward = currentPercept.getInternal()[0] if currentPercept.getInternal() else 0.0;
        agent->applyRewardModulation(reward, 0.0);
        
        // 8. Update development
        agent->updateDevelopment(0.1);
        
        // Print progress every 10 steps
        if (step % 10 == 0) {
            std::cout << step << " | " << brain->getFiringNeuronCount() << " | "
                      << agent->getCuriosityLevel() << " | " << agent->getNoveltyLevel() << " | "
                      << brain->getDevelopmentalStage() << " | "
                      << action.toString() << std::endl;
        }
    }
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Final brain state:" << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Final developmental stage: " << brain->getDevelopmentalStage() << std::endl;
    
    return 0;
}
