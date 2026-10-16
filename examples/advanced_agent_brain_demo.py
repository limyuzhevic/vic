# Example 5: Advanced Agent Brain Demo

import pynlm

print("=== Advanced Agent Brain Demo ===")

# Step 1: Create advanced agent brain
print("\n1. Creating advanced agent brain...")

# Create brain with advanced configuration
config = pynlm.createConfig()
config["brain.neuron_count"] = 2000
config["plasticity.stdp.enable"] = True
config["neuromod.dopamine.scale"] = 1.5
config["development.enable"] = True

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent brain
agent = pynlm.createAgentBrain(brain)

# Step 2: Configure world
print("\n2. Configuring world for agent...")

world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()
world.setAgentStart(15.0, 15.0)

# Add some obstacles and resources
for x in range(5, 25, 6):
    for y in range(5, 25, 6):
        if x != 15 or y != 15:  # Don't add at center
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, radius=0.8))

for x in range(8, 22, 8):
    for y in range(8, 22, 8):
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=10.0, radius=0.5))

agent.initialize(world)

# Step 3: Enable advanced learning
print("\n3. Enabling advanced learning subsystems...")

# Enable all subsystems
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

print("  Enabled subsystems: Reward Modulation, Structural Plasticity, Development, Curiosity")

# Step 4: Create advanced event system
print("\n4. Setting up advanced event system...")

class AdvancedEventSystem:
    def __init__(self, agent):
        self.agent = agent
        self.event_handlers = {}
        self.event_queue = []
        self.event_stats = {
            'spike_events': 0,
            'action_events': 0,
            'reward_events': 0,
            'development_events': 0
        }
    
    def add_handler(self, event_type, handler, priority=0):
        """Add event handler with priority"""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append((priority, handler))
        self.event_handlers[event_type].sort(reverse=True)  # Higher priority first
    
    def emit_event(self, event_type, data):
        """Emit event and call handlers in priority order"""
        self.event_queue.append((event_type, data, len(self.event_queue)))
        self.event_stats[event_type + '_events'] += 1
        
        # Call handlers
        if event_type in self.event_handlers:
            for priority, handler in self.event_handlers[event_type]:
                try:
                    handler(data)
                except Exception as e:
                    print(f"  Error in event handler: {e}")
    
    def process_events(self):
        """Process all queued events"""
        while self.event_queue:
            event_type, data, _ = self.event_queue.pop(0)
            self.emit_event(event_type, data)

# Create event system
event_system = AdvancedEventSystem(agent)

# Add event handlers
def on_spike(data):
    """Handle spike events with advanced analysis"""
    neuron_id = data['neuron_id']
    spike_time = data['timestamp']
    step = data['step']
    
    # Analyze spike patterns
    spike_rate = calculate_spike_rate(neuron_id)
    if spike_rate > 10.0:  # High firing rate
        print(f"    [EVENT] High firing spike at neuron {neuron_id} ({spike_rate:.1f} Hz)")
    
    # Trigger learning if spike rate is high
    if spike_rate > 15.0:
        print(f"    [EVENT] Triggering plasticity based on high spike rate")

def on_action(data):
    """Handle action events with curiosity-based selection"""
    action = data['action']
    action_type = action.type
    
    # Check curiosity level
    curiosity = agent.getCuriosityLevel()
    if curiosity > 0.7:
        print(f"    [EVENT] High curiosity ({curiosity:.3f}) - exploring new actions")
    
    # Log action with statistics
    print(f"    [EVENT] Action: {action_type} at time {data['time']}")

def on_reward(data):
    """Handle reward events with prediction error analysis"""
    reward = data['reward']
    predicted = data['predicted_reward']
    error = data['prediction_error']
    
    print(f"    [EVENT] Reward: {reward:.3f} (predicted: {predicted:.3f}, error: {error:.3f})")
    
    # Update neuromodulation level
    if abs(error) > 0.5:
        print(f"    [EVENT] Large prediction error - adjusting learning rate")

def on_development(data):
    """Handle development events with stage transitions"""
    stage = data['stage']
    age = data['age']
    
    print(f"    [EVENT] Developmental stage changed to {stage} (age: {age:.1f})")
    
    # Adjust plasticity based on developmental stage
    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
        print(f"    [EVENT] Critical period - high plasticity enabled")
    elif stage == pynlm.DevelopmentalStage.Adult:
        print(f"    [EVENT] Adult stage - reduced plasticity")

# Register event handlers
event_system.add_handler("spike", on_spike, priority=1)
event_system.add_handler("action", on_action, priority=2)
event_system.add_handler("reward", on_reward, priority=1)
event_system.add_handler("development", on_development, priority=0)

# Helper function (would need to be implemented)
def calculate_spike_rate(neuron_id):
    """Calculate spike rate for a neuron (placeholder)"""
    return 5.0  # Placeholder

