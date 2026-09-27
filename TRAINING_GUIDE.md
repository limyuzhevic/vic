# NLM Training Guide
# =================

This comprehensive guide documents the NLM training system, covering training module classes, training patterns, optimization techniques, and advanced training features for both Python and C++ implementations.

## Overview

The NLM training module provides a complete system for training NLM brains across various tasks while maintaining biological plausibility and simulation performance. It supports multiple training paradigms including reinforcement learning, supervised learning, unsupervised learning, and hybrid approaches.

### Training Module Architecture

The NLM training module consists of several key components:

1. **TrainingCore Class**: Main training loop, evaluation, and hyperparameter optimization
2. **TrainingConfig Class**: Configuration for training runs with episodes, learning rates, and stopping criteria
3. **Callback and Monitoring System**: Event-driven callbacks and performance metrics collection
4. **Advanced Training Features**: Curiosity-driven exploration, curriculum learning, transfer learning, and meta-learning
5. **Common Training Patterns**: RL, SL, US, and hybrid pattern implementations
6. **Save/Load Training**: Training session serialization and experiment tracking

## TrainingCore Class

### Main Training Controller

The `TrainingCore` class provides the main training loop and evaluation capabilities:

```python
class TrainingCore:
    def __init__(self):
        # Initialize training controller
        pass
    
    def trainAgent(self, brain, world, agent, training_config, callbacks=None):
        """Run complete training loop for an agent."""
        
        # Setup training environment
        self.setupTraining(brain, world, agent, training_config)
        
        # Main training loop
        for episode in range(training_config.maxEpisodes):
            self.runEpisode(episode, training_config, callbacks)
            
            # Check stopping criteria
            if self.shouldStopTraining(training_config, callbacks):
                break
        
        return True  # Training completed
    
    def evaluateAgent(self, brain, world, agent, training_config, num_episodes=10):
        """Evaluate agent performance on test cases."""
        
        # Reset brain and world for evaluation
        brain.reset()
        brain.initialize()
        world.reset()
        
        # Run evaluation episodes
        total_reward = 0.0
        for episode in range(num_episodes):
            reward = self.runEvaluationEpisode(episode, training_config, callbacks)
            total_reward += reward
        
        return total_reward / num_episodes
```

### Key Methods

#### Training Loop Management

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

