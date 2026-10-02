# Advanced Python APIs for NLM - Documentation

This documentation provides comprehensive information about the advanced Python APIs
for NLM (Neural Learning Machine), including usage examples, parameter descriptions,
and best practices.

## Table of Contents

1. [Overview](#overview)
2. [AdvancedBrainSimulation](#advancedbrainsimulation)
    - [run_stimulus_sequence()](#run_stimulus_sequence)
    - [run_adaptive_training()](#run_adaptive-training)
    - [run_brain_annealing()](#run_brain-annealing)
3. [BrainBatchProcessor](#brainbatchprocessor)
    - [create_brain_variants()](#create_brain_variants)
    - [run_batch_simulation()](#run_batch_simulation)
    - [run_hypothesis_test()](#run_hypothesis_test)
4. [BrainMLIntegration](#brainmlintegration)
    - [extract_neural_features()](#extract_neural_features)
    - [train_prediction_model()](#train_prediction_model)
    - [predict_brain_behavior()](#predict_brain_behavior)
5. [ExperimentManager](#experimentmanager)
    - [add_experiment()](#add_experiment)
    - [run_experiment()](#run_experiment)
    - [get_experiment_results()](#get_experiment_results)
    - [export_results()](#export_results)
6. [PerformanceMonitor](#performancemonitor)
    - [monitor_simulation()](#monitor_simulation)
    - [run_benchmark()](#run_benchmark)
    - [analyze_performance_trends()](#analyze_performance_trends)

## Overview

The advanced Python APIs provide powerful tools for complex NLM use cases,
research scenarios, and automated simulation workflows. These APIs extend
the basic NLM functionality with:

- **Advanced Simulation Control**: Automated experiments, adaptive training,
  optimization algorithms
- **Parallel Processing**: Batch processing for multiple brain configurations,
  hypothesis testing with statistical analysis
- **Machine Learning Integration**: Feature extraction, model training, prediction
- **Research Management**: Experiment design, result export, reproducibility
- **Performance Analysis**: Benchmarking, monitoring, trend analysis

## AdvancedBrainSimulation

The `AdvancedBrainSimulation` class provides high-level APIs for running complex
simulations with automated control and analysis.

### run_stimulus_sequence()

Run a predefined sequence of stimuli and analyze the brain's response.

```python
import pynlm
from pynlm import AdvancedBrainSimulation

# Define stimulus sequence
stimulus_sequence = [
    {
        'type': 'visual',
        'duration': 100,
        'intensity': 0.5,
        'pattern': 'random'
    },
    {
        'type': 'auditory',
        'duration': 200,
        'intensity': 0.8,
        'pattern': 'periodic'
    },
    {
        'type': 'tactile',
        'duration': 150,
        'intensity': 0.3,
        'pattern': 'burst'
    }
]

# Create simulation controller
brain = pynlm.create_brain_with_config(neuron_count=1000)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

world_simulator = pynlm.WorldSimulator(world)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

simulator = AdvancedBrainSimulation(brain, world)

# Run stimulus sequence
results = simulator.run_stimulus_sequence(stimulus_sequence, verbose=True)

print(f"Stimulus sequence completed with {len(results['results']['stimulus_types'])} stimuli")
print(f"Average response time: {results['analysis']['avg_response_time']:.2f} steps")
print(f"Adaptation score: {results['analysis']['adaptation_score']:.3f}")
```

**Parameters:**
- `stimulus_sequence`: List of stimulus dictionaries. Each stimulus can contain:
  - `type`: Stimulus type ('visual', 'auditory', 'tactile', etc.)
  - `duration`: Duration in simulation steps
  - `intensity`: Stimulus intensity (0.0 to 1.0)
  - `pattern`: Stimulus pattern ('random', 'periodic', 'burst', etc.)
  - Custom parameters: Any additional stimulus-specific parameters
- `agent`: Optional agent instance (creates default if None)
- `verbose`: Whether to print progress information

**Returns:**
- Dictionary containing:
  - `results`: Raw stimulus response data
  - `analysis`: Statistical analysis of sequence effectiveness
  - `training_history`: Complete training history for each stimulus

### run_adaptive_training()

Run adaptive training in a dynamic environment with evolving conditions.

```python
import pynlm
from pynlm import AdvancedBrainSimulation

# Define environment generator function
def dynamic_environment_generator():
    """Generate a dynamic environment with random characteristics."""
    import random
    
    return {
        'width': random.randint(15, 25),
        'height': random.randint(15, 25),
        'vision_range': random.randint(5, 10),
        'resource_density': random.uniform(0.1, 0.9),
        'hazard_probability': random.uniform(0.0, 0.3)
    }

# Create simulation controller
brain = pynlm.create_brain_with_config(neuron_count=2000)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

world_simulator = pynlm.WorldSimulator(world)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

simulator = AdvancedBrainSimulation(brain, world)

# Run adaptive training
training_results = simulator.run_adaptive_training(
    environment_generator=dynamic_environment_generator,
    max_episodes=1000,
    learning_rate=0.01,
    convergence_threshold=0.95,
    verbose=True
)

print(f"Training completed: {training_results['episodes_completed']} episodes")
print(f"Best performance: {training_results['best_performance']:.3f}")
print(f"Converged: {training_results['converged']}")
print(f"Final analysis: {training_results['final_analysis']}")
```

**Parameters:**
- `environment_generator`: Function that returns environment specifications
- `max_episodes`: Maximum number of training episodes
- `learning_rate`: Learning rate for adaptation (0.0 to 1.0)
- `convergence_threshold`: Performance threshold for early stopping (0.0 to 1.0)
- `verbose`: Whether to print progress information

**Returns:**
- Dictionary containing:
  - `training_log`: Complete training history
  - `final_analysis`: Statistical analysis of training results
  - `episodes_completed`: Number of episodes completed
  - `best_performance`: Best performance achieved
  - `converged`: Whether training converged

### run_brain_annealing()

Run brain annealing optimization to find optimal brain configurations.

```python
import pynlm
from pynlm import AdvancedBrainSimulation
import random

# Define objective function
def brain_objective_function(brain_state):
    """Example objective function for brain optimization."""
    # Simulate brain behavior and evaluate performance
    # This is a placeholder - implement your actual objective
    
    # Example: maximize firing rate while maintaining stability
    firing_rate = brain_state.get('firing_rate', 0.0)
    stability = brain_state.get('stability', 1.0)
    
    # Trade-off between high firing rate and stability
    return firing_rate * stability * 100

# Define temperature schedule (cooling schedule)
temperature_schedule = [
    100.0, 90.0, 80.0, 70.0, 60.0, 50.0, 40.0, 30.0, 20.0, 15.0,
    12.0, 10.0, 8.0, 6.0, 4.0, 3.0, 2.0, 1.5, 1.0, 0.8,
    0.6, 0.4, 0.3, 0.2, 0.1, 0.05, 0.02, 0.01, 0.005, 0.001
]

# Create simulation controller
brain = pynlm.create_brain_with_config(neuron_count=1500)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

world_simulator = pynlm.WorldSimulator(world)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

simulator = AdvancedBrainSimulation(brain, world)

# Run brain annealing
annealing_results = simulator.run_brain_annealing(
    temperature_schedule=temperature_schedule,
    objective_function=brain_objective_function,
    num_iterations=50,
    verbose=True
)

print(f"Brain annealing completed: {annealing_results['iterations_completed']} iterations")
print(f"Best score: {annealing_results['best_score']:.3f}")
print(f"Acceptance rate: {(annealing_results['annealing_log']['acceptances'] / 
                         (annealing_results['annealing_log']['acceptances'] + 
                          annealing_results['annealing_log']['rejections'])):.2%}")
```

**Parameters:**
- `temperature_schedule`: List of temperature values for each iteration
- `objective_function`: Function that evaluates brain state (returns score)
- `num_iterations`: Number of annealing iterations
- `verbose`: Whether to print progress information

**Returns:**
- Dictionary containing:
  - `annealing_log`: Complete annealing history
  - `final_analysis`: Statistical analysis of annealing results
  - `best_score`: Best score achieved
  - `iterations_completed`: Number of iterations completed

## BrainBatchProcessor

The `BrainBatchProcessor` class enables running multiple brain simulations in parallel
for comparative analysis and hyperparameter optimization.

### create_brain_variants()

Create multiple brain variants from a base configuration with different parameters.

```python
import pynlm
from pynlm import BrainBatchProcessor

# Base configuration template
base_config = {
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001
}

# Define variant specifications
variants = [
    {"name": "baseline", "brain.neuron_count": 1000, "connection_probability": 0.1},
    {"name": "high_frequency", "brain.neuron_count": 2000, "connection_probability": 0.15},
    {"name": "low_frequency", "brain.neuron_count": 500, "connection_probability": 0.05},
    {"name": "many_regions", "brain.region_count": 4, "connection_probability": 0.2},
    {"name": "few_regions", "brain.region_count": 1, "connection_probability": 0.05}
]

# Create batch processor
batch_processor = BrainBatchProcessor(max_workers=4)

# Create brain variants
brains = batch_processor.create_brain_variants(base_config, variants)

print(f"Created {len(brains)} brain variants:")
for i, brain_data in enumerate(brains):
    print(f"  {i}: {brain_data['parameters']['name']} - "
          f"{brain_data['metrics']['neuron_count']} neurons, "
          f"{brain_data['metrics']['connection_probability']:.2f} connectivity")
```

**Parameters:**
- `base_config`: Dictionary with base configuration parameters
- `variants`: List of variant dictionaries. Each variant can contain:
  - `name`: Variant name
  - Any configuration parameter to override

**Returns:**
- List of brain dictionaries with keys:
  - `id`: Brain ID
  - `config`: Complete configuration
  - `brain`: Brain instance
  - `parameters`: Variant parameters

### run_batch_simulation()

Run simulation for multiple brains in parallel and collect results.

```python
import pynlm
from pynlm import BrainBatchProcessor

# Create base configuration
base_config = {
    "brain.neuron_count": 1000,
    "brain.region_count": 2
}

# Define variants for hyperparameter search
variants = [
    {"name": "control", "connection_probability": 0.1},
    {"name": "hypothesis1", "connection_probability": 0.15},
    {"name": "hypothesis2", "connection_probability": 0.2},
    {"name": "hypothesis3", "connection_probability": 0.25}
]

# Create batch processor
batch_processor = BrainBatchProcessor(max_workers=3)

# Create brain variants
brains = batch_processor.create_brain_variants(base_config, variants)

# Run batch simulation
results = batch_processor.run_batch_simulation(
    brains,
    num_steps=200,
    metric_function=lambda brain: {
        'final_spike_rate': brain.getAverageFiringRate(),
        'stability': brain.getExcitationInhibitionRatio(),
        'development_progress': brain.getDevelopmentalStage()
    }
)

print("Batch simulation results:")
for result in results:
    print(f"  {result['parameters']['name']}:")
    print(f"    Final spike rate: {result['metrics']['final_spike_rate']:.3f}")
    print(f"    Stability (E/I ratio): {result['metrics']['stability']:.3f}")
    print(f"    Development stage: {result['metrics']['development_progress']}")
```

**Parameters:**
- `brains`: List of brain dictionaries (from create_brain_variants)
- `num_steps`: Number of simulation steps for each brain
- `metric_function`: Optional function to compute custom metrics

**Returns:**
- List of result dictionaries with keys:
  - `brain_id`: Brain identifier
  - `parameters`: Variant parameters
  - `metrics`: Computed metrics for each brain
  - `simulation_steps`: Number of steps simulated

### run_hypothesis_test()

Run statistical hypothesis tests comparing hypothesis and control brains.

```python
import pynlm
from pynlm import BrainBatchProcessor

# Create control and hypothesis brains
control_config = pynlm.createDefaultConfig()
control_brain = pynlm.createBrain(control_config)
control_brain.initialize()

hypothesis_config = pynlm.createDefaultConfig()
hypothesis_config.set("brain.neuron_count", 2000)
hypothesis_brain = pynlm.createBrain(hypothesis_config)
hypothesis_brain.initialize()

# Create batch processor
batch_processor = BrainBatchProcessor(max_workers=2)

# Run hypothesis test
results = batch_processor.run_hypothesis_test(
    hypothesis_brain=hypothesis_brain,
    control_brain=control_brain,
    test_metric='total_spikes',
    num_replicates=20
)

print("Hypothesis test results:")
print(f"  Hypothesis mean: {results['hypothesis_mean']:.1f} ± {results['hypothesis_std']:.1f}")
print(f"  Control mean: {results['control_mean']:.1f} ± {results['control_std']:.1f}")
print(f"  t-score: {results['t_score']:.3f}")
print(f"  p-value: {results['p_value']:.4f}")
print(f"  Significant: {results['significant']}")
print(f"  Effect size: {results['effect_size']:.3f}")
```

**Parameters:**
- `hypothesis_brain`: Brain instance representing the hypothesis
- `control_brain`: Brain instance representing the control
- `test_metric`: Metric to test ('total_spikes', 'average_firing_rate', etc.)
- `num_replicates`: Number of test replicates

**Returns:**
- Dictionary containing statistical test results:
  - `hypothesis_mean`: Mean of hypothesis test results
  - `hypothesis_std`: Standard deviation of hypothesis results
  - `control_mean`: Mean of control results
  - `control_std`: Standard deviation of control results
  - `t_score`: T-statistic value
  - `p_value`: P-value from t-test
  - `significant`: Whether difference is statistically significant (p < 0.05)
  - `effect_size`: Standardized effect size

## BrainMLIntegration

The `BrainMLIntegration` class provides APIs for machine learning applications
with NLM data.

### extract_neural_features()

Extract neural features from brain activity for machine learning.

```python
import pynlm
from pynlm import BrainMLIntegration

# Create ML integration
brain = pynlm.create_brain_with_config(neuron_count=1500)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

ml_integration = BrainMLIntegration(brain, world)

# Extract neural features
features = ml_integration.extract_neural_features(num_steps=100)

print("Extracted neural features:")
print(f"  Spike patterns: {len(features['spike_patterns'])} entries")
print(f"  Firing rates: {len(features['firing_rates'])} samples")
print(f"  Temporal features: {len(features['temporal_features'])} computed")
print(f"  Spatial features: {len(features['spatial_features'])} computed")
print(f"  Average firing rate: {sum(features['firing_rates'])/len(features['firing_rates']):.3f}")

# Prepare data for machine learning
X = []
y = []

for spike_pattern in features['spike_patterns']:
    # Extract features from spike pattern
    feature_vector = [
        spike_pattern['firing_count'],
        spike_pattern['total_spikes'] / (spike_pattern['step'] + 1),
        spike_pattern['step'] % 10,  # Step modulo for cyclic patterns
        sum(features['firing_rates'][-10:]) / 10  # Recent average firing rate
    ]
    
    X.append(feature_vector)
    y.append(features['firing_rates'][spike_pattern['step']])

print(f"Prepared {len(X)} feature vectors for ML training")
```

**Parameters:**
- `num_steps`: Number of simulation steps to run for feature extraction

**Returns:**
- Dictionary containing extracted features:
  - `spike_patterns`: List of spike pattern records
  - `firing_rates`: List of firing rates per step
  - `correlation_matrix`: Correlation matrix (if computable)
  - `spectrogram`: Frequency spectrum (if computable)
  - `temporal_features`: Extracted temporal features
  - `spatial_features`: Extracted spatial features
  - `connectivity_patterns`: Extracted connectivity patterns

### train_prediction_model()

Train a machine learning model to predict brain behavior.

```python
import pynlm
from pynlm import BrainMLIntegration
import numpy as np
from sklearn.ensemble import RandomForestRegressor

# Create ML integration
brain = pynlm.create_brain_with_config(neuron_count=1000)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

ml_integration = BrainMLIntegration(brain, world)

# Generate training data (example)
training_data = []
for i in range(50):
    # Create different brain configurations
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500 + i * 100)
    config.set("brain.region_count", 1 + (i % 3))
    
    train_brain = pynlm.createBrain(config)
    train_brain.initialize()
    
    # Run simulation
    for step in range(50):
        train_brain.step(step)
    
    # Extract features
    features = ml_integration.extract_neural_features(num_steps=50)
    
    # Create target (example: final spike count)
    target = train_brain.getTotalSpikeCount()
    
    training_data.append({
        'features': features,
        'target': target
    })

# Prepare data for ML
X = []
y = []

for data_point in training_data:
    # Simple feature extraction for ML
    features = data_point['features']
    feature_vector = [
        features['firing_rates'][-1] if features['firing_rates'] else 0,
        np.mean(features['firing_rates']) if features['firing_rates'] else 0,
        np.std(features['firing_rates']) if len(features['firing_rates']) > 1 else 0,
        len(features['spike_patterns']),
        data_point['brain'].getTotalNeuronCount() if 'brain' in data_point else 0
    ]
    
    X.append(feature_vector)
    y.append(data_point['target'])

# Train model
model = ml_integration.train_prediction_model(
    training_data=training_data,
    model_type='random_forest',
    hyperparameters={
        'n_estimators': 100,
        'max_depth': 10,
        'random_state': 42
    }
)

print(f"Trained model with {len(X)} samples")
print(f"Features per sample: {len(X[0])}")
print(f"Target range: {min(y):.0f} - {max(y):.0f}")
```

**Parameters:**
- `training_data`: List of training data dictionaries
- `model_type`: Type of ML model ('random_forest', 'neural_network', etc.)
- `hyperparameters`: Model hyperparameters

**Returns:**
- Trained machine learning model

### predict_brain_behavior()

Use trained model to predict brain behavior.

```python
import pynlm
from pynlm import BrainMLIntegration

# Create ML integration
brain = pynlm.create_brain_with_config(neuron_count=800)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

ml_integration = BrainMLIntegration(brain, world)

# Run simulation to get test data
for step in range(30):
    brain.step(step)

# Extract test features
test_features = ml_integration.extract_neural_features(num_steps=30)

# Prepare test data
test_data = [{
    'features': test_features,
    'brain': brain
}]

# Train a simple model for demonstration
from sklearn.linear_model import LinearRegression
X = [[1, 2, 3, 4, 5]]  # Dummy features
y = [100]  # Dummy target

model = LinearRegression()
model.fit(X, y)

# Make predictions
predictions = ml_integration.predict_brain_behavior(model, test_data)

print(f"Predictions: {predictions}")
```

**Parameters:**
- `model`: Trained machine learning model
- `test_data`: List of test data dictionaries

**Returns:**
- List of predictions (one per test data point)

## ExperimentManager

The `ExperimentManager` class provides APIs for designing, running, and analyzing
scientific experiments with NLM brains.

### add_experiment()

Add an experiment to the manager.

```python
import pynlm
from pynlm import ExperimentManager

# Create experiment manager
experiment_manager = ExperimentManager(
    name="NLM Brain Development Study",
    description="Study of brain development and learning dynamics"
)

# Create brains
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 1000)
brain1 = pynlm.createBrain(config1)
brain1.initialize()

config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 2000)
brain2 = pynlm.createBrain(config2)
brain2.initialize()

# Create worlds
world1 = pynlm.createSimpleWorld()
world1.configure(20, 20, 8, 8)
world1.reset()

world2 = pynlm.createSimpleWorld()
world2.configure(20, 20, 8, 8)
world2.reset()

# Define experiment protocols
standard_protocol = 'standard'
adaptive_protocol = 'adaptive'
ml_protocol = 'machine_learning'

# Add experiments
exp1_id = experiment_manager.add_experiment(
    brain=brain1,
    world=world1,
    protocol=standard_protocol,
    name="Standard Development",
    description="Standard developmental trajectory study"
)

exp2_id = experiment_manager.add_experiment(
    brain=brain2,
    world=world2,
    protocol=adaptive_protocol,
    name="Adaptive Learning",
    description="Study of adaptive learning in different environments"
)

exp3_id = experiment_manager.add_experiment(
    brain=brain1,
    world=world1,
    protocol=ml_protocol,
    name="Machine Learning Integration",
    description="Integration of ML techniques with NLM"
)

print(f"Added {len(experiment_manager.experiments)} experiments")
print(f"Experiment 1: {experiment_manager.experiments[0]['name']} (ID: {exp1_id})")
print(f"Experiment 2: {experiment_manager.experiments[1]['name']} (ID: {exp2_id})")
print(f"Experiment 3: {experiment_manager.experiments[2]['name']} (ID: {exp3_id})")
```

**Parameters:**
- `brain`: Brain instance
- `world`: World instance
- `protocol`: Experiment protocol ('standard', 'adaptive', 'machine_learning', etc.)
- `name`: Optional experiment name
- `description`: Optional experiment description

**Returns:**
- Experiment ID

### run_experiment()

Run a specific experiment.

```python
import pynlm
from pynlm import ExperimentManager

# Create experiment manager
experiment_manager = ExperimentManager("My Experiments")

# Setup experiment (simplified example)
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Add experiment
experiment_id = experiment_manager.add_experiment(
    brain=brain,
    world=world,
    protocol='standard',
    name="Test Experiment"
)

# Run experiment
print(f"Running experiment {experiment_id}...")
results = experiment_manager.run_experiment(experiment_id, verbose=True)

print("Experiment completed!")
print(f"Experiment status: {experiment_manager.experiments[experiment_id]['status']}")
if 'results' in experiment_manager.experiments[experiment_id]:
    print(f"Results: {experiment_manager.experiments[experiment_id]['results']}")
```

**Parameters:**
- `experiment_id`: Experiment ID to run
- `verbose`: Whether to print progress information

**Returns:**
- Experiment results dictionary

### get_experiment_results()

Get results for a specific experiment.

```python
import pynlm
from pynlm import ExperimentManager

# Create experiment manager
experiment_manager = ExperimentManager("My Experiments")

# Get experiment results
experiment_id = 0  # Example experiment ID
results = experiment_manager.get_experiment_results(experiment_id)

print(f"Experiment {experiment_id} results:")
print(f"  Name: {results['metadata']['name']}")
print(f"  Protocol: {results['metadata']['protocol']}")
print(f"  Status: {results['status']}")
if results['results']:
    print(f"  Results: {results['results']}")
```

**Parameters:**
- `experiment_id`: Experiment ID

**Returns:**
- Dictionary containing experiment metadata and results

### export_results()

Export experiment results to file.

```python
import pynlm
from pynlm import ExperimentManager

# Create experiment manager
experiment_manager = ExperimentManager("My Experiments")

# Add some experiments
for i in range(3):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000 + i * 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    experiment_id = experiment_manager.add_experiment(
        brain=brain,
        world=world,
        protocol='standard',
        name=f"Experiment_{i}"
    )
    
    # Run experiment
    experiment_manager.run_experiment(experiment_id)

# Export results to JSON file
export_path = "experiment_results.json"
experiment_manager.export_results(export_path, format='json')

print(f"Results exported to {export_path}")

# Export to CSV
experiment_manager.export_results("experiment_summary.csv", format='csv')
print(f"Summary exported to experiment_summary.csv")
```

**Parameters:**
- `filepath`: Output file path
- `format`: Export format ('json', 'csv')

## PerformanceMonitor

The `PerformanceMonitor` class provides APIs for monitoring simulation performance
and analyzing computational efficiency.

### monitor_simulation()

Monitor simulation performance.

```python
import pynlm
from pynlm import PerformanceMonitor

# Create performance monitor
monitor = PerformanceMonitor()

# Create brain
brain = pynlm.create_brain_with_config(neuron_count=1000)

# Monitor simulation
metrics = monitor.monitor_simulation(
    brain,
    num_steps=200,
    record_memory=True,
    record_cpu=True
)

print("Performance metrics:")
print(f"  Total time: {metrics['total_time']:.2f} seconds")
print(f"  Average step time: {metrics['avg_step_time']*1000:.2f} ms")
print(f"  Steps per second: {metrics['steps_per_second']:.1f}")
print(f"  Final firing rate: {metrics['final_firing_rate']:.3f}")
print(f"  Total spikes: {metrics['total_spikes']}")
if 'memory_increase_mb' in metrics:
    print(f"  Memory increase: {metrics['memory_increase_mb']:.1f} MB")
if 'cpu_usage' in metrics:
    print(f"  CPU usage: {metrics['cpu_usage']:.1%}")
```

**Parameters:**
- `brain`: Brain instance
- `num_steps`: Number of simulation steps
- `record_memory`: Whether to record memory usage
- `record_cpu`: Whether to record CPU usage

**Returns:**
- Dictionary containing performance metrics:
  - `total_time`: Total simulation time
  - `avg_step_time`: Average time per step
  - `steps_per_second`: Simulation speed
  - `final_firing_rate`: Final firing rate
  - `total_spikes`: Total spike count
  - `memory_increase_mb`: Memory increase in MB (if recorded)
  - `cpu_usage`: CPU usage percentage (if recorded)

### run_benchmark()

Run performance benchmark across different configurations.

```python
import pynlm
from pynlm import PerformanceMonitor

# Create performance monitor
monitor = PerformanceMonitor()

# Define benchmark configurations
configurations = {
    'small': {'brain.neuron_count': 500, 'brain.region_count': 1},
    'medium': {'brain.neuron_count': 1000, 'brain.region_count': 2},
    'large': {'brain.neuron_count': 2000, 'brain.region_count': 4},
    'xlarge': {'brain.neuron_count': 5000, 'brain.region_count': 8}
}

# Create base brain for testing
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Run benchmark
benchmark_results = monitor.run_benchmark(
    brain=brain,
    configurations=configurations,
    num_steps=100
)

print("Benchmark results:")
print(f"Best configuration: {benchmark_results['analysis']['best_configuration']}")
print(f"Best performance score: {benchmark_results['analysis']['best_score']:.3f}")
print(f"Number of configurations tested: {benchmark_results['analysis']['num_configurations']}")

# Show configuration improvements
if benchmark_results['analysis']['improvements']:
    print("\nConfiguration improvements:")
    for config, improvement in benchmark_results['analysis']['improvements'].items():
        print(f"  {config}: {improvement:+.1f}%")
```

**Parameters:**
- `brain`: Brain instance (template for all configurations)
- `configurations`: Dictionary of configuration names to parameter dictionaries
- `num_steps`: Number of simulation steps for each test

**Returns:**
- Dictionary containing benchmark results:
  - `configurations`: Results for each configuration
  - `analysis`: Statistical analysis of benchmark results

### analyze_performance_trends()

Analyze performance trends over time.

```python
import pynlm
from pynlm import PerformanceMonitor

# Create performance monitor
monitor = PerformanceMonitor()

# Simulate some performance data (in real usage, this would come from actual simulations)
monitor.performance_data = [
    {
        'timestamp': time.time() - 300,
        'metrics': {
            'avg_step_time': 0.015,
            'performance_trend': 0.01,
            'memory_increase_mb': 50,
            'cpu_usage': 0.3
        }
    },
    {
        'timestamp': time.time() - 240,
        'metrics': {
            'avg_step_time': 0.014,
            'performance_trend': 0.008,
            'memory_increase_mb': 55,
            'cpu_usage': 0.4
        }
    },
    {
        'timestamp': time.time() - 180,
        'metrics': {
            'avg_step_time': 0.013,
            'performance_trend': 0.006,
            'memory_increase_mb': 60,
            'cpu_usage': 0.5
        }
    }
]

# Analyze performance trends
trends = monitor.analyze_performance_trends(time_window=180)

print("Performance trends (last 180 seconds):")
print(f"  Step time trend: {trends.get('step_time_trend', {}).get('trend', 'unknown')}")
print(f"  Performance trend: {trends.get('performance_trend', {}).get('trend', 'unknown')}")
print(f"  Memory trend: {trends.get('memory_trend', {}).get('trend', 'unknown')}")
print(f"  CPU trend: {trends.get('cpu_trend', {}).get('trend', 'unknown')}")
```

**Parameters:**
- `time_window`: Time window for trend analysis (seconds)

**Returns:**
- Dictionary containing trend analysis:
  - `step_time_trend`: Trend analysis of step time
  - `performance_trend`: Trend analysis of performance
  - `memory_trend`: Trend analysis of memory usage
  - `cpu_trend`: Trend analysis of CPU usage

## Installation and Setup

### Prerequisites

- Python 3.8+
- pip
- C++ compiler and build tools (for C++ bindings)

### Installation

1. Clone the NLM repository:
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. Install Python dependencies:
   ```bash
   pip install -r requirements.txt
   ```

3. Build C++ bindings:
   ```bash
   pip install -e .
   ```

   Or build manually:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   pip install -e ../
   ```

4. Install advanced Python APIs:
   ```bash
   pip install -e .[advanced]
   ```

### Usage Examples

See the examples directory for comprehensive usage examples:

```
examples/
├── basic_usage.py           # Basic NLM usage
├── advanced_usage.py        # Advanced API usage
├── batch_processing.py       # Batch processing examples
├── machine_learning.py      # ML integration examples
├── experiment_design.py      # Experiment design examples
└── performance_analysis.py  # Performance analysis examples
```

## Additional Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **API Reference**: See the individual class and method documentation
- **Tutorials**: Check the examples and documentation for step-by-step guides

## Version Support

- **Python**: 3.8+
- **C++**: C++14+

## Contributing

Contributions to the advanced Python APIs are welcome! Please submit pull requests
with proper documentation and tests. Follow the existing code style and
add comprehensive docstrings for all new methods.

## License

MIT License

---

This documentation provides comprehensive information about the advanced Python
APIs for NLM (Neural Learning Machine). These APIs enable complex simulations,
research experiments, and performance analysis with the NLM framework.

For additional information or questions, please refer to the main NLM
documentation or contact the NLM development team.
