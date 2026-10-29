# Advanced NLM Tutorials

## Overview

This document provides comprehensive tutorials for advanced NLM (熙然) usage, covering multi-agent simulations, learning experiments, memory systems, neuromodulation, and custom brain architectures.

## 1. Multi-Agent Simulations with World Interaction

### 1.1 Basic Multi-Agent Setup

```python
import pynlm as nlm
import json

# Create simulation environment
world = nlm.SimpleWorld.create(200, 200, 20, 20)
world.reset()

# Create multiple agents
agents = []
for i in range(3):
    brain = nlm.Brain.createDefault()
    agent = nlm.AgentBrain.createWithBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agents.append(agent)

# Run simulation with all agents
steps = 1000
timestep = 0.01

for step in range(steps):
    # Each agent perceives the world
    percepts = [world.getSensoryPercept() for _ in agents]
    
    # Agents process sensory input
    for agent, percept in zip(agents, percepts):
        agent.processSensoryInput(percept)
    
    # All agents share the same world state
    world.update(timestep)
    
    # Each agent produces an action
    actions = [agent.decodeMotorCommand() for agent in agents]
    
    # Apply all actions to world
    for action in actions:
        world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {sum(a.get('action', 0) for a in actions)} total actions")

print("Multi-agent simulation completed!")
```

### 1.2 Competitive Multi-Agent Scenario

```python
import pynlm as nlm

# Create competitive environment
world = nlm.SimpleWorld.create(150, 150, 15, 15)
world.reset()

# Create competing agents with different behaviors
class CompetingAgent:
    def __init__(self, id, strategy):
        self.id = id
        self.strategy = strategy
        self.brain = nlm.Brain.createDefault()
        self.agent = nlm.AgentBrain.createWithBrain(self.brain)
        self.agent.initialize(world)
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
    
    def processStep(self, percept):
        self.agent.processSensoryInput(percept)
        return self.agent.decodeMotorCommand()

# Create different agent strategies
agents = [
    CompetingAgent(0, "exploration"),
    CompetingAgent(1, "exploitation"),
    CompetingAgent(2, "balanced")
]

# Competition simulation
episodes = 50
steps_per_episode = 100

for episode in range(episodes):
    world.reset()
    
    for step in range(steps_per_episode):
        percept = world.getSensoryPercept()
        
        # Each agent acts based on strategy
        actions = []
        for agent in agents:
            action = agent.processStep(percept)
            action['agent_id'] = agent.id
            action['strategy'] = agent.strategy
            actions.append(action)
        
        # Apply all actions (competitive for same resources)
        for action in actions:
            world.applyMotorCommand(action, world.getSimulationTime())
        
        world.update(0.01)
    
    # Evaluate episode results
    episode_rewards = []
    for agent in agents:
        # Get agent's performance metrics
        rewards = agent.brain.getRewards() if hasattr(agent.brain, 'getRewards') else []
        avg_reward = sum(rewards) / len(rewards) if rewards else 0
        episode_rewards.append({
            'agent_id': agent.id,
            'average_reward': avg_reward,
            'strategy': agent.strategy
        })
    
    print(f"Episode {episode + 1} Results:")
    for result in sorted(episode_rewards, key=lambda x: x['average_reward'], reverse=True):
        print(f"  Agent {result['agent_id']} ({result['strategy']}): {result['average_reward']:.3f}")

print("Competitive multi-agent tournament completed!")
```

## 2. Learning and Plasticity Experiments

### 2.1 Hebbian vs STDP Learning Comparison

```python
import pynlm as nlm

# Create experiment configuration
config = nlm.Config.createFromJSON('''{
    "brain.neuron_count": 500,
    "brain.region_count": 1,
    "plasticity.stdp_enabled": true,
    "plasticity.hebbian_enabled": true
}''')

brain = nlm.Brain.createDefault(config)
brain.initialize()
world = nlm.SimpleWorld.create(100, 100)
agent = nlm.AgentBrain.createWithBrain(brain)
agent.initialize(world)

# Experiment parameters
trials = 200
stimulus_duration = 20
inter_trial_interval = 50

# Trial structure: A -> B -> Test
pattern_a = "stimulus_A"
pattern_b = "stimulus_B"
test_pattern = "test_pattern"

for trial in range(trials):
    # Present pattern A
    world.reset()
    for step in range(stimulus_duration):
        percept = {"pattern": pattern_a, "time": step}
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        world.update(0.01)
    
    # Present pattern B (contiguously)
    for step in range(stimulus_duration):
        percept = {"pattern": pattern_b, "time": step + stimulus_duration}
        agent.processSensoryInput(percept)
        brain.step(step + stimulus_duration)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        world.update(0.01)
    
    # Test pattern (should evoke response if learning occurred)
    test_rewards = []
    for step in range(stimulus_duration):
        percept = {"pattern": test_pattern, "time": step + 2 * stimulus_duration}
        agent.processSensoryInput(percept)
        brain.step(step + 2 * stimulus_duration)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward for this step
        if hasattr(agent, 'getCurrentReward'):
            test_rewards.append(agent.getCurrentReward())
        
        world.update(0.01)
    
    # Apply inter-trial interval
    for step in range(inter_trial_interval):
        world.update(0.01)
    
    # Record trial statistics
    if trial % 20 == 0:
        avg_reward = sum(test_rewards) / len(test_rewards) if test_rewards else 0
        print(f"Trial {trial + 1}: Average test reward = {avg_reward:.3f}")

print("Hebbian vs STDP comparison experiment completed!")
```

### 2.2 Reward-Modulated Plasticity

```python
import pynlm as nlm

# Create brain with reward-modulated plasticity
config = nlm.Config.createFromJSON('''{
    "brain.neuron_count": 800,
    "brain.region_count": 2,
    "plasticity.reward_modulated": true,
    "plasticity.eligibility_trace_decay": 0.9,
    "neuromodulation.dopamine_sensitivity": 0.5
}''')

brain = nlm.Brain.createDefault(config)
brain.initialize()
world = nlm.SimpleWorld.create(120, 120)
agent = nlm.AgentBrain.createWithBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Create reward sequence (sparse rewards to encourage exploration)
reward_sequence = []
for i in range(1000):
    if i % 100 == 0:
        reward = 1.0  # Large reward at intervals
    elif i % 50 == 0:
        reward = 0.5  # Medium reward
    elif i % 25 == 0:
        reward = 0.1  # Small reward
    else:
        reward = -0.1  # Small penalty
    
    reward_sequence.append(reward)

# Run reward-modulated learning experiment
learning_curves = {'reward': [], 'error': [], 'confidence': []}

for step in range(1000):
    # Get current reward and apply to agent
    current_reward = reward_sequence[step]
    agent.applyRewardModulation(current_reward)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update world
    world.update(0.01)
    
    # Record learning metrics
    if step % 50 == 0:
        learning_curves['reward'].append(current_reward)
        learning_curves['error'].append(agent.getPredictionError() if hasattr(agent, 'getPredictionError') else 0)
        learning_curves['confidence'].append(agent.getConfidence() if hasattr(agent, 'getConfidence') else 0.5)
        
        print(f"Step {step}: Reward = {current_reward:.2f}, "
              f"Error = {learning_curves['error'][-1]:.3f}, "
              f"Confidence = {learning_curves['confidence'][-1]:.3f}")

# Analyze learning curves
final_reward = learning_curves['reward'][-1]
avg_error = sum(learning_curves['error']) / len(learning_curves['error'])
final_confidence = learning_curves['confidence'][-1]

print(f"\nLearning Analysis:")
print(f"Final reward: {final_reward:.3f}")
print(f"Average prediction error: {avg_error:.3f}")
print(f"Final confidence: {final_confidence:.3f}")

# Determine if learning occurred
if final_confidence > 0.7 and avg_error < 0.2:
    print("✓ Learning successful: High confidence, low error")
else:
    print("✗ Learning incomplete: Low confidence or high error")
```

## 3. Memory System Usage and Integration

### 3.1 Episodic Memory Replay

```python
import pynlm as nlm

# Create brain with episodic memory
config = nlm.Config.createFromJSON('''{
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "memory.episodic.enabled": true,
    "memory.episodic.capture_interval": 10,
    "memory.episodic.replay_interval": 100,
    "memory.episodic.consolidation_interval": 1000,
    "memory.working.enabled": true,
    "memory.associative.enabled": true
}''')

brain = nlm.Brain.createDefault(config)
brain.initialize()
world = nlm.SimpleWorld.create(150, 150)
agent = nlm.AgentBrain.createWithBrain(brain)
agent.initialize(world)

# Memory experiment configuration
experiment_duration = 2000
replay_cycles = []

# Run main experiment
for step in range(experiment_duration):
    # Generate and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    world.update(0.01)
    
    # Record memory events
    if step % 100 == 0:
        # Get memory statistics
        if hasattr(brain, 'getMemoryStats'):
            stats = brain.getMemoryStats()
            replay_cycles.append({
                'step': step,
                'episodes_stored': stats.get('episodes_stored', 0),
                'working_memory_size': stats.get('working_memory_size', 0),
                'replay_events': stats.get('replay_events', 0)
            })
            
            print(f"Step {step}:")
            print(f"  Episodes stored: {stats.get('episodes_stored', 0)}")
            print(f"  Working memory entries: {stats.get('working_memory_size', 0)}")
            print(f"  Replay events: {stats.get('replay_events', 0)}")

# Analyze memory dynamics
print("\nMemory System Analysis:")
if replay_cycles:
    final_storage = replay_cycles[-1]['episodes_stored']
    avg_working_memory = sum(c['working_memory_size'] for c in replay_cycles) / len(replay_cycles)
    replay_frequency = replay_cycles[-1]['replay_events'] / (experiment_duration / 100)
    
    print(f"Total episodes stored: {final_storage}")
    print(f"Average working memory size: {avg_working_memory:.1f}")
    print(f"Replay frequency (events per 100 steps): {replay_frequency:.2f}")
    
    # Check memory efficiency
    if final_storage > 50 and avg_working_memory > 10:
        print("✓ Memory system operating efficiently")
    else:
        print("✗ Memory system may need tuning")
else:
    print("No memory events recorded - check memory system implementation")
```

