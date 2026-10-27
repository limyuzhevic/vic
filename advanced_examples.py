# NLM Python Bindings - Advanced Examples and Tutorials

This document provides comprehensive examples for working with the advanced features of the NLM Python bindings.

## Advanced Simulation Control

### 1. Adaptive Simulation Control

```python
import pynlm as nlm
import time

# Create a brain and world
brain = nlm.createBrainTemplate("medium")
world = nynlm.createSimpleWorld()
world.configure(50, 50, 32, 32)
world.reset()

# Initialize agent with adaptive learning
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run adaptive simulation
print("Starting adaptive simulation...")
start_time = time.time()

result = nlm.control_simulation(
    brain,
    world,
    control_mode="adaptive",
    parameters={
        "target_firing_rate": 10.0,
        "learning_rate": 0.1
    }
)

end_time = time.time()
print(f"Adaptive simulation completed in {result.execution_time:.2f} seconds")
print(f"Final firing rate: {result.metrics['final_firing_rate']:.2f}")
print(f"Target was: {result.metrics['target_firing_rate']:.2f}")
print(f"Learning rate: {result.metrics['learning_rate']:.3f}")
print(f"Status: {result.status}")
```

### 2. Random Exploration

```python
import pynlm as nlm
import random

# Create simulation
brain = nlm.createBrainTemplate("small")
world = nlm.createSimpleWorld()
world.configure(30, 30, 16, 16)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable curiosity for exploration
agent.enableCuriosity(True)

# Run random exploration simulation
print("Starting random exploration simulation...")

result = nlm.control_simulation(
    brain,
    world,
    control_mode="random",
    parameters={
        "exploration_strength": 1.0
    }
)

print(f"Random exploration completed in {result.execution_time:.2f} seconds")
print(f"Exploration strength: {result.metrics['exploration_strength']}")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
```

### 3. Goal-Oriented Learning

```python
import pynlm as nlm

# Create goal-oriented simulation
brain = nlm.createBrainTemplate("medium")
world = nlm.createSimpleWorld()
world.configure(40, 40, 24, 24)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems for goal-oriented behavior
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run goal-oriented simulation
print("Starting goal-oriented learning simulation...")

result = nlm.control_simulation(
    brain,
    world,
    control_mode="goal_oriented",
    parameters={
        "goal_reward": 100.0,
        "convergence_threshold": 0.95
    }
)

print(f"Goal-oriented simulation completed!")
print(f"Goal reward: {result.metrics['goal_reward']}")
print(f"Cumulative reward achieved: {result.metrics['cumulative_reward']:.2f}")
print(f"Convergence threshold: {result.metrics['convergence_threshold']}")
print(f"Convergence achieved: {result.metrics['convergence_achieved'] * 100:.1f}%")
print(f"Execution time: {result.execution_time:.2f} seconds")
print(f"Status: {result.status}")
```

## Advanced Analysis and Profiling

### 4. Neural Network Profiling

```python
import pynlm as nlm
import time

# Create brain for profiling
brain = nlm.createBrainTemplate("large")

# Run performance profile
print("Profiling neural network simulation...")

profile = nlm.profile_brain_simulation(
    brain,
    num_steps=500,
    timestep=0.001,
    verbose=False
)

print("Simulation Profile:")
print(f"  Total CPU time: {profile.total_time:.2f} seconds")
print(f"  Wall clock time: {profile.wall_clock_time:.2f} seconds")
print(f"  Steps per second: {profile.steps_performed / profile.total_time:.1f}")
print(f"  Average firing rate: {profile.avg_firing_rate:.3f}")
print(f"  Peak memory usage: {profile.peak_memory_mb:.1f} MB")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

### 5. Comprehensive Neural Analysis

```python
import pynlm as nlm

# Create brain for analysis
brain = nlm.createBrainTemplate("medium")
brain.initialize()

# Run simulation for analysis
for step in range(200):
    brain.step(step, step * 0.001)

# Perform comprehensive analysis
print("Performing comprehensive neural analysis...")

analysis = nlm.analyze_neural_dynamics(brain, "comprehensive")

