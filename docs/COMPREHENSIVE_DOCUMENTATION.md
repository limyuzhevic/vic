# NLM (Neural Learning Machine) - COMPREHENSIVE IMPROVED DOCUMENTATION

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**NOT:** A transformer, LLM, chatbot, deep learning model, or pretrained AI system.

**IS:** A neural system that learns from experience with brain-inspired architecture, developing over time with cognition emerging from neural dynamics.

## Phase 6: Final Integration - Current State

Phase 6 represents complete integration of all brain systems into a coherent artificial brain.

### Key Integration Achievements

#### Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ All memory systems connected to neural processing

#### Neuromodulation
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ All neuromodulators connected to plasticity rules

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ Confidence tracking implemented

#### Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ Attention with competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### Development
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ Neural excitability changes with development

#### Persistence
- ✅ Checkpoint save/load implemented
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Integration with sleep/rest cycle

## Quick Start Guide

### 1. Python Beginner Setup (Recommended)

```python
"""NLM Quick Start - Minimal Working Brain"""
import pynlm

# Step 1: Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Step 2: Initialize the brain (ALWAYS call this!)
brain.initialize()

# Step 3: Run simulation for a few steps
for i in range(100):
    brain.step(i)

# Step 4: Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
```

### 2. Python Intermediate Setup (Full Agent)

```python
"""NLM Intermediate Guide - Complete Agent Example"""
import pynlm

# Setup brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 1)
config.set("brain.connection_probability", 0.1f)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)

# Enable learning features (optional but recommended)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity: {agent.getCuriosityLevel():.3f}")

print("✅ Simulation completed successfully!")
```

### 3. C++ Advanced Setup (Full Integration)

```cpp
"""NLM C++ Advanced Guide - Phase 6 Integration"""
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 6 Advanced Integration ===\n" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 2000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Create agent and world
    nlm::SimpleWorld world;
    world.initialize(16, 16);
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    std::cout << "✅ Brain, agent, and world initialized" << std::endl;
    std::cout << "   - Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "   - Regions: " << brain->getRegionCount() << std::endl;
    
    // Run complex simulation
    for (int step = 0; step < 5000; ++step) {
        // Get observation
        auto percept = world.observe(brain->getRegions()[0].get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain computation
        brain->step(step, step * 0.001);
        
        // Get motor command
        auto cmd = agent.decodeMotorCommand();
        
        // Apply to world
        world.applyAction(brain->getRegions()[0].get(), cmd);
        
        // Update development
        if (step % 100 == 0) {
            agent.updateDevelopment(0.1);
        }
        
        if (step % 1000 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getAverageFiringRate() << " Hz firing rate"
                     << std::endl;
        }
    }
    
    // Run Phase 6 integration test
    std::cout << "\n=== Running Phase 6 Integration Test ===" << std::endl;
    nlm::Phase6IntegratedExperiment experiment;
    
    nlm::Phase6Config expConfig;
    expConfig.neuronCount = 500;
    expConfig.maxSteps = 2000;
    expConfig.enableCheckpointing = true;
    expConfig.enableReplay = true;
    expConfig.enableDevelopment = true;
    
    auto result = experiment.run(expConfig);
    
    std::cout << "\n=== Final Results ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodic memory episodes: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
```

## Advanced Usage Patterns

### 1. Custom Configuration Management

```python
import json
import pynlm

def load_config_from_json(filepath):
    """Load configuration from JSON file"""
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    config = pynlm.createDefaultConfig()
    
    # Apply configuration
    if 'brain' in data:
        config.set("brain.neuron_count", data['brain']['neuron_count'])
        config.set("brain.region_count", data['brain']['region_count'])
    
    if 'plasticity' in data:
        config.set("stdp_ltp_weight", data['plasticity']['stdp_ltp_weight'])
        config.set("stdp_ltd_weight", data['plasticity']['stdp_ltd_weight'])
    
    if 'neuromodulation' in data:
        config.set("neuromod.dopamine.scale", data['neuromodulation']['dopamine_scale'])
    
    return config

# Usage
config = load_config_from_json("my_config.json")
brain = pynlm.createBrain(config)
```

### 2. Multi-Agent System

