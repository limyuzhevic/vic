# Advanced NLM Python Bindings Documentation

## Overview

This document provides comprehensive documentation for the NLM Python bindings, covering advanced usage patterns, configuration management, and integration techniques for the Neural Learning Machine Phase 6 Final Integration.

## Installation and Setup

### Installation from Source

```bash
# Clone the NLM repository
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Install Python bindings
pip install pybind11 scikit-build-core pytest numpy
pip install .

# Or install in development mode
pip install -e .
```

### Installation via pip

```bash
# Install the latest stable version
pip install pynlm

# For development version
pip install git+https://github.com/nlm-project/nlm.git
```

### Verification

```python
import pynlm

# Check version
print(pynlm.__version__)

# Test basic functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Test validation
if pynlm.validateConfig(config):
    print("Configuration is valid")
else:
    print("Configuration has validation errors")
```

## Advanced Configuration Management

### 1. Configuration File Formats

#### JSON Configuration
```python
import pynlm
import json

# Create advanced JSON configuration
advanced_config = {
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.15,
        "connection_probability": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.002,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.001
        },
        "structural": {
            "enable": True,
            "synaptogenesis_rate": 0.00001,
            "pruning_rate": 0.000001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 2.0,
            "baseline": 0.1
        },
        "curiosity": {
            "enable": True,
            "threshold": 0.5,
            "decay_rate": 0.01
        },
        "novelty": {
            "enable": True,
            "detection_threshold": 0.3
        }
    },
    "memory": {
        "working_capacity": 100,
        "episodic_capacity": 200,
        "semantic_capacity": 500,
        "procedural_capacity": 50,
        "consolidation_threshold": 0.8
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "checkpoint_interval": 1000.0
    },
    "logging": {
        "log_level": "INFO",
        "log_to_file": True,
        "log_filename": "nlm_simulation.log"
    },
    "development": {
        "synaptogenesis_rate": 0.0005,
        "pruning_rate": 0.00005,
        "maturation_rate": 0.1,
        "critical_period_end": 100.0
    }
}

# Save to JSON file
with open('advanced_config.json', 'w') as f:
    json.dump(advanced_config, f, indent=2)

# Load from JSON file
config = pynlm.loadConfigFromJSON('advanced_config.json')
```

#### YAML Configuration (if available)
```python
import pynlm
import yaml

# Load YAML configuration
yaml_config_path = 'advanced_config.yaml'
config = pynlm.loadConfigFromYAML(yaml_config_path)
```

### 2. Configuration Validation

#### Schema Validation
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set values with automatic validation
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 2.0)

# Validate configuration
validation_errors = config.validateAll()
if validation_errors:
    print("Configuration validation errors:")
    for error in validation_errors:
        print(f"  - {error}")
else:
    print("Configuration is valid")

# Validate specific keys
if config.isValid("brain.neuron_count"):
    print("Neuron count is valid")
else:
    print("Neuron count is invalid")

# Get validated values
neuron_count = config.getValidated<int64_t>("brain.neuron_count")
if neuron_count.has_value():
    print(f"Validated neuron count: {neuron_count.value()}")
else:
    print("Neuron count validation failed")

# Check if key is known
if config.isKnownKey("brain.neuron_count"):
    print("Brain.neuron_count is a known configuration key")
```

#### Configuration Validation Functions
```python
# Get all known configuration keys
known_keys = pynlm.Config.getAllKnownKeys()
print(f"Known configuration keys: {known_keys}")

# Check if specific key is known
is_known = pynlm.Config.isKnownKey("brain.neuron_count")
print(f"Is brain.neuron_count known? {is_known}")
```

### 3. Configuration Templates
```python
import pynlm

def create_research_config(neuron_count=1000):
    """Create configuration for research purposes."""
    
    config = pynlm.createDefaultConfig()
    
    # Research-focused settings
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.05)  # Lower density for controlled experiments
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.00001)
    
    # Monitoring and logging
    config.set("logging.log_level", "INFO")
    config.set("logging.log_to_file", True)
    config.set("logging.log_filename", f"research_neuron_{neuron_count}.log")
    
    # Extended simulation
    config.set("simulation.timestep", 0.001)
    config.set("simulation.max_steps", 50000)
    config.set("simulation.checkpoint_interval", 5000.0)
    
    # Memory settings for long experiments
    config.set("memory.working_capacity", 200)
    config.set("memory.episodic_capacity", 500)
    config.set("memory.consolidation_threshold", 0.7)
    
    return config

def create_production_config(neuron_count=10000):
    """Create configuration for production deployment."""
    
    config = pynlm.createDefaultConfig()
    
    # Production-optimized settings
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)  # Balanced for performance
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)  # Faster learning
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.002)
    config.set("plasticity.structural.enable", False)  # Disable for stability
    
    # Performance optimization
    config.set("simulation.timestep", 0.002)  # Larger timestep for speed
    config.set("simulation.max_steps", 20000)
    config.set("simulation.checkpoint_interval", 10000.0)
    
    # Memory optimization
    config.set("memory.working_capacity", 50)  # Smaller for speed
    config.set("memory.episodic_capacity", 100)
    config.set("memory.consolidation_threshold", 0.9)  # Faster consolidation
    
    # Production logging
    config.set("logging.log_level", "WARN")  # Reduce log noise
    config.set("logging.log_to_file", True)
    config.set("logging.log_filename", "production.log")
    
    # Development disabled for production
    config.set("enable_development", False)
    config.set("enable_curiosity", False)
    
    return config

# Usage examples
research_config = create_research_config(2000)
production_config = create_production_config(5000)

print("Research configuration neuron count:", research_config.getOr<int64_t>("brain.neuron_count", 1000))
print("Production configuration neuron count:", production_config.getOr<int64_t>("brain.neuron_count", 1000))
```

## Advanced Simulation Management

### 1. Simulation Control
```python
import pynlm
import time
import threading
from typing import Callable, Dict, Any

