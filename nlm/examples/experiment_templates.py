#!/usr/bin/env python3
"""
NLM Experiment Management Examples

These examples demonstrate how to use the NLM experiment management system
for running various experiments and analyzing results.
"""

from nlm import (
    BrainSimulation,
    ExperimentManager,
    get_experiment_manager,
    get_brain_simulation,
    create_medium_config,
    create_large_config,
)


def example_experiment_manager_setup():
    """Example 1: Setting up experiment manager"""
    print("=== Experiment Manager Setup Example ===")
    
    # Get the experiment manager instance
    manager = get_experiment_manager()
    
    print(f"Available experiments: {list(manager.available_experiments.keys())}")
    
    # Create a brain for experiments
    config = create_medium_config()
    simulation = BrainSimulation(config)
    simulation.initialize()
    
    print(f"Created brain with {simulation.brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {simulation.brain.getRegionCount()} regions")
    
    print()


def example_single_experiment():
    """Example 2: Running a single experiment"""
    print("=== Single Experiment Example ===")
    
    # Create brain and experiment manager
    config = create_medium_config()
    simulation = BrainSimulation(config)
    simulation.initialize()
    
    manager = get_experiment_manager()
    
    # Run a specific experiment
    print("Running working memory experiment...")
    result = manager.run_experiment(
        simulation.brain, 
        "working_memory", 
        trials=10, 
        steps_per_trial=100
    )
    
    print(f"Experiment completed!")
    print(f"Result keys: {list(result.keys()) if result else 'None'}")
    
    print()


def example_all_experiments():
    """Example 3: Running all experiments"""
    print("=== All Experiments Example ===")
    
    # Create brain with larger size for comprehensive experiments
    config = create_large_config()
    simulation = BrainSimulation(config)
    simulation.initialize()
    
    manager = get_experiment_manager()
    
    print("Running all experiments...")
    print("This may take several minutes...")
    
    results = manager.run_all_experiments(
        simulation.brain,
        trials=5,
        steps_per_trial=50
    )
    
    # Display results summary
    print("\nExperiment Results Summary:")
    print("=" * 50)
    
    successful_experiments = 0
    failed_experiments = 0
    
    for exp_name, result in results.items():
        if "error" in result:
            print(f"{exp_name}: FAILED - {result['error']}")
            failed_experiments += 1
        else:
            print(f"{exp_name}: SUCCESS")
            # Show key metrics if available
            if isinstance(result, dict):
                for key, value in result.items():
                    if key not in ['error'] and not key.startswith('_'):
                        print(f"  {key}: {value}")
            successful_experiments += 1
    
    print(f"\nSummary: {successful_experiments} successful, {failed_experiments} failed")
    
    print()


def example_experiment_with_input():
    """Example 4: Experiment with sensory input"""
    print("=== Experiment with Sensory Input Example ===")
    
    config = create_medium_config()
    simulation = BrainSimulation(config)
    simulation.initialize()
    
    manager = get_experiment_manager()
    
    print("Running temporal prediction experiment with visual input...")
    
    # First, inject some visual input to prime the system
    visual_input = [0.1, 0.3, 0.7, 0.9, 0.5] * 20
    
    # Inject input during some steps
    for step in range(0, 100, 10):
        simulation.inject_sensory_input(visual_input, "vision")
        simulation.run_simulation(10)
    
    # Now run the actual experiment
    result = manager.run_experiment(
        simulation.brain,
        "temporal_prediction",
        trials=3,
        steps_per_trial=80
    )
    
    print(f"Temporal prediction experiment completed")
    if result and "error" not in result:
        print(f"Key results: {list(result.keys())}")
    
    print()


