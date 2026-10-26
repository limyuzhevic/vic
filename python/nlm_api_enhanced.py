# NLM Python Bindings - Usage Examples

## Quick Start

This file demonstrates the enhanced Python API for NLM (Neural Learning Machine). The bindings provide comprehensive access to neural simulation capabilities.

```python
import pynlm

# Create basic components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent_brain = pynlm.createAgentBrain(brain)

# Configure the world
world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
world.setAgentStart(50, 50)

# Initialize the agent
agent_brain.initialize(world)

# Run simulation
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    
    # Update world
    world.update(0.1)
    
    # Generate action
    action = agent_brain.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, 0.1)
    
    # Check status
    print(f"Step {step}: Position ({world.getAgentBody().x}, {world.getAgentBody().y})")

# Save brain state
brain.save("saved_brain.nlm")
```

## Neuron Management

```python
# Access neurons in the brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Iterate through all neurons
for region in brain.getRegions():
    for population in region.getPopulations():
        for neuron in population.getNeurons():
            print(f"Neuron {neuron.getId()}: "
                  f"Type: {neuron.getType()}, "
                  f"Potential: {neuron.getMembranePotential()}")
```

## Memory Systems

```python
# Working memory
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

working_memory = brain.getWorkingMemory()
if working_memory:
    # Store a pattern
    pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
    working_memory.store(pattern)
    
    # Retrieve the pattern
    retrieved = working_memory.retrieve()
    print(f"Retrieved pattern: {retrieved}")

# Episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    # Create an episodic memory item
    item = pynlm.EpisodicMemoryItem()
    item.positionX = 10.0
    item.positionY = 20.0
    item.reward = 1.5
    item.action = pynlm.ActionType.Eat
    
    # Store the episode
    episodic_memory.storeEpisode(item)
    
    # Retrieve similar episodes
    similar = episodic_memory.retrieveSimilar([0.1, 0.2, 0.3])
    print(f"Found {len(similar)} similar episodes")
```

## Neuromodulation

```python
# Access neuromodulation systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Dopamine system
dopamine = brain.getDopamine()
if dopamine:
    dopamine.setLevel(0.8)  # High reward signal

# Curiosity system
curiosity = brain.getCuriosity()
if curiosity:
    curiosity.setLevel(0.6)  # Moderate curiosity

# Novelty detection
novelty = brain.getNovelty()
if novelty:
    novelty.setLevel(0.9)  # High novelty response
```

## Development

```python
# Development system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

development = brain.getDevelopmentSystem()
if development:
    print(f"Current stage: {development.getStage()}")
    print(f"Stage name: {development.getStageName()}")
    print(f"Plasticity modifier: {development.getPlasticityModifier()}")
    
    # Advance to next stage
    development.advanceStage()
```

## Prediction Systems

```python
# Prediction system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

prediction_system = brain.getPredictionSystem()
if prediction_system:
    # Make a prediction
    sensory_input = pynlm.Vision(20, 20)
    prediction = prediction_system.predictNextState(sensory_input)
    print(f"Prediction made: {prediction is not None}")

# Neural prediction
neural_prediction = brain.getPrediction()  # Hypothetical method
if neural_prediction:
    # Generate prediction
    predicted_state = neural_prediction.generatePrediction(0)
    print(f"Predicted state length: {len(predicted_state)}")
```

## Cognition Systems

```python
# Cognition systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Neural planner
planner = brain.getPlanner()
if planner:
    # Plan an action
    goal = [1.0, 1.0, 0.5]  # Target coordinates
    constraints = {}  # No constraints
    plan = planner.planAction(goal, constraints)
    print(f"Action plan: {plan}")

# Concept formation
concept_formation = brain.getConceptFormation()
if concept_formation:
    # Update with sensory input
    sensory_input = pynlm.Vision(20, 20)
    concept_formation.update(sensory_input)
    
    # Get current concepts
    concepts = concept_formation.getCurrentConcepts()
    print(f"Current concepts: {len(concepts)}")
```

