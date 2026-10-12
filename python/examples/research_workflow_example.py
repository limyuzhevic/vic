#!/usr/bin/env python3
"""
Research Workflow Example for Enhanced NLM Python Bindings

This script demonstrates a complete research workflow using the enhanced NLM
Python bindings, including learning experiments, parameter tuning, and analysis.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import plotly.graph_objects as go

def main():
    print("=== Enhanced NLM Python Bindings - Research Workflow Example ===\n")
    
    # Example 1: Learning Experiment API
    print("1. Learning Experiment API...")
    
    learning_api = pynlm.LearningExperimentAPI()
    
    # Run basic learning experiment
    basic_learning = learning_api.run_basic_learning(
        steps=1000,
        learning_rate=0.01,
        neuron_count=1000
    )
    
    print(f"   ✓ Basic learning experiment completed: {basic_learning.experiment_name}")
    print(f"   ✓ Steps: {basic_learning.step_data.shape[0] if not basic_learning.step_data.empty else 0}")
    
    # Run plasticity learning experiment
    plasticity_learning = learning_api.run_plasticity_learning(
        steps=2000,
        learning_rate=0.05,
        enable_stdp=True,
        enable_hebbian=True
    )
    
    print(f"   ✓ Plasticity learning experiment completed: {plasticity_learning.experiment_name}")
    
    # Run development learning experiment
    development_learning = learning_api.run_development_learning(
        steps=5000,
        learning_rate=0.001,
        enable_development=True
    )
    
    print(f"   ✓ Development learning experiment completed: {development_learning.experiment_name}")
    
    # Example 2: Integration Test API
    print("\n2. Integration Test API...")
    
    integration_api = pynlm.IntegrationTestAPI()
    
    # Run complete integration test
    complete_integration = integration_api.run_complete_integration(
        steps=10000,
        enable_checkpointing=True,
        enable_replay=True,
        enable_development=True
    )
    
    print(f"   ✓ Complete integration test completed: {complete_integration.experiment_name}")
    
    # Run memory integration test
    memory_integration = integration_api.run_memory_integration(
        steps=3000
    )
    
    print(f"   ✓ Memory integration test completed: {memory_integration.experiment_name}")
    
    # Run neuromodulation integration test
    neuromodulation_integration = integration_api.run_neuromodulation_integration(
        steps=2000
    )
    
    print(f"   ✓ Neuromodulation integration test completed: {neuromodulation_integration.experiment_name}")
    
    # Example 3: Parameter Tuning API
    print("\n3. Parameter Tuning API...")
    
    param_tuning_api = pynlm.ParameterTuningAPI()
    
    # Tune learning rates
    learning_rate_tuning = param_tuning_api.tune_learning_rate(
        learning_rates=[0.001, 0.005, 0.01, 0.05, 0.1],
        n_trials=20
    )
    
    print(f"   ✓ Learning rate tuning completed")
    print(f"   ✓ Best learning rate: {learning_rate_tuning['best_learning_rate']}")
    print(f"   ✓ Best score: {learning_rate_tuning['best_score']}")
    
    # Tune network sizes
    network_size_tuning = param_tuning_api.tune_network_size(
        neuron_counts=[500, 1000, 2000, 5000],
        n_trials=20
    )
    
    print(f"   ✓ Network size tuning completed")
    print(f"   ✓ Best neuron count: {network_size_tuning['best_neuron_count']}")
    print(f"   ✓ Best score: {network_size_tuning['best_score']}")
    
    # Example 4: Ablation Study API
    print("\n4. Ablation Study API...")
    
    ablation_api = pynlm.AblationStudyAPI()
    
    # Disable memory systems
    memory_ablation = ablation_api.disable_memory_systems(steps=500)
    print(f"   ✓ Memory ablation study completed: {memory_ablation.experiment_name}")
    
    # Disable neuromodulation
    neuromodulation_ablation = ablation_api.disable_neuromodulation(steps=500)
    print(f"   ✓ Neuromodulation ablation study completed: {neuromodulation_ablation.experiment_name}")
    
    # Disable plasticity
    plasticity_ablation = ablation_api.disable_plasticity(steps=500)
    print(f"   ✓ Plasticity ablation study completed: {plasticity_ablation.experiment_name}")
    
    # Example 5: Cross-Validation API
    print("\n5. Cross-Validation API...")
    
    cv_api = pynlm.CrossValidationAPI()
    
    # Validate learning
    learning_cv = cv_api.validate_learning(n_folds=5)
    print(f"   ✓ Learning cross-validation completed")
    print(f"   ✓ Results: {len(learning_cv)} parameter sets evaluated")
    
    # Validate memory
    memory_cv = cv_api.validate_memory(n_folds=5)
    print(f"   ✓ Memory cross-validation completed")
    print(f"   ✓ Results: {len(memory_cv)} parameter sets evaluated")
    
    # Example 6: Visualization
    print("\n6. Visualization...")
    
    # Create visualizer
    plotly_visualizer = pynlm.create_plotly_visualizer()
    
    # Create interactive dashboard
    dashboard = plotly_visualizer.create_interactive_dashboard()
    print("   ✓ Interactive dashboard created")
    
    # Create exploration plots
    exploration_plots = plotly_visualizer.create_exploration_plots()
    print(f"   ✓ Exploration plots created: {list(exploration_plots.keys())}")
    
    # Create real-time metrics plot
    real_time_plot = plotly_visualizer.plot_real_time_metrics()
    print("   ✓ Real-time metrics plot created")
    
    # Example 7: Data Analysis
    print("\n7. Data Analysis...")
    
    # Create comprehensive dataset
    np.random.seed(42)
    n_samples = 1000
    
    # Generate synthetic research data
    data = pd.DataFrame({
        'experiment': np.random.choice(['learning', 'integration', 'ablation'], n_samples),
        'brain_type': np.random.choice(['standard', 'development', 'advanced'], n_samples),
        'learning_rate': np.random.uniform(0.001, 0.1, n_samples),
        'neuron_count': np.random.randint(500, 5000, n_samples),
        'performance': np.random.normal(0.8, 0.1, n_samples),
        'memory_usage': np.random.uniform(0.5, 1.0, n_samples),
        'energy_consumption': np.random.uniform(0.3, 0.9, n_samples),
        'stability': np.random.uniform(0.6, 1.0, n_samples)
    })
    
    # Add some realistic correlations
    data['performance'] += data['learning_rate'] * 10
    data['performance'] -= abs(data['stability'] - 0.8) * 5
    data['performance'] = np.clip(data['performance'], 0, 1)
    
    # Create statistical analysis
    analysis = pynlm.create_statistical_analysis(data)
    
    # Describe data
    print("   Data Description:")
    describe_results = analysis.describe()
    for column, stats in describe_results.items():
        if column in ['performance', 'memory_usage', 'learning_rate']:
            print(f"     {column}: mean={stats.mean:.3f}, std={stats.std:.3f}")
    
    # Perform correlation analysis
    print("\n   Correlation Analysis:")
    for var1, var2 in [('learning_rate', 'performance'),
                      ('neuron_count', 'performance'),
                      ('memory_usage', 'performance')]:
        corr_results = analysis.correlation_analysis(data[var1], data[var2])
        print(f"     {var1} vs {var2}: r={corr_results.correlation:.3f}, p={corr_results.p_value:.3f}")
    
    # Perform t-tests
    print("\n   T-Test Analysis:")
    for group1, group2 in [('learning', 'integration'),
                          ('integration', 'ablation')]:
        t_test_results = analysis.t_test(
            data[data['experiment'] == group1]['performance'],
            data[data['experiment'] == group2]['performance']
        )
        print(f"     {group1} vs {group2}: t={t_test_results['t_statistic']:.3f}, p={t_test_results['p_value']:.3f}")
    
    # Example 8: Data Processing
    print("\n8. Data Processing...")
    
    # Create data processor
    data_processor = pynlm.create_research_data_processor(data)
    
    # Clean data
    data_processor.clean_data(dropna=False, fillna=True, strategy='mean')
    print("   ✓ Data cleaned and normalized")
    
    # Add features
    data_processor.feature_engineering({
        'squared_learning_rate': lambda df: df['learning_rate'] ** 2,
        'interaction_learning_neuron': lambda df: df['learning_rate'] * df['neuron_count'],
        'performance_scaled': lambda df: (df['performance'] - df['performance'].mean()) / df['performance'].std()
    })
    print("   ✓ Features engineered")
    
    # Split data
    X_train, X_test, y_train, y_test = data_processor.split_data('performance', test_size=0.2, random_state=42)
    print(f"   ✓ Data split: {X_train.shape[0]} train, {X_test.shape[0]} test")
    
    # Get correlation matrix
    print("\n   Top 5 Correlations:")
    corr_matrix = data_processor.get_correlation_matrix().abs().unstack()
    upper = corr_matrix.where(np.triu(np.ones(corr_matrix.shape), k=1).astype(bool))
    top_correlations = upper.stack().sort_values(ascending=False).head(5)
    for idx, corr in top_correlations.items():
        print(f"     {idx}: {corr:.3f}")
    
    # Example 9: Comprehensive Workflow
    print("\n9. Comprehensive Workflow...")
    
    # Create experiment manager
    exp_manager = pynlm.ExperimentManager()
    
    # Set up brain
    brain = pynlm.create_brain_manager()
    brain.set_learning_rate(0.01)
    brain.set_neuron_count(1000)
    exp_manager.set_brain_manager(brain)
    
    # Run parameter tuning
    param_space = {
        'learning_rate': [0.001, 0.005, 0.01, 0.05, 0.1],
        'neuron_count': [500, 1000, 2000, 5000],
        'connection_probability': [0.05, 0.1, 0.2]
    }
    
    best_params = exp_manager.parameter_tuning(
        param_space=param_space,
        n_trials=10,
        metric='performance'
    )
    
    print(f"   ✓ Parameter tuning completed")
    print(f"   ✓ Best parameters: {best_params}")
    
    # Apply best parameters and run experiment
    if brain:
        brain.set_learning_rate(best_params.get('learning_rate', 0.01))
        brain.set_neuron_count(best_params.get('neuron_count', 1000))
        
        results = brain.run_experiment(steps=500)
        print(f"   ✓ Best experiment results: {results.metrics}")
    
    print("\n=== Research Workflow Example Completed Successfully ===\n")
    
    # Display interactive plots
    print("Displaying interactive plots...")
    plotly_visualizer.show_interactive()
    
    return True
if __name__ == "__main__":
    try:
        success = main()
        if success:
            print("\nResearch workflow example completed successfully!")
            sys.exit(0)
        else:
            print("\nResearch workflow example failed!")
            sys.exit(1)
    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)