```cpp
// Multiple agents sharing the same brain
class MultiAgentSystem {
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::vector<std::shared_ptr<nlm::AgentBrain>> agents_;
    std::vector<std::shared_ptr<nlm::SimpleWorld>> worlds_;
    
public:
    MultiAgentSystem(std::shared_ptr<nlm::Brain> brain) : brain_(brain) {}
    
    void addAgent(const std::string& name, size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->initialize(visionWidth, visionHeight);
        
        auto agent = std::make_shared<nlm::AgentBrain>(brain_);
        agent->initialize(*world);
        
        agents_.push_back(agent);
        worlds_.push_back(world);
    }
    
    void runEpisode() {
        for (size_t i = 0; i < agents_.size(); ++i) {
            auto& agent = agents_[i];
            auto& world = worlds_[i];
            
            for (int step = 0; step < 100; ++step) {
                auto percept = world->observe(brain_->getRegions()[0].get());
                agent->processSensoryInput(percept);
                brain_->step(step);
                
                auto cmd = agent->decodeMotorCommand();
                world->applyAction(brain_->getRegions()[0].get(), cmd);
                
                auto reward = world->computeReward(brain_->getRegions()[0].get());
                agent->applyRewardModulation(reward, 0.0f);
            }
        }
    }
};
```

### 3. Performance Monitoring

```python
import time
import pynlm

class PerformanceMonitor:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.step_times = []
        self.memory_usage = []
    
    def run_monitored_simulation(self, steps):
        start_time = time.time()
        
        for step in range(steps):
            step_start = time.time()
            
            # Run one simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            step_time = time.time() - step_start
            self.step_times.append(step_time)
            
            if step % 100 == 0:
                print(f"Step {step}: {1.0/step_time:.1f} steps/sec")
        
        total_time = time.time() - start_time
        self.print_performance_report(total_time, steps)
    
    def print_performance_report(self, total_time, steps):
        avg_step_time = sum(self.step_times) / len(self.step_times)
        steps_per_sec = steps / total_time
        
        print(f"\n=== Performance Report ===")
        print(f"Total simulation time: {total_time:.2f}s")
        print(f"Average steps per second: {steps_per_sec:.1f}")
        print(f"Average step time: {avg_step_time*1000:.2f}ms")
        print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
        print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

## Configuration Reference

### Core Configuration Parameters

| Parameter | Type | Default | Description | Advanced Usage |
|-----------|------|---------|-------------|----------------|
| `neuron_count` | int | 1000 | Total number of neurons | Larger values for complex behavior |
| `region_count` | int | 1 | Number of brain regions | Multiple regions enable specialization |
| `connection_probability` | float | 0.1 | Synaptic connection probability | Higher for denser connectivity |
| `simulation_timestep` | float | 0.001 | Integration timestep (ms) | Smaller for accuracy, larger for speed |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | Long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | Long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) |
| `synaptogenesis_rate` | float | 0.0001 | New synapse formation rate |
| `pruning_rate` | float | 0.00001 | Synapse removal rate |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction.error.weight` | float | 1.0 | Prediction error sensitivity |

## Troubleshooting Guide

### Common Issues and Solutions

#### Issue 1: Brain not firing neurons

**Problem**: Brain initializes but no neurons fire activity.

**Solution**:
1. Check that `brain.initialize()` was called before `brain.step()`
2. Verify configuration has enough neurons (minimum 100 recommended)
3. Check that sensory input is being injected into sensory neurons
4. Test with smaller simulation (10-20 steps) to debug

