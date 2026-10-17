# NLM Getting Started Guide

## Welcome to NLM (熙然)

NLM (熙然) is an experimental artificial developmental brain project. This guide will help you get started with using NLM, from basic demonstrations to advanced experiments.

## What is NLM?

NLM (熙然, meaning "serene flow") is a computational brain project that:

- Starts in a primitive developmental state
- Acquires increasingly complex abilities through interaction with an environment
- Implements real spiking neural computation using Leaky Integrate-and-Fire (LIF) neurons
- Includes multiple cognitive systems (memory, prediction, planning, attention)
- Features neuromodulation (dopamine, curiosity, novelty detection)
- Has persistence mechanisms (checkpointing, replay, consolidation)

**IMPORTANT:** NLM is NOT a transformer, LLM, chatbot, deep learning model, or pretrained AI system. It's a research project investigating computational brain-like systems.

## System Requirements

- C++ compiler with C++20 support
- CMake 3.16 or higher
- Make
- A few gigabytes of RAM for large simulations

## Building NLM

### Basic Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Build Options

You can customize the build with these CMake options:

- `NLM_ENABLE_TESTS=ON` - Build and run tests
- `NLM_ENABLE_BENCHMARKS=ON` - Build benchmarks
- `NLM_ENABLE_EXPERIMENTS=ON` - Build experiment binaries
- `NLM_ENABLE_PYTHON_BINDINGS=ON` - Build Python bindings

Example:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_EXPERIMENTS=ON
```

## Quick Start

### Method 1: Using the Demo Script

The easiest way to get started is with the demo script:

```bash
# Make the script executable (first time only)
chmod +x ./nlm_demo.sh

# Run quick demonstration
./nlm_demo.sh --quick

# Show getting started guide
./nlm_demo.sh --getting-started

# Show system information
./nlm_demo.sh --info
```

### Method 2: Using NLM CLI

NLM now has a comprehensive command-line interface:

```bash
# Show help and all available commands
nlm --help

# Run integration demonstration (default)
nlm demo

# Run Phase 6 integration experiment
nlm experiment --steps 1000

# Verify all systems are integrated
nlm verify

# Show current system status
nlm status

# Show configuration information
nlm config

# Show getting started guide
nlm quickstart
```

### Method 3: Running Experiments Directly

You can also run specific experiments:

```bash
# Run Phase 6 integration experiment with custom settings
nlm experiment \
    --steps 5000 \
    --neuron-count 500 \
    --enable-checkpointing \
    --enable-replay \
    --enable-development

# Run quick demo for new users
nlm demo --steps 100 --quiet

# Run integration verification
nlm verify --quiet

# Show system benchmarks
nlm benchmark --runs 3
```

## Understanding NLM Modes

### 1. Demo Mode (`nlm demo`)

The demo mode runs a comprehensive integration demonstration that tests all major components:

**What it does:**
- Verifies integration of all cognitive systems
- Tests memory system functionality
- Validates neuromodulation systems
- Demonstrates checkpoint save/load
- Shows replay system functionality
- Runs a full integration experiment

**Typical output:**
```
=== NLM Phase 6 Integration Demo ===
Running comprehensive integration demonstration...

--- Integration Verification ---
[PASS] Working memory is integrated
[PASS] Episodic memory is integrated
[PASS] Dopamine system is integrated
[PASS] Curiosity system is integrated
[PASS] Prediction system is integrated
[PASS] Neural planner is integrated
[PASS] Concept formation is integrated
[PASS] Attention is integrated
[PASS] Development system is integrated

=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING
Replay: WORKING
```

### 2. Experiment Mode (`nlm experiment`)

The experiment mode runs a complete Phase 6 integration experiment with customizable parameters:

**Available options:**
- `--steps N` - Number of simulation steps (default: 10000)
- `--neuron-count N` - Number of neurons (default: 1000)
- `--region-count N` - Number of brain regions (default: 1)
- `--connection-prob P` - Connection probability (default: 0.1)
- `--enable-checkpointing` - Enable checkpoint save/load
- `--enable-replay` - Enable memory replay
- `--enable-development` - Enable developmental stages
- `--experiment-type TYPE` - Type of experiment (full, quick, detailed)

### 3. Status Mode (`nlm status`)

Shows current system status and brain statistics:

**Information displayed:**
- Basic brain statistics (neurons, synapses, firing rates)
- System integration status (which components are working)
- Memory system statistics
- Current configuration

### 4. Verify Mode (`nlm verify`)

Verifies that all integrated systems are working correctly:

**Tests performed:**
- Working memory integration
- Episodic memory integration
- Neuromodulation systems (dopamine, curiosity)
- Prediction system integration
- Cognitive systems (planner, concept formation, attention)
- Development system

### 5. Configuration Mode (`nlm config`)

Shows current configuration parameters:

**Displayed parameters:**
- Core parameters (simulation timestep, neuron count, etc.)
- Plasticity parameters (STDP weights, rates)
- Development parameters (replay intervals)

### 6. Benchmark Mode (`nlm benchmark`)

Runs performance benchmarks to test system speed:

**What it tests:**
- Brain initialization speed
- Simulation performance (steps per second)
- Memory usage
- Different brain sizes

## Learning NLM

### Understanding the Brain Systems

NLM includes these integrated systems:

#### Memory Systems
- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience storage with replay and consolidation
- **Associative Memory**: Pattern associations and connections

#### Neuromodulation
- **Dopamine**: Reward-based reinforcement learning
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning from prediction mismatches

#### Cognitive Systems
- **Neural Planner**: Action sequence evaluation
- **Concept Formation**: Pattern discovery from experience
- **Attention**: Competitive selection dynamics

#### Other Systems
- **Prediction System**: Sensory prediction and error computation
- **Development System**: Age-based plasticity modifiers
- **Structural Plasticity**: Synaptogenesis and pruning

### Interpreting Output

When running experiments, look for these key indicators:

#### Successful Integration Signs:
- All verification tests pass
- Memory systems report active traces/episodes
- Neuromodulators show activity
- Learning occurs (synaptic weight changes)
- Checkpoint operations complete successfully

#### Potential Issues:
- Missing memory traces (may be normal for simple tasks)
- No episodes (may occur with short simulations)
- No significant weight changes (may need longer training)

## Advanced Usage

### Custom Configuration Files

Create custom configuration files in the `configs/` directory:

```ini
# Example config file: configs/custom.cfg
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.01
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
replay_interval = 200
consolidation_interval = 2000
```

Use with: `nlm --config configs/custom.cfg experiment`

### Scripting and Automation

For automated testing and research:

```bash
#!/bin/bash
# Run experiment and save results
nlm experiment --steps 2000 --quiet > experiment_results.txt 2>&1

