# Neural Learning Machine - Training Loop Utilities

## Overview

This document provides comprehensive information about the new training loop utilities added to the Neural Learning Machine (NLM) Python bindings. These utilities provide a complete framework for reinforcement learning, curriculum learning, and simulation scheduling.

## Introduction

The NLM Python bindings have been enhanced with comprehensive training loop utilities that support:

- **Reinforcement Learning**: Multiple algorithms including Q-learning, policy gradients, and deep neural networks
- **Curriculum Learning**: Progressive difficulty adaptation for structured learning
- **Simulation Scheduling**: Advanced resource management and experiment orchestration
- **Experience Replay**: Memory mechanisms for learning from past experiences
- **Model Persistence**: Save/load functionality for training states
- **Distributed Training**: Support for multi-agent and multi-process training

## Core Classes

### 1. TrainingLoop Class

**Purpose**: Core training loop framework for neural network training

**Properties**:
- `agent`: Agent brain instance
- `environment`: Simulation environment
- `config`: Training configuration
- `policy`: Neural network policy
- `optimizer`: Reinforcement learning optimizer

**Methods**:
- `runEpisode()`: Run single episode with policy execution
- `runTraining()`: Run complete training process
- `collectExperience()`: Collect experience for learning
- `updatePolicy()`: Update policy based on experience
- `evaluatePolicy()`: Evaluate current policy performance
- `savePolicy()`: Save policy to file
- `loadPolicy()`: Load policy from file

**Usage Example**:
```python
import pynlm

# Create components
brain = pynlm.createQuickTestBrain()
environment = pynlm.SimpleWorld()
environment.configure(100, 100, 20, 20)
agent = pynlm.AgentBrain(brain)

# Create reinforcement learner
config = pynlm.ExperimentConfig.createTrainingConfig()
learner = pynlm.createLearner(pynlm.RLAlgorithm.DEEP_Q_NETWORK, brain, agent, environment, config)

# Create training loop
training_loop = pynlm.createTrainingLoop(agent, environment, config, brain, learner)

# Set up callbacks
def episode_callback(episode, reward, steps):
    print(f"Episode {episode}: Reward={reward:.2f}, Steps={steps}")

training_loop.setEpisodeCallback(episode_callback)

# Start training
training_loop.start()
training_loop.runTraining()
```

### 2. ReinforcementLearner Class

**Purpose**: Base class for reinforcement learning algorithms

**Properties**:
- `brain`: Neural network brain
- `agent`: Agent brain interface
- `world`: Simulation world
- `config`: Configuration

**Subclasses**:
- `TabularQLearning`: Tabular Q-learning implementation
- `DeepQLearning`: Deep Q-network implementation
- `PolicyGradientLearner`: Policy gradient implementation

**Methods**:
- `train()`: Main training method
- `predict()`: Predict actions given state
- `learn()`: Learn from experience
- `explore()`: Handle exploration vs exploitation
- `saveModel()`: Save model to file
- `loadModel()`: Load model from file

**Usage Example**:
```python
import pynlm

# Create reinforcement learner
brain = pynlm.createQuickTestBrain()
agent = pynlm.AgentBrain(brain)
environment = pynlm.SimpleWorld()
config = pynlm.ExperimentConfig.createTrainingConfig()

# Create Deep Q-Network learner
learner = pynlm.createLearner(pynlm.RLAlgorithm.DEEP_Q_NETWORK, brain, agent, environment, config)

# Train
learner.train(1000)  # Train for 1000 episodes

# Predict action
state = [0.5, 0.3, 0.8, 0.1]  # Example state
action = learner.predict(brain, state)
```

### 3. CurriculumLearner Class

**Purpose**: Curriculum learning framework for progressive difficulty adaptation

**Properties**:
- `difficulty_levels`: Array of difficulty levels
- `progress_tracker`: Learning progress tracking

**Methods**:
- `getCurrentDifficulty()`: Get current training difficulty
- `advanceCurriculum()`: Advance to next difficulty level
- `adaptEnvironment()`: Adapt environment based on performance
- `resetProgress()`: Reset learning progress

**Difficulty Levels**:
- `EASY`: Simple tasks with low complexity
- `MEDIUM`: Moderate complexity tasks
- `HARD`: Challenging tasks
- `EXPERT`: Advanced tasks
- `NIGHTMARE`: Maximum difficulty (for research/testing)