print("Neural Dynamics Analysis:")
print(f"  Pattern type: {analysis.pattern_type}")
print(f"  Total neurons: {analysis.metrics['total_neurons']}")
print(f"  Total synapses: {analysis.metrics['total_synapses']}")
print(f"  Active neurons: {analysis.metrics['active_neurons']} ({analysis.metrics['activity_percentage']:.1f}%)")
print(f"  Firing neurons: {analysis.metrics['firing_neurons']} ({analysis.metrics['firing_percentage']:.1f}%)")
print(f"  Average firing rate: {analysis.metrics['average_firing_rate']:.3f}")
print(f"  E/I ratio: {analysis.metrics['excitatory_inhibition_ratio']:.2f}")
print(f"  Total spikes: {analysis.metrics['total_spikes']}")
print(f"  Network efficiency: {analysis.metrics['network_efficiency']:.2f}")
print(f"  Connection density: {analysis.metrics['connection_density']:.3f}%")
print(f"\nInterpretation: {analysis.interpretation}")

# Pattern-specific analysis
print("\nActivity Pattern Analysis:")
activity_analysis = nlm.analyze_neural_patterns(brain, "activity")
print(f"  Mean firing rate: {activity_analysis.metrics['mean_firing_rate']:.3f}")
print(f"  Activity coherence: {activity_analysis.metrics['activity_coherence']:.3f}")
print(f"  Spike train regularity: {activity_analysis.metrics['spike_train_regularity']:.3f}")
print(f"  Interpretation: {activity_analysis.interpretation}")
```

### 6. Environment Interaction Analysis

```python
import pynlm as nlm

# Create complete agent-environment system
brain = nlm.createBrainTemplate("medium")
world = nlm.createSimpleWorld()
world.configure(60, 60, 32, 32)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all systems for comprehensive analysis
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run environment interaction analysis
print("Analyzing environment interaction...")

analysis = nlm.analyze_environment_interaction(brain, world, 300)

print("Environment Interaction Analysis:")
print(f"  Environment type: {analysis.environment_type}")
print(f"  Sensory processing efficiency: {analysis.interaction_metrics['sensory_processing_efficiency']:.2f}")
print(f"  Action selection speed: {analysis.interaction_metrics['action_selection_speed']:.2f}")
print(f"  Reward prediction accuracy: {analysis.interaction_metrics['reward_prediction_accuracy']:.2f}")
print(f"  Environmental adaptation: {analysis.interaction_metrics['environmental_adaptation']:.2f}")
print(f"  Behavioral patterns: {len(analysis.behavioral_patterns)} detected")
for pattern in analysis.behavioral_patterns:
    print(f"    - {pattern}")
print(f"  Adaptation score: {analysis.adaptation_score:.2f}")
```

## Advanced Configuration and Experiment Management

### 7. Configuration Optimization

```python
import pynlm as nlm

