# NLM (Neural Learning Machine) - Advanced HOW TO USE

## Overview

This document covers advanced usage patterns, configuration options, and detailed implementation techniques for the NLM Python API.

---

## Part 1: Advanced Configuration Management

### Configuration Classes

NLM provides multiple configuration approaches:

#### 1. Basic Configuration (`Config`)
```python
import pynlm

# Basic configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 2)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

#### 2. Enhanced Configuration (`EnhancedConfig`)
```python
import pynlm

# Enhanced configuration with chaining
config = (pynlm.createDefaultConfig()
         .setValue("brain.name", "My Brain")
         .setInt("brain.neuron_count", 500)
         .setFloat("brain.simulation_timestep", 0.01)
         .setInt("brain.region_count", 1)
         .setFloat("brain.connection_probability", 0.1))

print(config.summary())
```

#### 3. Context Manager Configuration
```python
import pynlm

# Automatic resource management
with pynlm.EnhancedConfig() as config:
    config.configure({
        "brain.neuron_count": 200,
        "brain.region_count": 2,
        "plasticity.stdp.enable": True
    })
    
    brain = pynlm.createBrain(config)
    brain.initialize()
```

### Configuration Validation

#### Basic Validation
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("simulation_timestep", 0.001)

# Validate configuration
errors = pynlm.validateConfig(config)
if errors:
    print(f"Configuration errors: {errors}")
else:
    print("Configuration is valid!")
```

#### Detailed Validation with Context
```python
import pynlm

def createValidatedConfig(settings):
    """Create configuration with comprehensive validation"""
    config = pynlm.createDefaultConfig()
    
    # Apply settings
    if settings:
        config.mergeFrom(settings)
    
    # Get detailed validation errors
    errors = pynlm.validateConfig(config)
    
    if errors:
        raise ValueError(f"Invalid configuration: {errors}")
    
    return config

# Usage
config = createValidatedConfig({
    "neuron_count": 500,
    "region_count": 3,
    "connection_probability": 0.05
})
```

### Configuration Presets

```python
import pynlm

# Configuration manager for presets
class ConfigManager:
    def __init__(self, name):
        self.name = name
        self.config = pynlm.createDefaultConfig()
    
    def apply_preset(self, preset_name):
        presets = {
            "default": {"neuron_count": 100, "region_count": 1, "connection_probability": 0.1},
            "high_performance": {"neuron_count": 1000, "region_count": 4, "connection_probability": 0.05},
            "quick_test": {"neuron_count": 10, "region_count": 1, "connection_probability": 0.3},
            "research": {"neuron_count": 5000, "region_count": 8, "connection_probability": 0.02}
        }
        
        if preset_name in presets:
            self.config.mergeFrom(presets[preset_name])
            return True
        return False
    
    def save(self, filename):
        self.config.saveToFile(filename)
    
    def load(self, filename):
        self.config.loadFromFile(filename)
    
    def get_config(self):
        return self.config

# Example usage
manager = ConfigManager("research")
manager.apply_preset("research")
manager.save("research_config.json")
```

---

## Part 2: Advanced Brain Operations

### Brain State Management

#### Brain Validation and Diagnostics
```python
import pynlm

def validateBrain(brain):
    """Comprehensive brain validation"""
    # Basic validation
    is_valid = brain.isValid()
    
    if not is_valid:
        errors = brain.validateState()
        print(f"Brain validation failed: {errors}")
        return False
    
    # Statistical validation
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    firing_neurons = brain.getFiringNeuronCount()
    avg_firing_rate = brain.getAverageFiringRate()
    
    # Check for reasonable values
    if total_neurons < 10:
        print("Warning: Very few neurons (< 10)")
    
    if avg_firing_rate < 0.1:
        print("Warning: Low average firing rate")
    elif avg_firing_rate > 100:
        print("Warning: High average firing rate")
    
    # Development stage check
    dev_stage = brain.getDevelopmentalStage()
    print(f"Development stage: {dev_stage}")
    
    return True

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

if validateBrain(brain):
    print("Brain validation passed!")
```

