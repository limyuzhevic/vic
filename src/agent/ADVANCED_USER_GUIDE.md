# NLM AgentBrain - Advanced User Guide

## Overview

The NLM (Neural Learning Machine) AgentBrain is an experimental artificial developmental brain system that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## New Improvements

### 1. Modular Architecture

The AgentBrain has been completely refactored with a modular design:

- **MotorCommandSelector**: Handles motor command selection with curiosity-based exploration
- **NoveltyCalculator**: Manages novelty detection and curiosity calculation
- **MotorDecoder**: Converts neural activity to motor commands with configurable thresholds
- **Configuration Manager**: Centralized configuration system for all agent parameters

### 2. Advanced Configuration System

The new configuration system provides extensive control over agent behavior:

```cpp
struct AgentBrainConfig {
    // Development configuration
    bool enableDevelopment = true;
    bool enableRewardModulation = true;
    bool enableStructuralPlasticity = true;
    bool enableCuriosity = true;
    
    // Sensory processing configuration
    float visionScaleFactor = 5.0f;
    float touchScaleFactor = 8.0f;
    float internalScaleFactor = 5.0f;
    float proprioceptionScaleFactor = 3.0f;
    
    // Novelty configuration
    float sensoryNoveltyDecay = 0.99f;
    
    // Curiosity parameters
    float curiosityThreshold = 0.3f;
    float randomActionThreshold = 0.5f;
    float maxExploreChance = 0.3f;
    
    // Command line interface options
    struct CommandLineOptions {
        bool verbose = false;
        bool debug = false;
        bool benchmark = false;
        std::string logLevel = "Info";
        std::string outputFormat = "text";
        std::map<std::string, std::string> customOptions;
    };
};
```

### 3. Comprehensive Command Line Interface

The CLI provides extensive control over the AgentBrain:

#### Basic Commands
- `nlm-agent run [options]` - Run agent in simulation mode
- `nlm-agent test <name>` - Run specific tests (learning, connectivity, plasticity)
- `nlm-agent config <file>` - Load configuration from file
- `nlm-agent stats <file>` - Export statistics to file
- `nlm-agent benchmark N` - Run benchmark for N iterations
- `nlm-agent interactive` - Start interactive mode

#### Available Options
- `--verbose, -v` - Enable verbose output
- `--debug, -d` - Enable debug mode
- `--world <width,height>` - Set world dimensions
- `--time <time>` - Set simulation time step (default: 0.1)
- `--steps <N>` - Set number of simulation steps (default: 1000)
- `--output <file>` - Set output file
- `--config <file>` - Load configuration from file

#### Advanced Options
- `--curiosity <level>` - Set curiosity threshold (0-1)
- `--plasticity <level>` - Set plasticity modifier (0-2)
- `--reward <factor>` - Set reward factor (0-10)
- `--no-curiosity` - Disable curiosity system
- `--no-development` - Disable development system
- `--no-plasticity` - Disable structural plasticity
- `--no-reward-modulation` - Disable reward modulation
- `--action-space <actions>` - Set action space (comma-separated)
- `--sensory-scales <scales>` - Set sensory scale factors (v,t,i,p)
- `--novelty-decay <decay>` - Set novelty decay factor (0-1)

### 4. Advanced Features

#### Action Space Configuration
```cpp
std::vector<MotorCommand> actions = {
    MotorCommand::MoveForward,
    MotorCommand::MoveBackward,
    MotorCommand::TurnLeft,
    MotorCommand::TurnRight,
    MotorCommand::LookLeft,
    MotorCommand::LookRight,
    MotorCommand::Interact,
    MotorCommand::Wait
};
agent->setActionSpace(actions);
```

#### Sensory Scale Configuration
```cpp
agent->setSensoryScaleFactors(10.0f, 15.0f, 8.0f, 5.0f);
// Adjusts vision, touch, internal, and proprioception scaling
```

#### Curiosity Parameter Tuning
```cpp
agent->setCuriosityParameters(0.2f, 0.25f);
// Lower threshold means more exploration, higher chance means more random actions
```

#### Configuration Management
```cpp
AgentBrainConfig config;
config.enableCuriosity = false;
config.enableDevelopment = true;
config.enableRewardModulation = true;

auto agent = std::make_shared<AgentBrain>(brain, config);
```

### 5. Extended Action Space

The new implementation supports extended motor commands:

- **Basic Actions**: MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
- **Sensory Actions**: LookLeft, LookRight
- **Extended Actions** (Phase 6+): Look, LookUp, LookDown, Eat, Drink, Rest, Custom

### 6. Improved Novelty and Curiosity System

The novelty calculation has been enhanced:

```cpp
struct NoveltyResult {
    float noveltyLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float predictionError = 0.0f;
};
```

The system now uses:
- **Novelty Decay**: Configurable decay rate for novelty levels
- **Curiosity Integration**: Combines novelty with prediction error
- **Threshold-based Exploration**: Configurable thresholds for different behaviors

