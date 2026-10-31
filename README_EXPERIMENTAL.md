# NLM Quick Start Guide

## Welcome to NLM (Neural Learning Machine)

This guide will help you get started with NLM - an experimental artificial developmental brain project. NLM demonstrates how complex cognitive abilities can emerge from neural dynamics and plasticity without symbolic AI.

## What NLM Is

NLM simulates a brain with:
- **Neurons**: Leaky Integrate-and-Fire (LIF) spiking neurons
- **Connections**: Event-driven synaptic connections with plasticity
- **Memory**: Working, episodic, and associative memory systems
- **Motivation**: Neuromodulators like dopamine and curiosity
- **Development**: Plasticity changes across "age" stages
- **Learning**: STDP, Hebbian learning, and reward-modulated plasticity

## What NLM Is Not

- **NOT** a transformer or LLM
- **NOT** a chatbot
- **NOT** traditional deep learning
- **NOT** human-like intelligence or consciousness

## Installation

### 1. Build from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Run Demos

```bash
# Go to build directory
cd build

# Run Phase 2: Basic neural computation (demonstrates core brain mechanics)
./nlm

# Or use the unified demo launcher (recommended)
./nlm_demo phase2

# View help information
./nlm_demo --help
```

## First Demos

### Phase 2: Real Neural Computation

Run this demo to see the core neural mechanics in action:

```bash
# Short version (faster, fewer neurons)
./nlm_demo phase2

# Show what Phase 2 demonstrates
./nlm_demo phase2 --help
```

**What you'll see:**
- ~500 neurons with random connections
- Sensory input injected into some neurons
- Spikes propagating through the network
- Synaptic weights changing through learning
- A functioning artificial neural substrate

### Phase 3: World Interaction

Run this demo to see how the brain can interact with and learn from an environment:

```bash
# Default: 5 episodes, 200 steps each
./nlm_demo phase3

# Custom settings
./nlm_demo phase3 --episodes 10 --steps 300
```

**What you'll see:**
- Autonomous agent exploring a 2D world
- Discovering resources (eats them, gains energy, positive reward)
- Avoiding hazards (takes damage, negative reward)
- Learning which actions work and which don't
- Remembering past experiences

### Phase 4: Emerging Cognition

Run this demo to see how complex cognition emerges from simple neural mechanisms:

```bash
# Default: 50 trials
./nlm_demo phase4

# With more neurons for complex behavior
./nlm_demo phase4 --neurons 2000 --trials 100
```

**What you'll see:**
- Temporal prediction (anticipating future events)
- Working memory (keeping information across delays)
- Concept formation (discovering patterns without labels)
- Planning (selecting sequences of actions)
- Self-awareness (understanding body and actions)

### Phase 6: Integration

Run this comprehensive test to verify all brain systems work together:

```bash
# Integration demo
./nlm_demo phase6

# All integration tests
./nlm_demo phase6_all
```

## Understanding the Brain

### Key Components

1. **Neurons**: Cells that receive inputs, integrate them, and can fire spikes
2. **Synapses**: Connections between neurons that transmit signals
3. **Memory Systems**: Working memory (active), episodic (experiences), associative (pattern links)
4. **Neuromodulators**: Chemicals that affect learning and motivation
   - Dopamine: Reward prediction error
   - Curiosity: Novelty seeking
   - Novelty: Change detection
5. **Development**: Changes in plasticity as the brain "ages"

### Learning Mechanisms

1. **STDP**: Spike-Timing Dependent Plasticity
   - Strengthen synapses when pre before post
   - Weaken synapses when pre after post

2. **Hebbian Learning**: "Cells that fire together, wire together"
   - Co-activity strengthens connections

3. **Homeostatic Plasticity**: Maintains stability
   - Prevents runaway excitation
   - Keeps firing rates in a healthy range

## Configuration

### Configuration Files

Configuration can be set in files like `configs/default.cfg`:n

```bash
# View default configuration
cat configs/default.cfg
```

**Key Parameters to Know:**

