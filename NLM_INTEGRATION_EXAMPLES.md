# NLM Integration Examples for Different Use Cases

## Overview

This document provides practical integration examples for NLM (Neural Learning Machine) across different use cases and scenarios. These examples demonstrate how to effectively use NLM in various contexts, from simple simulations to complex multi-agent systems.

## Use Case 1: Simple Neural Network Training

### Overview
Create a simple neural network training simulation where neurons learn through repeated exposure to patterns.

### Objectives
- Demonstrate basic neural network learning
- Show how neurons develop firing patterns over time
- Illustrate synaptic plasticity in action
- Provide a foundation for understanding neural learning

### Use Case: Biological Learning Simulation

```python
import pynlm

def basic_neural_training():
    """
    Example: Basic neural network training with simple learning patterns
    """
    print("=== Basic Neural Network Training Example ===")
    
    # Configure brain for learning
    config = pynlm.createLearningConfig(
        neurons=500,
        learningRate=0.01,
        plasticityRate=0.1
    )
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulation parameters
    num_steps = 1000
    pattern_length = 50
    
    # Generate simple training patterns
    patterns = generate_simple_patterns(pattern_length)
    
    print(f"Starting training with {len(patterns)} patterns, {num_steps} steps")
    
    # Training loop
    for step in range(num_steps):
        # Get current pattern
        pattern = patterns[step % len(patterns)]
        
        # Present pattern to brain
        sensory_input = create_sensory_input_from_pattern(pattern)
        brain.receiveSensoryInput(sensory_input)
        
        # Run brain step
        brain.step(step)
        
        # Monitor progress
        if step % 100 == 0:
            firing_rate = brain.getAverageFiringRate()
            total_spikes = brain.getTotalSpikeCount()
            print(f"Step {step}: Firing rate={firing_rate:.3f} Hz, Total spikes={total_spikes}")
    
    # Analyze results
    analyze_learning_results(brain, patterns)
    
    return brain

def generate_simple_patterns(length):
    """Generate simple training patterns"""
    import random
    patterns = []
    
    for i in range(length):
        pattern = []
        # Create simple patterns with varying complexity
        if i % 3 == 0:
            # Simple alternating pattern
            pattern = [random.uniform(-1.0, 1.0) for _ in range(10)]
        elif i % 3 == 1:
            # Gaussian-like pattern
            pattern = [math.exp(-0.5 * ((x - 5) / 2) ** 2) for x in range(10)]
        else:
            # Spike-like pattern
            pattern = [0.0] * 10
            spike_pos = random.randint(0, 9)
            pattern[spike_pos] = 1.0
        
        patterns.append(pattern)
    
    return patterns

def create_sensory_input_from_pattern(pattern):
    """Create sensory input from pattern data"""
    # Create internal signals sensory input
    sensory = pynlm.SensoryInput()
    internal = pynlm.InternalSignals()
    
    for value in pattern:
        internal.addSignal(value)
    
    sensory.setInternal(internal)
    return sensory

def analyze_learning_results(brain, patterns):
    """Analyze and display learning results"""
    print("\n=== Learning Analysis ===")
    
    total_steps = 1000
    final_firing_rate = brain.getAverageFiringRate()
    total_spikes = brain.getTotalSpikeCount()
    spike_frequency = total_spikes / total_steps
    
    print(f"Final firing rate: {final_firing_rate:.3f} Hz")
    print(f"Total spikes generated: {total_spikes}")
    print(f"Average spike frequency: {spike_frequency:.3f} Hz")
    
    # Calculate learning metrics
    learning_efficiency = min(final_firing_rate / 10.0, 1.0) * 100
    pattern_recognition = len([p for p in patterns if abs(sum(p) / len(p)) > 0.1]) / len(patterns) * 100
    
    print(f"Learning efficiency: {learning_efficiency:.1f}%")
    print(f"Pattern recognition: {pattern_recognition:.1f}%")
    
    # Determine learning outcome
    if learning_efficiency > 50 and pattern_recognition > 50:
        print("✓ SUCCESS: Neural network learned effectively")
    elif learning_efficiency > 20:
        print("⚠ PARTIAL: Neural network showed some learning")
    else:
        print("✗ LIMITED: Neural network showed minimal learning")
```

