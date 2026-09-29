# NLM Expert Python Guide

## Overview

This guide covers advanced Python features and commands for expert users of the NLM neural simulation framework. It builds on the beginner tutorials by showing how to use the advanced tools added to the Python bindings.

## New Expert Features

### 1. Performance Monitoring

#### BrainProfiler
Track and analyze brain performance with detailed metrics:

```python
import pynlm

# Create brain and configure monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create profiler for detailed analysis
profiler = pynlm.BrainProfiler(brain)
profiler.start()

# Run simulation steps
for step in range(1000):
    brain.step(step)
    
    # Get real-time performance metrics
    fps = profiler.get_steps_per_second()
    avg_step_time = profiler.get_average_step_time()
    memory_usage = profiler.get_memory_usage()

profiler.stop()

# Generate performance report
report = profiler.generate_report()
print(report)

# Find bottleneck regions
bottlenecks = profiler.find_performance_bottlenecks()
```

#### MemoryMonitor
Monitor memory usage and optimize memory consumption:

```python
import pynlm

# Create memory monitor for brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Start memory monitoring
monitor = pynlm.MemoryMonitor(brain)
monitor.start_monitoring()

# Run simulation
for step in range(500):
    brain.step(step)
    peak_memory = monitor.get_peak_memory_usage()
    current_memory = monitor.get_current_memory_usage()

monitor.stop_monitoring()

# Get memory optimization suggestions
suggestions = monitor.get_memory_optimization_suggestions()
print(suggestions)
```

### 2. Network Analysis

#### NetworkAnalyzer
Analyze brain connectivity and topology:

```python
import pynlm
import numpy as np

# Create brain and analyze network
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize network analyzer
analyzer = pynlm.NetworkAnalyzer(brain)

# Generate connectivity matrix (adjacency matrix)
connectivity_matrix = analyzer.calculate_connectivity_matrix()
print("Connectivity matrix shape:", connectivity_matrix.shape)

# Analyze network topology
topology = analyzer.analyze_network_topology()
print("Network density:", topology['density'])
print("Average clustering:", topology['average_clustering'])
print("Small-world coefficient:", topology['small_world_coefficient'])

# Find hubs and modules
hubs = analyzer.find_hubs(threshold=0.1)
modules = analyzer.find_modules()

# Calculate betweenness centrality
betweenness = analyzer.calculate_betweenness_centrality()
```

#### CompleteAgent
Create complete agent setups with configurable environments:

```python
import pynlm

# Create a complete agent with world
agent = pynlm.CompleteAgent()

# Configure environment
world = agent.get_world()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.setAgentStart(15.0, 15.0)
world.reset()

# Create brain with specialized configuration
brain_config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(brain_config)
brain.initialize()

# Initialize agent components
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Enable advanced learning features
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

# Start simulation with automatic data collection
simulation_data = agent.run_simulation(
    steps=2000,
    enable_monitoring=True,
    enable_data_collection=True
)

# Get performance analytics
analytics = simulation_data.get_performance_analytics()
print(f"Learning rate: {analytics['learning_rate']:.4f}")
print(f"Final curiosity: {analytics['final_curiosity']:.4f}")
```

### 3. Learning Curve Analysis

#### LearningCurveAnalyzer
Analyze learning dynamics and convergence:

```python
import pynlm
import matplotlib.pyplot as plt

# Create simulation with learning tracking
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable reward modulation for learning
agent = pynlm.createAgentBrain(brain)

# Create learning curve analyzer
learning_analyzer = pynlm.LearningCurveAnalyzer()

# Run learning simulation
learning_data = []
for step in range(1000):
    # Simulate environment interaction
    brain.step(step)
    
    # Simulate reward learning (simplified)
    reward = 0.5 if step % 50 == 0 else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Record learning metrics
    metrics = {
        'step': step,
        'firing_rate': brain.getAverageFiringRate(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError(),
        'reward': reward
    }
    learning_data.append(metrics)

# Analyze learning curves
analysis = learning_analyzer.analyze_learning_curves(learning_data)

# Plot learning curves
plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot([d['step'] for d in learning_data], 
         [d['firing_rate'] for d in learning_data])
plt.title('Firing Rate Over Time')
plt.xlabel('Step')
plt.ylabel('Firing Rate')

plt.subplot(2, 2, 2)
plt.plot([d['step'] for d in learning_data], 
         [d['curiosity'] for d in learning_data])
plt.title('Curiosity Level Over Time')
plt.xlabel('Step')
plt.ylabel('Curiosity')

plt.subplot(2, 2, 3)
plt.plot([d['step'] for d in learning_data], 
         [d['prediction_error'] for d in learning_data])
plt.title('Prediction Error Over Time')
plt.xlabel('Step')
plt.ylabel('Prediction Error')

plt.subplot(2, 2, 4)
plt.hist([d['reward'] for d in learning_data], bins=20, alpha=0.7)
plt.title('Reward Distribution')
plt.xlabel('Reward')
plt.ylabel('Frequency')

plt.tight_layout()
plt.show()

# Get learning statistics
statistics = analysis.get_learning_statistics()
print("Learning Statistics:")
print(f"  Convergence step: {statistics['convergence_step']}")
print(f"  Learning rate: {statistics['learning_rate']:.4f}")
print(f"  Stability score: {statistics['stability_score']:.4f}")
print(f"  Exploration efficiency: {statistics['exploration_efficiency']:.4f}")
```