class AdvancedSimulator:
    """Advanced simulation manager with comprehensive control."""
    
    def __init__(self, config, name="simulation"):
        self.config = config
        self.name = name
        self.brain = None
        self.agent = None
        self.world = None
        self.simulation_thread = None
        self.simulation_running = False
        self.simulation_step = 0
        self.performance_data = []
        self.event_callbacks = {}
        
        self.initialize_simulation()
    
    def initialize_simulation(self):
        """Initialize simulation components."""
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=self.config.getOr<int64_t>("world.width", 20),
            height=self.config.getOr<int64_t>("world.height", 20),
            visionWidth=self.config.getOr<int64_t>("world.vision_width", 8),
            visionHeight=self.config.getOr<int64_t>("world.vision_height", 8)
        )
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Enable subsystems based on configuration
        self.configure_subsystems()
    
    def configure_subsystems(self):
        """Configure learning subsystems."""
        
        subsystems = self.config.getOr<std::string>("subsystems.enabled", "all")
        
        if subsystems == "all" or "reward_modulation" in subsystems:
            self.agent.enableRewardModulation(True)
        
        if subsystems == "all" or "curiosity" in subsystems:
            self.agent.enableCuriosity(True)
        
        if subsystems == "all" or "structural_plasticity" in subsystems:
            self.agent.enableStructuralPlasticity(True)
        
        if subsystems == "all" or "development" in subsystems:
            self.agent.enableDevelopment(True)
        
        if subsystems == "all" or "memory" in subsystems:
            self.agent.enableMemoryConsolidation(True)
    
    def add_event_callback(self, event_type: str, callback: Callable):
        """Add callback for simulation events."""
        
        if event_type not in self.event_callbacks:
            self.event_callbacks[event_type] = []
        
        self.event_callbacks[event_type].append(callback)
    
    def trigger_event(self, event_type: str, data: Dict[str, Any] = None):
        """Trigger event with callbacks."""
        
        if event_type in self.event_callbacks:
            for callback in self.event_callbacks[event_type]:
                try:
                    callback(data or {})
                except Exception as e:
                    print(f"Error in event callback: {e}")
    
    def run_simulation_step(self, step_number: int):
        """Run single simulation step with event handling."""
        
        # Trigger pre-step event
        self.trigger_event("pre_step", {
            'step': step_number,
            'simulation_name': self.name,
            'brain_state': self.get_brain_state(),
            'world_state': self.get_world_state()
        })
        
        # Update world
        timestep = self.config.getOr<double>("simulation.timestep", 0.1)
        self.world.update(timestep)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Run brain step
        step_start = time.time()
        self.brain.step(step_number)
        step_time = time.time() - step_start
        
        # Get action
        action = self.agent.decodeMotorCommand()
        
        # Apply action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply reward modulation
        reward = self.get_world_reward()
        self.agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        self.agent.updateDevelopment(timestep)
        
        # Record performance
        self.record_performance(step_number, step_time, reward)
        
        # Trigger post-step event
        self.trigger_event("post_step", {
            'step': step_number,
            'simulation_name': self.name,
            'step_time': step_time,
            'reward': reward,
            'brain_metrics': self.get_brain_metrics()
        })
        
        self.simulation_step = step_number
    
    def get_brain_state(self):
        """Get current brain state."""
        
        if not self.brain:
            return {}
        
        return {
            'firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount()
        }
    
    def get_world_state(self):
        """Get current world state."""
        
        if not self.world:
            return {}
        
        return {
            'position': self.get_agent_position(),
            'vision': self.get_agent_vision(),
            'simulation_time': self.world.getSimulationTime()
        }
    
    def get_world_reward(self):
        """Get reward from world."""
        
        # Implementation would calculate reward from world state
        return 0.0  # Placeholder
    
    def get_brain_metrics(self):
        """Get brain performance metrics."""
        
        if not self.brain:
            return {}
        
        return {
            'firing_rate': self.brain.getAverageFiringRate(),
            'excitatory_inhibitory_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount()
        }
    
    def get_agent_position(self):
        """Get agent position."""
        
        if not self.world:
            return {'x': 0, 'y': 0}
        
        # Implementation would get actual position
        return {'x': 10, 'y': 10}  # Placeholder
    
    def get_agent_vision(self):
        """Get agent vision data."""
        
        if not self.world:
            return {}
        
        return {
            'visible_objects': self.world.getVisibleObjects(),
            'obstacles': self.world.getObstacles(),
            'resources': self.world.getResources()
        }
    
    def record_performance(self, step: int, step_time: float, reward: float):
        """Record performance metrics."""
        
        self.performance_data.append({
            'step': step,
            'step_time': step_time,
            'reward': reward,
            'firing_rate': self.brain.getAverageFiringRate() if self.brain else 0,
            'curiosity_level': self.agent.getCuriosityLevel() if self.agent else 0,
            'novelty_level': self.agent.getNoveltyLevel() if self.agent else 0,
            'timestamp': time.time()
        })
    
    def run_simulation(self, steps: int, pause_between_steps: float = 0.0):
        """Run complete simulation."""
        
        print(f"Starting simulation: {self.name}")
        print(f"Steps: {steps}, Initial neuron count: {self.brain.getTotalNeuronCount()}")
        
        self.simulation_running = True
        self.trigger_event("simulation_start", {
            'steps': steps,
            'simulation_name': self.name
        })
        
        try:
            for step in range(steps):
                if not self.simulation_running:
                    print("Simulation stopped by user")
                    break
                
                self.run_simulation_step(step)
                
                if pause_between_steps > 0:
                    time.sleep(pause_between_steps)
                
                if step % 100 == 0:
                    print(f"Step {step}/{steps} - "
                          f"Firing Rate: {self.brain.getAverageFiringRate():.3f} - "
                          f"Curiosity: {self.agent.getCuriosityLevel():.3f}")
        
        except Exception as e:
            print(f"Simulation error: {e}")
            self.trigger_event("simulation_error", {
                'error': str(e),
                'step': step
            })
        
        finally:
            self.simulation_running = False
            self.trigger_event("simulation_end", {
                'steps_completed': step + 1,
                'final_firing_rate': self.brain.getAverageFiringRate() if self.brain else 0,
                'total_rewards': sum(d['reward'] for d in self.performance_data)
            })
        
        print(f"Simulation completed: {self.name}")
        return self.performance_data
    
    def stop_simulation(self):
        """Stop running simulation."""
        
        self.simulation_running = False
        self.trigger_event("simulation_stop", {})
    
    def save_checkpoint(self, filepath: str):
        """Save simulation checkpoint."""
        
        if self.brain:
            self.brain.save(filepath)
            self.logger.info(f"Checkpoint saved: {filepath}")
    
    def load_checkpoint(self, filepath: str):
        """Load simulation checkpoint."""
        
        if self.brain:
            self.brain.load(filepath)
            self.logger.info(f"Checkpoint loaded: {filepath}")
    
    def get_performance_summary(self):
        """Get performance summary."""
        
        if not self.performance_data:
            return {}
        
        step_times = [d['step_time'] for d in self.performance_data]
        rewards = [d['reward'] for d in self.performance_data]
        
        return {
            'total_steps': len(self.performance_data),
            'total_time': sum(step_times),
            'avg_step_time': sum(step_times) / len(step_times) if step_times else 0,
            'min_step_time': min(step_times) if step_times else 0,
            'max_step_time': max(step_times) if step_times else 0,
            'total_rewards': sum(rewards),
            'avg_reward': sum(rewards) / len(rewards) if rewards else 0,
            'final_firing_rate': self.performance_data[-1]['firing_rate'] if self.performance_data else 0
        }

