"""
learning_experiment.py - Learning experiment example
===================================================

This script demonstrates learning capabilities of NLM Python bindings,
including plasticity, memory formation, and adaptive behavior.
"""

import sys
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_9408925c-2e91-4392-9006-99fe9a74848c')

import pynlm
import time
import numpy as np
def learning_experiment():
    print("=== NLM Learning Experiment ===\n")

    # Create brain with learning-focused configuration
    print("1. Creating brain with learning configuration...")
    config = pynlm.Config.create_default()
    config.set("plasticity.learning_rate", 0.05)
    config.set("plasticity.stdp_learning_rate", 0.01)
    config.set("neuromodulation.reward_sensitivity", 0.8)
    config.set("neuromodulation.curiosity_sensitivity", 0.6)
    config.set("neuromodulation.novelty_threshold", 0.3)

    brain = pynlm.create_brain(config)

    # Customize for learning experiment
    brain.config.set("simulation.neuron_count", 1000)
    brain.config.set("simulation.duration", 500.0)

    # Initialize brain
    print("\n2. Initializing brain...")
    brain.initialize()
    print("   Brain initialized successfully")

    # Phase 1: Exploration - Present novel stimuli
    print("\n3. Phase 1: Novelty-based Exploration")
    print("   Presenting novel sensory inputs...")

    # Create novel sensory inputs for exploration
    exploration_stimuli = [
        [0.1, 0.2, 0.3, 0.4, 0.5],
        [0.6, 0.7, 0.8, 0.9, 1.0],
        [0.0, 0.1, 0.9, 0.0, 0.1],
        [0.9, 0.0, 0.1, 0.9, 0.0],
        [0.3, 0.3, 0.3, 0.3, 0.3]
    ]

    exploration_rewards = [0.5, 0.8, 0.2, 0.9, 0.4]

    for i, (stimulus, reward) in enumerate(zip(exploration_stimuli, exploration_rewards)):
        # Process sensory input
        brain.prediction.process_sensory_input(stimulus)

        # Apply reward
        brain.neuromodulation.apply_reward(reward)

        # Update novelty detection
        brain.neuromodulation.update_novelty(stimulus)

        # Store episode in memory
        brain.memory.store_episode(
            sensory_input=stimulus,
            action_taken={"type": "explore", "stimulus_id": i},
            reward=reward,
            neural_pattern=brain.get_neural_pattern() if hasattr(brain, 'get_neural_pattern') else [0.5] * 5,
            timestamp=time.time()
        )

        print(f"   Stimulus {i+1}: Reward={reward:.2f}, Novelty detected")

    # Phase 2: Consolidation - Replay and strengthen connections
    print("\n4. Phase 2: Memory Consolidation")
    print("   Consolidating learned memories...")

    # Enable plasticity for learning
    brain.plasticity.initialize()

    # Simulate synaptic consolidation
    consolidation_steps = 100
    for step in range(consolidation_steps):
        brain.step(step, 0.001)

        # Apply reinforcement learning
        if step % 10 == 0:
            # Get prediction error
            prediction_error = brain.prediction.get_prediction_error()

            # Update weights based on prediction error
            brain.plasticity.apply_prediction_error(prediction_error)

            # Apply reward modulation
            if step % 20 == 0:
                brain.neuromodulation.apply_reward(0.5)

    print(f"   Completed {consolidation_steps} consolidation steps")

    # Phase 3: Test performance - Evaluate learned behavior
    print("\n5. Phase 3: Performance Testing")
    print("   Evaluating learned behavior...")

    # Test on novel stimuli
    test_stimuli = [
        [0.1, 0.1, 0.1, 0.1, 0.1],
        [0.5, 0.5, 0.5, 0.5, 0.5],
        [0.9, 0.9, 0.9, 0.9, 0.9]
    ]

    test_results = []
    for stimulus in test_stimuli:
        # Reset brain for clean test
        brain.reset()

        # Process stimulus
        brain.prediction.process_sensory_input(stimulus)
        prediction = brain.prediction.generate_prediction(stimulus)
        error = brain.prediction.calculate_error(stimulus, prediction)

        # Apply action
        action = brain.produce_action(stimulus)

        # Get result
        result = {
            "stimulus": stimulus,
            "prediction": prediction,
            "prediction_error": error,
            "action": action,
            "success": error < 0.1  # Success if prediction error is low
        }

        test_results.append(result)

        print(f"   Test stimulus {len(test_results)}: Error={error:.3f}, Action={action['type']}")

    # Phase 4: Development and adaptation
    print("\n6. Phase 4: Development and Adaptation")
    print("   Adapting to changing environment...")

    # Change developmental stage
    brain.development.advance_stage()

    # Update development system
    for step in range(50):
        brain.step(step + consolidation_steps, 0.001)

    # Check developmental changes
    dev_stage = brain.development.get_stage()
    age = brain.development.get_age()
    plasticity_mod = brain.development.get_plasticity_modulator()

    print(f"   Developmental stage: {dev_stage}")
    print(f"   Age: {age:.1f} time units")
    print(f"   Plasticity modulator: {plasticity_mod:.3f}")

    # Phase 5: Final evaluation and analysis
    print("\n7. Final Evaluation")

    # Analyze learning performance
    successful_tests = sum(1 for r in test_results if r["success"])
    success_rate = successful_tests / len(test_results)

    final_stats = brain.get_stats()
    final_neuromod = brain.neuromodulation.get_state()

    print(f"   Learning Success Rate: {success_rate:.1%}")
    print(f"   Total Spikes: {final_stats['total_spikes']}")
    print(f"   Final Firing Rate: {final_stats['firing_rate']:.3f} Hz")
    print(f"   Working Memory Usage: {brain.memory.get_working_memory_usage()}")
    print(f"   Episodic Memory Episodes: {brain.memory.get_episode_count()}")

    # Neuromodulation summary
    print("\n   Neuromodulation Summary:")
    print(f"   Dopamine Level: {final_neuromod['dopamine']:.3f}")
    print(f"   Curiosity Level: {final_neuromod['curiosity']:.3f}")
    print(f"   Novelty Level: {final_neuromod['novelty']:.3f}")
    print(f"   Prediction Error: {final_neuromod['prediction_error']:.3f}")

    # Create comprehensive visualization
    print("\n8. Creating visualizations...")
    visualizer = pynlm.Visualizer(save_plots=False)
    visualizer.plot_all(brain, save_frames=False)

    # Save brain state
    print("\n9. Checkpointing:")
    brain.save("visualizations/learning_experiment_checkpoint.json")
    print("   Brain state saved")

    # Create experiment
    print("\n10. Creating experiment...")
    experiment = pynlm.Experiment("learning_experiment", brain)
    experiment.parameters = {
        "phases": ["exploration", "consolidation", "testing", "development"],
        "novelty_threshold": 0.3,
        "learning_rate": 0.05,
        "reward_sensitivity": 0.8
    }

    # Run experiment
    result = experiment.run("learning_experiment_run")
    print(f"   Experiment completed: {result.duration:.2f} seconds")

    # Summary
    print("\n=== Learning Experiment Complete! ===")
    print("\nKey Findings:")
    print(f"- Successfully learned {successful_tests} out of {len(test_results)} tasks")
    print(f"- Average prediction error: {np.mean([r['prediction_error'] for r in test_results]):.3f}")
    print(f"- Memory consolidation: {brain.memory.get_episode_count()} episodes stored")
    print(f"- Neuromodulation levels: Dopamine={final_neuromod['dopamine']:.2f}, "
          f"Curiosity={final_neuromod['curiosity']:.2f}")

    return {
        "brain": brain,
        "test_results": test_results,
        "success_rate": success_rate,
        "final_stats": final_stats,
        "experiment": experiment,
        "result": result
    }

def main():
    """Main function to run the learning experiment."""
    learning_experiment()
if __name__ == "__main__":
    main()