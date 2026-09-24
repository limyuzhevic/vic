# NLM Examples

**Collection of example scripts and complete demonstrations for the NLM framework**

## Overview

This document provides a comprehensive collection of example scripts that demonstrate NLM capabilities:

1. **Minimal Working Examples** - Basic setup and essential functionality
2. **Complete Agent Simulations** - Full agent-world interaction systems
3. **Experimental Setups** - Research-quality experimental frameworks
4. **Performance Benchmarks** - Performance testing and optimization

## 1. Minimal Working Examples

### Example 1: The Simplest Brain

```python
"""
The absolute minimum example - creates a working brain with no configuration
"""
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("Total spikes:", brain.getTotalSpikeCount())
```

**What this demonstrates:**
- Basic brain creation and initialization
- The minimal code needed to run a simulation
- Accessing brain statistics

### Example 2: Brain with Sensory Processing

```python
"""
Brain that receives and processes sensory input
"""
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create sensory input (vision)
sensory_input = pynlm.Vision(10, 10, 3)  # 10x10 RGB image

# Add some test data (white pixels)
test_data = [1.0] * 300  # 10 * 10 * 3 = 300
sensory_input.setData(test_data)

# Inject sensory input into brain
brain.receiveSensoryInput(sensory_input)

# Run simulation
for step in range(20):
    brain.step(step)

print(f"Brain processed {brain.getTotalSpikeCount()} spikes from sensory input")
```

**What this demonstrates:**
- Creating and using sensory input objects
- Injecting sensory data into the brain
- Brain response to external input

### Example 3: Action Production

```python
"""
Brain that produces motor actions based on its activity
"""
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for a few steps to build up neural activity
for step in range(10):
    brain.step(step)

# Produce action based on current neural activity
action = brain.produceAction()

print(f"Brain produced action: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")
print(f"Action name: {action.getName()}")

# Clone the action
action_clone = action.clone()
print(f"Cloned action: {action_clone.getType()}")
```

**What this demonstrates:**
- Action production from neural activity
- Action object structure and methods
- Action cloning

### Example 4: Configuration Loading

```python
"""
Loading configuration from different sources
"""
import pynlm
import json
import tempfile

# Method 1: Create and save configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", "1000")
config.set("plasticity.stdp.enable", "true")
config.set("neuromodulation.dopamine.baseline", "0.5")

# Save to file
config.saveToFile("my_config.json")
print("Configuration saved to my_config.json")

# Method 2: Load from file
loaded_config = pynlm.createDefaultConfig()
if loaded_config.loadFromFile("my_config.json"):
    print("Configuration loaded successfully")
    print(f"Loaded config summary: {loaded_config.summary()}")
else:
    print("Failed to load configuration")

# Method 3: Export to Python dict
config_dict = loaded_config.exportToDict()
print(f"Exported to dict with {len(config_dict)} entries")

# Method 4: Import from dict
new_config = pynlm.createDefaultConfig()
new_config.importFromDict(config_dict)
print("Configuration imported from dict")
```

**What this demonstrates:**
- Configuration management
- File I/O for configurations
- Dictionary conversion
- Configuration validation

## 2. Complete Agent Simulations

### Example 1: Maze Navigation Agent

```python
"""
Agent that learns to navigate a maze
"""
import pynlm
import time

class MazeNavigationAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Track performance
        self.steps_taken = 0
        self.resources_collected = 0
        self.hazards_avoided = 0
        
    def is_wall(self, x, y):
        """Check if position is a wall"""
        return self.world.isValidPosition(x, y)
    
    def is_resource(self, x, y):
        """Check if position has resource"""
        return self.world.isValidPosition(x, y)  # Simplified check
    
    def is_hazard(self, x, y):
        """Check if position is hazard"""
        return self.world.isValidPosition(x, y)  # Simplified check
    
    def find_path_avoiding_hazards(self, start_x, start_y, target_x, target_y):
        """Simple pathfinding algorithm"""
        # This is a simplified implementation
        # In a real agent, this would use the brain's planning capabilities
        path = [(start_x, start_y)]
        
        # Try to move toward target
        dx = target_x - start_x
        dy = target_y - start_y
        
        # Simple greedy movement
        if abs(dx) > abs(dy):
            new_x = start_x + (1 if dx > 0 else -1)
            new_y = start_y
        else:
            new_x = start_x
            new_y = start_y + (1 if dy > 0 else -1)
        
        # Check if move is valid
        if self.is_wall(new_x, new_y) and not self.is_hazard(new_x, new_y):
            path.append((new_x, new_y))
        
        return path
    
    def run_simulation(self, max_steps=200):
        """Run the maze navigation simulation"""
        agent_body = self.world.getAgentBody()
        
        for step in range(max_steps):
            # Get current state
            agent_body = self.world.getAgentBody()
            percept = self.world.getSensoryPercept()
            
            # Decide action based on simple heuristics
            action = self.decide_action(agent_body, percept)
            
            # Execute action
            world_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update brain with action result
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Collect rewards
            self.resources_collected += 1 if world_result.reward > 0 else 0
            self.hazards_avoided += 1 if world_result.success else 0
            
            # Apply learning
            if step % 5 == 0:
                self.agent.applyRewardModulation(world_result.reward, self.agent.getPredictionError())
            
            # Check for completion
            if self.is_resource(agent_body.x, agent_body.y):
                print(f"🎉 Resource collected at step {step}!")
                break
            
            # Optional: Check for goal
            if step >= max_steps - 1:
                print("⏰ Maximum steps reached")
                break
        
        return self.get_performance_stats()
    
    def decide_action(self, agent_body, percept):
        """Decide what action to take"""
        # Simple behavior: move toward resources, avoid hazards
        x, y = agent_body.x, agent_body.y
        
        # Look for nearby resources (simplified vision processing)
        resource_detected = False
        hazard_detected = False
        
        # Check immediate surroundings
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if dx == 0 and dy == 0:
                    continue
                
                nx, ny = x + dx, y + dy
                if self.is_resource(nx, ny):
                    resource_detected = True
                if self.is_hazard(nx, ny):
                    hazard_detected = True
        
        # Choose action based on detected objects
        if hazard_detected:
            return pynlm.ActionType.TurnLeft  # Simple avoidance
        elif resource_detected:
            return pynlm.ActionType.Interact  # Collect resource
        else:
            return pynlm.ActionType.MoveForward  # Default movement
    
    def get_performance_stats(self):
        """Get simulation performance statistics"""
        return {
            'steps_taken': self.steps_taken,
            'resources_collected': self.resources_collected,
            'hazards_avoided': self.hazards_avoided,
            'final_position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount()
        }

# Complete maze navigation example
print("🏃 Running Maze Navigation Example")
print("=" * 40)

# Create brain and world
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", "1500")
config.set("plasticity.stdp.enable", "true")

brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
world.reset()
world.setAgentStart(2, 2)  # Start in corner

# Create and run agent
agent = MazeNavigationAgent(brain, world)
stats = agent.run_simulation(100)

print(f"\n📊 Maze Navigation Results:")
print(f"   - Resources collected: {stats['resources_collected']}")
print(f"   - Hazards avoided: {stats['hazards_avoided']}")
print(f"   - Final position: ({stats['final_position'][0]:.1f}, {stats['final_position'][1]:.1f})")
print(f"   - Total spikes: {stats['total_spikes']}")
```

