# NLM Python Advanced Documentation

## Overview

This document provides comprehensive guidance for advanced NLM (Neural Learning Machine) Python users, covering sophisticated usage patterns, performance optimization, and power user features.

## Advanced Topics

### 1. Performance Optimization

#### Profiling Your NLM Simulation

```python
import pynlm
import time
import cProfile
import pstats
from io import StringIO

class PerformanceProfiler:
    def __init__(self, brain):
        self.brain = brain
        self.timings = []
        self.memory_usage = []
    
    def profile_step(self, step):
        """Profile a single simulation step"""
        start_time = time.perf_counter()
        
        # Run brain step
        self.brain.step(step)
        
        end_time = time.perf_counter()
        self.timings.append(end_time - start_time)
        
        # Collect statistics
        stats = {
            'step': step,
            'time': end_time - start_time,
            'neurons_firing': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'memory_usage': self.brain.getTotalNeuronCount() * 2  # Approximate
        }
        
        return stats
    
    def profile_simulation(self, num_steps):
        """Profile complete simulation"""
        print(f"Profiling {num_steps} simulation steps...")
        
        for step in range(num_steps):
            stats = self.profile_step(step)
            
            if step % 100 == 0:
                print(f"Step {step}: {stats['time']:.4f}s, "
                      f"{stats['neurons_firing']} neurons firing")
        
        return self.generate_report()
    
    def generate_report(self):
        """Generate performance analysis report"""
        if not self.timings:
            return "No performance data collected"
        
        total_time = sum(self.timings)
        avg_time = total_time / len(self.timings)
        max_time = max(self.timings)
        min_time = min(self.timings)
        
        # Statistical analysis
        firing_rates = []
        for stats in self.timings:
            firing_rates.append(stats['average_firing_rate'])
        
        report = {
            'total_steps': len(self.timings),
            'total_time': total_time,
            'steps_per_second': len(self.timings) / total_time,
            'average_step_time': avg_time,
            'min_step_time': min_time,
            'max_step_time': max_time,
            'time_std': (sum([(t - avg_time) ** 2 for t in self.timings]) / len(self.timings)) ** 0.5,
            'firing_rate_mean': sum(firing_rates) / len(firing_rates),
            'firing_rate_std': (sum([(fr - sum(firing_rates)/len(firing_rates)) ** 2 for fr in firing_rates]) / len(firing_rates)) ** 0.5,
            'efficiency_score': sum(firing_rates) / (len(self.timings) * 1000)  # Normalized
        }
        
        return report

# Usage example
profiler = PerformanceProfiler(brain)
report = profiler.profile_simulation(1000)

print("Performance Report:")
for key, value in report.items():
    print(f"  {key}: {value:.4f}")
```

#### Memory Management and Optimization

```python
import pynlm
import gc

class MemoryEfficientSimulator:
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.step_count = 0
        
        # Optimize configuration for memory
        self._optimize_config_for_memory()
    
    def _optimize_config_for_memory(self):
        """Optimize configuration for reduced memory usage"""
        # Reduce neuron count for memory efficiency
        if not self.config.has("brain.neuron_count"):
            self.config.set("brain.neuron_count", 500)
        
        # Disable memory-intensive features
        self.config.set("plasticity.structural.enable", False)
        self.config.set("neuromod.curiosity.enable", False)
        self.config.set("neuromod.novelty.enable", False)
        
        # Enable memory-efficient learning
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.hebbian.enable", True)
    
    def create_efficient_brain(self):
        """Create brain with memory optimization"""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        return self
    
    def run_efficient_simulation(self, num_steps):
        """Run simulation with memory management"""
        print(f"Running memory-efficient simulation with {num_steps} steps")
        
        for step in range(num_steps):
            # Run simulation
            self.brain.step(step)
            self.step_count = step
            
            # Force garbage collection periodically
            if step % 100 == 0:
                gc.collect()
                print(f"Step {step}: Memory status - GC completed")
            
            # Get essential statistics
            if step % 50 == 0:
                stats = self.get_memory_stats()
                print(f"Step {step}: {stats}")
        
        return self
    
    def get_memory_stats(self):
        """Get current memory statistics"""
        return {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'step_count': self.step_count,
            'simulation_efficiency': self.brain.getAverageFiringRate()
        }
    
    def reset_memory_efficient(self):
        """Reset brain efficiently"""
        if self.brain:
            self.brain.reset()
            gc.collect()

# Usage example
efficient_sim = MemoryEfficientSimulator()
efficient_sim.create_efficient_brain()
results = efficient_sim.run_efficient_simulation(1000)
```

### 2. Advanced Configuration Management

#### Configuration Templates and Presets

```python
import pynlm
from typing import Dict, Any

class NLMConfigTemplates:
    @staticmethod
    def get_template(template_name: str) -> Dict[str, Any]:
        """Get predefined configuration templates"""
        templates = {
            "startup": {
                "brain.neuron_count": 100,
                "brain.synapse_density": 0.05,
                "brain.v_thresh": -50.0,
                "brain.v_rest": -70.0,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": False,
                "neuromod.curiosity.enable": False,
                "neuromod.novelty.enable": False,
            },
            
            "research": {
                "brain.neuron_count": 5000,
                "brain.synapse_density": 0.1,
                "brain.v_thresh": -45.0,
                "brain.v_rest": -65.0,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 2.0,
            },
            
            "energy_efficient": {
                "brain.neuron_count": 200,
                "brain.synapse_density": 0.02,
                "brain.v_thresh": -55.0,
                "brain.v_rest": -75.0,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": False,
                "plasticity.structural.enable": False,
                "neuromod.curiosity.enable": False,
                "neuromod.novelty.enable": False,
            },
            
            "real_time": {
                "brain.neuron_count": 1000,
                "brain.synapse_density": 0.08,
                "brain.v_thresh": -48.0,
                "brain.v_rest": -68.0,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": False,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": False,
            }
        }
        
        if template_name not in templates:
            raise ValueError(f"Unknown template: {template_name}")
        
        return templates[template_name]
    
    @staticmethod
    def apply_template(config, template_name: str) -> pynlm.Config:
        """Apply a configuration template to a config object"""
        template = NLMConfigTemplates.get_template(template_name)
        
        for key, value in template.items():
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
        
        return config
    
    @staticmethod
    def create_experiment_config(experiment_name: str, **kwargs) -> pynlm.Config:
        """Create experiment-specific configuration"""
        base_config = pynlm.createDefaultConfig()
        
        experiment_configs = {
            "memory_test": {
                "brain.neuron_count": 1000,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.enable": True,
            },
            
            "speed_test": {
                "brain.neuron_count": 500,
                "brain.synapse_density": 0.03,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": False,
            },
            
            "stability_test": {
                "brain.neuron_count": 2000,
                "brain.synapse_density": 0.07,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 1.5,
            }
        }
        
        # Use experiment config if available, otherwise create from kwargs
        if experiment_name in experiment_configs:
            experiment_config = experiment_configs[experiment_name]
            # Merge with user-provided kwargs
            for key, value in kwargs.items():
                experiment_config[key] = value
            config_dict = experiment_config
        else:
            config_dict = kwargs
        
        # Apply configuration
        for key, value in config_dict.items():
            if isinstance(value, bool):
                base_config.set(key, value)
            elif isinstance(value, int):
                base_config.set(key, value)
            elif isinstance(value, float):
                base_config.set(key, value)
            elif isinstance(value, str):
                base_config.set(key, value)
        
        return base_config

# Usage example
config = NLMConfigTemplates.apply_template(pynlm.createDefaultConfig(), "research")
brain = pynlm.createBrain(config)
brain.initialize()

# Create experiment-specific config
exp_config = NLMConfigTemplates.create_experiment_config(
    "memory_test", 
    "brain.neuron_count": 2000
)
```

