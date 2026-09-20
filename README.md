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

### Standard Build (Recommended)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Optimized Build (For Production)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DNLM_BUILD_SHARED=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON
make -j$(nproc)
```

### Python Development (For API Testing)

```bash
# Install Python dependencies first
pip install scikit-build-core pybind11 pytest numpy

# Build and install Python module
pip install -e .
```

## Running

### Phase 2: Real Neural Computation (Default Demo)

```bash
./nlm  # Run the main simulation demonstrating real LIF neurons
```

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo  # Comprehensive integration test
```

### Phase 4 Demo

```bash
./nlm_phase4_demo  # Advanced cognitive capabilities demo
```

### Python Examples

```python
# Quick test of the NLM system
python -c "import pynlm; config = pynlm.createDefaultConfig(); brain = pynlm.createBrain(config); brain.initialize(); print(f'Brain created: {brain.getTotalNeuronCount()} neurons')"

# Run an example simulation
python examples/example_simulation.py
```

### Running Tests

```bash
# Run all unit tests
ctest --output-on-failure

# Run specific test suite
./nlm_test
```

### Benchmarking

```bash
# Run performance benchmarks
./benchmark
```

## Quick Start for Developers

```bash
# Full development setup
./scripts/setup_dev.sh

# For rapid prototyping
./scripts/quick_protoype.sh
```

## Performance Optimization Tips

1. **Use Release Build:** Always build with `-DCMAKE_BUILD_TYPE=Release` for best performance
2. **Parallel Processing:** Use `make -j$(nproc)` to utilize all CPU cores
3. **Memory Management:** Monitor memory usage with `valgrind` for debugging
4. **Python Bindings:** For better Python performance, compile with `-DNLM_BUILD_SHARED=ON`

## Troubleshooting

### "CMake cannot find Python"

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

### "Compilation errors with C++20"

```bash
# Use GCC 11+ or Clang 14+
gcc --version  # Should show 11.x or higher
```

### "ImportError: No module named 'pynlm'"

```bash
pip install --force-reinstall .
```

### "Segmentation faults"

```bash
# Ensure proper initialization
brain.initialize()  # Call this before any simulation steps
brain.reset()       # Call before re-running simulation
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

## Quick Start Examples

### Example 1: Simple Neural Simulation

```python
import pynlm

# Create a basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for 100 simulation steps
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Example 2: Complete Agent in Environment

```python
import pynlm

# Create agent with world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Example 3: Learning Experiment

```python
import pynlm

# Create brain for learning experiment
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Record initial state
initial_weights = []
if region := brain.getRegion(1):
    for syn in region.getSynapses():
        initial_weights.append(syn.getWeight())

print(f"Initial average weight: {sum(initial_weights)/len(initial_weights):.3f}")

# Apply learning through experience
for step in range(1000):
    # Inject varied sensory input
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0 + (step % 100))
    brain.step(step)

# Check for learning
final_weights = []
if region := brain.getRegion(1):
    for syn in region.getSynapses():
        final_weights.append(syn.getWeight())

initial_avg = sum(initial_weights)/len(initial_weights)
final_avg = sum(final_weights)/len(final_weights)
learning_detected = abs(final_avg - initial_avg) > 0.01

print(f"Learning detected: {learning_detected}")
print(f"Weight change: {final_avg - initial_avg:.3f}")
```

### Example 4: Advanced Features Demonstration

```python
import pynlm

# Advanced configuration
config = pynlm.createDefaultConfig()

# Configure all major systems
config.set("brain.neuron_count", 1000)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("plasticity.structural.enable", True)
config.set("development.enable", True)

# Create and initialize
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Enable everything
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run with monitoring
def run_advanced_simulation():
    stats = {
        'steps': 0,
        'total_reward': 0.0,
        'curiosity': 0.0,
        'novelty': 0.0,
        'neurons_firing': 0,
        'total_spikes': 0
    }
    
    for step in range(200):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        stats.update({
            'steps': step + 1,
            'total_reward': stats['total_reward'] + reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neurons_firing': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        })
    
    return stats

results = run_advanced_simulation()
print(f"Advanced simulation completed:")
print(f"  Final curiosity: {results['curiosity']:.3f}")
print(f"  Final novelty: {results['novelty']:.3f}")
print(f"  Average firing rate: {results['neurons_firing']/results['steps']:.1f}")
```

## Quick Start Examples

### Example 1: Simple Neural Simulation

```python
import pynlm

# Create a basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for 100 simulation steps
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Example 2: Complete Agent in Environment

```python
import pynlm

# Create agent with world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Example 3: Learning Experiment

```python
import pynlm

# Create brain for learning experiment
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Record initial state
initial_weights = []
if region := brain.getRegion(1):
    for syn in region.getSynapses():
        initial_weights.append(syn.getWeight())

print(f"Initial average weight: {sum(initial_weights)/len(initial_weights):.3f}")

# Apply learning through experience
for step in range(1000):
    # Inject varied sensory input
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0 + (step % 100))
    brain.step(step)

# Check for learning
final_weights = []
if region := brain.getRegion(1):
    for syn in region.getSynapses():
        final_weights.append(syn.getWeight())

initial_avg = sum(initial_weights)/len(initial_weights)
final_avg = sum(final_weights)/len(final_weights)
learning_detected = abs(final_avg - initial_avg) > 0.01

print(f"Learning detected: {learning_detected}")
print(f"Weight change: {final_avg - initial_avg:.3f}")
```

### Example 4: Advanced Features Demonstration

```python
import pynlm

# Advanced configuration
config = pynlm.createDefaultConfig()

# Configure all major systems
config.set("brain.neuron_count", 1000)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("plasticity.structural.enable", True)
config.set("development.enable", True)

# Create and initialize
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Enable everything
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run with monitoring
def run_advanced_simulation():
    stats = {
        'steps': 0,
        'total_reward': 0.0,
        'curiosity': 0.0,
        'novelty': 0.0,
        'neurons_firing': 0,
        'total_spikes': 0
    }
    
    for step in range(200):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        stats.update({
            'steps': step + 1,
            'total_reward': stats['total_reward'] + reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neurons_firing': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        })
    
    return stats

results = run_advanced_simulation()
print(f"Advanced simulation completed:")
print(f"  Final curiosity: {results['curiosity']:.3f}")
print(f"  Final novelty: {results['novelty']:.3f}")
print(f"  Average firing rate: {results['neurons_firing']/results['steps']:.1f}")
```

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
