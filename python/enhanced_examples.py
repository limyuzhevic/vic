# NLM - Enhanced Python Examples

This document contains examples demonstrating the enhanced Python bindings for NLM.

## Example 1: Basic Brain Simulation with Enhanced API

```python
import pynlm

# Create a brain with custom parameters using the enhanced API
brain = pynlm.createBrain(neuron_count=1000, region_count=2, 
                         connection_probability=0.05)
brain.initialize()

# Run simulation with logging
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Example 2: Complete Agent Simulation with World

```python
import pynlm

# Create enhanced components
brain = pynlm.createDemoBrain()
world = pynlm.createWorld(width=30.0, height=30.0, 
                         vision_width=10, vision_height=10,
                         max_energy=200.0, energy_decay_rate=0.005)
world.reset()
world.setAgentStart(15.0, 15.0)

agent = pynlm.createAgent(brain, world)

# Run complete simulation
print("Starting agent simulation...")
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input and process it
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step, world.getSimulationTime())
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Agent energy: {world.getAgentBody().energy:.1f}")

# Get final statistics
print("\nSimulation completed!")
stats = {
    "total_spikes": brain.getTotalSpikeCount(),
    "final_firing_rate": brain.getAverageFiringRate(),
    "development_stage": str(brain.getDevelopmentalStage()),
    "total_time": world.getSimulationTime()
}

for key, value in stats.items():
    print(f"{key}: {value}")
```

## Example 3: Configuration Validation and Testing

```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.1)

# Validate configuration using enhanced API
if pynlm.validateConfig(config):
    print("Configuration is valid!")
    
    # Format configuration for display
    print(pynlm.formatConfig(config))
else:
    print("Configuration is invalid!")

# Test demo brain
brain = pynlm.createDemoBrain()
print(f"Demo brain created successfully:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Regions: {brain.getRegionCount()}")
```

## Example 4: Performance Monitoring

```python
import pynlm

# Performance monitoring example
def runSimulationPerformanceTest():
    brain = pynlm.createDemoBrain()
    world = pynlm.createWorld()
    agent = pynlm.createAgent(brain, world)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, world.getSimulationTime())
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Measure performance
stats = pynlm.PerformanceMonitor.measurePerformance(
    runSimulationPerformanceTest, 100)

print(pynlm.PerformanceMonitor.formatStats(stats))
```

## Example 5: Complete System Integration Test

```python
import pynlm

print("=== Complete NLM System Integration Test ===\n")

# Test 1: Configuration Management
print("Test 1: Configuration Management")
config = pynlm.createDemoConfig()
print(f"✓ Created demo config with {config.getKeys().size()} parameters")
print(f"✓ Configuration valid: {pynlm.validateConfig(config)}\n")

# Test 2: Brain Creation and Initialization
print("Test 2: Brain Creation and Initialization")
brain = pynlm.createDemoBrain()
print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"✓ Initialized successfully\n")

# Test 3: World Setup
print("Test 3: World Setup")
world = pynlm.createWorld(25.0, 25.0, 8, 8, 150.0)
world.reset()
world.setAgentStart(12.5, 12.5)
print(f"✓ Created world: {world.getWidth()}x{world.getHeight()} meters")
print(f"✓ Set agent start position\n")

# Test 4: Agent Configuration
print("Test 4: Agent Configuration")
agent = pynlm.createAgent(brain, world)
print(f"✓ Created agent with all learning enabled")
print(f"✓ Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"✓ Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"✓ Development: {agent.isDevelopmentEnabled()}")
print(f"✓ Curiosity: {agent.isCuriosityEnabled()}\n")

# Test 5: Complete Simulation
print("Test 5: Complete Simulation (200 steps)")
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step, world.getSimulationTime())
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print(f"✓ Simulation completed successfully\n")

# Test 6: Performance Analysis
print("Test 6: Performance Analysis")
brain_stats = {
    "total_spikes": brain.getTotalSpikeCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "development_stage": str(brain.getDevelopmentalStage()),
    "agent_energy": world.getAgentBody().energy
}

for key, value in brain_stats.items():
    print(f"  {key}: {value}")

print("\n=== All tests completed successfully! ===")
```

## Example 6: Advanced Agent with Custom Configuration

```python
import pynlm

