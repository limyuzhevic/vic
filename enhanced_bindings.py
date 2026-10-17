# Enhanced Python Bindings for NLM

## What's New

This improved version of the NLM Python bindings adds:

### 1. Enhanced Configuration System
- Configuration templates for common use cases
- Configuration validation
- Configuration comparison and merging
- Parameter sweeps for experiments

### 2. Advanced Brain Features
- Enhanced brain manipulation utilities
- Neural population management
- Regional connectivity management
- Advanced plasticity control

### 3. Rich Agent System
- Enhanced agent behaviors (curiosity-driven exploration, reward shaping)
- Agent training capabilities
- Reinforcement learning integration
- Multi-agent support

### 4. Advanced World Environments
- Multiple world types (beyond SimpleWorld)
- Complex object interactions
- Dynamic environment generation
- Environmental scripting

### 5. Visualization and Debugging
- Built-in visualization tools
- Performance monitoring
- Debug output utilities
- State inspection tools

### 6. Simulation Control
- Advanced simulation control
- Parameter sweeps
- Experiment scripting
- Checkpoint management with compression
- Rollback capabilities

### 7. Resource Management
- Context managers for automatic cleanup
- Memory management utilities
- Resource monitoring
- Batch processing methods

### 8. Performance Tools
- Performance monitoring
- Statistical analysis
- Bottleneck detection
- Optimization suggestions

## Installation and Setup

### Basic Installation (unchanged)
```bash
pip install pynlm
```

### Development Installation
```bash
pip install -e .
```

### Building from Source
```bash
mkdir build
cd build
cmake ..
make -j4
```

## Enhanced Configuration

### Configuration Templates
```python
import pynlm

# Create configuration from template
config = pynlm.createConfigTemplate("simple_brain")
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create configuration for agent training
train_config = pynlm.createConfigTemplate("agent_training")
train_config.set("agent.reward_shaping", 0.1)
train_config.set("agent.curiosity.enabled", True)
train_config.set("world.environment.type", "dynamic")
```

### Configuration Validation
```python
# Validate configuration
is_valid, errors = config.validate()
if not is_valid:
    print("Configuration errors:", errors)

# Get configuration suggestions
suggestions = config.getSuggestions()
for suggestion in suggestions:
    print(f"Suggestion: {suggestion}")
```

### Parameter Sweeps
```python
# Create parameter sweep
from pynlm import ParameterSweep

sweep = ParameterSweep("brain.neuron_count", [100, 500, 1000, 2000])
sweep.set_range("plasticity.stdp.learning_rate", 0.001, 0.1)

# Run sweep
results = sweep.run_experiment(100)  # 100 steps per parameter

# Get best parameters
best_params = sweep.getBestParameters()
```

## Enhanced Brain Features

### Neural Population Management
```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()

# Create neural populations
region_id = brain.addRegion("cortex")

# Create excitatory and inhibitory populations
sensory_pop = brain.createPopulation("sensory", pynlm.NeuronType.Sensory, 100)
motor_pop = brain.createPopulation("motor", pynlm.NeuronType.Motor, 50)

# Set connectivity
brain.connectRegions(region_id, "cortex", "motor", strength=0.5)

# Get population statistics
stats = brain.getPopulationStats("sensory")
print(f"Sensory population: {stats['neuron_count']} neurons")
```

### Advanced Plasticity Control
```python
# Enable and configure plasticity
brain.enablePlasticity("stdp", True)
brain.setPlasticityParams("stdp", {
    "learning_rate": 0.001,
    "tau_plus": 20.0,
    "tau_minus": 20.0
})

# Get current plasticity state
plasticity_state = brain.getPlasticityState()
print(f"STDP enabled: {plasticity_state['stdp']['enabled']}")

# Apply custom plasticity rules
brain.addCustomPlasticityRule("my_rule", 
    condition=lambda synapse: synapse.weight > 0.5,
    update=lambda synapse: synapse.weight *= 0.99)
```

## Enhanced Agent System

### Advanced Agent Behaviors
```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain()
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Configure advanced behaviors
agent.enableCuriosity(True, curiosity_strength=2.0)
agent.enableRewardModulation(True, reward_decay=0.95)
agent.enableStructuralPlasticity(True, growth_rate=0.001)

# Set exploration strategy
agent.setExplorationStrategy("epsilon_greedy", epsilon=0.1)

# Configure curiosity-driven exploration
agent.configureCuriosity(
    novelty_threshold=0.5,
    exploration_bonus=1.0,
    learning_rate=0.01
)

# Get agent statistics
stats = agent.getStatistics()
print(f"Curiosity level: {stats['curiosity_level']:.3f}")
print(f"Novelty level: {stats['novelty_level']:.3f}")
```

