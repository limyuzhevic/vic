#!/usr/bin/env python3
"""
Test script for Advanced Configuration Features in NLM

This script demonstrates the new configuration features:
- Schema validation
- Runtime configuration adjustment
- Auto-save functionality
- Configuration profiles
- Advanced configuration options
"""

import pynlm
import tempfile
import os

def test_basic_config():
    """Test basic configuration functionality"""
    print("=== Testing Basic Configuration ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    print(f"Created config with {len(config.getKeys())} default keys")
    
    # Test getting values
    neuron_count = config.getOr("brain.neuron_count", 1000)
    print(f"Default neuron count: {neuron_count}")
    
    # Test setting values
    config.set("brain.neuron_count", 2000)
    neuron_count = config.getOr("brain.neuron_count", 1000)
    print(f"After setting: {neuron_count}")
    
    # Test configuration summary
    summary = config.summary()
    print(f"Config summary (first 200 chars): {summary[:200]}...")
    
    return config

def test_advanced_config():
    """Test Advanced Configuration features"""
    print("\n=== Testing Advanced Configuration Features ===")
    
    # Create an advanced configuration (if available)
    try:
        from nlm import AdvancedConfig
        print("AdvancedConfig class available")
        
        # Create advanced configuration
        config = AdvancedConfig()
        
        # Register some schema entries
        config.registerSchema("brain.neuron_count", 1000, 
                            "Total number of neurons", True)
        config.registerSchema("brain.connection_probability", 0.1,
                            "Connection probability between neurons")
        config.registerSchema("plasticity.stdp.learning_rate", 0.01,
                            "STDP learning rate")
        
        # Set some values
        config.set("brain.neuron_count", 2000)
        config.set("brain.connection_probability", 0.15)
        config.set("plasticity.stdp.learning_rate", 0.02)
        
        # Test validation
        if config.validateAll():
            print("✓ All validations passed")
        else:
            errors = config.getValidationErrors()
            print(f"✗ Validation errors: {errors}")
        
        # Test configuration summary
        summary = config.summary()
        print(f"Advanced config summary (first 300 chars): {summary[:300]}...")
        
        return config
        
    except ImportError:
        print("AdvancedConfig not available, skipping advanced tests")
        return None

def test_runtime_adjustment():
    """Test runtime configuration adjustment"""
    print("\n=== Testing Runtime Configuration Adjustment ===")
    
    config = pynlm.createDefaultConfig()
    
    # Set initial value
    config.set("brain.neuron_count", 1000)
    print(f"Initial neuron count: {config.getOr('brain.neuron_count', 0)}")
    
    # Test adjustment operations
    success = config.adjust("brain.neuron_count", "+", "500")
    if success:
        new_value = config.getOr("brain.neuron_count", 0)
        print(f"After +500: {new_value}")
    
    success = config.adjust("brain.neuron_count", "*", "0.5")
    if success:
        new_value = config.getOr("brain.neuron_count", 0)
        print(f"After *0.5: {new_value}")
    
    return config

def test_profiles():
    """Test configuration profiles"""
    print("\n=== Testing Configuration Profiles ===")
    
    config = pynlm.createDefaultConfig()
    
    # Set some values
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.2)
    
    # Save a profile
    try:
        config.saveProfile("test_profile")
        print("✓ Saved profile 'test_profile'")
    except Exception as e:
        print(f"✗ Failed to save profile: {e}")
    
    # Load the profile
    try:
        config.loadProfile("test_profile")
        print(f"✓ Loaded profile, neuron count: {config.getOr('brain.neuron_count', 0)}")
    except Exception as e:
        print(f"✗ Failed to load profile: {e}")
    
    return config

def test_auto_save():
    """Test auto-save functionality"""
    print("\n=== Testing Auto-Save Functionality ===")
    
    config = pynlm.createDefaultConfig()
    
    # Enable auto-save
    try:
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            auto_save_file = f.name
        
        config.enableAutoSave(auto_save_file, 5.0)  # 5 second interval
        print(f"✓ Enabled auto-save to {auto_save_file}")
        
        # Set some values
        config.set("brain.neuron_count", 3000)
        
        # Wait a bit for auto-save
        import time
        time.sleep(6)
        
        # Check if file was created and has content
        if os.path.exists(auto_save_file):
            with open(auto_save_file, 'r') as f:
                content = f.read()
                if "brain.neuron_count" in content:
                    print("✓ Auto-save file created with correct content")
                else:
                    print("✗ Auto-save file exists but missing expected content")
        else:
            print("✗ Auto-save file not created")
        
        # Disable auto-save
        config.disableAutoSave()
        print("✓ Disabled auto-save")
        
        # Clean up
        if os.path.exists(auto_save_file):
            os.unlink(auto_save_file)
            
    except Exception as e:
        print(f"✗ Auto-save test failed: {e}")

def main():
    """Run all tests"""
    print("NLM Advanced Configuration Features Test")
    print("=" * 50)
    
    # Run all tests
    test_basic_config()
    test_advanced_config()
    test_runtime_adjustment()
    test_profiles()
    test_auto_save()
    
    print("\n" + "=" * 50)
    print("Test completed!")
    
    # Clean up
    print("\nCleaning up...")
    
    # Remove test profile files
    for filename in os.listdir("."):
        if filename.startswith("test_profile") and filename.endswith(".cfg"):
            try:
                os.unlink(filename)
                print(f"✓ Removed {filename}")
            except:
                pass

if __name__ == "__main__":
    main()