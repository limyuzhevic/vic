#!/usr/bin/env python3
"""
Simple test to verify Python module structure works
"""

import sys
import os

print("Testing NLM Python Module Structure")
print("=" * 50)

# Add the project to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Test basic imports directly
try:
    from pynlm.config import Config, ConfigSchema, ConfigValidator, create_default_config
    print("✓ pynlm.config module imported successfully")
except Exception as e:
    print(f"✗ pynlm.config import failed: {e}")

try:
    from pynlm.brain import Brain, ProgressCallback
    print("✓ pynlm.brain module imported successfully")
except Exception as e:
    print(f"✗ pynlm.brain import failed: {e}")

try:
    from pynlm.world import SimpleWorld
    print("✓ pynlm.world module imported successfully")
except Exception as e:
    print(f"✗ pynlm.world import failed: {e}")

try:
    from pynlm.agent import AgentBrain
    print("✓ pynlm.agent module imported successfully")
except Exception as e:
    print(f"✗ pynlm.agent import failed: {e}")

try:
    from pynlm.logger import LogLevel, StructuredLogger, get_logger
    print("✓ pynlm.logger module imported successfully")
except Exception as e:
    print(f"✗ pynlm.logger import failed: {e}")

# Test config module functionality
try:
    from pynlm.config import create_default_config
    cfg = create_default_config()
    print("✓ Default config created successfully")
    
    # Test basic config operations
    cfg.set("test_key", "test_value")
    assert cfg.get("test_key") == "test_value"
    print("✓ Config basic operations work")
    
    assert cfg.has("test_key") == True
    assert "test_key" in cfg.get_keys()
    print("✓ Config has method works")
    
except Exception as e:
    print(f"✗ Config functionality test failed: {e}")

# Test schema functionality
try:
    from pynlm.config import ConfigSchema, ConfigValidator
    
    schema = ConfigSchema("test_int", int, required=True, default=42)
    assert schema.name == "test_int"
    assert schema.data_type == int
    assert schema.default == 42
    print("✓ ConfigSchema works correctly")
    
    validator = ConfigValidator([schema])
    assert validator.get_default_config() == {"test_int": 42}
    print("✓ ConfigValidator works correctly")
    
except Exception as e:
    print(f"✗ Schema functionality test failed: {e}")

# Test logger functionality
try:
    from pynlm.logger import get_logger, LogLevel
    
    logger_instance = get_logger("test_logger", LogLevel.INFO)
    assert logger_instance.name == "test_logger"
    assert logger_instance.get_level() == LogLevel.INFO
    print("✓ Logger functionality works")
    
    # Test logging methods
    logger_instance.debug("Test debug message")
    logger_instance.info("Test info message")
    print("✓ Logger logging methods work")
    
except Exception as e:
    print(f"✗ Logger functionality test failed: {e}")

print("\n" + "=" * 50)
print("Basic Python module structure test complete!")
print("Note: This test verifies the Python wrapper modules are created correctly.")
print("C++ bindings may not be available in this environment.")