**Usage Example**:
```python
import pynlm

# Create curriculum learner (start with easy difficulty)
curriculum = pynlm.createCurriculumLearner(pynlm.DifficultyLevel.EASY)

# Get current difficulty
print(f"Current difficulty: {curriculum.getCurrentDifficulty()}")

# Adapt environment based on performance
brain = pynlm.createQuickTestBrain()
agent = pynlm.AgentBrain(brain)
environment = pynlm.SimpleWorld()
metrics = [0.8, 0.7, 0.9]  # Example performance metrics
curriculum.adaptEnvironment(brain, agent, environment, metrics)

# Advance curriculum
if curriculum.shouldAdvanceDifficulty(metrics):
    curriculum.advanceCurriculum()
    print("Advanced to next difficulty level")
```

### 4. SimulationScheduler Class

**Purpose**: Advanced simulation scheduling system

**Properties**:
- `episode_schedule`: Schedule of simulation runs
- `time_budget`: Time budget for simulations
- `resource_limits`: Computational resource limits

**Methods**:
- `scheduleSimulation()`: Schedule simulation runs
- `allocateResources()`: Allocate computational resources
- `monitorProgress()`: Monitor simulation progress
- `adjustSchedule()`: Adjust schedule dynamically

**Resource Management**:
- CPU cores allocation
- GPU memory allocation
- RAM allocation
- Priority-based scheduling
- Distributed training support

**Usage Example**:
```python
import pynlm
from datetime import datetime, timedelta

# Create experiment configurations
experiments = [
    pynlm.ExperimentConfig(
        experimentName="dqn_experiment",
        description="Deep Q-Network training",
        algorithm=pynlm.RLAlgorithm.DEEP_Q_NETWORK,
        episodeCount=100
    ),
    pynlm.ExperimentConfig(
        experimentName="pg_experiment",
        description="Policy Gradient training",
        algorithm=pynlm.RLAlgorithm.POLICY_GRADIENT,
        episodeCount=150
    )
]

# Create resource configuration
resources = pynlm.ResourceConfig()
resources.cpuCores = 2
resources.gpuMemory = 4  # GB
resources.ramMemory = 16  # GB
resources.maxConcurrentExperiments = 2

# Schedule training runs
scheduler = pynlm.scheduleTraining(experiments, resources)

# Get scheduler status
print(scheduler.getSchedulerStatus())
print(scheduler.getResourceReport())
```

## Module-Level Convenience Functions

### 1. `createTrainingLoop(agent, environment, config)`

**Description**: Factory function for creating training loops

**Parameters**:
- `agent`: Agent brain instance
- `environment`: Simulation environment
- `config`: Training configuration
- `policy`: Neural network policy (optional)
- `optimizer`: Reinforcement learning optimizer (optional)

**Returns**: TrainingLoop instance

### 2. `runReinforcementTraining(brain, world, agent, algorithm_config)`

**Description**: Run reinforcement training with specified algorithm

**Parameters**:
- `brain`: Neural network brain
- `world`: Simulation world
- `agent`: Agent brain
- `algorithm_config`: Algorithm configuration

**Returns**: Training results

### 3. `createCurriculumLearner(initial_difficulty)`

**Description**: Create curriculum learning environment

**Parameters**:
- `initial_difficulty`: Initial difficulty level (0 for easy, 4 for nightmare)

**Returns**: CurriculumLearner instance

### 4. `scheduleTraining(experiments_config, resource_config)`

**Description**: Schedule training runs

**Parameters**:
- `experiments_config`: List of experiment configurations
- `resource_config`: Resource allocation configuration

**Returns**: SimulationScheduler instance

## Supporting Classes and Structures

### 1. TrainingExperience

**Description**: Experience data for reinforcement learning

**Properties**:
- `brainState`: Brain state snapshot
- `worldState`: World state snapshot
- `agent`: Agent instance
- `state`: Current state vector
- `action`: Action taken
- `reward`: Reward received
- `nextState`: Next state vector
- `done`: Episode termination flag
- `timestamp`: Experience timestamp
- `step`: Step number
- `difficultyLevel`: Difficulty level

### 2. LearningProgress

**Description**: Progress tracking for curriculum learning

**Properties**:
- `successRate`: Success rate percentage
- `averageReward`: Average reward
- `averageSteps`: Average steps per episode
- `trainingTimePerEpisode`: Average training time
- `episodesCompleted`: Number of episodes completed
- `currentDifficulty`: Current difficulty level
- `performanceScore`: Normalized performance score

### 3. ExperienceReplay

**Description**: Experience replay buffer for reinforcement learning

**Methods**:
- `add(experience)`: Add experience to buffer
- `sample()`: Sample experience from buffer
- `sampleUniform()`: Sample experience uniformly
- `isReady(minSize)`: Check if buffer is ready
- `clear()`: Clear buffer

### 4. QTable

**Description**: Q-table for tabular Q-learning

**Methods**:
- `getValue(state, action)`: Get Q-value for state-action pair
- `setValue(state, action, value)`: Set Q-value
- `getBestAction(state)`: Get best action for state
- `update(state, action, reward, nextState, nextAction, alpha, gamma)`: Update Q-table
- `incrementVisitCount(state, action)`: Increment visit count
- `getVisitCount(state, action)`: Get visit count

