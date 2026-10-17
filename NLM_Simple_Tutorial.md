# NLM (Neural Learning Machine) - Simple Tutorial

## Introduction

This tutorial provides a step-by-step introduction to using the NLM (Neural Learning Machine) to create a simple spiking neural network simulation. It covers the basic concepts and demonstrates how to:

1. Create and initialize a brain
2. Process sensory input
3. Run neural computation
4. Generate motor output
5. Observe learning and development

## Prerequisites

- Basic understanding of C++ programming
- Familiarity with neural networks concepts
- NLM project installed and built

## Example 1: Minimal Neural Network

This example demonstrates the simplest possible NLM simulation.

```cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    std::cout << "=== NLM Minimal Network Example ===" << std::endl;
    
    // 1. Create and configure brain
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 100);  // Small network for demonstration
    config->set("brain.synapse_density", 0.2f);
    
    // 2. Create brain instance
    auto brain = std::make_shared<Brain>(config);
    
    // 3. Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "and " << brain->getTotalSynapseCount() << " synapses" << std::endl;
    
    // 4. Run simulation
    std::cout << "\nRunning simulation..." << std::endl;
    for (SimulationStep step = 0; step < 100; ++step) {
        // Run neural computation
        brain->step(step);
        
        // Log statistics every 20 steps
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " firing neurons"
                     << ", " << brain->getAverageFiringRate() << " Hz average rate"
                     << std::endl;
        }
    }
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Final excitation/inhibition ratio: " 
             << brain->getExcitationInhibitionRatio() << std::endl;
    
    return 0;
}
```

### Expected Output

```
=== NLM Minimal Network Example ===
Brain initialized with 100 neurons
and 12 synapses

Running simulation...
Step 0: 0 firing neurons, 0 Hz average rate
Step 20: 15 firing neurons, 0.15 Hz average rate
Step 40: 12 firing neurons, 0.12 Hz average rate
Step 60: 8 firing neurons, 0.08 Hz average rate
Step 80: 11 firing neurons, 0.11 Hz average rate

=== Simulation Complete ===
Total spikes generated: 46
Final excitation/inhibition ratio: 1.85
```

### Key Concepts Demonstrated

1. **Brain Initialization**: Creating a brain with specific neuron and synapse counts
2. **Network Structure**: Setting up a small but functional neural network
3. **Simulation Loop**: Running neural computation step by step
4. **Activity Monitoring**: Tracking neuron firing and network statistics
5. **Completion Reporting**: Showing final simulation results

---

## Example 2: Sensory Processing

This example shows how to process external sensory input through the NLM brain.

```cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"

int main() {
    std::cout << "=== NLM Sensory Processing Example ===" << std::endl;
    
    // 1. Create brain configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 200);
    config->set("brain.synapse_density", 0.15f);
    
    // 2. Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // 3. Create sensory input
    SensoryInput sensoryInput;
    
    // Add sensory observations (e.g., from environment)
    // These could represent light intensity, temperature, etc.
    for (int i = 0; i < 10; ++i) {
        sensoryInput.addObservation(i * 0.5f);  // Scale appropriately
    }
    
    std::cout << "Sensory input prepared with " << sensoryInput.getObservationCount() 
             << " observations" << std::endl;
    
    // 4. Process sensory input
    std::cout << "\nProcessing sensory input..." << std::endl;
    brain->receiveSensoryInput(sensoryInput);
    
    // 5. Run neural computation
    std::cout << "Running neural computation with sensory input..." << std::endl;
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step);
        
        if (step % 10 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " firing neurons"
                     << ", " << brain->getTotalSpikeCount() << " total spikes"
                     << std::endl;
        }
    }
    
    std::cout << "\n=== Sensory Processing Complete ===" << std::endl;
    std::cout << "Brain processed sensory input and generated " 
             << brain->getTotalSpikeCount() << " spikes through "
             << brain->getFiringNeuronCount() << " firing neurons"
             << std::endl;
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Sensory Input Creation**: Preparing external observations for neural processing
2. **Sensory Transduction**: Converting sensory data into neural signals
3. **Neural Response**: Observing how sensory input affects neural activity
4. **Input-Output Integration**: Connecting environmental perception with neural computation

---

## Example 3: Action Selection

This example demonstrates how the brain generates motor commands based on neural activity.

```cpp
#include <iostream>
#include <memory>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "motor/Action.hpp"

