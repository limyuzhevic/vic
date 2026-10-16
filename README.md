# NLM (熙然) - Neural Learning Machine
# Phase 2: Real Neural Computation

## Quick Start

This is NLM - a biologically-inspired artificial brain that learns from experience. Unlike ML frameworks, NLM implements its own spiking neural computation from the ground up.

### Basic Usage

```bash
# Build the system
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the Phase 2 demo
./nlm_phase2_demo

# Run tests
ctest --output-on-failure
```

### Python Bindings

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Create agent with world interaction
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

## Key Improvements in This Version

### 1. Enhanced Prediction System Integration
The prediction system now actively uses sensory observations and working memory to predict next states:

```cpp
// Update prediction system with current sensory input state
pImpl->predictionSystem->update(pImpl->timestep);

// Use predictions for attention and action selection
float predictionError = pImpl->predictionSystem->getPredictionError();
float attentionMod = 1.0f + predictionError * 2.0f;
pImpl->attention->setGlobalBias(attentionMod);
```

### 2. Real Neural Computation
**Neurons**: Leaky Integrate-and-Fire (LIF) with adaptive thresholds
**Dynamics**: Event-driven spike propagation with synaptic delays
**Plasticity**: STDP and Hebbian learning work together

### 3. Integrated Memory Systems
- **Working Memory**: Active maintenance of neural firing patterns
- **Episodic Memory**: Experience storage with replay mechanisms
- **Associative Memory**: Pattern-to-pattern relationships

### 4. Advanced Neuromodulation
- **Dopamine**: Reward prediction error and plasticity modulation
- **Novelty**: Prediction error-based novelty detection
- **Curiosity**: Exploration motivation
- **ACh, NE, 5-HT**: Full neurotransmitter integration

### 5. Cognitive Integration
- **Attention**: Competitive selection with top-down and bottom-up bias
- **Planning**: Multi-step action prediction and evaluation
- **Concept Formation**: Pattern discovery without predefined categories
- **Self-Model**: Sensorimotor self-awareness

## Command-Line Tools

### nlm-explore [directory]
Explore NLM codebase structure and find files by patterns

### nlm-learn [model-file] [--mode basic|advanced]
Train a neural model with detailed learning parameters

### nlm-validate [file] [--format json|csv|txt]
Validate a saved brain model and show its statistics

### nlm-comparison [file1] [file2] [--metric weights|spikes|learning]
Compare two brain models using advanced metrics

### nlm-debug [file] [--checkpoint step]
Load and debug a specific brain checkpoint

### nlm-test --all
Run comprehensive tests for all subsystems

### nlm-config --show
Display current configuration with all parameters

### nlm-visualize --help
View available visualization options

## Configuration Files

### configs/default.cfg
Default configuration for basic usage:

```ini
# Basic neural parameters
random_seed = 42
simulation_timestep = 0.001s
neuron_count = 1000
region_count = 1
connection_probability = 0.1f

# Plasticity parameters
stdp_ltp_weight = 0.01f
stdp_ltd_weight = 0.012f
stdp_tau = 20.0f
synaptogenesis_rate = 0.0001f
pruning_rate = 0.00001f

# Memory parameters
replay_interval = 100
consolidation_interval = 1000

# Development parameters
developmental_stage = 0  # 0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult
```

### configs/advanced.cfg
Advanced configuration for research:

```ini
# Enhanced neural parameters
random_seed = 12345
simulation_timestep = 0.0001s
neuron_count = 10000
region_count = 4
connection_probability = 0.15f

# Advanced plasticity
stdp_ltp_weight = 0.02f
stdp_ltd_weight = 0.015f
stdp_tau = 30.0f
synaptogenesis_rate = 0.00005f
pruning_rate = 0.000005f

# Memory systems
replay_interval = 50
consolidation_interval = 500
max_episodes = 5000
working_memory_capacity = 500

# Neuromodulation
novelty_sensitivity = 0.8f
curiosity_strength = 0.6f
prediction_error_gain = 2.0f

# Development
initial_plasticity = 1.0f
critical_period_end = 1000
maturation_end = 5000
```