### 3.2 Working Memory Capacity Limits

```python
import pynlm as nlm
import numpy as np

# Create brain with working memory
config = nlm.Config.createFromJSON('''{
    "brain.neuron_count": 1200,
    "brain.region_count": 2,
    "memory.working.capacity": 50,
    "memory.working.decay_rate": 0.95,
    "memory.working.reinforcement_threshold": 0.8
}''')

brain = nlm.Brain.createDefault(config)
brain.initialize()
world = nlm.SimpleWorld.create(100, 100)
agent = nlm.AgentBrain.createWithBrain(brain)
agent.initialize(world)

# Working memory capacity experiment
stimulus_patterns = 20
max_patterns = 50
training_trials = 1000

# Create distinct stimulus patterns
patterns = {}
for i in range(stimulus_patterns):
    patterns[f"pattern_{i}"] = {
        'features': np.random.rand(20),  # 20-dimensional pattern
        'reward': 1.0 if i < 10 else 0.0  # First 10 get reward
    }

# Training phase
memory_retention = []
for trial in range(training_trials):
    # Select random pattern
    pattern_id = f"pattern_{np.random.randint(0, stimulus_patterns)}"
    pattern_data = patterns[pattern_id]
    
    # Present pattern and attempt to store in working memory
    for step in range(20):  # 20-step presentation
        percept = {
            'pattern_id': pattern_id,
            'features': pattern_data['features'],
            'step': step,
            'reward': pattern_data['reward'] if step == 19 else 0.0
        }
        
        agent.processSensoryInput(percept)
        brain.step(trial * 20 + step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        world.update(0.01)
        
        # Store pattern in working memory if high confidence
        if hasattr(agent, 'storeWorkingMemory'):
            agent.storeWorkingMemory(pattern_id, pattern_data['features'])
    
    # Test retention (after delay)
    if trial % 100 == 99:
        # Delay period
        for delay_step in range(50):
            world.update(0.01)
        
        # Test recall
        recall_scores = []
        for test_pattern_id in list(patterns.keys())[:10]:  # Test top 10 patterns
            if test_pattern_id != pattern_id:  # Don't test same pattern
                test_percept = {
                    'pattern_id': test_pattern_id,
                    'features': patterns[test_pattern_id]['features']
                }
                
                agent.processSensoryInput(test_percept)
                brain.step(trial * 20 + 20 + delay_step)
                
                # Get recall confidence
                if hasattr(agent, 'getWorkingMemoryConfidence'):
                    confidence = agent.getWorkingMemoryConfidence(pattern_id)
                    recall_scores.append(confidence)
        
        if recall_scores:
            avg_recall = np.mean(recall_scores)
            memory_retention.append(avg_recall)
            print(f"Trial {trial + 1}: Average recall confidence = {avg_recall:.3f}")

# Analyze capacity limits
print("\nWorking Memory Capacity Analysis:")
if memory_retention:
    avg_retention = np.mean(memory_retention)
    retention_decay = np.std(memory_retention)
    
    print(f"Average recall confidence: {avg_retention:.3f}")
    print(f"Recall variability: {retention_decay:.3f}")
    
    # Capacity assessment
    if avg_retention > 0.7 and retention_decay < 0.2:
        print(f"✓ Working memory capacity of {max_patterns} items is sufficient")
    elif avg_retention > 0.5:
        print(f"⚠ Working memory capacity of {max_patterns} items may be near limit")
    else:
        print(f"✗ Working memory capacity of {max_patterns} items is insufficient")
else:
    print("No retention data collected - check working memory implementation")
```

## 4. Neuromodulation Experiments

### 4.1 Dopamine and Curiosity Integration

```python
import pynlm as nlm

# Create brain with neuromodulation systems
config = nlm.Config.createFromJSON('''{
    "brain.neuron_count": 800,
    "brain.region_count": 2,
    "neuromodulation.dopamine.enabled": true,
    "neuromodulation.curiosity.enabled": true,
    "neuromodulation.novelty.enabled": true,
    "neuromodulation.prediction_error.enabled": true,
    "neuromodulation.dopamine.sensitivity": 0.3,
    "neuromodulation.curiosity.decay_rate": 0.95
}''')

brain = nlm.Brain.createDefault(config)
brain.initialize()
world = nlm.SimpleWorld.create(120, 120)
agent = nlm.AgentBrain.createWithBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Neuromodulation experiment
experiment_duration = 1500
novelty_history = []
curiosity_history = []
dopamine_history = []
action_history = []

for step in range(experiment_duration):
    # Get current world state
    percept = world.getSensoryPercept()
    
    # Process sensory input (computes novelty)
    agent.processSensoryInput(percept)
    
    # Get neuromodulation signals
    if hasattr(agent, 'getNovelty'):
        novelty = agent.getNovelty()
        novelty_history.append(novelty)
    
    if hasattr(agent, 'getCuriosity'):
        curiosity = agent.getCuriosity()
        curiosity_history.append(curiosity)
    
    if hasattr(agent, 'getDopamine'):
        dopamine = agent.getDopamine()
        dopamine_history.append(dopamine)
    
    # Brain computation
    brain.step(step)
    
    # Get action (modulated by curiosity)
    action = agent.decodeMotorCommand()
    action_history.append(action)
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    world.update(0.01)
    
    # Record metrics
    if step % 100 == 0:
        avg_novelty = np.mean(novelty_history[-10:]) if len(novelty_history) >= 10 else 0
        avg_curiosity = np.mean(curiosity_history[-10:]) if len(curiosity_history) >= 10 else 0
        avg_dopamine = np.mean(dopamine_history[-10:]) if len(dopamine_history) >= 10 else 0
        
        exploration_rate = sum(1 for a in action_history[-10:] if a.get('exploration', False)) / 10
        
        print(f"Step {step}:")
        print(f"  Novelty: {avg_novelty:.3f}, Curiosity: {avg_curiosity:.3f}, Dopamine: {avg_dopamine:.3f}")
        print(f"  Exploration rate: {exploration_rate:.2f}")

# Analyze neuromodulation dynamics
print("\nNeuromodulation System Analysis:")
if dopamine_history and curiosity_history:
    avg_dopamine = np.mean(dopamine_history)
    avg_curiosity = np.mean(curiosity_history)
    dopamine_curiosity_corr = np.corrcoef(dopamine_history, curiosity_history)[0, 1]
    
    print(f"Average dopamine level: {avg_dopamine:.3f}")
    print(f"Average curiosity level: {avg_curiosity:.3f}")
    print(f"Dopamine-curiosity correlation: {dopamine_curiosity_corr:.3f}")
    
    # Check system balance
    if 0.1 < avg_dopamine < 0.5 and avg_curiosity > 0.3:
        print("✓ Neuromodulation systems in balanced state")
    elif avg_dopamine > 0.7:
        print("⚠ High dopamine may indicate over-excitation")
    elif avg_curiosity < 0.1:
        print("⚠ Low curiosity may indicate under-exploration")
    else:
        print("⚠ Neuromodulation systems may need adjustment")
```

### 4.2 Prediction Error and Learning

```python
import pynlm as nlm
import numpy as np

# Create brain with prediction system
config = nlm.Config.createFromJSON('''{
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "prediction.enabled": true,
    "prediction.horizon": 3,
    "prediction.error_sensitivity": 0.5,
    "neuromodulation.prediction_error.enabled": true
}''')

brain = nlm.Brain.createDefault(config)
brain.initialize()
world = nlm.SimpleWorld.create(100, 100)
agent = nlm.AgentBrain.createWithBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Prediction error experiment
environment_sequences = [
    {"name": "ABA", "sequence": [0, 1, 0]},
    {"name": "ABC", "sequence": [0, 1, 2]},
    {"name": "AABB", "sequence": [0, 0, 1, 1]}
]

for env in environment_sequences:
    print(f"\n{'='*50}")
    print(f"Testing environment: {env['name']}")
    print(f"{'='*50}")
    
    sequence = env['sequence']
    trial_duration = len(sequence) * 50  # Steps per trial
    prediction_errors = []
    prediction_confidence = []
    
    for trial in range(10):  # 10 trials per sequence
        # Reset environment and agent
        world.reset()
        
        # Present sequence
        for step, expected_state in enumerate(sequence):
            # Generate current state
            current_state = np.random.randint(0, 3)  # State 0-2
            
            # Create percept with expected next state
            percept = {
                'current_state': current_state,
                'expected_next': sequence[step % len(sequence)],
                'position': step,
                'trial': trial
            }
            
            # Process sensory input
            agent.processSensoryInput(percept)
            brain.step(trial * trial_duration + step)
            
            # Get prediction error
            if hasattr(agent, 'getPredictionError'):
                error = agent.getPredictionError()
                prediction_errors.append(error)
            
            # Get prediction confidence
            if hasattr(agent, 'getPredictionConfidence'):
                confidence = agent.getPredictionConfidence()
                prediction_confidence.append(confidence)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            world.update(0.01)
            
            # Provide reward based on prediction accuracy
            if hasattr(agent, 'getCurrentReward'):
                reward = agent.getCurrentReward()
                agent.applyRewardModulation(reward)
        
        # Calculate trial statistics
        if prediction_errors and prediction_confidence:
            avg_error = np.mean(prediction_errors[-len(sequence):])
            avg_confidence = np.mean(prediction_confidence[-len(sequence):])
            
            print(f"Trial {trial + 1}: Error = {avg_error:.3f}, Confidence = {avg_confidence:.3f}")
    
    # Analyze sequence learning
    final_errors = prediction_errors[-len(sequence):]
    final_confidence = prediction_confidence[-len(sequence):]
    
    avg_final_error = np.mean(final_errors)
    avg_final_confidence = np.mean(final_confidence)
    error_improvement = np.mean(prediction_errors[:-len(sequence)]) - avg_final_error
    
    print(f"\nSequence {env['name']} Results:")
    print(f"  Final prediction error: {avg_final_error:.3f}")
    print(f"  Final prediction confidence: {avg_final_confidence:.3f}")
    print(f"  Error improvement: {error_improvement:.3f}")
    
    # Evaluate learning success
    if avg_final_error < 0.2 and avg_final_confidence > 0.7:
        print(f"  ✓ Successfully learned sequence pattern")
    elif avg_final_error < 0.4:
        print(f"  ⚠ Partially learned sequence pattern")
    else:
        print(f"  ✗ Failed to learn sequence pattern")
```