- `neuron_count`: Number of neurons (500-5000 recommended)
- `region_count`: Brain regions (1-4)
- `connection_probability`: Sparsity of connections (0.01-0.1 recommended)
- `stdp_ltp_weight`: Learning strength for LTP
- `stdp_ltd_weight`: Learning strength for LTD
- `enable_homeostatic_plasticity`: Prevents runaway excitation

### Command Line Overrides

```bash
# Override configuration values
./nlm_demo phase3 --neurons 1000 --episodes 5

# Enable verbose output
./nlm_demo phase4 --verbose

# Enable/disable features
./nlm_demo phase3 --no-homeostatic
```

## Performance Tips

### For Faster Runs

```bash
# Fewer neurons, shorter episodes
./nlm_demo phase3 --neurons 200 --episodes 2 --steps 100
```

### For More Realistic Behavior

```bash
# More neurons, proper development
./nlm_demo phase4 --neurons 2000 --trials 50
```

### For Debugging

```bash
# See detailed output
./nlm_demo phase2 --verbose

# Run quietly (less output)
./nlm_demo phase3 --quiet
```

## Learning NLM

### Start Simple

1. **Run Phase 2**: See the basic neural computation
2. **Run Phase 3**: See world interaction and learning
3. **Run Phase 4**: See emerging cognition

### Explore Advanced Features

1. **Checkpoints**: Save/load brain state
2. **Custom configs**: Create your own settings
3. **Benchmark**: Measure performance
4. **All demos**: See everything together

### Advanced Usage

```bash
# Create custom configuration
cat > my_config.cfg << EOF
random_seed = 12345
neuron_count = 1500
connection_probability = 0.02
stdp_ltp_weight = 0.003
stdp_ltd_weight = 0.005
enable_homeostatic_plasticity = true
EOF

# Use custom config
./nlm_demo phase3 --config my_config.cfg --episodes 10
```

## Common Issues

### Issue: No Learning
**Solution:** Check plasticity parameters and ensure learning rates are set properly

### Issue: Too Much Activity (spikes everywhere)
**Solution:** Reduce `connection_probability` and enable homeostatic plasticity

### Issue: Slow Performance
**Solution:** Reduce number of neurons or trials

### Issue: No Output
**Solution:** Use `--verbose` flag or check log level

## Getting Help

### Help Commands

```bash
# General help
./nlm_demo --help

# Help for specific demo
./nlm_demo phase3 --help

# Show version and system info
./nlm_demo --version
```

### Documentation

- **Main docs:** `docs/` directory
- **Phase 6 audit:** `docs/PHASE6_FINAL_AUDIT.md`
- **Architecture:** `docs/ARCHITECTURE.md`
- **Science:** `docs/SCIENCE.md`

### Community

This is an experimental research project. For questions about the science behind NLM, consult the documentation files.

## Next Steps

### Once You Understand the Basics

1. **Create your own configurations**
2. **Modify existing demos**
3. **Add visualization capabilities**
4. **Explore Python bindings** (if available)

### For Research

1. **Run experiments with different parameters**
2. **Document your findings**
3. **Share configurations with others**

## Example: Custom Learning Experiment

```bash
# Create experimental configuration
cat > experiment.cfg << EOF
# Experiment parameters
random_seed = 99999
neuron_count = 800
region_count = 2
connection_probability = 0.015

# Learning parameters (conservative for stability)
stdp_ltp_weight = 0.002
stdp_ltd_weight = 0.003
stdp_tau = 20.0

# Homeostatic control
enable_homeostatic_plasticity = true
synaptic_scaling_interval = 200
synaptic_scaling_target = 2.0

# Environment settings
environment_width = 30
environment_height = 30
EOF

# Run experiment
./nlm_demo phase3 \
    --config experiment.cfg \
    --episodes 8 \
    --steps 250 \
    --verbose
```

## Summary

NLM is a research project demonstrating that:

- Complex behavior can emerge from simple neural mechanisms
- Learning and memory can arise from synaptic plasticity alone
- Cognitive abilities can develop from environmental interaction
- A brain-like system can solve problems without symbolic AI

The unified demo launcher `nlm_demo` provides a consistent interface to explore all aspects of this experimental brain system.

Enjoy exploring the NLM brain!

---

*Phase 6 Final Integration (2026-10-31) - All major systems successfully integrated and verified.*
