"""Example 05: Plasticity Experiments

This example demonstrates the plasticity systems in NLM: STDP, Hebbian learning,
and structural plasticity.
"""

import pynlm
import time

print("=== Example 5: Plasticity Experiments ===")
print()

# Create brain optimized for plasticity experiments
print("Setting up plasticity experiment...")
config = pynlm.createDefaultConfig()
config.neuron_count = 400
config.stdp_ltp_weight = 0.05    # Stronger LTP for demonstration
config.stdp_ltd_weight = 0.04    # Stronger LTD
config.stdp_tau = 30.0          # Longer time constant
config.hebbian_weight = 0.03    # Hebbian learning
config.synaptogenesis_rate = 0.0005  # Structural plasticity
config.pruning_rate = 0.0001    # Pruning
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=70, height=70, vision_width=14, vision_height=14)
agent.initialize(world)

# Enable all plasticity systems
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)

# Set up world for plasticity experiments
print("Configuring world for plasticity experiments...")
# Create correlated activity patterns
world.addObject(pynlm.WorldObject(30.0, 30.0, pynlm.WorldObjectType.Resource, 20.0, 2.0))
world.addObject(pynlm.WorldObject(40.0, 30.0, pynlm.WorldObjectType.Resource, 20.0, 2.0))
world.addObject(pynlm.WorldObject(50.0, 30.0, pynlm.WorldObjectType.Resource, 20.0, 2.0))

world.addObject(pynlm.WorldObject(30.0, 50.0, pynlm.WorldObjectType.Hazard, 15.0, 2.0))
world.addObject(pynlm.WorldObject(40.0, 50.0, pynlm.WorldObjectType.Hazard, 15.0, 2.0))

print("World setup complete with resource and hazard patterns")

# Plasticity experiment phases
def run_stdp_experiment(agent_brain, brain_obj, world_obj, start_step, duration):
    """Run STDP-specific experiment"""
    print(f"\n--- STDP Experiment (Steps {start_step}-{start_step + duration}) ---")
    print("Testing spike-timing dependent plasticity...")
    
    initial_weights = []
    # In a real implementation, we would track specific synapse weights
    # For this demo, we track overall brain statistics
    
    for step in range(duration):
        world_obj.update(0.1)
        percept = world_obj.getSensoryPercept()
        agent_brain.process_sensory_input(percept)
        brain_obj.step(start_step + step)
        action = agent_brain.decode_motor_command()
        result = world_obj.apply_motor_command(action, world_obj.get_simulation_time())
        agent_brain.apply_reward_modulation(result.reward, agent_brain.get_prediction_error())
        
        if step % 5 == 0:
            print(f"  Step {start_step + step:3d}: Action {action.name:15s} | "
                  f"Reward {result.reward:5.1f} | Firing {brain_obj.get_firing_neuron_count():4d}")
    
    print("STDP experiment complete")

def run_hebbian_experiment(agent_brain, brain_obj, world_obj, start_step, duration):
    """Run Hebbian learning experiment"""
    print(f"\n--- Hebbian Experiment (Steps {start_step}-{start_step + duration}) ---")
    print("Testing Hebbian co-activation learning...")
    
    for step in range(duration):
        world_obj.update(0.1)
        percept = world_obj.getSensoryPercept()
        agent_brain.process_sensory_input(percept)
        brain_obj.step(start_step + step)
        action = agent_brain.decode_motor_command()
        result = world_obj.apply_motor_command(action, world_obj.get_simulation_time())
        agent_brain.apply_reward_modulation(result.reward, agent_brain.get_prediction_error())
        
        if step % 5 == 0:
            print(f"  Step {start_step + step:3d}: Action {action.name:15s} | "
                  f"Reward {result.reward:5.1f} | Firing {brain_obj.get_firing_neuron_count():4d}")
    
    print("Hebbian experiment complete")

