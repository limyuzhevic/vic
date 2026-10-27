pynlm

NLM (Neural Learning Machine) Python Interface
===============================================

pynlm is a Python module that provides a user-friendly interface to the NLM C++ neural simulation framework. It wraps the comprehensive C++ bindings to provide:

**Key Features:**
- Simple API for creating and running neural simulations
- Convenience functions for common operations
- Comprehensive error handling
- Type hints and documentation
- Advanced features for experienced users

**Quick Start Examples:**

# Example 1: Basic Neural Simulation
```python
import pynlm

# Create a default brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation completed with {brain.getTotalNeuronCount()} neurons")
```

# Example 2: Complete Agent Simulation
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Decision making
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return {
        'final_spikes': brain.getTotalSpikeCount(),
        'final_neurons': brain.getFiringNeuronCount(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel()
    }

# Run simulation
results = run_agent_simulation(1000)
print(f"Simulation complete! Results: {results}")
```

# Example 3: Advanced Configuration Management
```python
import pynlm
import json

def create_advanced_brain(config_dict=None):
    """Create a brain with advanced configuration."""
    
    config = pynlm.createDefaultConfig()
    
    if config_dict:
        # Apply advanced configuration
        if 'neuron_count' in config_dict:
            config.set("brain.neuron_count", config_dict['neuron_count'])
        if 'synapse_density' in config_dict:
            config.set("brain.synapse_density", config_dict['synapse_density'])
        if 'plasticity' in config_dict:
            for key, value in config_dict['plasticity'].items():
                config.set(f"plasticity.{key}", value)
        if 'neuromod' in config_dict:
            for key, value in config_dict['neuromod'].items():
                config.set(f"neuromod.{key}", value)
        if 'development' in config_dict:
            for key, value in config_dict['development'].items():
                config.set(f"development.{key}", value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Return both brain and config for inspection
    return brain, config

# Create with specific configuration
advanced_config = {
    'neuron_count': 5000,
    'synapse_density': 0.15,
    'plasticity': {
        'stdp': {'enable': True, 'learning_rate': 0.002},
        'hebbian': {'enable': True, 'learning_rate': 0.001},
        'structural': {'enable': True, 'synaptogenesis_rate': 0.0001}
    },
    'neuromod': {
        'dopamine': {'scale': 1.5},
        'curiosity': {'enable': True, 'exploration_rate': 0.3},
        'novelty': {'enable': True, 'threshold': 0.5}
    }
}

brain, config = create_advanced_brain(advanced_config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Config summary: {config.summary()}")
```

# Example 4: Batch Processing and Experimentation
```python
import pynlm
import time

def run_batch_experiments(configs, num_steps=500):
    """Run multiple experiments with different configurations."""
    
    results = []
    
    for i, config_dict in enumerate(configs):
        print(f"\n--- Experiment {i+1}/{len(configs)} ---")
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        
        # Configure based on config_dict
        if 'curiosity' in config_dict:
            agent.enableCuriosity(config_dict['curiosity'])
        if 'reward' in config_dict:
            agent.enableRewardModulation(config_dict['reward'])
        
        # Run experiment
        start_time = time.time()
        
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if config_dict.get('reward_modulation', False):
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
        
        experiment_time = time.time() - start_time
        
        # Collect results
        result = {
            'experiment_id': i,
            'config': config_dict,
            'final_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'execution_time': experiment_time,
            'neurons_active': brain.getFiringNeuronCount()
        }
        
        results.append(result)
        print(f"Completed in {experiment_time:.2f}s - {result['final_spikes']} spikes")
    
    return results

# Define different experiment configurations
experiment_configs = [
    {'curiosity': True, 'reward_modulation': True},
    {'curiosity': False, 'reward_modulation': True},
    {'curiosity': True, 'reward_modulation': False},
    {'curiosity': False, 'reward_modulation': False}
]

# Run batch experiments
experiment_results = run_batch_experiments(experiment_configs, 300)

# Analyze results
print("\n=== EXPERIMENT SUMMARY ===")
for result in experiment_results:
    print(f"Experiment {result['experiment_id']}:")
    print(f"  Curiosity: {result['curiosity_level']:.3f}")
    print(f"  Novelty: {result['novelty_level']:.3f}")
    print(f"  Spikes: {result['final_spikes']}")
    print(f"  Time: {result['execution_time']:.2f}s")
```

# Example 5: Real-time Monitoring and Control
```python
import pynlm
import threading
import time

class MonitoringBrain:
    """Brain wrapper with real-time monitoring capabilities."""
    
    def __init__(self, config=None):
        self.brain = pynlm.createBrain(config if config else pynlm.createDefaultConfig())
        self.brain.initialize()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.monitor_active = False
        self.monitoring_data = []
        
    def start_monitoring(self, interval=0.1, duration=None):
        """Start real-time monitoring of brain activity."""
        
        def monitor():
            self.monitor_active = True
            start_time = time.time()
            step_count = 0
            
            while self.monitor_active:
                if step_count % 10 == 0:  # Sample every 10 steps
                    data_point = {
                        'step': step_count,
                        'time': time.time() - start_time,
                        'neurons_firing': self.brain.getFiringNeuronCount(),
                        'total_spikes': self.brain.getTotalSpikeCount(),
                        'firing_rate': self.brain.getAverageFiringRate(),
                        'curiosity': self.agent.getCuriosityLevel(),
                        'novelty': self.agent.getNoveltyLevel(),
                        'development_stage': self.brain.getDevelopmentalStage()
                    }
                    self.monitoring_data.append(data_point)
                
                # Run a brain step
                self.brain.step(step_count)
                step_count += 1
                
                # Optional duration limit
                if duration and (time.time() - start_time) > duration:
                    break
                
                time.sleep(interval)
            
            self.monitor_active = False
        
        self.monitor_thread = threading.Thread(target=monitor)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        return self.monitor_thread
    
    def stop_monitoring(self):
        """Stop real-time monitoring."""
        self.monitor_active = False
        if hasattr(self, 'monitor_thread'):
            self.monitor_thread.join(timeout=1.0)
    
    def get_monitoring_summary(self):
        """Get summary of monitoring data."""
        if not self.monitoring_data:
            return None
            
        spikes = [d['total_spikes'] for d in self.monitoring_data]
        firing_rates = [d['firing_rate'] for d in self.monitoring_data]
        curiosities = [d['curiosity'] for d in self.monitoring_data]
        
        return {
            'steps_recorded': len(self.monitoring_data),
            'max_firing_rate': max(firing_rates),
            'min_firing_rate': min(firing_rates),
            'avg_firing_rate': sum(firing_rates) / len(firing_rates),
            'max_curiosity': max(curiosities),
            'min_curiosity': min(curiosities),
            'total_spikes': spikes[-1] if spikes else 0,
            'spike_rate_growth': (spikes[-1] - spikes[0]) / spikes[0] if spikes and spikes[0] > 0 else 0
        }

# Create monitoring brain
monitor_brain = MonitoringBrain()

# Start monitoring for 5 seconds
print("Starting monitoring for 5 seconds...")
monitor_thread = monitor_brain.start_monitoring(interval=0.1, duration=5.0)

# Do some work while monitoring
for i in range(50):
    time.sleep(0.1)

# Stop monitoring
monitor_brain.stop_monitoring()

# Get results
print("\n=== MONITORING RESULTS ===")
summary = monitor_brain.get_monitoring_summary()
if summary:
    for key, value in summary.items():
        print(f"{key}: {value:.4f}" if isinstance(value, float) else f"{key}: {value}")
```

# Example 6: Advanced Actions and Motor Control
```python
import pynlm

def create_advanced_world():
    """Create a world with complex objects and scenarios."""
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.setMaxEnergy(1000.0)
    
    # Add resources
    for x in range(5, 45, 10):
        for y in range(5, 45, 10):
            world.addObject(pynlm.WorldObject(
                x=float(x), y=float(y),
                obj_type=pynlm.WorldObjectType.Resource,
                value=10.0
            ))
    
    # Add hazards
    for x in range(20, 30, 5):
        for y in range(20, 30, 5):
            world.addObject(pynlm.WorldObject(
                x=float(x), y=float(y),
                obj_type=pynlm.WorldObjectType.Hazard,
                value=-5.0
            ))
    
    # Add walls
    for x in range(0, 50, 2):
        world.addObject(pynlm.WorldObject(
            x=float(x), y=0.0,
            obj_type=pynlm.WorldObjectType.Wall,
            radius=1.0
        ))
        world.addObject(pynlm.WorldObject(
            x=float(x), y=49.0,
            obj_type=pynlm.WorldObjectType.Wall,
            radius=1.0
        ))
    
    for y in range(0, 50, 2):
        world.addObject(pynlm.WorldObject(
            x=0.0, y=float(y),
            obj_type=pynlm.WorldObjectType.Wall,
            radius=1.0
        ))
        world.addObject(pynlm.WorldObject(
            x=49.0, y=float(y),
            obj_type=pynlm.WorldObjectType.Wall,
            radius=1.0
        ))
    
    return world

def run_advanced_agent(world, brain, agent, num_steps=1000):
    """Run an agent with advanced motor control and action selection."""
    
    # Create complex action sequences
    action_sequences = [
        [pynlm.ActionType.MoveForward] * 10 + [pynlm.ActionType.TurnLeft] * 5,
        [pynlm.ActionType.TurnRight] * 10 + [pynlm.ActionType.MoveBackward] * 5,
        [pynlm.ActionType.Look] * 5 + [pynlm.ActionType.Interact] * 5,
        [pynlm.ActionType.Eat] * 3 + [pynlm.ActionType.Rest] * 7,
        [pynlm.ActionType.MoveLeft] * 8 + [pynlm.ActionType.MoveRight] * 8,
        [pynlm.ActionType.MoveForward] * 20
    ]
    
    sequence_index = 0
    action_in_sequence = 0
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Select action
        if step % 50 == 0 and sequence_index < len(action_sequences):
            # Start new action sequence
            action = action_sequences[sequence_index][0]
            action_in_sequence = 1
            sequence_index += 1
        elif action_in_sequence < len(action_sequences[sequence_index - 1]):
            action = action_sequences[sequence_index - 1][action_in_sequence]
            action_in_sequence += 1
        else:
            # Random action
            action = pynlm.ActionType.Wait
        
        # Execute action with enhanced feedback
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Enhanced reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Action: {action}")
            print(f"  Position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
            print(f"  Energy: {world.getAgentBody().energy:.1f}")
            print(f"  Health: {world.getAgentBody().health:.1f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Return final stats
    return {
        'final_position': (world.getAgentBody().x, world.getAgentBody().y),
        'final_energy': world.getAgentBody().energy,
        'final_health': world.getAgentBody().health,
        'total_steps': step,
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel()
    }

# Create and run advanced simulation
print("=== Advanced Simulation ===")
world = create_advanced_world()
world.reset()

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

results = run_advanced_agent(world, brain, agent, 500)
print(f"\nAdvanced simulation completed!")
print(f"Final position: {results['final_position']}")
print(f"Final energy: {results['final_energy']}")
print(f"Final health: {results['final_health']}")
```

__all__ = [
    'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
    'run_agent_simulation', 'create_advanced_brain', 'run_batch_experiments',
    'MonitoringBrain', 'create_advanced_world', 'run_advanced_agent'
]