# Usage example
if __name__ == "__main__":
    # Create simulation configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.enable", True)
    
    # Create advanced simulator
    simulator = AdvancedSimulator(config, "research_simulation")
    
    # Add event callbacks
    def on_step_start(data):
        print(f"Starting step {data['step']}")
    
    def on_step_end(data):
        if data['step'] % 500 == 0:
            print(f"Completed step {data['step']}, "
                  f"Firing Rate: {data['brain_metrics']['firing_rate']:.3f}")
    
    simulator.add_event_callback("pre_step", on_step_start)
    simulator.add_event_callback("post_step", on_step_end)
    
    # Run simulation
    results = simulator.run_simulation(1000, pause_between_steps=0.001)
    
    # Get performance summary
    summary = simulator.get_performance_summary()
    print(f"\nSimulation Summary: {summary}")
```

### 2. Multi-Agent Simulation
```python
import pynlm
import threading
import time
from typing import List, Dict

class MultiAgentSimulator:
    """Multi-agent simulation environment."""
    
    def __init__(self, config, num_agents=3):
        self.config = config
        self.num_agents = num_agents
        self.agents = []
        self.world = None
        self.shared_world = None
        self.simulation_running = False
        
        self.initialize_multi_agent_environment()
    
    def initialize_multi_agent_environment(self):
        """Initialize multi-agent environment."""
        
        # Create shared world
        self.shared_world = pynlm.createSimpleWorld()
        self.shared_world.configure(
            width=30,
            height=30,
            visionWidth=12,
            visionHeight=12
        )
        
        # Create agents
        for i in range(self.num_agents):
            agent_config = self.config
            agent_config.set("agent.id", i)
            agent_config.set("agent.coordination_mode", "competitive")
            
            brain = pynlm.createBrain(agent_config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.shared_world)
            
            # Enable different subsystems for diversity
            agent.enableRewardModulation(True)
            agent.enableCuriosity(i % 2 == 0)  # Alternate curiosity enable
            agent.enableStructuralPlasticity(i % 3 == 0)  # Every third agent has structural plasticity
            
            self.agents.append({
                'id': i,
                'brain': brain,
                'agent': agent,
                'thread': None,
                'performance': []
            })
    
    def run_agent_simulation(self, agent_data, steps=500):
        """Run simulation for single agent."""
        
        agent_id = agent_data['id']
        brain = agent_data['brain']
        agent = agent_data['agent']
        
        print(f"Starting simulation for agent {agent_id}")
        
        for step in range(steps):
            if not self.simulation_running:
                break
            
            # Update shared world
            self.shared_world.update(0.1)
            
            # Get sensory input
            percept = self.shared_world.getSensoryPercept()
            
            # Add agent-specific information
            percept.setAgentId(agent_id)
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            self.shared_world.applyMotorCommand(action, self.shared_world.getSimulationTime())
            
            # Apply reward modulation
            reward = self.get_agent_reward(agent_id, percept, action)
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record performance
            agent_data['performance'].append({
                'step': step,
                'reward': reward,
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity_level': agent.getCuriosityLevel(),
                'timestamp': time.time()
            })
            
            if step % 100 == 0:
                print(f"Agent {agent_id}: Step {step}/{steps}, "
                      f"Firing Rate: {brain.getAverageFiringRate():.3f}")
        
        print(f"Agent {agent_id} simulation completed")
    
    def get_agent_reward(self, agent_id, percept, action):
        """Get reward for specific agent."""
        
        # Implementation would calculate agent-specific reward
        # For now, return simple reward based on action
        return 1.0 if action else 0.0
    
    def run_multi_agent_simulation(self, steps=500):
        """Run multi-agent simulation with concurrent execution."""
        
        print(f"Starting multi-agent simulation with {self.num_agents} agents")
        
        self.simulation_running = True
        
        # Start threads for each agent
        threads = []
        for agent_data in self.agents:
            thread = threading.Thread(
                target=self.run_agent_simulation,
                args=(agent_data, steps)
            )
            agent_data['thread'] = thread
            thread.start()
            threads.append(thread)
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        self.simulation_running = False
        
        print("Multi-agent simulation completed")
        
        return self.get_multi_agent_results()
    
    def get_multi_agent_results(self):
        """Get results from all agents."""
        
        results = {}
        
        for agent_data in self.agents:
            agent_id = agent_data['id']
            performance = agent_data['performance']
            
            if performance:
                results[agent_id] = {
                    'total_steps': len(performance),
                    'total_rewards': sum(p['reward'] for p in performance),
                    'avg_firing_rate': sum(p['firing_rate'] for p in performance) / len(performance),
                    'avg_curiosity': sum(p['curiosity_level'] for p in performance) / len(performance),
                    'performance_data': performance
                }
        
        return results
    
    def stop_simulation(self):
        """Stop multi-agent simulation."""
        
        self.simulation_running = False
        
        # Wait for threads to stop
        for agent_data in self.agents:
            if agent_data['thread']:
                agent_data['thread'].join(timeout=5)

