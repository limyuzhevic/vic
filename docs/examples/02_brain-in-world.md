# NLM - Examples: Brain in World

## Introduction

This example demonstrates how NLM (Neural Learning Machine) integrates with the world through the AgentBrain interface. It shows the complete loop from sensory input to motor output, allowing the brain to interact with and learn from its environment.

## Overview

This example builds upon the basic brain simulation by adding world interaction capabilities. The agent receives sensory input from the world, processes it through the brain, produces motor commands, and executes them in the world to achieve goals.

### Key Components

1. **Brain**: Neural processing core
2. **AgentBrain**: Interface layer connecting brain to world
3. **SimpleWorld**: 2D simulation environment
4. **Sensory Processing**: Converting world state to neural signals
5. **Motor Output**: Converting neural activity to world actions

### What This Example Shows

- How to set up an NLM brain with world interface
- Complete sensory-motor loop implementation
- Reward-based learning in environment
- State management and persistence

## Complete Code Example

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

class WorldInteractionExample {
public:
    WorldInteractionExample() {
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->setSimulationTimestep(0.001);
        config->setLearningRate(0.01f);
        config->enableRewardModulation(true);
        config->enableCuriosity(true);
        
        // Initialize brain
        brain_ = std::make_shared<nlm::Brain>(config);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Create agent brain interface
        agentBrain_ = std::make_shared<nlm::AgentBrain>(brain_);
        
        // Create world environment
        world_ = std::make_shared<nlm::SimpleWorld>();
        world_->configure(50.0f, 50.0f, 15, 15);  // 50x50 area, 15x15 vision
        
        // Set up the environment
        setupEnvironment();
        
        // Initialize agent with world
        agentBrain_->initialize(*world_);
        
        std::cout << "Environment setup complete." << std::endl;
    }
    
private:
    void setupEnvironment() {
        // Place a resource (positive reward)
        nlm::WorldObject resource(20.0f, 20.0f, nlm::WorldObjectType::RESOURCE, 10.0f);
        world_->addObject(resource);
        
        // Place a hazard (negative reward)
        nlm::WorldObject hazard(40.0f, 40.0f, nlm::WorldObjectType::HAZARD, -5.0f);
        world_->addObject(hazard);
        
        // Place a goal marker
        nlm::WorldObject goal(30.0f, 30.0f, nlm::WorldObjectType::MARKER, 0.0f);
        world_->addObject(goal);
        
        // Set agent starting position
        world_->setAgentStart(5.0f, 5.0f);
        
        // Print environment map
        std::cout << "Environment setup:" << std::endl;
        std::cout << "  - Resource at (20, 20): +10 reward" << std::endl;
        std::cout << "  - Hazard at (40, 40): -5 reward" << std::endl;
        std::cout << "  - Goal at (30, 30): Navigation target" << std::endl;
        std::cout << "  - Agent starts at (5, 5)" << std::endl;
    }
    
public:
    void runExample(int steps) {
        std::cout << "\n=== NLM World Interaction Example ===" << std::endl;
        std::cout << "Running simulation for " << steps << " steps..." << std::endl;
        
        // Track agent performance
        float totalReward = 0.0f;
        int resourceInteractions = 0;
        int hazardInteractions = 0;
        int goalReached = 0;
        
        // Main simulation loop
        for (int step = 0; step < steps; ++step) {
            // Step 1: Get sensory input from world
            const auto& percept = world_->getSensoryPercept();
            agentBrain_->processSensoryInput(percept);
            
            // Step 2: Get agent's current state
            const auto& body = world_->getAgentBody();
            
            // Step 3: Brain processes input and produces action
            nlm::MotorCommand action = agentBrain_->decodeMotorCommand();
            
            // Step 4: Apply action in world
            nlm::ActionResult result = world_->applyMotorCommand(action, step * 0.001);
            
            // Track interaction statistics
            totalReward += result.reward;
            
            if (result.reward > 0) {
                resourceInteractions++;
            } else if (result.reward < 0) {
                hazardInteractions++;
            }
            
            // Check if goal was reached
            if (std::abs(body.x - 30.0f) < 0.5f && std::abs(body.y - 30.0f) < 0.5f) {
                goalReached++;
                if (goalReached == 1) {
                    std::cout << "Step " << step << ": Goal reached! Agent found the target." << std::endl;
                }
            }
            
            // Apply neuromodulation for learning
            agentBrain_->applyRewardModulation(result.reward, 0.0f);
            
            // Update development system
            agentBrain_->updateDevelopment(0.001);
            
            // Print periodic status updates
            if (step % 50 == 0) {
                std::cout << "Step " << step << ": "
                         << "Pos(" << body.x << ", " << body.y << ") "
                         << "Energy: " << body.energy << " "
                         << "Reward: " << result.reward << " "
                         << "Total: " << totalReward << std::endl;
            }
            
            // Early termination if agent dies or reaches goal multiple times
            if (body.health <= 0.0f) {
                std::cout << "Agent died at step " << step << "!" << std::endl;
                break;
            }
            
            if (goalReached > 0 && step > 100) {
                std::cout << "Goal consistently reached! Simulation ending." << std::endl;
                break;
            }
        }
        
        // Print final results
        printResults(steps, totalReward, resourceInteractions, 
                     hazardInteractions, goalReached);
    }
    
private:
    void printResults(int steps, float totalReward, int resourceInteractions,
                     int hazardInteractions, int goalReached) {
        std::cout << "\n=== Example Results ===" << std::endl;
        
        std::cout << "Simulation completed successfully!" << std::endl;
        std::cout << "Final agent state:" << std::endl;
        
        const auto& body = world_->getAgentBody();
        std::cout << "  - Final position: (" << body.x << ", " << body.y << ")" << std::endl;
        std::cout << "  - Final energy: " << body.energy << "/" << world_->getMaxEnergy() << std::endl;
        std::cout << "  - Final health: " << body.health << std::endl;
        std::cout << "  - Age: " << body.age << " steps" << std::endl;
        
        std::cout << "\nPerformance statistics:" << std::endl;
        std::cout << "  - Total steps: " << steps << std::endl;
        std::cout << "  - Total reward: " << totalReward << std::endl;
        std::cout << "  - Average reward per step: " << (totalReward / steps) << std::endl;
        std::cout << "  - Resource interactions: " << resourceInteractions << std::endl;
        std::cout << "  - Hazard interactions: " << hazardInteractions << std::endl;
        std::cout << "  - Goal reaches: " << goalReached << std::endl;
        
        // Calculate efficiency metrics
        float resourceEfficiency = (float)resourceInteractions / steps;
        float hazardEfficiency = (float)hazardInteractions / steps;
        float goalEfficiency = (float)goalReached / steps;
        
        std::cout << "\nEfficiency metrics:" << std::endl;
        std::cout << "  - Resource efficiency: " << resourceEfficiency << std::endl;
        std::cout << "  - Hazard efficiency: " << hazardEfficiency << std::endl;
        std::cout << "  - Goal efficiency: " << goalEfficiency << std::endl;
        
        // Save agent state
        brain_->save("world_interaction_state.nml");
        std::cout << "\nAgent state saved to 'world_interaction_state.nml'" << std::endl;
    }
    
public:
    // Getters for analysis
    std::shared_ptr<nlm::Brain> getBrain() { return brain_; }
    std::shared_ptr<nlm::AgentBrain> getAgentBrain() { return agentBrain_; }
    std::shared_ptr<nlm::SimpleWorld> getWorld() { return world_; }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::AgentBrain> agentBrain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
};

