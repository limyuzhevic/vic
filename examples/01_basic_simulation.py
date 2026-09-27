"""
basic_simulation.py - Basic simulation example
===============================================

This script demonstrates basic neural simulation with NLM Python bindings.
It provides a comprehensive example of running a simulation, collecting
statistics, and analyzing the results.

This example walks through the complete simulation workflow and can be
used as a template for creating custom simulations.
"""

import sys
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_9408925c-2e91-4392-9006-99fe9a74848c')

import pynlm
import time
import numpy as np
def main():
    print("=== Basic NLM Simulation Example ===\n")

    # Phase 1: Configuration Setup
    print("1. Creating brain with standard configuration...")
    brain = pynlm.create_brain(pynlm.Config.create_default())

    # Customize simulation parameters for this example
    print("\n2. Customizing simulation parameters...")
    brain.config.set("simulation.duration", 200.0)
    brain.config.set("simulation.timestep", 0.001)
    brain.config.set("simulation.neuron_count", 500)

    print("   Configuration parameters set:")
    print(f"   - Duration: {brain.config.get('simulation.duration')} seconds")
    print(f"   - Timestep: {brain.config.get('simulation.timestep')} seconds")
    print(f"   - Neuron count: {brain.config.get('simulation.neuron_count')}")

    # Phase 2: Brain Initialization
    print("\n3. Initializing brain...")
    brain.initialize()
    print("   Brain initialized successfully")

    # Phase 3: Simulation Execution
    print("\n4. Running simulation...")
    num_steps = 200

    start_time = time.time()

    print(f"   Running {num_steps} simulation steps...")

    for step in range(num_steps):
        brain.step(step)

        # Report progress every 20 steps
        if step % 20 == 0:
            stats = brain.get_stats()
            print(f"   Step {step:3d}/{num_steps}: "
                  f"Active neurons={stats['active_neurons']:4d}, "
                  f"Firing rate={stats['firing_rate']:6.3f} Hz, "
                  f"E/I ratio={stats['e_i_ratio']:5.2f}")

    end_time = time.time()
    duration = end_time - start_time

    print(f"\n   Simulation completed in {duration:.2f} seconds")

    # Phase 4: Comprehensive Results Analysis
    print("\n5. Simulation Results Analysis:")
    stats = brain.get_stats()

    print("   Basic Statistics:")
    print(f"   - Total simulation steps: {num_steps}")
    print(f"   - Total spikes: {stats['total_spikes']}")
    print(f"   - Average firing rate: {stats['firing_rate']:.3f} Hz")
    print(f"   - Excitation/Inhibition ratio: {stats['e_i_ratio']:.3f}")
    print(f"   - Active neurons: {stats['active_neurons']}")
    print(f"   - Working memory usage: {stats.get('firing_neuron_count', 'N/A')}")
    print(f"   - Total neurons: {stats.get('total_neuron_count', 'N/A')}")
    print(f"   - Total synapses: {stats.get('total_synapse_count', 'N/A')}")

    # Memory system analysis
    print("\n6. Memory System Analysis:")
    print(f"   - Working memory traces: {brain.memory.get_working_memory_usage()}")
    print(f"   - Episodic memory episodes: {brain.memory.get_episode_count()}")

    # Neuromodulation analysis
    print("\n7. Neuromodulation Analysis:")
    neuromod_state = brain.neuromodulation.get_state()
    print(f"   - Dopamine level: {neuromod_state['dopamine']:.3f}")
    print(f"   - Curiosity level: {neuromod_state['curiosity']:.3f}")
    print(f"   - Novelty level: {neuromod_state['novelty']:.3f}")
    print(f"   - Prediction error: {neuromod_state['prediction_error']:.3f}")

    # Cognitive system analysis
    print("\n8. Cognitive System Analysis:")
    cognitive_load = brain.cognition.get_cognitive_load()
    active_concepts = brain.cognition.get_active_concept_count()
    print(f"   - Cognitive load: {cognitive_load:.3f}")
    print(f"   - Active concepts: {active_concepts}")

    # Development analysis
    print("\n9. Development Analysis:")
    dev_stage = brain.development.get_stage()
    age = brain.development.get_age()
    plasticity_mod = brain.development.get_plasticity_modulator()
    print(f"   - Developmental stage: {dev_stage}")
    print(f"   - Age: {age:.1f}")
    print(f"   - Plasticity modulator: {plasticity_mod:.3f}")

    # Phase 5: Visualization
    print("\n10. Creating visualizations...")
    visualizer = pynlm.Visualizer(save_plots=False)

    # Create comprehensive visualization
    visualizer.plot_all(brain, save_frames=False)

    print("   Neural activity visualization saved to 'visualizations/neural_activity.png'")
    print("   Memory usage visualization saved to 'visualizations/memory_usage.png'")
    print("   Action selection visualization saved to 'visualizations/action_selection.png'")

    # Phase 6: Checkpointing
    print("\n11. Checkpointing:")
    brain.save("visualizations/brain_checkpoint.json")
    print("   Brain state saved to 'visualizations/brain_checkpoint.json'")

    # Phase 7: Experiment Creation
    print("\n12. Creating experiment...")
    experiment = pynlm.Experiment("basic_simulation", brain)
    experiment.parameters = {
        "max_steps": num_steps,
        "timestep": 0.001,
        "neuron_count": 500
    }

    print(f"   Created experiment: {experiment}")
    print(f"   Experiment parameters: {experiment.parameters}")

    # Phase 8: Experiment Execution
    print("\n13. Running experiment...")
    result = experiment.run("basic_run")
    print(f"   Experiment completed: {result.duration:.2f} seconds")

    # Phase 9: Results Comparison (if multiple runs)
    print("\n14. Experiment Analysis:")
    if experiment.get_result_count() > 1:
        comparison = experiment.compare_experiments(experiment.get_results())
        print(f"   Mean metric value: {comparison.get('mean_value', 'N/A')}")
        print(f"   Standard deviation: {comparison.get('std_value', 'N/A')}")
    else:
        print(f"   Single experiment run: {result}")

    # Phase 10: Summary and Reporting
    print("\n=== Basic Simulation Complete! ===")
    print("\nSimulation Summary:")
    print(f"- Duration: {num_steps} steps ({duration:.2f} seconds)")
    print(f"- Total spikes: {stats['total_spikes']}")
    print(f"- Final firing rate: {stats['firing_rate']:.3f} Hz")
    print(f"- Working memory traces: {brain.memory.get_working_memory_usage()}")
    print(f"- Episodic memory episodes: {brain.memory.get_episode_count()}")

    # Calculate derived metrics
    avg_firing_rate = stats['firing_rate']
    avg_active_neurons = stats['active_neurons']
    avg_ei_ratio = stats['e_i_ratio']

    print("\nPerformance Metrics:")
    print(f"- Average firing rate: {avg_firing_rate:.3f} Hz")
    print(f"- Average active neurons: {avg_active_neurons}")
    print(f"- Average E/I ratio: {avg_ei_ratio:.3f}")

    # Quality of simulation
    spike_density = stats['total_spikes'] / (num_steps * 0.001)  # spikes per second
    memory_utilization = brain.memory.get_working_memory_usage() / brain.memory.get_working_memory_usage() if brain.memory.get_working_memory_usage() > 0 else 0

    print("\nSimulation Quality:")
    print(f"- Spike density: {spike_density:.2f} Hz (target: 1-100 Hz)")
    print(f"- Memory utilization: {memory_utilization:.2f}")

    return {
        "brain": brain,
        "stats": stats,
        "visualizations": visualizer,
        "experiment": experiment,
        "result": result,
        "duration": duration,
        "final_stats": final_stats if 'final_stats' in locals() else stats
    }
if __name__ == "__main__":
    main()