### 3. Advanced Agent Programming

#### Agent Behavior Trees

```python
import pynlm
from typing import List, Callable, Dict, Any

class ActionNode:
    """Base class for behavior tree actions"""
    def __init__(self, name: str):
        self.name = name
    
    def execute(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, brain: pynlm.Brain) -> bool:
        """Execute the action, return True if completed"""
        raise NotImplementedError("Subclasses must implement execute method")
    
    def __str__(self):
        return self.name

class MoveToResource(ActionNode):
    """Move toward nearest resource"""
    def execute(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, brain: pynlm.Brain) -> bool:
        body = world.getAgentBody()
        percept = world.getSensoryPercept()
        
        # Find nearest resource
        nearest_resource = None
        min_distance = float('inf')
        
        # This would require accessing world objects - simplified here
        # In practice, you'd need to modify the SimpleWorld class
        
        if nearest_resource:
            distance = ((nearest_resource.x - body.x) ** 2 + 
                       (nearest_resource.y - body.y) ** 2) ** 0.5
            
            if distance < 5.0:  # Reached resource
                return True
            
            # Turn toward resource
            target_angle = math.atan2(nearest_resource.y - body.y, 
                                    nearest_resource.x - body.x) - body.orientation
            # Normalize angle
            target_angle = (target_angle + math.pi) % (2 * math.pi) - math.pi
            
            # Create action to turn
            action = pynlm.Action(pynlm.ActionType.TurnLeft, [target_angle * 0.5])
            world.applyMotorCommand(action, world.getSimulationTime())
            
            return False
        
        return True  # No resources found

class CollectResource(ActionNode):
    """Collect resource if nearby"""
    def execute(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, brain: pynlm.Brain) -> bool:
        body = world.getAgentBody()
        
        # Check for resource nearby (simplified - would need world object access)
        if self._check_resource_nearby(world, body.x, body.y):
            # Collect resource and apply reward
            agent.applyRewardModulation(1.0, 0.5)
            print("Resource collected!")
            return True
        
        return False
    
    def _check_resource_nearby(self, world: pynlm.SimpleWorld, x: float, y: float) -> bool:
        """Check if there's a resource nearby (simplified)"""
        # This would require access to world objects
        # For this example, return False
        return False

class Explore(ActionNode):
    """Random exploration behavior"""
    def __init__(self, name: str, exploration_rate: float = 0.3):
        super().__init__(name)
        self.exploration_rate = exploration_rate
        self.step_count = 0
    
    def execute(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, brain: pynlm.Brain) -> bool:
        self.step_count += 1
        
        # Random action selection
        if self.step_count % int(1.0 / self.exploration_rate) == 0:
            # Choose random action
            actions = [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft, 
                     pynlm.ActionType.TurnRight, pynlm.ActionType.Look]
            chosen_action = random.choice(actions)
            
            action = pynlm.Action(chosen_action, [])
            world.applyMotorCommand(action, world.getSimulationTime())
            
            print(f"Exploration: Taking random action {chosen_action}")
            return False
        
        return True

class BehaviorTree:
    """Simple behavior tree for agent control"""
    def __init__(self):
        self.root = None
        self.current_node = None
    
    def add_root(self, node: ActionNode):
        """Add root node to behavior tree"""
        self.root = node
        self.current_node = self.root
    
    def update(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, brain: pynlm.Brain) -> bool:
        """Update behavior tree, return True if tree completed"""
        if not self.root:
            return True
        
        # Execute current node
        if self.current_node:
            completed = self.current_node.execute(agent, world, brain)
            
            if completed:
                # Move to next node or reset
                self.current_node = self.find_next_node(self.current_node)
            
            return completed
        
        return True
    
    def find_next_node(self, current_node: ActionNode) -> ActionNode:
        """Find next node in sequence (simplified)"""
        # This would implement a proper behavior tree logic
        # For simplicity, return to root after completion
        return self.root

# Usage example
class AdvancedAgent:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.behavior_tree = BehaviorTree()
        self.episode_step = 0
    
    def setup(self, width: int = 50, height: int = 50):
        """Setup agent with world"""
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=width, height=height, 
                           visionWidth=16, visionHeight=16)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Setup behavior tree
        explore_node = Explore("Exploration", 0.2)
        collect_node = CollectResource("Collect Resource")
        move_node = MoveToResource("Move to Resource")
        
        # Simple sequence: Explore -> Try to collect -> Move to resource
        self.behavior_tree.add_root(explore_node)
        
        # Initialize world
        self.world.reset()
        self.world.setAgentStart(width // 2, height // 2)
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
    
    def run_episode(self, max_steps: int = 1000):
        """Run complete agent episode"""
        print(f"Starting advanced agent episode with {max_steps} steps")
        
        for step in range(max_steps):
            self.episode_step = step
            
            # Update world
            self.world.update(0.1)
            
            # Get percept and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Update behavior tree
            tree_completed = self.behavior_tree.update(self.agent, self.world, self.brain)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply neuromodulation
            curiosity = self.agent.getCuriosityLevel()
            if curiosity > 0.8:
                self.agent.applyRewardModulation(0.1, 0.2)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Print status
            if step % 100 == 0:
                self.print_status(step)
        
        return self.get_episode_results()
    
    def print_status(self, step: int):
        """Print current agent status"""
        print(f"\nStep {step}:")
        print(f"  Neurons firing: {self.brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {self.brain.getAverageFiringRate():.2f}")
        print(f"  Curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {self.brain.getDevelopmentalStage()}")
    
    def get_episode_results(self) -> Dict[str, Any]:
        """Get episode results summary"""
        return {
            'total_steps': self.episode_step + 1,
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'development_stage': self.brain.getDevelopmentalStage(),
        }

# Usage example
advanced_agent = AdvancedAgent()
advanced_agent.setup(width=60, height=60)
results = advanced_agent.run_episode(500)
print(f"\nEpisode completed: {results}")
```