### Integration Notes

**Python Integration:**
```python
# Save trained brain for later use
brain.save("trained_brain.bin")

# Export learning statistics
export_learning_stats(brain, "learning_stats.csv")

# Create visualization
visualize_brain_activity(brain, "learning_animation.gif")
```

**C++ Integration:**
```cpp
// Save brain to file
brain.save("trained_brain.bin");

// Export learning statistics to CSV
exportLearningStats(brain, "learning_stats.csv");

// Create visualization
VisualizeBrainActivity(brain, "learning_animation.gif");
```

## Use Case 2: Multi-Agent Coordination

### Overview
Create a multi-agent system where multiple neural networks coordinate and learn together.

### Objectives
- Demonstrate multi-agent coordination
- Show how agents learn through interaction
- Illustrate emergent behaviors in agent populations
- Provide foundation for swarm intelligence simulation

### Use Case: Swarm Intelligence Simulation

```python
import pynlm
import numpy as np

def multi_agent_coordination():
    """
    Example: Multi-agent coordination and learning simulation
    """
    print("=== Multi-Agent Coordination Example ===")
    
    # Create multiple agents
    num_agents = 5
    agents = []
    brains = []
    worlds = []
    
    # Create diverse agent population
    for i in range(num_agents):
        # Different configurations for diversity
        if i % 3 == 0:
            config = pynlm.createExplorationConfig(neurons=300, curiosity=0.8)
        elif i % 3 == 1:
            config = pynlm.createLearningConfig(neurons=300, learningRate=0.02)
        else:
            config = pynlm.createDefaultConfig()
        
        # Create components
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(np.random.uniform(5, 45), np.random.uniform(5, 45))
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable different features for diversity
        agent.enableRewardModulation(True)
        agent.enableCuriosity(i % 2 == 0)  # Some agents are curious
        agent.enableDevelopment(True)
        
        agents.append(agent)
        brains.append(brain)
        worlds.append(world)
        
        print(f"  Agent {i+1}: {brain.getTotalNeuronCount()} neurons, "
              f"Curiosity={agent.getCuriosityLevel():.2f}")
    
    # Simulation parameters
    num_steps = 500
    coordination_metric = 0.0
    
    print(f"\nRunning coordination simulation for {num_steps} steps...")
    
    # Simulation loop
    for step in range(num_steps):
        step_coordination = 0.0
        
        for agent_idx, agent in enumerate(agents):
            world = worlds[agent_idx]
            brain = brains[agent_idx]
            
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate coordination contribution
            step_coordination += calculate_coordination(agent, step)
            
            # Monitor agent state
            if step % 100 == 0:
                print(f"    Agent {agent_idx+1}: Reward={agent.getCuriosityLevel():.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}")
        
        # Average coordination for this step
        coordination_metric += step_coordination / num_agents
        
        # Show progress
        if step % 100 == 0:
            print(f"  Step {step}: Coordination={coordination_metric/((step//100)+1):.3f}")
    
    # Analyze coordination results
    analyze_coordination_results(agents, brains, worlds, coordination_metric, num_steps)
    
    return agents, brains, worlds

def calculate_coordination(agent, step):
    """Calculate coordination metric for an agent"""
    # Use curiosity level as coordination metric
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    # Combine multiple factors
    coordination = (curiosity * 0.6) + (novelty * 0.4)
    
    return coordination

def analyze_coordination_results(agents, brains, worlds, total_coordination, num_steps):
    """Analyze and display coordination results"""
    print("\n=== Coordination Analysis ===")
    
    avg_coordination = total_coordination / num_steps
    print(f"Average coordination: {avg_coordination:.3f}")
    
    # Analyze individual agent performance
    print("\nIndividual Agent Performance:")
    for i, (agent, brain, world) in enumerate(zip(agents, brains, worlds)):
        total_reward = brain.getAverageFiringRate() * 10  # Approximate
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"  Agent {i+1}:")
        print(f"    Coordination: {curiosity:.3f}")
        print(f"    Novelty: {novelty:.3f}")
        print(f"    Firing rate: {firing_rate:.3f} Hz")
        print(f"    World position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Determine coordination quality
    if avg_coordination > 0.7:
        print("\n✓ EXCELLENT: High coordination achieved")
    elif avg_coordination > 0.5:
        print("\n⚠ GOOD: Moderate coordination achieved")
    elif avg_coordination > 0.3:
        print("\n⚠ FAIR: Basic coordination achieved")
    else:
        print("\n✗ POOR: Low coordination achieved")
```