### 4. Development Stage Analysis

#### DevelopmentStageAnalyzer
Analyze brain development and maturation:

```python
import pynlm

# Create developing brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create developmental stage analyzer
stage_analyzer = pynlm.DevelopmentStageAnalyzer()

# Simulate brain development
development_data = []
for step in range(5000):
    brain.step(step)
    
    stage_data = {
        'step': step,
        'developmental_stage': brain.getDevelopmentalStage(),
        'plasticity_modifier': 1.0,  # This would come from actual development system
        'neuron_count': brain.getTotalNeuronCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'synaptic_weight_std': 0.1,  # Example metric
        'memory_efficiency': 0.5,   # Example metric
        'learning_rate': 0.01       # Example metric
    }
    development_data.append(stage_data)

# Analyze development stages
stage_analysis = stage_analyzer.analyze_development_stages(development_data)

# Plot development trajectory
import matplotlib.pyplot as plt

plt.figure(figsize=(15, 10))

plt.subplot(3, 2, 1)
stages = [0, 1, 2, 3, 4]  # Initial, CriticalPeriod, Maturation, Adult, Aging
plt.bar(stages, [stage_analysis['plasticity_by_stage'][s] for s in stages])
plt.title('Plasticity by Developmental Stage')
plt.xlabel('Stage')
plt.ylabel('Plasticity Modifier')

plt.subplot(3, 2, 2)
stages = [0, 1, 2, 3, 4]
plt.plot(stages, [stage_analysis['average_firing_by_stage'][s] for s in stages], 'o-')
plt.title('Average Firing by Developmental Stage')
plt.xlabel('Stage')
plt.ylabel('Firing Rate')

plt.subplot(3, 2, 3)
stages = [0, 1, 2, 3, 4]
plt.plot(stages, [stage_analysis['learning_capacity_by_stage'][s] for s in stages], 'o-')
plt.title('Learning Capacity by Developmental Stage')
plt.xlabel('Stage')
plt.ylabel('Learning Capacity')

plt.subplot(3, 2, 4)
stages = [0, 1, 2, 3, 4]
plt.plot(stages, [stage_analysis['memory_efficiency_by_stage'][s] for s in stages], 'o-')
plt.title('Memory Efficiency by Developmental Stage')
plt.xlabel('Stage')
plt.ylabel('Memory Efficiency')

plt.subplot(3, 2, 5)
stages = [0, 1, 2, 3, 4]
plt.plot(stages, [stage_analysis['developmental_duration_by_stage'][s] for s in stages], 'o-')
plt.title('Developmental Duration by Stage')
plt.xlabel('Stage')
plt.ylabel('Duration (steps)')

plt.subplot(3, 2, 6)
stages = [0, 1, 2, 3, 4]
plt.plot(stages, [stage_analysis['critical_transition_points'][s] for s in stages], 'o-', 'r--')
plt.title('Critical Transition Points')
plt.xlabel('Stage')
plt.ylabel('Transition Score')

plt.tight_layout()
plt.show()

# Get developmental stage insights
insights = stage_analysis.get_development_insights()
print("Development Stage Insights:")
for key, value in insights.items():
    print(f"  {key}: {value}")
```

### 5. Advanced Configuration

#### ConfigValidator
Validate configuration with intelligent suggestions:

