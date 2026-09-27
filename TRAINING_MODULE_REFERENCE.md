# NLM Training Module Reference
# ============================

Complete API reference for the NLM training module, including training classes,
configuration options, callback system, and advanced training features.

## Overview

The NLM training module provides a comprehensive framework for training NLM brains
across various tasks while maintaining biological plausibility and performance.
It includes training loops, optimization, callbacks, and common training patterns.

### Training Module Components

1. **TrainingCore Class**: Main training loop, evaluation, and hyperparameter optimization
2. **TrainingConfig Class**: Configuration for training runs with episodes, learning rates, and stopping criteria
3. **Callback and Monitoring System**: Event-driven callbacks and performance metrics collection
4. **Advanced Training Features**: Curiosity-driven exploration, curriculum learning, transfer learning, and meta-learning
5. **Common Training Patterns**: RL, SL, US, and hybrid pattern implementations
6. **Save/Load Training**: Training session serialization and experiment tracking

## TrainingCore Class

The `TrainingCore` class provides the main training loop and evaluation capabilities:

### Core Methods

```python
class TrainingCore:
    def __init__(self)
        Initialize training controller
    
    def trainAgent(self, brain, world, agent, training_config, callbacks=None)
        Run complete training loop for an agent
        
        Args:
            brain: Brain object
            world: World object  
            agent: Agent object
            training_config: Training configuration
            callbacks: List of training callbacks
        
        Returns:
            True if training completed successfully
    
    def evaluateAgent(self, brain, world, agent, training_config, num_episodes=10)
        Evaluate agent performance on test cases
        
        Args:
            brain: Brain object
            world: World object
            agent: Agent object
            training_config: Training configuration
            num_episodes: Number of evaluation episodes
        
        Returns:
            Average performance score
```

### Training Loop Management

```python
def setupTraining(self, brain, world, agent, config):
    """Initialize training environment."""
    # Configure subsystems
    brain.reset()
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    # Enable requested features
    if config.rewardModulation:
        agent.enableRewardModulation(True)
    if config.structuralPlasticity:
        agent.enableStructuralPlasticity(True)
    if config.development:
        agent.enableDevelopment(True)
    if config.curiosity:
        agent.enableCuriosity(True)

def runEpisode(self, episode, config, callbacks):
    """Run a single training episode."""
    episode_reward = 0.0
    
    for step in range(config.maxStepsPerEpisode):
        # Update world
        world.update(config.timeStep)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate and apply reward
        reward = self.calculateReward(world, brain, action, step)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(config.timeStep)
        
        # Call callbacks
        if callbacks:
            for callback in callbacks:
                callback.afterStep(episode, step, reward, 0.0)  # loss placeholder
        
        episode_reward += reward
    
    # Call episode end callback
    if callbacks:
        for callback in callbacks:
            callback.afterEpisode(episode, episode_reward, self.isEpisodeSuccessful(episode))
    
    return episode_reward
```

## TrainingConfig Class

### Training Configuration

The `TrainingConfig` class manages all training hyperparameters and settings:

```python
class TrainingConfig:
    def __init__(self):
        # Basic training parameters
        self.maxEpisodes = 1000
        self.maxStepsPerEpisode = 200
        self.timeStep = 0.1
        self.learningRate = 0.001
        self.discountFactor = 0.99
        self.explorationRate = 0.1
        
        # Reward configuration
        self.rewardConfig = RewardConfig()
        
        # Stopping criteria
        self.earlyStopping = True
        self.patience = 20
        self.minPerformance = 0.8
        self.improvementThreshold = 0.01
        
        # Performance monitoring
        self.checkpointFrequency = 50
        self.saveCheckpoints = True
        self.logFrequency = 10
        
        # Advanced features
        self.curiosityEnabled = True
        self.curriculumEnabled = False
        self.transferEnabled = False
        self.metaLearningEnabled = False
```

### Reward Configuration