**What this demonstrates:**
- Complete agent implementation with custom behavior
- Integration of multiple NLM components
- Performance tracking and metrics
- Real-world task implementation

### Example 2: Resource Collection Agent

```python
"""
Agent that specializes in collecting scattered resources
"""
import pynlm
import math

class ResourceCollectionAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        
        # Enable all learning systems for optimal performance
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        # Resource management
        self.discovered_resources = []
        self.collected_resources = 0
        self.energy_efficiency = []
        
    def sense_environment(self):
        """Process sensory input to identify resources"""
        percept = self.world.getSensoryPercept()
        
        # In a real implementation, this would analyze the percept
        # For demo, we'll simulate resource discovery
        agent_body = self.world.getAgentBody()
        
        # Check nearby area for resources (simplified)
        for i in range(5):
            distance = i + 1
            x = agent_body.x + distance * math.cos(math.radians(agent_body.orientation))
            y = agent_body.y + distance * math.sin(math.radians(agent_body.orientation))
            
            if self.world.isValidPosition(x, y):  # Resource check
                self.discovered_resources.append((x, y, distance))
        
        return percept
    
    def plan_resource_collection(self):
        """Plan optimal resource collection strategy"""
        if not self.discovered_resources:
            return None, None
        
        # Sort resources by distance (closest first)
        self.discovered_resources.sort(key=lambda r: r[2])
        
        # Choose nearest resource
        nearest_x, nearest_y, distance = self.discovered_resources[0]
        
        # Calculate required action (simplified path planning)
        agent_body = self.world.getAgentBody()
        
        # Simple movement toward resource
        dx = nearest_x - agent_body.x
        dy = nearest_y - agent_body.y
        
        if abs(dx) > abs(dy):
            return pynlm.ActionType.TurnLeft if dx < 0 else pynlm.ActionType.TurnRight, (nearest_x, nearest_y)
        else:
            return pynlm.ActionType.MoveForward, (nearest_x, nearest_y)
    
    def execute_resource_collection(self, target_position):
        """Execute the resource collection at target position"""
        agent_body = self.world.getAgentBody()
        target_x, target_y = target_position
        
        # Move toward target
        if abs(target_x - agent_body.x) > 0.5:
            action = pynlm.ActionType.TurnLeft if target_x < agent_body.x else pynlm.ActionType.TurnRight
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            return False, "Moving to resource"
        
        if abs(target_y - agent_body.y) > 0.5:
            action = pynlm.ActionType.TurnLeft if target_y < agent_body.y else pynlm.ActionType.TurnRight
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            return False, "Moving to resource"
        
        # At resource position - collect it
        action = pynlm.ActionType.Eat
        result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        if result.success and result.reward > 0:
            self.collected_resources += 1
            self.energy_efficiency.append(1.0 / result.reward)
            return True, f"Collected resource! Reward: {result.reward}"
        
        return False, "Failed to collect resource"
    
    def run_resource_mission(self, duration_steps=300):
        """Run complete resource collection mission"""
        print("🔄 Starting Resource Collection Mission")
        print("=" * 45)
        
        for step in range(duration_steps):
            # Sense environment
            percept = self.sense_environment()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processes information
            self.brain.step(step)
            
            # Plan and execute action
            action, target = self.plan_resource_collection()
            
            if target is not None:
                # Execute collection
                collected, message = self.execute_resource_collection(target)
                if collected:
                    print(f"✅ Step {step}: {message}")
                else:
                    print(f"⏳ Step {step}: {message}")
            else:
                # No resources detected - explore
                action = pynlm.ActionType.MoveForward
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                print(f"🔍 Step {step}: Exploring...")
            
            # Apply learning
            if step % 10 == 0:
                curiosity = self.agent.getCuriosityLevel()
                self.agent.applyRewardModulation(curiosity, 0.0)
            
            # Update development
            if step % 50 == 0:
                self.agent.updateDevelopment(0.1)
            
            # Print progress every 50 steps
            if step % 50 == 0:
                agent_body = self.world.getAgentBody()
                print(f"Progress: Pos({agent_body.x:.1f}, {agent_body.y:.1f}) "
                      f"Discovered({len(self.discovered_resources)}) "
                      f"Collected({self.collected_resources}) "
                      f"Curiosity({self.agent.getCuriosityLevel():.3f})")
        
        return self.get_mission_stats()
    
    def get_mission_stats(self):
        """Get complete mission statistics"""
        agent_body = self.world.getAgentBody()
        
        if self.energy_efficiency:
            avg_efficiency = sum(self.energy_efficiency) / len(self.energy_efficiency)
        else:
            avg_efficiency = 0.0
        
        return {
            'total_steps': len(self.discovered_resources) + len(self.collected_resources),
            'resources_discovered': len(self.discovered_resources),
            'resources_collected': self.collected_resources,
            'collection_rate': self.collected_resources / max(1, len(self.discovered_resources)),
            'final_energy_efficiency': avg_efficiency,
            'final_position': (agent_body.x, agent_body.y),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'learning_enabled': {
                'reward_modulation': self.agent.isRewardModulationEnabled(),
                'curiosity': self.agent.isCuriosityEnabled(),
                'development': self.agent.isDevelopmentEnabled(),
                'structural_plasticity': self.agent.isStructuralPlasticityEnabled()
            }
        }

# Complete resource collection example
print("🗄️  Running Resource Collection Example")
print("=" * 50)

# Setup world with scattered resources
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(5, 5)

# Add scattered resources
for i in range(15):
    x = (i * 7) % 28 + 1
    y = (i * 11 + 2) % 28 + 1
    resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
    world.addObject(resource)

# Create agent with optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", "2000")
config.set("plasticity.structural.enable", "true")
config.set("neuromodulation.curiosity.threshold", "0.05")

brain = pynlm.createBrain(config)

# Create and run resource collection agent
agent = ResourceCollectionAgent(brain, world)
mission_stats = agent.run_resource_mission(200)

print(f"\n🎯 Resource Collection Results:")
print(f"   - Resources discovered: {mission_stats['resources_discovered']}")
print(f"   - Resources collected: {mission_stats['resources_collected']}")
print(f"   - Collection efficiency: {mission_stats['collection_rate']:.2%}")
print(f"   - Energy efficiency: {mission_stats['final_energy_efficiency']:.3f}")
print(f"   - Final position: ({mission_stats['final_position'][0]:.1f}, {mission_stats['final_position'][1]:.1f})")
```

**What this demonstrates:**
- Complex agent behavior with planning
- Resource management and tracking
- Performance optimization and efficiency metrics
- Long-running simulation management

## 3. Experimental Setups

### Example 1: Development Experiment