### 4. Advanced State Management

#### Persistent State and Recovery

```python
import pynlm
import json
import pickle
import os
from datetime import datetime
from typing import Dict, Any, Optional

class NLMStateManager:
    """Advanced state manager for NLM simulations"""
    
    def __init__(self, save_directory: str = "nlm_saves"):
        self.save_directory = save_directory
        self.ensure_save_directory()
        self.current_session = None
    
    def ensure_save_directory(self):
        """Ensure save directory exists"""
        if not os.path.exists(self.save_directory):
            os.makedirs(self.save_directory)
    
    def create_session(self, brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                      agent: pynlm.AgentBrain, config: pynlm.Config) -> str:
        """Create new simulation session"""
        session_id = f"session_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        
        self.current_session = {
            'session_id': session_id,
            'timestamp': datetime.now().isoformat(),
            'config': self._config_to_dict(config),
            'brain': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'developmental_stage': brain.getDevelopmentalStage(),
                'total_spikes': brain.getTotalSpikeCount(),
            },
            'world': {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime(),
                'max_energy': world.getMaxEnergy(),
            },
            'agent': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError(),
            }
        }
        
        return session_id
    
    def save_session(self, session_id: str, brain: pynlm.Brain, 
                    world: pynlm.SimpleWorld, agent: pynlm.AgentBrain,
                    config: pynlm.Config, save_checkpoint: bool = True) -> bool:
        """Save complete session state"""
        try:
            session_data = {
                'session_info': self.current_session,
                'brain_state': {
                    'step': self.current_session['brain']['total_spikes'],
                    'neurons': brain.getTotalNeuronCount(),
                    'synapses': brain.getTotalSynapseCount(),
                    'spikes': brain.getTotalSpikeCount(),
                },
                'world_state': {
                    'time': world.getSimulationTime(),
                    'agent_pos': (world.getAgentBody().x, world.getAgentBody().y),
                    'agent_dir': world.getAgentBody().orientation,
                },
                'agent_state': {
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'neuromodulation': agent.getNeuromodulationLevel(),
                    'prediction_error': agent.getPredictionError(),
                }
            }
            
            # Save to JSON
            json_file = os.path.join(self.save_directory, f"{session_id}.json")
            with open(json_file, 'w') as f:
                json.dump(session_data, f, indent=2)
            
            # Save checkpoint if requested
            if save_checkpoint:
                checkpoint_file = os.path.join(self.save_directory, f"{session_id}.bin")
                if brain.save(checkpoint_file):
                    print(f"Checkpoint saved: {checkpoint_file}")
                else:
                    print(f"Failed to save checkpoint")
                    return False
            
            print(f"Session saved: {json_file}")
            return True
            
        except Exception as e:
            print(f"Error saving session: {e}")
            return False
    
    def load_session(self, session_id: str, config: Optional[pynlm.Config] = None) -> Dict[str, Any]:
        """Load session state"""
        try:
            json_file = os.path.join(self.save_directory, f"{session_id}.json")
            checkpoint_file = os.path.join(self.save_directory, f"{session_id}.bin")
            
            with open(json_file, 'r') as f:
                session_data = json.load(f)
            
            # Load brain state from checkpoint
            brain = None
            if os.path.exists(checkpoint_file):
                # In a real implementation, you would load the brain from the checkpoint
                print(f"Would load brain from checkpoint: {checkpoint_file}")
            
            return {
                'session_data': session_data,
                'brain': brain,
                'success': True
            }
            
        except Exception as e:
            print(f"Error loading session: {e}")
            return {'success': False, 'error': str(e)}
    
    def list_sessions(self) -> List[str]:
        """List all saved sessions"""
        try:
            files = os.listdir(self.save_directory)
            session_files = [f for f in files if f.startswith('session_') and f.endswith('.json')]
            return sorted(session_files)
        except Exception as e:
            print(f"Error listing sessions: {e}")
            return []
    
    def delete_session(self, session_id: str) -> bool:
        """Delete a saved session"""
        try:
            json_file = os.path.join(self.save_directory, f"{session_id}.json")
            checkpoint_file = os.path.join(self.save_directory, f"{session_id}.bin")
            
            os.remove(json_file)
            if os.path.exists(checkpoint_file):
                os.remove(checkpoint_file)
            
            print(f"Session deleted: {session_id}")
            return True
            
        except Exception as e:
            print(f"Error deleting session: {e}")
            return False
    
    def _config_to_dict(self, config: pynlm.Config) -> Dict[str, Any]:
        """Convert config to dictionary"""
        config_dict = {
            'keys': config.getKeys(),
            'settings': {}
        }
        
        for key in config_dict['keys']:
            try:
                value = config.get(key)
                if value is not None:
                    config_dict['settings'][key] = value
            except:
                # Skip keys that can't be accessed
                pass
        
        return config_dict

# Usage example
state_manager = NLMStateManager()

# Create and setup NLM components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create and save session
session_id = state_manager.create_session(brain, world, agent, config)
state_manager.save_session(session_id, brain, world, agent, config)

# List and load sessions
sessions = state_manager.list_sessions()
print("Available sessions:", sessions)

# Load a session if available
if sessions:
    loaded_data = state_manager.load_session(sessions[0])
    if loaded_data['success']:
        print("Session loaded successfully")
```