```python
import pynlm

# Create configuration validator
validator = pynlm.ConfigValidator()

# Test various configurations
test_configs = [
    {"brain.neuron_count": 100, "brain.synapse_density": 0.1},
    {"brain.neuron_count": 1000, "brain.synapse_density": 0.05},
    {"brain.neuron_count": 5000, "brain.synapse_density": 0.2},
    {"brain.neuron_count": 100, "plasticity.stdp.enable": True},
    {"brain.neuron_count": 1000, "neuromod.dopamine.scale": 2.0}
]

for i, config_dict in enumerate(test_configs):
    print(f"\nConfiguration {i+1}: {config_dict}")
    config = pynlm.createDefaultConfig()
    
    for key, value in config_dict.items():
        config.set(key, value)
    
    # Validate configuration
    is_valid, issues, suggestions = validator.validate(config)
    
    print(f"  Valid: {is_valid}")
    if issues:
        print("  Issues:")
        for issue in issues:
            print(f"    - {issue}")
    if suggestions:
        print("  Suggestions:")
        for suggestion in suggestions:
            print(f"    - {suggestion}")
```

#### CheckpointManager
Advanced checkpoint management:

```python
import pynlm
import time

# Create checkpoint manager
checkpoint_manager = pynlm.CheckpointManager("brain_checkpoints")

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Set up automatic checkpointing
checkpoint_manager.setup_automatic_checkpoints(
    interval=100,  # Save every 100 steps
    max_checkpoints=10,
    compression=True,
    backup=True
)

# Run simulation with automatic checkpointing
for step in range(1000):
    brain.step(step)
    
    # Checkpoint will be saved automatically
    if checkpoint_manager.should_checkpoint(step):
        print(f"Checkpoint saved at step {step}")

# Manual checkpointing
checkpoint_id = checkpoint_manager.save_checkpoint(brain, "manual_checkpoint")
print(f"Manual checkpoint saved with ID: {checkpoint_id}")

# Load from checkpoint
brain_copy = pynlm.createBrain(config)
brain_copy.load(checkpoint_id)

# Restore with version compatibility
checkpoint_manager.restore_checkpoint_with_version_check(
    brain_copy, 
    checkpoint_id,
    require_exact_version=True
)

# Get checkpoint statistics
stats = checkpoint_manager.get_checkpoint_statistics()
print(f"Checkpoints created: {stats['checkpoints_created']}")
print(f"Total checkpoint size: {stats['total_size_mb']:.2f} MB")
```

### 6. Advanced Analysis Functions

#### Comprehensive Brain Analysis
```python
import pynlm

# Create brain and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Perform comprehensive analysis
analysis_results = {
    'basic_statistics': {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'developmental_stage': brain.getDevelopmentalStage()
    },
    
    'performance_metrics': {
        'steps_per_second': 1.0,  # Would be measured with profiler
        'memory_usage': 0.0,      # Would be measured with monitor
        'stability_score': 0.8    # Example computed metric
    },
    
    'learning_dynamics': {
        'convergence_step': 250,
        'learning_rate': 0.01,
        'exploration_efficiency': 0.75,
        'memory_retention': 0.85
    }
}

# Print analysis report
print("=== NLM Brain Analysis Report ===")
for category, metrics in analysis_results.items():
    print(f"\n{category.upper().replace('_', ' ')}:")
    for key, value in metrics.items():
        print(f"  {key.replace('_', ' ').title()}: {value}")
```

### 7. Performance Optimization Tools

#### ProfileCompleteSimulation
Complete simulation with profiling and optimization:
```python
import pynlm

# Create optimized brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create brain with profiler
brain = pynlm.createBrain(config)
profiler = pynlm.BrainProfiler(brain)

# Run optimized simulation with monitoring
profiler.start()

for step in range(1000):
    brain.step(step)
    
    # Real-time optimization suggestions
    if profiler.get_memory_usage() > 0.8 * profiler.get_peak_memory_usage():
        print("Memory usage high - consider reducing brain size")
    
    if profiler.get_average_step_time() > 0.1:  # 100ms threshold
        print("Step time high - consider performance optimizations")

profiler.stop()

# Generate optimization report
optimization_report = profiler.generate_optimization_report()
print("\n=== Optimization Recommendations ===")
for recommendation in optimization_report['recommendations']:
    print(f"- {recommendation}")
```

## Best Practices for Expert Users

### 1. Performance Optimization
- Use BrainProfiler for detailed performance analysis
- Monitor memory usage with MemoryMonitor
- Optimize configuration for target use cases
- Profile before optimizing