### Reinforcement Learning Integration
```python
# Agent-RL integration
class RLAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.q_table = {}
        self.epsilon = 0.1
        
    def getState(self):
        # Convert world state to RL state
        percept = self.world.getSensoryPercept()
        return self.agent.processSensoryInput(percept)
        
    def chooseAction(self, state):
        # Epsilon-greedy action selection
        if np.random.random() < self.epsilon:
            return np.random.choice(self.getAvailableActions())
        return self.getBestAction(state)
        
    def learn(self, state, action, reward, next_state):
        # Q-learning update
        key = (state, action)
        if key not in self.q_table:
            self.q_table[key] = 0.0
            
        best_next = self.getBestAction(next_state)
        target = reward + 0.99 * self.q_table.get((next_state, best_next), 0.0)
        
        self.q_table[key] += 0.01 * (target - self.q_table[key])
        
        # Apply reward modulation to brain
        self.agent.applyRewardModulation(reward, 0.0)
```

## Advanced World Environments

### Dynamic World Environment
```python
import pynlm

# Create dynamic world
world = pynlm.createDynamicWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)

# Add moving objects
world.addMovingObject(5.0, 5.0, pynlm.WorldObjectType.Resource, velocity_x=0.5)
world.addMovingObject(25.0, 25.0, pynlm.WorldObjectType.Hazard, velocity_y=-0.3)

# Configure environmental factors
world.setEnvironmentalFactor("gravity", 9.8)
world.setEnvironmentalFactor("friction", 0.1)
world.setEnvironmentalFactor("wind", (1.0, 0.5))

# Create dynamic objects
for i in range(10):
    x = np.random.uniform(5, 25)
    y = np.random.uniform(5, 25)
    obj_type = np.random.choice([
        pynlm.WorldObjectType.Resource,
        pynlm.WorldObjectType.Hazard,
        pynlm.WorldObjectType.Wall
    ])
    world.addDynamicObject(x, y, obj_type, radius=0.3, dynamic=True)

# Get world statistics
stats = world.getWorldStats()
print(f"Objects in world: {stats['object_count']}")
print(f"Dynamic objects: {stats['dynamic_object_count']}")
```

### Environmental Scripting
```python
# Script world evolution
def createForest(world):
    """Create a forest environment with trees."""
    for i in range(20):
        x = np.random.uniform(5, 25)
        y = np.random.uniform(5, 25)
        world.addObject(pynlm.WorldObject(
            x, y, pynlm.WorldObjectType.Wall, 
            radius=0.8, value=0.0
        ))

def createCaveSystem(world):
    """Create cave system with hazardous areas."""
    for i in range(5):
        center_x = np.random.uniform(10, 20)
        center_y = np.random.uniform(10, 20)
        for angle in np.linspace(0, 2*np.pi, 20):
            x = center_x + np.cos(angle) * 3.0
            y = center_y + np.sin(angle) * 3.0
            world.addObject(pynlm.WorldObject(
                x, y, pynlm.WorldObjectType.Hazard,
                radius=0.5, value=-1.0
            ))

# Apply scripts
createForest(world)
createCaveSystem(world)
```

## Enhanced Visualization and Debugging

### Built-in Visualization
```python
import pynlm
import matplotlib.pyplot as plt

# Create simulation with visualization
brain = pynlm.createBrain()
brain.initialize()

# Enable visualization
brain.enableVisualization(True)
visualization = brain.getVisualization()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with visualization
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Get visualization data
    vis_data = visualization.getCurrentState()
    
    # Update visualization
    visualization.update(vis_data)
    
    # Save frames for movie
    if step % 10 == 0:
        visualization.saveFrame(f"frame_{step:04d}.png")

# Generate visualization report
report = visualization.generateReport()
print(f"Simulation completed with {len(report['frames'])} frames")

# Create animations
visualization.createAnimation(
    filename="simulation.gif",
    fps=10,
    duration=1000
)
```

### Performance Monitoring
```python
import pynlm
import time

# Create performance monitor
performance_monitor = pynlm.PerformanceMonitor()

# Create simulation
brain = pynlm.createBrain()
brain.initialize()

# Start monitoring
performance_monitor.startMonitoring()

# Run intensive simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    # Record performance at intervals
    if step % 100 == 0:
        performance_monitor.recordStep(step)

end_time = time.time()

# Stop monitoring
performance_monitor.stopMonitoring()

# Get performance report
report = performance_monitor.getReport()
print(f"Simulation time: {end_time - start_time:.2f} seconds")
print(f"Average steps per second: {report['avg_steps_per_second']:.0f}")
print(f"Memory usage: {report['memory_usage_mb']:.1f} MB")
print(f"CPU utilization: {report['cpu_utilization_percent']:.1f}%")

# Get bottleneck analysis
bottlenecks = performance_monitor.getBottlenecks()
for bottleneck in bottlenecks:
    print(f"Bottleneck: {bottleneck['area']} - Impact: {bottleneck['severity']:.2f}")
```