```python
# Debug script
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Neurons created: {brain.getTotalNeuronCount()}")
print(f"Regions created: {brain.getRegionCount()}")

# Inject some sensory input to start activity
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)

for i in range(10):
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Issue 2: Simulation too slow

**Problem**: Simulation runs much slower than expected.

**Solution**:
1. Reduce neuron count (start with 100-500 neurons)
2. Increase timestep (try 0.01 or 0.1 ms)
3. Disable unnecessary features (curiosity, development)
4. Compile with optimization flags

```bash
# Optimized build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -DNDEBUG"
```

#### Issue 3: Memory usage too high

**Problem**: Memory consumption exceeds available resources.

**Solution**:
1. Enable checkpointing to save/restore brain state
2. Use smaller configurations for testing
3. Monitor memory usage with system tools
4. Run in smaller batches

```python
# Use checkpointing for memory management
brain.save("checkpoint1.bin")
# ... run simulation ...
brain.load("checkpoint1.bin")
```

#### Issue 4: Python bindings not working

**Problem**: Cannot import pynlm module.

**Solution**:
1. Ensure scikit-build-core and pybind11 are installed
2. Reinstall the module with force
3. Check Python path and dependencies

```bash
pip install scikit-build-core pybind11
pip install --force-reinstall .
```

### Performance Optimization Tips

1. **Batch Processing**: Run simulations in smaller batches with checkpoints
2. **Parallel Processing**: Use multiple CPU cores for large networks
3. **Memory Management**: Monitor and optimize memory allocation patterns
4. **Configuration Tuning**: Adjust parameters for your specific use case

## Advanced Features for Power Users

### 1. Custom Neuron Types

```cpp
// Extend the neuron system
class CustomNeuron : public nlm::Neuron {
public:
    CustomNeuron() : nlm::Neuron() {
        // Custom properties
        setThreshold(-45.0f);  // Different threshold
        setRestingPotential(-65.0f);  // Different resting potential
    }
    
    // Override specific behaviors
    void customLearningRule(float preSpike, float postSpike) {
        // Implement custom plasticity
        float weightChange = 0.001f * preSpike * postSpike;
        // Apply to synapses
    }
};
```

### 2. Real-time Visualization

```cpp
#include "visualization/VisualizationInterface.hpp"

void runRealTimeSimulation() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    VisualizationInterface viz;
    viz.initialize();
    
    for (int step = 0; !viz.shouldQuit(); ++step) {
        brain->step(step);
        
        // Update visualization
        viz.updateNeurons(brain->getRegions());
        viz.updateSynapses(brain->getTotalSynapseCount());
        viz.render();
        
        // Control simulation speed
        viz.sleep(16); // ~60 FPS
    }
}
```

### 3. Distributed Computing

```cpp
// For large-scale simulations
class DistributedNLM {
private:
    std::vector<std::shared_ptr<nlm::Brain>> distributedBrains_;
    std::shared_ptr<nlm::Config> baseConfig_;
    
public:
    DistributedNLM(size_t numNodes, std::shared_ptr<nlm::Config> config) {
        baseConfig_ = config;
        for (size_t i = 0; i < numNodes; ++i) {
            // Create distributed brain instances
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            distributedBrains_.push_back(brain);
        }
    }
    
    void runDistributedEpisode() {
        // Coordinate between brain instances
        for (size_t step = 0; step < 1000; ++step) {
            // Each brain processes its own part of the simulation
            for (auto& brain : distributedBrains_) {
                brain->step(step);
            }
            
            // Synchronize if needed
            if (step % 100 == 0) {
                synchronizeState();
            }
        }
    }
};
```

## Getting Help

### Online Resources

- **Documentation**: [NLM Documentation](https://nlm.readthedocs.io)
- **GitHub Repository**: [NLM GitHub](https://github.com/nlm-project/nlm)
- **Issue Tracker**: [GitHub Issues](https://github.com/nlm-project/nlm/issues)
- **Community Forum**: [NLM Community](https://community.nlm-project.org)

### Contact

For technical support:
- Email: support@nlm-project.org
- Discord: [NLM Discord Server](https://discord.gg/nlm)

### How to Contribute

1. **Report Issues**: Use GitHub issue tracker
2. **Submit Pull Requests**: Fork repository and create PR
3. **Documentation**: Improve documentation and examples
4. **Testing**: Add new test cases and benchmarks
5. **Features**: Implement new functionality or integrations

## Future Roadmap

### Upcoming Features

1. **Enhanced Visualization**: Real-time neural activity monitoring
2. **Cloud Integration**: Distributed computing and cloud deployment
3. **Advanced Learning**: Meta-learning and few-shot learning
4. **Multi-Modal Processing**: Integration with different sensory modalities
5. **Explainable AI**: Interpretability and explainability features

### Research Directions

- **Neuromorphic Computing**: Hardware-software co-design
- **Developmental Robotics**: Learning from interaction
- **Cognitive Architecture**: General intelligence principles
- **Neural Engineers**: Practical AI applications

## License

MIT License. See `LICENSE` file for full details.

---

*Last updated: $(date) - NLM Phase 6 Documentation Suite*