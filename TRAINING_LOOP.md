# NLM Training Loop and Reinforcement Learning Documentation

## Overview

The NLM Training Loop and Reinforcement Learning framework provides a comprehensive system for training neural learning machines using various learning paradigms including reinforcement learning, curriculum learning, and experience replay. It integrates seamlessly with the NLM brain architecture and experiment management system.

This framework is designed for:
- **Reinforcement Learning** - Agent learning from environmental rewards
- **Curriculum Learning** - Progressive difficulty learning
- **Experience Replay** - Memory-based learning from past experiences
- **Multi-Agent Training** - Coordinated learning across multiple agents
- **Distributed Training** - Parallel training across multiple processes
- **Meta-Learning** - Learning to learn
- **Transfer Learning** - Knowledge transfer between tasks

---

## Core Components

### 1. TrainingLoop

**Purpose**: Core training loop framework for neural learning machine training with comprehensive support for reinforcement learning, curriculum learning, and experience replay.

**Key Features**:
- Multi-modal learning support
- Experience replay integration
- Curriculum learning
- Performance monitoring
- Adaptive learning rates
- Exploration-exploitation balance
- Reward shaping
- Learning curve analysis

**Python API**:
```python
import pynlm

# Create training loop
config = pynlm.TrainingLoop.TrainingLoopConfig()
training_loop = pynlm.TrainingLoop(config)

# Set up training components
training_loop.setup(brain, agent, world, environment)

# Configure learning parameters
training_loop.setLearningRate(0.001)
training_loop.setExplorationRate(0.1)
training_loop.enableExperienceReplay(True)
training_loop.enableCurriculumLearning(True)

# Set reward function
training_loop.setRewardFunction(my_reward_function)

# Start training
results = training_loop.run(max_episodes=1000)

# Get training metrics
metrics = training_loop.getTrainingMetrics()
print(f"Total reward: {metrics.total_reward}")
print(f"Average episode length: {metrics.avg_episode_length}")
print(f"Best episode reward: {metrics.best_reward}")
```

**Key Methods**:
- `setup()` - Setup training loop with components
- `run()` - Run training loop
- `setLearningRate()` - Set learning rate
- `setExplorationRate()` - Set exploration rate
- `enableExperienceReplay()` - Enable/disable experience replay
- `enableCurriculumLearning()` - Enable/disable curriculum learning
- `enableMetaLearning()` - Enable/disable meta learning
- `setRewardFunction()` - Set custom reward function
- `setCuriosityBonus()` - Set curiosity bonus
- `setNoveltyBonus()` - Set novelty bonus
- `addTrainingConstraint()` - Add training constraint
- `getTrainingMetrics()` - Get training metrics
- `getExperienceReplayBuffer()` - Get experience replay buffer
- `getCurriculumProgress()` - Get curriculum progress

### 2. ReinforcementLearner

**Purpose**: Reinforcement learning agent with Q-learning, policy gradient, and actor-critic algorithms.

**Key Features**:
- Multiple RL algorithms
- Deep Q-Network support
- Policy gradient methods
- Actor-critic algorithms
- Reward shaping
- Exploration strategies
- Target network support
- Gradient clipping
- Early stopping

**Python API**:
```python
import pynlm

# Create reinforcement learner
learner = pynlm.ReinforcementLearner()

# Configure Q-learning
learner.setAlgorithm(pynlm.ReinforcementLearner.Algorithm.Q_LEARNING)
learner.setLearningRate(0.001)
learner.setDiscountFactor(0.99)
learner.setExplorationRate(0.1)
learner.setTargetUpdateFrequency(100)

# Configure policy gradient
learner.setAlgorithm(pynlm.ReinforcementLearner.Algorithm.POLICY_GRADIENT)
learner.setLearningRate(0.0001)
learner.setEntropyBonus(0.01)

# Configure actor-critic
learner.setAlgorithm(pynlm.ReinforcementLearner.Algorithm.ACTOR_CRITIC)
learner.setLearningRate(0.001)
learner.setEntropyBonus(0.001)

# Set reward function
learner.setRewardFunction(reward_function)

# Set experience replay
learner.setExperienceReplayCapacity(10000)

# Start training
brain = pynlm.createQuickTestBrain()
brain.initialize()
learner.train(brain, max_episodes=1000)

# Get learned policy
policy = learner.getPolicy()

# Get performance metrics
metrics = learner.getPerformanceMetrics()
```

