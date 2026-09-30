# NLM Python Advanced Usage Examples

## Overview

This document provides advanced usage examples for the NLM Python bindings. It covers complex multi-step configurations, agent training scenarios, world interaction patterns, and performance optimization techniques.

## Table of Contents

1. [Multi-Step Brain Configuration](#multi-step-brain-configuration)
2. [Complex Agent Training Scenarios](#complex-agent-training-scenarios)
3. [World Interaction Examples](#world-interaction-examples)
4. [Memory System Usage](#memory-system-usage)
5. [Neuromodulation Examples](#neuromodulation-examples)
6. [Development Tracking](#development-tracking)
7. [Performance Benchmarking](#performance-benchmarking)

---

## Multi-Step Brain Configuration

### Example 1: Gradual Network Growth

```python
import pynlm
import time

class GradualGrowthBrain:
    def __init__(self, initial_neurons=500):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", initial_neurons)
        self.config.set("brain.synapse_density", 0.1)
        self.config.set("development.enable_synaptogenesis", True)
        self.config.set("development.critical_period_duration", 200.0)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        self.agent = pynlm.createAgentBrain(self.brain)
        
    def grow_network(self, target_neurons, growth_stages=5):
        """Gradually grow the neural network."""
        stage_neurons = []
        for stage in range(growth_stages):
            # Calculate target for this stage
            current_target = target_neurons * ((stage + 1) / growth_stages)
            stage_neurons.append(current_target)
            
            # Enable structural plasticity for growth
            self.agent.enableStructuralPlasticity(True)
            
            # Run for developmental period
            for step in range(200):
                self.agent.updateDevelopment(0.1)
                self.brain.step(step)
                
                if step % 50 == 0:
                    print(f"Stage {stage}: {self.brain.getTotalNeuronCount()} neurons")
        
        # Disable plasticity after growth complete
        self.agent.enableStructuralPlasticity(False)
        return stage_neurons

# Usage
grower = GradualGrowthBrain(initial_neurons=200)
neurons_evolved = grower.grow_network(target_neurons=2000, growth_stages=5)
print(f"Network evolved from 200 to {max(neurons_evolved)} neurons")
```

### Example 2: Regional Specialization

```python
import pynlm
import math

class RegionalSpecialization:
    def __init__(self, num_regions=5):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 2000)
        self.config.set("brain.synapse_density", 0.05)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self.regions = {}
        self.setup_specialized_regions(num_regions)
        
    def setup_specialized_regions(self, num_regions):
        """Create regions with different specializations."""
        region_types = [
            ("Sensory", pynlm.NeuronType.Sensory, 0.3),
            ("Motor", pynlm.NeuronType.Motor, 0.3),
            ("Internal", pynlm.NeuronType.Internal, 0.2),
            ("Excitatory", pynlm.NeuronType.Excitatory, 0.4),
            ("Inhibitory", pynlm.NeuronType.Inhibitory, 0.2)
        ]
        
        neurons_per_region = self.brain.getTotalNeuronCount() // num_regions
        
        for i, (region_name, neuron_type, connection_bias) in enumerate(region_types[:num_regions]):
            region_id = self.brain.addRegion(region_name)
            self.regions[region_id] = {
                'name': region_name,
                'neuron_type': neuron_type,
                'connection_bias': connection_bias,
                'target_neurons': neurons_per_region
            }
            print(f"Created {region_name} region with ID {region_id}")
    
    def simulate_regional_dynamics(self, num_steps):
        """Simulate regional-specific dynamics."""
        for step in range(num_steps):
            # Update development to allow regional specialization
            self.agent.updateDevelopment(0.1)
            
            # Inject type-specific current
            if step % 50 == 0:
                for region_id, region_info in self.regions.items():
                    neuron_type = region_info['neuron_type']
                    target_current = 0.5 * region_info['connection_bias']
                    self.brain.injectCurrentToNeurons(neuron_type, target_current)
            
            self.brain.step(step)
            
            if step % 100 == 0:
                # Analyze regional activity
                total_neurons = self.brain.getTotalNeuronCount()
                active_neurons = self.brain.getActiveNeuronCount()
                firing_rate = self.brain.getAverageFiringRate()
                
                print(f"Step {step}: {active_neurons}/{total_neurons} active, "
                      f"rate: {firing_rate:.3f}")
        
        return self.regions

# Usage
specializer = RegionalSpecialization(num_regions=5)
specializer.simulate_regional_dynamics(500)
```

## Complex Agent Training Scenarios

### Example 3: Reward Prediction Learning

```python
import pynlm
import numpy as np

class RewardPredictor:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 1000)
        self.config.set("brain.synapse_density", 0.08)
        self.config.set("neuromod.dopamine.scale", 1.5)
        self.config.set("plasticity.stdp.learning_rate", 0.01)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world and agent
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        self.world.reset()
        self.world.setAgentStart(15.0, 15.0)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        self.agent.initialize(self.world)
        
        self.prediction_history = []
        self.reward_history = []
        self.error_history = []
        
    def run_reward_prediction_episode(self, num_steps=1000):
        """Episode with reward prediction and learning."""
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get percept and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action and get result
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Extract reward from world result
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            
            # Get prediction error
            prediction_error = self.agent.getPredictionError()
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, prediction_error)
            
            # Record history
            self.prediction_history.append(prediction_error)
            self.reward_history.append(reward)
            self.error_history.append(abs(prediction_error - reward))
            
            # Track development
            self.agent.updateDevelopment(0.1)
            
            if step % 200 == 0:
                self.print_step_stats(step)
        
        return self.analyze_learning_curves()
    
    def analyze_learning_curves(self):
        """Analyze prediction accuracy and learning progress."""
        if not self.prediction_history:
            return None
        
        # Calculate statistics
        pred_array = np.array(self.prediction_history)
        reward_array = np.array(self.reward_history)
        error_array = np.array(self.error_history)
        
        analysis = {
            'steps': len(self.prediction_history),
            'avg_prediction_error': np.mean(pred_array),
            'std_prediction_error': np.std(pred_array),
            'avg_reward': np.mean(reward_array),
            'avg_absolute_error': np.mean(error_array),
            'final_prediction': pred_array[-1],
            'final_reward': reward_array[-1],
            'learning_rate_estimate': self.estimate_learning_rate(pred_array, reward_array)
        }
        
        return analysis
    
    def estimate_learning_rate(self, predictions, rewards):
        """Simple estimate of learning rate from prediction changes."""
        if len(predictions) < 10:
            return 0.0
        
        # Calculate correlation between prediction changes and reward changes
        pred_changes = np.diff(predictions)
        reward_changes = np.diff(rewards)
        
        if len(pred_changes) == 0:
            return 0.0
            
        correlation = np.corrcoef(pred_changes, reward_changes)[0, 1]
        return max(0.0, correlation) * 10.0  # Scale to 0-10 range
    
    def print_step_stats(self, step):
        """Print statistics at regular intervals."""
        brain_stats = {
            'neurons': self.brain.getTotalNeuronCount(),
            'active': self.brain.getActiveNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'spikes': self.brain.getTotalSpikeCount()
        }
        
        agent_stats = {
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError()
        }
        
        print(f"Step {step}:")
        print(f"  Brain: {brain_stats['active']}/{brain_stats['neurons']} active, "
              f"rate {brain_stats['firing_rate']:.3f}")
        print(f"  Agent: curiosity={agent_stats['curiosity']:.3f}, "
              f"novelty={agent_stats['novelty']:.3f}, "
              f"pred_error={agent_stats['prediction_error']:.3f}")

# Usage
predictor = RewardPredictor()
results = predictor.run_reward_prediction_episode(1000)
print("\nLearning Analysis:")
for key, value in results.items():
    print(f"  {key}: {value:.4f}")
```

### Example 4: Multi-Agent Cooperation

```python
import pynlm
import threading
import time
from concurrent.futures import ThreadPoolExecutor

class MultiAgentCooperation:
    def __init__(self, num_agents=3, shared_world=None):
        self.num_agents = num_agents
        self.shared_world = shared_world or pynlm.createSimpleWorld()
        self.shared_world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
        self.shared_world.reset()
        
        self.agents = []
        self.brains = []
        self.create_agents()
        
        self.communication_channels = {}
        for i in range(num_agents):
            self.communication_channels[i] = []
            
    def create_agents(self):
        """Create multiple agents with specialized roles."""
        agent_configs = [
            {"name": "Explorer", "neurons": 800, "focus": "novelty"},
            {"name": "Forager", "neurons": 1200, "focus": "reward"},
            {"name": "Defender", "neurons": 600, "focus": "safety"}
        ]
        
        for i, config in enumerate(agent_configs[:self.num_agents]):
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("brain.neuron_count", config["neurons"])
            brain_config.set(f"agent.{config['focus']}_priority", 1.0)
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            self.brains.append(brain)
            self.agents.append(agent)
            
            # Each agent gets different starting position
            angle = 2 * 3.14159 * i / self.num_agents
            start_x = 25 + 15 * math.cos(angle)
            start_y = 25 + 15 * math.sin(angle)
            self.shared_world.setAgentStart(start_x, start_y)
            
            agent.initialize(self.shared_world)
            
            print(f"Created agent {i}: {config['name']} with {config['neurons']} neurons")
    
    def run_cooperative_simulation(self, num_steps=500):
        """Run cooperative simulation with agent communication."""
        simulation_results = {
            'total_steps': num_steps,
            'agent_stats': {},
            'shared_resources': [],
            'successful_cooperations': 0
        }
        
        for step in range(num_steps):
            # Each agent acts and shares observations
            agent_actions = []
            agent_percepts = []
            
            for i, agent in enumerate(self.agents):
                # Update world for all agents
                self.shared_world.update(0.1)
                
                # Get percept
                percept = self.shared_world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Agent acts based on its role
                action = self.agent_activities(i, agent, step)
                agent_actions.append((i, action))
                agent_percepts.append((i, percept))
                
                # Share information with other agents
                self.share_information(i, percept, agent_actions)
                
                # Apply agent's action to world
                self.shared_world.applyMotorCommand(action, self.shared_world.getSimulationTime())
                
                # Update development
                agent.updateDevelopment(0.1)
            
            # Check for cooperative achievements
            self.check_cooperation(step, agent_actions, simulation_results)
            
            # Record statistics
            self.record_step_statistics(step, agent_actions, simulation_results)
            
            if step % 100 == 0:
                self.print_cooperation_status(step, simulation_results)
        
        return simulation_results
    
    def agent_activities(self, agent_id, agent, step):
        """Agent-specific behavior based on role."""
        # Different behaviors for different agents
        if agent_id == 0:  # Explorer
            return self.explorer_behavior(agent, step)
        elif agent_id == 1:  # Forager
            return self.forager_behavior(agent, step)
        elif agent_id == 2:  # Defender
            return self.defender_behavior(agent, step)
        else:
            return agent.decodeMotorCommand()
    
    def explorer_behavior(self, agent, step):
        """Agent that seeks novel environments."""
        curiosity = agent.getCuriosityLevel()
        
        if curiosity > 0.7:  # High curiosity = exploration
            return self.get_random_movement()
        else:  # Low curiosity = exploitation
            return agent.decodeMotorCommand()
    
    def forager_behavior(self, agent, step):
        """Agent that seeks resources."""
        percept = self.shared_world.getSensoryPercept()
        
        # Look for resources in vision
        vision = percept.getVision()
        if vision:
            # Simple resource detection (assuming bright pixels are resources)
            avg_brightness = sum(vision) / len(vision) if vision else 0.5
            if avg_brightness > 0.7:  # Resource found
                return pynlm.Action(pynlm.ActionType.Interact)
        
        return agent.decodeMotorCommand()
    
    def defender_behavior(self, agent, step):
        """Agent that defends against threats."""
        # Simple threat detection based on energy
        body = self.shared_world.getAgentBody()
        if body.energy < 50:  # Low energy = seek safety
            return pynlm.Action(pynlm.ActionType.Rest)
        elif step % 20 == 0:  # Patrol occasionally
            return self.get_random_movement()
        else:
            return agent.decodeMotorCommand()
    
    def get_random_movement(self):
        """Generate random movement action."""
        import random
        actions = [
            pynlm.Action(pynlm.ActionType.MoveForward),
            pynlm.Action(pynlm.ActionType.MoveBackward),
            pynlm.Action(pynlm.ActionType.TurnLeft),
            pynlm.Action(pynlm.ActionType.TurnRight)
        ]
        return random.choice(actions)
    
    def share_information(self, agent_id, percept, all_actions):
        """Agents share information with each other."""
        # Simple information sharing based on agent proximity
        for other_id, (other_action, _) in enumerate(all_actions):
            if agent_id != other_id:
                # Agents share observations of similar objects/events
                shared_data = {
                    'step': time.time(),
                    'agent_id': agent_id,
                    'percept_summary': str(percept)[:100],
                    'action_taken': other_action.getType().value if hasattr(other_action, 'getType') else str(other_action)
                }
                self.communication_channels[other_id].append(shared_data)
    
    def check_cooperation(self, step, actions, results):
        """Check for successful cooperative behaviors."""
        # Simple cooperation detection: when agents move toward each other
        agent_positions = []
        for agent_action, _ in actions:
            # In a real implementation, you'd track actual positions
            agent_positions.append((0, 0))  # Placeholder
        
        # Check if agents are close to each other (cooperation)
        for i, pos1 in enumerate(agent_positions):
            for j, pos2 in enumerate(agent_positions[i+1:], i+1):
                distance = math.sqrt((pos1[0]-pos2[0])**2 + (pos1[1]-pos2[1])**2)
                if distance < 10:  # Close proximity
                    results['successful_cooperations'] += 1
                    break
    
    def record_step_statistics(self, step, actions, results):
        """Record simulation statistics."""
        # Record agent statistics
        for i, (agent_action, _) in enumerate(actions):
            if i not in results['agent_stats']:
                results['agent_stats'][i] = {
                    'steps': 0,
                    'actions': [],
                    'energy_levels': []
                }
            
            results['agent_stats'][i]['steps'] += 1
            results['agent_stats'][i]['actions'].append(agent_action.getType().value 
                        if hasattr(agent_action, 'getType') else str(agent_action))
    
    def print_cooperation_status(self, step, results):
        """Print current cooperation status."""
        print(f"\nStep {step} Status:")
        print(f"  Successful cooperations: {results['successful_cooperations']}")
        
        total_actions = sum(len(stats['actions']) for stats in results['agent_stats'].values())
        print(f"  Total actions taken: {total_actions}")
        
        if results['agent_stats']:
            avg_actions = total_actions / len(results['agent_stats'])
            print(f"  Average actions per agent: {avg_actions:.1f}")

# Usage
print("Setting up multi-agent cooperative simulation...")
 coop_simulation = MultiAgentCooperation(num_agents=3)
 results = coop_simulation.run_cooperative_simulation(num_steps=200)
print("\nSimulation Complete!")
print(f"Total successful cooperations: {results['successful_cooperations']}")
print(f"Agents simulated: {len(results['agent_stats'])}")
```

## World Interaction Examples

### Example 5: Environmental Dynamics

```python
import pynlm
import numpy as np

class EnvironmentalDynamics:
    def __init__(self, world_size=20):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=world_size,
            height=world_size,
            visionWidth=8,
            visionHeight=8
        )
        self.world.reset()
        self.world.setRandomSeed(42)
        
        # Initialize brain and agent
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1200)
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        self.agent.initialize(self.world)
        
        # Environmental state tracking
        self.epochs = 0
        self.resource_distribution = []
        self.weather_patterns = []
        self.agent_trajectory = []
        
    def dynamic_environment_cycle(self, num_epochs=100):
        """Run simulation with dynamic environmental changes."""
        for epoch in range(num_epochs):
            # Environmental changes at specific epochs
            if epoch % 25 == 0:
                self.create_environmental_event(epoch)
            
            # Run epoch
            epoch_result = self.run_epoch(epoch)
            
            # Record environmental state
            self.record_environmental_state(epoch, epoch_result)
            
            if epoch % 20 == 0:
                self.print_epoch_summary(epoch, epoch_result)
            
            self.epochs = epoch + 1
        
        return self.analyze_environmental_impact()
    
    def create_environmental_event(self, epoch):
        """Create environmental events at specific times."""
        event_type = epoch % 4
        
        if event_type == 0:  # Resource bloom
            print(f"Epoch {epoch}: Resource bloom!")
            # Add new resources randomly
            for _ in range(10):
                x = np.random.uniform(5, 15)
                y = np.random.uniform(5, 15)
                if self.world.isValidPosition(x, y):
                    resource = pynlm.WorldObject(
                        x, y, 
                        pynlm.WorldObjectType.Resource, 
                        value=2.0,
                        radius=0.8
                    )
                    self.world.addObject(resource)
                    
        elif event_type == 1:  # Weather change (storm)
            print(f"Epoch {epoch}: Weather storm!")
            self.world.setEnergyDecayRate(0.05)  # Higher energy decay
            # Reduce agent energy faster
            
        elif event_type == 2:  # Hazard introduction
            print(f"Epoch {epoch}: Hazards introduced!")
            for _ in range(5):
                x = np.random.uniform(1, 19)
                y = np.random.uniform(1, 19)
                if self.world.isValidPosition(x, y):
                    hazard = pynlm.WorldObject(
                        x, y,
                        pynlm.WorldObjectType.Hazard,
                        value=-1.0,
                        radius=0.6
                    )
                    self.world.addObject(hazard)
                    
        elif event_type == 3:  # Resource depletion
            print(f"Epoch {epoch}: Resources depleted!")
            self.world.setEnergyDecayRate(0.01)  # Normal decay
    
    def run_epoch(self, epoch):
        """Run a single epoch of simulation."""
        epoch_data = {
            'step_count': 0,
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'resources_found': 0,
            'hazards_encountered': 0,
            'final_energy': 0.0
        }
        
        # Fixed number of steps per epoch
        steps_per_epoch = 100
        
        for step in range(steps_per_epoch):
            # Update world
            self.world.update(0.1)
            
            # Get percept and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action and get result
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Extract reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            epoch_data['total_reward'] += reward
            
            # Track environmental interactions
            if percept.getInternal() and len(percept.getInternal()) > 1:
                epoch_data['resources_found'] += percept.getInternal()[1]
                
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Track trajectory
            body = self.world.getAgentBody()
            self.agent_trajectory.append((body.x, body.y, self.world.getSimulationTime()))
            
            epoch_data['step_count'] += 1
            epoch_data['final_energy'] = body.energy
            
            # Update max curiosity
            curiosity = self.agent.getCuriosityLevel()
            epoch_data['max_curiosity'] = max(epoch_data['max_curiosity'], curiosity)
        
        return epoch_data
    
    def record_environmental_state(self, epoch, epoch_result):
        """Record environmental state for analysis."""
        self.resource_distribution.append({
            'epoch': epoch,
            'total_reward': epoch_result['total_reward'],
            'resources_found': epoch_result['resources_found'],
            'max_curiosity': epoch_result['max_curiosity']
        })
        
        # Weather pattern based on epoch
        weather = {
            'epoch': epoch,
            'type': epoch % 4,
            'energy_decay_rate': self.world.getEnergyDecayRate()
        }
        self.weather_patterns.append(weather)
    
    def analyze_environmental_impact(self):
        """Analyze how environmental changes affected agent behavior."""
        if not self.resource_distribution:
            return None
        
        dist_array = np.array(self.resource_distribution)
        
        analysis = {
            'total_epochs': len(self.resource_distribution),
            'avg_reward_per_epoch': np.mean(dist_array[:, 1]),
            'reward_std': np.std(dist_array[:, 1]),
            'total_resources_found': np.sum(dist_array[:, 2]),
            'avg_max_curiosity': np.mean(dist_array[:, 3]),
            'correlation_reward_curiosity': np.corrcoef(dist_array[:, 1], dist_array[:, 3])[0, 1]
        }
        
        return analysis
    
    def print_epoch_summary(self, epoch, epoch_result):
        """Print epoch summary."""
        print(f"\nEpoch {epoch} Summary:")
        print(f"  Steps: {epoch_result['step_count']}")
        print(f"  Total Reward: {epoch_result['total_reward']:.3f}")
        print(f"  Resources Found: {epoch_result['resources_found']}")
        print(f"  Max Curiosity: {epoch_result['max_curiosity']:.3f}")
        print(f"  Final Energy: {epoch_result['final_energy']:.3f}")

# Usage
print("Setting up dynamic environmental simulation...")
 env_sim = EnvironmentalDynamics(world_size=30)
 results = env_sim.dynamic_environment_cycle(num_epochs=200)

print("\nEnvironmental Impact Analysis:")
for key, value in results.items():
    print(f"  {key}: {value:.4f}")

print(f"\nAgent trajectory points: {len(env_sim.agent_trajectory)}")
print(f"Final agent position: ({env_sim.agent_trajectory[-1][0]:.1f}, {env_sim.agent_trajectory[-1][1]:.1f})")
```

### Example 6: Sensory Processing Complexities

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

class SensoryProcessingComplexities:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 2000)
        self.config.set("plasticity.stdp.enable", True)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        
        self.sensory_history = []
        self.processing_stats = []
        
    def complex_sensory_simulation(self, num_steps=500):
        """Simulate complex sensory processing scenarios."""
        for step in range(num_steps):
            # Create complex multi-modal sensory input
            percept = self.create_complex_percept(step)
            
            # Process through agent
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            
            # Get reward and apply modulation
            reward = self.extract_reward_from_percept(percept)
            prediction_error = self.agent.getPredictionError()
            self.agent.applyRewardModulation(reward, prediction_error)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record statistics
            self.record_processing_stats(step, percept, reward, action)
            
            if step % 100 == 0:
                self.print_step_stats(step)
        
        return self.analyze_sensory_processing()
    
    def create_complex_percept(self, step):
        """Create complex multi-modal sensory input."""
        percept = pynlm.SensoryPercept()
        
        # Vision: Complex pattern based on step
        vision_width = 16
        vision_height = 16
        vision_data = []
        
        for y in range(vision_height):
            row = []
            for x in range(vision_width):
                # Create dynamic visual patterns
                distance = np.sqrt((x - vision_width/2)**2 + (y - vision_height/2)**2)
                noise = np.random.normal(0, 0.1)
                
                # Different patterns based on step
                if step % 4 == 0:
                    # Moving spot
                    pattern = np.exp(-((x - step*0.1)%vision_width - vision_width/2)**2 / 50)
                elif step % 4 == 1:
                    # Rotating stripes
                    pattern = np.sin(2 * np.pi * (x * 0.1 + y * 0.1 + step*0.05))
                elif step % 4 == 2:
                    # Flickering
                    pattern = 0.5 + 0.5 * np.sin(step * 0.1)
                else:
                    # Random noise
                    pattern = np.random.normal(0.5, 0.2)
                
                pixel_value = max(0, min(1, pattern + noise))
                row.append(float(pixel_value))
            vision_data.append(row)
        
        vision = pynlm.Vision(vision_width, vision_height, 1)
        vision.setData([item for row in vision_data for item in row])
        percept.setVision(vision)
        
        # Audio: Time-varying sound
        audio_samples = []
        for i in range(22050):  # 1 second at 22kHz
            # Create musical tone that changes over time
            freq = 440 + (step % 100) * 10  # Varying frequency
            amplitude = 0.3 + 0.2 * np.sin(step * 0.05)
            sample = amplitude * np.sin(2 * np.pi * freq * i / 22050)
            audio_samples.append(float(sample))
        
        audio = pynlm.Audio(22050, len(audio_samples))
        audio.setData(audio_samples)
        percept.setAudio(audio)
        
        # Internal signals: Mixed internal states
        internal = pynlm.InternalSignals()
        internal.addSignal(0.5)  # Base reward expectation
        internal.addSignal(0.3)  # Current energy level
        internal.addSignal(step % 100 / 100.0)  # Novelty
        percept.setInternal([internal.getSignals()])
        
        # Proprioception: Simple body state
        proprioception = [0.0, 0.0]  # x, y position
        percept.setProprioception(proprioception)
        
        return percept
    
    def extract_reward_from_percept(self, percept):
        """Extract reward from complex percept."""
        internal = percept.getInternal()
        if internal and len(internal) > 0:
            signals = internal[0]
            if len(signals) >= 2:
                # Reward from internal signals
                return signals[1]  # Energy signal
        
        # Default reward based on action success
        return 0.1
    
    def record_processing_stats(self, step, percept, reward, action):
        """Record sensory processing statistics."""
        stats = {
            'step': step,
            'vision_entropy': self.calculate_vision_entropy(percept),
            'audio_energy': self.calculate_audio_energy(percept),
            'internal_variance': self.calculate_internal_variance(percept),
            'reward_received': reward,
            'action_type': action.getType().value if hasattr(action, 'getType') else str(action),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'prediction_error': self.agent.getPredictionError()
        }
        
        self.processing_stats.append(stats)
        
        # Store sensory data for later analysis
        self.sensory_history.append({
            'step': step,
            'vision_pattern': percept.getVision().getData() if percept.getVision() else None,
            'audio_pattern': percept.getAudio().getData() if percept.getAudio() else None,
            'timestamp': step * 0.1
        })
    
    def calculate_vision_entropy(self, percept):
        """Calculate entropy of vision data."""
        vision = percept.getVision()
        if not vision:
            return 0.0
        
        data = vision.getData()
        if not data:
            return 0.0
        
        # Simple histogram entropy
        hist, _ = np.histogram(data, bins=10, range=(0, 1))
        hist = hist / np.sum(hist)
        entropy = -np.sum(hist * np.log(hist + 1e-10))
        return entropy
    
    def calculate_audio_energy(self, percept):
        """Calculate energy of audio signal."""
        audio = percept.getAudio()
        if not audio:
            return 0.0
        
        data = audio.getData()
        if not data:
            return 0.0
        
        return np.mean(np.square(data))
    
    def calculate_internal_variance(self, percept):
        """Calculate variance of internal signals."""
        internal = percept.getInternal()
        if not internal or not internal[0]:
            return 0.0
        
        signals = internal[0]
        return np.var(signals) if len(signals) > 1 else 0.0
    
    def print_step_stats(self, step):
        """Print step statistics."""
        if step >= len(self.processing_stats):
            return
        
        stats = self.processing_stats[step]
        
        print(f"\nStep {step}:")
        print(f"  Vision Entropy: {stats['vision_entropy']:.3f}")
        print(f"  Audio Energy: {stats['audio_energy']:.3f}")
        print(f"  Internal Variance: {stats['internal_variance']:.3f}")
        print(f"  Reward: {stats['reward_received']:.3f}")
        print(f"  Action: {stats['action_type']}")
        print(f"  Curiosity: {stats['curiosity_level']:.3f}")
        print(f"  Prediction Error: {stats['prediction_error']:.3f}")
    
    def analyze_sensory_processing(self):
        """Analyze sensory processing patterns."""
        if not self.processing_stats:
            return None
        
        stats_array = np.array(self.processing_stats)
        
        analysis = {
            'total_steps': len(self.processing_stats),
            'avg_vision_entropy': np.mean(stats_array[:, 0]),
            'avg_audio_energy': np.mean(stats_array[:, 1]),
            'avg_internal_variance': np.mean(stats_array[:, 2]),
            'avg_reward': np.mean(stats_array[:, 3]),
            'action_distribution': self.calculate_action_distribution(),
            'curiosity_dynamics': self.calculate_curiosity_dynamics(),
            'prediction_accuracy': self.calculate_prediction_accuracy()
        }
        
        return analysis
    
    def calculate_action_distribution(self):
        """Calculate distribution of actions taken."""
        if not self.processing_stats:
            return {}
        
        actions = [stats['action_type'] for stats in self.processing_stats]
        unique, counts = np.unique(actions, return_counts=True)
        return dict(zip(unique, counts))
    
    def calculate_curiosity_dynamics(self):
        """Calculate curiosity level dynamics."""
        if not self.processing_stats:
            return None
        
        curiosities = [stats['curiosity_level'] for stats in self.processing_stats]
        
        return {
            'initial': curiosities[0] if curiosities else 0,
            'final': curiosities[-1] if curiosities else 0,
            'max': np.max(curiosities) if curiosities else 0,
            'min': np.min(curiosities) if curiosities else 0,
            'std': np.std(curiosities) if curiosities else 0,
            'trend': np.polyfit(range(len(curiosities)), curiosities, 1)[0] if len(curiosities) > 1 else 0
        }
    
    def calculate_prediction_accuracy(self):
        """Calculate prediction accuracy."""
        if not self.processing_stats:
            return None
        
        rewards = [stats['reward_received'] for stats in self.processing_stats]
        prediction_errors = [stats['prediction_error'] for stats in self.processing_stats]
        
        accuracy = 1.0 / (1.0 + np.array(prediction_errors))
        
        return {
            'mean_accuracy': np.mean(accuracy),
            'final_accuracy': accuracy[-1] if len(accuracy) > 0 else 0,
            'accuracy_variance': np.var(accuracy) if len(accuracy) > 1 else 0,
            'reward_prediction_correlation': np.corrcoef(rewards[:-1], prediction_errors[1:])[0, 1]
        }

# Usage
print("Setting up complex sensory processing simulation...")
 sensory_sim = SensoryProcessingComplexities()
 results = sensory_sim.complex_sensory_simulation(num_steps=500)

print("\nSensory Processing Analysis:")
for category in ['action_distribution', 'curiosity_dynamics', 'prediction_accuracy']:
    if category in results:
        print(f"\n{category.replace('_', ' ').title()}:")
        if isinstance(results[category], dict):
            for key, value in results[category].items():
                print(f"  {key}: {value:.4f}")
        else:
            print(f"  {results[category]:.4f}")
```

## Memory System Usage

### Example 7: Working Memory Integration

```python
import pynlm
import numpy as np

class WorkingMemoryIntegration:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 3000)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("plasticity.stdp.enable", True)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        self.memory_traces = []
        self.working_memory_metrics = []
        
    def working_memory_training(self, num_steps=1000):
        """Train with working memory system."""
        for step in range(num_steps):
            # Create multi-step sensory sequence
            sensory_sequence = self.generate_sensory_sequence(step)
            
            # Process sequence through working memory
            for i, sensory_input in enumerate(sensory_sequence):
                self.agent.processSensoryInput(sensory_input)
                self.brain.step(step + i)
                
                # Track working memory usage
                self.track_working_memory(step, i, sensory_input)
            
            # Apply reward modulation
            reward = self.calculate_reward(sensory_sequence)
            prediction_error = self.agent.getPredictionError()
            self.agent.applyRewardModulation(reward, prediction_error)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            if step % 200 == 0:
                self.print_working_memory_stats(step)
        
        return self.analyze_working_memory_performance()
    
    def generate_sensory_sequence(self, step):
        """Generate multi-step sensory sequence."""
        sequences = []
        
        for i in range(3):
            # Create different sensory inputs
            percept = pynlm.SensoryPercept()
            
            # Vision: Simple pattern
            vision_data = []
            for y in range(8):
                row = []
                for x in range(8):
                    # Create pattern that varies with step and position
                    pattern = ((x + step*0.1) % 8) * ((y + step*0.05) % 8) / 64.0
                    row.append(float(pattern))
                vision_data.append(row)
            
            vision = pynlm.Vision(8, 8, 1)
            vision.setData([item for row in vision_data for item in row])
            percept.setVision(vision)
            
            # Internal signals: Working memory trace
            internal = pynlm.InternalSignals()
            internal.addSignal(0.1 * (i + 1))  # Signal strength
            internal.addSignal(step * 0.001)  # Time signal
            internal.addSignal(i * 0.3)       # Position in sequence
            percept.setInternal([internal.getSignals()])
            
            sequences.append(percept)
        
        return sequences
    
    def track_working_memory(self, step, sequence_index, sensory_input):
        """Track working memory metrics."""
        brain_state = {
            'step': step,
            'sequence_index': sequence_index,
            'active_neurons': self.brain.getFiringNeuronCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'curiosity': self.agent.getCuriosityLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'sensory_input_type': 'vision' if sensory_input.getVision() else 'internal'
        }
        
        self.working_memory_metrics.append(brain_state)
        
        # Store memory trace
        self.memory_traces.append({
            'step': step,
            'sequence_index': sequence_index,
            'pattern': sensory_input.getVision().getData() if sensory_input.getVision() else None,
            'internal_signals': sensory_input.getInternal()[0] if sensory_input.getInternal() else None,
            'brain_state': brain_state
        })
    
    def calculate_reward(self, sequence):
        """Calculate reward based on sequence processing."""
        # Simple reward based on sequence completion
        if len(sequence) == 3:
            return 0.5  # Reward for completing sequence
        return 0.1
    
    def print_working_memory_stats(self, step):
        """Print working memory statistics."""
        if not self.working_memory_metrics:
            return
        
        recent_metrics = self.working_memory_metrics[-100:]  # Last 100 steps
        
        avg_active = np.mean([m['active_neurons'] for m in recent_metrics])
        avg_rate = np.mean([m['firing_rate'] for m in recent_metrics])
        avg_curiosity = np.mean([m['curiosity'] for m in recent_metrics])
        
        print(f"\nStep {step} - Working Memory Stats:")
        print(f"  Active Neurons: {avg_active:.1f}")
        print(f"  Average Firing Rate: {avg_rate:.3f}")
        print(f"  Average Curiosity: {avg_curiosity:.3f}")
        print(f"  Total Memory Traces: {len(self.memory_traces)}")
        print(f"  Current Sequence Position: {len([m for m in self.working_memory_metrics if m['step'] == step])}")
    
    def analyze_working_memory_performance(self):
        """Analyze working memory performance."""
        if not self.working_memory_metrics:
            return None
        
        metrics_array = np.array(self.working_memory_metrics)
        
        analysis = {
            'total_steps': len(self.working_memory_metrics),
            'avg_active_neuron_ratio': np.mean(metrics_array[:, 1]) / np.mean(metrics_array[:, 2]),
            'avg_firing_rate': np.mean(metrics_array[:, 3]),
            'total_spikes': np.sum(metrics_array[:, 4]),
            'avg_curiosity': np.mean(metrics_array[:, 5]),
            'avg_prediction_error': np.mean(metrics_array[:, 6]),
            'memory_efficiency': self.calculate_memory_efficiency()
        }
        
        return analysis
    
    def calculate_memory_efficiency(self):
        """Calculate memory system efficiency."""
        # Simple efficiency metric: useful output per computational cost
        if len(self.memory_traces) < 10:
            return 0.0
        
        # Calculate based on sequence completion and firing patterns
        sequence_completions = sum(1 for trace in self.memory_traces 
                                  if trace['sequence_index'] == 2 and trace['step'] % 3 == 2)
        
        total_spikes = sum(trace['brain_state']['spike_count'] for trace in self.memory_traces)
        
        efficiency = sequence_completions / (total_spikes + 1)
        return efficiency

# Usage
print("Setting up working memory integration...")
 wm_integration = WorkingMemoryIntegration()
 results = wm_integration.working_memory_training(num_steps=1000)

print("\nWorking Memory Analysis:")
for key, value in results.items():
    print(f"  {key}: {value:.4f}")

print(f"\nTotal memory traces: {len(wm_integration.memory_traces)}")
print(f"Efficiency score: {results['memory_efficiency']:.4f}")
```

## Neuromodulation Examples

### Example 8: Reward-Modulated Learning

```python
import pynlm
import numpy as np

class RewardModulatedLearning:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 2000)
        self.config.set("neuromod.dopamine.scale", 2.0)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.hebbian.enable", True)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        
        self.neuromodulation_history = []
        self.reward_history = []
        self.plasticity_history = []
        
    def reward_modulated_training(self, num_steps=2000):
        """Training with reward-modulated plasticity."""
        for step in range(num_steps):
            # Create challenging environment
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
            world.reset()
            
            # Create agent and initialize
            agent = pynlm.createAgentBrain(self.brain)
            agent.enableRewardModulation(True)
            agent.initialize(world)
            
            # Run episode
            episode_result = self.run_episode(step, world, agent)
            
            # Apply neuromodulation
            self.apply_neuromodulation(step, episode_result, agent)
            
            # Record neuromodulation state
            self.record_neuromodulation_state(step, episode_result, agent)
            
            if step % 400 == 0:
                self.print_neuromodulation_stats(step, episode_result, agent)
        
        return self.analyze_neuromodulation_effects()
    
    def run_episode(self, step, world, agent):
        """Run single episode with reward learning."""
        episode_data = {
            'steps': 0,
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'spike_count': 0,
            'prediction_error_sum': 0.0
        }
        
        for i in range(100):  # Fixed episode length
            # Update world
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step + i)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Extract reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            episode_data['total_reward'] += reward
            episode_data['max_curiosity'] = max(episode_data['max_curiosity'], 
                                              agent.getCuriosityLevel())
            
            # Track prediction error
            prediction_error = agent.getPredictionError()
            episode_data['prediction_error_sum'] += abs(prediction_error)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            episode_data['steps'] += 1
            episode_data['spike_count'] = self.brain.getTotalSpikeCount()
        
        return episode_data
    
    def apply_neuromodulation(self, step, episode_result, agent):
        """Apply reward-modulated plasticity."""
        # Calculate average reward
        avg_reward = episode_result['total_reward'] / episode_result['steps']
        
        # Calculate prediction error
        avg_prediction_error = episode_result['prediction_error_sum'] / episode_result['steps']
        
        # Apply neuromodulation
        agent.applyRewardModulation(avg_reward, avg_prediction_error)
        
        # Enable/disable learning based on performance
        if avg_reward > 0.3:  # Good performance
            agent.enableCuriosity(True)
        elif avg_reward < 0.1:  # Poor performance
            agent.enableCuriosity(True)  # Encourage exploration
        else:
            agent.enableCuriosity(True)  # Maintain curiosity
        
    def record_neuromodulation_state(self, step, episode_result, agent):
        """Record neuromodulation state."""
        neuromodulation_state = {
            'step': step,
            'reward': episode_result['total_reward'] / episode_result['steps'],
            'prediction_error': agent.getPredictionError(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'developmental_stage': agent.getDevelopmentalStage().value,
            'spike_count': episode_result['spike_count']
        }
        
        self.neuromodulation_history.append(neuromodulation_state)
        
    def print_neuromodulation_stats(self, step, episode_result, agent):
        """Print neuromodulation statistics."""
        neuromod_state = self.neuromodulation_history[-1]
        
        print(f"\nStep {step} - Neuromodulation Stats:")
        print(f"  Average Reward: {neurom_state['reward']:.3f}")
        print(f"  Prediction Error: {neurom_state['prediction_error']:.3f}")
        print(f"  Curiosity Level: {neurom_state['curiosity']:.3f}")
        print(f"  Novelty Level: {neurom_state['novelty']:.3f}")
        print(f"  Neuromodulation Level: {neurom_state['neuromodulation_level']:.3f}")
        print(f"  Developmental Stage: {neurom_state['developmental_stage']}")
        print(f"  Spike Count: {neurom_state['spike_count']}")
    
    def analyze_neuromodulation_effects(self):
        """Analyze neuromodulation effects."""
        if not self.neuromodulation_history:
            return None
        
        history_array = np.array(self.neuromodulation_history)
        
        analysis = {
            'total_episodes': len(self.neuromodulation_history),
            'avg_reward': np.mean(history_array[:, 0]),
            'avg_prediction_error': np.mean(history_array[:, 1]),
            'avg_curiosity': np.mean(history_array[:, 2]),
            'avg_novelty': np.mean(history_array[:, 3]),
            'avg_neuromodulation': np.mean(history_array[:, 4]),
            'reward_curiosity_correlation': np.corrcoef(
                history_array[:, 0], history_array[:, 2])[0, 1],
            'learning_efficiency': self.calculate_learning_efficiency(history_array)
        }
        
        return analysis
    
    def calculate_learning_efficiency(self, history_array):
        """Calculate learning efficiency metric."""
        # Efficiency: reward learning per unit of prediction error
        rewards = history_array[:, 0]
        errors = history_array[:, 1]
        
        # Calculate reduction in prediction error over time
        if len(rewards) >= 10:
            early_errors = np.mean(errors[:10])
            late_errors = np.mean(errors[-10:])
            error_reduction = (early_errors - late_errors) / early_errors if early_errors > 0 else 0
            
            # Combined efficiency metric
            efficiency = error_reduction * np.mean(rewards)
            return max(0, efficiency)
        
        return 0.0

# Usage
print("Setting up reward-modulated learning...")
 rml = RewardModulatedLearning()
 results = rml.reward_modulated_training(num_steps=2000)

print("\nNeuromodulation Analysis:")
for key, value in results.items():
    print(f"  {key}: {value:.4f}")
```

## Development Tracking

### Example 9: Developmental Stage Monitoring

```python
import pynlm
import numpy as np

class DevelopmentalTracking:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 1500)
        self.config.set("development.enable_synaptogenesis", True)
        self.config.set("development.critical_period_duration", 150.0)
        self.config.set("development.pruning_rate", 0.008)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        self.development_log = []
        self.performance_metrics = []
        
    def developmental_tracking_simulation(self, num_steps=1500):
        """Simulation with developmental tracking."""
        for step in range(num_steps):
            # Create world and agent
            world = pynlm.createSimpleWorld()
            world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
            world.reset()
            
            agent = pynlm.createAgentBrain(self.brain)
            agent.enableRewardModulation(True)
            agent.enableDevelopment(True)
            agent.initialize(world)
            
            # Run episode
            episode_result = self.run_developmental_episode(step, world, agent)
            
            # Record developmental state
            self.record_developmental_state(step, episode_result, agent)
            
            # Record performance
            self.record_performance(step, episode_result)
            
            if step % 300 == 0:
                self.print_development_summary(step, episode_result, agent)
        
        return self.analyze_developmental_trajectory()
    
    def run_developmental_episode(self, step, world, agent):
        """Run single developmental episode."""
        episode_data = {
            'steps': 0,
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'final_energy': 0.0,
            'final_development': 0.0
        }
        
        for i in range(80):  # Fixed episode length
            # Update world
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step + i)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Extract reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            episode_data['total_reward'] += reward
            episode_data['max_curiosity'] = max(episode_data['max_curiosity'], 
                                              agent.getCuriosityLevel())
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Track state
            body = world.getAgentBody()
            episode_data['final_energy'] = body.energy
            episode_data['final_development'] = agent.getDevelopmentalStage().value
            
            episode_data['steps'] += 1
        
        return episode_data
    
    def record_developmental_state(self, step, episode_result, agent):
        """Record developmental state for analysis."""
        developmental_state = {
            'step': step,
            'developmental_stage': agent.getDevelopmentalStage().value,
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'brain_development': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'spike_count': self.brain.getTotalSpikeCount()
            }
        }
        
        self.development_log.append(developmental_state)
        
    def record_performance(self, step, episode_result):
        """Record performance metrics."""
        performance = {
            'step': step,
            'reward_per_step': episode_result['total_reward'] / episode_result['steps'],
            'max_curiosity_reached': episode_result['max_curiosity'],
            'final_energy_level': episode_result['final_energy'],
            'development_progress': episode_result['final_development']
        }
        
        self.performance_metrics.append(performance)
    
    def print_development_summary(self, step, episode_result, agent):
        """Print developmental summary."""
        if not self.development_log:
            return
        
        current_dev = self.development_log[-1]
        current_perf = self.performance_metrics[-1]
        
        print(f"\nStep {step} - Development Summary:")
        print(f"  Developmental Stage: {current_dev['developmental_stage']}")
        print(f"  Curiosity Level: {current_dev['curiosity_level']:.3f}")
        print(f"  Novelty Level: {current_dev['novelty_level']:.3f}")
        print(f"  Neuromodulation Level: {current_dev['neuromodulation_level']:.3f}")
        print(f"  Brain State: {current_dev['brain_development']['total_neurons']} neurons, "
              f"{current_dev['brain_development']['firing_rate']:.3f} firing rate")
        print(f"  Performance - Reward/Step: {current_perf['reward_per_step']:.3f}, "
              f"Max Curiosity: {current_perf['max_curiosity_reached']:.3f}")
        print(f"  Development Progress: {current_perf['development_progress']:.3f}")
    
    def analyze_developmental_trajectory(self):
        """Analyze developmental trajectory."""
        if not self.development_log or not self.performance_metrics:
            return None
        
        dev_array = np.array(self.development_log)
        perf_array = np.array(self.performance_metrics)
        
        analysis = {
            'total_steps': len(self.development_log),
            'developmental_stages': self.count_developmental_stages(dev_array),
            'performance_trends': self.analyze_performance_trends(perf_array),
            'developmental_efficiency': self.calculate_developmental_efficiency(dev_array, perf_array),
            'critical_period_effectiveness': self.evaluate_critical_period(dev_array)
        }
        
        return analysis
    
    def count_developmental_stages(self, dev_array):
        """Count time spent in each developmental stage."""
        stages = {}
        for state in dev_array:
            stage = state[1]  # developmental_stage
            if stage not in stages:
                stages[stage] = 0
            stages[stage] += 1
        
        return stages
    
    def analyze_performance_trends(self, perf_array):
        """Analyze performance trends over development."""
        rewards = perf_array[:, 0]
        curiosities = perf_array[:, 1]
        energies = perf_array[:, 2]
        
        return {
            'reward_trend': np.polyfit(range(len(rewards)), rewards, 1)[0],
            'curiosity_trend': np.polyfit(range(len(curiosities)), curiosities, 1)[0],
            'energy_trend': np.polyfit(range(len(energies)), energies, 1)[0],
            'performance_variance': np.var(rewards)
        }
    
    def calculate_developmental_efficiency(self, dev_array, perf_array):
        """Calculate developmental efficiency."""
        # Efficiency: performance improvement per developmental stage
        final_stage = dev_array[-1, 1]
        initial_stage = dev_array[0, 1]
        stage_progress = (final_stage - initial_stage) / max(1, (len(dev_array) - 1))
        
        final_reward = perf_array[-1, 0] if len(perf_array) > 0 else 0
        
        efficiency = stage_progress * final_reward
        return efficiency
    
    def evaluate_critical_period(self, dev_array):
        """Evaluate critical period effectiveness."""
        # Look for characteristic critical period patterns
        stages = [state[1] for state in dev_array]
        
        # Count transitions between stages
        transitions = []
        for i in range(1, len(stages)):
            if stages[i] != stages[i-1]:
                transitions.append((i, stages[i-1], stages[i]))
        
        return {
            'total_transitions': len(transitions),
            'transitions': transitions,
            'critical_period_reached': 'CriticalPeriod' in stages
        }

# Usage
print("Setting up developmental tracking simulation...")
 dt = DevelopmentalTracking()
 results = dt.developmental_tracking_simulation(num_steps=1500)

print("\nDevelopmental Analysis:")
for category in ['developmental_stages', 'performance_trends', 'developmental_efficiency']:
    if category in results:
        print(f"\n{category.replace('_', ' ').title()}:")
        if isinstance(results[category], dict):
            for key, value in results[category].items():
                print(f"  {key}: {value}")
        else:
            print(f"  {results[category]}")
```

## Performance Benchmarking

### Example 10: Performance Benchmarking

```python
import pynlm
import time
import numpy as np

class PerformanceBenchmarking:
    def __init__(self, config_variants=None):
        self.base_config = pynlm.createDefaultConfig()
        self.config_variants = config_variants or self.create_config_variants()
        
        self.benchmark_results = {}
        
    def create_config_variants(self):
        """Create different configuration variants for benchmarking."""
        variants = {
            'minimal': {
                'name': 'Minimal Configuration',
                'brain.neuron_count': 500,
                'brain.synapse_density': 0.05,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': False
            },
            'balanced': {
                'name': 'Balanced Configuration',
                'brain.neuron_count': 2000,
                'brain.synapse_density': 0.08,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'neuromod.dopamine.scale': 1.0
            },
            'advanced': {
                'name': 'Advanced Configuration',
                'brain.neuron_count': 5000,
                'brain.synapse_density': 0.1,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'neuromod.dopamine.scale': 1.5,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True
            }
        }
        
        return variants
    
    def run_comprehensive_benchmark(self):
        """Run comprehensive benchmark of all configuration variants."""
        print("Starting comprehensive performance benchmark...")
        
        for variant_name, variant_config in self.config_variants.items():
            print(f"\nBenchmarking: {variant_config['name']} ({variant_name})")
            
            results = self.benchmark_config(variant_config)
            self.benchmark_results[variant_name] = results
            
            self.print_benchmark_summary(variant_name, results)
        
        return self.analyze_benchmark_results()
    
    def benchmark_config(self, config_dict):
        """Benchmark specific configuration."""
        # Create configuration
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            if key != 'name':
                config.set(key, value)
        
        # Benchmark multiple runs
        run_results = []
        num_runs = 3
        
        for run in range(num_runs):
            print(f"  Run {run + 1}/{num_runs}")
            
            # Create brain and world
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.initialize(world)
            
            # Run benchmark simulation
            run_result = self.benchmark_simulation(run, brain, agent, world, config_dict)
            run_results.append(run_result)
        
        # Calculate statistics
        return self.calculate_benchmark_statistics(run_results)
    
    def benchmark_simulation(self, run_id, brain, agent, world, config_dict):
        """Run single benchmark simulation."""
        start_time = time.time()
        
        # Track performance metrics
        metrics = {
            'run_id': run_id,
            'config_name': config_dict['name'],
            'timings': {},
            'memory_usage': {},
            'performance_metrics': {}
        }
        
        # Phase 1: Initialization timing
        init_start = time.time()
        brain.initialize()
        agent.initialize(world)
        init_time = time.time() - init_start
        metrics['timings']['initialization'] = init_time
        
        # Phase 2: Simulation timing
        sim_start = time.time()
        
        steps_per_run = 500
        for step in range(steps_per_run):
            # World update
            world_update_start = time.time()
            world.update(0.1)
            metrics['timings']['world_update'] += (time.time() - world_update_start)
            
            # Sensory processing
            sensory_start = time.time()
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            metrics['timings']['sensory_processing'] += (time.time() - sensory_start)
            
            # Brain computation
            brain_start = time.time()
            brain.step(step)
            metrics['timings']['brain_computation'] += (time.time() - brain_start)
            
            # Action decoding
            action_start = time.time()
            action = agent.decodeMotorCommand()
            metrics['timings']['action_decoding'] += (time.time() - action_start)
            
            # Motor execution
            motor_start = time.time()
            world.applyMotorCommand(action, world.getSimulationTime())
            metrics['timings']['motor_execution'] += (time.time() - motor_start)
            
            # Neuromodulation
            neuromod_start = time.time()
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            metrics['timings']['neuromodulation'] += (time.time() - neuromod_start)
            
            # Development
            dev_start = time.time()
            agent.updateDevelopment(0.1)
            metrics['timings']['development'] += (time.time() - dev_start)
        
        total_time = time.time() - sim_start
        metrics['timings']['simulation'] = total_time
        metrics['timings']['total'] = total_time + init_time
        
        # Calculate performance metrics
        metrics['performance_metrics'] = {
            'steps_per_second': steps_per_run / total_time,
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_prediction_error': agent.getPredictionError(),
            'energy_efficiency': metrics['performance_metrics'].get('total_spikes', 0) / max(1, total_time)
        }
        
        return metrics
    
    def calculate_benchmark_statistics(self, run_results):
        """Calculate benchmark statistics across runs."""
        stats = {
            'config_name': run_results[0]['config_name'],
            'num_runs': len(run_results),
            'timings': {},
            'performance_metrics': {}
        }
        
        # Aggregate timings
        timing_categories = ['initialization', 'simulation', 'world_update', 
                           'sensory_processing', 'brain_computation', 'action_decoding',
                           'motor_execution', 'neuromodulation', 'development']
        
        for category in timing_categories:
            times = [run['timings'].get(category, 0) for run in run_results]
            stats['timings'][category] = {
                'mean': np.mean(times),
                'std': np.std(times),
                'min': np.min(times),
                'max': np.max(times)
            }
        
        # Aggregate performance metrics
        perf_categories = ['steps_per_second', 'total_spikes', 'average_firing_rate',
                         'final_curiosity', 'final_prediction_error', 'energy_efficiency']
        
        for category in perf_categories:
            values = []
            for run in run_results:
                perf_metrics = run['performance_metrics']
                if category in perf_metrics:
                    values.append(perf_metrics[category])
            
            if values:
                stats['performance_metrics'][category] = {
                    'mean': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values)
                }
        
        return stats
    
    def print_benchmark_summary(self, variant_name, results):
        """Print benchmark summary."""
        print(f"\n=== {results['config_name']} ===")
        
        print(f"Total Time: {results['timings']['total']['mean']:.3f}s ± {results['timings']['total']['std']:.3f}s")
        print(f"Simulation Time: {results['timings']['simulation']['mean']:.3f}s ± {results['timings']['simulation']['std']:.3f}s")
        print(f"Steps per Second: {results['performance_metrics']['steps_per_second']['mean']:.1f} ± "
              f"{results['performance_metrics']['steps_per_second']['std']:.1f}")
        
        print("\nTop 5 Most Expensive Operations:")
        ops_sorted = sorted(results['timings'].items(), 
                          key=lambda x: x[1]['mean'], reverse=True)[:5]
        
        for op_name, op_stats in ops_sorted:
            print(f"  {op_name}: {op_stats['mean']:.3f}s ± {op_stats['std']:.3f}s")
    
    def analyze_benchmark_results(self):
        """Analyze and compare benchmark results."""
        if not self.benchmark_results:
            return None
        
        analysis = {
            'config_comparisons': {},
            'performance_ranking': [],
            'scalability_analysis': {}
        }
        
        # Compare configurations
        config_names = list(self.benchmark_results.keys())
        
        for metric in ['steps_per_second', 'total_spikes', 'average_firing_rate']:
            if metric in self.benchmark_results[config_names[0]]['performance_metrics']:
                config_values = {}
                for config_name in config_names:
                    config_values[config_name] = self.benchmark_results[config_name]['performance_metrics'][metric]['mean']
                
                analysis['config_comparisons'][metric] = config_values
        
        # Performance ranking
        performance_ranking = sorted(config_names, 
                                    key=lambda x: self.benchmark_results[x]['performance_metrics']['steps_per_second']['mean'],
                                    reverse=True)
        analysis['performance_ranking'] = performance_ranking
        
        # Scalability analysis (if multiple neuron counts available)
        scalability_data = {}
        for config_name in config_names:
            neurons = self.get_config_neuron_count(config_name)
            if neurons:
                if neurons not in scalability_data:
                    scalability_data[neurons] = {}
                scalability_data[neurons][config_name] = self.benchmark_results[config_name]['performance_metrics']['steps_per_second']['mean']
        
        analysis['scalability_analysis'] = scalability_data
        
        return analysis
    
    def get_config_neuron_count(self, config_name):
        """Extract neuron count from configuration."""
        # This would need to parse the configuration
        # For now, return some example values
        config_neuron_counts = {
            'minimal': 500,
            'balanced': 2000,
            'advanced': 5000
        }
        return config_neuron_counts.get(config_name)