# Create different configurations to optimize
configs = []
for neurons in [100, 500, 1000]:
    config = nlm.createDefaultConfig()
    config.set("neuron_count", neurons)
    config.set("region_count", max(1, neurons // 200))
    config.set("simulation_timestep", 0.001)
    configs.append(config)

# Optimize configurations
print("Optimizing configurations...")

result = nlm.optimize_configuration(
    configs[0],  # Use first config as base
    optimization_target="performance",
    iterations=30
)

print("Configuration Optimization Results:")
print(f"  Best score achieved: {result.best_score:.2f}")
print(f"  Number of iterations: {len(result.convergence_history)}")
print(f"  Convergence trend: {'Improved' if result.convergence_history[-1] > result.convergence_history[0] else 'Degraded'}")

# Show convergence history
print(f"\nConvergence history (last 10 iterations):")
for i, score in enumerate(result.convergence_history[-10:]):
    print(f"  Iter {len(result.convergence_history) - 10 + i + 1}: {score:.2f}")
```

### 8. Advanced Checkpoint Management

```python
import pynlm as nlm
import tempfile
import os

# Create brain for checkpoint testing
brain = nlm.createBrainTemplate("medium")
brain.initialize()

# Create temporary directory for checkpoints
temp_dir = tempfile.mkdtemp()
print(f"Creating checkpoints in: {temp_dir}")

# Create advanced checkpoints with metadata
print("Creating advanced checkpoints...")

checkpoints = nlm.advanced_checkpoint_manager(
    brain,
    base_path=temp_dir,
    max_checkpoints=5,
    compress=False
)

print(f"Created {len(checkpoints)} checkpoints:")
for i, checkpoint in enumerate(checkpoints):
    print(f"  Checkpoint {i + 1}:")
    print(f"    Path: {checkpoint.path}")
    print(f"    Timestamp: {checkpoint.timestamp}")
    print(f"    Neurons: {checkpoint.neuron_count}")
    print(f"    Synapses: {checkpoint.synapse_count}")
    print(f"    Spikes: {checkpoint.spike_count}")

# Cleanup
import shutil
shutil.rmtree(temp_dir)
print(f"Cleaned up temporary directory: {temp_dir}")
```

### 9. Batch Processing

```python
import pynlm as nlm

# Create multiple configurations for batch processing
print("Setting up batch processing...")

configs = []
worlds = []

# Configuration 1: Small brain for quick testing
config1 = nlm.createDefaultConfig()
config1.set("neuron_count", 100)
config1.set("region_count", 2)
configs.append(config1)

# Configuration 2: Medium brain for general use
config2 = nlm.createDefaultConfig()
config2.set("neuron_count", 500)
config2.set("region_count", 4)
configs.append(config2)

# Configuration 3: Large brain for complex tasks
config3 = nlm.createDefaultConfig()
config3.set("neuron_count", 1000)
config3.set("region_count", 6)
configs.append(config3)

# Create worlds for different environments
for i in range(3):
    world = nlm.createSimpleWorld()
    world.configure(30 + i * 10, 30 + i * 10, 16 + i * 4, 16 + i * 4)
    worlds.append(world)

simulation_names = ["Small_Brain_Test", "Medium_Brain_Test", "Large_Brain_Test"]

# Run batch simulation
print("Running batch simulations...")

results = nlm.run_batch_simulation(
    configs,
    worlds,
    steps_per_config=100,
    timestep=0.001,
    parallel=False
)

print("Batch Simulation Results:")
print("=" * 60)
for i, result in enumerate(results):
    print(f"\nSimulation {i + 1} ({simulation_names[i]}):")
    print(f"  Config index: {int(result['config_index'])}")
    print(f"  Average firing rate: {result['avg_firing_rate']:.3f}")
    print(f"  Total reward: {result['total_reward']:.3f}")
    print(f"  Steps completed: {int(result['steps_completed'])}")
    print(f"  Successful steps: {int(result['successful_steps'])} ({result['successful_steps'] / result['steps_completed'] * 100:.1f}%)")

# Summary
print("\n" + "=" * 60)
print("Batch processing completed successfully!")
```

## Advanced Neural Network Analysis

### 10. Pattern Analysis

```python
import pynlm as nlm

# Create brain for pattern analysis
brain = nlm.createBrainTemplate("large")
brain.initialize()

# Run simulation to generate patterns
for step in range(500):
    brain.step(step, step * 0.001)

print("Analyzing neural patterns...")

# Analyze different pattern types
pattern_types = ["activity", "connectivity", "plasticity"]

for pattern_type in pattern_types:
    print(f"\n{pattern_type.upper()} Pattern Analysis:")
    analysis = nlm.analyze_neural_patterns(brain, pattern_type)
    
    print(f"  Pattern type: {analysis.pattern_type}")
    print(f"  Number of metrics: {len(analysis.metrics)}")
    
    for metric, value in analysis.metrics.items():
        print(f"    {metric}: {value:.3f}")
    
    print(f"  Time series points: {len(analysis.time_series)}")
    print(f"  Interpretation: {analysis.interpretation}")
```

### 11. Neural Debugging

```python
import pynlm as nlm

# Create brain for debugging
brain = nlm.createBrainTemplate("medium")
brain.initialize()

# Add some activity to make debugging more interesting
for step in range(100):
    brain.step(step, step * 0.001)

# Run neural network debugging
print("Debugging neural network...")

debug_info = nlm.debug_neural_network(brain, detailed=True)

print(f"Debug Status: {debug_info.status}")
print(f"Number of warnings: {len(debug_info.warnings)}")
for warning in debug_info.warnings:
    print(f"  WARNING: {warning}")

print(f"Number of errors: {len(debug_info.errors)}")
for error in debug_info.errors:
    print(f"  ERROR: {error}")

print(f"\nDebug Metrics:")
for metric, value in debug_info.debug_metrics.items():
    print(f"  {metric}: {value}")
```

## Advanced Simulation Comparison

### 12. Complete Simulation Comparison

```python
import pynlm as nlm

print("Setting up comprehensive simulation comparison...")

# Create multiple brains with different configurations
brains = []
worlds = []

# Brain 1: Minimal configuration
brain1 = nlm.createBrainTemplate("small")
brains.append(brain1)

# Brain 2: Optimized configuration
brain2 = nlm.createBrainTemplate("medium")
brains.append(brain2)

# Brain 3: High-performance configuration
brain3 = nlm.createBrainTemplate("large")
brains.append(brain3)

# Create different worlds for each brain
for i in range(3):
    world = nlm.createSimpleWorld()
    world.configure(20 + i * 20, 20 + i * 20, 10 + i * 5, 10 + i * 5)
    worlds.append(world)

simulation_names = [
    "Small_Network_Baseline",
    "Medium_Network_Optimized", 
    "Large_Network_Experimental"
]

# Run comprehensive comparison
print("Running simulation comparison...")

comparison = nlm.compare_simulations(
    brains,
    worlds,
    simulation_names,
    comparison_steps=200
)

print("\n" + "=" * 70)
print("SIMULATION COMPARISON RESULTS")
print("=" * 70)
print(f"Comparison title: {comparison.title}")
print(f"Simulations compared: {len(comparison.simulation_names)}")

print("\nDetailed Metrics:")
print("-" * 70)
print(f"{'Simulation':<25} {'Avg Firing':<12} {'Success%':<10} {'Total Reward':<12} {'Neurons':<10} {'Synapses':<10}")
print("-" * 70)

for i, name in enumerate(comparison.simulation_names):
    metrics = comparison.metrics_per_simulation[i]
    print(f"{name:<25} {metrics['average_firing_rate']:<12.3f} "
          f"{metrics['successful_steps']/comparison.comparison_steps*100:<10.1f} "
          f"{metrics['total_reward']:<12.3f} "
          f"{int(metrics['neuron_count']):<10} "
          f"{int(metrics['synapse_count']):<10}")

print("\nPerformance Ranking:")
print("-" * 50)
for name, score in comparison.performance_ranking.items():
    print(f"{name:<40}: {score:.2f}")

print(f"\nRecommendation: {comparison.recommendation}")
```

## Advanced Examples with Complete Agent Systems

### 13. Learning with Curiosity

```python
import pynlm as nlm
import time

# Create advanced agent with all systems enabled
brain = nlm.createBrainTemplate("medium")
world = nlm.createSimpleWorld()
world.configure(50, 50, 32, 32)
world.reset()

# Initialize agent with adaptive learning
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning systems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore novel things
agent.enableDevelopment(True)           # Mature over time
agent.enableStructuralPlasticity(True)  # Grow new connections

print("Starting advanced learning simulation with curiosity...")
start_time = time.time()

# Run learning simulation
for episode in range(20):
    episode_reward = 0.0
    
    for step in range(100):
        # Update world
        world.update(0.01)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step + episode * 100, (step + episode * 100) * 0.01)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, (step + episode * 100) * 0.01)
        
        # Get reward and apply learning
        try:
            reward = world.computeReward(brain.get())
            episode_reward += reward
            
            # Apply reward with prediction error
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
            
        except:
            pass
        
        # Update development
        agent.updateDevelopment(0.01)
        
        # Log progress every 20 steps
        if step % 20 == 0:
            print(f"  Episode {episode + 1}, Step {step}: "
                  f"Reward={reward:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}, "
                  f"Development={brain.getDevelopmentalStage()}")

