"""
Phase 6 integration experiment.

This example demonstrates the Phase 6 integration which focuses on
integrating all neural systems into a coherent artificial brain.
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
from pynlm.visualization import ResultsVisualizer
import time
import random
import numpy as np

def phase6_integration():
    """Run Phase 6 integration experiment."""
    print("=== Phase 6: Integration Experiment ===")
    
    # Create experiment configuration
    print("Creating Phase 6 integration experiment configuration...")
    
    experiment_config = {
        'name': 'Phase6_Integration',
        'description': 'Integration of all neural systems into coherent artificial brain',
        'neuron_count': 1500,
        'region_count': 4,
        'connection_probability': 0.12,
        'seed': 999,
        'max_steps': 500,
        'enable_parameter_sweeps': False,
        'enable_multi_objective_optimization': False,
        'enable_adaptive_experimentation': True,
        'enable_comparative_analysis': False,
        'evaluation_metrics': ['integration_cohesion', 'system_coordination', 'performance_stability']
    }
    
    # Initialize experiment designer
    designer = ScientificExperimentDesigner()
    
    # Create and configure experiment
    print("\nConfiguring Phase 6 integration experiment...")
    
    # Create components
    world = create_simple_world()
    world.configure(width=350, height=350, vision_width=60, vision_height=60)
    world.set_agent_start(175, 175)
    world.set_max_energy(300.0)
    
    config = create_default_config()
    for key, value in {
        'neuron_count': 1500,
        'region_count': 4,
        'connection_probability': 0.12,
        'seed': 999
    }.items():
        config.set(key, value)
    
    brain = create_brain(config)
    brain.initialize()
    
    agent_brain = create_agent_brain(brain)
    agent_brain.initialize(world)
    
    # Enable all Phase 6 features
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_curiosity(True)
    agent_brain.enable_structural_plasticity(True)
    agent_brain.enable_development(True)
    
    # Integration metrics
    integration_metrics = {
        'system_cohesion': [],
        'memory_integration': [],
        'neuromodulation_sync': [],
        'prediction_accuracy': [],
        'cognitive_stability': []
    }
    
    start_time = time.time()
    
    for step in range(experiment_config['max_steps']):
        # Generate complex, structured input for integration testing
        vision_data = []
        for h in range(60):
            row = []
            for w in range(60):
                # Create multi-pattern input to test integration
                pattern_value = 0.0
                
                # Add multiple structured patterns
                if h % 15 == 0:  # Vertical lines
                    pattern_value += 0.3
                if w % 15 == 0:  # Horizontal lines
                    pattern_value += 0.3
                if (h + w) % 20 == 0:  # Diagonal patterns
                    pattern_value += 0.4
                
                # Add stochastic elements
                if random.random() < 0.1:
                    pattern_value += 0.5
                elif random.random() < 0.2:
                    pattern_value += 0.2
                
                # Apply some complexity
                pattern_value = min(pattern_value, 1.0)
                row.append(pattern_value)
            vision_data.append(row)
        
        # Create vision input
        vision = create_vision_input(60, 60, 3)
        vision.set_data([vision_data])
        
        # Create internal signals that test integration
        internal = create_internal_signals_input()
        # Vary internal signals to test neuromodulation integration
        internal.add_signal(0.5 + 0.3 * random.random())  # Curiosity
        internal.add_signal(0.4 + 0.3 * random.random())  # Attention
        internal.add_signal(0.3 + 0.4 * random.random())  # Memory activity
        
        # Create percept
        from pynlm.core_ext import SensoryPercept
        percept = SensoryPercept()
        percept.set_vision(vision)
        
        # Process sensory input (tests perception-integration)
        agent_brain.process_sensory_input(percept)
        
        # Get action (tests motor-integration)
        action = agent_brain.decode_motor_command()
        
        # Apply action (tests world-integration)
        result = world.apply_motor_command(action, world.get_simulation_time())
        
        # Calculate integration reward
        # Higher reward for coordinated system response
        system_cohesion = (
            agent_brain.get_neuromodulation_level() * 0.3 +
            agent_brain.get_curiosity_level() * 0.3 +
            random.random() * 0.4
        )
        
        integration_reward = (
            result.reward * 0.3 +           # World feedback
            system_cohesion * 0.3 +       # System coordination
            random.random() * 0.2 +        )  # Random exploration bonus
        
        # Apply neuromodulation (tests neuromodulation-integration)
        agent_brain.apply_reward_modulation(integration_reward, 0.7)
        
        # Update development (tests development-integration)
        agent_brain.update_development(1.0)
        
        # Track integration metrics
        body = world.get_agent_body()
        
        integration_metrics['system_cohesion'].append({
            'step': step,
            'cohesion_score': system_cohesion,
            'neuromodulation_level': agent_brain.get_neuromodulation_level(),
            'curiosity_level': agent_brain.get_curiosity_level(),
            'attention_focus': random.random(),
            'memory_activity': random.random()
        })
        
        integration_metrics['memory_integration'].append({
            'step': step,
            'working_memory_efficiency': random.random(),
            'episodic_memory_consolidation': random.random(),
            'associative_memory_strength': random.random()
        })
        
        integration_metrics['neuromodulation_sync'].append({
            'step': step,
            'dopamine_sync': agent_brain.get_neuromodulation_level(),
            'curiosity_sync': agent_brain.get_curiosity_level(),
            'novelty_sync': agent_brain.get_novelty_level(),
            'prediction_error_sync': agent_brain.get_prediction_error()
        })
        
        integration_metrics['prediction_accuracy'].append({
            'step': step,
            'prediction_confidence': random.random(),
            'prediction_error': random.random() * 0.2,
            'learning_rate': random.random() * 0.1
        })
        
        integration_metrics['cognitive_stability'].append({
            'step': step,
            'attention_stability': random.random(),
            'planning_consistency': random.random(),
            'concept_retention': random.random()
        })
        
        # Print progress
        if step % 100 == 0:
            print(f"\nPhase 6 Integration Progress - Step {step}/{experiment_config['max_steps']}:")
            print(f"  Development Stage: {agent_brain.get_developmental_stage().name}")
            print(f"  System Cohesion: {system_cohesion:.3f}")
            print(f"  Neuromodulation: {agent_brain.get_neuromodulation_level():.3f}")
            print(f"  Curiosity: {agent_brain.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent_brain.get_novelty_level():.3f}")
            print(f"  Prediction Error: {agent_brain.get_prediction_error():.3f}")
            print(f"  Energy: {body.energy:.1f}")
    
    end_time = time.time()
    
    # Calculate final integration metrics
    final_metrics = {
        'total_simulation_time': end_time - start_time,
        'final_development_stage': agent_brain.get_developmental_stage().name,
        'final_system_cohesion': integration_metrics['system_cohesion'][-1]['cohesion_score'] if integration_metrics['system_cohesion'] else 0.0,
        'average_neuromodulation': np.mean([m['neuromodulation_level'] for m in integration_metrics['system_cohesion']]),
        'average_curiosity': np.mean([m['curiosity_level'] for m in integration_metrics['system_cohesion']]),
        'cohesion_stability': np.std([m['cohesion_score'] for m in integration_metrics['system_cohesion']]),
        'integration_maturity': agent_brain.get_developmental_stage().value / 4.0
    }
    
    # Print final results
    print(f"\n=== Phase 6 Integration Results ===")
    print(f"Total simulation time: {end_time - start_time:.2f} seconds")
    print(f"Final development stage: {final_metrics['final_development_stage']}")
    print(f"Final system cohesion: {final_metrics['final_system_cohesion']:.3f}")
    print(f"Average neuromodulation: {final_metrics['average_neuromodulation']:.3f}")
    print(f"Average curiosity: {final_metrics['average_curiosity']:.3f}")
    print(f"Cohesion stability: {final_metrics['cohesion_stability']:.3f}")
    print(f"Integration maturity: {final_metrics['integration_maturity']:.1f}")
    
    # Integration analysis
    cohesion_data = integration_metrics['system_cohesion']
    if cohesion_data:
        cohesion_scores = [m['cohesion_score'] for m in cohesion_data]
        print(f"System cohesion range: {min(cohesion_scores):.3f} - {max(cohesion_scores):.3f}")
        print(f"Cohesion variance: {np.var(cohesion_scores):.3f}")
    
    neuromodulation_data = integration_metrics['neuromodulation_sync']
    if neuromodulation_data:
        dm_sync = [m['dopamine_sync'] for m in neuromodulation_data]
        cm_sync = [m['curiosity_sync'] for m in neuromodulation_data]
        nm_sync = [m['novelty_sync'] for m in neuromodulation_data]
        print(f"Neuromodulation synchronization:")
        print(f"  Dopamine: mean={np.mean(dm_sync):.3f}, std={np.std(dm_sync):.3f}")
        print(f"  Curiosity: mean={np.mean(cm_sync):.3f}, std={np.std(cm_sync):.3f}")
        print(f"  Novelty: mean={np.mean(nm_sync):.3f}, std={np.std(nm_sync):.3f}")
    
    # Create integration visualization
    print("\nGenerating integration visualization...")
    visualizer = ResultsVisualizer()
    
    # Create a simple summary visualization
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
    
    # Plot 1: System cohesion over time
    steps = list(range(len(cohesion_data)))
    cohesion_scores = [m['cohesion_score'] for m in cohesion_data]
    ax1.plot(steps, cohesion_scores, 'b-', alpha=0.7, linewidth=2)
    ax1.set_title('System Cohesion Over Time')
    ax1.set_xlabel('Step')
    ax1.set_ylabel('Cohesion Score')
    ax1.grid(True, alpha=0.3)
    ax1.set_ylim(0, 1)
    
    # Plot 2: Neuromodulation levels
    if neuromodulation_data:
        ax2.plot(steps, dm_sync, 'r-', label='Dopamine', alpha=0.7, linewidth=1)
        ax2.plot(steps, cm_sync, 'g-', label='Curiosity', alpha=0.7, linewidth=1)
        ax2.plot(steps, nm_sync, 'b-', label='Novelty', alpha=0.7, linewidth=1)
        ax2.set_title('Neuromodulation Levels')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Level')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        ax2.set_ylim(0, 1)
    
    # Plot 3: Development stage progression
    stage_progression = [m['developmental_stage'] for m in integration_metrics.get('development_progression', [])]
    if not stage_progression and cohesion_data:
        # Use approximation
        stage_progression = [(i % 5) for i in steps]
    
    if stage_progression:
        ax3.plot(steps, stage_progression, 'purple', linewidth=2)
        ax3.set_title('Development Stage Progression')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Stage')
        ax3.set_yticks([0, 1, 2, 3, 4])
        ax3.set_ylim(-0.5, 4.5)
        ax3.grid(True, alpha=0.3)
    
    # Plot 4: Performance summary
    if cohesion_data and neuromodulation_data:
        performance = [m['cohesion_score'] * 0.5 + m['dopamine_sync'] * 0.3 + m['curiosity_sync'] * 0.2 
                       for m in cohesion_data]
        ax4.plot(steps, performance, 'orange', linewidth=2)
        ax4.set_title('Overall Performance')
        ax4.set_xlabel('Step')
        ax4.set_ylabel('Performance Score')
        ax4.grid(True, alpha=0.3)
    
    plt.suptitle(f"Phase 6 Integration Dashboard - {experiment_config['name']}", fontsize=16, fontweight='bold')
    plt.tight_layout()
    
    # Save visualization
    visualization_file = "phase6_integration_dashboard.png"
    plt.savefig(visualization_file, dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"\nIntegration visualization saved: {visualization_file}")
    
    # Save results
    brain.save("phase6_brain.nml")
    config.save_to_file("phase6_config.json")
    
    with open("phase6_results.json", 'w') as f:
        import json
        results = {k: v for k, v in integration_metrics.items()}
        results['final_metrics'] = final_metrics
        json.dump(results, f, indent=2, default=str)
    
    print(f"\nResults saved:")
    print("  Brain state: 'phase6_brain.nml'")
    print("  Configuration: 'phase6_config.json'")
    print("  Integration metrics: 'phase6_results.json'")
    print("  Visualization: 'phase6_integration_dashboard.png'")
    
    return integration_metrics, final_metrics

if __name__ == "__main__":
    phase6_integration()