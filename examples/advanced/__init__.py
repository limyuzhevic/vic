# Advanced NLM Examples

This directory contains advanced examples for expert users of the NLM Python API. These examples demonstrate complex usage patterns, optimization techniques, and advanced features.

## Structure

```
examples/
├── advanced/
│   ├── brain_learning/
│   │   ├── reinforcement_learning.py
│   │   ├── behavior_adaptation.py
│   │   └── developmental_stages.py
│   ├── multi_agent/
│   │   ├── competition.py
│   │   ├── cooperation.py
│   │   └── swarm_intelligence.py
│   ├── neural_architectures/
│   │   ├── recurrent_network.py
│   │   ├── sparse_connections.py
│   │   └── modular_brain.py
│   ├── visualization/
│   │   ├── neural_activity.py
│   │   ├── memory_traces.py
│   │   └── development_tracking.py
│   └── performance_optimization/
│       ├── memory_pools.py
│       ├── vectorization.py
│       └── profiling.py
├── tutorials/
│   ├── step1_basic_usage.md
│   ├── step2_advanced_control.md
│   ├── step3_cognitive_learning.md
│   ├── step4_advanced_optimization.md
│   └── step5_research_protocols.md
└── recipes/
    ├── common_patterns.py
    ├── troubleshooting.py
    └── configuration_templates.py
```

## Advanced Examples

### 1. Reinforcement Learning with NLM

This example demonstrates how to implement a reinforcement learning system using NLM's brain, agent, and world components.

```python
# examples/advanced/brain_learning/reinforcement_learning.py
import pynlm
import numpy as np
import time

class ReinforcementLearningAgent:
    """Reinforcement learning agent using NLM brain."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createCustomConfig(
            neuron_count=2000,
            connection_probability=0.15,
            enable_reward_modulation=True,
            enable_curiosity=True
        )
        
        # Create brain, agent, and world
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.world.setAgentStart(15.0, 15.0)
        
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # RL state
        self.state_history = []
        self.action_history = []
        self.reward_history = []
        self.episode_rewards = []
        
    def getStateFromPercept(self, percept):
        """Extract relevant features from sensory percept."""
        vision = percept.getVision()
        internal = percept.getInternal()
        
        features = []
        
        # Vision features (greyscale average)
        if vision:
            vision_array = vision.getData()
            if vision_array:
                features.extend(vision_array)
        
        # Internal signal features
        if internal:
            features.extend(internal)
        
        # Agent position and orientation
        body = self.world.getAgentBody()
        features.extend([body.x, body.y, body.orientation])
        
        return np.array(features, dtype=np.float32)
    
    def runEpisode(self, max_steps=500):
        """Run one episode of reinforcement learning."""
        self.state_history = []
        self.action_history = []
        self.reward_history = []
        
        episode_reward = 0.0
        
        for step in range(max_steps):
            # Get current state
            percept = self.world.getSensoryPercept()
            state = self.getStateFromPercept(percept)
            self.state_history.append(state)
            
            # Update world
            self.world.update(0.1)
            
            # Agent processes sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.action_history.append(action)
            
            # Apply action in world
            if action:
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward from world
            current_percept = self.world.getSensoryPercept()
            internal_signals = current_percept.getInternal() if current_percept else []
            reward = internal_signals[0] if internal_signals else 0.0
            
            self.reward_history.append(reward)
            episode_reward += reward
            
            # Apply reward modulation to brain
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Check if episode should end
            if self.world.getAgentBody().energy <= 0:
                break
        
        self.episode_rewards.append(episode_reward)
        
        return episode_reward
    
    def train(self, num_episodes=100):
        """Train the agent using reinforcement learning."""
        print(f"Starting reinforcement learning training for {num_episodes} episodes...")
        
        for episode in range(num_episodes):
            episode_reward = self.runEpisode()
            
            # Print progress
            if (episode + 1) % 10 == 0:
                avg_reward = np.mean(self.episode_rewards[-10:])
                print(f"Episode {episode + 1}/{num_episodes}, "
                      f"Reward: {episode_reward:.2f}, "
                      f"Avg Reward (last 10): {avg_reward:.2f}")
                
                # Save checkpoint if performance improves
                if episode_reward > avg_reward and episode > 0:
                    checkpoint_file = f"checkpoints/rl_agent_ep{episode + 1}.bin"
                    self.brain.save(checkpoint_file)
                    print(f"  → Saved checkpoint: {checkpoint_file}")
        
        # Save final trained agent
        self.brain.save("checkpoints/final_trained_agent.bin")
        print("Training completed. Final agent saved.")
        
        return self.episode_rewards

# Example usage
if __name__ == "__main__":
    # Create and train agent
    agent = ReinforcementLearningAgent()
    
    # Train for 200 episodes
    rewards = agent.train(200)
    
    # Analyze results
    print(f"\nTraining Results:")
    print(f"Total episodes: {len(rewards)}")
    print(f"Average reward: {np.mean(rewards):.2f}")
    print(f"Best reward: {np.max(rewards):.2f}")
    print(f"Final reward: {rewards[-1]:.2f}")
    
    # Visualize learning curve (requires matplotlib)
    try:
        import matplotlib.pyplot as plt
        plt.figure(figsize=(10, 6))
        plt.plot(rewards)
        plt.title("Reinforcement Learning Performance")
        plt.xlabel("Episode")
        plt.ylabel("Total Reward")
        plt.grid(True)
        plt.savefig("rl_training_curve.png")
        plt.close()
        print("Learning curve saved as rl_training_curve.png")
    except ImportError:
        print("Install matplotlib to generate learning curve")
```

