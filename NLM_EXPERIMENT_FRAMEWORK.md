# NLM Experiment Framework - User Guide

## Overview

The NLM (Neural Learning Machine) now provides a unified, extensible experiment framework that makes it easy to run different experiment types and add new experiments.

## Quick Start

### Basic Usage

```bash
# Run the default Phase 6 integrated experiment
nlm

# Run specific experiments
nlm --experiment=phase3_demo
nlm phase4_demo
nlm --experiment=phase5
nlm --experiment=phase6_demo
nlm --experiment=phase6_integrated

# Debug mode with more steps
nlm --debug --experiment=phase6_integrated --steps=5000
```

### Legacy Support

For backward compatibility, the original Phase 2 experiments are still available:
```bash
nlm --experiment=phase2
```

## Experiment Types

### Phase 2: Legacy Experiments
- **What it does**: Three hardcoded tests (basic connectivity, plasticity, STDP)
- **Use case**: Testing the original NLM architecture
- **When to use**: Legacy compatibility, understanding original codebase

### Phase 3: Basic Neural Development
- **What it does**: Sensory processing and basic learning
- **Use case**: Entry-level neural simulation
- **Features**: Simple object detection, early learning capabilities

### Phase 4: Cognitive Development
- **What it does**: Motor skills, spatial navigation, concept formation
- **Use case**: Advanced cognitive abilities
- **Features**: Attention, planning, social learning, generalization

### Phase 5: Comprehensive Lifetime Learning
- **What it does**: 7-phase development over 400k+ steps
- **Use case**: Full lifecycle learning and adaptation
- **Features**: Memory consolidation, spatial representation, social development, performance optimizations

### Phase 6: Integration Testing
- **Phase 6 Demo**: Quick integration test of all systems
- **Phase 6 Integrated**: Complete integration experiment with checkpointing, replay, and development

## Advanced Usage

### Configuration Files

Create a configuration file (`configs/my_experiment.cfg`):

```ini
# Configuration for Phase 5 experiment
random_seed = 12345
neuron_count = 2000
max_simulation_steps = 100000
enable_multithreading = true
thread_count = 4
enable_SIMD = true
enable_sparse_connectivity = true
output_dir = "./results/experiment_001"
save_checkpoints = true
generate_visualizations = true
run_ablation = false
```

Run with custom config:
```bash
nlm --experiment=phase5 --config=configs/my_experiment.cfg
```

### Command Line Options

| Option | Description |
|--------|-------------|
| `-h, --help` | Show help message and available experiments |
| `-d, --debug` | Enable debug logging |
| `--config FILE` | Configuration file (default: configs/default.cfg) |
| `--seed N` | Random seed (default: 42) |
| `--steps N` | Maximum simulation steps (default: 1000) |
| `--output FILE` | Output file for results |
| `--version` | Show version information |

### Python API

For Python users, the `pynlm` package provides a high-level interface:

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 steps
for i in range(100):
    brain.step(i)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
```

## Adding New Experiments

The framework is designed to be easily extensible. To add a new experiment:

### 1. Create Experiment Class
```cpp
// New file: src/experiments/MyNewExperiment.hpp
#pragma once

#include "../experiments/Experiment.hpp"

namespace nlm {

class MyNewExperiment : public NLMExperiment {
public:
    std::string getName() const override { return "my_new_experiment"; }
    std::string getDescription() const override { 
        return "New experiment type that does something interesting";
    }
    bool run(const std::vector<std::string>& args, std::shared_ptr<Config> config) override;
    std::string getHelp() const override;
};

} // namespace nlm
```

### 2. Implement Methods
```cpp
// New file: src/experiments/MyNewExperiment.cpp
#include "MyNewExperiment.hpp"

bool MyNewExperiment::run(const std::vector<std::string>& args, std::shared_ptr<Config> config) {
    // Your experiment logic here
    NLM_LOG_INFO("Running My New Experiment...");
    // ... implement your experiment ...
    return true;
}

std::string MyNewExperiment::getHelp() const {
    return "New experiment for doing something specific. "
           "Features: interesting capability, advanced behavior";
}
```

### 3. Update Main Framework
```cpp
// In main.cpp, add your experiment to the registry:
g_experimentRegistry.registerExperiment(
    std::make_unique<MyNewExperiment>()
);
```

### 4. Update Build System
```cmake
# In CMakeLists.txt
add_library(nlm_my_experiment STATIC
    src/experiments/MyNewExperiment.cpp
)
target_include_directories(nlm_my_experiment PUBLIC src)
target_link_libraries(nlm_my_experiment PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)

