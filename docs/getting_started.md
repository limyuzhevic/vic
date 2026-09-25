# NLM Getting Started Guide

## Quick Start (5 Minutes)

This guide will help you get NLM up and running in under 5 minutes. By the end, you'll have a working neural simulation system demonstrating basic learning.

## 1. Installation

### Step 1: Install Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git

# macOS
brew install cmake

# Windows (with Chocolatey)
choco install cmake
```

### Step 2: Clone NLM

```bash
git clone <repository-url>
cd nlm
```

### Step 3: Build NLM

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

Your terminal should show compilation progress. If successful, you'll have:
- `nlm` - Basic neural computation demo
- `nlm_phase3_demo` - Agent-based learning demo
- `nlm_phase4_demo` - Advanced experiment demo

## 2. Running Your First Simulation

### Basic Neural Computation (30 seconds)

Run the basic neural simulation to see spiking neurons in action:

```bash
./nlm
```

**Expected Output:**
- Neural network initialization
- Basic connectivity test (spike propagation)
- Learning experiment (plasticity demonstration)
- STDP verification (synaptic learning)
- Final brain status report

This demonstrates:
- ✅ Real LIF neuron dynamics
- ✅ Event-driven spike propagation  
- ✅ STDP and Hebbian plasticity
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Learning through experience

### Agent-Based Learning (2 minutes)

Run the agent learning demo:

```bash
./nlm_phase3_demo
```

**Expected Output:**
- Agent initialization with sensory capabilities
- World exploration and interaction
- Learning from rewards and punishments
- Behavior development through experience

This demonstrates:
- ✅ Agent perception (vision, touch, proprioception)
- ✅ Motor action selection and control
- ✅ Reward-based learning
- ✅ Developmental behavior progression

## 3. Exploring NLM Features

### Configuration Options

See all available options:

```bash
./nlm --help
```

**Common Configurations:**

```bash
# Small network for quick testing
./nlm --neurons 100 --regions 1

# Medium network for realistic behavior
./nlm --neurons 1000 --regions 2 --timestep 0.001

# Large network with advanced features
./nlm --neurons 5000 --enable-visualization --log-level Debug
```

### Configuration File

Create a custom configuration file:

```bash
cat > my_config.cfg << EOF
{
  "simulation": {
    "neuron-count": 1000,
    "region-count": 1,
    "timestep": 0.001,
    "connection-probability": 0.1
  },
  "plasticity": {
    "stdp": {
      "ltp-weight": 0.02,
      "ltd-weight": 0.015
    }
  },
  "neuromodulation": {
    "curiosity": {
      "exploration-factor": 0.3
    }
  }
}
EOF

# Use the configuration
./nlm --config my_config.cfg
EOF
```

## 4. Understanding NLM Components

### Brain Architecture

NLM implements a complete artificial brain with:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Key Systems

1. **Neural Core**: LIF neurons, spikes, synapses, plasticity
2. **Memory Systems**: Working memory, episodic memory, associative memory
3. **Prediction System**: Predicting outcomes and errors
4. **Cognition**: Planning, concept formation, attention
5. **Development**: Learning developmental stages and maturation
6. **Neuromodulation**: Dopamine, curiosity, novelty, reward
7. **Agent Interface**: Sensory processing and motor control
8. **Environment**: World simulation and interaction

## 5. Example Experiments

### Experiment 1: Learning Demonstration

```bash
# Run learning experiment with detailed logging
./nlm --neurons 2000 --stdp-ltp 0.02 --stdp-ltd 0.015 \
     --hebbian-learning 0.01 --development-stage critical \
     --log-level Info --checkpoint-dir ./checkpoints
```

**What you'll observe:**
- Synaptic weights changing over time (learning)
- Spike patterns evolving (behavior adaptation)
- Memory consolidation during development
- Curiosity-driven exploration

### Experiment 2: Development Study

```bash
# Study developmental progression
./nlm --neurons 1000 --plasticity-mod 1.0 --development-stage initial \
     --replay-interval 100 --consolidation-interval 1000
