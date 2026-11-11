# NLM Advanced Usage Guide

This document provides advanced examples and techniques for using the NLM (Neural Learning Machine) framework. For basic usage, see `easy_usage.md` first.

## Advanced Agent Architecture

The NLM agent architecture connects the neural brain to the external world through several key components:

```python
# Component structure:
#   World → SensoryInput → AgentBrain → Brain → Action → World
```

### Key Interfaces

#### 1. SimpleWorld
```python
# World provides the agent with sensory input
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
world.reset()
world.setAgentStart(50.0, 50.0)

# Update world and get perception
world.update(0.1)
percept = world.getSensoryPercept()

# Apply agent's actions back to world
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### 2. Brain
```python
# The brain is the core neural simulation
brain = pynlm.createBrain(config)
brain.initialize()

# Core processing methods:
brain.step(step)           # Neural computation
brain.injectCurrent(neuron, current)  # External input
brain.receiveSensoryInput(percept)   # Process perception
brain.produceAction()       # Generate motor output
brain.reset()               # Reset state
brain.save("state.bin")     # Checkpoint
brain.load("state.bin")     # Restore state
```

#### 3. AgentBrain
```python
# AgentBrain connects brain to world interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Key methods:
agent.processSensoryInput(percept)     # Percept → brain
agent.decodeMotorCommand()              # Brain → action
agent.applyRewardModulation(reward, pred)  # Learning
agent.updateDevelopment(timestep)       # Developmental changes
```

## Advanced Patterns

### Pattern 1: Hierarchical Task Decomposition

```python
def run_hierarchical_task(config, tasks):
    """Run tasks with hierarchical organization."""
    
    # Create brain with increased resources for complex tasks
    config.set("brain.neuron_count", 5000)
    config.set("brain.connection_probability", 0.05)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Create specialized worlds for different task types
    worlds = {}
    for task_name, task_config in tasks.items():
        world = pynlm.createSimpleWorld()
        world.configure(**task_config['world_config'])
        world.reset()
        world.setAgentStart(0.5, 0.5)  # Normalized position
        worlds[task_name] = world
        agent.initialize(world)
    
    # Enable different subsystem combinations per task
    task_subsystems = {
        'exploration': [True, False, True, True],
        'exploitation': [True, True, True, False],
        'learning': [True, True, True, True]
    }
    
    # Run task sequence
    results = {}
    for task_name in tasks:
        subsystems = task_subsystems.get(task_name, [True, True, True, True])
        agent.enableRewardModulation(subsystems[0])
        agent.enableStructuralPlasticity(subsystems[1])
        agent.enableDevelopment(subsystems[2])
        agent.enableCuriosity(subsystems[3])
        
        # Run task with specific parameters
        result = run_single_task(agent, worlds[task_name], tasks[task_name])
        results[task_name] = result
    
    return results

def run_single_task(agent, world, task_config):
    """Run a single task with given configuration."""
    steps = task_config['steps']
    target_reward = task_config['target_reward']
    
    task_reward = 0.0
    task_success = False
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        task_reward += reward
        
        if task_reward >= target_reward:
            task_success = True
            break
        
        agent.updateDevelopment(0.1)
    
    return {
        'reward': task_reward,
        'success': task_success,
        'steps': step + 1,
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel()
    }