# Step 5: Run advanced simulation
print("\n5. Running advanced simulation with events...")

# Helper functions for advanced simulation
def add_exploration_noise(action, curiosity_level):
    """Add curiosity-based exploration noise to action selection"""
    import random
    if random.random() < curiosity_level * 0.3:
        # Occasionally choose a different action
        all_actions = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
        new_action = random.choice([a for a in all_actions if a != action.type])
        return pynlm.Action(new_action)
    return action

def check_task_completion(world_obj, config):
    """Check if task is completed (placeholder)"""
    return False

# Run simulation with events
for step in range(50):
    # Update world
    world.update(0.1)
    
    # Get perception
    percept = world.getSensoryPercept()
    
    # Emit pre-step event
    event_system.emit_event("action", {
        'action': percept,
        'time': world.getSimulationTime()
    })
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Emit spike event (simulated)
    event_system.emit_event("spike", {
        'neuron_id': 42,
        'timestamp': step * 0.001,
        'step': step
    })
    
    # Get action with curiosity
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    
    # Emit reward event
    event_system.emit_event("reward", {
        'reward': reward,
        'predicted_reward': 0.0,
        'prediction_error': reward - 0.0
    })
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Emit development event
    event_system.emit_event("development", {
        'stage': agent.getDevelopmentalStage(),
        'age': agent.getBrain().getDevelopmentalStage()
    })
    
    if step % 10 == 0:
        print(f"    Step {step}: "
              f"Action: {action.name()}, "
              f"Reward: {reward:.3f}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Dev Stage: {agent.getDevelopmentalStage()}")

# Process remaining events
event_system.process_events()

# Step 6: Analyze event statistics
print("\n6. Event system statistics...")
print(f"  Total spike events: {event_system.event_stats['spike_events']}")
print(f"  Total action events: {event_system.event_stats['action_events']}")
print(f"  Total reward events: {event_system.event_stats['reward_events']}")
print(f"  Total development events: {event_system.event_stats['development_events']}")

# Step 7: Advanced agent brain features
print("\n7. Advanced agent brain features...")

# Get agent statistics
stats = agent.getStats()
print(f"  Agent statistics:")
print(f"    Total steps: {stats['totalSteps']}")
print(f"    Average curiosity: {stats['avgCuriosity']:.3f}")
print(f"    Average novelty: {stats['avgNovelty']:.3f}")
print(f"    Total reward: {stats['totalReward']:.3f}")
print(f"    Episodes stored: {stats['episodesStored']}")

# Get brain statistics
brain_stats = brain.getSimulationStats()
print(f"  Brain statistics:")
print(f"    Total neurons: {brain_stats['total_neurons']}")
print(f"    Total synapses: {brain_stats['total_synapses']}")
print(f"    Firing neurons: {brain_stats['active_neurons']}")
print(f"    Average firing rate: {brain_stats['avg_firing_rate']:.3f} Hz")
print(f"    Development stage: {brain_stats['dev_stage']}")

# Step 8: Advanced action control
print("\n8. Advanced action control...")

# Demonstrate advanced action features
def analyze_action_potential(agent_brain, action):
    """Analyze potential action based on neural activity"""
    
    # Get motor neuron groups
    motor_groups = {
        'forward': agent_brain.getMotorNeurons(pynlm.MotorCommand.MoveForward),
        'backward': agent_brain.getMotorNeurons(pynlm.MotorCommand.MoveBackward),
        'turn_left': agent_brain.getMotorNeurons(pynlm.MotorCommand.TurnLeft),
        'turn_right': agent_brain.getMotorNeurons(pynlm.MotorCommand.TurnRight),
        'interact': agent_brain.getMotorNeurons(pynlm.MotorCommand.Interact),
        'wait': agent_brain.getMotorNeurons(pynlm.MotorCommand.Wait)
    }
    
    # Calculate action potentials
    action_potentials = {}
    for action_name, neurons in motor_groups.items():
        potential = sum(get_neuron_potential(n) for n in neurons) / len(neurons) if neurons else 0
        action_potentials[action_name] = potential
    
    return action_potentials

def get_neuron_potential(neuron):
    """Get potential of a neuron (placeholder)"""
    return 0.5  # Placeholder

# Analyze action potentials
action_potentials = analyze_action_potential(agent, None)
print(f"  Action potentials:")
for action_name, potential in action_potentials.items():
    print(f"    {action_name}: {potential:.3f}")

# Step 9: Performance monitoring
print("\n9. Performance monitoring...")

# Monitor agent performance
performance_metrics = {
    'steps_completed': 50,
    'average_curiosity': agent.getCuriosityLevel(),
    'average_novelty': agent.getNoveltyLevel(),
    'total_reward_received': agent.getNeuromodulationLevel(),
    'development_progress': agent.getDevelopmentalStage(),
    'learning_efficiency': calculate_learning_efficiency(agent)
}