#### Safe Brain Operations
```python
import pynlm

class SafeBrainSimulator:
    def __init__(self, config):
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.errors = []
    
    def safe_step(self, step, time=0.0):
        """Safely execute a brain step"""
        errors = pynlm.safeBrainStep(self.brain, step, time)
        self.errors.extend(errors)
        return len(errors) == 0
    
    def simulate(self, steps, timestep=0.1):
        """Simulate multiple steps safely"""
        for step in range(steps):
            self.safe_step(step, step * timestep)
        
        return {
            "steps_completed": steps,
            "total_errors": len(self.errors),
            "errors": self.errors
        }

# Usage
config = (pynlm.createDefaultConfig()
         .setInt("neuron_count", 100)
         .setInt("region_count", 2))

simulator = SafeBrainSimulator(config)
result = simulator.simulate(100)
print(f"Simulation completed with {result['total_errors']} errors")
```

### Advanced Brain Features

#### Neuromodulation Control
```python
import pynlm

# Create agent brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)

# Enable specific neuromodulation features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)

# Check feature status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"Novelty enabled: {agent.isNoveltyEnabled()}")

# Get neuromodulation levels
print(f"Current neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
```

#### Development Management
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)

# Simulate development over time
for step in range(100):
    world.update(0.1)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Check development stage
    dev_stage = agent.getDevelopmentalStage()
    print(f"Step {step}: Development stage = {dev_stage}")
    
    # Development affects plasticity rates
    if dev_stage == pynlm.DevelopmentalStage.CriticalPeriod:
        print("Critical period - enhanced plasticity")
    elif dev_stage == pynlm.DevelopmentalStage.Maturation:
        print("Maturation - reduced plasticity")
```

---

## Part 3: Advanced World Operations

### World Configuration and Validation

#### Create Validated World
```python
import pynlm

class WorldManager:
    def __init__(self, width=20, height=20, vision_width=8, vision_height=8):
        self.world = pynlm.createValidatedWorld(width, height, vision_width, vision_height)
        self.agent = None
    
    def set_agent(self, agent):
        self.agent = agent
        self.agent.initialize(self.world)
    
    def run_episode(self, steps=100, timestep=0.1):
        """Run episode with comprehensive error handling"""
        all_errors = []
        
        for step in range(steps):
            # Safe world update
            errors = pynlm.safeWorldUpdate(self.world, timestep)
            all_errors.extend(errors)
            
            # Agent processing
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain operations
            self.agent.getBrain().step(step)
            
            # Action processing
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Neuromodulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
        
        return {
            "steps_completed": steps,
            "total_errors": len(all_errors),
            "errors": all_errors,
            "final_time": self.world.getSimulationTime()
        }

# Usage
world_manager = WorldManager(30, 30, 10, 10)

# Create agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world_manager.set_agent(agent)

# Enable features
agent.enableAllFeatures(True)

# Run simulation
result = world_manager.run_episode(200, 0.05)
print(f"Episode completed: {result['steps_completed']} steps, {result['total_errors']} errors")
```

### Advanced World Features

#### World Object Management
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

# Add various world objects
resources = [
    pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0),
    pynlm.WorldObject(15.0, 10.0, pynlm.WorldObjectType.Resource, value=5.0),
    pynlm.WorldObject(10.0, 15.0, pynlm.WorldObjectType.Hazard, value=2.0),
    pynlm.WorldObject(2.0, 2.0, pynlm.WorldObjectType.Wall, value=1.0)
]

for obj in resources:
    world.addObject(obj)

# Test world properties
print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Agent position: {world.getAgentBody().x}, {world.getAgentBody().y}")
print(f"Max energy: {world.getMaxEnergy()}")

# Check object interactions
test_pos = (world.getAgentBody().x + 1.0, world.getAgentBody().y)
if world.isValidPosition(test_pos[0], test_pos[1]):
    print(f"Position {test_pos} is valid")
else:
    print(f"Position {test_pos} is invalid (collision)")
```

---

## Part 4: Performance Optimization

