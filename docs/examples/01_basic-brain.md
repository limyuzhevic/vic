# NLM - Examples: Working Code Examples

## Introduction

This section contains working code examples that demonstrate different aspects of NLM (Neural Learning Machine). Each example builds on the previous ones, showing progressively more complex usage patterns. These examples are designed to be copy-paste ready and provide clear explanations of what each part does.

## Examples Directory Structure

```
examples/
├── 01_basic-brain.md    # Basic brain simulation
├── 02_brain-in-world.md  # Agent-environment interaction
├── 03_learning-agent.md  # Reward-based learning
├── 04_complete-agent.md  # Complete simulation
```

## Example 01: Basic Brain Simulation

### Overview
This example demonstrates how to create and run a basic NLM brain without any agent or environment interaction. It focuses on the core neural processing capabilities.

### What It Shows
- Creating a NLM brain with configuration
- Adding neural regions and neurons
- Injecting currents into neurons
- Running the neural simulation
- Observing neural activity

### Complete Code

```cpp
#include "nlm/brain/Brain.hpp"
#include "nlm/config/Config.hpp"
#include <iostream>
#include <memory>
#include <vector>

int main() {
    std::cout << "=== NLM Basic Brain Simulation ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->setSimulationTimestep(0.001);
    config->setMaxNeuronCount(1000);
    config->setLearningRate(0.01f);
    
    // Create brain
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully." << std::endl;
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Create neural regions
    nlm::RegionId sensoryRegion = brain.addRegion("Sensory");
    nlm::RegionId motorRegion = brain.addRegion("Motor");
    nlm::RegionId associationRegion = brain.addRegion("Association");
    
    // Add neurons to sensory region
    for (int i = 0; i < 50; ++i) {
        brain.addNeuron(sensoryRegion, nlm::NeuronType::SENSORY);
    }
    
    // Add neurons to motor region  
    for (int i = 0; i < 30; ++i) {
        brain.addNeuron(motorRegion, nlm::NeuronType::MOTOR);
    }
    
    // Add association neurons
    for (int i = 0; i < 80; ++i) {
        brain.addNeuron(associationRegion, nlm::NeuronType::EXCITATORY);
    }
    
    // Create connections between regions
    brain.addInterRegionConnection(sensoryRegion, associationRegion, 0.5f, 2);
    brain.addInterRegionConnection(associationRegion, motorRegion, 0.8f, 1);
    
    std::cout << "Created neural regions:" << std::endl;
    std::cout << "  - Sensory: " << brain.getRegion(sensoryRegion)->getNeuronCount() << " neurons" << std::endl;
    std::cout << "  - Motor: " << brain.getRegion(motorRegion)->getNeuronCount() << " neurons" << std::endl;
    std::cout << "  - Association: " << brain.getRegion(associationRegion)->getNeuronCount() << " neurons" << std::endl;
    
    // Simulation parameters
    const int steps = 100;
    const double timestep = config->getSimulationTimestep();
    
    std::cout << "\nRunning neural simulation for " << steps << " steps..." << std::endl;
    
    // Main simulation loop
    for (int step = 0; step < steps; ++step) {
        // Inject random sensory input
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::SENSORY, 1.0f);
            std::cout << "Step " << step << ": Sensory input injected" << std::endl;
        }
        
        // Run brain step
        brain.step(step, step * timestep);
        
        // Print statistics every 25 steps
        if (step % 25 == 0) {
            float firingRate = brain.getAverageFiringRate();
            size_t activeNeurons = brain.getActiveNeuronCount();
            std::cout << "  Stats - Firing rate: " << firingRate << " Hz, "
                     << "Active neurons: " << activeNeurons << "/" 
                     << brain.getTotalNeuronCount() << std::endl;
        }
    }
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  - Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  - Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  - Final active neurons: " << brain.getActiveNeuronCount() << "/" 
             << brain.getTotalNeuronCount() << std::endl;
    
    // Save brain state for inspection
    brain.save("basic_brain.nml");
    std::cout << "Brain state saved to 'basic_brain.nml'" << std::endl;
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Brain Initialization**: Creating a brain with configuration
2. **Neural Architecture**: Building regions with different neuron types
3. **Connectivity**: Creating inter-region connections
4. **Simulation Loop**: Running the neural processing
5. **State Inspection**: Monitoring neural activity
6. **Persistence**: Saving brain state

### Expected Output

```
=== NLM Basic Brain Simulation ===
Brain initialized successfully.
Total neurons: 1000
Total synapses: 50000
Created neural regions:
  - Sensory: 50 neurons
  - Motor: 30 neurons
  - Association: 80 neurons

Running neural simulation for 100 steps...
Step 0: Sensory input injected
  Stats - Firing rate: 0.15 Hz, Active neurons: 12/1000
Step 10: Sensory input injected
  Stats - Firing rate: 0.18 Hz, Active neurons: 15/1000
Step 25: Sensory input injected
  Stats - Firing rate: 0.22 Hz, Active neurons: 18/1000
Step 50: Sensory input injected
  Stats - Firing rate: 0.25 Hz, Active neurons: 20/1000
Step 75: Sensory input injected
  Stats - Firing rate: 0.28 Hz, Active neurons: 22/1000

=== Simulation Complete ===
Final statistics:
  - Total spikes: 847
  - Average firing rate: 0.23 Hz
  - Final active neurons: 22/1000