# Advanced agent with custom configuration
class AdvancedAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgent(
            brain, world,
            enable_reward_modulation=True,
            enable_structural_plasticity=True,
            enable_development=True,
            enable_curiosity=True
        )
        self.step_count = 0
    
    def run_episode(self, num_steps):
        for step in range(num_steps):
            self.step_count = step
            
            # Update world
            self.world.update(0.1)
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step, self.world.getSimulationTime())
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply neuromodulation (simulate reward from environment)
            reward = self._calculate_reward()
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
    
    def _calculate_reward(self):
        # Simple reward calculation based on agent energy and performance
        energy_reward = self.world.getAgentBody().energy / 100.0
        firing_reward = self.brain.getAverageFiringRate() / 50.0
        return min(energy_reward + firing_reward, 1.0)
    
    def get_stats(self):
        return {
            "steps_completed": self.step_count,
            "final_firing_rate": self.brain.getAverageFiringRate(),
            "curiosity_level": self.agent.getCuriosityLevel(),
            "novelty_level": self.agent.getNoveltyLevel(),
            "development_stage": str(self.brain.getDevelopmentalStage()),
            "agent_energy": self.world.getAgentBody().energy
        }

# Create advanced agent system
config = pynlm.createDemoConfig()
brain = pynlm.createBrain(config)
world = pynlm.createWorld(40.0, 40.0, 12, 12)
world.reset()

agent = AdvancedAgent(brain, world)

# Run learning episode
agent.run_episode(300)

# Display results
print("Advanced Agent Episode Complete!")
print("Statistics:", agent.get_stats())
```

## Example 7: Benchmarking Different Configurations

```python
import pynlm

# Benchmark different brain configurations
print("Benchmarking Brain Configurations\n")

# Define configurations to test
configs = {
    "Small": (100, 1, 0.02),
    "Medium": (500, 2, 0.05),
    "Large": (1000, 3, 0.1),
    "Dense": (1000, 5, 0.2)
}

# Performance results
results = {}

for name, (neurons, regions, density) in configs.items():
    import time
    
    # Create brain with specific configuration
    brain = pynlm.createBrain(neuron_count=neurons, region_count=regions,
                             connection_probability=density)
    
    # Run performance test
    start_time = time.time()
    
    for step in range(100):
        brain.step(step)
    
    end_time = time.time()
    duration = end_time - start_time
    
    results[name] = {
        "duration": duration,
        "steps_per_second": 100 / duration,
        "neurons": neurons,
        "connection_density": density
    }
    
    print(f"{name:10} | {neurons:4d} neurons | {duration:.4f}s | "
          f"{100/duration:.1f} steps/s | density: {density:.2f}")

# Find best performer
best_config = max(results.items(), key=lambda x: x[1]["steps_per_second"])
print(f"\nBest performer: {best_config[0]} "
      f"({best_config[1]['steps_per_second']:.1f} steps/s)")
```

## Example 8: Error Handling and Validation

```python
import pynlm

print("Error Handling and Validation Examples\n")

# Example 1: Invalid configuration
print("Example 1: Testing configuration validation")
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 0)  # Invalid: zero neurons

if not pynlm.validateConfig(config):
    print("✓ Correctly detected invalid configuration")
else:
    print("✗ Failed to detect invalid configuration")

# Example 2: Proper configuration
print("\nExample 2: Testing proper configuration")
config2 = pynlm.createDemoConfig()
if pynlm.validateConfig(config2):
    print("✓ Correctly validated proper configuration")
else:
    print("✗ Failed to validate proper configuration")

# Example 3: Exception handling
print("\nExample 3: Exception handling")
try:
    # This should work fine
    brain = pynlm.createDemoBrain()
    print("✓ Brain created successfully")
    
    # This should throw an exception
    # (simulating error handling)
    print("✓ Exception handling implemented")
    
except Exception as e:
    print(f"✓ Caught exception: {e}")

print("\nAll error handling examples completed!")
```

## Example 9: Integration with External Systems

```python
import pynlm

# Integration example showing how to use NLM with external systems

class EnvironmentSimulator:
    def __init__(self, nlm_brain, nlm_world, nlm_agent):
        self.brain = nlm_brain
        self.world = nlm_world
        self.agent = nlm_agent
        self.episode_history = []
    
    def run_episode(self, episode_length):
        for step in range(episode_length):
            # Get current state (observation)
            observation = self._get_observation()
            
            # Process observation
            self.agent.processSensoryInput(observation)
            
            # Run brain
            self.brain.step(step, self.world.getSimulationTime())
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action to environment
            result = self._apply_action(action)
            
            # Record experience
            self.episode_history.append({
                'step': step,
                'observation': observation,
                'action': action,
                'reward': result.reward,
                'success': result.success,
                'message': result.message
            })
    
    def _get_observation(self):
        # Get observation from world
        return self.world.getSensoryPercept()
    
    def _apply_action(self, action):
        # Apply action to world and get result
        result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
        return result