### Batch Processing
```python
import pynlm
import time
import statistics

class BatchProcessor:
    def __init__(self, config_template):
        self.config_template = config_template
    
    def create_configs(self, num_configs, variations):
        """Create multiple configurations with variations"""
        configs = []
        
        for i in range(num_configs):
            config = self.config_template.copy()
            
            # Apply variations
            for key, variation_range in variations.items():
                original_value = config.getOr(key, 0)
                variation = (i % variation_range) + 1
                config.set(key, original_value + variation, pynlm.ConfigSource::User)
            
            configs.append(config)
        
        return configs
    
    def create_brains(self, configs):
        """Create multiple brains from configurations"""
        brains = []
        
        for i, config in enumerate(configs):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
            
            if (i + 1) % 10 == 0:
                print(f"Created {i + 1}/{len(configs)} brains")
        
        return brains
    
    def benchmark_performance(self, num_configs=100, num_brains=50):
        """Benchmark performance of batch operations"""
        print(f"Benchmarking: {num_configs} configs, {num_brains} brains")
        
        # Time config creation
        start_time = time.time()
        configs = self.create_configs(num_configs, {
            "neuron_count": 10,
            "region_count": 2,
            "simulation_timestep": 5
        })
        config_time = time.time() - start_time
        
        # Time brain creation
        start_time = time.time()
        brains = self.create_brains(configs[:num_brains])
        brain_time = time.time() - start_time
        
        # Calculate statistics
        neuron_counts = [brain.getTotalNeuronCount() for brain in brains]
        avg_neurons = statistics.mean(neuron_counts)
        std_neurons = statistics.stdev(neuron_counts) if len(neurons) > 1 else 0
        
        print(f"\nPerformance Results:")
        print(f"  - Config creation: {config_time:.3f}s ({num_configs/config_time:.1f} configs/s)")
        print(f"  - Brain creation: {brain_time:.3f}s ({num_brains/brain_time:.1f} brains/s)")
        print(f"  - Average neurons per brain: {avg_neurons:.1f} ± {std_neurons:.1f}")
        
        return {
            "config_time": config_time,
            "brain_time": brain_time,
            "avg_neurons": avg_neurons,
            "std_neurons": std_neurons,
            "configs_per_sec": num_configs / config_time,
            "brains_per_sec": num_brains / brain_time
        }

# Usage
config_template = (pynlm.createDefaultConfig()
                 .setInt("neuron_count", 100)
                 .setInt("region_count", 2))

processor = BatchProcessor(config_template)
results = processor.benchmark_performance(200, 100)
```

### Memory Management
```python
import pynlm

class MemoryEfficientSimulator:
    def __init__(self, max_brains=10):
        self.brains = []
        self.max_brains = max_brains
    
    def add_brain(self, config):
        """Add a brain, removing oldest if at capacity"""
        if len(self.brains) >= self.max_brains:
            self.brains.pop(0)  # Remove oldest
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        self.brains.append(brain)
        return len(self.brains) - 1
    
    def run_simulation_batch(self, steps=50):
        """Run simulation for all stored brains"""
        results = []
        
        for i, brain in enumerate(self.brains):
            brain_errors = []
            
            for step in range(steps):
                errors = pynlm.safeBrainStep(brain, step)
                brain_errors.extend(errors)
            
            results.append({
                "brain_index": i,
                "steps_completed": steps,
                "total_errors": len(brain_errors),
                "final_neurons": brain.getTotalNeuronCount()
            })
        
        return results
    
    def get_memory_stats(self):
        """Get memory usage statistics"""
        total_neurons = sum(brain.getTotalNeuronCount() for brain in self.brains)
        avg_neurons = total_neurons / len(self.brains) if self.brains else 0
        
        return {
            "brains_stored": len(self.brains),
            "total_neurons": total_neurons,
            "average_neurons_per_brain": avg_neurons,
            "max_brains": self.max_brains
        }

# Usage with memory management
memory_sim = MemoryEfficientSimulator(max_brains=5)

for i in range(10):
    config = (pynlm.createDefaultConfig()
             .setInt("neuron_count", 50 + i * 10)
             .setInt("region_count", 1 + (i % 3)))
    memory_sim.add_brain(config)

results = memory_sim.run_simulation_batch(30)
for result in results:
    print(f"Brain {result['brain_index']}: {result['steps_completed']} steps, "
          f"{result['total_errors']} errors")

stats = memory_sim.get_memory_stats()
print(f"\nMemory stats: {stats['brains_stored']} brains, "
      f"{stats['total_neurons']} total neurons")
```

---