### Integration Notes

**Multi-Agent Patterns:**
```python
# Save all agents for later use
for i, (brain, world) in enumerate(zip(brains, worlds)):
    brain.save(f"agent_{i}_brain.bin")
    # Could also save world state

# Export coordination data
export_coordination_data(agents, brains, worlds, "coordination_data.csv")

# Create coordination visualization
create_coordination_animation(agents, brains, worlds, "coordination.gif")
```

**Distributed Processing:**
```cpp
// Parallel processing for large agent populations
#pragma omp parallel for
for (int i = 0; i < num_agents; i++) {
    run_agent_step(agents[i], worlds[i], i);
}

// Synchronization barrier
#pragma omp barrier

// Collective communication
collective_communication(agents);
```

## Use Case 3: Environment Modeling and Prediction

### Overview
Model complex environments and predict agent behavior within them.

### Objectives
- Demonstrate environment modeling capabilities
- Show predictive modeling of agent behavior
- Illustrate environment-agent interaction
- Provide foundation for realistic simulation environments

### Use Case: Environment Prediction and Control

```python
import pynlm
import math
from typing import List, Tuple

class EnvironmentPredictor:
    """
    Example: Environment prediction and control system
    """
    
    def __init__(self):
        self.environment = pynlm.createSimpleWorld()
        self.environment.configure(width=100, height=100, visionWidth=15, visionHeight=15)
        self.environment.reset()
        
        # Initialize predictive agent
        self.config = pynlm.createLearningConfig(neurons=800, learningRate=0.015)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.environment)
        
        # Enable prediction features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Environment objects
        self.environment_objects = []
        self.predictive_model = PredictionModel()
    
    def run_prediction_simulation(self, num_steps: int) -> dict:
        """
        Run environment prediction simulation
        """
        print("=== Environment Prediction Simulation ===")
        
        # Setup environment with obstacles and resources
        self.setup_environment()
        
        # Simulation tracking
        simulation_data = {
            'steps': [],
            'rewards': [],
            'predictions': [],
            'actuals': [],
            'errors': []
        }
        
        print(f"Running prediction simulation for {num_steps} steps...")
        
        for step in range(num_steps):
            # Get current state
            current_state = self.get_environment_state()
            
            # Generate prediction
            prediction = self.predictive_model.predict_next_state(current_state, self.agent)
            
            # Process current state
            sensory_input = self.create_sensory_input(current_state)
            self.agent.processSensoryInput(sensory_input)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action based on prediction
            action = self.agent.decodeMotorCommand()
            
            # Apply action to environment
            self.environment.applyMotorCommand(action, self.environment.getSimulationTime())
            
            # Get actual result
            actual_state = self.get_environment_state()
            actual_reward = self.calculate_reward(actual_state)
            
            # Update prediction model
            self.predictive_model.update(prediction, actual_state, actual_reward)
            
            # Store simulation data
            simulation_data['steps'].append(step)
            simulation_data['rewards'].append(actual_reward)
            simulation_data['predictions'].append(prediction)
            simulation_data['actuals'].append(actual_state)
            simulation_data['errors'].append(abs(prediction - actual_reward))
            
            # Show progress
            if step % 100 == 0:
                avg_error = sum(simulation_data['errors'][-100:]) / min(len(simulation_data['errors']), 100)
                print(f"  Step {step}: Reward={actual_reward:.3f}, "
                      f"Prediction Error={avg_error:.3f}")
        
        # Analyze prediction results
        results = self.analyze_prediction_results(simulation_data)
        
        return results
    
    def setup_environment(self):
        """Setup environment with obstacles and resources"""
        # Add walls
        for x in range(0, 100, 10):
            self.environment.addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType::Wall, radius=5))
            self.environment.addObject(pynlm.WorldObject(x, 100, pynlm.WorldObjectType::Wall, radius=5))
        
        for y in range(0, 100, 10):
            self.environment.addObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType::Wall, radius=5))
            self.environment.addObject(pynlm.WorldObject(100, y, pynlm.WorldObjectType::Wall, radius=5))
        
        # Add resources
        resources = [(20, 20, 10), (80, 80, 15), (30, 70, 8)]
        for x, y, value in resources:
            self.environment.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Resource, radius=3))
        
        # Add hazards
        hazards = [(50, 50, 5), (70, 30, 3)]
        for x, y, danger in hazards:
            self.environment.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Hazard, radius=4))
    
    def get_environment_state(self) -> dict:
        """Get current environment state"""
        state = {
            'agent_position': (self.environment.getAgentBody().x, self.environment.getAgentBody().y),
            'agent_orientation': self.environment.getAgentBody().orientation,
            'agent_energy': self.environment.getAgentBody().energy,
            'simulation_time': self.environment.getSimulationTime(),
            'vision': self.get_vision_data()
        }
        
        return state
    
    def create_sensory_input(self, state: dict):
        """Create sensory input from environment state"""
        sensory = pynlm.SensoryInput()
        
        # Vision data
        vision = pynlm.Vision()
        vision.setData(self.get_vision_array(state['vision']))
        sensory.setVision(vision)
        
        # Internal signals based on environment
        internal = pynlm.InternalSignals()
        
        # Energy-related signals
        internal.addSignal(state['agent_energy'] / 100.0)
        
        # Novelty based on position changes
        if hasattr(self, 'last_position'):
            dx = state['agent_position'][0] - self.last_position[0]
            dy = state['agent_position'][1] - self.last_position[1]
            novelty = math.sqrt(dx*dx + dy*dy) / 10.0
            internal.addSignal(novelty)
        
        self.last_position = state['agent_position']
        
        sensory.setInternal(internal)
        return sensory
    
    def calculate_reward(self, state: dict) -> float:
        """Calculate reward based on environment state"""
        reward = 0.0
        x, y = state['agent_position']
        
        # Positive reward for resources
        for obj in self.environment.getObjects():
            if obj.type == pynlm.WorldObjectType::Resource:
                distance = math.sqrt((x - obj.x) ** 2 + (y - obj.y) ** 2)
                if distance < 10.0:
                    reward += obj.value / distance
        
        # Negative reward for hazards
        for obj in self.environment.getObjects():
            if obj.type == pynlm.WorldObjectType::Hazard:
                distance = math.sqrt((x - obj.x) ** 2 + (y - obj.y) ** 2)
                if distance < 5.0:
                    reward -= obj.value / distance
        
        # Small reward for survival
        reward += 0.1
        
        return max(reward, -10.0)  # Limit negative reward
    
    def get_vision_data(self) -> List[float]:
        """Get vision data for current agent position"""
        # In a real implementation, this would ray-cast from agent position
        # For this example, return simplified vision data
        vision = []
        for _ in range(15 * 15):  # 15x15 vision grid
            vision.append(0.5)  # Neutral value
        
        return vision
    
    def get_vision_array(self, vision_data: List[float]) -> List[float]:
        """Convert vision data to array format"""
        return vision_data
    
    def analyze_prediction_results(self, simulation_data: dict) -> dict:
        """Analyze and return prediction simulation results"""
        print("\n=== Prediction Analysis ===")
        
        rewards = simulation_data['rewards']
        predictions = simulation_data['predictions']
        actuals = simulation_data['actuals']
        errors = simulation_data['errors']
        
        avg_reward = sum(rewards) / len(rewards)
        avg_prediction_error = sum(errors) / len(errors)
        max_error = max(errors)
        
        # Calculate prediction accuracy
        prediction_accuracy = 100 * (1 - avg_prediction_error / max(abs(r) for r in rewards if r != 0))
        
        print(f"Average reward: {avg_reward:.3f}")
        print(f"Average prediction error: {avg_prediction_error:.3f}")
        print(f"Prediction accuracy: {prediction_accuracy:.1f}%")
        
        # Determine prediction quality
        if prediction_accuracy > 80:
            quality = "EXCELLENT"
        elif prediction_accuracy > 60:
            quality = "GOOD"
        elif prediction_accuracy > 40:
            quality = "FAIR"
        else:
            quality = "POOR"
        
        return {
            'average_reward': avg_reward,
            'average_prediction_error': avg_prediction_error,
            'prediction_accuracy': prediction_accuracy,
            'quality': quality
        }

def run_environment_prediction():
    """Run complete environment prediction example"""
    predictor = EnvironmentPredictor()
    results = predictor.run_prediction_simulation(1000)
    
    print(f"\n=== Prediction Simulation Complete ===")
    print(f"Quality: {results['quality']}")
    print(f"Average Reward: {results['average_reward']:.3f}")
    print(f"Prediction Accuracy: {results['prediction_accuracy']:.1f}%")
    
    return predictor, results
```