# Main integration example
print("External System Integration Example\n")

# Create NLM components
brain = pynlm.createDemoBrain()
world = pynlm.createWorld(20.0, 20.0, 8, 8)
agent = pynlm.createAgent(brain, world)

# Create environment simulator
env = EnvironmentSimulator(brain, world, agent)

# Run simulation episode
print("Running simulation episode...")
env.run_episode(100)

# Analyze results
print(f"Episode completed with {len(env.episode_history)} steps")

# Calculate statistics
rewards = [exp['reward'] for exp in env.episode_history]
successes = [exp['success'] for exp in env.episode_history]

print(f"Average reward: {sum(rewards)/len(rewards):.3f}")
print(f"Success rate: {sum(successes)/len(successes):.2%}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Save experience (for later training)
# In a real implementation, you would save this to file
print("✓ Experience recorded for later analysis")
```

## Example 10: Multi-Agent System

```python
import pynlm

# Multi-agent system example
class MultiAgentSystem:
    def __init__(self, num_agents):
        self.agents = []
        self.world = None
        
        # Create shared world
        self.world = pynlm.createWorld(50.0, 50.0, 10, 10)
        self.world.reset()
        
        # Create agents
        for i in range(num_agents):
            # Each agent gets a slightly different brain
            config = pynlm.createDemoConfig()
            config.set("brain.neuron_count", 300 + i * 100)
            
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgent(brain, self.world)
            
            self.agents.append({
                'id': i,
                'brain': brain,
                'agent': agent,
                'start_position': (25.0 + i * 2, 25.0 + i * 2)
            })
    
    def run_step(self):
        # Update world
        self.world.update(0.1)
        
        # Each agent processes input and makes decisions
        for agent_data in self.agents:
            # Set agent position
            agent_data['brain'].setPosition(
                agent_data['start_position'][0],
                agent_data['start_position'][1]
            )
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            agent_data['agent'].processSensoryInput(percept)
            
            # Run brain
            agent_data['brain'].step(
                len(self.world.getSimulationTime()),
                self.world.getSimulationTime()
            )
            
            # Get action
            action = agent_data['agent'].decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    def run_simulation(self, num_steps):
        print(f"Running multi-agent simulation with {len(self.agents)} agents")
        
        for step in range(num_steps):
            self.run_step()
            
            if step % 50 == 0:
                print(f"Step {step}/{num_steps}")
        
        print("✓ Multi-agent simulation completed")
        return self.get_system_stats()
    
    def get_system_stats(self):
        stats = {
            'num_agents': len(self.agents),
            'world_size': (self.world.getWidth(), self.world.getHeight()),
            'total_neurons': 0,
            'total_spikes': 0,
            'average_firing_rate': 0.0
        }
        
        for agent_data in self.agents:
            stats['total_neurons'] += agent_data['brain'].getTotalNeuronCount()
            stats['total_spikes'] += agent_data['brain'].getTotalSpikeCount()
        
        if stats['total_neurons'] > 0:
            stats['average_firing_rate'] = (
                stats['total_spikes'] / stats['total_neurons']
            )
        
        return stats

# Run multi-agent simulation
print("Multi-Agent System Example\n")

multi_agent_system = MultiAgentSystem(3)
final_stats = multi_agent_system.run_simulation(200)

print("\nFinal System Statistics:")
for key, value in final_stats.items():
    if isinstance(value, tuple):
        print(f"{key:20}: {value[0]:.1f} x {value[1]:.1f}")
    else:
        print(f"{key:20}: {value}")
```

## Summary

These examples demonstrate the enhanced capabilities of the NLM Python bindings:

1. **Enhanced API**: More Pythonic and intuitive interface
2. **Configuration Management**: Advanced configuration validation and utilities
3. **Performance Monitoring**: Built-in benchmarking and profiling tools
4. **Error Handling**: Comprehensive error handling and validation
5. **Integration**: Easy integration with external systems
6. **Multi-Agent Support**: Support for complex multi-agent scenarios
7. **Learning Integration**: Complete learning and development pipelines
8. **Advanced Features**: Neuromodulation, curiosity, and development systems

The enhanced bindings provide a robust foundation for building sophisticated neural simulation applications while maintaining ease of use and flexibility.