## Enhanced Simulation Control

### Advanced Simulation Control
```python
import pynlm

# Create simulation controller
controller = pynlm.SimulationController()

# Configure simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("world.timestep", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()

# Run controlled simulation
controller.runSimulation(
    brain=brain,
    num_steps=1000,
    checkpoint_interval=100,
    enable_visualization=True,
    on_step_callback=lambda step: print(f"Completed step {step}")
)

# Get simulation statistics
stats = controller.getStatistics()
print(f"Simulation completed: {stats['steps_completed']} steps")
print(f"Final firing rate: {stats['final_firing_rate']:.3f}")

# Save simulation state
controller.saveSimulation("simulation_state.npz")
```

### Parameter Sweeps
```python
import pynlm
import numpy as np

# Create parameter sweep controller
from pynlm import ParameterSweep

# Define parameter space
param_space = {
    "brain.neuron_count": [100, 500, 1000, 2000],
    "brain.synapse_density": [0.05, 0.1, 0.2],
    "brain.v_thresh": [-55, -60, -65],
    "plasticity.stdp.learning_rate": [0.001, 0.01, 0.1]
}

# Create sweep
sweep = ParameterSweep(param_space)

# Configure objective function
from pynlm import ObjectiveFunction

def learning_efficiency(objective_params):
    """Objective function: measure learning efficiency."""
    config = pynlm.createDefaultConfig()
    
    # Apply parameters
    for key, value in objective_params.items():
        config.set(key, value)
        
    # Run simulation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(200):
        brain.step(step)
        
    # Calculate efficiency metric
    final_spikes = brain.getTotalSpikeCount()
    efficiency = final_spikes / config.get("brain.neuron_count", 1000)
    
    return efficiency

# Run sweep
best_result = sweep.optimize(
    objective_function=learning_efficiency,
    max_evaluations=50,
    parallel=True
)

print(f"Best parameters: {best_result['parameters']}")
print(f"Best score: {best_result['score']:.4f}")
```

## Enhanced Resource Management

### Context Managers
```python
import pynlm
import numpy as np

# Use context manager for automatic resource cleanup
with pynlm.SimulationContext(config) as sim:
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get agent state
        state = agent.getState()
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Context manager automatically cleans up resources
print("Simulation completed and resources cleaned up")

# Batch processing with resource management
def run_multiple_simulations(configs, num_steps=100):
    """Run multiple simulations with resource management."""
    results = []
    
    with pynlm.BatchSimulationContext() as batch:
        for i, config in enumerate(configs):
            print(f"Running simulation {i+1}/{len(configs)}")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(num_steps):
                brain.step(step)
                
            results.append({
                'config': config,
                'final_spike_count': brain.getTotalSpikeCount(),
                'final_firing_rate': brain.getAverageFiringRate()
            })
            
            batch.recordProgress(i + 1, len(configs))
            
    return results
```

### Memory Management Utilities
```python
import pynlm
import numpy as np
import gc

# Create memory manager
memory_manager = pynlm.MemoryManager()

# Create large simulation
brain = pynlm.createBrain()
brain.initialize()

# Check memory usage before simulation
initial_memory = memory_manager.getMemoryUsage()
print(f"Initial memory usage: {initial_memory:.1f} MB")

# Run intensive simulation
for step in range(1000):
    brain.step(step)
    
    # Monitor memory
    if step % 100 == 0:
        current_memory = memory_manager.getMemoryUsage()
        print(f"Step {step}: {current_memory:.1f} MB")

# Force garbage collection
gc.collect()

# Get memory statistics
stats = memory_manager.getStatistics()
print(f"Peak memory usage: {stats['peak_memory_mb']:.1f} MB")
print(f"Average memory usage: {stats['average_memory_mb']:.1f} MB")

# Memory optimization suggestions
suggestions = memory_manager.getOptimizationSuggestions()
for suggestion in suggestions:
    print(f"Suggestion: {suggestion}")
```

## Enhanced Performance Tools

### Performance Monitoring
```python
import pynlm
import time
import numpy as np

# Create performance analyzer
analyzer = pynlm.PerformanceAnalyzer()

# Create simulation
brain = pynlm.createBrain()
brain.initialize()

# Configure performance monitoring
analyzer.configure(
    monitor_cpu=True,
    monitor_memory=True,
    monitor_network=True,
    sampling_rate=0.01
)

# Start analysis
analyzer.startAnalysis(brain)

# Run performance test
sim_start = time.time()
for step in range(1000):
    brain.step(step)
    
    # Record performance data
    if step % 10 == 0:
        analyzer.recordStep(step)

sim_end = time.time()

# Stop analysis
analyzer.stopAnalysis()

# Get performance report
report = analyzer.getReport()
print(f"Simulation completed in {sim_end - sim_start:.2f} seconds")
print(f"Performance score: {report['performance_score']:.3f}")
print(f"Bottlenecks: {len(report['bottlenecks'])}")

# Get optimization recommendations
recommendations = analyzer.getRecommendations()
for rec in recommendations:
    print(f"Recommendation: {rec['issue']} -> {rec['solution']}")
```