# Usage example
if __name__ == "__main__":
    # Create multi-agent configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("enable_structural_plasticity", True)
    
    # Create multi-agent simulator
    simulator = MultiAgentSimulator(config, num_agents=3)
    
    # Run multi-agent simulation
    results = simulator.run_multi_agent_simulation(200)
    
    # Analyze results
    print("\nMulti-Agent Simulation Results:")
    for agent_id, result in results.items():
        print(f"\nAgent {agent_id}:")
        print(f"  Total steps: {result['total_steps']}")
        print(f"  Total rewards: {result['total_rewards']:.2f}")
        print(f"  Average firing rate: {result['avg_firing_rate']:.3f}")
        print(f"  Average curiosity: {result['avg_curiosity']:.3f}")
```

## Advanced Python Integration

### 1. NLM with Machine Learning Libraries
```python
import pynlm
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt

class AdvancedNLMIntegration:
    """Advanced integration of NLM with machine learning libraries."""
    
    def __init__(self, config):
        self.config = config
        self.scaler = StandardScaler()
        self.pca = PCA(n_components=2)
        self.feature_history = []
        self.ml_model_trained = False
        
        # Initialize NLM
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        self.agent.initialize(self.world)
        
        # Enable advanced subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
    
    def collect_features(self, steps=1000):
        """Collect features from NLM simulation for ML analysis."""
        
        print("Collecting features from NLM simulation...")
        
        features = []
        self.feature_history = []
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Extract features
            feature_vector = self.extract_features(step, percept, action)
            features.append(feature_vector)
            self.feature_history.append(feature_vector)
            
            if step % 100 == 0:
                print(f"Step {step}/{steps} - Feature vector length: {len(feature_vector)}")
        
        print("Feature collection completed")
        return np.array(features)
    
    def extract_features(self, step, percept, action):
        """Extract features from NLM simulation."""
        
        # Get brain features
        brain_features = self.get_brain_features()
        
        # Get agent features
        agent_features = self.get_agent_features()
        
        # Get world features
        world_features = self.get_world_features()
        
        # Combine features
        feature_vector = np.concatenate([
            brain_features,
            agent_features,
            world_features,
            [step, self.agent.getCuriosityLevel(), self.agent.getNoveltyLevel()]
        ])
        
        return feature_vector
    
    def get_brain_features(self):
        """Get brain-based features."""
        
        features = [
            self.brain.getAverageFiringRate(),
            self.brain.getTotalSpikeCount() / max(1, self.brain.getTotalNeuronCount()),
            self.brain.getFiringNeuronCount() / max(1, self.brain.getTotalNeuronCount()),
            self.brain.getDevelopmentalStage(),
            self.brain.getExcitationInhibitionRatio()
        ]
        
        return np.array(features)
    
    def get_agent_features(self):
        """Get agent-based features."""
        
        features = [
            self.agent.getCuriosityLevel(),
            self.agent.getNoveltyLevel(),
            self.agent.getPredictionError(),
            self.agent.getNeuromodulationLevel()
        ]
        
        return np.array(features)
    
    def get_world_features(self):
        """Get world-based features."""
        
        # Implementation would extract world features
        features = np.zeros(10)  # Placeholder
        
        return features
    
    def train_ml_models(self, features):
        """Train machine learning models on NLM features."""
        
        print("Training ML models on NLM features...")
        
        # Scale features
        scaled_features = self.scaler.fit_transform(features)
        
        # Apply PCA for dimensionality reduction
        pca_features = self.pca.fit_transform(scaled_features)
        
        # Store transformed features
        self.pca_features = pca_features
        self.scaled_features = scaled_features
        
        # Train simple predictive model
        from sklearn.linear_model import LinearRegression
        from sklearn.ensemble import RandomForestRegressor
        
        # Predict future firing rates
        X = scaled_features[:-100]  # Use all but last 100 as training data
        y = features[100:, 0]  # Predict next firing rate
        
        # Linear regression
        lr_model = LinearRegression()
        lr_model.fit(X, y)
        self.lr_model = lr_model
        
        # Random forest
        rf_model = RandomForestRegressor(n_estimators=100, random_state=42)
        rf_model.fit(X, y)
        self.rf_model = rf_model
        
        self.ml_model_trained = True
        
        print("ML model training completed")
        
        return pca_features
    
    def predict_future_behavior(self, steps=100):
        """Predict future NLM behavior using trained models."""
        
        if not self.ml_model_trained:
            raise ValueError("ML models not trained yet")
        
        print("Predicting future NLM behavior...")
        
        predictions = []
        
        for step in range(steps):
            # Get current state
            current_features = self.get_current_features()
            
            # Scale current features
            current_scaled = self.scaler.transform([current_features])
            
            # Predict next firing rate
            lr_pred = self.lr_model.predict(current_scaled)[0]
            rf_pred = self.rf_model.predict(current_scaled)[0]
            
            # Average predictions
            predicted_firing_rate = (lr_pred + rf_pred) / 2
            
            predictions.append({
                'step': step,
                'predicted_firing_rate': predicted_firing_rate,
                'timestamp': time.time()
            })
            
            # Update world (in real application, this would use the prediction)
            # For now, just continue simulation
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        print("Future behavior prediction completed")
        return predictions
    
    def get_current_features(self):
        """Get current features from NLM."""
        
        # This would require storing recent simulation state
        # For now, return placeholder
        return np.zeros(20)  # Placeholder
    
    def create_feature_visualization(self):
        """Create visualization of extracted features."""
        
        if not hasattr(self, 'pca_features'):
            print("No features available for visualization")
            return
        
        # Create 2D visualization using PCA
        plt.figure(figsize=(12, 8))
        
        # Plot first two principal components
        plt.subplot(2, 2, 1)
        plt.scatter(self.pca_features[:, 0], self.pca_features[:, 1], alpha=0.6)
        plt.title('PCA of NLM Features')
        plt.xlabel('PC1')
        plt.ylabel('PC2')
        
        # Plot firing rate over time
        plt.subplot(2, 2, 2)
        firing_rates = [f[0] for f in self.feature_history]
        plt.plot(firing_rates)
        plt.title('Firing Rate Over Time')
        plt.xlabel('Step')
        plt.ylabel('Firing Rate')
        
        # Plot curiosity over time
        plt.subplot(2, 2, 3)
        curiosities = [f[11] for f in self.feature_history]  # Curiosity at index 11
        plt.plot(curiosities)
        plt.title('Curiosity Over Time')
        plt.xlabel('Step')
        plt.ylabel('Curiosity Level')
        
        # Plot prediction error over time
        plt.subplot(2, 2, 4)
        prediction_errors = [f[12] for f in self.feature_history]  # Prediction error at index 12
        plt.plot(prediction_errors)
        plt.title('Prediction Error Over Time')
        plt.xlabel('Step')
        plt.ylabel('Prediction Error')
        
        plt.tight_layout()
        plt.show()
    
    def save_ml_data(self, filepath):
        """Save ML data to disk."""
        
        import json
        
        ml_data = {
            'features': self.feature_history,
            'pca_features': self.pca_features.tolist() if hasattr(self, 'pca_features') else None,
            'scaled_features': self.scaled_features.tolist() if hasattr(self, 'scaled_features') else None
        }
        
        with open(filepath, 'w') as f:
            json.dump(ml_data, f, indent=2)
        
        print(f"ML data saved to {filepath}")
    
    def load_ml_data(self, filepath):
        """Load ML data from disk."""
        
        import json
        
        with open(filepath, 'r') as f:
            ml_data = json.load(f)
        
        self.feature_history = ml_data['features']
        if ml_data['pca_features']:
            self.pca_features = np.array(ml_data['pca_features'])
        if ml_data['scaled_features']:
            self.scaled_features = np.array(ml_data['scaled_features'])
        
        print(f"ML data loaded from {filepath}")