```

**What you'll observe:**
- High plasticity in initial stage (rapid learning)
- Reduced plasticity during critical period (stable learning)
- Maturation phase (refined behavior)
- Adult stability (consolidated behavior)

### Experiment 3: Performance Analysis

```bash
# Benchmark system performance
./nlm --neurons 5000 --timing --performance-metrics
```

**What you'll observe:**
- Neural computation speed (spikes per second)
- Memory usage patterns
- Scalability with network size
- Resource efficiency

## 6. Saving and Loading Brain States

### Save Brain State
```bash
./nlm --save-brain ./checkpoints/brain_state_1.nlm
```

### Load Brain State
```bash
./nlm --load-brain ./checkpoints/brain_state_1.nlm
```

### Create Backups
```bash
# Create automatic backups
./nlm --backup-config ./backups/config_backup_$(date +%Y%m%d_%H%M%S).cfg
```

## 7. Troubleshooting

### Common Issues and Solutions

**Issue: Build fails**
```bash
# Try with verbose output
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
make -j4 2>&1 | tail -50
```

**Issue: Program runs slowly**
```bash
# Optimize for performance
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DOPTIMIZE_FOR_PERFORMANCE=ON
make -j4
./nlm  # Should be faster now
```

**Issue: Memory usage is high**
```bash
# Reduce memory requirements
./nlm --neurons 500 --working-memory-capacity 50 --episodic-max-episodes 100
```

**Issue: Configuration file not found**
```bash
# Check if file exists
ls -la ./configs/
cat my_config.cfg  # Check syntax
```

### Getting Help

**Command-line help:**
```bash
./nlm --help  # All configuration options
```

**Documentation:**
- **GitHub Repository**: Check for examples and guides
- **Issue Tracker**: Report bugs and request features
- **Community Forum**: Connect with other users

## 8. Next Steps

### For Beginners
1. **Complete the Quick Start**: Run all three demo programs
2. **Explore Configuration Options**: Try different parameter combinations
3. **Study Learning Experiments**: Understand how plasticity works
4. **Experiment with Development**: See how behavior changes over time

### For Researchers
1. **Create Custom Experiments**: Design specific research scenarios
2. **Analyze Performance Data**: Measure system capabilities
3. **Compare Configurations**: Test different parameter sets
4. **Extend the Framework**: Add new neuron types or plasticity rules

### For Developers
1. **Study the Code**: Understand the architecture and implementation
2. **Run Tests**: Ensure your changes don't break existing functionality
3. **Add Documentation**: Improve comments and guides
4. **Contribute**: Share improvements with the community

## 9. Advanced Features (Optional)

### Python Bindings

For easier integration with Python:

```python
import nlm

# Quick Python example
config = nlm.Config()
config.set("neurons", 500)
brain = nlm.Brain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Final weight mean: {brain.getExcitationInhibitionRatio():.4f}")
```

### Visualization

Enable visualization system:
```bash
./nlm --enable-visualization --visualization-backend glfw
```

### Performance Monitoring

Monitor system performance:
```bash
./nlm --timing --performance-metrics --log-level Debug
```

## 10. Conclusion

You now have a complete NLM system running on your machine! This represents a functioning artificial neural substrate capable of:

- **Real spiking neural computation**
- **Experience-dependent synaptic changes**
- **Curiosity-driven exploration**
- **Developmental behavior progression**
- **Memory-based learning and planning**
- **Adaptive interaction with environments**

The NLM framework provides a foundation for investigating artificial developmental brains and understanding how complex cognition can emerge from neural dynamics, plasticity, and experience.

---

**Getting Started Complete!** 🎉

You have successfully installed and run NLM. Explore further with custom configurations and experiments to deepen your understanding of artificial developmental systems.

**Ready for more?** Check out the [User Guide](./user_guide.md) for detailed tutorials and the [API Reference](./api_reference/) for complete documentation.