**Key Methods**:
- `setAlgorithm()` - Set reinforcement learning algorithm
- `setLearningRate()` - Set learning rate
- `setDiscountFactor()` - Set discount factor
- `setExplorationRate()` - Set exploration rate
- `setTargetUpdateFrequency()` - Set target network update frequency
- `setEntropyBonus()` - Set entropy bonus for exploration
- `setRewardFunction()` - Set reward function
- `setExperienceReplayCapacity()` - Set experience replay buffer capacity
- `setCurriculumLevel()` - Set curriculum learning level
- `train()` - Train the agent
- `getPolicy()` - Get the learned policy
- `getExperienceReplayBuffer()` - Get experience replay buffer
- `getPerformanceMetrics()` - Get performance metrics

### 3. CurriculumLearner

**Purpose**: Curriculum learning framework for progressive skill acquisition and difficulty progression.

**Key Features**:
- Progressive difficulty learning
- Skill dependency management
- Performance-based progression
- Difficulty adaptation
- Multi-task curriculum
- Skill mastery tracking
- Difficulty scheduling
- Performance requirements

**Python API**:
```python
import pynlm

# Create curriculum learner
learner = pynlm.CurriculumLearner()

# Define curriculum tasks
tasks = [
    pynlm.CurriculumLearner.CurriculumTask("basic_movement", 1),
    pynlm.CurriculumLearner.CurriculumTask("navigation", 2),
    pynlm.CurriculumLearner.CurriculumTask("exploration", 3),
    pynlm.CurriculumLearner.CurriculumTask("problem_solving", 4),
]

learner.setCurriculum(tasks)

# Set performance requirements
performance_requirements = {
    "basic_movement": {"min_reward": 0.5, "min_episodes": 100},
    "navigation": {"min_reward": 0.7, "min_episodes": 200},
    "exploration": {"min_reward": 0.6, "min_episodes": 150},
    "problem_solving": {"min_reward": 0.8, "min_episodes": 300},
}
learner.setPerformanceRequirements(performance_requirements)

# Set difficulty progression
learner.setDifficultyProgression("linear")
learner.setProgressionRate(1.0)

# Set adaptation strategy
learner.setAdaptationStrategy("performance_based")
learner.setAdaptationRate(0.1)

# Start curriculum learning
brain = pynlm.createQuickTestBrain()
brain.initialize()
learner.train(brain, max_episodes=1000)

# Get curriculum progress
progress = learner.getCurriculumProgress()
print(f"Current task: {progress.current_task}")
print(f"Task progress: {progress.task_progress}")
print(f"Overall progress: {progress.overall_progress}")

# Get mastery levels
mastery = learner.getMasteryLevels()
```

**Key Methods**:
- `setCurriculum()` - Set curriculum tasks
- `setPerformanceRequirements()` - Set performance requirements for tasks
- `setDifficultyProgression()` - Set difficulty progression strategy
- `setProgressionRate()` - Set progression rate
- `setAdaptationStrategy()` - Set adaptation strategy
- `setAdaptationRate()` - Set adaptation rate
- `train()` - Train the agent with curriculum learning
- `getCurriculumProgress()` - Get curriculum progress
- `getMasteryLevels()` - Get mastery levels for tasks
- `getCurrentTask()` - Get current task
- `getTaskDifficulty()` - Get task difficulty

### 4. SimulationScheduler

**Purpose**: Advanced simulation scheduler for efficient and coordinated training across multiple agents and environments.

**Key Features**:
- Parallel simulation execution
- Synchronous/asynchronous scheduling
- Resource management
- Load balancing
- Dynamic scheduling
- Simulation time management
- Environment management
- Agent coordination