### 5. ResourceConfig

**Description**: Resource allocation configuration

**Properties**:
- `cpuCores`: Number of CPU cores
- `gpuMemory`: GPU memory in GB
- `ramMemory`: RAM in GB
- `maxConcurrentExperiments`: Maximum concurrent experiments
- `cpuUsageLimit`: CPU usage limit (0.0-1.0)
- `gpuUsageLimit`: GPU usage limit (0.0-1.0)
- `memoryUsageLimit`: Memory usage limit (0.0-1.0)
- `priority`: Priority level ("high", "medium", "low")

### 6. ExperimentConfig

**Description**: Experiment configuration for scheduling

**Properties**:
- `experimentName`: Name of the experiment
- `description`: Description of the experiment
- `algorithm`: Reinforcement learning algorithm
- `episodeCount`: Number of episodes
- `config`: Additional configuration
- `resources`: Resource requirements
- `tags`: Tags for categorization
- `scheduledTime`: Scheduled start time
- `priority`: Scheduling priority

## Enumerations

### 1. RLAlgorithm

**Values**:
- `TABULAR_Q_LEARNING`: Tabular Q-learning
- `DEEP_Q_NETWORK`: Deep Q-network
- `POLICY_GRADIENT`: Policy gradient
- `Q_LEARNING`: Classic Q-learning
- `SARSA`: SARSA algorithm
- `EXPECTATION_MAXIMIZATION`: Expectation maximization
- `PPO`: Proximal Policy Optimization
- `A2C`: Advantage Actor-Critic
- `DDPG`: Deep Deterministic Policy Gradient
- `SAC`: Soft Actor-Critic

### 2. ExplorationStrategy

**Values**:
- `EPSILON_GREEDY`: Epsilon-greedy exploration
- `UCB1`: Upper Confidence Bound 1
- `THERMAL`: Temperature-based exploration
- `NOISE`: Action noise exploration
- `COUNT`: Count-based exploration
- `ADAPTIVE`: Adaptive exploration

### 3. DifficultyLevel

**Values**:
- `EASY`: Easy difficulty
- `MEDIUM`: Medium difficulty
- `HARD`: Hard difficulty
- `EXPERT`: Expert difficulty
- `NIGHTMARE`: Nightmare difficulty

## Advanced Features

### 1. Neural Network Utilities

**Class**: `NeuralNetworkUtils`

**Static Methods**:
- `softmax(logits)`: Compute softmax activation
- `entropy(probabilities)`: Compute entropy
- `clippedGradient(gradient, clipNorm)`: Clip gradient by norm
- `oneHotEncode(action, actionSpaceSize)`: One-hot encode action
- `muSigmaToDistribution(mu, sigma)`: Convert mu-sigma to distribution

### 2. Factory Classes

#### ReinforcementLearnerFactory

**Static Methods**:
- `createLearner(algorithm, brain, agent, world, config)`: Create reinforcement learner
- `getSupportedAlgorithms()`: Get list of supported algorithms
- `stringToAlgorithm(algorithmName)`: Convert string to enum
- `algorithmToString(algorithm)`: Convert enum to string

#### CurriculumLearnerFactory

**Static Methods**:
- `createStandardCurriculum(numDifficultyLevels, curriculumType)`: Create standard curriculum
- `createAdaptiveCurriculum(performanceEvaluator, minEpisodesPerLevel)`: Create adaptive curriculum
- `createStructuredCurriculum(difficultyOrder, adaptations)`: Create structured curriculum

#### SimulationSchedulerFactory

**Static Methods**:
- `createLocalScheduler(systemResources)`: Create local scheduler
- `createClusterScheduler(workerResources, masterResources)`: Create cluster scheduler
- `createCloudScheduler(provider, region, credentials)`: Create cloud scheduler
- `optimizeExperimentSchedule(experiments, availableResources)`: Optimize experiment schedule

## Integration with Existing NLM Framework

The training loop utilities integrate seamlessly with the existing NLM framework:

### 1. Brain Integration

All training loop classes work with existing NLM brain components:
- `Brain`: Central neural simulation brain
- `AgentBrain`: Agent brain interface
- `SimpleWorld`: Simulation world
- `SensoryInput`: Sensory input processing
- `MotorCommand`: Motor command execution

### 2. Reinforcement Learning Algorithms

The framework supports multiple reinforcement learning paradigms:
- **Episodic Learning**: Learning through discrete episodes
- **Continuous Learning**: Continuous learning in dynamic environments
- **Online Learning**: Learning while interacting with the environment
- **Offline Learning**: Learning from stored experiences

