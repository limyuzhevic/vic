# NLM — 熙然

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Quick Start (1 Minute Setup)

NLM comes with several interactive demonstrations that showcase its capabilities:

### Phase 2: Real Neural Computation
```bash
# Build and run Phase 2 demo
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_phase2_demo
```

### Phase 3: World Interaction
```bash
# Run world interaction demo
./nlm_phase3_demo
# Or with custom parameters
./nlm_phase3_demo 10 200
```

### Phase 4: Emerging Cognition
```bash
# Run cognitive development demo
./nlm_phase4_demo
# With trial count
./nlm_phase4_demo 100
```

### Phase 5: Lifetime Learning
```bash
# Run comprehensive lifetime learning
./nlm_phase5_demo
```

### Phase 6: Integration
```bash
# Run final integration test
./nlm_phase6_demo
```

## Quick Reference Guide

### Basic Usage (Python API)

**Create and initialize a brain:**
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

**Run simple simulation:**
```python
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

**Create world-environment simulation:**
```python
# Setup agent in world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run episode
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**Enable learning capabilities:**
```python
# Enable all learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections  
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

**Advanced Features:**
```python
# Configure brain parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.5f)

# Save/load brain state
brain.save("my_brain_checkpoint.bin")
# ... later
brain.load("my_brain_checkpoint.bin")

# Get detailed statistics
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

## What NLM IS NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system
- A biologically realistic model of the human brain

## What NLM IS:
- A neural system that learns from experience
- A brain-inspired architecture with spiking neurons
- A system that develops and adapts over time
- A substrate where cognition emerges from neural dynamics
- A research platform for studying developmental AI

## Advanced Usage Patterns

### 1. Custom World Creation
```python
import pynlm

# Extend SimpleWorld for custom environments
class CustomWorld(pynlm.SimpleWorld):
    def __init__(self):
        super().__init__()
        self.resources = {}
        self.hazards = {}
    
    def addResource(self, x, y, value):
        self.resources[(x, y)] = value
    
    def getNearbyResources(self, x, y, radius):
        nearby = {}
        for (rx, ry), val in self.resources.items():
            if abs(rx - x) < radius and abs(ry - y) < radius:
                nearby[(rx, ry)] = val
        return nearby
```

### 2. Custom Experiment Framework
```python
import pynlm
from abc import ABC, abstractmethod

class CustomExperiment(ABC):
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.metrics = {}
    
    @abstractmethod
    def setup(self):
        """Initialize experiment state"""
        pass
    
    @abstractmethod
    def runEpisode(self, episode):
        """Run a single episode"""
        pass
    
    def run(self, numEpisodes):
        """Run complete experiment"""
        self.setup()
        for episode in range(numEpisodes):
            self.runEpisode(episode)
        return self.metrics
```

### 3. Neural Network Analysis
```python
import pynlm
import numpy as np

def analyzeBrainConnectivity(brain):
    """Analyze neural connectivity patterns"""
    regions = brain.getRegions()
    
    # Get all neurons and their connections
    all_neuron_connections = {}
    for region in regions:
        for pop in region->getPopulations():
            for neuron in pop->getNeurons():
                # Analyze synaptic connections
                connections = []
                # Implementation depends on NLM's internal APIs
                all_neuron_connections[neuron->getId()] = connections
    
    # Compute graph metrics
    return {
        'total_neurons': len(all_neuron_connections),
        'avg_connections_per_neuron': np.mean([len(c) for c in all_neuron_connections.values()]),
        'connection_distribution': np.histogram([len(c) for c in all_neuron_connections.values()], bins=20)
    }
```

## Development Best Practices

### 1. Configuration Management
```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()

# Set various parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05f)
config.set("plasticity.stdp.learning_rate", 0.001f)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0f)
config.set("neuromod.curiosity.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### 2. Error Handling and Validation
```python
import pynlm

def safeBrainStep(brain, step, time):
    """Safely perform brain step with error handling"""
    try:
        # Validate input
        if step < 0:
            raise ValueError("Step number must be non-negative")
        if time < 0:
            raise ValueError("Time must be non-negative")
        
        # Perform step
        brain.step(step, time)
        return True, f"Step {step} completed successfully"
    except Exception as e:
        return False, f"Error at step {step}: {str(e)}"
```

### 3. Performance Monitoring
```python
import time
import pynlm

