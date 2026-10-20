# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm --demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Full Integration Test

```bash
./nlm
```

This runs the complete Phase 6 integration test with full testing of all neural systems.

### Custom Configuration

Create a custom configuration file and run:

```bash
./nlm --config custom.cfg
```

### Save and Load Brain States

To save a trained brain state:

```bash
./nlm --save trained_brain.json
```

To load a previously saved brain state:

```bash
./nlm --load trained_brain.json
```

### Test-Only Mode

For quick testing without demonstrations:

```bash
./nlm --test-only
```

### Development and Debugging

For development and debugging with verbose output:

```bash
./nlm --verbose --help
```

## Project Structure

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
├── configs/
│   └── default.cfg
├── python/
│   ├── bindings.cpp              # Python bindings
│   └── setup.py                 # Build configuration
├── src/
│   ├── core/                   # Core utilities
│   ├── brain/                  # Neural components (integrated)
│   ├── dynamics/               # Neural dynamics
│   ├── plasticity/             # Plasticity rules
│   ├── development/            # Developmental system
│   ├── neuromodulation/        # Neuromodulators (integrated)
│   ├── memory/                # Memory systems (integrated)
│   ├── prediction/            # Prediction systems (integrated)
│   ├── cognition/             # Cognitive mechanisms (integrated)
│   ├── sensory/               # Sensory processing
│   ├── motor/                 # Motor system
│   ├── environment/           # Environment interface
│   ├── experiments/           # Experiment framework (Phase 6)
│   └── visualization/         # Visualization
├── tests/
└── easy_usage.md              # Quick start guide
└── HOW_TO_USE.md               # Detailed usage instructions
```

## Configuration Options

### Core Parameters

- `random_seed`: Random seed for reproducible experiments (default: 42)
- `simulation_timestep`: Simulation timestep in seconds (default: 0.001)
- `neuron_count`: Number of neurons in the brain (default: 500)
- `region_count`: Number of brain regions (default: 1)
- `connection_probability`: Connection probability for random connectivity (default: 0.15)

### Plasticity Settings

- `plasticity_learning_rate`: Learning rate for plasticity (default: 0.01)
- `stdp_ltp_weight`: STDP Long-Term Potentiation weight (default: 0.02)
- `stdp_ltd_weight`: STDP Long-Term Depression weight (default: 0.015)
- `stdp_time_constant`: STDP time constant (default: 20.0)

### Development Settings

- `development_synaptogenesis_rate`: Development synaptogenesis rate (default: 0.0001)
- `development_pruning_rate`: Development pruning rate (default: 0.00001)

### Neuromodulation Settings

- `dopamine_baseline`: Dopamine baseline level (default: 0.1)
- `reward_discount_factor`: Reward discount factor (default: 0.99)

### Environment Settings

- `environment_name`: Environment name (default: GridWorld)
- `environment_width`: Environment width (default: 20)
- `environment_height`: Environment height (default: 20)

### Logging Settings

- `log_level`: Logging level (DEBUG, INFO, WARN, ERROR) (default: INFO)
- `log_to_file`: Log to file (true/false) (default: false)
- `log_filename`: Log filename (default: nlm.log)

### Simulation Settings

- `max_simulation_steps`: Maximum simulation steps (default: 10000)
- `simulation_time_limit`: Simulation time limit in seconds (default: 0.0)

### Visualization Settings

- `visualization_enabled`: Enable visualization (true/false) (default: false)
- `visualization_update_rate`: Visualization update rate (default: 30.0)

## Command Line Interface

The NLM executable supports the following command line options:

### General Options

- `--help, -h`: Show this help message
- `--version`: Show version information
- `--verbose, -v`: Enable verbose logging
- `--quiet, -q`: Suppress output

### Configuration Options

- `--config FILE, -c FILE`: Load configuration from FILE (default: configs/default.cfg)
- `--save STATE, -s STATE`: Save brain state to STATE after execution
- `--load STATE, -l STATE`: Load brain state from STATE before execution

### Simulation Control

- `--demo`: Run demonstration mode (shortened tests)
- `--test-only`: Run only tests, skip demonstrations

### Experiment Control

- `--random-seed SEED, -r SEED`: Override random seed
- `--neuron-count COUNT, -n COUNT`: Override number of neurons
- `--simulation-timestep TS, -t TS`: Override simulation timestep

## Python API

The NLM framework provides comprehensive Python bindings via the `pynlm` module. Here's a quick example:

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Create a world
world = pynlm.createSimpleWorld()

# Create an agent brain
agent = pynlm.createAgentBrain(brain)

# Run a few simulation steps
for step in range(100):
    brain.step(step, step * 0.001)

# Save the brain state
brain.save("my_brain.json")
```

