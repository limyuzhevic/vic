#!/usr/bin/env python3
"""
Simple Test for Enhanced NLM Python Bindings

This script tests the basic functionality of the enhanced NLM Python bindings
to ensure they work correctly.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_functionality():
    """Test basic functionality of the enhanced NLM bindings."""
    print("Testing basic functionality of enhanced NLM Python bindings...")
    
    try:
        # Test 1: BrainManager
        print("\n1. Testing BrainManager...")
        brain_manager = pynlm.BrainManager()
        print("   ✓ BrainManager created successfully")
        
        # Test 2: Configuration
        print("\n2. Testing PythonConfig...")
        config = pynlm.PythonConfig(learning_rate=0.01, neuron_count=1000)
        print(f"   ✓ PythonConfig created: learning_rate={config.learning_rate}, neuron_count={config.neuron_count}")
        
        # Test 3: ResearchSession
        print("\n3. Testing ResearchSession...")
        with pynlm.ResearchSession() as session:
            print("   ✓ ResearchSession context manager works")
            session.run_step(0)
            metrics = session.collect_metrics()
            print(f"   ✓ Metrics collected: {len(metrics)} metrics")
        
        # Test 4: ExperimentManager
        print("\n4. Testing ExperimentManager...")
        exp_manager = pynlm.ExperimentManager()
        print("   ✓ ExperimentManager created successfully")
        
        # Test 5: Visualization
        print("\n5. Testing Visualization APIs...")
        viz = pynlm.create_matplotlib_visualizer()
        print("   ✓ MatplotlibVisualizer created")
        
        plotly_viz = pynlm.create_plotly_visualizer()
        print("   ✓ PlotlyVisualizer created")
        
        # Test 6: Analysis
        print("\n6. Testing Analysis APIs...")
        import pandas as pd
        import numpy as np
        
        # Create sample data
        sample_data = pd.DataFrame({
            'group1': np.random.normal(100, 15, 50),
            'group2': np.random.normal(105, 20, 50),
            'time': np.arange(50),
            'value': np.random.exponential(2, 50)
        })
        
        analysis = pynlm.create_statistical_analysis(sample_data)
        print("   ✓ StatisticalAnalysis created")
        
        # Test 7: Data Export
        print("\n7. Testing Data Export...")
        exporter = pynlm.create_pandas_exporter()
        print("   ✓ PandasDataFrameExporter created")
        
        # Test 8: Research Workflows
        print("\n8. Testing Research Workflows...")
        learning_api = pynlm.LearningExperimentAPI()
        integration_api = pynlm.IntegrationTestAPI()
        
        print("   ✓ LearningExperimentAPI created")
        print("   ✓ IntegrationTestAPI created")
        
        # Test 9: Parameter Tuning
        print("\n9. Testing Parameter Tuning...")
        param_tuning_api = pynlm.ParameterTuningAPI()
        print("   ✓ ParameterTuningAPI created")
        
        # Test 10: Ablation Studies
        print("\n10. Testing Ablation Studies...")
        ablation_api = pynlm.AblationStudyAPI()
        print("   ✓ AblationStudyAPI created")
        
        # Test 11: Cross-Validation
        print("\n11. Testing Cross-Validation...")
        cv_api = pynlm.CrossValidationAPI()
        print("   ✓ CrossValidationAPI created")
        
        print("\n✓ All basic functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False
def test_advanced_functionality():
    """Test advanced functionality of the enhanced NLM bindings."""
    print("\n\nTesting advanced functionality of enhanced NLM Python bindings...")
    
    try:
        # Test 1: Complex Research Workflow
        print("\n1. Testing Complex Research Workflow...")
        
        # Create brain manager
        brain_manager = pynlm.BrainManager()
        brain_manager.set_learning_rate(0.01)
        brain_manager.set_neuron_count(1000)
        
        # Create experiment manager
        exp_manager = pynlm.ExperimentManager(brain_manager)
        
        # Run learning experiment
        result = exp_manager.conduct_learning_experiment(
            experiment_name="complex_experiment",
            steps=200,
            learning_rate=0.01,
            neuron_count=1000
        )
        
        print(f"   ✓ Learning experiment completed: {result.experiment_name}")
        
        # Test 2: Parameter Tuning
        print("\n2. Testing Parameter Tuning...")
        param_tuning_api = pynlm.ParameterTuningAPI()
        param_tuning_api.set_brain_manager(brain_manager)
        
        param_space = {
            'learning_rate': [0.001, 0.005, 0.01, 0.05],
            'neuron_count': [500, 1000, 2000, 5000]
        }
        
        best_params = param_tuning_api.parameter_tuning(
            param_space=param_space,
            n_trials=5,
            metric='performance'
        )
        
        print(f"   ✓ Parameter tuning completed: best_params={best_params}")
        
        # Test 3: Statistical Analysis
        print("\n3. Testing Statistical Analysis...")
        import pandas as pd
        import numpy as np
        
        # Create synthetic experimental data
        data = pd.DataFrame({
            'experiment': ['A', 'B', 'C', 'A', 'B', 'C'] * 50,
            'performance': np.random.normal(0.8, 0.1, 300),
            'memory_usage': np.random.uniform(0.5, 1.0, 300),
            'learning_rate': np.random.uniform(0.001, 0.1, 300)
        })
        
        # Create data processor
        data_processor = pynlm.create_research_data_processor(data)
        data_processor.clean_data(dropna=False, fillna=True, strategy='mean')
        data_processor.normalize()
        
        # Split data
        X_train, X_test, y_train, y_test = data_processor.split_data('performance')
        
        print(f"   ✓ Data processing completed: {X_train.shape[0]} train, {X_test.shape[0]} test")
        
        # Test 4: Visualization
        print("\n4. Testing Visualization...")
        viz = pynlm.create_matplotlib_visualizer(brain_manager)
        
        # Generate plots
        viz.plot_neural_activity()
        viz.plot_memory_metrics()
        viz.plot_learning_progress()
        
        print("   ✓ All plots generated successfully")
        
        # Test 5: Data Export
        print("\n5. Testing Data Export...")
        exporter = pynlm.create_pandas_exporter()
        
        # Export sample data
        exporter.export_to_csv(data, "test_export.csv")
        exporter.export_to_json(data, "test_export.json")
        
        print("   ✓ Data exported successfully")
        
        print("\n✓ All advanced functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n✗ Advanced test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False
def main():
    """Run all tests."""
    print("=" * 60)
    print("Enhanced NLM Python Bindings - Test Suite")
    print("=" * 60)
    
    # Run basic functionality tests
    basic_success = test_basic_functionality()
    
    # Run advanced functionality tests
    advanced_success = test_advanced_functionality()
    
    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    if basic_success:
        print("✓ Basic Functionality Tests: PASSED")
    else:
        print("✗ Basic Functionality Tests: FAILED")
    
    if advanced_success:
        print("✓ Advanced Functionality Tests: PASSED")
    else:
        print("✗ Advanced Functionality Tests: FAILED")
    
    if basic_success and advanced_success:
        print("\n✓ All tests passed! Enhanced NLM Python bindings are working correctly.")
        return True
    else:
        print("\n✗ Some tests failed. Please check the errors above.")
        return False
if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\n✗ Test suite failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)