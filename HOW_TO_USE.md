## Part 8: Advanced Examples

### Advanced Configuration

```python
import pynlm

# Create a custom configuration with advanced settings
config = pynlm.createDefaultConfig()

# Configure brain parameters for specific use case
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.12)
config.set("brain.v_thresh", -45.0)  // Higher threshold for control
config.set("brain.tau_mem", 25.0)   // Slower dynamics for stability

# Configure learning mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.003)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.002)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save configuration
config.saveToFile("advanced_config.json")
```

### Advanced World Setup

```python
import pynlm

# Create world with custom terrain
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=10,
    visionHeight=10
)

# Add obstacles and resources
for i in range(5):
    world.addObject(pynlm.WorldObject(
        x=5 + i * 5,
        y=15,
        type=pynlm.WorldObjectType.Resource,
        value=2.0
    ))

for i in range(3):
    world.addObject(pynlm.WorldObject(
        x=20 + i * 5,
        y=20,
        type=pynlm.WorldObjectType.Hazard,
        value=-1.5
    ))

# Configure advanced vision
world.configureVision(
    fov=120.0,
    blurAmount=0.2,
    noiseLevel=0.1,
    contrast=1.5,
    brightness=1.0
)
```

### Advanced Agent Configuration

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run advanced simulation with custom metrics
import time
start_time = time.time()

for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print periodic statistics
    if step % 100 == 0:
        print(f"Step {step}: Firing rate={brain.getAverageFiringRate():.2f}, " +
              f"Curiosity={agent.getCuriosityLevel():.2f}, " +
              f"Novelty={agent.getNoveltyLevel():.2f}")

print(f"Simulation completed in {time.time() - start_time:.2f} seconds")
```

### Custom Learning Metrics

```python
import pynlm

class AdvancedLearningMetrics:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.metrics_log = []
    
    def calculatePlasticityIndex(self):
        """Calculate overall network plasticity."""
        # Combine multiple plasticity indicators
        stdp_factor = 1.0  # Based on STDP activity
        hebbian_factor = 1.0  # Based on Hebbian activity
        structural_factor = 1.0  # Based on structural changes
        
        return (stdp_factor + hebbian_factor + structural_factor) / 3.0
    
    def calculateNetworkStability(self):
        """Calculate network stability."""
        # Check for oscillations and instability
        firing_rate = self.brain.getAverageFiringRate()
        stability = max(0.0, 1.0 - firing_rate / 100.0)
        return stability
    
    def calculateLearningEfficiency(self):
        """Calculate how efficiently the network is learning."""
        # Compare expected vs actual learning
        # This would typically involve tracking changes in behavior
        novelty = self.agent.getNoveltyLevel()
        curiosity = self.agent.getCuriosityLevel()
        
        return (novelty + curiosity) / 2.0
    
    def computeAllMetrics(self):
        """Compute all metrics and log them."""
        metrics = {
            'plasticity': self.calculatePlasticityIndex(),
            'stability': self.calculateNetworkStability(),
            'learning_efficiency': self.calculateLearningEfficiency(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel()
        }
        
        self.metrics_log.append(metrics)
        return metrics
    
    def printMetrics(self, step):
        """Print metrics in a formatted way."""
        metrics = self.computeAllMetrics()
        print(f"\n=== Step {step} Metrics ===")
        for key, value in metrics.items():
            print(f"{key:20s}: {value:10.4f}")

# Usage in simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

metrics = AdvancedLearningMetrics(brain, world, agent)

for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        metrics.printMetrics(step)
```

## Part 9: Configuration Options

### Configuration File Examples

#### Basic Configuration

```ini
# configs/basic.cfg
# Basic brain configuration
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05
brain.v_thresh = -50.0
brain.v_rest = -65.0
brain.tau_mem = 20.0
brain.tau_ref = 2.0
```

#### Advanced Configuration

```json
// configs/advanced.json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.12,
        "connection_probability": 0.08,
        "v_thresh": -45.0,
        "v_rest": -65.0,
        "tau_mem": 25.0,
        "tau_ref": 3.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.003,
            "tau_plus": 30.0,
            "tau_minus": 35.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.002
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.5
        },
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.3
        },
        "novelty": {
            "enable": true,
            "sensitivity": 0.8
        }
    },
    "development": {
        "enable": true,
        "stage_transition_speed": 30.0,
        "plasticity_factor": 0.8
    }
}
```

#### Command Line Arguments

```bash
# Run NLM with custom configuration
./nlm --config my_config.json --random_seed 123 --neuron_count 1500

# Override specific parameters
./nlm --config base.json --brain.synapse_density 0.15 --plasticity.stdp.learning_rate 0.005