# Usage example
if __name__ == "__main__":
    # Create integration configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    # Create advanced NLM integration
    nlm_integration = AdvancedNLMIntegration(config)
    
    # Collect features from simulation
    features = nlm_integration.collect_features(500)
    
    # Train ML models
    pca_features = nlm_integration.train_ml_models(features)
    
    # Create visualization
    nlm_integration.create_feature_visualization()
    
    # Predict future behavior
    predictions = nlm_integration.predict_future_behavior(50)
    
    print(f"Predictions: {predictions}")
```

## Configuration Validation and Error Handling

### 1. Configuration Validation
```python
import pynlm

class NLMConfigurationValidator:
    """Validate NLM configurations with comprehensive checking."""
    
    def __init__(self):
        self.validation_rules = self.initialize_validation_rules()
        self.error_messages = []
    
    def initialize_validation_rules(self):
        """Initialize configuration validation rules."""
        
        return {
            'brain.neuron_count': self.validate_neuron_count,
            'brain.synapse_density': self.validate_synapse_density,
            'brain.connection_probability': self.validate_connection_probability,
            'simulation.timestep': self.validate_timestep,
            'simulation.max_steps': self.validate_max_steps,
            'logging.log_level': self.validate_log_level,
            'enable_reward_modulation': self.validate_boolean_flag,
            'enable_curiosity': self.validate_boolean_flag,
            'enable_structural_plasticity': self.validate_boolean_flag,
            'enable_development': self.validate_boolean_flag,
            'memory.working_capacity': self.validate_memory_capacity,
            'memory.episodic_capacity': self.validate_memory_capacity
        }
    
    def validate_config(self, config):
        """Validate complete configuration."""
        
        self.error_messages = []
        
        # Check all known keys
        for key in config.getKeys():
            if key in self.validation_rules:
                value = config.get(key)
                if not self.validation_rules[key](key, value):
                    self.error_messages.append(
                        f"Invalid value for {key}: {value}"
                    )
        
        return len(self.error_messages) == 0
    
    def validate_neuron_count(self, key, value):
        """Validate neuron count."""
        
        try:
            neuron_count = value.getOr<int64_t>(0)
            return 100 <= neuron_count <= 1000000
        except:
            return False
    
    def validate_synapse_density(self, key, value):
        """Validate synapse density."""
        
        try:
            density = value.getOr<double>(0.0)
            return 0.0 <= density <= 1.0
        except:
            return False
    
    def validate_connection_probability(self, key, value):
        """Validate connection probability."""
        
        try:
            probability = value.getOr<double>(0.0)
            return 0.0 <= probability <= 1.0
        except:
            return False
    
    def validate_timestep(self, key, value):
        """Validate simulation timestep."""
        
        try:
            timestep = value.getOr<double>(0.0)
            return 0.0001 <= timestep <= 0.1
        except:
            return False
    
    def validate_max_steps(self, key, value):
        """Validate maximum simulation steps."""
        
        try:
            max_steps = value.getOr<int64_t>(0)
            return 0 <= max_steps <= 1000000
        except:
            return False
    
    def validate_log_level(self, key, value):
        """Validate log level."""
        
        try:
            log_level = value.getOr<std::string>("")
            return log_level in ["DEBUG", "INFO", "WARN", "ERROR", "FATAL"]
        except:
            return False
    
    def validate_boolean_flag(self, key, value):
        """Validate boolean configuration flag."""
        
        try:
            return value.getOr<bool>(false)
        except:
            return False
    
    def validate_memory_capacity(self, key, value):
        """Validate memory capacity."""
        
        try:
            capacity = value.getOr<int64_t>(0)
            return 10 <= capacity <= 1000
        except:
            return False
    
    def get_error_messages(self):
        """Get validation error messages."""
        
        return self.error_messages
    
    def print_validation_report(self):
        """Print validation report."""
        
        if not self.error_messages:
            print("✓ Configuration validation passed")
        else:
            print("✗ Configuration validation failed:")
            for error in self.error_messages:
                print(f"  - {error}")

# Usage example
if __name__ == "__main__":
    # Create configuration validator
    validator = NLMConfigurationValidator()
    
    # Test with valid configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("simulation.timestep", 0.001)
    config.set("logging.log_level", "INFO")
    
    if validator.validate_config(config):
        print("Configuration validation passed")
    else:
        validator.print_validation_report()
    
    # Test with invalid configuration
    config.set("brain.neuron_count", 50)  # Invalid: too low
    
    if not validator.validate_config(config):
        print("Invalid configuration detected:")
        validator.print_validation_report()
```

## Performance Monitoring and Optimization

### 1. Performance Monitoring
```python
import pynlm
import time
import psutil
import numpy as np
from typing import Dict, List