## Advanced Features

### 1. Prediction-Based Attention
The system uses prediction error to modulate attention:
- High prediction error → increased attention to unexpected events
- Novelty detection amplifies attention for exploration
- Top-down goals can bias attention toward relevant features

### 2. Planning with Predictions
The neural planner uses learned predictions to:
- Evaluate action sequences
- Select actions leading to high-value outcomes
- Adapt plans based on prediction confidence

### 3. Sleep-Rest Cycles
During rest phases:
- Memory consolidation (episodic → semantic)
- Replay of important experiences
- Synaptic downscaling to prevent saturation

### 4. Developmental Stages
- **Initial**: High plasticity, rapid learning
- **Critical Period**: Sensitive windows for skill acquisition
- **Maturation**: Balance exploration vs. exploitation
- **Adult**: Stable performance with maintenance learning

## Visualization and Analysis

### nlm-visualize --neurons
Display neural population activity

### nlm-visualize --memory
Show working and episodic memory content

### nlm-visualize --plasticity
Display synaptic weight changes over time

### nlm-visualize --development
Track developmental stage progression

### nlm-stats --output json
Export brain statistics in JSON format for analysis

## Research Capabilities

### Multi-Agent Social Learning
```python
# Create multiple agents
brain1 = pynlm.createBrain(config)
brain2 = pynlm.createBrain(config)

# Enable social learning
brain1.getSocialLearning()->enableObservation(true)
brain2.getSocialLearning()->enableObservation(true)

# Run interaction
agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)
agent1.initialize(world)
agent2.initialize(world)

# They can imitate each other
for step in range(1000):
    agent1.step(step)
    agent2.step(step)
```

### Continual Learning
```python
# Prevent catastrophic forgetting
brain.getWorkingMemory()->enable rehearsal(true)
brain.getEpisodicMemory()->enable consolidation(true)

# Track performance over time
performance_history = []
for epoch in range(100):
    # Train on new task
    train_new_task()
    
    # Test on old tasks
    old_task_performance = test_old_tasks()
    performance_history.append(old_task_performance)
```

### Goal-Directed Behavior
```python
# Set goals
brain.getPlanner()->setCurrentGoal({0.5f, 0.5f, 0.0f, 1.0f})  # Coordinates in space

# The brain will plan actions to reach this goal
for step in range(1000):
    brain.step(step)
    action = brain.produceAction()
```

## Troubleshooting

### Memory Issues
```bash
# If memory usage is high
./nlm-debug model.bin --checkpoint 100
# Check working memory size
./nlm-stats model.bin --memory-usage
```

### Performance Issues
```bash
# Reduce model size for faster execution
./nlm-learn model.bin --mode basic
# Use optimized configuration
./nlm-config --set neuron_count=100
```

### Learning Issues
```bash
# Check if learning is happening
./nlm-compare initial.bin final.bin --metric learning
# Reset brain and try different parameters
./nlm-debug initial.bin --reset
```

## Future Enhancements

The NLM brain is designed for continual improvement:

1. **Enhanced Sensory Processing**: Multi-modal input integration
2. **Motor Control**: Complex action sequencing and learning
3. **Social Intelligence**: Theory of mind and communication
4. **Abstract Reasoning**: Concept-to-concept associations
5. **Meta-Learning**: Learning how to learn

## Summary

NLM (熙然) is a research-grade artificial brain that:

- ✅ Implements real neural computation from scratch
- ✅ Integrates multiple cognitive systems
- ✅ Learns from experience using biological mechanisms
- ✅ Adapts through development and plasticity
- ✅ Supports complex behaviors through prediction and planning

The system is ready for research, education, and development of advanced AI capabilities.