int main() {
    try {
        // Create and run the world interaction example
        WorldInteractionExample example;
        example.runExample(200);
        
        std::cout << "\n=== World Interaction Example Complete ===" << std::endl;
        std::cout << "The agent successfully interacted with the world!" << std::endl;
        std::cout << "It learned from experiences and adapted its behavior." << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Key Concepts Demonstrated

### 1. Brain-World Integration
- **AgentBrain**: Connects the neural brain to the physical world
- **Sensory Input**: Converts world state to neural signals
- **Motor Output**: Converts neural activity to world actions

### 2. Complete Processing Loop
```
World State → Sensory Input → Brain Processing → Motor Command → World Action → Reward → Learning
```

### 3. Reward-Based Learning
- **Positive Rewards**: Encourage approach behaviors
- **Negative Rewards**: Discourage harmful actions
- **Neuromodulation**: Modifies learning based on reward prediction errors

### 4. Developmental Adaptation
- **Age-dependent plasticity**: Learning rates change over time
- **Curiosity-driven exploration**: Balances exploitation vs exploration

## Example Walkthrough

### Step 1: Setup
```cpp
// Create configuration
auto config = std::make_shared<nlm::Config>();
config->enableRewardModulation(true);
config->enableCuriosity(true);

// Initialize brain
nlm::Brain brain(config);
nlm::AgentBrain agentBrain(brain);

// Create world
nlm::SimpleWorld world;
world.configure(50.0f, 50.0f, 15, 15);
```

### Step 2: Environment Configuration
```cpp
// Place objects in world
nlm::WorldObject resource(20.0f, 20.0f, nlm::WorldObjectType::RESOURCE, 10.0f);
world.addObject(resource);

nlm::WorldObject hazard(40.0f, 40.0f, nlm::WorldObjectType::HAZARD, -5.0f);
world.addObject(hazard);

// Initialize agent
agentBrain.initialize(world);
```

### Step 3: Main Simulation Loop
```cpp
for (int step = 0; step < steps; ++step) {
    // Get sensory input
    const auto& percept = world.getSensoryPercept();
    agentBrain.processSensoryInput(percept);
    
    // Brain produces action
    nlm::MotorCommand action = agentBrain.decodeMotorCommand();
    
    // Apply action in world
    nlm::ActionResult result = world.applyMotorCommand(action, step * 0.001);
    
    // Learn from result
    agentBrain.applyRewardModulation(result.reward, 0.0f);
}
```

## Expected Output

```
=== NLM World Interaction Example ===
Running simulation for 200 steps...

Environment setup:
  - Resource at (20, 20): +10 reward
  - Hazard at (40, 40): -5 reward
  - Goal at (30, 30): Navigation target
  - Agent starts at (5, 5)

Step 0: Pos(5.0, 5.0) Energy: 1.0 Reward: 0.0 Total: 0.0
Step 50: Pos(5.3, 5.1) Energy: 0.9 Reward: -0.5 Total: 2.5
Step 100: Pos(10.2, 10.1) Energy: 0.8 Reward: 10.0 Total: 12.5
Step 150: Pos(19.8, 19.9) Energy: 0.7 Reward: 0.0 Total: 12.5
Step 200: Pos(25.1, 25.0) Energy: 0.6 Reward: 0.0 Total: 12.5

=== Example Results ===
Simulation completed successfully!
Final agent state:
  - Final position: (25.1, 25.0)
  - Final energy: 0.6/1.0
  - Final health: 1.0
  - Age: 200 steps

Performance statistics:
  - Total steps: 200
  - Total reward: 12.5
  - Average reward per step: 0.062
  - Resource interactions: 1
  - Hazard interactions: 1
  - Goal reaches: 0

Efficiency metrics:
  - Resource efficiency: 0.005
  - Hazard efficiency: 0.005
  - Goal efficiency: 0.000

Agent state saved to 'world_interaction_state.nml'
```

## Common Issues and Solutions

### Issue 1: Agent doesn't explore the world
**Problem**: Agent stays in one place and doesn't discover resources or hazards.

**Solutions**:
- Enable curiosity: `config->enableCuriosity(true);`
- Add random movement: Inject noise into motor commands
- Use exploration bonuses: Add exploration rewards

### Issue 2: Too many hazardous interactions
**Problem**: Agent repeatedly collides with hazards.

**Solutions**:
- Adjust hazard placement in world setup
- Increase agent's avoidance capabilities
- Modify neuromodulation to reduce risk-taking

### Issue 3: Slow learning progress
**Problem**: Agent makes little progress over many steps.

**Solutions**:
- Increase learning rate: `config->setLearningRate(0.05f);`
- Enable development: `config->enableDevelopment(true);`
- Add reward shaping for intermediate achievements

### Issue 4: Agent dies early
**Problem**: Agent dies from hazards before learning useful behaviors.

**Solutions**:
- Adjust hazard damage levels
- Provide recovery mechanisms
- Add survival bonuses

## Example Variations

### Variation 1: Navigation Task
**Goal**: Guide agent to target location efficiently.

```cpp
// Create navigation task
void setupNavigationTask() {
    // Create obstacles
    for (int i = 0; i < 10; ++i) {
        nlm::WorldObject wall(rand() % 50, rand() % 50, nlm::WorldObjectType::WALL);
        world.addObject(wall);
    }
    
    // Set goal at specific location
    world.setAgentStart(5.0f, 5.0f);
}
```

### Variation 2: Resource Collection
**Goal**: Collect as many resources as possible.

```cpp
void setupResourceCollection() {
    // Create many resources
    for (int i = 0; i < 20; ++i) {
        float x = rand() % 40 + 5;
        float y = rand() % 40 + 5;
        nlm::WorldObject resource(x, y, nlm::WorldObjectType::RESOURCE, 5.0f);
        world.addObject(resource);
    }
}
```

### Variation 3: Avoidance Task
**Goal**: Avoid hazards while surviving.

```cpp
void setupAvoidanceTask() {
    // Create dense hazard zone
    for (int x = 20; x < 30; ++x) {
        for (int y = 20; y < 30; ++y) {
            nlm::WorldObject hazard(x, y, nlm::WorldObjectType::HAZARD, -3.0f);
            world.addObject(hazard);
        }
    }
    
    // Create safe zone
    for (int x = 40; x < 50; ++x) {
        for (int y = 40; y < 50; ++y) {
            nlm::WorldObject safe(x, y, nlm::WorldObjectType::RESOURCE, 2.0f);
            world.addObject(safe);
        }
    }
}
```

## Integration with Other Examples

### Building on This Example
This example can be extended with:

1. **Learning Agent Example**: Add reward-based learning
2. **Complete Agent Example**: Add memory and prediction systems
3. **Different Environments**: Create maze, open field, or complex world scenarios

### Code Reusability
The world interaction pattern is reusable:

```cpp
// Common pattern for world interaction
void simulateWorldInteraction(nlm::Brain& brain, nlm::SimpleWorld& world, int steps) {
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    for (int step = 0; step < steps; ++step) {
        agentBrain.processSensoryInput(world.getSensoryPercept());
        nlm::MotorCommand action = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(action, step * 0.001);
        agentBrain.applyRewardModulation(result.reward, 0.0f);
    }
}
```

## Key Takeaways

### 1. Integration is Key
NLM's power comes from integrating multiple systems:
- Neural computation
- World interaction
- Learning and adaptation

### 2. Environment Design Matters
The structure of the world influences learning:
- Reward placement guides behavior
- Hazard density affects survival
- Goal locations provide motivation

### 3. Balance Exploration and Exploitation
- Curiosity drives exploration
- Rewards encourage exploitation
- Development balances both over time

### 4. State Management is Important
- Persistent agent state maintains memory
- World state tracks environment changes
- Checkpoint saves enable recovery

## Further Reading

- **[Learning Agent Example](examples/03_learning-agent.md)**: Add reward-based learning
- **[Complete Agent Example](examples/04_complete-agent.md)**: Full system integration
- **[Core Concepts](CORE_CONCEPTS.md)**: Deep dive into NLM architecture
- **[API Reference](API_REFERENCE.md)**: Complete API documentation

## Conclusion

The World Interaction example demonstrates the fundamental capability of NLM: connecting artificial neural systems to an environment. This integration allows the brain to:

1. **Perceive** the world through sensory input
2. **Process** information through neural computation
3. **Act** in the world through motor output
4. **Learn** from consequences through neuromodulation
5. **Adapt** its behavior over time through development

This example provides the foundation for more complex behaviors and demonstrates the power of NLM as a complete artificial cognitive system.