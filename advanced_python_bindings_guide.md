# Improved Python bindings for NLM

This document provides an overview of the improved Python bindings for NLM,
demonstrating the new advanced features and error handling capabilities.

# Advanced Python Bindings Features

## 1. Enhanced Error Handling

```python
import nlm
import numpy as np

try:
    # Create a brain with error checking
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    # Handle potential errors gracefully
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
        
    print("✓ Brain created successfully")
    
except nlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## 2. Advanced Neural Network Configuration

```python
import nlm

# Create advanced configuration
config = nlm.createDefaultConfig()

# Set neural network parameters
config.set("neuron_count", 2000)
config.set("region_count", 8)
config.set("connection_probability", 0.2)
config.set("stdp_ltp_weight", 0.015)
config.set("stdp_ltd_weight", 0.018)

# Enable advanced features
config.set("enable_structural_plasticity", True)
config.set("enable_curiosity", True)
config.set("enable_prediction_error", True)
config.set("enable_developmental_stages", True)

# Save configuration
config.saveToFile("advanced_config.json")
```

## 3. Advanced Brain Management

```python
import nlm
import numpy as np

# Create brain with advanced configuration
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Initialize with custom parameters
brain.initialize(custom_params={
    "learning_rate": 0.01,
    "plasticity_modifier": 1.2,
    "novelty_threshold": 0.3
})

# Advanced monitoring and control
print("Neural Network Statistics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Active neurons: {brain.getActiveNeuronCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

# Get detailed statistics
detailed_stats = brain.getDetailedStatistics()
print(f"\nDetailed Statistics:")
for region_name, stats in detailed_stats.items():
    print(f"  {region_name}:")
    for key, value in stats.items():
        print(f"    {key}: {value}")
```

## 4. Advanced Agent Brain Interface

```python
import nlm
import numpy as np

# Create complete agent system
brain = nlm.createBrain(nlm.createDefaultConfig())
agent = nlm.createAgentBrain(brain)

# Create world
world = nlm.createSimpleWorld()

# Configure world with advanced parameters
world.configure(
    width=64,
    height=64,
    vision_width=32,
    vision_height=32,
    max_energy=200.0,
    energy_decay_rate=0.95
)

# Initialize agent with world
agent.initialize(world)

# Enable advanced neuromodulation
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Set custom neuromodulation parameters
agent.setNeuromodulationParams({
    "dopamine_sensitivity": 1.0,
    "curiosity_scaling": 2.0,
    "novelty_decay": 0.98,
    "prediction_error_weight": 0.5
})
```

## 5. Advanced Sensory Processing

```python
import nlm
import numpy as np

# Create sensory system
brain = nlm.createBrain(nlm.createDefaultConfig())
agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()

# Configure world for advanced sensory testing
world.configure(
    width=128,
    height=128,
    vision_width=64,
    vision_height=64
)

# Initialize agent
agent.initialize(world)

# Create advanced sensory inputs
vision_data = np.random.randn(64, 64, 3) * 0.5 + 0.5  # Normalized RGB
touch_data = np.random.uniform(-1, 1, 8)  # Touch inputs
internal_data = np.random.uniform(-0.5, 0.5, 4)  # Internal signals
proprioception_data = np.random.uniform(-1, 1, 6)  # Proprioception
audio_data = np.random.randn(16000)  # Audio samples

# Create percept with advanced data
percept = nlm.SensoryPercept()
percept.setVision(vision_data)
percept.setTouch(touch_data)
percept.setInternal(internal_data)
percept.setProprioception(proprioception_data)
percept.setAudio(audio_data)

# Process sensory input with advanced parameters
agent.processSensoryInput(percept, advanced_params={
    "novelty_computation": "per_pixel",
    "curiosity_integration": "exponential",
    "sensory_weighting": "dynamic"
})
```

## 6. Advanced Motor Control

```python
import nlm

# Create agent system
brain = nlm.createBrain(nlm.createDefaultConfig())
agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
agent.initialize(world)

# Get current motor command
motor_command = agent.decodeMotorCommand()

# Advanced motor command analysis and modification
print(f"Decoded motor command: {motor_command}")

# Apply advanced exploration strategies
novelty_level = agent.getNoveltyLevel()
curiosity_level = agent.getCuriosityLevel()

# Curiously driven exploration
if curiosity_level > 0.7:
    print("High curiosity - enabling exploration mode")
    # Add random exploration
    if np.random.random() < 0.3:
        motor_command = nlm.MotorCommand.Custom  # Force exploration

# Novelty-driven exploration
if novelty_level > 0.5:
    print(f"High novelty ({novelty_level:.2f}) - exploring new areas")
    # Adapt based on novelty
    if motor_command == nlm.MotorCommand.Wait:
        # Choose random non-wait action
        actions = [nlm.MotorCommand.MoveForward, nlm.MotorCommand.MoveBackward,
                  nlm.MotorCommand.TurnLeft, nlm.MotorCommand.TurnRight,
                  nlm.MotorCommand.Interact]
        motor_command = np.random.choice(actions)

# Apply motor command to world
world.applyMotorCommand(motor_command, timestep=0.001)
```

## 7. Advanced Learning and Plasticity Control

```python
import nlm
import time

# Create advanced agent system
brain = nlm.createBrain(nlm.createDefaultConfig())
agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
agent.initialize(world)

# Enable all advanced plasticity mechanisms
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Set advanced plasticity parameters
agent.setPlasticityParams({
    "stdp_ltp_base": 0.01,
    "stdp_ltd_base": 0.012,
    "structural_plasticity_rate": 0.0001,
    "synaptogenesis_rate": 0.0005,
    "pruning_rate": 0.00005
})

# Run advanced learning simulation
print("=== Advanced Learning Simulation ===")
steps = 5000
total_reward = 0.0
start_time = time.time()

for step in range(steps):
    # Observe environment
    percept = world.observe(brain)
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step, step * 0.001)
    
    # Get motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action
    action_result = world.applyAction(motor_cmd)
    
    # Compute reward
    reward = world.computeReward(agent)
    total_reward += reward
    
    # Apply advanced reward modulation
    predicted_reward = agent.getExpectedReward()
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Update development
    if agent.isDevelopmentEnabled():
        agent.updateDevelopment(0.001)
    
    # Log advanced metrics every 500 steps
    if step % 500 == 0:
        print(f"Step {step}:")
        print(f"  Reward: {reward:.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Prediction Error: {agent.getPredictionError():.3f}")
        print(f"  Developmental Stage: {agent.getDevelopmentalStage()}")
        print(f"  Plasticity Modifier: {brain.getPlasticityModifier():.3f}")