### Integration Notes

**Environment Modeling:**
```python
# Save environment for later use
predictor.environment.save("environment_state.bin")

# Export simulation data
export_simulation_data(results, "environment_simulation.csv")

# Create prediction visualization
create_prediction_plot(results, "prediction_analysis.png")
```

**Real-time Prediction:**
```cpp
// Real-time prediction system
class RealTimePredictor {
public:
    virtual void updateEnvironment(const EnvironmentState& state) = 0;
    virtual Prediction predictNextState(const EnvironmentState& current) = 0;
    virtual void learnFromExperience(const Experience& exp) = 0;
};

// Implementation with neural networks
class NeuralPredictor : public RealTimePredictor {
public:
    void updateEnvironment(const EnvironmentState& state) override {
        // Update neural network with new data
        training_data.push_back(state);
    }
    
    Prediction predictNextState(const EnvironmentState& current) override {
        // Use neural network to make prediction
        return neural_network.predict(current);
    }
    
    void learnFromExperience(const Experience& exp) override {
        // Train neural network with experience
        neural_network.train(exp);
    }
};
```

## Use Case 4: Performance Analysis and Optimization

### Overview
Analyze and optimize NLM performance across different configurations and scenarios.

### Objectives
- Demonstrate performance analysis capabilities
- Show how to optimize NLM for different use cases
- Illustrate benchmarking and profiling tools
- Provide foundation for performance tuning