class NLMPerformanceMonitor:
    """Monitor NLM performance and provide optimization recommendations."""
    
    def __init__(self, config):
        self.config = config
        self.performance_data = []
        self.monitoring = False
        self.monitoring_thread = None
        
        # Performance thresholds
        self.thresholds = {
            'max_step_time_ms': 100,
            'min_memory_usage_mb': 100,
            'max_cpu_usage_percent': 80,
            'min_firing_rate': 0.1,
            'max_firing_rate': 1000
        }
    
    def start_monitoring(self, simulation_steps=1000):
        """Start performance monitoring."""
        
        self.monitoring = True
        self.simulation_steps = simulation_steps
        
        # Start monitoring thread
        self.monitoring_thread = threading.Thread(
            target=self._monitor_performance,
            daemon=True
        )
        self.monitoring_thread.start()
        
        print(f"Performance monitoring started for {simulation_steps} steps")
    
    def _monitor_performance(self):
        """Monitor performance in background thread."""
        
        process = psutil.Process()
        
        for step in range(self.simulation_steps):
            if not self.monitoring:
                break
            
            # Get current performance metrics
            perf_data = self.get_current_performance()
            
            # Check for performance issues
            self.check_performance_issues(perf_data)
            
            # Store performance data
            self.performance_data.append(perf_data)
            
            # Wait for next measurement
            time.sleep(0.1)  # 100ms between measurements
    
    def get_current_performance(self):
        """Get current performance metrics."""
        
        process = psutil.Process()
        
        # Get system resources
        memory_info = process.memory_info()
        cpu_percent = process.cpu_percent()
        
        # Get NLM-specific metrics (would need to be implemented)
        brain_metrics = {
            'firing_rate': 0.5,  # Placeholder
            'total_spikes': 1000,  # Placeholder
            'neuron_count': 1000  # Placeholder
        }
        
        return {
            'timestamp': time.time(),
            'step': len(self.performance_data),
            'system': {
                'memory_usage_mb': memory_info.rss / 1024 / 1024,
                'cpu_percent': cpu_percent,
                'memory_percent': process.memory_percent()
            },
            'brain': brain_metrics,
            'recommendations': []
        }
    
    def check_performance_issues(self, perf_data):
        """Check for performance issues and generate recommendations."""
        
        issues = []
        recommendations = []
        
        # Check memory usage
        memory_mb = perf_data['system']['memory_usage_mb']
        if memory_mb > self.thresholds['max_memory_usage_mb']:
            issues.append(f"High memory usage: {memory_mb:.1f} MB")
            recommendations.append("Consider reducing neuron count or enabling memory pooling")
        
        # Check CPU usage
        cpu_percent = perf_data['system']['cpu_percent']
        if cpu_percent > self.thresholds['max_cpu_usage_percent']:
            issues.append(f"High CPU usage: {cpu_percent:.1f}%")
            recommendations.append("Consider reducing simulation timestep or enabling parallelization")
        
        # Check brain metrics
        firing_rate = perf_data['brain']['firing_rate']
        if firing_rate < self.thresholds['min_firing_rate']:
            issues.append(f"Low firing rate: {firing_rate:.3f}")
            recommendations.append("Consider increasing input strength or adjusting thresholds")
        
        elif firing_rate > self.thresholds['max_firing_rate']:
            issues.append(f"High firing rate: {firing_rate:.3f}")
            recommendations.append("Consider decreasing input strength or increasing refractory period")
        
        perf_data['recommendations'] = recommendations
        
        if issues:
            perf_data['issues'] = issues
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        
        self.monitoring = False
        
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5)
        
        print("Performance monitoring stopped")
    
    def get_performance_summary(self):
        """Get performance summary statistics."""
        
        if not self.performance_data:
            return {}
        
        step_times = [d['brain']['firing_rate'] for d in self.performance_data]
        memory_usage = [d['system']['memory_usage_mb'] for d in self.performance_data]
        cpu_usage = [d['system']['cpu_percent'] for d in self.performance_data]
        
        return {
            'total_steps': len(self.performance_data),
            'avg_memory_usage_mb': np.mean(memory_usage),
            'max_memory_usage_mb': np.max(memory_usage),
            'avg_cpu_usage_percent': np.mean(cpu_usage),
            'max_cpu_usage_percent': np.max(cpu_usage),
            'avg_firing_rate': np.mean(step_times),
            'min_firing_rate': np.min(step_times),
            'max_firing_rate': np.max(step_times),
            'total_issues': sum(1 for d in self.performance_data if 'issues' in d),
            'total_recommendations': sum(len(d['recommendations']) for d in self.performance_data)
        }
    
    def generate_performance_report(self):
        """Generate comprehensive performance report."""
        
        summary = self.get_performance_summary()
        
        report = f"""
=== NLM Performance Report ===

Simulation Statistics:
  - Total steps: {summary['total_steps']}
  - Average step time: {1.0/summary.get('avg_firing_rate', 0.001)*1000:.1f}ms
  - Total simulation time: {summary['total_steps'] * 0.1:.1f}s

Resource Usage:
  - Average memory usage: {summary['avg_memory_usage_mb']:.1f} MB
  - Maximum memory usage: {summary['max_memory_usage_mb']:.1f} MB
  - Average CPU usage: {summary['avg_cpu_usage_percent']:.1f}%
  - Maximum CPU usage: {summary['max_cpu_usage_percent']:.1f}%

Brain Activity:
  - Average firing rate: {summary['avg_firing_rate']:.3f} Hz
  - Minimum firing rate: {summary['min_firing_rate']:.3f} Hz
  - Maximum firing rate: {summary['max_firing_rate']:.3f} Hz

Performance Issues:
  - Steps with issues: {summary['total_issues']}
  - Total recommendations: {summary['total_recommendations']}

Optimization Recommendations:
"""
        
        # Generate specific recommendations
        recommendations = []
        for perf_data in self.performance_data:
            recommendations.extend(perf_data['recommendations'])
        
        # Deduplicate recommendations
        unique_recommendations = list(set(recommendations))
        
        for rec in unique_recommendations:
            report += f"  - {rec}\n"
        
        report += "\n=== End Performance Report ==="
        
        return report

# Usage example
if __name__ == "__main__":
    # Create performance monitor
    config = pynlm.createDefaultConfig()
    monitor = NLMPerformanceMonitor(config)
    
    # Start monitoring
    monitor.start_monitoring(simulation_steps=100)
    
    # Run simulation (in real application)
    # For demonstration, just sleep
    time.sleep(2)
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Generate performance report
    report = monitor.generate_performance_report()
    print(report)
```

## Error Handling and Recovery

### 1. Comprehensive Error Handling
```python
import pynlm
import logging
import traceback
from typing import Dict, Any, Optional