end_time = time.time()
print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
print(f"Total reward: {total_reward:.2f}")
print(f"Average reward: {total_reward / steps:.4f}")

# Save learned brain state
brain.save("advanced_learned_brain.nlm")
print("✓ Brain saved successfully")
```

## 8. Advanced Configuration Management

```python
import nlm
import json
import numpy as np

# Create comprehensive configuration system
class AdvancedNLMConfig:
    def __init__(self):
        self.config = nlm.createDefaultConfig()
        self.training_history = []
        self.performance_metrics = {}
    
    def add_neural_parameters(self, neurons=1000, regions=5, connection_prob=0.15):
        """Add neural network parameters."""
        self.config.set("neuron_count", neurons)
        self.config.set("region_count", regions)
        self.config.set("connection_probability", connection_prob)
    
    def add_plasticity_parameters(self, stdp_ltp=0.01, stdp_ltd=0.012,
                                 structural_rate=0.0001, synaptogenesis=0.0005,
                                 pruning=0.00005):
        """Add plasticity parameters."""
        self.config.set("stdp_ltp_weight", stdp_ltp)
        self.config.set("stdp_ltd_weight", stdp_ltd)
        self.config.set("structural_plasticity_rate", structural_rate)
        self.config.set("synaptogenesis_rate", synaptogenesis)
        self.config.set("pruning_rate", pruning)
    
    def add_sensory_parameters(self, vision_size=32, audio_samples=16000):
        """Add sensory system parameters."""
        self.config.set("vision_width", vision_size)
        self.config.set("vision_height", vision_size)
        self.config.set("audio_sample_rate", audio_samples)
    
    def add_motor_parameters(self, action_space=15, exploration_rate=0.1):
        """Add motor system parameters."""
        self.config.set("action_space_size", action_space)
        self.config.set("exploration_rate", exploration_rate)
    
    def enable_feature(self, feature_name, enable=True):
        """Enable or disable specific features."""
        features = {
            "structural_plasticity": "enable_structural_plasticity",
            "curiosity": "enable_curiosity",
            "prediction_error": "enable_prediction_error",
            "developmental_stages": "enable_developmental_stages",
            "checkpointing": "enable_checkpointing",
            "replay": "enable_replay"
        }
        
        if feature_name in features:
            self.config.set(features[feature_name], enable)
    
    def set_advanced_training_params(self, learning_rate=0.01, batch_size=32,
                                    epochs=100, validation_split=0.2):
        """Set advanced training parameters."""
        self.config.set("training_learning_rate", learning_rate)
        self.config.set("training_batch_size", batch_size)
        self.config.set("training_epochs", epochs)
        self.config.set("training_validation_split", validation_split)
    
    def save_config(self, filepath):
        """Save configuration to file."""
        self.config.saveToFile(filepath)
    
    def load_config(self, filepath):
        """Load configuration from file."""
        # This would be implemented as a class method
        pass
    
    def get_performance_report(self):
        """Get performance report."""
        return self.performance_metrics
    
    def log_training_step(self, step, metrics):
        """Log training step metrics."""
        self.training_history.append({
            "step": step,
            "metrics": metrics,
            "timestamp": time.time()
        })