### 3. Experience Management

Experience replay mechanisms support:
- ** episodic Memory**: Storage of episodic experiences
- **Working Memory**: Short-term experience storage
- **Associative Memory**: Pattern-based experience storage

### 4. Neuromodulation Integration

Training loop utilities integrate with NLM neuromodulation systems:
- **Dopamine**: Reward signaling
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning signals

## Performance Considerations

### 1. Resource Management

The simulation scheduler optimizes resource allocation:
- **CPU/GPU Allocation**: Efficient distribution of computing resources
- **Memory Management**: Optimized memory usage
- **Parallel Processing**: Support for distributed training

### 2. Scalability

The framework supports various scales of training:
- **Single Machine**: Training on a single machine
- **Multi-GPU**: Training with multiple GPUs
- **Distributed Systems**: Training across multiple machines
- **Cloud Computing**: Training in cloud environments

### 3. Performance Monitoring

Comprehensive monitoring and reporting:
- **Real-time Metrics**: Monitoring of training progress
- **Resource Usage**: Tracking of resource consumption
- **Performance Analysis**: Analysis of training performance

## Error Handling

All classes use NLMRuntimeError for comprehensive error handling:
- **Input Validation**: Validation of input parameters
- **State Validation**: Validation of object states
- **Resource Validation**: Validation of resource availability
- **Exception Handling**: Proper exception propagation and catching

## Installation and Setup

### 1. System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Python**: Python 3.7 or higher
- **C++ Compiler**: C++11 or higher
- **CMake**: CMake 3.10 or higher

### 2. Building from Source

1. Clone the repository
2. Create build directory: `mkdir build && cd build`
3. Configure with CMake: `cmake ..`
4. Build with: `make -j$(nproc)`
5. Install Python bindings: `pip install pynlm`

### 3. Running Examples

1. **Basic Example**:
```bash
cd /path/to/nlm/repository
cd python
cp demo_training_loop.py test_training_loop.py
python test_training_loop.py
```

2. **Comprehensive Demo**:
```bash
cd /path/to/nlm/repository/python
python demo_training_loop.py
```

## API Documentation

### Complete API Reference

For complete API documentation, see:
- `pynlm.py`: Main Python bindings file
- `TrainingLoop.hpp`/`TrainingLoop.cpp`: TrainingLoop implementation
- `ReinforcementLearner.hpp`/`ReinforcementLearner.cpp`: ReinforcementLearner implementation
- `CurriculumLearner.hpp`/`CurriculumLearner.cpp`: CurriculumLearner implementation
- `SimulationScheduler.hpp`/`SimulationScheduler.cpp`: SimulationScheduler implementation

### Python API Examples

#### Basic Training Loop Example
```python
import pynlm

# Create training components
brain = pynlm.createQuickTestBrain()
environment = pynlm.SimpleWorld()
config = pynlm.ExperimentConfig.createTrainingConfig()
agent = pynlm.AgentBrain(brain)

# Create reinforcement learner
learner = pynlm.createLearner(pynlm.RLAlgorithm.DEEP_Q_NETWORK, brain, agent, environment, config)

# Create training loop
training_loop = pynlm.createTrainingLoop(agent, environment, config, brain, learner)

# Start training
training_loop.start()
training_loop.runTraining(100)  # Run for 100 episodes

# Save policy
training_loop.savePolicy("trained_policy.bin")
```

#### Curriculum Learning Example
```python
import pynlm

# Create curriculum learner (start with easy difficulty)
curriculum = pynlm.createCurriculumLearner(pynlm.DifficultyLevel.EASY)

# Run curriculum learning
for episode in range(100):
    # Adapt environment based on performance
    brain = pynlm.createQuickTestBrain()
    agent = pynlm.AgentBrain(brain)
    environment = pynlm.SimpleWorld()
    
    # Record episode result
    reward = curriculum.simulateEpisodeReward(curriculum.getCurrentDifficulty())
    steps = curriculum.calculateExpectedSteps(curriculum.getCurrentDifficulty())
    curriculum.recordEpisodeResult(reward, steps, 1.0, curriculum.getCurrentDifficulty())
    
    # Advance if performance is good
    if curriculum.shouldIncreaseDifficulty([]):
        curriculum.advanceCurriculum()
```