### 2. Multi-Agent Competition

This example demonstrates multiple agents competing in the same environment.

```python
# examples/advanced/multi_agent/competition.py
import pynlm
import numpy as np
from threading import Thread
import time

class CompetitionAgent:
    """Agent participating in competition."""
    
    def __init__(self, agent_id, world, start_position):
        self.agent_id = agent_id
        self.world = world
        self.config = pynlm.createCustomConfig(
            neuron_count=1500,
            connection_probability=0.12,
            enable_curiosity=True,
            enable_reward_modulation=True
        )
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(world)
        
        # Set initial position
        self.agent_body = self.world.getAgentBody()
        self.agent_body.x = start_position[0]
        self.agent_body.y = start_position[1]
        self.agent_body.orientation = np.random.uniform(0, 2 * np.pi)
        
        self.reward_history = []
        
    def processStep(self):
        """Process one step of the agent's competition."""
        # Update world
        self.world.update(0.1)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Brain thinks
        self.brain.step(self.world.getSimulationTime())
        
        # Get action
        action = self.agent.decodeMotorCommand()
        
        # Apply action
        if action:
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Get reward
        internal = percept.getInternal() if percept else []
        reward = internal[0] if internal else 0.0
        self.reward_history.append(reward)
        
        return reward

class CompetitionEnvironment:
    """Environment hosting multiple competing agents."""
    
    def __init__(self, width=50, height=50, num_agents=3):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agents = []
        self.agent_ids = list(range(num_agents))
        
        # Create agents at random positions
        import random
        for i, agent_id in enumerate(self.agent_ids):
            start_x = random.uniform(5, width - 5)
            start_y = random.uniform(5, height - 5)
            agent = CompetitionAgent(agent_id, self.world, (start_x, start_y))
            self.agents.append(agent)
        
        self.step_count = 0
        self.competition_results = []
        
    def runCompetition(self, max_steps=1000):
        """Run the competition between agents."""
        print(f"Starting competition with {len(self.agents)} agents...")
        
        for step in range(max_steps):
            step_rewards = []
            
            # All agents take actions simultaneously
            for agent in self.agents:
                reward = agent.processStep()
                step_rewards.append(reward)
            
            self.step_count += 1
            
            # Print status periodically
            if step % 100 == 0:
                print(f"Step {step}: " + ", ".join(
                    [f"Agent {i}: {reward:.2f}" for i, reward in enumerate(step_rewards)]))
            
            # Check if competition should end
            if self.step_count >= max_steps:
                break
        
        # Determine competition results
        self.calculateResults()
        return self.competition_results
    
    def calculateResults(self):
        """Calculate final competition results."""
        for i, agent in enumerate(self.agents):
            total_reward = sum(agent.reward_history)
            avg_reward = total_reward / len(agent.reward_history) if agent.reward_history else 0
            final_position = (agent.world.getAgentBody().x, agent.world.getAgentBody().y)
            
            result = {
                'agent_id': i,
                'total_reward': total_reward,
                'average_reward': avg_reward,
                'final_position': final_position,
                'steps_survived': len(agent.reward_history)
            }
            self.competition_results.append(result)
    
    def getRanking(self):
        """Get agents ranked by performance."""
        return sorted(self.competition_results, 
                     key=lambda x: x['total_reward'], reverse=True)
    
    def printResults(self):
        """Print competition results in a formatted way."""
        print("\n=== Competition Results ===")
        ranking = self.getRanking()
        
        for rank, result in enumerate(ranking, 1):
            print(f"Rank {rank}:")
            print(f"  Agent ID: {result['agent_id']}")
            print(f"  Total Reward: {result['total_reward']:.2f}")
            print(f"  Average Reward: {result['average_reward']:.2f}")
            print(f"  Final Position: ({result['final_position'][0]:.1f}, {result['final_position'][1]:.1f})")
            print(f"  Steps Survived: {result['steps_survived']}")
            print()

# Example usage
if __name__ == "__main__":
    # Create competition environment
    env = CompetitionEnvironment(width=100, height=100, num_agents=5)
    
    # Run competition
    results = env.runCompetition(max_steps=500)
    
    # Print detailed results
    env.printResults()
    
    # Save competition data
    import json
    with open("competition_results.json", 'w') as f:
        json.dump(results, f, indent=2)
    print("Results saved to competition_results.json")
```