## Attention Systems

```python
# Attention system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

attention = brain.getAttention()
if attention:
    # Process competing inputs
    competitors = [1, 2, 3, 4, 5]  # Neuron IDs
    winners = attention.processCompetition(competitors)
    print(f"Winners: {winners}")
    
    # Apply attention to a region
    region = pynlm.RegionId(1)
    attention.focusOnRegion(region)
```

## Visualization Integration

```python
# Visualization integration (if available)
import matplotlib.pyplot as plt
import numpy as np

# Example of how visualization would be integrated
# This is a hypothetical example showing how users might visualize simulation results

def plot_neural_activity(brain):
    """Plot neural activity patterns"""
    # Get all neurons
    neurons = []
    for region in brain.getRegions():
        for population in region.getPopulations():
            neurons.extend(population.getNeurons())
    
    # Extract activity data
    activity_levels = [neuron.getMembranePotential() for neuron in neurons]
    
    # Create visualization
    plt.figure(figsize=(10, 6))
    plt.hist(activity_levels, bins=50, alpha=0.7)
    plt.title("Neural Activity Distribution")
    plt.xlabel("Membrane Potential (mV)")
    plt.ylabel("Count")
    plt.grid(True, alpha=0.3)
    plt.show()

def plot_simulation_results(brain, world):
    """Plot simulation results over time"""
    # This would typically be called at the end of a simulation
    # to visualize the brain's development and performance
    
    # Get agent body position history
    body = world.getAgentBody()
    
    # Create visualizations
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 5))
    
    # Plot 1: Agent trajectory
    ax1.plot([body.x], [body.y], 'ro-', markersize=10)
    ax1.set_xlim(0, world.getWidth())
    ax1.set_ylim(0, world.getHeight())
    ax1.set_title("Agent Position")
    ax1.set_xlabel("X")
    ax1.set_ylabel("Y")
    ax1.grid(True, alpha=0.3)
    
    # Plot 2: Neural statistics
    neurons = []
    for region in brain.getRegions():
        neurons.extend(region.getAllNeurons())
    
    if neurons:
        avg_potential = np.mean([n.getMembranePotential() for n in neurons])
        avg_rate = np.mean([n.getFiringRate() for n in neurons])
        
        stats = ['Avg Potential (mV)', 'Avg Firing Rate (Hz)']
        values = [avg_potential, avg_rate]
        
        ax2.bar(stats, values, color=['skyblue', 'lightcoral'])
        ax2.set_title("Neural Statistics")
        ax2.set_ylabel("Value")
        ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()
```

## Performance Monitoring

```python
# Performance monitoring utilities
class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.step_times = []
        self.memory_usage = []
        self.energy_levels = []
    
    def start_step(self):
        """Record start time of a simulation step"""
        import time
        self.step_start = time.time()
    
    def end_step(self):
        """Record end time and calculate duration"""
        import time
        step_time = time.time() - self.step_start
        self.step_times.append(step_time)
    
    def record_memory_usage(self):
        """Record current memory usage (if available)"""
        # This is platform-specific
        try:
            import psutil
            process = psutil.Process()
            self.memory_usage.append(process.memory_info().rss / 1024 / 1024)  # MB
        except ImportError:
            self.memory_usage.append(0)
    
    def get_statistics(self):
        """Get performance statistics"""
        import statistics
        
        stats = {}
        if self.step_times:
            stats['avg_step_time'] = statistics.mean(self.step_times)
            stats['min_step_time'] = min(self.step_times)
            stats['max_step_time'] = max(self.step_times)
            if len(self.step_times) > 1:
                stats['std_step_time'] = statistics.stdev(self.step_times)
        
        if self.memory_usage:
            stats['avg_memory_usage'] = statistics.mean(self.memory_usage)
            stats['max_memory_usage'] = max(self.memory_usage)
        
        return stats
    
    def generate_report(self):
        """Generate a performance report"""
        stats = self.get_statistics()
        
        report = "Performance Report\n"
        report += "==================\n\n"
        
        if 'avg_step_time' in stats:
            report += f"Step Time:\n"
            report += f"  Average: {stats['avg_step_time']:.4f} seconds\n"
            report += f"  Min: {stats['min_step_time']:.4f} seconds\n"
            report += f"  Max: {stats['max_step_time']:.4f} seconds\n"
            if 'std_step_time' in stats:
                report += f"  Standard deviation: {stats['std_step_time']:.4f} seconds\n"
        
        if 'avg_memory_usage' in stats:
            report += f"\nMemory Usage:\n"
            report += f"  Average: {stats['avg_memory_usage']:.2f} MB\n"
            report += f"  Maximum: {stats['max_memory_usage']:.2f} MB\n"
        
        return report

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

monitor = PerformanceMonitor(brain)

for step in range(100):
    monitor.start_step()
    
    # Run simulation step
    brain.step(step)
    
    monitor.end_step()
    monitor.record_memory_usage()

# Generate and print report
print(monitor.generate_report())
```

