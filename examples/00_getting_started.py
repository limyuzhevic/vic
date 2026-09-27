"""
getting_started.py - Quick start guide for NLM Python bindings
=============================================================

This script demonstrates the basic usage of the NLM Python bindings
for neural simulation and learning. It provides a comprehensive example
that walks through the complete workflow from initialization to analysis.

This is designed as both a tutorial and a working example that can be
run immediately to understand the basic functionality of the NLM Python
bindings.
"""

import sys
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_9408925c-2e91-4392-9006-99fe9a74848c')

import pynlm
import time
import numpy as np
def main():
    print("=== NLM Python Bindings - Getting Started ===\n")

    # Phase 1: Configuration Setup
    print("1. Creating configuration...")
    config = pynlm.Config.create_default()
    
    # Customize configuration for this example
    config.set("simulation.neuron_count", 500)
    config.set("plasticity.learning_rate", 0.01)
    config.set("neuromodulation.reward_sensitivity", 1.0)

    print(f"   Created configuration with {config.get('simulation.neuron_count')} neurons")

    # Phase 2: Brain Creation and Initialization
    print("\n2. Creating brain...")
    brain = pynlm.create_brain(config)
    print(f"   Created brain: {brain}")

    print("\n3. Initializing brain...")
    if brain.initialize():
        print("   Brain initialized successfully")
    else:
        print("   Brain initialization failed!")
        return

    # Phase 3: Basic Simulation
    print("\n4. Running simulation...")
    num_steps = 100

    for step in range(num_steps):
        brain.step(step)

        # Collect statistics every 10 steps
        if step % 10 == 0:
            stats = brain.get_stats()
            print(f"   Step {step:3d}: Active neurons={stats['active_neurons']:4d}, "
                  f"Firing rate={stats['firing_rate']:6.3f} Hz, "
                  f"Total spikes={stats['total_spikes']:4d}")

    print(f"\n   Simulation completed: {num_steps} steps")

    # Phase 4: Basic Statistics Analysis
    print("\n5. Final Statistics:")
    final_stats = brain.get_stats()
    for key, value in final_stats.items():
        print(f"   {key}: {value}")

    # Phase 5: Visualization
    print("\n6. Creating visualizations...")
    visualizer = pynlm.Visualizer(save_plots=False)
    
    # Create various plots to demonstrate capabilities
    try:
        visualizer.plot_activity("visualizations/neural_activity.png")
        print("   Neural activity visualization saved")
        
        visualizer.plot_memory_traces(brain, "visualizations/memory_usage.png")
        print("   Memory usage visualization saved")
        
        visualizer.plot_action_selection(brain, "visualizations/action_selection.png")
        print("   Action selection visualization saved")
    except Exception as e:
        print(f"   Visualization warning: {e}")

    print("   Visualizations saved to 'visualizations/' directory")

    # Phase 6: Memory System Demonstration
    print("\n7. Memory System Demonstration:")

    # Store working memory traces
    brain.memory.store_working_memory(1, 0.8, 10.0)
    brain.memory.store_working_memory(2, 0.6, 11.0)
    brain.memory.store_working_memory(3, 0.9, 12.0)

    print(f"   Stored {brain.memory.get_working_memory_usage()} working memory traces")

    # Store episode
    brain.memory.store_episode(
        sensory_input=[0.1, 0.2, 0.3, 0.4, 0.5],
        action_taken={"type": "move", "direction": "forward"},
        reward=1.0,
        neural_pattern=[0.8, 0.6, 0.9],
        timestamp=15.0
    )

    print(f"   Stored episode in episodic memory (total episodes: {brain.memory.get_episode_count()})")

    # Phase 7: Plasticity System Demonstration
    print("\n8. Plasticity System Demonstration:")

    # Create a mock synapse for demonstration
    class MockSynapse:
        def __init__(self):
            self.state = type('State', (), {
                'weight': 0.5,
                'last_pre_spike': 0.0,
                'last_post_spike': 0.0,
                'history_pre': [1.0, 2.0, 3.0],
                'history_post': [1.5, 2.5, 3.5]
            })()

        def set_weight(self, weight):
            self.state.weight = weight

    synapse = MockSynapse()

    # Apply STDP
    brain.plasticity.get_stdp().update(synapse, synapse.state.history_pre,
                                       synapse.state.history_post, 0.001)

    print(f"   Applied STDP: synapse weight = {synapse.state.weight:.3f}")

    # Phase 8: Neuromodulation System Demonstration
    print("\n9. Neuromodulation System Demonstration:")

    brain.neuromodulation.set_dopamine_level(0.7)
    brain.neuromodulation.set_novelty_level(0.4)
    brain.neuromodulation.set_curiosity_level(0.5)
    brain.neuromodulation.apply_reward(1.0)

    neuromod_stats = brain.neuromodulation.update(0.001)
    print(f"   Neuromodulation levels: Dopamine={neurom_stats['dopamine']:.2f}, "
          f"Novelty={neurom_stats['novelty']:.2f}, "
          f"Curiosity={neurom_stats['curiosity']:.2f}")

    # Phase 9: Cognitive System Demonstration
    print("\n10. Cognitive System Demonstration:")

    brain.cognition.set_planning_depth(3)
    brain.cognition.set_attention_inhibition(0.6)
    brain.cognition.set_attention_excitation(1.2)

    cognitive_stats = brain.cognition.update(0.001)
    print(f"   Cognitive load: {cognitive_stats['cognitive_load']:.2f}")
    print(f"   Active concepts: {brain.cognition.get_active_concept_count()}")

    # Phase 10: Development System Demonstration
    print("\n11. Development System Demonstration:")

    print(f"   Current developmental stage: {brain.development.get_stage()}")
    print(f"   Age: {brain.development.get_age():.1f}")
    print(f"   Plasticity modulator: {brain.development.get_plasticity_modulator():.2f}")

    # Phase 11: Prediction System Demonstration
    print("\n12. Prediction System Demonstration:")

    prediction = brain.prediction
    prediction.is_active = True

    test_state = [0.1, 0.2, 0.3, 0.4, 0.5]
    predicted_state = prediction.generate_prediction(test_state)
    error = prediction.calculate_error(test_state, predicted_state)

    print(f"   Current prediction error: {error:.3f}")
    print(f"   Prediction confidence: {prediction.get_confidence():.2f}")

    # Phase 12: Checkpointing
    print("\n13. Checkpointing:")
    brain.save("visualizations/brain_checkpoint.json")
    print("   Brain state saved to 'visualizations/brain_checkpoint.json'")

    # Phase 13: Loading (simulating process interruption)
    print("\n14. Loading brain state:")
    loaded_brain = pynlm.create_brain(config)
    loaded_brain.load("visualizations/brain_checkpoint.json")
    print("   Brain state loaded successfully")

    # Phase 14: Summary
    print("\n=== Getting Started Complete! ===")
    print("\nKey Achievements:")
    print(f"- Created brain with {config.get('simulation.neuron_count')} neurons")
    print(f"- Ran simulation for {num_steps} steps")
    print(f"- Collected {brain.memory.get_episode_count()} episodes in episodic memory")
    print(f"- Applied STDP: synapse weight changed to {synapse.state.weight:.3f}")
    print(f"- Neuromodulation levels: Dopamine={neurom_stats['dopamine']:.2f}, "
          f"Novelty={neurom_stats['novelty']:.2f}, "
          f"Curiosity={neurom_stats['curiosity']:.2f}")
    print(f"- Cognitive load: {cognitive_stats['cognitive_load']:.2f}")
    print(f"- Developmental stage: {brain.development.get_stage()}")
    print(f"- Prediction error: {error:.3f}")

    print("\nNext Steps:")
    print("1. Explore the examples directory for more advanced usage")
    print("2. Customize configuration parameters for your use case")
    print("3. Run experiments with different parameter combinations")
    print("4. Use visualization tools to analyze simulation results")
    print("5. Integrate with real-world data or environments")

    return {
        "brain": brain,
        "config": config,
        "visualizer": visualizer,
        "synapse": synapse,
        "stats": final_stats,
        "neuromod_stats": neuromod_stats,
        "cognitive_stats": cognitive_stats
    }
if __name__ == "__main__":
    main()