## Part 5: Advanced Error Handling and Recovery

### Comprehensive Error Handling
```python
import pynlm

class ErrorHandlingSimulator:
    def __init__(self):
        self.error_log = []
        self.recovery_actions = []
    
    def safe_operation(self, operation_name, operation_func):
        """Execute operation safely and log errors"""
        try:
            result = operation_func()
            self.error_log.append({
                "operation": operation_name,
                "status": "success",
                "timestamp": time.time()
            })
            return result, True
        except Exception as e:
            self.error_log.append({
                "operation": operation_name,
                "status": "error",
                "error": str(e),
                "timestamp": time.time()
            })
            return None, False
    
    def recover_from_error(self, error_type, context):
        """Implement recovery actions"""
        recovery_strategies = {
            "brain_initialization": self._recover_brain_init,
            "world_update": self._recover_world_update,
            "action_application": self._recover_action_application
        }
        
        if error_type in recovery_strategies:
            result = recovery_strategies[error_type](context)
            self.recovery_actions.append({
                "error_type": error_type,
                "action": "recovery_attempted",
                "result": result
            })
            return result
        
        return False
    
    def _recover_brain_init(self, context):
        """Recover from brain initialization error"""
        try:
            # Try with default configuration
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            return True
        except:
            return False
    
    def _recover_world_update(self, context):
        """Recover from world update error"""
        try:
            # Reset world to safe state
            context['world'].reset()
            return True
        except:
            return False
    
    def _recover_action_application(self, context):
        """Recover from action application error"""
        try:
            # Apply default safe action
            default_action = pynlm.Action(pynlm.ActionType.Wait)
            context['world'].applyMotorCommand(default_action, context['time'])
            return True
        except:
            return False
    
    def run_safe_simulation(self, config, world, agent, steps=100):
        """Run simulation with comprehensive error handling"""
        self.error_log.clear()
        self.recovery_actions.clear()
        
        success_count = 0
        error_count = 0
        
        for step in range(steps):
            # Try safe brain step
            result, success = self.safe_operation(
                f"brain_step_{step}",
                lambda: pynlm.safeBrainStep(agent.getBrain(), step)
            )
            
            if success:
                success_count += 1
            else:
                error_count += 1
                self.recover_from_error("brain_initialization", {"brain": agent.getBrain()})
            
            # Try world update
            result, success = self.safe_operation(
                f"world_update_{step}",
                lambda: pynlm.safeWorldUpdate(world, 0.1)
            )
            
            if success:
                success_count += 1
            else:
                error_count += 1
                self.recover_from_error("world_update", {"world": world})
            
            # Process agent actions if steps completed successfully
            if success and step > 0:
                # Agent processing
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Action processing
                action = agent.decodeMotorCommand()
                result, success = self.safe_operation(
                    f"action_application_{step}",
                    lambda: world.applyMotorCommand(action, world.getSimulationTime())
                )
                
                if success:
                    success_count += 1
                else:
                    error_count += 1
                    self.recover_from_error("action_application", {
                        "world": world,
                        "action": action,
                        "time": world.getSimulationTime()
                    })
        
        return {
            "steps_completed": steps,
            "successful_operations": success_count,
            "error_operations": error_count,
            "error_log": self.error_log,
            "recovery_actions": self.recovery_actions
        }

# Usage
error_handler = ErrorHandlingSimulator()

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)

result = error_handler.run_safe_simulation(brain, world, agent, 50)
print(f"Simulation completed with {result['successful_operations']} successful operations, "
      f"{result['error_operations']} errors")
```

---

## Part 6: Integration with External Systems

