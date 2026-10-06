#!/usr/bin/env python3
"""
Tests for NLM Enhanced Python API

Tests the new convenience methods and improved APIs added to NLM Python bindings.
"""

import sys
import os
import tempfile
import json
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    import advanced_config as ac
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: Required modules not found. Tests may fail.")
def test_convenience_methods():
    """Test new convenience methods"""
    print("Testing convenience methods...")
    
    if not PYNLM_AVAILABLE:
        print("  Skipped: pynlm not available")
        return True
    
    try:
        # Test createBrainFromDict
        config_dict = {
            "neuron_count": 100,
            "region_count": 1,
            "connection_probability": 0.1
        }
        brain = pynlm.createBrainFromDict(config_dict)
        assert brain is not None
        print("  ✓ createBrainFromDict works")
        
        # Test createAgentEnvironment
        brain, agent, world = pynlm.createAgentEnvironment()
        assert brain is not None
        assert agent is not None
        assert world is not None
        print("  ✓ createAgentEnvironment works")
        
        # Test getBrainStats
        stats = pynlm.getBrainStats(brain)
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        print("  ✓ getBrainStats works")
        
        # Test createWorldObjects
        objects = pynlm.createWorldObjects([
            {"x": 0.0, "y": 0.0, "type": pynlm.WorldObjectType.Resource, "value": 1.0},
            {"x": 1.0, "y": 1.0, "type": pynlm.WorldObjectType.Hazard, "value": -0.5}
        ])
        assert len(objects) == 2
        print("  ✓ createWorldObjects works")
        
        # Test getCompletePercept
        percept = pynlm.getCompletePercept(world)
        assert "vision_width" in percept
        assert "vision_height" in percept
        assert "vision_channels" in percept
        print("  ✓ getCompletePercept works")
        
        # Test applyActionSafely
        action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 1.0])
        # This should not raise an exception
        pynlm.applyActionSafely(world, action, 0.0)
        print("  ✓ applyActionSafely works")
        
        # Test loadConfigWithDefaults
        config = pynlm.loadConfigWithDefaults("non_existent.json", {"neuron_count": 50})
        assert config is not None
        print("  ✓ loadConfigWithDefaults works")
        
        # Test quickBenchmark
        result = pynlm.quickBenchmark(50)
        assert "steps_per_second" in result
        assert "neurons" in result
        print("  ✓ quickBenchmark works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False
def test_custom_exceptions():
    """Test custom exception hierarchy"""
    print("Testing custom exceptions...")
    
    if not PYNLM_AVAILABLE:
        print("  Skipped: pynlm not available")
        return True
    
    try:
        # Test exception types exist
        assert hasattr(pynlm, "NLMError")
        assert hasattr(pynlm, "ConfigurationError")
        assert hasattr(pynlm, "SimulationError")
        assert hasattr(pynlm, "SensoryError")
        assert hasattr(pynlm, "ActionError")
        assert hasattr(pynlm, "MemoryError")
        assert hasattr(pynlm, "DevelopmentError")
        print("  ✓ All exception types exist")
        
        # Test exception inheritance
        assert issubclass(pynlm.ConfigurationError, pynlm.NLMError)
        assert issubclass(pynlm.SimulationError, pynlm.NLMError)
        assert issubclass(pynlm.SensoryError, pynlm.NLMError)
        assert issubclass(pynlm.ActionError, pynlm.NLMError)
        assert issubclass(pynlm.MemoryError, pynlm.NLMError)
        assert issubclass(pynlm.DevelopmentError, pynlm.NLMError)
        print("  ✓ Exception inheritance correct")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False
def test_performance_monitoring():
    """Test performance monitoring"""
    print("Testing performance monitoring...")
    
    if not PYNLM_AVAILABLE:
        print("  Skipped: pynlm not available")
        return True
    
    try:
        # Test quickBenchmark
        result = pynlm.quickBenchmark(50)
        assert isinstance(result, dict)
        assert "steps_per_second" in result
        print("  ✓ quickBenchmark returns dict")
        
        # Test runBenchmarks
        results = pynlm.runBenchmarks()
        assert isinstance(results, list)
        assert len(results) > 0
        print("  ✓ runBenchmarks returns list")
        
        # Test benchmarkPerformance
        result = pynlm.benchmarkPerformance(50, 100)
        assert hasattr(result, 'neurons')
        assert hasattr(result, 'synapses')
        assert hasattr(result, 'stepsPerSecond')
        print("  ✓ benchmarkPerformance returns BenchmarkResult")
        
        # Test PerformanceMonitor
        monitor = pynlm.PerformanceMonitor()
        assert monitor is not None
        monitor.start()
        import time
        time.sleep(0.01)
        monitor.record_event("test")
        monitor.stop()
        
        stats = monitor.get_stats()
        assert isinstance(stats, list)
        print("  ✓ PerformanceMonitor works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False
def test_advanced_configuration():
    """Test advanced configuration management"""
    print("Testing advanced configuration...")
    
    if not PYNLM_AVAILABLE:
        print("  Skipped: pynlm not available")
        return True
    
    try:
        # Test basic ConfigManager
        config = ac.ConfigManager()
        config.set("neuron_count", 100)
        config.set("connection_probability", 0.1)
        
        assert config.get("neuron_count") == 100
        assert config.get("connection_probability") == 0.1
        print("  ✓ ConfigManager basic operations")
        
        # Test schema validation
        errors = config.validate()
        assert isinstance(errors, list)
        print("  ✓ ConfigManager validation")
        
        # Test ConfigTemplate
        template = ac.ConfigTemplate("Test Template", "A test template")
        template.add_config({
            "neuron_count": 200,
            "region_count": 2
        })
        
        merged = template.merge_with({"neuron_count": 100})
        assert merged["neuron_count"] == 200  # Template overrides
        assert merged["region_count"] == 2
        print("  ✓ ConfigTemplate merging")
        
        # Test PresetManager
        preset_manager = ac.PresetManager()
        preset_manager.save_preset("test_preset",
                                   "A test preset",
                                   {"neuron_count": 50},
                                   ["test", "quick"])
        
        preset = preset_manager.load_preset("test_preset")
        assert preset is not None
        assert preset.name == "test_preset"
        print("  ✓ PresetManager save/load")
        
        # Test AdvancedConfig
        advanced = ac.AdvancedConfig()
        advanced.load("non_existent.json")  # Should not crash
        advanced.set("neuron_count", 300)
        assert advanced.get("neuron_count") == 300
        print("  ✓ AdvancedConfig basic operations")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False
def test_configuration_file_operations():
    """Test configuration file operations"""
    print("Testing configuration file operations...")
    
    try:
        import tempfile
        import os
        
        # Create temporary directory for test files
        with tempfile.TemporaryDirectory() as tmpdir:
            config_file = os.path.join(tmpdir, "test_config.json")
            
            # Test save and load
            config = ac.ConfigManager()
            config.set("neuron_count", 100)
            config.set("connection_probability", 0.05)
            
            # Save to file
            success = config.save_to_file(config_file)
            assert success
            print("  ✓ Configuration saved to file")
            
            # Load from file
            config2 = ac.ConfigManager()
            success = config2.load_from_file(config_file)
            assert success
            print("  ✓ Configuration loaded from file")
            
            # Verify data
            assert config2.get("neuron_count") == 100
            assert config2.get("connection_probability") == 0.05
            print("  ✓ Configuration data verified")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False
def test_cli_interface():
    """Test CLI interface functionality"""
    print("Testing CLI interface...")
    
    try:
        # Test that CLI module can be imported
        from nlm_cli import NLMCLI
        cli = NLMCLI()
        print("  ✓ CLI interface can be imported")
        
        # Test that help can be generated (this would require CLI parsing)
        # We'll just check that the module structure is correct
        assert hasattr(cli, 'run_simulation')
        assert hasattr(cli, 'run_benchmark')
        assert hasattr(cli, 'run_episodes')
        assert hasattr(cli, 'load_config')
        assert hasattr(cli, 'save_config')
        assert hasattr(cli, 'print_config')
        print("  ✓ CLI methods exist")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False
def test_documentation():
    """Test that documentation exists"""
    print("Testing documentation...")
    
    try:
        # Check that documentation files exist
        docs_dir = Path("docs")
        if docs_dir.exists():
            print("  ✓ docs directory exists")
        else:
            print("  - docs directory not found (optional)")
        
        # Check that enhanced documentation exists
        enhanced_doc = Path("NLM_Enhanced_Documentation.md")
        if enhanced_doc.exists():
            print("  ✓ Enhanced documentation exists")
        else:
            print("  ✗ Enhanced documentation not found")
            return False
        
        # Check that CLI documentation exists
        cli_doc = Path("nlm_cli.py")
        if cli_doc.exists():
            print("  ✓ CLI implementation exists")
        else:
            print("  ✗ CLI implementation not found")
            return False
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False
def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Enhanced Python API Tests")
    print("=" * 60)
    
    tests = [
        ("Convenience Methods", test_convenience_methods),
        ("Custom Exceptions", test_custom_exceptions),
        ("Performance Monitoring", test_performance_monitoring),
        ("Advanced Configuration", test_advanced_configuration),
        ("Configuration File Operations", test_configuration_file_operations),
        ("CLI Interface", test_cli_interface),
        ("Documentation", test_documentation),
    ]
    
    passed = 0
    failed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n{test_name}:")
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"  ✗ Test crashed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed}/{total} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("✓ All tests passed!")
        return True
    else:
        print(f"✗ {failed} test(s) failed")
        return False
if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