def example_experiment_results_analysis():
    """Example 5: Analyzing experiment results"""
    print("=== Experiment Results Analysis Example ===")
    
    # This example shows how to analyze and visualize experiment results
    config = create_medium_config()
    simulation = BrainSimulation(config)
    simulation.initialize()
    
    manager = get_experiment_manager()
    
    # Run a few experiments
    print("Running selected experiments for analysis...")
    
    experiments_to_run = ["working_memory", "episodic_recall", "attention"]
    all_results = {}
    
    for exp_name in experiments_to_run:
        print(f"\nRunning {exp_name}...")
        result = manager.run_experiment(simulation.brain, exp_name, trials=3, steps_per_trial=50)
        all_results[exp_name] = result
    
    # Analyze results
    print("\n" + "=" * 60)
    print("EXPERIMENT RESULTS ANALYSIS")
    print("=" * 60)
    
    analysis_summary = {}
    
    for exp_name, result in all_results.items():
        if "error" not in result:
            print(f"\n{exp_name.upper()}:")
            
            # Extract key metrics
            metrics = {}
            if isinstance(result, dict):
                for key, value in result.items():
                    if key not in ['error'] and isinstance(value, (int, float)):
                        metrics[key] = value
            
            # Calculate performance scores (simplified)
            if metrics:
                # Example analysis: calculate normalized performance
                print("  Key metrics:")
                for key, value in metrics.items():
                    print(f"    {key}: {value:.4f}")
                
                # Calculate overall score (placeholder)
                overall_score = sum(metrics.values()) / len(metrics) if metrics else 0
                analysis_summary[exp_name] = overall_score
                print(f"  Overall score: {overall_score:.4f}")
    
    # Compare experiments
    if analysis_summary:
        print(f"\nEXPERIMENT COMPARISON:")
        print("-" * 40)
        for exp_name, score in sorted(analysis_summary.items(), key=lambda x: x[1], reverse=True):
            print(f"{exp_name}: {score:.4f}")
    
    print()


def example_experiment_reproduction():
    """Example 6: Reproducing experiments"""
    print("=== Experiment Reproduction Example ===")
    
    # Demonstrate how to reproduce experiments with different configurations
    configs = {
        "Small test": create_medium_config(),
        "Large scale": create_large_config(),
    }
    
    for config_name, config in configs.items():
        print(f"\n{config_name} experiment:")
        print(f"  Neurons: {config['neuron_count']}")
        print(f"  Regions: {config['region_count']}")
        
        simulation = BrainSimulation(config)
        simulation.initialize()
        
        manager = get_experiment_manager()
        
        # Run a simple experiment
        result = manager.run_experiment(
            simulation.brain,
            "working_memory",
            trials=2,
            steps_per_trial=30
        )
        
        print(f"  Completed: {'Success' if result and 'error' not in result else 'Failed'}")
    
    print()


def example_experiment_parameter_sweep():
    """Example 7: Experiment parameter sweep"""
    print("=== Experiment Parameter Sweep Example ===")
    
    # Demonstrate parameter sweep for a specific experiment
    print("Running parameter sweep for attention experiment...")
    print("(This would typically run many simulations with different parameters)")
    
    base_config = create_medium_config()
    
    # Different trials for parameter sweep
    trial_configs = [
        {"trials": 2, "steps": 40, "neurons": 500},
        {"trials": 5, "steps": 80, "neurons": 1000},
        {"trials": 10, "steps": 100, "neurons": 1500},
    ]
    
    results = []
    
    for i, params in enumerate(trial_configs):
        print(f"\nTrial {i+1}: {params}")
        
        # Adjust config based on parameters
        config = base_config.copy()
        config["neuron_count"] = params["neurons"]
        
        simulation = BrainSimulation(config)
        simulation.initialize()
        
        manager = get_experiment_manager()
        
        result = manager.run_experiment(
            simulation.brain,
            "attention",
            trials=params["trials"],
            steps_per_trial=params["steps"]
        )
        
        results.append({
            "trial": i+1,
            "parameters": params,
            "result": result,
            "success": result is not None and "error" not in result,
        })
    
    # Summarize parameter sweep results
    print(f"\nParameter Sweep Summary:")
    print("-" * 40)
    successful_trials = sum(1 for r in results if r["success"])
    print(f"Successful trials: {successful_trials}/{len(results)}")
    
    if successful_trials > 0:
        print("\nSuccessful trials performance:")
        for trial in results:
            if trial["success"]:
                print(f"  Trial {trial['trial']}: {trial['parameters']}")
    
    print()


if __name__ == "__main__":
    """
    Run all experiment management examples.
    
    This script demonstrates the experiment management features
    of the enhanced NLM interface.
    """
    
    print("NLM Experiment Management Examples")
    print("=" * 50)
    print()
    
    # Run examples (uncomment to run specific examples)
    # example_experiment_manager_setup()
    example_single_experiment()
    # example_all_experiments()
    # example_experiment_with_input()
    # example_experiment_results_analysis()
    # example_experiment_reproduction()
    example_experiment_parameter_sweep()
    
    print("\nExperiment examples completed!")
    print("\nFor more complex experiment scenarios, see the examples/ directory")
    print("for scripts that demonstrate specific experimental paradigms and analysis.")
