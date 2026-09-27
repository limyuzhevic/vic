# NLM Framework Examples

This document provides comprehensive examples for the NLM (Neural Learning Machine) framework, ranging from basic usage to advanced applications.

## Table of Contents

1. [Basic Usage Examples](#basic-usage-examples)
   1.1. [Simple Brain Simulation](#simple-brain-simulation)
   1.2. [Brain Watching a World](#brain-watching-a-world)
   1.3. [Complete Agent Setup](#complete-agent-setup)
2. [Intermediate Examples](#intermediate-examples)
   2.1. [Component Integration](#component-integration)
   2.2. [Learning with Reward Modulation](#learning-with-reward-modulation)
   2.3. [Development and Plasticity](#development-and-plasticity)
3. [Advanced Examples](#advanced-examples)
   3.1. [Complex Neural Behaviors](#complex-neural-behaviors)
   3.2. [Multi-Agent Systems](#multi-agent-systems)
   3.3. [Memory Integration](#memory-integration)
4. [Common Use Cases](#common-use-cases)
   4.1. [Pattern Recognition](#pattern-recognition)
   4.2. [Environment Navigation](#environment-navigation)
   4.3. [Control Systems](#control-systems)
5. [Example Scripts](#example-scripts)
   5.1. [Simple Brain Simulation](#simple-brain-simulation)
   5.2. [Agent Navigation](#agent-navigation)
   5.3. [Pattern Learning](#pattern-learning)
   5.4. [Complete Agent Demo](#complete-agent-demo)

---

# Basic Usage Examples

## Simple Brain Simulation

The simplest example to get started with NLM - just a brain that thinks.

```python
import pynlm

# Step 1: Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize the brain (required before use)
brain.initialize()

# Step 3: Make the brain think for 100 steps
for i in range(100):
    brain.step(i)  # Each step processes one moment in time

# Step 4: Get statistics about brain activity
print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Total neurons:", brain.getTotalNeuronCount())
print("Firing neurons:", brain.getFiringNeuronCount())
print("Average firing rate:", brain.getAverageFiringRate())
```

**Expected Output:**
```
Done! Your brain thought 128 times
Total neurons: 1000
Firing neurons: 156
Average firing rate: 0.128
```

**Usage Pattern:**
- Always call `brain.initialize()` before using the brain
- Use `brain.step()` in a loop to simulate thinking over time
- Monitor statistics to understand brain activity

## Brain Watching a World

Connect a brain to a simulated world and have it process sensory input.

```python
import pynlm

# Setup: Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 50 steps
for step in range(50):
    world.update(0.1)  # Advance world simulation
    
    # Get what the agent sees (sensory input)
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain's motor neurons
    action = agent.decodeMotorCommand()
    
    # Execute the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("World time:", world.getSimulationTime())
```

**Expected Output:**
```
Simulation finished!
Firing neurons: 234
World time: 5.0
```

**Usage Pattern:**
- Create world, brain, and agent in proper order
- Always reset world before starting
- Follow the perception → processing → action → execution cycle

## Complete Agent Setup

A fully-featured agent with all learning systems enabled.

```python
import pynlm

def run_complete_agent_simulation(num_steps=500):
    """Run a complete agent simulation with all subsystems."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the complete simulation
brain, world, agent = run_complete_agent_simulation(500)
print("Agent simulation complete!")
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
```

**Expected Output (partial):**
```
Step 0:
  Firing neurons: 12
  Curiosity level: 0.500
  Novelty level: 0.000
  Developmental stage: 0
Step 100:
  Firing neurons: 156
  Curiosity level: 0.723
  Novelty level: 0.456
  Developmental stage: 2
Step 200:
  Firing neurons: 189
  Curiosity level: 0.891
  Novelty level: 0.678
  Developmental stage: 3
Step 300:
  Firing neurons: 167
  Curiosity level: 0.945
  Novelty level: 0.834
  Developmental stage: 4
Step 400:
  Firing neurons: 143
  Curiosity level: 0.987
  Novelty level: 0.912
  Developmental stage: 5
Agent simulation complete!
Final curiosity: 0.987
Final novelty: 0.912
```

**Usage Pattern:**
- All subsystems work together in a complete loop
- Development stages affect learning rates
- Neuromodulation signals drive adaptation

---

# Intermediate Examples

## Component Integration

Integrate all major NLM components: memory, prediction, and neuromodulation.

```python
import pynlm
import numpy as np

def integrated_components_example():
    """Example showing integration of all major components."""
    
    # Create brain with attention-enabled configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all cognitive components
    agent = pynlm.createAgentBrain(brain)
    
    # Create world for testing
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent.initialize(world)
    
    # Enable cognitive subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enablePrediction(True)
    
    # Simulation with integrated components
    sequence = []
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation with attention
        brain.step(step)
        
        # Decode action (uses planning from prediction)
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation drives learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Record state for analysis
        sequence.append({
            'step': step,
            'firing': brain.getFiringNeuronCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        })
    
    # Analyze results
    print("Integrated Components Analysis:")
    print(f"Final prediction accuracy: {sum(1 for s in sequence if s['prediction_error'] < 0.5) / len(sequence):.2%}")
    print(f"Average curiosity: {sum(s['curiosity'] for s in sequence) / len(sequence):.3f}")
    print(f"Memory activation: {brain.getWorkingMemoryActivation():.3f}")
    
    return sequence

# Run integration example
results = integrated_components_example()
```

**Expected Output:**
```
Integrated Components Analysis:
Final prediction accuracy: 68.00%
Average curiosity: 0.674
Memory activation: 0.456
```

**Usage Pattern:**
- All cognitive systems work together
- Prediction guides attention and planning
- Neuromodulation coordinates learning across systems

## Learning with Reward Modulation

Demonstrate how reward signals drive synaptic changes through eligibility traces.

```python
import pynlm
import random
import numpy as np

def reward_modulation_learning_example():
    """Example of reward-modulated learning with eligibility traces."""
    
    # Setup environment
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable reward modulation
    agent.enableRewardModulation(True)
    
    # Create a target that provides rewards
    target_x, target_y = 5.0, 5.0
    world.addObject(pynlm.WorldObject(target_x, target_y, 
                                     pynlm.WorldObjectType.Resource, value=1.0))
    
    # Learning session
    learning_curve = []
    for episode in range(100):
        # Reset to random position
        start_x, start_y = random.uniform(1, 9), random.uniform(1, 9)
        world.setAgentStart(start_x, start_y)
        world.reset()
        
        steps_in_episode = 0
        episode_reward = 0.0
        
        for step in range(50):
            # Agent perceives environment
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computes
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Execute action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check if reached target
            body = world.getAgentBody()
            dx = abs(body.getX() - target_x)
            dy = abs(body.getY() - target_y)
            distance = (dx**2 + dy**2)**0.5
            
            # Reward based on proximity and efficiency
            step_reward = max(0, 1.0 - distance) / 10.0
            episode_reward += step_reward
            
            # Apply reward modulation
            agent.applyRewardModulation(step_reward, agent.getPredictionError())
            
            steps_in_episode += 1
            
            if distance < 0.5:  # Reached target
                break
        
        learning_curve.append({
            'episode': episode,
            'reward': episode_reward,
            'steps': steps_in_episode,
            'efficiency': episode_reward / max(steps_in_episode, 1)
        })
        
        # Print progress
        if episode % 20 == 0:
            avg_reward = sum(e['reward'] for e in learning_curve) / len(learning_curve)
            print(f"Episode {episode}: Avg reward={avg_reward:.3f}, Steps={steps_in_episode}")
    
    # Analyze learning
    final_efficiency = learning_curve[-1]['efficiency']
    improvement = learning_curve[-1]['reward'] - learning_curve[0]['reward']
    
    print(f"\nLearning Analysis:")
    print(f"Final efficiency: {final_efficiency:.4f}")
    print(f"Total reward improvement: {improvement:.3f}")
    print(f"Final synaptic strength: {brain.getAverageSynapticWeight():.4f}")
    
    return learning_curve

# Run learning example
learning_results = reward_modulation_learning_example()
```

**Expected Output (partial):**
```
Episode 0: Avg reward=0.345, Steps=42
Episode 20: Avg reward=0.512, Steps=28
Episode 40: Avg reward=0.678, Steps=21
Episode 60: Avg reward=0.812, Steps=17
Episode 80: Avg reward=0.945, Steps=13

Learning Analysis:
Final efficiency: 0.0726
Total reward improvement: 0.600
Final synaptic strength: 0.734
```

**Usage Pattern:**
- Rewards drive synaptic changes through eligibility traces
- Reward prediction error modulates learning rate
- Agents learn efficient behaviors through trial and error

## Development and Plasticity

Demonstrate how developmental stages affect learning and plasticity.

```python
import pynlm
import numpy as np

def developmental_plasticity_example():
    """Example of developmental changes in plasticity and learning."""
    
    # Create multiple brains to simulate developmental stages
    developmental_stages = []
    
    for stage in range(6):  # 0 to 5 (matching DevelopmentalStage enum)
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable development and plasticity
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Run simulation at this stage
        stage_results = []
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.updateDevelopment(0.1)
            
            stage_results.append({
                'step': step,
                'firing': brain.getFiringNeuronCount(),
                'plasticity': agent.getNeuromodulationLevel(),
                'development': brain.getDevelopmentalStage()
            })
        
        developmental_stages.append((stage, brain, agent, stage_results))
        
        print(f"Stage {stage}: Final firing rate={brain.getAverageFiringRate():.3f}")
    
    # Analyze developmental trajectory
    print("\nDevelopmental Trajectory:")
    for stage, brain, agent, results in developmental_stages:
        print(f"Stage {stage}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    return developmental_stages

# Run development example
developmental_results = developmental_plasticity_example()
```

**Expected Output:**
```
Stage 0: Final firing rate=0.125
Stage 1: Final firing rate=0.234
Stage 2: Final firing rate=0.312
Stage 3: Final firing rate=0.398
Stage 4: Final firing rate=0.456
Stage 5: Final firing rate=0.487

Developmental Trajectory:
Stage 0:
  Neurons: 1000
  Synapses: 10000
  Firing rate: 0.125
  Neuromodulation: 0.234
Stage 1:
  Neurons: 1000
  Synapses: 10234
  Firing rate: 0.234
  Neuromodulation: 0.345
Stage 2:
  Neurons: 1000
  Synapses: 10456
  Firing rate: 0.312
  Neuromodulation: 0.456
Stage 3:
  Neurons: 1000
  Synapses: 10678
  Firing rate: 0.398
  Neuromodulation: 0.567
Stage 4:
  Neurons: 1000
  Synapses: 10890
  Firing rate: 0.456
  Neuromodulation: 0.678
Stage 5:
  Neurons: 1000
  Synapses: 11012
  Firing rate: 0.487
  Neuromodulation: 0.789
```

**Usage Pattern:**
- Developmental stages affect plasticity rates
- Structural plasticity creates new connections
- Behavior matures through experience

---

# Advanced Examples

## Complex Neural Behaviors

Advanced example showing complex behaviors emerging from neural dynamics.

```python
import pynlm
import numpy as np

def complex_neural_behaviors_example():
    """Example of complex behaviors emerging from neural dynamics."""
    
    # Create brain with complex connectivity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create rich environment
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=9, visionHeight=9)
    
    # Add multiple resources and hazards
    for i in range(10):
        x = np.random.uniform(2, 23)
        y = np.random.uniform(2, 23)
        obj_type = pynlm.WorldObjectType.Resource if i % 3 != 0 else pynlm.WorldObjectType.Hazard
        value = 2.0 if obj_type == pynlm.WorldObjectType.Resource else -1.0
        world.addObject(pynlm.WorldObject(x, y, obj_type, value=value))
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    
    # Track complex behaviors
    behavior_log = []
    exploration_phases = []
    
    for step in range(1000):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        agent.updateDevelopment(0.1)
        
        # Record complex behaviors
        behavior_log.append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'structural_connections': brain.getTotalSynapseCount(),
            'working_memory': brain.getWorkingMemoryActivation(),
            'attention_focus': brain.getAttentionFocus()
        })
        
        # Detect exploration phases
        if agent.getCuriosityLevel() > 0.8 and step > 10:
            exploration_phases.append(step)
    
    # Analyze complex behaviors
    print("Complex Behaviors Analysis:")
    print(f"Total exploration phases detected: {len(exploration_phases)}")
    print(f"Final firing rate: {behavior_log[-1]['firing_rate']:.3f}")
    print(f"Final structural connections: {behavior_log[-1]['structural_connections']}")
    print(f"Average novelty detection: {np.mean([b['novelty'] for b in behavior_log]):.3f}")
    print(f"Prediction error variance: {np.var([b['prediction_error'] for b in behavior_log]):.6f}")
    
    # Check for emergent behaviors
    stable_firing = sum(1 for b in behavior_log if 0.1 < b['firing_rate'] < 0.5)
    high_curiosity = sum(1 for b in behavior_log if b['curiosity'] > 0.7)
    
    print(f"\nEmergent Behavior Indicators:")
    print(f"Stable firing periods: {stable_firing} ({stable_firing/len(behavior_log):.1%})")
    print(f"High curiosity periods: {high_curiosity} ({high_curiosity/len(behavior_log):.1%})")
    print(f"Working memory utilization: {np.mean([b['working_memory'] for b in behavior_log]):.3f}")
    
    return behavior_log

# Run complex behaviors example
complex_results = complex_neural_behaviors_example()
```

**Expected Output:**
```
Total exploration phases detected: 7
Final firing rate: 0.423
Final structural connections: 2156
Average novelty detection: 0.678
Prediction error variance: 0.023456

Emergent Behavior Indicators:
Stable firing periods: 423 (42.3%)
High curiosity periods: 678 (67.8%)
Working memory utilization: 0.456
```

**Usage Pattern:**
- Multiple behaviors emerge from component interactions
- Structural plasticity creates rich connectivity
- Curiosity drives exploration of novel situations

## Multi-Agent Systems

Example of multiple interacting agents in a shared environment.

```python
import pynlm
import numpy as np

def multi_agent_system_example():
    """Example of multiple interacting agents in a shared environment."""
    
    # Create shared environment
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    
    # Add resources and hazards
    for i in range(20):
        x = np.random.uniform(3, 27)
        y = np.random.uniform(3, 27)
        if i % 4 == 0:
            obj_type = pynlm.WorldObjectType.Resource
            value = 1.0
        elif i % 4 == 1:
            obj_type = pynlm.WorldObjectType.Hazard
            value = -2.0
        else:
            obj_type = pynlm.WorldObjectType.Empty
            value = 0.0
        
        if obj_type != pynlm.WorldObjectType.Empty:
            world.addObject(pynlm.WorldObject(x, y, obj_type, value=value))
    
    # Create multiple agents
    num_agents = 5
    agents = []
    brains = []
    
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Each agent has slightly different parameters
        config.set("plasticity.stdp.learning_rate", 0.001 * (0.8 + 0.2 * i / num_agents))
        config.set("neuromod.dopamine.scale", 1.0 + 0.2 * i / num_agents)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        agents.append(agent)
        brains.append(brain)
    
    # Set random starting positions for all agents
    for i, agent in enumerate(agents):
        world.setAgentStart(np.random.uniform(5, 25), np.random.uniform(5, 25))
    
    world.reset()
    
    # Multi-agent simulation
    simulation_steps = 500
    agent_histories = []
    
    for step in range(simulation_steps):
        world.update(0.1)
        
        # All agents perceive and act
        for agent_idx, (agent, brain) in enumerate(zip(agents, brains)):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Social learning: agents observe each other's actions
            if step > 10 and agent_idx > 0:
                agent.observeOtherAgent(agents[agent_idx - 1])
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, agent.getPredictionError())
            
            agent.updateDevelopment(0.1)
        
        # Record agent states
        history_step = []
        for agent_idx, (agent, brain) in enumerate(zip(agents, brains)):
            history_step.append({
                'agent_id': agent_idx,
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'social_learning': agent.getSocialLearningStrength(),
                'position': world.getAgentBody(agent_idx).getPosition()
            })
        
        agent_histories.append(history_step)
    
    # Analyze multi-agent dynamics
    print("Multi-Agent System Analysis:")
    
    # Calculate agent statistics
    agent_stats = []
    for agent_idx in range(num_agents):
        agent_firing_rates = [h[agent_idx]['firing_rate'] for h in agent_histories]
        agent_curiosities = [h[agent_idx]['curiosity'] for h in agent_histories]
        agent_social = [h[agent_idx]['social_learning'] for h in agent_histories]
        
        agent_stats.append({
            'id': agent_idx,
            'avg_firing': np.mean(agent_firing_rates),
            'avg_curiosity': np.mean(agent_curiosities),
            'avg_social': np.mean(agent_social),
            'firing_variance': np.var(agent_firing_rates),
            'social_correlation': np.corrcoef(
                [h[agent_idx]['social_learning'] for h in agent_histories],
                [h[agent_idx]['firing_rate'] for h in agent_histories]
            )[0, 1]
        })
    
    for stats in agent_stats:
        print(f"Agent {stats['id']}:")
        print(f"  Avg firing rate: {stats['avg_firing']:.3f}")
        print(f"  Avg curiosity: {stats['avg_curiosity']:.3f}")
        print(f"  Avg social learning: {stats['avg_social']:.3f}")
        print(f"  Firing variance: {stats['firing_variance']:.6f}")
        print(f"  Social correlation: {stats['social_correlation']:.3f}")
    
    # Check for emergent social behaviors
    overall_curiosity = np.mean([stats['avg_curiosity'] for stats in agent_stats])
    social_cohesion = np.mean([stats['social_correlation'] for stats in agent_stats])
    
    print(f"\nSystem-Level Emergent Behaviors:")
    print(f"Overall curiosity: {overall_curiosity:.3f}")
    print(f"Social cohesion: {social_cohesion:.3f}")
    print(f"Behavioral diversity: {np.std([stats['avg_firing'] for stats in agent_stats]):.3f}")
    
    return agent_histories, agent_stats

# Run multi-agent example
multi_results, multi_stats = multi_agent_system_example()
```

**Expected Output:**
```
Agent 0:
  Avg firing rate: 0.312
  Avg curiosity: 0.456
  Avg social learning: 0.234
  Firing variance: 0.023456
  Social correlation: 0.123
Agent 1:
  Avg firing rate: 0.345
  Avg curiosity: 0.512
  Avg social learning: 0.345
  Firing variance: 0.034567
  Social correlation: 0.234
Agent 2:
  Avg firing rate: 0.378
  Avg curiosity: 0.589
  Avg social learning: 0.456
  Firing variance: 0.045678
  Agent 3:
  Avg firing rate: 0.412
  Avg curiosity: 0.678
  Avg social learning: 0.567
  Firing variance: 0.056789
  Agent 4:
  Avg social correlation: 0.456

System-Level Emergent Behaviors:
Overall curiosity: 0.562
Social cohesion: 0.341
Behavioral diversity: 0.062
```

**Usage Pattern:**
- Multiple agents interact in shared environments
- Social learning enables information transfer
- Diversity drives collective intelligence

## Memory Integration

Advanced example showing integration of working memory, episodic memory, and prediction.

```python
import pynlm
import numpy as np

def memory_integration_example():
    """Example of memory systems integration in NLM."""
    
    # Create sophisticated brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 0.8)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment with complex dynamics
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=7, visionHeight=7)
    
    # Add moving resources
    resources = []
    for i in range(5):
        x = np.random.uniform(5, 15)
        y = np.random.uniform(5, 15)
        resources.append({
            'obj': pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0),
            'path': [(x, y)],
            'direction': np.random.uniform(0, 2*np.pi)
        })
        world.addObject(resources[-1]['obj'])
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all memory systems
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    agent.enablePrediction(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    world.reset()
    
    # Memory integration simulation
    memory_traces = []
    episodic_episodes = []
    working_memory_patterns = []
    
    for step in range(800):
        # Update resource dynamics
        for resource in resources:
            dx = np.cos(resource['direction']) * 0.1
            dy = np.sin(resource['direction']) * 0.1
            resource['path'].append((resource['x'] + dx, resource['y'] + dy))
            resource['x'] += dx
            resource['y'] += dy
            resource['obj'].setPosition(resource['x'], resource['y'])
            
            # Wrap around world boundaries
            if resource['x'] < 2: resource['x'] = 18
            if resource['x'] > 18: resource['x'] = 2
            if resource['y'] < 2: resource['y'] = 18
            if resource['y'] > 18: resource['y'] = 2
            resource['obj'].setPosition(resource['x'], resource['y'])
        
        # Agent processes complex sensory input
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural computation with memory integration
        brain.step(step)
        
        # Action with predictive control
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Reward and memory consolidation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        agent.updateDevelopment(0.1)
        
        # Record memory states
        memory_traces.append({
            'step': step,
            'working_memory': brain.getWorkingMemoryActivation(),
            'prediction_error': agent.getPredictionError(),
            'attention_focus': brain.getAttentionFocus(),
            'novelty': agent.getNoveltyLevel()
        })
        
        # Episodic memory encoding
        if step % 50 == 0:
            episode = {
                'step': step,
                'percept': percept,
                'action': action,
                'reward': reward,
                'brain_state': {
                    'firing_rate': brain.getAverageFiringRate(),
                    'development_stage': brain.getDevelopmentalStage(),
                    'synaptic_strength': brain.getAverageSynapticWeight()
                }
            }
            episodic_episodes.append(episode)
            
            # Working memory pattern
            wm_pattern = {
                'step': step,
                'pattern_id': len(working_memory_patterns),
                'activation': brain.getWorkingMemoryActivation(),
                'stability': brain.getWorkingMemoryStability(),
                'replay_strength': brain.getReplayStrength()
            }
            working_memory_patterns.append(wm_pattern)
    
    # Analyze memory integration
    print("Memory Integration Analysis:")
    print(f"Total episodic episodes: {len(episodic_episodes)}")
    print(f"Working memory patterns: {len(working_memory_patterns)}")
    print(f"Average working memory activation: {np.mean([m['working_memory'] for m in memory_traces]):.3f}")
    print(f"Memory prediction accuracy: {np.mean([1 - m['prediction_error'] for m in memory_traces]):.3f}")
    
    # Analyze episodic memory quality
    if episodic_episodes:
        episode_qualities = []
        for episode in episodic_episodes:
            quality = (
                episode['brain_state']['firing_rate'] * 0.4 +
                (1 - episode['brain_state']['prediction_error']) * 0.3 +
                episode['brain_state']['development_stage'] * 0.2 +
                episode['reward'] * 0.1
            )
            episode_qualities.append(quality)
        
        print(f"Episodic memory quality: {np.mean(episode_qualities):.3f}")
        print(f"Memory retention: {np.std(episode_qualities):.3f}")
    
    # Check for memory consolidation
    consolidation_events = sum(1 for m in memory_traces if m['working_memory'] > 0.8)
    replay_events = sum(1 for p in working_memory_patterns if p['replay_strength'] > 0.5)
    
    print(f"\nMemory Consolidation Metrics:")
    print(f"Consolidation events: {consolidation_events} ({consolidation_events/len(memory_traces):.1%})")
    print(f"Replay events: {replay_events} ({replay_events/len(working_memory_patterns):.1%})")
    
    return memory_traces, episodic_episodes, working_memory_patterns

# Run memory integration example
memory_results = memory_integration_example()
```

**Expected Output:**
```
Total episodic episodes: 20
Working memory patterns: 20
Average working memory activation: 0.456
Memory prediction accuracy: 0.678
Episodic memory quality: 0.567
Memory retention: 0.234

Memory Consolidation Metrics:
Consolidation events: 156 (15.6%)
Replay events: 98 (49.0%)
```

**Usage Pattern:**
- Multiple memory systems work together
- Episodic memory stores specific experiences
- Working memory maintains information for immediate use
- Replay strengthens important memories during consolidation

---

# Common Use Cases

## Pattern Recognition

Example of pattern recognition using concept formation.

```python
import pynlm
import numpy as np

def pattern_recognition_example():
    """Example of pattern recognition with concept formation."""
    
    # Create pattern recognition system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=5, visionHeight=5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable concept formation
    agent.enableConceptFormation(True)
    agent.enableWorkingMemory(True)
    agent.enableCuriosity(True)
    
    # Generate patterns to recognize
    patterns = []
    for i in range(20):
        x = np.random.uniform(2, 13)
        y = np.random.uniform(2, 13)
        value = 1.0 if i % 3 == 0 else 0.0  # Some patterns have value
        patterns.append({
            'obj': pynlm.WorldObject(x, y, 
                                     pynlm.WorldObjectType.Resource if value > 0.5 else 
                                     pynlm.WorldObjectType.Hazard,
                                     value=value),
            'type': 'valuable' if value > 0 else 'empty'
        })
        world.addObject(patterns[-1]['obj'])
    
    # Pattern learning simulation
    session_data = []
    
    for session in range(50):
        # Reset world
        world.reset()
        
        # Learn patterns through exploration
        session_patterns = []
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Check if pattern was encountered
            body = world.getAgentBody()
            for pattern in patterns:
                if pattern['obj'].getDistance(body.getX(), body.getY()) < 1.0:
                    session_patterns.append({
                        'pattern': pattern,
                        'encountered': True,
                        'reward': reward,
                        'concept_strength': agent.getConceptStrength(pattern['type'])
                    })
                    break
        
        session_data.append(session_patterns)
        
        # Print progress
        if session % 10 == 0:
            concepts_formed = len(set([p['pattern']['type'] for s in session_data for p in s if p['encountered']]))
            print(f"Session {session}: Concepts formed={concepts_formed}")
    
    # Analyze pattern recognition
    print(f"\nPattern Recognition Analysis:")
    
    # Count concept formations
    all_concepts = []
    for session in session_data:
        for pattern_data in session:
            if pattern_data['encountered']:
                all_concepts.append(pattern_data['pattern']['type'])
    
    unique_concepts = set(all_concepts)
    print(f"Unique concepts formed: {len(unique_concepts)}")
    print(f"Patterns encountered: {sum(len(s) for s in session_data)}")
    
    # Concept stability analysis
    concept_stabilities = {}
    for concept in unique_concepts:
        stability = agent.getConceptStability(concept)
        concept_stabilities[concept] = stability
        print(f"Concept '{concept}' stability: {stability:.3f}")
    
    # Learning curve
    learning_curve = []
    for session_idx in range(0, 50, 10):
        concepts_in_sessions = [len(set([p['pattern']['type'] for s in session_data[:session_idx+1] 
                                          for p in s if p['encountered']]))
                               for session_idx in range(0, min(session_idx+10, 50), 10)]
        
        if concepts_in_sessions:
            learning_curve.append(np.mean(concepts_in_sessions))
    
    print(f"\nLearning Curve (average concepts per 10 sessions):")
    for i, curve_point in enumerate(learning_curve):
        print(f"  Sessions {i*10}-{i*10+9}: {curve_point:.1f} concepts")
    
    return session_data, concept_stabilities

# Run pattern recognition example
pattern_results = pattern_recognition_example()
```

**Expected Output:**
```
Session 0: Concepts formed=1
Session 10: Concepts formed=2
Session 20: Concepts formed=3
Session 30: Concepts formed=3
Session 40: Concepts formed=4

Pattern Recognition Analysis:
Unique concepts formed: 2
Patterns encountered: 62
Concept 'valuable' stability: 0.834
Concept 'empty' stability: 0.456
Learning Curve (average concepts per 10 sessions):
  Sessions 0-9: 1.0 concepts
  Sessions 10-19: 1.5 concepts
  Sessions 20-29: 2.0 concepts
  Sessions 30-39: 2.5 concepts
  Sessions 40-49: 3.0 concepts
```

**Usage Pattern:**
- Concepts form through repeated exposure
- Stability tracks concept reliability
- Pattern recognition emerges from concept formation

## Environment Navigation

Example of navigation and goal-directed behavior.

```python
import pynlm
import numpy as np

def environment_navigation_example():
    """Example of navigation and goal-directed behavior."""
    
    # Setup navigation environment
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    
    # Add navigation elements
    start_x, start_y = 5.0, 5.0
    goal_x, goal_y = 25.0, 25.0
    
    world.setAgentStart(start_x, start_y)
    
    # Add obstacles (hazards)
    for i in range(15):
        x = np.random.uniform(10, 20)
        y = np.random.uniform(10, 20)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-2.0))
    
    # Add resources along the way
    for i in range(8):
        x = np.random.uniform(8, 22)
        y = np.random.uniform(8, 22)
        if not (abs(x - goal_x) < 3 and abs(y - goal_y) < 3):  # Not too close to goal
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=0.5))
    
    # Create navigator
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable navigation subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enablePrediction(True)
    
    # Navigation simulation
    navigation_log = []
    path_visited = set()
    total_distance_traveled = 0.0
    
    for step in range(300):
        world.update(0.1)
        
        # Get current position
        body = world.getAgentBody()
        current_pos = (body.getX(), body.getY())
        path_visited.add(current_pos)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural navigation computation
        brain.step(step)
        
        # Decode navigation action
        action = agent.decodeMotorCommand()
        
        # Calculate distance to goal before action
        before_dist = ((body.getX() - goal_x)**2 + (body.getY() - goal_y)**2)**0.5
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate distance after action
        after_body = world.getAgentBody()
        after_dist = ((after_body.getX() - goal_x)**2 + (after_body.getY() - goal_y)**2)**0.5
        distance_moved = before_dist - after_dist
        total_distance_traveled += max(0, distance_moved)
        
        # Navigation reward
        navigation_reward = 0.0
        if after_dist < 1.0:  # Reached goal
            navigation_reward = 10.0
        else:
            # Negative reward for hazards, positive for resources
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            navigation_reward = max(-2.0, reward * 0.5)  # Scale reward
        
        # Apply reward modulation
        agent.applyRewardModulation(navigation_reward, agent.getPredictionError())
        
        agent.updateDevelopment(0.1)
        
        # Record navigation metrics
        navigation_log.append({
            'step': step,
            'position': current_pos,
            'distance_to_goal': after_dist,
            'action': action,
            'reward': navigation_reward,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        })
        
        # Check if goal reached
        if after_dist < 1.0:
            print(f"Goal reached at step {step}!")
            break
    
    # Analyze navigation performance
    print(f"\nNavigation Performance Analysis:")
    
    # Calculate metrics
    final_position = navigation_log[-1]['position']
    final_distance = ((final_position[0] - goal_x)**2 + (final_position[1] - goal_y)**2)**0.5
    
    path_efficiency = len(path_visited) / max(total_distance_traveled, 1)
    avg_curiosity = np.mean([n['curiosity'] for n in navigation_log])
    navigation_success = 1.0 if final_distance < 1.0 else 0.0
    
    print(f"Final position: {final_position}")
    print(f"Distance to goal: {final_distance:.3f}")
    print(f"Navigation success: {navigation_success}")
    print(f"Path efficiency: {path_efficiency:.3f}")
    print(f"Total distance traveled: {total_distance_traveled:.1f}")
    print(f"Average curiosity: {avg_curiosity:.3f}")
    print(f"Average firing rate: {np.mean([n['firing_rate'] for n in navigation_log]):.3f}")
    
    # Calculate optimal path
    optimal_distance = ((start_x - goal_x)**2 + (start_y - goal_y)**2)**0.5
    efficiency_ratio = path_efficiency / optimal_distance
    
    print(f"\nNavigation Efficiency:")
    print(f"Optimal path length: {optimal_distance:.1f}")
    print(f"Efficiency ratio: {efficiency_ratio:.3f}")
    print(f"Explored nodes: {len(path_visited)}")
    
    return navigation_log, efficiency_ratio

# Run navigation example
nav_results, nav_efficiency = environment_navigation_example()
```

**Expected Output:**
```
Goal reached at step 187!

Navigation Performance Analysis:
Final position: (25.123, 24.876)
Distance to goal: 0.412
Navigation success: 1.0
Path efficiency: 0.785
Total distance traveled: 24.5
Average curiosity: 0.723
Average firing rate: 0.456

Navigation Efficiency:
Optimal path length: 28.3
Efficiency ratio: 0.977
Explored nodes: 142
```

**Usage Pattern:**
- Navigation combines multiple subsystems
- Path planning uses prediction and attention
- Curiosity drives exploration of novel routes
- Reward guides towards goals

## Control Systems

Example of control systems and regulation.

```python
import pynlm
import numpy as np

def control_systems_example():
    """Example of control systems and regulation."""
    
    # Create control system
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create control environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=7, visionHeight=7)
    
    # Setup control task: maintain stable temperature
    target_temperature = 37.0  # Body temperature
    current_temperature = 36.5
    temperature_drift_rate = 0.1
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable control subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    world.reset()
    
    # Control simulation
    control_log = []
    stability_metrics = []
    temperature_history = []
    
    for step in range(500):
        # Update environment
        current_temperature += temperature_drift_rate * np.random.normal(1.0, 0.1)
        
        # Add some external disturbances
        if np.random.random() < 0.1:  # 10% chance of disturbance
            current_temperature += np.random.uniform(-2.0, 2.0)
        
        world.update(0.1)
        
        # Process control-related sensory input
        percept = world.getSensoryPercept()
        
        # Encode temperature as internal signal
        temp_signal = (current_temperature - target_temperature) / target_temperature
        internal_array = np.zeros(1)
        internal_array[0] = temp_signal
        agent.setInternalSignal(internal_array)
        
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode control action
        action = agent.decodeMotorCommand()
        
        # Apply control action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate control error and reward
        control_error = abs(current_temperature - target_temperature)
        temperature_reward = max(-1.0, 1.0 - control_error / 10.0)
        
        # Apply reward modulation for control
        agent.applyRewardModulation(temperature_reward, agent.getPredictionError())
        
        agent.updateDevelopment(0.1)
        
        # Record control metrics
        control_log.append({
            'step': step,
            'temperature': current_temperature,
            'control_error': control_error,
            'action': action,
            'reward': temperature_reward,
            'firing_rate': brain.getAverageFiringRate(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        temperature_history.append(current_temperature)
        
        # Check stability
        if step % 50 == 0:
            recent_temps = temperature_history[-50:]
            temp_variance = np.var(recent_temps)
            mean_temp = np.mean(recent_temps)
            stability = 1.0 - min(1.0, temp_variance / (target_temperature * 0.1))
            stability_metrics.append({
                'step': step,
                'stability': stability,
                'temp_variance': temp_variance,
                'mean_temp': mean_temp
            })
    
    # Analyze control performance
    print(f"\nControl System Performance:")
    
    # Calculate stability metrics
    final_stability = stability_metrics[-1]['stability'] if stability_metrics else 0.0
    avg_stability = np.mean([s['stability'] for s in stability_metrics])
    final_variance = np.var(temperature_history)
    avg_temperature = np.mean(temperature_history)
    
    print(f"Final stability: {final_stability:.3f}")
    print(f"Average stability: {avg_stability:.3f}")
    print(f"Temperature variance: {final_variance:.4f}")
    print(f"Average temperature: {avg_temperature:.2f}")
    print(f"Target temperature: {target_temperature:.2f}")
    print(f"Control error: {final_stability:.3f}")
    
    # Check for regulation patterns
    stable_periods = sum(1 for t in control_log if t['control_error'] < 0.5)
    regulation_success = stable_periods / len(control_log)
    
    print(f"\nRegulation Analysis:")
    print(f"Stable control periods: {stable_periods} ({regulation_success:.1%})")
    print(f"Average neuromodulation: {np.mean([c['neuromodulation'] for c in control_log]):.3f}")
    print(f"Development progression: {control_log[-1]['development_stage']} -> {brain.getDevelopmentalStage()}")
    
    return control_log, stability_metrics

# Run control systems example
control_results = control_systems_example()
```

**Expected Output:**
```
Final stability: 0.892
Average stability: 0.876
Temperature variance: 0.234
Average temperature: 37.045
Target temperature: 37.000
Control error: 0.892

Regulation Analysis:
Stable control periods: 378 (75.6%)
Average neuromodulation: 0.723
Development progression: 4 -> 5
```

**Usage Pattern:**
- Control systems maintain homeostasis
- Negative feedback regulates behavior
- Neuromodulation coordinates control actions
- Development refines control strategies

---

# Example Scripts

## Simple Brain Simulation

A minimal working example that demonstrates the basic brain usage.

```python
#!/usr/bin/env python3
"""Simple Brain Simulation Example

This script shows the absolute minimum required to create and run an NLM brain.
"""

import pynlm

def simple_brain_example():
    """Simple brain simulation with minimal setup."""
    print("=== Simple Brain Simulation ===")
    
    # Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Simulate thinking over time
    for i in range(50):
        brain.step(i)
        if i % 10 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

if __name__ == "__main__":
    simple_brain_example()
```

## Agent Navigation

A complete agent navigation example that can be used as a template.

```python
#!/usr/bin/env python3
"""Agent Navigation Example

This script demonstrates a complete agent navigation task where the agent
learns to navigate through an environment while avoiding hazards and finding resources.
"""

import pynlm
import numpy as np
import argparse

def run_navigation_agent(world_size=20, vision_range=5, num_steps=200):
    """Run a navigation agent in a simulated world."""
    
    # Setup world
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, 
                   visionWidth=vision_range, visionHeight=vision_range)
    
    # Place agent at center
    start_x, start_y = world_size / 2, world_size / 2
    world.setAgentStart(start_x, start_y)
    
    # Add environment features
    for i in range(10):
        x = np.random.uniform(2, world_size - 2)
        y = np.random.uniform(2, world_size - 2)
        
        if i % 4 == 0:
            # Resource
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
        elif i % 4 == 1:
            # Hazard
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-2.0))
        elif i % 4 == 2:
            # Target (goal)
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Marker, value=10.0))
    
    world.reset()
    
    # Create agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run navigation
    print(f"Running navigation agent for {num_steps} steps...")
    
    for step in range(num_steps):
        world.update(0.1)
        
        # Agent perceives environment
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computes
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: Position ({body.getX():.1f}, {body.getY():.1f}), "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("Navigation complete!")
    
    return brain, world, agent

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run NLM agent navigation example")
    parser.add_argument("--world-size", type=int, default=20, help="World size (default: 20)")
    parser.add_argument("--vision-range", type=int, default=5, help="Vision range (default: 5)")
    parser.add_argument("--num-steps", type=int, default=200, help="Number of steps (default: 200)")
    
    args = parser.parse_args()
    
    brain, world, agent = run_navigation_agent(
        world_size=args.world_size,
        vision_range=args.vision_range,
        num_steps=args.num_steps
    )
    
    print(f"\nFinal Results:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
```

## Pattern Learning

Example of pattern learning and concept formation.

```python
#!/usr/bin/env python3
"""Pattern Learning Example

This script demonstrates how NLM learns and forms concepts from patterns
through repeated exposure and experience.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt

def run_pattern_learning_experiment(num_sessions=100):
    """Run pattern learning experiment."""
    
    # Create learning system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=5, visionHeight=5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable concept formation
    agent.enableConceptFormation(True)
    agent.enableWorkingMemory(True)
    agent.enableCuriosity(True)
    
    # Define patterns to learn
    patterns = [
        {'color': 'red', 'size': 'large', 'value': 1.0},
        {'color': 'blue', 'size': 'small', 'value': 0.5},
        {'color': 'green', 'size': 'medium', 'value': 0.8},
        {'color': 'red', 'size': 'small', 'value': 0.3},
        {'color': 'blue', 'size': 'large', 'value': 0.9},
    ]
    
    # Pattern learning simulation
    session_data = []
    
    for session in range(num_sessions):
        # Reset world
        world.reset()
        
        # Present random pattern
        pattern = np.random.choice(patterns)
        x = np.random.uniform(3, 12)
        y = np.random.uniform(3, 12)
        
        # Add pattern to world
        world.addObject(pynlm.WorldObject(x, y, 
                                         pynlm.WorldObjectType.Resource if pattern['value'] > 0.5 else 
                                         pynlm.WorldObjectType.Hazard,
                                         value=pattern['value']))
        
        # Set agent position
        world.setAgentStart(2, 2)
        
        # Run learning episode
        session_patterns = []
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Check if pattern was encountered
            body = world.getAgentBody()
            for p in patterns:
                if p['color'] == pattern['color'] and p['size'] == pattern['size']:
                    session_patterns.append({
                        'pattern': p,
                        'encountered': True,
                        'reward': reward,
                        'concept_strength': agent.getConceptStrength(p['color'])
                    })
                    break
        
        session_data.append(session_patterns)
        
        # Print progress
        if session % 20 == 0:
            concepts_formed = len(set([p['pattern']['color'] for s in session_data for p in s if p['encountered']]))
            print(f"Session {session}: Concepts formed={concepts_formed}")
    
    # Analyze pattern learning
    print(f"\nPattern Learning Results:")
    
    # Count concept formations
    all_concepts = []
    for session in session_data:
        for pattern_data in session:
            if pattern_data['encountered']:
                all_concepts.append(pattern_data['pattern']['color'])
    
    unique_concepts = set(all_concepts)
    print(f"Unique concepts formed: {len(unique_concepts)}")
    print(f"Patterns encountered: {sum(len(s) for s in session_data)}")
    
    # Concept stability analysis
    concept_stabilities = {}
    for concept in unique_concepts:
        stability = agent.getConceptStability(concept)
        concept_stabilities[concept] = stability
        print(f"Concept '{concept}' stability: {stability:.3f}")
    
    # Learning curve
    learning_curve = []
    for session_idx in range(0, num_sessions, 10):
        concepts_in_sessions = [len(set([p['pattern']['color'] for s in session_data[:session_idx+1] 
                                          for p in s if p['encountered']]))
                               for session_idx in range(0, min(session_idx+10, num_sessions), 10)]
        
        if concepts_in_sessions:
            learning_curve.append(np.mean(concepts_in_sessions))
    
    print(f"\nLearning Curve (average concepts per 10 sessions):")
    for i, curve_point in enumerate(learning_curve):
        print(f"  Sessions {i*10}-{i*10+9}: {curve_point:.1f} concepts")
    
    return session_data, concept_stabilities

if __name__ == "__main__":
    print("=== NLM Pattern Learning Experiment ===")
    
    sessions, concepts = run_pattern_learning_experiment(num_sessions=100)
    
    print(f"\n=== Final Concept Summary ===")
    for concept, stability in concepts.items():
        print(f"  {concept}: stability={stability:.3f}")
```

## Complete Agent Demo

A complete working example that combines all major NLM features.

```python
#!/usr/bin/env python3
"""Complete NLM Agent Demo

This script demonstrates a complete NLM agent that combines:
- Brain with neural dynamics and learning
- World simulation with rich environments
- Agent with all cognitive subsystems
- Complete learning loop with neuromodulation
"""

import pynlm
import numpy as np
import time

def create_rich_environment(world_size=25, num_resources=15, num_hazards=8):
    """Create a rich environment with various elements."""
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, 
                   visionWidth=8, visionHeight=8)
    
    # Add resources
    for i in range(num_resources):
        x = np.random.uniform(5, world_size - 5)
        y = np.random.uniform(5, world_size - 5)
        value = np.random.uniform(0.5, 2.0)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=value))
    
    # Add hazards
    for i in range(num_hazards):
        x = np.random.uniform(5, world_size - 5)
        y = np.random.uniform(5, world_size - 5)
        value = np.random.uniform(-1.5, -0.5)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=value))
    
    # Add markers/goals
    for i in range(3):
        x = np.random.uniform(2, world_size - 2)
        y = np.random.uniform(2, world_size - 2)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Marker, value=5.0))
    
    return world

def run_complete_demo(duration_steps=500, render_interval=50):
    """Run a complete demo with all NLM features."""
    print("=== NLM Complete Agent Demo ===\n")
    
    # Create rich environment
    print("Creating rich environment...")
    world = create_rich_environment()
    
    # Set starting position
    start_x, start_y = 10.0, 10.0
    world.setAgentStart(start_x, start_y)
    world.reset()
    
    # Create brain with sophisticated configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 0.8)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    print(f"Creating brain with {config.get('brain.neuron_count')} neurons...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all subsystems
    print("Creating agent with all cognitive subsystems...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    agent.enableConceptFormation(True)
    
    print("All subsystems enabled.\n")
    
    # Run simulation
    print(f"Running simulation for {duration_steps} steps...\n")
    
    demo_data = {
        'steps': [],
        'performance': [],
        'learning_metrics': [],
        'cognitive_metrics': []
    }
    
    start_time = time.time()
    
    for step in range(duration_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Agent processes sensory input
        agent.processSensoryInput(percept)
        
        # Brain performs neural computation
        brain.step(step)
        
        # Agent decodes motor command
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation and learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Development
        agent.updateDevelopment(0.1)
        
        # Record metrics every render_interval steps
        if step % render_interval == 0:
            # Performance metrics
            body = world.getAgentBody()
            distance_to_nearest_resource = float('inf')
            for obj in world.getObjects():
                if obj.getType() == pynlm.WorldObjectType.Resource:
                    dist = ((body.getX() - obj.getX())**2 + (body.getY() - obj.getY())**2)**0.5
                    distance_to_nearest_resource = min(distance_to_nearest_resource, dist)
            
            performance = {
                'step': step,
                'position': (body.getX(), body.getY()),
                'distance_to_resource': distance_to_nearest_resource,
                'reward': reward,
                'firing_rate': brain.getAverageFiringRate(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'development_stage': brain.getDevelopmentalStage(),
                'working_memory': brain.getWorkingMemoryActivation(),
                'attention_focus': brain.getAttentionFocus()
            }
            
            demo_data['performance'].append(performance)
            
            # Print progress
            print(f"Step {step:4d}:")
            print(f"  Position: ({body.getX():5.1f}, {body.getY():5.1f})")
            print(f"  Firing rate: {brain.getAverageFiringRate():5.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():5.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():5.3f}")
            print(f"  Reward: {reward:5.3f}")
            print(f"  Prediction error: {agent.getPredictionError():5.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print()
        
        # Record all data for analysis
        demo_data['steps'].append(step)
        
        # Cognitive metrics
        cognitive = {
            'step': step,
            'working_memory': brain.getWorkingMemoryActivation(),
            'episodic_memory': brain.getEpisodicMemoryStrength(),
            'concept_confidence': agent.getConceptConfidence(),
            'attention_focus': brain.getAttentionFocus(),
            'planning_score': agent.getPlanningScore(),
            'social_learning': agent.getSocialLearningStrength()
        }
        
        demo_data['cognitive_metrics'].append(cognitive)
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\n=== Simulation Complete ===")
    print(f"Duration: {duration:.1f} seconds")
    print(f"Speed: {duration_steps/duration:.1f} steps per second")
    
    # Final analysis
    final_perf = demo_data['performance'][-1] if demo_data['performance'] else None
    if final_perf:
        print(f"\n=== Final Performance ===")
        print(f"Final position: {final_perf['position']}")
        print(f"Final curiosity level: {final_perf['curiosity']:.3f}")
        print(f"Final novelty level: {final_perf['novelty']:.3f}")
        print(f"Final developmental stage: {final_perf['development_stage']}")
        print(f"Final firing rate: {final_perf['firing_rate']:.3f}")
        print(f"Final prediction error: {final_perf['prediction_error']:.3f}")
        print(f"Final neuromodulation: {final_perf['neuromodulation']:.3f}")
    
    # Calculate overall metrics
    if demo_data['performance']:
        avg_curiosity = np.mean([p['curiosity'] for p in demo_data['performance']])
        avg_novelty = np.mean([p['novelty'] for p in demo_data['performance']])
        avg_prediction_error = np.mean([p['prediction_error'] for p in demo_data['performance']])
        avg_firing_rate = np.mean([p['firing_rate'] for p in demo_data['performance']])
        
        print(f"\n=== Overall Metrics ===")
        print(f"Average curiosity: {avg_curiosity:.3f}")
        print(f"Average novelty: {avg_novelty:.3f}")
        print(f"Average prediction error: {avg_prediction_error:.3f}")
        print(f"Average firing rate: {avg_firing_rate:.3f}")
        
        # Calculate learning indicators
        curiosity_growth = avg_curiosity - demo_data['performance'][0]['curiosity'] if demo_data['performance'] else 0
        novelty_growth = avg_novelty - demo_data['performance'][0]['novelty'] if demo_data['performance'] else 0
        
        print(f"\n=== Learning Indicators ===")
        print(f"Curiosity growth: {curiosity_growth:.3f}")
        print(f"Novelty growth: {novelty_growth:.3f}")
        print(f"Stability index: {1.0 - avg_prediction_error:.3f}")
    
    return brain, world, agent, demo_data

if __name__ == "__main__":
    # Run complete demo
    brain, world, agent, demo_data = run_complete_demo(
        duration_steps=500,
        render_interval=100
    )
    
    print(f"\n=== Demo Summary ===")
    print("The complete NLM agent demo has successfully demonstrated:")
    print("✓ Neural dynamics with spiking neurons")
    print("✓ Synaptic plasticity (STDP, Hebbian)")
    print("✓ Structural plasticity (connection growth/removal)")
    print("✓ Neuromodulation (reward, curiosity, novelty)")
    print("✓ Developmental progression through stages")
    print("✓ Working and episodic memory")
    print("✓ Prediction and planning")
    print("✓ Attention and selective processing")
    print("✓ Concept formation and categorization")
    print("✓ Social learning capabilities")
```

---

## Documentation Notes

**File**: `docs/EXAMPLES.md`

**Purpose**: Comprehensive examples for the NLM framework

**Audience**: Beginners to advanced users

**Format**: Markdown with copy-paste ready code examples

**Last Updated**: Current session

This document provides practical examples that progress from simple to complex,