# Usage
print("Setting up performance benchmarking...")
 benchmark = PerformanceBenchmarking()
 results = benchmark.run_comprehensive_benchmark()

print("\n=== BENCHMARKING RESULTS ===")

if 'config_comparisons' in results:
    print("\nConfiguration Comparisons:")
    for metric, values in results['config_comparisons'].items():
        print(f"\n{metric.replace('_', ' ').title()}:")
        for config_name, value in values.items():
            print(f"  {config_name}: {value:.2f}")

if 'performance_ranking' in results:
    print(f"\nPerformance Ranking:")
    for i, config_name in enumerate(results['performance_ranking'], 1):
        print(f"  {i}. {config_name}")

if 'scalability_analysis' in results:
    print(f"\nScalability Analysis:")
    for neuron_count, config_values in results['scalability_analysis'].items():
        print(f"\n{neuron_count:,} Neurons:")
        for config_name, spsec in config_values.items():
            print(f"  {config_name}: {spsec:.1f} steps/sec")
```

---

## Next Steps

1. **For Documentation Updates**: Refer to `docs/python_api.md` for the latest API reference
2. **For Advanced Examples**: Explore additional use cases in subsequent documentation versions
3. **For Performance Analysis**: Consult `docs/benchmarking.md` for detailed performance metrics
4. **For Installation Help**: Check `docs/installing.md` for setup and troubleshooting
5. **For Common Issues**: Review `docs/troubleshooting.md` for problem resolution

---

## Version Information

This documentation covers NLM Python bindings version based on the current project configuration.

## References

- NLM Architecture Documentation (`docs/ARCHITECTURE.md`)
- NLM Scientific Background (`docs/SCIENCE.md`)
- Performance Benchmarking Guidelines (`docs/benchmarking.md`)
- Installation Troubleshooting (`docs/installing.md`)
