# NLM - Comprehensive User Guide

## Welcome to NLM (Neural Learning Machine)

NLM (熙然) is an experimental artificial developmental brain project that simulates spiking neural networks. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

This guide provides comprehensive documentation for using NLM, from basic setup to advanced configuration and troubleshooting.

---

## Quick Start for Beginners

### Example 1: Create a Simple Brain

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

// Create configuration
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 500);
config->set("region_count", 1);
config->set("connection_probability", 0.15f);

// Create and initialize brain
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

// Run simulation
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step);
    
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": " 
                  << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
    }
}
```

### Example 2: Complete Agent Simulation

```cpp
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/world/SimpleWorld.hpp>

// Setup brain and agent
auto config = std::make_shared<nlm::Config>();
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

auto agent = std::make_shared<nlm::AgentBrain>(brain);

// Setup world
nlm::SimpleWorld world;
world.configure(20.0f, 20.0f, 8, 8);
world.reset();
agent->initialize(world);

// Enable learning
agent->enableRewardModulation(true);
agent->enableCuriosity(true);
agent->enableDevelopment(true);

// Run episode
for (int step = 0; step < 1000; ++step) {
    world.update(0.1f);  // 100ms per simulation step
    
    auto percept = world.getSensoryPercept();
    agent->processSensoryInput(percept);
    
    brain->step(step);
    
    auto action = agent->decodeMotorCommand();
    world.applyMotorCommand(action, world.getSimulationTime());
}
```

---

## Core Concepts

### 1. Brain (nlm::Brain)
The central neural system that contains neurons and synapses.

**Key Methods:**
- `initialize()`: Set up the brain with configuration
- `step(step)`: Process one simulation time step
- `injectCurrent(neuron, current)`: Add external input
- `produceAction()`: Generate motor commands

### 2. AgentBrain (nlm::AgentBrain)
The interface between brain and environment.

**Key Methods:**
- `processSensoryInput(percept)`: Receive sensory data
- `decodeMotorCommand()`: Convert neural activity to actions
- `applyRewardModulation(reward, predicted)`: Learn from rewards

### 3. SimpleWorld (nlm::SimpleWorld)
The simulation environment.

**Key Methods:**
- `configure(width, height, vision_w, vision_h)`: Set environment size
- `update(timestep)`: Advance simulation time
- `applyMotorCommand(cmd, time)`: Execute actions
- `getSensoryPercept()`: Get what agent senses

---

## Configuration System

### File-based Configuration

**Format:** `key = value` (similar to INI files)

```ini
# nlm/config.cfg

# Brain settings
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity
synapse_learning_rate = 0.01
stdp_enabled = true

# World settings
environment_width = 20
environment_height = 20
vision_width = 8
vision_height = 8

# Logging
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### Command Line Options

```bash
./nlm --config=config.cfg --seed=42 --neurons=500 --steps=1000 --output=results.txt
```

**Available Options:**
- `--config=FILE`: Load configuration from file
- `--seed=NUMBER`: Set random seed
- `--neurons=NUMBER`: Set neuron count
- `--steps=NUMBER`: Set maximum simulation steps
- `--output=FILE`: Output results to file
- `--verbose, -v`: Enable verbose output
- `--help, -h`: Show help message

### Programmatic Configuration

```cpp
auto config = std::make_shared<nlm::Config>();

// Basic settings
config->set("neuron_count", 1000);
config->set("region_count", 2);
config->set("connection_probability", 0.1f);

// Advanced settings
config->set("plasticity_learning_rate", 0.005f);
config->set("enable_structural_plasticity", true);
config->set("enable_reward_modulation", true);

// Save configuration
config->saveToFile("my_config.cfg");
```

---

## Running Experiments

### Phase 2: Basic Neural Computation

```bash
# Build and run Phase 2 demo
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm
```

### Phase 6: Full Integration Demo

```bash
./nlm_phase6_demo
```

### Benchmark Tests

```bash
./nlm_benchmark
```

---

## Python Bindings (pynlm)

NLM includes Python bindings for easier experimentation and research.

### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Complete Agent Example

```python
import pynlm

# Create all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: "
              f"{brain.getFiringNeuronCount()} firing, "
              f"{agent.getCuriosityLevel():.3f} curiosity")
```