**Python API**:
```python
import pynlm

# Create simulation scheduler
config = pynlm.SimulationScheduler.SchedulerConfig()
config.num_workers = 4
config.simulation_mode = pynlm.SimulationScheduler.SimulationMode.ASYNCHRONOUS
config.resource_limit = 0.8

scheduler = pynlm.SimulationScheduler(config)

# Add training environments
environments = []
for i in range(3):
    env = pynlm.createTrainingEnvironment()
    environments.append(env)

scheduler.addEnvironments(environments)

# Add agents
agents = []
for i in range(3):
    agent = pynlm.createAgentBrain(pynlm.createQuickTestBrain())
    agents.append(agent)

scheduler.addAgents(agents)

# Start training
results = scheduler.train(config, max_episodes=500)

# Get simulation statistics
stats = scheduler.getSimulationStatistics()
print(f"Total simulations: {stats.total_simulations}")
print(f"Average simulation time: {stats.avg_simulation_time}")
print(f"Parallel efficiency: {stats.parallel_efficiency}")

# Get resource utilization
resources = scheduler.getResourceUtilization()
print(f"CPU utilization: {resources.cpu_utilization}")
print(f"Memory utilization: {resources.memory_utilization}")
```

**Key Methods**:
- `addEnvironment()` - Add environment to scheduler
- `addAgent()` - Add agent to scheduler
- `train()` - Run training with multiple agents
- `pause()` - Pause training
- `resume()` - Resume training
- `isRunning()` - Check if training is running
- `getSimulationStatistics()` - Get simulation statistics
- `getResourceUtilization()` - Get resource utilization
- `getEnvironmentStatus()` - Get environment status
- `getAgentStatus()` - Get agent status

---

## Convenience Functions

**Purpose**: High-level convenience functions for common training tasks.

**Available Functions**:
- `createTrainingEnvironment()` - Create a complete training environment
- `createEvaluationEnvironment()` - Create an evaluation environment with advanced features
- `runReinforcementTraining()` - Run reinforcement learning training
- `runCurriculumTraining()` - Run curriculum learning training
- `runMetaLearning()` - Run meta-learning training
- `evaluatePolicy()` - Evaluate a trained policy
- `visualizeTrainingProgress()` - Visualize training progress

**Python API**:
```python
import pynlm

# Create training environment
brain, world, agent = pynlm.createTrainingEnvironment()
brain.initialize()

# Create evaluation environment
brain, world, agent = pynlm.createEvaluationEnvironment()
brain.initialize()

# Run reinforcement training
learner = pynlm.ReinforcementLearner()
learner.setAlgorithm(pynlm.ReinforcementLearner.Algorithm.Q_LEARNING)
results = learner.train(brain, max_episodes=1000)

# Run curriculum training
curriculum_learner = pynlm.CurriculumLearner()
tasks = [
    pynlm.CurriculumLearner.CurriculumTask("basic", 1),
    pynlm.CurriculumLearner.CurriculumTask("intermediate", 2),
    pynlm.CurriculumLearner.CurriculumTask("advanced", 3),
]
curriculum_learner.setCurriculum(tasks)
curriculum_learner.train(brain, max_episodes=500)

# Run meta-learning
meta_learner = pynlm.MetaLearner()
meta_learner.train(brain, meta_episodes=100, inner_episodes=50)

# Evaluate policy
learner = pynlm.ReinforcementLearner()
learner.load("trained_agent.pkl")
policy = learner.getPolicy()
performance = pynlm.evaluatePolicy(policy, environment, num_episodes=100)

# Visualize training progress
pynlm.visualizeTrainingProgress(training_results, save_path="training_progress.png")
```

---

## Usage Examples

### Example 1: Basic Reinforcement Learning

```python
import pynlm

# Create training environment
brain, world, agent = pynlm.createTrainingEnvironment()
brain.initialize()

# Create reinforcement learner
learner = pynlm.ReinforcementLearner()
learner.setAlgorithm(pynlm.ReinforcementLearner.Algorithm.Q_LEARNING)
learner.setLearningRate(0.001)
learner.setDiscountFactor(0.99)
learner.setExplorationRate(0.1)

# Set reward function
def reward_function(brain, world, agent, state):
    if world:
        return world.computeReward(brain)
    return 0.0

learner.setRewardFunction(reward_function)

# Set up experience replay
learner.setExperienceReplayCapacity(10000)

# Train the agent
print("Starting reinforcement learning training...")
learner.train(brain, max_episodes=1000)

# Save the trained agent
learner.save("trained_agent.pkl")

# Evaluate the trained agent
performance = learner.evaluate(brain, num_episodes=100)
print(f"Training complete! Average reward: {performance.average_reward}")
```

