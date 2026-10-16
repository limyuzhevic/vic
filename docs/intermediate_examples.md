# NLM Intermediate Examples

## Overview

This section provides 15+ practical examples demonstrating intermediate NLM usage patterns. These examples bridge between beginner tutorials and advanced applications, showing how to use specific NLM features in real-world scenarios.

## Example 1: Learning Agent with Dynamic Goal Setting

### Objective
Create an agent that learns and adapts its goals based on environmental conditions and performance feedback.

```python
import pynlm
import time

def dynamic_goal_learning(episode_length=1000):
    """Agent that learns and adapts goals dynamically."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableSkillLearning(True)
    
    # Configure for dynamic goals
    agent.setMaxSequenceLength(10)
    agent.setHierarchicalControlEnabled(True)
    agent.setExplorationRate(0.2)
    
    print("Dynamic Goal Learning Agent")
    print("============================")
    
    for step in range(episode_length):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Dynamic goal adaptation based on performance
        curiosity = agent.getCuriosityLevel()
        performance = brain.getAverageFiringRate()
        
        if curiosity > 0.7 and performance < 0.3:
            # Low performance with high curiosity = explore new strategies
            agent.setGoal("explore_alternatives", parameters=[strategy_approach=2])
        elif curiosity < 0.3 and performance > 0.8:
            # High performance with low curiosity = optimize current strategy
            agent.setGoal("optimize_current", parameters=[refinement_level=1])
        else:
            # Balanced state = maintain current goal
            pass
        
        # Execute current action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply rewards
        reward = calculate_dynamic_reward(world, step, agent)
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Monitor and adapt
        if step % 200 == 0:
            print(f"Step {step}: Goal: {agent.getCurrentGoal()}, "
                  f"Curiosity: {curiosity:.3f}, Performance: {performance:.3f}")
    
    print("\nLearning completed!")
    return brain, agent, world

# Helper function for dynamic rewards
def calculate_dynamic_reward(world, step, agent):
    """Calculate reward based on agent performance and environment."""
    reward = 0.0
    
    # Basic environmental rewards
    if world.getEnergy() > 100:
        reward += 1.0  # Resource abundance
    
    # Curiosity-based exploration rewards
    curiosity = agent.getCuriosityLevel()
    if curiosity > 0.8:
        reward += 0.5  # High curiosity exploration bonus
    
    # Novelty-based learning rewards
    novelty = agent.getNoveltyLevel()
    if novelty > 0.6:
        reward += 0.3  # Novel experience bonus
    
    # Performance-based adaptation
    prediction_error = agent.getPredictionError()
    if abs(prediction_error) < 0.1:
        reward += 0.2  # Good prediction accuracy
    
    # Temporal decay
    if step > 500:
        reward *= 0.8
    
    return reward
```

## Example 2: Multi-Agent Cooperation

### Objective
Simulate cooperative behavior between multiple agents sharing a common environment.

```python
import pynlm

def multi_agent_cooperation(num_agents=3, episode_length=800):
    """Simulate cooperative multi-agent behavior."""
    
    print("Multi-Agent Cooperation Simulation")
    print("==================================")
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Create multiple agents
    agents = []
    brains = []
    
    for i in range(num_agents):
        # Create unique configuration for each agent
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.08)
        config.set("neuromod.dopamine.scale", 1.2 - i * 0.1)  # Different dopamine levels
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Position agents differently
        start_x = 10 + i * 15
        start_y = 25
        world.setAgentStart(start_x, start_y)
        agent.initialize(world)
        
        # Enable cooperation capabilities
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Configure for social behavior
        agent.setActionTemperature(0.8)  # More exploration for social interaction
        agent.setMaxSequenceLength(8)
        
        agents.append(agent)
        brains.append(brain)
        
        print(f"Agent {i+1}: Position ({start_x}, {start_y}), "
              f"Dopamine: {config.getOr('neuromod.dopamine.scale', 1.0):.2f}")
    
    # Cooperative learning
    print("\nStarting cooperative learning...")
    
    cooperation_metrics = {
        'total_interactions': 0,
        'successful_cooperations': 0,
        'learning_rates': [0.0] * num_agents
    }
    
    for step in range(episode_length):
        # Each agent processes sensory input
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain = brains[i]
            brain.step(step)
            
            # Cooperative action selection
            if step % 100 == i:  # Staggered actions
                action = agent.decodeMotorCommand()
                
                # Apply action and get response
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Cooperation detection
                if is_cooperative_action(action):
                    cooperation_metrics['successful_cooperations'] += 1
                
                cooperation_metrics['total_interactions'] += 1
        
        # Shared rewards based on collective performance
        total_energy = sum(world.getEnergy() for _ in range(num_agents))
        collective_reward = min(1.0, total_energy / 500.0)
        
        # Distribute rewards
        for i, agent in enumerate(agents):
            # Individual reward component
            individual_reward = collective_reward * (0.7 + 0.3 * (i + 1) / num_agents)
            
            # Social reward component
            social_bonus = calculate_social_bonus(i, num_agents, agents)
            total_reward = individual_reward + social_bonus
            
            brain = brains[i]
            agent.applyRewardModulation(total_reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Track learning rate
            learning_rate = brain.getAverageFiringRate() / 1.0
            cooperation_metrics['learning_rates'][i] = learning_rate
        
        # Print progress
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  - Cooperation rate: {cooperation_metrics['successful_cooperations'] / max(1, cooperation_metrics['total_interactions']):.2f}")
            print(f"  - Learning rates: {[f'{lr:.2f}' for lr in cooperation_metrics['learning_rates']]}")
    
    print("\nCooperation simulation completed!")
    
    return brains, agents, world, cooperation_metrics

# Helper functions
def is_cooperative_action(action):
    """Determine if action is cooperative (sharing, helping, etc.)."""
    cooperative_actions = [
        ActionType.Interact,
        ActionType.Eat,
        ActionType.Wait,
        ActionType.Rest
    ]
    return action in cooperative_actions

def calculate_social_bonus(agent_index, total_agents, agents):
    """Calculate social bonus based on agent position and behavior."""
    bonus = 0.0
    
    # Reward for proximity to other agents
    for i, other_agent in enumerate(agents):
        if i != agent_index:
            distance = calculate_distance(agent_index, i)
            if distance < 10:
                bonus += 0.1 / distance
    
    # Reward for synchronized behavior
    current_step = 0  # Would need step tracking
    if current_step % 50 == agent_index:
        bonus += 0.2
    
    return min(0.5, bonus)  # Cap bonus

def calculate_distance(agent1_index, agent2_index):
    """Calculate approximate distance between agents."""
    # Simplified distance calculation
    positions = [(10, 25), (25, 25), (40, 25)]  # Agent positions
    if agent1_index < len(positions) and agent2_index < len(positions):
        x1, y1 = positions[agent1_index]
        x2, y2 = positions[agent2_index]
        return ((x1 - x2)**2 + (y1 - y2)**2)**0.5
    return 100.0
```