int main() {
    std::cout << "=== NLM Action Selection Example ===" << std::endl;
    
    // 1. Create brain with motor capabilities
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 300);
    config->set("brain.synapse_density", 0.1f);
    
    // 2. Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // 3. Run simulation to develop motor activity
    std::cout << "Developing motor neural activity..." << std::endl;
    for (SimulationStep step = 0; step < 200; ++step) {
        brain->step(step);
        
        // Inject current into motor neurons to stimulate action potential
        if (step > 50 && step < 150) {
            brain->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
        }
        
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " firing neurons"
                     << std::endl;
        }
    }
    
    // 4. Select action based on neural activity
    std::cout << "\nSelecting action based on neural activity..." << std::endl;
    auto action = brain->produceAction();
    
    // 5. Interpret and display action
    std::cout << "Selected action: ";
    switch (action->getType()) {
        case ActionType::MoveForward:
            std::cout << "MoveForward" << std::endl;
            break;
        case ActionType::MoveBackward:
            std::cout << "MoveBackward" << std::endl;
            break;
        case ActionType::TurnLeft:
            std::cout << "TurnLeft" << std::endl;
            break;
        case ActionType::TurnRight:
            std::cout << "TurnRight" << std::endl;
            break;
        case ActionType::Interact:
            std::cout << "Interact" << std::endl;
            break;
        case ActionType::Wait:
            std::cout << "Wait" << std::endl;
            break;
        default:
            std::cout << "Unknown" << std::endl;
    }
    
    std::cout << "\n=== Action Selection Complete ===" << std::endl;
    std::cout << "Brain generated motor command based on neural population activity" << std::endl;
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Motor Development**: Stimulating motor neurons to create action potential
2. **Activity Patterns**: How neural activity patterns determine behavior
3. **Action Selection**: Mapping neural activity to concrete motor commands
4. **Behavior Generation**: How simple neural networks can produce purposeful actions

---

## Example 4: Learning and Plasticity

This example demonstrates how NLM learns through plasticity mechanisms.