end_time = time.time()

print(f"\nAdvanced learning simulation completed!")
print(f"Total time: {end_time - start_time:.2f} seconds")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### 14. Performance-Optimized Simulation

```python
import pynlm as nlm
import time

# Create high-performance simulation
brain = nlm.createBrainTemplate("large")
world = nlm.createSimpleWorld()
world.configure(100, 100, 50, 50)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Optimize for performance
agent.enableRewardModulation(True)
agent.enableCuriosity(False)  # Disable curiosity for performance
agent.enableDevelopment(False)  # Disable development for stability
agent.enableStructuralPlasticity(False)  # Disable structural changes

print("Starting performance-optimized simulation...")
start_time = time.time()

# Run performance test
profile = nlm.profile_brain_simulation(
    brain,
    world,
    num_steps=1000,
    timestep=0.001,
    verbose=False
)

end_time = time.time()

print("Performance Analysis:")
print(f"  Total simulation time: {end_time - start_time:.2f} seconds")
print(f"  Profile duration: {profile.total_time:.2f} seconds")
print(f"  Steps per second: {profile.steps_performed / profile.total_time:.1f}")
print(f"  CPU efficiency: {(profile.steps_performed / profile.total_time) / 1000:.2%}")
print(f"  Average firing rate: {profile.avg_firing_rate:.3f}")
print(f"  Memory efficiency: {brain.getTotalNeuronCount() / profile.peak_memory_mb:.1f} neurons/MB")
print(f"  Spike processing rate: {profile.steps_performed / brain.getTotalSpikeCount():.1f} steps/spike")
```