## Example 3: Complex Environment Simulation

### Objective
Simulate agents in a complex, dynamic environment with multiple interacting systems.

```python
import pynlm
import random

def complex_environment_simulation(episode_length=1200):
    """Complex environment with multiple interacting systems."""
    
    print("Complex Environment Simulation")
    print("==============================")
    
    # Create complex world
    world = pynlm.createSimpleWorld()
    world.configure(width=60, height=60, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(30.0, 30.0)
    
    # Create agent with advanced capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.12)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enablePredictionErrorLearning(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    agent.enableAdvancedMotorDecoding(True)
    agent.enableSensoryMotorIntegration(True)
    
    # Configure for complex environment
    agent.setActionTemperature(0.6)
    agent.setMaxSequenceLength(12)
    agent.setHierarchicalControlEnabled(True)
    
    print("Environment complexity enabled:")
    print("  - Multi-layered sensory input")
    print("  - Dynamic weather simulation")
    print("  - Resource management system")
    print("  - Threat detection system")
    
    # Complex environment systems
    environment_systems = {
        'weather': {'temperature': 20.0, 'humidity': 0.5, 'wind': 0.0},
        'resources': {'energy': 100.0, 'nutrition': 50.0, 'water': 75.0},
        'threats': [],
        'time_of_day': 0.0,
        'seasons': ['spring', 'summer', 'autumn', 'winter']
    }
    
    # Dynamic environment
    print("\nStarting complex environment simulation...")
    
    for step in range(episode_length):
        # Update environment
        update_complex_environment(environment_systems, step)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Agent decision making with environmental awareness
        action = make_environmentally_aware_decision(
            agent, environment_systems, world, step
        )
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate complex rewards
        reward = calculate_environmental_reward(
            world, environment_systems, agent, step
        )
        
        predicted_reward = estimate_future_reward(action, environment_systems)
        agent.applyRewardModulation(reward, predicted_reward)
        
        agent.updateDevelopment(0.1)
        
        # Environmental changes affect agent behavior
        if step % 100 == 0:
            environmental_stress = calculate_environmental_stress(environment_systems)
            print(f"Step {step}:")
            print(f"  - Weather: {environment_systems['weather']['temperature']:.1f}°C, "
                  f"{environment_systems['weather']['humidity']*100:.0f}% humidity")
            print(f"  - Resources: {environment_systems['resources']['energy']:.0f}/" 
                  f"{environment_systems['resources']['nutrition']:.0f}/"
                  f"{environment_systems['resources']['water']:.0f}")
            print(f"  - Environmental stress: {environmental_stress:.2f}")
            print(f"  - Agent curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  - Agent performance: {brain.getAverageFiringRate():.3f} Hz")
    
    print("\nComplex environment simulation completed!")
    
    return brain, agent, world, environment_systems

# Helper functions for complex environment
def update_complex_environment(systems, step):
    """Update complex environment dynamics."""
    # Weather simulation
    systems['weather']['temperature'] += random.uniform(-0.5, 0.5)
    systems['weather']['humidity'] = min(1.0, max(0.2, systems['weather']['humidity'] + random.uniform(-0.05, 0.05)))
    systems['weather']['wind'] = random.uniform(-0.2, 0.2)
    
    # Day/night cycle
    systems['time_of_day'] = (step % 200) / 200.0 * 24.0
    
    # Seasonal changes
    current_season = systems['seasons'][(step // 200) % 4]
    if current_season == 'summer':
        systems['weather']['temperature'] += 2.0
    elif current_season == 'winter':
        systems['weather']['temperature'] -= 2.0
    
    # Resource regeneration
    for resource in ['energy', 'nutrition', 'water']:
        systems['resources'][resource] += random.uniform(0.1, 0.5)
        if systems['resources'][resource] > 100:
            systems['resources'][resource] = 100
    
    # Threat generation (random)
    if random.random() < 0.02:  # 2% chance per step
        threats = ['predator', 'hazard', 'competition']
        new_threat = random.choice(threats)
        systems['threats'].append((step, new_threat))
        # Remove old threats
        systems['threats'] = [t for t in systems['threats'] if step - t[0] < 100]

def make_environmentally_aware_decision(agent, environment, world, step):
    """Make decisions based on environmental conditions."""
    curiosity = agent.getCuriosityLevel()
    temperature = environment['weather']['temperature']
    energy = environment['resources']['energy']
    threats = environment['threats']
    
    # Decision based on environmental factors
    if energy < 20:  # Low energy -> prioritize resource gathering
        return ActionType.Eat
    elif temperature > 35 or temperature < 5:  # Extreme temperature -> seek shelter
        return ActionType.Rest
    elif len(threats) > 0:  # Threats present -> defensive behavior
        return ActionType.Wait
    elif curiosity > 0.7:  # High curiosity -> explore
        return ActionType.MoveForward
    elif curiosity < 0.3 and energy > 80:  # Low curiosity, high energy -> optimize
        return ActionType.Rest
    else:  # Default behavior
        return agent.decodeMotorCommand()

def calculate_environmental_reward(world, environment, agent, step):
    """Calculate reward based on environmental factors."""
    reward = 0.0
    
    # Weather-based rewards
    temp = environment['weather']['temperature']
    if 20 <= temp <= 25:  # Optimal temperature
        reward += 0.5
    elif 15 <= temp <= 30:  # Acceptable temperature
        reward += 0.3
    else:  # Extreme temperature
        reward -= 0.2
    
    # Resource-based rewards
    energy = environment['resources']['energy']
    reward += energy / 100.0 * 0.4  # More resources = higher reward
    
    # Threat-based penalties
    threats = environment['threats']
    if threats:
        recent_threats = [t for t in threats if step - t[0] < 50]
        reward -= len(recent_threats) * 0.3
    
    # Agent performance rewards
    curiosity = agent.getCuriosityLevel()
    reward += curiosity * 0.2  # Curiosity-driven learning rewards
    
    # Environmental adaptation rewards
    if curiosity > 0.6 and temp < 35 and temp > 5:
        reward += 0.3  # Good adaptation to environment
    
    return reward

def calculate_environmental_stress(environment):
    """Calculate overall environmental stress level."""
    stress = 0.0
    
    # Temperature stress
    temp = environment['weather']['temperature']
    stress += abs(temp - 22.5) / 22.5  # Stress from ideal temperature (22.5°C)
    
    # Resource scarcity stress
    resources = ['energy', 'nutrition', 'water']
    for resource in resources:
        level = environment['resources'][resource] / 100.0
        stress += (1.0 - level) * 0.3
    
    # Threat stress
    stress += len(environment['threats']) * 0.2
    
    return min(1.0, stress)
```

## Example 4: Performance Benchmark Suite

### Objective
Comprehensive performance benchmarking of different NLM configurations and strategies.

