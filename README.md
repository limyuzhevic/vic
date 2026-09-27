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
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

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

```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

---

## Getting Started - Practical Implementation Guide

This section provides **actionable examples** to help you get started with NLM immediately.

### Quick Start (15 seconds)

```bash
# Clone the repository
cd /path/to/nlm

# Build (if needed)
mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4

# Install Python bindings
pip install -e .

# Run the integration demo
./nlm_phase6_demo
```

### Basic Python Usage (Your First NLM Brain)

```python
import pynlm

# Create a complete artificial developmental brain (Phase 6)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation (your brain thinks!)
for step in range(100):
    brain.step(step)

# See what happened
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Currently firing: {brain.getFiringNeuronCount()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Complete Agent Example (Realistic NLM Usage)

```python
import pynlm

def run_complete_agent_episode(world_size=20, num_steps=1000):
    """Run a complete NLM agent episode with all Phase 6 features."""
    
    # 1. Create and initialize the brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure the world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(world_size/2.0, world_size/2.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable realistic NLM features
    agent.enableRewardModulation(True)      # Dopamine-based learning
    agent.enableStructuralPlasticity(True)  # Synaptogenesis and pruning
    agent.enableDevelopment(True)           # Age-dependent plasticity
    agent.enableCuriosity(True)             # Exploration drive
    
    # 6. Run the complete episode
    print(f"=== NLM Agent Episode ({world_size}x{world_size} world) ===")
    
    for step in range(num_steps):
        # Update world environment
        world.update(0.1)
        
        # Get sensory percept (vision, proprioception, internal signals)
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Brain computes neural dynamics
        brain.step(step)
        
        # Agent decodes motor command from brain activity
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 200 steps
        if step % 200 == 0 and step > 0:
            print(f"\nProgress Step {step}:")
            print(f"  Neural Activity: {brain.getFiringNeuronCount()} firing")
            print(f"  Learning Signals: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
    
    print(f"\n=== Episode Complete ===")
    print(f"Total reward received: {agent.getNeuromodulationLevel():.3f}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total developmental stages experienced: {brain.getTotalSpikeCount() // 100}")
    
    return {
        'total_reward': agent.getNeuromodulationLevel(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'development_stages': brain.getDevelopmentalStage(),
        'final_curiosity': agent.getCuriosityLevel()
    }

# Run the episode
results = run_complete_agent_episode(world_size=15, num_steps=2000)
```

### Advanced Configuration Examples

#### Example 1: High-Performance Brain Configuration

```python
import pynlm

# Create a high-performance brain for complex tasks
config = pynlm.createDefaultConfig()

# Optimize for performance and learning
config.set("brain.neuron_count", 3000)              # More neurons for capacity
config.set("brain.synapse_density", 0.08)            # Balanced connectivity
config.set("plasticity.stdp.enable", True)          # Spike-timing learning
config.set("plasticity.hebbian.enable", True)       # Hebbian learning
config.set("plasticity.structural.enable", True)    # Structural plasticity
config.set("neuromod.dopamine.scale", 1.5)          # Stronger reward signals
config.set("neuromod.curiosity.enable", True)       # High exploration
config.set("neuromod.novelty.enable", True)         # Novelty detection
config.set("development.enable_critical_periods", True)  # Advanced development

# Create and initialize
brain = pynlm.createBrain(config)
brain.initialize()

print(f"High-performance brain created: {brain.getTotalNeuronCount()} neurons")
print(f"E/I balance: {brain.getExcitationInhibitionRatio():.3f}")
```

#### Example 2: Memory-Optimized Configuration

```python
import pynlm

# Create a memory-focused brain
config = pynlm.createDefaultConfig()

# Optimize for memory and episodic learning
config.set("brain.neuron_count", 2000)              # Fewer neurons, more efficient
config.set("plasticity.stdp.enable", True)          # STDP for memory binding
config.set("plasticity.hebbian.enable", True)       # Hebbian for pattern association
config.set("neuromod.dopamine.scale", 0.8)          # Moderate reward sensitivity
config.set("neuromod.curiosity.enable", False)      # Less exploration, more focus
config.set("neuromod.novelty.enable", True)         # Novelty detection for memory

# Enable memory subsystems
config.set("memory.episodic.enable", True)          # Episodic memory
config.set("memory.working.enable", True)           # Working memory
config.set("memory.associative.enable", True)       # Associative memory

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Memory-optimized brain: {brain.getTotalNeuronCount()} neurons")
```

#### Example 3: Developmental Trajectory Configuration

```python
import pynlm

# Create a brain with extended developmental period
config = pynlm.createDefaultConfig()

# Configure developmental trajectory
config.set("brain.neuron_count", 1500)
config.set("development.initial_plasticity", 1.0)   # High initial plasticity
config.set("development.critical_period_duration", 5000)  # Extended critical period
config.set("development.maturation_rate", 0.001)     # Slow maturation
config.set("development.pruning_rate", 0.0001)      # Gentle pruning

# Set up learning systems for development
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)    # Structural changes during development
config.set("neuromod.dopamine.scale", 1.2)          # Reward-guided development

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Developmental brain: {brain.getTotalNeuronCount()} neurons")
print(f"Initial development stage: {brain.getDevelopmentalStage()}")
```

### Real-World Application Patterns

#### Pattern 1: Continuous Learning Agent

```python
import pynlm
import time

class NLMContinuousLearner:
    def __init__(self, world_size=30):
        self.world_size = world_size
        self.create_systems()
        self.enable_adaptive_behavior()
    
    def create_systems(self):
        # Create brain with adaptive capacity
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 2000)
        self.config.set("plasticity.structural.enable", True)
        
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=self.world_size,
            height=self.world_size,
            visionWidth=10,
            visionHeight=10
        )
        
        # Initialize systems
        self.brain.initialize()
        self.agent.enableAllFeatures()
        self.agent.initialize(self.world)
    
    def enable_adaptive_behavior(self):
        # Enable all NLM's advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def run_learning_session(self, duration_steps=5000):
        print(f"=== Continuous Learning Session ({duration_steps} steps) ===")
        
        start_time = time.time()
        last_log = 0
        
        for step in range(duration_steps):
            # Update environment
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Neural computation
            self.brain.step(step)
            
            # Action selection and execution
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Reward processing and development
            reward = self.get_environment_reward()
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            # Log progress periodically
            if step - last_log >= 500:
                self.log_session_progress(step, duration_steps)
                last_log = step
        
        elapsed = time.time() - start_time
        self.print_session_summary(duration_steps, elapsed)
        
        return self.get_session_stats()
    
    def get_environment_reward(self):
        # Example reward function - adapt to your environment
        percept = self.world.getSensoryPercept()
        body = self.world.getAgentBody()
        
        # Reward for being alive and energetic
        reward = body.health * 0.1
        
        # Additional rewards for specific achievements
        if body.energy > 50:
            reward += 0.5  # Energy reward
        
        return min(reward, 1.0)
    
    def log_session_progress(self, step, total):
        print(f"\nStep {step}/{total} ({step/total*100:.1f}%):")
        print(f"  Neural: {self.brain.getFiringNeuronCount()} firing, {self.brain.getAverageFiringRate():.2f} Hz")
        print(f"  Learning: {self.agent.getNeuromodulationLevel():.3f}, {self.agent.getCuriosityLevel():.3f} curiosity")
        print(f"  Development: {self.brain.getDevelopmentalStage()}")
    
    def print_session_summary(self, total_steps, elapsed_time):
        print(f"\n=== Learning Session Complete ===")
        print(f"Total steps: {total_steps}")
        print(f"Time elapsed: {elapsed_time:.2f}s")
        print(f"Performance: {total_steps/elapsed_time:.0f} steps/second")
        print(f"Final state:")
        print(f"  Development stage: {self.brain.getDevelopmentalStage()}")
        print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
    
    def get_session_stats(self):
        return {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'final_development': self.brain.getDevelopmentalStage(),
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_prediction_error': self.agent.getPredictionError()
        }

# Usage example
learner = NLMContinuousLearner(world_size=25)
results = learner.run_learning_session(duration_steps=10000)
```

#### Pattern 2: Checkpoint-Based Learning

```python
import pynlm
import os

def run_checkpointed_learning(session_id, max_steps=5000):
    """Run learning with checkpointing for recovery and analysis."""
    
    checkpoint_dir = f"checkpoints/session_{session_id}"
    os.makedirs(checkpoint_dir, exist_ok=True)
    
    # Create brain with persistent learning capacity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure for long-term learning
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    agent.enableAllFeatures()
    
    # Enable checkpointing
    brain.enableCheckpointing(True)
    
    print(f"=== Checkpointed Learning Session {session_id} ===")
    
    # Try to load from previous checkpoint
    last_checkpoint = f"{checkpoint_dir}/checkpoint_{session_id-1}.bin"
    if os.path.exists(last_checkpoint):
        print(f"Loading from checkpoint: {last_checkpoint}")
        brain.load(last_checkpoint)
    else:
        print("Starting new session (no previous checkpoint)")
        brain.initialize()
        agent.initialize(world)
    
    # Run learning with periodic checkpointing
    for step in range(max_steps):
        world.update(0.1)
        
        # Store previous state for comparison
        prev_spikes = brain.getTotalSpikeCount()
        prev_development = brain.getDevelopmentalStage()
        
        # Process and learn
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)  # Small reward for learning
        agent.updateDevelopment(0.1)
        
        # Checkpoint progress periodically
        if step % 1000 == 0 and step > 0:
            checkpoint_file = f"{checkpoint_dir}/checkpoint_{session_id}.bin"
            brain.save(checkpoint_file)
            print(f"Checkpoint saved: {checkpoint_file}")
            
            # Analyze progress
            learning_progress = {
                'step': step,
                'spike_increase': brain.getTotalSpikeCount() - prev_spikes,
                'development_stage': brain.getDevelopmentalStage(),
                'development_change': brain.getDevelopmentalStage() != prev_development,
                'neuromodulation': agent.getNeuromodulationLevel()
            }
            print(f"  Progress: {learning_progress}")
    
    # Save final state
    final_checkpoint = f"{checkpoint_dir}/checkpoint_{session_id}_final.bin"
    brain.save(final_checkpoint)
    print(f"Final checkpoint: {final_checkpoint}")
    
    return final_checkpoint

# Run multiple learning sessions with checkpoint recovery
for session_id in range(3, 6):
    checkpoint = run_checkpointed_learning(session_id, max_steps=3000)
    print(f"Session {session_id} completed. Checkpoint: {checkpoint}\n")
```

---

## Project Structure Overview

```
NLM/
├── src/
│   ├── core/           # Core utilities and types
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/        # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
│
├── python/             # Python bindings
│   └── bindings.cpp    # Pybind11 interface
│
├── CMakeLists.txt      # Build configuration
├── pyproject.toml      # Python package configuration
├── README.md           # Project documentation
├── HOW_TO_USE.md       # Usage documentation
├── easy_usage.md       # Beginner-friendly guide
└── docs/               # Comprehensive documentation
    ├── ARCHITECTURE.md
    ├── SCIENCE.md
    ├── ROADMAP.md
    ├── EXPERIMENTS.md
    ├── LIMITATIONS.md
    └── ...
```

---

## Quick Command Reference

### Building and Installation

```bash
# Build the C++ core
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install -e .

# Or use modern Python packaging
python -m build
pip install dist/*.whl
```

### Running Demos

```bash
# Phase 6 Integration Demo
./nlm_phase6_demo

# Phase 3 Demonstration  
./nlm_phase3_demo

# Phase 4 Demonstration
./nlm_phase4_demo

# Unit tests
./nlm_test
```

### Python Usage

```bash
# Verify installation
python -c "import pynlm; print(pynlm.__version__)"

# Run example scripts
python examples/simple_brain.py
python examples/complete_agent.py
python examples/checkpoint_learning.py
```

### Development Commands

```bash
# Build in debug mode
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run all tests
ctest --output-on-failure

# Build with specific options
cmake .. -DPYTHON_EXECUTABLE=/path/to/python -DCMAKE_CXX_STANDARD=20
```

---

## Contributing

This project is an active research initiative. Contributions are welcome for:

- **Code improvements** and optimizations
- **New experiment designs** and implementations  
- **Performance enhancements** and scaling
- **Documentation improvements**
- **Python API extensions**
- **Bug fixes and testing**

See the `docs/` directory for detailed contribution guidelines and research protocols.

---

## Architecture Philosophy

The NLM brain operates as a **closed-loop system** where cognition emerges from neural dynamics:

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

This architecture enables **artificial developmental brains** that learn, remember, and adapt through experience, with multiple memory systems, neuromodulation, prediction, and cognitive subsystems all integrated into a coherent neural substrate.

---

## License

MIT License. See `LICENSE` file for details.

---

## Authors

Research project — See `docs/` for scientific background and methodologies.

**Phase 6 Complete (Final Integration)** - All systems integrated and functional.

---

*This documentation provides **practical implementation guidance** for using NLM as a complete artificial developmental brain system. For theoretical background, see the `docs/` directory.*
```

---

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently
```

---

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

The NLM brain is now a functioning artificial neural substrate
 capable of changing its own synaptic connections through experience.

---

## Authors

Research project — See docs for scientific background.

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

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