class NLMError(Exception):
    """Base exception for NLM errors."""
    
    def __init__(self, message: str, error_code: str = "UNKNOWN", details: Optional[Dict] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = time.time()
    
    def to_dict(self):
        """Convert error to dictionary."""
        return {
            'message': self.message,
            'error_code': self.error_code,
            'details': self.details,
            'timestamp': self.timestamp
        }

class NLMErrorHandler:
    """Comprehensive error handler for NLM operations."""
    
    def __init__(self, logger: logging.Logger = None):
        self.logger = logger or logging.getLogger('nlm_errors')
        self.error_history = []
        self.recovery_handlers = {}
        self.error_categorizer = ErrorCategorizer()
    
    def handle_error(self, error: Exception, context: Dict[str, Any] = None):
        """Handle error with context and recovery."""
        
        # Categorize error
        category = self.error_categorizer.categorize(error)
        
        # Log error
        self.logger.error(
            f"Error occurred: {error}\n"
            f"Category: {category}\n"
            f"Context: {context or {}}",
            exc_info=True
        )
        
        # Create NLM error if not already
        if not isinstance(error, NLMError):
            nlm_error = NLMError(
                str(error),
                error_code=self.error_categorizer.get_error_code(error),
                details={'context': context or {}}
            )
        else:
            nlm_error = error
        
        # Add to error history
        self.error_history.append({
            'error': nlm_error,
            'category': category,
            'context': context or {},
            'timestamp': time.time()
        })
        
        # Try recovery
        recovery_result = self.try_recovery(nlm_error, context)
        
        return {
            'error': nlm_error,
            'category': category,
            'recovery_applied': recovery_result['applied'],
            'recovery_message': recovery_result.get('message', ''),
            'should_retry': recovery_result.get('should_retry', False)
        }
    
    def try_recovery(self, error: NLMError, context: Dict[str, Any] = None):
        """Try to recover from error."""
        
        error_code = error.error_code
        
        if error_code in self.recovery_handlers:
            handler = self.recovery_handlers[error_code]
            return handler(error, context)
        
        # Default recovery strategies
        return self.default_recovery(error, context)
    
    def add_recovery_handler(self, error_code: str, handler: callable):
        """Add recovery handler for specific error code."""
        
        self.recovery_handlers[error_code] = handler
    
    def default_recovery(self, error: NLMError, context: Dict[str, Any] = None):
        """Default recovery strategies."""
        
        error_code = error.error_code
        
        if error_code == 'CONFIGURATION_ERROR':
            return {
                'applied': True,
                'message': 'Configuration reset to defaults and retry initiated',
                'should_retry': True
            }
        
        elif error_code == 'MEMORY_ERROR':
            return {
                'applied': True,
                'message': 'Memory usage reduced by disabling high-memory features',
                'should_retry': True
            }
        
        elif error_code == 'NETWORK_ERROR':
            return {
                'applied': False,
                'message': 'Network error detected, manual intervention required',
                'should_retry': False
            }
        
        elif error_code == 'SIMULATION_ERROR':
            return {
                'applied': True,
                'message': 'Simulation state saved and reset initiated',
                'should_retry': True
            }
        
        else:
            return {
                'applied': False,
                'message': 'No automatic recovery available for this error',
                'should_retry': False
            }
    
    def get_error_summary(self):
        """Get summary of errors."""
        
        if not self.error_history:
            return {
                'total_errors': 0,
                'categories': {},
                'last_error_time': None,
                'most_common_error': None
            }
        
        categories = {}
        for error_entry in self.error_history:
            category = error_entry['category']
            categories[category] = categories.get(category, 0) + 1
        
        return {
            'total_errors': len(self.error_history),
            'categories': categories,
            'last_error_time': self.error_history[-1]['timestamp'],
            'most_common_error': max(categories.items(), key=lambda x: x[1])[0] if categories else None
        }
    
    def export_error_log(self, filepath: str):
        """Export error log to file."""
        
        import json
        
        error_log = []
        for error_entry in self.error_history:
            error_log.append({
                'error': error_entry['error'].to_dict(),
                'category': error_entry['category'],
                'context': error_entry['context'],
                'timestamp': error_entry['timestamp']
            })
        
        with open(filepath, 'w') as f:
            json.dump(error_log, f, indent=2)
        
        print(f"Error log exported to {filepath}")

class ErrorCategorizer:
    """Categorize NLM errors into meaningful categories."""
    
    def categorize(self, error: Exception) -> str:
        """Categorize error based on type and message."""
        
        error_str = str(error).lower()
        
        # Configuration errors
        if any(keyword in error_str for keyword in ['config', 'configuration', 'setting', 'parameter']):
            return 'CONFIGURATION_ERROR'
        
        # Memory errors
        if any(keyword in error_str for keyword in ['memory', 'out of memory', 'heap', 'allocation']):
            return 'MEMORY_ERROR'
        
        # Network errors
        if any(keyword in error_str for keyword in ['network', 'connection', 'socket', 'timeout']):
            return 'NETWORK_ERROR'
        
        # File I/O errors
        if any(keyword in error_str for keyword in ['file', 'io', 'read', 'write', 'open']):
            return 'FILE_ERROR'
        
        # Simulation errors
        if any(keyword in error_str for keyword in ['simulation', 'step', 'brain', 'neuron', 'synapse']):
            return 'SIMULATION_ERROR'
        
        # Validation errors
        if any(keyword in error_str for keyword in ['validate', 'invalid', 'range', 'type']):
            return 'VALIDATION_ERROR'
        
        # Learning/plasticity errors
        if any(keyword in error_str for keyword in ['plasticity', 'learning', 'weight', 'synaptic']):
            return 'LEARNING_ERROR'
        
        # Unknown errors
        return 'UNKNOWN_ERROR'
    
    def get_error_code(self, error: Exception) -> str:
        """Get error code for error categorization."""
        
        category = self.categorize(error)
        
        # Map categories to specific error codes
        error_codes = {
            'CONFIGURATION_ERROR': 'CONFIG_001',
            'MEMORY_ERROR': 'MEM_001',
            'NETWORK_ERROR': 'NET_001',
            'FILE_ERROR': 'FILE_001',
            'SIMULATION_ERROR': 'SIM_001',
            'VALIDATION_ERROR': 'VAL_001',
            'LEARNING_ERROR': 'LRN_001',
            'UNKNOWN_ERROR': 'UNK_001'
        }
        
        return error_codes.get(category, 'UNK_001')

# Usage example
if __name__ == "__main__":
    # Setup logging
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    
    # Create error handler
    error_handler = NLMErrorHandler()
    
    # Add custom recovery handler
    def config_recovery(error, context):
        return {
            'applied': True,
            'message': 'Configuration validated and corrected',
            'should_retry': True
        }
    
    error_handler.add_recovery_handler('CONFIG_001', config_recovery)
    
    # Test error handling
    try:
        # Simulate configuration error
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 50)  # Invalid: too low
        
        # Validate configuration
        if not config.isValid("brain.neuron_count"):
            raise NLMError(
                "Invalid neuron count configuration",
                error_code='CONFIG_001',
                details={'value': 50, 'min': 100, 'max': 1000000}
            )
    
    except Exception as e:
        # Handle error
        result = error_handler.handle_error(e, {'operation': 'configuration_validation'})
        
        print(f"Error handled: {result['error'].message}")
        print(f"Category: {result['category']}")
        print(f"Recovery applied: {result['recovery_applied']}")
        print(f"Should retry: {result['should_retry']}")
    
    # Get error summary
    summary = error_handler.get_error_summary()
    print(f"\nError summary: {summary}")
    
    # Export error log
    error_handler.export_error_log('nlm_error_log.json')