```

### Pattern 2: Memory-Enhanced Learning

```python
def run_memory_enhanced_learning(config):
    """Simulate learning with memory integration."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Create world with episodic memory features
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    
    # Configure world for memory research
    world.setMemoryEnabled(True)
    world.setEp episodicRecallEnabled(True)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Memory research protocol
    episodes = 20
    trials_per_episode = 50
    
    memory_results = []
    
    for episode in range(episodes):
        episode_rewards = []
        episode_patterns = []
        
        for trial in range(trials_per_episode):
            # Create distinctive patterns for memory research
            pattern = generate_distinctive_pattern(trial)
            
            # Present pattern as sensory input
            vision = pynlm.Vision(8, 8, 3)
            vision.setData(pattern)
            
            percept = world.getSensoryPercept()
            percept.setVision(vision)
            
            # Run trial
            world.update(0.1)
            agent.processSensoryInput(percept)
            brain.step(trial + episode * trials_per_episode)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            episode_rewards.append(reward)
            episode_patterns.append(pattern.copy())
            
            agent.updateDevelopment(0.1)
        
        # Test memory recall
        recall_accuracy = test_memory_recall(agent, episode_patterns)
        
        memory_results.append({
            'episode': episode,
            'mean_reward': sum(episode_rewards) / len(episode_rewards),
            'reward_variance': calculate_variance(episode_rewards),
            'recall_accuracy': recall_accuracy,
            'final_curiosity': agent.getCuriosityLevel()
        })
        
        print(f"Episode {episode}: Recall accuracy = {recall_accuracy:.2%}")
    
    # Analyze memory consolidation
    analyze_memory_consolidation(memory_results)
    
    return memory_results

def generate_distinctive_pattern(trial_id):
    """Generate a distinctive pattern for memory testing."""
    import random
    pattern = []
    
    # Create spatial patterns
    for i in range(64):  # 8x8 vision grid
        if random.random() < 0.3:
            pattern.append(random.uniform(0.0, 1.0))
        else:
            pattern.append(0.0)
    
    # Add temporal sequence markers
    if trial_id % 5 == 0:
        pattern[0] = 1.0  # Add distinctive marker
    
    return pattern

def test_memory_recall(agent, patterns):
    """Test memory recall accuracy."""
    recall_correct = 0
    total_tests = 0
    
    for i, pattern in enumerate(patterns):
        # Present pattern for recall test
        vision = pynlm.Vision(8, 8, 3)
        vision.setData(pattern)
        
        percept = world.getSensoryPercept()
        percept.setVision(vision)
        
        # Allow agent to process and generate response
        agent.processSensoryInput(percept)
        brain.step(i)
        
        action = agent.decodeMotorCommand()
        
        # Check if response matches pattern
        if check_response_matches_pattern(action, pattern):
            recall_correct += 1
        
        total_tests += 1
    
    return recall_correct / total_tests if total_tests > 0 else 0.0

def calculate_variance(values):
    """Calculate variance of a list of values."""
    if not values:
        return 0.0
    
    mean = sum(values) / len(values)
    squared_diff = sum((x - mean) ** 2 for x in values)
    return squared_diff / len(values)
```

### Pattern 3: Development and Aging Simulation

```python
def run_development_simulation(config, duration_steps=5000):
    """Simulate brain development and aging over time."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Development phases
    development_phases = {
        'initial': {'start': 0, 'end': 1000, 'plasticity': 1.0},
        'critical_period': {'start': 1000, 'end': 2000, 'plasticity': 0.5},
        'maturation': {'start': 2000, 'end': 3000, 'plasticity': 0.3},
        'adult': {'start': 3000, 'end': 5000, 'plasticity': 0.1}
    }
    
    development_results = []
    
    for phase_name, phase_config in development_phases.items():
        phase_start = phase_config['start']
        phase_end = phase_config['end']
        base_plasticity = phase_config['plasticity']
        
        phase_results = run_development_phase(
            agent, brain, world, phase_start, phase_end, base_plasticity
        )
        
        development_results.append({
            'phase': phase_name,
            'start_step': phase_start,
            'end_step': phase_end,
            'final_stage': phase_results['final_stage'],
            'final_plasticity': phase_results['final_plasticity'],
            'performance': phase_results['performance']
        })
        
        print(f"Completed {phase_name} phase")
    
    return development_results