### Statistical Analysis
```python
import pynlm
import numpy as np
from scipy import stats

# Create statistical analyzer
stat_analyzer = pynlm.StatisticalAnalyzer()

# Collect simulation data
simulations = []
for i in range(10):
    brain = pynlm.createBrain()
    brain.initialize()
    
    spike_counts = []
    for step in range(100):
        brain.step(step)
        spike_counts.append(brain.getTotalSpikeCount())
        
    simulations.append({
        'simulation_id': i,
        'total_spikes': spike_counts[-1],
        'spike_counts': spike_counts,
        'mean_firing_rate': np.mean(spike_counts),
        'std_firing_rate': np.std(spike_counts)
    })

# Convert to DataFrame for analysis
sim_df = pynlm.convertToDataFrame(simulations)

# Perform statistical analysis
stat_results = stat_analyzer.analyzeDataFrame(sim_df)

print("Statistical Analysis Results:")
print(f"Mean total spikes: {stat_results['mean']['total_spikes']:.0f}")
print(f"Std total spikes: {stat_results['std']['total_spikes']:.0f}")
print(f"Skewness: {stat_results['skewness']['total_spikes']:.3f}")
print(f"Kurtosis: {stat_results['kurtosis']['total_spikes']:.3f}")

# Check for normality (for parametric tests)
normality_test = stat_results['normality_test']
print(f"Normality p-value: {normality_test['p_value']:.4f}")
print(f"Data is {'normal' if normality_test['is_normal'] else 'not normal'}")

# Perform hypothesis tests (if data is normal)
if normality_test['is_normal']:
    # Independent t-test between first and last 5 simulations
    first_half = sim_df['total_spikes'].iloc[:5]
    second_half = sim_df['total_spikes'].iloc[5:]
    
    t_stat, p_value = stats.ttest_ind(first_half, second_half)
    print(f"T-test p-value: {p_value:.4f}")
    
    # Correlation analysis
    correlation = sim_df['mean_firing_rate'].corr(sim_df['std_firing_rate'])
    print(f"Firing rate correlation: {correlation:.3f}")

# Get outlier analysis
outliers = stat_analyzer.detectOutliers(sim_df['total_spikes'])
print(f"Outliers detected: {len(outliers)}")
for outlier in outliers:
    print(f"  Simulation {outlier['simulation_id']}: {outlier['value']:.0f} (z-score: {outlier['z_score']:.2f})")
```

## Quick Start Examples

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create and run simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    
print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Complete Agent Simulation
```python
import pynlm
import numpy as np

# Create complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

### Example 3: Parameter Sweep
```python
import pynlm
from pynlm import ParameterSweep

# Define parameter space
param_space = {
    "brain.neuron_count": [500, 1000, 2000],
    "brain.synapse_density": [0.05, 0.1, 0.2]
}

# Create and run sweep
sweep = ParameterSweep(param_space)

# Run sweep with custom objective function
def objective_function(params):
    brain = pynlm.createBrain()
    
    # Apply parameters
    for key, value in params.items():
        brain.setConfigValue(key, value)
        
    # Run simulation
    brain.initialize()
    for step in range(100):
        brain.step(step)
        
    # Return performance metric
    return brain.getAverageFiringRate()

# Run sweep
best_result = sweep.run(
    objective_function=objective_function,
    max_evaluations=20
)

print(f"Best parameters: {best_result['parameters']}")
print(f"Best score: {best_result['score']:.4f}")
```

## Conclusion

These enhanced Python bindings provide:

1. **Better Developer Experience**: More Pythonic API with convenience methods and context managers
2. **Advanced Features**: Parameter sweeps, visualization, performance monitoring
3. **Resource Management**: Automatic cleanup and memory management
4. **Performance Tools**: Statistical analysis and bottleneck detection
5. **Advanced Capabilities**: Custom plasticity, agent behaviors, world environments
6. **Better Documentation**: Comprehensive examples and detailed API reference

The enhanced bindings maintain full backward compatibility while adding powerful new features for both beginners and advanced users. Whether you're running simple simulations or complex experiments, the enhanced NLM Python bindings provide the tools you need to build sophisticated neural simulations.

For more detailed examples and advanced usage patterns, refer to the updated `easy_usage.md` and `HOW_TO_USE.md` documentation files.