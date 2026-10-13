#!/usr/bin/env python3
"""
Tests for the enhanced NLM Python configuration module.
"""

import sys
import os
import tempfile
import unittest
from unittest.mock import patch, MagicMock

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from config import Config, ConfigSource, NLMConfigError
from nlm import get_config, set_config_value, get_config_value, SimulationError

class TestConfig(unittest.TestCase):
    """Test cases for the Config class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = Config()
    
    def testConfigCreation(self):
        """Test basic config creation"""
        self.assertIsNotNone(self.config)
        self.assertEqual(len(self.config.get_keys()), 17)  # Default values
    
    def testConfigSetAndGet(self):
        """Test setting and getting configuration values"""
        self.config.set("test_key", 123, ConfigSource.RUNTIME, "Test key")
        self.assertTrue(self.config.has("test_key"))
        
        value = self.config.get("test_key")
        self.assertEqual(value, 123)
        
        # Test get_or with default
        default_value = self.config.get_or("missing_key", 456)
        self.assertEqual(default_value, 456)
        
        # Test existing key
        existing_value = self.config.get_or("test_key", 456)
        self.assertEqual(existing_value, 123)
    
    def testConfigRemove(self):
        """Test removing configuration values"""
        self.config.set("temp_key", "temp_value", ConfigSource.RUNTIME)
        self.assertTrue(self.config.has("temp_key"))
        
        self.config.remove("temp_key")
        self.assertFalse(self.config.has("temp_key"))
    
    def testConfigKeys(self):
        """Test getting configuration keys"""
        self.config.set("key1", 1, ConfigSource.RUNTIME)
        self.config.set("key2", 2, ConfigSource.RUNTIME)
        self.config.set("key3", 3, ConfigSource.RUNTIME)
        
        keys = self.config.get_keys()
        self.assertEqual(len(keys), 20)  # 17 default + 3 new
        self.assertIn("key1", keys)
        self.assertIn("key2", keys)
        self.assertIn("key3", keys)
    
    def testConfigSource(self):
        """Test configuration source handling"""
        self.config.set("source_key", "file_value", ConfigSource.File, "Test file source")
        self.assertEqual(self.config.get("source_key"), "file_value")
        
        # Verify source is preserved
        entry = self._find_entry("source_key")
        self.assertEqual(entry.source, ConfigSource.File)
    
    def testConfigClear(self):
        """Test clearing configuration"""
        self.config.set("temp_key", "temp_value", ConfigSource.RUNTIME)
        self.assertTrue(self.config.has("temp_key"))
        
        self.config.clear()
        self.assertEqual(len(self.config.get_keys()), 17)  # Back to defaults
        self.assertFalse(self.config.has("temp_key"))
    
    def testConfigSummary(self):
        """Test configuration summary"""
        self.config.set("summary_key", "summary_value", ConfigSource.RUNTIME)
        
        summary = self.config.summary()
        self.assertIn("Configuration (18 entries):", summary)
        self.assertIn("summary_key = summary_value", summary)
        self.assertIn("(2)", summary)  # Runtime source
    
    def _find_entry(self, key):
        """Helper method to find a config entry (internal)"""
        for entry in self.config._Config__entries.values():
            if entry.key == key:
                return entry
        return None

class TestGlobalConfig(unittest.TestCase):
    """Test cases for global configuration functions"""
    
    def testGlobalConfigFunctions(self):
        """Test global configuration convenience functions"""
        # Test setting and getting global config
        set_config_value("global_test", 999, ConfigSource.RUNTIME)
        value = get_config_value("global_test")
        self.assertEqual(value, 999)
        
        # Test default value
        default_value = get_config_value("nonexistent_global", 111)
        self.assertEqual(default_value, 111)
    
    def testGlobalConfigInstance(self):
        """Test that global config instance is consistent"""
        config1 = get_config()
        config2 = get_config()
        self.assertIs(config1, config2)  # Should be same instance

class TestConfigErrorHandling(unittest.TestCase):
    """Test error handling in configuration"""
    
    def testInvalidKeyHandling(self):
        """Test handling of invalid keys"""
        # Note: The Config class doesn't validate keys in set() method
        # This test ensures the interface is robust
        config = Config()
        
        # These should not raise exceptions (current behavior)
        config.set("", "value", ConfigSource.RUNTIME)  # Empty key
        config.set("key with spaces", "value", ConfigSource.RUNTIME)  # Spaces in key
        config.set("key=equals", "value", ConfigSource.RUNTIME)  # Equals in key
        
        # Valid key should still work
        config.set("valid_key", 123, ConfigSource.RUNTIME)
        self.assertEqual(config.get("valid_key"), 123)
    
    def testTypeHandling(self):
        """Test handling of different value types"""
        config = Config()
        
        # Test various types
        config.set("int_value", 42, ConfigSource.RUNTIME)
        config.set("float_value", 3.14, ConfigSource.RUNTIME)
        config.set("string_value", "hello", ConfigSource.RUNTIME)
        config.set("bool_value", True, ConfigSource.RUNTIME)
        
        self.assertEqual(config.get("int_value"), 42)
        self.assertAlmostEqual(config.get("float_value"), 3.14)
        self.assertEqual(config.get("string_value"), "hello")
        self.assertEqual(config.get("bool_value"), True)

class TestConfigFileOperations(unittest.TestCase):
    """Test file operations for configuration"""
    
    def testFileOperationsPlaceholder(self):
        """Placeholder test for file operations (currently not implemented)"""
        config = Config()
        
        # These methods currently return True/False without actual implementation
        # This test ensures they don't crash
        result = config.load_from_file("nonexistent_file.cfg")
        self.assertIsInstance(result, bool)
        
        result = config.save_to_file("test_config.cfg")
        self.assertIsInstance(result, bool)

class TestSimulationContext(unittest.TestCase):
    """Test simulation context functionality"""
    
    def testSimulationContextImport(self):
        """Test that simulation context can be imported"""
        # This test ensures the simulation context function exists
        from nlm import simulation_context
        
        # The function should exist even if the underlying modules are not available
        self.assertIsNotNone(simulation_context)

class TestErrorHandling(unittest.TestCase):
    """Test error handling utilities"""
    
    def testSimulationErrorClass(self):
        """Test that SimulationError exception exists"""
        from nlm import SimulationError
        
        # Create a simulation error
        error = SimulationError("Test error")
        self.assertEqual(str(error), "Test error")
        
        # Test exception inheritance
        self.assertTrue(issubclass(SimulationError, Exception))

if __name__ == '__main__':
    print("=== NLM Python Configuration Tests ===")
    
    # Run all tests
    suite = unittest.TestLoader().loadTestsFromTestCase(TestConfig)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestGlobalConfig))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestConfigErrorHandling))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestConfigFileOperations))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestSimulationContext))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestErrorHandling))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    if not result.wasSuccessful():
        sys.exit(1)
    else:
        print("\n=== All Python Configuration Tests PASSED ===")