### 3. Neural Architecture Search

This example demonstrates different neural architectures and their effects on learning.

```python
# examples/advanced/neural_architectures/modular_brain.py
import pynlm
import numpy as np

class ModularBrain:
    """Brain with modular architecture (sensory, cognitive, motor modules)."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createCustomConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create specialized regions for different functions
        self.createModularArchitecture()
        
    def createModularArchitecture(self):
        """Create modular brain architecture."""
        # Clear existing regions
        while self.brain.getRegionCount() > 0:
            # Remove all regions except the first one
            if self.brain.getRegionCount() > 1:
                region_ids = self.brain.getRegionIds()
                self.brain.getRegion(region_ids[-1])
                # Note: This would need a removeRegion method
        
        # Create specialized populations
        config = self.brain.getConfig()
        neuron_count = config.getOr("brain.neuron_count", 1000)
        
        # Sensory module (40% neurons)
        sensory_neurons = int(neuron_count * 0.4)
        sensory_region = self.brain.getRegion(pynlm.RegionId(1))
        if sensory_region:
            sensory_region.addPopulation(sensory_neurons, pynlm.NeuronType.Sensory)
        
        # Cognitive module (40% neurons)
        cognitive_neurons = int(neuron_count * 0.4)
        cognitive_region = self.brain.addRegion("Cognitive")
        if cognitive_region:
            cognitive_region.addPopulation(cognitive_neurons, pynlm.NeuronType.Internal)
        
        # Motor module (20% neurons)
        motor_neurons = int(neuron_count * 0.2)
        motor_region = self.brain.addRegion("Motor")
        if motor_region:
            motor_region.addPopulation(motor_neurons, pynlm.NeuronType.Motor)
    
    def processSensoryInput(self, input_data):
        """Process sensory input through sensory module."""
        self.brain.receiveSensoryInput(input_data)
    
    def getCognitionOutput(self):
        """Get cognitive processing output."""
        return self.brain.produceAction()
    
    def step(self, step_number):
        """Process one simulation step."""
        self.brain.step(step_number)

class RecurrentNetworkBrain:
    """Brain with recurrent connections for temporal processing."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createCustomConfig(
            brain.neuron_count=2000,
            brain.connection_probability=0.08  # Lower density for recurrent networks
        )
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enhance recurrent connections
        self.enhanceRecurrentConnections()
        
    def enhanceRecurrentConnections(self):
        """Enhance recurrent connections between cognitive neurons."""
        # This would involve adding inter-region connections
        # and modifying connection weights for temporal dynamics
        pass
    
    def processSequence(self, sequence_inputs):
        """Process a sequence of inputs (recurrent processing)."""
        outputs = []
        
        for i, input_data in enumerate(sequence_inputs):
            self.processSensoryInput(input_data)
            action = self.getCognitionOutput()
            outputs.append(action)
            
            # Short-term memory via recurrent connections
            self.brain.step(i)
        
        return outputs
    
    def processSensoryInput(self, input_data):
        """Process sensory input."""
        self.brain.receiveSensoryInput(input_data)
    
    def getCognitionOutput(self):
        """Get cognitive output."""
        return self.brain.produceAction()
    
    def step(self, step_number):
        """Process one step."""
        self.brain.step(step_number)

class SparseConnectionBrain:
    """Brain with sparse, efficient connections."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createCustomConfig(
            brain.neuron_count=3000,
            brain.connection_probability=0.03  # Very sparse
        )
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create highly specialized modules
        self.createSparseArchitecture()
        
    def createSparseArchitecture(self):
        """Create architecture with sparse, focused connections."""
        # Many small, specialized regions
        for i in range(10):  # 10 specialized modules
            region_name = f"Module_{i}"
            neurons = 300  # 300 neurons per module
            
            # Each module handles specific functionality
            if i == 0:
                neuron_type = pynlm.NeuronType.Sensory
            elif i == 1:
                neuron_type = pynlm.NeuronType.Internal
            elif i == 2:
                neuron_type = pynlm.NeuronType.Motor
            else:
                # Mixed types for other modules
                neuron_type = pynlm.NeuronType.Internal
            
            region = self.brain.addRegion(region_name)
            if region:
                region.addPopulation(neurons, neuron_type)
    
    def addCrossModuleConnections(self, source_module, target_module, weight=0.5):
        """Add connections between modules for integration."""
        # Implementation would add inter-region connections
        pass
    
    def processComplexTask(self, task_input):
        """Process complex task using modular sparse network."""
        # Process through different modules sequentially
        # or in parallel for complex computations
        pass

# Example usage
if __name__ == "__main__":
    # Compare different architectures
    architectures = {
        'Modular': ModularBrain(),
        'Recurrent': RecurrentNetworkBrain(),
        'Sparse': SparseConnectionBrain()
    }
    
    print("Neural Architecture Comparison")
    print("=" * 50)
    
    for name, architecture in architectures.items():
        print(f"\n{name} Architecture:")
        print(f"  Region count: {architecture.brain.getRegionCount()}")
        print(f"  Total neurons: {architecture.brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {architecture.brain.getTotalSynapseCount()}")
        print(f"  Average firing rate: {architecture.brain.getAverageFiringRate():.3f}")
```