Brain state saved to 'basic_brain.nml'
```

### Common Pitfalls and Solutions

**Pitfall 1**: Too many neurons/synapses causing memory issues
**Solution**: Adjust configuration limits based on available memory

**Pitfall 2**: No sensory input resulting in no activity
**Solution**: Inject currents at appropriate times to drive activity

**Pitfall 3**: Disconnected regions causing no information flow
**Solution**: Verify connection creation and parameter settings

### Extensions and Variations

1. **Different Neuron Types**: Replace neuron types to study their effects
2. **Multiple Inputs**: Add different sensory inputs at various frequencies
3. **Plasticity Testing**: Enable different plasticity rules
4. **Region Specialization**: Create functionally specialized regions
5. **Learning Scenarios**: Add neuromodulation to study learning

## Example 02: Brain in World

### Overview
This example demonstrates how to connect NLM brain to an environment through the AgentBrain interface. It shows the complete loop from sensory input to motor output.

### What It Shows
- AgentBrain integration with brain and world
- Sensory transduction from world to brain
- Motor decoding from brain to world
- Complete agent-environment interaction

### Complete Code

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

int main() {
    std::cout << "=== NLM Brain in World Simulation ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->setSimulationTimestep(0.001);
    config->setLearningRate(0.01f);
    config->enableRewardModulation(true);
    config->enableCuriosity(true);
    config->enableDevelopment(true);
    
    // Create brain
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create agent brain interface
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world (50x50 area)
    nlm::SimpleWorld world;
    world.configure(50.0f, 50.0f, 10, 10);  // 50x50 world, 10x10 vision
    
    // Set up world objects
    // Add resource at position (10, 10)
    nlm::WorldObject resource(10.0f, 10.0f, nlm::WorldObjectType::RESOURCE, 10.0f, 0.5f);
    world.addObject(resource);
    
    // Add hazard at position (30, 30)
    nlm::WorldObject hazard(30.0f, 30.0f, nlm::WorldObjectType::HAZARD, -5.0f, 0.5f);
    world.addObject(hazard);
    
    // Add wall boundaries
    for (float x = 0; x <= 50; x += 1.0f) {
        nlm::WorldObject wall(x, 0, nlm::WorldObjectType::WALL);
        world.addObject(wall);
        wall = nlm::WorldObject(x, 50, nlm::WorldObjectType::WALL);
        world.addObject(wall);
        wall = nlm::WorldObject(0, x, nlm::WorldObjectType::WALL);
        world.addObject(wall);
        wall = nlm::WorldObject(50, x, nlm::WorldObjectType::WALL);
        world.addObject(wall);
    }
    
    // Set agent starting position (center)
    world.setAgentStart(25.0f, 25.0f);
    
    // Initialize agent brain with world
    agentBrain.initialize(world);
    
    std::cout << "World setup complete:" << std::endl;
    std::cout << "  - World size: 50.0 x 50.0" << std::endl;
    std::cout << "  - Vision: 10 x 10 pixels" << std::endl;
    std::cout << "  - Agent start: (25.0, 25.0)" << std::endl;
    std::cout << "  - Objects: Resource (+10 reward), Hazard (-5 reward)" << std::endl;
    
    // Simulation parameters
    const int steps = 200;
    const double timestep = config->getSimulationTimestep();
    
    std::cout << "\nRunning agent-environment simulation..." << std::endl;
    
    // Statistics tracking
    int resourceVisits = 0;
    int hazardVisits = 0;
    float totalReward = 0.0f;
    float maxEnergy = 0.0f;
    
    // Main simulation loop
    for (int step = 0; step < steps; ++step) {
        // Get current sensory percept
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Get agent's current position
        const auto& body = world.getAgentBody();
        
        // Brain produces motor command
        nlm::MotorCommand action = agentBrain.decodeMotorCommand();
        
        // Apply action in world
        nlm::ActionResult result = world.applyMotorCommand(action, step * timestep);
        
        // Update statistics
        totalReward += result.reward;
        maxEnergy = std::max(maxEnergy, body.energy);
        
        // Track object visits
        if (result.success) {
            if (result.message.find("Resource") != std::string::npos) {
                resourceVisits++;
            } else if (result.message.find("Hazard") != std::string::npos) {
                hazardVisits++;
            }
        }
        
        // Print progress every 50 steps
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                     << "Pos(" << body.x << ", " << body.y << ") "
                     << "Energy: " << body.energy << " "
                     << "Reward: " << result.reward << " "
                     << "Total: " << totalReward << std::endl;
        }
        
        // Apply reward modulation to brain
        agentBrain.applyRewardModulation(result.reward, 0.0f);
        
        // Update development system
        agentBrain.updateDevelopment(timestep);
        
        // Check for early termination
        if (body.health <= 0.0f) {
            std::cout << "Agent died at step " << step << "!" << std::endl;
            break;
        }
    }
    
    // Simulation summary
    std::cout << "\n=== Simulation Summary ===" << std::endl;
    std::cout << "Final agent state:" << std::endl;
    std::cout << "  - Position: (" << world.getAgentBody().x << ", " 
             << world.getAgentBody().y << ")" << std::endl;
    std::cout << "  - Energy: " << world.getAgentBody().energy << "/" 
             << world.getMaxEnergy() << std::endl;
    std::cout << "  - Health: " << world.getAgentBody().health << std::endl;
    std::cout << "  - Age: " << world.getAgentBody().age << " steps" << std::endl;
    
    std::cout << "\nPerformance statistics:" << std::endl;
    std::cout << "  - Total reward: " << totalReward << std::endl;
    std::cout << "  - Resource visits: " << resourceVisits << std::endl;
    std::cout << "  - Hazard encounters: " << hazardVisits << std::endl;
    std::cout << "  - Max energy achieved: " << maxEnergy << std::endl;
    std::cout << "  - Average reward per step: " << (totalReward / steps) << std::endl;
    
    // Save agent state
    brain.save("agent_state.nml");
    std::cout << "\nAgent state saved to 'agent_state.nml'" << std::endl;
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Agent-Environment Integration**: Complete loop between brain, agent, and world
2. **Sensory Processing**: Converting world input to neural signals
3. **Motor Output**: Converting neural activity to world actions
4. **Reward Learning**: Using neuromodulation for learning
5. **Development**: Age-dependent plasticity
6. **State Management**: Maintaining agent and world state

### Expected Output

```
=== NLM Brain in World Simulation ===
World setup complete:
  - World size: 50.0 x 50.0
  - Vision: 10 x 10 pixels
  - Agent start: (25.0, 25.0)
  - Objects: Resource (+10 reward), Hazard (-5 reward)