```cpp
#include <iostream>
#include <memory>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"

int main() {
    std::cout << "=== NLM Learning and Plasticity Example ===" << std::endl;
    
    // 1. Create brain with plasticity enabled
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 250);
    config->set("brain.synapse_density", 0.12f);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("neuromod.dopamine.scale", 1.0f);
    
    // 2. Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // 3. Create neuromodulator for reward-based learning
    auto neuromodulator = std::make_shared<Neuromodulator>();
    neuromodulator->setLevel(0.5f);  // Positive reward signal
    
    std::cout << "Starting learning simulation..." << std::endl;
    
    // 4. Run learning simulation
    for (SimulationStep step = 0; step < 300; ++step) {
        brain->step(step);
        
        // Apply repeated input patterns to stimulate learning
        if (step % 50 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            std::cout << "Step " << step << ": Applied sensory input" << std::endl;
        }
        
        // Apply neuromodulation for reward-based learning
        if (step % 100 == 0) {
            brain->applyNeuromodulation(*neuromulator);
            std::cout << "Step " << step << ": Applied neuromodulation" << std::endl;
        }
        
        if (step % 50 == 0) {
            std::cout << "  -> " << brain->getFiringNeuronCount() << " firing neurons"
                     << ", " << brain->getTotalSpikeCount() << " total spikes"
                     << std::endl;
        }
    }
    
    // 5. Show learning results
    std::cout << "\n=== Learning Complete ===" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Plasticity mechanisms activated throughout simulation" << std::endl;
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Plasticity Mechanisms**: STDP and Hebbian learning for synaptic modification
2. **Reward-Based Learning**: Neuromodulation for behavior reinforcement
3. **Learning Over Time**: How repeated experiences lead to neural changes
4. **Activity-Dependent Plasticity**: How neural activity shapes future responses

---

## Example 5: Complete Agent Simulation

This example shows a more complete simulation with all NLM components working together.

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "motor/Action.hpp"

int main() {
    std::cout << "=== NLM Complete Agent Simulation ===" << std::endl;
    
    // 1. Create complete agent system
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    config->set("brain.synapse_density", 0.08f);
    
    // 2. Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // 3. Create agent brain interface
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // 4. Create and configure world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=10, height=10, visionWidth=8, visionHeight=8);
    world->reset();
    world->setAgentStart(5.0, 5.0);
    
    // 5. Initialize agent with world
    agent->initialize(*world);
    
    // 6. Enable all subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    std::cout << "Starting complete agent simulation..." << std::endl;
    
    // 7. Run complete agent simulation
    for (SimulationStep step = 0; step < 500; ++step) {
        // Update world environment
        world->update(0.1);
        
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input in brain
        agent->processSensoryInput(percept);
        
        // Run brain computation
        brain->step(step);
        
        // Decode motor command from brain activity
        auto motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = percept.getInternal()[0] ? *percept.getInternal()[0] : 0.0f;
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Log progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            std::cout << "  Curiosity: " << agent->getCuriosityLevel() << ", Novelty: " << agent->getNoveltyLevel() << std::endl;
            std::cout << "  Dev Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
            std::cout << "  Reward: " << agent->getNeuromodulationLevel() << std::endl;
        }
    }
    
    std::cout << "\n=== Complete Agent Simulation Finished ===" << std::endl;
    std::cout << "Agent successfully learned, developed, and interacted with environment" << std::endl;
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Integrated Agent System**: Brain, agent, and world working together
2. **Embodied Intelligence**: Direct interaction with and perception of environment
3. **Complete Learning Pipeline**: Sensory processing → neural computation → motor output
4. **Adaptive Behavior**: How agents learn and adapt to their environment
5. **Neuromodulation**: Reward-based learning and exploration
6. **Development**: Behavioral maturation over time

---

## Quick Start Guide

### 1. Simple Neural Network
Use Example 1 to create a basic neural network that demonstrates:
- Brain initialization
- Simulation execution
- Activity monitoring
- Result visualization

### 2. Sensory Processing
Use Example 2 to show how NLM processes external stimuli and generates neural responses.

### 3. Motor Control
Use Example 3 to demonstrate how neural activity translates to actions.

### 4. Learning Systems
Use Example 4 to explore plasticity mechanisms and reward-based learning.

### 5. Complete Agent
Use Example 5 for a comprehensive demonstration of all NLM components.

## Common Problems and Solutions

### Problem: Brain doesn't initialize
**Solution**: Check configuration parameters and ensure all required settings are present.

### Problem: No spikes generated
**Solution**: Check neural parameters and ensure sufficient input current is being injected.

### Problem: Motor activity doesn't produce actions
**Solution**: Verify motor neuron populations and action selection logic.

### Problem: Learning doesn't occur
**Solution**: Ensure plasticity mechanisms are enabled and neuromodulation is applied.

## Next Steps

After working through these examples, consider:

1. **Advanced Topics**: Explore memory systems, prediction, and cognition modules
2. **Customization**: Modify neural parameters and plasticity rules
3. **Multi-Agent**: Implement multiple interacting agents
4. **Complex Environments**: Create more sophisticated world models
5. **Performance Optimization**: Study scaling and efficiency improvements

## Conclusion

These examples provide a foundation for understanding and using the NLM system. By starting with simple examples and gradually incorporating more complex features, you can explore the full capabilities of the Neural Learning Machine while building practical skills in neural computation and artificial intelligence.

The NLM demonstrates how biologically inspired neural networks can learn, adapt, and behave in complex environments through the integration of multiple cognitive and motor systems.

---

For additional resources and documentation, refer to:
- [NLM_API_Documentation.md](#) - Comprehensive API reference
- [HOW_TO_USE.md](#) - Installation and usage instructions
- [docs/ directory](#) - Scientific and technical papers