### 4. Performance Optimization Tools

This example demonstrates performance optimization techniques.

```python
# examples/advanced/performance_optimization/profiling.py
import pynlm
import time
import cProfile
import pstats
import io
from contextlib import contextmanager
import matplotlib.pyplot as plt

class PerformanceProfiler:
    """Performance profiling and optimization tools for NLM."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.profiles = {}
        self.benchmarks = {}
        
    @contextmanager
    def time_operation(self, operation_name):
        """Context manager to time operations."""
        start_time = time.time()
        yield
        end_time = time.time()
        self.benchmarks[operation_name] = end_time - start_time
    
    def profileBrainStep(self, num_steps=100):
        """Profile brain stepping performance."""
        print(f"Profiling brain step with {num_steps} steps...")
        
        pr = cProfile.Profile()
        pr.enable()
        
        start_time = time.time()
        for step in range(num_steps):
            self.brain.step(step)
        end_time = time.time()
        
        pr.disable()
        
        # Store profile
        self.profiles['brain_step'] = pr
        
        # Print summary
        s = io.StringIO()
        ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
        ps.print_stats(20)  # Top 20 time-consuming functions
        print("Profile summary:")
        print(s.getvalue())
        
        return end_time - start_time
    
    def profileCompleteSimulation(self, num_steps=500):
        """Profile complete simulation."""
        print(f"Profiling complete simulation with {num_steps} steps...")
        
        pr = cProfile.Profile()
        pr.enable()
        
        start_time = time.time()
        for step in range(num_steps):
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            if action:
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        end_time = time.time()
        
        pr.disable()
        
        self.profiles['complete_simulation'] = pr
        
        return end_time - start_time
    
    def profileMemoryUsage(self, operation_name, func, *args, **kwargs):
        """Profile memory usage of an operation."""
        try:
            import memory_profiler
            
            @memory_profiler.profile
            def wrapper():
                return func(*args, **kwargs)
            
            result = wrapper()
            self.benchmarks[f'{operation_name}_memory'] = memory_profiler.memory_usage(-1, interval=0.1)
            return result
            
        except ImportError:
            print("Install memory_profiler to profile memory usage")
            return func(*args, **kwargs)
    
    def compareConfigurations(self, configs, steps_per_config=100):
        """Compare performance of different configurations."""
        print("Comparing different configurations...")
        
        results = {}
        
        for config_name, config in configs.items():
            print(f"\nTesting {config_name}:")
            
            # Create fresh brain for each test
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time the simulation
            start_time = time.time()
            for step in range(steps_per_config):
                brain.step(step)
            end_time = time.time()
            
            duration = end_time - start_time
            results[config_name] = {
                'duration': duration,
                'steps_per_second': steps_per_config / duration,
                'neuron_count': config.getOr('brain.neuron_count', 0),
                'connection_probability': config.getOr('brain.connection_probability', 0)
            }
            
            print(f"  Duration: {duration:.2f}s")
            print(f"  Speed: {results[config_name]['steps_per_second']:.0f} steps/sec")
        
        return results
    
    def visualizePerformance(self, benchmarks):
        """Create performance visualizations."""
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 5))
        
        # Time comparison
        operations = list(benchmarks.keys())
        times = [benchmarks[op] for op in operations]
        
        ax1.bar(range(len(operations)), times)
        ax1.set_xticks(range(len(operations)))
        ax1.set_xticklabels(operations, rotation=45)
        ax1.set_title('Operation Times')
        ax1.set_ylabel('Time (seconds)')
        
        # Speed comparison (if available)
        if any('speed' in str(v) for v in benchmarks.values()):
            speeds = []
            for op, data in benchmarks.items():
                if isinstance(data, dict) and 'steps_per_second' in data:
                    speeds.append(data['steps_per_second'])
                elif isinstance(data, (int, float)):
                    speeds.append(1.0 / data if data > 0 else 0)
            
            if speeds:
                ax2.bar(range(len(speeds)), speeds)
                ax2.set_xticks(range(len(speeds)))
                ax2.set_xticklabels(operations[:len(speeds)], rotation=45)
                ax2.set_title('Speed (steps/sec or items/sec)')
                ax2.set_ylabel('Speed')
        
        plt.tight_layout()
        plt.savefig('performance_analysis.png')
        plt.close()
        print("Performance visualization saved as performance_analysis.png")
    
    def runCompleteProfile(self, num_steps=200):
        """Run complete performance profiling."""
        print("Running complete performance profile...")
        
        # Profile brain stepping
        brain_time = self.profileBrainStep(num_steps)
        
        # Profile complete simulation
        sim_time = self.profileCompleteSimulation(num_steps)
        
        # Create simple benchmark comparison
        self.benchmarks = {
            'brain_step': brain_time,
            'complete_simulation': sim_time
        }
        
        # Visualize results
        self.visualizePerformance(self.benchmarks)
        
        return {
            'brain_step_time': brain_time,
            'simulation_time': sim_time,
            'speedup': sim_time / brain_time if brain_time > 0 else 0
        }

# Example usage
if __name__ == "__main__":
    # Create test setup
    config = pynlm.createCustomConfig(neuron_count=1000, connection_probability=0.1)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Create profiler
    profiler = PerformanceProfiler(brain, world, agent)
    
    # Run performance profiling
    results = profiler.runCompleteProfile(num_steps=100)
    
    print(f"\nPerformance Results:")
    print(f"Brain step time: {results['brain_step_time']:.3f}s")
    print(f"Simulation time: {results['simulation_time']:.3f}s")
    print(f"Speedup factor: {results['speedup']:.2f}x")
```