### Example 2: Curriculum Learning

```python
import pynlm

# Create curriculum learner
curriculum_learner = pynlm.CurriculumLearner()

# Define curriculum tasks
tasks = [
    pynlm.CurriculumLearner.CurriculumTask("basic_movement", 1),
    pynlm.CurriculumLearner.CurriculumTask("navigation", 2),
    pynlm.CurriculumLearner.CurriculumTask("exploration", 3),
    pynlm.CurriculumLearner.CurriculumTask("problem_solving", 4),
]

curriculum_learner.setCurriculum(tasks)

# Set performance requirements
performance_requirements = {
    "basic_movement": {"min_reward": 0.5, "min_episodes": 100},
    "navigation": {"min_reward": 0.7, "min_episodes": 200},
    "exploration": {"min_reward": 0.6, "min_episodes": 150},
    "problem_solving": {"min_reward": 0.8, "min_episodes": 300},
}

curriculum_learner.setPerformanceRequirements(performance_requirements)

# Set difficulty progression
learner.setDifficultyProgression("exponential")
learner.setProgressionRate(1.5)

# Start curriculum learning
brain = pynlm.createQuickTestBrain()
brain.initialize()

print("Starting curriculum learning...")
curriculum_learner.train(brain, max_episodes=1000)

# Get curriculum progress
progress = curriculum_learner.getCurriculumProgress()
print(f"Curriculum completed: {progress.overall_progress}")
print(f"Current task: {progress.current_task}")
print(f"Mastery levels: {progress.mastery_levels}")
```

### Example 3: Experience Replay

```python
import pynlm

# Create reinforcement learner with experience replay
learner = pynlm.ReinforcementLearner()
learner.setAlgorithm(pynlm.ReinforcementLearner.Algorithm.Q_LEARNING)
learner.setExperienceReplayCapacity(10000)

# Enable experience replay
learner.enableExperienceReplay(True)
learner.setReplayStartSize(1000)  # Start learning after collecting 1000 experiences
learner.setReplayFrequency(4)   # Sample 4 experiences per update

# Create training environment
brain, world, agent = pynlm.createTrainingEnvironment()
brain.initialize()

# Set reward function
def reward_function(brain, world, agent, state):
    if world:
        return world.computeReward(brain)
    return 0.0

learner.setRewardFunction(reward_function)

# Train with experience replay
print("Starting training with experience replay...")
learner.train(brain, max_episodes=2000)

# Get experience replay buffer
replay_buffer = learner.getExperienceReplayBuffer()
print(f"Experience replay buffer size: {len(replay_buffer)}")
print(f"Buffer capacity: {learner.getExperienceReplayCapacity()}")
```

### Example 4: Parallel Training

```python
import pynlm

# Create simulation scheduler
config = pynlm.SimulationScheduler.SchedulerConfig()
config.num_workers = 4
config.simulation_mode = pynlm.SimulationScheduler.SimulationMode.ASYNCHRONOUS
config.resource_limit = 0.8

scheduler = pynlm.SimulationScheduler(config)

# Add multiple training environments
environments = []
for i in range(3):
    env = pynlm.createTrainingEnvironment()
    environments.append(env)
    scheduler.addEnvironment(env)

# Add multiple agents
agents = []
for i in range(3):
    agent = pynlm.createAgentBrain(pynlm.createQuickTestBrain())
    agents.append(agent)
    scheduler.addAgent(agent)

# Start parallel training
print("Starting parallel training...")
results = scheduler.train(config, max_episodes=500)

# Get simulation statistics
stats = scheduler.getSimulationStatistics()
print(f"Total simulations: {stats.total_simulations}")
print(f"Average simulation time: {stats.avg_simulation_time}")
print(f"Parallel efficiency: {stats.parallel_efficiency}")

# Get resource utilization
resources = scheduler.getResourceUtilization()
print(f"CPU utilization: {resources.cpu_utilization}")
print(f"Memory utilization: {resources.memory_utilization}")
```

---

## Best Practices

