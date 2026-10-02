import pynlm

def test_config():
    """Test configuration management."""
    
    print("Testing configuration management...")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Test has method
    assert hasattr(config, 'has'), "has method not available"
    
    # Test setting values
    config.set("brain.neuron_count", 750)
    assert config.has("brain.neuron_count"), "Setting config value failed"
    
    # Test getKeys method
    assert hasattr(config, 'getKeys'), "getKeys method not available"
    keys = config.getKeys()
    print(f"Config keys: {len(keys)} keys found")
    
    # Test clear method
    assert hasattr(config, 'clear'), "clear method not available"
    
    # Test summary method
    assert hasattr(config, 'summary'), "summary method not available"
    summary = config.summary()
    print(f"Config summary: {summary}")
    
    print("✓ Configuration management tests passed")
    return config

if __name__ == "__main__":
    test_config()