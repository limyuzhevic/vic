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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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

Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

## Python API Usage

The NLM framework includes a comprehensive Python API that provides access to the C++ neural simulation framework. The Python API has been significantly enhanced with:

### Installation

Install the NLM Python bindings:

```bash
pip install .
```

For development with testing dependencies:
```bash
pip install .[dev]
```

### Basic Usage

The NLM (熙然) Python bindings provide access to the C++ neural simulation framework through a Python interface. The improved API now includes:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create research-grade configuration
config = pynlm.createResearchConfig()

# Create debug configuration
config = pynlm.createDebugConfig()

# Create performance-optimized configuration
config = pynlm.createPerformanceConfig()

# Create brain and environment components
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize and run simulation
brain.initialize()
world.configure(20, 20, 16, 16)
agent.initialize(world)

for step in range(1000):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step, step * 0.001)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply to world
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    agent.applyRewardModulation(result.reward, 0.0)
    agent.updateDevelopment(0.001)
```

### Configuration Management

The Python API supports multiple configuration formats:

```python
# Load from file (JSON, YAML, or simple key=value)
config.loadFromFile("configs/default.cfg")
config.loadFromFile("config.json")

# Load from command line arguments
config.loadFromArgs(["--neurons=500", "--steps=2000"])

# Load from JSON string
json_config = '{"neuron_count": 1000, "region_count": 2}'
config.loadFromJsonString(json_config)

# Save configuration
config.saveToFile("output_config.cfg")

# Get configuration values
neuron_count = config.getInt("neuron_count")
simulation_timestep = config.getDouble("simulation_timestep")
enabled = config.getBool("enable_development")
```

### Advanced Features

The Python API includes advanced features for research and development:

```python
# Enable specific subsystems
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Get system states and metrics
neuromodulation_level = agent.getNeuromodulationLevel()
curiosity_level = agent.getCuriosityLevel()
developmental_stage = agent.getDevelopmentalStage()

# Brain statistics
total_neurons = brain.getTotalNeuronCount()
total_spikes = brain.getTotalSpikeCount()
avg_firing_rate = brain.getAverageFiringRate()
```

### Full Integration Example

Here's a complete example demonstrating the full integration:

```python
import pynlm

# Create research configuration
config = pynlm.createResearchConfig()

# Initialize all components
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.setRandomSeed(42)
agent.initialize(world)

# Enable learning systems
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run complete simulation loop
print("Starting NLM simulation...")
for step in range(500):
    # Perceive environment
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Neural processing
    brain.step(step, step * 0.001)
    
    # Decide and act
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    agent.applyRewardModulation(result.reward, 0.0)
    agent.updateDevelopment(0.001)
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}: Reward={result.reward:.3f}, "
              f"Firing={brain.getAverageFiringRate():.2f}Hz")

# Print final results
print(f"\nSimulation Complete!")
print(f"Total neuromodulation: {agent.getNeuromodulationLevel():.3f}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
```

### Configuration Templates

The NLM framework provides several standard configuration templates:

- **Default**: Minimal configuration for basic operation
- **Research**: Enhanced configuration for scientific experiments
- **Debug**: Configuration optimized for development and debugging
- **Performance**: Configuration optimized for large-scale simulations

Each template is available via the factory methods shown above.

### Configuration File Formats

The configuration system supports multiple file formats:

**Simple Key=Value Format:**
```
neuron_count = 500
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
```

**JSON Format:**
```json
{
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "log_level": "INFO"
}
```

**YAML Format:**
```yaml
neuron_count: 500
region_count: 2
connection_probability: 0.1
simulation_timestep: 0.001
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
log_level: INFO
```

## Python API Reference

See `python_API.md` in the repository for complete API documentation, including all available classes, methods, and examples.

## Additional Resources

For more detailed information and examples:

- **Python API Documentation**: `python_API.md` in the repository
- **Phase 6 Documentation**: `docs/PHASE6_FINAL_AUDIT.md`
- **Architecture**: `docs/ARCHITECTURE.md`
- **Science Documentation**: `docs/SCIENCE.md`

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
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
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