### Reinforcement Learning
```python
# Use appropriate exploration strategies
learner.setExplorationRate(0.1)  # Start with 10% exploration
learner.annealExplorationRate(0.001)  # Linearly decrease exploration

# Use target networks for stability
learner.setTargetUpdateFrequency(100)  # Update target every 100 steps

# Use reward shaping for better learning
learner.setRewardShaping(True)
learner.setSuccessBonus(1.0)
learner.setFailurePenalty(-1.0)

# Monitor learning progress
learner.setEarlyStopping(True)
learner.setEarlyStoppingPatience(50)
learner.setEarlyStoppingMetric("average_reward")
```

### Curriculum Learning
```python
# Design curriculum with gradual difficulty increase
tasks = [
    pynlm.CurriculumLearner.CurriculumTask("simple", 1),
    pynlm.CurriculumLearner.CurriculumTask("medium", 2),
    pynlm.CurriculumLearner.CurriculumTask("hard", 3),
]

# Set realistic performance requirements
performance_requirements = {
    "simple": {"min_reward": 0.6, "min_episodes": 50},
    "medium": {"min_reward": 0.7, "min_episodes": 100},
    "hard": {"min_reward": 0.8, "min_episodes": 150},
}

# Use performance-based adaptation
learner.setAdaptationStrategy("performance_based")
learner.setAdaptationRate(0.1)
```

### Experience Replay
```python
# Set appropriate replay buffer size
learner.setExperienceReplayCapacity(10000)

# Start learning at appropriate time
learner.setReplayStartSize(1000)

# Use appropriate sampling
learner.setReplayFrequency(4)
learner.setImportanceSampling(True)
learner.setAlpha(0.6)

# Monitor buffer health
learner.setBufferHealthCheck(True)
learner.setBufferMinSize(100)
```

---

## Integration with Other NLM Components

### Integration with Brain
- Automatic learning signal integration
- Weight updates from learner
- Synaptic plasticity integration
- Neuromodulation support

### Integration with World
- Environment interaction for learning
- Reward calculation
- State observation
- Action execution

### Integration with Agent
- Sensory input processing
- Motor output generation
- Neuromodulation integration
- Development system interaction

### Integration with Experiment Management
- Experiment metrics integration
- Training progress tracking
- Result recording
- Performance monitoring

---

## Performance Considerations

### Memory Optimization
```python
# Use smaller replay buffers for memory-constrained environments
learner.setExperienceReplayCapacity(1000)

# Use compressed experience replay
learner.setCompressExperienceReplay(True)

# Use prioritized experience replay
learner.setPrioritizedReplay(True)
```

### Speed Optimization
```python
# Use parallel processing for faster learning
learner.enableParallelLearning(True)

# Use asynchronous updates
learner.enableAsynchronousUpdates(True)

# Use GPU acceleration if available
learner.useGPU(True)
```

### Monitoring Performance
```python
# Monitor learning progress
learner.setLearningProgressCallback(learning_progress_callback)

# Monitor performance metrics
learner.setPerformanceMetricsCallback(performance_metrics_callback)

# Generate training reports
learner.generateTrainingReport("training_report.html")
```

---

## Conclusion

The NLM Training Loop and Reinforcement Learning framework provides a comprehensive and flexible system for training neural learning machines using various learning paradigms. It includes:

- **Multiple learning algorithms** - Q-learning, policy gradient, actor-critic
- **Experience replay** - Memory-based learning from past experiences
- **Curriculum learning** - Progressive difficulty learning
- **Parallel training** - Efficient training across multiple processes
- **Meta-learning** - Learning to learn
- **Performance monitoring** - Real-time performance tracking
- **Reward shaping** - Custom reward functions
- **Exploration strategies** - Balanced exploration-exploitation

This framework enables researchers and developers to:
- Train agents in complex environments
- Learn efficient policies
- Generalize learned skills
- Optimize learning performance
- Debug learning problems
- Reproduce experiments

The training loop framework is an essential component of the NLM system, providing the infrastructure needed for advanced training and learning research with neural learning machines.

---

*Last updated: 2026-09-28*

This documentation provides comprehensive guidance for using the NLM Training Loop and Reinforcement Learning framework to design, run, and analyze training and learning experiments with neural learning machines.