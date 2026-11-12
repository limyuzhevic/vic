# NLM (熙然) - Usage Examples and Tutorials

## Overview

This document provides comprehensive examples and tutorials for using the NLM (Neural Learning Machine) system. It covers:

- Basic NLM system usage
- Phase-specific demonstrations
- Memory system examples
- Integration examples
- Advanced usage patterns
- Troubleshooting and debugging

## Prerequisites

Before running examples, ensure you have:

1. **NLM built and installed** (see INSTALL.md for setup)
2. **Configuration files** (configs/default.cfg, configs/development.cfg, etc.)
3. **Required dependencies** (see requirements in INSTALL.md)

## Quick Start Examples

### Example 1: Basic NLM Phase 6 Demo

```bash
# Navigate to project root
cd /path/to/nlm

# Run Phase 6 integration demo for 1000 steps
./bin/nlm_phase6_demo --steps 1000

# With custom configuration
./bin/nlm_phase6_demo --config my_config.cfg --steps 500

# With verbose output
./bin/nlm_phase6_demo --verbose
```

### Example 2: Multiple Phase Demos

```bash
# Run all available demos sequentially
./bin/nlm --steps 200     # Phase 2: Basic neural simulation
./bin/nlm_phase3_demo --steps 200  # Phase 3: Advanced neural dynamics
./bin/nlm_phase4_demo --steps 200  # Phase 4: Complex behaviors

# Check if Phase 5 demo exists
if [ -f "./bin/nlm_phase5_demo" ]; then
    ./bin/nlm_phase5_demo --steps 200
fi

# Run Phase 6 integration test
./bin/nlm_phase6_demo --steps 500

# Run benchmarks
./bin/nlm_benchmark --duration 60
```

## Code Examples

### Example 1: Creating and Initializing a Brain

```cpp
#include "nlm/Brain.hpp"
#include "nlm/Config.hpp"
#include <iostream>
#include <memory>

int main() {
    try {
        // Create configuration
        auto config = std::make_shared<nlm::Config>("configs/default.cfg");
        
        // Set some parameters
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        // Create brain
        nlm::Brain brain(config);
        
        // Initialize the brain
        if (!brain.initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        // Print brain statistics
        brain.getGlobalLogger().info("Brain initialized successfully");
        brain.logStatus();
        
        // Run for 100 steps
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
            std::cout << "Step " << step << " completed" << std::endl;
        }
        
        std::cout << "Simulation completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Example 2: Working with Neurons

```cpp
#include "nlm/Brain.hpp"
#include "nlm/Neuron.hpp"
#include <iostream>