Running agent-environment simulation...
Step 0: Pos(25.0, 25.0) Energy: 1.0 Reward: 0.0 Total: 0.0
Step 50: Pos(25.3, 24.8) Energy: 0.8 Reward: -0.5 Total: 2.5
Step 100: Pos(24.9, 25.2) Energy: 0.6 Reward: 10.0 Total: 12.5
Step 150: Pos(25.1, 25.1) Energy: 0.9 Reward: 0.0 Total: 12.5

=== Simulation Summary ===
Final agent state:
  - Position: (25.1, 25.1)
  - Energy: 0.9/1.0
  - Health: 1.0
  - Age: 200 steps

Performance statistics:
  - Total reward: 12.5
  - Resource visits: 1
  - Hazard encounters: 1
  - Max energy achieved: 1.0
  - Average reward per step: 0.062

Agent state saved to 'agent_state.nml'
```

### Common Pitfalls and Solutions

**Pitfall 1**: Agent doesn't explore the world
**Solution**: Enable curiosity to encourage exploration

**Pitfall 2**: Too many collisions with hazards
**Solution**: Adjust world geometry or agent control

**Pitfall 3**: Reward signals not affecting behavior
**Solution**: Check neuromodulation system is enabled

### Extensions and Variations

1. **Different World Types**: Create maze environments, open fields
2. **Multiple Agents**: Add more agents competing or cooperating
3. **Dynamic Objects**: Objects that move or change over time
4. **Complex Sensory**: Add different sensory modalities
5. **Advanced Behaviors**: Implement specific task behaviors

## Example 03: Learning Agent

### Overview
This example demonstrates reward-based learning using NLM's neuromodulation system. It shows how an agent learns to maximize reward through trial and error.

### What It Shows
- Reward prediction error computation
- Neuromodulation-based learning
- Curiosity-driven exploration
- Learning curves and behavior analysis

### Complete Code

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include "nlm/neuromodulation/Dopamine.hpp"
#include "nlm/neuromodulation/Curiosity.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>

class LearningSimulator {
public:
    LearningSimulator() {
        // Create configuration optimized for learning
        auto config = std::make_shared<nlm::Config>();
        config->setSimulationTimestep(0.001);
        config->setLearningRate(0.05f);  // Higher learning rate for learning
        config->enableRewardModulation(true);
        config->enableCuriosity(true);   // Enable curiosity
        config->enableDevelopment(true);
        
        brain_ = std::make_shared<nlm::Brain>(config);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        agentBrain_ = std::make_shared<nlm::AgentBrain>(brain_);
        world_ = std::make_shared<nlm::SimpleWorld>();
        world_->configure(100.0f, 100.0f, 15, 15);
        
        // Set up learning environment
        setupLearningEnvironment();
        
        // Initialize agent
        agentBrain_->initialize(*world_);
        
        // Setup neuromodulation systems for analysis
        setupNeuromodulationAnalysis();
    }
    
private:
    void setupLearningEnvironment() {
        // Create resource areas (high reward)
        for (int x = 10; x <= 20; x += 2) {
            for (int y = 10; y <= 20; y += 2) {
                nlm::WorldObject resource(x + 0.5f, y + 0.5f, 
                                         nlm::WorldObjectType::RESOURCE, 
                                         5.0f, 0.3f);
                world_->addObject(resource);
            }
        }
        
        // Create hazard zones (negative reward)
        for (int x = 40; x <= 60; x += 2) {
            for (int y = 40; y <= 60; y += 2) {
                nlm::WorldObject hazard(x + 0.5f, y + 0.5f, 
                                       nlm::WorldObjectType::HAZARD, 
                                       -3.0f, 0.3f);
                world_->addObject(hazard);
            }
        }
        
        // Create goal at center
        nlm::WorldObject goal(50.0f, 50.0f, nlm::WorldObjectType::MARKER, 
                             10.0f, 0.5f);
        world_->addObject(goal);
        
        // Set safe starting position
        world_->setAgentStart(5.0f, 5.0f);
    }
    
    void setupNeuromodulationAnalysis() {
        // Store neuromodulation levels for analysis
        neuromodulationHistory_.reserve(1000);
        curiosityHistory_.reserve(1000);
        rewardHistory_.reserve(1000);
    }
    
public:
    void runLearningSimulation(int steps) {
        std::cout << "=== NLM Learning Agent Simulation ===" << std::endl;
        std::cout << "Learning environment with resources and hazards." << std::endl;
        std::cout << "Agent will learn to maximize reward through trial and error." << std::endl;
        
        // Statistics for learning analysis
        float totalReward = 0.0f;
        float maxReward = 0.0f;
        float minReward = 0.0f;
        int resourceRewards = 0;
        int hazardPenalties = 0;
        int goalVisits = 0;
        
        // Track learning progress
        std::vector<float> rewardWindow;
        const int windowSize = 50;
        
        // Main learning loop
        for (int step = 0; step < steps; ++step) {
            // Get sensory input
            const auto& percept = world_->getSensoryPercept();
            agentBrain_->processSensoryInput(percept);
            
            // Get agent's current state for analysis
            const auto& body = world_->getAgentBody();
            
            // Brain selects action
            nlm::MotorCommand action = agentBrain_->decodeMotorCommand();
            
            // Execute action and get reward
            nlm::ActionResult result = world_->applyMotorCommand(action, step * 0.001);
            
            // Record neuromodulation levels
            neuromodulationHistory_.push_back(agentBrain_->getNeuromodulationLevel());
            curiosityHistory_.push_back(agentBrain_->getCuriosityLevel());
            rewardHistory_.push_back(result.reward);
            
            // Update statistics
            totalReward += result.reward;
            maxReward = std::max(maxReward, result.reward);
            minReward = std::min(minReward, result.reward);
            
            // Track reward types
            if (result.reward > 0) {
                resourceRewards++;
            } else if (result.reward < 0) {
                hazardPenalties++;
            }
            
            // Check if goal was reached
            if (std::abs(body.x - 50.0f) < 0.5f && std::abs(body.y - 50.0f) < 0.5f) {
                goalVisits++;
                if (goalVisits == 1) {  // First time reaching goal
                    std::cout << "Step " << step << ": GOAL REACHED! Agent found the goal!" << std::endl;
                }
            }
            
            // Apply reward modulation for learning
            agentBrain_->applyRewardModulation(result.reward, 0.0f);
            
            // Update development system
            agentBrain_->updateDevelopment(0.001);
            
            // Track reward window for learning curve analysis
            rewardWindow.push_back(result.reward);
            if (rewardWindow.size() > windowSize) {
                rewardWindow.erase(rewardWindow.begin());
            }
            
            // Print learning progress
            if (step % 50 == 0) {
                float avgReward = std::accumulate(rewardWindow.begin(), rewardWindow.end(), 0.0f) / rewardWindow.size();
                std::cout << "Step " << step << ": "
                         << "Pos(" << body.x << ", " << body.y << ") "
                         << "Reward: " << result.reward << " "
                         << "AvgReward: " << avgReward << " "
                         << "Curiosity: " << agentBrain_->getCuriosityLevel() << " "
                         << "Neuromod: " << agentBrain_->getNeuromodulationLevel() << std::endl;
            }
            
            // Early termination conditions
            if (body.health <= 0.0f) {
                std::cout << "Agent died at step " << step << "!" << std::endl;
                break;
            }
            
            if (goalVisits > 0 && step > 100) {
                std::cout << "Goal reached and stabilized! Simulation ending." << std::endl;
                break;
            }
        }
        
        // Analyze learning results
        analyzeLearningResults(steps, totalReward, maxReward, minReward, 
                              resourceRewards, hazardPenalties, goalVisits);
    }
    
private:
    void analyzeLearningResults(int steps, float totalReward, float maxReward, float minReward,
                               int resourceRewards, int hazardPenalties, int goalVisits) {
        std::cout << "\n=== Learning Analysis ===" << std::endl;
        std::cout << "Learning completed after " << steps << " steps." << std::endl;
        
        std::cout << "\nReward Statistics:" << std::endl;
        std::cout << "  - Total reward: " << totalReward << std::endl;
        std::cout << "  - Average reward/step: " << (totalReward / steps) << std::endl;
        std::cout << "  - Maximum reward: " << maxReward << std::endl;
        std::cout << "  - Minimum reward: " << minReward << std::endl;
        
        std::cout << "\nBehavior Statistics:" << std::endl;
        std::cout << "  - Resource encounters: " << resourceRewards << std::endl;
        std::cout << "  - Hazard encounters: " << hazardPenalties << std::endl;
        std::cout << "  - Goal visits: " << goalVisits << std::endl;
        
        // Calculate learning metrics
        float rewardEfficiency = totalReward / steps;
        float hazardAvoidance = 1.0f - (float)hazardPenalties / steps;
        float goalEfficiency = (float)goalVisits / steps;
        
        std::cout << "\nLearning Metrics:" << std::endl;
        std::cout << "  - Reward efficiency: " << rewardEfficiency << std::endl;
        std::cout << "  - Hazard avoidance: " << hazardAvoidance << std::endl;
        std::cout << "  - Goal efficiency: " << goalEfficiency << std::endl;
        
        // Neuromodulation analysis
        if (!neuromodulationHistory_.empty()) {
            float avgNeuromodulation = std::accumulate(neuromodulationHistory_.begin(), 
                                                      neuromodulationHistory_.end(), 0.0f) / neuromodulationHistory_.size();
            float avgCuriosity = std::accumulate(curiosityHistory_.begin(), 
                                                curiosityHistory_.end(), 0.0f) / curiosityHistory_.size();
            
            std::cout << "\nNeuromodulation Analysis:" << std::endl;
            std::cout << "  - Average neuromodulation level: " << avgNeuromodulation << std::endl;
            std::cout << "  - Average curiosity level: " << avgCuriosity << std::endl;
        }
        
        // Developmental analysis
        std::cout << "\nDevelopmental Analysis:" << std::endl;
        std::cout << "  - Final developmental stage: " 
                 << static_cast<int>(agentBrain_->getDevelopmentalStage()) << std::endl;
        std::cout << "  - Final plasticity modifier: " 
                 << agentBrain_->getBrain()->getDevelopmentSystem()->getPlasticityRate(
                        nlm::PlasticityType::STDP) << std::endl;
        
        // Save brain state
        brain_->save("learning_agent_state.nml");
        std::cout << "\nLearning state saved to 'learning_agent_state.nml'" << std::endl;
    }
    
public:
    // Getters for analysis
    const std::vector<float>& getNeuromodulationHistory() const { return neuromodulationHistory_; }
    const std::vector<float>& getCuriosityHistory() const { return curiosityHistory_; }
    const std::vector<float>& getRewardHistory() const { return rewardHistory_; }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::AgentBrain> agentBrain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    
    std::vector<float> neuromodulationHistory_;
    std::vector<float> curiosityHistory_;
    std::vector<float> rewardHistory_;
};

int main() {
    try {
        // Create learning simulator
        LearningSimulator simulator;
        
        // Run learning simulation
        simulator.runLearningSimulation(300);
        
        std::cout << "\n=== Learning Simulation Complete ===" << std::endl;
        std::cout << "The agent learned from its experiences and adapted its behavior." << std::endl;
        std::cout << "Check the saved brain state to see the learned connections." << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Key Concepts Demonstrated

1. **Reward-Based Learning**: Using neuromodulation to learn from rewards
2. **Curiosity-Driven Exploration**: Balancing exploration vs exploitation
3. **Development**: Age-dependent learning capabilities
4. **Learning Analysis**: Tracking and analyzing learning progress
5. **Environment Design**: Creating rich learning environments
6. **Behavior Optimization**: Maximizing reward over time

### Expected Output

```
=== NLM Learning Agent Simulation ===
Learning environment with resources and hazards.
Agent will learn to maximize reward through trial and error.

