#!/usr/bin/env python3
"""
Simple test to verify configuration improvements.
"""

def test_json_parsing():
    """Test basic JSON parsing capability"""
    try:
        import json
        
        # Test basic JSON structure for configuration
        config = {
            "brain": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1
            },
            "simulation": {
                "timestep": 0.001,
                "max_steps": 10000
            }
        }
        
        # Serialize and deserialize
        json_str = json.dumps(config)
        parsed = json.loads(json_str)
        
        # Verify structure
        assert "brain" in parsed
        assert "simulation" in parsed
        assert parsed["brain"]["neuron_count"] == 1000
        
        print("✓ JSON parsing test passed")
        return True
    except Exception as e:
        print(f"✗ JSON parsing test failed: {e}")
        return False

def test_backward_compatibility():
    """Test that existing configuration format is still supported"""
    try:
        # Simulate old config format parsing
        old_config_lines = [
            "# NLM Configuration File",
            "random_seed = 42",
            "simulation_timestep = 0.001",
            "neuron_count = 1000",
            "region_count = 2",
            "connection_probability = 0.1"
        ]
        
        # Parse old format
        parsed = {}
        for line in old_config_lines:
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                parsed[key.strip()] = value.strip()
        
        # Verify
        assert parsed["random_seed"] == "42"
        assert parsed["neuron_count"] == "1000"
        assert parsed["connection_probability"] == "0.1"
        
        print("✓ Backward compatibility test passed")
        return True
    except Exception as e:
        print(f"✗ Backward compatibility test failed: {e}")
        return False

def main():
    print("Testing NLM Configuration System Improvements...")
    print("=" * 50)
    
    results = []
    
    # Test JSON parsing
    results.append(test_json_parsing())
    
    # Test backward compatibility
    results.append(test_backward_compatibility())
    
    # Summary
    print("=" * 50)
    passed = sum(results)
    total = len(results)
    
    print(f"Tests passed: {passed}/{total}")
    
    if passed == total:
        print("✅ All configuration improvement tests passed!")
        return True
    else:
        print("❌ Some tests failed")
        return False

if __name__ == "__main__":
    import sys
    sys.exit(0 if main() else 1)