int main() {
    try {
        // Create brain
        auto config = std::make_shared<nlm::Config>("configs/default.cfg");
        nlm::Brain brain(config);
        brain.initialize();
        
        // Access brain components
        nlm::Brain* rawBrain = brain.getBrain();
        
        // Get regions from brain
        const auto& regions = rawBrain->getRegions();
        if (!regions.empty()) {
            // Get neurons from first region
            auto& region = *regions[0];
            const auto& populations = region.getPopulations();
            
            if (!populations.empty()) {
                // Get neurons from first population
                const auto& neurons = populations[0]->getNeurons();
                
                if (!neurons.empty()) {
                    // Access first neuron
                    nlm::Neuron* neuron = neurons[0];
                    
                    std::cout << "Neuron ID: " << neuron->getId().index() << std::endl;
                    std::cout << "Neuron Type: " << static_cast<int>(neuron->getType()) << std::endl;
                    std::cout << "Firing Rate: " << neuron->getFiringRate() << " Hz" << std::endl;
                    std::cout << "Membrane Potential: " << neuron->getMembranePotential() << " mV" << std::endl;
                    
                    // Inject current to activate neuron
                    neuron->injectCurrent(5.0f);  // 5mV current injection
                    
                    // Simulate one step
                    neuron->step(0.0);
                    
                    std::cout << "After injection - Firing: " << (neuron->isFiring() ? "Yes" : "No") << std::endl;
                }
            }
        }
        
        std::cout << "Neuron example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Example 3: Working with Synapses

```cpp
#include "nlm/Brain.hpp"
#include "nlm/Synapse.hpp"
#include <iostream>

int main() {
    try {
        // Create brain
        auto config = std::make_shared<nlm::Config>("configs/default.cfg");
        nlm::Brain brain(config);
        brain.initialize();
        
        // Get brain components
        nlm::Brain* rawBrain = brain.getBrain();
        nlm::STDP* stdp = rawBrain->getSTDP();
        
        if (stdp) {
            // Create a synapse
            nlm::Synapse synapse(
                nlm::SynapseId(1),
                nlm::NeuronId(10),
                nlm::NeuronId(20)
            );
            
            // Configure synapse
            synapse.setType(nlm::SynapseType::Excitatory);
            synapse.setWeight(0.5f);
            synapse.setDelay(1);
            
            std::cout << "Created synapse:" << std::endl;
            std::cout << "  ID: " << synapse.getId().index() << std::endl;
            std::cout << "  Source: " << synapse.getSourceNeuron().index() << std::endl;
            std::cout << "  Destination: " << synapse.getDestinationNeuron().index() << std::endl;
            std::cout << "  Weight: " << synapse.getWeight() << std::endl;
            std::cout << "  Delay: " << synapse.getDelay() << " steps" << std::endl;
            std::cout << "  Type: " << (synapse.isExcitatory() ? "Excitatory" : "Inhibitory") << std::endl;
            
            // Record pre and post spikes for STDP
            synapse.recordPreSpike(0.0);
            synapse.recordPostSpike(0.1);
            
            std::cout << "Synapse history:" << std::endl;
            std::cout << "  Pre-spikes: " << synapse.getPreSpikeHistory().size() << std::endl;
            std::cout << "  Post-spikes: " << synapse.getPostSpikeHistory().size() << std::endl;
            
            // Apply STDP
            auto preSpikes = synapse.getPreSpikeHistory();
            auto postSpikes = synapse.getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                stdp->update(&synapse, preSpikes, postSpikes, 0.001);
                std::cout << "STDP applied. New weight: " << synapse.getWeight() << std::endl;
            }
        }
        
        std::cout << "Synapse example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Example 4: Memory System Usage

```cpp
#include "nlm/Brain.hpp"
#include "nlm/NeuralWorkingMemory.hpp"
#include "nlm/NeuralEpisodicMemory.hpp"
#include <iostream>
#include <vector>

int main() {
    try {
        // Create brain
        auto config = std::make_shared<nlm::Config>("configs/default.cfg");
        nlm::Brain brain(config);
        brain.initialize();
        
        // Access memory systems
        nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
        nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
        
        if (workingMemory && episodicMemory) {
            // Configure working memory
            workingMemory->setCapacity(50);
            workingMemory->setDecayRate(0.01f);
            
            // Store a pattern in working memory
            std::vector<float> pattern = {0.8f, 0.6f, 0.4f, 0.2f, 0.1f};
            workingMemory->store(pattern, 1.0f);
            
            // Retrieve working memory
            auto retrieved = workingMemory->retrieve();
            std::cout << "Working memory retrieved pattern of size " << retrieved.size() << std::endl;
            
            // Create and store an episodic memory item
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = 100;
            episode.positionX = 5.0f;
            episode.positionY = 3.0f;
            episode.orientation = 0.0f;
            episode.action = nlm::ActionType::MoveForward;
            episode.reward = 10.0f;
            episode.energy = 5.0f;
            episode.novelty = 0.8f;
            episode.resultingReward = 12.0f;
            
            // Add neural activity pattern
            episode.activeNeurons.push_back(nlm::NeuronId(100));
            episode.activeNeurons.push_back(nlm::NeuronId(101));
            episode.neuronActivations.push_back(0.9f);
            episode.neuronActivations.push_back(0.7f);
            
            // Store episode
            episodicMemory->storeEpisode(episode);
            
            std::cout << "Stored episodic memory episode:" << std::endl;
            std::cout << "  Timestamp: " << episode.timestamp << std::endl;
            std::cout << "  Action: " << static_cast<int>(episode.action) << std::endl;
            std::cout << "  Reward: " << episode.reward << std::endl;
            std::cout << "  Active neurons: " << episode.activeNeurons.size() << std::endl;
            
            // Retrieve similar episodes
            auto similar = episodicMemory->retrieveSimilar(pattern, 3);
            std::cout << "Found " << similar.size() << " similar episodes" << std::endl;
            
            // Store episodes for replay
            episodicMemory->enableReplay(true);
            episodicMemory->replayEpisode(&episode);
            
            std::cout << "Memory system example completed successfully!" << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Example 5: Configuration Management

```cpp
#include "nlm/Config.hpp"
#include <iostream>
#include <fstream>

int main() {
    try {
        // Create configuration
        nlm::Config config;
        
        // Set values using different types
        config.set("neuron_count", 1000);
        config.set("connection_probability", 0.1f);
        config.set("simulation_timestep", 0.001);
        config.set("random_seed", 42);
        config.set("enable_development", true);
        config.set("learning_rate", 0.01);
        config.set("batch_size", 32);
        config.set("log_level", std::string("info"));
        
        // Get values
        std::optional<int> neuronCount = config.get<int>("neuron_count");
        if (neuronCount) {
            std::cout << "Neuron count: " << *neuronCount << std::endl;
        }
        
        std::optional<float> connectionProb = config.get<float>("connection_probability");
        if (connectionProb) {
            std::cout << "Connection probability: " << *connectionProb << std::endl;
        }
        
        // Get with default value
        int maxEpisodes = config.getOr<int>("max_episodes", 1000);
        std::cout << "Max episodes: " << maxEpisodes << std::endl;
        
        // Check if key exists
        if (config.has("enable_development")) {
            std::cout << "Development is enabled" << std::endl;
        }
        
        // Load from file
        if (config.loadFromFile("configs/default.cfg")) {
            std::cout << "Loaded configuration from file" << std::endl;
        }
        
        // Save to file
        if (config.saveToFile("configs/custom.cfg")) {
            std::cout << "Saved configuration to file" << std::endl;
        }
        
        // Show configuration summary
        std::cout << "Configuration summary:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        std::cout << "Configuration example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Example 6: AgentBrain Integration

```cpp
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>

int main() {
    try {
        // Create brain and agent brain
        auto config = std::make_shared<nlm::Config>("configs/default.cfg");
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(&brain, [](nlm::Brain*) {}));
        
        // Create world
        nlm::SimpleWorld world(100, 100);
        
        // Initialize agent brain with world
        agentBrain.initialize(world);
        
        std::cout << "AgentBrain initialized:" << std::endl;
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << std::endl;
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << std::endl;
        
        // Create sensory percept
        nlm::SensoryPercept percept;
        // Fill with some values
        std::vector<float> vision = {0.8f, 0.6f, 0.4f, 0.2f};
        std::vector<float> touch = {0.1f, 0.2f};
        std::vector<float> internal = {0.0f};
        std::vector<float> proprioception = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        
        percept.setVision(vision);
        percept.setTouch(touch);
        percept.setInternal(internal);
        percept.setProprioception(proprioception);
        
        // Process sensory input
        agentBrain.processSensoryInput(percept);
        
        // Decode motor command
        nlm::MotorCommand command = agentBrain.decodeMotorCommand();
        std::cout << "Decoded command: " << static_cast<int>(command) << std::endl;
        
        // Get motor group activities
        auto activities = agentBrain.getMotorGroupActivities();
        for (const auto& act : activities) {
            std::cout << "  " << act.name << ": activity=" << act.activity 
                      << ", firingRate=" << act.firingRate << std::endl;
        }
        
        std::cout << "AgentBrain example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Phase-Specific Examples

### Phase 2: Basic Neural Simulation

```bash
# Command line usage
./nlm --steps 1000 --config configs/default.cfg

# Programmatic usage
#include "src/main.cpp"  // Phase 2 demo

// Basic neuron and synapse operations
// Simple recurrent network
// Basic learning and memory
```

### Phase 3: Advanced Neural Dynamics

```bash
# Command line usage  
./nlm_phase3_demo --steps 2000 --learning-rate 0.001

# Programmatic usage
#include "src/experiments/Phase3Experiment.cpp"

// Implement synaptic plasticity
// Create complex neural circuits
// Simulate biological learning processes
```

### Phase 4: Complex Behaviors

```bash
# Command line usage
./nlm_phase4_demo --steps 3000 --exploration-factor 0.3

# Programmatic usage
#include "src/experiments/Phase4Experiment.cpp"

// Implement cognitive behaviors
// Create action selection mechanisms
// Simulate goal-directed behavior
```

### Phase 5: Advanced Cognition

```bash
# Command line usage
./nlm_phase5_demo --steps 5000 --social-learning

# Programmatic usage
#include "src/experiments/Phase5Experiment.cpp"

// Implement social learning
// Create self-modeling
// Develop preference traits
```

### Phase 6: Final Integration

```bash
# Command line usage
./nlm_phase6_demo --steps 10000 --checkpoint-interval 1000

# Programmatic usage
#include "src/experiments/Phase6IntegratedExperiment.cpp"

// Integrate all systems
// Test system interactions
// Validate complete brain functionality
```

## Advanced Usage Patterns

### Example 1: Checkpointing and Persistence

```cpp
#include "nlm/Brain.hpp"
#include <iostream>
#include <filesystem>

int main() {
    try {
        // Create and initialize brain
        auto config = std::make_shared<nlm::Config>("configs/default.cfg");
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run simulation for a while
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain.step(step);
        }
        
        // Save brain state
        std::string checkpointPath = "checkpoints/brain_checkpoint.bin";
        std::filesystem::create_directories(std::filesystem::path(checkpointPath).parent_path());
        
        if (brain.save(checkpointPath)) {
            std::cout << "Brain saved to: " << checkpointPath << std::endl;
        } else {
            std::cerr << "Failed to save brain state" << std::endl;
        }
        
        // Later, load brain state
        nlm::Brain brain2(config);
        brain2.initialize();  // Reset before loading
        
        if (brain2.load(checkpointPath)) {
            std::cout << "Brain loaded successfully from: " << checkpointPath << std::endl;
            brain2.logStatus();
        } else {
            std::cerr << "Failed to load brain state" << std::endl;
        }
        
        std::cout << "Checkpointing example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Example 2: Performance Benchmarking

```bash
#!/bin/bash
# benchmark.sh

# Set up benchmark environment
mkdir -p benchmarks

echo "Running NLM Phase 6 Performance Benchmark..."
echo "=============================================="

# Run benchmark with different configurations
./bin/nlm_benchmark --duration 60 --steps 1000

# Collect performance metrics
./bin/nlm_benchmark --collect-metrics --output benchmarks/metrics.json

# Generate performance report
./bin/nlm_benchmark --generate-report --input benchmarks/metrics.json

# Compare with baseline
echo "Benchmark completed. Results saved to benchmarks/"
```

### Example 3: Development and Debugging

```cpp
#include "nlm/Brain.hpp"
#include <iostream>
#include <chrono>

int main() {
    try {
        // Create brain
        auto config = std::make_shared<nlm::Config>("configs/default.cfg");
        nlm::Brain brain(config);
        
        // Initialize with debug logging
        brain.getGlobalLogger().setLevel(nlm::LogLevel::Debug);
        brain.getGlobalLogger().info("Starting debug simulation");
        
        brain.initialize();
        
        // Run with timing
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < 500; ++step) {
            brain.step(step);
            
            if (step % 100 == 0) {
                brain.logStatus();
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "Simulation completed in " << duration.count() << " ms" << std::endl;
        std::cout << "Average speed: " << (500.0 / (duration.count() / 1000.0)) << " steps/second" << std::endl;
        
        std::cout << "Development and debugging example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Real-World Applications

### Example 1: Sensorimotor Control

```cpp
// Implement a sensorimotor controller using NLM
// Connect sensory input (vision, touch) to motor output
// Learn to navigate complex environments
// Demonstrate adaptive behavior through experience
```

### Example 2: Pattern Recognition

```cpp
// Use working memory for pattern storage and recall
// Implement Hebbian learning for pattern associations
// Use episodic memory for context-dependent behavior
// Demonstrate generalization from episodic experiences
```

### Example 3: Decision Making

```cpp
// Integrate multiple neuromodulatory systems
// Use curiosity for exploration vs. exploitation
// Implement reward prediction and prediction error
// Demonstrate value-based and goal-directed decision making
```

## Common Issues and Solutions

### Issue 1: Segmentation Fault

**Solution**:
```bash
# Check system resources
ulimit -v 4294967296  # Increase virtual memory
ulimit -s 67108864   # Increase stack size

# Run with debug build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
gmake
valgrind --leak-check=full ./nlm_phase6_demo
```

### Issue 2: Configuration Errors

**Solution**:
```bash
# Validate configuration
./bin/nlm_phase6_demo --validate-config configs/my_config.cfg

# Create configuration from defaults
cp configs/default.cfg configs/my_config.cfg

# Check for syntax errors
./bin/nlm_phase6_demo --check-config configs/my_config.cfg
```

### Issue 3: Performance Issues

**Solution**:
```bash
# Use optimized build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Profile performance
perf record --output=perf.data ./nlm_phase6_demo --steps 1000
perf report
```

## Getting Help

### Online Resources
- **Documentation**: https://github.com/your-repo/nlm/blob/main/EXAMPLES.md
- **Issues**: https://github.com/your-repo/nlm/issues
- **Discussions**: https://github.com/your-repo/nlm/discussions
- **Examples**: https://github.com/your-repo/nlm/tree/main/examples

### Community Support
```bash
# Join our community
# Discord: https://discord.gg/nlm
# Slack: https://nlm.slack.com
# Mailing List: https://lists.example.com/listinfo/nlm-dev
```

### Troubleshooting

If you encounter issues:

1. **Check system requirements**: Ensure your system meets minimum requirements
2. **Verify configuration**: Check configuration files for errors
3. **Enable debug logging**: Use `--verbose` flag for more information
4. **Check documentation**: Review relevant documentation sections
5. **Report issues**: Create detailed issue reports with system information

## Additional Resources

### Documentation
- **INSTALL.md**: Complete installation guide
- **DEVELOPING.md**: Development guidelines
- **API_REFERENCE.md**: Complete API documentation
- **README.md**: Project overview and status

### Examples
- **Source Code**: `/examples/` directory
- **Test Cases**: `/tests/` directory  
- **Tutorials**: `/docs/tutorials/` directory
- **Benchmarks**: `/benchmarks/` directory

### Related Projects
- **NLM Phase 2**: Basic neural simulation
- **NLM Phase 3**: Advanced neural dynamics
- **NLM Phase 4**: Complex behaviors
- **NLM Phase 5**: Advanced cognition
- **NLM Phase 6**: Final integration

## Conclusion

These examples demonstrate the versatility and power of the NLM system across different use cases and phases. By exploring these examples and adapting them to your specific needs, you can develop sophisticated neural systems for a wide range of applications.

Remember to:

1. **Start simple**: Begin with basic examples and gradually increase complexity
2. **Understand the system**: Review documentation to understand system architecture and limitations
3. **Test thoroughly**: Use provided testing frameworks and debugging tools
4. **Optimize for your use case**: Adjust parameters and configurations for optimal performance
5. **Contribute**: Share your examples and improvements with the community

The NLM system provides a powerful foundation for developing brain-inspired artificial systems with real neural computation, learning, and adaptive behavior.