### 7. Performance Optimizations

- **Modular Components**: Reduced coupling between subsystems
- **Configurable Thresholds**: Optimized for different scenarios
- **Efficient Memory Management**: Better resource utilization
- **Parallel Processing**: Potential for future parallelization

### 8. Enhanced Error Handling

- **Input Validation**: Validates configuration parameters
- **Graceful Degradation**: Falls back to defaults on errors
- **Detailed Logging**: Comprehensive logging for debugging
- **Exception Safety**: Proper exception handling throughout

## Quick Start Guide

### Basic Usage
```bash
# Run simple simulation
nlm-agent run --world 30,30 --steps 500

# Run with curiosity enabled
nlm-agent run --curiosity 0.7 --no-curiosity

# Run advanced learning test
nlm-agent test learning --benchmark

# Export statistics
nlm-agent run stats output.txt

# Run benchmark
nlm-agent benchmark 100

# Start interactive mode
nlm-agent interactive
```

### Python Integration
```python
import pynlm

# Create agent with advanced configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced Configuration
```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Set advanced parameters
config.set("curiosity_threshold", 0.4)
config.set("plasticity_modifier", 0.8)
config.set("reward_factor", 2.0)
config.set("novelty_decay", 0.95)

# Create brain with configuration
brain = pynlm.createBrain(config)

# Create agent with configuration
agent = pynlm.createAgentBrain(brain)

# Configure agent features
agent.setSensoryScaleFactors(8.0, 12.0, 6.0, 4.0)
agent.setActionSpace([
    pynlm.MotorCommand.MoveForward,
    pynlm.MotorCommand.TurnLeft,
    pynlm.MotorCommand.TurnRight,
    pynlm.MotorCommand.Interact
])
```

## Testing and Validation

### Built-in Tests

The system includes comprehensive tests:

1. **Learning Test**: Tests curiosity-driven exploration and reward-based learning
2. **Connectivity Test**: Tests basic neural connectivity and spike propagation
3. **Plasticity Test**: Tests synaptic changes and structural plasticity

To run tests:
```bash
nlm-agent test learning
nlm-agent test connectivity
nlm-agent test plasticity
```

### Benchmarking

Run performance benchmarks:
```bash
nlm-agent benchmark 100    # Run 100 iterations
nlm-agent benchmark 1000   # Run 1000 iterations
```

### Configuration Validation

Validate your configuration:
```bash
nlm-agent config validate my_config.json
```

## Examples

### Example 1: Basic Exploration
```bash
nlm-agent run --world 40,40 --steps 1000 --curiosity 0.6 --action-space "MoveForward,TurnLeft,TurnRight,Wait"
```

### Example 2: Learning Agent
```bash
nlm-agent run --world 30,30 --steps 2000 --curiosity 0.8 --plasticity 1.2 --reward 1.5
```

### Example 3: Experimental Setup
```bash
nlm-agent run --world 25,25 --steps 1500 --no-curiosity --no-development --novelty-decay 0.9
```

### Example 4: Interactive Session
```bash
nlm-agent interactive
# Inside interactive mode:
> status                    # Show agent status
> set curiosity 0.5         # Set curiosity level
> step                      # Take one simulation step
> run 200                   # Run 200 simulation steps
```

## Migration Guide

### From Phase 4 to Phase 6

If you're migrating from an older version, here are the key changes:

1. **New Configuration System**: Replace direct parameter setting with `AgentBrainConfig`
2. **Modular Architecture**: New classes replace monolithic functions
3. **Extended Action Space**: Support for additional motor commands
4. **Enhanced Curiosity**: More sophisticated novelty and curiosity calculations
5. **CLI Interface**: New command-line interface for control

### Upgrading Existing Code

```cpp
// Old way
AgentBrain agent(brain);
agent.enableCuriosity(true);

// New way
AgentBrainConfig config;
config.enableCuriosity = true;
AgentBrain agent(brain, config);
```

## Troubleshooting

### Common Issues

1. **No spikes generated**
   - Ensure sensory input is connected
   - Check neuron parameters
   - Verify brain initialization

2. **High CPU usage**
   - Reduce simulation steps
   - Disable unnecessary features
   - Optimize scale factors

3. **Configuration errors**
   - Validate configuration files
   - Check for syntax errors
   - Use defaults and modify gradually

4. **Interactive mode issues**
   - Ensure terminal supports input
   - Check for permission issues
   - Restart the application

### Debug Commands

```bash
nlm-agent run --debug
nlm-agent test learning --verbose
nlm-agent benchmark 10 --debug
```

## Further Reading

For more information, see:

- `docs/ARCHITECTURE.md` - Detailed architecture documentation
- `docs/SCIENCE.md` - Scientific background and principles
- `docs/ROADMAP.md` - Future development plans
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration audit

## License

MIT License - See LICENSE file for details.