### Configuration File Integration
```python
import pynlm
import json

class ConfigurationManager:
    def __init__(self, config_file="nlm_config.json"):
        self.config_file = config_file
        self.config_cache = {}
    
    def export_config(self, config, name):
        """Export configuration to cache"""
        # Convert config to dictionary
        config_dict = {}
        keys = config.getKeys()
        for key in keys:
            if config.has(key):
                # Get value as appropriate type
                if config.has(key):
                    config_dict[key] = config.getOr(key, None)
        
        self.config_cache[name] = config_dict
        self._save_to_file()
        return True
    
    def import_config(self, name):
        """Import configuration from cache"""
        if name not in self.config_cache:
            raise ValueError(f"Configuration '{name}' not found")
        
        config = pynlm.createDefaultConfig()
        
        for key, value in self.config_cache[name].items():
            if isinstance(value, int):
                config.set(key, value, pynlm.ConfigSource::User)
            elif isinstance(value, float):
                config.set(key, value, pynlm.ConfigSource::User)
            elif isinstance(value, str):
                config.set(key, value, pynlm.ConfigSource::User)
        
        return config
    
    def export_to_json(self, config, filename=None):
        """Export configuration to JSON file"""
        if filename is None:
            filename = self.config_file
        
        config_dict = {}
        keys = config.getKeys()
        for key in keys:
            if config.has(key):
                config_dict[key] = config.getOr(key, None)
        
        with open(filename, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        return True
    
    def import_from_json(self, filename=None):
        """Import configuration from JSON file"""
        if filename is None:
            filename = self.config_file
        
        with open(filename, 'r') as f:
            config_dict = json.load(f)
        
        config = pynlm.createDefaultConfig()
        
        for key, value in config_dict.items():
            config.set(key, value, pynlm.ConfigSource::User)
        
        return config
    
    def _save_to_file(self):
        """Save entire cache to file"""
        with open(self.config_file, 'w') as f:
            json.dump(self.config_cache, f, indent=2)

# Usage
config_manager = ConfigurationManager("my_experiments.json")

# Create and export configuration
config = (pynlm.createDefaultConfig()
         .setInt("neuron_count", 1000)
         .setInt("region_count", 4))

config_manager.export_config(config, "high_performance")
config_manager.export_to_json(config)

# Import configuration
imported_config = config_manager.import_config("high_performance")
print(f"Imported config neurons: {imported_config.getOr('neuron_count', 0)}")
```

### Data Persistence
```python
import pynlm
import pickle
import os

class PersistentSimulator:
    def __init__(self, save_directory="nlm_saves"):
        self.save_directory = save_directory
        if not os.path.exists(save_directory):
            os.makedirs(save_directory)
    
    def save_simulation(self, brain, world, agent, filename, metadata=None):
        """Save complete simulation state"""
        save_data = {
            "brain": brain,
            "world": world,
            "agent": agent,
            "metadata": metadata or {},
            "timestamp": time.time()
        }
        
        filepath = os.path.join(self.save_directory, f"{filename}.pkl")
        
        with open(filepath, 'wb') as f:
            pickle.dump(save_data, f)
        
        # Also save brain state separately
        brain_filepath = os.path.join(self.save_directory, f"{filename}_brain.bin")
        brain.save(brain_filepath)
        
        return True
    
    def load_simulation(self, filename):
        """Load complete simulation state"""
        filepath = os.path.join(self.save_directory, f"{filename}.pkl")
        
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Save file '{filename}' not found")
        
        with open(filepath, 'rb') as f:
            save_data = pickle.load(f)
        
        # Note: For large simulations, you might want to load
        # only the brain state and recreate other components
        return save_data
    
    def save_checkpoint(self, brain, step, filename=None):
        """Save simulation checkpoint"""
        if filename is None:
            filename = f"checkpoint_{step}"
        
        brain.save(os.path.join(self.save_directory, f"{filename}.bin"))
        
        # Create checkpoint metadata
        metadata = {
            "step": step,
            "neuron_count": brain.getTotalNeuronCount(),
            "region_count": brain.getRegionCount(),
            "total_spikes": brain.getTotalSpikeCount()
        }
        
        return self.save_simulation(brain, None, None, filename, metadata)
    
    def load_checkpoint(self, filename):
        """Load simulation checkpoint"""
        if filename.endswith(".bin"):
            # Load brain state from checkpoint
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(os.path.join(self.save_directory, filename))
            return brain
        else:
            return self.load_simulation(filename)

# Usage
persistent_sim = PersistentSimulator("my_simulations")

# Run simulation then save
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate some steps
for step in range(100):
    brain.step(step)

# Save simulation
persistent_sim.save_simulation(brain, None, None, "experiment_1")
persistent_sim.save_checkpoint(brain, 100)

# Load simulation later
loaded_brain = persistent_sim.load_checkpoint("checkpoint_100")
print(f"Loaded brain with {loaded_brain.getTotalNeuronCount()} neurons")
```

