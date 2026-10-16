// ex07_integration.cpp - Complete Brain Integration

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/environment/Environment.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/experiments/Experiment.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

class MazeEnvironment : public nlm::Environment {
public:
    MazeEnvironment() {
        // Initialize simple maze environment
        reset();
    }
    
    std::vector<float> getSensoryInput() const override {
        std::vector<float> input;
        
        // Position in maze
        input.push_back(static_cast<float>(x));
        input.push_back(static_cast<float>(y));
        
        // Direction faced
        input.push_back(static_cast<float>(direction));
        
        // Nearby obstacles (4 sensor readings)
        input.push_back(hasWallAhead() ? 1.0f : 0.0f);
        input.push_back(hasWallRight() ? 1.0f : 0.0f);
        input.push_back(hasWallLeft() ? 1.0f : 0.0f);
        input.push_back(hasWallBehind() ? 1.0f : 0.0f);
        
        // Distance to goal
        float dx = goalX - x;
        float dy = goalY - y;
        float distance = std::sqrt(dx*dx + dy*dy);
        input.push_back(std::min(1.0f, distance / 10.0f)); // Normalized to [0,1]
        
        return input;
    }
    
    void applyAction(const nlm::MotorCommand& action) override {
        // Parse action and move agent
        if (action.forward) {
            moveForward();
        }
        if (action.turnLeft) {
            turnLeft();
        }
        if (action.turnRight) {
            turnRight();
        }
        if (action.interact) {
            interact();
        }
        
        steps++;
        calculateReward();
        checkTermination();
    }
    
    float getReward() const override {
        return reward;
    }
    
    bool isTerminal() const override {
        return episodeComplete;
    }
    
    void reset() override {
        // Reset to starting position
        x = 0.0f;
        y = 0.0f;
        direction = 0.0f;
        goalX = 9.0f;
        goalY = 9.0f;
        steps = 0;
        episodeComplete = false;
        reward = 0.0f;
        
        // Place some obstacles (walls)
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
                if ((i + j) % 3 == 0) {
                    obstacle[i][j] = true;
                } else {
                    obstacle[i][j] = false;
                }
            }
        }
    }
    
