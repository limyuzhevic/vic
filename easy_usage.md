# NLM Training Module

## Overview

This document provides comprehensive training examples for the NLM (Neural Learning Machine) training module. The training module provides a complete system for training NLM brains for various tasks while maintaining biological plausibility and performance of the underlying simulation.

## Training Module Components

The training module consists of several key components:

1. **TrainingCore Class**: Main training loop, evaluation, and hyperparameter optimization
2. **TrainingConfig Class**: Configuration for training runs (episodes, learning rates, stopping criteria)
3. **Callback and Monitoring System**: Training callbacks, progress monitoring, and performance metrics collection
4. **Advanced Training Features**: Curiosity-driven exploration, curriculum learning, transfer learning, and meta-learning
5. **Common Training Patterns**: Reinforcement learning, supervised learning, unsupervised learning, and hybrid training patterns
6. **Save/Load Training**: Training session serialization, checkpoint resumption, model persistence, and experiment tracking

## High-Level Convenience Functions

### create_simple_brain

```python
import pynlm as nlm

# Create a simple brain with optimal defaults
brain = nlm.create_simple_brain(neuron_count=1000, complexity=0.7)
brain.initialize()

# Use with additional configuration
brain = nlm.create_simple_brain(
    neuron_count=500,
    complexity=0.5,
    config={
        "brain": {
            "excitatory_neurons": 400,
            "inhibitory_neurons": 100,
            "v_thresh": -50.0
        }
    }
)
```

### create_standard_experiment

```python
import pynlm as nlm

# Create a standard experiment setup
experiment = nlm.create_standard_experiment(world_size=(20, 20))

brain = experiment['brain']
world = experiment['world']
agent = experiment['agent']

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### quick_test

```python
import pynlm as nlm

# Quick connectivity test
results = nlm.quick_test(mode='connectivity', steps=500)
if results['passed']:
    print("✓ Connectivity test passed!")
    print(f"  Average firing rate: {results['metrics'].get('firing_rate_mean', 0):.3f}")
else:
    print("✗ Test failed!")
    for error in results['errors']:
        print(f"  Error: {error}")

# Quick firing rate test
results = nlm.quick_test(mode='firing', steps=200)
if results['passed']:
    print("✓ Firing rate test passed!")
    print(f"  Firing rate: {results['metrics'].get('firing_rate_mean', 0):.3f}")

# Quick learning mode test
results = nlm.quick_test(mode='learning', steps=300)
if results['passed']:
    print("✓ Learning mode test passed!")
    print(f"  Developmental stage progression detected")
```

### train_agent

```python
import pynlm as nlm

# Define custom reward function
def custom_reward_function(world, brain, action, step):
    reward = 0.0
    
    # Reward for movement toward center
    if hasattr(world, 'getAgentBody'):
        body = world.getAgentBody()
        center_x = world.getWidth() / 2
        center_y = world.getHeight() / 2
        distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
        reward += 1.0 * max(0, 1.0 - distance / min(world.getWidth(), world.getHeight()))
    
    # Reward for learning activity
    firing_rate = brain.get_average_firing_rate()
    if firing_rate > 5.0:
        reward += firing_rate * 0.01
    
    # Penalty for excessive firing
    if firing_rate > 50.0:
        reward -= firing_rate * 0.05
    
    return reward

# Create standard experiment
experiment = nlm.create_standard_experiment(world_size=(15, 15))
brain = experiment['brain']
world = experiment['world']
agent = experiment['agent']

# Enable learning features
brain.enable_features(reward_modulation=True, curiosity=True)
agent.enable_learning(True)

# Train agent with custom reward
print("Starting training with custom reward function...")
training_results = nlm.train_agent(
    world, brain, episodes=200,
    reward_function=custom_reward_function,
    stop_criteria={
        'max_steps_per_episode': 100,
        'min_performance': 0.5,
        'patience': 10
    }
)

print(f"Training completed: {training_results['episodes_completed']} episodes")
print(f"Average reward: {training_results['average_reward']:.3f}")
print(f"Best performance: {training_results['best_performance']:.3f}")
```

### evaluate_performance

```python
import pynlm as nlm
from pynlm.training import SimpleTrainingCallback

# Create test scenarios
test_cases = [
    {
        'type': 'connectivity',
        'steps': 200,
        'target_metrics': {'min_active': 10},
        'weight': 1.0
    },
    {
        'type': 'firing',
        'steps': 100,
        'target_metrics': {'min_firing_rate': 1.0},
        'weight': 1.0
    },
    {
        'type': 'learning',
        'steps': 300,
        'target_metrics': {'stage_change': True},
        'weight': 1.0
    },
    {
        'type': 'memory',
        'steps': 400,
        'target_metrics': {'stability': 0.8},
        'weight': 1.0
    }
]

# Create brain
brain = nlm.create_simple_brain(neuron_count=1000, complexity=0.7)
brain.initialize()

# Evaluate performance
print("Evaluating brain performance...")
results = nlm.evaluate_performance(
    brain, 
    test_cases, 
    metrics=['speed', 'stability', 'accuracy', 'learning_rate']
)

print(f"Test cases evaluated: {results['test_cases_evaluated']}")
print(f"Overall performance: {results['overall_performance']:.3f}")
print(f"Performance score: {results['performance_scores']}")

if results['recommendations']:
    print("\nRecommendations:")
    for rec in results['recommendations']:
        print(f"  - {rec}")
```

### optimize_hyperparameters

```python
import pynlm as nlm

# Define hyperparameter search space
param_space = {
    'neuron_count': [500, 1000, 2000],
    'complexity': [0.3, 0.5, 0.7],
    'learning_rate': [0.001, 0.01, 0.1],
    'exploration_rate': [0.1, 0.3, 0.5],
    'discount_factor': [0.9, 0.95, 0.99],
}

# Create brain to optimize
brain = nlm.create_simple_brain(neuron_count=1000, complexity=0.5)

# Define evaluation function
def evaluation_func(brain_obj):
    """Custom evaluation function for hyperparameter optimization."""
    try:
        brain_obj.reset()
        brain_obj.initialize()
        
        # Run simulation
        for step in range(100):
            brain_obj.step(step)
        
        # Calculate score based on multiple metrics
        score = 0.0
        weight = 0.0
        
        if hasattr(brain_obj, 'getAverageFiringRate'):
            firing_rate = brain_obj.getAverageFiringRate()
            score += min(firing_rate / 20.0, 1.0)  # Normalize firing rate
            weight += 1.0
        
        if hasattr(brain_obj, 'getActiveNeuronCount'):
            active = brain_obj.getActiveNeuronCount()
            total = brain_obj.getTotalNeuronCount()
            if total > 0:
                score += (active / total)  # Active neuron ratio
                weight += 1.0
        
        if hasattr(brain_obj, 'getDevelopmentalStage'):
            stage = brain_obj.getDevelopmentalStage()
            # Map developmental stage to score (0.0 to 1.0)
            stage_score = 0.5  # Default
            if hasattr(stage, 'value'):
                stage_enum = stage.value
                if stage_enum == 'Adult':
                    stage_score = 1.0
                elif stage_enum == 'Maturation':
                    stage_score = 0.75
                elif stage_enum == 'CriticalPeriod':
                    stage_score = 0.5
            score += stage_score
            weight += 1.0
        
        if weight > 0:
            score = score / weight
        
        return score
        
    except Exception:
        return float('-inf')  # Invalid configuration

# Run hyperparameter optimization
print("Starting hyperparameter optimization...")
optimization_results = nlm.optimize_hyperparameters(
    brain, 
    param_space, 
    evaluation_func=evaluation_func,
    iterations=30
)

print(f"Optimization completed: {optimization_results['iterations']} iterations")
print(f"Best score: {optimization_results['best_score']:.4f}")

if optimization_results['best_parameters']:
    print("\nBest hyperparameters:")
    for param in optimization_results['best_parameters'].items():
        print(f"  {param[0]}: {param[1]}")

if optimization_results['recommendations']:
    print("\nOptimization recommendations:")
    for rec in optimization_results['recommendations']:
        print(f"  - {rec}")
```

## Complete Examples

### Example 1: Complete Learning System Setup

```python
import pynlm as nlm

print("=== Complete Learning System Setup ===")

# 1. Create brain with optimal configuration
print("1. Creating brain...")
brain = nlm.create_simple_brain(
    neuron_count=2000,
    complexity=0.7,
    config={
        "brain": {
            "excitatory_neurons": 1600,
            "inhibitory_neurons": 400,
            "v_thresh": -50.0,
            "tau_mem": 20.0
        },
        "plasticity": {
            "stdp": {"learning_rate": 0.001},
            "hebbian": {"enable": True}
        }
    }
)

# 2. Create world
print("2. Creating world...")
world = nlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=12,
    visionHeight=12
)
world.reset()
world.setAgentStart(15.0, 15.0)

# 3. Create agent interface
print("3. Creating agent interface...")
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable learning features
print("4. Enabling learning features...")
brain.enable_features(
    reward_modulation=True,
    structural_plasticity=True,
    development=True,
    curiosity=True
)
agent.enable_learning(True)

# 5. Define reward function
print("5. Defining reward function...")
def comprehensive_reward(world, brain, action, step):
    reward = 0.0
    
    # Movement reward (closer to center)
    if hasattr(world, 'getAgentBody'):
        body = world.getAgentBody()
        center_x = world.getWidth() / 2
        center_y = world.getHeight() / 2
        distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
        reward += 2.0 * max(0, 1.0 - distance / min(world.getWidth(), world.getHeight()))
    
    # Learning activity reward
    firing_rate = brain.get_average_firing_rate()
    if 5.0 < firing_rate < 30.0:
        reward += firing_rate * 0.02
    elif firing_rate < 2.0:
        reward -= 0.5
    
    # Novelty exploration reward
    novelty = agent.get_novelty_level() if hasattr(agent, 'get_novelty_level') else 0.0
    if novelty > 0.3:
        reward += novelty * 5.0
    
    # Survival reward
    energy = world.getMaxEnergy() if hasattr(world, 'getMaxEnergy') else 10.0
    if hasattr(world, 'getAgentEnergy'):
        agent_energy = world.getAgentEnergy()
        reward += (agent_energy / energy) * 3.0
    
    return reward

# 6. Quick test before training
print("6. Running quick validation test...")
validation_results = nlm.quick_test(
    mode='all',
    steps=200,
    brain=brain
)

if validation_results['passed']:
    print("✓ Validation test passed!")
else:
    print("✗ Validation test failed - check errors:")
    for error in validation_results['errors']:
        print(f"  - {error}")

# 7. Train agent
print("7. Training agent...")
training_results = nlm.train_agent(
    world, brain, episodes=500,
    reward_function=comprehensive_reward,
    stop_criteria={
        'max_steps_per_episode': 200,
        'min_performance': 0.8,
        'patience': 20,
        'improvement_threshold': 0.01
    }
)

print(f"Training completed: {training_results['episodes_completed']} episodes")
print(f"Average reward: {training_results['average_reward']:.3f}")
print(f"Best performance: {training_results['best_performance']:.3f}")

# 8. Evaluate performance
print("8. Evaluating trained agent...")
test_cases = [
    {'type': 'connectivity', 'steps': 300, 'target_metrics': {'min_active': 20}},
    {'type': 'firing', 'steps': 200, 'target_metrics': {'min_firing_rate': 2.0}},
    {'type': 'learning', 'steps': 400, 'target_metrics': {'stage_change': True}},
]

evaluation_results = nlm.evaluate_performance(brain, test_cases, metrics=['speed', 'stability', 'accuracy'])
print(f"Evaluation score: {evaluation_results['overall_performance']:.3f}")

# 9. Save experiment
print("9. Saving experiment...")
from pynlm.training import TrainingPersistence
persistence = TrainingPersistence()
persistence.saveTrainingSession(
    "comprehensive_experiment.nlm",
    brain, agent, training_results['episodes_completed']
)

print("✓ Complete learning system setup and training finished successfully!")
```

### Example 2: Rapid Prototyping with Convenience Functions

```python
import pynlm as nlm

print("=== Rapid Prototyping with Convenience Functions ===")

# 1. Quick setup and test
print("1. Quick setup and test...")
experiment = nlm.create_standard_experiment(world_size=(10, 10))
brain = experiment['brain']
world = experiment['world']
agent = experiment['agent']

# Quick validation
validation = nlm.quick_test(mode='connectivity', steps=100)
if not validation['passed']:
    print("Quick test failed - aborting")
    exit()

print("✓ Quick setup and test passed!")

# 2. Modify parameters easily
print("2. Modifying parameters...")
# Configure world for faster interaction
world.configure(
    width=8, height=8,
    visionWidth=6, visionHeight=6
)

# Configure brain for quick learning
brain = nlm.create_simple_brain(
    neuron_count=500,
    complexity=0.4,
    config={"brain": {"learning_rate": 0.01}}
)

# 3. Quick training session
print("3. Quick training session...")
def simple_reward(world, brain, action, step):
    return 1.0  # Simple fixed reward

training_results = nlm.train_agent(
    world, brain, episodes=50,
    reward_function=simple_reward,
    stop_criteria={'max_steps_per_episode': 50}
)

print(f"Quick training: {training_results['episodes_completed']} episodes completed")

# 4. Performance evaluation
print("4. Performance evaluation...")
test_cases = [{'type': 'firing', 'steps': 150, 'target_metrics': {'min_firing_rate': 1.0}}]
evaluation = nlm.evaluate_performance(brain, test_cases)

print(f"Performance: {evaluation['overall_performance']:.3f}")

# 5. Hyperparameter tuning (optional)
print("5. Optional hyperparameter tuning...")
param_space = {
    'neuron_count': [300, 500, 800],
    'complexity': [0.3, 0.6],
    'learning_rate': [0.001, 0.01]
}

# Note: Use with caution in production - for prototyping only
optimization_results = nlm.optimize_hyperparameters(
    brain, param_space, iterations=10
)

print(f"Best parameters found: {optimization_results['best_score']:.4f}")

print("✓ Rapid prototyping completed successfully!")
```

### Example 3: Experiment Tracking Workflow

```python
import pynlm as nlm
from pynlm.training import TrainingPersistence
import os
import json

print("=== Experiment Tracking Workflow ===")

# Setup directories
os.makedirs("experiments", exist_ok=True)

# 1. Create initial experiment
print("1. Creating initial experiment...")
experiment = nlm.create_standard_experiment(world_size=(20, 20))
brain = experiment['brain']
world = experiment['world']
agent = experiment['agent']

# Enable features
brain.enable_features(reward_modulation=True, curiosity=True)
agent.enable_learning(True)

# 2. Define experiment configuration
experiment_config = {
    'name': 'curiosity_driven_learning',
    'description': 'Learning with curiosity-driven exploration',
    'parameters': {
        'neuron_count': 1500,
        'world_size': (20, 20),
        'max_episodes': 1000,
        'learning_rate': 0.001
    },
    'objectives': ['maximize_reward', 'maintain_stability', 'encourage_exploration']
}

# 3. Create persistence manager
print("2. Setting up experiment persistence...")
persistence = TrainingPersistence("experiments")

# 4. Run training with automatic checkpointing
print("3. Running training with checkpointing...")

def exploration_reward(world, brain, action, step):
    """Reward function encouraging exploration and learning."""
    reward = 1.0  # Base reward
    
    # Curiosity bonus
    curiosity = agent.get_curiosity_level() if hasattr(agent, 'get_curiosity_level') else 0.0
    reward += curiosity * 2.0
    
    # Novelty bonus
    novelty = agent.get_novelty_level() if hasattr(agent, 'get_novelty_level') else 0.0
    reward += novelty * 1.5
    
    # Complexity bonus (for more complex behaviors)
    if brain.get_average_firing_rate() > 10.0:
        reward += brain.get_average_firing_rate() * 0.01
    
    return reward

training_results = nlm.train_agent(
    world, brain, episodes=500,
    reward_function=exploration_reward,
    stop_criteria={
        'max_steps_per_episode': 200,
        'min_performance': 0.7,
        'patience': 30
    }
)

# 5. Save experiment metadata
print("4. Saving experiment metadata...")
experiment_metadata = {
    'config': experiment_config,
    'results': training_results,
    'timestamp': nlm.getCurrentTime(),
    'environment': 'NLM Phase 6',
    'python_version': '.'.join(map(str, nlm.__version__.split('.')[:3]))
}

with open('experiments/experiment_metadata.json', 'w') as f:
    json.dump(experiment_metadata, f, indent=2)

print("✓ Experiment metadata saved!")

# 6. Load and analyze previous experiments
print("5. Loading and analyzing previous experiments...")
if os.path.exists('experiments/experiment_metadata.json'):
    with open('experiments/experiment_metadata.json', 'r') as f:
        metadata = json.load(f)
    
    print(f"Experiment: {metadata['config']['name']}")
    print(f"Description: {metadata['config']['description']}")
    print(f"Duration: {metadata['results']['episodes_completed']} episodes")
    print(f"Final performance: {metadata['results']['average_reward']:.3f}")

# 7. Compare with baseline
print("6. Comparing with baseline performance...")
# This would involve loading and comparing with previous runs
# For demonstration, we'll just show the structure
print("✓ Experiment tracking workflow completed!")

print("\n=== Experiment Tracking Summary ===")
print(f"• Experiment name: {experiment_config['name']}")
print(f"• Episodes completed: {training_results['episodes_completed']}")
print(f"• Final reward: {training_results['average_reward']:.3f}")
print(f"• Best performance: {training_results['best_performance']:.3f}")
print(f"• Checkpoints saved: {len(os.listdir('experiments'))}")
```

## Quick Reference

### Core Functions

| Function | Description | Example |
|----------|-------------|---------|
| `create_simple_brain()` | Create brain with optimal defaults | `brain = create_simple_brain(neuron_count=1000)` |
| `create_standard_experiment()` | Complete experiment setup | `experiment = create_standard_experiment(world_size=(15,15))` |
| `quick_test()` | Validate system functionality | `results = quick_test(mode='connectivity', steps=500)` |
| `train_agent()` | High-level training loop | `results = train_agent(world, brain, episodes=100)` |
| `evaluate_performance()` | Performance assessment | `results = evaluate_performance(brain, test_cases)` |
| `optimize_hyperparameters()` | Automated parameter tuning | `results = optimize_hyperparameters(brain, param_space, iterations=30)` |

### Common Workflows

#### Rapid Prototyping
1. Use `create_standard_experiment()` for quick setup
2. Use `quick_test()` for validation
3. Use `train_agent()` with simple reward function
4. Use `evaluate_performance()` to assess results

#### Production Training
1. Use `create_simple_brain()` for optimized configuration
2. Use `create_standard_experiment()` for complete setup
3. Define custom reward function
4. Use `train_agent()` with comprehensive stopping criteria
5. Use `evaluate_performance()` for detailed assessment
6. Use `optimize_hyperparameters()` for parameter tuning
7. Use experiment tracking for reproducibility

#### Research Experiments
1. Use `create_standard_experiment()` as base
2. Configure brain with specific parameters
3. Enable advanced features
4. Use `train_agent()` with custom reward functions
5. Use `quick_test()` for system validation
6. Use `evaluate_performance()` for comparative analysis
7. Use experiment tracking for documentation

## Installation

To use the training module, install the training dependencies:

```bash
pip install pynlm[training]
```

Or install all development dependencies:

```bash
pip install pynlm[dev]
```

## Getting Started

1. **Basic Training**: Start with Example 1 for complete system setup
2. **Quick Prototyping**: Use Example 2 for rapid development and testing
3. **Experiment Tracking**: Use Example 3 for research workflows and reproducibility
4. **Hyperparameter Optimization**: Use convenience functions for automated tuning
5. **Performance Evaluation**: Use evaluation functions for system assessment

## Troubleshooting

### Common Issues

**"Quick test fails"
- Check that C++ bindings are built
- Verify brain initialization with `brain.initialize()`
- Check system resources and memory
- Ensure all required features are enabled

**"Training takes too long"
- Reduce maxEpisodes in training config
- Increase stopping criteria patience
- Use hyperparameter optimization for better configurations
- Adjust reward function for faster learning

**"Evaluation fails"
- Ensure test cases are properly formatted
- Check that brain has required methods
- Verify performance metrics are available
- Check for runtime errors in evaluation code

**"Experiment tracking fails"
- Ensure write permissions to experiments directory
- Check that required dependencies are installed
- Verify file paths and naming conventions
- Ensure checkpoint format is compatible

### Solutions

**For "Quick test fails"
```python
# Verify all steps are properly implemented
brain = create_simple_brain(neuron_count=500)
brain.initialize()  # This must be called before any operations

# Run quick test
results = quick_test(mode='connectivity', steps=200)
```

**For "Training takes too long"
```python
training_config = TrainingConfig()
training_config.maxEpisodes = 100  # Reduced from 1000
training_config.minPerformance = 0.5  # Lower threshold
training_config.patience = 5  # Fewer patience steps
```

**For "Evaluation fails"
```python
test_cases = [
    {'type': 'connectivity', 'steps': 100, 'target_metrics': {'min_active': 5}},
    {'type': 'firing', 'steps': 50, 'target_metrics': {'min_firing_rate': 1.0}},
]

results = evaluate_performance(brain, test_cases, metrics=['speed', 'stability'])
```

## Next Steps

When you're comfortable with the training module:

1. **Integrate with your specific task** by creating custom reward functions
2. **Experiment with different training patterns** to find what works best for your use case
3. **Use hyperparameter optimization** to automate configuration discovery
4. **Set up experiment tracking** for reproducible research
5. **Combine multiple advanced features** for state-of-the-art training
6. **Deploy to production** using the training persistence and logging capabilities

The NLM Training Module provides a comprehensive, extensible system for training NLM brains for various tasks while maintaining biological plausibility and performance of the underlying simulation.

## License

See the LICENSE file in the root directory for full license information.

## Version

- Training Module Version: 1.0.0
- Documentation Version: 1.0.0
- Last Updated: 2026-09-27

## Acknowledgments

- Based on the NLM neural simulation framework
- Special thanks to the NLM development team
- Documentation generated from source code and examples