```python
class RewardConfig:
    def __init__(self):
        self.type = RewardType.WEIGHTED
        self.weighted = WeightedRewardConfig()
        self.goalBased = GoalBasedRewardConfig()
        self.adversarial = AdversarialRewardConfig()

class WeightedRewardConfig:
    def __init__(self):
        # Reward component weights
        self.stepCompletion = 1.0
        self.novelty = 0.5
        self.survival = 0.8
        self.efficiency = 1.0
        self.curiosity = 0.3
        self.predictionError = 0.5
        self.social = 0.6
        self.competition = 0.5

class GoalBasedRewardConfig:
    def __init__(self):
        self.goalX = 0.0
        self.goalY = 0.0
        self.goalRadius = 1.0
        self.goalReward = 10.0
        self.proximityReward = 1.0

class AdversarialRewardConfig:
    def __init__(self):
        self.opponentModel = None
        self.exploitabilityReward = 5.0
        self.robustnessReward = 2.0
```

## Callback System

### Training Callbacks

The callback system enables event-driven training monitoring and custom behavior:

```python
class TrainingCallback:
    """Base class for training callbacks."""
    
    def beforeTraining(self, trainer, brain, world, agent, config):
        """Called before training starts."""
        pass
    
    def afterEpisode(self, episode, reward, success):
        """Called after each training episode."""
        pass
    
    def afterStep(self, episode, step, reward, loss):
        """Called after each training step."""
        pass
    
    def beforeEvaluation(self, trainer, brain, world, agent, config):
        """Called before evaluation starts."""
        pass
    
    def afterEvaluation(self, episode, reward, success):
        """Called after each evaluation episode."""
        pass
    
    def onTrainingEnd(self, trainer, results):
        """Called when training completes."""
        pass
    
    def onTrainingStop(self, trainer, results, reason):
        """Called when training stops early."""
        pass
```

### Simple Training Callback

```python
class SimpleTrainingCallback(TrainingCallback):
    """Simple callback that logs episode and step information."""
    
    def __init__(self):
        self.episode_rewards = []
        self.step_losses = []
        self.episode_count = 0
        self.best_reward = float('-inf')
        
    def afterEpisode(self, episode, reward, success):
        self.episode_rewards.append(reward)
        self.episode_count = episode + 1
        
        if reward > self.best_reward:
            self.best_reward = reward
        
        # Log progress
        if episode % 50 == 0:
            avg_reward = sum(self.episode_rewards) / len(self.episode_rewards)
            print(f"    Episode {episode}: Reward = {reward:.2f}, "
                  f"Success = {success}, Avg = {avg_reward:.2f}")
    
    def afterStep(self, episode, step, reward, loss):
        self.step_losses.append(loss)
    
    def getPerformanceSummary(self):
        """Get comprehensive performance summary."""
        if not self.episode_rewards:
            return {
                'episodes_completed': 0,
                'status': 'no_data'
            }
        
        avg_reward = sum(self.episode_rewards) / len(self.episode_rewards)
        avg_loss = sum(self.step_losses) / len(self.step_losses) if self.step_losses else 0
        
        return {
            'episodes_completed': len(self.episode_rewards),
            'total_reward': sum(self.episode_rewards),
            'average_reward': avg_reward,
            'best_reward': max(self.episode_rewards),
            'worst_reward': min(self.episode_rewards),
            'average_loss': avg_loss,
            'episodes_successful': sum(1 for r in self.episode_rewards if r > 10.0),
            'success_rate': sum(1 for r in self.episode_rewards if r > 10.0) / len(self.episode_rewards),
            'learning_progress': {
                'initial_performance': self.episode_rewards[0] if len(self.episode_rewards) > 0 else 0,
                'final_performance': self.episode_rewards[-1] if len(self.episode_rewards) > 0 else 0,
                'improvement': (self.episode_rewards[-1] - (self.episode_rewards[0] if len(self.episode_rewards) > 0 else 0)) / max(1, len(self.episode_rewards) - 1)
            }
        }
```

## Advanced Training Features

### Curiosity-Driven Exploration

