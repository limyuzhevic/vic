#!/usr/bin/env python3
"""
Comprehensive NLM Advanced Examples

This script demonstrates all the new advanced features in the NLM Python bindings.
These examples showcase the enhanced simulation control, visualization tools,
analysis capabilities, configuration management, and advanced learning features
that make NLM powerful for advanced users.

Author: NLM Project
Date: 2026
License: MIT
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
import json
import time
import threading
import concurrent.futures
import pickle

def example_advanced_simulation_control():
    """Example 1: Advanced Simulation Control with Batch Processing"""
    print("=" * 80)
    print("Example 1: Advanced Simulation Control - Batch Processing")
    print("=" * 80)
    
    # Create configuration templates for batch processing
    configs = []
    
    # Base configuration
    base_config = pynlm.createDefaultConfig()
    base_config.set("brain.neuron_count", 500)
    base_config.set("brain.synapse_density", 0.1)
    configs.append({"name": "small_brain", "config": base_config, "steps": 1000})
    
    # Larger brain configuration
    large_config = pynlm.createDefaultConfig()
    large_config.set("brain.neuron_count", 2000)
    large_config.set("brain.synapse_density", 0.05)
    large_config.set("plasticity.stdp.enable", True)
    configs.append({"name": "large_brain", "config": large_config, "steps": 2000})
    
    # Complex brain configuration
    complex_config = pynlm.createDefaultConfig()
    complex_config.set("brain.neuron_count", 1000)
    complex_config.set("brain.synapse_density", 0.15)
    complex_config.set("plasticity.stdp.enable", True)
    complex_config.set("plasticity.hebbian.enable", True)
    complex_config.set("neuromod.curiosity.enable", True)
    complex_config.set("neuromod.novelty.enable", True)
    configs.append({"name": "complex_brain", "config": complex_config, "steps": 3000})
    
    print(f"Created {len(configs)} configuration templates for batch processing")
    
    # Run batch simulations using the new batch simulation method
    all_results = []
    
    for config_info in configs:
        print(f"\nRunning batch simulation for {config_info['name']}...")
        
        # Create list of configurations for batch processing
        batch_configs = [config_info["config"]]
        
        # Use the enhanced batch simulation run method
        batch_results = pynlm.runBatchSimulations(
            pynlm.createBrain(batch_configs[0]),
            batch_configs,
            [config_info["steps"]],
            parallel=False,
            max_parallel=2
        )
        
        all_results.extend(batch_results)
        print(f"  Completed batch simulation for {config_info['name']}")
    
    print(f"\nTotal batch simulations completed: {len(all_results)}")
    
    # Analyze results
    total_steps = sum(result.get('total_steps', 0) for result in all_results)
    avg_performance = np.mean([result.get('performance_score', 0) for result in all_results])
    
    print(f"\nBatch Processing Summary:")
    print(f"  Total steps executed: {total_steps}")
    print(f"  Average performance score: {avg_performance:.3f}")
    print(f"  Configuration variations tested: {len(configs)}")
    
    # Save batch results
    with open("batch_simulation_results.json", "w") as f:
        json.dump({
            "timestamp": datetime.now().isoformat(),
            "configs_tested": len(configs),
            "total_simulations": len(all_results),
            "total_steps": total_steps,
            "average_performance": avg_performance,
            "results": all_results
        }, f, indent=2)
    
    print("✓ Batch simulation results saved to 'batch_simulation_results.json'")
    return all_results

def example_advanced_visualization():
    """Example 2: Advanced Visualization Tools"""
    print("\n" + "=" * 80)
    print("Example 2: Advanced Visualization Tools")
    print("=" * 80)
    
    # Create brain for visualization
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation to generate data
    print("Running simulation to generate visualization data...")
    for i in range(500):
        brain.step(i)
    
    # Generate advanced visualization data using new visualization methods
    print("Generating advanced visualization data...")
    
    # Create visualization options
    viz_options = {
        "neural_activity_range": 1.0,
        "spatial_resolution": "high",
        "color_scheme": "neuroscience",
        "include_connectivity": True,
        "include_metrics": True
    }
    
    # Use the new visualization method
    viz_data = pynlm.createVisualizationData(
        brain,
        "comprehensive_analysis",
        viz_options
    )
    
    print(f"Generated visualization data with:")
    print(f"  Neural activity trace points: {len(viz_data.get('neural_activity_trace', []))}")
    print(f"  Population rate measurements: {len(viz_data.get('population_rates', []))}")
    print(f"  Spatial coordinate points: {len(viz_data.get('spatial_coordinates', []))}")
    print(f"  Connectivity matrix size: {len(viz_data.get('connectivity_matrix', []))}x{len(viz_data.get('connectivity_matrix', [])[0]) if viz_data.get('connectivity_matrix') else 0}")
    
    # Create comprehensive visualization plots
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle('NLM Advanced Visualization Suite', fontsize=16, fontweight='bold')
    
    # Plot 1: Neural activity trace
    ax = axes[0, 0]
    activity_trace = viz_data.get('neural_activity_trace', [])
    ax.plot(activity_trace, linewidth=1, alpha=0.7)
    ax.set_title('Neural Activity Trace', fontweight='bold')
    ax.set_xlabel('Time Step')
    ax.set_ylabel('Firing Rate')
    ax.grid(True, alpha=0.3)
    
    # Plot 2: Population rates
    ax = axes[0, 1]
    population_rates = viz_data.get('population_rates', [])
    if len(population_rates) >= 3:
        ax.bar(['Type 1', 'Type 2', 'Type 3'], population_rates[:3], 
               color=['#3498db', '#2ecc71', '#e74c3c'], alpha=0.8)
    ax.set_title('Population Firing Rates', fontweight='bold')
    ax.set_ylabel('Firing Rate (Hz)')
    ax.grid(True, alpha=0.3)
    
    # Plot 3: Spatial distribution
    ax = axes[0, 2]
    spatial_coords = viz_data.get('spatial_coordinates', [])
    if spatial_coords:
        x_coords = [coord[0] for coord in spatial_coords]
        y_coords = [coord[1] for coord in spatial_coords]
        ax.scatter(x_coords, y_coords, alpha=0.6, s=10, c='blue')
    ax.set_title('Neural Population Spatial Distribution', fontweight='bold')
    ax.set_xlabel('X Coordinate')
    ax.set_ylabel('Y Coordinate')
    ax.grid(True, alpha=0.3)
    
    # Plot 4: Connectivity matrix visualization
    ax = axes[1, 0]
    connectivity = viz_data.get('connectivity_matrix', [])
    if connectivity:
        # Display a subset of connectivity matrix
        subset_size = min(20, len(connectivity))
        for i in range(subset_size):
            for j in range(subset_size):
                if connectivity[i][j] > 0.5:
                    ax.plot([i, j], [j, i], 'r-', alpha=0.3, linewidth=1)
        ax.set_title('Connectivity Matrix (Selected Regions)', fontweight='bold')
        ax.set_xlabel('Source Region')
        ax.set_ylabel('Target Region')
        ax.set_xlim(-0.5, subset_size-0.5)
        ax.set_ylim(-0.5, subset_size-0.5)
        ax.grid(True, alpha=0.3)
    
    # Plot 5: Time series analysis
    ax = axes[1, 1]
    time_series = viz_data.get('time_series', {})
    if 'firing_rate' in time_series:
        ax.plot(time_series['firing_rate'], label='Firing Rate', linewidth=2)
        ax.set_title('Time Series Analysis', fontweight='bold')
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Value')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    # Plot 6: Metrics over time
    ax = axes[1, 2]
    metrics_over_time = viz_data.get('metrics_over_time', {})
    if metrics_over_time:
        for metric_name, values in metrics_over_time.items():
            ax.plot(values, label=metric_name, linewidth=2)
        ax.set_title('Metrics Evolution', fontweight='bold')
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Metric Value')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('advanced_visualization_suite.png', dpi=150, bbox_inches='tight')
    print("✓ Advanced visualization suite saved to 'advanced_visualization_suite.png'")
    plt.close()
    
    return viz_data

def example_pattern_analysis():
    """Example 3: Pattern Recognition and Classification"""
    print("\n" + "=" * 80)
    print("Example 3: Pattern Recognition and Classification")
    print("=" * 80)
    
    # Create brain for pattern analysis
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation to generate rich data
    print("Running simulation for pattern analysis...")
    for i in range(1000):
        brain.step(i)
    
    # Detect patterns using advanced pattern recognition
    print("Performing pattern detection and classification...")
    
    pattern_types = ["burst_patterns", "synchrony_patterns", "frequency_patterns", "network_dynamics"]
    pattern_results = {}
    
    for pattern_type in pattern_types:
        patterns = pynlm.detectPatterns(brain, pattern_type, confidence_threshold=0.7)
        pattern_results[pattern_type] = patterns
        
        print(f"\n{pattern_type.upper()} Analysis:")
        print(f"  Detected patterns: {len(patterns.get('detected_patterns', []))}")
        print(f"  Dominant pattern: {patterns.get('dominant_pattern', 'None')}")
        print(f"  Pattern consistency: {patterns.get('pattern_consistency', 0):.3f}")
        
        # Show pattern details
        for i, (pattern_name, confidence) in enumerate(zip(
            patterns.get('detected_patterns', []),
            patterns.get('pattern_confidence_scores', []))):
            timestamps = patterns.get('pattern_timestamps', {}).get(pattern_name, [])
            print(f"    - {pattern_name}: confidence={confidence:.3f}, occurrences={len(timestamps)}")
    
    # Create pattern visualization
    fig, axes = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle('Pattern Recognition and Classification Results', fontsize=16, fontweight='bold')
    
    # Plot 1: Pattern detection results
    ax = axes[0, 0]
    pattern_names = []
    confidences = []
    all_timestamps = []
    
    for pattern_type, patterns in pattern_results.items():
        detected = patterns.get('detected_patterns', [])
        confidences_found = patterns.get('pattern_confidence_scores', [])
        timestamps_found = patterns.get('pattern_timestamps', {})
        
        for i, (pattern, conf) in enumerate(zip(detected, confidences_found)):
            pattern_names.append(f"{pattern_type}: {pattern}")
            confidences.append(conf)
            all_timestamps.extend(timestamps_found.get(pattern, []))
    
    if pattern_names:
        colors = plt.cm.RdYlBu(np.linspace(0, 1, len(pattern_names)))
        bars = ax.bar(range(len(pattern_names)), confidences, color=colors, alpha=0.8)
        ax.set_title('Pattern Detection Confidence Scores', fontweight='bold')
        ax.set_xlabel('Pattern Instance')
        ax.set_ylabel('Confidence Score')
        ax.set_xticks(range(0, len(pattern_names), max(1, len(pattern_names) // 10)))
        ax.set_xticklabels([p[:20] + '...' if len(p) > 20 else p for p in pattern_names[::max(1, len(pattern_names) // 10)]], rotation=45)
        ax.grid(True, alpha=0.3)
    
    # Plot 2: Pattern timeline
    ax = axes[0, 1]
    if all_timestamps:
        # Create timeline visualization
        time_bins = np.arange(0, max(all_timestamps) + 1, 100)
        hist, bins, _ = ax.hist(all_timestamps, bins=time_bins, alpha=0.7, color='steelblue')
        ax.set_title('Pattern Occurrence Timeline', fontweight='bold')
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Number of Patterns')
        ax.grid(True, alpha=0.3)
    
    # Plot 3: Pattern frequency distribution
    ax = axes[1, 0]
    if pattern_names:
        pattern_counts = {}
        for pattern_name in pattern_names:
            pattern_counts[pattern_name] = pattern_counts.get(pattern_name, 0) + 1
        
        sorted_patterns = sorted(pattern_counts.items(), key=lambda x: x[1], reverse=True)
        pattern_labels = [p[0][:30] + '...' if len(p[0]) > 30 else p[0] for p in sorted_patterns[0:10]]
        pattern_values = [p[1] for p in sorted_patterns[0:10]]
        
        bars = ax.barh(range(len(pattern_values)), pattern_values, color='lightcoral', alpha=0.8)
        ax.set_title('Pattern Frequency Distribution', fontweight='bold')
        ax.set_xlabel('Frequency')
        ax.set_ylabel('Pattern')
        ax.set_yticks(range(len(pattern_values)))
        ax.set_yticklabels(pattern_labels)
        ax.invert_yaxis()
        ax.grid(True, alpha=0.3)
    
    # Plot 4: Pattern consistency radar chart
    ax = axes[1, 1]
    if pattern_names:
        # Create radar chart for pattern characteristics
        categories = ['Consistency', 'Dominance', 'Frequency', 'Duration', 'Complexity']
        num_categories = len(categories)
        
        angles = np.linspace(0, 2 * np.pi, num_categories, endpoint=False).tolist()
        angles += angles[:1]  # Complete the circle
        
        for i, (pattern_type, patterns) in enumerate(pattern_results.items()):
            if patterns.get('dominant_pattern'):
                values = [
                    patterns.get('pattern_consistency', 0.5),
                    0.8,  # Dominance (assumed)
                    0.6,  # Frequency (assumed)
                    0.7,  # Duration (assumed)
                    0.5   # Complexity (assumed)
                ]
                
                values += values[:1]  # Complete the circle
                ax.plot(angles, values, 'o-', linewidth=2, label=pattern_type, alpha=0.8)
                ax.fill(angles, values, alpha=0.25)
        
        ax.set_ylim(0, 1)
        ax.set_xticks(angles[:-1])
        ax.set_xticklabels(categories)
        ax.set_title('Pattern Characteristic Radar Chart', fontweight='bold')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('pattern_analysis_results.png', dpi=150, bbox_inches='tight')
    print("✓ Pattern analysis results saved to 'pattern_analysis_results.png'")
    plt.close()
    
    return pattern_results

def example_configuration_optimization():
    """Example 4: Configuration Optimization and Templates"""
    print("\n" + "=" * 80)
    print("Example 4: Configuration Optimization and Templates")
    print("=" * 80)
    
    # Create configuration template
    base_config = pynlm.createDefaultConfig()
    base_config.set("brain.neuron_count", 1000)
    base_config.set("brain.synapse_density", 0.1)
    
    # Define variations for different use cases
    variations = {
        "high_performance": lambda config: (
            config.set("brain.neuron_count", 2000),
            config.set("brain.synapse_density", 0.05),
            config.set("plasticity.stdp.enable", True)
        ),
        "balanced": lambda config: (
            config.set("brain.neuron_count", 1500),
            config.set("brain.synapse_density", 0.1),
            config.set("plasticity.hebbian.enable", True)
        ),
        "exploratory": lambda config: (
            config.set("brain.neuron_count", 800),
            config.set("brain.synapse_density", 0.2),
            config.set("neuromod.curiosity.enable", True)
        )
    }
    
    parameter_ranges = {
        "brain.neuron_count": (500, 3000),
        "brain.synapse_density": (0.01, 0.5),
        "plasticity.stdp.learning_rate": (0.001, 0.1),
        "neuromod.curiosity.threshold": (0.1, 1.0)
    }
    
    # Create configuration template
    config_template = pynlm.createConfigurationTemplate(
        "nlm_advanced_template",
        base_config,
        variations,
        parameter_ranges
    )
    
    print(f"Created configuration template: {config_template.get('name', 'Unknown')}")
    print(f"Base configuration neuron count: {config_template.get('base_config', {}).get('brain', {}).get('neuron_count', 'Unknown')}")
    print(f"Available variations: {len(variations)}")
    print(f"Parameter ranges defined: {len(parameter_ranges)}")
    
    # Optimize parameters using genetic algorithm
    print("\nRunning parameter optimization using genetic algorithm...")
    
    brain_template = pynlm.createBrain(base_config)
    
    optimized_config, optimization_metrics = pynlm.optimizeParameters(
        brain_template,
        parameter_ranges,
        generations=50,
        mutation_rate=0.1,
        crossover_rate=0.8
    )
    
    print(f"\nOptimization Results:")
    print(f"  Generations completed: {optimization_metrics.get('generations', 'Unknown')}")
    print(f"  Best fitness score: {optimization_metrics.get('best_fitness', 0):.4f}")
    print(f"  Mutation rate: {optimization_metrics.get('mutation_rate', 0):.2f}")
    print(f"  Crossover rate: {optimization_metrics.get('crossover_rate', 0):.2f}")
    
    # Apply optimized configuration
    optimized_brain = pynlm.createBrain(optimized_config)
    optimized_brain.initialize()
    
    # Run test with optimized configuration
    print("\nRunning performance test with optimized configuration...")
    start_time = time.time()
    
    for i in range(500):
        optimized_brain.step(i)
    
    end_time = time.time()
    execution_time = end_time - start_time
    
    final_metrics = {
        "total_neurons": optimized_brain.getTotalNeuronCount(),
        "total_synapses": optimized_brain.getTotalSynapseCount(),
        "firing_neurons": optimized_brain.getFiringNeuronCount(),
        "total_spikes": optimized_brain.getTotalSpikeCount(),
        "average_firing_rate": optimized_brain.getAverageFiringRate(),
        "execution_time_seconds": execution_time,
        "steps_per_second": 500 / execution_time
    }
    
    print(f"\nOptimized Configuration Performance:")
    print(f"  Total neurons: {final_metrics['total_neurons']}")
    print(f"  Total synapses: {final_metrics['total_synapses']}")
    print(f"  Firing neurons: {final_metrics['firing_neurons']}")
    print(f"  Total spikes: {final_metrics['total_spikes']}")
    print(f"  Average firing rate: {final_metrics['average_firing_rate']:.2f} Hz")
    print(f"  Execution time: {final_metrics['execution_time_seconds']:.2f}s")
    print(f"  Steps per second: {final_metrics['steps_per_second']:.1f}")
    
    # Save optimization results
    optimization_results = {
        "timestamp": datetime.now().isoformat(),
        "template_name": config_template.get('name', 'Unknown'),
        "parameter_ranges": parameter_ranges,
        "variations": list(variations.keys()),
        "optimization_metrics": optimization_metrics,
        "final_metrics": final_metrics,
        "optimized_config": optimized_config
    }
    
    with open("configuration_optimization_results.json", "w") as f:
        json.dump(optimization_results, f, indent=2)
    
    print("✓ Configuration optimization results saved to 'configuration_optimization_results.json'")
    
    return optimization_results

def example_advanced_learning_features():
    """Example 5: Advanced Learning Features"""
    print("\n" + "=" * 80)
    print("Example 5: Advanced Learning Features")
    print("=" * 80)
    
    # Create meta-learning framework
    print("Creating meta-learning framework...")
    meta_framework = pynlm.createMetaLearningFramework(
        max_meta_iterations=30,
        meta_learning_rate=0.01,
        initial_learning_rate=0.1
    )
    
    print(f"Created meta-learning framework with:")
    print(f"  Max meta-iterations: {meta_framework.get('max_meta_iterations', 'Unknown')}")
    print(f"  Meta-learning rate: {meta_framework.get('meta_learning_rate', 'Unknown')}")
    print(f"  Initial learning rate: {meta_framework.get('initial_learning_rate', 'Unknown')}")
    print(f"  Learned configurations: {len(meta_framework.get('learned_configurations', []))}")
    
    # Create curriculum learning stages
    print("\nDesigning curriculum learning stages...")
    
    curriculum_stages = [
        {
            "stage_name": "sensory_basic",
            "learning_inputs": [[0.1], [0.2], [0.3], [0.4], [0.5]],
            "min_steps_required": 100,
            "completion_criteria": lambda brain: brain.getAverageFiringRate() > 0.5
        },
        {
            "stage_name": "motor_coordination",
            "learning_inputs": [[0.6], [0.7], [0.8], [0.9], [1.0]],
            "min_steps_required": 200,
            "completion_criteria": lambda brain: brain.getTotalSpikeCount() > 500
        },
        {
            "stage_name": "cognitive_reasoning",
            "learning_inputs": [[0.2, 0.3], [0.4, 0.5], [0.6, 0.7]],
            "min_steps_required": 300,
            "completion_criteria": lambda brain: brain.getExcitationInhibitionRatio() > 1.0
        }
    ]
    
    # Run curriculum learning
    print("\nRunning curriculum learning...")
    
    base_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    curriculum_results = pynlm.curriculumLearning(base_brain, curriculum_stages)
    
    print(f"Completed {len(curriculum_results)} curriculum stages")
    
    for i, stage_result in enumerate(curriculum_results):
        print(f"\nStage {i+1} ({stage_result.get('stage_name', f'Stage {i+1}')}):")
        print(f"  Total steps completed: {stage_result.get('total_steps', 'Unknown')}")
        print(f"  Execution time: {stage_result.get('execution_time', 'Unknown'):.2f}s")
        print(f"  Average firing rate: {stage_result.get('metrics', [{}])[0].get('firing_rate', 0):.3f}")
    
    # Implement active learning
    print("\nImplementing active learning strategy...")
    
    active_learning_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    active_query = pynlm.activeLearning(
        active_learning_brain,
        query_input=[0.5, 0.5, 0.5],
        uncertainty_threshold=0.6,
        max_queries=5
    )
    
    print(f"Active learning results:")
    print(f"  Number of selected queries: {len(active_query.get('selected_queries', []))}")
    print(f"  Uncertainty threshold: {active_query.get('uncertainty_threshold', 'Unknown')}")
    print(f"  Max queries: {active_query.get('max_queries', 'Unknown')}")
    
    for i, (query, uncertainty) in enumerate(zip(
        active_query.get('selected_queries', []),
        [q[1] for q in active_query.get('selected_queries', [])]
    )):
        print(f"    Query {i+1}: input={query[0]}, uncertainty={uncertainty:.3f}")
    
    # Run the selected queries
    print("\nExecuting selected queries...")
    for query, _ in active_query.get('selected_queries', []):
        active_learning_brain.receiveSensoryInput(query)
        active_learning_brain.step(0)
    
    final_active_metrics = {
        "total_spikes": active_learning_brain.getTotalSpikeCount(),
        "firing_neurons": active_learning_brain.getFiringNeuronCount(),
        "average_firing_rate": active_learning_brain.getAverageFiringRate()
    }
    
    print(f"\nActive learning final metrics:")
    print(f"  Total spikes: {final_active_metrics['total_spikes']}")
    print(f"  Firing neurons: {final_active_metrics['firing_neurons']}")
    print(f"  Average firing rate: {final_active_metrics['average_firing_rate']:.3f} Hz")
    
    # Save advanced learning results
    learning_results = {
        "timestamp": datetime.now().isoformat(),
        "meta_framework": meta_framework,
        "curriculum_stages": len(curriculum_stages),
        "curriculum_results": curriculum_results,
        "active_learning": active_query,
        "final_metrics": final_active_metrics
    }
    
    with open("advanced_learning_results.json", "w") as f:
        json.dump(learning_results, f, indent=2)
    
    print("✓ Advanced learning results saved to 'advanced_learning_results.json'")
    
    return learning_results

def example_integration_and_workflow_tools():
    """Example 6: Integration and Workflow Tools"""
    print("\n" + "=" * 80)
    print("Example 6: Integration and Workflow Tools")
    print("=" * 80)
    
    # Multi-environment testing
    print("Setting up multi-environment testing...")
    
    # Create brains for different environments
    environments = [
        {"name": "stable_environment", "params": {"noise_level": 0.1, "reward_scale": 1.0}},
        {"name": "dynamic_environment", "params": {"noise_level": 0.5, "reward_scale": 2.0}},
        {"name": "challenging_environment", "params": {"noise_level": 1.0, "reward_scale": 0.5}}
    ]
    
    # Create brains for each environment
    brains = []
    for env in environments:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("neuromod.curiosity.enable", True)
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append((env["name"], brain, env["params"]))
    
    print(f"Created {len(brains)} brains for multi-environment testing")
    
    # Run multi-environment testing
    environment_names = [env["name"] for env in environments]
    environment_params = [env["params"] for env in environments]
    
    multi_env_results = pynlm.multiEnvironmentTesting(brains, environment_names, environment_params)
    
    print(f"\nMulti-environment testing results:")
    print(f"  Environments tested: {len(multi_env_results.get('environment_results', {}))}")
    print(f"  Transfer success rate: {multi_env_results.get('transfer_success_rate', 0):.3f}")
    print(f"  Successful transfers: {len(multi_env_results.get('successful_transfers', []))}")
    print(f"  Failed transfers: {len(multi_env_results.get('failed_transfers', []))}")
    
    for env_name, result in multi_env_results.get('environment_results', {}).items():
        print(f"    {env_name}: {result.get('total_steps', 'Unknown')} steps, "
              f"{result.get('execution_time', 'Unknown'):.2f}s execution")
    
    # Skill transfer across tasks
    print("\nTesting skill transfer across tasks...")
    
    # Create source brain with learned skills
    source_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    source_brain.initialize()
    
    # Run source brain to learn skills
    for i in range(200):
        source_brain.step(i)
    
    # Create target brains
    target_brains = []
    for i in range(3):
        target_config = pynlm.createDefaultConfig()
        target_config.set("brain.neuron_count", 500)
        target_brain = pynlm.createBrain(target_config)
        target_brain.initialize()
        target_brains.append(target_brain)
    
    skill_patterns = [[0.3, 0.7], [0.6, 0.4], [0.8, 0.2], [0.4, 0.6]]
    
    # Perform skill transfer
    skill_transfer_results = pynlm.skillTransfer(source_brain, target_brains, skill_patterns)
    
    print(f"Skill transfer results:")
    print(f"  Source brain performance: {source_brain.getTotalSpikeCount()} spikes")
    print(f"  Number of target brains: {len(skill_transfer_results)}")
    
    for i, transfer_result in enumerate(skill_transfer_results):
        print(f"    Target brain {i+1}: efficiency={transfer_result.get('transfer_efficiency', 0):.3f}, "
              f"success={transfer_result.get('adaptation_metrics', {}).get('skill_transfer_success', 0)}")
    
    # Create comprehensive workflow visualization
    print("\nCreating comprehensive workflow analysis...")
    
    workflow_results = {
        "timestamp": datetime.now().isoformat(),
        "multi_environment": multi_env_results,
        "skill_transfer": skill_transfer_results,
        "source_brain_performance": {
            "total_spikes": source_brain.getTotalSpikeCount(),
            "firing_neurons": source_brain.getFiringNeuronCount(),
            "average_firing_rate": source_brain.getAverageFiringRate()
        },
        "target_brains_performance": [
            {
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_firing_rate": brain.getAverageFiringRate()
            }
            for brain in target_brains
        ]
    }
    
    with open("integration_workflow_results.json", "w") as f:
        json.dump(workflow_results, f, indent=2)
    
    print("✓ Integration workflow results saved to 'integration_workflow_results.json'")
    
    return workflow_results

def main():
    """Run all advanced examples"""
    print("NLM Advanced Examples Suite")
    print("=" * 80)
    print("This script demonstrates all the new advanced features in NLM.")
    print("Each example showcases different capabilities for advanced users.")
    print()
    
    # Run all examples
    try:
        # Example 1: Advanced Simulation Control
        example_advanced_simulation_control()
        
        # Example 2: Advanced Visualization
        example_advanced_visualization()
        
        # Example 3: Pattern Analysis
        example_pattern_analysis()
        
        # Example 4: Configuration Optimization
        example_configuration_optimization()
        
        # Example 5: Advanced Learning Features
        example_advanced_learning_features()
        
        # Example 6: Integration and Workflow Tools
        example_integration_and_workflow_tools()
        
        print("\n" + "=" * 80)
        print("ALL ADVANCED EXAMPLES COMPLETED SUCCESSFULLY!")
        print("=" * 80)
        print("\nKey features demonstrated:")
        print("1. ✓ Advanced batch simulation control and parallel execution")
        print("2. ✓ Comprehensive visualization tools for neural networks")
        print("3. ✓ Pattern recognition and classification capabilities")
        print("4. ✓ Configuration optimization and template management")
        print("5. ✓ Advanced learning features (meta-learning, curriculum learning)")
        print("6. ✓ Integration tools for multi-environment testing")
        print("\nCheck the output files and visualizations in the current directory.")
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    return True

if __name__ == "__main__":
    main()