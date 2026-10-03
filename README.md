# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

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

**PHASE 6: FINAL INTEGRATION** ✅ **COMPLETE**

Phase 6 successfully integrates all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ All memory systems connected to neural processing

#### Neuromodulation Integration
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ All neuromodulators connected to plasticity rules

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ Confidence tracking implemented

#### Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ Attention with competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### Development Integration
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ Neural excitability changes with development

#### Persistence
- ✅ Checkpoint save/load implemented
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Integration with sleep/rest cycle

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
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Advanced Integration Examples

#### Complete Lifecycle Simulation
```bash
# Run 500,000 steps with development and learning
./nlm_phase6_demo --episodes 100 --steps-per-episode 5000
```

#### Memory Replay Simulation
```bash
# Run with replay enabled for memory consolidation
./nlm_phase6_demo --enable-replay --replay-iterations 3
```

#### Damage Recovery Experiment
```bash
# Test brain's ability to recover from damage
./nlm_phase6_demo --enable-checkpointing --test-damage-recovery
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
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

### Phase 1 (Complete)
- ✅ Project skeleton
- ✅ Core types and configuration
- ✅ Neural interfaces

### Phase 2 (Complete)
- ✅ Real LIF neuron dynamics
- ✅ Event-driven spike propagation
- ✅ STDP and Hebbian plasticity
- ✅ Structural plasticity

### Phase 3 (Complete)
- ✅ World interaction loop
- ✅ Sensory input and motor output
- ✅ Reward prediction error
- ✅ Developmental stages
- ✅ Novelty and curiosity

### Phase 4 (Complete)
- ✅ Neural prediction system
- ✅ Working memory
- ✅ Episodic-like memory
- ✅ Concept formation
- ✅ Neural attention (NOT Transformer)
- ✅ Predictive planning
- ✅ Self-model
- ✅ Social learning
- ✅ Continual learning

### Phase 5 (Complete)
- ✅ Performance optimizations
- ✅ Memory pools and event queues
- ✅ SIMD vectorization
- ✅ Parallel processing
- ✅ Checkpoint system

### Phase 6 (Complete - Final Integration)
- ✅ All systems integrated into coherent brain loop
- ✅ Memory systems connected to neural processing
- ✅ Neuromodulation affects plasticity and dynamics
- ✅ Prediction integrated with learning
- ✅ Development affects plasticity rates
- ✅ Checkpoint save/load working
- ✅ Replay and consolidation functional
- ✅ Phase 6 integration experiment created

## Advanced Features Available in Phase 6

### 1. Multi-Level Memory Systems
- **Working Memory**: Maintains active information across simulation steps
- **Episodic Memory**: Stores experiences with temporal/spatial indexing
- **Associative Memory**: Forms pattern-based associations
- **Integration**: All systems connected to neural processing for emergent behavior

### 2. Neuromodulation Framework
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation based on novelty detection
- **Novelty**: Surprises in environment trigger exploration
- **Prediction Error**: Timing-based learning signals
- **Integration**: Neuromodulators affect plasticity across all brain regions

### 3. Predictive Processing
- **Temporal Prediction**: Learns sequences of events
- **Action Consequence Prediction**: Anticipates outcomes of actions
- **Confidence Tracking**: Estimates reliability of predictions
- **Integration**: Prediction guides attention and action selection

### 4. Cognitive Architecture
- **Attentional Selection**: Competitive neural processing for feature selection
- **Concept Formation**: Discovers patterns without explicit training
- **Neural Planning**: Evaluates multiple action sequences using predictions
- **Self-Model**: Learns body schema and sensorimotor relationships
- **Social Learning**: Observes and imitates other agents

### 5. Developmental Biology
- **Developmental Stages**: Initial → Critical Period → Maturation → Adult → Aging
- **Structural Plasticity**: Synaptogenesis (forming new connections) and pruning (eliminating weak connections)
- **Maturation**: Adult property development and specialization
- **Plasticity Modulation**: Learning rates change with developmental stage

### 6. Advanced Persistence
- **Checkpointing**: Save/load brain state at any simulation point
- **Replay**: Re-experience important episodes for consolidation
- **Memory Integration**: Learning connects to memory systems for long-term adaptation

### 7. Power-User Features

#### 1. Custom Brain Configurations
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Larger brain
config.set("brain.synapse_density", 0.15)  # More connections
config.set("development.enable", True)  # Enable development
config.set("neuromod.curiosity.scale", 2.0)  # More exploration
```

#### 2. Multi-Agent Simulation
```python
import pynlm

# Create multiple brains with shared world
config = pynlm.createDefaultConfig()
config.set("environment.width", 100)
config.set("environment.height", 100)

brain1 = pynlm.createBrain(config)
brain2 = pynlm.createBrain(config)  # Same config, different internal state

world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)

agent1.initialize(world)
agent2.initialize(world)

# Enable social learning and interaction
agent1.enableSocialLearning(True)
agent2.enableSocialLearning(True)
agent1.enableCuriosity(True)
agent2.enableCuriosity(True)
```

#### 3. Advanced Experiment Control
```bash
./nlm_phase6_demo \
  --neuron-count 5000 \
  --enable-replay \
  --replay-iterations 5 \
  --enable-development \
  --enable-curiosity \
  --enable-structural-plasticity \
  --checkpoint-interval 10000 \
  --max-steps 1000000
```

#### 4. Visualization and Analysis
```python
import pynlm

def analyze_brain_behavior(brain, steps=1000):
    """Advanced analysis of brain behavior and learning patterns"""
    
    # Collect metrics during simulation
    firing_rates = []
    curiosity_levels = []
    prediction_errors = []
    memory_usage = []
    
    for step in range(steps):
        brain.step(step)
        
        # Advanced metrics collection
        firing_rates.append(brain.getAverageFiringRate())
        curiosity_levels.append(brain.getCuriosityLevel())
        prediction_errors.append(brain.getPredictionError())
        memory_usage.append(brain.getEpisodicMemory().getEpisodeCount())
    
    # Statistical analysis
    return {
        "avg_firing_rate": sum(firing_rates) / len(firing_rates),
        "curiosity_trend": calculate_trend(curiosity_levels),
        "prediction_accuracy": 1.0 - sum(prediction_errors) / len(prediction_errors),
        "memory_growth": memory_usage[-1] - memory_usage[0],
        "learning_efficiency": sum(firing_rates) / steps
    }
```

#### 5. Custom Neural Topologies
```python
import pynlm

def create_custom_brain(region_config):
    """Create brain with custom regional architecture"""
    config = pynlm.createDefaultConfig()
    
    # Define different regions for different functions
    region_config = {
        "sensory": {"neuron_count": 500, "synapse_density": 0.2},
        "motor": {"neuron_count": 300, "synapse_density": 0.15},
        "cortex": {"neuron_count": 2000, "synapse_density": 0.1},
        "hippocampus": {"neuron_count": 800, "synapse_density": 0.12},
        "amygdala": {"neuron_count": 400, "synapse_density": 0.18}
    }
    
    # Apply region-specific settings
    config.set("brain.region_count", len(region_config))
    config.set("brain.synapse_density", 0.1)  # Default
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Add regions with specific characteristics
    for region_name, region_spec in region_config.items():
        region_id = brain.addRegion(region_name)
        # Could set specific parameters per region
    
    return brain
```

## Advanced Commands for Power Users

### Configuration File Generation

```bash
# Generate optimized configuration for specific research tasks
./nlm generate-config --task reinforcement-learning --neurons 3000 --stability high

# Export configuration to JSON for documentation
./nlm export-config --format json > nlm_config.json
```

### Simulation Management

```bash
# Run multiple simulations with different random seeds
./nlm_phase6_demo --multi-seed 10 --episodes 50

# Run with detailed logging for debugging
./nlm_phase6_demo --log-level debug --checkpoint-interval 5000
```

### Performance Monitoring

```bash
# Monitor simulation performance in real-time
./nlm --performance-monitor --metrics-file performance.json

# Profile simulation for optimization
./nlm --profile --output profile_report.txt
```

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

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

## License

MIT

## Authors

Research project — See docs for scientific background.