#### Simulation Scheduling Example
```python
import pynlm
from datetime import datetime, timedelta

# Create experiment configurations
experiments = [
    pynlm.ExperimentConfig(
        experimentName="experiment1",
        algorithm=pynlm.RLAlgorithm.TABULAR_Q_LEARNING,
        episodeCount=200
    ),
    pynlm.ExperimentConfig(
        experimentName="experiment2",
        algorithm=pynlm.RLAlgorithm.POLICY_GRADIENT,
        episodeCount=300
    )
]

# Create resource configuration
resources = pynlm.ResourceConfig()
resources.cpuCores = 4
resources.gpuMemory = 8
resources.ramMemory = 32

# Schedule training runs
scheduler = pynlm.scheduleTraining(experiments, resources)

# Start simulations
scheduler.startSimulation("experiment1")
scheduler.startSimulation("experiment2")

# Monitor progress
while scheduler.getActiveSimulationCount() > 0:
    for sim in scheduler.getActiveSimulations():
        print(f"Progress: {sim.experimentId} - {sim.progress * 100:.1f}%")
    time.sleep(1)

# Complete simulations
scheduler.completeSimulation("experiment1")
scheduler.completeSimulation("experiment2")
```

## Advanced Usage

### 1. Distributed Training

The simulation scheduler supports distributed training:
```python
# Create cluster scheduler
worker_resources = [
    pynlm.ResourceConfig(cpuCores=2, gpuMemory=2, ramMemory=8),
    pynlm.ResourceConfig(cpuCores=2, gpuMemory=2, ramMemory=8)
]
master_resources = pynlm.ResourceConfig(cpuCores=4, gpuMemory=4, ramMemory=16)

scheduler = pynlm.createClusterScheduler(worker_resources, master_resources)
```

### 2. Hyperparameter Optimization

The framework supports hyperparameter sweeps:
```python
# Create hyperparameter sweep configuration
config = pynlm.ExperimentConfig.createTrainingConfig()
config.set("learning_rate", 0.01)
config.set("batch_size", 32)
config.set("discount_factor", 0.99)

# Run hyperparameter sweep
for lr in [0.001, 0.01, 0.1]:
    for batch_size in [16, 32, 64]:
        config.set("learning_rate", lr)
        config.set("batch_size", batch_size)
        experiment = pynlm.ExperimentConfig(
            experimentName=f"sweep_lr{lr}_batch{batch_size}",
            algorithm=pynlm.RLAlgorithm.DEEP_Q_NETWORK,
            episodeCount=50,
            config=config
        )
        scheduler.scheduleSimulation(experiment)
```

### 3. Ablation Studies

The framework supports ablative studies:
```python
# Create ablative experiment
config = pynlm.ExperimentConfig.createAblationConfig()
config.setNeuronCount(100)  # Reduced network size
config.setMaxSteps(500)      # Reduced training steps

ablation_experiment = pynlm.ExperimentConfig(
    experimentName="ablation_memory",
    description="Memory system ablation study",
    algorithm=pynlm.RLAlgorithm.TABULAR_Q_LEARNING,
    episodeCount=100,
    config=config
)

scheduler.scheduleSimulation(ablation_experiment)
```

### 4. Reproducible Research

The framework supports reproducible research with checkpointing:
```python
# Enable checkpointing
config = pynlm.ExperimentConfig.createTrainingConfig()
config.setEnableCheckpointing(True)
config.setCheckpointPath("./checkpoints/")

# Run training with checkpointing
training_loop = pynlm.createTrainingLoop(agent, environment, config, brain, learner)
training_loop.start()
training_loop.runTraining(1000)  # This will save checkpoints every 100 episodes

# Resume training from checkpoint
training_loop.loadPolicy("./checkpoints/trained_policy.bin")
```

## Troubleshooting

### 1. Common Issues

**Issue**: Python bindings not found
**Solution**: Ensure the C++ bindings are compiled and installed:
```bash
cd /path/to/nlm/repository
mkdir build && cd build
cmake ..
make -j$(nproc)
pip install pynlm
```

**Issue**: Missing dependencies
**Solution**: Install required Python packages:
```bash
pip install numpy matplotlib pandas scikit-learn
```

**Issue**: Resource allocation failed
**Solution**: Check resource availability and adjust configuration:
```python
# Increase available resources
resources = pynlm.ResourceConfig()
resources.cpuCores = 8
resources.gpuMemory = 16
resources.ramMemory = 64
```

### 2. Debug Mode

To enable debug mode, set the environment variable:
```bash
export NLM_DEBUG=1
```

### 3. Logging

The framework includes comprehensive logging:
```python
import pynlm
pynlm.Logger.setGlobal(pynlm.ConsoleLogger(pynlm.LogLevel.Info))
```

## Contributing

### 1. Code Style

Follow the existing NLM code style:
- C++11/14 compatible code
- Consistent naming conventions
- Comprehensive documentation
- Unit testing

### 2. Testing

Run the existing test suite:
```bash
python test_new_api.py
python demo_training_loop.py
python test_training_loop.py
```

### 3. Documentation

- Update API documentation
- Add usage examples
- Provide troubleshooting guide
- Maintain consistent documentation format

