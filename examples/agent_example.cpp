// NLM Agent Example: Smart Agent with Curiosity and Learning

#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include "nlm/config/Config.hpp"
#include <iostream>
#include <random>

class SimpleWorld {
public:
    SimpleWorld() {
        width = 20;
        height = 20;
        visionWidth = 8;
        visionHeight = 8;
        reset();
    }
    
    void configure(int w, int h, int vw, int vh) {
        width = w;
        height = h;
        visionWidth = vw;
        visionHeight = vh;
    }
    
    void reset() {
        agentX = 5;
        agentY = 5;
        time = 0.0;
        visionData.clear();
        std::generate(visionData.begin(), visionData.end(), []() { return rand() % 100 / 100.0f; });
    }
    
    void update(double dt) {
        time += dt;
        
        // Simple movement simulation
        if (agentX > 0 && agentY > 0) {
            // Add some random movement
            int moveX = (rand() % 3) - 1;  // -1, 0, or 1
            int moveY = (rand() % 3) - 1;
            
            agentX = std::max(0, std::min(width - 1, agentX + moveX));
            agentY = std::max(0, std::min(height - 1, agentY + moveY));
        }
        
        // Update vision data
        for (int i = 0; i < visionData.size(); ++i) {
            visionData[i] = (visionData[i] * 0.9f + (rand() % 100 / 100.0f) * 0.1f);
        }
        
        // Add some pattern to vision based on position
        int centerIndex = (agentY * visionWidth + agentX) % visionData.size();
        visionData[centerIndex] = 1.0f;
    }
    
    nlm::SensoryPercept getSensoryPercept() const {
        nlm::SensoryPercept percept;
        percept.vision = visionData;
        percept.touch = 0.5f;
        percept.internal = 0.3f;
        percept.proprioception = 0.2f;
        return percept;
    }
    
    void applyMotorCommand(const nlm::MotorCommand& command, double time) {
        // Simulate agent movement based on command
        switch (command.actionType) {
            case nlm::ActionType::MOVE_FORWARD:
                if (agentY > 0) agentY--;
                break;
            case nlm::ActionType::MOVE_BACKWARD:
                if (agentY < height - 1) agentY++;
                break;
            case nlm::ActionType::TURN_LEFT:
                if (agentX > 0) agentX--;
                break;
            case nlm::ActionType::TURN_RIGHT:
                if (agentX < width - 1) agentX++;
                break;
            case nlm::ActionType::INTERACT:
                // Simulate interaction
                std::cout << "      [Interaction occurred!]" << std::endl;
                break;
            case nlm::ActionType::WAIT:
                // Do nothing
                break;
        }
        
        std::cout << "      Moved to position (" << agentX << ", " << agentY << ")" << std::endl;
    }
    
    double getSimulationTime() const { return time; }
    
private:
    int width;
    int height;
    int visionWidth;
    int visionHeight;
    int agentX;
    int agentY;
    double time;
    std::vector<float> visionData;
};

float calculateReward(const SimpleWorld& world, const nlm::MotorCommand& action) {
    // Simple reward function
    float reward = 0.0f;
    
    // Reward for reaching certain positions
    if (world.getSimulationTime() > 5.0) {
        reward += 0.5f;
    }
    
    // Bonus for exploration (novel positions)
    if (world.getSimulationTime() > 10.0) {
        reward += 0.3f;
    }
    
    // Reward for interaction
    if (action.actionType == nlm::ActionType::INTERACT) {
        reward += 1.0f;
    }
    
    return reward;
}

int main() {
    std::cout << "=== NLM Agent Example ===" << std::endl;
    
    // Create world
    SimpleWorld world;
    world.configure(20, 20, 8, 8);
    
    // Create brain with agent configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::User);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::User);
    config->set("connection_probability", 0.12f, nlm::ConfigSource::User);
    
    // Configure memory systems
    config->set("working_memory_capacity", 300, nlm::ConfigSource::User);
    config->set("episodic_memory_capacity", 1000, nlm::ConfigSource::User);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create agent
    nlm::AgentBrain agent(&brain);
    agent.initialize(world);
    
    // Enable learning features
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    std::cout << "Agent created successfully!" << std::endl;
    std::cout << "World size: " << world.getConfig().width << "x" << world.getConfig().height << std::endl;
    std::cout << "Vision size: " << world.getConfig().visionWidth << "x" << world.getConfig().visionHeight << std::endl;
    
    // Run simulation
    std::cout << "\nStarting agent simulation..." << std::endl;
    
    for (int step = 0; step < 100; ++step) {
        std::cout << "\n=== Step " << step << " ===" << std::endl;
        
        // Update world
        world.update(0.1);
        
        // Get what agent sees
        auto percept = world.getSensoryPercept();
        std::cout << "Sensory input:" << std::endl;
        std::cout << "  Vision: [";
        for (float v : percept.vision) {
            std::cout << v << " ";
        }
        std::cout << "]" << std::endl;
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain thinks
        brain.step(step);
        
        // Get action from brain
        auto action = agent.decodeMotorCommand();
        
        std::cout << "Decision made:" << std::endl;
        std::cout << "  Action type: " << static_cast<int>(action.actionType) << std::endl;
        std::cout << "  Action parameters: [";
        for (float p : action.parameters) {
            std::cout << p << " ";
        }
        std::cout << "]" << std::endl;
        
        // Execute action
        std::cout << "Executing action..." << std::endl;
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Apply reward modulation
        float reward = calculateReward(world, action);
        agent.applyRewardModulation(reward, 0.0f);
        
        // Log agent state
        std::cout << "Agent state:" << std::endl;
        std::cout << "  Developmental stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        std::cout << "  Neuromodulation level: " << agent.getNeuromodulationLevel() << std::endl;
        std::cout << "  Curiosity level: " << agent.getCuriosityLevel() << std::endl;
        std::cout << "  Novelty level: " << agent.getNoveltyLevel() << std::endl;
        std::cout << "  Prediction error: " << agent.getPredictionError() << std::endl;
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Print brief summary every 20 steps
        if (step % 20 == 0 && step > 0) {
            std::cout << "\n--- Progress Report (Step " << step << ") ---" << std::endl;
            std::cout << "Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
            std::cout << "Active neurons: " << brain.getActiveNeuronCount() << "/" << brain.getTotalNeuronCount() << std::endl;
            std::cout << "Working memory usage: " << agent.getWorkingMemory()->getCurrentSize() 
                     << "/" << agent.getWorkingMemory()->getCapacity() << std::endl;
        }
    }
    
    std::cout << "\n=== Agent Simulation Completed ===" << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Learning complete!" << std::endl;
    
    return 0;
}