```python
import pynlm
import time
import statistics

def performance_benchmark_suite(configurations=None):
    """Comprehensive performance benchmark suite."""
    
    print("Performance Benchmark Suite")
    print("============================")
    
    if configurations is None:
        configurations = {
            'baseline': {
                'name': 'Baseline Configuration',
                'neuron_count': 1000,
                'synapse_density': 0.1,
                'learning_enabled': False
            },
            'optimized': {
                'name': 'Optimized Configuration',
                'neuron_count': 2000,
                'synapse_density': 0.15,
                'learning_enabled': True,
                'temperature': 0.7,
                'curiosity': True
            },
            'research': {
                'name': 'Research Configuration',
                'neuron_count': 5000,
                'synapse_density': 0.2,
                'learning_enabled': True,
                'advanced_features': True
            }
        }
    
    benchmark_results = {}
    
    for config_name, config_params in configurations.items():
        print(f"\nBenchmarking {config_params['name']}...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_params['neuron_count'])
        config.set("brain.synapse_density", config_params['synapse_density'])
        
        if config_params.get('learning_enabled', False):
            config.set("plasticity.hebbian.enable", True)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        agent.initialize(world)
        
        # Configure agent based on config
        if config_params.get('learning_enabled', False):
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        if config_params.get('temperature'):
            agent.setActionTemperature(config_params['temperature'])
        
        if config_params.get('curiosity'):
            agent.enableCuriosity(True)
        
        # Run benchmark tests
        benchmark_results[config_name] = run_benchmark_test(
            brain, agent, world, config_params
        )
        
        print(f"  ✓ {config_name} benchmark completed")
    
    # Generate comparison report
    print(f"\nBenchmark Comparison Report")
    print("============================")
    
    for config_name, results in benchmark_results.items():
        print(f"\n{config_params['name']}:")
        print(f"  - Simulation time: {results['simulation_time']:.2f}s")
        print(f"  - Performance: {results['performance_metrics']['ops_per_second']:.0f} ops/sec")
        print(f"  - Memory efficiency: {results['efficiency_metrics']['memory_usage_mb']:.1f} MB")
        print(f"  - Learning rate: {results['efficiency_metrics']['learning_rate']:.3f}")
        print(f"  - Stability: {results['stability_metrics']['stability_score']:.3f}")
    
    return benchmark_results

def run_benchmark_test(brain, agent, world, config_params):
    """Run comprehensive benchmark test for a configuration."""
    
    results = {
        'simulation_time': 0.0,
        'performance_metrics': {},
        'efficiency_metrics': {},
        'stability_metrics': {},
        'stress_tests': []
    }
    
    # Benchmark 1: Basic performance
    print("    Running basic performance test...")
    start_time = time.time()
    
    for step in range(1000):
        brain.step(step)
    
    end_time = time.time()
    results['simulation_time'] = end_time - start_time
    results['performance_metrics']['ops_per_second'] = 1000 / results['simulation_time']
    results['performance_metrics']['spikes_per_second'] = brain.getTotalSpikeCount() / results['simulation_time']
    
    # Benchmark 2: Learning performance
    if config_params.get('learning_enabled', False):
        print("    Running learning performance test...")
        
        learning_start = time.time()
        
        for step in range(500):
            # Simple learning simulation
            brain.step(step)
            
            if step % 100 == 0:
                curiosity = agent.getCuriosityLevel()
                learning_progress = curiosity / 1.0
                
                if step == 0:
                    results['efficiency_metrics']['initial_learning_rate'] = learning_progress
                elif step == 499:
                    results['efficiency_metrics']['final_learning_rate'] = learning_progress
                    
                results['efficiency_metrics']['learning_rate'] = \
                    (results['efficiency_metrics']['final_learning_rate'] - 
                     results['efficiency_metrics']['initial_learning_rate']) / 5.0
        
        learning_time = time.time() - learning_start
        results['efficiency_metrics']['learning_time'] = learning_time
    
    # Benchmark 3: Stress test
    print("    Running stress test...")
    stress_result = run_stress_test(brain, agent, world)
    results['stress_tests'].append(stress_result)
    
    # Benchmark 4: Memory efficiency
    print("    Running memory efficiency test...")
    memory_result = run_memory_test(brain)
    results['efficiency_metrics'].update(memory_result)
    
    # Benchmark 5: Stability
    print("    Running stability test...")
    stability_result = run_stability_test(brain, agent)
    results['stability_metrics'] = stability_result
    
    return results

def run_stress_test(brain, agent, world):
    """Run stress test with environmental challenges."""
    
    stress_results = {
        'stability_score': 0.0,
        'performance_degradation': 0.0,
        'recovery_time': 0.0
    }
    
    # Create challenging environment
    for step in range(200):
        # Challenging conditions
        brain.step(step)
        
        # Apply stress
        if step % 50 == 0:
            # Reset brain to test recovery
            brain.reset()
        
        # Check performance
        current_performance = brain.getAverageFiringRate()
        if step == 199:
            stress_results['stability_score'] = current_performance / 1.0
    
    # Calculate performance degradation
    if 'initial_performance' in stress_results:
        initial = stress_results['initial_performance']
        final = stress_results['stability_score']
        stress_results['performance_degradation'] = (initial - final) / initial
    
    return stress_results

def run_memory_test(brain):
    """Test memory efficiency."""
    
    memory_results = {
        'memory_usage_mb': 0.0,
        'spike_efficiency': 0.0,
        'neuron_efficiency': 0.0
    }
    
    # Simple memory usage estimate
    neuron_count = brain.getTotalNeuronCount()
    spike_count = brain.getTotalSpikeCount()
    
    # Estimate memory usage (rough approximation)
    memory_results['memory_usage_mb'] = (neuron_count * 32 + spike_count * 8) / (1024 * 1024)
    
    # Calculate efficiencies
    memory_results['spike_efficiency'] = spike_count / neuron_count
    memory_results['neuron_efficiency'] = neuron_count / 1000  # Normalize to 1000 neurons
    
    return memory_results

def run_stability_test(brain, agent):
    """Test simulation stability over time."""
    
    stability_results = {
        'stability_score': 0.0,
        'consistency_score': 0.0,
        'learning_consistency': 0.0
    }
    
    # Collect performance data
    performances = []
    learning_rates = []
    
    for step in range(300):
        brain.step(step)
        
        performance = brain.getAverageFiringRate()
        performances.append(performance)
        
        if agent:
            learning_rates.append(agent.getCuriosityLevel())
    
    # Calculate stability scores
    if len(performances) > 1:
        # Stability as inverse of variance
        performance_variance = statistics.variance(performances) if len(performances) > 1 else 0
        stability_results['stability_score'] = 1.0 / (1.0 + performance_variance)
        
        # Consistency as coefficient of variation
        mean_performance = statistics.mean(performances)
        if mean_performance > 0:
            cv = statistics.stdev(performances) / mean_performance
            stability_results['consistency_score'] = 1.0 / (1.0 + cv)
    
    if len(learning_rates) > 1:
        # Learning consistency
        learning_variance = statistics.variance(learning_rates) if len(learning_rates) > 1 else 0
        stability_results['learning_consistency'] = 1.0 / (1.0 + learning_variance)
    
    return stability_results
```