## License

This software is licensed under the terms of the MIT License. See LICENSE file for details.

## Acknowledgements

The training loop utilities build upon the existing NLM framework and incorporate contributions from:

- NLM Research Team
- Contributors to the NLM codebase
- Open source community
- Academic collaborators

## Future Enhancements

### 1. Planned Features

- **Advanced Neural Networks**: Integration with more sophisticated neural network architectures
- **Multi-Agent Systems**: Support for multi-agent reinforcement learning
- **Model Compression**: Support for model compression and quantization
- **Automated Architecture Search**: Integration with neural architecture search
- **Real-time Training**: Support for online learning in real-time environments

### 2. Research Areas

- **Meta-Learning**: Learning how to learn across tasks
- **Transfer Learning**: Knowledge transfer between domains
- **Meta-Reinforcement Learning**: Reinforcement learning at the meta-level
- **Hierarchical Reinforcement Learning**: Learning through hierarchies of policies

## Contact Information

For questions, suggestions, or issues:

- **GitHub Repository**: https://github.com/your-username/nlm
- **Issue Tracker**: https://github.com/your-username/nlm/issues
- **Documentation**: https://nlm.readthedocs.io
- **Support**: support@nlm.ai

## Version Information

- **Current Version**: 2.0.0
- **Build Date**: $(date)
- **Compiler**: $(g++ --version)
- **Python Version**: $(python --version)
- **CMake Version**: $(cmake --version)

---

*This documentation is generated automatically from the source code and is subject to change.*

# Neural Learning Machine (NLM) Training Loop Utilities

## Quick Start Guide

This guide provides a quick introduction to using the training loop utilities in the Neural Learning Machine framework.

### 1. Installation

```bash
# Clone the repository
git clone https://github.com/your-username/nlm.git
cd nlm

# Build the C++ bindings
mkdir build && cd build
cmake ..
make -j$(nproc)

# Install Python bindings
pip install pynlm
```

### 2. Basic Usage

```python
import pynlm
import time

# Create a test brain
print("Creating test brain...")
brain = pynlm.createQuickTestBrain()
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Create simulation environment
print("Creating simulation environment...")
environment = pynlm.SimpleWorld()
environment.configure(100, 100, 20, 20)
environment.setAgentStart(50, 50)

# Create agent brain
print("Creating agent brain...")
agent = pynlm.AgentBrain(brain)
agent.initialize(environment)

# Create configuration
print("Creating configuration...")
config = pynlm.ExperimentConfig.createTrainingConfig()
config.setNeuronCount(brain.getTotalNeuronCount())
config.setMaxSteps(1000)

# Create reinforcement learner
print("Creating reinforcement learner...")
learner = pynlm.createLearner(pynlm.RLAlgorithm.DEEP_Q_NETWORK, brain, agent, environment, config)
print(f"Learner created with algorithm: {learner.getAlgorithm()}")

# Create training loop
print("Creating training loop...")
training_loop = pynlm.createTrainingLoop(agent, environment, config, brain, learner)

# Set up progress callbacks
def episode_callback(episode, reward, steps):
    if episode % 10 == 0:
        print(f"  Episode {episode}: Reward={reward:.2f}, Steps={steps}")

training_loop.setEpisodeCallback(episode_callback)

# Start training
print("Starting training...")
start_time = time.time()
training_loop.start()
training_loop.runTraining(50)  # Run for 50 episodes
end_time = time.time()

print(f"Training completed in {end_time - start_time:.2f} seconds")
print(f"Final metrics:\n{training_loop.getMetricsSummary()}")

# Save trained model
training_loop.savePolicy("trained_policy.bin")
learner.saveModel("trained_learner.pkl")
print("Model saved successfully")
```

### 3. Advanced Usage

```python
import pynlm
import numpy as np

# Create multiple experiments for scheduling
experiments = [
    pynlm.ExperimentConfig(
        experimentName="dqn_experiment",
        algorithm=pynlm.RLAlgorithm.DEEP_Q_NETWORK,
        episodeCount=100
    ),
    pynlm.ExperimentConfig(
        experimentName="pg_experiment",
        algorithm=pynlm.RLAlgorithm.POLICY_GRADIENT,
        episodeCount=150
    ),
    pynlm.ExperimentConfig(
        experimentName="tabular_experiment",
        algorithm=pynlm.RLAlgorithm.TABULAR_Q_LEARNING,
        episodeCount=200
    )
]

# Create resource configuration for parallel execution
resources = pynlm.ResourceConfig()
resources.cpuCores = 6
resources.gpuMemory = 12  # GB
resources.ramMemory = 48  # GB
resources.maxConcurrentExperiments = 2
resources.priority = "high"

# Schedule and run experiments
print("Scheduling experiments...")
scheduler = pynlm.scheduleTraining(experiments, resources)

# Start all experiments
for experiment in experiments:
    scheduler.startSimulation(experiment.experimentName)

# Monitor progress
print("Monitoring progress...")
while scheduler.getActiveSimulationCount() > 0:
    for sim in scheduler.getActiveSimulations():
        status = f"{sim.experimentId}: {sim.progress * 100:.1f}% complete"
        if sim.active:
            status += f", reward={sim.currentReward:.2f}"
        print(f"  {status}")
    time.sleep(2)

# Complete all experiments
for experiment in experiments:
    scheduler.completeSimulation(experiment.experimentName)

print("All experiments completed successfully!")
print(f"Scheduler report:\n{scheduler.getOverallScheduleReport()}")
```