## Experiment Management

```python
# Experiment management utilities
class NLMExperiment:
    def __init__(self, name, config):
        self.name = name
        self.config = config
        self.results = []
        self.brain_history = []
        self.world_history = []
    
    def run(self, world, steps=100):
        """Run an experiment"""
        print(f"Running experiment: {self.name}")
        
        for step in range(steps):
            # Record state before step
            self.brain_history.append({
                'step': step,
                'neurons': self._record_neurons(),
                'memory': self._record_memory(),
                'development': self._record_development()
            })
            
            self.world_history.append({
                'step': step,
                'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
                'agent_energy': world.getAgentBody().energy
            })
            
            # Run simulation step
            percept = world.getSensoryPercept()
            agent_brain = world.getAgentBrain()
            
            if agent_brain:
                agent_brain.processSensoryInput(percept)
                action = agent_brain.decodeMotorCommand()
                world.applyMotorCommand(action, 0.1)
                world.update(0.1)
            
            # Record results
            self.results.append({
                'step': step,
                'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
                'agent_energy': world.getAgentBody().energy,
                'brain_state': self._record_brain_state()
            })
        
        print(f"Experiment completed: {len(self.results)} steps recorded")
    
    def _record_neurons(self):
        """Record neuron states"""
        neurons = []
        for region in self.brain.getRegions():
            for population in region.getPopulations():
                for neuron in population.getNeurons():
                    neurons.append({
                        'id': neuron.getId().value,
                        'type': str(neuron.getType()),
                        'potential': neuron.getMembranePotential(),
                        'firing_rate': neuron.getFiringRate(),
                        'is_firing': neuron.isFiring()
                    })
        return neurons
    
    def _record_memory(self):
        """Record memory system states"""
        memory_data = {}
        
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            memory_data['working_memory'] = {
                'active_traces': working_memory.getActiveTraces(),
                'capacity': working_memory.getCapacity(),
                'decay_rate': working_memory.getDecayRate()
            }
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            memory_data['episodic_memory'] = {
                'episode_count': episodic_memory.getEpisodeCount(),
                'replay_enabled': episodic_memory.isReplayEnabled()
            }
        
        return memory_data
    
    def _record_development(self):
        """Record developmental state"""
        development = self.brain.getDevelopmentSystem()
        if development:
            return {
                'stage': str(development.getStage()),
                'stage_name': development.getStageName(),
                'plasticity_modifier': development.getPlasticityModifier(),
                'developmental_age': development.getDevelopmentalAge(),
                'is_critical_period': development.isCriticalPeriod()
            }
        return None
    
    def _record_brain_state(self):
        """Record brain state for comparison"""
        return {
            'regions': self._record_regions(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'average_firing_rate': self.brain.getAverageFiringRate()
        }
    
    def _record_regions(self):
        """Record region information"""
        regions = []
        for region in self.brain.getRegions():
            regions.append({
                'id': region.getId().value,
                'name': region.getName(),
                'population_count': region.getPopulationCount(),
                'total_neuron_count': region.getTotalNeuronCount()
            })
        return regions
    
    def save_results(self, filename):
        """Save experiment results to file"""
        import json
        
        results = {
            'name': self.name,
            'config': {
                'steps': len(self.results),
                'final_energy': self.results[-1]['agent_energy'] if self.results else 0,
                'final_position': self.results[-1]['agent_position'] if self.results else (0, 0)
            },
            'results': self.results,
            'brain_history': self.brain_history,
            'world_history': self.world_history
        }
        
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"Results saved to: {filename}")
    
    def analyze_results(self):
        """Analyze experiment results and return statistics"""
        if not self.results:
            return None
        
        analysis = {
            'total_steps': len(self.results),
            'final_energy': self.results[-1]['agent_energy'],
            'energy_trajectory': [r['agent_energy'] for r in self.results],
            'position_trajectory': [r['agent_position'] for r in self.results],
            'neuron_count_trajectory': [r.get('brain_state', {}).get('total_neurons', 0) for r in self.results]
        }
        
        return analysis

# Example usage
config = pynlm.createDefaultConfig()
experiment = NLMExperiment("Exploration Test", config)

world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
world.setAgentStart(10, 10)

brain = pynlm.createBrain(config)
brain.initialize()

agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run experiment
experiment.run(world, steps=200)

# Save results
experiment.save_results("exploration_experiment.json")

# Analyze results
analysis = experiment.analyze_results()
if analysis:
    print(f"Experiment Analysis:")
    print(f"  Total steps: {analysis['total_steps']}")
    print(f"  Final energy: {analysis['final_energy']:.2f}")
    print(f"  Energy trajectory: {analysis['energy_trajectory'][-10:]}...")  # Last 10 values
```