### 2. Advanced Analysis
- Use NetworkAnalyzer for connectivity studies
- Analyze learning curves with LearningCurveAnalyzer
- Monitor development stages with DevelopmentStageAnalyzer
- Use CheckpointManager for state management

### 3. Production Considerations
- Implement proper error handling
- Use advanced validation with ConfigValidator
- Monitor performance in production
- Implement backup strategies for checkpoints

### 4. Research Applications
- Use CompleteAgent for complex experimental setups
- Analyze network properties for theoretical studies
- Track learning dynamics for research papers
- Use comprehensive analysis functions for data collection

## Installation and Setup

```bash
# Install with advanced features
pip install pynlm[advanced]

# Or install in development mode
pip install -e .[advanced]
```

## Example Research Application

Here's a complete example of using expert features for a research application:

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import time

class NeuralDevelopmentStudy:
    def __init__(self):
        self.profiler = pynlm.BrainProfiler(None)
        self.memory_monitor = pynlm.MemoryMonitor(None)
        self.network_analyzer = pynlm.NetworkAnalyzer(None)
        self.learning_analyzer = pynlm.LearningCurveAnalyzer()
        self.development_analyzer = pynlm.DevelopmentStageAnalyzer()
    
    def run_development_study(self, num_steps=2000):
        """Run a complete study of neural development"""
        
        # Create brain with specific configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("brain.synapse_density", 0.08)
        config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(config)
        
        # Initialize monitoring tools
        self.profiler.set_brain(brain)
        self.memory_monitor.set_brain(brain)
        self.network_analyzer.set_brain(brain)
        
        # Start monitoring
        self.profiler.start()
        self.memory_monitor.start_monitoring()
        
        # Data collection
        study_data = {
            'steps': [],
            'firing_rates': [],
            'learning_curves': [],
            'memory_usage': [],
            'network_metrics': [],
            'development_stages': []
        }
        
        # Run simulation
        for step in range(num_steps):
            brain.step(step)
            
            # Collect data
            study_data['steps'].append(step)
            study_data['firing_rates'].append(brain.getAverageFiringRate())
            study_data['memory_usage'].append(
                self.memory_monitor.get_current_memory_usage()
            )
            
            # Get development stage
            study_data['development_stages'].append(
                brain.getDevelopmentalStage()
            )
            
            # Sample network analysis every 100 steps
            if step % 100 == 0:
                network_metrics = {
                    'density': self.network_analyzer.calculate_connectivity_matrix().mean(),
                    'clustering': self.network_analyzer.analyze_network_topology()['average_clustering'],
                    'characteristic_path_length': self.network_analyzer.calculate_characteristic_path_length()
                }
                study_data['network_metrics'].append(network_metrics)
        
        # Stop monitoring
        self.profiler.stop()
        self.memory_monitor.stop_monitoring()
        
        # Analyze data
        analysis_results = self.analyze_study_data(study_data)
        
        return study_data, analysis_results
    
    def analyze_study_data(self, data):
        """Perform comprehensive analysis of study data"""
        
        analysis = {
            'performance': self.profiler.generate_optimization_report(),
            'learning_dynamics': self.learning_analyzer.analyze_learning_curves(data['learning_curves']),
            'network_properties': self.network_analyzer.calculate_network_properties(data['network_metrics']),
            'development_trajectory': self.development_analyzer.analyze_development_stages(data['development_stages']),
            'memory_efficiency': self.memory_monitor.get_efficiency_analysis(data['memory_usage'])
        }
        
        return analysis

# Run the study
study = NeuralDevelopmentStudy()
print("Running neural development study...")

start_time = time.time()
study_data, analysis = study.run_development_study(num_steps=1000)
end_time = time.time()

print(f"Study completed in {end_time - start_time:.2f} seconds")

# Generate comprehensive report
report = self.generate_research_report(study_data, analysis)
print(report)

# Save results for further analysis
import json
with open('neural_development_study.json', 'w') as f:
    json.dump({
        'data': study_data,
        'analysis': analysis,
        'metadata': {
            'duration': end_time - start_time,
            'steps': len(study_data['steps']),
            'timestamp': time.time()
        }
    }, f, indent=2)

return study_data, analysis

if __name__ == "__main__":
    study = NeuralDevelopmentStudy()
    data, analysis = study.run_development_study()
```

This guide provides comprehensive documentation for all advanced features available in the NLM Python bindings. Expert users can now leverage these tools for sophisticated neural network research and development.