## 5. Development and Maturation Studies

### 5.1 Developmental Plasticity

```python
import pynlm as nlm
import numpy as np

# Create brain with developmental system
config = nlm.Config.createFromJSON('''{
    "brain.neuron_count": 800,
    "brain.region_count": 2,
    "development.enabled": true,
    "development.plasticity_rate": 0.1,
    "development.critical_period_duration": 500,
    "development.maturation_rate": 0.05,
    "plasticity.stdp_enabled": true,
    "plasticity.hebbian_enabled": true
}''')

brain = nlm.Brain.createDefault(config)
brain.initialize()
world = nlm.SimpleWorld.create(120, 120)
agent = nlm.AgentBrain.createWithBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Developmental study parameters
total_steps = 2000
plastic_reinforcement_steps = 100
learning_stimuli = [
    {"id": "A", "features": np.random.rand(10), "reward": 1.0},
    {"id": "B", "features": np.random.rand(10), "reward": 0.5},
    {"id": "C", "features": np.random.rand(10), "reward": 0.1}
]

# Developmental timeline tracking
development_log = {
    'steps': [],
    'plasticity_rates': [],
    'learning_curves': [],
    'behavioral_metrics': []
}

for step in range(total_steps):
    # Get current developmental stage
    if hasattr(brain, 'getDevelopmentalStage'):
        stage = brain.getDevelopmentalStage()
        
        # Get plasticity rate for this stage
        if hasattr(brain, 'getPlasticityRate'):
            plasticity_rate = brain.getPlasticityRate()
        else:
            plasticity_rate = config.getOr("development.plasticity_rate", 0.1)
        
        # Present learning stimuli
        current_stimulus = np.random.choice(learning_stimuli)
        
        percept = {
            'stimulus_id': current_stimulus['id'],
            'features': current_stimulus['features'],
            'step': step,
            'developmental_stage': stage,
            'plasticity_rate': plasticity_rate
        }
        
        # Process sensory input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Apply reward
        agent.applyRewardModulation(current_stimulus['reward'])
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        world.update(0.01)
        
        # Record developmental metrics
        if step % 50 == 0:
            learning_curve = agent.getLearningCurve() if hasattr(agent, 'getLearningCurve') else 0
            behavioral_metric = agent.getBehavioralMetric() if hasattr(agent, 'getBehavioralMetric') else 0
            
            development_log['steps'].append(step)
            development_log['plasticity_rates'].append(plasticity_rate)
            development_log['learning_curves'].append(learning_curve)
            development_log['behavioral_metrics'].append(behavioral_metric)
            
            print(f"Step {step}: Stage={stage}, Plasticity={plasticity_rate:.3f}, "
                  f"Learning={learning_curve:.3f}, Behavior={behavioral_metric:.3f}")

# Analyze development patterns
print("\nDevelopmental Analysis:")
if development_log['plasticity_rates']:
    initial_plasticity = development_log['plasticity_rates'][0]
    final_plasticity = development_log['plasticity_rates'][-1]
    plasticity_change = final_plasticity - initial_plasticity
    
    initial_learning = development_log['learning_curves'][0]
    final_learning = development_log['learning_curves'][-1]
    learning_improvement = final_learning - initial_learning
    
    print(f"Plasticity rate change: {plasticity_change:+.3f} "
          f"({'increase' if plasticity_change > 0 else 'decrease'})")
    print(f"Learning improvement: {learning_improvement:+.3f} "
          f"({'improvement' if learning_improvement > 0 else 'decline'})")
    
    # Determine developmental outcome
    if plasticity_change < -0.05 and learning_improvement > 0.2:
        print("✓ Normal development: Plasticity decreases, learning improves")
    elif plasticity_change > 0.05 and learning_improvement < 0.1:
        print("⚠ Extended critical period: Plasticity remains high")
    elif plasticity_change < -0.1:
        print("⚠ Premature maturation: Plasticity decreases too quickly")
    else:
        print("✓ Developmental trajectory within normal range")
```

### 5.2 Structural Plasticity

```python
import pynlm as nlm
import networkx as nx

# Create brain with structural plasticity
config = nlm.Config.createFromJSON('''{
    "brain.neuron_count": 600,
    "brain.region_count": 2,
    "plasticity.structural.enabled": true,
    "plasticity.structural.synaptogenesis_rate": 0.01,
    "plasticity.structural.pruning_threshold": 0.3,
    "plasticity.structural.reinforcement_threshold": 0.8,
    "development.enabled": true
}''')

brain = nnl.Brain.createDefault(config)
brain.initialize()
world = nlm.SimpleWorld.create(100, 100)
agent = nlm.AgentBrain.createWithBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Structural plasticity experiment
experiment_duration = 1500
network_history = []
connectivity_stats = []

for step in range(experiment_duration):
    # Get current network statistics
    if hasattr(brain, 'getNetworkStats'):
        stats = brain.getNetworkStats()
        network_history.append(stats)
    
    # Generate learning experience
    reward = np.random.random() * 2 - 1  # Random reward (-1 to 1)
    
    percept = {
        'step': step,
        'reward': reward,
        'reinforce': abs(reward) > 0.7  # High reward reinforces connections
    }
    
    # Process sensory input
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Apply reward modulation
    agent.applyRewardModulation(reward)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    world.update(0.01)
    
    # Record connectivity statistics
    if step % 100 == 0 and network_history:
        current_stats = network_history[-1]
        connectivity_stats.append({
            'step': step,
            'total_connections': current_stats.get('total_connections', 0),
            'average_strength': current_stats.get('average_strength', 0),
            'max_degree': current_stats.get('max_degree', 0),
            'cluster_coefficient': current_stats.get('cluster_coefficient', 0)
        })
        
        print(f"Step {step}:")
        print(f"  Total connections: {current_stats.get('total_connections', 0)}")
        print(f"  Average connection strength: {current_stats.get('average_strength', 0):.3f}")
        print(f"  Max neuron degree: {current_stats.get('max_degree', 0)}")
        print(f"  Network clustering: {current_stats.get('cluster_coefficient', 0):.3f}")

# Analyze structural plasticity
print("\nStructural Plasticity Analysis:")
if connectivity_stats:
    initial_connections = connectivity_stats[0]['total_connections']
    final_connections = connectivity_stats[-1]['total_connections']
    connection_change = final_connections - initial_connections
    
    initial_clustering = connectivity_stats[0]['cluster_coefficient']
    final_clustering = connectivity_stats[-1]['cluster_coefficient']
    clustering_change = final_clustering - initial_clustering
    
    print(f"Connection count change: {connection_change:+d} "
          f"({'growth' if connection_change > 0 else 'pruning'})")
    print(f"Network clustering change: {clustering_change:+.3f}")
    
    # Assess structural development
    if connection_change > 0 and 0.3 < clustering_change < 0.6:
        print("✓ Healthy structural development: Network growing and organizing")
    elif connection_change > 0:
        print("⚠ Rapid growth without organization: May need pruning")
    elif abs(connection_change) < 10:
        print("✓ Stable network: No significant structural changes")
    else:
        print("⚠ Network degradation: Excessive pruning or instability")
else:
    print("No connectivity data collected - check structural plasticity implementation")
```

## 6. Custom Brain Architectures

### 6.1 Modular Brain Construction

```python
import pynlm as nlm
import numpy as np

# Define custom brain architecture
class CustomBrainArchitecture:
    def __init__(self, config_template):
        self.config_template = config_template
        self.regions = {}
        self.connections = []
    
    def createRegion(self, name, neuron_count, neuron_type="excitatory"):
        """Create a custom brain region"""
        region_config = self.config_template.copy()
        region_config.update({
            "brain.region_name": name,
            "brain.neuron_count": neuron_count,
            "brain.neuron_type": neuron_type
        })
        
        region = nlm.Brain.createDefault(json.dumps(region_config))
        self.regions[name] = region
        return region
    
    def connectRegions(self, source_region, target_region, connection_probability=0.1):
        """Create connections between regions"""
        connection = {
            "source": source_region,
            "target": target_region,
            "probability": connection_probability,
            "weight": np.random.random() * 0.5 + 0.1
        }
        self.connections.append(connection)
        return connection
    
    def buildBrain(self) -> nlm.Brain:
        """Build the complete custom brain"""
        # Create regions
        sensory_region = self.createRegion("sensory", 200, "excitatory")
        association_region = self.createRegion("association", 300, "excitatory")
        motor_region = self.createRegion("motor", 150, "inhibitory")
        
        # Create connections
        self.connectRegions(sensory_region, association_region, 0.15)
        self.connectRegions(association_region, motor_region, 0.2)
        self.connectRegions(association_region, association_region, 0.05)  # Recurrent
        
        # Create main brain
        main_config = self.config_template.copy()
        main_config.update({
            "brain.custom_architecture": True,
            "brain.region_count": len(self.regions),
            "brain.total_neurons": sum(
                self.config_template.get("brain.neuron_count", 500) 
                for region in self.regions.values()
            )
        })
        
        return nlm.Brain.createDefault(json.dumps(main_config))

# Use custom architecture
config_template = {
    "brain.neuron_count": 500,
    "brain.region_count": 3,
    "brain.connection_probability": 0.1,
    "brain.learning_rate": 0.01
}

# Build custom brain
builder = CustomBrainArchitecture(config_template)
custom_brain = builder.buildBrain()

print("Custom brain architecture built successfully!")
print(f"Regions created: {list(builder.regions.keys())}")
print(f"Connections configured: {len(builder.connections)}")
```

### 6.2 Hybrid Learning Systems