class PerformanceMonitor:
    def __init__(self):
        self.timings = []
        self.memory_usage = []
    
    def measureStep(self, brain, step):
        """Measure performance of a single brain step"""
        start_time = time.time()
        
        # Execute step
        brain.step(step)
        
        end_time = time.time()
        self.timings.append(end_time - start_time)
        
        return {
            'step': step,
            'time_ms': (end_time - start_time) * 1000,
            'neurons_firing': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate()
        }
    
    def getStatistics(self):
        """Get performance statistics"""
        if not self.timings:
            return {}
        
        return {
            'avg_time_ms': np.mean(self.timings) * 1000,
            'min_time_ms': np.min(self.timings) * 1000,
            'max_time_ms': np.max(self.timings) * 1000,
            'std_time_ms': np.std(self.timings) * 1000,
            'steps_per_second': 1.0 / np.mean(self.timings)
        }
```

## Troubleshooting

### Common Issues and Solutions

**Q: "My brain isn't doing anything - all neurons are inactive"**
- **Solution**: Call `brain.initialize()` before starting simulation
- **Also**: Check that you're injecting sufficient sensory input
- **Debug**: Add print statements like `brain.getFiringNeuronCount()`

**Q: "Segmentation fault or crash"**
- **Solution**: Ensure `initialize()` is called before `step()`
- **Also**: Check configuration values (especially neuron_count)
- **Debug**: Run with smaller neuron counts first

**Q: "Python import fails - no module named 'pynlm'"**
- **Solution**: Install with `pip install --force-reinstall .`
- **Also**: Ensure build dependencies are installed
- **Debug**: Check build logs if using development install

**Q: "Learning doesn't seem to happen"**
- **Solution**: Enable learning subsystems with `agent.enableRewardModulation(True)`
- **Also**: Check reward values and prediction errors
- **Debug**: Add logging with `brain.logStatus()`

### Advanced Debugging Tips

```python
# Enable detailed logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Create custom logger for brain operations
brain_logger = logging.getLogger('brain')
brain_logger.setLevel(logging.DEBUG)

# Add console handler
console_handler = logging.StreamHandler()
console_handler.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))
brain_logger.addHandler(console_handler)
```

## Frequently Asked Questions

### Q: How many neurons should I start with?
**A**: For testing: 100-500 neurons. For research: 1000-10000 neurons.
Larger numbers require more memory and CPU but provide richer dynamics.

### Q: What's the difference between Python and C++ APIs?
**A**: Python bindings provide easy scripting access, while C++ API offers more control and performance.
Use Python for rapid prototyping and C++ for performance-critical applications.

### Q: How do I save and load brain states?
**A**: Use `brain.save("filename.bin")` and `brain.load("filename.bin")`.
Saved states include all synaptic weights and neural states.

### Q: Can I run multiple brains concurrently?
**A**: Yes, create multiple `Brain` objects with different configurations.
Each brain maintains its own independent neural state.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── python/                 # Python bindings
│   ├── bindings.cpp       # C++ extension
│   └── CMakeLists.txt     # Build configuration
├── src/                    # C++ source code
│   ├── brain/             # Core neural components
│   ├── agent/             # Agent interface
│   ├── world/             # Environment simulation
│   ├── experiments/       # Research experiments
│   └── ...                # Other subsystems
├── configs/                # Configuration files
├── tests/                  # Unit tests
├── docs/                   # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   └── ...                # More documentation
└── examples/               # Example applications
```

## Performance Recommendations

### For Research (Phase 4+ Experiments)
```bash
# Build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SIMD=ON -DENABLE_MULTITHREADING=ON

# Configure for research
neuron_count = 5000-20000
connection_probability = 0.05-0.2
enable_event_driven = true
```

### For Real-time Applications
```bash
# Build with real-time optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DTHREAD_COUNT=4 -DENABLE_SPARSE_CONNECTIVITY=ON

# Configure for real-time
neuron_count = 100-1000
simulation_timestep = 0.001-0.01
enable_structural_plasticity = false
```

### For Development and Testing
```bash
# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Smaller scale for faster iteration
neuron_count = 100
max_steps = 1000
```

## Getting Help

1. **Documentation**: Read `docs/ARCHITECTURE.md` for technical details
2. **Examples**: Look in `examples/` directory for application examples
3. **Community**: Join the NLM discussion forums for questions and collaboration
4. **Issues**: Report bugs and feature requests on GitHub

## License

MIT License - See `LICENSE` file for details.

## Authors

Research project - See docs for scientific background and contributions.

---

**Ready to explore the brain? Start by running `./nlm_phase2_demo` to see basic neural computation in action!**