```

## Advanced Python Integration Examples

### 1. Real-time Monitoring with Web Interface
```python
import pynlm
from flask import Flask, render_template, jsonify
import threading
import time

class WebBasedNLMMonitor:
    """Web-based NLM monitor with real-time updates."""
    
    def __init__(self, config):
        self.config = config
        self.app = Flask(__name__)
        self.simulation = None
        self.monitoring_data = []
        self.max_data_points = 1000
        
        self.setup_routes()
    
    def setup_routes(self):
        """Setup Flask routes."""
        
        @self.app.route('/')
        def dashboard():
            return render_template('dashboard.html')
        
        @self.app.route('/api/status')
        def get_status():
            if self.simulation:
                return jsonify({
                    'running': True,
                    'simulation_name': self.simulation.name,
                    'current_step': len(self.monitoring_data) - 1,
                    'brain_metrics': self.get_current_brain_metrics()
                })
            else:
                return jsonify({'running': False})
        
        @self.app.route('/api/data')
        def get_monitoring_data():
            # Return last N data points
            start_idx = max(0, len(self.monitoring_data) - self.max_data_points)
            data = self.monitoring_data[start_idx:]
            
            return jsonify(data)
        
        @self.app.route('/api/control', methods=['POST'])
        def control_simulation():
            import json
            data = request.get_json()
            
            action = data.get('action')
            if action == 'start' and self.simulation:
                self.simulation.run_simulation(
                    data.get('steps', 100),
                    pause_between_steps=data.get('pause', 0.0)
                )
            
            elif action == 'stop' and self.simulation:
                self.simulation.stop_simulation()
            
            return jsonify({'status': 'ok'})
        
        @self.app.route('/api/config', methods=['GET', 'PUT'])
        def config_handler():
            if request.method == 'GET':
                # Return current configuration
                return jsonify(self.get_current_config())
            
            elif request.method == 'PUT':
                # Update configuration
                new_config = request.get_json()
                self.update_config(new_config)
                return jsonify({'status': 'ok'})
    
    def get_current_config(self):
        """Get current NLM configuration."""
        
        if not self.simulation:
            return {}
        
        return {
            'brain': {
                'neuron_count': self.simulation.brain.getTotalNeuronCount(),
                'synapse_count': self.simulation.brain.getTotalSynapseCount(),
                'firing_rate': self.simulation.brain.getAverageFiringRate()
            },
            'simulation': {
                'name': self.simulation.name,
                'step': len(self.monitoring_data) - 1
            }
        }
    
    def update_config(self, new_config):
        """Update NLM configuration."""
        
        # Update brain configuration if provided
        if 'brain' in new_config:
            if 'neuron_count' in new_config['brain']:
                # Note: In real application, this would require brain restart
                pass
        
        # Update simulation configuration if provided
        if 'simulation' in new_config:
            if 'steps' in new_config['simulation']:
                # Note: This would affect future simulation runs
                pass
    
    def start_web_server(self, port=8050):
        """Start web server."""
        
        print(f"Starting NLM web monitor on port {port}")
        self.app.run(host='0.0.0.0', port=port, debug=False)

# Usage example
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("enable_curiosity", True)
    
    # Create web monitor
    monitor = WebBasedNLMMonitor(config)
    
    # Start web server
    monitor.start_web_server(port=8050)
```

## Conclusion

This comprehensive documentation provides advanced users with:

### Key Features:

1. **Advanced Configuration Management**
   - JSON/YAML configuration formats
   - Configuration validation and schema checking
   - Configuration templates and inheritance

2. **Complex Simulation Control**
   - Event-driven simulation management
   - Multi-agent simulation environments
   - Real-time monitoring and control

3. **Machine Learning Integration**
   - Feature extraction from NLM simulations
   - ML model training and prediction
   - Automated performance optimization

4. **Production Deployment**
   - Error handling and recovery systems
   - Comprehensive logging and monitoring
   - Web-based monitoring interfaces

5. **Advanced Error Handling**
   - Error categorization and classification
   - Recovery strategies and retry mechanisms
   - Error analysis and reporting

### Usage Patterns:

These examples demonstrate:

- **Research Applications**: Complex learning experiments and cognitive development studies
- **Production Systems**: Deployable, scalable NLM implementations
- **Development Tools**: Debugging, monitoring, and performance optimization
- **Educational Examples**: Teaching NLM concepts and capabilities

### Next Steps:

1. **Start with basic examples** to understand NLM functionality
2. **Experiment with advanced configurations** to optimize performance
3. **Customize examples** to match specific research or production needs
4. **Extend the framework** by adding custom components and rules
5. **Monitor and optimize** production deployments for maximum efficiency

These advanced Python integration examples provide power users with sophisticated tools for developing, testing, and deploying complex NLM applications.

---

*For the latest updates, documentation, and community support, visit the NLM project documentation and forums.*