## Example 5: Real-World Application - Resource Management

### Objective
Simulate realistic resource management and optimization using NLM capabilities.

```python
import pynlm
import numpy as np

def resource_management_simulation(episode_length=1500):
    """Real-world resource management simulation."""
    
    print("Resource Management Simulation")
    print("==============================")
    
    # Create resource management system
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.12)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=45, height=45, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(22.5, 22.5)
    agent.initialize(world)
    
    # Enable resource management systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Configure for resource management
    agent.setActionTemperature(0.5)
    agent.setMaxSequenceLength(15)
    agent.setHierarchicalControlEnabled(True)
    
    # Resource management state
    resource_state = {
        'total_resources': 1000.0,
        'available_resources': 1000.0,
        'consumption_rate': 0.5,
        'regeneration_rate': 0.3,
        'efficiency': 0.8,
        'waste_factor': 0.1,
        'optimal_storage': 200.0,
        'emergency_level': 50.0
    }
    
    print("Resource Management System Initialized:")
    print(f"  - Total resources: {resource_state['total_resources']}")
    print(f"  - Optimal storage: {resource_state['optimal_storage']}")
    print(f"  - Efficiency: {resource_state['efficiency']*100:.0f}%")
    
    # Resource management history
    resource_history = []
    agent_history = []
    
    print("\nStarting resource management simulation...")
    
    for step in range(episode_length):
        # Update resources
        update_resource_system(resource_state, step)
        
        # Get environmental input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Resource management decision making
        action = make_resource_management_decision(
            agent, resource_state, world, step
        )
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Resource consumption based on action
        resource_consumed = calculate_resource_consumption(action, resource_state)
        resource_state['available_resources'] -= resource_consumed
        
        # Resource regeneration
        resource_regenerated = resource_state['regeneration_rate'] * resource_state['efficiency']
        resource_state['available_resources'] += resource_regenerated
        
        # Resource limits
        resource_state['available_resources'] = min(
            resource_state['total_resources'],
            max(0, resource_state['available_resources'])
        )
        
        # Calculate rewards
        reward = calculate_resource_management_reward(
            resource_state, action, step, agent
        )
        
        predicted_reward = estimate_resource_future_reward(
            action, resource_state, step
        )
        
        agent.applyRewardModulation(reward, predicted_reward)
        
        agent.updateDevelopment(0.1)
        
        # Record history
        resource_history.append(resource_state.copy())
        agent_history.append({
            'step': step,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'performance': brain.getAverageFiringRate(),
            'action': action,
            'reward': reward
        })
        
        # Print periodic reports
        if step % 150 == 0:
            print(f"\nStep {step}:")
            print(f"  - Available resources: {resource_state['available_resources']:.1f}/" 
                  f"{resource_state['total_resources']}")
            print(f"  - Resource efficiency: {resource_state['efficiency']*100:.1f}%")
            print(f"  - Consumption rate: {resource_consumed:.2f}")
            print(f"  - Agent curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  - Agent performance: {brain.getAverageFiringRate():.3f} Hz")
            
            # Resource management alerts
            if resource_state['available_resources'] < resource_state['emergency_level']:
                print(f"  🚨 EMERGENCY: Low resources! ({resource_state['available_resources']:.1f})")
            
            if resource_state['efficiency'] < 0.6:
                print(f"  ⚠️  WARNING: Low efficiency! ({resource_state['efficiency']*100:.1f}%)")
    
    print("\nResource management simulation completed!")
    
    return brain, agent, world, resource_state, resource_history, agent_history

# Helper functions for resource management
def update_resource_system(state, step):
    """Update resource system dynamics."""
    # Natural regeneration
    natural_regeneration = state['regeneration_rate'] * state['optimal_storage']
    state['available_resources'] += natural_regeneration
    
    # Consumption decay
    state['consumption_rate'] = calculate_consumption_rate(state['efficiency'])
    
    # Efficiency changes based on management
    if step % 300 == 0:
        # Simulate efficiency improvements
        state['efficiency'] += 0.05
        state['waste_factor'] *= 0.9
    
    # Optimal storage adjustments
    optimal_range = 0.2 * state['optimal_storage']
    lower_bound = state['optimal_storage'] - optimal_range
    upper_bound = state['optimal_storage'] + optimal_range
    
    if state['available_resources'] < lower_bound:
        state['efficiency'] *= 1.1  # Penalize under-storage
    elif state['available_resources'] > upper_bound:
        state['efficiency'] *= 1.05  # Reward over-storage
    
    # Keep efficiency within bounds
    state['efficiency'] = min(1.0, max(0.5, state['efficiency']))

def calculate_consumption_rate(efficiency):
    """Calculate resource consumption rate based on efficiency."""
    return 0.5 * (2.0 - efficiency)

def make_resource_management_decision(agent, state, world, step):
    """Make resource management decisions."""
    curiosity = agent.getCuriosityLevel()
    available = state['available_resources']
    efficiency = state['efficiency']
    
    # Decision logic based on resource levels and curiosity
    if available < state['emergency_level']:
        # Emergency: prioritize resource gathering
        return ActionType.Eat
    elif curiosity > 0.7 and available > state['optimal_storage'] * 1.5:
        # High curiosity with abundant resources: explore
        return ActionType.MoveForward
    elif curiosity < 0.3 and efficiency > 0.8:
        # Low curiosity with high efficiency: optimize
        return ActionType.Rest
    elif available < state['optimal_storage'] * 0.5:
        # Low resources: conserve
        return ActionType.Wait
    elif available > state['optimal_storage'] * 2:
        # Abundant resources: distribute
        return ActionType.Interact
    else:
        # Default behavior
        return agent.decodeMotorCommand()

def calculate_resource_consumption(action, state):
    """Calculate resource consumption based on action."""
    consumption_map = {
        ActionType.Eat: 10.0,      # Resource gathering
        ActionType.MoveForward: 2.0,    # Movement
        ActionType.TurnLeft: 1.0,      # Turning
        ActionType.TurnRight: 1.0,     # Turning
        ActionType.Interact: 15.0,     # Complex interaction
        ActionType.Wait: 0.5,        # Waiting/conserving
        ActionType.Rest: 3.0,        # Resting/recovering
    }
    
    base_consumption = consumption_map.get(action, 1.0)
    
    # Apply efficiency factor
    actual_consumption = base_consumption * state['efficiency']
    
    # Apply waste factor
    actual_consumption *= (1.0 + state['waste_factor'])
    
    return actual_consumption

def calculate_resource_management_reward(state, action, step, agent):
    """Calculate reward based on resource management performance."""
    reward = 0.0
    
    # Resource availability reward
    availability_ratio = state['available_resources'] / state['total_resources']
    reward += availability_ratio * 2.0  # Max 2.0 reward
    
    # Efficiency reward
    efficiency_bonus = (state['efficiency'] - 0.5) * 2.0
    reward += efficiency_bonus
    
    # Action-specific rewards
    if action == ActionType.Eat:
        reward += 1.0  # Successful resource gathering
    elif action == ActionType.Interact:
        reward += 0.5  # Resource sharing
    
    # Novelty and learning rewards
    novelty = agent.getNoveltyLevel()
    curiosity = agent.getCuriosityLevel()
    reward += novelty * 0.3 + curiosity * 0.2
    
    # Time-based efficiency rewards
    if step > 0 and step % 300 == 0:
        if availability_ratio > 0.8:
            reward += 1.0  # Good resource management
        elif availability_ratio > 0.5:
            reward += 0.5  # Acceptable management
        else:
            reward -= 0.5  # Poor management
    
    return reward

def estimate_resource_future_reward(action, state, step):
    """Estimate future reward from current action."""
    # Simplified future reward estimation
    base_future = 0.5
    
    # Adjust based on action type
    if action == ActionType.Eat:
        future_boost = 1.0  # Resource gathering has high future value
    elif action == ActionType.Interact:
        future_boost = 0.7  # Resource sharing has medium value
    else:
        future_boost = 0.3  # Other actions have lower future value
    
    # Adjust based on current state
    availability_factor = state['available_resources'] / state['total_resources']
    future_estimate = base_future + future_boost * availability_factor
    
    return min(2.0, future_estimate)
```