# Run with command line parameters only
./nlm --neuron_count 500 --region_count 2 --simulation_timestep 0.001
```

## Part 10: Troubleshooting

### Common Issues and Solutions

#### Issue: Brain Doesn't Initialize

**Problem**: Brain fails to initialize with error messages or crashes.

**Solutions**:

1. **Check Configuration Validity**:
   ```bash
   # Verify configuration file syntax
   # For JSON: use jq to validate
   jq . configs/your_config.json
   
   # For simple format: check for valid key=value pairs
   grep -E '^[a-zA-Z_][a-zA-Z0-9_]*\s*=\s*[^=]+$' configs/your_config.cfg
   ```

2. **Check System Requirements**:
   ```bash
   # Verify available memory and CPU
   free -h
   nproc
   ```

3. **Use Minimal Configuration**:
   ```python
   import pynlm
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 100)  # Start small
   config.set("brain.region_count", 1)
   ```

#### Issue: Simulation Runs Too Slowly

**Problem**: Simulation performance is poor.

**Solutions**:

1. **Optimize Configuration**:
   ```python
   # Use smaller network for testing
   config.set("brain.neuron_count", 500)
   config.set("brain.synapse_density", 0.05)
   ```

2. **Enable Performance Optimizations**:
   ```python
   # Configure for performance
   config.set("plasticity.structural.enable", False)  // Disable structural plasticity
   config.set("development.enable", False)            // Disable developmental processes
   ```

3. **Profile and Tune**:
   ```bash
   # Use performance profiling tools
   gprof ./nlm program.prof
   ```

#### Issue: Learning Doesn't Occur

**Problem**: Neural network shows no learning or weight changes.

**Solutions**:

1. **Verify Plasticity is Enabled**:
   ```python
   # Check plasticity configuration
   if config.getOr<bool>("plasticity.stdp.enable", true) {
       std::cout << "STDP is enabled" << std::endl;
   }
   if config.getOr<bool>("plasticity.hebbian.enable", true) {
       std::cout << "Hebbian learning is enabled" << std::endl;
   }
   ```

2. **Check Neuron Activity**:
   ```python
   // Ensure neurons are firing
   if brain.getFiringNeuronCount() < 10) {
       std::cout << "Too few firing neurons. Inject more current." << std::endl;
       brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
   }
   ```

3. **Verify Synaptic Connections**:
   ```python
   if brain.getTotalSynapseCount() < 10) {
       std::cout << "Too few synapses. Increase connection probability." << std::endl;
   }
   ```

#### Issue: Memory Usage is High

**Problem**: Simulation consumes excessive memory.

**Solutions**:

1. **Optimize Configuration**:
   ```python
   config.set("working_memory.capacity", 200)     // Limit working memory
   config.set("episodic_memory.max_episodes", 100) // Limit episodic memory
   ```

2. **Use More Efficient Structures**:
   ```python
   // Use more efficient data structures
   // Consider disabling certain features
   agent.enableCuriosity(false)    // Disable curiosity-driven exploration
   agent.enableDevelopment(false)   // Disable developmental processes
   ```

3. **Monitor Memory Usage**:
   ```bash
   # Use system monitoring tools
   top -p $(pidof nlm)
   # Or monitor within the program
   ```

### Advanced Debugging

```python
import pynlm
import logging

# Configure detailed logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_debug.log'),
        logging.StreamHandler()
    ]
)

logger = logging.getLogger('nlm')

# Add custom logging for specific components
class DebugBrain(pynlm.Brain):
    def step(self, step_number, time=0.0):
        logger.debug(f"Brain step {step_number}, time {time}")
        try:
            super().step(step_number, time);
            logger.debug(f"Brain step {step_number} completed successfully");
        } catch (const std::exception& e) {
            logger.error(f"Error in brain step {step_number}: {e.what()}");
            throw;
        }

# Usage
debug_brain = DebugBrain(pynlm.createDefaultConfig())
debug_brain.initialize()

# Run simulation with enhanced debugging
for step in range(100):
    debug_brain.step(step)
    logger.debug(f"Current state: {debug_brain.logStatus()}")
```

## Part 11: Further Reading

### Architecture Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ROADMAP.md](docs/ROADMAP.md) - Project roadmap and future plans
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations and research directions

### Research Papers and References

The NLM project draws from multiple research areas:

1. **Neuromodulation**: References to dopamine, curiosity, and novelty detection research
2. **Memory Systems**: Working memory, episodic memory, and associative memory models
3. **Prediction and Planning**: Predictive coding and neural planning algorithms
4. **Development**: Developmental psychology and neurodevelopmental principles
5. **Plasticity**: Hebbian learning, STDP, and structural plasticity research

### Community and Support

For questions, discussion, and support:

1. **GitHub Repository**: https://github.com/nlm-project/nlm
2. **Documentation**: https://nlm.readthedocs.io
3. **Issues and Feature Requests**: Submit issues on GitHub
4. **Discussions**: Join the community discussion

### Contributing

NLM is an open-source research project. Contributions are welcome:

1. **Code Contributions**: Submit pull requests with feature implementations
2. **Bug Reports**: File issues with detailed reproduction steps
3. **Documentation**: Improve documentation and add examples
4. **Research**: Submit papers based on NLM research findings
5. **Testing**: Add new test cases and improve test coverage

### License

MIT License - See LICENSE file for details.

All rights reserved.