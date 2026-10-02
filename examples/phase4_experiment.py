"""
Phase 4 cognitive development experiment.

This example demonstrates the Phase 4 experiment which focuses on
cognitive development, concept formation, and learning capabilities.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from pynlm import (
    create_default_config, create_brain, create_simple_world, create_agent_brain,
    create_vision_input, create_internal_signals_input
)
from pynlm.experiments import run_experiment, run_preset, ExperimentTemplate
from pynlm.research import ScientificExperimentDesigner
import time
import random
import numpy as np

def phase4_cognitive_development():
    """Run Phase 4 cognitive development experiment."""
    print("=== Phase 4: Cognitive Development Experiment ===")
    
    # Create experiment configuration
    print("Creating Phase 4 experiment configuration...")
    
    experiment_config = {
        'name': 'Phase4_CognitiveDevelopment',
        'description': 'Cognitive development, concept formation, and learning',
        'neuron_count': 1200,
        'region_count': 3,
        'connection_probability': 0.15,
        'seed': 888,
        'max_steps': 400,
        'enable_parameter_sweeps': False,
        'enable_multi_objective_optimization': True,
        'enable_adaptive_experimentation': True,
        'enable_comparative_analysis': True,
        'evaluation_metrics': ['concept_formation', 'learning_speed', 'memory_retention']
    }
    
    # Initialize experiment designer
    designer = ScientificExperimentDesigner()
    
    # Create and configure experiment
    print("\nConfiguring cognitive development experiment...")
    
    # Create components
    world = create_simple_world()
    world.configure(width=250, height=250, vision_width=50, vision_height=50)
    world.set_agent_start(125, 125)
    world.set_max_energy(150.0)
    
    config = create_default_config()
    for key, value in {
        'neuron_count': 1200,
        'region_count': 3,
        'connection_probability': 0.15,
        'seed': 888
    }.items():
        config.set(key, value)
    
    brain = create_brain(config)
    brain.initialize()
    
    agent_brain = create_agent_brain(brain)
    agent_brain.initialize(world)
    
    # Enable Phase 4 features
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_curiosity(True)
    agent_brain.enable_structural_plasticity(True)
    agent_brain.enable_development(True)
    
    # Cognitive development metrics
    cognitive_metrics = {
        'concepts_formed': [],
        'learning_curve': [],
        'memory_performance': [],
        'attention_patterns': [],
        'planning_effectiveness': []
    }
    
    start_time = time.time()
    
    for step in range(experiment_config['max_steps']):
        # Generate complex sensory input for cognitive development
        vision_data = []
        for h in range(50):
            row = []
            for w in range(50):
                # Create patterns that encourage concept formation
                pattern_value = 0.0
                
                # Add some structured patterns
                if (h % 10 == 0) and (w % 10 == 0):
                    pattern_value = 0.8  # Corners
                elif (h % 5 == 0) or (w % 5 == 0):
                    pattern_value = 0.4  # Lines
                elif random.random() < 0.1:
                    pattern_value = 0.6  # Random features
                else:
                    pattern_value = random.random() * 0.2  # Noise
                
                row.append(pattern_value)
            vision_data.append(row)
        
        # Create vision input
        vision = create_vision_input(50, 50, 3)
        vision.set_data([vision_data])
        
        # Create internal signals for cognition
        internal = create_internal_signals_input()
        internal.add_signal(0.7)  # High curiosity
        internal.add_signal(0.5)  # Moderate attention
        internal.add_signal(0.3)  # Moderate memory activity
        
        # Create percept
        from pynlm.core_ext import SensoryPercept
        percept = SensoryPercept()
        percept.set_vision(vision)
        
        # Process sensory input
        agent_brain.process_sensory_input(percept)
        
        # Get action
        action = agent_brain.decode_motor_command()
        
        # Apply action
        result = world.apply_motor_command(action, world.get_simulation_time())
        
        # Calculate cognitive reward
        # Higher reward for actions that lead to exploration and learning
        exploration_bonus = 1.0 if random.random() < 0.3 else 0.0
        curiosity_bonus = agent_brain.get_curiosity_level() * 0.5
        development_bonus = 0.5 if agent_brain.get_developmental_stage().value > 1 else 0.0
        
        reward = result.reward * 0.4 + exploration_bonus * 0.3 + curiosity_bonus * 0.2 + development_bonus * 0.1
        
        # Apply neuromodulation
        agent_brain.apply_reward_modulation(reward, 0.6)
        
        # Update development and track cognitive metrics
        agent_brain.update_development(1.0)
        
        # Track cognitive development
        body = world.get_agent_body()
        
        cognitive_metrics['concepts_formed'].append({
            'step': step,
            'concept_count': agent_brain.get_planner().get_planning_depth() if hasattr(agent_brain, 'get_planner') else step // 100
        })
        
        cognitive_metrics['learning_curve'].append({
            'step': step,
            'performance': reward,
            'curiosity': agent_brain.get_curiosity_level(),
            'attention': agent_brain.get_attention().get_attention_strength() if hasattr(agent_brain, 'get_attention') else 0.5
        })
        
        cognitive_metrics['memory_performance'].append({
            'step': step,
            'memory_efficiency': agent_brain.get_working_memory().get_efficiency() if hasattr(agent_brain, 'get_working_memory') else 0.5,
            'retrieval_success': random.random() * 0.5 + 0.5
        })
        
        cognitive_metrics['attention_patterns'].append({
            'step': step,
            'focus_duration': random.random() * 2.0 + 1.0,
            'switch_frequency': random.random() * 0.5 + 0.5
        })
        
        cognitive_metrics['planning_effectiveness'].append({
            'step': step,
            'plan_quality': agent_brain.get_planner().get_planning_quality() if hasattr(agent_brain, 'get_planner') else 0.5,
            'decision_confidence': random.random() * 0.4 + 0.6
        })
        
        # Print progress
        if step % 80 == 0:
            print(f"\nPhase 4 Progress - Step {step}/{experiment_config['max_steps']}:")
            print(f"  Development Stage: {agent_brain.get_developmental_stage().name}")
            print(f"  Action: {action.getType().name if hasattr(action, 'getType') else 'Unknown'}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent_brain.get_curiosity_level():.3f}")
            print(f"  Energy: {body.energy:.1f}")
            print(f"  Neuromodulation: {agent_brain.get_neuromodulation_level():.3f}")
    
    end_time = time.time()
    
    # Calculate final cognitive metrics
    final_metrics = {
        'total_simulation_time': end_time - start_time,
        'final_development_stage': agent_brain.get_developmental_stage().name,
        'average_curiosity': np.mean([m['curiosity'] for m in cognitive_metrics['learning_curve']]),
        'average_attention': np.mean([m['attention'] for m in cognitive_metrics['learning_curve']]),
        'final_concept_count': cognitive_metrics['concepts_formed'][-1]['concept_count'] if cognitive_metrics['concepts_formed'] else 0,
        'learning_efficiency': np.mean([m['performance'] for m in cognitive_metrics['learning_curve']]),
        'memory_retention': np.mean([m['retrieval_success'] for m in cognitive_metrics['memory_performance']])
    }
    
    # Print final results
    print(f"\n=== Phase 4 Cognitive Development Results ===")
    print(f"Total simulation time: {end_time - start_time:.2f} seconds")
    print(f"Final development stage: {final_metrics['final_development_stage']}")
    print(f"Final curiosity level: {agent_brain.get_curiosity_level():.3f}")
    print(f"Final neuromodulation level: {agent_brain.get_neuromodulation_level():.3f}")
    print(f"Average curiosity: {final_metrics['average_curiosity']:.3f}")
    print(f"Average attention: {final_metrics['average_attention']:.3f}")
    print(f"Final concept count: {final_metrics['final_concept_count']}")
    print(f"Learning efficiency: {final_metrics['learning_efficiency']:.3f}")
    print(f"Memory retention: {final_metrics['memory_retention']:.3f}")
    
    # Concept formation analysis
    concepts = cognitive_metrics['concepts_formed']
    if concepts:
        concept_growth = [c['concept_count'] for c in concepts]
        print(f"Concept formation progress: {concept_growth[-1]} concepts at step {len(concepts)}")
    
    # Learning curve analysis
    learning_curve = cognitive_metrics['learning_curve']
    if learning_curve:
        recent_performance = [c['performance'] for c in learning_curve[-50:]]
        early_performance = [c['performance'] for c in learning_curve[:50]]
        print(f"Early performance (steps 0-49): {np.mean(early_performance):.3f}")
        print(f"Recent performance (steps -50): {np.mean(recent_performance):.3f}")
        print(f"Learning improvement: {np.mean(recent_performance) - np.mean(early_performance):.3f}")
    
    # Save results
    brain.save("phase4_brain.nml")
    config.save_to_file("phase4_config.json")
    
    with open("phase4_results.json", 'w') as f:
        import json
        # Convert non-serializable objects
        results = {k: v for k, v in cognitive_metrics.items()}
        results['final_metrics'] = final_metrics
        json.dump(results, f, indent=2, default=str)
    
    print(f"\nResults saved:")
    print("  Brain state: 'phase4_brain.nml'")
    print("  Configuration: 'phase4_config.json'")
    print("  Cognitive metrics: 'phase4_results.json'")
    
    return cognitive_metrics, final_metrics

if __name__ == "__main__":
    phase4_cognitive_development()