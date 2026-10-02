"""
Phase 3 world interaction experiment.

This example demonstrates the Phase 3 experiment which focuses on
world interaction, sensory processing, and motor control.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from pynlm import (
    create_default_config, create_brain, create_simple_world, create_agent_brain,
    create_vision_input, create_internal_signals_input
)
from pynlm.experiments import run_experiment, run_preset, ExperimentTemplate
import time
import random

def phase3_world_interaction():
    """Run Phase 3 world interaction experiment."""
    print("=== Phase 3: World Interaction Experiment ===")
    
    # Create experiment configuration
    print("Creating Phase 3 experiment configuration...")
    
    experiment_config = {
        'name': 'Phase3_WorldInteraction',
        'description': 'World interaction, sensory processing, and motor control',
        'neuron_count': 800,
        'region_count': 2,
        'connection_probability': 0.12,
        'seed': 777,
        'max_steps': 300,
        'enable_parameter_sweeps': False,
        'enable_multi_objective_optimization': False,
        'enable_adaptive_experimentation': True,
        'enable_comparative_analysis': False,
        'evaluation_metrics': ['navigation_success', 'sensory_accuracy', 'motor_precision']
    }
    
    # Create and run experiment
    print("\nRunning Phase 3 experiment...")
    
    # Create components
    world = create_simple_world()
    world.configure(width=300, height=300, vision_width=40, vision_height=40)
    world.set_agent_start(150, 150)
    world.set_max_energy(200.0)
    
    config = create_default_config()
    for key, value in {
        'neuron_count': 800,
        'region_count': 2,
        'connection_probability': 0.12,
        'seed': 777
    }.items():
        config.set(key, value)
    
    brain = create_brain(config)
    brain.initialize()
    
    agent_brain = create_agent_brain(brain)
    agent_brain.initialize(world)
    
    # Enable Phase 3 features
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_curiosity(True)
    agent_brain.enable_structural_plasticity(True)
    agent_brain.enable_development(True)
    
    # Experiment simulation
    experiment_results = {
        'experiment_name': experiment_config['name'],
        'success_rate': 0,
        'average_reward': 0.0,
        'navigation_metrics': [],
        'sensory_metrics': [],
        'motor_metrics': []
    }
    
    start_time = time.time()
    
    for step in range(experiment_config['max_steps']):
        # Generate world state
        vision_data = [
            [random.random() * (1 if random.random() > 0.7 else 0.3) 
             for _ in range(40)] 
            for _ in range(40)
        ]
        
        # Add some obstacles
        for i in range(5):
            for j in range(5):
                if random.random() < 0.1:
                    vision_data[i + 20][j + 20] = 0.8  # Obstacle
        
        # Create sensory input
        vision = create_vision_input(40, 40, 3)
        vision.set_data([vision_data])
        
        internal = create_internal_signals_input()
        internal.add_signal(0.5)  # Curiosity signal
        internal.add_signal(0.3)  # Attention signal
        
        # Create simple percept (simplified for example)
        from pynlm.core_ext import SensoryPercept
        percept = SensoryPercept()
        percept.set_vision(vision)
        
        # Process sensory input
        agent_brain.process_sensory_input(percept)
        
        # Get action
        action = agent_brain.decode_motor_command()
        
        # Apply action
        result = world.apply_motor_command(action, world.get_simulation_time())
        
        # Calculate reward based on success and energy
        reward = result.reward * 0.5 + (world.get_max_energy() - world.get_agent_body().energy) * 0.1
        
        # Apply neuromodulation
        agent_brain.apply_reward_modulation(reward, 0.5)
        
        # Update development
        agent_brain.update_development(1.0)
        
        # Track metrics
        experiment_results['success_rate'] += 1 if result.success else 0
        experiment_results['average_reward'] += reward
        
        body = world.get_agent_body()
        experiment_results['navigation_metrics'].append({
            'step': step,
            'x': body.x,
            'y': body.y,
            'energy': body.energy,
            'distance_from_center': ((body.x - 150) ** 2 + (body.y - 150) ** 2) ** 0.5
        })
        
        # Print progress
        if step % 60 == 0:
            print(f"\nPhase 3 Progress - Step {step}/{experiment_config['max_steps']}:")
            print(f"  Action: {action.getType().name if hasattr(action, 'getType') else 'Unknown'}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Success: {result.success}")
            print(f"  Energy: {body.energy:.1f}")
            print(f"  Neuromodulation: {agent_brain.get_neuromodulation_level():.3f}")
            print(f"  Curiosity: {agent_brain.get_curiosity_level():.3f}")
    
    end_time = time.time()
    
    # Calculate final metrics
    experiment_results['average_reward'] /= experiment_config['max_steps']
    experiment_results['success_rate'] /= experiment_config['max_steps']
    experiment_results['simulation_time'] = end_time - start_time
    
    # Print final results
    print(f"\n=== Phase 3 Experiment Results ===")
    print(f"Total simulation time: {end_time - start_time:.2f} seconds")
    print(f"Success rate: {experiment_results['success_rate'] * 100:.1f}%")
    print(f"Average reward: {experiment_results['average_reward']:.3f}")
    print(f"Final neuromodulation level: {agent_brain.get_neuromodulation_level():.3f}")
    print(f"Final curiosity level: {agent_brain.get_curiosity_level():.3f}")
    print(f"Development stage: {agent_brain.get_developmental_stage().name}")
    
    # Navigation analysis
    nav_metrics = experiment_results['navigation_metrics']
    if nav_metrics:
        avg_distance = sum(m['distance_from_center'] for m in nav_metrics) / len(nav_metrics)
        print(f"Average distance from center: {avg_distance:.1f}")
        print(f"Final position: ({nav_metrics[-1]['x']:.1f}, {nav_metrics[-1]['y']:.1f})")
    
    # Save results
    brain.save("phase3_brain.nml")
    config.save_to_file("phase3_config.json")
    
    with open("phase3_results.json", 'w') as f:
        import json
        json.dump(experiment_results, f, indent=2)
    
    print(f"\nResults saved:")
    print("  Brain state: 'phase3_brain.nml'")
    print("  Configuration: 'phase3_config.json'")
    print("  Experiment results: 'phase3_results.json'")
    
    return experiment_results

if __name__ == "__main__":
    phase3_world_interaction()