---

## Part 7: Advanced Agent Programming

### Custom Agent Behaviors
```python
import pynlm

class CustomAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        
        # Custom parameters
        self.exploration_rate = 0.1
        self.exploitation_rate = 0.9
        self.reward_history = []
    
    def epsilon_greedy_action(self, available_actions):
        """Choose action using epsilon-greedy strategy"""
        import random
        
        if random.random() < self.exploration_rate:
            # Explore: choose random action
            return random.choice(available_actions)
        else:
            # Exploit: use agent's decision
            return self.agent.decodeMotorCommand()
    
    def adaptive_curiosity(self, percept):
        """Adaptive curiosity-driven exploration"""
        novelty = percept.getInternal()[0] if percept.getInternal() else 0.0
        
        # Adjust curiosity based on novelty
        if novelty > 0.8:
            self.exploration_rate = min(0.3, self.exploration_rate + 0.1)
            self.exploitation_rate = max(0.5, self.exploitation_rate - 0.1)
        elif novelty < 0.2:
            self.exploration_rate = max(0.05, self.exploration_rate - 0.1)
            self.exploitation_rate = min(0.95, self.exploitation_rate + 0.1)
        
        return novelty
    
    def run_custom_episode(self, max_steps=1000):
        """Run episode with custom agent behavior"""
        episode_reward = 0.0
        episode_steps = 0
        
        for step in range(max_steps):
            # Get current state
            percept = self.world.getSensoryPercept()
            
            # Adaptive curiosity
            novelty = self.adaptive_curiosity(percept)
            
            # Agent processing
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Custom action selection
            action = self.epsilon_greedy_action(None)
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            episode_reward += reward
            self.reward_history.append(reward)
            
            episode_steps += 1
            
            # Decay exploration rate
            self.exploration_rate *= 0.99
            self.exploitation_rate = 1.0 - self.exploration_rate
            
            # End episode if task completed
            if self._task_completed():
                break
        
        return {
            "total_reward": episode_reward,
            "steps_completed": episode_steps,
            "average_reward": episode_reward / episode_steps if episode_steps > 0 else 0,
            "novelty_score": statistics.mean([abs(r) for r in self.reward_history]) if self.reward_history else 0
        }
    
    def _task_completed(self):
        """Check if task is completed (custom implementation)"""
        # Example: task completion based on brain state
        return (self.brain.getTotalSpikeCount() > 1000 and 
                self.brain.getFiringNeuronCount() > 10)

# Usage
custom_agent = CustomAgent(
    pynlm.createBrain(pynlm.createDefaultConfig()),
    pynlm.createSimpleWorld()
)

custom_agent.world.configure(30, 30, 10, 10)
custom_agent.world.reset()

# Enable features
custom_agent.agent.enableAllFeatures(True)

# Run custom episode
results = custom_agent.run_custom_episode(max_steps=500)
print(f"Custom episode results:")
print(f"  - Total reward: {results['total_reward']:.2f}")
print(f"  - Steps completed: {results['steps_completed']}")
print(f"  - Average reward: {results['average_reward']:.4f}")
print(f"  - Novelty score: {results['novelty_score']:.4f}")
```

---

## Part 8: Testing and Validation