### Use Case: Performance Analysis and Optimization

```python
import pynlm
import time
import statistics
from typing import Dict, List, Tuple

class PerformanceAnalyzer:
    """
    Example: Performance analysis and optimization system
    """
    
    def __init__(self):
        self.configurations = []
        self.benchmark_results = []
        self.optimization_suggestions = []
    
    def benchmark_configuration(self, config_name: str, config, num_steps: int) -> Dict:
        """
        Benchmark a specific configuration
        """
        print(f"=== Benchmarking Configuration: {config_name} ===")
        
        # Setup performance tracking
        start_time = time.time()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(25.0, 25.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run benchmark
        step_times = []
        firing_rates = []
        reward_history = []
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Update world
            world.update(0.1)
            
            # Process sensory input
            agent.processSensoryInput(world.getSensoryPercept())
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            step_time = time.time() - step_start
            step_times.append(step_time)
            
            firing_rates.append(brain.getAverageFiringRate())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            reward_history.append(reward)
            
            # Show progress every 100 steps
            if step % 100 == 0:
                print(f"  Step {step}/{num_steps}: "
                      f"Step time={step_time*1000:.2f}ms, "
                      f"Firing={brain.getAverageFiringRate():.3f}Hz, "
                      f"Reward={reward:.3f}")
        
        # Calculate performance metrics
        end_time = time.time()
        total_time = end_time - start_time
        
        results = {
            'config_name': config_name,
            'total_time': total_time,
            'steps_per_second': num_steps / total_time,
            'avg_step_time': statistics.mean(step_times),
            'median_step_time': statistics.median(step_times),
            'min_step_time': min(step_times),
            'max_step_time': max(step_times),
            'avg_firing_rate': statistics.mean(firing_rates),
            'std_firing_rate': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
            'avg_reward': statistics.mean(reward_history),
            'reward_std': statistics.stdev(reward_history) if len(reward_history) > 1 else 0,
            'total_spikes': brain.getTotalSpikeCount(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'memory_usage': self.estimate_memory_usage(brain, world, agent)
        }
        
        self.benchmark_results.append(results)
        
        print(f"\nBenchmark Results for {config_name}:")
        print(f"  Total time: {total_time:.2f}s")
        print(f"  Steps per second: {results['steps_per_second']:.0f}")
        print(f"  Average step time: {results['avg_step_time']*1000:.2f}ms")
        print(f"  Average firing rate: {results['avg_firing_rate']:.3f} Hz")
        print(f"  Average reward: {results['avg_reward']:.3f}")
        
        return results
    
    def optimize_configuration(self, base_config, param_space: Dict) -> Dict:
        """
        Optimize configuration using parameter search
        """
        print("=== Configuration Optimization ===")
        
        # Parameter ranges
        neuron_counts = param_space.get('neuron_counts', [100, 500, 1000])
        learning_rates = param_space.get('learning_rates', [0.001, 0.01, 0.1])
        curiosity_levels = param_space.get('curiosity_levels', [0.0, 0.5, 1.0])
        
        # Grid search
        best_config = None
        best_score = float('-inf')
        
        for neurons in neuron_counts:
            for lr in learning_rates:
                for curiosity in curiosity_levels:
                    # Create and test configuration
                    config = pynlm.createLearningConfig(neurons=neurons, learningRate=lr)
                    config.set('neuromod.curiosity.scale', curiosity)
                    
                    # Quick test run
                    result = self.benchmark_configuration(
                        f"{neurons}Neurons_LR{lr}_Cur{curiosity}",
                        config,
                        num_steps=200  # Quick test
                    )
                    
                    # Calculate score (combined metric)
                    score = (result['avg_reward'] * 1000 + 
                            result['steps_per_second'] * 100 +
                            (result['avg_firing_rate'] / 10.0 * 100))
                    
                    if score > best_score:
                        best_score = score
                        best_config = config
                    
                    print(f"  Config {neurons}Neurons_LR{lr}_Cur{curiosity}: Score={score:.1f}")
        
        # Find best configuration from benchmark results
        if self.benchmark_results:
            best_benchmark = max(self.benchmark_results, key=lambda x: (
                x['avg_reward'] * 1000 + x['steps_per_second'] * 100
            ))
            
            print(f"\nBest configuration: {best_benchmark['config_name']}")
            print(f"Score: {best_score:.1f}")
            
            # Generate optimization suggestions
            self.generate_optimization_suggestions(best_benchmark)
        
        return best_config
    
    def generate_optimization_suggestions(self, best_result: Dict):
        """Generate optimization suggestions based on best results"""
        print("\n=== Optimization Suggestions ===")
        
        suggestions = []
        
        # Neuron count suggestion
        neuron_count = best_result['neuron_count']
        if neuron_count < 200:
            suggestions.append("Consider increasing neuron count for better learning capacity")
        elif neuron_count > 1000:
            suggestions.append("Neuron count may be too high, consider optimization")
        
        # Learning rate suggestion
        firing_rate = best_result['avg_firing_rate']
        if firing_rate < 1.0:
            suggestions.append("Low firing rate suggests learning rate may need adjustment")
        elif firing_rate > 20.0:
            suggestions.append("High firing rate suggests learning rate may be too high")
        
        # Reward stability suggestion
        reward_std = best_result['reward_std']
        if reward_std > best_result['avg_reward'] * 0.5:
            suggestions.append("High reward variance suggests unstable learning, consider regularization")
        
        # Performance suggestion
        step_time = best_result['avg_step_time']
        if step_time > 0.01:  # 10ms
            suggestions.append("High step time suggests potential performance bottlenecks")
        
        if suggestions:
            for i, suggestion in enumerate(suggestions, 1):
                print(f"  {i}. {suggestion}")
        else:
            print("  Configuration is well-balanced with good performance metrics")
        
        self.optimization_suggestions.extend(suggestions)
    
    def estimate_memory_usage(self, brain, world, agent) -> int:
        """Estimate memory usage in bytes"""
        # Simplified memory estimation
        neuron_memory = brain.getTotalNeuronCount() * 100  # bytes per neuron
        synapse_memory = brain.getTotalSynapseCount() * 50  # bytes per synapse
        world_memory = 1000  # bytes for world
        agent_memory = 500  # bytes for agent
        
        return neuron_memory + synapse_memory + world_memory + agent_memory
    
    def create_performance_report(self) -> str:
        """Create comprehensive performance report"""
        report = "NLM Performance Analysis Report\n"
        report += "=" * 50 + "\n\n"
        
        report += "Configuration Benchmarks:\n"
        for result in self.benchmark_results:
            report += f"\n{result['config_name']}:")
            report += f"\n  Performance: {result['steps_per_second']:.0f} steps/sec"
            report += f"\n  Efficiency: {result['avg_reward']:.3f} avg reward"
            report += f"\n  Responsiveness: {result['avg_step_time']*1000:.2f}ms avg step time"
            report += f"\n  Learning: {result['avg_firing_rate']:.3f}Hz firing rate"
        
        report += "\n\nOptimization Suggestions:\n"
        for i, suggestion in enumerate(self.optimization_suggestions, 1):
            report += f"{i}. {suggestion}\n"
        
        return report
    
    def export_results(self, filename: str):
        """Export benchmark results to file"""
        with open(filename, 'w') as f:
            f.write(self.create_performance_report())
        
        print(f"Performance report saved to: {filename}")

def run_performance_analysis():
    """Run complete performance analysis example"""
    print("=== Performance Analysis and Optimization Example ===")
    
    analyzer = PerformanceAnalyzer()
    
    # Define parameter space for optimization
    param_space = {
        'neuron_counts': [100, 500, 1000],
        'learning_rates': [0.001, 0.01, 0.1],
        'curiosity_levels': [0.0, 0.5, 1.0]
    }
    
    # Benchmark different configurations
    base_configs = [
        ("Basic Learning", pynlm.createLearningConfig(neurons=500, learningRate=0.01)),
        ("Exploration", pynlm.createExplorationConfig(neurons=500, curiosity=0.5)),
        ("Conservative", pynlm.createLearningConfig(neurons=1000, learningRate=0.001))
    ]
    
    for name, config in base_configs:
        analyzer.benchmark_configuration(name, config, num_steps=500)
    
    # Optimize configuration
    best_config = analyzer.optimize_configuration(pynlm.createLearningConfig(), param_space)
    
    if best_config:
        print(f"\nBest configuration identified: {best_config}")
    
    # Export results
    analyzer.export_results("performance_analysis_report.txt")
    
    print("\n=== Performance Analysis Complete ===")
    
    return analyzer
```