```python
"""
Experiment tracking brain development over time
"""
import pynlm
import json
from datetime import datetime

class DevelopmentExperiment:
    def __init__(self, brain, world, agent, experiment_name):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.experiment_name = experiment_name
        
        # Experiment data
        self.experiment_data = {
            'metadata': {
                'name': experiment_name,
                'start_time': datetime.now().isoformat(),
                'brain_size': brain.getTotalNeuronCount(),
                'configuration': brain.getConfig().exportToDict()
            },
            'developmental_stages': [],
            'performance_metrics': [],
            'learning_curves': {
                'curiosity': [],
                'novelty': [],
                'prediction_error': [],
                'neuromodulation': []
            },
            'structural_changes': []
        }
        
    def record_developmental_stage(self, step, agent):
        """Record developmental stage information"""
        stage = agent.getDevelopmentalStage()
        stage_name = ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"][stage]
        
        developmental_info = {
            'step': step,
            'stage': stage,
            'stage_name': stage_name,
            'plasticity_modulator': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        }
        
        self.experiment_data['developmental_stages'].append(developmental_info)
        return developmental_info
    
    def record_performance(self, step, world, agent, brain):
        """Record performance metrics"""
        agent_body = world.getAgentBody()
        
        performance_info = {
            'step': step,
            'position': (agent_body.x, agent_body.y),
            'energy': agent_body.energy,
            'health': agent_body.health,
            'age': agent_body.age,
            'brain_activity': {
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio()
            },
            'agent_state': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError()
            }
        }
        
        self.experiment_data['performance_metrics'].append(performance_info)
        return performance_info
    
    def record_learning_curves(self, step, agent):
        """Record learning-related metrics"""
        self.experiment_data['learning_curves']['curiosity'].append(agent.getCuriosityLevel())
        self.experiment_data['learning_curves']['novelty'].append(agent.getNoveltyLevel())
        self.experiment_data['learning_curves']['prediction_error'].append(agent.getPredictionError())
        self.experiment_data['learning_curves']['neuromodulation'].append(agent.getNeuromodulationLevel())
    
    def run_development_experiment(self, total_steps, enable_features=True):
        """Run the complete development experiment"""
        print(f"🧪 Running Development Experiment: {self.experiment_name}")
        print("=" * 60)
        
        # Enable learning features
        if enable_features:
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableStructuralPlasticity(True)
        
        # Main experiment loop
        for step in range(total_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processes
            self.brain.step(step)
            
            # Agent produces action
            action = self.agent.decodeMotorCommand()
            
            # Execute action
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            if step % 5 == 0:
                self.agent.applyRewardModulation(result.reward, self.agent.getPredictionError())
            
            # Update development
            if step % 20 == 0:
                self.agent.updateDevelopment(0.1)
            
            # Record data
            self.record_developmental_stage(step, self.agent)
            self.record_performance(step, self.world, self.agent, self.brain)
            self.record_learning_curves(step, self.agent)
            
            # Print progress
            if step % 50 == 0:
                stage = self.agent.getDevelopmentalStage()
                stage_names = ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"]
                print(f"Step {step} (Stage: {stage_names[stage]}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}, "
                      f"Energy: {self.world.getAgentBody().energy:.2f})")
        
        return self.experiment_data
    
    def analyze_results(self):
        """Analyze and summarize experiment results"""
        print(f"\n📊 Development Experiment Analysis")
        print("=" * 40)
        
        stages = self.experiment_data['developmental_stages']
        performance = self.experiment_data['performance_metrics']
        learning_curves = self.experiment_data['learning_curves']
        
        # Analyze developmental progression
        print(f"\n📈 Developmental Progression:")
        for stage_info in stages:
            stage_name = stage_info['stage_name']
            step = stage_info['step']
            print(f"   - {stage_name} at step {step}")
        
        # Analyze performance metrics
        if performance:
            initial_energy = performance[0]['energy']
            final_energy = performance[-1]['energy']
            energy_change = final_energy - initial_energy
            
            avg_firing_rate = sum(p['brain_activity']['average_firing_rate'] 
                                for p in performance) / len(performance)
            
            print(f"\n⚡ Performance Metrics:")
            print(f"   - Initial energy: {initial_energy:.2f}")
            print(f"   - Final energy: {final_energy:.2f}")
            print(f"   - Energy change: {energy_change:+.2f}")
            print(f"   - Average firing rate: {avg_firing_rate:.2f} Hz")
            print(f"   - Total simulation steps: {len(performance)}")
        
        # Analyze learning curves
        print(f"\n🎯 Learning Curve Analysis:")
        for metric_name, values in learning_curves.items():
            if values:
                initial = values[0]
                final = values[-1]
                trend = "increasing" if final > initial else "decreasing"
                change = final - initial
                print(f"   - {metric_name}: {initial:.3f} → {final:.3f} ({trend})")
        
        # Save experiment data
        self.save_experiment_data()
        
    def save_experiment_data(self):
        """Save experiment data to file"""
        filename = f"experiment_{self.experiment_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        with open(filename, 'w') as f:
            json.dump(self.experiment_data, f, indent=2, default=str)
        
        print(f"\n💾 Experiment data saved to: {filename}")

# Complete development experiment example
print("🧬 Running Development Experiment")
print("=" * 50)

# Create experimental setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", "1800")
config.set("development.stages.0.plasticity", "1.5")  # Enhanced initial plasticity
config.set("development.stages.1.plasticity", "0.8")

brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(3, 3)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create and run development experiment
experiment = DevelopmentExperiment(brain, world, agent, "brain_development_tracking")
experiment_data = experiment.run_development_experiment(300, enable_features=True)

# Analyze results
experiment.analyze_results()
```

**What this demonstrates:**
- Scientific experimental design
- Comprehensive data collection and analysis
- Development tracking over time
- Experimental best practices
- Data export for further analysis

### Example 2: Multi-Agent Competition Experiment

