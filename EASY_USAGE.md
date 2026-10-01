# NLM Quick Start Guide

This guide provides the essential information needed to start using NLM (Neural Learning Machine) neural simulations.

## What is NLM?

NLM (熙然) is an experimental artificial developmental brain that implements real spiking neural computation from first principles. It demonstrates how complex cognitive abilities can emerge from simple neural dynamics and plasticity rules without any predefined knowledge or symbolic AI.

## Key Features

- **Real neural computation** with Leaky Integrate-and-Fire (LIF) neurons
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity rules**: STDP, Hebbian, structural plasticity
- **Integrated learning systems**: memory, prediction, cognition
- **Developmental stages** from initial to aging
- **World interaction** with sensory-motor loops

## Getting Started

### 1. Build NLM

If you haven't built NLM yet, follow these steps:

```bash
# Clone the repository (if needed)
git clone <repository-url>
cd nlm

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallelization
make -j4
```

After building, you'll have these executables:

- `nlm` - Phase 2 real neural computation test suite
- `nlm_phase3_demo` - Phase 3 world interaction demo  
- `nlm_phase4_demo` - Phase 4 cognitive experiments
- `nlm_phase6_demo` - Phase 6 integration test

### 2. Run a Simple Simulation

#### Phase 2: Real Neural Computation

The simplest way to start is with Phase 2, which demonstrates the core neural computation capabilities:

```bash
# Run the basic test suite
./nlm

# With verbose output
./nlm --verbose

# With custom neuron count
./nlm --set neuron_count=1000

# Run specific tests only
./nlm --test 1    # Basic connectivity test
./nlm --test 2    # Plasticity experiment
./nlm --test 3    # STDP verification
```

#### Phase 3: World Interaction

Phase 3 demonstrates how the brain interacts with and learns about a simple world:

```bash
# Run the world interaction demo
./nlm_phase3_demo

# Customize the simulation
./nlm_phase3_demo 10           # 10 episodes (default: 5)
./nlm_phase3_demo 10 1000      # 10 episodes, 1000 steps each

# With logging
./nlm_phase3_demo --verbose --log-level INFO
```

#### Phase 4: Cognitive Experiments

Phase 4 demonstrates the emergence of complex cognitive abilities:

```bash
# Run cognitive experiments
./nlm_phase4_demo

# Customize for faster testing
./nlm_phase4_demo 20           # 20 trials (default: 50)
```

#### Phase 6: Integration Test

Phase 6 verifies that all systems work together correctly:

```bash
# Run integration test
./nlm_phase6_demo

# With custom settings
./nlm_phase6_demo --neuron-count 2000 --max-steps 5000
```

### 3. Configuration Management

NLM uses configuration files to control simulation parameters. Configuration files use a simple key=value format.

#### Create a Configuration File

```bash
# Create a minimal configuration
cat > my_config.cfg << EOF
neuron_count = 500
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42
EOF
```

#### Use Your Configuration File

```bash
# Run simulation with custom configuration
./nlm --config my_config.cfg

# Override values from command line
./nlm --config my_config.cfg --set neuron_count=1000
```

### 4. Python API

For programmatic access, use the Python bindings:

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation step
brain.step(0, 0.0)

# Create agent for world interaction
world = pynlm.createSimpleWorld()
world.configure(20, 20, 16, 16)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a complete episode
for episode in range(10):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Simulate brain
    for _ in range(10):
        brain.step(episode * 1000 + _)
    
    # Get motor command
    cmd = agent.decodeMotorCommand()
    
    # Apply action
    result = world.applyMotorCommand(cmd, world.getSimulationTime())
    
    # Update world and development
    world.update(0.01)
    agent.updateDevelopment(0.01)
