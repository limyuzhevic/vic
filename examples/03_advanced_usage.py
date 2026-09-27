"""
advanced_usage.py - Advanced NLM usage examples
================================================

This script demonstrates advanced usage patterns and integration scenarios
for the NLM Python bindings.
"""

import sys
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_9408925c-2e91-4392-9006-99fe9a74848c')

import pynlm
import numpy as np
import time
from typing import Dict, Any, List
def advanced_usage():
    print("=== Advanced NLM Usage Examples ===\n")

    # Example 1: Custom architecture design
    print("1. Custom Architecture Design")
    print("   Creating brain with specialized architecture...")

    config = pynlm.Config.create_default()

    # Design a multi-region architecture
    architecture_config = {
        "regions": {
            "sensory": {"neuron_count": 200, "type": "sensory"},
            "cortex": {"neuron_count": 500, "type": "association"},
            "motor": {"neuron_count": 300, "type": "motor"}
        },
        "connectivity": {
            "sensory_to_cortex": 0.8,
            "cortex_to_motor": 0.9,
            "cortex_to_cortex": 0.6
        }
    }

    config.set("architecture.regions", architecture_config["regions"])
    config.set("architecture.connectivity", architecture_config["connectivity"])
    config.set("simulation.neuron_count", sum(r["neuron_count"] for r in architecture_config["regions"].values()))

    brain = pynlm.create_brain(config)
    brain.initialize()

    print(f"   Created brain with {brain.config.get('simulation.neuron_count')} neurons")
    print(f"   Architecture regions: {list(architecture_config['regions'].keys())}")

    # Example 2: Advanced plasticity configuration
    print("\n2. Advanced Plasticity Configuration")
    print("   Configuring sophisticated plasticity rules...")

    brain.plasticity.initialize()

    # Set up multiple plasticity mechanisms
    plasticity_config = {
        "hebbian": {
            "learning_rate": 0.02,
            "eligibility_trace": 0.95,
            "weight_range": [0.0, 1.0]
        },
        "stdp": {
            "learning_rate": 0.01,
            "window_size": 20.0,
            "pre_post_pair": True
        },
        "reward_modulated": {
            "learning_rate": 0.05,
            "prediction_error_gain": 1.0,
            "credit_assignment": "decay"
        },
        "structural": {
            "max_synapses_per_neuron": 100,
            "add_rate": 0.001,
            "prune_rate": 0.01
        }
    }

    # Apply plasticity configuration
    for rule_name, rule_config in plasticity_config.items():
        if hasattr(brain.plasticity, f'set_{rule_name}'):
            getattr(brain.plasticity, f'set_{rule_name}')(rule_config)
        elif hasattr(brain.plasticity, rule_name):
            setattr(brain.plasticity, rule_name, rule_config)

    print("   Plasticity rules configured:")
    for rule_name in plasticity_config.keys():
        print(f"     - {rule_name.replace('_', ' ').title()}")

    # Example 3: Complex neuromodulation system
    print("\n3. Complex Neuromodulation System")
    print("   Setting up advanced neuromodulation...")

    brain.neuromodulation.initialize()

    # Configure neuromodulators with interactions
    neuromod_config = {
        "dopamine": {
            "baseline": 0.1,
            "learning_signal": True,
            "phasicity": "burst",
            "reverberating": True
        },
        "curiosity": {
            "baseline": 0.05,
            "driven_by_novelty": True,
            "threshold": 0.2,
            "adaptation": 0.98
        },
        "novelty": {
            "baseline": 0.05,
            "detection_window": 10.0,
            "adaptation_rate": 0.9
        },
        "prediction_error": {
            "baseline": 0.0,
            "learning_signal": True,
            "signed": True
        }
    }

    # Apply neuromodulation configuration
    for modulator_name, modulator_config in neuromod_config.items():
        if hasattr(brain.neuromodulation, f'set_{modulator_name}'):
            getattr(brain.neuromodulation, f'set_{modulator_name}')(modulator_config)

    print("   Neuromodulators configured:")
    for modulator_name in neuromod_config.keys():
        print(f"     - {modulator_name.title()}")

    # Example 4: Cognitive system with planning
    print("\n4. Cognitive System with Planning")
    print("   Configuring cognitive planning capabilities...")

    brain.cognition.initialize()

    cognitive_config = {
        "planning": {
            "depth": 3,
            "lookahead": 2,
            "heuristic": "prediction_error"
        },
        "attention": {
            "inhibition_strength": 0.7,
            "excitation_strength": 1.2,
            "selection_threshold": 0.5
        },
        "concept_formation": {
            "cohesion_threshold": 0.8,
            "activation_threshold": 0.3,
            "decay_rate": 0.95
        }
    }

    # Apply cognitive configuration
    for cognitive_name, cognitive_config in cognitive_config.items():
        if hasattr(brain.cognition, f'set_{cognitive_name}'):
            getattr(brain.cognition, f'set_{cognitive_name}')(cognitive_config)

    print("   Cognitive capabilities configured:")
    for cognitive_name in cognitive_config.keys():
        print(f"     - {cognitive_name.title()}")

    # Example 5: Prediction system with forward models
    print("\n5. Prediction System with Forward Models")
    print("   Setting up prediction and forward modeling...")

    brain.prediction.is_active = True
    brain.prediction.forward_model_enabled = True

    prediction_config = {
        "forward_model": {
            "prediction_horizon": 5,
            "error_weight": 0.3,
            "confidence_weight": 0.7
        },
        "sensory_prediction": {
            "prediction_error_threshold": 0.1,
            "prediction_update_rate": 0.9
        },
        "action_prediction": {
            "prediction_window": 3,
            "action_precision": 0.8
        }
    }

    # Apply prediction configuration
    for pred_name, pred_config in prediction_config.items():
        if hasattr(brain.prediction, f'set_{pred_name}'):
            getattr(brain.prediction, f'set_{pred_name}')(pred_config)

    print("   Prediction capabilities configured")

    # Example 6: Development system with critical periods
    print("\n6. Development System with Critical Periods")
    print("   Configuring developmental processes...")

    brain.development.initialize()

    # Set initial developmental stage
    brain.development.set_developmental_stage("critical_period")

    development_config = {
        "critical_period": {
            "active": True,
            "sensitivity": 0.8,
            "duration": 100.0
        },
        "maturation": {
            "rate": 0.3,
            "threshold": 0.7,
            "irreversibility": True
        },
        "plasticity_modulation": {
            "baseline": 1.0,
            "critical_period_modulator": 1.5,
            "maturation_modulator": 0.6
        }
    }

    # Apply development configuration
    for dev_name, dev_config in development_config.items():
        if hasattr(brain.development, f'set_{dev_name}'):
            getattr(brain.development, f'set_{dev_name}')(dev_config)

    print("   Developmental processes configured:")
    print(f"   - Current stage: {brain.development.get_stage()}")
    print(f"   - Current age: {brain.development.get_age():.1f}")
    print(f"   - Plasticity modulator: {brain.development.get_plasticity_modulator():.3f}")

    # Example 7: Visualization with custom settings
    print("\n7. Visualization with Custom Settings")
    print("   Configuring advanced visualization...")

    visualizer_config = {
        "output_format": "png",
        "quality": 95,
        "figure_size": (12, 8),
        "dpi": 150,
        "save_intermediate": True,
        "plot_types": [
            "neural_activity",
            "memory_usage",
            "action_selection",
            "learning_curve",
            "neuromodulation",
            "development"
        ]
    }

    visualizer = pynlm.Visualizer(save_plots=True, **visualizer_config)

    print("   Visualization configuration:")
    for setting, value in visualizer_config.items():
        print(f"     - {setting}: {value}")

    # Example 8: Complex simulation scenario
    print("\n8. Complex Simulation Scenario")
    print("   Running advanced simulation...")

    # Run a complex simulation with all systems active
    simulation_steps = 500
    dt = 0.001

    # Enable all plasticity mechanisms
    brain.plasticity.enable_hebbian(True)
    brain.plasticity.enable_stdp(True)
    brain.plasticity.enable_reward_modulated(True)

    # Run simulation
    for step in range(simulation_steps):
        # Update all systems
        brain.step(step, dt)

        # Apply dynamic changes
        if step == simulation_steps // 4:
            brain.development.advance_stage()
            brain.neuromodulation.set_dopamine_level(0.8)
        elif step == simulation_steps // 2:
            brain.development.set_developmental_stage("maturation")
            brain.neuromodulation.set_curiosity_level(0.6)
        elif step == (3 * simulation_steps) // 4:
            brain.development.set_developmental_stage("adult")
            brain.neuromodulation.set_novelty_level(0.3)

        # Log progress
        if step % 50 == 0:
            stats = brain.get_stats()
            neuromod_state = brain.neuromodulation.get_state()
            print(f"   Step {step:3d}: "
                  f"Firing rate={stats['firing_rate']:6.3f} Hz, "
                  f"E/I ratio={stats['e_i_ratio']:5.2f}, "
                  f"Dopamine={neurom_state['dopamine']:.2f}")

    # Example 9: Experiment with parameter sweeps
    print("\n9. Experiment with Parameter Sweeps")
    print("   Setting up parameter sweep experiment...")

    experiment = pynlm.Experiment("advanced_usage", brain)

    # Add parameter sweeps
    experiment.add_parameter_sweep("learning_rate", [0.01, 0.05, 0.1])
    experiment.add_parameter_sweep("novelty_threshold", [0.1, 0.3, 0.5])
    experiment.add_parameter_sweep("curiosity_sensitivity", [0.3, 0.6, 0.9])

    # Run parameter sweep
    results = experiment.run_parameter_sweep()
    print(f"   Completed parameter sweep with {len(results)} combinations")

    # Analyze results
    print("\n   Parameter Sweep Analysis:")
    for i, result in enumerate(results):
        if hasattr(result, 'stats'):
            stats = result.stats
            print(f"     Combination {i+1}: "
                  f"Learning rate={stats.get('learning_rate', 'N/A')}, "
                  f"Success={stats.get('success_rate', 'N/A')}")

    # Example 10: Integration with external data
    print("\n10. Integration with External Data")
    print("   Demonstrating external data integration...")

    # Simulate external data integration
    external_data = {
        "sensory_inputs": np.random.randn(100, 5) * 0.1,
        "environmental_context": ["normal", "stress", "reward", "punishment"] * 25,
        "social_interactions": np.random.randint(0, 10, 100)
    }

    # Process external data
    for i, (input_data, context, interactions) in enumerate(
            zip(external_data["sensory_inputs"],
                external_data["environmental_context"],
                external_data["social_interactions"])):

        # Apply context-dependent processing
        context_multiplier = {"normal": 1.0, "stress": 0.7, "reward": 1.2, "punishment": 0.5}
        context_factor = context_multiplier.get(context, 1.0)

        # Scale input by context
        scaled_input = input_data * context_factor

        # Process through brain
        brain.prediction.process_sensory_input(scaled_input.tolist())

        # Store contextual episode
        brain.memory.store_episode(
            sensory_input=scaled_input.tolist(),
            action_taken={"type": "context_processing", "context": context},
            reward=min(interactions * 0.01, 1.0),
            neural_pattern=brain.get_neural_pattern() if hasattr(brain, 'get_neural_pattern') else [0.5] * 5,
            timestamp=time.time()
        )

        if i % 20 == 0:
            print(f"   Processed {i+1} external data points with context '{context}'")

    # Final visualization
    print("\n11. Final Visualization")
    print("   Creating comprehensive visualization...")

    visualizer.plot_all(brain, save_frames=False)

    # Save brain state
    print("\n12. Checkpointing")
    brain.save("visualizations/advanced_usage_checkpoint.json")
    print("   Brain state saved")

    # Summary
    print("\n=== Advanced Usage Complete! ===")
    print("\nAdvanced Features Demonstrated:")
    print("- Custom multi-region architecture")
    print("- Advanced plasticity rules (Hebbian, STDP, reward-modulated, structural)")
    print("- Complex neuromodulation system with interactions")
    print("- Cognitive planning and concept formation")
    print("- Forward model prediction systems")
    print("- Developmental critical periods and maturation")
    print("- Custom visualization settings")
    print("- Parameter sweep experiments")
    print("- External data integration")

    return {
        "brain": brain,
        "visualizer": visualizer,
        "experiment": experiment,
        "results": results,
        "external_data": external_data
    }
def main():
    """Main function to run advanced usage examples."""
    advanced_usage()
if __name__ == "__main__":
    main()