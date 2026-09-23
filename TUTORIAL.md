# NLM Neural Simulation Framework - Tutorial Guide

## Overview

This tutorial guide provides step-by-step instructions for getting started with the NLM (Neural Learning Machine) neural simulation framework. It covers basic concepts, setup procedures, and example simulations to help users quickly understand and use the framework effectively.

## Prerequisites

Before starting with NLM, ensure you have:

### 1. Development Environment
- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 19+)
- **Build Tools**: CMake 3.10 or higher
- **Memory**: At least 4 GB RAM (8 GB recommended for larger simulations)
- **Storage**: At least 100 MB free space for build artifacts

### 2. Project Setup
```bash
# Clone the NLM repository
cd /path/to
-git clone https://github.com/your-username/nlm-framework.git

cd nlm-framework
```

### 3. Build Instructions
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the framework
cmake --build . -- -j$(nproc)  # Linux/macOS
# cmake --build . -- /m // Windows
```

## Quick Start Examples

### Example 1: Simple Brain Simulation

This example demonstrates the basic workflow of creating and simulating a simple brain.

```cpp
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/world/SimpleWorld.hpp"

int main() {
    // Step 1: Create configuration
    auto config = std::make_shared<Config>();
    config->setNeuronCount(1000);
    config->setSynapseDensity(0.1);
    config->setSimulationTimestep(0.001);
    config->setMaxSimulationTime(10.0);
    
    // Step 2: Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Step 3: Initialize brain
    bool initialized = brain->initialize();
    if (!initialized) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Step 4: Create agent interface
    auto agentBrain = std::make_shared<AgentBrain>(brain, config);
    
    // Step 5: Create world
    auto world = std::make_shared<SimpleWorld>();
    world->setDimensions(100.0, 100.0);
    
    // Step 6: Initialize agent with world
    agentBrain->initialize(*world);
    
    // Step 7: Create agent body
    auto agentBody = std::make_shared<AgentBody>();
    agentBody->setPosition(50.0, 50.0);
    agentBody->setOrientation(0.0);
    
    // Step 8: Run simulation
    std::cout << "Starting simulation..." << std::endl;
    for (int step = 0; step < 10000; ++step) {
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // Process and inject into brain
        agentBrain->processSensoryInput(percept);
        
        // Run brain simulation
        brain->step(step);
        
        // Decode motor command
        auto motorCommand = agentBrain->decodeMotorCommand();
        
        // Apply to world
        world->applyMotorCommand(motorCommand);
        
        // Update agent state
        auto actionResult = world->getLastActionResult();
        agentBody->applyMotorCommand(motorCommand, 0.001, actionResult);
        
        // Print status every 1000 steps
        if (step % 1000 == 0) {
            std::cout << "Step: " << step 
                      << " | Position: (" 
                      << agentBody->getPosition().first << ", "
                      << agentBody->getPosition().second << ")"
                      << " | Energy: " << agentBody->getEnergy()
                      << " | Health: " << agentBody->getHealth() << std::endl;
        }
    }
    
    // Step 9: Save simulation results
    brain->save("simulation_results.dat");
    std::cout << "Simulation completed successfully!" << std::endl;
    
    return 0;
}
```

### Example 2: Working with Neurons

This example demonstrates how to work with individual neurons and their properties.

```cpp
#include "src/brain/Neuron.hpp"
#include "src/core/Random/Random.hpp"

void neuronExample() {
    // Step 1: Create neuron
    NeuronId neuronId(42);
    Neuron neuron(neuronId);
    
    // Step 2: Configure neuron parameters
    neuron.setThreshold(-50.0f);           // Lower threshold = more excitable
    neuron.setLeakConductance(15.0f);      // Higher conductance = faster decay
    neuron.setRestingPotential(-65.0f);   // Different resting potential
    neuron.setType(NeuronType::Internal);  // Internal neuron type
    
    // Step 3: Initialize with random parameters
    RandomGenerator rng(12345);  // Fixed seed for reproducibility
    neuron.initializeRandom(rng);
    
    // Step 4: Inject input and simulate
    std::cout << "Initial membrane potential: " 
              << neuron.getMembranePotential() << " mV" << std::endl;
    
    // Inject excitatory input
    neuron.receiveExcitatoryInput(5.0f);
    
    // Step 5: Update neural dynamics
    neuron.step(1000, 0.001);  // timestamp, dt
    
    // Step 6: Check for spikes
    if (neuron.checkThreshold()) {
        neuron.recordSpike(1000);
        std::cout << "Neuron spiked at time 1000!" << std::endl;
    }
    
    // Step 7: Check state
    auto state = neuron.getState();
    std::cout << "Current membrane potential: " 
              << neuron.getMembranePotential() << " mV" << std::endl;
    std::cout << "Neuron firing state: " << (neuron.isFiring() ? "Firing" : "Not firing") << std::endl;
    std::cout << "Neuron refractory: " << (neuron.isRefractory() ? "Yes" : "No") << std::endl;
    
    // Step 8: Modify neuron properties
    neuron.setRefractoryPeriod(10);
    std::cout << "Refractory period set to: " << neuron.getRefractoryPeriod() << std::endl;
    
    // Step 9: Clear spike history
    neuron.clearSpikeHistory();
    std::cout << "Spike history cleared. Current spikes: " 
              << neuron.getSpikeHistory().size() << std::endl;
}
```

### Example 3: Working with Synapses

This example demonstrates how to work with synapses and their properties.

```cpp
#include "src/brain/Synapse.hpp"