```python
class CuriositySystem:
    def __init__(self):
        self.intrinsicMotivationEnabled = True
        self.explorationBonus = 0.1
        self.noveltyThreshold = 0.5
        self.uncertaintyEstimation = True
    
    def calculateCuriosityBonus(self, world, brain, action):
        """Calculate intrinsic motivation bonus."""
        bonus = 0.0
        
        # Novelty bonus based on prediction error
        if self.uncertaintyEstimation:
            prediction_error = self.getPredictionError(brain)
            if prediction_error > self.noveltyThreshold:
                bonus += self.explorationBonus * prediction_error
        
        # Randomness bonus for exploration
        if np.random.random() < self.explorationRate:
            bonus += self.explorationBonus
        
        return bonus
    
    def getPredictionError(self, brain):
        """Estimate prediction error for curiosity."""
        # Placeholder implementation
        return np.random.uniform(0, 1)
```

### Curriculum Learning

```python
def runCurriculumLearning(training_core, brain, world, agent, config):
    """Run curriculum learning with progressively difficult tasks."""
    difficulty = 1  # Start with difficulty level 1
    max_difficulty = 10
    successThreshold = 0.8
    
    for episode in range(config.maxEpisodes):
        # Set curriculum difficulty based on performance
        if difficulty < max_difficulty and episode % 100 == 0:
            difficulty += 1
            print(f"Curriculum progression: Level {difficulty}")
        
        # Configure task based on difficulty
        config = getDifficultyConfig(difficulty)
        
        # Run episode with current difficulty
        reward = training_core.runEpisode(episode, config, [])
        success = reward > config.minRewardForSuccess
        
        # Progress curriculum if successful
        if success:
            print(f"Difficulty {difficulty} mastered - progressing to {difficulty + 1}")
        else:
            print(f"Difficulty {difficulty} not mastered - repeating")
            # Repeat same difficulty until successful
            if episode > 0:
                episode -= 1  # Repeat previous episode
        
        # Check if all levels mastered
        if difficulty >= max_difficulty and success:
            print("Curriculum completed successfully!")
            break
def getDifficultyConfig(difficulty):
    """Get configuration for specific difficulty level."""
    config = TrainingConfig()
    
    # Scale parameters based on difficulty
    config.maxStepsPerEpisode = min(50 + difficulty * 10, 500)
    config.explorationRate = min(0.1 + difficulty * 0.05, 0.8)
    config.learningRate = max(0.001 / difficulty, 0.0001)
    config.discountFactor = 0.9 + difficulty * 0.01
    
    # Adjust goal proximity based on difficulty
    config.goalX = np.random.uniform(0, 10 - difficulty)
    config.goalY = np.random.uniform(0, 10 - difficulty)
    
    return config
```

## Common Training Patterns

### Reinforcement Learning Pattern

```python
def runReinforcementLearningPattern(trainer, brain, world, agent, config, callback):
    """Run reinforcement learning pattern with reward-based learning."""
    print("=== Running Reinforcement Learning Pattern ===")
    
    # Enable RL-specific features
    trainer.setupForRL(config)
    
    # Run training episodes
    for episode in range(config.maxEpisodes):
        # Execute RL episode
        reward = trainer.runRLEpisode(episode, config, callback)
        
        # Log progress
        if episode % 100 == 0:
            print(f"    Episode {episode}: Reward = {reward:.2f}")
        
        # Check stopping criteria
        if trainer.shouldStopRL(config, callback):
            break
    
    return True
def runReinforcementLearningPattern(trainer, brain, world, agent, config, callback):
    """Run reinforcement learning pattern with reward-based learning."""
    print("=== Running Reinforcement Learning Pattern ===")
    
    # Enable RL-specific features
    trainer.setupForRL(config)
    
    # Run training episodes
    for episode in range(config.maxEpisodes):
        # Execute RL episode
        reward = trainer.runRLEpisode(episode, config, callback)
        
        # Log progress
        if episode % 100 == 0:
            print(f"    Episode {episode}: Reward = {reward:.2f}")
        
        # Check stopping criteria
        if trainer.shouldStopRL(config, callback):
            break
    
    return True
```

### Supervised Learning Pattern

