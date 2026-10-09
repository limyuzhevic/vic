#!/usr/bin/env python3
"""
Basic test for NLM Python module
================================

This is a basic test to verify the NLM Python module installation and functionality.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import numpy as np
import pandas as pd
from unittest.mock import Mock, patch

def test_basic_imports():
    """Test that all core modules can be imported"""
    print("Testing basic imports...")
    
    try:
        from nlm_python_module import NLPSimulation, SimulationState
        print("✓ NLM Python module imported successfully")
    except ImportError as e:
        print(f"✗ Failed to import NLM Python module: {e}")
        return False
    
    try:
        from nlm_configuration_module import NLMConfigurator, NetworkArchitecture
        print("✓ NLM Configuration module imported successfully")
    except ImportError as e:
        print(f"✗ Failed to import NLM Configuration module: {e}")
        return False
    
    try:
        from nlm_analysis_module import NLPAnalysisSuite
        print("✓ NLM Analysis module imported successfully")
    except ImportError as e:
        print(f"✗ Failed to import NLM Analysis module: {e}")
        return False
    
    try:
        from nlm_ml_module import DeepLearningFramework
        print("✓ NLM Machine Learning module imported successfully")
    except ImportError as e:
        print(f"✗ Failed to import NLM Machine Learning module: {e}")
        return False
    
    try:
        from nlm_debug_module import NLPToolkit
        print("✓ NLM Debug module imported successfully")
    except ImportError as e:
        print(f"✗ Failed to import NLM Debug module: {e}")
        return False
    
    return True
def test_nlp_simulation_creation():
    """Test NLPSimulation creation and basic functionality"""
    print("\nTesting NLPSimulation creation...")
    
    try:
        from nlm_python_module import NLPSimulation, SimulationState
        
        # Test simulation creation
        sim = NLPSimulation(headless=True)
        print("✓ NLPSimulation created successfully")
        
        # Test simulation status
        status = sim.get_simulation_status()
        assert 'state' in status
        assert status['state'] == SimulationState.STOPPED.value
        print("✓ Simulation status retrieved successfully")
        
        # Test configuration methods
        sim.configure_neural_network(
            region_name="test_region",
            neuron_count=10,
            neuron_type="internal"
        )
        print("✓ Neural network configuration successful")
        
        # Test plasticity configuration
        sim.configure_plasticity(
            plasticity_types=["stdp", "hebbian"],
            learning_rate=0.01
        )
        print("✓ Plasticity configuration successful")
        
        # Test neuromodulation configuration
        sim.configure_neuromodulation(
            neuromodulators=["dopamine", "curiosity"]
        )
        print("✓ Neuromodulation configuration successful")
        
        return True
        
    except Exception as e:
        print(f"✗ NLPSimulation test failed: {e}")
        return False
def test_nlm_configurator():
    """Test NLMConfigurator functionality"""
    print("\nTesting NLMConfigurator...")
    
    try:
        from nlm_configuration_module import NLMConfigurator
        
        # Test configurator creation
        configurator = NLMConfigurator()
        print("✓ NLMConfigurator created successfully")
        
        # Test architecture creation
        arch = configurator.create_architecture(
            name="test_architecture",
            regions=[
                {'name': 'region1', 'neuron_count': 10, 'neuron_type': 'internal'},
                {'name': 'region2', 'neuron_count': 20, 'neuron_type': 'excitatory'}
            ],
            connections=[
                {'source_region': 'region1', 'target_region': 'region2', 'weight': 0.5}
            ]
        )
        print("✓ Network architecture created successfully")
        
        # Test configuration summary
        summary = configurator.get_configuration_summary()
        assert 'architectures_count' in summary
        print("✓ Configuration summary retrieved successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ NLMConfigurator test failed: {e}")
        return False
def test_nlp_analysis():
    """Test NLPAnalysis functionality"""
    print("\nTesting NLPAnalysis...")
    
    try:
        from nlm_analysis_module import NLPAnalysisSuite, AnalysisType
        
        # Test analysis suite creation
        analyzer = NLPAnalysisSuite()
        print("✓ NLPAnalysisSuite created successfully")
        
        # Test analysis with different types
        analysis_types = list(AnalysisType)
        print(f"✓ Analysis types available: {[t.value for t in analysis_types]}")
        
        # Test visualization types
        from nlm_analysis_module import VisualizationType
        viz_types = list(VisualizationType)
        print(f"✓ Visualization types available: {[t.value for t in viz_types]}")
        
        return True
        
    except Exception as e:
        print(f"✗ NLPAnalysis test failed: {e}")
        return False
def test_nlp_ml():
    """Test NLM machine learning functionality"""
    print("\nTesting NLM Machine Learning...")
    
    try:
        from nlm_ml_module import DeepLearningFramework, ModelArchitecture
        
        # Test framework creation
        framework = DeepLearningFramework()
        print("✓ DeepLearningFramework created successfully")
        
        # Test model factory
        model_factory = framework.model_factory
        print("✓ Model factory available")
        
        # Test model types
        model_types = list(ModelArchitecture)
        print(f"✓ Model types available: {[t.value for t in model_types]}")
        
        return True
        
    except Exception as e:
        print(f"✗ NLM Machine Learning test failed: {e}")
        return False
def test_nlp_debug():
    """Test NLM debugging functionality"""
    print("\nTesting NLM Debugging...")
    
    try:
        from nlm_debug_module import NLPToolkit, ErrorSeverity, ErrorCategory
        
        # Test toolkit creation
        toolkit = NLPToolkit()
        print("✓ NLPToolkit created successfully")
        
        # Test error handling
        error_context = ErrorContext()
        error_context.component = "test"
        
        # Test error severity
        severities = list(ErrorSeverity)
        print(f"✓ Error severities available: {[s.value for s in severities]}")
        
        # Test error categories
        categories = list(ErrorCategory)
        print(f"✓ Error categories available: {[c.value for c in categories]}")
        
        return True
        
    except Exception as e:
n\t       print(f"✗ NLM Debugging test failed: {e}")
        return False
def test_package_metadata():
    """Test package metadata"""
    print("\nTesting package metadata...")
    
    try:
        import pkg_resources
        from nlm_python_module import __version__, __description__
        
        print(f"✓ Package version: {__version__}")
        print(f"✓ Package description: {__description__}")
        
        # Check if package can be imported
        import nlm_python_module
        print("✓ Package metadata accessible")
        
        return True
        
    except Exception as e:
        print(f"✗ Package metadata test failed: {e}")
        return False
def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Module - Basic Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("NLPSimulation Creation", test_nlp_simulation_creation),
        ("NLMConfigurator", test_nlm_configurator),
        ("NLPAnalysis", test_nlp_analysis),
        ("NLM Machine Learning", test_nlp_ml),
        ("NLM Debugging", test_nlp_debug),
        ("Package Metadata", test_package_metadata),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'='*40}")
        print(f"Running: {test_name}")
        print(f"{'='*40}")
        
        try:
            result = test_func()
            results.append((test_name, result, None))
            status = "PASS" if result else "FAIL"
            print(f"\n{test_name}: {status}")
        except Exception as e:
            results.append((test_name, False, str(e)))
            print(f"\n{test_name}: ERROR - {e}")
    
    # Summary
    print(f"\n{'='*60}")
    print("Test Summary")
    print(f"{'='*60}")
    
    passed = sum(1 for _, result, _ in results if result)
    failed = sum(1 for _, result, _ in results if not result)
    total = len(results)
    
    for test_name, result, error in results:
        status = "PASS" if result else "FAIL"
        error_msg = f" ({error})" if error else ""
        print(f"  {test_name}: {status}{error_msg}")
    
    print(f"\nTotal: {total}, Passed: {passed}, Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! NLM Python module is working correctly.")
        return True
    else:
        print(f"\n❌ {failed} tests failed. Please check the errors above.")
        return False
if __name__ == '__main__':
    # Run tests
    success = run_all_tests()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)