## Example 6: Advanced Event System

### Objective
Demonstrate advanced event-driven programming and custom processing.

```python
import pynlm
from typing import Dict, List, Callable

class AdvancedEventSystem:
    """Advanced event system for custom NLM processing."""
    
    def __init__(self):
        self.events = {
            'neural_spike': [],
            'plasticity_event': [],
            'neuromodulation_change': [],
            'behavior_change': [],
            'environment_change': []
        }
        
        self.event_handlers = {}
        self.event_statistics = {}
        self.custom_processing = {}
    
    def register_event_handler(self, event_type: str, handler: Callable):
        """Register a custom event handler."""
        if event_type not in self.events:
            self.events[event_type] = []
        
        self.event_handlers[event_type] = handler
        print(f"   Registered handler for '{event_type}' events")
    
    def process_custom_events(self, brain, agent, world, step):
        """Process custom events with advanced logic."""
        
        # Neural event processing
        if 'neural_spike' in self.events:
            spike_events = detect_neural_events(brain)
            for event in spike_events:
                self.trigger_event('neural_spike', event, brain, agent, world, step)
        
        # Plasticity event processing
        if 'plasticity_event' in self.events:
            plasticity_events = detect_plasticity_events(brain)
            for event in plasticity_events:
                self.trigger_event('plasticity_event', event, brain, agent, world, step)
        
        # Neuromodulation event processing
        if 'neuromodulation_change' in self.events:
            neuromod_events = detect_neuromodulation_events(brain, agent)
            for event in neuromod_events:
                self.trigger_event('neuromodulation_change', event, brain, agent, world, step)
        
        # Behavior change detection
        if 'behavior_change' in self.events:
            behavior_events = detect_behavior_changes(agent, world, step)
            for event in behavior_events:
                self.trigger_event('behavior_change', event, brain, agent, world, step)
        
        # Environmental change detection
        if 'environment_change' in self.events:
            env_events = detect_environmental_changes(world, step)
            for event in env_events:
                self.trigger_event('environment_change', event, brain, agent, world, step)
    
    def trigger_event(self, event_type: str, event_data, brain, agent, world, step):
        """Trigger an event and execute registered handlers."""
        event_info = {
            'type': event_type,
            'data': event_data,
            'step': step,
            'timestamp': time.time(),
            'brain_stats': {
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount()
            },
            'agent_stats': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError()
            },
            'world_stats': {
                'simulation_time': world.getSimulationTime(),
                'position': (world.getAgentBody().x, world.getAgentBody().y)
            }
        }
        
        # Execute custom handler if registered
        if event_type in self.event_handlers:
            handler = self.event_handlers[event_type]
            try:
                result = handler(event_info)
                if result:
                    event_info['result'] = result
                    print(f"   Event '{event_type}' processed successfully")
            except Exception as e:
                print(f"   Event '{event_type}' processing failed: {e}")
        
        # Store event for analysis
        if event_type not in self.event_statistics:
            self.event_statistics[event_type] = []
        
        self.event_statistics[event_type].append(event_info)
        
        # Apply custom processing if defined
        if event_type in self.custom_processing:
            processor = self.custom_processing[event_type]
            processor(event_info)
    
    def setup_custom_processing(self, event_type: str, processor: Callable):
        """Setup custom event processing."""
        self.custom_processing[event_type] = processor
        print(f"   Custom processor registered for '{event_type}'")
    
    def analyze_event_statistics(self):
        """Analyze and report event statistics."""
        report = {}
        
        for event_type, events in self.event_statistics.items():
            if events:
                report[event_type] = {
                    'total_events': len(events),
                    'events_per_step': len(events) / (events[-1]['step'] + 1),
                    'event_types': self.categorize_events(events),
                    'impact_assessment': self.assess_event_impact(events)
                }
        
        return report
    
    def categorize_events(self, events: List[Dict]) -> Dict[str, int]:
        """Categorize events by type and severity."""
        categories = {}
        for event in events:
            # Simple categorization based on event data
            if 'magnitude' in event['data']:
                magnitude = event['data']['magnitude']
                if magnitude > 0.8:
                    category = 'high_magnitude'
                elif magnitude > 0.5:
                    category = 'medium_magnitude'
                else:
                    category = 'low_magnitude'
            else:
                category = 'unknown'
            
            categories[category] = categories.get(category, 0) + 1
        
        return categories
    
    def assess_event_impact(self, events: List[Dict]) -> Dict[str, float]:
        """Assess impact of events on system behavior."""
        impact = {
            'learning_impact': 0.0,
            'performance_impact': 0.0,
            'behavior_impact': 0.0,
            'overall_impact': 0.0
        }
        
        if not events:
            return impact
        
        # Calculate average impacts
        for event in events:
            impact['learning_impact'] += self.calculate_learning_impact(event)
            impact['performance_impact'] += self.calculate_performance_impact(event)
            impact['behavior_impact'] += self.calculate_behavior_impact(event)
        
        # Normalize
        num_events = len(events)
        for key in impact:
            if key != 'overall_impact':
                impact[key] /= num_events
        
        # Calculate overall impact
        impact['overall_impact'] = (
            impact['learning_impact'] * 0.4 +
            impact['performance_impact'] * 0.3 +
            impact['behavior_impact'] * 0.3
        )
        
        return impact
    
    def calculate_learning_impact(self, event: Dict) -> float:
        """Calculate impact of event on learning."""
        # Simplified impact calculation
        curiosity_change = abs(event['agent_stats']['curiosity'] - 0.5)
        novelty_change = abs(event['agent_stats']['novelty'] - 0.5)
        return min(1.0, curiosity_change + novelty_change)
    
    def calculate_performance_impact(self, event: Dict) -> float:
        """Calculate impact of event on performance."""
        # Based on neural activity changes
        firing_change = event['brain_stats']['firing_neurons'] / 100.0
        spike_change = event['brain_stats']['total_spikes'] / 1000.0
        return min(1.0, firing_change + spike_change)
    
    def calculate_behavior_impact(self, event: Dict) -> float:
        """Calculate impact of event on behavior."""
        # Based on environmental and behavioral changes
        behavior_score = 0.0
        if 'data' in event and 'action' in event['data']:
            behavior_score += 0.5
        if 'position' in event['world_stats']:
            behavior_score += 0.3
        if 'prediction_error' in event['agent_stats']:
            error = event['agent_stats']['prediction_error']
            behavior_score += min(0.2, abs(error))
        
        return min(1.0, behavior_score)

# Example usage of advanced event system
def advanced_event_system_example(episode_length=1000):
    """Example of advanced event system usage."""
    
    print("Advanced Event System Example")
    print("============================")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=35, height=35, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(17.5, 17.5)
    agent.initialize(world)
    
    # Enable all systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enablePredictionErrorLearning(True)
    agent.enableDevelopment(True)
    
    # Setup advanced event system
    event_system = AdvancedEventSystem()
    
    # Register custom event handlers
    def handle_neural_events(event_info):
        print(f"     Neural event: {event_info['data']['type']}")
        return 'processed'
    
    def handle_plasticity_events(event_info):
        print(f"     Plasticity event: {event_info['data']['change_type']}")
        return 'strengthening'
    
    def handle_neuromodulation_events(event_info):
        print(f"     Neuromodulation event: {event_info['data']['modulator']} = {event_info['data']['level']}")
        return 'balanced'
    
    event_system.register_event_handler('neural_spike', handle_neural_events)
    event_system.register_event_handler('plasticity_event', handle_plasticity_events)
    event_system.register_event_handler('neuromodulation_change', handle_neuromodulation_events)
    
    # Setup custom processing
    def spike_analysis_processor(event_info):
        spikes = event_info['brain_stats']['total_spikes']
        if spikes % 100 == 0:
            print(f"     [Spike Analysis] Total spikes: {spikes}")
    
    event_system.setup_custom_processing('neural_spike', spike_analysis_processor)
    
    print("Advanced event system configured and ready!")
    
    # Main simulation loop with event processing
    for step in range(episode_length):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Process custom events
        event_system.process_custom_events(brain, agent, world, step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply rewards
        reward = 0.5 if step % 100 == 0 else 0.1
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Periodic event analysis
        if step % 300 == 0 and step > 0:
            stats = event_system.analyze_event_statistics()
            print(f"\nEvent Analysis (Step {step}):")
            for event_type, analysis in stats.items():
                print(f"  {event_type}:")
                print(f"    - Total events: {analysis['total_events']}")
                print(f"    - Events per step: {analysis['events_per_step']:.2f}")
                print(f"    - Categories: {analysis['event_types']}")
                print(f"    - Overall impact: {analysis['impact_assessment']['overall_impact']:.3f}")
    
    print("\nAdvanced event system simulation completed!")
    
    # Final event analysis
    final_stats = event_system.analyze_event_statistics()
    print(f"\nFinal Event System Statistics:")
    for event_type, analysis in final_stats.items():
        print(f"  {event_type}:")
        print(f"    - Total events processed: {analysis['total_events']}")
        print(f"    - Average events per step: {analysis['events_per_step']:.2f}")
    
    return brain, agent, world, event_system

# Helper functions for event system (simplified versions)
def detect_neural_events(brain):
    """Detect neural spike events."""
    # Simplified detection - would use actual spike detection
    events = []
    if brain.getFiringNeuronCount() > 50:  # Threshold for event generation
        events.append({
            'type': 'high_activity_spike',
            'magnitude': brain.getFiringNeuronCount() / brain.getTotalNeuronCount()
        })
    return events

def detect_plasticity_events(brain):
    """Detect plasticity events."""
    events = []
    # Simplified plasticity detection
    if brain.getAverageFiringRate() > 0.5:
        events.append({
            'type': 'synaptic_weight_change',
            'change_type': 'strengthening'
        })
    return events

def detect_neuromodulation_events(brain, agent):
    """Detect neuromodulation events."""
    events = []
    # Simplified neuromodulation detection
    if agent.getCuriosityLevel() > 0.7:
        events.append({
            'type': 'curiosity_peak',
            'modulator': 'curiosity',
            'level': agent.getCuriosityLevel()
        })
    return events

def detect_behavior_changes(agent, world, step):
    """Detect behavior changes."""
    events = []
    # Simplified behavior detection
    if step % 150 == 0:
        events.append({
            'type': 'behavioral_pattern_change',
            'action_change': True
        })
    return events

def detect_environmental_changes(world, step):
    """Detect environmental changes."""
    events = []
    # Simplified environmental detection
    if step % 200 == 0:
        events.append({
            'type': 'environmental_stimulus',
            'stimulus_strength': 0.8
        })
    return events
```

