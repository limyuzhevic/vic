## Building

### Standard Installation (Recommended for most users)

```bash
# Install with pip (this will build from source)
pip install .

# Or install in development mode
pip install -e .
```

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

### Using pip for Development

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .
```

## Running

### Phase 6 Demo (Integration Test)

```bash
# After building, run the integration test
./nlm_phase6_demo
```

### Python Integration Tests

```bash
# Run Python tests (requires pytest)
pytest
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Scientific documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                     # C++ source code
│   ├── core/               # Core utilities
│   ├── brain/              # Neural components (integrated)
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules
│   ├── development/        # Developmental system
│   ├── neuromodulation/     # Neuromodulators (integrated)
│   ├── memory/             # Memory systems (integrated)
│   ├── prediction/         # Prediction systems (integrated)
│   ├── cognition/          # Cognitive mechanisms (integrated)
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── experiments/        # Experiment framework (Phase 6)
│   └── visualization/       # Visualization
├── tests/                  # C++ unit tests
├── python/                 # Python bindings (source)
├── configs/                # Configuration files
└── docs/                   # Documentation
```

## Python API Overview

NLM provides a comprehensive Python API for simulating artificial brains. The API is organized around three main concepts:

1. **Brain**: The virtual neural system that thinks and learns
2. **World**: The environment the brain interacts with
3. **Agent**: The interface that connects brain to world

### Quick Start

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Phase 6: Complete Brain Integration

Phase 6 represents the final integration of all NLM systems into a coherent artificial brain. This is the default configuration when using Python bindings.

### Key Features of Phase 6

1. **Memory Systems**: Working, episodic, and associative memory
2. **Neuromodulation**: Dopamine, curiosity, and novelty detection
3. **Prediction System**: Forward models and prediction error
4. **Cognition Systems**: Planning, concept formation, attention
5. **Development**: Age-dependent plasticity and maturation

### Learning Systems Enabled

```python
# All learning systems are enabled by default in Phase 6
agent.enableRewardModulation(True)      # Reinforcement learning
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

## Advanced Examples

### Example 1: Complete Learning Agent

```python
import pynlm

def run_learning_agent(config=None, steps=1000):
    """Run a complete learning agent with all systems enabled"""
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getAgentBody().energy / 100.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return brain, agent, world

# Run the agent
brain, agent, world = run_learning_agent()
```

### Example 2: Brain State Management

```python
import pynlm
import tempfile
import os

def save_and_continue(config, steps=500):
    """Save brain state and continue from saved state"""
    # Create and run brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    # Save brain state
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
        save_path = f.name
    
    if brain.save(save_path):
        print(f"Brain saved to: {save_path}")
        
        # Create new brain and load state
        new_brain = pynlm.createBrain(config)
        if new_brain.load(save_path):
            print("Brain loaded successfully!")
            
            # Continue from saved state
            for step in range(steps, steps * 2):
                new_brain.step(step)
            
            print(f"Loaded brain total spikes: {new_brain.getTotalSpikeCount()}")
        
        # Clean up
        os.unlink(save_path)
    
    return brain
```

### Example 3: Custom Configuration

```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()

# Modify brain parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.v_thresh", -50.0)

# Enable specific plasticity rules
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)

print(config.summary())
```

## Python Testing

### Basic Python Tests

```python
import pynlm

def test_basic_functionality():
    """Test basic NLM Python bindings"""
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic operations
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Run simulation
    for step in range(10):
        brain.step(step)
    
    # Check results
    assert brain.getTotalSpikeCount() > 0
    assert brain.getFiringNeuronCount() >= 0
    
    print("✓ Basic functionality test passed")

def test_agent_system():
    """Test agent-brain and world integration"""
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Run cycle
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check results
    assert brain.getTotalSpikeCount() > 0
    assert agent.getCuriosityLevel() >= 0
    
    print("✓ Agent system test passed")

# Run tests
if __name__ == "__main__":
    test_basic_functionality()
    test_agent_system()
    print("\nAll Python tests passed!")
```

## Performance Considerations

### Brain Size and Performance

```python
# Performance scales approximately with neuron count
# For testing and development:
config.set("brain.neuron_count", 100)      # Fast, small brain
# For realistic simulations:
config.set("brain.neuron_count", 10000)    # Slower, more complex
# For research experiments:
config.set("brain.neuron_count", 100000)   # Large, computationally intensive
```

### Memory Management

```python
import pynlm
import gc

def memory_efficient_simulation(config, steps=1000):
    """Run simulation with memory management"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Check memory usage periodically
        if step % 500 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
            
            # Force garbage collection
            gc.collect()
    
    return brain
```

## Advanced Features

### Direct Neuron Control

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Find sensory neurons and inject current
sensory_neurons = []
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    # Find sensory neurons in region
    # Implementation depends on specific region structure

# Inject current into sensory neurons
for neuron_id in sensory_neurons:
    brain.injectCurrent(neuron_id, 1.0)  # Inject 1.0 current units
```

### Custom Sensory Input

```python
import pynlm
from pynlm import Vision, InternalSignals

# Create custom sensory input
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create vision input with custom pattern
vision_data = [0.1, 0.5, 0.9, 0.3, 0.7, 0.2, 0.8, 0.4] * 10  # 80 values (8x10)
vision = Vision(8, 10, 1)  # 8x10 grayscale
vision.setData(vision_data)

# Create internal signals
internal = InternalSignals()
internal.addSignal(0.5)  # Add some internal state
internal.addSignal(-0.3)

# Create percept and send to brain
percept = pynlm.SensoryPercept()
percept.setVision(vision)
percept.setInternal(internal)

brain.receiveSensoryInput(percept)
```

## Troubleshooting

### Common Issues and Solutions

**"ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

**"Segmentation fault during simulation"
```
# Ensure proper initialization
brain.initialize()  # ALWAYS call this before brain.step()
```

**"Brain not learning or showing activity"
```python
# Check that learning systems are enabled
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

**"Memory usage too high"
```python
# Use smaller brain size for testing
config.set("brain.neuron_count", 100)
```

**"Performance is too slow"
```python
# Increase C++ optimization
# Re-run with: cmake .. -DCMAKE_BUILD_TYPE=Release
```

## Further Reading

1. **Architecture Documentation**: `docs/ARCHITECTURE.md` - Detailed system overview
2. **Science Documentation**: `docs/SCIENCE.md` - Scientific background and principles
3. **Experiments Documentation**: `docs/EXPERIMENTS.md` - Experiment descriptions and results
4. **Performance Documentation**: `docs/PERFORMANCE.md` - Performance benchmarks and optimizations

## License

MIT License - See LICENSE file for details.

## Authors

Research project — See docs for scientific background.