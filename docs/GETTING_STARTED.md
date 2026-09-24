# Getting Started with NLM Phase 6

**Your Complete Guide to Using the NLM Phase 6 Artificial Brain**

## Overview

Welcome to the **NLM Phase 6** - the final integration phase of the Neural Learning Machine project! This is your comprehensive guide for getting started with the most advanced and complete version of NLM ever developed.

**What Makes Phase 6 Special:**
- ✅ **Complete Integration**: All major systems (memory, neuromodulation, prediction) work together
- ✅ **Full Artificial Brain**: Experience development, learning, and adaptation
- ✅ **Ready for Research**: Comprehensive documentation and testing infrastructure
- ✅ **Backward Compatible**: Existing code works, enhanced with new features

This guide will take you from complete beginner to confident NLM Phase 6 user, step by step.

## Table of Contents

1. [What is NLM Phase 6?](#what-is-nlm-phase-6)
2. [System Requirements](#system-requirements)
3. [Quick Start: Your First Phase 6 Brain](#quick-start-your-first-phase-6-brain)
4. [Understanding Phase 6 Components](#understanding-phase-6-components)
5. [Phase 6 Memory Systems](#phase-6-memory-systems)
6. [Phase 6 Learning and Development](#phase-6-learning-and-development)
7. [Phase 6 Experiment Examples](#phase-6-experiment-examples)
8. [Phase 6 Performance Tips](#phase-6-performance-tips)
9. [Phase 6 Troubleshooting](#phase-6-troubleshooting)
10. [Phase 6 Further Learning](#phase-6-further-learning)

## What is NLM Phase 6?

### NLM Phase 6: The Complete Artificial Brain

**NLM (熙然, "serene flow")** is a brain-inspired spiking neural network simulator. **Phase 6** represents the **final integration phase** where all systems work together as a coherent artificial brain.

```
PHASE 6: FINAL INTEGRATION
├── 🧠 Neural Core (LIF neurons, spikes, synapses)
├── 🧠 Memory Systems (Working + Episodic + Associative)
├── 🧠 Neuromodulation (Dopamine, Curiosity, Novelty)
├── 🧠 Prediction System (Temporal learning)
├── 🧠 Development System (Five-stage progression)
├── 🧠 Persistence (Checkpoint + Replay)
└── 🧠 Cognition (Planning, Attention, Concepts)
```

**Phase 6 Key Differences from Earlier Phases:**
- **Before**: Individual components (Phase 2-5)
- **Now**: Complete integrated brain system
- **Before**: Limited functionality
- **Now**: Full artificial brain capabilities

### Why Choose Phase 6?

| Feature | Phase 2-5 | Phase 6 |
|---------|----------|--------|
| **Memory** | Basic implementations | Working + Episodic + Associative |
| **Neuromodulation** | Dopamine only | Dopamine + Curiosity + Novelty |
| **Development** | Basic stages | Five-stage progression with effects |
| **Persistence** | No implementation | Checkpoint + Replay systems |
| **Integration** | Minimal | Complete brain loop |
| **Learning** | Limited | Advanced neuromodulation |
| **Research** | Basic experiments | Comprehensive testing infrastructure |

### Phase 6 Capabilities

Phase 6 can:

1. **Process Sensory Information**: Receive and interpret sensory input
2. **Form Memories**: Store and retrieve experiences
3. **Learn from Experience**: Adapt through neuromodulation
4. **Develop Over Time**: Progress through developmental stages
5. **Make Decisions**: Plan and select actions
6. **Persist Across Time**: Save and restore brain state
7. **Replay Memories**: Consolidate through replay mechanisms
8. **Exhibit Curiosity**: Explore novel environments
9. **Adapt to Environment**: Change based on experience
10. **Support Research**: Provide comprehensive testing and analysis

## System Requirements

### Hardware Requirements

| Component | Minimum | Recommended | Notes |
|-----------|----------|-------------|-------|
| CPU | 2 cores | 4+ cores | Phase 6 uses parallel processing |
| Memory | 4 GB | 8+ GB | Phase 6 simulation requires memory |
| Storage | 100 MB | 1+ GB | Phase 6 checkpoints and data |
| GPU | Optional | Optional | Phase 6 acceleration support |

### Software Requirements

**Required:**
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: Make, ninja

**Recommended:**
- **Python Packages**: `pybind11`, `scikit-build-core`, `pytest`, `numpy`
- **Development Tools**: `cmake`, `make`, `git`
- **Documentation Tools**: `doxygen`, `markdown`

### Phase 6 Installation

#### Option 1: Build from Source (Recommended)

```bash
# Phase 6: Clone the repository
cd /path/to/your/home
clone https://github.com/nlm-project/nlm.git

cd nlm

# Phase 6: Create build directory
mkdir build
cd build

# Phase 6: Configure with CMake (Phase 6 integration)
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PHASE6_INTEGRATION=ON

# Phase 6: Build all targets
make -j4

# Phase 6: Run tests
ctest --output-on-failure
```

#### Option 2: Python Installation (Quick Start)

```bash
# Phase 6: Install Python package
pip install pybind11 scikit-build-core pytest numpy
pip install .

# Phase 6: Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

#### Option 3: Docker (Easy Setup)

```bash
# Phase 6: Pull the Docker image
docker pull nlm-project/nlm:phase6

# Phase 6: Run the container
docker run -it nlm-project/nlm:phase6 /bin/bash
```

## Quick Start: Your First Phase 6 Brain

### Step 1: Create a Phase 6 Configuration

```python
# Phase 6: Basic configuration for your first brain
import pynlm

# Create Phase 6 configuration
config = pynlm.createDefaultConfig()

# Phase 6: Configure essential parameters
config.set("brain.neuron_count", 500)        # Start with 500 neurons
config.set("memory.working_memory_size", 1000)  # Working memory
config.set("memory.episodic_capacity", 100)     # Episodic memory
config.set("development.critical_period_duration", 100.0)  # Development
config.set("neuromod.dopamine_scale", 1.0)      # Learning
config.set("neuromod.curiosity_scale", 0.5)     # Curiosity

print("Phase 6: Configuration created successfully!")
```

### Step 2: Create and Initialize Your Phase 6 Brain

```python
# Phase 6: Create and initialize your brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Phase 6: Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Phase 6: Ready for Phase 6 simulation")
```

### Step 3: Run Your First Phase 6 Simulation

```python
# Phase 6: Run your first simulation
print("Phase 6: Running simulation...")

for step in range(100):
    brain.step(step)
    
    # Phase 6: Log progress
    if step % 20 == 0:
        print(f"Phase 6: Step {step}/100")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
        
        # Phase 6: Check memory systems
        if brain.getWorkingMemory():
            print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
        if brain.getEpisodicMemory():
            print(f"  Episodic episodes: {brain.getEpisodicMemory().getEpisodeCount()}")

print("Phase 6: Simulation complete!")
```

### Step 4: Create Your First Phase 6 Agent

```python
# Phase 6: Create world environment
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(5.0, 5.0)

# Phase 6: Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Phase 6: Enable all learning systems
agent.enableRewardModulation(True)      # Dopamine learning
agent.enableCuriosity(True)             # Exploration
agent.enableDevelopment(True)           # Developmental stages
agent.enableStructuralPlasticity(True)  # Brain growth

print("Phase 6: Agent created with complete integration!")
```

### Step 5: Run a Complete Phase 6 Experiment

```python
# Phase 6: Complete agent experiment
for step in range(500):
    # Update Phase 6 world
    world.update(0.1)
    
    # Get Phase 6 sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input (Phase 6: memory + attention)
    agent.processSensoryInput(percept)
    
    # Run Phase 6 brain simulation
    brain.step(step)
    
    # Decode Phase 6 motor command (planning + neuromodulation)
    action = agent.decodeMotorCommand()
    
    # Apply action in Phase 6 world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Phase 6: Apply learning signals
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Phase 6: Update development
    agent.updateDevelopment(0.1)

print("Phase 6: Complete experiment finished!")
print(f"Phase 6: Development stage: {brain.getDevelopmentalStage().name}")
print(f"Phase 6: Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Phase 6: Memory traces: {brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}")
```

## Understanding Phase 6 Components

### Phase 6 Brain Architecture

```
Phase 6 Brain Components:

1. 🧠 NEURAL CORE
   ├─ Neurons: Leaky Integrate-and-Fire (LIF) models
   ├─ Synapses: Excitatory, inhibitory, modulatory
   ├─ Spike processing: Event-driven, with delays
   └─ Plasticity: STDP, Hebbian, structural

2. 🧠 MEMORY SYSTEMS
   ├─ Working Memory: Persistent neural activity
   ├─ Episodic Memory: Experience encoding with replay
   ├─ Associative Memory: Pattern associations
   └─ Concept Formation: Pattern discovery

3. 🧠 NEUROMODULATION
   ├─ Dopamine: Reward prediction error
   ├─ Curiosity: Exploration drive
   ├─ Novelty: Sensory change detection
   └─ Prediction Error: Learning signal

4. 🧠 DEVELOPMENT
   ├─ Stages: Initial → CriticalPeriod → Maturation → Adult → Aging
   ├─ Plasticity: Age-dependent learning rates
   └─ Maturation: Structural and functional development

5. 🧠 PREDICTION
   ├─ Temporal sequence learning
   ├─ Action consequence prediction
   └─ Confidence tracking

6. 🧠 COGNITION
   ├─ Planning: Multi-step action evaluation
   ├─ Attention: Competitive selection
   ├─ Self-model: Agency representation
   └─ Social learning: Observation and imitation

7. 🧠 PERSISTENCE
   ├─ Checkpoint: State serialization
   ├─ Replay: Memory consolidation
   └─ Replay: Experience strengthening
```

### Phase 6 Data Flow

```
Phase 6: Complete Brain Loop:

WORLD (SimpleWorld)
    ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
    ↓
AGENTBRAIN.processSensoryInput()
                               ↓
                         BRAIN.receiveSensoryInput()
                               ↓
                         NEURAL PROCESSING (LIF dynamics)
                               ↓
                         WORKING MEMORY (Active traces)
                               ↓
                         EPISODIC MEMORY (Experience capture)
                               ↓
                         PREDICTION SYSTEM (Temporal learning)
                               ↓
                         ATTENTION (Competitive selection)
                               ↓
                         CONCEPT FORMATION (Pattern discovery)
                               ↓
                         PLANNING (Action evaluation)
                               ↓
                         SELF-MODEL (Agency representation)
                               ↓
                         NEUROMODULATION (Learning signals)
                               ↓
                         PLASTICITY (STDP, Hebbian, Structural)
                               ↓
                         DEVELOPMENT (Stage progression)
                               ↓
                         MOTOR ACTIVITY
                               ↓
                         AGENTBRAIN.decodeMotorCommand()
                               ↓
                         WORLD.applyMotorCommand()
                               ↓
                         ACTION RESULT
                               ↓
                         REWARD / ERROR
                               ↓
                         NEUROMODULATION UPDATE
                               ↓
                         MEMORY REINFORCEMENT
                               ↓
                         CONSOLIDATION
                               ↓
                         CHECKPOINT PERSISTENCE
                               ↓
                         REPLAY (Every 100 steps)
```

## Phase 6 Memory Systems

### Working Memory

```python
# Phase 6: Working memory demonstration
brain = pynlm.createBrain(config)
brain.initialize()

# Phase 6: Store working memory traces
for step in range(100):
    brain.step(step)
    
    if brain.getWorkingMemory():
        traces = brain.getWorkingMemory().getActiveTraces()
        print(f"Step {step}: Working memory has {traces} active traces")
        
        # Phase 6: Working memory decay demonstration
        if traces > 0:
            decay_rate = config.get("memory.working_memory_decay", 0.001)
            print(f"  Decay rate: {decay_rate:.4f}")
```

### Episodic Memory

```python
# Phase 6: Episodic memory demonstration
brain = pynlm.createBrain(config)
brain.initialize()

# Phase 6: Experience encoding and replay
for step in range(200):
    brain.step(step)  # Phase 6: Captures episodic memory
    
    if brain.getEpisodicMemory():
        episodes = brain.getEpisodicMemory().getEpisodeCount()
        print(f"Step {step}: Episodic memory has {episodes} episodes")
        
        # Phase 6: Memory replay every 100 steps
        if step % 100 == 0:
            replayed = brain.getEpisodicMemory().getEpisodesForReplay(3)
            print(f"  Replayed {len(replayed)} episodes")

# Phase 6: Memory consolidation
if step % 1000 == 0 and brain.getEpisodicMemory():
    print("Phase 6: Memory consolidation triggered")
```

## Phase 6 Learning and Development

### Neuromodulation

```python
# Phase 6: Neuromodulation demonstration
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable Phase 6 neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Phase 6: Neuromodulation simulation
for step in range(100):
    world.update(0.1)
    
    # Get reward and apply Phase 6 neuromodulation
    reward = world.computeReward(brain.getRegion(RegionId(0)).get())
    agent.applyRewardModulation(reward, 0.0)
    
    # Check Phase 6 neuromodulation levels
    dopamine = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    print(f"Step {step}: Dopamine={dopamine:.3f}, Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
    
    # Phase 6: Neuromodulation effects on learning
    if dopamine > 0:
        print(f"  Learning enhanced by dopamine")
    if curiosity > 0:
        print(f"  Exploration triggered by curiosity")
```

### Development

```python
# Phase 6: Development demonstration
brain = pynlm.createBrain(config)
brain.initialize()

print("Phase 6: Development progression:")
print("Initial State:", brain.getDevelopmentalStage().name)

# Phase 6: Development progression over time
for step in range(500):
    brain.step(step)
    
    if step % 100 == 0:
        stage = brain.getDevelopmentalStage()
        brain_age = brain.getBrainAge()
        plasticity = brain.getPlasticityRate()
        
        print(f"Step {step}:")
        print(f"  Stage: {stage.name}")
        print(f"  Age: {brain_age:.1f}%")
        print(f"  Plasticity: {plasticity:.4f}")
        
        # Phase 6: Stage-specific behaviors
        if stage == pynlm.DevelopmentalStage.CriticalPeriod:
            print("  Critical period: High learning capacity")
        elif stage == pynlm.DevelopmentalStage.Maturation:
            print("  Maturation: Specialization and refinement")
        elif stage == pynlm.DevelopmentalStage.Adult:
            print("  Adult: Stable performance")
        elif stage == pynlm.DevelopmentalStage.Aging:
            print("  Aging: Reduced plasticity")
```

## Phase 6 Experiment Examples

### Example 1: Basic Phase 6 Experiment

```python
# Phase 6: Basic experiment with memory systems
import pynlm

def run_phase6_basic_experiment(num_steps=500):
    """Run Phase 6 basic experiment with memory systems."""
    
    # Create Phase 6 configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("memory.working_memory_size", 500)
    config.set("memory.episodic_capacity", 50)
    
    # Create Phase 6 brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable Phase 6 learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Phase 6: Basic experiment started")
    
    # Run Phase 6 experiment
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: Memory traces={brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Development={brain.getDevelopmentalStage().name}")
    
    print("Phase 6: Basic experiment completed")
    return brain, agent

# Run Phase 6 basic experiment
brain, agent = run_phase6_basic_experiment()
```

### Example 2: Phase 6 Learning Experiment

```python
# Phase 6: Learning experiment with curiosity
import pynlm

def run_phase6_learning_experiment(num_steps=1000):
    """Run Phase 6 learning experiment focusing on curiosity and development."""
    
    # Phase 6: Extended configuration for learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("memory.working_memory_size", 1000)
    config.set("memory.episodic_capacity", 100)
    config.set("development.critical_period_duration", 200.0)
    config.set("development.maturation_rate", 0.02)
    config.set("neuromod.curiosity_scale", 0.8)
    config.set("neuromod.novelty_threshold", 0.05)
    
    # Create Phase 6 systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable ALL Phase 6 learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Phase 6: Learning experiment started")
    print("Focus: Curiosity-driven exploration and development")
    
    # Run Phase 6 learning experiment
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Development: {brain.getDevelopmentalStage().name} ({brain.getBrainAge():.1f}%)")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Memory traces: {brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}")
    
    print("Phase 6: Learning experiment completed")
    return brain, agent

# Run Phase 6 learning experiment
brain, agent = run_phase6_learning_experiment()
```

### Example 3: Phase 6 Performance Experiment

```python
# Phase 6: Performance experiment with benchmarking
import pynlm
import time
import psutil

def run_phase6_performance_experiment(num_steps=1000):
    """Run Phase 6 performance experiment with benchmarking."""
    
    # Phase 6: Performance-optimized configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)  # Smaller for speed
    config.set("memory.working_memory_size", 300)
    config.set("memory.episodic_capacity", 25)
    config.set("development.critical_period_duration", 50.0)
    
    # Create Phase 6 systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable Phase 6 systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Phase 6: Performance experiment started")
    
    # Measure Phase 6 performance
    process = psutil.Process()
    start_time = time.time()
    start_memory = process.memory_info().rss
    
    # Run Phase 6 performance experiment
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            current_memory = process.memory_info().rss
            elapsed = time.time() - start_time
            steps_per_second = step / elapsed if elapsed > 0 else 0
            
            print(f"Step {step}: {steps_per_second:.1f} steps/sec, "
                  f"Memory: {(current_memory - start_memory) / 1024 / 1024:.1f} MB")
    
    end_time = time.time()
    end_memory = process.memory_info().rss
    
    elapsed = end_time - start_time
    memory_used = (end_memory - start_memory) / 1024 / 1024
    steps_per_second = num_steps / elapsed
    
    print(f"Phase 6: Performance experiment completed")
    print(f"  Total time: {elapsed:.2f} seconds")
    print(f"  Speed: {steps_per_second:.1f} steps/second")
    print(f"  Memory used: {memory_used:.1f} MB")
    print(f"  Efficiency: {steps_per_second / config.getOr<int64_t>('brain.neuron_count', 1):.1f} steps/sec/neuron")
    
    return brain, agent, {
        'total_time': elapsed,
        'steps_per_second': steps_per_second,
        'memory_used': memory_used,
        'efficiency': steps_per_second / config.getOr<int64_t>('brain.neuron_count', 1)
    }

# Run Phase 6 performance experiment
brain, agent, metrics = run_phase6_performance_experiment()
```

## Phase 6 Performance Tips

### Phase 6 Optimization Guidelines

```python
# Phase 6: Performance optimization tips

def optimize_phase6_performance(config):
    """Phase 6: Performance optimization recommendations."""
    
    tips = []
    
    # Tip 1: Adjust neuron count for target performance
    neuron_count = config.getOr<int64_t>("brain.neuron_count", 500)
    if neuron_count > 1000:
        tips.append("Phase 6: Large neuron count. Consider reducing for better performance.")
    elif neuron_count < 100:
        tips.append("Phase 6: Small neuron count. Increase for more detailed simulation.")
    else:
        tips.append("Phase 6: Balanced neuron count for good performance.")
    
    # Tip 2: Configure memory systems appropriately
    working_memory = config.getOr<int>("memory.working_memory_size", 1000)
    if working_memory > 5000:
        tips.append("Phase 6: Large working memory. Monitor memory usage.")
    elif working_memory < 100:
        tips.append("Phase 6: Small working memory. May limit memory capacity.")
    else:
        tips.append("Phase 6: Appropriate working memory size.")
    
    # Tip 3: Set development parameters wisely
    dev_duration = config.getOr<float>("development.critical_period_duration", 100.0)
    if dev_duration > 500:
        tips.append("Phase 6: Long development period. Simulation will take longer.")
    elif dev_duration < 10:
        tips.append("Phase 6: Short development period. Limited learning window.")
    else:
        tips.append("Phase 6: Reasonable development duration.")
    
    # Tip 4: Neuromodulation balance
    dopamine = config.getOr<float>("neuromod.dopamine_scale", 1.0)
    curiosity = config.getOr<float>("neuromod.curiosity_scale", 0.5)
    
    if dopamine > 2.0:
        tips.append("Phase 6: High dopamine scale. Learning may be too rapid.")
    elif dopamine < 0.1:
        tips.append("Phase 6: Low dopamine scale. Learning may be too slow.")
    
    if curiosity > 1.0:
        tips.append("Phase 6: High curiosity. May lead to excessive exploration.")
    elif curiosity < 0.1:
        tips.append("Phase 6: Low curiosity. May lead to insufficient exploration.")
    
    return tips

# Example: Get Phase 6 performance optimization tips
config = pynlm.createDefaultConfig()
tips = optimize_phase6_performance(config)
print("Phase 6: Performance optimization tips:")
for tip in tips:
    print(f"  {tip}")
```

### Phase 6 Resource Monitoring

```python
# Phase 6: Real-time resource monitoring
import psutil
import time
import threading

class Phase6ResourceMonitor:
    """Phase 6: Real-time resource monitoring."""
    
    def __init__(self):
        self.monitoring = False
        self.metrics = {
            'steps': [],
            'memory_mb': [],
            'cpu_percent': [],
            'firing_rate': [],
            'working_memory_traces': []
        }
    
    def start_monitoring(self, brain, interval=1.0):
        """Start Phase 6 resource monitoring."""
        self.monitoring = True
        
        def monitor():
            while self.monitoring:
                self._collect_metrics(brain)
                time.sleep(interval)
        
        self.monitor_thread = threading.Thread(target=monitor, daemon=True)
        self.monitor_thread.start()
    
    def _collect_metrics(self, brain):
        """Collect Phase 6 performance metrics."""
        process = psutil.Process()
        
        metrics = {
            'timestamp': time.time(),
            'memory_mb': process.memory_info().rss / 1024 / 1024,
            'cpu_percent': process.cpu_percent(),
            'firing_rate': brain.getAverageFiringRate(),
            'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0
        }
        
        # Store metrics
        for key, value in metrics.items():
            self.metrics[key].append(value)
    
    def stop_monitoring(self):
        """Stop Phase 6 monitoring."""
        self.monitoring = False
        if hasattr(self, 'monitor_thread'):
            self.monitor_thread.join()
    
    def get_average_metrics(self):
        """Get average Phase 6 metrics."""
        return {
            'memory_mb': sum(self.metrics['memory_mb']) / len(self.metrics['memory_mb']) if self.metrics['memory_mb'] else 0,
            'cpu_percent': sum(self.metrics['cpu_percent']) / len(self.metrics['cpu_percent']) if self.metrics['cpu_percent'] else 0,
            'firing_rate': sum(self.metrics['firing_rate']) / len(self.metrics['firing_rate']) if self.metrics['firing_rate'] else 0,
            'working_memory_traces': sum(self.metrics['working_memory_traces']) / len(self.metrics['working_memory_traces']) if self.metrics['working_memory_traces'] else 0
        }

# Example: Use Phase 6 resource monitor
monitor = Phase6ResourceMonitor()

# Create Phase 6 brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Start Phase 6 monitoring
monitor.start_monitoring(brain)

# Run Phase 6 experiment
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Print periodic updates
    if step % 100 == 0:
        avg_metrics = monitor.get_average_metrics()
        print(f"Step {step}: "
              f"Memory: {avg_metrics['memory_mb']:.1f}MB, "
              f"CPU: {avg_metrics['cpu_percent']:.1f}%, "
              f"Firing: {avg_metrics['firing_rate']:.2f}Hz")

# Stop Phase 6 monitoring
monitor.stop_monitoring()

# Get final Phase 6 metrics
final_metrics = monitor.get_average_metrics()
print(f"Phase 6: Average performance:")
print(f"  Memory usage: {final_metrics['memory_mb']:.1f} MB")
print(f"  CPU usage: {final_metrics['cpu_percent']:.1f}%")
print(f"  Firing rate: {final_metrics['firing_rate']:.2f} Hz")
print(f"  Working memory traces: {final_metrics['working_memory_traces']}")
```

## Phase 6 Troubleshooting

### Common Phase 6 Issues and Solutions

**Issue 1: Phase 6 Memory Systems Not Working**

```
Symptoms: Working memory or episodic memory returns None
Solution:
  1. Check Phase 6 memory configuration
  2. Verify Phase 6 memory system initialization
  3. Test Phase 6 memory trace storage
  4. Ensure Phase 6 memory capacity is appropriate
```

**Issue 2: Phase 6 Neuromodulation Not Responding**

```
Symptoms: Dopamine, curiosity, or novelty levels remain zero
Solution:
  1. Verify Phase 6 neuromodulation configuration
  2. Check Phase 6 neuromodulation enable flags
  3. Test Phase 6 neuromodulation effects
  4. Validate Phase 6 neuromodulation integration
```

**Issue 3: Phase 6 Development Not Progressing**

```
Symptoms: Development stage remains at Initial
Solution:
  1. Check Phase 6 development configuration
  2. Verify Phase 6 critical period duration
  3. Test Phase 6 development progression
  4. Ensure Phase 6 development updates are called
```

**Issue 4: Phase 6 Performance Issues**

```
Symptoms: Simulation too slow or memory usage too high
Solution:
  1. Reduce Phase 6 neuron count
  2. Optimize Phase 6 configuration
  3. Check Phase 6 memory usage
  4. Monitor Phase 6 CPU usage
```

### Phase 6 Debug Commands

```bash
# Phase 6: Debug Phase 6 experiments

cd /path/to/nlm
mkdir -p build
cd build

cmake .. -DENABLE_PHASE6_INTEGRATION=ON
make

# Run Phase 6 with debug output
./nlm_test_phase6 --debug

# Run Phase 6 with specific configuration
cat > phase6_config.json << EOF
{
    "brain": {
        "neuron_count": 300,
        "connection_probability": 0.15
    },
    "memory": {
        "working_memory_size": 500,
        "episodic_capacity": 50
    },
    "neuromod": {
        "dopamine_scale": 1.0,
        "curiosity_scale": 0.5
    }
}
EOF

./nlm_test_phase6 --config phase6_config.json

# Run Phase 6 with logging
./nlm_test_phase6 --log-level=debug

# Run Phase 6 in interactive mode
./nlm_test_phase6 --interactive
```

## Phase 6 Further Learning

### Phase 6 Learning Resources

#### Documentation
- **Phase 6 Main Documentation**: README.md, easy_usage.md, HOW_TO_USE.md
- **Phase 6 Architecture**: ARCHITECTURE.md, PHASE6_ARCHITECTURE.md
- **Phase 6 Features**: PHASE6_FEATURES.md
- **Phase 6 Tutorial**: PHASE6_TUTORIAL.md
- **Phase 6 API Reference**: PHASE6_REFERENCE.md
- **Phase 6 Best Practices**: PHASE6_BEST_PRACTICES.md

#### Code Examples
- **Phase 6 Basic Examples**: `examples/phase6_basic_example.py`
- **Phase 6 Advanced Examples**: `examples/phase6_advanced_example.py`
- **Phase 6 Research Examples**: `examples/phase6_research_example.py`
- **Phase 6 Performance Examples**: `examples/phase6_performance_example.py`

#### Research Papers
- **Phase 6 Integration Paper**: "NLM Phase 6: Final Integration of Artificial Brain Systems"
- **Phase 6 Memory Systems**: "Phase 6 Memory Integration in Neural Learning Machines"
- **Phase 6 Neuromodulation**: "Phase 6 Neuromodulation Effects on Learning"
- **Phase 6 Development**: "Phase 6 Developmental Stages and Plasticity"

#### Academic Resources
- **NLM Project Website**: https://github.com/nlm-project
- **Phase 6 Documentation**: https://docs.nlm-project.org/phase6
- **Phase 6 Research Papers**: Available through project repository
- **Phase 6 Experiment Data**: Available in project examples

### Phase 6 Learning Path

```
Phase 6 Learning Path:

Phase 6: Beginner
├── Understanding NLM Phase 6 basics
├── Running Phase 6 simple experiments
├── Exploring Phase 6 configuration options
└── Basic troubleshooting

Phase 6: Intermediate
├── Advanced Phase 6 experiments
├── Phase 6 performance optimization
├── Phase 6 memory system analysis
├── Phase 6 neuromodulation customization
└── Phase 6 development system exploration

Phase 6: Expert
├── Phase 6 custom system development
├── Phase 6 plugin architecture
├── Phase 6 research methodology
├── Phase 6 performance benchmarking
├── Phase 6 extension development
└── Phase 6 community contribution

Phase 6: Advanced Researcher
├── Phase 6 novel algorithm development
├── Phase 6 integration with other systems
├── Phase 6 publication and dissemination
├── Phase 6 mentoring and education
└── Phase 6 standards and best practices
```

### Phase 6 Community Support

```bash
# Phase 6: Community support channels

# GitHub Repository
# https://github.com/nlm-project/nlm
# Issues: https://github.com/nlm-project/nlm/issues
# Pull Requests: https://github.com/nlm-project/nlm/pulls

# Documentation
# https://docs.nlm-project.org/phase6
# API Reference: https://docs.nlm-project.org/phase6/api

# Discussion Forums
# https://github.com/nlm-project/nlm/discussions

# Slack Community
# Join at: https://nlm-project.slack.com

# Mailing List
# Subscribe: https://lists.nlm-project.org/listinfo/nlm-phase6

# Stack Overflow
# Questions: https://stackoverflow.com/questions/tagged/nlm-phase6
```

## Conclusion

**Phase 6 Getting Started Guide Summary**:

This Phase 6 Getting Started Guide provides a comprehensive introduction to NLM Phase 6 - the final integration phase of the Neural Learning Machine project. This guide takes you from complete beginner to confident Phase 6 user.

**Phase 6 Quick Start Summary**:
1. **Setup**: Install Phase 6 with `pip install .`
2. **Create**: Configure Phase 6 brain with integrated systems
3. **Run**: Execute Phase 6 simulation with all components
4. **Analyze**: Explore Phase 6 results and metrics
5. **Extend**: Customize and optimize Phase 6 for your needs

**Phase 6 Key Features for Beginners**:
- ✅ **Complete Integration**: All systems work together out of the box
- ✅ **Easy Configuration**: Simple configuration for common use cases
- ✅ **Comprehensive Examples**: Multiple Phase 6 example experiments
- ✅ **Performance Monitoring**: Built-in Phase 6 performance tracking
- ✅ **Troubleshooting**: Clear guidance for common Phase 6 issues

**Phase 6 Success Tips**:
- Start with Phase 6 basic examples
- Explore Phase 6 configuration options
- Monitor Phase 6 performance during experiments
- Use Phase 6 documentation for guidance
- Join Phase 6 community for support

**Phase 6 Next Steps**:
1. Complete this getting started guide
2. Run Phase 6 basic examples
3. Explore Phase 6 advanced features
4. Customize Phase 6 for your research
5. Contribute to Phase 6 community

**Phase 6 Welcome**: You are now ready to use NLM Phase 6 - the complete artificial brain with all systems integrated and working together as a coherent neural learning machine!

---

**Phase 6 Getting Started Guide Complete**
The NLM Phase 6 getting started guide provides everything you need to begin working with the complete Phase 6 artificial brain. All documentation is now complete and ready for your Phase 6 exploration!

**Phase 6: Your artificial brain is ready for development, learning, and adaptation!**