```python
import pynlm as nlm
import numpy as np

# Create hybrid learning system with multiple plasticity mechanisms
class HybridLearningSystem:
    def __init__(self):
        self.config = {
            "brain.neuron_count": 800,
            "brain.region_count": 2,
            "plasticity.stdp.enabled": True,
            "plasticity.hebbian.enabled": True,
            "plasticity.reward_modulated.enabled": True,
            "plasticity.eligibility_trace.enabled": True,
            "neuromodulation.dopamine.enabled": True,
            "neuromodulation.curiosity.enabled": True
        }
        
        self.brain = None
        self.world = None
        self.agent = None
        self.eligibility_traces = {}
    
    def initialize(self):
        """Initialize hybrid learning system"""
        config_json = json.dumps(self.config)
        self.brain = nlm.Brain.createDefault(config_json)
        self.brain.initialize()
        
        self.world = nlm.SimpleWorld.create(120, 120)
        self.agent = nlm.AgentBrain.createWithBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        
        print("Hybrid learning system initialized")
    
    def getCurrentState(self):
        """Get current system state"""
        state = {
            'step': self.brain.getStepCount() if hasattr(self.brain, 'getStepCount') else 0,
            'total_spikes': self.brain.getTotalSpikeCount() if hasattr(self.brain, 'getTotalSpikeCount') else 0,
            'eligibility_traces': self.eligibility_traces.copy()
        }
        return state
    
    def runHybridExperiment(self, duration=1000):
        """Run experiment with hybrid learning"""
        learning_metrics = {
            'steps': list(range(0, duration, 100)),
            'stdp_weight_changes': [],
            'hebbian_weight_changes': [],
            'reward_modulated_changes': [],
            'curiosity_driven_actions': 0
        }
        
        for step in range(duration):
            # Get current neuromodulation state
            curiosity = self.agent.getCuriosity() if hasattr(self.agent, 'getCuriosity') else 0
            dopamine = self.agent.getDopamine() if hasattr(self.agent, 'getDopamine') else 0
            
            # Generate reward with exploration bias
            base_reward = np.random.random() * 2 - 1  # -1 to 1
            exploration_bonus = curiosity * 0.5 if curiosity > 0.5 else 0
            total_reward = base_reward + exploration_bonus
            
            # Apply reward modulation
            self.agent.applyRewardModulation(total_reward)
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation with hybrid plasticity
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Track curiosity-driven actions
            if action.get('exploration', False):
                learning_metrics['curiosity_driven_actions'] += 1
            
            # Record learning metrics periodically
            if step % 100 == 0:
                # Get weight changes from different mechanisms
                if hasattr(self.brain, 'getSTDPWeightChanges'):
                    stdp_changes = self.brain.getSTDPWeightChanges()
                    learning_metrics['stdp_weight_changes'].append(stdp_changes)
                
                if hasattr(self.brain, 'getHebbianWeightChanges'):
                    hebbian_changes = self.brain.getHebbianWeightChanges()
                    learning_metrics['hebbian_weight_changes'].append(hebbian_changes)
                
                if hasattr(self.brain, 'getRewardModulatedChanges'):
                    reward_changes = self.brain.getRewardModulatedChanges()
                    learning_metrics['reward_modulated_changes'].append(reward_changes)
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            self.world.update(0.01)
        
        return learning_metrics
    
    def analyzeHybridLearning(self, metrics):
        """Analyze hybrid learning effectiveness"""
        print("\nHybrid Learning Analysis:")
        
        if metrics['stdp_weight_changes']:
            avg_stdp = np.mean(metrics['stdp_weight_changes'], axis=0)
            stdp_magnitude = np.sqrt(np.sum(avg_stdp ** 2))
            print(f"STDP learning magnitude: {stdp_magnitude:.4f}")
        
        if metrics['hebbian_weight_changes']:
            avg_hebbian = np.mean(metrics['hebbian_weight_changes'], axis=0)
            hebbian_magnitude = np.sqrt(np.sum(avg_hebbian ** 2))
            print(f"Hebbian learning magnitude: {hebbian_magnitude:.4f}")
        
        if metrics['reward_modulated_changes']:
            avg_reward = np.mean(metrics['reward_modulated_changes'], axis=0)
            reward_magnitude = np.sqrt(np.sum(avg_reward ** 2))
            print(f"Reward-modulated learning magnitude: {reward_magnitude:.4f}")
        
        curiosity_ratio = metrics['curiosity_driven_actions'] / sum(1 for s in metrics['steps'])
        print(f"Curiosity-driven action ratio: {curiosity_ratio:.3f}")
        
        # Overall learning effectiveness
        total_magnitude = 0
        if metrics['stdp_weight_changes']:
            total_magnitude += stdp_magnitude
        if metrics['hebbian_weight_changes']:
            total_magnitude += hebbian_magnitude
        if metrics['reward_modulated_changes']:
            total_magnitude += reward_magnitude
        
        if total_magnitude > 0.5:
            print("✓ Hybrid learning system highly effective")
        elif total_magnitude > 0.2:
            print("⚠ Hybrid learning system moderately effective")
        else:
            print("✗ Hybrid learning system needs improvement")

# Use hybrid learning system
hybrid_system = HybridLearningSystem()
hybrid_system.initialize()

# Run experiment
print("Running hybrid learning experiment...")
metrics = hybrid_system.runHybridExperiment(1000)

# Analyze results
hybrid_system.analyzeHybridLearning(metrics)
```

## 7. Integration Examples

### 7.1 Complete Integration Workflow

```python
import pynlm as nlm
import time

def runCompleteIntegrationWorkflow(config_file=None):
    """Run complete NLM integration workflow"""
    print("Starting Complete NLM Integration Workflow")
    print("=" * 60)
    
    # Step 1: Initialize components
    print("\nStep 1: Initializing Components")
    start_time = time.time()
    
    # Create brain with configuration
    if config_file:
        with open(config_file, 'r') as f:
            config_json = f.read()
        brain = nlm.Brain.createDefault(config_json)
    else:
        brain = nlm.Brain.createDefault()
    
    brain.initialize()
    print(f"  Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Create world
    world = nlm.SimpleWorld.create(200, 200, 20, 20)
    world.reset()
    print(f"  World created: {world.getWidth()} x {world.getHeight()}")
    
    # Create agent
    agent = nlm.AgentBrain.createWithBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    print("  Agent configured with neuromodulation")
    
    # Create manager and runner
    manager = nlm.SimulationManager(brain, world)
    runner = nlm.ExperimentRunner()
    print("  Simulation manager and experiment runner created")
    
    init_time = time.time() - start_time
    print(f"  Initialization completed in {init_time:.2f} seconds")
    
    # Step 2: Run main experiment
    print("\nStep 2: Running Main Experiment")
    experiment_start = time.time()
    
    # Run agent experiment
    print("  Running agent experiment...")
    experiment_results = runner.runAgentExperiment(world, agent, 50, 20)
    
    # Get brain state
    state = manager.getState()
    print(f"  Experiment completed: {state['step_count']} steps")
    print(f"  Total spikes: {state['total_spikes']}")
    print(f"  Average firing rate: {state['average_firing_rate']:.2f} Hz")
    
    # Step 3: Save checkpoint
    print("\nStep 3: Saving Checkpoint")
    checkpoint_start = time.time()
    
    checkpoint_file = "nlm_integration_checkpoint.bin"
    manager.saveCheckpoint(checkpoint_file)
    print(f"  Checkpoint saved to: {checkpoint_file}")
    
    checkpoint_time = time.time() - checkpoint_start
    print(f"  Checkpoint completed in {checkpoint_time:.2f} seconds")
    
    # Step 4: Visualization
    print("\nStep 4: Visualization")
    viz = nlm.Visualization()
    
    # Visualize brain state
    brain_viz = viz.visualizeBrain(brain)
    print("  Brain visualization generated")
    
    # Visualize world state
    world_viz = viz.visualizeWorld(world)
    print("  World visualization generated")
    
    # Step 5: Analysis
    print("\nStep 5: Analysis")
    analysis_start = time.time()
    
    # Analyze experiment results
    print("  Analyzing experiment results...")
    experiment_analysis = {
        'total_reward': experiment_results.get('total_reward', 0),
        'average_reward': experiment_results.get('average_reward', 0),
        'success_rate': experiment_results.get('success_rate', 0),
        'learning_curve': experiment_results.get('learning_curve', [])
    }
    
    # Analyze brain state
    print("  Analyzing brain state...")
    brain_analysis = {
        'neuron_count': brain.getTotalNeuronCount(),
        'spike_count': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'memory_usage': state.get('memory_mb', 0)
    }
    
    analysis_time = time.time() - analysis_start
    print(f"  Analysis completed in {analysis_time:.2f} seconds")
    
    # Step 6: Report
    print("\nStep 6: Integration Report")
    total_time = time.time() - experiment_start
    
    print(f"  Total experiment time: {total_time:.2f} seconds")
    print(f"  Final brain state: {brain_analysis['neuron_count']} neurons, "
          f"{brain_analysis['spike_count']} spikes")
    print(f"  Memory usage: {brain_analysis['memory_usage']:.2f} MB")
    print(f"  Learning rate: {brain_analysis['firing_rate']:.2f} Hz")
    
    # Integration success assessment
    integration_score = 0
    if brain_analysis['firing_rate'] > 0.5:
        integration_score += 1
    if experiment_analysis['success_rate'] > 0.5:
        integration_score += 1
    if brain_analysis['memory_usage'] < 100:
        integration_score += 1
    
    print(f"\nIntegration Score: {integration_score}/3")
    if integration_score == 3:
        print("✓ Integration successful - All systems working correctly")
    elif integration_score >= 2:
        print("⚠ Integration partially successful - Minor issues detected")
    else:
        print("✗ Integration failed - Major issues detected")
    
    print("\n" + "=" * 60)
    print("Integration workflow completed!")
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'manager': manager,
        'results': experiment_analysis,
        'state': brain_analysis,
        'success': integration_score >= 2
    }

# Run complete workflow if executed directly
if __name__ == "__main__":
    results = runCompleteIntegrationWorkflow()
```

### 7.2 Advanced Memory Integration Example