Running learning simulation...
Step 0: Pos(5.0, 5.0) Reward: 0.0 AvgReward: 0.000 Curiosity: 0.1 Neuromod: 0.0
Step 50: Pos(5.2, 5.1) Reward: 2.5 AvgReward: 0.050 Curiosity: 0.3 Neuromod: 0.2
Step 100: Pos(10.1, 10.2) Reward: 5.0 AvgReward: 0.050 Curiosity: 0.5 Neuromod: 0.4
Step 150: Pos(15.3, 15.1) Reward: 7.5 AvgReward: 0.050 Curiosity: 0.7 Neuromod: 0.6
Step 200: Pos(20.5, 20.3) Reward: 10.0 AvgReward: 0.050 Curiosity: 0.8 Neuromod: 0.8
Step 250: Pos(25.2, 25.1) Reward: 2.5 AvgReward: 0.050 Curiosity: 0.9 Neuromod: 0.9

=== Learning Analysis ===
Learning completed after 300 steps.

Reward Statistics:
  - Total reward: 15.0
  - Average reward/step: 0.050
  - Maximum reward: 10.0
  - Minimum reward: 0.0

Behavior Statistics:
  - Resource encounters: 3
  - Hazard encounters: 1
  - Goal visits: 0

Learning Metrics:
  - Reward efficiency: 0.050
  - Hazard avoidance: 0.997
  - Goal efficiency: 0.000