```python
def runSupervisedLearningPattern(trainer, brain, world, agent, config, callback):
    """Run supervised learning pattern with target-driven learning."""
    print("=== Running Supervised Learning Pattern ===")
    
    # Prepare training data
    training_data = trainer.prepareSupervisedData(config)
    
    # Train using supervised learning
    for epoch in range(config.epochs):
        epoch_loss = 0.0
        
        for batch in training_data.getBatches(config.batchSize):
            loss = trainer.trainSupervisedBatch(brain, batch, config)
            epoch_loss += loss
            
            if callback:
                callback.afterStep(epoch, batch[0].step, batch[0].reward, loss)
        
        print(f"    Epoch {epoch}: Loss = {epoch_loss:.4f}")
        
        # Evaluate on validation set
        if config.validationSplit > 0:
            val_loss = trainer.evaluateSupervised(brain, training_data.validation, config)
    
    return True
```

### Unsupervised Learning Pattern

```python
def runUnsupervisedLearningPattern(trainer, brain, world, agent, config, callback):
    """Run unsupervised learning pattern with self-supervised learning."""
    print("=== Running Unsupervised Learning Pattern ===")
    
    # Initialize auto-encoder or similar architecture
    autoencoder = trainer.setupAutoencoder(brain, config)
    
    for epoch in range(config.epochs):
        epoch_loss = 0.0
        
        for batch in trainer.getTrainingBatches(config):
            # Encode input
            encoded = autoencoder.encode(batch.input)
            
            # Decode reconstruction
            reconstructed = autoencoder.decode(encoded)
            
            # Calculate reconstruction loss
            loss = autoencoder.reconstructionLoss(batch.input, reconstructed)
            
            # Add regularization losses
            if config.useNegativeSamples:
                negative_loss = autoencoder.contrastiveLoss(batch.negative_samples)
                loss += config.contrastiveWeight * negative_loss
            
            if config.regularizationWeight > 0:
                reg_loss = autoencoder.regularizationLoss()
                loss += config.regularizationWeight * reg_loss
            
            # Update autoencoder
            autoencoder.update(loss, config.learningRate)
            
            epoch_loss += loss.item()
            
            if callback:
                callback.afterStep(epoch, batch[0].step, batch[0].reward, loss.item())
        
        print(f"    Epoch {epoch}: Loss = {epoch_loss:.4f}")
    
    return True
```

### Hybrid Training Pattern

```python
def runHybridTrainingPattern(trainer, brain, world, agent, config, callback):
    """Run hybrid training pattern combining multiple learning paradigms."""
    print("=== Running Hybrid Training Pattern ===")
    
    # Initialize components
    rl_agent = ReinforcementLearningComponent(brain, config.rlWeight)
    sl_learner = SupervisedLearningComponent(brain, config.slWeight)
    
    interleaved_episodes = config.interleavedEpisodes or 10
    
    for cycle in range(config.maxCycles):
        print(f"Hybrid cycle {cycle + 1}/{config.maxCycles}")
        
        # RL phase
        for episode in range(interleaved_episodes):
            reward = rl_agent.trainEpisode(brain, world, agent, config)
            
            if callback:
                callback.afterStep(cycle * interleaved_episodes + episode, episode, reward, 0.0)
        
        # SL phase
        for batch in sl_learner.getTrainingBatches(config):
            loss = sl_learner.trainBatch(brain, batch, config)
            
            if callback:
                callback.afterStep(cycle * interleaved_episodes + episode + 0.5, batch[0].step, 0.0, loss)
        
        # Evaluation phase
        if callback:
            performance = trainer.evaluateAgent(brain, world, agent, config)
            callback.afterEpisode(cycle * interleaved_episodes, performance, performance > config.targetPerformance)
        
        # Adjust weights based on performance
        if cycle > 0:
            config.rl_weight, config.sl_weight = trainer.adjustHybridWeights(
                config.rl_weight, config.sl_weight, performance
            )
        
        print(f"    Weights: RL={config.rl_weight:.2f}, SL={config.sl_weight:.2f}, Performance={performance:.3f}")
    
    return True
```

## Save/Load Training

### Training Persistence