```python
import pynlm as nlm
import numpy as np

class AdvancedMemoryIntegration:
    """Advanced example showing memory system integration"""
    
    def __init__(self):
        self.setupConfiguration()
        self.setupComponents()
    
    def setupConfiguration(self):
        """Setup advanced memory configuration"""
        self.config = {
            "brain.neuron_count": 1200,
            "brain.region_count": 3,
            "memory.episodic.enabled": True,
            "memory.episodic.capacity": 100,
            "memory.episodic.recall_threshold": 0.7,
            "memory.working.enabled": True,
            "memory.working.capacity": 80,
            "memory.working.decay_rate": 0.98,
            "memory.associative.enabled": True,
            "memory.associative.similarity_threshold": 0.6,
            "memory.concept_formation.enabled": True,
            "memory.concept_formation.stability_threshold": 0.8,
            "neuromodulation.dopamine.enabled": True,
            "neuromodulation.dopamine.sensitivity": 0.4,
            "neuromodulation.curiosity.enabled": True
        }
    
    def setupComponents(self):
        """Initialize all components"""
        config_json = json.dumps(self.config)
        self.brain = nlm.Brain.createDefault(config_json)
        self.brain.initialize()
        
        self.world = nlm.SimpleWorld.create(150, 150, 15, 15)
        self.world.reset()
        
        self.agent = nlm.AgentBrain.createWithBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        
        self.manager = nlm.SimulationManager(self.brain, self.world)
        self.viz = nlm.Visualization()
    
    def runMemoryDemonstration(self, duration=2000):
        """Run comprehensive memory integration demonstration"""
        print("Advanced Memory Integration Demonstration")
        print("=" * 60)
        
        # Memory tracking
        memory_log = {
            'steps': [],
            'episodic_memory': [],
            'working_memory': [],
            'concept_formation': [],
            'recall_accuracy': [],
            'consolidation_events': 0
        }
        
        # Generate structured experience episodes
        episode_templates = self.generateEpisodeTemplates()
        
        for step in range(duration):
            # Determine current episode based on step
            episode_idx = (step // 50) % len(episode_templates)
            episode = episode_templates[episode_idx]
            
            # Create perceptual input based on episode
            percept = self.generatePercept(episode, step)
            
            # Process sensory input (engages memory systems)
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Apply reward modulation
            reward = episode.get('reward', 0.1)
            self.agent.applyRewardModulation(reward)
            
            # Get action (may trigger memory recall)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            self.world.update(0.01)
            
            # Record memory metrics
            if step % 100 == 0:
                self.recordMemoryMetrics(step, memory_log, episode)
                
                # Generate visualization every 500 steps
                if step % 500 == 0:
                    self.generateVisualizations(step)
        
        return memory_log
    
    def generateEpisodeTemplates(self):
        """Generate structured experience templates"""
        return [
            {
                'name': 'Exploration',
                'features': {
                    'novelty': np.random.random(),
                    'complexity': np.random.random() * 0.5,
                    'reward_predictability': 0.7
                },
                'reward': 0.5,
                'duration': 50
            },
            {
                'name': 'Exploitation',
                'features': {
                    'novelty': 0.0,
                    'complexity': np.random.random() * 0.3 + 0.1,
                    'reward_predictability': 0.9
                },
                'reward': 1.0,
                'duration': 50
            },
            {
                'name': 'Novelty Seeking',
                'features': {
                    'novelty': np.random.random() * 0.8 + 0.2,
                    'complexity': np.random.random(),
                    'reward_predictability': 0.5
                },
                'reward': 0.3,
                'duration': 50
            },
            {
                'name': 'Rest/Consolidation',
                'features': {
                    'novelty': 0.0,
                    'complexity': 0.0,
                    'reward_predictability': 0.0
                },
                'reward': 0.0,
                'duration': 50
            }
        ]
    
    def generatePercept(self, episode, step):
        """Generate perceptual input based on episode template"""
        base_features = np.random.random(20)
        
        # Apply episode-specific modifications
        features = base_features.copy()
        features[0] = episode['features']['novelty']  # Novelty channel
        features[1] = episode['features']['complexity']  # Complexity channel
        features[2] = episode['features']['reward_predictability']  # Reward channel
        
        return {
            'features': features,
            'episode_name': episode['name'],
            'step_in_episode': step % episode['duration'],
            'total_steps': episode['duration']
        }
    
    def recordMemoryMetrics(self, step, memory_log, episode):
        """Record comprehensive memory metrics"""
        # Get episodic memory statistics
        if hasattr(self.brain, 'getEpisodicMemoryStats'):
            episodic_stats = self.brain.getEpisodicMemoryStats()
            memory_log['episodic_memory'].append({
                'step': step,
                'episode_count': episodic_stats.get('episode_count', 0),
                'storage_utilization': episodic_stats.get('storage_utilization', 0),
                'recall_success_rate': episodic_stats.get('recall_success_rate', 0)
            })
        
        # Get working memory statistics
        if hasattr(self.brain, 'getWorkingMemoryStats'):
            working_stats = self.brain.getWorkingMemoryStats()
            memory_log['working_memory'].append({
                'step': step,
                'active_entries': working_stats.get('active_entries', 0),
                'capacity_utilization': working_stats.get('capacity_utilization', 0),
                'average_confidence': working_stats.get('average_confidence', 0)
            })
        
        # Get concept formation statistics
        if hasattr(self.brain, 'getConceptFormationStats'):
            concept_stats = self.brain.getConceptFormationStats()
            memory_log['concept_formation'].append({
                'step': step,
                'concept_count': concept_stats.get('concept_count', 0),
                'concept_stability': concept_stats.get('concept_stability', 0),
                'generalization_ability': concept_stats.get('generalization_ability', 0)
            })
        
        # Record episode information
        memory_log['steps'].append(step)
        
        # Check for consolidation events
        if step % 1000 == 0 and hasattr(self.brain, 'triggerConsolidation'):
            self.brain.triggerConsolidation()
            memory_log['consolidation_events'] += 1
    
    def generateVisualizations(self, step):
        """Generate memory system visualizations"""
        print(f"\nVisualization at step {step}:")
        
        # Brain visualization showing memory activity
        brain_viz = self.viz.visualizeBrain(self.brain)
        print("  Brain state (with memory patterns):")
        for line in brain_viz.split('\n')[:10]:
            print(f"    {line}")
        
        # Memory-specific visualization
        if hasattr(self.brain, 'visualizeMemoryState'):
            memory_viz = self.brain.visualizeMemoryState()
            print("  Memory system state:")
            for line in memory_viz.split('\n')[:10]:
                print(f"    {line}")
    
    def analyzeMemoryPerformance(self, memory_log):
        """Analyze memory system performance"""
        print("\nMemory Performance Analysis:")
        print("=" * 60)
        
        if not memory_log['episodic_memory']:
            print("No memory data available")
            return
        
        # Analyze episodic memory
        episodic_data = memory_log['episodic_memory']
        final_episodic = episodic_data[-1]
        episodic_storage = final_episodic['storage_utilization']
        episodic_recall = final_episodic['recall_success_rate']
        
        print(f"Episodic Memory:")
        print(f"  Storage utilization: {episodic_storage:.2%}")
        print(f"  Recall success rate: {episodic_recall:.2%}")
        
        # Analyze working memory
        working_data = memory_log['working_memory']
        final_working = working_data[-1]
        working_utilization = final_working['capacity_utilization']
        working_confidence = final_working['average_confidence']
        
        print(f"\nWorking Memory:")
        print(f"  Capacity utilization: {working_utilization:.2%}")
        print(f"  Average confidence: {working_confidence:.2%}")
        
        # Analyze concept formation
        concept_data = memory_log['concept_formation']
        final_concept = concept_data[-1]
        concept_count = final_concept['concept_count']
        concept_stability = final_concept['concept_stability']
        concept_generalization = final_concept['generalization_ability']
        
        print(f"\nConcept Formation:")
        print(f"  Number of concepts: {concept_count}")
        print(f"  Concept stability: {concept_stability:.2%}")
        print(f"  Generalization ability: {concept_generalization:.2%}")
        
        # Overall memory assessment
        memory_score = (
            min(episodic_storage, 1.0) * 0.3 +
            min(episodic_recall, 1.0) * 0.3 +
            min(working_utilization, 1.0) * 0.2 +
            min(working_confidence, 1.0) * 0.2
        )
        
        print(f"\nOverall Memory Performance Score: {memory_score:.2%}")
        
        if memory_score > 0.7:
            print("✓ Memory systems performing well")
        elif memory_score > 0.4:
            print("⚠ Memory systems need optimization")
        else:
            print("✗ Memory systems underperforming")
        
        # Consolidation analysis
        print(f"\nConsolidation Events: {memory_log['consolidation_events']}")
        if memory_log['consolidation_events'] > 0:
            print("✓ Memory consolidation active")

# Run advanced memory integration demonstration
if __name__ == "__main__":
    advanced_memory = AdvancedMemoryIntegration()
    memory_log = advanced_memory.runMemoryDemonstration(2000)
    advanced_memory.analyzeMemoryPerformance(memory_log)
```

## 8. Command-Line Interface Examples

### 8.1 Basic CLI Usage

```bash
# Run NLM with default configuration
./nlm_phase6_demo

# Run with custom configuration file
./nlm_phase6_demo --config my_config.json

# Run with verbose output
./nlm_phase6_demo --verbose

# Run with specific number of episodes
./nlm_phase6_demo --episodes 100

# Run with timestep
./nlm_phase6_demo --timestep 0.01
```

### 8.2 Configuration File Examples

**Example config.json:**
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "learning_rate": 0.01,
        "plasticity_strength": 0.5
    },
    "memory": {
        "episodic": {
            "enabled": true,
            "capacity": 100,
            "recall_threshold": 0.7
        },
        "working": {
            "enabled": true,
            "capacity": 80,
            "decay_rate": 0.98
        }
    },
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "sensitivity": 0.4
        },
        "curiosity": {
            "enabled": true,
            "decay_rate": 0.95
        }
    },
    "world": {
        "width": 200,
        "height": 200,
        "vision_width": 20,
        "vision_height": 20
    }
}
```

### 8.3 Batch Processing Script

**batch_experiment.sh:**
```bash
#!/bin/bash

# Batch processing script for NLM experiments

EXPERIMENT_DIR="experiments"
CONFIG_TEMPLATE="config_template.json"
RESULTS_DIR="results"

mkdir -p $RESULTS_DIR