## Error Handling and Validation

```python
# Enhanced error handling
class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class SimulationError(NLMError):
    """Error during simulation"""
    pass

class ConfigurationError(NLMError):
    """Error in configuration"""
    pass

class MemoryError(NLMError):
    """Error in memory systems"""
    pass

# Validation functions
def validate_brain_state(brain):
    """Validate brain state"""
    if brain is None:
        raise ConfigurationError("Brain cannot be None")
    
    if not brain.isInitialized():
        raise SimulationError("Brain is not initialized")
    
    # Check for valid regions
    regions = brain.getRegions()
    if not regions:
        raise SimulationError("Brain has no regions")
    
    # Check for valid neurons
    total_neurons = brain.getTotalNeuronCount()
    if total_neurons <= 0:
        raise SimulationError("Brain has no neurons")
    
    return True

def validate_world_state(world):
    """Validate world state"""
    if world is None:
        raise ConfigurationError("World cannot be None")
    
    # Check world dimensions
    if world.getWidth() <= 0 or world.getHeight() <= 0:
        raise ConfigurationError("World dimensions must be positive")
    
    # Check agent body
    agent_body = world.getAgentBody()
    if agent_body is None:
        raise SimulationError("World has no agent body")
    
    # Check agent position is within bounds
    if (agent_body.x < 0 or agent_body.x > world.getWidth() or
        agent_body.y < 0 or agent_body.y > world.getHeight()):
        raise SimulationError("Agent position is out of bounds")
    
    return True

# Example error handling
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()

try:
    validate_brain_state(brain)
    validate_world_state(world)
    
    # Run simulation
    percept = world.getSensoryPercept()
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, 0.1)
    world.update(0.1)
    
    print("Simulation completed successfully")
    
except NLMError as e:
    print(f"NLM Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Advanced Features

```python
# Advanced features and utilities