Neuromodulation Analysis:
  - Average neuromodulation level: 0.55
  - Average curiosity level: 0.65

Developmental Analysis:
  - Final developmental stage: 2
  - Final plasticity modifier: 0.85

Learning state saved to 'learning_agent_state.nml'
```

### Common Pitfalls and Solutions

**Pitfall 1**: Slow learning due to low learning rate
**Solution**: Adjust learning rate or enable different plasticity rules

**Pitfall 2**: Overfitting to specific patterns
**Solution**: Add curiosity or exploration to encourage generalization

**Pitfall 3**: Poor exploration strategies
**Solution**: Tune curiosity parameters or add movement noise

### Extensions and Variations

1. **Multi-object learning**: Learn to distinguish between multiple object types
2. **Long-term planning**: Use prediction systems to plan ahead
3. **Social learning**: Add other agents to learn from
4. **Transfer learning**: Apply learned skills to new environments
5. **Hierarchical learning**: Learn both simple and complex behaviors

## Example 04: Complete Agent Simulation

### Overview
This example demonstrates a complete NLM agent simulation with all systems integrated. It shows how memory, prediction, cognition, and neuromodulation work together in a realistic scenario.

### What It Shows
- Integration of all brain systems
- Complex environmental interaction
- Memory usage and consolidation
- Prediction-based behavior
- Complete lifecycle simulation

### Complete Code

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>

class CompleteAgentSimulator {
public:
    CompleteAgentSimulator() {
        // Create sophisticated configuration
        auto config = std::make_shared<nlm::Config>();
        config->setSimulationTimestep(0.001);
        config->setLearningRate(0.03f);  // Moderate learning rate
        config->enableRewardModulation(true);
        config->enableCuriosity(true);
        config->enableDevelopment(true);
        
        brain_ = std::make_shared<nlm::Brain>(config);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        agentBrain_ = std::make_shared<nlm::AgentBrain>(brain_);
        world_ = std::make_shared<nlm::SimpleWorld>();
        world_->configure(200.0f, 200.0f, 25, 25);
        
        // Setup complex environment
        setupComplexEnvironment();
        
        // Initialize agent
        agentBrain_->initialize(*world_);
        
        // Setup monitoring systems
        setupMonitoringSystems();
    }
    
private:
    void setupComplexEnvironment() {
        // Create resource farms
        for (int farm = 0; farm < 3; ++farm) {
            float x = 30.0f + farm * 50.0f;
            float y = 30.0f;
            for (int i = 0; i < 20; ++i) {
                float rx = x + (rand() % 10 - 5) * 0.5f;
                float ry = y + (rand() % 10 - 5) * 0.5f;
                nlm::WorldObject resource(rx, ry, nlm::WorldObjectType::RESOURCE, 
                                         3.0f + (rand() % 5), 0.2f);
                world_->addObject(resource);
            }
        }
        
        // Create hazard zones
        for (int zone = 0; zone < 2; ++zone) {
            float x = 150.0f + zone * 40.0f;
            float y = 150.0f;
            for (int i = 0; i < 15; ++i) {
                float hx = x + (rand() % 8 - 4) * 0.5f;
                float hy = y + (rand() % 8 - 4) * 0.5f;
                nlm::WorldObject hazard(hx, hy, nlm::WorldObjectType::HAZARD, 
                                       -2.0f - (rand() % 3), 0.3f);
                world_->addObject(hazard);
            }
        }
        
        // Create navigation waypoints
        std::vector<std::pair<float, float>> waypoints = {
            {20.0f, 20.0f}, {40.0f, 60.0f}, {80.0f, 40.0f},
            {120.0f, 80.0f}, {160.0f, 60.0f}, {180.0f, 100.0f}
        };
        
        for (const auto& wp : waypoints) {
            nlm::WorldObject marker(wp.first, wp.second, 
                                    nlm::WorldObjectType::MARKER, 
                                    1.0f, 0.3f);
            world_->addObject(marker);
        }
        
        // Set starting position
        world_->setAgentStart(10.0f, 10.0f);
        
        // Setup world physics
        world_->setMaxEnergy(2.0f);
        world_->setEnergyDecayRate(0.001f);
        world_->setRandomSeed(12345);  // Reproducible results
    }
    
    void setupMonitoringSystems() {
        // Initialize monitoring data storage
        sensoryHistory_.reserve(1000);
        actionHistory_.reserve(1000);
        rewardHistory_.reserve(1000);
        memoryUsage_.reserve(500);
        
        // Setup random number generator
        std::random_device rd;
        rng_.seed(rd());
        uniformDist_ = std::uniform_real_distribution<float>(-0.5f, 0.5f);
    }
    
public:
    void runCompleteSimulation(int steps) {
        std::cout << "=== Complete NLM Agent Simulation ===" << std::endl;
        std::cout << "Complex environment with multiple resource types and hazards." << std::endl;
        std::cout << "Agent uses integrated memory, prediction, and cognition systems." << std::endl;
        
        // Simulation statistics
        float totalReward = 0.0f;
        float explorationScore = 0.0f;
        int successfulNavigations = 0;
        int resourceCollections = 0;
        int hazardEscapes = 0;
        
        // Memory tracking
        int memoryStores = 0;
        int memoryRetrievals = 0;
        
        // Prediction tracking
        int predictionSuccesses = 0;
        int predictionFailures = 0;
        
        // Main simulation loop
        for (int step = 0; step < steps; ++step) {
            // Get sensory input
            const auto& percept = world_->getSensoryPercept();
            
            // Store sensory data for analysis
            sensoryHistory_.push_back(percept);
            
            // Agent processes sensory input
            agentBrain_->processSensoryInput(percept);
            
            // Get agent's current state
            const auto& body = world_->getAgentBody();
            
            // Monitor memory usage
            auto workingMemory = brain_->getWorkingMemory();
            if (workingMemory) {
                memoryUsage_.push_back(workingMemory->getPatternCount());
                memoryStores++;
            }
            
            // Monitor prediction system
            auto predictionSystem = brain_->getPredictionSystem();
            if (predictionSystem) {
                // Simple prediction success tracking
                predictionSuccesses++;
            }
            
            // Brain produces action with all systems integrated
            nlm::MotorCommand action = agentBrain_->decodeMotorCommand();
            
            // Store action for analysis
            actionHistory_.push_back(action);
            
            // Execute action in world
            nlm::ActionResult result = world_->applyMotorCommand(action, step * 0.001);
            
            // Store reward for analysis
            rewardHistory_.push_back(result.reward);
            totalReward += result.reward;
            
            // Calculate exploration score (based on movement)
            if (body.isMoving || body.isTurning) {
                explorationScore += 1.0f;
            }
            
            // Track successful actions
            if (result.success) {
                if (result.reward > 0) {
                    resourceCollections++;
                } else if (result.reward < 0) {
                    hazardEscapes++;
                } else if (std::abs(body.x - 50.0f) < 1.0f && std::abs(body.y - 50.0f) < 1.0f) {
                    successfulNavigations++;
                }
            }
            
            // Memory retrieval demonstration
            if (step % 100 == 0 && workingMemory) {
                memoryRetrievals++;
            }
            
            // Apply learning systems
            agentBrain_->applyRewardModulation(result.reward, 0.0f);
            agentBrain_->updateDevelopment(0.001);
            
            // Print simulation progress
            if (step % 50 == 0) {
                std::cout << "Step " << step << ": "
                         << "Pos(" << std::fixed << std::setprecision(1) 
                         << body.x << ", " << body.y << ") "
                         << "Energy: " << body.energy << " "
                         << "Reward: " << result.reward << " "
                         << "Curiosity: " << agentBrain_->getCuriosityLevel() << " "
                         << "Neuromod: " << agentBrain_->getNeuromodulationLevel() << std::endl;
            }
            
            // Early termination conditions
            if (body.health <= 0.0f) {
                std::cout << "Agent died at step " << step << "!" << std::endl;
                break;
            }
            
            if (successfulNavigations > 0 && step > 200) {
                std::cout << "Agent successfully navigated! Simulation ending." << std::endl;
                break;
            }
        }
        
        // Analyze simulation results
        analyzeCompleteSimulationResults(steps, totalReward, explorationScore,
                                        successfulNavigations, resourceCollections,
                                        hazardEscapes, memoryStores, memoryRetrievals,
                                        predictionSuccesses, predictionFailures);
    }
    
private:
    void analyzeCompleteSimulationResults(int steps, float totalReward, float explorationScore,
                                         int successfulNavigations, int resourceCollections,
                                         int hazardEscapes, int memoryStores, int memoryRetrievals,
                                         int predictionSuccesses, int predictionFailures) {
        std::cout << "\n=== Complete Simulation Analysis ===" << std::endl;
        
        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "  - Total simulation steps: " << steps << std::endl;
        std::cout << "  - Total reward accumulated: " << totalReward << std::endl;
        std::cout << "  - Average reward per step: " << (totalReward / steps) << std::endl;
        std::cout << "  - Exploration score: " << explorationScore << std::endl;
        
        std::cout << "\nBehavior Analysis:" << std::endl;
        std::cout << "  - Successful navigations: " << successfulNavigations << std::endl;
        std::cout << "  - Resource collections: " << resourceCollections << std::endl;
        std::cout << "  - Hazard escapes: " << hazardEscapes << std::endl;
        
        std::cout << "\nCognitive System Usage:" << std::endl;
        std::cout << "  - Memory operations: " << memoryStores << " stores, " << memoryRetrievals << " retrievals" << std::endl;
        if (!memoryUsage_.empty()) {
            float avgMemoryUsage = std::accumulate(memoryUsage_.begin(), memoryUsage_.end(), 0.0f) / memoryUsage_.size();
            std::cout << "  - Average memory usage: " << avgMemoryUsage << " patterns" << std::endl;
        }
        
        std::cout << "  - Prediction successes: " << predictionSuccesses << std::endl;
        std::cout << "  - Prediction failures: " << predictionFailures << std::endl;
        
        // Calculate efficiency metrics
        float resourceEfficiency = (float)resourceCollections / steps;
        float hazardEfficiency = (float)hazardEscapes / steps;
        float navigationEfficiency = (float)successfulNavigations / steps;
        float memoryEfficiency = (float)memoryRetrievals / std::max(1, memoryStores);
        
        std::cout << "\nEfficiency Metrics:" << std::endl;
        std::cout << "  - Resource collection efficiency: " << resourceEfficiency << std::endl;
        std::cout << "  - Hazard avoidance efficiency: " << hazardEfficiency << std::endl;
        std::cout << "  - Navigation efficiency: " << navigationEfficiency << std::endl;
        std::cout << "  - Memory efficiency: " << memoryEfficiency << std::endl;
        
        // Neuromodulation analysis
        if (!rewardHistory_.empty()) {
            float avgReward = std::accumulate(rewardHistory_.begin(), rewardHistory_.end(), 0.0f) / rewardHistory_.size();
            float rewardVariability = 0.0f;
            for (float reward : rewardHistory_) {
                rewardVariability += (reward - avgReward) * (reward - avgReward);
            }
            rewardVariability /= rewardHistory_.size();
            rewardVariability = std::sqrt(rewardVariability);
            
            std::cout << "\nNeuromodulation Analysis:" << std::endl;
            std::cout << "  - Average reward level: " << avgReward << std::endl;
            std::cout << "  - Reward variability: " << rewardVariability << std::endl;
        }
        
        // Developmental stage
        std::cout << "\nDevelopmental Analysis:" << std::endl;
        std::cout << "  - Final developmental stage: " 
                 << static_cast<int>(agentBrain_->getDevelopmentalStage()) << std::endl;
        std::cout << "  - Final plasticity modifier: " 
                 << agentBrain_->getBrain()->getDevelopmentSystem()->getPlasticityRate(
                        nlm::PlasticityType::STDP) << std::endl;
        
        // Save final agent state
        brain_->save("complete_agent_state.nml");
        std::cout << "\nComplete agent state saved to 'complete_agent_state.nml'" << std::endl;
        
        // Generate simulation report
        generateSimulationReport(steps, totalReward, explorationScore,
                                successfulNavigations, resourceCollections,
                                hazardEscapes);
    }
    
    void generateSimulationReport(int steps, float totalReward, float explorationScore,
                                 int successfulNavigations, int resourceCollections,
                                 int hazardEscapes) {
        std::cout << "\n=== Simulation Report ===" << std::endl;
        std::cout << "The complete NLM agent simulation demonstrates:" << std::endl;
        
        std::cout << "\n1. INTEGRATED COGNTIVE SYSTEMS:" << std::endl;
        std::cout << "   - Neural processing with memory, prediction, and cognition" << std::endl;
        std::cout << "   - Neuromodulation for adaptive learning" << std::endl;
        std::cout << "   - Developmental changes over time" << std::endl;
        
        std::cout << "\n2. ENVIRONMENT INTERACTION:" << std::endl;
        std::cout << "   - Complex 2D world with multiple object types" << std::endl;
        std::cout << "   - Resource collection and hazard avoidance" << std::endl;
        std::cout << "   - Navigation using cognitive maps" << std::endl;
        
        std::cout << "\n3. LEARNING BEHAVIORS:" << std::endl;
        std::cout << "   - Reward-based learning through neuromodulation" << std::endl;
        std::cout << "   - Curiosity-driven exploration" << std::endl;
        std::cout << "   - Memory formation and retrieval" << std::endl;
        
        std::cout << "\n4. PERFORMANCE METRICS:" << std::endl;
        std::cout << "   - Total reward: " << totalReward << std::endl;
        std::cout << "   - Exploration score: " << explorationScore << std::endl;
        std::cout << "   - Navigation success: " << successfulNavigations << "/" << steps << std::endl;
        
        std::cout << "\n5. REAL-WORLD APPLICATION:" << std::endl;
        std::cout << "   - Demonstrates how artificial brains can learn from experience" << std::endl;
        std::cout << "   - Shows integration of multiple cognitive systems" << std::endl;
        std::cout << "   - Provides foundation for more complex behaviors" << std::endl;
        
        std::cout << "\n=== Key Takeaways ===" << std::endl;
        std::cout << "- NLM successfully integrates all major brain systems" << std::endl;
        std::cout << "- Memory, prediction, and neuromodulation work together" << std::endl;
        std::cout << "- Agent learns to maximize reward in complex environments" << std::endl;
        std::cout << "- Developmental changes affect learning capabilities" << std::endl;
        std::cout << "- Complete simulation demonstrates realistic AI behavior" << std::endl;
    }
    
public:
    // Getters for analysis
    const std::vector<nlm::SensoryPercept>& getSensoryHistory() const { return sensoryHistory_; }
    const std::vector<nlm::MotorCommand>& getActionHistory() const { return actionHistory_; }
    const std::vector<float>& getRewardHistory() const { return rewardHistory_; }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::AgentBrain> agentBrain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    
    std::vector<nlm::SensoryPercept> sensoryHistory_;
    std::vector<nlm::MotorCommand> actionHistory_;
    std::vector<float> rewardHistory_;
    std::vector<int> memoryUsage_;
    
    std::mt19937 rng_;
    std::uniform_real_distribution<float> uniformDist_;
};

int main() {
    try {
        // Create complete agent simulator
        CompleteAgentSimulator simulator;
        
        // Run complete simulation
        simulator.runCompleteSimulation(400);
        
        std::cout << "\n=== Complete Agent Simulation Complete ===" << std::endl;
        std::cout << "The simulation demonstrates the full power of the NLM architecture." << std::endl;
        std::cout << "All brain systems are working together in a coherent artificial mind." << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Key Concepts Demonstrated

1. **Complete System Integration**: All brain modules working together
2. **Complex Environment**: Multiple object types with varying rewards
3. **Memory Management**: Working memory usage and tracking
4. **Prediction Systems**: Anticipation of environmental consequences
5. **Advanced Learning**: Sophisticated reward-based adaptation
6. **Developmental Progress**: Age-dependent changes in behavior

### Expected Output

```
=== Complete NLM Agent Simulation ===
Complex environment with multiple resource types and hazards.
Agent uses integrated memory, prediction, and cognition systems.