class SimpleTrainingCallback(TrainingCallback):
    """Simple callback that logs episode and step information."""
    
    def __init__(self):
        self.episodeRewards = []
        self.stepLosses = []
        self.episodeCount = 0
    
    def afterEpisode(self, episode, reward, success):
        self.episodeRewards.append(reward)
        self.episodeCount = episode + 1
        
        if episode % 50 == 0:
            avg_reward = sum(self.episodeRewards) / len(self.episodeRewards)
            print(f"Episode {episode}: Reward = {reward:.2f}, Success = {success}, Avg = {avg_reward:.2f}")
    
    def afterStep(self, episode, step, reward, loss):
        self.stepLosses.append(loss)
    
    def getPerformanceSummary(self):
        """Get summary of training performance."""
        if not self.episodeRewards:
            return {"episodes_completed": 0}
        
        return {
            'episodes_completed': len(self.episodeRewards),
            'average_reward': sum(self.episodeRewards) / len(self.episodeRewards),
            'best_reward': max(self.episodeRewards),
            'worst_reward': min(self.episodeRewards),
            'total_steps': sum(1 for _ in self.stepLosses)
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
def runCurriculmLearning(training_core, brain, world, agent, config):
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

### Transfer Learning

```python
class TransferLearning:
    def __init__(self):
        self.sourceTasks = []
        self.transferWeight = 0.5
        self.adaptationRate = 0.1
    
    def transferFromSource(self, target_brain, source_task):
        """Transfer knowledge from source task to target brain."""
        # Extract features from source task
        source_features = self.extractTaskFeatures(source_task)
        
        # Find similar patterns in target brain
        similar_patterns = self.findSimilarPatterns(target_brain, source_features)
        
        # Apply transfer weights
        for pattern in similar_patterns:
            weight = self.transferWeight * pattern.similarity
            target_brain.applyTransfer(pattern, weight)
    
    def fineTuneTarget(self, target_brain, target_task, episodes):
        """Fine-tune transferred knowledge on target task."""
        for episode in range(episodes):
            # Apply transferred knowledge with adaptation
            self.applyAdaptedTransfer(target_brain, episode)
            
            # Update based on target task performance
            if episode % 10 == 0:
                self.adaptTransferWeights(target_brain, episode)

def extractTaskFeatures(task):
    """Extract relevant features from a task."""
    features = {
        'neuron_distribution': task.getNeuronDistribution(),
        'plasticity_patterns': task.getPlasticityPatterns(),
        'learning_curves': task.getLearningCurves(),
        'performance_metrics': task.getPerformanceMetrics()
    }
    return features
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
        if episode % 50 == 0:
            print(f"RL Episode {episode}: Reward = {reward:.3f}")
        
        # Check stopping criteria
        if trainer.shouldStopRL(config, callback):
            break
    
    return True

class ReinforcementLearningSetup:
    def setupForRL(self, config):
        """Configure for reinforcement learning."""
        config.rewardConfig.type = RewardType.WEIGHTED
        config.rewardConfig.weighted.stepCompletion = 1.0
        config.rewardConfig.weighted.exploration = 0.3
        config.explorationRate = 0.2
        config.discountFactor = 0.99
        config.curiosityEnabled = True
    
    def runRLEpisode(self, episode, config, callback):
        """Execute one RL episode."""
        episode_reward = 0.0
        
        for step in range(config.maxStepsPerEpisode):
            # Get state from world
            state = self.getWorldState()
            
            # Select action with exploration
            action = self.selectAction(state, config.explorationRate)
            
            # Execute action
            next_state, reward, done = self.executeAction(action)
            
            # Store experience for learning
            self.storeExperience(state, action, reward, next_state, done)
            
            # Learn from experience
            self.learnFromExperience()
            
            episode_reward += reward
            
            if done:
                break
        
        return episode_reward
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
        
        # Evaluate on validation set
        if config.validationSplit > 0:
            val_loss = trainer.evaluateSupervised(brain, training_data.validation, config)
        
        # Early stopping check
        if config.earlyStopping and epoch > config.patience:
            if val_loss > config.minImprovement:
                print(f"Early stopping at epoch {epoch}")
                break
        
        print(f"SL Epoch {epoch}: Loss = {epoch_loss:.4f}, Val Loss = {val_loss:.4f}")
    
    return True

class SupervisedLearningSetup:
    def prepareSupervisedData(self, config):
        """Prepare supervised learning data."""
        data = SupervisedData()
        
        # Generate input-output pairs
        for i in range(config.maxExamples):
            # Create example from brain state
            input_data = self.createNeuralInput(brain)
            
            # Get target output
            target = self.createNeuralTarget(brain, config)
            
            data.addExample(input_data, target)
        
        # Split into train/validation
        data.split(config.validationSplit)
        
        return data
    
    def trainSupervisedBatch(self, brain, batch, config):
        """Train one batch of supervised data."""
        total_loss = 0.0
        
        for input_data, target in batch:
            # Forward pass
            output = brain.forward(input_data)
            
            # Calculate loss
            loss = self.calculateLoss(output, target)
            
            # Backward pass
            brain.backward(loss, config.learningRate)
            
            total_loss += loss
        
        return total_loss / len(batch)
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
        
        print(f"US Epoch {epoch}: Loss = {epoch_loss:.4f}")
    
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
            config.rlWeight, config.slWeight = trainer.adjustHybridWeights(
                config.rlWeight, config.slWeight, performance
            )
        
        print(f"  Weights: RL={config.rlWeight:.2f}, SL={config.slWeight:.2f}, Performance={performance:.3f}")
    
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
            return "insufficient_data"
        
        # Simple linear regression
        x = list(range(len(rewards)))
        y = rewards
        
        n = len(x)
        sum_x = sum(x)
        sum_y = sum(y)
        sum_xy = sum(xi * yi for xi, yi in zip(x, y))
        sum_x2 = sum(xi ** 2 for xi in x)
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x ** 2)
        
        if slope > 0.01:
            return "improving"
        elif slope < -0.01:
            return "declining"
        else:
            return "stable"
    
    def calculateStability(self, rewards):
        """Calculate training stability."""
        if len(rewards) < 2:
            return "unstable"
        
        # Calculate coefficient of variation
        mean_reward = sum(rewards) / len(rewards)
        if mean_reward == 0:
            return "neutral"
        
        variance = sum((r - mean_reward) ** 2 for r in rewards) / len(rewards)
        std_dev = variance ** 0.5
        coefficient_of_variation = std_dev / mean_reward
        
        if coefficient_of_variation < 0.1:
            return "stable"
        elif coefficient_of_variation < 0.3:
            return "moderate"
        else:
            return "unstable"
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

# Configure with CMake (Release for best performance)
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

## Troubleshooting

### Common Issues

**"Training is taking too long"
- Reduce maxEpisodes or increase stopping criteria
- Use hyperparameter optimization to find better configurations
- Adjust learning rate or exploration rate

**"Callbacks not being called"
- Ensure callbacks are properly registered
- Check that training configuration enables the features callbacks expect
- Verify callback order and implementation

**"Saving training sessions fails"
- Ensure sufficient disk space
- Check file permissions
- Verify that required dependencies are installed
- Consider using compressed checkpoints for large brains

**"Advanced features not working"
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