### 4. Curriculum Learning Example

```python
import pynlm

# Create curriculum learner with adaptive difficulty
print("Creating curriculum learner...")
curriculum = pynlm.createCurriculumLearner(pynlm.DifficultyLevel.MEDIUM)

# Simulate curriculum learning
print("Running curriculum learning...")
for episode in range(50):
    # Create fresh components for each episode
    brain = pynlm.createQuickTestBrain()
    agent = pynlm.AgentBrain(brain)
    environment = pynlm.SimpleWorld()
    
    # Get current difficulty
    difficulty = curriculum.getCurrentDifficulty()
    print(f"Episode {episode}: Running at {difficulty} difficulty")
    
    # Run episode with curriculum adaptation
    success = curriculum.runEpisodeWithDifficulty(
        brain, agent, environment, 
        pynlm.DifficultyLevel(int(difficulty))
    )
    
    if success:
        print(f"  ✓ Episode completed successfully")
        
        # Record performance and adapt curriculum
        curriculum.updateProgress([0.8, 0.7, 0.9])  # Example performance metrics
        
        # Check if difficulty should be adjusted
        if curriculum.shouldIncreaseDifficulty([]):
            curriculum.advanceCurriculum()
            print(f"  → Advanced to next difficulty level")
        elif curriculum.shouldDecreaseDifficulty([]):
            # Implement curriculum decrease if needed
            pass
    else:
        print(f"  ✗ Episode failed")

# Show curriculum progress
print(f"\nCurriculum progress:\n{curriculum.getProgressReport()}")
```

### 5. Model Persistence Example

```python
import pynlm
import os

# Create and train a model
print("Training model...")
# [Training code from previous examples would go here]

# Save models with error handling
try:
    training_loop.savePolicy("checkpoints/policy_checkpoint.bin")
    print("✓ Policy saved successfully")
except Exception as e:
    print(f"✗ Failed to save policy: {e}")

try:
    learner.saveModel("checkpoints/learner_checkpoint.pkl")
    print("✓ Learner model saved successfully")
except Exception as e:
    print(f"✗ Failed to save learner: {e}")

# Load models with error handling
if os.path.exists("checkpoints/policy_checkpoint.bin"):
    try:
        training_loop.loadPolicy("checkpoints/policy_checkpoint.bin")
        print("✓ Policy loaded successfully")
    except Exception as e:
        print(f"✗ Failed to load policy: {e}")

if os.path.exists("checkpoints/learner_checkpoint.pkl"):
    try:
        learner.loadModel("checkpoints/learner_checkpoint.pkl")
        print("✓ Learner model loaded successfully")
    except Exception as e:
        print(f"✗ Failed to load learner: {e}")
```

## Common Problems and Solutions

### Problem: "ModuleNotFoundError: No module named 'pynlm'"

**Solution**: Ensure the C++ bindings are properly installed:

```bash
# Check if pynlm is installed
python -c "import pynlm; print(pynlm.__version__)"

# If not installed, install with verbose output
pip install --verbose pynlm
```

### Problem: "ImportError: libpynlm.so: cannot open shared object file"

**Solution**: Set the library path:

```bash
# Set LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/nlm/build:$LD_LIBRARY_PATH

# Or install to system library path
sudo make install
```

### Problem: "TrainingError: Insufficient resources"

**Solution**: Check resource availability and adjust configuration:

```python
import pynlm

# Check current resources
scheduler = pynlm.createLocalScheduler(pynlm.ResourceConfig())
available = scheduler.getAvailableResources()
print(f"Available resources: {available.cpuCores} CPU cores, {available.gpuMemory} GPU memory, {available.ramMemory} GB RAM")

# Create scheduler with more resources
resources = pynlm.ResourceConfig()
resources.cpuCores = 8
resources.gpuMemory = 16
resources.ramMemory = 64
scheduler = pynlm.scheduleTraining([], resources)
```

### Problem: "TrainingError: Algorithm not supported"

**Solution**: Check supported algorithms and use a valid one:

```python
import pynlm

# Get supported algorithms
algorithms = pynlm.getSupportedAlgorithms()
print(f"Supported algorithms: {algorithms}")

# Use a valid algorithm
valid_algorithm = pynlm.RLAlgorithm.DEEP_Q_NETWORK
experiment = pynlm.ExperimentConfig(
    experimentName="test",
    algorithm=valid_algorithm,
    episodeCount=100
)
```

## Performance Tips

### 1. Resource Optimization

```python
# Use resource-efficient algorithms for simple tasks
simple_experiment = pynlm.ExperimentConfig(
    experimentName="simple_task",
    algorithm=pynlm.RLAlgorithm.TABULAR_Q_LEARNING,  # Lightweight algorithm
    episodeCount=100
)

# Use GPU-accelerated algorithms for complex tasks
gpu_experiment = pynlm.ExperimentConfig(
    experimentName="complex_task",
    algorithm=pynlm.RLAlgorithm.DEEP_Q_NETWORK,  # GPU-accelerated
    episodeCount=200,
    requiresGPU=True
)
```

### 2. Memory Management

```python
# Use experience replay with smaller buffer size for memory constraints
replay = pynlm.ExperienceReplay(1000)  # Smaller buffer

# Use curriculum learning to reduce training time
curriculum = pynlm.createCurriculumLearner(pynlm.DifficultyLevel.EASY)
```

### 3. Parallel Execution

```python
# Use cluster scheduler for parallel execution
worker_resources = [
    pynlm.ResourceConfig(cpuCores=2, gpuMemory=2, ramMemory=8),
    pynlm.ResourceConfig(cpuCores=2, gpuMemory=2, ramMemory=8)
]
master_resources = pynlm.ResourceConfig(cpuCores=4, gpuMemory=4, ramMemory=16)

scheduler = pynlm.createClusterScheduler(worker_resources, master_resources)
```

## Getting Help

### 1. Documentation

- **API Reference**: https://nlm.readthedocs.io/en/latest/
- **Tutorials**: https://nlm.readthedocs.io/en/latest/tutorials/
- **Examples**: https://github.com/your-username/nlm/tree/main/examples

### 2. Community

- **GitHub Issues**: https://github.com/your-username/nlm/issues
- **Discussions**: https://github.com/your-username/nlm/discussions
- **Email Support**: support@nlm.ai

### 3. Stack Overflow

Ask questions with the `[nlm]` tag:
https://stackoverflow.com/questions/tagged/nlm

## Acknowledgements

The training loop utilities are made possible through the contributions of:

- NLM Research Team
- Contributors to the NLM codebase
- Open source community
- Academic collaborators in neural learning research

## License

This software is licensed under the MIT License. See the LICENSE file for details.

---

*For more information, visit the NLM GitHub repository or contact the NLM team at support@nlm.ai.*


# Quick Start Guide

## 1. Installation

```bash
# Clone repository and build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Install Python bindings
pip install pynlm
```

## 2. Basic Usage

```python
import pynlm
import time

# Create all components
brain = pynlm.createQuickTestBrain()
environment = pynlm.SimpleWorld()
config = pynlm.ExperimentConfig.createTrainingConfig()
agent = pynlm.AgentBrain(brain)

# Create training loop
training_loop = pynlm.createTrainingLoop(agent, environment, config, brain, learner)

# Start training
training_loop.start()
training_loop.runTraining(50)

# Save results
training_loop.savePolicy("policy.bin")
```

## 3. Advanced Usage

```python
import pynlm

# Schedule multiple experiments
experiments = [
    pynlm.ExperimentConfig(algorithm=pynlm.RLAlgorithm.DEEP_Q_NETWORK, episodeCount=100),
    pynlm.ExperimentConfig(algorithm=pynlm.RLAlgorithm.POLICY_GRADIENT, episodeCount=150)
]

# Create resource configuration
resources = pynlm.ResourceConfig(cpuCores=4, gpuMemory=8, ramMemory=32)

# Schedule training
scheduler = pynlm.scheduleTraining(experiments, resources)

# Run all experiments
for exp in experiments:
    scheduler.startSimulation(exp.experimentName)
```

## 4. Common Issues

- **Installation**: Ensure C++ bindings are compiled
- **Resources**: Check CPU/GPU/CPU availability
- **Algorithms**: Use supported RLAlgorithm values

## 5. Performance

- Use `TabularQLearning` for simple tasks
- Use `DeepQLearning` for complex tasks with GPU
- Use `CurriculumLearner` to reduce training time
- Monitor resource usage with `SimulationScheduler`

## 6. Support

- GitHub Issues
- Documentation
- Community forums

Enjoy using the Neural Learning Machine training loop utilities!
```