### Available Python Classes

- `Config`: Configuration management
- `Brain`: Central neural simulation brain
- `AgentBrain`: Agent brain interface
- `SimpleWorld`: 2D world simulation
- `Vision`: Visual sensory input
- `Audio`: Audio sensory input
- `InternalSignals`: Internal state signals
- `Action`: Motor actions
- `ActionResult`: Action execution results
- `SensoryPercept`: Integrated sensory data
- `WorldObject`: World objects (resources, hazards, etc.)
- `AgentBody`: Agent physical state

### Key Methods

- `brain.step(step, time)`: Advance simulation by one step
- `brain.receiveSensoryInput(input)`: Inject sensory data
- `brain.produceAction()`: Generate motor command
- `world.update(timestep)`: Update world state
- `brain.save(filepath)`: Save brain state
- `brain.load(filepath)`: Load brain state

## Getting Started

### Quick Start

1. **Build the project:**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the demonstration:**
   ```bash
   ./nlm --demo
   ```

3. **Explore the Python API:**
   ```bash
   python3 -c "import pynlm; print(pynlm.version())"
   ```

### Learning Resources

- **Architecture Documentation**: See `docs/ARCHITECTURE.md` for detailed system design
- **Science Documentation**: See `docs/SCIENCE.md` for scientific background
- **Roadmap**: See `docs/ROADMAP.md` for future development plans
- **Experiments**: See `docs/EXPERIMENTS.md` for experimental protocols
- **Limitations**: See `docs/LIMITATIONS.md` for known limitations
- **Phase 6 Audit**: See `docs/PHASE6_FINAL_AUDIT.md` for final integration audit

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

### Ethical Considerations

NLM is an experimental research tool designed for scientific investigation. Users should:

- Use NLM responsibly and ethically
- Respect intellectual property and research integrity
- Follow all applicable laws and regulations
- Ensure proper oversight and validation of research results
- Consider the broader implications of brain-inspired research

## License

MIT

## Authors

Research project — See docs for scientific background.

## Support and Community

For questions, issues, or collaboration opportunities:

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Contact**: See AUTHORS file for project contacts

## Troubleshooting

### Common Issues

1. **Build fails**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Python bindings not found**
   ```bash
   pip install -e .
   ```

3. **Configuration file errors**
   Check `configs/default.cfg` for syntax errors

4. **Memory issues**
   Reduce neuron count in configuration: `neuron_count = 100`

5. **Performance issues**
   Use `--demo` mode for faster testing
   Reduce simulation time with `--simulation-timestep 0.01`

### Getting Help

If you encounter issues not covered above:

1. Check the documentation in `docs/`
2. Search for existing issues in the repository
3. Create a new issue with detailed error information
4. Check the development logs in `nlm.log`

## Future Development

### Planned Features

1. **Enhanced Memory Systems**
   - Long-term memory consolidation
   - Sleep and wake cycles
   - Memory replay mechanisms

2. **Advanced Neuromodulation**
   - Multiple neuromodulators
   - Complex reward learning
   - Motivational systems

3. **Improved Cognition**
   - Theory of mind
   - Social learning
   - Abstract reasoning

4. **Expanded Environments**
   - 3D world simulation
   - Richer sensory modalities
   - Complex task environments

5. **Development and Maturation**
   - Lifespan development
   - Aging processes
   - Developmental disorders

6. **Advanced Learning**
   - Continual learning
   - Transfer learning
   - Meta-learning

### Research Directions

- **Brain-Computer Interface Integration**
- **Neuromorphic Computing**
- **Cognitive Architecture**
- **Neural Engineering**
- **Artificial General Intelligence**

## Conclusion

NLM represents a comprehensive effort to create an artificial brain that integrates multiple systems into a coherent, functioning whole. While the current implementation is experimental and has limitations, it provides a solid foundation for future research in brain-inspired artificial intelligence.

The project demonstrates that it is possible to integrate memory, neuromodulation, prediction, and development into a single neural system that can learn and adapt through experience. This represents a significant step toward understanding the fundamental principles of brain function and developing artificial systems that can replicate aspects of biological intelligence.

The NLM framework is open to collaboration and welcomes researchers interested in brain-inspired artificial intelligence, computational neuroscience, and cognitive architecture.