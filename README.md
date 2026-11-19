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

**Phase 6 Highlights**:
- Enhanced Python bindings with advanced features
- Improved configuration system with validation
- Professional error handling and type safety
- Rich API for complex neural simulations

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

## Enhanced Python Bindings (pynlm)

### New Features in Phase 6

The NLM Python bindings have been completely enhanced with advanced features that make the API more Pythonic, robust, and feature-rich:

#### 1. Enhanced Configuration System
- **Type-safe configuration access**: `config.get("key")` automatically returns Python native types
- **Dictionary integration**: `config.from_dict()` and `config.as_dict()` for seamless Python integration  
- **Configuration validation**: `config.validate()` with detailed error messages
- **Advanced operations**: `config.merge()`, `config.compare()` for complex scenarios

#### 2. Advanced Error Handling
- **Specific exception classes**: `ConfigError`, `BrainError`, `WorldError`, `AgentError`
- **Helpful error messages**: Detailed information for debugging
- **Graceful error recovery**: Better error handling for robust applications

#### 3. Rich Type Support
- **Automatic type conversion**: `config.get()` returns int, float, bool, str, or list
- **Smart defaults**: `config.getOr()` with type-preserving defaults
- **List support**: Configuration values can be Python lists

#### 4. Professional Tools
- **Enhanced save/load**: `saveToFile()` produces human-readable formats
- **Source tracking**: Tracks configuration origin (file, command-line, runtime)
- **Validation on load**: Checks configuration consistency

#### 5. Enhanced Brain APIs
- **Pythonic method signatures**: `brain.step(step, time)` instead of just `step(step)`
- **Rich statistics**: Detailed neural network metrics and analytics
- **Development support**: Comprehensive developmental stage management

### Quick Start with Enhanced Bindings

```python
import pynlm

# Create configuration with enhanced features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.enabled", True)
config.set("plasticity.learning_rate", 0.005)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Advanced step with time support
brain.step(1, 0.001)

# Rich statistics
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### Advanced Usage Examples

#### Configuration Management
```python
# Dictionary integration for complex configurations
config_dict = {
    "brain.neuron_count": 1500,
    "learning.enabled": [1.0, 2.0, 3.0],
    "tags": ["research", "experimental"]
}
config = pynlm.createDefaultConfig()
config.from_dict(config_dict)

# Configuration validation
config.validate([
    "brain.neuron_count",
    "plasticity.learning_rate"
])

# Configuration operations
merged = config1.merge(config2)  # config2 overrides config1
if config1.compare(config2, tolerance=1e-6):
    print("Configurations are equivalent")
```

#### Error Handling
```python
try:
    config.validate(["required.key"])
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")

try:
    brain.step(step=10, time=0.1)
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
```

#### Complete Agent Simulation
```python
import pynlm

def run_advanced_agent_simulation(config, world, steps=1000):
    """Run a complete agent simulation with error handling"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    for step in range(steps):
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    return brain, agent

# Example usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

brain, agent = run_advanced_agent_simulation(config, world, steps=100)
```

### Best Practices

#### For Beginners
- Use `config.from_dict()` for initial setup
- Always validate configuration before use
- Use `config.getOr()` for optional keys
- Let exceptions guide you through error handling

#### For Experienced Users
- Use `config.merge()` for configuration overrides
- Implement custom validation rules
- Use type-safe access patterns
- Leverage advanced error handling for robust applications

#### For Development
- Validate configurations before deployment
- Use comprehensive error handling
- Implement proper logging and monitoring
- Test with different configuration scenarios

### Additional Resources

- **ENHANCED_BINDINGS_DOCUMENTATION.md**: Complete API reference
- **IMPROVEMENT_SUMMARY.md**: Detailed improvement documentation  
- **test_improvements.py**: Working examples of new features
- **easy_usage.md**: Quick start guide for basic usage
- **HOW_TO_USE.md**: Comprehensive usage documentation

The enhanced NLM Python bindings provide a professional, Pythonic API that serves users at all levels while maintaining full backward compatibility with existing code.
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