---

## Performance Optimization

### Configuration for Better Performance

```ini
# Enable SIMD vectorization
use_simd = true

# Enable parallel processing
enable_parallel_processing = true

# Use sparse connectivity
use_sparse_connectivity = true

# Optimize memory pools
enable_memory_pooling = true
```

### Runtime Optimization Tips

1. **Increase simulation timestep** for faster but less accurate simulations
2. **Reduce neuron count** for quick testing
3. **Use batch processing** for multiple simulations
4. **Enable checkpointing** to resume interrupted simulations

### Profiling and Monitoring

```cpp
// Enable profiling in config
enable_profiling = true

// Monitor performance metrics
brain->logStatus();
```

---

## Troubleshooting

### Common Issues and Solutions

**Issue 1: "Brain initialization failed"

**Solution:**
- Check that configuration is valid (all required keys present)
- Ensure sufficient system resources
- Try reducing neuron count

**Issue 2: "Configuration validation failed"

**Solution:**
- Check for invalid values in configuration
- Ensure configuration keys are spelled correctly
- Use default configuration: `configs/default.cfg`

**Issue 3: "No spikes generated"

**Solution:**
- Check that neurons have sufficient current input
- Verify that threshold values are reasonable
- Enable verbose logging to debug

**Issue 4: "Memory errors"

**Solution:**
- Reduce neuron and synapse counts
- Check system memory usage
- Use checkpointing to save partial results

### Debug Commands

```bash
# Enable verbose output
./nlm --verbose

# Run with custom configuration
./nlm --config=my_config.cfg

# Debug with smaller brain
./nlm --neurons=100 --steps=100
```

---

## Advanced Features for Power Users

### Custom Plasticity Rules

```cpp
// Implement custom STDP rule
class CustomSTDP : public nlm::PlasticityRule {
public:
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, TimestepDuration dt) override {
        // Custom learning rule implementation
    }
};
```

### Multi-region Brain Architecture

```cpp
// Add multiple brain regions
RegionId region1 = brain->addRegion("sensory");
RegionId region2 = brain->addRegion("motor");
RegionId region3 = brain->addRegion("association");

// Connect regions
brain->addInterRegionConnection(region1, region2, 0.5f, 10);
brain->addInterRegionConnection(region2, region3, 0.3f, 20);
```

### Custom Environment

```cpp
class CustomWorld : public nlm::Environment {
public:
    void update(double timestep) override {
        // Custom world physics
    }
    
    nlm::SensoryPercept getSensoryPercept() override {
        // Custom sensory processing
    }
};
```

---

## API Reference

### Configuration Keys

| Key | Type | Default | Description |
|-----|------ muuta |
| `neuron_count` | int | 1000 | Total number of neurons |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Connection probability |
| `simulation_timestep` | float | 0.001 | Simulation time step |
| `random_seed` | int | 42 | Random seed for reproducibility |
| `enable_structural_plasticity` | bool | true | Enable structural plasticity |
| `enable_reward_modulation` | bool | true | Enable reward modulation |

### Error Codes

- `0`: Success
- `1`: General error
- `2`: Configuration error
- `3`: Initialization error
- `4`: Memory error

---

## Further Reading

### Recommended Documentation

1. **ARCHITECTURE.md** - Detailed system architecture
2. **SCIENCE.md** - Scientific background and principles
3. **EXPERIMENTS.md** - Experiment descriptions and results
4. **PHASE6_FINAL_AUDIT.md** - Phase 6 implementation details

### Research Papers

- "NLM: An Artificial Developmental Brain" (2024)
- "Spiking Neural Network Simulation with Event-driven Dynamics" (2023)
- "Emergent Cognition in Developmental Neural Networks" (2022)

### Community Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **User Forum**: https://forum.nlm-project.org

---

## Support

For technical support:
1. Check the documentation in this guide
2. Visit the GitHub repository issues page
3. Consult the troubleshooting section
4. Contact the development team

For research collaborations:
- Review the EXPERIMENTS.md for current research directions
- Contact the project leads for collaboration opportunities
- Join the research mailing list

---

*Last Updated: November 8, 2026*
*NLM Version: 0.1.0 (Phase 6)
This documentation is continuously updated based on user feedback and research findings.*