```python
class TrainingPersistence:
    def __init__(self, save_dir="checkpoints"):
        self.save_dir = save_dir
        os.makedirs(save_dir, exist_ok=True)
    
    def saveTrainingSession(self, filepath, brain, agent, episode):
        """Save complete training session state."""
        try:
            # Create checkpoint directory
            checkpoint_dir = os.path.join(self.save_dir, f"checkpoint_{episode}")
            os.makedirs(checkpoint_dir, exist_ok=True)
            
            # Save brain state
            brain_path = os.path.join(checkpoint_dir, "brain.nlm")
            brain.save(brain_path)
            
            # Save agent state
            agent_path = os.path.join(checkpoint_dir, "agent.nlm")
            agent.save(agent_path)
            
            # Save training metadata
            metadata = {
                'episode': episode,
                'timestamp': time.time(),
                'brain_config': brain.getConfig(),
                'agent_config': agent.getConfig()
            }
            
            metadata_path = os.path.join(checkpoint_dir, "metadata.json")
            with open(metadata_path, 'w') as f:
                json.dump(metadata, f, indent=2)
            
            # Create experiment record
            experiment_path = os.path.join(self.save_dir, "experiments.json")
            experiment_record = self.loadExperimentRecord() if os.path.exists(experiment_path) else {
                'experiments': [],
                'checkpoints': []
            }
            
            experiment_record['experiments'].append({
                'episode': episode,
                'brain_path': brain_path,
                'agent_path': agent_path,
                'timestamp': metadata['timestamp']
            })
            
            experiment_record['checkpoints'].append(checkpoint_dir)
            
            with open(experiment_path, 'w') as f:
                json.dump(experiment_record, f, indent=2)
            
            print(f"✓ Training session saved: {checkpoint_dir}")
            return True
            
        except Exception as e:
            print(f"✗ Failed to save training session: {e}")
            return False
    
    def loadTrainingSession(self, filepath, trainer, brain, agent, target_episode):
        """Load training session from file."""
        try:
            # Load experiment record
            experiment_path = os.path.join(self.save_dir, "experiments.json")
            experiment_record = self.loadExperimentRecord()
            
            # Find matching checkpoint
            for experiment in experiment_record['experiments']:
                if experiment['episode'] == target_episode:
                    # Load brain state
                    brain.load(experiment['brain_path'])
                    
                    # Load agent state
                    agent.load(experiment['agent_path'])
                    
                    print(f"✓ Training session loaded: Episode {target_episode}")
                    return True
            
            print(f"✗ No training session found for episode {target_episode}")
            return False
            
        except Exception as e:
            print(f"✗ Failed to load training session: {e}")
            return False
    
    def loadExperimentRecord(self):
        """Load experiment record from file."""
        experiment_path = os.path.join(self.save_dir, "experiments.json")
        
        if os.path.exists(experiment_path):
            with open(experiment_path, 'r') as f:
                return json.load(f)
        
        return {'experiments': [], 'checkpoints': []}
    
    def analyzeExperiment(self, episode):
        """Analyze experiment data and provide insights."""
        try:
            experiment_path = os.path.join(self.save_dir, "experiments.json")
            experiment_record = self.loadExperimentRecord()
            
            # Find experiment
            for experiment in experiment_record['experiments']:
                if experiment['episode'] == episode:
                    # Load all checkpoints up to this episode
                    checkpoints = [exp for exp in experiment_record['experiments'] if exp['episode'] <= episode]
                    
                    if len(checkpoints) < 2:
                        return {"message": "Not enough data for analysis"}
                    
                    # Analyze learning progress
                    rewards = []
                    for exp in checkpoints:
                        # Load experiment data (simplified)
                        reward = exp.get('reward', 0.0)
                        rewards.append(reward)
                    
                    # Calculate statistics
                    analysis = {
                        'total_episodes': len(checkpoints),
                        'final_reward': rewards[-1],
                        'average_reward': sum(rewards) / len(rewards),
                        'best_reward': max(rewards),
                        'worst_reward': min(rewards),
                        'reward_trend': self.calculateTrend(rewards),
                        'stability': self.calculateStability(rewards)
                    }
                    
                    return analysis
            
            return {"message": "Experiment not found"}
            
        except Exception as e:
            return {"error": f"Analysis failed: {e}"}
    
    def calculateTrend(self, rewards):
        """Calculate learning trend."""
        if len(rewards) < 2:
            return 'insufficient_data'
        
        # Simple linear regression
        x = list(range(len(rewards)))
        y = rewards
        
        n = len(x)
        sum_x = sum(x)
        sum_y = sum(y)
        sum_xy = sum(xi * yi for xi, yi in zip(x, y))
        sum_x2 = sum(xi ** 2 for xi in x)
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x ** 2)
        
        if slope > 0.1:
            return 'improving'
        elif slope > 0.01:
            return 'slightly_improving'
        elif slope > -0.01:
            return 'stable'
        elif slope > -0.1:
            return 'declining'
        else:
            return 'strongly_declining'
    
    def calculateStability(self, rewards):
        """Calculate training stability."""
        if len(rewards) < 2:
            return 'unstable'
        
        # Calculate coefficient of variation
        mean_reward = sum(rewards) / len(rewards)
        if mean_reward == 0:
            return 'neutral'
        
        variance = sum((r - mean_reward) ** 2 for r in rewards) / len(rewards)
        std_dev = variance ** 0.5
        coefficient_of_variation = std_dev / mean_reward
        
        if coefficient_of_variation < 0.1:
            return 'stable'
        elif coefficient_of_variation < 0.3:
            return 'moderate'
        else:
            return 'unstable'
```