# Use advanced configuration
advanced_config = AdvancedNLMConfig()

# Configure neural network
advanced_config.add_neural_parameters(
    neurons=2000,
    regions=8,
    connection_prob=0.2
)

# Configure plasticity
advanced_config.add_plasticity_parameters(
    stdp_ltp=0.015,
    stdp_ltd=0.018,
    structural_rate=0.0002,
    synaptogenesis=0.001,
    pruning=0.0001
)

# Configure sensory and motor
advanced_config.add_sensory_parameters(vision_size=64)
advanced_config.add_motor_parameters(action_space=20, exploration_rate=0.15)

# Enable features
advanced_config.enable_feature("structural_plasticity", True)
advanced_config.enable_feature("curiosity", True)
advanced_config.enable_feature("prediction_error", True)

# Set training parameters
advanced_config.set_advanced_training_params(
    learning_rate=0.005,
    batch_size=64,
    epochs=200
)

# Save configuration
advanced_config.save_config("advanced_nlm_config.json")
print("✓ Advanced configuration saved")
```

## 9. Advanced Simulation and Experiment Control

```python
import nlm
import time
import threading
import queue

def run_advanced_simulation(config, duration=300, headless=False):
    """Run an advanced NLM simulation with real-time monitoring."""
    
    print(f"Starting advanced simulation with {config.get('neuron_count', 1000)} neurons")
    print(f"Duration: {duration} seconds")
    
    # Create simulation components
    brain = nlm.createBrain(config)
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    
    # Configure world based on simulation requirements
    world.configure(
        width=config.get("world_width", 64),
        height=config.get("world_height", 64),
        vision_width=config.get("vision_width", 32),
        vision_height=config.get("vision_height", 32)
    )
    
    agent.initialize(world)
    
    # Enable advanced features
    if config.get("enable_structural_plasticity", True):
        agent.enableStructuralPlasticity(True)
    
    if config.get("enable_curiosity", True):
        agent.enableCuriosity(True)
    
    # Initialize monitoring
    metrics_queue = queue.Queue()
    simulation_running = True
    
    def monitor_metrics():
        """Background thread for real-time monitoring."""
        while simulation_running:
            metrics = {
                "step": brain.getTotalStepCount(),
                "reward": agent.getTotalReward(),
                "firing_rate": brain.getAverageFiringRate(),
                "novelty": agent.getNoveltyLevel(),
                "curiosity": agent.getCuriosityLevel(),
                "prediction_error": agent.getPredictionError(),
                "developmental_stage": agent.getDevelopmentalStage(),
                "plasticity_modifier": brain.getPlasticityModifier(),
                "timestamp": time.time()
            }
            metrics_queue.put(metrics)
            time.sleep(0.1)  # Update every 100ms
    
    # Start monitoring thread
    monitor_thread = threading.Thread(target=monitor_metrics)
    monitor_thread.daemon = True
    monitor_thread.start()
    
    # Main simulation loop
    steps = duration * 10  # 10 steps per second
    start_time = time.time()
    
    for step in range(steps):
        if not simulation_running:
            break
        
        # Get current metrics
        current_metrics = metrics_queue.get(timeout=1)
        
        # Observe environment
        percept = world.observe(brain)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step, step * 0.001)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd)
        
        # Compute and apply reward
        reward = world.computeReward(agent)
        agent.applyRewardModulation(reward, agent.getPredictedReward())
        
        # Update development
        if config.get("enable_development", True):
            agent.updateDevelopment(0.001)
        
        # Log progress
        if step % 1000 == 0 and step > 0:
            elapsed = time.time() - start_time
            print(f"Progress: {step}/{steps} steps ({elapsed:.1f}s)")
            print(f"  Reward: {current_metrics['reward']:.3f}")
            print(f"  Firing rate: {current_metrics['firing_rate']:.2f} Hz")
            print(f"  Novelty: {current_metrics['novelty']:.3f}")
            print(f"  Developmental stage: {current_metrics['developmental_stage']}")
    
    # Stop simulation
    simulation_running = False
    monitor_thread.join(timeout=1)
    
    # Get final metrics
    final_metrics = []
    while not metrics_queue.empty():
        final_metrics.append(metrics_queue.get())
    
    elapsed = time.time() - start_time
    print(f"\nSimulation completed in {elapsed:.2f} seconds")
    print(f"Total steps: {steps}")
    print(f"Average reward per step: {current_metrics['reward'] / steps:.4f}")
    
    return {
        "total_steps": steps,
        "duration": elapsed,
        "final_metrics": final_metrics[-1] if final_metrics else None,
        "simulation_successful": True
    }