## Tutorials

### Step 1: Basic Usage

This tutorial covers the basics of using the NLM Python API.

```markdown
# Step 1: Basic Usage

## What You'll Learn

- Create a brain and initialize it
- Create a world and agent
- Run a simple simulation
- Access brain statistics

## Your First NLM Program

```python
import pynlm

# 1. Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# 2. Create a world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# 3. Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable some features
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# 5. Run a simple simulation
for step in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    if action:
        world.applyMotorCommand(action, world.getSimulationTime())

# 6. Check results
print(f"Simulation completed!")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
```

## Exercises

1. **Try Different Configurations**
   - Modify `brain.neuron_count` and observe effects
   - Change `brain.connection_probability`
   - Try different developmental stages

2. **Explore Agent Behavior**
   - Disable curiosity and see how exploration changes
   - Enable/disable reward modulation
   - Try different action types

3. **World Interaction**
   - Change world dimensions
   - Modify agent starting position
   - Add obstacles to the world

## What You've Accomplished

✅ Created a working NLM brain
✅ Set up environment interaction
✅ Implemented agent behavior
✅ Ran a complete simulation
✅ Accessed brain statistics

## Next Steps

Read [Step 2: Advanced Control](step2_advanced_control.md) to learn about controlling brain development and neuromodulation.
```

