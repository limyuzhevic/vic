#!/usr/bin/env python3
"""
Test script for NLM Visualization and Debugging Tools

This script performs basic tests to verify the installation and
functionality of the NLM visualization and debugging tools.
"""

import sys
import os
import json
import time

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_import():
    """Test basic import of NLM tools"""
    print("Testing basic import...")
    
    try:
        from nlm_visualization_tools import (
            VisualizationInterface,
            DebugInterface,
            VisualizationMode,
            DebugMode,
            NLMVisualizationTools
        )
        print("✓ All imports successful")
        return True
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False

def test_initialization():
    """Test initialization of NLM tools"""
    print("\nTesting initialization...")
    
    try:
        from nlm_visualization_tools import NLMVisualizationTools
        
        tools = NLMVisualizationTools()
        if tools.initialize():
            print("✓ NLM tools initialized successfully")
            
            # Get interfaces
            viz = tools.get_visualization_interface()
            debug = tools.get_debug_interface()
            
            # Test basic operations
            viz.set_update_rate(30.0)
            viz.set_visualization_mode(VisualizationMode.REAL_TIME)
            
            debug.set_debug_mode(DebugMode.DETAILED)
            
            # Test data operations
            viz.add_time_series_metric("test_step", 1.0)
            data = viz.get_time_series_data("test_step")
            if len(data) == 1 and data[0].value == 1.0:
                print("✓ Time series data operations successful")
            else:
                print("✗ Time series data operations failed")
                return False
            
            # Cleanup
            tools.close()
            print("✓ Tools cleaned up successfully")
            return True
        else:
            print("✗ NLM tools initialization failed")
            return False
            
    except Exception as e:
        print(f"✗ Initialization test failed: {e}")
        return False

def test_data_structures():
    """Test data structure classes"""
    print("\nTesting data structures...")
    
    try:
        from nlm_visualization_tools import (
            NeuronActivityData,
            TimeSeriesDataPoint,
            DebugInfo,
            PerformanceMetrics,
            NeuralMetrics
        )
        
        # Test NeuronActivityData
        neuron_data = NeuronActivityData(
            neuron_id=1,
            membrane_potential=45.0,
            firing_rate=12.5,
            inhibition_level=0.2,
            last_spike_time=time.time(),
            is_active=True
        )
        
        assert neuron_data.neuron_id == 1
        assert neuron_data.membrane_potential == 45.0
        assert neuron_data.firing_rate == 12.5
        assert neuron_data.is_active == True
        
        print("✓ NeuronActivityData test passed")
        
        # Test TimeSeriesDataPoint
        ts_data = TimeSeriesDataPoint(
            timestamp=time.time(),
            metric_name="test_metric",
            value=42.0
        )
        
        assert ts_data.metric_name == "test_metric"
        assert ts_data.value == 42.0
        
        print("✓ TimeSeriesDataPoint test passed")
        
        # Test DebugInfo
        debug_info = DebugInfo(
            component_name="TestComponent",
            current_state="Active",
            warnings=["Warning 1"],
            errors=[],
            last_update_time=time.time(),
            is_healthy=True
        )
        
        assert debug_info.component_name == "TestComponent"
        assert debug_info.is_healthy == True
        
        print("✓ DebugInfo test passed")
        
        # Test PerformanceMetrics
        perf_metrics = PerformanceMetrics(
            cpu_usage=50.0,
            memory_usage=60.0,
            total_memory=16 * 1024 * 1024 * 1024ULL,
            used_memory=8 * 1024 * 1024 * 1024ULL,
            peak_memory=12 * 1024 * 1024 * 1024ULL,
            network_bytes_sent=1024,
            network_bytes_received=2048,
            disk_usage=30.0,
            timestamp=time.time()
        )
        
        assert perf_metrics.cpu_usage == 50.0
        assert perf_metrics.memory_usage == 60.0
        
        print("✓ PerformanceMetrics test passed")
        
        # Test NeuralMetrics
        neural_metrics = NeuralMetrics(
            total_neuron_count=100,
            active_neuron_count=50,
            spiking_neuron_count=20,
            average_firing_rate=15.0,
            excitation_inhibition_ratio=1.2,
            mean_membrane_potential=-65.0,
            mean_synaptic_weight=0.5,
            connectivity_density=0.15,
            timestamp=time.time()
        )
        
        assert neural_metrics.total_neuron_count == 100
        assert neural_metrics.average_firing_rate == 15.0
        
        print("✓ NeuralMetrics test passed")
        
        return True
        
    except Exception as e:
        print(f"✗ Data structure test failed: {e}")
        return False

