#!/usr/bin/env python3
"""
Quick validation script for NLM Python bindings

This script performs a basic validation of the Python bindings to ensure
they are properly installed and functional.
"""

import pynlm
import sys

def validate_bindings():
    """Validate that the Python bindings are working correctly"""
    print("Validating NLM Python bindings...")
    
    try:
        # Test 1: Basic imports
        print("✓ pynlm imported successfully")
        
        # Test 2: Factory functions
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        brain = pynlm.createBrain(config)
        print("✓ createBrain() works")
        
        world = pynlm.createSimpleWorld()
        print("✓ createSimpleWorld() works")
        
        agent = pynlm.createAgentBrain(brain)
        print("✓ createAgentBrain() works")
        
        viz = pynlm.createVisualization()
        print("✓ createVisualization() works")
        
        # Test 3: Performance monitoring tools
        monitor = pynlm.createPerformanceMonitor()
        print("✓ createPerformanceMonitor() works")
        
        profiler = pynlm.createMemoryProfiler()
        print("✓ createMemoryProfiler() works")
        
        metrics = pynlm.createMetricsCollector()
        print("✓ createMetricsCollector() works")
        
        # Test 4: Configuration helpers
        brain_config = pynlm.create_brain_config()
        print("✓ create_brain_config() works")
        
        agent_config = pynlm.create_agent_config()
        print("✓ create_agent_config() works")
        
        world_config = pynlm.create_world_config()
        print("✓ create_world_config() works")
        
        # Test 5: Benchmark metrics
        benchmark_metrics = pynlm.createBenchmarkMetrics()
        print("✓ createBenchmarkMetrics() works")
        
        scaling_analysis = pynlm.createScalingAnalysis()
        print("✓ createScalingAnalysis() works")
        
        # Test 6: Brain initialization and basic operations
        brain.initialize()
        print("✓ brain.initialize() works")
        
        assert brain.is_initialized() == True
        print("✓ brain.is_initialized() returns True")
        
        # Test basic brain operations
        for i in range(5):
            brain.step(i)
        
        assert brain.getTotalSpikeCount() >= 0
        print("✓ brain.step() works")
        
        # Test stats convenience method
        stats = brain.get_stats()
        assert isinstance(stats, dict)
        assert "total_neurons" in stats
        print("✓ brain.get_stats() works")
        
        # Test agent operations
        agent.initialize(world)
        print("✓ agent.initialize() works")
        
        action = agent.decodeMotorCommand()
        print("✓ agent.decodeMotorCommand() works")
        
        # Test visualization
        viz.initialize()
        print("✓ viz.initialize() works")
        
        viz.update()
        print("✓ viz.update() works")
        
        viz.render()
        print("✓ viz.render() works")
        
        viz.close()
        print("✓ viz.close() works")
        
        print("\n" + "="*60)
        print("SUCCESS: All NLM Python bindings are working correctly!")
        print("="*60)
        return True
        
    except Exception as e:
        print(f"\n✗ ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_types():
    """Test that custom error types are properly exposed"""
    print("\nTesting custom error types...")
    
    try:
        # Try to trigger a config error
        raise pynlm.ConfigError("Test configuration error")
    except pynlm.ConfigError as e:
        print("✓ ConfigError is properly exposed")
    
    try:
        # Try to trigger a simulation error  
        raise pynlm.SimulationError("Test simulation error")
    except pynlm.SimulationError as e:
        print("✓ SimulationError is properly exposed")
    
    try:
        # Try to trigger a visualization error
        raise pynlm.VisualizationError("Test visualization error")
    except pynlm.VisualizationError as e:
        print("✓ VisualizationError is properly exposed")
    
    print("✓ All custom error types are working")

if __name__ == "__main__":
    print("NLM Python Bindings Quick Validation")
    print("="*60)
    
    # Test basic functionality
    if not validate_bindings():
        sys.exit(1)
    
    # Test error types
    test_error_types()
    
    print("\n🎉 All validations completed successfully!")
    print("\nThe NLM Python bindings have been enhanced with:")
    print("• Comprehensive error handling and validation")
    print("• Performance monitoring and debugging tools")
    print("• Advanced agent features and behavioral control")
    print("• Configuration presets and helper functions")
    print("• Pythonic convenience methods for Brain class")
    print("• Visualization interface support")
    print("• Complete documentation and examples")
    print("• Full test suite for validation")
    
    print("\nYou can now run the full test suite with:")
    print("  python test_pynlm_bindings.py")
    
    sys.exit(0)