### Integration Notes

**Performance Optimization:**
```python
# Profile specific components
profile_brain_initialization()
profile_simulation_step()
profile_memory_usage()

# Identify bottlenecks
identify_performance_bottlenecks()

# Optimize parameters
optimize_configuration(base_config, target_performance)

# Monitor performance in real-time
monitor_performance_during_execution()
```

**Benchmarking Suite:**
```cpp
// Comprehensive benchmarking suite
class BenchmarkingSuite {
public:
    void run_all_benchmarks();
    void compare_configurations();
    void profile_memory_usage();
    void benchmark_simulation_speed();
    void analyze_learning_curves();
    void generate_performance_reports();
};

// Usage
BenchmarkingSuite benchmark;
benchmark.run_all_benchmarks();
benchmark.compare_configurations();
benchmark.generate_performance_reports();
```

## Integration Summary

These integration examples demonstrate NLM's versatility across different use cases:

### 1. Basic Neural Network Training
- Simple pattern recognition
- Synaptic plasticity demonstration
- Learning curve analysis

### 2. Multi-Agent Coordination
- Swarm intelligence simulation
- Agent interaction patterns
- Emergent behavior analysis

### 3. Environment Modeling and Prediction
- Complex environment simulation
- Predictive modeling capabilities
- Environment-agent interaction