## Hyperparameter Optimization

### Automated Hyperparameter Tuning

```python
class HyperparameterOptimizer:
    def __init__(self):
        self.searchHistory = []
        self.bestParameters = None
        self.bestScore = float('-inf')
    
    def optimize(self, paramSpace, evaluationFunc, strategy='random_search', maxIterations=50):
        """Optimize hyperparameters using specified strategy."""
        print(f"Starting hyperparameter optimization with {strategy} strategy")
        print(f"Parameter space size: {len(paramSpace)} parameters")
        print(f"Max iterations: {maxIterations}")
        
        for iteration in range(maxIterations):
            print(f"\nIteration {iteration + 1}/{maxIterations}")
            
            # Generate parameters based on strategy
            if strategy == 'random_search':
                params = self.randomSearch(paramSpace)
            elif strategy == 'grid_search':
                params = self.gridSearch(paramSpace, iteration)
            elif strategy == 'bayesian_search':
                params = self.bayesianSearch(paramSpace, iteration)
            elif strategy == 'genetic_algorithm':
                params = self.geneticAlgorithm(paramSpace, iteration)
            else:
                raise ValueError(f"Unknown optimization strategy: {strategy}")
            
            # Evaluate parameters
            print(f"Testing parameters: {params}")
            score = evaluationFunc(params)
            
            # Record results
            self.searchHistory.append({
                'iteration': iteration,
                'parameters': params,
                'score': score,
                'success': score != float('-inf')
            })
            
            # Update best parameters
            if score > self.bestScore:
                self.bestScore = score
                self.bestParameters = params.copy()
                print(f"✓ New best score: {score:.4f}")
            
            # Print progress
            if (iteration + 1) % 10 == 0:
                self.printProgress(iteration + 1)
        
        # Generate recommendations
        recommendations = self.generateRecommendations()
        
        return self.searchHistory, recommendations
    
    def randomSearch(self, paramSpace):
        """Generate random parameter combination."""
        params = {}
        
        for param in paramSpace:
            if param.get('discrete', False):
                # Choose from discrete values
                params[param['name']] = np.random.choice(param['values'])
            else:
                # Sample from continuous range
                min_val = param.get('min', param.get('value', 0))
                max_val = param.get('max', param.get('value', 1))
                params[param['name']] = np.random.uniform(min_val, max_val)
        
        return params
    
    def bayesianSearch(self, paramSpace, iteration):
        """Bayesian optimization with surrogate model."""
        # Simplified implementation
        # In practice, would use Gaussian processes or other surrogate models
        
        if iteration == 0:
            # Initial random exploration
            return self.randomSearch(paramSpace)
        
        # Model-based selection (placeholder)
        return self.randomSearch(paramSpace)
    
    def geneticAlgorithm(self, paramSpace, iteration):
        """Genetic algorithm for hyperparameter optimization."""
        if iteration == 0:
            # Initialize population
            self.population = [self.randomSearch(paramSpace) for _ in range(10)]
            return self.population[0]
        
        # Simplified genetic operations
        return self.randomSearch(paramSpace)
    
    def generateRecommendations(self):
        """Generate recommendations based on optimization results."""
        recommendations = []
        
        if not self.searchHistory:
            return recommendations
        
        # Analyze best parameters
        if self.bestParameters:
            for param in self.searchHistory[0]['parameters']:
                name = param['name']
                value = self.bestParameters[name]
                
                # Generate insights based on parameter values
                if 'learning_rate' in name.lower():
                    if value < 0.0001:
                        recommendations.append("Very low learning rate - consider increasing for faster convergence")
                    elif value > 0.1:
                        recommendations.append("Very high learning rate - may cause instability")
                
                elif 'discount_factor' in name.lower():
                    if value < 0.9:
                        recommendations.append("Low discount factor - agent may focus on immediate rewards")
                    elif value > 0.999:
                        recommendations.append("High discount factor - agent plans far into future")
                
                elif 'exploration_rate' in name.lower():
                    if value < 0.01:
                        recommendations.append("Very low exploration - may get stuck in local optima")
                    elif value > 0.5:
                        recommendations.append("High exploration - may be inefficient")
        
        # Analyze convergence
        recent_scores = [h['score'] for h in self.searchHistory[-10:]]
        if len(recent_scores) >= 2:
            if max(recent_scores) - min(recent_scores) < 0.01:
                recommendations.append("Search appears to have converged - consider exploring other parameter ranges")
        
        return recommendations
    
    def printProgress(self, iteration):
        """Print optimization progress."""
        completed = [h for h in self.searchHistory if h['success']]
        if not completed:
            print(f"  Completed: 0/{iteration}")
            return
        
        scores = [h['score'] for h in completed]
        print(f"  Completed: {len(completed)}/{iteration}, "
              f"Best: {max(scores):.4f}, "
              f"Avg: {sum(scores)/len(scores):.4f}")
```

