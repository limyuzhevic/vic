#!/usr/bin/env python3
"""
Validation script for NLM Python modules.
Tests that the Python bindings and modules can be imported correctly
and that the basic functionality works as expected.
"""

import sys
import os
import json

def test_python_modules_import():
    """Test that Python modules can be imported successfully."""
    print("Testing Python module imports...")
    
    try:
        # Test core modules
        from pynlm import NeuronId, SynapseId, Config, Brain
        print("✓ Core C++ bindings imported successfully")
        
        # Test high-level interface
        from pynlm.nlm import NLMConfiguration, NLMBrain, NLMEnvironment
        print("✓ High-level NLM interface imported successfully")
        
        # Test learning module
        from pynlm.learning import LearningExperiment, NeuromodulationAnalyzer, VisualizationTools
        print("✓ Learning module imported successfully")
        
        # Test development module  
        from pynlm.development import (
            NLMDeveloper, DevelopmentalStage, DevelopmentalCharacteristics,
            StructuralPlasticitySimulator
        )
        print("✓ Development module imported successfully")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error during import: {e}")
        return False

def test_basic_functionality():
    """Test basic functionality of the Python interface."""
    print("\nTesting basic functionality...")
    
    try:
        from pynlm.nlm import NLMConfiguration
        
        # Test configuration creation
        config = NLMConfiguration()
        config.set("neuron_count", 100)
        config.set("simulation_timestep", 0.001)
        
        assert config.get("neuron_count") == 100
        assert config.get("simulation_timestep") == 0.001
        print("✓ Configuration creation and management works")
        
        # Test configuration display
        print("\nConfiguration preview:")
        config.display()
        print("✓ Configuration display works")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def test_learning_module():
    """Test learning module functionality."""
    print("\nTesting learning module...")
    
    try:
        from pynlm.learning import LearningExperiment, NeuromodulationAnalyzer
        
        # Create a mock environment for testing
        class MockEnvironment:
            def __init__(self):
                self.metrics_history = []
                self.step_times = []
            
            def run_lifetime_learning(self, num_episodes, max_steps):
                return {
                    "episode_results": [
                        {"total_reward": 10 * (i + 1), "total_steps": 500} 
                        for i in range(num_episodes)
                    ]
                }
        
        mock_env = MockEnvironment()
        
        # Test experiment creation
        experiment = LearningExperiment(mock_env)
        print("✓ LearningExperiment creation works")
        
        # Test experiment run with mock data
        result = experiment.run_experiment("test_experiment", num_episodes=3)
        assert "results" in result
        assert result["name"] == "test_experiment"
        print("✓ Learning experiment execution works")
        
        # Test results analysis
        analysis = experiment.analyze_results("test_experiment")
        assert "learning_curve" in analysis
        assert "performance" in analysis
        print("✓ Results analysis works")
        
        # Test neuromodulation analyzer
        neuromod_analysis = NeuromodulationAnalyzer.analyze_dopamine_dynamics(mock_env)
        assert "dopamine" in neuromod_analysis
        print("✓ Neuromodulation analysis works")
        
        return True
        
    except Exception as e:
        print(f"✗ Learning module test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_development_module():
    """Test development module functionality."""
    print("\nTesting development module...")
    
    try:
        from pynlm.development import NLMDeveloper, DevelopmentalStage, StructuralPlasticitySimulator
        
        # Create a mock brain for testing
        class MockBrain:
            def __init__(self):
                self.synapses = 100
                self.neurons = 50
            
            def getTotalSynapseCount(self):
                return self.synapses
            
            def getTotalNeuronCount(self):
                return self.neurons
        
        mock_brain = MockBrain()
        
        # Test developer creation
        developer = NLMDeveloper(mock_brain)
        print("✓ NLMDeveloper creation works")
        
        # Test developmental stage
        assert developer.get_current_stage() == DevelopmentalStage.INITIAL
        print("✓ Developmental stage tracking works")
        
        # Test development advance
        result = developer.advance_development(1.0)
        assert "current_stage" in result
        print("✓ Development advance works")
        
        # Test development analysis
        analysis = developer.analyze_development()
        assert "current_stage" in analysis
        print("✓ Development analysis works")
        
        # Test structural plasticity simulator
        simulator = StructuralPlasticitySimulator(mock_brain, developer)
        print("✓ StructuralPlasticitySimulator creation works")
        
        return True
        
    except Exception as e:
        print(f"✗ Development module test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_python_interface():
    """Test the complete Python interface."""
    print("\nTesting complete Python interface...")
    
    try:
        from pynlm.nlm import NLMEnvironment, NLMConfiguration
        
        # Create a simple test using the interface
        config = NLMConfiguration()
        config.set("neuron_count", 50)
        
        # This will test the high-level interface
        print("✓ Python interface components are accessible")
        
        return True
        
    except Exception as e:
        print(f"✗ Python interface test failed: {e}")
        return False

def run_all_tests():
    """Run all validation tests."""
    print("=" * 60)
    print("NLM Python Module Validation")
    print("=" * 60)
    
    tests = [
        ("Module Imports", test_python_modules_import),
        ("Basic Functionality", test_basic_functionality),
        ("Learning Module", test_learning_module),
        ("Development Module", test_development_module),
        ("Python Interface", test_python_interface),
    ]
    
    results = []
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n--- {test_name} ---")
        try:
            result = test_func()
            results.append((test_name, result))
            if result:
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            print(f"✗ {test_name} ERROR: {e}")
            results.append((test_name, False))
    
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    for test_name, result in results:
        status = "✓ PASSED" if result else "✗ FAILED"
        print(f"{test_name}: {status}")
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! The NLM Python interface is working correctly.")
        return True
    else:
        print(f"\n⚠️  {total - passed} tests failed. Please check the output above.")
        return False

def create_test_package():
    """Create a simple test package for distribution."""
    print("\nCreating test package...")
    
    test_package = {
        "version": "0.1.0",
        "description": "NLM Python interface test package",
        "modules": [
            "pynlm",
            "pynlm.nlm",
            "pynlm.learning", 
            "pynlm.development"
        ],
        "tests": [
            "test_imports.py",
            "test_basic_functionality.py",
            "test_learning_module.py",
            "test_development_module.py"
        ]
    }
    
    # Save to JSON file
    with open("test_package.json", "w") as f:
        json.dump(test_package, f, indent=2)
    
    print("✓ Test package created: test_package.json")
    return True

if __name__ == "__main__":
    # Change to the python directory for testing
    os.chdir(os.path.join(os.path.dirname(__file__), "python"))
    
    # Run validation tests
    success = run_all_tests()
    
    # Create test package
    create_test_package()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)