### 5. Advanced Integration and Extensions

#### Custom Environment Integration

```python
import pynlm
import math
from abc import ABC, abstractmethod
from typing import Dict, Any

class CustomWorldInterface:
    """Interface for custom world implementations"""
    
    @abstractmethod
    def update(self, timestep: float) -> None:
        """Update world state"""
        pass
    
    @abstractmethod
    def get_sensory_percept(self) -> pynlm.SensoryPercept:
        """Get current sensory percept"""
        pass
    
    @abstractmethod
    def apply_motor_command(self, action: pynlm.Action, current_time: float) -> pynlm.ActionResult:
        """Apply motor command and return result"""
        pass

class PhysicsBasedWorld(CustomWorldInterface):
    """Physics-based world simulation"""
    
    def __init__(self, width: float = 100.0, height: float = 100.0):
        self.width = width
        self.height = height
        self.agent_x = width // 2
        self.agent_y = height // 2
        self.agent_angle = 0.0
        self.agent_velocity_x = 0.0
        self.agent_velocity_y = 0.0
        self.agent_angular_velocity = 0.0
        
        # Physics parameters
        self.gravity = 9.81
        self.friction = 0.95
        self.max_force = 10.0
        
        # Environment elements
        self.resources = []
        self.hazards = []
        self.walls = []
        
        # Physics simulation
        self.current_time = 0.0
        self._initialize_environment()
    
    def _initialize_environment(self):
        """Initialize environment with physics properties"""
        # Add walls
        wall_thickness = 2.0
        self.walls = [
            {'x': 0, 'y': 0, 'type': 'boundary', 'bounds': (0, 0, wall_thickness, self.height)},
            {'x': self.width, 'y': 0, 'type': 'boundary', 'bounds': (self.width - wall_thickness, 0, wall_thickness, self.height)},
            {'x': 0, 'y': self.height, 'type': 'boundary', 'bounds': (0, self.height - wall_thickness, wall_thickness, wall_thickness)},
            {'x': self.width, 'y': self.height, 'type': 'boundary', 'bounds': (self.width - wall_thickness, self.height - wall_thickness, wall_thickness, wall_thickness)},
        ]
        
        # Add resources with physics properties
        for i in range(10):
            x = random.uniform(wall_thickness + 10, self.width - wall_thickness - 10)
            y = random.uniform(wall_thickness + 10, self.height - wall_thickness - 10)
            self.resources.append({
                'x': x, 'y': y,
                'type': pynlm.WorldObjectType.Resource,
                'value': random.uniform(1, 5),
                'mass': random.uniform(5, 15),
                'static': True
            })
        
        # Add hazards with physics properties
        for i in range(5):
            x = random.uniform(wall_thickness + 20, self.width - wall_thickness - 20)
            y = random.uniform(wall_thickness + 20, self.height - wall_thickness - 20)
            self.hazards.append({
                'x': x, 'y': y,
                'type': pynlm.WorldObjectType.Hazard,
                'value': 1.0,
                'radius': 3.0,
                'static': True
            })
    
    def update(self, timestep: float) -> None:
        """Update physics simulation"""
        self.current_time += timestep
        
        # Update agent physics
        self._update_agent_physics(timestep)
        
        # Check for collisions
        self._check_collisions()
        
        # Update environment
        self._update_environment(timestep)
    
    def _update_agent_physics(self, timestep: float):
        """Update agent physics simulation"""
        # Calculate forces
        force_x = 0.0
        force_y = 0.0
        torque = 0.0
        
        # Apply friction
        force_x *= self.friction
        force_y *= self.friction
        self.agent_angular_velocity *= self.friction
        
        # Update velocity
        self.agent_velocity_x += force_x * timestep
        self.agent_velocity_y += force_y * timestep
        self.agent_angular_velocity += torque * timestep
        
        # Update position
        self.agent_x += self.agent_velocity_x * timestep
        self.agent_y += self.agent_velocity_y * timestep
        self.agent_angle += self.agent_angular_velocity * timestep
        
        # Constrain to world bounds
        self.agent_x = max(5, min(self.agent_x, self.width - 5))
        self.agent_y = max(5, min(self.agent_y, self.height - 5))
        self.agent_angle = (self.agent_angle + math.pi) % (2 * math.pi) - math.pi
    
    def _check_collisions(self):
        """Check and handle collisions"""
        # Check wall collisions
        if self.agent_x < 5 or self.agent_x > self.width - 5:
            self.agent_velocity_x *= -0.5
        
        if self.agent_y < 5 or self.agent_y > self.height - 5:
            self.agent_velocity_y *= -0.5
        
        # Check resource collisions
        for resource in self.resources:
            distance = math.sqrt((resource['x'] - self.agent_x) ** 2 + 
                               (resource['y'] - self.agent_y) ** 2)
            if distance < 2.0:
                print(f"Agent collected resource worth {resource['value']}")
                self.resources.remove(resource)
        
        # Check hazard collisions
        for hazard in self.hazards:
            distance = math.sqrt((hazard['x'] - self.agent_x) ** 2 + 
                               (hazard['y'] - self.agent_y) ** 2)
            if distance < hazard['radius']:
                print(f"Agent hit hazard!")
                self.agent_velocity_x *= 0.5
                self.agent_velocity_y *= 0.5
    
    def _update_environment(self, timestep: float):
        """Update environment state"""
        # Add new resources over time
        if len(self.resources) < 10 and self.current_time % 100 < timestep:
            x = random.uniform(10, self.width - 10)
            y = random.uniform(10, self.height - 10)
            self.resources.append({
                'x': x, 'y': y,
                'type': pynlm.WorldObjectType.Resource,
                'value': random.uniform(1, 5),
                'mass': random.uniform(5, 15),
                'static': True
            })
        
        # Environmental effects
        if int(self.current_time) % 200 == 0:  # Every 200 simulation time units
            # Spawn hazard
            x = random.uniform(20, self.width - 20)
            y = random.uniform(20, self.height - 20)
            self.hazards.append({
                'x': x, 'y': y,
                'type': pynlm.WorldObjectType.Hazard,
                'value': 1.0,
                'radius': 3.0,
                'static': True
            })
    
    def get_sensory_percept(self) -> pynlm.SensoryPercept:
        """Get current sensory percept with physics context"""
        percept = pynlm.SensoryPercept()
        
        # Vision component (simplified)
        vision = pynlm.Vision()
        # In practice, you'd implement proper vision simulation
        # This is a simplified placeholder
        percept.setVision(vision)
        
        # Internal signals based on physics
        internal = pynlm.InternalSignals()
        
        # Add signals based on environment conditions
        distance_to_nearest_resource = float('inf')
        for resource in self.resources:
            dist = math.sqrt((resource['x'] - self.agent_x) ** 2 + 
                           (resource['y'] - self.agent_y) ** 2)
            distance_to_nearest_resource = min(distance_to_nearest_resource, dist)
        
        internal.addSignal(1.0 / (distance_to_nearest_resource + 1))  # Resource proximity
        internal.addSignal(self.agent_velocity_x)  # Movement velocity
        internal.addSignal(abs(self.agent_angular_velocity))  # Turning rate
        
        percept.setInternal(internal)
        
        return percept
    
    def apply_motor_command(self, action: pynlm.Action, current_time: float) -> pynlm.ActionResult:
        """Apply motor command with physics simulation"""
        result = pynlm.ActionResult()
        
        # Parse action and convert to forces/torque
        action_type = action.getType()
        parameters = action.getParameters()
        
        if action_type == pynlm.ActionType.MoveForward:
            force_magnitude = parameters[0] if parameters else 1.0
            angle_rad = self.agent_angle
            force_x = force_magnitude * math.cos(angle_rad)
            force_y = force_magnitude * math.sin(angle_rad)
        
        elif action_type == pynlm.ActionType.MoveBackward:
            force_magnitude = parameters[0] if parameters else 1.0
            angle_rad = self.agent_angle
            force_x = -force_magnitude * math.cos(angle_rad)
            force_y = -force_magnitude * math.sin(angle_rad)
        
        elif action_type == pynlm.ActionType.TurnLeft:
            torque = parameters[0] if parameters else 0.1
            self.agent_angular_velocity += torque
        
        elif action_type == pynlm.ActionType.TurnRight:
            torque = parameters[0] if parameters else 0.1
            self.agent_angular_velocity -= torque
        
        elif action_type == pynlm.ActionType.Interact:
            # Check for interaction with nearby objects
            distance_to_nearest = float('inf')
            nearest_object = None
            
            for resource in self.resources:
                dist = math.sqrt((resource['x'] - self.agent_x) ** 2 + 
                               (resource['y'] - self.agent_y) ** 2)
                if dist < distance_to_nearest:
                    distance_to_nearest = dist
                    nearest_object = resource
            
            for hazard in self.hazards:
                dist = math.sqrt((hazard['x'] - self.agent_x) ** 2 + 
                               (hazard['y'] - self.agent_y) ** 2)
                if dist < distance_to_nearest:
                    distance_to_nearest = dist
                    nearest_object = hazard
            
            if nearest_object:
                result.success = True
                result.reward = nearest_object['value'] if nearest_object['type'] == pynlm.WorldObjectType.Resource else -nearest_object['value']
                result.message = f"Interacted with {nearest_object['type']}"
        
        else:
            result.success = True
            result.reward = 0.0
            result.message = "No interaction"
        
        return result

# Usage example
class PhysicsBasedAgent:
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.custom_world = None
    
    def setup(self):
        """Setup physics-based simulation"""
        # Create brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create custom physics world
        self.custom_world = PhysicsBasedWorld(width=200, height=200)
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
    
    def run_simulation(self, num_steps: int = 1000):
        """Run physics-based simulation"""
        print(f"Starting physics-based simulation with {num_steps} steps")
        
        for step in range(num_steps):
            # Update physics world
            self.custom_world.update(0.1)
            
            # Get sensory input
            percept = self.custom_world.get_sensory_percept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply motor command to physics world
            result = self.custom_world.apply_motor_command(action, self.custom_world.current_time)
            
            # Apply reward modulation
            if result.success:
                self.agent.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Print status
            if step % 100 == 0:
                print(f"Step {step}: Agent at ({self.custom_world.agent_x:.1f}, {self.custom_world.agent_y:.1f}), "
                      f"Velocity ({self.custom_world.agent_velocity_x:.2f}, {self.custom_world.agent_velocity_y:.2f}), "
                      f"Reward: {result.reward}")
        
        return {
            'final_position': (self.custom_world.agent_x, self.custom_world.agent_y),
            'final_velocity': (self.custom_world.agent_velocity_x, self.custom_world.agent_velocity_y),
            'final_angle': self.custom_world.agent_angle,
            'resources_remaining': len(self.custom_world.resources),
            'hazards_remaining': len(self.custom_world.hazards),
        }

# Usage
physics_agent = PhysicsBasedAgent()
physics_agent.setup()
results = physics_agent.run_simulation(500)
print(f"\nPhysics simulation completed: {results}")
```

