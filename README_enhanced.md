## Python API

The NLM framework provides a comprehensive Python API for advanced users. Install dependencies:

```bash
pip install -r python_requirements.txt
```

Basic usage:
```python
from pynlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain

# Create a default configuration
config = createDefaultConfig()

# Create the neural brain
brain = createBrain(config)

# Create a simulation world
world = createSimpleWorld()

# Create an agent brain interface
agent = createAgentBrain(brain)

# Initialize the agent
agent.initialize(world)

# Configure for exploration
agent.enableLearningMode(True)
agent.setExplorationRate(0.3)

# Run exploration
agent.explore(maxSteps=2000)

# Get statistics
stats = agent.getLearningStats()
print(f"Learning statistics: {stats}")
```

### Advanced Python Features

For experienced users, the NLM framework provides advanced exploration and learning capabilities:

#### Experience Replay
```python
# Record experiences during simulation
agent.recordExperience(
    action="MoveForward",
    observation=[0.1, 0.2, 0.3, ...],  # 256-element sensory observation
    reward=0.8,
    terminal=False
)

# Save experience replay for later analysis
agent.saveExperienceReplay("experiences.json")
```

#### Learning and Exploration
```python
# Enable learning mode
agent.enableLearningMode(True)

# Configure exploration rate (0.0 = greedy, 1.0 = random)
agent.setExplorationRate(0.3)

# Run structured exploration
agent.explore(maxSteps=5000)

# Simulate complete episode
agent.simulateEpisode(maxSteps=1000)
```

#### Motor Command Analysis
```python
# Get current motor command activation strengths
strengths = agent.getMotorCommandStrengths()
print("Motor command strengths:", strengths)

# Get recent observations for analysis
observations = agent.getRecentObservations(count=100)

# Get action history for pattern analysis
action_history = agent.getActionHistory(count=50)
```

#### Statistics and Monitoring
```python
# Get learning statistics
learning_stats = agent.getLearningStats()
print(f"Learning statistics: {learning_stats}")

# Get experience count
exp_count = agent.getExperienceCount()
print(f"Recorded experiences: {exp_count}")
```

## Command-Line Tools

The NLM framework includes advanced command-line tools for configuration and simulation:

### nlm_configure

Advanced configuration tool with support for:
- JSON/YAML configuration files
- Command-line argument parsing
- Environment variable configuration
- Schema validation

Example usage:
```bash
nlm_configure --config experiments/phase6_config.json --output custom_config.json
nlm_configure --set neuron_count=1000 --set connection_probability=0.15
```

### nlm_simulate

Advanced simulation controller with support for:
- Multi-stage simulation runs
- Checkpoint management
- Performance profiling
- Result analysis
- Visualization integration

Example usage:
```bash
nlm_simulate --config experiments/phase6_config.json --checkpoint-dir ./checkpoints --max-steps 5000
nlm_simulate --mode debug --verbose --profile-performance
```

### nlm_visualize

Interactive visualization tool for:
- Real-time brain activity monitoring
- Neural population visualization
- Memory system dynamics
- Action selection patterns

Example usage:
```bash
nlm_visualize --brain-data brain_state.bin --world-data world_state.bin --port 8080
```

## New Features Summary

The NLM framework has been enhanced with the following improvements:

### Python API Enhancements

1. **Advanced AgentBrain Features**: Added exploration, learning, and experience replay capabilities for sophisticated usage patterns

2. **Experience Replay System**: Implement persistent memory of agent experiences for learning and analysis

3. **Exploration Strategies**: Configurable exploration behaviors including curiosity-driven and random exploration

4. **Learning Mode**: Enable/disable learning with action value tracking and reinforcement learning support

5. **Performance Monitoring**: Comprehensive statistics collection and analysis tools

### Command-Line Interface

1. **Configuration Tool**: Advanced configuration management with schema validation
2. **Simulation Controller**: Multi-stage simulation runs with checkpoint management
3. **Visualization Tool**: Real-time brain activity monitoring and analysis

### Enhanced Integration

The framework now provides better integration between:
- Memory systems and neural processing
- Neuromodulation and learning mechanisms
- Prediction systems and action selection
- Development stages and plasticity

### Performance Improvements

1. **Experience Replay**: Efficient storage and retrieval of agent experiences
2. **Exploration Optimization**: Curated exploration strategies for efficient learning
3. **Memory Management**: Better management of learning buffers and caches

These enhancements make the NLM framework more accessible to new users while providing advanced capabilities for experienced users and researchers.

## Enhanced Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── python/             # Python bindings and examples
│   ├── examples/       # Usage examples
│   └── advanced/       # Advanced usage patterns
├── tools/              # Command-line utilities
│   ├── configure/      # Configuration tools
│   ├── simulate/       # Simulation tools
│   └── visualize/      # Visualization tools
├── tests/              # Comprehensive test suite
└── configs/            # Default configurations
```

## License

MIT

## Authors

Research project — See docs for scientific background.