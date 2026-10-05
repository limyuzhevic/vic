# Advanced NLM Examples and Comprehensive Documentation

This document contains comprehensive examples and documentation for the enhanced NLM (Neural Learning Machine) framework.

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Configuration Presets Examples](#configuration-presets-examples)
3. [Advanced Simulation Examples](#advanced-simulation-examples)
4. [Machine Learning Workflows](#machine-learning-workflows)
5. [Performance Monitoring Examples](#performance-monitoring-examples)
6. [Visualization and Debugging Examples](#visualization-and-debugging-examples)
7. [Real-World Use Cases](#real-world-use-cases)
8. [API Reference](#api-reference)

## Quick Start Guide

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Agent-Based Simulation

```python
import pynlm

# Create an agent system with advanced configuration
brain, world, agent = pynlm.createAgentSystem(
    config=pynlm.createExplorerConfig(),
    worldWidth=50,
    worldHeight=50,
    visionWidth=12,
    visionHeight=12
)

# Enable learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run a complete learning simulation
result = pynlm.runCompleteAgentSimulation(
    numSteps=5000,
    preset="explorer"
)

print(f"Learning simulation completed!")
print(f"Final firing rate: {result['firing_rate_mean']:.2f} Hz")
print(f"Total spikes: {result['total_spike_count']}")
print(f"Learning achieved: {result['learning_enabled']}")
```

## Configuration Presets Examples

### Default Configuration

The default configuration is designed for general-purpose neural simulation:

```python
config = pynlm.createDefaultConfig()
print(config.summary())
```

### Explorer Configuration (Curiosity-Driven Learning)

```python
# Perfect for agents that need to explore and discover new environments
config = pynlm.createExplorerConfig()
config.set("brain.neuron_count", 2000)
config.set("neuromod.curiosity.exploration_rate", 0.5f)
config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
```

### Storage Configuration (Memory-Optimized)

```python
# Optimized for tasks requiring strong memory formation and retention
config = pynlm.createStorageConfig()
config.set("brain.neuron_count", 1500)
config.set("plasticity.hebbian.learning_rate", 0.0002)
config.set("neuromod.novelty.sensitivity", 0.8f)
```

### Social Configuration (Social Learning)

```python
# Designed for multi-agent systems and social learning scenarios
config = pynlm.createSocialConfig()
config.set("brain.neuron_count", 2500)
config.set("plasticity.stdp.learning_rate", 0.002f)
config.set("neuromod.dopamine.scale", 1.5f)
```

### Minimal Configuration

```python
# Lightweight configuration for resource-constrained environments
config = pynlm.createMinimalConfig()
config.set("brain.neuron_count", 500)
config.set("simulation_timestep", 0.01)
```

### Debug Configuration

```python
# Configuration for debugging and development
config = pynlm.createDebugConfig()
config.set("brain.neuron_count", 100)
config.set("log_level", "debug")
config.set("enable_profiling", true)
config.set("enable_memory_tracking", true)
```

## Advanced Simulation Examples

### Pattern Recognition Task

```python
def runPatternRecognition(config=pynlm.createDefaultConfig()):
    """Simulate an agent learning to recognize visual patterns."""
    
    # Create agent system
    brain, world, agent = pynlm.createAgentSystem(config, 20, 20, 8, 8)
    
    # Enable relevant learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Define visual patterns
    patterns = [
        [[1, 0, 1], [0, 1, 0], [1, 0, 1]],  # Checkerboard
        [[1, 1, 1], [0, 1, 0], [0, 0, 0]],  # L-shape
        [[1, 1, 0], [1, 1, 0], [0, 0, 0]],  # Block
    ]
    
    # Run simulation
    for step in range(3000):
        # Update world with random changes
        if step % 100 == 0:
            # Introduce pattern changes
            for pattern in patterns:
                # Create pattern-based rewards
                reward = calculatePatternReward(percept, patterns)
                agent.applyRewardModulation(reward, 0.0)
        
        # Agent processes sensory input
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes information
        brain.step(step)
        
        # Agent learns from experience
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain, world, agent

def calculatePatternReward(percept, targetPatterns):
    """Calculate reward based on pattern recognition."""
    # Implementation details...
    return reward_value
```

### Reinforcement Learning Task

```python
def runReinforcementLearning(config=pynlm.createSocialConfig()):
    """Simulate a reinforcement learning agent."""
    
    brain, world, agent = pynlm.createAgentSystem(config, 30, 30, 10, 10)
    
    # Enable reinforcement learning
    agent.enableRewardModulation(True)
    
    # Track learning progress
    learning_curve = []
    rewards = []
    
    for episode in range(50):
        episode_reward = 0
        
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(action_result.reward, agent.getPredictionError())
            
            episode_reward += action_result.reward
            
            if action_result.success:
                learning_curve.append(brain.getAverageFiringRate())
                rewards.append(episode_reward)
                break
        
        print(f"Episode {episode + 1}: Reward = {episode_reward}, Success = {action_result.success}")
    
    return brain, world, agent, learning_curve, rewards
```

### Developmental Learning Task

```python
def runDevelopmentalLearning(config=pynlm.createExplorerConfig()):
    """Simulate brain development and maturation."""
    
    brain, world, agent = pynlm.createAgentSystem(config, 25, 25, 8, 8)
    
    # Enable developmental systems
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    agent.enableCuriosity(True)
    
    # Track developmental stages
    developmental_stages = []
    structural_changes = []
    
    for step in range(5000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Record developmental metrics
        developmental_stages.append(brain.getDevelopmentalStage())
        structural_changes.append(brain.getTotalSynapseCount())
        
        # Enable development at specific stages
        if step % 1000 == 0:
            agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 500 == 0:
            print(f"Step {step}: Stage {brain.getDevelopmentalStage()}, "
                  f"Neurons: {brain.getTotalNeuronCount()}, "
                  f"Synapses: {brain.getTotalSynapseCount()}")
    
    return brain, world, agent, developmental_stages, structural_changes
```

## Machine Learning Workflows

### 1. Supervised Learning Simulation

```python
def supervisedLearningWorkflow():
    """Simulate supervised learning with labeled patterns."""
    
    # Create training configuration
    config = pynlm.createStorageConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.hebbian.learning_rate", 0.0003)
    
    # Create agent system
    brain, world, agent = pynlm.createAgentSystem(config, 20, 20, 8, 8)
    
    # Training data
    training_patterns = [
        {"input": [[1,0,1],[0,1,0],[1,0,1]], "output": [1,0,1]},
        {"input": [[1,1,0],[0,1,0],[0,0,0]], "output": [1,1,0]},
        {"input": [[0,0,1],[1,1,1],[0,0,0]], "output": [0,0,1]},
    ]
    
    # Training loop
    for epoch in range(100):
        epoch_error = 0
        
        for pattern in training_patterns:
            # Present input pattern
            percept = createSensoryInput(pattern["input"])
            agent.processSensoryInput(percept)
            
            # Get brain output
            action = agent.decodeMotorCommand()
            
            # Calculate error
            target = pattern["output"]
            error = calculateError(action, target)
            epoch_error += error
            
            # Apply reward based on correctness
            reward = 1.0 if error < 0.1 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        print(f"Epoch {epoch + 1}: Average error = {epoch_error / len(training_patterns):.4f}")
        
        if epoch_error / len(training_patterns) < 0.01:
            print("Converged!")
            break
    
    return brain, world, agent
```

### 2. Unsupervised Learning Simulation

```python
def unsupervisedLearningWorkflow():
    """Simulate unsupervised learning with clustering."""
    
    config = pynlm.createExplorerConfig()
    config.set("brain.neuron_count", 1500)
    config.set("neuromod.curiosity.exploration_rate", 0.3f)
    
    brain, world, agent = pynlm.createAgentSystem(config, 30, 30, 10, 10)
    
    # Enable unsupervised learning systems
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    
    # Track cluster formation
    clusters = []
    novelty_scores = []
    
    for step in range(4000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Calculate novelty
        novelty = agent.getNoveltyLevel()
        novelty_scores.append(novelty)
        
        # Form new clusters based on novelty
        if novelty > 0.7:
            cluster = createNewCluster(percept)
            clusters.append(cluster)
        
        # Print progress
        if step % 500 == 0:
            print(f"Step {step}: Novelty = {novelty:.3f}, Clusters = {len(clusters)}")
    
    return brain, world, agent, clusters, novelty_scores
```

### 3. Reinforcement Learning with Function Approximation

```python
def reinforcementLearningWithFunctionApprox(config=pynlm.createSocialConfig()):
    """Simulate RL with neural network function approximation."""
    
    brain, world, agent = pynlm.createAgentSystem(config, 40, 40, 12, 12)
    
    # Enable RL systems
    agent.enableRewardModulation(True)
    agent.enablePredictionError(True)
    
    # State-action value tracking
    q_table = {}
    exploration_rate = 0.1
    
    for episode in range(100):
        state = getInitialState(world)
        done = False
        episode_reward = 0
        
        while not done:
            # Epsilon-greedy action selection
            if pynlm.random() < exploration_rate:
                action = pynlm.randomAction()
            else:
                action = agent.decodeMotorCommand()
                # Estimate Q-value using brain output
                q_value = estimateQValue(brain, action)
                q_table[action] = q_value
            
            # Take action
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Observe next state
            next_state = getStateFromWorld(world)
            
            # Store transition
            q_table[(state, action)] = (
                (1 - learning_rate) * q_table.get((state, action), 0) +
                learning_rate * (action_result.reward + discount_factor * maxFutureQ(q_table, next_state))
            )
            
            # Update agent with reward prediction error
            agent.applyRewardModulation(action_result.reward, agent.getPredictionError())
            
            episode_reward += action_result.reward
            state = next_state
            
            if action_result.success or step > 200:
                done = True
        
        # Decay exploration rate
        exploration_rate *= 0.995
        
        print(f"Episode {episode + 1}: Reward = {episode_reward:.2f}, Exploration = {exploration_rate:.3f}")
    
    return brain, world, agent, q_table
```

## Performance Monitoring Examples

### Basic Performance Monitoring

```python
def basicPerformanceMonitoring():
    """Monitor basic performance metrics during simulation."""
    
    # Start monitoring
    pynlm.startPerformanceMonitoring(True)
    
    # Run simulation
    brain, world, agent = pynlm.createAgentSystem(preset="default")
    
    start_time = pynlm.getSystemTime()
    
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Record metrics periodically
        if step % 100 == 0:
            pynlm.recordSimulationStep(
                brain.getFiringNeuronCount(),
                brain.getTotalSpikeCount(),
                brain.getAverageFiringRate()
            )
    
    end_time = pynlm.getSystemTime()
    
    # Get performance statistics
    stats = pynlm.getMonitoringStats()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Average firing rate: {stats.average_firing_rate:.2f} Hz")
    print(f"Peak memory usage: {stats.peak_memory_usage} bytes")
    print(f"Convergence score: {stats.convergence_score:.3f}")
    
    return brain, world, agent, stats
```

### Advanced Performance Profiling

```python
def advancedPerformanceProfiling():
    """Advanced performance profiling with detailed metrics."""
    
    # Start comprehensive profiling
    pynlm.enableAllProfiling(True)
    pynlm.startMemoryMonitoring(True)
    
    config = pynlm.createDebugConfig()
    brain, world, agent = pynlm.createAgentSystem(config, 25, 25, 8, 8)
    
    # Profile different operations
    pynlm.startProfiling("brain_step")
    pynlm.startProfiling("agent_processing")
    pynlm.startProfiling("world_update")
    
    for step in range(2000):
        # Profile world update
        pynlm.startProfiling("world_update")
        world.update(0.1)
        pynlm.stopProfiling("world_update")
        
        # Profile agent processing
        pynlm.startProfiling("agent_processing")
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        pynlm.stopProfiling("agent_processing")
        
        # Profile brain step
        pynlm.startProfiling("brain_step")
        brain.step(step)
        pynlm.stopProfiling("brain_step")
        
        # Record memory usage
        pynlm.recordMemoryUsage(pynlm.getCurrentMemoryUsage())
        
        # Take benchmark snapshots
        if step % 500 == 0:
            pynlm.takeBenchmarkSnapshot(f"step_{step}")
    
    # Get profiling results
    profiles = pynlm.getProfileData()
    memory_history = pynlm.getMemoryUsageHistory()
    
    print("Performance Profile Summary:")
    for profile in profiles:
        print(f"  {profile['operation']}: {profile['total_time']} ns total, "
              f"{profile['call_count']} calls")
    
    print(f"Memory usage history: {len(memory_history)} samples")
    print(f"Peak memory: {pynlm.getPeakMemoryUsage()} bytes")
    
    return brain, world, agent, profiles, memory_history
```

## Visualization and Debugging Examples

### Debug Visualization

```python
def debugVisualizationExample():
    """Create debug visualizations and snapshots."""
    
    # Enable debug logging
    pynlm.setGlobalLogLevel("debug")
    
    # Create debug configuration
    config = pynlm.createDebugConfig()
    config.set("enable_visualization", True)
    
    brain, world, agent = pynlm.createAgentSystem(config, 20, 20, 8, 8)
    
    # Take debug snapshots
    pynlm.takeDebugSnapshot("initialization")
    
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        if step % 100 == 0:
            # Take snapshot at each checkpoint
            pynlm.takeDebugSnapshot(f"step_{step}")
            
            # Get current debug state
            debug_data = pynlm.getDebugSnapshot(step)
            visualizeDebugData(debug_data)
    
    # Get all debug snapshots
    all_snapshots = pynlm.getDebugSnapshots()
    
    # Analyze changes over time
    analyzeSnapshotChanges(all_snapshots)
    
    return brain, world, agent, all_snapshots
```

### Brain State Visualization

```python
def brainStateVisualization(brain):
    """Visualize brain state and activity."""
    
    # Get brain statistics
    stats = pynlm.getBrainStatistics(brain)
    
    # Create visualization data
    visualization_data = {
        'neuron_distribution': createNeuronDistribution(brain),
        'firing_patterns': getFiringPatterns(brain),
        'synapse_topology': getSynapseTopology(brain),
        'activity_heatmap': createActivityHeatmap(brain),
        'developmental_trajectory': getDevelopmentalTrajectory(brain),
    }
    
    # Generate visualizations
    plot_neuron_distribution(visualization_data['neuron_distribution'])
    plot_firing_patterns(visualization_data['firing_patterns'])
    plot_synapse_topology(visualization_data['synapse_topology'])
    plot_activity_heatmap(visualization_data['activity_heatmap'])
    plot_developmental_trajectory(visualization_data['developmental_trajectory'])
    
    return visualization_data
```

## Real-World Use Cases

### 1. Robotic Control

```python
def roboticControlExample():
    """Simulate a robot learning to control its environment."""
    
    # Create robot simulation configuration
    config = pynlm.createExplorerConfig()
    config.set("brain.neuron_count", 1500)
    config.set("world.width", 50)
    config.set("world.height", 50)
    config.set("robot.sensors", 12)
    config.set("robot.actuators", 6)
    
    # Create robot agent system
    brain, world, robot = pynlm.createAgentSystem(config)
    
    # Enable robotic control systems
    robot.enableMovementControl(True)
    robot.enableObstacleAvoidance(True)
    robot.enableGoalSeeking(True)
    
    # Define goal position
    goal_position = (25.0, 25.0)
    
    # Run control simulation
    for step in range(3000):
        # Get sensor readings
        sensors = robot.getSensorReadings()
        percept = createSensoryInput(sensors)
        
        # Process sensory input
        robot.processSensoryInput(percept)
        
        # Brain decides action
        brain.step(step)
        action = robot.decodeMotorCommand()
        
        # Execute action
        robot.applyAction(action)
        
        # Check if goal reached
        current_position = robot.getPosition()
        distance_to_goal = calculateDistance(current_position, goal_position)
        
        # Apply reward
        reward = calculateControlReward(action, distance_to_goal, goal_position)
        robot.applyRewardModulation(reward, 0.0)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Position {current_position}, "
                  f"Distance to goal {distance_to_goal:.2f}, Reward {reward:.2f}")
        
        if distance_to_goal < 2.0:
            print(f"Goal reached in {step} steps!")
            break
    
    return brain, world, robot, goal_position
```

### 2. Financial Market Simulation

```python
def financialMarketSimulation():
    """Simulate financial market dynamics with neural agents."""
    
    # Create market simulation configuration
    config = pynlm.createSocialConfig()
    config.set("brain.neuron_count", 2000)
    config.set("market.participants", 50)
    config.set("market.assets", 10)
    config.set("trading.frequency", "high")
    
    # Create market environment
    brain, market, trader = pynlm.createAgentSystem(config, 100, 100, 20, 20)
    
    # Enable trading systems
    trader.enableTechnicalAnalysis(True)
    trader.enableSentimentAnalysis(True)
    trader.enableRiskManagement(True)
    
    # Market data
    market_data = initializeMarketData()
    price_history = []
    trader_performance = []
    
    # Run market simulation
    for step in range(5000):
        # Update market conditions
        market.update(step, market_data)
        
        # Traders make decisions based on market data
        for trader in market.getTraders():
            # Get market percept
            percept = createMarketPercept(market_data, trader)
            trader.processSensoryInput(percept)
            
            # Make trading decision
            brain.step(step)
            action = trader.decodeMotorCommand()
            
            # Execute trade
            trade_result = trader.executeTrade(action, market_data)
            
            # Record performance
            trader_performance.append({
                'step': step,
                'trader_id': trader.getId(),
                'action': action,
                'profit': trade_result.profit,
                'risk': trade_result.risk,
            })
        
        # Update price history
        price_history.append(market_data.current_price)
        
        # Print market status
        if step % 500 == 0:
            print(f"Step {step}: Price = ${market_data.current_price:.2f}, "
                  f"Volume = {market_data.volume}, "
                  f"Volatility = {market_data.volatility:.3f}")
    
    return brain, market, trader, price_history, trader_performance
```

### 3. Educational Tutoring System

```python
def educationalTutoringExample():
    """Simulate an intelligent tutoring system."""
    
    # Create tutoring configuration
    config = pynlm.createStorageConfig()
    config.set("brain.neuron_count", 1200)
    config.set("student.profile", "advanced")
    config.set("teaching.method", "adaptive")
    config.set("difficulty.level", "medium")
    
    # Create student agent system
    brain, world, student = pynlm.createAgentSystem(config, 30, 30, 10, 10)
    
    # Enable educational systems
    student.enableAdaptiveLearning(True)
    student.enableProgressTracking(True)
    student.enableFeedbackGeneration(True)
    student.enableMotivationManagement(True)
    
    # Course content
    course_modules = [
        {"topic": "Algebra", "difficulty": "easy", "completed": False},
        {"topic": "Geometry", "difficulty": "medium", "completed": False},
        {"topic": "Calculus", "difficulty": "hard", "completed": False},
    ]
    
    # Run tutoring simulation
    student_progress = []
    learning_metrics = []
    
    for step in range(4000):
        # Present learning material
        current_module = getCurrentModule(course_modules)
        material = createLearningMaterial(current_module)
        
        # Student processes material
        percept = createEducationalPercept(material, student)
        student.processSensoryInput(percept)
        
        # Brain processes and responds
        brain.step(step)
        response = student.decodeMotorCommand()
        
        # Execute educational action
        action_result = student.applyAction(response, material)
        
        # Record progress
        student_progress.append({
            'step': step,
            'module': current_module,
            'score': action_result.score,
            'time_spent': action_result.time_spent,
            'engagement': action_result.engagement,
        })
        
        # Update learning metrics
        learning_metrics.append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'novelty': student.getNoveltyLevel(),
            'curiosity': student.getCuriosityLevel(),
        })
        
        # Adapt difficulty based on performance
        adaptDifficulty(student, action_result.score, current_module)
        
        # Check if module completed
        if isModuleCompleted(student, current_module):
            markModuleCompleted(current_module)
            print(f"Student completed {current_module['topic']} at step {step}")
    
    return brain, world, student, student_progress, learning_metrics
```

## API Reference

### Core Classes

#### Brain
```python
class Brain:
    def __init__(self, config):
        # Create brain with configuration
        
    def initialize(self):
        # Initialize brain
        
    def step(self, step, time=None):
        # Perform simulation step
        
    def processSensoryInput(self, input):
        # Process sensory input
        
    def decodeMotorCommand(self):
        # Decode motor command
        
    def getStatistics(self):
        # Get brain statistics
        
    def getTotalNeuronCount(self):
        # Get total neuron count
        
    def getTotalSynapseCount(self):
        # Get total synapse count
        
    def getFiringNeuronCount(self):
        # Get firing neuron count
        
    def getAverageFiringRate(self):
        # Get average firing rate
```

#### Config
```python
class Config:
    def __init__(self):
        # Create default configuration
        
    def loadFromFile(self, filepath):
        # Load configuration from file
        
    def loadFromArgs(self, argc, argv):
        # Load configuration from command line
        
    def saveToFile(self, filepath):
        # Save configuration to file
        
    def has(self, key):
        # Check if configuration key exists
        
    def set(self, key, value, source=ConfigSource.Runtime):
        # Set configuration value
        
    def get(self, key):
        # Get configuration value
        
    def getOr(self, key, default_value):
        # Get configuration value with default
```

#### AgentBrain
```python
class AgentBrain:
    def __init__(self, brain):
        # Create agent brain interface
        
    def initialize(self, world):
        # Initialize with world
        
    def processSensoryInput(self, percept):
        # Process sensory input
        
    def decodeMotorCommand(self):
        # Decode motor command
        
    def applyRewardModulation(self, reward, predicted_reward):
        # Apply reward modulation
        
    def updateDevelopment(self, timestep):
        # Update development system
        
    def enableRewardModulation(self, enable):
        # Enable/disable reward modulation
        
    def enableStructuralPlasticity(self, enable):
        # Enable/disable structural plasticity
        
    def enableDevelopment(self, enable):
        # Enable/disable development
        
    def enableCuriosity(self, enable):
        # Enable/disable curiosity
```

#### SimpleWorld
```python
class SimpleWorld:
    def __init__(self):
        # Create world
        
    def configure(self, width, height, vision_width, vision_height):
        # Configure world dimensions
        
    def reset(self):
        # Reset world state
        
    def setAgentStart(self, x, y):
        # Set agent starting position
        
    def update(self, timestep):
        # Update world state
        
    def applyMotorCommand(self, cmd, time):
        # Apply motor command
        
    def getSensoryPercept(self):
        # Get sensory percept
        
    def getAgentBody(self):
        # Get agent body state
```

### Factory Functions

#### Configuration Presets
```python
def createDefaultConfig():
    """Create default configuration"""
    
def createExplorerConfig():
    """Create explorer configuration"""
    
def createStorageConfig():
    """Create storage configuration"""
    
def createSocialConfig():
    """Create social configuration"""
    
def createMinimalConfig():
    """Create minimal configuration"""
    
def createDebugConfig():
    """Create debug configuration"""
```

#### Agent System Creation
```python
def createAgentSystem(config=None, worldWidth=20, worldHeight=20, 
                      visionWidth=8, visionHeight=8):
    """Create complete agent system"""
    
def createAgentBrain(brain):
    """Create agent brain interface"""
    
def createBrain(config=None):
    """Create brain with configuration"""
```

#### Simulation Utilities
```python
def runSimulationSteps(brain, world, agent, num_steps, verbose=False):
    """Run complete simulation"""
    
def getBrainStatistics(brain):
    """Get brain statistics"""
    
def runCompleteAgentSimulation(num_steps=1000, preset="default"):
    """Run complete agent simulation with learning"""
```

### Monitoring and Debugging

#### Performance Monitoring
```python
def startPerformanceMonitoring(enable=True):
    """Start performance monitoring"""
    
def recordSimulationStep(neurons_firing, total_spikes, average_firing_rate):
    """Record simulation step metrics"""
    
def getMonitoringStats():
    """Get monitoring statistics"""
    
def resetMonitoringStats():
    """Reset monitoring statistics"""
```

#### Debugging
```python
def setGlobalLogLevel(level):
    """Set global log level"""
    
def takeDebugSnapshot(stage):
    """Take debug snapshot"""
    
def getDebugSnapshots():
    """Get debug snapshots"""
    
def clearDebugSnapshots():
    """Clear debug snapshots"""
```

#### Profiling
```python
def startProfiling(operation):
    """Start profiling"""
    
def stopProfiling(operation):
    """Stop profiling"""
    
def getProfileData():
    """Get profile data"""
    
def clearProfiles():
    """Clear profiles"""
```

### Constants and Enums

#### Log Levels
```python
LogLevel.Debug
LogLevel.Info
LogLevel.Warning
LogLevel.Error
LogLevel.Critical
```

#### Action Types
```python
ActionType.MoveForward
ActionType.MoveBackward
ActionType.MoveLeft
ActionType.MoveRight
ActionType.TurnLeft
ActionType.TurnRight
ActionType.Look
ActionType.LookUp
ActionType.LookDown
ActionType.Interact
ActionType.Eat
ActionType.Drink
ActionType.Rest
ActionType.Wait
ActionType.Custom
```

#### Neuron Types
```python
NeuronType.Excitatory
NeuronType.Inhibitory
NeuronType.Modulatory
NeuronType.Sensory
NeuronType.Motor
NeuronType.Internal
```

#### Synapse Types
```python
SynapseType.Excitatory
SynapseType.Inhibitory
SynapseType.Modulatory
SynapseType.Electrical
SynapseType.GapJunction
```

## Best Practices

### 1. Configuration Management
- Use configuration presets for different use cases
- Validate configurations before use
- Implement configuration templates for consistency
- Use environment-specific configurations

### 2. Simulation Design
- Start with small, controlled simulations
- Gradually increase complexity
- Monitor performance at each scale
- Implement checkpoint/restart mechanisms

### 3. Learning System Design
- Enable appropriate learning subsystems based on task
- Balance exploration and exploitation
- Monitor learning progress continuously
- Implement adaptive learning rates

### 4. Performance Optimization
- Profile your simulation regularly
- Monitor memory usage
- Optimize critical paths
- Use appropriate configuration presets

### 5. Debugging and Monitoring
- Enable comprehensive logging for development
- Use debug snapshots for analysis
- Monitor performance metrics continuously
- Implement automated testing

## Getting Help

If you encounter issues or need assistance:

1. **Documentation**: Check the NLM documentation for detailed information
2. **Examples**: Run the provided examples to understand the API
3. **Community**: Join the NLM community for support
4. **Issues**: Report bugs and feature requests on GitHub
5. **Contact**: Contact the NLM team for technical support

## Future Enhancements

The NLM framework is continuously evolving. Future enhancements may include:

1. **Multi-agent systems**: Support for complex multi-agent interactions
2. **External integration**: Integration with external systems and APIs
3. **Advanced visualization**: Enhanced visualization and analysis tools
4. **Distributed computing**: Support for distributed simulations
5. **Machine learning pipelines**: Integration with ML frameworks
6. **Real-time simulation**: Real-time simulation capabilities
7. **Hardware acceleration**: GPU and specialized hardware support

## License

This work is licensed under the MIT License. See the LICENSE file for details.

---

*Document generated by NLM Documentation System*  
*Version: 1.0.0*  
*Last Updated: 2024-01-15*