print(f"  Performance metrics:")
for metric, value in performance_metrics.items():
    print(f"    {metric}: {value:.3f}" if isinstance(value, float) else f"    {metric}: {value}")

def calculate_learning_efficiency(agent_obj):
    """Calculate learning efficiency (placeholder)"""
    return 0.75  # Placeholder

# Step 10: Advanced state management
print("\n10. Advanced state management...")

# Demonstrate advanced state features
state_manager = AdvancedStateManager(agent)

# Save state
save_result = state_manager.save_state(f"agent_state_{int(world.getSimulationTime())}.json")
print(f"  State saved: {save_result}")

# Load state
load_result = state_manager.load_state("agent_state_0.json")
print(f"  State loaded: {load_result}")

# Get state statistics
state_stats = state_manager.get_state_statistics()
print(f"  State statistics:")
for stat, value in state_stats.items():
    print(f"    {stat}: {value:.3f}" if isinstance(value, float) else f"    {stat}: {value}")

# Step 11: Machine learning integration
print("\n11. Machine learning integration...")

# Create ML-enhanced agent
ml_agent = MLEnhancedAgent(agent)

# Train ML model
ml_data = collect_ml_data(agent, world)
ml_agent.train(ml_data)

# Use ML for action selection
ml_action = ml_agent.select_action(percept)
print(f"  ML-enhanced action selected: {ml_action.name()}")

# Step 12: Real-time adaptation
print("\n12. Real-time adaptation...")

# Demonstrate real-time adaptation
adaptation_manager = RealTimeAdaptationManager(agent)

# Adapt to environment changes
adaptation_result = adaptation_manager.adapt_environment(world)
print(f"  Adaptation result: {adaptation_result}")

# Step 13: Visualization
print("\n13. Visualization (matplotlib required)...")

import matplotlib.pyplot as plt
import numpy as np

# Create comprehensive visualization
fig, axes = plt.subplots(3, 3, figsize=(15, 12))
fig.suptitle("Advanced Agent Brain Simulation", fontsize=16)

# 1. Neural activity plot
ax = axes[0, 0]
time_points = np.arange(0, 50, 1)
firing_rates = [brain.getAverageFiringRate() for _ in range(50)]
ax.plot(time_points, firing_rates, 'b-', label='Firing Rate', linewidth=2)
ax.set_xlabel('Step')
ax.set_ylabel('Firing Rate (Hz)')
ax.set_title('Neural Activity')
ax.legend()
ax.grid(True)

# 2. Curiosity plot
ax = axes[0, 1]
curiosity_levels = [agent.getCuriosityLevel() for _ in range(50)]
ax.plot(time_points, curiosity_levels, 'r-', label='Curiosity', linewidth=2)
ax.set_xlabel('Step')
ax.set_ylabel('Curiosity Level')
ax.set_title('Curiosity Evolution')
ax.legend()
ax.grid(True)

# 3. Novelty plot
ax = axes[0, 2]
novelty_levels = [agent.getNoveltyLevel() for _ in range(50)]
ax.plot(time_points, novelty_levels, 'g-', label='Novelty', linewidth=2)
ax.set_xlabel('Step')
ax.set_ylabel('Novelty Level')
ax.set_title('Novelty Evolution')
ax.legend()
ax.grid(True)

# 4. Reward plot
ax = axes[1, 0]
rewards = [world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0 for _ in range(50)]
ax.plot(time_points, rewards, 'orange', label='Reward', linewidth=2, alpha=0.7)
ax.set_xlabel('Step')
ax.set_ylabel('Reward')
ax.set_title('Reward Sequence')
ax.legend()
ax.grid(True)

# 5. Development stage plot
ax = axes[1, 1]
development_stages = [agent.getDevelopmentalStage() for _ in range(50)]
stage_values = [1 if stage == pynlm.DevelopmentalStage.CriticalPeriod else 
                 2 if stage == pynlm.DevelopmentalStage.Maturation else 
                 3 if stage == pynlm.DevelopmentalStage.Adult else 0 
                 for stage in development_stages]
ax.plot(time_points, stage_values, 'purple', marker='o', markersize=3, linewidth=2)
ax.set_xlabel('Step')
ax.set_ylabel('Developmental Stage')
ax.set_title('Developmental Stage Evolution')
ax.set_yticks([0, 1, 2, 3])
ax.set_yticklabels(['Initial', 'Critical', 'Mature', 'Adult'])
ax.grid(True)