private:
    bool hasWallAhead() const {
        float nextX = x;
        float nextY = y;
        
        switch (static_cast<int>(direction)) {
            case 0: nextY += 1; break; // North
            case 1: nextX += 1; break; // East
            case 2: nextY -= 1; break; // South
            case 3: nextX -= 1; break; // West
        }
        
        return isWall(nextX, nextY);
    }
    
    bool hasWallRight() const {
        int rightDirection = (static_cast<int>(direction) + 1) % 4;
        float rightX = x;
        float rightY = y;
        
        switch (rightDirection) {
            case 0: rightY += 1; break;
            case 1: rightX += 1; break;
            case 2: rightY -= 1; break;
            case 3: rightX -= 1; break;
        }
        
        return isWall(rightX, rightY);
    }
    
    bool hasWallLeft() const {
        int leftDirection = (static_cast<int>(direction) - 1 + 4) % 4;
        float leftX = x;
        float leftY = y;
        
        switch (leftDirection) {
            case 0: leftY += 1; break;
            case 1: leftX += 1; break;
            case 2: leftY -= 1; break;
            case 3: leftX -= 1; break;
        }
        
        return isWall(leftX, leftY);
    }
    
    bool hasWallBehind() const {
        int behindDirection = (static_cast<int>(direction) + 2) % 4;
        float behindX = x;
        float behindY = y;
        
        switch (behindDirection) {
            case 0: behindY += 1; break;
            case 1: behindX += 1; break;
            case 2: behindY -= 1; break;
            case 3: behindX -= 1; break;
        }
        
        return isWall(behindX, behindY);
    }
    
    bool isWall(float worldX, float worldY) const {
        int ix = static_cast<int>(worldX);
        int iy = static_cast<int>(worldY);
        
        if (ix < 0 || ix >= width || iy < 0 || iy >= height) {
            return true;
        }
        
        return obstacle[ix][iy];
    }
    
    void moveForward() {
        float nextX = x;
        float nextY = y;
        
        switch (static_cast<int>(direction)) {
            case 0: nextY += 1; break;
            case 1: nextX += 1; break;
            case 2: nextY -= 1; break;
            case 3: nextX -= 1; break;
        }
        
        if (!isWall(nextX, nextY)) {
            x = nextX;
            y = nextY;
            
            if (x == goalX && y == goalY) {
                episodeComplete = true;
                reward += 10.0f; // Goal reached bonus
            }
        }
    }
    
    void turnLeft() {
        direction = (direction - 1 + 4) % 4;
    }
    
    void turnRight() {
        direction = (direction + 1) % 4;
    }
    
    void interact() {
        // Pick up items, open doors, etc.
        reward += 1.0f;
    }
    
    void calculateReward() {
        // Base reward for movement
        reward = 0.1f;
        
        // Distance-based penalty (negative)
        float dx = goalX - x;
        float dy = goalY - y;
        float distance = std::sqrt(dx*dx + dy*dy);
        reward -= distance * 0.01f;
        
        // Penalty for taking too many steps
        reward -= steps * 0.001f;
        
        // Bonus for approaching goal
        if (distance < 2.0f) {
            reward += 2.0f;
        }
        
        // Ensure reward is not negative
        if (reward < -1.0f) {
            reward = -1.0f;
        }
    }
    
    void checkTermination() {
        if (episodeComplete) {
            return;
        }
        if (steps >= maxSteps) {
            episodeComplete = true;
            reward -= 5.0f; // Time penalty
        }
    }
    
    const int width = 10;
    const int height = 10;
    const int maxSteps = 200;
    
    float x, y;
    float goalX, goalY;
    float direction;
    bool obstacle[10][10];
    int steps;
    bool episodeComplete;
    float reward;
};

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== Example 7: Complete Brain Integration ===" << std::endl;
    
    // Create configuration for complete integration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 500);
    config->set("brain.simulation.maxSteps", 1000);
    
    // Enable all major systems
    config->set("brain.memory.workingMemory.enabled", true);
    config->set("brain.memory.workingMemory.capacity", 200);
    config->set("brain.memory.episodicMemory.enabled", true);
    config->set("brain.memory.associativeMemory.enabled", true);
    config->set("brain.neuromodulation.dopamine.enabled", true);
    config->set("brain.neuromodulation.curiosity.enabled", true);
    config->set("brain.neuromodulation.novelty.enabled", true);
    config->set("brain.development.enabled", true);
    config->set("brain.development.plasticityDecayRate", 0.02);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create environment
    auto environment = std::make_unique<MazeEnvironment>();
    agent->initialize(*environment);
    
    std::cout << "Complete integrated brain initialized!" << std::endl;
    std::cout << "Environment: " << environment->getSensoryInput().size() << " sensory inputs" << std::endl;
    std::cout << "Motor outputs: " << agent->getMotorOutputSize() << std::endl;
    
    // Create experiment to track performance
    auto experiment = std::make_shared<nlm::Experiment>("Complete Integration", brain);
    
    std::cout << "\n--- Integration Simulation ---" << std::endl;
    
    // Main simulation loop
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        // Get sensory input
        auto sensoryInput = environment->getSensoryInput();
        auto percept = nlm::SensoryPercept(sensoryInput);
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Step brain
        brain->step(step);
        
        // Get motor output
        auto action = agent->decodeMotorCommand();
        
        // Apply action to environment
        environment->applyAction(action);
        
        // Apply neuromodulation based on reward
        float reward = environment->getReward();
        float predictionError = 0.0f; // Would come from prediction system
        agent->applyRewardModulation(reward, predictionError);
        
        // Log progress every 50 steps
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                     << "Reward: " << reward << ", "
                     << "Neurons: " << brain->getTotalNeuronCount() << ", "
                     << "Spikes: " << brain->getTotalSpikeCount() << ", "
                     << "Plasticity: " << brain->getExcitationInhibitionRatio() << std::endl;
        }
        
        if (environment->isTerminal()) {
            std::cout << "Episode completed at step " << step << " with reward " << reward << std::endl;
            environment->reset();
            agent->reset();
            brain->reset();
            experiment->recordTrial(step, reward);
        }
    }
    
    // Save final brain state
    brain->save("ex07_final.brain");
    std::cout << "\nFinal brain state saved to 'ex07_final.brain'" << std::endl;
    
    // Get experiment results
    auto results = experiment->getResults();
    std::cout << "\n=== Example 7 Complete ===" << std::endl;
    std::cout << "Integration simulation completed successfully!" << std::endl;
    std::cout << "Average reward per step: " << results.averageReward << std::endl;
    std::cout << "Total trials completed: " << results.trialsCompleted << std::endl;
    
    return 0;
}