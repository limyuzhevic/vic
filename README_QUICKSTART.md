# NLM (Neural Learning Machine) - Quick Start

## Welcome to NLM!

NLM is an experimental artificial developmental brain system that simulates neural learning and behavior. This document provides quick start instructions for the improved version.

## Quick Start Options

### Option 1: Python (Recommended for research and prototyping)

```bash
# Install Python bindings
pip install -e .

# Create a simple test script
test_script.py
```

```python
import pynlm

# Create brain with demo configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(16, 16)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step, step * 0.001)
    
    cmd = agent.decodeMotorCommand()
    result = world.applyMotorCommand(cmd, step * 0.001)
    agent.applyRewardModulation(result.reward, 0.0)

# Analyze results
brain.logStatus()
```

### Option 2: Command Line Interface

```bash
# Run the CLI help
./nlm_cli.py --help

# Run a quick demo
./nlm_cli.py demo --steps 500

# Initialize and run a simulation
./nlm_cli.py init --width 20 --height 20
./nlm_cli.py run --steps 2000

# Analyze brain state
./nlm_cli.py analyze --output results.json
```

### Option 3: C++ (Maximum performance)

```bash
# Build the system
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the demo
./nlm_phase6_demo

# Run tests
ctest --output-on-failure
```

## Key Improvements

This version includes significant improvements:

### 1. Fixed Novelty System
- Novelty now properly detects changes in sensory input
- Promotes exploration behavior
- Modulates neural excitability based on novelty

### 2. Enhanced Checkpointing
- Complete synapse loading implementation
- Accurate brain state restoration
- Better error handling for mismatched data

### 3. Performance Optimizations
- Statistics collection in single pass through regions
- Reduced redundant iterations
- More efficient memory usage

### 4. Rich Python Bindings
- Novelty system exposed to Python
- Helper functions for common operations
- Better error handling

### 5. Command Line Interface
- Comprehensive CLI tool for all operations
- Easy simulation management
- Checkpoint handling
- Brain analysis

### 6. Complete Documentation
- Extensive user guide
- API documentation
- Examples and troubleshooting

## Examples Directory

The `examples/` directory contains additional usage examples:

- `simple_demo.py`: Basic simulation with Python bindings
- `checkpoint_demo.py`: Checkpoint management example
- `analysis_demo.py`: Advanced brain analysis
- `config_demo.py`: Configuration management

## Testing

Run the comprehensive test suite:

```bash
cd build
ctest --output-on-failure
```

The test suite includes:
- Integration verification tests
- Memory system integration tests
- Neuromodulation integration tests
- Checkpointing tests
- Replay system tests
- Performance tests

## Support

For issues, questions, or suggestions:
1. Check the documentation in `docs/USER_GUIDE.md`
2. Review `CODE_IMPROVEMENTS.md` for implementation details
3. Use the CLI tool for debugging and analysis
4. Check the examples directory for additional usage patterns

## License

MIT License

Copyright (c) 2026 NLM Project

## Quick Setup Script

For automated setup and build, use the `setup.sh` script:

```bash
chmod +x setup.sh
./setup.sh
```

Select from the available options (1-5) to build and run the system.

---

**Enjoy exploring the NLM neural learning machine!**