def test_demo_script():
    """Test the main demo script"""
    print("\nTesting demo script...")
    
    try:
        # Check if main_demo.py exists
        demo_path = "python/main_demo.py"
        if os.path.exists(demo_path):
            print("✓ Demo script found at:", demo_path)
            
            # Check if it can be imported
            with open(demo_path, 'r') as f:
                content = f.read()
                if "NLM Visualization and Debugging Tools" in content:
                    print("✓ Demo script contains expected content")
                    return True
                else:
                    print("✗ Demo script content validation failed")
                    return False
        else:
            print("✗ Demo script not found")
            return False
            
    except Exception as e:
        print(f"✗ Demo script test failed: {e}")
        return False

def test_usage_example():
    """Test the usage example script"""
    print("\nTesting usage example...")
    
    try:
        # Check if usage_example.py exists
        example_path = "python/usage_example.py"
        if os.path.exists(example_path):
            print("✓ Usage example found at:", example_path)
            
            # Check if it can be imported
            with open(example_path, 'r') as f:
                content = f.read()
                if "NLM Visualization and Debugging Tools" in content:
                    print("✓ Usage example contains expected content")
                    return True
                else:
                    print("✗ Usage example content validation failed")
                    return False
        else:
            print("✗ Usage example not found")
            return False
            
    except Exception as e:
        print(f"✗ Usage example test failed: {e}")
        return False

def test_export_files():
    """Test export file functionality"""
    print("\nTesting export file functionality...")
    
    try:
        from nlm_visualization_tools import NLMVisualizationTools, OutputFormat
        
        tools = NLMVisualizationTools()
        if not tools.initialize():
            print("✗ Tools initialization failed")
            return False
        
        # Add some data
        viz = tools.get_visualization_interface()
        viz.add_time_series_metric("test_metric", 100.0)
        
        # Export data
        export_file = "test_export.json"
        tools.export_data(export_file, OutputFormat.JSON)
        
        if os.path.exists(export_file):
            print("✓ Export file created:", export_file)
            
            # Read and validate file
            with open(export_file, 'r') as f:
                data = json.load(f)
            
            if 'time_series_data' in data:
                print("✓ Exported data contains expected keys")
            else:
                print("✗ Exported data missing expected keys")
                return False
            
            # Clean up
            os.remove(export_file)
            print("✓ Test export file cleaned up")
            
            return True
        else:
            print("✗ Export file was not created")
            return False
            
    except Exception as e:
        print(f"✗ Export file test failed: {e}")
        return False

def run_all_tests():
    """Run all tests"""
    print("Running NLM Visualization and Debugging Tools Tests")
    print("=" * 60)
    
    tests = [
        ("Import Test", test_import),
        ("Initialization Test", test_initialization),
        ("Data Structures Test", test_data_structures),
        ("Demo Script Test", test_demo_script),
        ("Usage Example Test", test_usage_example),
        ("Export Files Test", test_export_files),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{test_name}")
        print("-" * len(test_name))
        
        try:
            if test_func():
                results.append((test_name, True, "Passed"))
            else:
                results.append((test_name, False, "Failed"))
        except Exception as e:
            results.append((test_name, False, f"Error: {e}"))
    
    # Print summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_name, success, message in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{status} {test_name}: {message}")
        
        if success:
            passed += 1
        else:
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Total Tests: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print("=" * 60)
    
    if failed == 0:
        print("\n🎉 All tests passed successfully!")
        return True
    else:
        print(f"\n❌ {failed} tests failed")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    
    if not success:
        print("\nSome tests failed. Please check the output above.")
        sys.exit(1)
    
    print("\nAll tests completed successfully!")
    sys.exit(0)