### Python Binding Tests
```python
import pynlm

class PythonBindingTester:
    def __init__(self):
        self.test_results = []
    
    def test_config_api(self):
        """Test enhanced configuration API"""
        print("Testing configuration API...")
        
        # Test chaining
        config = (pynlm.createDefaultConfig()
                 .setValue("name", "Test")
                 .setInt("neuron_count", 100)
                 .setFloat("rate", 0.01))
        
        assert config.getValue("name", "") == "Test"
        assert config.getInt("neuron_count", 0) == 100
        assert config.getFloat("rate", 0.0) == 0.01
        
        self.test_results.append({"test": "config_api", "status": "passed"})
        print("  ✓ Configuration API test passed")
    
    def test_context_manager(self):
        """Test context manager functionality"""
        print("Testing context manager...")
        
        with pynlm.EnhancedConfig() as config:
            config.setValue("test", "value")
            assert config.getValue("test", "") == "value"
        
        self.test_results.append({"test": "context_manager", "status": "passed"})
        print("  ✓ Context manager test passed")
    
    def test_validation(self):
        """Test validation functions"""
        print("Testing validation...")
        
        # Valid configuration
        config = (pynlm.createDefaultConfig()
                 .setInt("neuron_count", 100)
                 .setInt("region_count", 2)
                 .setFloat("simulation_timestep", 0.001))
        
        errors = pynlm.validateConfig(config)
        assert len(errors) == 0
        
        # Invalid configuration
        config2 = (pynlm.createDefaultConfig()
                  .setInt("neuron_count", -100))
        
        errors2 = pynlm.validateConfig(config2)
        assert len(errors2) > 0
        
        self.test_results.append({"test": "validation", "status": "passed"})
        print("  ✓ Validation test passed")
    
    def test_brain_operations(self):
        """Test brain operations"""
        print("Testing brain operations...")
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Test basic operations
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() >= 0
        
        # Test step operations
        brain.step(0)
        brain.step(1)
        
        # Test action production
        action = brain.produceAction()
        assert action is not None
        
        # Test validation
        assert brain.isValid()
        
        self.test_results.append({"test": "brain_operations", "status": "passed"})
        print("  ✓ Brain operations test passed")
    
    def test_world_operations(self):
        """Test world operations"""
        print("Testing world operations...")
        
        world = pynlm.createValidatedWorld(10, 10, 5, 5)
        
        # Test configuration
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        
        # Test updates
        world.update(0.1)
        assert world.getSimulationTime() > 0
        
        # Test agent interaction
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test sensory input
        percept = world.getSensoryPercept()
        assert percept is not None
        
        self.test_results.append({"test": "world_operations", "status": "passed"})
        print("  ✓ World operations test passed")
    
    def run_all_tests(self):
        """Run all tests"""
        print("Running Python binding tests...")
        
        self.test_config_api()
        self.test_context_manager()
        self.test_validation()
        self.test_brain_operations()
        self.test_world_operations()
        
        # Summary
        passed = len([r for r in self.test_results if r["status"] == "passed"])
        total = len(self.test_results)
        
        print(f"\nTest Summary: {passed}/{total} tests passed")
        
        return passed == total

# Usage
binder_tester = PythonBindingTester()
all_passed = binder_tester.run_all_tests()

if all_passed:
    print("\n✓ All Python binding tests passed!")
else:
    print("\n✗ Some Python binding tests failed!")
```

---

## Part 9: Performance Monitoring and Benchmarking