# Update main executable
target_link_libraries(nlm PUBLIC nlm_core nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_my_experiment)
```

### 5. Test Your Experiment
```bash
nlm --experiment=my_new_experiment
```

## Configuration Management

### Default Configuration
The framework includes several configuration options:

```cpp
// Default values in main.cpp
config->set("random_seed", static_cast<int64_t>(parser.getSeed()), ConfigSource::Default);
config->set("max_simulation_steps", parser.getSteps(), ConfigSource::Default);
```

### Configuration File Format
The configuration system uses a simple key=value format:

```ini
# configs/default.cfg
random_seed = 42
neuron_count = 1000
max_simulation_steps = 10000
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
enable_Sensory_processing = true
enable_reward_modulation = true
```

### Command Line Override
Command line arguments override configuration file values:

```bash
nlm --experiment=phase5 --seed=12345 --steps=50000
```

## Performance Optimization

### Available Optimizations
The framework supports several performance optimizations:

- **Multithreading**: Parallel processing across multiple CPU cores
- **SIMD**: Single instruction, multiple data vectorization
- **Sparse Connectivity**: Efficient storage for sparse neural connections
- **Event-Driven Processing**: Only process neurons that fire

### Enabling Optimizations
```bash
# Use Phase 5 for performance optimizations
nlm --experiment=phase5

# Custom configuration
nlm --experiment=phase5 --config=configs/optimized.cfg
```

### Performance Monitoring
The framework includes performance monitoring:

- Progress reporting with ETA
- Wall-clock time tracking
- Memory usage monitoring
- CPU utilization tracking

## Debugging and Troubleshooting

### Debug Mode
Enable debug logging with:
```bash
nlm --debug --experiment=phase6_integrated
```

### Common Issues

**"My brain isn't doing anything"**
- Ensure `brain.initialize()` is called
- Try increasing the number of steps
- Check configuration values

**"The agent isn't moving"**
- Ensure `world.applyMotorCommand()` is being called
- Verify `world.update()` is in the loop

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

## Experiment Results

The framework provides detailed experiment results:

### Phase 6 Integrated Results
```
=== FINAL RESULTS ===
Total reward: 42.5
Avg firing rate: 12.3 Hz
Episodes stored: 125
Dopamine level: 0.85

=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING
```

### Output Files
Experiments can save results to files:

- Checkpoints: `./checkpoint_test.bin`
- Results: `./results/experiment_name/`
- Logs: `./logs/experiment_name.log`

## Migration Guide

### From Phase 2 to Phase 6

**Old Way (Phase 2):**
```cpp
// Single monolithic main.cpp with hardcoded tests
// runBasicConnectivityTest(brain);
// runPlasticityExperiment(brain);
// runStdpVerification(brain);
```

**New Way (Phase 6):**
```bash
# Command line interface
nlm --experiment=phase6_integrated

# Or use the unified framework
nlm
```

**Benefits of the new system:**

1. **Modular**: Each experiment is independent
2. **Extensible**: Easy to add new experiments
3. **Configurable**: Multiple configuration options
4. **Documented**: Comprehensive help and examples
5. **Tested**: Better error handling and validation
6. **Performant**: Optimized for large-scale simulations
7. **Flexible**: Multiple experiment types for different use cases

## Getting Help

### Documentation
- `easy_usage.md` - Simple usage guide for beginners
- `HOW_TO_USE.md` - Detailed usage instructions
- `docs/ARCHITECTURE.md` - System architecture
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration audit

### Examples
- `examples/` - Additional experiment examples
- `tests/` - Unit tests for the framework

### Community
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: Report bugs and feature requests

## Conclusion

The NLM experiment framework provides a production-ready, extensible system for running neural learning experiments. Whether you're just getting started with neural simulation or building complex lifetime learning systems, this framework makes it easy to:

- Run experiments with simple commands
- Add new experiment types easily
- Configure simulations with flexibility
- Monitor and debug experiments effectively
- Scale to large, complex simulations

The framework maintains full backward compatibility while providing powerful new capabilities for advanced users and researchers.