## Example 7: Real-Time Performance Optimization

### Objective
Demonstrate real-time performance optimization and adaptive resource management.

```python
import pynlm
import time
import statistics

def real_time_optimization(episode_length=2000):
    """Real-time performance optimization demonstration."""
    
    print("Real-Time Performance Optimization")
    print("=================================")
    
    # Create optimization test setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    agent.initialize(world)
    
    # Enable all systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Configure for optimization
    agent.setActionTemperature(0.8)
    agent.setMaxSequenceLength(10)
    
    print("Performance optimization system initialized")
    print("Enabled adaptive resource management")
    
    # Optimization state
    optimization_state = {
        'current_performance': 0.0,
        'target_performance': 1.0,
        'adaptation_rate': 0.01,
        'stability_buffer': 0.9,
        'resource_efficiency': 0.8,
        'learning_efficiency': 0.7,
        'temperature_adaptation': True,
        'curiosity_adaptation': True
    }
    
    # Performance tracking
    performance_history = {
        'steps': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'learning_rates': [],
        'efficiency_scores': []
    }
    
    print("\nStarting real-time optimization simulation...")
    
    for step in range(episode_length):
        # Real-time performance monitoring
        current_firing = brain.getFiringNeuronCount() / brain.getTotalNeuronCount()
        current_curiosity = agent.getCuriosityLevel()
        current_novelty = agent.getNoveltyLevel()
        current_learning = calculate_learning_rate(brain, step)
        
        # Calculate overall efficiency score
        efficiency_score = (
            current_firing * 0.4 +
            current_curiosity * 0.3 +
            current_learning * 0.3
        )
        
        # Record performance
        performance_history['steps'].append(step)
        performance_history['firing_rates'].append(current_firing)
        performance_history['curiosity_levels'].append(current_curiosity)
        performance_history['learning_rates'].append(current_learning)
        performance_history['efficiency_scores'].append(efficiency_score)
        
        # Adaptation based on performance
        adapt_optimization_parameters(
            optimization_state, current_firing, current_curiosity, 
            current_learning, efficiency_score, step
        )
        
        # Apply adapted parameters
        agent.setActionTemperature(
            optimization_state['current_temperature']
        )
        agent.setExplorationRate(
            optimization_state['current_exploration_rate']
        )
        
        # Update environment
        world.update(0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate adaptive rewards
        reward = calculate_adaptive_reward(
            optimization_state, current_firing, current_curiosity,
            current_learning, efficiency_score, step
        )
        
        predicted_reward = estimate_future_optimization_reward(
            action, optimization_state, step
        )
        
        agent.applyRewardModulation(reward, predicted_reward)
        
        agent.updateDevelopment(0.1)
        
        # Real-time optimization metrics
        if step % 200 == 0 and step > 0:
            analyze_real_time_optimization(
                performance_history, optimization_state, step
            )
            
            # Check if optimization is successful
            if efficiency_score > optimization_state['target_performance'] * 0.9:
                print(f"\n✓ Optimization Success at Step {step}!")
                print(f"  - Efficiency Score: {efficiency_score:.3f}")
                print(f"  - Target Achievement: {(efficiency_score / optimization_state['target_performance']) * 100:.1f}%")
            
            # Reset for next optimization cycle
            optimization_state['current_performance'] = efficiency_score
    
    print("\nReal-time optimization simulation completed!")
    
    # Final optimization analysis
    final_analysis = analyze_final_optimization(performance_history, optimization_state)
    
    return brain, agent, world, performance_history, final_analysis

# Helper functions for real-time optimization
def calculate_learning_rate(brain, step):
    """Calculate current learning rate based on brain activity."""
    firing_rate = brain.getAverageFiringRate()
    spike_count = brain.getTotalSpikeCount()
    
    # Normalize learning rate
    learning_rate = (firing_rate * 0.6 + spike_count / 1000.0 * 0.4)
    return min(1.0, learning_rate)

def adapt_optimization_parameters(state, firing_rate, curiosity, learning_rate, efficiency, step):
    """Adapt optimization parameters in real-time."""
    
    # Temperature adaptation
    if state['temperature_adaptation']:
        if efficiency < state['stability_buffer']:
            # Low efficiency -> increase exploration
            state['current_temperature'] += state['adaptation_rate'] * 0.5
        else:
            # Good efficiency -> reduce exploration
            state['current_temperature'] -= state['adaptation_rate'] * 0.3
    
    # Curiosity adaptation
    if state['curiosity_adaptation']:
        if curiosity < 0.3:
            # Low curiosity -> encourage exploration
            state['current_exploration_rate'] = min(1.0, state['current_exploration_rate'] + state['adaptation_rate'])
        elif curiosity > 0.8:
            # High curiosity -> reduce random exploration
            state['current_exploration_rate'] = max(0.1, state['current_exploration_rate'] - state['adaptation_rate'] * 0.5)
    
    # Learning rate adaptation
    if learning_rate < state['learning_efficiency'] * 0.5:
        # Low learning -> increase plasticity
        state['plasticity_modifier'] = min(1.5, state['plasticity_modifier'] + state['adaptation_rate'])
    else:
        # Good learning -> stabilize
        state['plasticity_modifier'] = max(0.8, state['plasticity_modifier'] - state['adaptation_rate'] * 0.5)
    
    # Resource efficiency adaptation
    if efficiency < 0.6:
        # Poor efficiency -> optimize resources
        state['resource_efficiency'] = min(1.0, state['resource_efficiency'] + state['adaptation_rate'])
    else:
        # Good efficiency -> maintain
        state['resource_efficiency'] = max(0.7, state['resource_efficiency'] - state['adaptation_rate'] * 0.2)
    
    # Store adapted parameters
    state['current_temperature'] = max(0.1, min(2.0, state['current_temperature']))
    state['current_exploration_rate'] = max(0.1, min(1.0, state['current_exploration_rate']))
    state['plasticity_modifier'] = max(0.5, min(2.0, state['plasticity_modifier']))
    state['resource_efficiency'] = max(0.6, min(1.0, state['resource_efficiency']))
    
    # Calculate current overall performance
    state['current_performance'] = (
        firing_rate * 0.4 +
        curiosity * 0.3 +
        learning_rate * 0.3
    )

def calculate_adaptive_reward(state, firing_rate, curiosity, learning_rate, efficiency, step):
    """Calculate adaptive reward based on optimization state."""
    reward = 0.0
    
    # Performance-based reward
    reward += firing_rate * 0.5  # High firing rate = good performance
    reward += efficiency * 0.3   # High efficiency = good optimization
    reward += learning_rate * 0.2 # Good learning = adaptive improvement
    
    # Adaptation bonus
    if step > 100 and state['current_performance'] > state['current_performance'] * 1.2:
        reward += 0.5  # Significant improvement
    
    # Stability bonus
    if efficiency > 0.8 and curiosity > 0.5:
        reward += 0.3  # Balanced performance
    
    return min(2.0, reward)

def estimate_future_optimization_reward(action, state, step):
    """Estimate future reward from current action."""
    # Base future reward
    base_future = 0.3
    
    # Adjust based on action type
    if action in [ActionType.MoveForward, ActionType.Interact]:
        future_boost = 0.8  # Action-oriented behaviors have high future value
    elif action == ActionType.Rest:
        future_boost = 0.5  # Resting has medium future value
    else:
        future_boost = 0.2  # Other actions have lower future value
    
    # Adjust based on current state
    availability_factor = state['resource_efficiency']
    future_estimate = base_future + future_boost * availability_factor
    
    return min(2.0, future_estimate)

def analyze_real_time_optimization(history, state, step):
    """Analyze real-time optimization performance."""
    
    if step < 100:
        return
    
    # Calculate moving averages
    recent_steps = history['steps'][-100:]
    recent_firing = history['firing_rates'][-100:]
    recent_curiosity = history['curiosity_levels'][-100:]
    recent_learning = history['learning_rates'][-100:]
    recent_efficiency = history['efficiency_scores'][-100:]
    
    # Calculate trends
    firing_trend = calculate_trend(recent_firing)
    curiosity_trend = calculate_trend(recent_curiosity)
    efficiency_trend = calculate_trend(recent_efficiency)
    
    print(f"    Real-time Optimization Analysis (Step {step}):")
    print(f"      - Firing Rate Trend: {firing_trend:.3f}")
    print(f"      - Curiosity Trend: {curiosity_trend:.3f}")
    print(f"      - Efficiency Trend: {efficiency_trend:.3f}")
    print(f"      - Current Adaptation Rate: {state['adaptation_rate']:.3f}")
    print(f"      - Current Temperature: {state['current_temperature']:.3f}")
    print(f"      - Current Exploration: {state['current_exploration_rate']:.3f}")
    
    # Adaptation recommendations
    if efficiency_trend < -0.01:
        print(f"      ⚠️  Efficiency decreasing - increase adaptation rate")
    elif firing_trend > 0.01:
        print(f"      ✅ Performance improving - maintain current settings")
    else:
        print(f"      📊 Performance stable - fine-tune parameters")

def analyze_final_optimization(history, state):
    """Analyze final optimization results."""
    
    analysis = {
        'final_efficiency': 0.0,
        'optimization_success': False,
        'adaptation_effectiveness': 0.0,
        'performance_stability': 0.0,
        'recommendations': []
    }
    
    if not history['efficiency_scores']:
        return analysis
    
    # Calculate final metrics
    analysis['final_efficiency'] = history['efficiency_scores'][-1]
    
    # Calculate optimization success
    if analysis['final_efficiency'] > 0.7:
        analysis['optimization_success'] = True
    
    # Calculate adaptation effectiveness
    if len(history['efficiency_scores']) > 10:
        initial_efficiency = history['efficiency_scores'][0]
        final_efficiency = history['efficiency_scores'][-1]
        if final_efficiency > initial_efficiency:
            improvement = (final_efficiency - initial_efficiency) / initial_efficiency
            analysis['adaptation_effectiveness'] = min(1.0, improvement)
    
    # Calculate performance stability
    efficiency_std = statistics.stdev(history['efficiency_scores']) if len(history['efficiency_scores']) > 1 else 0
    analysis['performance_stability'] = 1.0 / (1.0 + efficiency_std)
    
    # Generate recommendations
    if analysis['adaptation_effectiveness'] < 0.5:
        analysis['recommendations'].append(
            "Increase adaptation rate for better optimization"
        )
    
    if analysis['performance_stability'] < 0.7:
        analysis['recommendations'].append(
            "Reduce parameter volatility for better stability"
        )
    
    if analysis['final_efficiency'] < 0.6:
        analysis['recommendations'].append(
            "Consider different optimization strategies"
        )
    
    if not analysis['recommendations']:
        analysis['recommendations'].append(
            "Current optimization strategy is effective"
        )
    
    return analysis

def calculate_trend(values):
    """Calculate trend from a series of values."""
    if len(values) < 2:
        return 0.0
    
    # Simple linear regression slope
    n = len(values)
    sum_x = sum(range(n))
    sum_y = sum(values)
    sum_xy = sum(i * values[i] for i in range(n))
    sum_x2 = sum(i * i for i in range(n))
    
    if sum_x2 - sum_x * sum_x / n == 0:
        return 0.0
    
    slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
    return slope
```