# Batch processing for multiple simulations
class SimulationBatch:
    def __init__(self):
        self.simulations = []
    
    def add_simulation(self, brain, world, steps=100):
        """Add a simulation to the batch"""
        self.simulations.append({
            'brain': brain,
            'world': world,
            'steps': steps
        })
    
    def run_batch(self):
        """Run all simulations in the batch"""
        results = []
        for i, sim in enumerate(self.simulations):
            print(f"Running simulation {i+1}/{len(self.simulations)}")
            
            # Run simulation
            for step in range(sim['steps']):
                brain = sim['brain']
                world = sim['world']
                
                percept = world.getSensoryPercept()
                agent_brain = pynlm.createAgentBrain(brain)
                agent_brain.initialize(world)
                
                agent_brain.processSensoryInput(percept)
                action = agent_brain.decodeMotorCommand()
                world.applyMotorCommand(action, 0.1)
                world.update(0.1)
            
            results.append(sim)
        
        return results

# Statistical analysis
class NeuralStatistics:
    @staticmethod
    def compute_network_density(brain):
        """Compute network density (synapses/neurons)"""
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        
        if neurons == 0:
            return 0.0
        
        return synapses / neurons
    
    @staticmethod
    def compute_activity_patterns(brain):
        """Compute activity patterns across neurons"""
        neurons = []
        for region in brain.getRegions():
            for population in region.getPopulations():
                neurons.extend(population.getNeurons())
        
        if not neurons:
            return None
        
        # Extract activity features
        potentials = [n.getMembranePotential() for n in neurons]
        firing_rates = [n.getFiringRate() for n in neurons]
        is_firing = [n.isFiring() for n in neurons]
        
        patterns = {
            'potential_mean': sum(potentials) / len(potentials),
            'potential_std': sum((p - sum(potentials) / len(potentials)) ** 2 for p in potentials) / len(potentials),
            'firing_rate_mean': sum(firing_rates) / len(firing_rates),
            'firing_rate_std': sum((fr - sum(firing_rates) / len(firing_rates)) ** 2 for fr in firing_rates) / len(firing_rates),
            'firing_percentage': sum(is_firing) / len(is_firing) * 100
        }
        
        return patterns
    
    @staticmethod
    def detect_outliers(data, threshold=2.0):
        """Detect outliers in data using z-score"""
        if not data:
            return []
        
        mean = sum(data) / len(data)
        variance = sum((x - mean) ** 2 for x in data) / len(data)
        std = variance ** 0.5
        
        if std == 0:
            return []
        
        z_scores = [(x - mean) / std for x in data]
        outliers = [i for i, z in enumerate(z_scores) if abs(z) > threshold]
        
        return outliers

# Example usage of advanced features
brain = pynlm.createBrain(config)
brain.initialize()

# Network density
network_density = NeuralStatistics.compute_network_density(brain)
print(f"Network density: {network_density:.4f} synapses/neuron")

# Activity patterns
patterns = NeuralStatistics.compute_activity_patterns(brain)
if patterns:
    print("Activity patterns:")
    for key, value in patterns.items():
        print(f"  {key}: {value:.4f}")
```

## Installation and Setup

```bash
# Clone the NLM repository
# git clone https://github.com/nlm-project/nlm.git
# cd nlm

# Install Python bindings
pip install -e python/

# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install and run tests
pip install -e python/[dev]
pytest python/tests/
```

## API Reference

### Core Classes

- `Config`: Configuration management system
- `Brain`: Central neural simulation brain
- `AgentBrain`: Agent brain interface connecting NLM to world
- `SimpleWorld`: Simple 2D world for simulation
- `Neuron`: Individual neuron implementing LIF dynamics
- `Synapse`: Synaptic connection between neurons
- `NeuralPopulation`: Group of neurons with shared properties
- `NeuralRegion`: Brain region with multiple populations

### Memory Systems

- `NeuralWorkingMemory`: Working memory using neural dynamics
- `NeuralEpisodicMemory`: Episodic memory system
- `NeuralAssociativeMemory`: Associative memory system
- `AttentionalSelection`: Neural attention through competition

### Development Systems

- `DevelopmentSystem`: Orchestrates neural development
- `Synaptogenesis`: Synapse formation
- `Pruning`: Synapse pruning
- `Maturation`: Neural maturation

### Prediction Systems

- `PredictionSystem`: Predicts future sensory states
- `NeuralPrediction`: Implements temporal prediction
- `ActionConsequencePredictor`: Learns action-consequence mappings
- `PredictionErrorSignal`: Computes prediction error

### Cognition Systems

- `NeuralPlanner`: Neural action planner
- `ConceptFormation`: Pattern discovery from experience

### Neuromodulation Systems

- `Neuromodulator`: Abstract base for neuromodulators
- `Dopamine`: Reward and reinforcement
- `Novelty`: Novelty detection
- `Curiosity`: Exploration motivation
- `PredictionError`: Prediction error signal

### Sensory and Motor Systems

- `SensoryInput`: Base class for sensory input
- `Vision`: Vision sensory input
- `Audio`: Audio sensory input
- `InternalSignals`: Internal signals sensory input
- `Action`: Action representation for motor output
- `WorldObject`: World object representation
- `AgentBody`: Agent body state
- `ActionResult`: Action result from world
- `SensoryPercept`: Sensory percept data

### Error Types

- `NLMError`: Base exception for NLM errors
- `SimulationError`: Error during simulation
- `ConfigurationError`: Error in configuration
- `MemoryError`: Error in memory systems

### Factory Methods

- `createDefaultConfig()`: Create a default configuration
- `createBrain(config)`: Create a new brain with configuration
- `createSimpleWorld()`: Create a new simple world
- `createAgentBrain(brain)`: Create a new agent brain interface

### Constants

- `INVALID_NEURON_ID`: Invalid neuron ID sentinel
- `INVALID_SYNAPSE_ID`: Invalid synapse ID sentinel
- `INVALID_REGION_ID`: Invalid region ID sentinel
- `INVALID_POPULATION_ID`: Invalid population ID sentinel

## Troubleshooting

### Common Issues

1. **Import Error**
   ```python
   # Error: cannot import name 'pynlm'
   
   # Solution: Ensure the Python bindings are properly installed
   pip install pynlm
   ```

2. **Simulation Errors**
   ```python
   # Error: Simulation failed or invalid state
   
   # Solution: Use validation functions
   validate_brain_state(brain)
   validate_world_state(world)
   ```

3. **Memory Issues**
   ```python
   # Error: Memory limit exceeded or invalid memory state
   
   # Solution: Monitor memory usage and optimize memory management
   ```

4. **Performance Issues**
   ```python
   # Error: Simulation too slow
   
   # Solution: Use performance monitoring and optimization
   ```

### Debugging Tips

1. **Use print statements**: Add print statements to debug simulation flow
2. **Check validation**: Use validation functions to identify issues
3. **Monitor memory**: Use memory monitoring utilities
4. **Profile performance**: Use performance monitoring tools
5. **Save intermediate states**: Save brain and world states for debugging

## Future Enhancements

The enhanced Python API can be further extended with:

1. **Machine Learning Integration**: Integrate with scikit-learn, TensorFlow, or PyTorch
2. **Advanced Visualization**: 3D visualization and interactive plots
3. **Parallel Processing**: Multi-core and GPU acceleration
4. **Distributed Computing**: Multi-agent simulations
5. **Real-time Interfaces**: Integration with ROS or other real-time systems
6. **Web Interfaces**: Jupyter notebooks and web-based visualization
7. **REST API**: Web API for remote access
8. **Container Support**: Docker and Kubernetes support

## Conclusion

The enhanced Python API for NLM provides a comprehensive interface for neural simulation with:

- **Complete functionality**: All major NLM components are exposed
- **Pythonic interface**: Properties, context managers, and error handling
- **Performance monitoring**: Tools to track and optimize simulation performance
- **Visualization integration**: Support for visualization and analysis
- **Experiment management**: Tools to design and run experiments
- **Documentation and examples**: Comprehensive documentation and usage examples

This API makes it easy to:
- Create and run neural simulations
- Analyze simulation results
- Design experiments
- Integrate NLM with other Python tools and libraries
- Extend NLM functionality

The enhanced Python API bridges the gap between the powerful C++ core and Python users, providing an intuitive and flexible interface for neural simulation research and applications.