"""
Visualization and results analysis examples.

This example demonstrates comprehensive visualization capabilities for
NLM experiment results including statistical analysis, comparison charts,
and interactive visualizations.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from pynlm import (
    create_default_config, create_brain, create_simple_world, create_agent_brain
)
from pynlm.experiments import run_experiment, ExperimentResult
from pynlm.visualization import (
    ResultsVisualizer, VisualizationConfig, plot_experiment_comparison,
    plot_statistical_analysis, create_research_dashboard, save_visualization_results
)
from pynlm.research import ScientificExperimentDesigner
import time
import random
import numpy as np
import matplotlib.pyplot as plt

def visualization_examples():
    """Run visualization examples."""
    print("=== Visualization and Results Analysis Examples ===")
    
    # Create some dummy experiment results for demonstration
    print("Creating example experiment results...")
    
    # Create example experiment results
    example_results = []
    
    # Generate diverse results for different experiments
    experiment_names = ['exp_baseline', 'exp_variant1', 'exp_variant2', 'exp_optimized']
    seeds = [42, 123, 456, 789]
    
    for i, (name, seed) in enumerate(zip(experiment_names, seeds)):
        # Generate realistic experiment results
        base_performance = np.random.normal(0.5, 0.1)
        performance_values = np.random.normal(base_performance, 0.05, 20)
        spike_counts = np.random.poisson(800 + i * 200)
        
        result = ExperimentResult(
            experiment_name=name,
            seed=seed,
            steps_completed=200,
            final_spike_count=spike_counts,
            average_firing_rate=np.mean(performance_values),
            performance_metrics={
                'simulation_time': 45.2 + i * 10,
                'memory_usage': np.random.uniform(100, 500),
                'cpu_usage': np.random.uniform(20, 80),
                'total_steps': 200
            },
            custom_metrics={
                'accuracy': base_performance,
                'learning_speed': np.random.uniform(0.3, 0.8),
                'convergence_rate': np.random.uniform(0.4, 0.9)
            },
            success=True,
            timestamp=time.time()
        )
        example_results.append(result)
    
    # Add some failed results for robustness
    failed_result = ExperimentResult(
        experiment_name='exp_failed',
        seed=999,
        steps_completed=0,
        final_spike_count=0,
        average_firing_rate=0.0,
        performance_metrics={},
        success=False,
        error_message='Simulation timeout',
        timestamp=time.time()
    )
    example_results.append(failed_result)
    
    print(f"Created {len(example_results)} example experiment results")
    
    # Step 1: Basic visualization setup
    print("\n=== 1. Basic Visualization Setup ===")
    
    config = VisualizationConfig(
        figure_size=(12, 8),
        style='seaborn-v0_8',
        save_plots=True,
        output_format='png',
        dpi=150,
        color_scheme='scientific'
    )
    
    visualizer = ResultsVisualizer(config)
    print(f"Initialized visualizer with {config.figure_size} size, {config.style} style")
    
    # Step 2: Time series visualization
    print("\n=== 2. Time Series Visualization ===")
    
    # Create time series data
    time_series_data = {}
    time_steps = list(range(100))
    
    for result in example_results:
        if result.success:
            # Generate synthetic time series data
            baseline = np.random.normal(0.5, 0.1, 100)
            trend = np.linspace(0, 0.2, 100)
            noise = np.random.normal(0, 0.05, 100)
            
            time_series_data[result.experiment_name] = baseline + trend + noise
    
    # Plot time series
    if time_series_data:
        timeseries_file = visualizer.plot_time_series(
            data=time_series_data,
            x_label='Simulation Steps',
            title='Experiment Performance Over Time',
            save_path='./visualization_results'
        )
        print(f"Time series visualization saved: {timeseries_file}")
    
    # Step 3: Scatter comparison
    print("\n=== 3. Scatter Comparison ===")
    
    # Prepare scatter data
    scatter_data = {}
    for result in example_results:
        if result.success:
            scatter_data[result.experiment_name] = [
                result.average_firing_rate,
                float(result.final_spike_count),
                result.custom_metrics.get('accuracy', 0.0)
            ]
    
    if scatter_data:
        scatter_file = visualizer.plot_scatter_comparison(
            data=scatter_data,
            x_label='Experiment Name',
            y_label='Performance Metrics',
            title='Experiment Performance Comparison',
            save_path='./visualization_results'
        )
        print(f"Scatter comparison saved: {scatter_file}")
    
    # Step 4: Histogram
    print("\n=== 4. Histogram Visualization ===")
    
    # Collect performance values
    performance_values = []
    for result in example_results:
        if result.success:
            performance_values.append(result.average_firing_rate)
    
    if performance_values:
        hist_file = visualizer.plot_histogram(
            data=performance_values,
            bins=15,
            title='Distribution of Experiment Performance',
            save_path='./visualization_results'
        )
        print(f"Histogram saved: {hist_file}")
    
    # Step 5: Box plot
    print("\n=== 5. Box Plot ===")
    
    # Prepare box plot data
    boxplot_data = {}
    for result in example_results:
        if result.success:
            boxplot_data[result.experiment_name] = [result.average_firing_rate]
    
    if boxplot_data:
        boxplot_file = visualizer.plot_box_plot(
            data=boxplot_data,
            title='Experiment Performance Box Plot',
            save_path='./visualization_results'
        )
        print(f"Box plot saved: {boxplot_file}")
    
    # Step 6: Comparison chart
    print("\n=== 6. Experiment Comparison Chart ===")
    
    metrics = ['average_firing_rate', 'final_spike_count', 'accuracy']
    
    comparison_file = visualizer.plot_comparison_chart(
        results=example_results,
        metrics=metrics,
        title='Multi-Metric Experiment Comparison',
        save_path='./visualization_results'
    )
    print(f"Comparison chart saved: {comparison_file}")
    
    # Step 7: Heatmap
    print("\n=== 7. Heatmap Visualization ===")
    
    heatmap_file = visualizer.plot_results_heatmap(
        results=example_results,
        metrics=metrics,
        title='Experiment Performance Heatmap',
        save_path='./visualization_results'
    )
    print(f"Heatmap saved: {heatmap_file}")
    
    # Step 8: Statistical analysis
    print("\n=== 8. Statistical Analysis ===")
    
    # Perform statistical analysis for each metric
    for metric in metrics:
        stat_results = plot_statistical_analysis(example_results, metric)
        print(f"Statistical analysis for '{metric}':")
        print(f"  Mean: {stat_results.mean:.3f}")
        print(f"  Std: {stat_results.std:.3f}")
        print(f"  Min: {stat_results.min:.3f}")
        print(f"  Max: {stat_results.max:.3f}")
        print(f"  P-value: Would require t-test")
    
    # Step 9: Research dashboard
    print("\n=== 9. Research Dashboard ===")
    
    # Create interactive dashboard
    dashboard = visualizer.create_dashboard(
        results=example_results,
        metrics=metrics
    )
    
    dashboard_file = './visualization_results/research_dashboard.html'
    dashboard.write_html(dashboard_file)
    print(f"Interactive dashboard saved: {dashboard_file}")
    
    # Step 10: Export results
    print("\n=== 10. Results Export ===")
    
    export_file = './visualization_results/experiment_results.json'
    success = visualizer.export_results(example_results, export_file, 'json')
    if success:
        print(f"Results exported: {export_file}")
    
    # Step 11: Save all plots
    print("\n=== 11. Save All Plots ===")
    
    save_success = visualizer.save_all_plots('./visualization_results')
    if save_success:
        print("All plots saved successfully")
    
    # Create directory if it doesn't exist
    os.makedirs('./visualization_results', exist_ok=True)
    
    # Step 12: Generate comprehensive report
    print("\n=== 12. Comprehensive Report ===")
    
    # Generate summary statistics
    total_experiments = len(example_results)
    successful_experiments = sum(1 for r in example_results if r.success)
    failed_experiments = total_experiments - successful_experiments
    
    # Calculate aggregate statistics
    all_performance = []
    for result in example_results:
        if result.success:
            all_performance.extend([
                result.average_firing_rate,
                float(result.final_spike_count),
                result.custom_metrics.get('accuracy', 0.0)
            ])
    
    report = {
        'visualization_summary': {
            'total_experiments': total_experiments,
            'successful_experiments': successful_experiments,
            'failed_experiments': failed_experiments,
            'success_rate': successful_experiments / total_experiments * 100,
            'output_directory': './visualization_results',
            'files_generated': [
                'time_series_1.png',
                'scatter_1.png',
                'histogram_1.png',
                'box_plot_1.png',
                'comparison_1.png',
                'heatmap_1.png',
                'research_dashboard.html',
                'experiment_results.json'
            ]
        },
        'performance_statistics': {
            'mean_firing_rate': np.mean(all_performance) if all_performance else 0,
            'std_firing_rate': np.std(all_performance) if all_performance else 0,
            'min_firing_rate': np.min(all_performance) if all_performance else 0,
            'max_firing_rate': np.max(all_performance) if all_performance else 0,
            'coefficient_of_variation': np.std(all_performance) / np.mean(all_performance) if all_performance and np.mean(all_performance) > 0 else 0
        },
        'recommendations': [
            'Focus on experiments with firing rates above 0.6',
            'Investigate the failed experiment for potential issues',
            'Consider multi-seed experiments for statistical validation',
            'Use parameter sweeps to optimize exploration-exploitation balance'
        ],
        'next_steps': [
            'Run parameter sweep optimization',
            'Perform statistical significance testing',
            'Generate interactive visualizations',
            'Create comparative analysis reports'
        ]
    }
    
    # Save report
    report_file = './visualization_results/comprehensive_report.json'
    with open(report_file, 'w') as f:
        import json
        json.dump(report, f, indent=2, default=str)
    
    print(f"Comprehensive report saved: {report_file}")
    
    # Print final summary
    print(f"\n=== Visualization Summary ===")
    print(f"Total experiments analyzed: {total_experiments}")
    print(f"Successful experiments: {successful_experiments}")
    print(f"Failed experiments: {failed_experiments}")
    print(f"Success rate: {report['visualization_summary']['success_rate']:.1f}%")
    print(f"Performance mean: {report['performance_statistics']['mean_firing_rate']:.3f}")
    print(f"Performance std: {report['performance_statistics']['std_firing_rate']:.3f}")
    print(f"Output directory: ./visualization_results/")
    
    print(f"\nVisualization examples completed successfully!")
    print(f"All results and visualizations are available in ./visualization_results/")
    
    return example_results

if __name__ == "__main__":
    visualization_examples()