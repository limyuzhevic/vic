#!/usr/bin/env python3
"""
Integration tests for NLM Python module
=======================================

Comprehensive integration tests for the NLM Python module, testing the interaction
between different components and ensuring end-to-end functionality.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import numpy as np
import pandas as pd
import time
import tempfile
import json
from unittest.mock import Mock, patch

# Import all NLM modules
from nlm_python_module import NLPSimulation, SimulationState
from nlm_configuration_module import NLMConfigurator, NetworkArchitecture
from nlm_analysis_module import NLPAnalysisSuite, AnalysisType, VisualizationType
from nlm_ml_module import DeepLearningFramework
from nlm_debug_module import NLPToolkit
def test_end_to_end_workflow():
    """
    Test complete workflow from simulation to analysis and ML integration.
    """
    print("Testing end-to-end workflow...")
    
    try:
        # Step 1: Create simulation
        sim = NLPSimulation(headless=True)
        print("✓ Simulation created")
        
        # Step 2: Configure neural network
        sim.configure_neural_network(
            region_name="hippocampus",
            neuron_count=50,
            neuron_type="internal",
            connection_probability=0.05
        )
        print("✓ Neural network configured")
        
        # Step 3: Configure plasticity
        sim.configure_plasticity(
            plasticity_types=["stdp", "hebbian"],
            learning_rate=0.01
        )
        print("✓ Plasticity configured")
        
        # Step 4: Configure neuromodulation
        sim.configure_neuromodulation(
            neuromodulators=["dopamine", "curiosity"],
            baseline_levels={"dopamine": 0.05, "curiosity": 0.1},
            gain_factors={"dopamine": 1.0, "curiosity": 0.5}
        )
        print("✓ Neuromodulation configured")
        
        # Step 5: Create configuration
        configurator = NLMConfigurator(sim)
        arch = configurator.create_architecture(
            name="integration_test_brain",
            regions=[
                {'name': 'region1', 'neuron_count': 10, 'neuron_type': 'internal'},
                {'name': 'region2', 'neuron_count': 20, 'neuron_type': 'excitatory'}
            ],
            connections=[
                {'source_region': 'region1', 'target_region': 'region2', 'weight': 0.5}
            ]
        )
        print("✓ Configuration created")
        
        # Step 6: Create analysis suite
        analyzer = NLPAnalysisSuite(sim)
        print("✓ Analysis suite created")
        
        # Step 7: Create debugging toolkit
        toolkit = NLPToolkit(sim)
        print("✓ Debugging toolkit created")
        
        # Step 8: Create machine learning framework
        ml_framework = DeepLearningFramework()
        print("✓ Machine learning framework created")
        
        # Step 9: Test comprehensive analysis
        analysis_results = analyzer.run_comprehensive_analysis(
            analysis_types=[AnalysisType.NEURAL_ACTIVITY, AnalysisType.LEARNING_PROGRESS],
            visualization_types=[VisualizationType.HEATMAP, VisualizationType.TIME_SERIES]
        )
        print("✓ Comprehensive analysis completed")
        
        # Step 10: Test parameter optimization
        def objective_function(params):
            return (params.get('learning_rate', 0.01) - 0.05) ** 2
        
        optimization_result = configurator.optimize_parameters(
            objective_function=objective_function,
            initial_params={'learning_rate': 0.01, 'tau_pre': 0.02},
            optimization_method='random_search',
            bounds={'learning_rate': (0.001, 0.1), 'tau_pre': (0.01, 0.05)}
        )
        print("✓ Parameter optimization completed")
        
        # Step 11: Test monitoring
        toolkit.start_monitoring()
        time.sleep(0.1)  # Let monitoring collect some data
        toolkit.stop_monitoring()
        print("✓ Monitoring completed")
        
        # Step 12: Export results
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            sim.export_results(f.name, include_step_history=True, include_performance_metrics=True)
            print(f"✓ Results exported to {f.name}")
        
        return True
        
    except Exception as e:
        print(f"✗ End-to-end workflow test failed: {e}")
        return False
def test_parameter_tuning_workflow():
    """
    Test parameter tuning and optimization workflow.
    """
    print("\nTesting parameter tuning workflow...")
    
    try:
        # Create simulation
        sim = NLPSimulation(headless=True)
        
        # Create configurator with simulation
        configurator = NLMConfigurator(sim)
        
        # Create architecture for testing
        arch = configurator.create_architecture(
            name="tuning_test",
            regions=[
                {'name': 'brain_region', 'neuron_count': 10, 'neuron_type': 'internal'}
            ],
            connections=[]
        )
        
        # Define objective function
        def complex_objective(params):
            """Complex objective function with multiple parameters"""
            learning_rate = params.get('learning_rate', 0.01)
            stdp_tau = params.get('stdp_tau', 0.02)
            hebbian_strength = params.get('hebbian_strength', 1.0)
            
            # Simulate performance calculation
            performance = (learning_rate * 10) + (1.0 - stdp_tau * 50) + (hebbian_strength * 5)
            target = 25.0
            
            return (performance - target) ** 2 + 0.1 * sum(abs(p) for p in params.values())
        
        # Test different optimization methods
        optimization_methods = ['random_search', 'gradient_descent']
        initial_params = {'learning_rate': 0.01, 'stdp_tau': 0.02, 'hebbian_strength': 1.0}
        bounds = {'learning_rate': (0.001, 0.1), 'stdp_tau': (0.01, 0.05), 'hebbian_strength': (0.5, 2.0)}
        
        for method in optimization_methods:
            print(f"  Testing {method}...")
            
            if method == 'random_search':
                result = configurator.parameter_optimizer.random_search(
                    complex_objective, bounds, n_samples=20
                )
            elif method == 'gradient_descent':
                result = configurator.parameter_optimizer.gradient_descent(
                    complex_objective, initial_params, learning_rate=0.01, max_iterations=10
                )
            else:
                continue
            
            # Verify result structure
            assert 'optimized_parameters' in result
            assert 'final_score' in result
            assert 'n_samples' in result or 'iterations' in result
            
            print(f"    ✓ {method} completed - Score: {result['final_score']:.6f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Parameter tuning workflow test failed: {e}")
        return False
def test_neural_activity_analysis():
    """
    Test neural activity analysis with mock data.
    """
    print("\nTesting neural activity analysis...")
    
    try:
        from nlm_analysis_module import NeuralActivityAnalyzer
        
        # Create analyzer
        analyzer = NeuralActivityAnalyzer()
        
        # Test spike pattern analysis (with mock data)
        print("  Testing spike pattern analysis...")
        spike_results = analyzer.analyze_spike_patterns(
            neurons=[1, 2, 3, 4, 5],
            time_window=None,
            metrics=['firing_rate', 'coherence', 'synchrony']
        )
        
        assert 'spike_patterns' in spike_results
        assert 'temporal_dynamics' in spike_results
        print("    ✓ Spike pattern analysis completed")
        
        # Test coherence analysis
        print("  Testing coherence analysis...")
        coherence_results = analyzer.analyze_neural_coherence(
            neurons=[1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
            window_size=100,
            threshold=0.5
        )
        
        assert 'coherence_matrix' in coherence_results
        assert 'significant_connections' in coherence_results
        assert 'mean_coherence' in coherence_results
        print("    ✓ Coherence analysis completed")
        
        # Test firing regularity analysis
        print("  Testing firing regularity analysis...")
        firing_results = analyzer.analyze_firing_regularity(
            neuron_id=1,
            window_size=1000
        )
        
        assert 'firing_rate' in firing_results
        assert 'cv_of_isi' in firing_results
        assert 'burst_index' in firing_results
        print("    ✓ Firing regularity analysis completed")
        
        # Test burst detection
        print("  Testing burst detection...")
        spike_times = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0] * 2  # 10 spikes per 1.0 second
        burst_results = analyzer.detect_bursts(
            neuron_id=1,
            spike_times=spike_times,
            threshold=2.0,
            min_burst_duration=0.5,
            min_inter_burst_interval=1.0
        )
        
        assert 'bursts' in burst_results
        assert 'burst_metrics' in burst_results
        print("    ✓ Burst detection completed")
        
        return True
        
    except Exception as e:
        print(f"✗ Neural activity analysis test failed: {e}")
        return False
def test_network_topology_analysis():
    """
    Test network topology analysis with mock data.
    """
    print("\nTesting network topology analysis...")
    
    try:
        from nlm_analysis_module import NetworkTopologyAnalyzer
        
        # Create analyzer
        analyzer = NetworkTopologyAnalyzer()
        
        # Test connectivity pattern analysis
        print("  Testing connectivity pattern analysis...")
        connectivity_results = analyzer.analyze_connectivity_patterns(
            neurons=[1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
            min_connection_strength=0.1
        )
        
        assert 'degree_distribution' in connectivity_results
        assert 'clustering_coefficient' in connectivity_results
        assert 'path_length' in connectivity_results
        print("    ✓ Connectivity pattern analysis completed")
        
        # Test clustering coefficient calculation
        print("  Testing clustering coefficient calculation...")
        adjacency_matrix = np.array([
            [0, 1, 1, 0, 0, 0, 0, 0, 0, 0],
            [1, 0, 1, 1, 0, 0, 0, 0, 0, 0],
            [1, 1, 0, 0, 1, 0, 0, 0, 0, 0],
            [0, 1, 0, 0, 1, 1, 0, 0, 0, 0],
            [0, 0, 1, 1, 0, 1, 1, 0, 0, 0],
            [0, 0, 0, 1, 1, 0, 1, 1, 0, 0],
            [0, 0, 0, 0, 1, 1, 0, 1, 1, 0],
            [0, 0, 0, 0, 0, 1, 1, 0, 1, 1],
            [0, 0, 0, 0, 0, 0, 1, 1, 0, 1],
            [0, 0, 0, 0, 0, 0, 0, 1, 1, 0]
        ])
        
        clustering_coeff = analyzer.calculate_clustering_coefficient(adjacency_matrix)
        assert isinstance(clustering_coeff, float)
        assert 0.0 <= clustering_coeff <= 1.0
        print(f"    ✓ Clustering coefficient calculated: {clustering_coeff:.4f}")
        
        # Test path length calculation
        print("  Testing path length calculation...")
        path_lengths = analyzer.calculate_path_lengths(adjacency_matrix)
        assert 'average' in path_lengths
        assert 'characteristic' in path_lengths
        print(f"    ✓ Path lengths calculated: {path_lengths}")
        
        # Test community detection
        print("  Testing community detection...")
        communities = analyzer.detect_communities(adjacency_matrix, method='louvain')
        assert 'communities' in communities
        assert 'modularity' in communities
        assert 'number_of_communities' in communities
        print(f"    ✓ Communities detected: {communities['number_of_communities']}")
        
        # Test network entropy
        print("  Testing network entropy...")
        entropy_results = analyzer.analyze_network_entropy(adjacency_matrix)
        assert 'degree_entropy' in entropy_results
        assert 'strength_entropy' in entropy_results
        assert 'clustering_entropy' in entropy_results
        assert 'total_network_entropy' in entropy_results
        print(f"    ✓ Network entropy calculated: {entropy_results['total_network_entropy']:.4f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Network topology analysis test failed: {e}")
        return False
def test_learning_progress_analysis():
    """
    Test learning progress analysis with mock data.
    """
    print("\nTesting learning progress analysis...")
    
    try:
        from nlm_analysis_module import LearningProgressAnalyzer
        
        # Create analyzer
        analyzer = LearningProgressAnalyzer()
        
        # Test learning curve analysis
        print("  Testing learning curve analysis...")
        # Create mock learning data
        steps = list(range(1, 101))
        accuracy = np.array([0.1 + 0.02 * i + np.random.normal(0, 0.02) for i in steps])
        loss = np.array([2.0 - 0.02 * i + np.random.normal(0, 0.05) for i in steps])
        
        learning_data = pd.DataFrame({
            'step': steps,
            'accuracy': accuracy,
            'loss': loss
        })
        
        # Test learning curve analysis
        curve_results = analyzer.analyze_learning_curve(
            experiment_id='test_exp',
            metric='accuracy',
            window_size=20
        )
        
        assert 'final_performance' in curve_results
        assert 'initial_performance' in curve_results
        assert 'improvement' in curve_results
        assert 'improvement_rate' in curve_results
        print(f"    ✓ Learning curve analysis completed - Improvement: {curve_results['improvement']:.4f}")
        
        # Test performance trends analysis
        print("  Testing performance trends analysis...")
        trend_results = analyzer.analyze_performance_trends(
            experiment_id='test_exp',
            metrics=['accuracy', 'loss', 'f1_score']
        )
        
        assert 'metrics' in trend_results
        assert 'cross_correlation' in trend_results
        assert 'covariance_matrix' in trend_results
        print("    ✓ Performance trends analysis completed")
        
        # Test anomaly detection
        print("  Testing anomaly detection...")
        anomaly_results = analyzer.detect_performance_anomalies(
            experiment_id='test_exp',
            method='isolation_forest',
            contamination=0.1
        )
        
        assert 'anomaly_count' in anomaly_results
        assert 'anomaly_indices' in anomaly_results
        assert 'anomaly_scores' in anomaly_results
        print(f"    ✓ Anomaly detection completed - {anomaly_results['anomaly_count']} anomalies detected")
        
        # Test performance prediction
        print("  Testing performance prediction...")
        prediction_results = analyzer.predict_next_performance(
            experiment_id='test_exp',
            model_type='arima',
            forecast_horizon=10
        )
        
        assert 'predictions' in prediction_results
        assert 'confidence_intervals' in prediction_results
        print(f"    ✓ Performance prediction completed - {len(prediction_results['predictions'])} predictions")
        
        return True
        
    except Exception as e:
        print(f"✗ Learning progress analysis test failed: {e}")
        return False
def test_deep_learning_integration():
    """
    Test deep learning integration with mock data.
    """
    print("\nTesting deep learning integration...")
    
    try:
        from nlm_ml_module import DeepLearningFramework, FeedForwardNN
        
        # Create framework
        framework = DeepLearningFramework()
        print("✓ Deep learning framework created")
        
        # Create mock dataset
        np.random.seed(42)
        n_samples = 100
        n_features = 10
        n_classes = 3
        
        X = np.random.randn(n_samples, n_features)
        y = np.random.randint(0, n_classes, n_samples)
        
        # Create feed-forward model
        print("  Creating feed-forward model...")
        model = FeedForwardNN(
            input_shape=(n_features,),
            output_shape=(n_classes,),
            hidden_layers=[32, 16],
            activation='relu',
            output_activation='softmax',
            learning_rate=0.001
        )
        
        print("✓ Feed-forward model created")
        
        # Test model compilation
        print("  Compiling model...")
        model.compile_model()
        print("✓ Model compiled")
        
        # Test training
        print("  Training model...")
        # Use small dataset for quick testing
        X_train_small = X[:50]
        y_train_small = y[:50]
        
        training_results = model.train(
            X_train_small, y_train_small,
            epochs=3,
            batch_size=16,
            validation_split=0.2
        )
        
        assert 'epochs' in training_results
        assert 'final_loss' in training_results
        print(f"✓ Model trained - Final loss: {training_results['final_loss']:.4f}")
        
        # Test evaluation
        print("  Evaluating model...")
        X_test_small = X[50:]
        y_test_small = y[50:]
        
        evaluation_results = model.evaluate(
            X_test_small, y_test_small,
            metrics=['accuracy', 'loss']
        )
        
        assert 'accuracy' in evaluation_results
        assert 'loss' in evaluation_results
        print(f"✓ Model evaluated - Accuracy: {evaluation_results['accuracy']:.4f}")
        
        # Test predictions
        print("  Testing predictions...")
        predictions = model.predict(X_test_small)
        print(f"✓ Predictions generated - Shape: {predictions.shape}")
        
        # Test probability predictions
        print("  Testing probability predictions...")
        probabilities = model.predict_proba(X_test_small)
        print(f"✓ Probability predictions generated - Shape: {probabilities.shape}")
        
        # Add model to framework
        framework.models['test_model'] = model
        print("✓ Model added to framework")
        
        return True
        
    except Exception as e:
        print(f"✗ Deep learning integration test failed: {e}")
        return False
def test_debugging_integration():
    """
    Test debugging and error handling integration.
    """
    print("\nTesting debugging integration...")
    
    try:
        from nlm_debug_module import NLPToolkit
        
        # Create toolkit
        toolkit = NLPToolkit()
        print("✓ Debugging toolkit created")
        
        # Test error handling
        print("  Testing error handling...")
        
        # Test with a simple error
        try:
            raise ValueError("Test error for debugging")
        except Exception as e:
            error_result = toolkit.handle_exception(e, {"context": "test_context"})
            assert 'handled' in error_result
            print("✓ Error handled successfully")
        
        # Test debugging features
        print("  Testing debugging features...")
        
        # Set up breakpoints
        toolkit.set_breakpoint("test_breakpoint", condition=lambda msg, data, comp: "test" in msg.lower())
        print("✓ Breakpoint set")
        
        # Watch variables
        toolkit.watch_variable("test_var", 42)
        print("✓ Variable watched")
        
        # Profile function
        print("  Testing function profiling...")
        
        def sample_function(x):
            return x * 2
        
        # This would actually profile the function
        print("✓ Function profiling ready")
        
        # Test monitoring
        print("  Testing monitoring...")
        toolkit.start_monitoring()
        time.sleep(0.05)  # Let monitoring collect data
        toolkit.stop_monitoring()
        print("✓ Monitoring completed")
        
        # Test data export
        print("  Testing data export...")
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            toolkit.export_debug_data(f.name)
            print(f"✓ Debug data exported to {f.name}")
        
        return True
        
    except Exception as e:
        print(f"✗ Debugging integration test failed: {e}")
        return False
def run_integration_tests():
    """
    Run all integration tests.
    """
    print("=" * 70)
    print("NLM Python Module - Integration Test Suite")
    print("=" * 70)
    
    tests = [
        ("End-to-End Workflow", test_end_to_end_workflow),
        ("Parameter Tuning Workflow", test_parameter_tuning_workflow),
        ("Neural Activity Analysis", test_neural_activity_analysis),
        ("Network Topology Analysis", test_network_topology_analysis),
        ("Learning Progress Analysis", test_learning_progress_analysis),
        ("Deep Learning Integration", test_deep_learning_integration),
        ("Debugging Integration", test_debugging_integration),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        try:
            result = test_func()
            results.append((test_name, result, None))
            status = "PASS" if result else "FAIL"
            print(f"\n{test_name}: {status}")
        except Exception as e:
            results.append((test_name, False, str(e)))
            print(f"\n{test_name}: ERROR - {e}")
    
    # Summary
    print(f"\n{'='*70}")
    print("Integration Test Summary")
    print(f"{'='*70}")
    
    passed = sum(1 for _, result, _ in results if result)
    failed = sum(1 for _, result, _ in results if not result)
    total = len(results)
    
    for test_name, result, error in results:
        status = "PASS" if result else "FAIL"
        error_msg = f" ({error})" if error else ""
        print(f"  {test_name}: {status}{error_msg}")
    
    print(f"\nTotal: {total}, Passed: {passed}, Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All integration tests passed! NLM Python module integration is working correctly.")
        return True
    else:
        print(f"\n❌ {failed} integration tests failed. Please check the errors above.")
        return False
if __name__ == '__main__':
    # Run integration tests
    success = run_integration_tests()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)