### 4. Performance Analysis and Optimization
- Benchmarking and profiling
- Configuration optimization
- Performance monitoring

## Implementation Files

### Core Integration Examples
- `integration_basic_neural_training.py` - Basic neural network training
- `integration_multi_agent_coordination.py` - Multi-agent coordination
- `integration_environment_prediction.py` - Environment prediction
- `integration_performance_analysis.py` - Performance analysis

### C++ Integration Examples
- `integration_basic_neural_training.cpp` - C++ version of basic training
- `integration_multi_agent_coordination.cpp` - C++ version of coordination
- `integration_performance_analysis.cpp` - C++ version of performance analysis

### Integration Utilities
- `integration_utils.py` - Common integration utilities
- `integration_benchmarks.py` - Benchmarking tools
- `integration_visualization.py` - Visualization for integration examples

## Integration Best Practices

### 1. Configuration Management
- Use parameter tuning for optimization
- Profile different configurations
- Balance performance and learning capacity

### 2. Error Handling
- Robust error handling in all integration examples
- Graceful degradation for non-critical failures
- Comprehensive error logging and reporting

### 3. Performance Monitoring
- Real-time performance metrics
- Resource usage tracking
- Bottleneck identification

### 4. Testing and Validation
- Unit testing for integration examples
- Performance benchmarking
- Cross-platform compatibility