## Summary and Key Takeaways

### Example Summary

This document provides 7 comprehensive intermediate examples covering:

1. **Dynamic Goal Learning**: Agents that adapt goals based on performance
2. **Multi-Agent Cooperation**: Multiple agents working together cooperatively
3. **Complex Environment Simulation**: Dynamic environments with interacting systems
4. **Performance Benchmark Suite**: Comprehensive performance testing and comparison
5. **Real-World Application**: Resource management simulation
6. **Advanced Event System**: Event-driven custom processing
7. **Real-Time Optimization**: Adaptive performance optimization

### Key Technical Concepts

#### Advanced Agent Behaviors

1. **Dynamic Goal Adaptation**: Agents modify goals based on curiosity and performance
2. **Multi-Agent Cooperation**: Social behavior and collective intelligence
3. **Environmental Awareness**: Agents respond to changing environmental conditions
4. **Resource Management**: Efficient resource allocation and optimization

#### Performance Optimization

1. **Adaptive Parameters**: Real-time adjustment of learning rates and exploration
2. **Efficiency Metrics**: Comprehensive performance measurement and analysis
3. **Stress Testing**: Evaluation under challenging conditions
4. **Stability Analysis**: Consistent performance over time

#### Advanced Features

1. **Event-Driven Architecture**: Custom event handling and processing
2. **Custom Processing**: User-defined event handlers and processors
3. **Real-Time Optimization**: Continuous performance adaptation
4. **Resource Management**: Efficient resource allocation and consumption