## Integration Examples

### 15. Complete Research Experiment

```python
import pynlm as nlm
import json
import os

# Design a comprehensive research experiment
class NeuralResearchExperiment:
    def __init__(self, experiment_name):
        self.name = experiment_name
        self.results = {}
        self.configurations = []
        self.worlds = []
        self.brains = []
        self.agents = []
    
    def setup_experiment(self, config_templates):
        """Setup experiment with multiple configurations."""
        print(f"Setting up experiment: {self.name}")
        
        for i, template_name in enumerate(config_templates):
            # Create brain with specific template
            brain = nlm.createBrainTemplate(template_name)
            self.brains.append(brain)
            
            # Create world with specific parameters
            world = nlm.createSimpleWorld()
            size = 20 + i * 20
            world.configure(size, size, 10 + i * 5, 10 + i * 5)
            self.worlds.append(world)
            
            # Create agent brain
            agent = nlm.createAgentBrain(brain)
            agent.initialize(world)
            self.agents.append(agent)
            
            # Enable different subsystems based on template
            if template_name == "small":
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
            elif template_name == "medium":
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
            elif template_name == "large":
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
                agent.enableStructuralPlasticity(True)
            
            print(f"  Configuration {i + 1} ({template_name}): ")
            print(f"    Neurons: {brain.getTotalNeuronCount()}")
            print(f"    World size: {size}x{size}")
    
    def run_research_phase(self, phase_name, steps_per_phase, enable_profiling=True):
        """Run a specific research phase with data collection."""
        print(f"\nRunning research phase: {phase_name}")
        
        phase_data = {
            'name': phase_name,
            'steps_completed': 0,
            'total_reward': 0.0,
            'neural_metrics': {},
            'behavioral_metrics': {},
            'environment_interactions': []
        }
        
        for phase_idx, (brain, world, agent) in enumerate(zip(self.brains, self.worlds, self.agents)):
            print(f"  Phase {phase_idx + 1} processing...")
            
            brain.initialize()
            
            for step in range(steps_per_phase):
                # Update environment
                world.update(0.01)
                
                # Process sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run neural simulation
                brain.step(step, step * 0.01)
                
                # Generate action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, step * 0.01)
                
                # Collect data
                try:
                    reward = world.computeReward(brain.get())
                    phase_data['total_reward'] += reward
                except:
                    reward = 0.0
                
                # Collect neural metrics
                if phase_idx not in phase_data['neural_metrics']:
                    phase_data['neural_metrics'][phase_idx] = []
                
                phase_data['neural_metrics'][phase_idx].append({
                    'firing_rate': brain.getAverageFiringRate(),
                    'active_neurons': brain.getActiveNeuronCount(),
                    'spikes': brain.getTotalSpikeCount(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'development_stage': str(brain.getDevelopmentalStage())
                })
                
                phase_data['environment_interactions'].append({
                    'step': step,
                    'action': str(action),
                    'reward': reward,
                    'world_state': {
                        'width': world.getWidth(),
                        'height': world.getHeight(),
                        'simulation_time': world.getSimulationTime()
                    }
                })
                
                phase_data['steps_completed'] += 1
        
        self.results[phase_name] = phase_data
        return phase_data
    
    def analyze_results(self):
        """Analyze and interpret experiment results."""
        print(f"\nAnalyzing experiment results for: {self.name}")
        
        analysis_report = {
            'experiment_name': self.name,
            'phases_completed': list(self.results.keys()),
            'total_steps': sum(phase['steps_completed'] for phase in self.results.values()),
            'total_reward': sum(phase['total_reward'] for phase in self.results.values()),
            'neural_performance': {},
            'behavioral_insights': []
        }
        
        # Analyze neural performance
        for phase_name, phase_data in self.results.items():
            phase_neural_perf = {}
            
            for config_idx, metrics in phase_data['neural_metrics'].items():
                firing_rates = [m['firing_rate'] for m in metrics]
                curiosities = [m['curiosity'] for m in metrics]
                novelties = [m['novelty'] for m in metrics]
                
                phase_neural_perf[f"config_{config_idx}"] = {
                    'avg_firing_rate': sum(firing_rates) / len(firing_rates),
                    'firing_rate_variance': sum((x - sum(firing_rates)/len(firing_rates))**2 for x in firing_rates) / len(firing_rates),
                    'avg_curiosity': sum(curiosities) / len(curiosities),
                    'avg_novelty': sum(novelties) / len(novelties),
                    'stability_score': 1.0 - (max(firing_rates) - min(firing_rates)) / max(firing_rates) if max(firing_rates) > 0 else 0.0
                }
            
            analysis_report['neural_performance'][phase_name] = phase_neural_perf
            
            # Generate behavioral insights
            if phase_data['total_reward'] > 0:
                analysis_report['behavioral_insights'].append(
                    f"Phase {phase_name} achieved positive reward ({phase_data['total_reward']:.2f}) - successful learning"
                )
            else:
                analysis_report['behavioral_insights'].append(
                    f"Phase {phase_name} achieved neutral/negative reward ({phase_data['total_reward']:.2f}) - needs investigation"
                )
        
        return analysis_report
    
    def save_results(self, output_dir):
        """Save experiment results to files."""
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # Save raw data
        with open(os.path.join(output_dir, f"{self.name}_raw_data.json"), 'w') as f:
            json.dump(self.results, f, indent=2)
        
        # Save analysis
        analysis = self.analyze_results()
        with open(os.path.join(output_dir, f"{self.name}_analysis.json"), 'w') as f:
            json.dump(analysis, f, indent=2)
        
        print(f"Experiment results saved to: {output_dir}")

# Run the complete research experiment
experiment = NeuralResearchExperiment("Advanced_Neural_Learning_Study")

experiment.setup_experiment(["small", "medium", "large"])

experiment.run_research_phase("Phase_1_Baseline", 100)
experiment.run_research_phase("Phase_2_Curiosity_Driven", 150, enable_profiling=True)
experiment.run_research_phase("Phase_3_Goal_Oriented", 200)

# Analyze and save results
analysis = experiment.analyze_results()
experiment.save_results("./experiment_results")

print("\n" + "=" * 60)
print("RESEARCH EXPERIMENT COMPLETED")
print("=" * 60)
print(f"Experiment name: {experiment.name}")
print(f"Phases completed: {len(experiment.results)}")
print(f"Total steps: {analysis['total_steps']}")
print(f"Total reward: {analysis['total_reward']:.2f}")
print(f"Neural performance analyzed for {len(analysis['neural_performance'])} phases")
print(f"Behavioral insights: {len(analysis['behavioral_insights'])}")
for insight in analysis['behavioral_insights']:
    print(f"  - {insight}")
```

## Running Advanced Examples

To run these advanced examples, ensure you have:

1. **Basic setup**: All NLM dependencies installed
2. **Advanced requirements**: May need additional computational resources for large simulations

```bash
# Install basic NLM
pip install pynlm

# For advanced examples with profiling and analysis
pip install numpy scipy matplotlib psutil

# Run a specific example
python advanced_example.py
```

## Performance Considerations

### For Large-Scale Advanced Simulations:

1. **Resource allocation**: Use larger brains only for complex experiments
2. **Memory management**: Monitor memory usage with `profile_brain_simulation`
3. **Parallel processing**: Use `run_batch_simulation` for parallel experiments
4. **Configuration optimization**: Use `optimize_configuration` to find optimal parameters

### For Resource-Constrained Environments:

1. **Use smaller templates**: Start with "small" brains for initial testing
2. **Disable advanced features**: Turn off curiosity, development, etc. for speed
3. **Batch processing**: Run multiple simpler simulations instead of one complex one
4. **Configuration optimization**: Find optimal balance between complexity and performance

These advanced examples demonstrate the full power of the NLM Python bindings, enabling researchers and developers to conduct sophisticated neural network experiments with comprehensive control, analysis, and optimization capabilities.