```python
"""
Experiment comparing different agent strategies
"""
import pynlm
import statistics
import random

class CompetitorAgent:
    """Base class for competing agents"""
    def __init__(self, brain, world, agent_id, strategy):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent_id = agent_id
        self.strategy = strategy
        self.agent.initialize(world)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Performance tracking
        self.performance_metrics = {
            'resources_collected': 0,
            'steps_taken': 0,
            'hazards_avoided': 0,
            'final_energy': 0.0,
            'learning_rate': 0.0
        }
        
    def get_action(self, percept, agent_body):
        """Get action based on strategy"""
        # Base implementation - overridden by subclasses
        return self.agent.decodeMotorCommand()
    
    def update_performance(self, action, result, agent_body):
        """Update performance metrics"""
        self.performance_metrics['steps_taken'] += 1
        self.performance_metrics['final_energy'] = agent_body.energy
        
        if result.reward > 0:
            self.performance_metrics['resources_collected'] += 1
        
        if result.success:
            self.performance_metrics['hazards_avoided'] += 1
    
    def apply_learning(self, step):
        """Apply learning for this step"""
        if step % 5 == 0:
            curiosity = self.agent.getCuriosityLevel()
            self.agent.applyRewardModulation(curiosity, 0.0)

class GreedyAgent(CompetitorAgent):
    """Agent that always tries to collect resources immediately"""
    def get_action(self, percept, agent_body):
        # Look for nearby resources
        for i in range(3):
            distance = i + 1
            if self.is_resource_nearby(agent_body, distance):
                return pynlm.ActionType.Interact
        
        # Otherwise move toward most promising area
        return pynlm.ActionType.MoveForward
    
    def is_resource_nearby(self, agent_body, distance):
        """Check if resource is nearby"""
        # Simplified - would use actual world perception
        return random.random() < 0.3  # Sometimes finds resources

class CautiousAgent(CompetitorAgent):
    """Agent that avoids hazards more aggressively"""
    def get_action(self, percept, agent_body):
        # First check for hazards
        for i in range(3):
            distance = i + 1
            if self.is_hazard_nearby(agent_body, distance):
                return pynlm.ActionType.TurnLeft
        
        # Then look for resources
        for i in range(3):
            distance = i + 1
            if self.is_resource_nearby(agent_body, distance):
                return pynlm.ActionType.Interact
        
        # Otherwise explore carefully
        return pynlm.ActionType.TurnLeft  # Less aggressive movement
    
    def is_hazard_nearby(self, agent_body, distance):
        """Check if hazard is nearby"""
        return random.random() < 0.2  # Sometimes detects hazards

class BalancedAgent(CompetitorAgent):
    """Agent that balances exploration and exploitation"""
    def __init__(self, brain, world, agent_id, strategy):
        super().__init__(brain, world, agent_id, strategy)
        self.exploration_cycles = 0
        self.collection_cycles = 0
        
    def get_action(self, percept, agent_body):
        curiosity = self.agent.getCuriosityLevel()
        
        # If highly curious, explore more
        if curiosity > 0.7:
            self.exploration_cycles += 1
            return pynlm.ActionType.TurnLeft  # More exploration
        elif curiosity < 0.3:
            self.collection_cycles += 1
            # Look for resources
            if self.is_resource_nearby(agent_body, 1):
                return pynlm.ActionType.Interact
        
        # Default: balanced behavior
        return pynlm.ActionType.MoveForward
    
    def is_resource_nearby(self, agent_body, distance):
        """Check if resource is nearby"""
        return random.random() < 0.4  # Moderate resource detection

class NoveltySeekingAgent(CompetitorAgent):
    """Agent driven by novelty seeking behavior"""
    def get_action(self, percept, agent_body):
        novelty = self.agent.getNoveltyLevel()
        
        # High novelty -> explore new areas
        if novelty > 0.5:
            # Change direction randomly
            if random.random() < 0.6:
                return pynlm.ActionType.TurnLeft
            elif random.random() < 0.6:
                return pynlm.ActionType.TurnRight
            else:
                return pynlm.ActionType.MoveForward
        
        # Low novelty -> try different actions
        elif novelty < 0.2:
            # Try interacting with environment
            return pynlm.ActionType.Interact
        
        # Medium novelty -> normal behavior
        return self.agent.decodeMotorCommand()

def create_competition_environment():
    """Create environment for multi-agent competition"""
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(5, 5)
    
    # Add scattered resources
    for i in range(20):
        x = (i * 3) % 38 + 1
        y = (i * 5 + 2) % 38 + 1
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
        world.addObject(resource)
    
    # Add hazards
    for i in range(10):
        x = (i * 7 + 10) % 38 + 1
        y = (i * 9 + 15) % 38 + 1
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-0.5)
        world.addObject(hazard)
    
    return world

def run_competition_experiment():
    """Run competition between different agent strategies"""
    print("🏆 Multi-Agent Competition Experiment")
    print("=" * 60)
    
    # Create environment
    world = create_competition_environment()
    
    # Create competitors with different strategies
    strategies = {
        'greedy': GreedyAgent,
        'cautious': CautiousAgent,
        'balanced': BalancedAgent,
        'novelty_seeking': NoveltySeekingAgent
    }
    
    agents = {}
    brains = {}
    
    for agent_name, strategy_class in strategies.items():
        # Create unique brain for each agent
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", "1200")
        
        brain = pynlm.createBrain(config)
        brains[agent_name] = brain
        
        agent = strategy_class(brain, world, agent_name, strategy_class.__name__)
        agents[agent_name] = agent
        
        print(f"Created {agent_name} agent with {brain.getTotalNeuronCount()} neurons")
    
    # Run competition
    num_steps = 200
    for step in range(num_steps):
        world.update(0.1)
        
        # Each agent perceives and acts
        for agent_name, agent in agents.items():
            percept = world.getSensoryPercept()
            agent.agent.processSensoryInput(percept)
            brain.step(step)
            
            # Get action and execute
            action = agent.get_action(percept, world.getAgentBody())
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update performance
            agent.update_performance(action, result, world.getAgentBody())
            
            # Apply learning
            agent.apply_learning(step)
        
        # Print progress
        if step % 50 == 0:
            print(f"\nStep {step}:")
            for agent_name, agent in agents.items():
                metrics = agent.performance_metrics
                print(f"   {agent_name:15}: "
                      f"Resources({metrics['resources_collected']:2d}) "
                      f"Hazards({metrics['hazards_avoided']:2d}) "
                      f"Energy({metrics['final_energy']:.2f})")
    
    # Analyze competition results
    analyze_competition_results(agents, brains)
    
    return agents, brains

def analyze_competition_results(agents, brains):
    """Analyze and rank competition results"""
    print(f"\n🏅 Competition Results")
    print("=" * 30)
    
    # Collect results for each agent
    results = []
    for agent_name, agent in agents.items():
        metrics = agent.performance_metrics
        
        # Calculate performance score
        score = (
            metrics['resources_collected'] * 10 +
            metrics['hazards_avoided'] * 5 +
            metrics['final_energy'] * 2
        )
        
        results.append({
            'agent': agent_name,
            'strategy': agent.strategy,
            'score': score,
            'resources': metrics['resources_collected'],
            'hazards': metrics['hazards_avoided'],
            'final_energy': metrics['final_energy'],
            'steps_taken': metrics['steps_taken']
        })
    
    # Sort by performance
    results.sort(key=lambda x: x['score'], reverse=True)
    
    # Display ranking
    for i, result in enumerate(results, 1):
        rank_emoji = ["🥇", "🥈", "🥉"][min(i-1, 2)]
        print(f"\n{rank_emoji} {i}. {result['agent']} ({result['strategy']})")
        print(f"   Performance score: {result['score']:.1f}")
        print(f"   Resources collected: {result['resources']}")
        print(f"   Hazards avoided: {result['hazards']}")
        print(f"   Final energy: {result['final_energy']:.2f}")
        print(f"   Steps taken: {result['steps_taken']}")
    
    # Calculate statistical insights
    print(f"\n📊 Statistical Insights:")
    resource_counts = [r['resources'] for r in results]
    hazard_counts = [r['hazards'] for r in results]
    
    print(f"   - Average resources collected: {statistics.mean(resource_counts):.1f}")
    print(f"   - Resource collection std: {statistics.stdev(resource_counts) if len(resource_counts) > 1 else 0:.1f}")
    print(f"   - Average hazard avoidance: {statistics.mean(hazard_counts):.1f}")
    print(f"   - Hazard avoidance std: {statistics.stdev(hazard_counts) if len(hazard_counts) > 1 else 0:.1f}")
    
    # Determine most successful strategy
    strategy_performance = {}
    for result in results:
        strategy = result['strategy']
        if strategy not in strategy_performance:
            strategy_performance[strategy] = []
        strategy_performance[strategy].append(result['score'])
    
    best_strategy = max(strategy_performance.items(), 
                       key=lambda x: statistics.mean(x[1]))
    
    print(f"\n🎯 Best Strategy: {best_strategy[0]}")
    print(f"   Average score: {statistics.mean(best_strategy[1]):.1f}")
    print(f"   Performance range: {min(best_strategy[1]):.1f} - {max(best_strategy[1]):.1f}")

# Run competition experiment
agents, brains = run_competition_experiment()
```

**What this demonstrates:**
- Multi-agent system design
- Strategy comparison and competition
- Statistical analysis of experimental results
- Agent behavior diversity
- Competitive evaluation framework

## 4. Performance Benchmarks

### Example 1: Performance Benchmarking

