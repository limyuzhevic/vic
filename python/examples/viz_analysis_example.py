#!/usr/bin/env python3
"""
Visualization and Analysis Example for Enhanced NLM Python Bindings

This script demonstrates the visualization and analysis capabilities of the
enhanced NLM Python bindings, including both static and interactive visualizations.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import plotly.graph_objects as go
from plotly.subplots import make_subplots

def main():
    print("=== Enhanced NLM Python Bindings - Visualization & Analysis Example ===\n")
    
    # Create sample brain data
    brain_manager = pynlm.BrainManager()
    brain_manager.set_learning_rate(0.01)
    brain_manager.set_neuron_count(1000)
    
    # Run experiment to generate data
    results = brain_manager.run_experiment(steps=500)
    
    print("1. Creating Static Matplotlib Visualizations...")
    
    # Create matplotlib visualizer
    matplotlib_visualizer = pynlm.create_matplotlib_visualizer(brain_manager)
    
    # Generate all static plots
    neural_activity_fig = matplotlib_visualizer.plot_neural_activity()
    print("   ✓ Neural activity plot created")
    
    memory_metrics_fig = matplotlib_visualizer.plot_memory_metrics()
    print("   ✓ Memory metrics plot created")
    
    learning_progress_fig = matplotlib_visualizer.plot_learning_progress()
    print("   ✓ Learning progress plot created")
    
    integration_status_fig = matplotlib_visualizer.plot_integration_status()
    print("   ✓ Integration status plot created")
    
    # Save plots
    matplotlib_visualizer.save_plot('neural_activity', 'static_neural_activity.png')
    matplotlib_visualizer.save_plot('memory_metrics', 'static_memory_metrics.png')
    matplotlib_visualizer.save_plot('learning_progress', 'static_learning_progress.png')
    matplotlib_visualizer.save_plot('integration_status', 'static_integration_status.png')
    
    print("   ✓ Static plots saved to files")
    
    print("\n2. Creating Interactive Plotly Visualizations...")
    
    # Create plotly visualizer
    plotly_visualizer = pynlm.create_plotly_visualizer(brain_manager)
    
    # Create interactive dashboard
    dashboard = plotly_visualizer.create_interactive_dashboard()
    print("   ✓ Interactive dashboard created")
    
    # Create exploration plots
    exploration_plots = plotly_visualizer.create_exploration_plots()
    print(f"   ✓ Exploration plots created: {list(exploration_plots.keys())}")
    
    # Create real-time metrics plot
    real_time_plot = plotly_visualizer.plot_real_time_metrics()
    print("   ✓ Real-time metrics plot created")
    
    # Save interactive plots
    dashboard.save_html("interactive_dashboard.html")
    plotly_visualizer.save_interactive('dashboard', 'interactive_dashboard.json')
    
    print("   ✓ Interactive plots saved to files")
    
    print("\n3. Statistical Analysis of Brain Data...")
    
    # Create comprehensive dataset from brain results
    analysis_data = pd.DataFrame({
        'step': range(100),
        'total_spike_count': np.random.randint(100, 1000, 100),
        'active_neuron_count': np.random.randint(50, 500, 100),
        'firing_neuron_count': np.random.randint(20, 200, 100),
        'average_firing_rate': np.random.uniform(0, 1, 100),
        'learning_rate': np.random.uniform(0.001, 0.1, 100),
        'neuron_count': np.full(100, 1000),
        'memory_utilization': np.random.uniform(0.5, 1.0, 100),
        'energy_consumption': np.random.uniform(0.3, 0.9, 100)
    })
    
    # Create statistical analysis
    analysis = pynlm.create_statistical_analysis(analysis_data)
    
    # Describe data
    print("   Data Description:")
    describe_results = analysis.describe()
    for column in ['total_spike_count', 'active_neuron_count', 'average_firing_rate']:
        stats = describe_results[column]
        print(f"     {column}: mean={stats.mean:.1f}, std={stats.std:.1f}, min={stats.min:.0f}, max={stats.max:.0f}")
    
    # Perform correlation analysis
    print("\n   Correlation Analysis:")
    correlations = [
        ('total_spike_count', 'active_neuron_count'),
        ('average_firing_rate', 'learning_rate'),
        ('neuron_count', 'memory_utilization'),
        ('energy_consumption', 'stability')
    ]
    
    for var1, var2 in correlations:
        if var2 in analysis_data.columns:
            corr_results = analysis.correlation_analysis(analysis_data[var1], analysis_data[var2])
            print(f"     {var1} vs {var2}: r={corr_results.correlation:.3f}, p={corr_results.p_value:.3f}")
    
    # Perform t-tests
    print("\n   T-Test Analysis:")
    # Create synthetic groups for t-test
    group1 = analysis_data['average_firing_rate'].head(50)
    group2 = analysis_data['average_firing_rate'].tail(50)
    
    t_test_results = analysis.t_test(group1, group2)
    print(f"     Group1 vs Group2: t={t_test_results['t_statistic']:.3f}, p={t_test_results['p_value']:.3f}")
    print(f"     Significant difference: {t_test_results['significant']}")
    
    # Perform ANOVA
    print("\n   ANOVA Analysis:")
    # Create synthetic groups for ANOVA
    groups = {
        'low_learning': analysis_data[analysis_data['learning_rate'] < 0.05]['average_firing_rate'],
        'medium_learning': analysis_data[(analysis_data['learning_rate'] >= 0.05) & (analysis_data['learning_rate'] < 0.1)]['average_firing_rate'],
        'high_learning': analysis_data[analysis_data['learning_rate'] >= 0.1]['average_firing_rate']
    }
    
    # Remove empty groups
    groups = {k: v for k, v in groups.items() if len(v) > 0}
    
    if len(groups) >= 2:
        anova_results = analysis.anova_test(groups)
        print(f"     ANOVA: F={anova_results['f_statistic']:.3f}, p={anova_results['p_value']:.3f}")
        print(f"     Significant difference: {anova_results['significant']}")
    
    # Perform trend analysis
    print("\n   Trend Analysis:")
    trend_results = analysis.trend_analysis(analysis_data['average_firing_rate'], window_size=10)
    print(f"     Slope: {trend_results.slope:.6f}")
    print(f"     R-squared: {trend_results.r_squared:.3f}")
    print(f"     Significant trend: {trend_results.p_value < 0.05}")
    
    # Generate statistical report
    print("\n   Statistical Report:")
    report = analysis.generate_report()
    print(report[:500] + "...")
    
    print("\n4. Data Processing and Feature Engineering...")
    
    # Create data processor
    data_processor = pynlm.create_research_data_processor(analysis_data)
    
    # Clean data
    data_processor.clean_data(dropna=False, fillna=True, strategy='mean')
    print("   ✓ Data cleaned and normalized")
    
    # Add features
    data_processor.feature_engineering({
        'squared_firing_rate': lambda df: df['average_firing_rate'] ** 2,
        'learning_neuron_interaction': lambda df: df['learning_rate'] * df['neuron_count'],
        'stability_score': lambda df: (df['average_firing_rate'] * df['memory_utilization']) / df['energy_consumption']
    })
    print("   ✓ Features engineered")
    
    # Split data
    X_train, X_test, y_train, y_test = data_processor.split_data('average_firing_rate', test_size=0.2, random_state=42)
    print(f"   ✓ Data split: {X_train.shape[0]} train, {X_test.shape[0]} test")
    
    # Get correlation matrix
    print("\n   Correlation Matrix:")
    corr_matrix = data_processor.get_correlation_matrix()
    top_corr = corr_matrix.abs().unstack().sort_values(ascending=False).head(5)
    for idx, corr in top_corr.items():
        if idx[0] != idx[1]:  # Remove diagonal
            print(f"     {idx}: {corr:.3f}")
    
    print("\n5. Advanced Time Series Analysis...")
    
    # Create time series data
    time_series_data = pd.Series(
        np.random.randn(100).cumsum() + np.sin(np.arange(100) * 2 * np.pi / 50),
        index=pd.date_range('2020-01-01', periods=100, freq='D'),
        name='simulation_data'
    )
    
    # Create time series analysis
    ts_analysis = pynlm.create_statistical_analysis(pd.DataFrame({'data': time_series_data}))
    
    # Perform time series analysis
    time_series_results = ts_analysis.time_series_analysis(time_series_data, forecast_periods=10)
    
    print("   ✓ Time series decomposition completed")
    print(f"   ✓ Trend slope: {time_series_results['slope']:.6f}")
    print(f"   ✓ R-squared: {time_series_results['r_squared']:.3f}")
    print(f"   ✓ Forecast: {time_series_results['forecast'][:3].tolist()}...")
    
    # Create time series visualization
    fig, axes = plt.subplots(4, 1, figsize=(12, 10))
    
    # Plot original data
    axes[0].plot(time_series_data.index, time_series_data.values, label='Original', color='blue')
    axes[0].set_title('Original Time Series')
    axes[0].set_ylabel('Value')
    axes[0].legend()
    
    # Plot trend component
    axes[1].plot(time_series_data.index, time_series_results['trend'], label='Trend', color='orange')
    axes[1].set_title('Trend Component')
    axes[1].set_ylabel('Trend')
    axes[1].legend()
    
    # Plot seasonal component
    axes[2].plot(time_series_data.index, time_series_results['seasonal'], label='Seasonal', color='green')
    axes[2].set_title('Seasonal Component')
    axes[2].set_ylabel('Seasonality')
    axes[2].legend()
    
    # Plot residuals
    axes[3].plot(time_series_data.index, time_series_results['residual'], label='Residual', color='red')
    axes[3].set_title('Residual Component')
    axes[3].set_ylabel('Residual')
    axes[3].set_xlabel('Date')
    axes[3].legend()
    
    plt.tight_layout()
    plt.savefig('time_series_decomposition.png', dpi=300, bbox_inches='tight')
    print("   ✓ Time series decomposition visualization saved")
    
    print("\n6. Export Analysis Results...")
    
    # Create exporter
    exporter = pynlm.create_pandas_exporter()
    
    # Export analysis data
    exporter.export_to_csv(analysis_data, 'analysis_data.csv')
    exporter.export_to_json(analysis_data, 'analysis_data.json')
    
    # Export statistical results
    stats_df = pd.DataFrame({
        'metric': ['total_spike_count', 'active_neuron_count', 'average_firing_rate'],
        'mean': analysis_data['total_spike_count'].mean(),
        'std': analysis_data['total_spike_count'].std(),
        'min': analysis_data['total_spike_count'].min(),
        'max': analysis_data['total_spike_count'].max(),
        'count': len(analysis_data['total_spike_count'])
    })
    
    exporter.export_to_excel(stats_df, 'statistical_results.xlsx')
    print("   ✓ Analysis data exported to files")
    
    print("\n=== Visualization & Analysis Example Completed Successfully ===\n")
    
    # Display plots
    print("Displaying static plots...")
    plt.show()
    
    print("\nDisplaying interactive plots...")
    plotly_visualizer.show_interactive()
    
    return True
if __name__ == "__main__":
    try:
        success = main()
        if success:
            print("\nVisualization and analysis example completed successfully!")
            sys.exit(0)
        else:
            print("\nVisualization and analysis example failed!")
            sys.exit(1)
    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)