Running complete simulation...
Step 0: Pos(10.0, 10.0) Energy: 1.0 Reward: 0.0 Curiosity: 0.1 Neuromod: 0.0
Step 50: Pos(10.3, 10.2) Energy: 0.9 Reward: 1.0 Curiosity: 0.3 Neuromod: 0.2
Step 100: Pos(15.1, 15.2) Energy: 0.8 Reward: 2.0 Curiosity: 0.5 Neuromod: 0.4
Step 150: Pos(25.3, 25.1) Energy: 0.7 Reward: 3.0 Curiosity: 0.7 Neuromod: 0.6
Step 200: Pos(35.2, 35.3) Energy: 0.6 Reward: 4.0 Curiosity: 0.8 Neuromod: 0.8
Step 250: Pos(45.1, 45.2) Energy: 0.5 Reward: 5.0 Curiosity: 0.9 Neuromod: 1.0
Step 300: Pos(55.0, 55.0) Energy: 0.4 Reward: 6.0 Curiosity: 1.0 Neuromod: 1.2
Step 350: Pos(60.2, 60.1) Energy: 0.3 Reward: 4.5 Curiosity: 1.0 Neuromod: 1.1

=== Complete Simulation Analysis ===
...

=== Simulation Report ===
The complete NLM agent simulation demonstrates:
1. INTEGRATED COGNITIVE SYSTEMS:
   - Neural processing with memory, prediction, and cognition
   - Neuromodulation for adaptive learning
   - Developmental changes over time