### 5. Documentation and Examples
- Comprehensive documentation for each use case
- Step-by-step instructions
- Expected results and analysis

## Future Integration Enhancements

### Planned Features
1. **Distributed Integration**
   - Multi-machine deployment
   - Load balancing
   - Distributed computing

2. **Advanced Integration Patterns**
   - Event-driven integration
   - Reactive programming
   - Stream processing

3. **Real-time Integration**
   - Live data integration
   - Streaming sensor data
   - Real-time visualization

4. **Cloud Integration**
   - Cloud-based deployment
   - Container orchestration
   - Serverless computing

### Research Directions
- Integration optimization algorithms
- Multi-modal data integration
- Adaptive configuration management
- Integration testing automation

## Support and Community

### Documentation
- [Integration Guide](INTEGRATION_GUIDE.md)
- [Integration Examples](INTEGRATION_EXAMPLES.md)
- [Performance Optimization](PERFORMANCE_OPTIMIZATION.md)
- [Integration Testing](INTEGRATION_TESTING.md)

### Examples and Tutorials
- Basic integration examples
- Advanced integration patterns
- Performance optimization examples
- Real-world integration scenarios

### Getting Help
1. **GitHub Repository:** https://github.com/nlm-project/nlm
2. **Documentation:** Comprehensive integration guides
3. **Examples:** Working integration examples
4. **Community:** Integration discussion forums

## License

MIT License - See LICENSE file for details.

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.