```python
"""
Benchmark brain performance with different configurations
"""
import pynlm
import time
import statistics

class PerformanceBenchmark:
    def __init__(self):
        self.results = []
        
    def benchmark_brain_configuration(self, name, config, test_steps=100):
        """Benchmark a specific brain configuration"""
        print(f"\n🔧 Benchmarking: {name}")
        print(f"   Configuration: {config.summary()}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure initialization time
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Measure simulation performance
        start_time = time.time()
        for step in range(test_steps):
            brain.step(step)
        sim_time = time.time() - start_time
        
        # Calculate metrics
        metrics = {
            'name': name,
            'configuration': config.exportToDict(),
            'init_time': init_time,
            'sim_time': sim_time,
            'avg_step_time': sim_time / test_steps,
            'neurons_per_second': brain.getTotalNeuronCount() / sim_time,
            'spikes_per_second': brain.getTotalSpikeCount() / sim_time,
            'total_steps': test_steps,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'memory_usage_estimate': brain.getTotalNeuronCount() * 4  # Rough estimate
        }
        
        self.results.append(metrics)
        
        print(f"   ✅ Initialization: {init_time:.4f}s")
        print(f"   ✅ Simulation: {sim_time:.4f}s ({test_steps} steps)")
        print(f"   ✅ Performance: {metrics['neurons_per_second']:.0f} neurons/sec")
        print(f"   ✅ Activity: {metrics['spikes_per_second']:.0f} spikes/sec")
        print(f"   ✅ Average firing rate: {metrics['firing_rate']:.2f} Hz")
        
        return metrics
    
    def benchmark_agent_performance(self, name, brain, world, test_steps=200):
        """Benchmark agent performance"""
        print(f"\n🤖 Benchmarking Agent: {name}")
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Measure initialization time
        start_time = time.time()
        agent.initialize(world)
        init_time = time.time() - start_time
        
        # Measure simulation performance
        start_time = time.time()
        for step in range(test_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            if step % 10 == 0:
                agent.applyRewardModulation(0.1, 0.0)
        
        sim_time = time.time() - start_time
        
        # Calculate metrics
        metrics = {
            'name': name,
            'type': 'agent',
            'init_time': init_time,
            'sim_time': sim_time,
            'avg_step_time': sim_time / test_steps,
            'agents_per_second': 1 / sim_time,
            'total_steps': test_steps,
            'total_spikes': brain.getTotalSpikeCount(),
            'learning_cycles': test_steps // 10,
            'avg_curiosity': agent.getCuriosityLevel(),
            'avg_novelty': agent.getNoveltyLevel(),
            'neuromodulation_level': agent.getNeuromodulationLevel()
        }
        
        self.results.append(metrics)
        
        print(f"   ✅ Initialization: {init_time:.4f}s")
        print(f"   ✅ Simulation: {sim_time:.4f}s ({test_steps} steps)")
        print(f"   ✅ Performance: {metrics['agents_per_second']:.0f} agents/sec")
        print(f"   ✅ Learning cycles: {metrics['learning_cycles']}")
        print(f"   ✅ Average curiosity: {metrics['avg_curiosity']:.3f}")
        
        return metrics
    
    def run_comprehensive_benchmark(self):
        """Run comprehensive benchmark suite"""
        print("🚀 NLM Performance Benchmark Suite")
        print("=" * 50)
        
        # Create test world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(2, 2)
        
        # Add test objects
        for i in range(5):
            x = 3 + i * 3
            y = 3
            resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
            world.addObject(resource)
        
        # Benchmark different brain configurations
        print(f"\n🧠 Brain Configuration Benchmarks")
        
        # Small brain
        config_small = pynlm.createDefaultConfig()
        config_small.set("brain.neuron_count", "200")
        self.benchmark_brain_configuration("Small Brain", config_small)
        
        # Medium brain
        config_medium = pynlm.createDefaultConfig()
        config_medium.set("brain.neuron_count", "1000")
        config_medium.set("plasticity.stdp.enable", "true")
        self.benchmark_brain_configuration("Medium Brain (with STDP)", config_medium)
        
        # Large brain
        config_large = pynlm.createDefaultConfig()
        config_large.set("brain.neuron_count", "3000")
        config_large.set("plasticity.stdp.enable", "true")
        config_large.set("plasticity.hebbian.enable", "true")
        config_large.set("neuromodulation.curiosity.enable", "true")
        self.benchmark_brain_configuration("Large Brain (full features)", config_large)
        
        # Benchmark agent performance with different brains
        print(f"\n🤖 Agent Performance Benchmarks")
        
        # Test with each brain configuration
        for result in self.results:
            if 'type' not in result:  # Brain benchmark
                config = pynlm.createDefaultConfig()
                for key, value in result['configuration'].items():
                    config.set(key, str(value))
                
                agent_config = pynlm.createDefaultConfig()
                agent_config.set("brain.neuron_count", "800")
                
                self.benchmark_agent_performance(f"Agent with {result['name']}", 
                                               pynlm.createBrain(agent_config), world)
        
        # Generate benchmark report
        self.generate_report()
        
    def generate_report(self):
        """Generate comprehensive benchmark report"""
        print(f"\n📋 Performance Benchmark Report")
        print("=" * 35)
        
        # Separate brain and agent results
        brain_results = [r for r in self.results if 'type' not in r]
        agent_results = [r for r in self.results if 'type' == 'agent']
        
        if brain_results:
            print(f"\n🧠 Brain Performance Analysis:")
            
            # Find fastest configuration
            fastest_brain = min(brain_results, key=lambda x: x['avg_step_time'])
            print(f"   ⚡ Fastest brain: {fastest_brain['name']}")
            print(f"      Average step time: {fastest_brain['avg_step_time']*1000:.2f}ms")
            print(f"      Performance: {fastest_brain['neurons_per_second']:.0f} neurons/sec")
            
            # Find most active brain
            most_active_brain = max(brain_results, key=lambda x: x['spikes_per_second'])
            print(f"   🔥 Most active brain: {most_active_brain['name']}")
            print(f"      Spike rate: {most_active_brain['spikes_per_second']:.0f} spikes/sec")
            
            # Find most efficient configuration
            most_efficient = min(brain_results, key=lambda x: x['avg_step_time'] / (x['neurons_per_second'] + 1))
            print(f"   ⚖️  Most efficient: {most_efficient['name']}")
            print(f"      Time per neuron: {most_efficient['avg_step_time'] / (most_efficient['neurons_per_second'] + 1):.6f}s")
        
        if agent_results:
            print(f"\n🤖 Agent Performance Analysis:")
            
            # Find fastest agent
            fastest_agent = min(agent_results, key=lambda x: x['avg_step_time'])
            print(f"   ⚡ Fastest agent: {fastest_agent['name']}")
            print(f"      Average step time: {fastest_agent['avg_step_time']*1000:.2f}ms")
            print(f"      Performance: {fastest_agent['agents_per_second']:.0f} agents/sec")
            
            # Find most learned agent
            most_learned = max(agent_results, key=lambda x: x['avg_curiosity'])
            print(f"   🧠 Most learned agent: {most_learned['name']}")
            print(f"      Average curiosity: {most_learned['avg_curiosity']:.3f}")
            print(f"      Neuromodulation: {most_learned['neuromodulation_level']:.3f}")
        
        # Overall recommendations
        print(f"\n💡 Performance Recommendations:")
        print(f"   - For real-time applications: Use smallest viable brain size")
        print(f"   - For complex tasks: Enable full plasticity and neuromodulation")
        print(f"   - For research: Use medium configurations for balance")
        print(f"   - For benchmarking: Compare average step times and spike rates")
        
    def save_results(self, filename):
        """Save benchmark results to file"""
        import json
        
        results_data = {
            'benchmark_suite': 'NLM Performance Benchmark',
            'timestamp': time.time(),
            'results': self.results
        }
        
        with open(filename, 'w') as f:
            json.dump(results_data, f, indent=2)
        
        print(f"\n💾 Benchmark results saved to: {filename}")

# Run comprehensive performance benchmarks
print("🚀 Starting Performance Benchmark Suite")
print("=" * 60)

benchmark = PerformanceBenchmark()
benchmark.run_comprehensive_benchmark()

# Save benchmark results for future reference
benchmark.save_results("nlm_performance_benchmark.json")
```