### Performance Profiling
```python
import pynlm
import time
import statistics

class PerformanceProfiler:
    def __init__(self):
        self.benchmark_results = {}
    
    def benchmark_config_creation(self, num_configs=100):
        """Benchmark configuration creation performance"""
        print(f"Benchmarking {num_configs} configuration creations...")
        
        start_time = time.time()
        configs = []
        
        for i in range(num_configs):
            config = (pynlm.createDefaultConfig()
                     .setInt("neuron_count", 50 + (i % 50))
                     .setFloat("simulation_timestep", 0.001 + (i % 10) * 0.001))
            configs.append(config)
        
        end_time = time.time()
        
        duration = end_time - start_time
        configs_per_sec = num_configs / duration
        avg_time_per_config = duration / num_configs
        
        self.benchmark_results["config_creation"] = {
            "total_configs": num_configs,
            "duration_seconds": duration,
            "configs_per_second": configs_per_sec,
            "average_time_per_config_ms": avg_time_per_config * 1000,
            "min_time_per_config_ms": min_time,
            "max_time_per_config_ms": max_time
        }
        
        return self.benchmark_results["config_creation"]
    
    def benchmark_brain_creation(self, num_brains=50):
        """Benchmark brain creation performance"""
        print(f"Benchmarking {num_brains} brain creations...")
        
        start_time = time.time()
        brains = []
        
        for i in range(num_brains):
            config = (pynlm.createDefaultConfig()
                     .setInt("neuron_count", 100)
                     .setInt("region_count", 2))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        end_time = time.time()
        
        duration = end_time - start_time
        brains_per_sec = num_brains / duration
        avg_time_per_brain = duration / num_brains
        
        neuron_counts = [brain.getTotalNeuronCount() for brain in brains]
        
        self.benchmark_results["brain_creation"] = {
            "total_brains": num_brains,
            "duration_seconds": duration,
            "brains_per_second": brains_per_sec,
            "average_time_per_brain_ms": avg_time_per_brain * 1000,
            "avg_neurons": statistics.mean(neuron_counts),
            "std_neurons": statistics.stdev(neuron_counts) if len(neuron_counts) > 1 else 0,
            "min_neurons": min(neuron_counts),
            "max_neurons": max(neuron_counts)
        }
        
        return self.benchmark_results["brain_creation"]
    
    def benchmark_simulation(self, brain, steps=100, timestep=0.1):
        """Benchmark simulation performance"""
        print(f"Benchmarking simulation with {steps} steps...")
        
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step, step * timestep)
        
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_sec = steps / duration
        avg_time_per_step = duration / steps
        
        neuron_counts = [brain.getFiringNeuronCount() for _ in range(0, steps, 10)]
        avg_firing = statistics.mean(neuron_counts) if neuron_counts else 0
        
        self.benchmark_results["simulation"] = {
            "total_steps": steps,
            "duration_seconds": duration,
            "steps_per_second": steps_per_sec,
            "average_time_per_step_ms": avg_time_per_step * 1000,
            "avg_firing_neurons": avg_firing,
            "neuron_counts": neuron_counts
        }
        
        return self.benchmark_results["simulation"]
    
    def print_benchmark_report(self):
        """Print comprehensive benchmark report"""
        if not self.benchmark_results:
            print("No benchmark results available. Run benchmarks first.")
            return
        
        print("\n" + "="*60)
        print("NLM PERFORMANCE BENCHMARK REPORT")
        print("="*60)
        
        for benchmark_name, results in self.benchmark_results.items():
            print(f"\n{benchmark_name.upper().replace('_', ' ')}:")
            print(f"  Total operations: {results.get('total_configs', results.get('total_brains', results.get('total_steps')))}")
            print(f"  Duration: {results['duration_seconds']:.3f} seconds")
            print(f"  Operations per second: {results.get('configs_per_second', results.get('brains_per_second', results.get('steps_per_second'))):.1f}")
            print(f"  Average time per operation: {results.get('average_time_per_config_ms', results.get('average_time_per_brain_ms', results.get('average_time_per_step_ms'))):.2f} ms")
            
            if 'avg_neurons' in results:
                print(f"  Average neurons: {results['avg_neurons']:.1f} ± {results['std_neurons']:.1f}")
            
            if 'avg_firing_neurons' in results:
                print(f"  Average firing neurons: {results['avg_firing_neurons']:.1f}")
        
        print("\n" + "="*60)

# Usage
profiler = PerformanceProfiler()

# Run benchmarks
config_results = profiler.benchmark_config_creation(200)
brain_results = profiler.benchmark_brain_creation(100)

# Create brain for simulation benchmark
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
sim_results = profiler.benchmark_simulation(brain, 200)

# Print report
profiler.print_benchmark_report()
```

---

## Conclusion

This advanced documentation provides comprehensive coverage of NLM's enhanced Python API, including:

### Key Improvements Made:

1. **Enhanced Configuration API**: Chaining, validation, context managers
2. **Better Error Handling**: Safe operations, validation, recovery mechanisms
3. **Performance Tools**: Benchmarking, profiling, optimization guidance
4. **Advanced Agent Features**: Custom behaviors, exploration strategies
5. **Integration Capabilities**: File I/O, data persistence, external system integration
6. **Testing Framework**: Comprehensive test suites and validation
7. **Performance Monitoring**: Detailed profiling and benchmarking

### Backward Compatibility:

All new features are **fully backward compatible** with existing code. The basic API remains unchanged, while new features are available through enhanced classes and functions.

### Next Steps:

1. **Explore the new features** in `easy_usage.md`
2. **Study the advanced usage** in this document
3. **Implement custom agents** using the new APIs
4. **Run benchmarks** to optimize your simulations
5. **Integrate with external systems** for enhanced functionality

This enhanced NLM Python API provides powerful tools for neural simulation research and development while maintaining ease of use and backward compatibility.