### Step 2: Advanced Control

This tutorial covers advanced control mechanisms including development, neuromodulation, and plasticity.

```markdown
# Step 2: Advanced Control

## What You'll Learn

- Control brain development stages
- Use neuromodulation for learning
- Configure plasticity rules
- Save and load brain states

## Advanced NLM Features

```python
import pynlm

# Create brain with advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.development.enabled", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Get access to advanced systems
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
agent.initialize(world)

# Enable advanced features
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)

# Run simulation with development tracking
for step in range(500):
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Update development every 50 steps
    if step % 50 == 0:
        agent.updateDevelopment(0.05)
        
        # Log development progress
        print(f"Step {step}: Developmental Stage = {brain.getDevelopmentalStage()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    if action:
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation based on world feedback
    # (This would depend on your specific environment)
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)

# Save the developed brain
brain.save("developed_brain.bin")
print("Brain saved to developed_brain.bin")

# Load brain for testing
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.initialize()
new_brain.load("developed_brain.bin")
print("Brain loaded successfully")
```

## Advanced Topics

### Development Stages

```python
# Different developmental stages affect brain behavior
stages = pynlm.DevelopmentalStage

for stage in [stages.Initial, stages.CriticalPeriod, 
              stages.Maturation, stages.Adult, stages.Aging]:
    brain.setDevelopmentalStage(stage)
    print(f"Stage: {stage}, Neuron count: {brain.getTotalNeuronCount()}")
```

### Neuromodulation Levels

```python
# Monitor neuromodulation systems
print(f"Dopamine level: {brain.getDopamine().getLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

### Plasticity Configuration

```python
# Configure plasticity rules through config
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
```

## Exercises

1. **Development Experiments**
   - Start with Initial stage and observe development
   - Compare learning speed in CriticalPeriod vs Maturation
   - Test effects of aging on cognitive abilities

2. **Neuromodulation Studies**
   - Vary dopamine levels and observe effects on behavior
   - Test curiosity-driven exploration
   - Study novelty detection

3. **Plasticity Analysis**
   - Enable/disable different plasticity types
   - Measure synaptic changes over time
   - Compare STDP vs Hebbian learning

## What You've Accomplished

✅ Configured advanced brain development
✅ Used neuromodulation systems
✅ Managed plasticity rules
✅ Saved and loaded brain states
✅ Tracked developmental progress

## Next Steps

Read [Step 3: Cognitive Learning](step3_cognitive_learning.md) to learn about memory systems and cognition.
```

## Other Tutorials

- **[Step 3: Cognitive Learning](tutorials/step3_cognitive_learning.md)** - Memory systems and concept formation
- **[Step 4: Advanced Optimization](tutorials/step4_advanced_optimization.md)** - Performance tuning and large-scale simulations
- **[Step 5: Research Protocols](tutorials/step5_research_protocols.md)** - Scientific research methodologies

## Recipes

### Common Patterns

See `examples/recipes/common_patterns.py` for solutions to common NLM usage patterns.

### Troubleshooting

See `examples/recipes/troubleshooting.py` for common issues and their solutions.

### Configuration Templates

See `examples/recipes/configuration_templates.py` for ready-to-use configuration templates.

## Quick Start Summary

For a quick overview of NLM usage, see:

- `easy_usage.md` - Beginner-friendly guide
- `HOW_TO_USE.md` - Comprehensive usage documentation
- `python/api_reference.md` - Complete API reference

The advanced examples and tutorials provide comprehensive coverage of NLM features, from basic usage to advanced research applications.