**What this demonstrates:**
- Comprehensive performance testing
- Scalability analysis
- Benchmarking methodology
- Performance comparison across configurations
- Performance reporting and analysis

### Example 2: Memory Usage Benchmark

```python
"""
Benchmark memory usage with different brain sizes
"""
import pynlm
import tracemalloc
import time

class MemoryBenchmark:
    def __init__(self):
        self.memory_results = []
        
    def benchmark_memory_usage(self, neuron_count, steps=50):
        """Benchmark memory usage for a specific neuron count"""
        print(f"\n💾 Memory Usage Benchmark: {neuron_count} neurons")
        
        # Configure brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", str(neuron_count))
        
        # Start memory tracking
        tracemalloc.start()
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Take memory snapshot
        snapshot1 = tracemalloc.take_snapshot()
        
        # Run simulation
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        sim_time = time.time() - start_time
        
        # Take final memory snapshot
        snapshot2 = tracemalloc.take_snapshot()
        
        # Stop memory tracking
        tracemalloc.stop()
        
        # Calculate memory metrics
        current_memory = tracemalloc.get_traced_memory()
        memory_growth = current_memory[1] - current_memory[0]
        
        # Compare memory snapshots
        top_stats = snapshot2.compare_to(snapshot1, 'lineno')
        
        metrics = {
            'neuron_count': neuron_count,
            'init_time': init_time,
            'sim_time': sim_time,
            'avg_step_time': sim_time / steps,
            'memory_growth_bytes': memory_growth,
            'memory_growth_mb': memory_growth / (1024 * 1024),
            'memory_per_neuron_bytes': memory_growth / neuron_count if neuron_count > 0 else 0,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'memory_hotspots': [
                {
                    'file': stat.traceback.format()[-1] if stat.traceback else 'unknown',
                    'line': stat.size / 1024 if stat.size > 0 else 0,
                    'count': stat.count
                }
                for stat in top_stats[:5]  # Top 5 memory consumers
            ]
        }
        
        self.memory_results.append(metrics)
        
        print(f"   ✅ Initialization: {init_time:.4f}s")
        print(f"   ✅ Simulation: {sim_time:.4f}s ({steps} steps)")
        print(f"   ✅ Memory growth: {metrics['memory_growth_mb']:.2f} MB")
        print(f"   ✅ Memory per neuron: {metrics['memory_per_neuron_bytes']:.1f} bytes")
        print(f"   ✅ Total spikes: {metrics['total_spikes']}")
        
        return metrics
    
    def run_memory_benchmark_suite(self):
        """Run comprehensive memory benchmark suite"""
        print("💾 NLM Memory Usage Benchmark Suite")
        print("=" * 50)
        
        # Test different brain sizes
        neuron_sizes = [100, 500, 1000, 2000]
        
        for size in neuron_sizes:
            self.benchmark_memory_usage(size)
        
        # Generate memory analysis
        self.analyze_memory_usage()
        
    def analyze_memory_usage(self):
        """Analyze memory usage patterns and trends"""
        print(f"\n🧠 Memory Usage Analysis")
        print("=" * 25)
        
        if not self.memory_results:
            print("   No memory benchmark results available")
            return
        
        # Memory efficiency analysis
        print(f"\n📊 Memory Efficiency Analysis:")
        
        efficiencies = []
        for result in self.memory_results:
            efficiency = result['memory_per_neuron_bytes']
            efficiencies.append(efficiency)
            print(f"   {result['neuron_count']:4d} neurons: {efficiency:.1f} bytes/neuron")
        
        # Calculate efficiency metrics
        avg_efficiency = statistics.mean(efficiencies)
        efficiency_std = statistics.stdev(efficiencies) if len(efficiencies) > 1 else 0
        
        print(f"\n📈 Efficiency Metrics:")
        print(f"   - Average memory per neuron: {avg_efficiency:.1f} bytes")
        print(f"   - Standard deviation: {efficiency_std:.1f} bytes")
        print(f"   - Best efficiency: {min(efficiencies):.1f} bytes/neuron")
        print(f"   - Worst efficiency: {max(efficiencies):.1f} bytes/neuron")
        
        # Memory growth analysis
        print(f"\n📈 Memory Growth Analysis:")
        for result in self.memory_results:
            growth_factor = result['memory_growth_mb'] / result['neuron_count']
            print(f"   {result['neuron_count']:4d} neurons: {growth_factor*1000:.2f} KB per 1000 neurons")
        
        # Recommendation based on analysis
        print(f"\n💡 Memory Usage Recommendations:")
        
        # Find most memory-efficient configuration
        most_efficient = min(self.memory_results, key=lambda x: x['memory_per_neuron_bytes'])
        print(f"   - For memory-constrained environments: Use {most_efficient['neuron_count']} neurons ({most_efficient['memory_per_neuron_bytes']:.1f} bytes/neuron)")
        
        # Check for linear scaling
        if len(self.memory_results) >= 3:
            # Check if memory per neuron increases with size (bad scaling)
            sizes = [r['neuron_count'] for r in self.memory_results]
            efficiencies = [r['memory_per_neuron_bytes'] for r in self.memory_results]
            
            # Simple linear regression check
            if all(efficiencies[i] <= efficiencies[i+1] for i in range(len(efficiencies)-1)):
                print(f"   - Memory scales linearly with size (expected)")
            else:
                print(f"   ⚠️  Non-linear memory scaling detected")
                
        # Memory hotspot analysis
        print(f"\n🔥 Memory Hotspots (Top 5 per benchmark):")
        total_hotspots = []
        for result in self.memory_results:
            for hotspot in result['memory_hotspots']:
                total_hotspots.append(hotspot)
        
        if total_hotspots:
            # Group by file
            file_hotspots = {}
            for hotspot in total_hotspots:
                filename = hotspot['file'].split('/')[-1]
                if filename not in file_hotspots:
                    file_hotspots[filename] = []
                file_hotspots[filename].append(hotspot['line'])
            
            for filename, lines in file_hotspots.items():
                total_memory = sum(lines)
                print(f"   - {filename}: {total_memory:.1f} KB total")
                
    def save_memory_results(self, filename):
        """Save memory benchmark results to file"""
        import json
        
        results_data = {
            'benchmark_suite': 'NLM Memory Usage Benchmark',
            'timestamp': time.time(),
            'results': self.memory_results
        }
        
        with open(filename, 'w') as f:
            json.dump(results_data, f, indent=2)
        
        print(f"\n💾 Memory benchmark results saved to: {filename}")

# Run comprehensive memory benchmark
print("💾 Starting Memory Usage Benchmark Suite")
print("=" * 60)

memory_benchmark = MemoryBenchmark()
memory_benchmark.run_memory_benchmark_suite()

# Save memory benchmark results
memory_benchmark.save_memory_results("nlm_memory_benchmark.json")
```