### 6. Testing and Validation

#### Unit Testing Framework

```python
import pynlm
import unittest
from typing import Dict, Any

class BrainUnitTests(unittest.TestCase):
    """Unit tests for Brain functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up after tests"""
        if self.brain:
            self.brain.reset()
    
    def test_brain_creation(self):
        """Test basic brain creation"""
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        # Brain should already be initialized
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
    
    def test_step_functionality(self):
        """Test basic step functionality"""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run step
        self.brain.step(0)
        
        # Spikes should increase or stay the same
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), initial_spikes)
    
    def test_region_management(self):
        """Test region management"""
        initial_count = self.brain.getRegionCount()
        
        # Add region
        region_id = self.brain.addRegion("test_region")
        self.assertIsNotNone(region_id)
        
        # Check region count increased
        self.assertEqual(self.brain.getRegionCount(), initial_count + 1)
        
        # Get region
        region = self.brain.getRegion(region_id)
        self.assertIsNotNone(region)
    
    def test_configuration_access(self):
        """Test configuration access"""
        # Test configuration
        self.config.set("test.key", "test_value")
        self.assertTrue(self.config.has("test.key"))
        
        # Get value
        value = self.config.get("test.key")
        self.assertEqual(value, "test_value")
        
        # Test getOr with default
        default_value = self.config.getOr("nonexistent.key", "default")
        self.assertEqual(default_value, "default")
    
    def test_agent_functionality(self):
        """Test agent functionality"""
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Check subsystem status
        self.assertTrue(agent.isRewardModulationEnabled())
        self.assertTrue(agent.isCuriosityEnabled())
        
        # Test sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Test motor decoding
        action = agent.decodeMotorCommand()
        self.assertIsNotNone(action)
        self.assertIsNotNone(action.getType())

class ConfigUnitTests(unittest.TestCase):
    """Unit tests for Config functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = pynlm.createDefaultConfig()
    
    def test_config_creation(self):
        """Test config creation"""
        self.assertIsNotNone(self.config)
    
    def test_config_set_get(self):
        """Test config set and get operations"""
        # Test different data types
        self.config.set("string_value", "hello")
        self.config.set("int_value", 42)
        self.config.set("float_value", 3.14)
        self.config.set("bool_value", True)
        
        self.assertEqual(self.config.get("string_value").value(), "hello")
        self.assertEqual(self.config.get("int_value").value(), 42)
        self.assertAlmostEqual(self.config.get("float_value").value(), 3.14)
        self.assertEqual(self.config.get("bool_value").value(), True)
    
    def test_config_get_or(self):
        """Test config getOr method"""
        # Test with existing key
        self.config.set("existing", "value")
        self.assertEqual(self.config.getOr("existing", "default"), "value")
        
        # Test with missing key
        self.assertEqual(self.config.getOr("missing", "default"), "default")
    
    def test_config_has(self):
        """Test config has method"""
        self.assertFalse(self.config.has("nonexistent"))
        
        self.config.set("test", "value")
        self.assertTrue(self.config.has("test"))
    
    def test_config_remove(self):
        """Test config remove method"""
        self.config.set("test", "value")
        self.assertTrue(self.config.has("test"))
        
        self.config.remove("test")
        self.assertFalse(self.config.has("test"))
    
    def test_config_keys(self):
        """Test config getKeys method"""
        initial_keys = self.config.getKeys()
        
        self.config.set("new_key", "value")
        
        new_keys = self.config.getKeys()
        self.assertEqual(len(new_keys), len(initial_keys) + 1)
        self.assertIn("new_key", new_keys)

class WorldUnitTests(unittest.TestCase):
    """Unit tests for World functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_world_creation(self):
        """Test world creation"""
        self.assertIsNotNone(self.world)
        self.assertEqual(self.world.getWidth(), 20)
        self.assertEqual(self.world.getHeight(), 20)
        self.assertEqual(self.world.getMaxEnergy(), 100.0)
    
    def test_world_update(self):
        """Test world update"""
        initial_time = self.world.getSimulationTime()
        
        self.world.update(0.1)
        
        self.assertGreater(self.world.getSimulationTime(), initial_time)
    
    def test_world_configure(self):
        """Test world configuration"""
        self.world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        
        self.assertEqual(self.world.getWidth(), 30)
        self.assertEqual(self.world.getHeight(), 30)
    
    def test_world_agent_interaction(self):
        """Test world-agent interaction"""
        body = self.world.getAgentBody()
        self.assertEqual(body.x, 10.0)  # Default starting position
        self.assertEqual(body.y, 10.0)
        
        # Move agent
        action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0])
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Check that agent moved
        self.assertNotEqual(body.x, 10.0) or self.assertNotEqual(body.y, 10.0)
    
    def test_world_objects(self):
        """Test world object management"""
        # Create test object
        obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
        
        # Add object
        self.world.addObject(obj)
        
        # Remove object
        self.world.removeObject(5.0, 5.0)

class AgentUnitTests(unittest.TestCase):
    """Unit tests for Agent functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_creation(self):
        """Test agent creation"""
        self.assertIsNotNone(self.agent)
    
    def test_agent_initialization(self):
        """Test agent initialization"""
        # Agent should already be initialized
        self.assertGreater(self.agent.getSensoryInputSize(), 0)
        self.assertGreater(self.agent.getMotorOutputSize(), 0)
    
    def test_agent_subsystem_control(self):
        """Test agent subsystem control"""
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Check status
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())
        
        # Disable subsystems
        self.agent.enableRewardModulation(False)
        self.agent.enableCuriosity(False)
        
        # Check status
        self.assertFalse(self.agent.isRewardModulationEnabled())
        self.assertFalse(self.agent.isCuriosityEnabled())
    
    def test_agent_neuromodulation(self):
        """Test agent neuromodulation"""
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Check neuromodulation levels
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        neuromodulation = self.agent.getNeuromodulationLevel()
        
        self.assertGreaterEqual(curiosity, 0.0)
        self.assertGreaterEqual(novelty, 0.0)
        self.assertGreaterEqual(neuromodulation, 0.0)
    
    def test_agent_development(self):
        """Test agent development"""
        initial_stage = self.agent.getDevelopmentalStage()
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Development stage may change
        self.assertGreaterEqual(self.agent.getDevelopmentalStage(), initial_stage)
    
    def test_agent_reset(self):
        """Test agent reset"""
        # Modify some state
        self.agent.applyRewardModulation(1.0, 0.0)
        
        # Reset agent
        self.agent.reset()
        
        # State should be reset
        # (Specific reset behavior depends on implementation)

class IntegrationTests:
    """Integration tests for complete system"""
    
    @staticmethod
    def run_complete_agent_episode(max_steps: int = 500) -> Dict[str, Any]:
        """Run complete agent episode and return results"""
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run episode
        results = {
            'steps_completed': 0,
            'final_curiosity': 0.0,
            'final_novelty': 0.0,
            'final_prediction_error': 0.0,
            'total_spikes': 0,
            'development_stage': None,
        }
        
        for step in range(max_steps):
            # Update world
            world.update(0.1)
            
            # Get and process percept
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain
            brain.step(step)
            
            # Decode and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update learning
            reward = 0.0  # Simplified reward calculation
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            results['steps_completed'] = step + 1
            results['final_curiosity'] = agent.getCuriosityLevel()
            results['final_novelty'] = agent.getNoveltyLevel()
            results['final_prediction_error'] = agent.getPredictionError()
            results['total_spikes'] = brain.getTotalSpikeCount()
            results['development_stage'] = brain.getDevelopmentalStage()
        
        return results

# Run all tests
def run_all_tests():
    """Run all unit tests"""
    print("Running NLM Python Bindings Unit Tests...")
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(BrainUnitTests)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(ConfigUnitTests))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(WorldUnitTests))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(AgentUnitTests))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Run integration tests
    print("\nRunning Integration Tests...")
    integration_results = IntegrationTests.run_complete_agent_episode(100)
    print("Integration Test Results:")
    for key, value in integration_results.items():
        print(f"  {key}: {value}")
    
    return result.wasSuccessful()

if __name__ == '__main__':
    success = run_all_tests()
    if success:
        print("\nAll tests passed!")
    else:
        print("\nSome tests failed!")
    exit(0 if success else 1)
```

