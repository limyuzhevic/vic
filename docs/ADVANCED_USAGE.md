# NLM Advanced Usage Guide

This document provides advanced usage instructions for NLM (熙然), focusing on:
- Configuration management
- Performance optimization
- Experiment customization
- Advanced Python features

## Table of Contents

1. [Advanced Configuration](#advanced-configuration)
2. [Performance Optimization](#performance-optimization)
3. [Experiment Customization](#experiment-customization)
4. [Python API Advanced Features](#python-api-advanced-features)
5. [Checkpoint Management](#checkpoint-management)
6. [Debugging and Monitoring](#debugging-and-monitoring)
7. [Custom Development](#custom-development)

## Advanced Configuration

### Configuration Templates

NLM provides several predefined configuration templates for different use cases:

#### SimpleNetwork
- **Purpose**: Basic neural network testing
- **Neurons**: 1000
- **Regions**: 2
- **Best for**: Initial testing and validation

#### HighPerformance
- **Purpose**: Maximum performance optimization
- **Neurons**: 5000
- **Regions**: 4
- **Best for**: Large-scale simulations

#### LearningFocused
- **Purpose**: Enhanced learning capabilities
- **Neurons**: 2000
- **Features**: Novelty detection, curiosity, advanced plasticity

#### Developmental
- **Purpose**: Lifespan development studies
- **Neurons**: 3000
- **Features**: Age-dependent plasticity, developmental stages

#### Testing
- **Purpose**: Unit and integration testing
- **Neurons**: 100
- **Features**: Checkpointing, replay capabilities

#### Research
- **Purpose**: Scientific research and experimentation
- **Neurons**: 10000
- **Features**: All systems enabled, reproduction mode

### Configuration File Formats

#### JSON Configuration
```json
{
    "name": "MyBrain",
    "neuron_count": 2000,
    "plasticity_learning_rate": 0.02,
    "stdp_ltp_weight": 0.02
}
```

#### YAML Configuration
```yaml
name: MyBrain
neuron_count: 2000
plasticity_learning_rate: 0.02
stdp_ltp_weight: 0.02
```

#### Simple Format
```
# Simple configuration file
neuron_count = 1000
region_count = 2
connection_probability = 0.1
```

### Configuration Commands

The NLM CLI provides advanced configuration management:

```bash
# Show current configuration
nlm config show

# Set configuration value
nlm config set neuron_count 5000

# Get configuration value
nlm config get neuron_count

# Load from JSON file
nlm config load config.json

# Save configuration with comments
nlm config save my_config.cfg

# Validate configuration
nlm config validate
```

## Performance Optimization

### Performance Profiling

Use the benchmarking tools to analyze performance:

```bash
# Run performance benchmark
nlm benchmark --detailed --output benchmark.json

# Profile specific operations
nlm profile --operation plasticity --output profile.txt

# Compare different configurations
nlm benchmark --compare config1.json config2.json
```

### Optimization Tips

1. **Memory Configuration**:
   - Use HighPerformance template for large networks
   - Enable SIMD vectorization
   - Adjust memory pool sizes

2. **Plasticity Optimization**:
   - Adjust STDP time constants
   - Configure structural plasticity rates
   - Enable development stages

3. **Simulation Parameters**:
   - Reduce timestep for higher precision (but slower)
   - Increase connection probability for denser networks
   - Configure logging level appropriately

### Performance Monitoring

```python
import pynlm

# Create brain with monitoring
brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()

# Start monitoring
monitor = brain.get_monitor()

# Run simulation while monitoring
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        stats = monitor.get_stats()
        print(f"Step {step}: {stats['avg_firing_rate']} Hz")
```

## Experiment Customization

### Custom Experiment Framework

Create custom experiments with the NLM experiment system:

```python
import pynlm

class CustomExperiment:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.metrics = {}
    
    def run_episode(self, steps=1000):
        """Run a single episode of the experiment"""
        for step in range(steps):
            # Get observation
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Record metrics
            self.record_metrics(step)
        
        return self.get_episode_results()
    
    def record_metrics(self, step):
        """Record experiment metrics"""
        self.metrics['spike_count'] = self.brain.getTotalSpikeCount()
        self.metrics['firing_rate'] = self.brain.getAverageFiringRate()
        self.metrics['reward'] = self.world.getLastReward()
        self.metrics['curiosity'] = self.agent.getCuriosityLevel()
    
    def get_episode_results(self):
        """Get episode results as dictionary"""
        return {
            'total_spikes': self.metrics.get('spike_count', 0),
            'average_firing_rate': self.metrics.get('firing_rate', 0),
            'total_reward': sum(self.metrics.get('reward', 0) for _ in range(self.metrics.get('episodes', 0))),
            'final_curiosity': self.metrics.get('curiosity', 0),
            'steps': self.metrics.get('steps', 0)
        }
```

### Experiment Templates

```bash
# Create experiment template
nlm experiment create --template learning --output learning_exp.json

# Run experiment with custom parameters
nlm experiment run learning_exp.json --steps 5000 --neurons 3000

# Compare experiments
nlm experiment compare exp1.json exp2.json --metrics performance
```

## Python API Advanced Features

### Context Managers

```python
import pynlm

# Using context manager for automatic cleanup
with pynlm.SimulationContext(brain, world, agent, 1000) as sim:
    results = sim.run()
    print(f"Simulation completed: {results}")

# Context manager with custom step function
def custom_step(step, time):
    # Custom step logic here
    pass

with pynlm.SimulationContext(brain, world, agent, 1000) as sim:
    results = sim.run_with_step(custom_step)
```

### Enhanced Brain Interface

```python
import pynlm

# Create enhanced brain
config = pynlm.create_default_config()
config.set("neuron_count", 2000)
config.set("plasticity_learning_rate", 0.02)

brain = pynlm.createBrain(config)
brain.initialize()

# Get detailed statistics
stats = brain.get_detailed_stats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']} Hz")
print(f"Dopa\u0301mine level: {stats.get('dopamine_level', 0)}")

# Save and load checkpoints
brain.save_checkpoint("checkpoint1.bin")
brain.load_checkpoint("checkpoint1.bin")
```

### Advanced Agent Features

```python
import pynlm

# Create agent with enhanced capabilities
agent = pynlm.createAgentBrain(brain)

# Setup learning environment
agent.setup_learning(
    enableRewardModulation=True,
    enableCuriosity=True,
    enableDevelopment=True,
    enableStructuralPlasticity=True
)

# Process input and get action
percept = world.getSensoryPercept()
result = agent.process_and_step(percept, step, time)
print(f"Action: {result['action']}")
print(f"Curiosity: {result['curiosity']}")
print(f"Prediction Error: {result['prediction_error']}")

# Get advanced metrics
metrics = agent.get_advanced_metrics()
print(f"Neuromodulation: {metrics['neuromodulation']}")
print(f"Developmental Stage: {metrics['developmental_stage']}")
```

### Helper Functions

```python
import pynlm

# Create custom configuration
config = pynlm.create_default_config()
config.set("neuron_count", 3000)
config.set("plasticity_learning_rate", 0.03)

# Create simple simulation
brain, world, agent = pynlm.create_simple_simulation(
    neuron_count=2000,
    region_count=2
)

# Setup learning
brain.setup_learning(enableLearning=True, enableNovelty=True)

# Run complete simulation
results = pynlm.run_complete_simulation(
    steps=5000,
    enableLearning=True
)

print(f"Simulation Results:")
print(f"  Total Spikes: {results['final_stats']['total_spikes']}")
print(f"  Average Firing Rate: {results['final_stats']['average_firing_rate']}")
print(f"  Total Reward: {results['final_stats']['total_reward']}")
```

## Checkpoint Management

### Checkpoint Operations

```bash
# Create checkpoint
nlm checkpoint save --path ./checkpoint.bin

# Load checkpoint
nlm checkpoint load ./checkpoint.bin

# List checkpoints
nlm checkpoint list

# Compare checkpoints
nlm checkpoint compare checkpoint1.bin checkpoint2.bin

# Delete checkpoint
nlm checkpoint delete ./checkpoint.bin
```

### Checkpoint Commands

```python
import pynlm

# Save brain state
brain = pynlm.createBrain(config)
brain.initialize()
brain.save_checkpoint("my_checkpoint.bin")

# Load brain state
brain.load_checkpoint("my_checkpoint.bin")

# Verify checkpoint
if brain.verify_checkpoint("my_checkpoint.bin"):
    print("Checkpoint is valid")
else:
    print("Checkpoint is corrupted")

# Manage checkpoints
pynlm.manage_checkpoints({
    'max_checkpoints': 5,
    'checkpoint_path': './checkpoints/',
    'auto_cleanup': True
})
```

## Debugging and Monitoring

### Debug Tools

```bash
# Enable debug logging
nlm run --verbose --log-level DEBUG

# Start debug server
nlm debug start --port 8080

# Connect to debug server
nlm debug connect localhost:8080

# Profile specific operations
nlm profile --operation all --output profile.json

# Monitor performance in real-time
nlm monitor start --interval 1.0
```

### Debugging Scripts

```python
import pynlm

# Create debug monitor
monitor = pynlm.DebugMonitor()

# Start monitoring
monitor.start()

# Run simulation with debugging
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        debug_info = monitor.get_debug_info()
        print(f"Step {step}:")
        print(f"  Active neurons: {debug_info['active_neurons']}")
        print(f"  Spike rate: {debug_info['spike_rate']}")
        print(f"  Memory usage: {debug_info['memory_usage']}")

monitor.stop()
```

## Custom Development

### Extending NLM

#### Creating Custom Environments

```cpp
// Custom environment implementation
class CustomEnvironment : public Environment {
public:
    CustomEnvironment() : Environment() {}
    
    std::unique_ptr<SensoryInput> reset() override {
        // Initialize environment
        return std::make_unique<SensoryInput>();
    }
    
    std::unique_ptr<SensoryInput> step(const Action& action) override {
        // Process action and return observation
        return std::make_unique<SensoryInput>();
    }
    
    // Implement other pure virtual methods...
};
```

#### Custom Plasticity Rules

```cpp
// Custom plasticity rule implementation
class CustomPlasticityRule : public PlasticityRule {
public:
    void applyPlasticity(Synapse* synapse, float preSpikeTime, float postSpikeTime) override {
        // Custom plasticity computation
        float delta = computeCustomDelta(preSpikeTime, postSpikeTime);
        synapse->addToWeight(delta);
    }
    
    // Implement other required methods...
};
```

#### Plugin Architecture

```cpp
// Plugin interface
class NLMSPlugin {
public:
    virtual ~NLMSPlugin() = default;
    virtual std::string getName() const = 0;
    virtual void initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool canHandle(const std::string& type) const = 0;
    virtual std::unique_ptr<void> create(const std::string& type) = 0;
};

// Plugin factory
extern "C" NLMSPlugin* createPlugin() {
    return new CustomPlugin();
}
```

### Building Custom Components

```bash
# Build with custom plugins
mkdir build
cd build
cmake .. -DPLUGIN_DIR=./plugins -DEXPERIMENT_DIR=./experiments
make -j4

# Install plugins
make install
```

## Migration Guide

### From Phase 2 to Phase 6

1. **Configuration Updates**:
   - Phase 2 config files may need format updates
   - Use `nlm config validate` to check compatibility

2. **API Changes**:
   - New enhanced API available in pynlm_enhanced
   - Old API still supported for backward compatibility

3. **Experiment Migration**:
   - Update experiment scripts to use new templates
   - Use enhanced experiment framework

4. **Performance Considerations**:
   - Larger networks may need more memory
   - Enable performance optimizations

### Troubleshooting

#### Common Issues

**Issue**: Brain initialization fails
**Solution**:
```python
# Check configuration
config = pynlm.create_default_config()
config.set("neuron_count", 100)  # Start small
brain = pynlm.createBrain(config)
brain.initialize()  # Should work now
```

**Issue**: Python bindings not found
**Solution**:
```bash
# Install enhanced bindings
pip install ./build/pynlm_enhanced.whl

# Or build from source
cd python
cmake .
make
pip install .
```

**Issue**: Performance is slow
**Solution**:
```python
# Use performance-optimized configuration
config = pynlm.create_default_config()
config.set("neuron_count", 5000)
config.set("plasticity_learning_rate", 0.005)

# Enable optimization features
brain = pynlm.createBrain(config)
brain.initialize()
brain.enable_optimization("simd", True)
brain.enable_optimization("memory_pool", True)
```

## Conclusion

This advanced usage guide covers the key features of NLM Phase 6. By leveraging these advanced capabilities, you can:

1. **Configure** complex neural networks with JSON/YAML
2. **Optimize** performance for different use cases
3. **Customize** experiments for specific research questions
4. **Leverage** enhanced Python APIs for easier development
5. **Manage** checkpoints for reproducible research
6. **Debug** and monitor system performance
7. **Extend** NLM with custom components

The enhanced NLM platform provides everything needed for advanced neural simulation research and development.

---

*Last updated: $(date)*
*Version: 2.0.0-enhanced*