#!/usr/bin/env python3
"""
NLM Python Example: Developmental Learning
=============================================

This example demonstrates the enhanced NLM API with developmental learning,
synaptogenesis, pruning, and critical periods.
"""

import pynlm as nlm
import pynlm.utils as utils
import numpy as np

def main():
    print("NLM Developmental Learning Example")
    print("=" * 50)
    
    # Create development configuration
    print("1. Creating developmental configuration...")
    config = utils.ConfigUtils.create_development_config()
    config.set("development.initial_plasticity", 2.0)
    config.set("development.maturation_rate", 0.01)
    config.set("development.enable_synaptogenesis", True)
    config.set("development.enable_pruning", True)
    config.set("development.critical_period.enabled", True)
    config.set("development.critical_period.duration", 100)
    config.set("development.critical_period.sensitivity", 1.5)
    
    # Create brain
    print("\n2. Creating brain with developmental settings...")
    brain = nlm.create_brain(config)
    initial_stage = brain.get_developmental_stage()
    print(f"   Initial developmental stage: {initial_stage}")
    
    # Create agent
    print("\n3. Creating agent...")
    agent = nlm.create_agent_brain(brain)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Create simple world
    print("\n4. Creating world...")
    world = utils.WorldUtils.create_empty_world(width=100, height=100)
    
    # Record developmental stages
    print("\n5. Running developmental simulation...")
    development_stages = []
    neuromodulation_levels = []
    curiosity_levels = []
    
    developmental_steps = 200
    
    for step in range(developmental_steps):
        # Update agent development
        agent.update_development(step)
        
        # Record developmental stage
        current_stage = agent.get_developmental_stage()
        status = agent.get_agent_status()
        
        development_stages.append(current_stage)
        neuromodulation_levels.append(status['neuromodulation_level'])
        curiosity_levels.append(status['curiosity_level'])
        
        # Print developmental milestones
        if step % 20 == 0:
            print(f"   Step {step}:")
            print(f"     Developmental stage: {current_stage}")
            print(f"     Neuromodulation level: {status['neuromodulation_level']:.2f}")
            print(f"     Curiosity level: {status['curiosity_level']:.2f}")
            print(f"     Novelty level: {status['novelty_level']:.2f}")
            print(f"     Prediction error: {status['prediction_error']:.2f}")
    
    # Analyze developmental results
    print("\n6. Analyzing developmental results...")
    analysis = utils.DevelopmentUtils.evaluate_development(
        [{'developmental_stage': stage} for stage in development_stages]
    )
    
    print(f"   Developmental analysis:")
    print(f"     Initial stage: {analysis['initial_stage']}")
    print(f"     Final stage: {analysis['final_stage']}")
    print(f"     Stage progression: {analysis['stage_progression']}")
    print(f"     Development rate: {analysis['development_rate']:.3f} per step")
    
    # Analyze neuromodulation
    neuromod_analysis = utils.NeuromodulationUtils.analyze_neuromodulation_levels(
        agent, developmental_steps
    )
    
    print(f"\n   Neuromodulation analysis:")
    print(f"     Dopamine (neuromodulation) mean: {np.mean(neuromod_analysis['dopamine']):.2f}")
    print(f"     Curiosity mean: {np.mean(neuromod_analysis['curiosity']):.2f}")
    print(f"     Novelty mean: {np.mean(neuromod_analysis['novelty']):.2f}")
    print(f"     Prediction error mean: {np.mean(neuromod_analysis['prediction_error']):.2f}")
    
    # Plot developmental trajectories
    try:
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(3, 1, figsize=(10, 12))
        
        # Developmental stages
        axes[0].step(range(developmental_steps), development_stages, where='post')
        axes[0].set_title('Developmental Stage Progression')
        axes[0].set_xlabel('Step')
        axes[0].set_ylabel('Stage')
        axes[0].set_ylim(-0.1, 1.1)
        
        # Neuromodulation levels
        axes[1].plot(neurom_analysis['dopamine'], label='Dopamine')
        axes[1].plot(neurom_analysis['curiosity'], label='Curiosity')
        axes[1].plot(neurom_analysis['novelty'], label='Novelty')
        axes[1].set_title('Neuromodulation Levels')
        axes[1].set_xlabel('Step')
        axes[1].set_ylabel('Level')
        axes[1].legend()
        
        # Prediction error
        axes[2].plot(neurom_analysis['prediction_error'])
        axes[2].set_title('Prediction Error')
        axes[2].set_xlabel('Step')
        axes[2].set_ylabel('Error')
        
        plt.tight_layout()
        plt.savefig('developmental_results.png', dpi=150, bbox_inches='tight')
        print("\n   Plot saved: developmental_results.png")
        
    except ImportError:
        print("\n   Matplotlib not available, skipping plots")
    
    # Show final brain state
    print("\n7. Final brain state...")
    brain_stats = nlm.BrainUtils.get_neuron_statistics(brain)
    print(f"   Final statistics:")
    print(f"     Total neurons: {brain_stats['total_neurons']}")
    print(f"     Average firing rate: {brain_stats['avg_rate']:.2f} Hz")
    print(f"     E/I ratio: {brain_stats['e_i_ratio']:.2f}")
    print(f"     Final developmental stage: {agent.get_developmental_stage()}")
    
    # Save results
    print("\n8. Saving results...")
    brain.save("developmental_brain.nlm")
    utils.DataUtils.save_to_file({
        'development_stages': development_stages,
        'neuromodulation_levels': neuromodulation_levels,
        'curiosity_levels': curiosity_levels,
        'analysis': analysis,
        'neuromod_analysis': neuromod_analysis,
        'brain_stats': brain_stats
    }, "developmental_results.json")
    print("   Saved brain to: developmental_brain.nlm")
    print("   Saved results to: developmental_results.json")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