**What this demonstrates:**
- Memory usage analysis
- Scalability testing
- Memory efficiency calculations
- Memory hotspot identification
- Resource planning recommendations

## Complete Example: NLM Full System

Here's a complete example that integrates all NLM components:

```python
"""
Complete NLM system demonstrating all major components
"""
import pynlm
import time
import statistics

class CompleteNLMSystem:
    """Complete NLM system with all components integrated"""
    
    def __init__(self, config_name="default"):
        self.config_name = config_name
        self.setup_system()
        self.performance_data = []
        self.learning_metrics = []
        
    def setup_system(self):
        """Setup complete NLM system with all components"""
        print(f"🏗️  Setting up NLM System: {self.config_name}")
        print("=" * 50)
        
        # 1. Create brain with full configuration
        config = pynlm.createDefaultConfig()
        
        # Configure based on system type
        if self.config_name == "basic":
            config.set("brain.neuron_count", "500")
        elif self.config_name == "advanced":
            config.set("brain.neuron_count", "2000")
            config.set("plasticity.stdp.enable", "true")
            config.set("plasticity.hebbian.enable", "true")
            config.set("neuromodulation.dopamine.baseline", "0.5")
            config.set("neuromodulation.curiosity.threshold", "0.1")
        elif self.config_name == "research":
            config.set("brain.neuron_count", "5000")
            config.set("plasticity.stdp.enable", "true")
            config.set("plasticity.hebbian.enable", "true")
            config.set("plasticity.structural.enable", "true")
            config.set("neuromodulation.dopamine.baseline", "0.7")
            config.set("neuromodulation.curiosity.threshold", "0.05")
            config.set("neuromodulation.novelty.decay_rate", "0.95")
            config.set("development.stages.0.plasticity", "2.0")
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # 2. Create world environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.world.setAgentStart(5, 5)
        
        # Add environmental elements
        self.setup_environment()
        
        # 3. Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # 4. Enable learning systems based on configuration
        if self.config_name in ["advanced", "research"]:
            self.agent.enableRewardModulation(True)
        
        if self.config_name == "research":
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableStructuralPlasticity(True)
        
        print(f"   🧠 Brain: {self.brain.getTotalNeuronCount()} neurons")
        print(f"   🌍 World: {self.world.getWidth()}x{self.world.getHeight()} cells")
        print(f"   🤖 Agent: Enabled subsystems = {self.agent.getEnablements()}")
        
    def setup_environment(self):
        """Setup environmental elements"""
        # Add resources (good things to collect)
        resources_per_side = 5
        for i in range(resources_per_side):
            for j in range(resources_per_side):
                x = 3 + i * 5
                y = 3 + j * 5
                resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
                self.world.addObject(resource)
        
        # Add hazards (challenging elements)
        for i in range(8):
            x = 25 - i * 3
            y = 25
            hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-0.5)
            self.world.addObject(hazard)
        
        # Add walls (obstacles)
        for x in range(10, 20):
            wall = pynlm.WorldObject(x, 15, pynlm.WorldObjectType.Wall, value=0.0)
            self.world.addObject(wall)
        
        print(f"   📦 Environment setup complete")
        print(f"      - Resources: {resources_per_side * resources_per_side}")
        print(f"      - Hazards: 8")
        print(f"      - Walls: 10")
        
    def run_simulation_phase(self, phase_name, duration_steps, special_conditions=None):
        """Run a simulation phase with specific conditions"""
        print(f"\n📋 Running Phase: {phase_name}")
        print(f"   Duration: {duration_steps} steps")
        print("=" * 50)
        
        phase_data = {
            'name': phase_name,
            'steps': [],
            'start_time': time.time(),
            'special_conditions': special_conditions or {}
        }
        
        for step in range(duration_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processes
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Execute action
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning based on phase conditions
            self.apply_phase_learning(step, phase_name, special_conditions)
            
            # Record data
            agent_body = self.world.getAgentBody()
            step_data = {
                'step': step,
                'time': time.time() - phase_data['start_time'],
                'action': action.getType(),
                'reward': result.reward,
                'success': result.success,
                'position': (agent_body.x, agent_body.y),
                'energy': agent_body.energy,
                'brain_activity': {
                    'firing': self.brain.getFiringNeuronCount(),
                    'spikes': self.brain.getTotalSpikeCount(),
                    'rate': self.brain.getAverageFiringRate()
                },
                'agent_state': {
                    'curiosity': self.agent.getCuriosityLevel(),
                    'novelty': self.agent.getNoveltyLevel(),
                    'neuromodulation': self.agent.getNeuromodulationLevel(),
                    'prediction_error': self.agent.getPredictionError()
                }
            }
            
            phase_data['steps'].append(step_data)
            self.performance_data.append(step_data)
            self.learning_metrics.append({
                'step': step,
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'neuromodulation': self.agent.getNeuromodulationLevel()
            })
            
            # Print progress
            if step % 25 == 0:
                print(f"   Step {step}: Pos({agent_body.x:.1f}, {agent_body.y:.1f}) "
                      f"Action({action.getType()}) "
                      f"Energy({agent_body.energy:.2f}) "
                      f"Curiosity({self.agent.getCuriosityLevel():.3f})")
        
        return phase_data
        
    def apply_phase_learning(self, step, phase_name, special_conditions):
        """Apply learning specific to the current phase"""
        # Apply different learning patterns based on phase
        if phase_name == "exploration":
            # High curiosity phase - encourage exploration
            if step % 10 == 0:
                curiosity = self.agent.getCuriosityLevel() * 1.2
                self.agent.applyRewardModulation(min(1.0, curiosity), 0.0)
        
        elif phase_name == "exploitation":
            # Low curiosity phase - reward exploitation
            if step % 15 == 0:
                self.agent.applyRewardModulation(0.5, 0.1)
        
        elif phase_name == "learning":
            # Balanced phase with development updates
            if step % 20 == 0:
                self.agent.updateDevelopment(0.1)
            if step % 12 == 0:
                curiosity = self.agent.getCuriosityLevel()
                self.agent.applyRewardModulation(curiosity, 0.0)
        
        # Special conditions learning
        if special_conditions:
            if 'reward_boost' in special_conditions and step % special_conditions['reward_boost'] == 0:
                self.agent.applyRewardModulation(2.0, 0.0)
            
            if 'novelty_increase' in special_conditions and step % special_conditions['novelty_increase'] == 0:
                # In real system, this would involve changing environment
                pass
        
        # Regular learning
        if step % 8 == 0:
            self.agent.applyRewardModulation(0.1, self.agent.getPredictionError())
        
    def run_complete_simulation(self):
        """Run complete simulation with multiple phases"""
        print("🚀 Complete NLM System Simulation")
        print("=" * 60)
        
        # Phase 1: Initial exploration and discovery
        phase1_data = self.run_simulation_phase(
            "exploration",
            100,
            {'reward_boost': 25}
        )
        
        # Phase 2: Resource exploitation
        phase2_data = self.run_simulation_phase(
            "exploitation", 
            150,
            {'reward_boost': 30, 'novelty_increase': 40}
        )
        
        # Phase 3: Integrated learning and development
        phase3_data = self.run_simulation_phase(
            "learning",
            200,
            {'development_phase': True}
        )
        
        # Phase 4: Advanced capabilities
        phase4_data = self.run_simulation_phase(
            "advanced",
            100,
            {'complex_tasks': True}
        )
        
        # Analyze results
        self.analyze_complete_results(phase1_data, phase2_data, phase3_data, phase4_data)
        
        return {
            'phase1': phase1_data,
            'phase2': phase2_data,
            'phase3': phase3_data,
            'phase4': phase4_data
        }
        
    def analyze_complete_results(self, phase1, phase2, phase3, phase4):
        """Analyze complete simulation results"""
        print(f"\n📊 Complete Simulation Analysis")
        print("=" * 40)
        
        all_phases = [phase1, phase2, phase3, phase4]
        
        for i, phase in enumerate(all_phases, 1):
            print(f"\n📈 Phase {i}: {phase['name']}")
            
            steps = phase['steps']
            if not steps:
                continue
            
            # Calculate phase metrics
            positions = [s['position'] for s in steps]
            energies = [s['energy'] for s in steps]
            actions = [s['action'] for s in steps]
            rewards = [s['reward'] for s in steps]
            curiosities = [s['agent_state']['curiosity'] for s in steps]
            firing_rates = [s['brain_activity']['rate'] for s in steps]
            
            # Calculate statistics
            start_pos = positions[0]
            end_pos = positions[-1]
            displacement = ((end_pos[0] - start_pos[0])**2 + (end_pos[1] - start_pos[1])**2)**0.5
            
            action_counts = {}
            for action in actions:
                action_counts[action] = action_counts.get(action, 0) + 1
            
            # Display results
            print(f"   📊 Performance:")
            print(f"      - Steps: {len(steps)}")
            print(f"      - Position change: {displacement:.2f} cells")
            print(f"      - Final energy: {energies[-1]:.2f}")
            print(f"      - Average energy: {statistics.mean(energies):.2f}")
            print(f"      - Total reward: {sum(rewards):.2f}")
            
            print(f"   🧠 Neural Activity:")
            print(f"      - Average firing rate: {statistics.mean(firing_rates):.2f} Hz")
            print(f"      - Peak firing rate: {max(firing_rates):.2f} Hz")
            
            print(f"   🎯 Agent Behavior:")
            print(f"      - Average curiosity: {statistics.mean(curiosities):.3f}")
            print(f"      - Top actions: {sorted(action_counts.items(), key=lambda x: x[1], reverse=True)[:3]}")
            
        # Overall system assessment
        print(f"\n🏆 Overall System Assessment")
        print("=" * 30)
        
        total_steps = sum(len(phase['steps']) for phase in all_phases)
        total_spikes = sum(
            phase['steps'][-1]['brain_activity']['spikes'] 
            for phase in all_phases 
            if phase['steps']
        )
        
        total_curiosity = statistics.mean(
            m['curiosity'] for phase in all_phases 
            for m in phase.get('steps', [])
            for phase_data in [phase]
            if 'agent_state' in phase_data
        )
        
        print(f"   - Total simulation time: {total_steps} steps")
        print(f"   - Total neural activity: {total_spikes:,} spikes")
        print(f"   - Average curiosity: {total_curiosity:.3f}")
        print(f"   - Learning cycles completed: {total_steps // 8}")
        
        # System efficiency
        if total_steps > 0:
            efficiency = total_spikes / total_steps
            print(f"   - Neural efficiency: {efficiency:.1f} spikes/step")
        
        # Save complete results
        self.save_complete_results()
        
    def save_complete_results(self):
        """Save complete simulation results"""
        import json
        from datetime import datetime
        
        results = {
            'simulation_metadata': {
                'name': self.config_name,
                'completed_at': datetime.now().isoformat(),
                'total_steps': sum(len(phase['steps']) for phase in [p for p in [p for p in [p for p in self.performance_data]]]),
                'brain_size': self.brain.getTotalNeuronCount(),
                'configuration': self.brain.getConfig().exportToDict()
            },
            'performance_summary': {
                'total_steps': len(self.performance_data),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'final_energy': self.world.getAgentBody().energy,
                'learning_cycles': len(self.learning_metrics)
            },
            'detailed_data': self.performance_data,
            'learning_trajectory': self.learning_metrics
        }
        
        filename = f"complete_nlm_simulation_{self.config_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"\n💾 Complete simulation saved to: {filename}")

# Run complete NLM system examples
print("🎯 NLM Complete System Examples")
print("=" * 60)

# Example 1: Basic system
print(f"\n📦 Example 1: Basic NLM System")
basic_system = CompleteNLMSystem("basic")
basic_results = basic_system.run_complete_simulation()

# Example 2: Advanced system  
print(f"\n\n📦 Example 2: Advanced NLM System")
advanced_system = CompleteNLMSystem("advanced")
advanced_results = advanced_system.run_complete_simulation()

# Example 3: Research system
print(f"\n\n📦 Example 3: Research NLM System")
research_system = CompleteNLMSystem("research")
research_results = research_system.run_complete_simulation()

print(f"\n✅ All complete system examples completed successfully!")
print(f"📊 Results saved to JSON files for analysis")
print(f"🔍 Use the analysis tools to compare different system configurations")
```