# Function to run experiment with given parameters
run_experiment() {
    local experiment_id=$1
    local neuron_count=$2
    local episodes=$3
    local timestep=$4
    
    echo "Running experiment $experiment_id..."
    
    # Create experiment configuration
    config=$(cat $CONFIG_TEMPLATE | \
        sed "s/{{neuron_count}}/$neuron_count/g" | \
        sed "s/{{episodes}}/$episodes/g" | \
        sed "s/{{timestep}}/$timestep/g")
    
    echo "$config" > "${RESULTS_DIR}/config_${experiment_id}.json"
    
    # Run experiment
    ./nlm_phase6_demo --config "${RESULTS_DIR}/config_${experiment_id}.json" \
        --episodes $episodes \
        --timestep $timestep \
        --output "${RESULTS_DIR}/results_${experiment_id}.json"
    
    echo "Experiment $experiment_id completed"
}

# Run parameter sweep
run_experiment "baseline" 500 50 0.01
run_experiment "high_neural" 1000 50 0.01
run_experiment "long_training" 500 200 0.005
run_experiment "fast_simulation" 500 50 0.02

# Combine results
echo "Combining results..."
python3 combine_results.py $RESULTS_DIR

echo "Batch processing completed!"
```

### 8.4 Python Script for CLI Integration

**nlm_script.py:**
```python
#!/usr/bin/env python3
import pynlm as nlm
import json
import sys

def run_from_cli():
    """Run NLM from command-line interface"""
    if len(sys.argv) < 2:
        print("Usage: python nlm_script.py <command> [options]")
        print("Commands: run, analyze, visualize, benchmark")
        return
    
    command = sys.argv[1]
    
    if command == "run":
        # Run NLM simulation
        episodes = int(sys.argv[2]) if len(sys.argv) > 2 else 50
        timestep = float(sys.argv[3]) if len(sys.argv) > 3 else 0.01
        
        print(f"Running NLM simulation: {episodes} episodes, timestep {timestep}")
        
        # Create brain
        brain = nlm.Brain.createDefault()
        brain.initialize()
        
        # Create world
        world = nlm.SimpleWorld.create(100, 100)
        world.reset()
        
        # Create agent
        agent = nlm.AgentBrain.createWithBrain(brain)
        agent.initialize(world)
        agent.enableCuriosity(True)
        agent.enableRewardModulation(True)
        
        # Run simulation
        for episode in range(episodes):
            for step in range(int(100 / timestep)):
                # Get percept
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                world.update(timestep)
                
                if step % 20 == 0:
                    print(f"Episode {episode + 1}/{episodes}, Step {step}: "
                          f"{brain.getActiveNeuronCount()} active neurons")
        
        print("Simulation completed!")
        
    elif command == "analyze":
        # Analyze saved results
        results_file = sys.argv[2] if len(sys.argv) > 2 else "results.json"
        
        with open(results_file, 'r') as f:
            results = json.load(f)
        
        print("Analyzing results...")
        print(f"Total episodes: {results.get('episodes', 0)}")
        print(f"Average reward: {results.get('average_reward', 0):.3f}")
        print(f"Learning improvement: {results.get('improvement', 0):.3f}")
        
    elif command == "visualize":
        # Generate visualizations
        output_dir = sys.argv[2] if len(sys.argv) > 2 else "visualizations"
        
        print(f"Generating visualizations in {output_dir}")
        
        # Create brain and world
        brain = nlm.Brain.createDefault()
        brain.initialize()
        
        world = nlm.SimpleWorld.create(100, 100)
        world.reset()
        
        viz = nlm.Visualization()
        
        # Generate brain visualization
        brain_viz = viz.visualizeBrain(brain)
        with open(f"{output_dir}/brain_state.txt", 'w') as f:
            f.write(brain_viz)
        
        # Generate world visualization
        world_viz = viz.visualizeWorld(world)
        with open(f"{output_dir}/world_state.txt", 'w') as f:
            f.write(world_viz)
        
        print(f"Visualizations saved to {output_dir}")
        
    elif command == "benchmark":
        # Run benchmark tests
        timesteps = [0.01, 0.005, 0.001]
        episode_counts = [10, 50, 100]
        
        print("Running benchmark tests...")
        
        for timestep in timesteps:
            for episode_count in episode_counts:
                print(f"Benchmark: {episode_count} episodes, timestep {timestep}")
                
                start_time = time.time()
                
                brain = nlm.Brain.createDefault()
                brain.initialize()
                
                world = nlm.SimpleWorld.create(100, 100)
                world.reset()
                
                agent = nlm.AgentBrain.createWithBrain(brain)
                agent.initialize(world)
                
                # Run simulation
                for episode in range(episode_count):
                    for step in range(int(100 / timestep)):
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                        brain.step(step)
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                        world.update(timestep)
                
                elapsed = time.time() - start_time
                fps = (episode_count * 100) / elapsed
                
                print(f"  Performance: {fps:.1f} FPS")
        
        print("Benchmark completed!")

if __name__ == "__main__":
    run_from_cli()
```

### 8.5 Configuration File Generator

**config_generator.py:**
```python
#!/usr/bin/env python3
import pynlm as nlm
import json
import argparse