void synapseExample() {
    // Step 1: Create synapse
    SynapseId synapseId(1);
    NeuronId sourceId(100);
    NeuronId targetId(200);
    
    Synapse synapse(synapseId, sourceId, targetId);
    
    // Step 2: Configure synaptic properties
    synapse.setType(SynapseType::Excitatory);
    synapse.setWeight(1.5f);    // Strong excitation
    synapse.setDelay(2);        // 2 timestep delay
    
    // Step 3: Record spike activity for plasticity
    synapse.recordPreSpike(1000);
    synapse.recordPostSpike(1002);
    
    // Step 4: Simulate synaptic transmission
    synapse.step(1002);
    
    // Step 5: Check properties
    std::cout << "Synapse ID: " << synapse.getId() << std::endl;
    std::cout << "Source neuron: " << synapse.getSourceNeuron() << std::endl;
    std::cout << "Target neuron: " << synapse.getTargetNeuron() << std::endl;
    std::cout << "Synaptic weight: " << synapse.getWeight() << std::endl;
    std::cout << "Transmission delay: " << synapse.getDelay() << std::endl;
    std::cout << "Synapse type: " << (synapse.isExcitatory() ? "Excitatory" : "Inhibitory") << std::endl;
    
    // Step 6: Check activity history
    const auto& preSpikes = synapse.getPreSpikeHistory();
    const auto& postSpikes = synapse.getPostSpikeHistory();
    
    std::cout << "Presynaptic spikes: ";
    for (auto spikeTime : preSpikes) {
        std::cout << spikeTime << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Postsynaptic spikes: ";
    for (auto spikeTime : postSpikes) {
        std::cout << spikeTime << " ";
    }
    std::cout << std::endl;
    
    // Step 7: Clear history
    synapse.clearHistory();
    std::cout << "Spike history cleared." << std::endl;
}
```

### Example 4: Agent-Environment Interaction

This example demonstrates a complete agent-environment interaction scenario.

```cpp
#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/world/SimpleWorld.hpp"

void agentEnvironmentExample() {
    // Step 1: Create components
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    auto agentBrain = std::make_shared<AgentBrain>(brain, config);
    auto agentBody = std::make_shared<AgentBody>();
    auto world = std::make_shared<SimpleWorld>();
    
    // Step 2: Configure world
    world->setDimensions(200.0, 200.0);
    world->addResource(50.0, 50.0, 10.0);    // Resource at (50, 50) with value 10
    world->addHazard(150.0, 150.0, 5.0);    // Hazard at (150, 150) with damage 5
    world->addWall(0.0, 200.0);             // Wall along left edge
    world->addWall(200.0, 0.0);             // Wall along top edge
    
    // Step 3: Initialize components
    brain->initialize();
    agentBrain->initialize(*world);
    agentBody->setPosition(10.0, 10.0);
    agentBody->setOrientation(0.0);
    
    // Step 4: Set agent goals
    std::vector<float> goal{100.0, 100.0};  // Goal position
    agentBrain->setCurrentGoal(goal);
    
    // Step 5: Run simulation loop
    std::cout << "Starting agent-environment simulation..." << std::endl;
    for (int step = 0; step < 5000; ++step) {
        // Reset agent at regular intervals
        if (step % 1000 == 0) {
            agentBrain->reset();
            agentBody->reset();
            agentBody->setPosition(10.0, 10.0);
            agentBrain->setCurrentGoal({100.0 + step % 50, 100.0 + step % 50});
            std::cout << "--- New episode ---" << std::endl;
        }
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // Run brain simulation
        brain->step(step);
        
        // Decode motor command
        auto motorCommand = agentBrain->decodeMotorCommand();
        
        // Apply action to world
        auto actionResult = world->applyMotorCommand(motorCommand);
        
        // Update agent state
        agentBody->applyMotorCommand(motorCommand, 0.001, actionResult);
        
        // Print status
        if (step % 500 == 0) {
            auto pos = agentBody->getPosition();
            float reward = actionResult.reward;
            std::cout << "Step " << step << ": Position (" 
                      << pos.first << ", " << pos.second << ") "
                      << "Reward: " << reward 
                      << " | Energy: " << agentBody->getEnergy() << std::endl;
        }
    }
    
    // Step 6: Print final statistics
    std::cout << "\nSimulation completed!" << std::endl;
    std::cout << "Final agent position: ("
              << agentBody->getPosition().first << ", "
              << agentBody->getPosition().second << ")" << std::endl;
    std::cout << "Final energy: " << agentBody->getEnergy() << std::endl;
    std::cout << "Final health: " << agentBody->getHealth() << std::endl;
    std::cout << "Final age: " << agentBody->getAge() << std::endl;
    
    // Step 7: Save results
    brain->save("agent_simulation_results.dat");
    std::cout << "Results saved to agent_simulation_results.dat" << std::endl;
}
```

## Advanced Examples

### Example 5: Custom Plasticity Configuration

This example demonstrates how to configure custom plasticity rules.

```cpp
#include "src/brain/Brain.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"

void customPlasticityExample() {
    // Step 1: Create configuration
    auto config = std::make_shared<Config>();
    config->setSTDPEnabled(true);
    config->setHebbianEnabled(true);
    config->setSTDPWeight(0.01f);
    config->setSTDPTimeConstant(20.0f);
    config->setHebbianLearningRate(0.005f);
    
    // Step 2: Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Step 3: Create custom plasticity rules
    auto stdpRule = std::make_unique<STDP>();
    stdpRule->setLTPWeight(0.01f);
    stdpRule->setLTDWeight(0.01f);
    stdpRule->setTimeConstant(20.0f);
    
    auto hebbianRule = std::make_unique<Hebbian>();
    hebbianRule->setLearningRate(0.005f);
    
    // Step 4: Configure brain with custom plasticity
    brain->enablePlasticity(stdpRule, hebbianRule);
    
    // Step 5: Add neurons and synapses with specific plasticity
    auto neuron1 = brain->addNeuron();
    auto neuron2 = brain->addNeuron();
    
    auto synapse = brain->addSynapse(neuron1->getId(), neuron2->getId());
    synapse->enablePlasticity(true, true, false);  // STDP + Hebbian, no reward modulation
    
    // Step 6: Run simulation with custom plasticity
    std::cout << "Running simulation with custom plasticity..." << std::endl;
    for (int step = 0; step < 2000; ++step) {
        // Inject input to trigger plasticity
        if (step % 100 == 0) {
            neuron1->receiveExcitatoryInput(2.0f);
        }
        if (step % 150 == 0) {
            neuron2->receiveExcitatoryInput(2.0f);
        }
        
        // Update brain
        brain->step(step);
        
        // Print synapse weight
        if (step % 200 == 0) {
            std::cout << "Step " << step 
                      << ", Synapse weight: " << synapse->getWeight() << std::endl;
        }
    }
    
    // Step 7: Save custom configuration
    brain->save("custom_plasticity_simulation.dat");
    std::cout << "Custom plasticity simulation saved!" << std::endl;
}
```

### Example 6: Developmental Process

This example demonstrates developmental processes and stage transitions.

```cpp
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"

void developmentalExample() {
    // Step 1: Create configuration
    auto config = std::make_shared<Config>();
    config->setDevelopmentEnabled(true);
    config->setDevelopmentStageDuration(100.0, 200.0, 300.0, 400.0);  // Initial, Critical, Maturation, Adult
    config->setPlasticityModifierInitial(1.0f);
    config->setPlasticityModifierCritical(2.0f);
    config->setPlasticityModifierMaturation(0.8f);
    config->setPlasticityModifierAdult(0.5f);
    
    // Step 2: Create components
    auto brain = std::make_shared<Brain>(config);
    auto agentBrain = std::make_shared<AgentBrain>(brain, config);
    
    brain->initialize();
    agentBrain->initialize(SimpleWorld());
    
    // Step 3: Run development simulation
    std::cout << "Starting developmental simulation..." << std::endl;
    std::cout << "Initial developmental stage: " 
              << developmentalStageToString(agentBrain->getDevelopmentalStage()) << std::endl;
    
    for (int step = 0; step < 1000; ++step) {
        // Update development
        agentBrain->updateDevelopment(0.001);
        
        // Print developmental stage
        if (step % 100 == 0) {
            std::cout << "Step " << step 
                      << ", Developmental stage: " 
                      << developmentalStageToString(agentBrain->getDevelopmentalStage())
                      << ", Plasticity modifier: " 
                      << brain->getDevelopmentSystem()->getPlasticityModifier()
                      << ", Age: " << brain->getDevelopmentSystem()->getDevelopmentalAge() << std::endl;
        }
        
        // Update brain (simplified for developmental example)
        brain->step(step);
    }
    
    // Step 4: Print final developmental statistics
    std::cout << "\nDevelopment completed!" << std::endl;
    std::cout << "Final developmental stage: " 
              << developmentalStageToString(agentBrain->getDevelopmentalStage()) << std::endl;
    std::cout << "Final plasticity modifier: " 
              << brain->getDevelopmentSystem()->getPlasticityModifier() << std::endl;
    std::cout << "Final developmental age: " 
              << brain->getDevelopmentSystem()->getDevelopmentalAge() << std::endl;
    
    // Step 5: Save developmental results
    brain->save("developmental_simulation.dat");
    std::cout << "Developmental simulation saved!" << std::endl;
}
```

## Installation and Setup

### 1. Building from Source

#### Clone the Repository
```bash
# Clone the repository
cd /path/to
git clone https://github.com/your-username/nlm-framework.git

# Navigate to the project directory
cd nlm-framework
```

#### Configure Build
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON ..

# Alternative: Configure with specific options
cmake -DNLM_ENABLE_EXPERIMENTS=ON -DNLM_ENABLE_PYTHON_BINDING=ON -DNLM_ENABLE_DOCS=ON ..
```

#### Build
```bash
# Build the framework
cmake --build . -- -j$(nproc)  # Linux/macOS
# cmake --build . -- /m        # Windows

# Build with specific target
cmake --build . --target all-examples
```

### 2. Running Examples

#### Basic Examples
```bash
# Run basic simulation example
cd build
./examples/basic_simulation

# Run agent-environment example
cd build
./examples/agent_environment

# Run plasticity example
cd build
./examples/plasticity_demo
```

#### Running with Visualization
```bash
# If visualization is enabled
cd build
./examples/visualization_example
```

### 3. Running Tests

#### Unit Tests
```bash
# Run unit tests
cd build
ctest --output-on-failure

# Run specific test suite
ctest -R "neuron_tests"
ctest -R "synapse_tests"
ctest -R "brain_tests"
```

#### Performance Tests
```bash
# Run performance benchmarks
cd build
./benchmarks/neuron_benchmark
./benchmarks/synapse_benchmark
./benchmarks/brain_benchmark
```

## Frequently Asked Questions

### Q: What programming language is used?
A: NLM is implemented in C++17, providing high performance and memory efficiency.

### Q: Do I need prior neural networks knowledge?
A: No prior neural networks knowledge is required. The framework provides comprehensive documentation and examples for users with basic programming knowledge.

### Q: Can I extend the framework?
A: Yes, the NLM framework is designed to be extensible. You can add new neuron types, plasticity rules, memory systems, and components through the plugin architecture.

### Q: What are the system requirements?
A: The minimum requirements are 4 GB RAM and a C++17 compatible compiler. For larger simulations, 8 GB RAM or more is recommended.

### Q: How do I contribute?
A: Contributions are welcome! Please see the CONTRIBUTING.md file for guidelines on contributing to the project.

## Further Reading

### Related Resources

1. **NLM Documentation**: Comprehensive API documentation in the `docs/` directory
2. **Research Papers**: References to computational neuroscience papers
3. **Tutorial Videos**: Video tutorials and demonstrations
4. **Community Forums**: Discussion forums for questions and collaboration

### Next Steps

1. **Complete Basic Examples**: Start with the basic simulation examples to understand the framework
2. **Explore Advanced Examples**: Try more complex examples involving plasticity and development
3. **Study Architecture Documentation**: Understand the system architecture and component interactions
4. **Performance Optimization**: Learn about performance characteristics and optimization strategies
5. **Extend the Framework**: Add new components or modify existing ones to meet your needs

## Conclusion

This tutorial guide provides a comprehensive introduction to the NLM neural simulation framework. By following these examples and guidelines, you can quickly become proficient in using the framework to build and simulate neural systems with biologically plausible dynamics.

The NLM framework offers a powerful platform for neural simulation with its modular architecture, comprehensive documentation, and extensive examples. Whether you are a researcher, student, or developer, NLM provides the tools and capabilities needed to explore neural computation and behavior.

For further assistance, please consult the framework documentation, community forums, or support channels provided by the NLM team.