**What this demonstrates:**
- Complete system integration
- Multi-phase simulation
- Configuration-based system setup
- Comprehensive data collection
- Performance analysis and reporting
- Real-world application examples

## Examples Summary

This examples collection provides:

1. **Minimal Examples** - Essential functionality with minimal code
2. **Complete Simulations** - Full agent-world interaction systems
3. **Research Experiments** - Scientific experimental frameworks
4. **Performance Benchmarks** - Comprehensive performance testing
5. **Memory Analysis** - Memory usage and efficiency testing
6. **System Integration** - Complete system demonstrations

### Quick Start Examples

For quick setup, try these examples:

```bash
# Run minimal brain example
python3 example1_simple_brain.py

# Run agent simulation
python3 example2_agent_simulation.py  

# Run resource collection
python3 example3_resource_collection.py

# Run competition experiment
python3 example4_competition.py

# Run performance benchmark
python3 example5_performance_benchmark.py
```

### Example File Structure

```
examples/
├── minimal/
│   ├── simple_brain.py
│   ├── sensory_processing.py
│   └── action_production.py
├── agent_simulations/
│   ├── maze_navigation.py
│   ├── resource_collection.py
│   └── pattern_learning.py
├── research/
│   ├── development_experiment.py
│   ├── multi_agent_competition.py
│   └── environmental_change.py
├── benchmarks/
│   ├── performance_benchmark.py
│   ├── memory_usage.py
│   └── system_efficiency.py
└── complete_system/
    ├── basic_system.py
    ├── advanced_system.py
    └── research_system.py
```

### Usage Tips

1. **Start with minimal examples** to understand NLM basics
2. **Study the advanced examples** to understand complex behaviors
3. **Run benchmarks** to understand performance characteristics
4. **Use research examples** as templates for your own experiments
5. **Combine techniques** from different examples for custom solutions

### Next Steps

After exploring these examples:

1. **Create your own example** based on specific research questions
2. **Extend existing examples** with new features or behaviors
3. **Benchmark your custom system** against these examples
4. **Document your findings** and share with the community
5. **Contribute to the examples collection** by adding new demonstrations

---

**Document Version**: NLM Examples Collection
**Last Updated**: September 2026
**Status**: Complete Examples Suite

These examples provide comprehensive documentation for using NLM across different use cases, from simple demonstrations to complex research experiments. Each example is designed to be self-contained while illustrating key NLM concepts and capabilities.