# 6. Action frequency plot
ax = axes[1, 2]
action_types = ['Move', 'Turn', 'Interact', 'Wait']
action_freq = [10, 8, 5, 27]  # Example frequencies
ax.bar(action_types, action_freq, color=['blue', 'green', 'orange', 'red'])
ax.set_xlabel('Action Type')
ax.set_ylabel('Frequency')
ax.set_title('Action Selection Distribution')
ax.grid(True, alpha=0.3)

# 7. Combined metrics plot
ax = axes[2, 0]
combined_metrics = [agent.getNeuromodulationLevel() + agent.getCuriosityLevel() + agent.getNoveltyLevel() for _ in range(50)]
ax.plot(time_points, combined_metrics, 'brown', label='Combined Metrics', linewidth=2)
ax.set_xlabel('Step')
ax.set_ylabel('Combined Metrics')
ax.set_title('Combined Internal State')
ax.legend()
ax.grid(True)

# 8. World state plot
ax = axes[2, 1]
agent_x = [agent.getBrain().getRegion(1).getAllNeurons()[0]->getState().membranePotential for _ in range(50)]
ax.plot(agent_x, 'cyan', label='Agent Potential', linewidth=2)
ax.set_xlabel('Step')
ax.set_ylabel('Membrane Potential (mV)')
ax.set_title('Agent Neural State')
ax.legend()
ax.grid(True)

# 9. Simulation efficiency plot
ax = axes[2, 2]
steps_per_sec = [50 / (step + 1) for step in range(50)]
ax.plot(time_points, steps_per_sec, 'black', label='Steps/Second', linewidth=2)
ax.set_xlabel('Step')
ax.set_ylabel('Efficiency (steps/sec)')
ax.set_title('Simulation Efficiency')
ax.legend()
ax.grid(True)

plt.tight_layout()
print("  Comprehensive visualization created (saved to file if available)")

print("\n=== Advanced Agent Brain Demo Complete ===")

# Class definitions for demo
def calculate_spike_rate(neuron_id):
    """Calculate spike rate for a neuron (placeholder implementation)"""
    return np.random.uniform(1.0, 15.0)  # Random rate between 1-15 Hz

class AdvancedStateManager:
    """Manage advanced agent state operations"""
    
    def __init__(self, agent):
        self.agent = agent
        self.state_history = []
    
    def save_state(self, filename):
        """Save current state to file"""
        import json
        import os
        
        state = {
            'simulation_time': os.path.getmtime(filename) if os.path.exists(filename) else 0,
            'brain_state': 'saved',
            'agent_state': {
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'development_stage': self.agent.getDevelopmentalStage()
            }
        }
        
        with open(filename, 'w') as f:
            json.dump(state, f, indent=2)
        
        return f"State saved to {filename}"
    
    def load_state(self, filename):
        """Load state from file"""
        import json
        import os
        
        if not os.path.exists(filename):
            return "State file not found"
        
        with open(filename, 'r') as f:
            state = json.load(f)
        
        return f"State loaded from {filename}: {state.get('simulation_time', 0)}"
    
    def get_state_statistics(self):
        """Get current state statistics"""
        return {
            'simulation_steps': 50,
            'average_curiosity': self.agent.getCuriosityLevel(),
            'average_novelty': self.agent.getNoveltyLevel(),
            'development_progress': 0.75,
            'learning_rate': 0.001
        }

class MLEnhancedAgent:
    """ML-enhanced agent wrapper"""
    
    def __init__(self, agent):
        self.agent = agent
        self.ml_model = None
    
    def collect_data(self, agent_obj, world_obj):
        """Collect data for ML training"""
        data = []
        # Implement data collection logic
        return data
    
    def train(self, training_data):
        """Train ML model"""
        print(f"  Training ML model with {len(training_data)} samples")
        self.ml_model = "trained_model"
    
    def select_action(self, percept):
        """Select action using ML model"""
        if self.ml_model:
            # Use ML model for action selection
            return pynlm.Action(pynlm.MotorCommand.MoveForward)
        return self.agent.decodeMotorCommand()

class RealTimeAdaptationManager:
    """Manage real-time adaptation"""
    
    def __init__(self, agent):
        self.agent = agent
    
    def adapt_environment(self, world):
        """Adapt environment based on agent performance"""
        # Implement adaptation logic
        return "environment_adapted"
    
    def optimize_performance(self):
        """Optimize agent performance"""
        # Implement performance optimization
        return "performance_optimized"

# Run the demo if this script is executed directly
if __name__ == "__main__":
    print("Starting Advanced Agent Brain Demo...")
    print("This demo showcases advanced NLM features including:")
    print("  - Event-driven systems")
    print("  - Advanced learning subsystems")
    print("  - Machine learning integration")
    print("  - Real-time adaptation")
    print("  - Comprehensive visualization")
    print("\nStarting simulation...")
    # Note: The full demo would require the actual implementation
    # of all helper functions and event system
    print("Demo setup complete. Run the full implementation for detailed results.")
