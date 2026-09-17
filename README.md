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

## Key Improvements (Phase 6.5 - Enhanced Modularity)

This version includes significant improvements over Phase 6:

### 🚀 Advanced Configuration System
- **Parameter Validation**: Type-safe parameter validation with descriptive error messages
- **Parameter Groups**: Organize parameters by functionality (e.g., "learning", "development", "neuromodulation")
- **Performance Mode**: Built-in performance monitoring and profiling capabilities
- **Checkpoint Customization**: Configure checkpoint intervals, compression, and versioning

### 🧠 Enhanced AgentBrain with Dependency Injection
- **Strategy Pattern**: Separate motor decoding and curiosity handling logic
- **Null Safety**: Comprehensive null pointer checks throughout
- **Configuration-Driven**: Dynamic sensor/motor sizing based on configuration
- **Backward Compatible**: Legacy APIs still supported for existing code

### 🏗️ Improved Modularity
- **Abstract Interfaces**: IMemorySystem, IBrainInterface for better testability
- **Optional Subsystems**: All subsystems are optional with graceful degradation
- **Dependency Injection**: Easy mocking and testing through interfaces
- **Factory Pattern**: Subsystem registry for clean instantiation

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6.5 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected with the new modular architecture.

### Advanced Usage Examples

#### Example 1: Configuration-Driven Agent Setup
```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Configure parameter groups
config.addParameterGroup("memory", ["working_memory_capacity", "episodic_retention"])
config.addParameterGroup("learning", ["plasticity_rate", "novelty_weight"])

# Set advanced parameters
config.set("learning.plasticity_rate", 0.005)
config.set("neuromodulation.curiosity_weight", 2.5)
config.set("performance.monitoring", True)

# Validate configuration
if config.validateAll(errors):
    for error in errors:
        print(f"Validation error: {error}")
else:
    print("Configuration valid!")

# Create brain and agent with modern AgentBrain
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with dependency injection
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enablePerformanceMonitoring(True)
agent.setCheckpointInterval(500)
```

#### Example 2: Custom Strategy Implementation
```python
import pynlm

# Custom motor decoding strategy
class CustomMotorDecodingStrategy(pynlm.MotorDecodingStrategy):
    def decode(self, neurons):
        # Custom logic based on neuron types
        for i, neuron in enumerate(neurons):
            if neuron.getType() == pynlm.NeuronType.Motor:
                if neuron.getState().firingRate > 10.0:
                    return self.mapIndexToCommand(i)
        return pynlm.MotorCommand.Wait
    
    def getName(self):
        return "CustomWeightedStrategy"
    
    def mapIndexToCommand(self, index):
        commands = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
        return commands[min(index, len(commands) - 1)]

# Custom curiosity handler
class AdvancedCuriosityHandler(pynlm.CuriosityHandler):
    def __init__(self, explorationFactor=1.5, decayRate=0.95):
        self.explorationFactor = explorationFactor
        self.decayRate = decayRate
        self.curiosityLevel = 0.0
    
    def update(self, novelty, predictionError, timestep):
        # Advanced curiosity calculation
        self.curiosityLevel = (novelty * 2.0 + abs(predictionError) * 0.5) * explorationFactor
        self.curiosityLevel = min(self.curiosityLevel, 1.0)
        self.curiosityLevel *= pow(decayRate, timestep)
    
    def getCuriosityLevel(self):
        return self.curiosityLevel
    
    def getType(self):
        return "Advanced"

# Create agent with custom strategies
brain = pynlm.createBrain(config)
brain.initialize()

customMotorStrategy = CustomMotorDecodingStrategy()
customCuriosityHandler = AdvancedCuriosityHandler()

agent = pynlm.createAgentBrain(brain, customMotorStrategy, customCuriosityHandler, config)
agent.initialize(world)
```

#### Example 3: Performance Monitoring
```python
import pynlm

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
config.setPerformanceMode(True)
config.setProfilingInterval(0.5)  # Profile every 0.5 seconds
config.setCheckpointInterval(1000)
config.setCheckpointCompression(True)
config.setCheckpointVersion("2.0")

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with monitoring
for step in range(10000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check performance metrics
    if step % 100 == 0:
        firingRate = brain.getAverageFiringRate()
        curiosityLevel = agent.getCuriosityLevel()
        
        print(f"Step {step}: Firing Rate={firingRate:.2f} Hz, "
              f"Curiosity={curiosityLevel:.3f}")

# Save checkpoint with custom settings
brain.save("performance_checkpoint.nlm")
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md          # System architecture overview
│   ├── SCIENCE.md              # Scientific background
│   ├── ROADMAP.md              # Development roadmap
│   ├── EXPERIMENTS.md          # Experiment descriptions
│   ├── LIMITATIONS.md          # Current limitations
│   └── PHASE6_ENHANCEMENTS.md   # New modularity features
├── src/
│   ├── agent/
│   │   ├── AgentBrain.cpp          # Agent with dependency injection
│   │   ├── AgentBrain.hpp          # Agent interface
│   │   ├── ModularityLayer.hpp    # Strategy patterns and DI
│   │   └── StrategyImplementations.cpp - Strategy implementations
│   ├── core/
│   │   ├── Config/              # Enhanced configuration system
│   │   └── Logger/              # Logging system
│   ├── brain/                   # Brain components
│   ├── world/                   # Environment system
│   └── ...                      # Other systems
├── tests/                      # Unit tests
└── examples/                  # Example scripts
    ├── basic_usage.py
    ├── advanced_configuration.py
    ├── custom_strategies.py
    └── performance_monitoring.py
```

## Phase Summary

### Phase 1 (Complete) - Project Skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete) - Core Neuroscience
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete) - World Interaction
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages

### Phase 4 (Complete) - Advanced Cognition
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation

### Phase 5 (Complete) - Performance & Integration
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- **NEW**: Configuration validation and parameter groups

### Phase 6 (Complete) - Final Integration
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

### Phase 6.5 (Complete) - Enhanced Modularity
- **Strategy Pattern**: Motor decoding and curiosity handling
- **Dependency Injection**: Loose coupling through interfaces
- **Configuration System**: Advanced validation and organization
- **Performance Monitoring**: Built-in profiling and metrics
- **Backward Compatibility**: Legacy APIs still supported

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

## Architecture Philosophy

The NLM brain operates as a closed-loop system with enhanced modularity:

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
ACTION SELECTION (with custom strategies)
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

## New Modularity Features

### Strategy Pattern
Motor decoding and curiosity handling are now pluggable:
- `MotorDecodingStrategy`: Abstract interface for motor control
- `CuriosityHandler`: Abstract interface for exploration behavior
- Multiple built-in strategies with custom implementations possible

### Dependency Injection
- `IBrainInterface`: Abstract brain interface for testing
- `IMemorySystem`: Abstract memory system
- `SubsystemRegistry`: Clean subsystem management

### Advanced Configuration
- Type-safe parameter validation
- Parameter groups for organization
- Performance monitoring and checkpoint customization
- Debug mode and log level control

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Help

1. **Basic Usage**: Read `easy_usage.md` for quick start
2. **Detailed Instructions**: Read `HOW_TO_USE.md` for comprehensive guidance
3. **Architecture**: Read `docs/ARCHITECTURE.md` to understand system integration
4. **Advanced Features**: Explore examples in the `examples/` directory
5. **Configuration**: Use the new `config.setParameterGroup()` and validation APIs

The enhanced NLM system now provides both simple usage for beginners and advanced customization for power users while maintaining full backward compatibility.