def generate_config():
    """Generate configuration file for NLM"""
    parser = argparse.ArgumentParser(description="Generate NLM configuration")
    
    parser.add_argument("--neuron-count", type=int, default=1000,
                       help="Number of neurons in the brain")
    parser.add_argument("--region-count", type=int, default=2,
                       help="Number of brain regions")
    parser.add_argument("--connection-prob", type=float, default=0.1,
                       help="Probability of synaptic connections")
    parser.add_argument("--learning-rate", type=float, default=0.01,
                       help="Learning rate for plasticity")
    parser.add_argument("--plasticity-strength", type=float, default=0.5,
                       help="Overall plasticity strength")
    parser.add_argument("--world-width", type=int, default=200,
                       help="World width")
    parser.add_argument("--world-height", type=int, default=200,
                       help="World height")
    parser.add_argument("--vision-width", type=int, default=20,
                       help="Vision width")
    parser.add_argument("--vision-height", type=int, default=20,
                       help="Vision height")
    parser.add_argument("--output", type=str, default="nlm_config.json",
                       help="Output configuration file")
    
    args = parser.parse_args()
    
    # Create configuration
    config = {
        "brain": {
            "neuron_count": args.neuron_count,
            "region_count": args.region_count,
            "connection_probability": args.connection_prob,
            "learning_rate": args.learning_rate,
            "plasticity_strength": args.plasticity_strength
        },
        "memory": {
            "episodic": {
                "enabled": True,
                "capacity": min(100, args.neuron_count // 10),
                "recall_threshold": 0.7
            },
            "working": {
                "enabled": True,
                "capacity": min(80, args.neuron_count // 12),
                "decay_rate": 0.98
            },
            "associative": {
                "enabled": True,
                "similarity_threshold": 0.6
            }
        },
        "neuromodulation": {
            "dopamine": {
                "enabled": True,
                "sensitivity": 0.4
            },
            "curiosity": {
                "enabled": True,
                "decay_rate": 0.95
            }
        },
        "world": {
            "width": args.world_width,
            "height": args.world_height,
            "vision_width": args.vision_width,
            "vision_height": args.vision_height
        }
    }
    
    # Write configuration to file
    with open(args.output, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"Configuration generated: {args.output}")
    
    # Print configuration summary
    print("\nConfiguration Summary:")
    print(f"  Neurons: {args.neuron_count}")
    print(f"  Regions: {args.region_count}")
    print(f"  Connections: {args.neuron_count * args.connection_prob:.0f} expected")
    print(f"  Learning rate: {args.learning_rate}")
    print(f"  Plasticity strength: {args.plasticity_strength}")
    print(f"  World size: {args.world_width} x {args.world_height}")
    print(f"  Vision range: {args.vision_width} x {args.vision_height}")

if __name__ == "__main__":
    generate_config()
```

## 9. Performance Optimization Examples

### 9.1 Efficient Memory Usage

```python
import pynlm as nlm
import time
import psutil
import os

class MemoryOptimizedSimulation:
    """Simulation optimized for memory efficiency"""
    
    def __init__(self, max_memory_mb=500):
        self.max_memory_mb = max_memory_mb
        self.setupOptimizedBrain()
    
    def setupOptimizedBrain(self):
        """Setup brain with memory optimizations"""
        config = nlm.Config.createFromJSON('''{
            "brain.neuron_count": 800,
            "brain.region_count": 2,
            "brain.use_compact_representation": true,
            "memory.episodic.use_compression": true,
            "memory.working.use_sparse_storage": true,
            "memory.associative.use_approximate": true
        }''')
        
        self.brain = nlm.Brain.createDefault(config)
        self.brain.initialize()
        
        self.world = nlm.SimpleWorld.create(150, 150)
        self.world.reset()
        
        self.agent = nlm.AgentBrain.createWithBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        
        self.manager = nlm.SimulationManager(self.brain, self.world)
    
    def runOptimized(self, duration=1000):
        """Run simulation with memory monitoring"""
        print("Running memory-optimized simulation...")
        
        memory_samples = []
        time_samples = []
        
        for step in range(duration):
            # Get current memory usage
            process = psutil.Process(os.getpid())
            memory_mb = process.memory_info().rss / 1024 / 1024
            memory_samples.append(memory_mb)
            
            # Check memory limit
            if memory_mb > self.max_memory_mb:
                print(f"Warning: Memory usage {memory_mb:.1f} MB exceeds limit {self.max_memory_mb} MB")
                self.forceGarbageCollection()
            
            # Run simulation step
            start_time = time.time()
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            self.world.update(0.01)
            
            elapsed = time.time() - start_time
            time_samples.append(elapsed)
            
            # Log metrics periodically
            if step % 100 == 0:
                avg_memory = sum(memory_samples[-10:]) / len(memory_samples[-10:])
                avg_time = sum(time_samples[-10:]) / len(time_samples[-10:])
                
                print(f"Step {step}: Memory={avg_memory:.1f}MB, "
                      f"Time={avg_time*1000:.1f}ms, "
                      f"FPS={1/avg_time:.1f}")
        
        # Calculate performance metrics
        total_time = sum(time_samples)
        avg_memory = sum(memory_samples) / len(memory_samples)
        max_memory = max(memory_samples)
        
        print(f"\nPerformance Summary:")
        print(f"  Total time: {total_time:.2f}s")
        print(f"  Average memory: {avg_memory:.1f} MB")
        print(f"  Peak memory: {max_memory:.1f} MB")
        print(f"  FPS: {duration / total_time:.1f}")
        print(f"  Memory efficiency: {(avg_memory / self.max_memory_mb):.1%}")
        
        return {
            'total_time': total_time,
            'average_memory': avg_memory,
            'peak_memory': max_memory,
            'fps': duration / total_time
        }
    
    def forceGarbageCollection(self):
        """Force garbage collection to free memory"""
        import gc
        gc.collect()
        print("  Garbage collection triggered")
```

### 9.2 Parallel Processing Example

```python
import pynlm as nlm
import numpy as np
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import time

class ParallelExperimentRunner:
    """Run NLM experiments in parallel"""
    
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or os.cpu_count()
        self.setupConfiguration()
    
    def setupConfiguration(self):
        """Setup configuration for parallel experiments"""
        self.base_config = {
            "brain.neuron_count": 500,
            "brain.region_count": 2,
            "brain.learning_rate": 0.01
        }
    
    def runSingleExperiment(self, experiment_id, parameters):
        """Run a single experiment with given parameters"""
        print(f"Starting experiment {experiment_id}...")
        
        # Create experiment-specific configuration
        config = self.base_config.copy()
        config.update(parameters)
        
        # Create components
        brain = nlm.Brain.createDefault(json.dumps(config))
        brain.initialize()
        
        world = nlm.SimpleWorld.create(100, 100)
        world.reset()
        
        agent = nlm.AgentBrain.createWithBrain(brain)
        agent.initialize(world)
        agent.enableCuriosity(True)
        agent.enableRewardModulation(True)
        
        # Run experiment
        episodes = parameters.get('episodes', 50)
        timestep = parameters.get('timestep', 0.01)
        
        rewards = []
        for episode in range(episodes):
            for step in range(int(100 / timestep)):
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                world.update(timestep)
                
                # Collect reward
                if hasattr(agent, 'getCurrentReward'):
                    rewards.append(agent.getCurrentReward())
        
        # Calculate results
        results = {
            'experiment_id': experiment_id,
            'total_reward': sum(rewards),
            'average_reward': np.mean(rewards) if rewards else 0,
            'reward_std': np.std(rewards) if rewards else 0,
            'episode_count': episodes,
            'parameters': parameters
        }
        
        print(f"Experiment {experiment_id} completed: "
              f"Avg reward = {results['average_reward']:.3f}")
        
        return results
    
    def runParallelExperiments(self, experiment_configs):
        """Run multiple experiments in parallel"""
        print(f"Running {len(experiment_configs)} experiments in parallel...")
        print(f"Using {self.num_workers} workers")
        
        start_time = time.time()
        
        # Use ThreadPoolExecutor for I/O bound tasks
        with ThreadPoolExecutor(max_workers=self.num_workers) as executor:
            # Submit all experiments
            futures = []
            for i, config in enumerate(experiment_configs):
                future = executor.submit(self.runSingleExperiment, i, config)
                futures.append(future)
            
            # Collect results
            results = []
            for future in futures:
                try:
                    result = future.result()
                    results.append(result)
                except Exception as e:
                    print(f"Experiment failed: {e}")
        
        total_time = time.time() - start_time
        
        # Sort results by experiment ID
        results.sort(key=lambda x: x['experiment_id'])
        
        print(f"\nAll experiments completed in {total_time:.2f} seconds")
        print(f"Average time per experiment: {total_time/len(experiment_configs):.2f} seconds")
        
        return results
    
    def analyzeResults(self, results):
        """Analyze parallel experiment results"""
        print("\nParallel Experiment Analysis:")
        print("=" * 60)
        
        # Group by parameters
        by_params = {}
        for result in results:
            params_key = str(result['parameters'])
            if params_key not in by_params:
                by_params[params_key] = []
            by_params[params_key].extend(result['rewards'] if 'rewards' in result else [])
        
        # Compare performance
        print("\nPerformance by Configuration:")
        for params, rewards in by_params.items():
            avg_reward = np.mean(rewards) if rewards else 0
            reward_std = np.std(rewards) if rewards else 0
            
            print(f"  {params}:")
            print(f"    Average reward: {avg_reward:.3f}")
            print(f"    Reward std: {reward_std:.3f}")
            print(f"    Samples: {len(rewards)}")
        
        # Overall statistics
        all_rewards = []
        for result in results:
            if 'rewards' in result:
                all_rewards.extend(result['rewards'])
        
        if all_rewards:
            print(f"\nOverall Statistics:")
            print(f"  Total experiments: {len(results)}")
            print(f"  Total reward samples: {len(all_rewards)}")
            print(f"  Global average reward: {np.mean(all_rewards):.3f}")
            print(f"  Global reward std: {np.std(all_rewards):.3f}")
            print(f"  Best experiment reward: {max(all_rewards):.3f}")
            print(f"  Worst experiment reward: {min(all_rewards):.3f}")

# Example usage
if __name__ == "__main__":
    # Define experiment configurations
    experiment_configs = [
        {'episodes': 50, 'timestep': 0.01},
        {'episodes': 100, 'timestep': 0.01},
        {'episodes': 50, 'timestep': 0.005},
        {'episodes': 100, 'timestep': 0.005},
        {'episodes': 50, 'timestep': 0.02}
    ]
    
    # Run parallel experiments
    runner = ParallelExperimentRunner(num_workers=3)
    results = runner.runParallelExperiments(experiment_configs)
    
    # Analyze results
    runner.analyzeResults(results)
```

## 10. Testing and Debugging Examples

### 10.1 Unit Testing Framework

```python
import pynlm as nlm
import unittest
import json

class TestNLMBindings(unittest.TestCase):
    """Unit tests for NLM Python bindings"""
    
    def setUp(self):
        """Setup test fixtures"""
        self.config = nlm.Config.createFromJSON('''{
            "brain.neuron_count": 100,
            "brain.region_count": 1,
            "brain.connection_probability": 0.1
        }''')
        
        self.brain = nlm.Brain.createDefault()
        self.brain.initialize()
        
        self.world = nlm.SimpleWorld.create(50, 50)
        self.world.reset()
        
        self.agent = nlm.AgentBrain.createWithBrain(self.brain)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up after tests"""
        if hasattr(self, 'manager'):
            del self.manager
    
    def test_config_creation(self):
        """Test configuration creation"""
        self.assertIsNotNone(self.config)
        self.assertEqual(len(self.config), 0)  # Empty config
    
    def test_config_access(self):
        """Test configuration access"""
        self.config["test_key"] = "test_value"
        self.assertEqual(self.config["test_key"], "test_value")
        self.assertIn("test_key", self.config)
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        self.assertIsNotNone(self.brain)
        self.assertTrue(self.brain.isInitialized())
    
    def test_brain_step(self):
        """Test brain computation step"""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run a step
        self.brain.step(0)
        
        # Check that step completed
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), initial_spikes)
    
    def test_world_creation(self):
        """Test world creation"""
        self.assertIsNotNone(self.world)
        self.assertEqual(self.world.getWidth(), 50)
        self.assertEqual(self.world.getHeight(), 50)
    
    def test_agent_creation(self):
        """Test agent creation"""
        self.assertIsNotNone(self.agent)
        self.assertEqual(self.agent.getBrain(), self.brain)
    
    def test_sensory_input(self):
        """Test sensory input processing"""
        percept = {"features": [0.1, 0.2, 0.3], "source": "test"}
        
        # This should not raise an exception
        self.agent.processSensoryInput(percept)
        self.brain.step(0)
    
    def test_motor_output(self):
        """Test motor output decoding"""
        action = self.agent.decodeMotorCommand()
        
        self.assertIsNotNone(action)
        self.assertIsInstance(action, dict)
    
    def test_reward_modulation(self):
        """Test reward modulation"""
        # Apply reward
        self.agent.applyRewardModulation(1.0)
        
        # Check that reward was processed
        # (Implementation depends on specific API)
        pass
    
    def test_context_manager(self):
        """Test context manager support"""
        with self.brain as managed_brain:
            self.assertEqual(managed_brain, self.brain)
            managed_brain.step(0)
    
    def test_error_handling(self):
        """Test error handling"""
        # Test invalid configuration
        invalid_config = nlm.Config.createFromJSON('''{
            "brain.neuron_count": -100
        }''')
        
        # Should handle invalid values gracefully
        self.assertIsNotNone(invalid_config)
    
    def test_memory_usage(self):
        """Test memory usage monitoring"""
        state = self.brain.getMemoryUsage()
        
        self.assertIsNotNone(state)
        self.assertIn('memory_mb', state)
        self.assertGreater(state['memory_mb'], 0)
    
    def test_statistics(self):
        """Test statistics collection"""
        # Run simulation
        for step in range(10):
            self.brain.step(step)
        
        # Check statistics
        stats = {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'step_count': self.brain.getStepCount()
        }
        
        self.assertGreaterEqual(stats['total_spikes'], 0)
        self.assertGreaterEqual(stats['active_neurons'], 0)
        self.assertGreaterEqual(stats['average_firing_rate'], 0)
        self.assertGreaterEqual(stats['step_count'], 0)

if __name__ == '__main__':
    unittest.main()
```

### 10.2 Debugging Tools

```python
import pynlm as nlm
import json
import time

class NLMDebugger:
    """Debugging tools for NLM"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.debug_log = {
            'steps': [],
            'errors': [],
            'warnings': [],
            'performance': []
        }
    
    def runWithLogging(self, duration=100):
        """Run simulation with detailed logging"""
        print("Running simulation with debugging enabled...")
        
        for step in range(duration):
            # Record step start
            step_start = time.time()
            
            try:
                # Get current state
                percept = self.world.getSensoryPercept()
                brain_state_before = self.getBrainState()
                world_state_before = self.getWorldState()
                agent_state_before = self.getAgentState()
                
                # Run simulation step
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                self.world.update(0.01)
                
                # Get state after step
                brain_state_after = self.getBrainState()
                world_state_after = self.getWorldState()
                agent_state_after = self.getAgentState()
                
                # Check for anomalies
                self.checkForAnomalies(step, brain_state_before, brain_state_after,
                                      world_state_before, world_state_after,
                                      agent_state_before, agent_state_after)
                
                # Record step data
                step_time = time.time() - step_start
                self.debug_log['steps'].append({
                    'step': step,
                    'time': step_time,
                    'brain_change': self.calculateBrainChange(brain_state_before, brain_state_after),
                    'world_change': self.calculateWorldChange(world_state_before, world_state_after),
                    'agent_change': self.calculateAgentChange(agent_state_before, agent_state_after),
                    'action': action
                })
                
                # Log performance
                if step % 10 == 0:
                    performance = {
                        'step': step,
                        'brain_activity': self.getBrainActivity(),
                        'memory_usage': self.getMemoryUsage(),
                        'simulation_speed': 1 / step_time if step_time > 0 else 0
                    }
                    self.debug_log['performance'].append(performance)
                
                # Print debug info
                if step % 20 == 0:
                    self.printDebugInfo(step)
                
            except Exception as e:
                self.debug_log['errors'].append({
                    'step': step,
                    'error': str(e),
                    'time': time.time()
                })
                print(f"Error at step {step}: {e}")
                
                # Try to continue if possible
                try:
                    self.world.update(0.01)
                except:
                    break
        
        print("Simulation completed with debugging")
        return self.debug_log
    
    def getBrainState(self):
        """Get current brain state"""
        return {
            'step': self.brain.getStepCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'memory_usage': self.brain.getMemoryUsage()
        }
    
    def getWorldState(self):
        """Get current world state"""
        return {
            'width': self.world.getWidth(),
            'height': self.world.getHeight(),
            'time': self.world.getSimulationTime(),
            'percept': self.world.getSensoryPercept()
        }
    
    def getAgentState(self):
        """Get current agent state"""
        return {
            'curiosity': self.agent.getCuriosity() if hasattr(self.agent, 'getCuriosity') else 0,
            'reward': self.agent.getCurrentReward() if hasattr(self.agent, 'getCurrentReward') else 0,
            'action_history': self.agent.getActionHistory() if hasattr(self.agent, 'getActionHistory') else []
        }
    
    def checkForAnomalies(self, step, brain_before, brain_after,
                          world_before, world_after, agent_before, agent_after):
        """Check for potential issues"""
        # Check for brain anomalies
        if brain_after['firing_rate'] > 100:  # Excessive firing rate
            self.debug_log['warnings'].append({
                'step': step,
                'type': 'high_firing_rate',
                'value': brain_after['firing_rate']
            })
        
        if brain_after['memory_usage'] > 500:  # Excessive memory usage
            self.debug_log['warnings'].append({
                'step': step,
                'type': 'high_memory_usage',
                'value': brain_after['memory_usage']
            })
        
        # Check for world anomalies
        if world_after['percept'] and np.any(np.isnan(world_after['percept'])):
            self.debug_log['warnings'].append({
                'step': step,
                'type': 'invalid_percept',
                'value': world_after['percept']
            })
        
        # Check for agent anomalies
        if agent_after.get('curiosity', 0) > 1.0:  # Excessive curiosity
            self.debug_log['warnings'].append({
                'step': step,
                'type': 'high_curiosity',
                'value': agent_after['curiosity']
            })
    
    def calculateBrainChange(self, before, after):
        """Calculate changes in brain state"""
        return {
            'spike_change': after['total_spikes'] - before['total_spikes'],
            'activity_change': after['active_neurons'] - before['active_neurons'],
            'firing_rate_change': after['firing_rate'] - before['firing_rate'],
            'memory_change': after['memory_usage'] - before['memory_usage']
        }
    
    def calculateWorldChange(self, before, after):
        """Calculate changes in world state"""
        return {
            'time_change': after['time'] - before['time']
        }
    
    def calculateAgentChange(self, before, after):
        """Calculate changes in agent state"""
        return {
            'curiosity_change': after.get('curiosity', 0) - before.get('curiosity', 0),
            'reward_change': after.get('reward', 0) - before.get('reward', 0)
        }
    
    def getBrainActivity(self):
        """Get brain activity metrics"""
        return {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_percentage': (self.brain.getActiveNeuronCount() / 
                                self.brain.getTotalNeuronCount()) * 100
        }
    
    def getMemoryUsage(self):
        """Get memory usage metrics"""
        state = self.brain.getMemoryUsage()
        return state.get('memory_mb', 0)
    
    def printDebugInfo(self, step):
        """Print current debug information"""
        current_step = self.debug_log['steps'][-1]
        
        print(f"\nStep {step} Debug Info:")
        print(f"  Time per step: {current_step['time']*1000:.1f} ms")
        print(f"  Brain activity: {current_step['brain_change']}")
        print(f"  Memory usage: {self.getMemoryUsage():.1f} MB")
        
        # Print any warnings for this step
        step_warnings = [w for w in self.debug_log['warnings'] if w['step'] == step]
        if step_warnings:
            print(f"  Warnings: {len(step_warnings)}")
            for warning in step_warnings:
                print(f"    - {warning['type']}: {warning['value']}")
    
    def analyzeDebugLog(self):
        """Analyze debug log for issues"""
        print("\nDebug Log Analysis:")
        print("=" * 60)
        
        if not self.debug_log['steps']:
            print("No simulation steps recorded")
            return
        
        # Calculate statistics
        total_steps = len(self.debug_log['steps'])
        avg_step_time = np.mean([s['time'] for s in self.debug_log['steps']])
        total_brain_activity = sum(abs(s['brain_change']['spike_change']) for s in self.debug_log['steps'])
        
        print(f"Total simulation steps: {total_steps}")
        print(f"Average step time: {avg_step_time*1000:.1f} ms")
        print(f"Total brain activity: {total_brain_activity}")
        
        # Check for errors
        if self.debug_log['errors']:
            print(f"\nErrors encountered: {len(self.debug_log['errors'])}")
            for error in self.debug_log['errors'][:5]:  # Show first 5 errors
                print(f"  - Step {error['step']}: {error['error']}")
            
            if len(self.debug_log['errors']) > 5:
                print(f"  ... and {len(self.debug_log['errors']) - 5} more")
        
        # Check for warnings
        if self.debug_log['warnings']:
            print(f"\nWarnings encountered: {len(self.debug_log['warnings'])}")
            
            # Group warnings by type
            warning_types = {}
            for warning in self.debug_log['warnings']:
                wtype = warning['type']
                if wtype not in warning_types:
                    warning_types[wtype] = []
                warning_types[wtype].append(warning)
            
            for wtype, warnings in warning_types.items():
                print(f"  {wtype}: {len(warnings)} occurrences")
        
        # Performance analysis
        if self.debug_log['performance']:
            print(f"\nPerformance Analysis:")
            perf_data = self.debug_log['performance']
            
            avg_fps = np.mean([p['simulation_speed'] for p in perf_data])
            max_fps = np.max([p['simulation_speed'] for p in perf_data])
            avg_memory = np.mean([p['memory_usage'] for p in perf_data])
            
            print(f"  Average FPS: {avg_fps:.1f}")
            print(f"  Maximum FPS: {max_fps:.1f}")
            print(f"  Average memory: {avg_memory:.1f} MB")
        
        # Recommendations
        print(f"\nRecommendations:")
        if avg_step_time > 0.1:  # Steps taking more than 100ms
            print("  - Simulation steps are slow - consider optimization")
        
        if total_brain_activity > 10000:  # High activity
            print("  - Brain activity is high - may need tuning")
        
        if len(self.debug_log['errors']) > 0:
            print("  - Simulation errors detected - review error logs")
        
        if len(self.debug_log['warnings']) > 0:
            print("  - Warnings detected - review system parameters")