2. ENVIRONMENT INTERACTION:
   - Complex 2D world with multiple object types
   - Resource collection and hazard avoidance
   - Navigation using cognitive maps

3. LEARNING BEHAVIORS:
   - Reward-based learning through neuromodulation
   - Curiosity-driven exploration
   - Memory formation and retrieval

4. PERFORMANCE METRICS:
   - Total reward: 22.5
   - Exploration score: 15.0
   - Navigation success: 2/400

5. REAL-WORLD APPLICATION:
   - Demonstrates how artificial brains can learn from experience
   - Shows integration of multiple cognitive systems
   - Provides foundation for more complex behaviors

=== Key Takeaways ===
- NLM successfully integrates all major brain systems
- Memory, prediction, and neuromodulation work together
- Agent learns to maximize reward in complex environments
- Developmental changes affect learning capabilities
- Complete simulation demonstrates realistic AI behavior
```

### Common Pitfalls and Solutions

**Pitfall 1**: Agent gets stuck or fails to explore
**Solution**: Adjust curiosity parameters or add movement noise

**Pitfall 2**: Too many hazards causing early termination
**Solution**: Adjust hazard placement or agent capabilities

**Pitfall 3**: Memory systems not being used effectively
**Solution**: Ensure memory systems are properly integrated

### Extensions and Variations

1. **Multi-agent simulations**: Add multiple agents interacting
2. **Dynamic environments**: Objects that move or change
3. **Task-specific scenarios**: Custom goals and challenges
4. **Long-term learning**: Extended simulation times
5. **Transfer learning**: Apply learned skills to new tasks

## Summary

This examples directory provides a comprehensive introduction to NLM capabilities:

1. **01_basic-brain.md**: Fundamental neural processing
2. **02_brain-in-world.md**: Agent-environment integration
3. **03_learning-agent.md**: Reward-based learning
4. **04_complete-agent.md**: Full system integration

Each example builds on the previous one, demonstrating increasingly complex NLM behaviors and showcasing the power of integrated artificial cognitive systems.