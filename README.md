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

### C++ Installation

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Bindings

```bash
# Install Python bindings dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install Python package
cd /path/to/nlm
pip install .

# Or install in development mode
pip install -e .
```

## Running

### C++ Executables

#### Phase 2 Real Neural Computation

```bash
./nlm
```

This runs the original Phase 2 demonstration showing real LIF neuron dynamics, STDP learning, and Hebbian plasticity.

#### Phase 6 Integration Test

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Usage Examples

#### Basic Brain Usage

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
```

#### Complete Agent Example

```python
import pynlm

def run_complete_agent(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_complete_agent(1000)
```

#### AgentBrain with World Integration

```python
import pynlm

# Setup a simple agent-world interaction
def setup_and_run_agent():
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=15,
        height=15,
        visionWidth=8,
        visionHeight=8
    )
    
    # Reset world to initial state
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Agent setup complete:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  World: {world.getWidth()}x{height} cells")
    print(f"  Agent sensory input size: {agent.getSensoryInputSize()}")
    print(f"  Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Run one episode
    for step in range(500):
        # Update environment
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Every 100 steps, show status
        if step % 100 == 0:
            firing_rate = brain.getAverageFiringRate()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"  Step {step}: FR={firing_rate:.1f}Hz, "
                  f"Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
    
    print("Episode complete!")
    return brain, agent, world

# Run the example
brain, agent, world = setup_and_run_agent()
```

#### Sensory-Motor Processing Example

```python
import pynlm

# Demonstrate sensory processing and motor control
def demonstrate_sensory_motor():
    """Show how AgentBrain processes sensory input and generates motor commands."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world with visual features
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    # Set agent at center
    world.setAgentStart(5.0, 5.0)
    agent.initialize(world)
    
    print("=== Sensory-Motor Processing Demo ===")
    print("Starting simulation with visual input...")
    
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get visual percept
        percept = world.getSensoryPercept()
        vision = percept.getVision()
        
        print(f"\nStep {step}:")
        print(f"  Vision input (first 4 values): {[f'{v:.2f}' for v in vision[:4]]}...")
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        print(f"  Motor command: {motor_cmd}")
        
        # Apply action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Log activity
        if step % 50 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            curiosity = agent.getCuriosityLevel()
            print(f"    Activity: {firing} firing neurons, {rate:.1f}Hz rate, "
                  f"Curiosity={curiosity:.2f}")
    
    print("\nDemo complete!")
    print(f"Final state:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")

# Run the demonstration
demonstrate_sensory_motor()
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # C++ build configuration
├── pyproject.toml                    # Python package configuration
├── README.md                         # This documentation
├── docs/                             # Technical documentation
│   ├── ARCHITECTURE.md               # System architecture
│   ├── SCIENCE.md                    # Scientific background
│   ├── ROADMAP.md                    # Development roadmap
│   ├── EXPERIMENTS.md                 # Experiment descriptions
│   ├── LIMITATIONS.md                 # Scientific limitations
│   └── PHASE6_FINAL_AUDIT.md          # Phase 6 audit report
├── src/                              # C++ source code
│   ├── core/                         # Core utilities and types
│   ├── brain/                        # Neural components
│   │   ├── Brain.cpp                  # Main brain coordination
│   │   ├── Brain.hpp                  # Brain class interface
│   │   ├── Neuron.cpp                 # LIF neuron implementation
│   │   ├── Neuron.hpp                 # Neuron interface
│   │   ├── Synapse.cpp                 # Synaptic transmission
│   │   └── Synapse.hpp                 # Synapse interface
│   ├── dynamics/                     # Neural dynamics
│   ├── plasticity/                   # Plasticity rules (STDP, Hebbian)
│   ├── development/                  # Developmental system
│   ├── neuromodulation/               # Neuromodulators (dopamine, curiosity)
│   ├── memory/                      # Memory systems
│   ├── prediction/                   # Prediction system
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiment framework
│   └── visualization/                 # Visualization tools
├── tests/                           # Test suite
│   ├── test_*.cpp                    # Unit tests for individual components
│   ├── test_phase2.cpp                # Phase 2 integration tests
│   ├── test_phase6_integration.cpp    # Phase 6 comprehensive integration tests
│   └── benchmarks/                    # Performance benchmarks
├── python/                          # Python bindings
│   └── bindings.cpp                   # C++ bindings implementation
├── configs/                         # Configuration files
│   ├── default.cfg                   # Default configuration
│   └── example.json                   # JSON configuration example
└── examples/                        # Usage examples
    ├── python/                       # Python examples
    │   ├── simple_brain.py
    │   ├── agent_simulation.py
    │   └── complete_agent.py
    └── cpp/                          # C++ examples
        ├── basic_test.cpp
        └── learning_experiment.cpp
```

## Installation and Setup

### Prerequisites

- **C++**: GCC 10+ or Clang 12+ with C++20 support
- **CMake**: Version 3.16 or higher
- **Python**: 3.10+ (for Python bindings)

### Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. **Build the C++ project**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Install Python bindings**
   ```bash
   pip install scikit-build-core pybind11 pytest numpy
   pip install .
   ```

4. **Run basic tests**
   ```bash
   cd build
   ./nlm_test --gtest_list_tests
   ./nlm_test  # Run all tests
   ```

5. **Run integration demonstration**
   ```bash
   ./nlm_phase6_demo  # Comprehensive Phase 6 integration test
   ```

## Testing

### Unit Tests

```bash
# Build and run unit tests
cd build
make nlm_test
./nlm_test
```

### Integration Tests

```bash
# Run Phase 6 integration tests
cd build
./nlm_phase6_tests
```

### Performance Tests

```bash
# Run performance benchmarks
cd build
./nlm_benchmark --benchmark_iterations 100
```

## Configuration

The NLM system uses configuration files to control brain parameters. Examples are available in the `configs/` directory.

### Configuration Parameters

- `brain.neuron_count`: Total number of neurons
- `brain.synapse_density`: Synapse connectivity density
- `brain.connection_probability`: Probability of connection between neurons
- `brain.v_thresh`: Neuron threshold potential
- `brain.tau_mem`: Membrane time constant
- `plasticity.stdp.enable`: Enable/disable STDP
- `neuromod.dopamine.scale`: Dopamine modulation scale
- `neuromod.curiosity.enable`: Enable/disable curiosity

### Python Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
```

## Advanced Features

### Development System

The NLM brain develops over time through the developmental system:

- **Initial**: High plasticity, rapid learning
- **Critical Period**: Moderate plasticity, pattern learning
- **Maturation**: Low plasticity, stable behavior
- **Adult**: Minimal plasticity, refined responses

```python
agent.updateDevelopment(0.1)  # Advance development by 0.1 time units
stage = agent.getDevelopmentalStage()  # Get current developmental stage
```

### Neuromodulation

- **Dopamine**: Reward prediction error, plasticity modulation
- **Curiosity**: Novelty detection, exploration drive
- **Novelty**: Surprise detection, learning trigger

```python
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore novel things
agent.enableNoveltyDetection(True)      # Detect surprises
```

### Structural Plasticity

The brain can grow new connections and prune unused ones:

```python
agent.enableStructuralPlasticity(True)  # Enable synaptogenesis/pruning
```

## Scientific Background

NLM is based on several key scientific principles:

1. **Spiking Neural Networks**: Leaky Integrate-and-Fire neurons
2. **Spike-Timing-Dependent Plasticity**: STDP learning rules
3. **Neuromodulation**: Dopamine, curiosity, novelty signals
4. **Developmental Psychology**: Stages of learning and maturation
5. **Memory Systems**: Working, episodic, and associative memory

## License

MIT

## Authors

Research project — See docs for scientific background.

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions