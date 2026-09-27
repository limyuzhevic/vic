# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API with high-level classes
4. Training and optimization examples

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Part 3: Using the NLM Python API (High-Level Interface)

### Basic Usage with High-Level Classes

#### Creating a Brain with NLMBrain

```python
import pynlm as nlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Creating an Experiment with NLMExperiment

```python
import pynlm as nlm

# Create an experiment with standard setup
experiment = nlm.NLMExperiment.create_standard(
    world_size=(20, 20),
    config={
        "brain": {
            "neuron_count": 2000,
            "complexity": 0.7
        }
    }
)

brain = experiment.get_brain()
world = experiment.get_world()
agent = experiment.get_agent()

# Enable learning features
experiment.enable_features(
    reward_modulation=True,
    structural_plasticity=True,
    development=True,
    curiosity=True
)

print("Experiment setup complete!")
```

#### Running a Complete Simulation

```python
import pynlm as nlm

# Create experiment
experiment = nlm.NLMExperiment.create_standard(world_size=(15, 15))

# Run simulation
results = experiment.run(num_steps=1000)

# Access results
print(f"Final firing rate: {results.metrics['final_firing_rate']:.3f}")
print(f"Total spikes: {results.metrics['final_spikes']}")
print(f"Average firing rate: {results.metrics['average_firing_rate']:.3f}")

# Save results to file
with open('experiment_results.json', 'w') as f:
    json.dump(results.to_dict(), f, indent=2)
```

### Advanced Training Examples

#### Example 1: Basic Reinforcement Learning with High-Level API

```python
import pynlm as nlm
from pynlm.training import TrainingConfig, SimpleTrainingCallback

print("=== Example 1: Basic Reinforcement Learning ===")

# Create training setup using the high-level API
trainer, brain, world, agent = nlm.create_training_setup()

# Configure basic training
training_config = TrainingConfig()
training_config.maxEpisodes = 500
training_config.learningRate = 0.001
training_config.discountFactor = 0.99
training_config.explorationRate = 0.1

# Create simple training callback
class BasicCallback(SimpleTrainingCallback):
    def afterEpisode(self, episode, reward, success):
        if episode % 50 == 0:
            print(f"Episode {episode}: Reward = {reward:.2f}, Success = {success}")
    
    def afterStep(self, episode, step, reward, loss):
        pass

# Enable features for RL
brain.enable_features(reward_modulation=True, curiosity=True)
world.enable_features(exploration=True)
agent.enable_learning(True)

# Run training
print("Starting basic reinforcement learning training...")
callback = BasicCallback()
training_complete = trainer.trainAgent(brain, world, agent, training_config, [callback])

if training_complete:
    print("✓ Basic training completed successfully!")
else:
    print("✗ Training was stopped early")

# Evaluate agent performance
print("Evaluating agent performance...")
performance = trainer.evaluateAgent(brain, world, agent, training_config, 20)
print(f"Average evaluation reward: {performance:.3f}")
```

#### Example 2: Advanced Training with Callbacks and Monitoring

```python
import pynlm as nlm
from pynlm.training import TrainingConfig, SimpleTrainingCallback

print("=== Example 2: Advanced Training with Monitoring ===")

# Create training setup
trainer, brain, world, agent = nlm.create_training_setup()

# Configure advanced training
training_config = TrainingConfig()
training_config.maxEpisodes = 1000
training_config.learningRate = 0.001
training_config.explorationRate = 0.1
training_config.rewardConfig.type = nlm.RewardConfig.Type.WEIGHTED
training_config.rewardConfig.weighted.stepCompletion = 1.0
training_config.rewardConfig.weighted.novelty = 0.5
training_config.rewardConfig.weighted.survival = 0.8
training_config.rewardConfig.weighted.efficiency = 1.0

# Create callback that tracks performance
class PerformanceTrackingCallback(SimpleTrainingCallback):
    def __init__(self):
        super().__init__()
        self.episode_rewards = []
        self.step_losses = []
    
    def afterEpisode(self, episode, reward, success):
        self.episode_rewards.append(reward)
        super().afterEpisode(episode, reward, success)
    
    def afterStep(self, episode, step, reward, loss):
        self.step_losses.append(loss)
        super().afterStep(episode, step, reward, loss)
    
    def getPerformanceSummary(self):
        if not self.episode_rewards:
            return "No performance data"
        
        avg_reward = sum(self.episode_rewards) / len(self.episode_rewards)
        avg_loss = sum(self.step_losses) / len(self.step_losses) if self.step_losses else 0
        
        return {
            'episodes_completed': len(self.episode_rewards),
            'average_reward': avg_reward,
            'average_loss': avg_loss,
            'best_reward': max(self.episode_rewards),
            'worst_reward': min(self.episode_rewards)
        }

# Enable advanced features
class AdvancedFeaturesCallback(SimpleTrainingCallback):
    def afterEpisode(self, episode, reward, success):
        # Enable curriculum learning based on performance
        if success and episode % 100 == 0:
            print(f"Curriculum progression: Episode {episode} successful")
        
        # Log advanced feature activation
        if agent.isCuriosityEnabled():
            curiosity = agent.getCuriosityLevel()
            if curiosity > 0.5:
                print(f"High curiosity detected: {curiosity:.2f}")
        
        super().afterEpisode(episode, reward, success)

# Enable all features
brain.enable_features(
    reward_modulation=True,
    structural_plasticity=True,
    development=True,
    curiosity=True
)

world.enable_features(
    exploration=True,
    reward=True,
    novelty=True
)

agent.enable_learning(True)

# Run advanced training
print("Starting advanced training with monitoring...")
performance_callback = PerformanceTrackingCallback()
advanced_callback = AdvancedFeaturesCallback()

training_complete = trainer.trainAgent(
    brain, world, agent, training_config,
    [performance_callback, advanced_callback]
)

if training_complete:
    print("✓ Advanced training completed successfully!")
    summary = performance_callback.getPerformanceSummary()
    print(f"Performance Summary: {summary}")
else:
    print("✗ Training was stopped early")

# Evaluate agent performance
print("Evaluating agent performance...")
performance = trainer.evaluateAgent(brain, world, agent, training_config, 20)
print(f"Average evaluation reward: {performance:.3f}")
```

#### Example 3: Hyperparameter Optimization

```python
import pynlm as nlm
from pynlm.training import HyperparameterOptimizer, OptimizationStrategy

print("=== Example 3: Hyperparameter Optimization ===")

# Define hyperparameter search space
param_space = [
    {"name": "learning_rate", "value": 0.001, "min": 0.0001, "max": 0.01},
    {"name": "discount_factor", "value": 0.99, "min": 0.9, "max": 0.999},
    {"name": "exploration_rate", "value": 0.1, "min": 0.01, "max": 0.5},
    {"name": "batch_size", "value": 32, "min": 16, "max": 128, "discrete": True},
    {"name": "hidden_size", "value": 256, "min": 128, "max": 1024, "discrete": True},
]

# Define evaluation function that runs a training session
def evaluate_training_params(params):
    print(f"Testing params: LR={params['learning_rate']}, DF={params['discount_factor']}, ER={params['exploration_rate']}")
    
    # Create training setup
    trainer, brain, world, agent = nlm.create_training_setup()
    
    # Configure training with these hyperparameters
    training_config = nlm.TrainingConfig()
    training_config.maxEpisodes = 100  # Short evaluation for speed
    training_config.learningRate = params['learning_rate']
    training_config.discountFactor = params['discount_factor']
    training_config.explorationRate = params['exploration_rate']
    
    # Run training (simplified for optimization)
    callback = SimpleTrainingCallback()
    success = trainer.trainAgent(brain, world, agent, training_config, [callback])
    
    if success:
        # Calculate performance score
        # In a real implementation, you would collect final reward or other metrics
        return 85.5  # Placeholder score
    else:
        return 0.0  # Penalty for failed training

# Create optimizer and run optimization
print("Starting hyperparameter optimization...")
optimizer = HyperparameterOptimizer()

results = optimizer.optimize(
    param_space,
    evaluate_training_params,
    OptimizationStrategy.RANDOM_SEARCH,
    maxIterations=50
)

# Print top results
print("\nTop 5 hyperparameter configurations:")
print("Rank | Score | Learning Rate | Discount Factor | Exploration Rate")
print("-----|-------|---------------|-----------------|------------------")
for i, (params, score) in enumerate(results[:5]):
    print(f"{i+1:4d} | {score:5.2f} | {params['learning_rate']:13.6f} | {params['discount_factor']:15.6f} | {params['exploration_rate']:18.6f}")

# Get best hyperparameters
best_params = results[0]
print(f"\nBest hyperparameters:")
for param in best_params.first:
    print(f"  {param['name']}: {param['value']}")
```

#### Example 4: Common Training Patterns

```python
import pynlm as nlm
from pynlm.training import SimpleTrainingConfig, CommonTrainingPatterns, SimpleTrainingCallback

print("=== Example 4: Common Training Patterns ===")

# Create training setup
trainer, brain, world, agent = nlm.create_training_setup()

# Enable all features
brain.enable_features(reward_modulation=True, curiosity=True)
agent.enable_learning(True)

# Create callback
from pynlm.training import SimpleTrainingCallback
callback = SimpleTrainingCallback()

print("=== Running Reinforcement Learning Pattern ===")
# Configure RL pattern
rl_config = SimpleTrainingConfig.RLPattern()
rl_config.gamma = 0.99
rl_config.epsilon = 0.1
rl_config.learningRate = 0.001
rl_config.maxSteps = 1000
rl_config.updateInterval = 10

# Enable curiosity for RL
if agent.isCuriosityEnabled():
    trainer.addCuriosityDrivenExplorationPattern(brain, world)

# Run RL pattern
rl_result = CommonTrainingPatterns.runReinforcementLearningPattern(
    trainer, brain, world, agent, rl_config, callback
)
print(f"RL Pattern Result: {'Success' if rl_result else 'Failed'}")

print("\n=== Running Supervised Learning Pattern ===")
# Configure SL pattern
sl_config = SimpleTrainingConfig.SupervisedPattern()
sl_config.epochs = 100
sl_config.learningRate = 0.001
sl_config.validationSplit = 0.2
sl_config.earlyStopping = True
sl_config.patience = 10

# Run SL pattern
sl_result = CommonTrainingPatterns.runSupervisedLearningPattern(
    trainer, brain, world, agent, sl_config, callback
)
print(f"SL Pattern Result: {'Success' if sl_result else 'Failed'}")

print("\n=== Running Unsupervised Learning Pattern ===")
# Configure US pattern
us_config = SimpleTrainingConfig.UnsupervisedPattern()
us_config.epochs = 50
us_config.reconstructionLossWeight = 1.0
us_config.regularizationWeight = 0.01
us_config.useNegativeSamples = True

# Run US pattern
us_result = CommonTrainingPatterns.runUnsupervisedLearningPattern(
    trainer, brain, world, agent, us_config, callback
)
print(f"US Pattern Result: {'Success' if us_result else 'Failed'}")

print("\n=== Running Hybrid Training Pattern ===")
# Configure hybrid pattern
hybrid_config = SimpleTrainingConfig.HybridPattern()
hybrid_config.rlWeight = 0.7
hybrid_config.slWeight = 0.3
hybrid_config.interleavedEpisodes = 10

# Run hybrid pattern
hybrid_result = CommonTrainingPatterns.runHybridTrainingPattern(
    trainer, brain, world, agent, hybrid_config, callback
)
print(f"Hybrid Pattern Result: {'Success' if hybrid_result else 'Failed'}")
```

#### Example 5: Experiment Tracking and Persistence

```python
import pynlm as nlm
from pynlm.training import TrainingPersistence

print("=== Example 5: Experiment Tracking and Persistence ===")

# Create training setup
trainer, brain, world, agent = nlm.create_training_setup()

# Configure training
training_config = nlm.TrainingConfig()
training_config.maxEpisodes = 500
training_config.checkpointFrequency = 50

# Create callback that tracks experiments
class ExperimentTrackingCallback:
    def __init__(self):
        self.experiments = []
        self.checkpoints = []
    
    def afterEpisode(self, episode, reward, success):
        # Create experiment for this episode
        experiment = {
            'episode': episode,
            'reward': reward,
            'success': success,
            'timestamp': nlm.getCurrentTime()  # Assuming this function exists
        }
        self.experiments.append(experiment)
        
        # Save checkpoint every 50 episodes
        if episode % 50 == 0:
            checkpoint_path = f"experiments/checkpoint_ep{episode}.nlm"
            if trainer.saveTrainingSession(checkpoint_path, brain, agent, episode):
                self.checkpoints.append(checkpoint_path)
                print(f"✓ Checkpoint saved: {checkpoint_path}")
    
    def onTrainingEnd(self):
        print(f"✓ Training completed. {len(self.experiments)} experiments recorded.")
        print(f"✓ {len(self.checkpoints)} checkpoints saved.")

# Enable features
brain.enable_features(reward_modulation=True, curiosity=True)
agent.enable_learning(True)

# Run training with experiment tracking
print("Starting training with experiment tracking...")
experiment_callback = ExperimentTrackingCallback()

training_complete = trainer.trainAgent(
    brain, world, agent, training_config,
    [experiment_callback]
)

# Save complete experiment
if training_complete:
    print("\nSaving complete experiment...")
    if trainer.saveTrainingSession(
        "final_experiment.nlm",
        brain, agent,
        experiment_callback.experiments[-1]['episode']
    ):
        print("✓ Complete experiment saved successfully")
        
        # Load experiment for analysis
        print("Loading experiment for analysis...")
        loaded_trainer = nlm.createTrainingCore()
        loaded_brain = nlm.cloneBrain(brain)
        loaded_agent = nlm.cloneAgent(agent)
        loaded_episode = experiment_callback.experiments[-1]['episode']
        
        if trainer.loadTrainingSession(
            "final_experiment.nlm",
            loaded_trainer, loaded_brain, loaded_agent,
            loaded_episode
        ):
            print("✓ Experiment loaded successfully")
            
            # Analyze experiment data
            print("\nExperiment Analysis:")
            print(f"Total episodes: {len(experiment_callback.experiments)}")
            print(f"Successful episodes: {sum(1 for exp in experiment_callback.experiments if exp['success'])}")
            print(f"Average reward: {sum(exp['reward'] for exp in experiment_callback.experiments) / len(experiment_callback.experiments):.2f}")
            print(f"Best reward: {max(exp['reward'] for exp in experiment_callback.experiments):.2f}")
            print(f"Checkpoints saved: {len(experiment_callback.checkpoints)}")
```

#### Example 6: Advanced Features Integration

```python
import pynlm as nlm
from pynlm.training import TrainingConfig, AdvancedTrainingFeatures, SimpleTrainingCallback

print("=== Example 6: Advanced Features Integration ===")

# Create training setup
trainer, brain, world, agent = nlm.create_training_setup()

# Enable advanced training features
features = AdvancedTrainingFeatures()

# Configure curiosity-driven exploration
features.enableCuriosityDrivenExploration()
features.curiosityConfig.explorationBonus = 0.1
features.curiosityConfig.enableIntrinsicMotivation = True

# Configure curriculum learning
features.enableCurriculumLearning()
features.curriculumConfig.enableCurriculum = True
features.curriculumConfig.initialDifficulty = 1
features.curriculumConfig.maxDifficulty = 10
features.curriculumConfig.successThreshold = 0.8

# Configure transfer learning
features.enableTransferLearning("source_task")
features.transferConfig.enableTransfer = True
features.transferConfig.transferWeight = 0.5

# Configure meta-learning
features.enableMetaLearning()
features.metaLearningConfig.enableMetaLearning = True
features.metaLearningConfig.metaSteps = 10

# Enable features on brain and world
brain.enable_features(reward_modulation=True, curiosity=True)
world.enable_features(exploration=True, reward=True)
agent.enable_learning(True)

# Create callback that uses advanced features
class AdvancedFeaturesCallback(SimpleTrainingCallback):
    def __init__(self, features):
        super().__init__()
        self.features = features
        self.episode_count = 0
    
    def afterEpisode(self, episode, reward, success):
        self.episode_count = episode
        
        # Monitor and adjust advanced features based on performance
        if self.features.isCurriculumEnabled() and success:
            # Progress curriculum based on success
            print(f"Curriculum progression: Episode {episode} successful")
        
        if self.features.isCuriosityEnabled():
            curiosity = agent.getCuriosityLevel()
            if curiosity > 0.7:
                print(f"High curiosity level detected: {curiosity:.2f}")
        
        super().afterEpisode(episode, reward, success)

# Configure training with advanced features
training_config = TrainingConfig()
training_config.maxEpisodes = 1000
training_config.learningRate = 0.001
training_config.explorationRate = 0.1

# Run training with advanced features
print("Starting training with advanced features...")
advanced_callback = AdvancedFeaturesCallback(features)

training_complete = trainer.trainAgent(
    brain, world, agent, training_config,
    [advanced_callback]
)

if training_complete:
    print("✓ Advanced training completed successfully!")
    
    # Get feature status
    print("\nAdvanced Features Status:")
    print(f"Curiosity-driven exploration: {'Enabled' if features.isCuriosityEnabled() else 'Disabled'}")
    print(f"Curriculum learning: {'Enabled' if features.isCurriculumEnabled() else 'Disabled'}")
    print(f"Transfer learning: {'Enabled' if features.isTransferEnabled() else 'Disabled'}")
    print(f"Meta-learning: {'Enabled' if features.isMetaLearningEnabled() else 'Disabled'}")
    
    if features.isCuriosityEnabled():
        print(f"Current curiosity level: {agent.getCuriosityLevel():.2f}")
    
    if features.isCurriculumEnabled():
        print(f"Current developmental stage: {agent.getDevelopmentalStage()}")
else:
    print("✗ Training was stopped early")
```

## Key Classes and Methods (High-Level API)

### TrainingCore Class

```python
# Creation
trainer = nlm.createTrainingCore()

# Training
trainer.trainAgent(brain, world, agent, training_config, callbacks)
trainer.evaluateAgent(brain, world, agent, training_config, num_episodes)

# Advanced features
trainer.addCuriosityDrivenExplorationPattern(brain, world)
```

### TrainingConfig Class

```python
# Create
training_config = nlm.TrainingConfig()

# Query and modify
if training_config.maxEpisodes:
    print(f"Max episodes: {training_config.maxEpisodes}")

training_config.maxEpisodes = 1000
training_config.learningRate = 0.001
training_config.explorationRate = 0.1

# Reward configuration
training_config.rewardConfig.type = nlm.RewardConfig.Type.WEIGHTED
training_config.rewardConfig.weighted.stepCompletion = 1.0
```

### TrainingCallback Classes

```python
# Create custom callback
class MyCallback(SimpleTrainingCallback):
    def afterEpisode(self, episode, reward, success):
        # Custom logic here
        pass

# Create callback instance and register
callback = MyCallback()
trainer.trainAgent(brain, world, agent, training_config, [callback])
```

### CommonTrainingPatterns

```python
# Use predefined patterns
from pynlm.training import CommonTrainingPatterns

# RL pattern
CommonTrainingPatterns.runReinforcementLearningPattern(
    trainer, brain, world, agent, rl_config, callback
)

# SL pattern
CommonTrainingPatterns.runSupervisedLearningPattern(
    trainer, brain, world, agent, sl_config, callback
)

# US pattern
CommonTrainingPatterns.runUnsupervisedLearningPattern(
    trainer, brain, world, agent, us_config, callback
)

# Hybrid pattern
CommonTrainingPatterns.runHybridTrainingPattern(
    trainer, brain, world, agent, hybrid_config, callback
)
```

### TrainingPersistence

```python
# Save training session
from pynlm.training import TrainingPersistence

persistence = TrainingPersistence()
persistence.saveTrainingSession("checkpoint_ep50.nlm", brain, agent, 50)

# Load training session
persistence.loadTrainingSession("checkpoint_ep50.nlm", trainer, brain, agent, 50)
```

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

1. **Basic Training**: Start with Example 1 for simple reinforcement learning
2. **Advanced Training**: Use Example 2 for training with callbacks and monitoring
3. **Hyperparameter Optimization**: Use Example 3 for automated hyperparameter tuning
4. **Common Patterns**: Use Example 4 to experiment with different training patterns
5. **Experiment Tracking**: Use Example 5 for persistent experiment tracking
6. **Advanced Features**: Use Example 6 to integrate curiosity-driven exploration, curriculum learning, transfer learning, and meta-learning

## Troubleshooting

### Common Issues

**"Training is taking too long"**
- Reduce maxEpisodes or increase stopping criteria
- Use hyperparameter optimization to find better configurations
- Adjust learning rate or exploration rate

**"Callbacks not being called"**
- Ensure callbacks are properly registered
- Check that training configuration enables the features callbacks expect
- Verify callback order and implementation

**"Saving training sessions fails"**
- Ensure sufficient disk space
- Check file permissions
- Verify that required dependencies are installed
- Consider using compressed checkpoints for large brains

**"Advanced features not working"**
- Verify that the agent supports the required features
- Check that the world simulation supports the advanced feature modules
- Ensure feature flags are properly enabled
- Debug feature configuration

## Next Steps

When you're comfortable with the training module:

1. **Integrate with your specific task** by creating custom reward functions
2. **Experiment with different training patterns** to find what works best for your use case
3. **Use hyperparameter optimization** to automate configuration discovery
4. **Set up experiment tracking** for reproducible research
5. **Combine multiple advanced features** for state-of-the-art training
6. **Deploy to production** using the training persistence and logging capabilities

The NLM Training Module provides a comprehensive, extensible system for training NLM brains for various tasks while maintaining biological plausibility and performance of the underlying simulation.