### Learning Path Recommendations

1. **Study Examples 1-2**: Understand dynamic behavior and multi-agent systems
2. **Study Examples 3-4**: Master complex environment simulation and performance optimization
3. **Study Examples 5-7**: Explore advanced applications and optimization strategies
4. **Practice**: Modify examples to create your own applications
5. **Research**: Apply patterns to solve specific research problems

### Technical Implementation Details

#### Code Structure

```python
# Common patterns across examples:
# 1. Configuration setup
config = pynlm.createDefaultConfig()

# 2. Brain and agent creation
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# 3. Environment setup
world = pynlm.createSimpleWorld()
world.configure(width, height, visionWidth, visionHeight)

# 4. Learning subsystem enablement
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# 5. Configuration of advanced features
agent.setActionTemperature(0.7)
agent.setMaxSequenceLength(10)

# 6. Simulation loop
for step in range(episode_length):
    # Update environment
    world.update(0.1)
    
    # Process sensory input
    agent.processSensoryInput(world.getSensoryPercept())
    
    # Run brain step
    brain.step(step)
    
    # Get and apply action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply rewards
    reward = calculate_reward(...)
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

### Next Steps

1. **Study Related Guides**: Read `brain_architecture_guide.md` and `enhanced_python_api_guide.md`
2. **Practice Examples**: Run and modify intermediate examples
3. **Create Applications**: Build your own applications using patterns from examples
4. **Performance Testing**: Use benchmark suite for optimization
5. **Research Applications**: Apply to specific research problems

### Final Tips

1. **Start with Simple**: Begin with basic examples before adding complexity
2. **Understand Patterns**: Master common patterns across examples
3. **Practice Optimization**: Use examples to learn optimization techniques
4. **Document Results**: Record results and insights from examples
5. **Extend Features**: Add new features to existing examples

---

*This intermediate example series provides practical, reusable patterns for advanced NLM applications. Each example demonstrates real-world scenarios and technical implementation details.*

## Related Resources

- **Beginner Tutorials**: `tutorial_series.md`
- **Enhanced Python API**: `enhanced_python_api_guide.md`
- **AgentBrain Motor Control**: `agent_brain_motor_control_guide.md`
- **Neuromodulation Systems**: `neuromodulation_guide.md`
- **Brain Architecture**: `brain_architecture_guide.md`
- **Research Examples**: `research_examples.md`
- **Configuration Examples**: `configuration_examples.md`
- **Performance Examples**: `performance_examples.md`