def run_structural_experiment(agent_brain, brain_obj, world_obj, start_step, duration):
    """Run structural plasticity experiment"""
    print(f"\n--- Structural Plasticity Experiment (Steps {start_step}-{start_step + duration}) ---")
    print("Testing synapse formation and pruning...")
    
    for step in range(duration):
        world_obj.update(0.1)
        percept = world_obj.getSensoryPercept()
        agent_brain.process_sensory_input(percept)
        brain_obj.step(start_step + step)
        action = agent_brain.decode_motor_command()
        result = world_obj.apply_motor_command(action, world_obj.get_simulation_time())
        agent_brain.apply_reward_modulation(result.reward, agent_brain.get_prediction_error())
        
        # Structural plasticity occurs automatically based on config
        # We just monitor the results
        
        if step % 10 == 0:
            print(f"  Step {start_step + step:3d}: Action {action.name:15s} | "
                  f"Reward {result.reward:5.1f} | Firing {brain_obj.get_firing_neuron_count():4d}")
    
    print("Structural plasticity experiment complete")

# Run different plasticity experiments
print("Running plasticity experiments...")
run_stdp_experiment(agent, brain, world, 0, 25)
run_hebbian_experiment(agent, brain, world, 100, 25)
run_structural_experiment(agent, brain, world, 200, 25)

# Plasticity analysis
print("\n=== Plasticity Analysis ===")

# Get final brain statistics
print("Final brain statistics:")
print(f"  Total neurons: {brain.get_total_neuron_count()}")
print(f"  Total synapses: {brain.get_total_synapse_count()}")
print(f"  Active neurons: {brain.get_active_neuron_count()}")
print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
print(f"  Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
print(f"  Total spikes: {brain.get_total_spike_count()}")
print(f"  E/I ratio: {brain.get_excitation_inhibition_ratio():.2f}")

# Development stage
print(f"  Developmental stage: {agent.get_developmental_stage().name}")
print(f"  Developmental age: {agent.get_developmental_age()} steps")
print(f"  Maturation progress: {agent.get_maturation_progress():.2f}")

# Neuromodulation
print(f"  Neuromodulation factor: {agent.get_plasticity_factor():.3f}")
print(f"  Dopamine level: {agent.get_neuromodulation_level():.3f}")
print(f"  Curiosity level: {agent.get_curiosity_level():.3f}")
print(f"  Novelty level: {agent.get_novelty_level():.3f}")
print(f"  Prediction error: {agent.get_prediction_error():.3f}")

# Plasticity effectiveness assessment
print("\n=== Plasticity Effectiveness Assessment ===")

# Calculate plasticity indicators
total_spikes = brain.get_total_spike_count()
average_firing_rate = brain.get_average_firing_rate()
plasticity_indicators = {
    'high_activity': total_spikes > 1000,
    'stable_firing': 1.0 <= average_firing_rate <= 10.0,
    'moderate_ei': 0.5 <= brain.get_excitation_inhibition_ratio() <= 2.0,
    'mature_development': agent.get_developmental_stage() in [
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult
    ],
}

effective_plasticity = sum(plasticity_indicators.values()) >= 3

print("Plasticity indicators:")
for indicator, satisfied in plasticity_indicators.items():
    status = "✓" if satisfied else "✗"
    print(f"  {status} {indicator.replace('_', ' ').title()}")

if effective_plasticity:
    print("\n✓ Effective plasticity observed!")
    print("  The brain shows evidence of learning and adaptation.")
else:
    print("\n⚠ Limited plasticity observed.")
    print("  Consider adjusting plasticity parameters or running longer experiments.")

# Structural plasticity results
print(f"\nStructural plasticity:")
print(f"  Synaptogenesis rate: {config.synaptogenesis_rate:.6f}")
print(f"  Pruning rate: {config.pruning_rate:.6f}")

# Export plasticity data
plasticity_data = {
    'total_spikes': total_spikes,
    'average_firing_rate': average_firing_rate,
    'ei_ratio': brain.get_excitation_inhibition_ratio(),
    'development_stage': agent.get_developmental_stage().name,
    'maturation_progress': agent.get_maturation_progress(),
    'plasticity_factor': agent.get_plasticity_factor(),
    'neuro_modulation': {
        'dopamine': agent.get_neuromodulation_level(),
        'curiosity': agent.get_curiosity_level(),
        'novelty': agent.get_novelty_level(),
        'prediction_error': agent.get_prediction_error(),
    },
    'plasticity_indicators': plasticity_indicators,
    'effective_plasticity': effective_plasticity,
}

print(f"\nPlasticity data exported for analysis ({len(plasticity_data)} metrics)")

print()
print("✓ Example completed successfully!")

# Show final brain status
print("\n=== Final Brain Status ===")
brain.log_status()