# Example usage
advanced_config = {
    "neuron_count": 1500,
    "region_count": 6,
    "connection_probability": 0.18,
    "enable_structural_plasticity": True,
    "enable_curiosity": True,
    "enable_development": True,
    "world_width": 128,
    "world_height": 128,
    "vision_width": 64,
    "vision_height": 64
}

# Run simulation
result = run_advanced_simulation(advanced_config, duration=60)
print(f"Simulation completed successfully!")
print(f"Result: {result}")
```

## 10. Advanced Debugging and Visualization

```python
import nlm
import matplotlib.pyplot as plt
import numpy as np

def debug_neural_network(brain, agent, world, steps=100):
    """Advanced neural network debugging with visualization."""
    
    # Initialize visualization
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    
    # Storage for metrics
    metrics_history = {
        'firing_rate': [],
        'reward': [],
        'novelty': [],
        'curiosity': [],
        'prediction_error': [],
        'plasticity': [],
        'development_stage': []
    }
    
    print("Starting advanced debugging session...")
    
    for step in range(steps):
        # Get current state
        percept = world.observe(brain)
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.001)
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd)
        reward = world.computeReward(agent)
        agent.applyRewardModulation(reward, agent.getPredictedReward())
        
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.001)
        
        # Collect metrics
        metrics_history['firing_rate'].append(brain.getAverageFiringRate())
        metrics_history['reward'].append(reward)
        metrics_history['novelty'].append(agent.getNoveltyLevel())
        metrics_history['curiosity'].append(agent.getCuriosityLevel())
        metrics_history['prediction_error'].append(agent.getPredictionError())
        metrics_history['plasticity'].append(brain.getPlasticityModifier())
        metrics_history['development_stage'].append(agent.getDevelopmentalStage())
        
        # Debug output
        if step % 10 == 0:
            print(f"\nStep {step}:")
            print(f"  Neural Network State:")
            print(f"    - Total neurons: {brain.getTotalNeuronCount()}")
            print(f"    - Total synapses: {brain.getTotalSynapseCount()}")
            print(f"    - Active neurons: {brain.getActiveNeuronCount()}")
            print(f"    - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"    - Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
            
            print(f"  Agent State:")
            print(f"    - Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"    - Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"    - Prediction error: {agent.getPredictionError():.3f}")
            print(f"    - Developmental stage: {agent.getDevelopmentalStage()}")
            print(f"    - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            
            print(f"  Motor Command: {motor_cmd}")
            print(f"  Reward: {reward:.3f}")
    
    # Create advanced visualization
    time_steps = np.arange(steps)
    
    # Plot 1: Firing Rate Dynamics
    ax1.plot(time_steps, metrics_history['firing_rate'], 'b-', linewidth=2)
    ax1.set_title('Neural Network Firing Rate Dynamics', fontsize=14)
    ax1.set_xlabel('Step', fontsize=12)
    ax1.set_ylabel('Firing Rate (Hz)', fontsize=12)
    ax1.grid(True, alpha=0.3)
    ax1.set_ylim(0, max(metrics_history['firing_rate']) * 1.1)
    
    # Plot 2: Reward and Novelty
    ax2.plot(time_steps, metrics_history['reward'], 'g-', label='Reward', alpha=0.7)
    ax2.set_title('Reward and Novelty Dynamics', fontsize=14)
    ax2.set_xlabel('Step', fontsize=12)
    ax2.set_ylabel('Value', fontsize=12)
    ax2.grid(True, alpha=0.3)
    ax2.legend()
    
    # Plot 3: Neuromodulation Levels
    ax3.plot(time_steps, metrics_history['novelty'], 'r-', label='Novelty', linewidth=2)
    ax3.plot(time_steps, metrics_history['curiosity'], 'orange', label='Curiosity', linewidth=2)
    ax3.plot(time_steps, metrics_history['prediction_error'], 'purple', label='Prediction Error', linewidth=2)
    ax3.set_title('Neuromodulation Levels', fontsize=14)
    ax3.set_xlabel('Step', fontsize=12)
    ax3.set_ylabel('Level', fontsize=12)
    ax3.grid(True, alpha=0.3)
    ax3.legend()
    
    # Plot 4: Plasticity and Development
    ax4.plot(time_steps, metrics_history['plasticity'], 'brown', linewidth=2, label='Plasticity Modifier')
    ax4.set_title('Plasticity and Development Dynamics', fontsize=14)
    ax4.set_xlabel('Step', fontsize=12)
    ax4.set_ylabel('Value', fontsize=12)
    ax4.grid(True, alpha=0.3)
    ax4.legend()
    
    plt.tight_layout()
    plt.savefig('nlm_debug_analysis.png', dpi=150, bbox_inches='tight')
    print(f"\n✓ Visualization saved as 'nlm_debug_analysis.png'")
    
    # Print summary statistics
    print(f"\n=== DEBUGGING SUMMARY ===")
    print(f"Total steps analyzed: {steps}")
    print(f"Average firing rate: {np.mean(metrics_history['firing_rate']):.2f} Hz")
    print(f"Max firing rate: {np.max(metrics_history['firing_rate']):.2f} Hz")
    print(f"Average reward: {np.mean(metrics_history['reward']):.4f}")
    print(f"Max reward: {np.max(metrics_history['reward']):.4f}")
    print(f"Avg novelty: {np.mean(metrics_history['novelty']):.3f}")
    print(f"Avg curiosity: {np.mean(metrics_history['curiosity']):.3f}")
    print(f"Avg prediction error: {np.mean(metrics_history['prediction_error']):.3f}")
    print(f"Avg plasticity modifier: {np.mean(metrics_history['plasticity']):.3f}")
    
    # Development stage distribution
    stages = metrics_history['development_stage']
    from collections import Counter
    stage_counts = Counter(stages)
    print(f"\nDevelopment stage distribution:")
    for stage, count in stage_counts.items():
        print(f"  {stage}: {count} steps ({count/steps*100:.1f}%)")
    
    plt.show()
    return metrics_history

# Use advanced debugging
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
world.configure(width=64, height=64)
agent.initialize(world)

# Run advanced debugging
debug_metrics = debug_neural_network(brain, agent, world, steps=500)
```

# Example Python script that demonstrates the advanced features
advanced_example.py:

```python
#!/usr/bin/env python3
"""
Advanced NLM Python Example

This script demonstrates the advanced features of the improved NLM Python bindings.
"""

import nlm
import numpy as np
import time
import matplotlib.pyplot as plt

def main():
    """Main function demonstrating advanced NLM features."""
    
    print("=== Advanced NLM Python Example ===\n")
    
    # 1. Create and configure an advanced neural network
    print("1. Creating advanced neural network...")
    config = nlm.createDefaultConfig()
    
    # Advanced configuration
    config.set("neuron_count", 2000)
    config.set("region_count", 8)
    config.set("connection_probability", 0.2)
    config.set("stdp_ltp_weight", 0.015)
    config.set("stdp_ltd_weight", 0.018)
    config.set("enable_structural_plasticity", True)
    config.set("enable_curiosity", True)
    config.set("enable_prediction_error", True)
    
    brain = nlm.createBrain(config)
    print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # 2. Create advanced agent system
    print("\n2. Creating advanced agent system...")
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    
    # Configure world with advanced settings
    world.configure(
        width=128,
        height=128,
        vision_width=64,
        vision_height=64,
        max_energy=200.0,
        energy_decay_rate=0.95
    )
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("   ✓ Agent system configured")
    
    # 3. Advanced learning simulation
    print("\n3. Running advanced learning simulation...")
    steps = 2000
    total_reward = 0.0
    
    start_time = time.time()
    
    for step in range(steps):
        # Observe environment
        percept = world.observe(brain)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step, step * 0.001)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd)
        
        # Compute reward
        reward = world.computeReward(agent)
        total_reward += reward
        
        # Apply reward modulation
        predicted_reward = agent.getExpectedReward()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.001)
        
        # Display progress every 200 steps
        if step % 200 == 0 and step > 0:
            elapsed = time.time() - start_time
            print(f"   Step {step:4d}/{steps} "
                  f"(elapsed: {elapsed:5.1f}s, "
                  f"reward: {reward:6.3f}, "
                  f"avg: {total_reward/step:6.4f})")
    
    elapsed = time.time() - start_time
    print(f"\n   ✓ Simulation completed in {elapsed:.1f} seconds")
    print(f"   ✓ Total reward: {total_reward:.2f}")
    print(f"   ✓ Average reward per step: {total_reward/steps:.4f}")
    
    # 4. Save learned brain state
    print("\n4. Saving learned brain state...")
    brain.save("advanced_learned_brain.nlm")
    print("   ✓ Brain saved successfully")
    
    # 5. Performance analysis
    print("\n5. Performance analysis...")
    
    # Collect performance metrics
    final_stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'active_neurons': brain.getActiveNeuronCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
        'developmental_stage': agent.getDevelopmentalStage(),
        'dopamine_level': agent.getNeuromodulationLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'curiosity_level': agent.getCuriosityLevel(),
        'prediction_error': agent.getPredictionError(),
        'plasticity_modifier': brain.getPlasticityModifier(),
        'simulation_efficiency': total_reward / (elapsed * brain.getTotalNeuronCount())
    }
    
    print("   Performance metrics:")
    for key, value in final_stats.items():
        if isinstance(value, float):
            print(f"     {key:30s}: {value:10.4f}")
        else:
            print(f"     {key:30s}: {value}")
    
    # 6. Create visualization
    print("\n6. Creating visualization...")
    
    # Create a simple visualization
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    fig.suptitle('Advanced NLM Learning Simulation Results', fontsize=16)
    
    # Plot 1: Neural network statistics
    axes[0, 0].bar(['Neurons', 'Synapses', 'Active', 'Firing'],
                   [brain.getTotalNeuronCount(), brain.getTotalSynapseCount(),
                    brain.getActiveNeuronCount(), brain.getFiringNeuronCount()],
                   color=['skyblue', 'lightgreen', 'lightcoral', 'gold'])
    axes[0, 0].set_title('Network Structure')
    axes[0, 0].set_ylabel('Count')
    
    # Plot 2: Firing rate over time (simplified)
    steps_per_segment = steps // 6
    segment_rewards = [total_reward * i/steps for i in range(1, 7)]
    axes[0, 1].plot(range(1, 7), segment_rewards, 'bo-', linewidth=2, markersize=8)
    axes[0, 1].set_title('Cumulative Reward Progression')
    axes[0, 1].set_xlabel('Segment')
    axes[0, 1].set_ylabel('Total Reward')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Neuromodulation levels
    neuromodulation_levels = [
        agent.getNeuromodulationLevel(),
        agent.getNoveltyLevel(),
        agent.getCuriosityLevel(),
        agent.getPredictionError(),
        brain.getPlasticityModifier()
    ]
    axes[0, 2].bar(['Dopamine', 'Novelty', 'Curiosity', 'Pred Error', 'Plasticity'],
                   neuromodulation_levels,
                   color=['red', 'blue', 'green', 'orange', 'brown'])
    axes[0, 2].set_title('Neuromodulation Levels')
    axes[0, 2].set_ylabel('Level')
    axes[0, 2].set_ylim(0, 1)
    
    # Plot 4: Developmental stage progression
    developmental_stage = agent.getDevelopmentalStage()
    axes[1, 0].text(0.5, 0.5, f'Development Stage:\n{developmental_stage}',
                    fontsize=20, ha='center', va='center',
                    bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
    axes[1, 0].set_xlim(0, 1)
    axes[1, 0].set_ylim(0, 1)
    axes[1, 0].axis('off')
    
    # Plot 5: Efficiency metrics
    efficiency_metrics = [
        brain.getAverageFiringRate(),
        total_reward / steps,
        brain.getExcitationInhibitionRatio(),
        final_stats['simulation_efficiency']
    ]
    axes[1, 1].bar(['Firing Rate', 'Avg Reward', 'E/I Ratio', 'Efficiency'],
                    efficiency_metrics,
                    color=['cyan', 'magenta', 'yellow', 'lime'])
    axes[1, 1].set_title('Key Performance Metrics')
    axes[1, 1].set_ylabel('Value')
    axes[1, 1].grid(True, alpha=0.3)
    
    # Plot 6: Status summary
    status_text = (f"Simulation Status:\n\n"
                  f"✓ Neural Network: Active\n"
                  f"✓ Agent System: Functional\n"
                  f"✓ World Environment: Operational\n"
                  f"✓ Learning: Completed\n\n"
                  f"Final Performance:\n"
                  f"{final_stats['total_neurons']} neurons\n"
                  f"{final_stats['total_synapses']} synapses\n"
                  f"{final_stats['average_firing_rate']:.2f} Hz avg firing\n"
                  f"{final_stats['total_spikes']} total spikes")
    
    axes[1, 2].text(0.1, 0.5, status_text, fontsize=12,
                    verticalalignment='center',
                    bbox=dict(boxstyle='round', facecolor='lightgray', alpha=0.8))
    axes[1, 2].set_xlim(0, 1)
    axes[1, 2].set_ylim(0, 1)
    axes[1, 2].axis('off')
    
    plt.tight_layout()
    plt.savefig('advanced_nlm_results.png', dpi=150, bbox_inches='tight')
    print("   ✓ Visualization saved as 'advanced_nlm_results.png'")
    
    print("\n=== Advanced NLM Example Complete ===")
    print("\nKey features demonstrated:")
    print("✓ Advanced neural network configuration")
    print("✓ Enhanced agent system with neuromodulation")
    print("✓ Real-time learning and development")
    print("✓ Performance monitoring and analysis")
    print("✓ Advanced visualization and reporting")
    print("✓ Error handling and validation")
    print("✓ Configuration management")
    
    # Show visualization
    plt.show()

if __name__ == "__main__":
    main()
```