# Run verification and capture output
nlm verify --quiet > verification_results.txt 2>&1

# Run benchmark for performance analysis
nlm benchmark --runs 5 > benchmark_results.txt 2>&1
```

### Integration with Other Tools

NLM's integrated systems can be used as building blocks:

- The brain interface can be integrated into larger simulation frameworks
- Memory systems can store experiment results
- Neuromodulation can drive exploration in other systems
- Prediction errors can guide learning in complementary systems

## Troubleshooting

### Common Issues

**Issue: NLM executable not found**

Solution: Build NLM:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**Issue: Demo fails to run**

Solution: Check system requirements and build options. Make sure all components are enabled:
```bash
cmake .. -DNLM_ENABLE_EXPERIMENTS=ON -DNLM_ENABLE_BENCHMARKS=ON
```

**Issue: No learning detected**

Solution: Try longer simulations or adjust plasticity parameters:
```bash
nlm experiment --steps 10000 --enable-development
```

**Issue: Memory systems not working**

Solution: Check integration status:
```bash
nlm verify
```

**Issue: Performance is slow**

Solution: Try smaller brain size or run benchmarks:
```bash
nlm benchmark --runs 3
nlm experiment --neuron-count 500
```

### Getting Help

If you encounter issues:

1. **Check the logs**: Enable verbose mode for more details
2. **Run verification**: `nlm verify` to check system integration
3. **Check system status**: `nlm status` for current brain state
4. **Review documentation**: This guide and the NLM documentation
5. **Community resources**: Visit the NLM GitHub repository

## Further Reading

### Technical Documentation

- **ARCHITECTURE.md**: Detailed system architecture
- **SCIENCE.md**: Scientific background and principles
- **ROADMAP.md**: Development roadmap and future plans
- **EXPERIMENTS.md**: Experimental methodology
- **PHASE6_FINAL_AUDIT.md**: Phase 6 integration audit

### Research Papers

- [NLM Phase 6 Integration Results](https://github.com/nlm-project/nlm/blob/main/docs/PHASE6_FINAL_RESULTS.md)
- [NLM Limitations and Future Directions](https://github.com/nlm-project/nlm/blob/main/docs/LIMITATIONS.md)

### Examples and Tutorials

- **Phase6Demo**: Basic integration demonstration
- **Phase6IntegratedExperiment**: Complete lifetime experiment
- **Phase5Experiment**: Performance and scaling experiments
- **Phase4Experiment**: Cognitive system demonstrations

## Contribution Guidelines

### Code Standards

- Follow the existing C++ coding style
- Use consistent naming conventions (camelCase for methods, UPPER_CASE for constants)
- Include comprehensive comments and documentation
- Follow the project's license (MIT)

### Testing

- All new code should have corresponding tests
- Tests should be run with `ctest` or `make test`
- Benchmarks should be updated when performance changes

### Experiment Design

When adding new experiments:
- Follow the existing experiment framework
- Use proper configuration management
- Include detailed documentation
- Provide clear usage examples

## Support and Community

### Getting Help

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Check the NLM documentation
- **Examples**: Use the demo script and examples
- **Community**: Join discussions in the repository

### Project Status

Current status (as of Phase 6):

- **Neural Core**: ✅ Working (LIF neurons, spikes, plasticity)
- **Memory Systems**: ✅ Connected (working, episodic, associative)
- **Neuromodulation**: ✅ Working (dopamine, curiosity, novelty)
- **Cognition Systems**: ✅ Connected (planner, concept formation, attention)
- **Prediction System**: ✅ Connected
- **Development System**: ✅ Minimal integration (needs more work)
- **Persistence**: ⚠️ Stubs (checkpoint system implemented but not fully connected)
- **Embodiment**: ✅ Working (Agent/SimpleWorld integration)
- **Experiments**: ✅ Framework exists, limited execution

**Overall Integration Score**: 43/120 (35.8%) - Focus is on integration over new features.

## Summary

NLM represents a sophisticated but not yet fully integrated artificial brain. The core neural computation systems are working, but many cognitive systems need better integration into the main brain loop.

For most users, NLM provides:
- A functioning spiking neural network
- Memory systems for experience storage
- Neuromodulation for learning guidance
- Basic cognitive capabilities
- Persistence mechanisms for long-term learning

The primary goal of Phase 6 is to **connect existing systems** rather than add new features. The codebase has excellent architecture with all the components needed for a sophisticated artificial brain - the work now focuses on making them work together.

**Get started today with:**
```bash
./nlm_demo.sh --quick
```

Or explore the full capabilities:
```bash
nlm help
```

Enjoy exploring the world of computational brain simulation!