```

## B. Index

This document provides comprehensive coverage of NLM (熙然) advanced features, including:

### Multi-Agent Systems
- Multi-agent simulation setup and coordination
- Competitive agent scenarios
- Agent communication and cooperation

### Learning and Plasticity
- Hebbian vs STDP learning comparisons
- Reward-modulated plasticity
- Eligibility trace mechanisms

### Memory Systems
- Episodic memory replay and consolidation
- Working memory capacity limits
- Concept formation and abstraction
- Memory integration with neural processing

### Neuromodulation
- Dopamine and curiosity integration
- Novelty detection and exploration
- Prediction error signals
- Neuromodulation in learning

### Development and Maturation
- Developmental plasticity
- Structural plasticity
- Critical periods and maturation
- Age-dependent learning rates

### Custom Architectures
- Modular brain construction
- Hybrid learning systems
- Multi-plasticity mechanisms
- Custom neural populations

### Integration Examples
- Complete integration workflows
- Memory system integration
- Neuromodulation integration
- Advanced simulation scenarios

### Command-Line Interface
- Basic CLI usage
- Configuration file examples
- Batch processing scripts
- Python script integration

### Performance Optimization
- Memory-efficient simulations
- Parallel processing
- Memory monitoring and garbage collection
- Performance benchmarking

### Testing and Debugging
- Unit testing framework
- Integration testing strategies
- Debugging tools and logging
- Error analysis and troubleshooting

## C. Related Documents

This documentation complements the following NLM documents:

- **README.md** - Overview of NLM project and basic usage
- **docs/ARCHITECTURE.md** - Core NLM architecture documentation
- **docs/PHASE6_ARCHITECTURE.md** - Phase 6 final integration architecture
- **python/EnhancedBindingsDocumentation.md** - Enhanced Python API documentation
- **docs/DEVELOPER_INTEGRATION.md** - Developer integration guide (see separate document)
- **docs/CONFIGURATION_EXAMPLES.md** - Configuration examples (see separate document)
- **docs/PHASE6_EXAMPLES.md** - Phase 6 integration examples (see separate document)
- **docs/CLI_GUIDE.md** - Command-line interface guide (see separate document)

## D. Version Information

- **Version**: 1.0
- **Phase**: Phase 6 Integration
- **Date**: October 29, 2026
- **Status**: Complete
- **Author**: NLM Development Team

This document is part of the NLM project's comprehensive documentation suite, providing advanced users with detailed examples and best practices for NLM usage.