```

## Learning Path

### For Beginners

1. **Start with Phase 2**: Run `./nlm` to see the basic neural computation in action
2. **Try Phase 3**: Experience how the brain interacts with its environment
3. **Explore configurations**: Modify parameters to see how they affect behavior

### For Researchers

1. **Phase 4 experiments**: Test specific cognitive mechanisms
2. **Phase 6 integration**: Verify system compatibility
3. **Python API**: Build custom experiments and analyses
4. **Configuration optimization**: Find optimal parameters for specific tasks

### For Developers

1. **CLI framework**: Understand the unified argument parsing system
2. **Core architecture**: Explore the brain and neural systems
3. **Performance optimization**: Use benchmarking tools
4. **Code contributions**: Extend the functionality

## Common Tasks

### 1. Running a Custom Simulation

```bash
# Create custom configuration
cat > custom.cfg << EOF
neuron_count = 1000
region_count = 3
connection_probability = 0.2
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
simulation_timestep = 0.005
random_seed = 12345
EOF

# Run with custom settings
./nlm_phase3_demo \
    --episodes 15 \
    --steps 800 \
    --neuron-count 800 \
    --verbose \
    --config custom.cfg \
    --set random_seed=999
```

### 2. Saving and Loading Simulation State

```bash
# Save brain state (Phase 3 example)
./nlm_phase3_demo --episodes 3 --steps 200 > output.log

# Note: Checkpoint functionality requires additional implementation
# For now, simulation state is not saved to disk
```

### 3. Performance Testing

```bash
# Run benchmark for Phase 4
./nlm_phase4_demo --trials 100 > performance.log

# Monitor system resources
# (Use external tools like 'top', 'htop', or 'htop-ng')
```

### 4. Configuration Validation

```bash
# Create configuration with validation issues
cat > bad_config.cfg << EOF
neuron_count = 50          # Too few neurons
connection_probability = 1.5  # Invalid probability
random_seed = "not_a_number"  # Wrong type
EOF

# Run simulation (may produce warnings)
./nlm --config bad_config.cfg
```

## Troubleshooting

### Problem: "Command not found"

**Solution**: Ensure you've built the project and are in the build directory:

```bash
cd /path/to/nlm/build
./nlm
```

### Problem: "Invalid configuration file"

**Solution**: Check the configuration file syntax:

```bash
# Use a valid configuration file format
cat > good_config.cfg << EOF
neuron_count = 500
region_count = 2
connection_probability = 0.1
EOF

# Verify file exists and is readable
ls -la good_config.cfg

# Run simulation
./nlm --config good_config.cfg
```

### Problem: Simulation takes too long

**Solution**: Reduce simulation parameters:

```bash
# Reduce neuron count
./nlm --set neuron_count=100

# Reduce number of trials/steps  
./nlm_phase3_demo --episodes 3 --steps 300
```

### Problem: Memory usage too high

**Solution**: Use fewer neurons and regions:

```bash
# Use minimal configuration for memory constraints
./nlm --set neuron_count=200 --set region_count=1
```

## Getting Help

### Command Line Help

All NLM executables provide context-sensitive help:

```bash
# General help
./nlm --help

# Specific help for executables
./nlm_phase3_demo --help
./nlm_phase4_demo --help
```

### Project Documentation

For detailed information, see:

- `README.md` - Project overview and summary
- `easy_usage.md` - Quick start guide
- `HOW_TO_USE.md` - Comprehensive user guide
- `CLI_GUIDE.md` - This CLI guide
- `docs/` - Additional technical documentation

### Python API Documentation

For Python bindings documentation, see the docstrings in the Python code:

```python
import pynlm
help(pynlm)
```

## Next Steps

1. **Explore the documentation** to understand all available features
2. **Try different configurations** to see how they affect behavior
3. **Compare results** across different simulation settings
4. **Consider contributing** to the project
5. **Join the community** for discussions and support

## Version Information

Current version: Phase 2.0
CLI Framework: Version 1.0

## Support

For issues, questions, or suggestions:

- Check the project repository for existing issues
- Consult the documentation for troubleshooting
- Review examples and community contributions
- Report bugs with detailed information

## License

This project is part of the NLM (熙然) research project. See LICENSE file for details.