## Installation

### Training Module Dependencies

```bash
# Install core NLM
pip install pynlm

# Install training module with additional dependencies
pip install pynlm[training]

# Or install all development dependencies
pip install pynlm[dev]
```

### Building from Source

```bash
# Clone repository
https://github.com/your-username/nlm.git

# Create build directory
cd nlm
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Build Python bindings
pip install -e .
```

## Getting Started

### Basic Training

1. **Start with Example 1**: Simple reinforcement learning with basic callbacks
2. **Use the TrainingCore class**: Main training loop and evaluation
3. **Configure TrainingConfig**: Set hyperparameters and stopping criteria
4. **Implement custom callbacks**: Track performance and monitor training

### Advanced Training

1. **Experiment with different patterns**: RL, SL, US, and hybrid patterns
2. **Use hyperparameter optimization**: Automated parameter tuning
3. **Implement advanced features**: Curiosity-driven exploration, curriculum learning
4. **Set up experiment tracking**: Persistent experiment storage and analysis

### Research Experiments

1. **Integrate with your specific task** by creating custom reward functions
2. **Experiment with different training patterns** to find what works best for your use case
3. **Use hyperparameter optimization** to automate configuration discovery
4. **Set up experiment tracking** for reproducible research
5. **Combine multiple advanced features** for state-of-the-art training
6. **Deploy to production** using the training persistence and logging capabilities

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

### Solutions

**For "Quick test fails"**
```python
# Verify all steps are properly implemented
brain = create_simple_brain(neuron_count=500)
brain.initialize()  # This must be called before any operations

# Run quick test
results = quick_test(mode='connectivity', steps=200)
```

**For "Training takes too long"**
```python
training_config = TrainingConfig()
training_config.maxEpisodes = 100  # Reduced from 1000
training_config.minPerformance = 0.5  # Lower threshold
training_config.patience = 5  # Fewer patience steps
```

**For "Evaluation fails"**
```python
test_cases = [
    {'type': 'connectivity', 'steps': 100, 'target_metrics': {'min_active': 5}},
    {'type': 'firing', 'steps': 80, 'target_metrics': {'min_firing_rate': 1.0}},
    {'type': 'learning', 'steps': 150, 'target_metrics': {'stage_change': True}},
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