def run_development_phase(agent, brain, world, start_step, end_step, base_plasticity):
    """Run a single development phase."""
    
    phase_results = {
        'stage_history': [],
        'plasticity_history': [],
        'performance_history': []
    }
    
    for step in range(start_step, end_step):
        # Update development system with phase-specific plasticity
        age_factor = (step - start_step) / (end_step - start_step)
        current_plasticity = base_plasticity * (1.0 - age_factor * 0.9)
        
        agent.updateDevelopment(0.1)
        
        # Store phase-specific metrics
        if step % 100 == 0:
            phase_results['stage_history'].append({
                'step': step,
                'stage': brain.getDevelopmentalStage(),
                'plasticity': current_plasticity,
                'performance': calculate_performance(brain, agent)
            })
        
        # Update world
        world.update(0.1)
        
        # Standard agent loop
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    return phase_results

def calculate_performance(brain, agent):
    """Calculate overall performance metrics."""
    firing_rate = brain.getAverageFiringRate()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    
    # Weighted performance score
    performance = (firing_rate * 0.3 + curiosity * 0.25 + novelty * 0.2 + 
                   max(0.0, 1.0 - prediction_error) * 0.25)
    
    return performance
```

### Pattern 4: Multi-Agent Cooperation

```python
def run_multi_agent_cooperation(config, num_agents=3):
    """Simulate multiple cooperating agents."""
    
    # Create shared brain for all agents
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create multiple agent interfaces
    agents = []
    worlds = []
    
    for i in range(num_agents):
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(worlds[i])
        agents.append(agent)
    
    # Configure individual worlds
    for i, world in enumerate(worlds):
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.setAgentStart(i * 10.0, i * 10.0)
        world.reset()
    
    # Enable different subsystem specializations
    subsystem_profiles = [
        [True, False, True, False],   # Agent 1: Learning-focused
        [False, True, True, True],    # Agent 2: Plasticity-focused
        [True, True, False, True]     # Agent 3: Curiosity-focused
    ]
    
    for i, agent in enumerate(agents):
        subsys = subsystem_profiles[i]
        agent.enableRewardModulation(subsys[0])
        agent.enableStructuralPlasticity(subsys[1])
        agent.enableDevelopment(subsys[2])
        agent.enableCuriosity(subsys[3])
    
    # Cooperative learning protocol
    steps = 2000
    cooperation_metrics = {
        'individual_rewards': [[] for _ in range(num_agents)],
        'shared_memory': {},
        'cooperation_bonus': 0.0
    }
    
    for step in range(steps):
        # Each agent observes and acts
        for i, agent in enumerate(agents):
            worlds[i].update(0.1)
            percept = worlds[i].getSensoryPercept()
            
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            worlds[i].applyMotorCommand(action, worlds[i].getSimulationTime())
            
            # Shared memory update
            if i == 0:  # Lead agent shares knowledge
                agent.shareKnowledge(cooperation_metrics['shared_memory'])
            
            # Collect individual reward
            reward = worlds[i].getSensoryPercept().getInternal()[0] if worlds[i].getSensoryPercept().getInternal() else 0.0
            
            # Apply individual reward
            agent.applyRewardModulation(reward, 0.0)
            
            cooperation_metrics['individual_rewards'][i].append(reward)
            
            # Collect cooperation bonus
            agent.updateDevelopment(0.1)
    
    # Analyze cooperation effectiveness
    analysis = analyze_cooperation_effectiveness(cooperation_metrics)
    
    return {
        'agents': agents,
        'worlds': worlds,
        'metrics': cooperation_metrics,
        'analysis': analysis
    }

if __name__ == "__main__":
    # Example usage
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.1)
    
    print("=== NLM Advanced Usage Examples ===")
    print("1. Advanced agent simulation:")
    # result = run_advanced_agent_simulation(config)
    
    print("\n2. Memory-enhanced learning:")
    # memory_results = run_memory_enhanced_learning(config)
    
    print("\n3. Development simulation:")
    # development_results = run_development_simulation(config)
    
    print("\n4. Multi-agent cooperation:")
    # cooperation_result = run_multi_agent_cooperation(config)
    
    print("\nTo run specific examples, uncomment the desired calls above.")
