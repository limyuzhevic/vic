// exploration_example.cpp
// Curiosity-driven exploration example
// Demonstrates how agents explore and discover new information

#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/neuromodulation/Neuromodulator.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <iostream>
#include <random>

// ExplorationAgent: Agent focused on curiosity-driven exploration
class ExplorationAgent {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    float curiosityLevel;
    float explorationBias;
    std::vector<std::pair<float, float>> exploredPositions;
    
public:
    ExplorationAgent(std::shared_ptr<Brain> b) : brain(b), curiosityLevel(0.0f), 
                                                 explorationBias(0.5f) {}
    
    void initialize(std::shared_ptr<SimpleWorld> w) {
        world = w;
        exploredPositions.clear();
        // Add starting position
        auto startPos = world->getAgentBody().getPosition();
        exploredPositions.push_back({startPos[0], startPos[1]});
        std::cout << "ExplorationAgent initialized" << std::endl;
    }
    
    void processSensoryInput(const std::vector<float>& percept) {
        brain->receiveSensoryInput(percept);
    }
    
    Action decodeMotorCommand() {
        Action action = brain->produceAction();
        
        // Apply exploration bias if curiosity is high
        if (curiosityLevel > 0.7f) {
            // Increase likelihood of novel actions
            action = addExplorationNoise(action);
        }
        
        return action;
    }
    
    void updateCuriosity(float novelty) {
        // Update curiosity level based on novelty
        curiosityLevel += novelty * 0.1f;
        curiosityLevel = std::min(curiosityLevel, 1.0f);
        
        // Track explored positions
        auto agentPos = world->getAgentBody().getPosition();
        bool alreadyExplored = false;
        
        for (const auto& pos : exploredPositions) {
            if (std::abs(pos.first - agentPos[0]) < 1.0f && 
                std::abs(pos.second - agentPos[1]) < 1.0f) {
                alreadyExplored = true;
                break;
            }
        }
        
        if (!alreadyExplored) {
            exploredPositions.push_back({agentPos[0], agentPos[1]});
            std::cout << "  Discovered new location: (" 
                      << agentPos[0] << ", " << agentPos[1] << ")" << std::endl;
        }
    }
    
    float getCuriosityLevel() const { return curiosityLevel; }
    int getExplorationProgress() const { return exploredPositions.size(); }
    
private:
    Action addExplorationNoise(const Action& original) {
        // Simple exploration strategy: sometimes move randomly
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        if (dist(gen) < explorationBias) {
            // Choose random action
            std::vector<Action> possibleActions = {
                Action(ActionType::MoveForward),
                Action(ActionType::TurnLeft),
                Action(ActionType::TurnRight),
                Action(ActionType::Interact)
            };
            return possibleActions[std::rand() % possibleActions.size()];
        }
        
        return original;
    }
};

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Curiosity-Driven Exploration Example ===" << std::endl;
    
    // Create configuration optimized for exploration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 12345, ConfigSource::Default);
    config->set("neuron_count", 800, ConfigSource::Default);
    
    // Enable curiosity and novelty systems
    config->set("neuromod.curiosity.enable", True, ConfigSource::File);
    config->set("neuromod.novelty.enable", True, ConfigSource::File);
    config->set("plasticity.stdp.enable", True, ConfigSource::File);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    std::cout << "Creating exploration brain with " << brain->getTotalNeuronCount() 
              << " neurons..." << std::endl;
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create and configure world for exploration
    auto world = std::make_shared<SimpleWorld>();
    world->configure(
        width=40,
        height=40,
        visionWidth=12,
        visionHeight=12
    );
    world->reset();
    world->setAgentStart(20.0, 20.0);  // Center start position
    
    // Add interesting objects for exploration
    std::vector<std::pair<float, float>> objectPositions = {
        {10.0, 10.0}, {30.0, 10.0}, {10.0, 30.0}, {30.0, 30.0},
        {5.0, 5.0}, {35.0, 5.0}, {5.0, 35.0}, {35.0, 35.0}
    };
    
    for (size_t i = 0; i < objectPositions.size(); ++i) {
        // Alternate between resources and hazards
        auto objType = (i % 2 == 0) ? 
            pynlm.WorldObjectType.Resource : pynlm.WorldObjectType.Hazard;
        float value = (objType == pynlm.WorldObjectType.Resource) ? 10.0f : 5.0f;
        
        world->addObject(pynlm.WorldObject(
            objectPositions[i].first,
            objectPositions[i].second,
            objType,
            value
        ));
    }
    
    std::cout << "World configured for exploration: 40x40 cells with 12x12 vision" << std::endl;
    std::cout << "Objects placed for exploration: Resources and hazards scattered across map" << std::endl;
    
    // Create exploration agent
    auto agent = std::make_shared<ExplorationAgent>(brain);
    agent->initialize(world);
    
    // Enable exploration features
    // (In full implementation, would connect to brain neuromodulation)
    
    // Run exploration episode
    std::cout << "\nStarting curiosity-driven exploration..." << std::endl;
    std::cout << "Step | Firing | Curiosity | Novelty | Explored | Action" << std::endl;
    std::cout << "-----|--------|----------|---------|----------|-------" << std::endl;
    
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Update world
        world->update(0.05);  // 50ms timesteps for finer control
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step);
        
        // Get action
        Action action = agent->decodeMotorCommand();
        
        // Execute action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Update curiosity based on environment (simplified)
        auto currentPercept = world->getSensoryPercept();
        float novelty = 0.1f;  // Simplified novelty detection
        agent->updateCuriosity(novelty);
        
        // Print progress every 100 steps
        if (step % 100 == 0) {
            auto agentPos = world->getAgentBody().getPosition();
            std::cout << step << " | " << brain->getFiringNeuronCount() << " | "
                      << agent->getCuriosityLevel() << " | " << novelty << " | "
                      << agent->getExplorationProgress() << " | "
                      << action.toString() << std::endl;
        }
        
        // Check for completion condition
        if (agent->getExplorationProgress() >= 20 && step > 500) {
            std::cout << "\nExploration completed! Discovered " 
                      << agent->getExplorationProgress() << " locations." << std::endl;
            break;
        }
    }
    
    std::cout << "\n=== Exploration Complete ===" << std::endl;
    std::cout << "Exploration Summary:" << std::endl;
    std::cout << "  Total steps: " << (agent->getExplorationProgress() >= 20 ? "completed early" : "completed full") << std::endl;
    std::cout << "  Locations discovered: " << agent->getExplorationProgress() << std::endl;
    std::cout << "  Final curiosity level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    std::cout << "\nExploration strategies demonstrated:" << std::endl;
    std::cout << "  - Curiosity-driven exploration" << std::endl;
    std::cout << "  - Novelty detection" << std::endl;
    std::cout << "  - Environment discovery" << std::endl;
    std::cout << "  - Random exploration when curiosity is high" << std::endl;
    
    return 0;
}