### 7. Performance Monitoring and Profiling

#### Real-time Performance Monitoring

```python
import pynlm
import time
import threading
from typing import Dict, Any, List

class PerformanceMonitor:
    """Real-time performance monitoring for NLM simulations"""
    
    def __init__(self, brain: pynlm.Brain, sampling_rate: float = 10.0):
        self.brain = brain
        self.sampling_rate = sampling_rate  # Hz
        self.sampling_interval = 1.0 / sampling_rate
        self.data_collector = DataCollector(sampling_rate)
        self.monitoring_thread = None
        self.is_monitoring = False
        
        # Performance metrics
        self.cpu_usage = []
        self.memory_usage = []
        self.step_times = []
        self.neural_activity = []
        self.development_metrics = []
    
    def start_monitoring(self):
        """Start real-time performance monitoring"""
        self.is_monitoring = True
        self.monitoring_thread = threading.Thread(target=self._monitor_loop)
        self.monitoring_thread.daemon = True
        self.monitoring_thread.start()
        print(f"Performance monitoring started at {self.sampling_rate} Hz")
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.is_monitoring = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
            print("Performance monitoring stopped")
    
    def _monitor_loop(self):
        """Main monitoring loop"""
        last_time = time.time()
        
        while self.is_monitoring:
            current_time = time.time()
            
            # Check if it's time to collect data
            if current_time - last_time >= self.sampling_interval:
                self._collect_performance_data()
                last_time = current_time
            
            time.sleep(0.01)  # Small sleep to prevent busy waiting
    
    def _collect_performance_data(self):
        """Collect performance data"""
        start_time = time.time()
        
        # Collect basic brain metrics
        step_data = {
            'timestamp': time.time(),
            'step_time': time.time() - start_time,
            'neurons_firing': self.brain.getFiringNeuronCount(),
            'total_neutrons': self.brain.getTotalNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'development_stage': self.brain.getDevelopmentalStage(),
        }
        
        # Add memory usage (simplified)
        import sys
        memory_usage = sys.getsizeof(self.brain)
        step_data['memory_usage'] = memory_usage
        
        # Add CPU usage (simplified)
        cpu_usage = 0.0  # Would need proper CPU monitoring
        step_data['cpu_usage'] = cpu_usage
        
        # Add development metrics
        step_data['plasticity_score'] = min(1.0, self.brain.getTotalSpikeCount() / 1000.0)
        step_data['maturity_score'] = {
            'Initial': 0.1,
            'CriticalPeriod': 0.5,
            'Maturation': 0.7,
            'Adult': 0.9,
            'Aging': 1.0
        }[step_data['development_stage']]
        
        # Store data
        self.data_collector.add_data(step_data)
        
        # Update metrics for analysis
        self.step_times.append(step_data['step_time'])
        self.neural_activity.append(step_data['neurons_firing'])
        self.development_metrics.append(step_data['development_stage'])
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Generate performance report"""
        if not self.data_collector.data:
            return {"error": "No performance data collected"}
        
        recent_data = self.data_collector.get_recent_data()
        
        report = {
            'monitoring_stats': {
                'total_samples': len(self.data_collector.data),
                'sampling_rate': self.sampling_rate,
                'total_duration': (max(d['timestamp'] for d in self.data_collector.data) - 
                                 min(d['timestamp'] for d in self.data_collector.data)),
            },
            
            'neural_performance': {
                'average_firing_rate': sum(d['average_firing_rate'] for d in recent_data) / len(recent_data),
                'firing_rate_variance': sum((d['average_firing_rate'] - sum(d['average_firing_rate'] for d in recent_data) / len(recent_data)) ** 2 for d in recent_data) / len(recent_data),
                'neurons_firing_mean': sum(d['neurons_firing'] for d in recent_data) / len(recent_data),
                'neurons_firing_std': (sum((d['neurons_firing'] - sum(d['neurons_firing'] for d in recent_data) / len(recent_data)) ** 2 for d in recent_data) / len(recent_data)) ** 0.5,
            },
            
            'system_performance': {
                'average_step_time': sum(d['step_time'] for d in recent_data) / len(recent_data),
                'step_time_min': min(d['step_time'] for d in recent_data),
                'step_time_max': max(d['step_time'] for d in recent_data),
                'memory_usage_mean': sum(d['memory_usage'] for d in recent_data) / len(recent_data),
                'cpu_usage_mean': sum(d['cpu_usage'] for d in recent_data) / len(recent_data),
            },
            
            'development_trends': {
                'stage_distribution': self._count_development_stages(recent_data),
                'maturity_trend': self._calculate_maturity_trend(recent_data),
                'plasticity_effectiveness': self._calculate_plasticity_effectiveness(recent_data),
            },
            
            'health_indicators': self._calculate_health_indicators(recent_data),
        }
        
        return report
    
    def _count_development_stages(self, data: List[Dict]) -> Dict[str, float]:
        """Count development stages"""
        stage_counts = {}
        for d in data:
            stage = d['development_stage']
            stage_counts[stage] = stage_counts.get(stage, 0) + 1
        
        total = len(data)
        return {stage: count / total for stage, count in stage_counts.items()}
    
    def _calculate_maturity_trend(self, data: List[Dict]) -> str:
        """Calculate maturity trend"""
        if len(data) < 2:
            return "insufficient_data"
        
        maturity_scores = [{
            'Initial': 0.1,
            'CriticalPeriod': 0.5,
            'Maturation': 0.7,
            'Adult': 0.9,
            'Aging': 1.0
        }[d['development_stage']] for d in data]
        
        first_half = sum(maturity_scores[:len(maturity_scores)//2]) / (len(maturity_scores)//2)
        second_half = sum(maturity_scores[len(maturity_scores)//2:]) / (len(maturity_scores)//2)
        
        if second_half > first_half + 0.1:
            return "increasing"
        elif second_half < first_half - 0.1:
            return "decreasing"
        else:
            return "stable"
    
    def _calculate_plasticity_effectiveness(self, data: List[Dict]) -> float:
        """Calculate plasticity effectiveness"""
        if len(data) < 2:
            return 0.0
        
        # Plasticity effectiveness is based on firing rate changes
        firing_rates = [d['average_firing_rate'] for d in data]
        
        # Calculate coefficient of variation
        mean_firing = sum(firing_rates) / len(firing_rates)
        if mean_firing == 0:
            return 0.0
        
        std_firing = (sum((fr - mean_firing) ** 2 for fr in firing_rates) / len(firing_rates)) ** 0.5
        cv = std_firing / mean_firing if mean_firing > 0 else 0.0
        
        # Normalize to 0-1 range
        effectiveness = min(1.0, cv * 10)  # Scaling factor
        
        return effectiveness
    
    def _calculate_health_indicators(self, data: List[Dict]) -> Dict[str, Any]:
        """Calculate health indicators"""
        if not data:
            return {"error": "No data available"}
        
        # Calculate firing rate stability
        firing_rates = [d['average_firing_rate'] for d in data]
        mean_firing = sum(firing_rates) / len(firing_rates)
        std_firing = (sum((fr - mean_firing) ** 2 for fr in firing_rates) / len(firing_rates)) ** 0.5
        cv = std_firing / mean_firing if mean_firing > 0 else 0.0
        
        # Health indicators
        health_indicators = {
            'neural_stability': max(0.0, 1.0 - cv),  # Stability inversely related to CV
            'learning_efficiency': min(1.0, mean_firing / 100.0),  # Normalized firing rate
            'development_health': self._get_development_health(data),
            'system_efficiency': 1.0 / (sum(d['step_time'] for d in data) / len(data)),  # Inverse of step time
        }
        
        return health_indicators
    
    def _get_development_health(self, data: List[Dict]) -> Dict[str, float]:
        """Get development health indicators"""
        stage_scores = {
            'Initial': 0.2,
            'CriticalPeriod': 0.8,
            'Maturation': 0.9,
            'Adult': 1.0,
            'Aging': 0.7
        }
        
        stage_scores_list = [stage_scores[d['development_stage']] for d in data]
        average_score = sum(stage_scores_list) / len(stage_scores_list)
        
        return {
            'maturity': average_score,
            'plasticity': stage_scores_list[-1] if stage_scores_list else 0.0,
            'stability': 1.0 - (max(stage_scores_list) - min(stage_scores_list)),  # Stability based on score variance
        }

class DataCollector:
    """Data collector for performance monitoring"""
    
    def __init__(self, sampling_rate: float):
        self.sampling_rate = sampling_rate
        self.data = []
        self.max_data_points = 10000  # Maximum data points to store
    
    def add_data(self, data_point: Dict):
        """Add data point"""
        self.data.append(data_point)
        
        # Limit data size
        if len(self.data) > self.max_data_points:
            # Remove oldest data points
            self.data = self.data[-self.max_data_points:]
    
    def get_recent_data(self, time_window: float = 60.0) -> List[Dict]:
        """Get recent data within time window"""
        if not self.data:
            return []
        
        cutoff_time = self.data[-1]['timestamp'] - time_window
        return [d for d in self.data if d['timestamp'] >= cutoff_time]
    
    def clear(self):
        """Clear all data"""
        self.data = []

# Usage example
class PerformanceMonitoringExample:
    def __init__(self):
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create monitor
        self.monitor = PerformanceMonitor(self.brain, sampling_rate=5.0)  # 5 Hz
        
        # Create world for context
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
    
    def run_monitoring_experiment(self, duration: float = 120.0):
        """Run experiment with performance monitoring"""
        print(f"Starting performance monitoring experiment ({duration}s)")
        
        start_time = time.time()
        end_time = start_time + duration
        
        # Start monitoring
        self.monitor.start_monitoring()
        
        try:
            step = 0
            while time.time() < end_time:
                # Update world
                self.world.update(0.1)
                
                # Get and process percept
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Run brain
                self.brain.step(step)
                
                # Decode and apply action
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Update learning
                reward = 0.0  # Simplified
                self.agent.applyRewardModulation(reward, 0.0)
                self.agent.updateDevelopment(0.1)
                
                step += 1
                
                if step % 50 == 0:
                    print(f"Step {step}: {self.brain.getFiringNeuronCount()} neurons firing")
        
        finally:
            # Stop monitoring
            self.monitor.stop_monitoring()
        
        # Generate performance report
        report = self.monitor.get_performance_report()
        
        print(f"\nExperiment completed: {step} steps in {duration}s")
        print("Performance Report:")
        for category, metrics in report.items():
            print(f"  {category}:")
            if isinstance(metrics, dict):
                for key, value in metrics.items():
                    print(f"    {key}: {value:.4f}")
        
        return report

# Usage
if __name__ == "__main__":
    example = PerformanceMonitoringExample()
    report = example.run_monitoring_experiment(duration=60.0)
```

## Conclusion

The NLM Python bindings provide a powerful interface for neural simulation research. This advanced documentation covers:

1. **Performance Optimization** - Profiling, memory management, and efficiency techniques
2. **Advanced Configuration** - Templates, presets, and experiment management
3. **Complex Agent Programming** - Behavior trees, custom environments, and integration patterns
4. **State Management** - Persistent storage, recovery, and session management
5. **Testing and Validation** - Comprehensive unit tests and integration testing
6. **Performance Monitoring** - Real-time monitoring and profiling tools

These advanced features enable you to build sophisticated neural simulation systems, conduct research experiments, and optimize performance for your specific use cases.

**Happy simulating! 🤖**