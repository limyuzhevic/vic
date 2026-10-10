#!/usr/bin/env python3
"""
Test: NLM Plugin System Unit Tests

This file contains unit tests for the NLM plugin system.
"""

import sys
import os
import json

# Add the plugin directory to the Python path
plugin_path = os.path.join(os.path.dirname(__file__), '..', 'plugin')
sys.path.insert(0, plugin_path)

# Import test framework
import unittest

class TestPluginConfig(unittest.TestCase):
    """Test PluginConfig class"""
    
    def test_config_creation(self):
        """Test creating a PluginConfig"""
        from plugin import PluginConfig
        config = PluginConfig()
        self.assertIsInstance(config, PluginConfig)
    
    def test_set_get_value(self):
        """Test setting and getting config values"""
        from plugin import PluginConfig
        config = PluginConfig()
        config.set_value("name", "test_plugin")
        self.assertEqual(config.get_value("name"), "test_plugin")
        self.assertTrue(config.has_value("name"))
        self.assertFalse(config.has_value("nonexistent"))
    
    def test_remove_value(self):
        """Test removing config values"""
        from plugin import PluginConfig
        config = PluginConfig()
        config.set_value("name", "test_plugin")
        config.remove_value("name")
        self.assertEqual(config.get_value("name"), "")
        self.assertFalse(config.has_value("name"))
    
    def test_serialize_deserialize(self):
        """Test serializing and deserializing config"""
        from plugin import PluginConfig
        config = PluginConfig()
        config.set_value("name", "test_plugin")
        config.set_value("version", "1.0.0")
        
        serialized = config.serialize()
        self.assertIn("name=test_plugin&", serialized)
        self.assertIn("version=1.0.0", serialized)
        
        # Create new config and deserialize
        config2 = PluginConfig()
        success = config2.deserialize(serialized)
        self.assertTrue(success)
        self.assertEqual(config2.get_value("name"), "test_plugin")
        self.assertEqual(config2.get_value("version"), "1.0.0")
    
    def test_validate(self):
        """Test config validation"""
        from plugin import PluginConfig
        config = PluginConfig()
        
        # Empty config should fail validation
        self.assertFalse(config.validate())
        
        # Set required fields
        config.set_value("name", "test_plugin")
        config.set_value("version", "1.0.0")
        
        # Now it should pass validation
        self.assertTrue(config.validate())

class TestPluginInfo(unittest.TestCase):
    """Test PluginInfo class"""
    
    def test_plugin_info_creation(self):
        """Test creating a PluginInfo"""
        from plugin import PluginInfo, PluginType
        info = PluginInfo("test_plugin", "1.0.0", "Test plugin", "Test Author",
                         "test_plugin.dll", "plugins", PluginType::BRAIN_REGION)
        self.assertEqual(info.get_name(), "test_plugin")
        self.assertEqual(info.get_version(), "1.0.0")
        self.assertEqual(info.get_description(), "Test plugin")
        self.assertEqual(info.get_author(), "Test Author")
        self.assertEqual(info.get_file_path(), "test_plugin.dll")
        self.assertEqual(info.get_directory(), "plugins")
        self.assertEqual(info.get_type(), PluginType::BRAIN_REGION)
    
    def test_plugin_info_validation(self):
        """Test PluginInfo validation"""
        from plugin import PluginInfo, PluginType
        
        # Empty fields should fail validation
        info = PluginInfo()
        self.assertFalse(info.getIsValid())
        
        # Set all required fields
        info.setName("test_plugin")
        info.setVersion("1.0.0")
        info.setDescription("Test plugin")
        info.setAuthor("Test Author")
        
        # Now it should be valid
        self.assertTrue(info.getIsValid())
    
    def test_plugin_info_comparison(self):
        """Test PluginInfo comparison operators"""
        from plugin import PluginInfo, PluginType
        
        info1 = PluginInfo("plugin1", "1.0.0", "Plugin 1", "Author", "plugin1.dll", "plugins", PluginType::BRAIN_REGION)
        info2 = PluginInfo("plugin1", "1.0.0", "Plugin 1", "Author", "plugin1.dll", "plugins", PluginType::BRAIN_REGION)
        info3 = PluginInfo("plugin2", "1.0.0", "Plugin 2", "Author", "plugin2.dll", "plugins", PluginType::BRAIN_REGION)
        
        self.assertEqual(info1, info2)
        self.assertNotEqual(info1, info3)
        self.assertLess(info1, info3)
    
    def test_plugin_info_info_string(self):
        """Test PluginInfo info string generation"""
        from plugin import PluginInfo, PluginType
        
        info = PluginInfo("test_plugin", "1.0.0", "Test plugin", "Test Author",
                         "test_plugin.dll", "plugins", PluginType::BRAIN_REGION)
        
        info_str = info.getInfoString()
        self.assertIn("Plugin: test_plugin", info_str)
        self.assertIn("Version: 1.0.0", info_str)
        self.assertIn("Description: Test plugin", info_str)
        self.assertIn("Author: Test Author", info_str)
        self.assertIn("Type: 0", info_str)

class TestPluginManager(unittest.TestCase):
    """Test PluginManager class"""
    
    def test_plugin_manager_creation(self):
        """Test creating a PluginManager"""
        from plugin import PluginManager
        manager = PluginManager()
        self.assertIsInstance(manager, PluginManager)
        self.assertEqual(manager.getPluginCount(), 0)
    
    def test_plugin_manager_registration(self):
        """Test registering plugins with PluginManager"""
        from plugin import PluginManager, PluginInfo, PluginType
        
        manager = PluginManager()
        
        # Create a test plugin
        plugin_info = PluginInfo("test_plugin", "1.0.0", "Test plugin", "Test Author",
                                "test_plugin.dll", "plugins", PluginType::BRAIN_REGION)
        
        # Create a mock plugin interface
        class MockPlugin:
            def getName(self):
                return "test_plugin"
            def getVersion(self):
                return "1.0.0"
            def getDescription(self):
                return "Test plugin"
            def getAuthor(self):
                return "Test Author"
            def getDependencies(self):
                return []
            def initialize(self, config):
                return True
            def shutdown(self):
                pass
            def isInitialized(self):
                return True
            def validate(self):
                return True
            def getErrorMessage(self):
                return ""
            def serialize(self):
                return ""
            def deserialize(self, data):
                return True
            def getPluginType(self):
                return PluginType::BRAIN_REGION
        
        mock_plugin = MockPlugin()
        
        # Register the plugin
        manager.registerPlugin("test_plugin", mock_plugin)
        self.assertEqual(manager.getPluginCount(), 1)
        self.assertIsNotNone(manager.getPlugin("test_plugin"))
        
        # Test getting plugin by type
        plugin = manager.getPlugin(PluginType::BRAIN_REGION)
        self.assertIsNotNone(plugin)
        self.assertEqual(plugin.getName(), "test_plugin")
        
        # Test getting all plugins
        all_plugins = manager.getAllPlugins()
        self.assertEqual(len(all_plugins), 1)
        
        # Test getting plugins by type
        brain_region_plugins = manager.getPlugins(PluginType::BRAIN_REGION)
        self.assertEqual(len(brain_region_plugins), 1)
    
    def test_plugin_manager_config(self):
        """Test PluginManager configuration"""
        from plugin import PluginManager
        
        manager = PluginManager()
        config = {"name": "test_plugin", "version": "1.0.0"}
        
        # Set config
        manager.setPluginConfig("test_plugin", config)
        
        # Get config
        retrieved_config = manager.getPluginConfig("test_plugin")
        self.assertEqual(retrieved_config.get_value("name"), "test_plugin")
        self.assertEqual(retrieved_config.get_value("version"), "1.0.0")

class TestPluginRegistry(unittest.TestCase):
    """Test PluginRegistry class"""
    
    def test_plugin_registry_creation(self):
        """Test creating a PluginRegistry"""
        from plugin import PluginRegistry
        registry = PluginRegistry()
        self.assertIsInstance(registry, PluginRegistry)
        self.assertEqual(registry.getPluginCount(), 0)
    
    def test_plugin_registry_registration(self):
        """Test registering plugins with PluginRegistry"""
        from plugin import PluginRegistry, PluginInfo, PluginType
        
        registry = PluginRegistry()
        
        # Create a test plugin info
        plugin_info = PluginInfo("test_plugin", "1.0.0", "Test plugin", "Test Author",
                                "test_plugin.dll", "plugins", PluginType::BRAIN_REGION)
        
        # Register the plugin
        success = registry.registerPlugin(plugin_info)
        self.assertTrue(success)
        self.assertEqual(registry.getPluginCount(), 1)
        
        # Find the plugin
        found_info = registry.findPlugin("test_plugin")
        self.assertEqual(found_info.get_name(), "test_plugin")
        self.assertEqual(found_info.get_version(), "1.0.0")
        
        # Find plugins by type
        brain_region_plugins = registry.findPlugins(PluginType::BRAIN_REGION)
        self.assertEqual(len(brain_region_plugins), 1)
        
        # Find plugins by directory
        plugins_in_dir = registry.findPlugins("plugins")
        self.assertEqual(len(plugins_in_dir), 1)
        
        # Unregister the plugin
        success = registry.unregisterPlugin("test_plugin")
        self.assertTrue(success)
        self.assertEqual(registry.getPluginCount(), 0)
    
    def test_plugin_registry_dependencies(self):
        """Test PluginRegistry dependency management"""
        from plugin import PluginRegistry, PluginInfo, PluginType
        
        registry = PluginRegistry()
        
        # Create plugins with dependencies
        plugin_info = PluginInfo("test_plugin", "1.0.0", "Test plugin", "Test Author",
                                "test_plugin.dll", "plugins", PluginType::BRAIN_REGION)
        plugin_info.addDependency("dependency1")
        plugin_info.addDependency("dependency2")
        
        # Register the plugin
        registry.registerPlugin(plugin_info)
        
        # Check dependencies
        dependencies = registry.getPluginDependencies("test_plugin")
        self.assertEqual(len(dependencies), 2)
        self.assertIn("dependency1", dependencies)
        self.assertIn("dependency2", dependencies)
        
        # Check if all dependencies are registered (should be false)
        self.assertFalse(registry.checkPluginDependencies("test_plugin"))
        
        # Register a dependency
        dependency_info = PluginInfo("dependency1", "1.0.0", "Dependency", "Author",
                                    "dependency1.dll", "plugins", PluginType::NEUROMODULATOR)
        registry.registerPlugin(dependency_info)
        
        # Now all dependencies should be registered
        self.assertTrue(registry.checkPluginDependencies("test_plugin"))

class TestPluginFactory(unittest.TestCase):
    """Test PluginFactory class"""
    
    def test_plugin_factory_creation(self):
        """Test creating a PluginFactory"""
        from plugin import PluginFactory
        factory = PluginFactory()
        self.assertIsInstance(factory, PluginFactory)
    
    def test_plugin_factory_config(self):
        """Test PluginFactory configuration"""
        from plugin import PluginFactory, PluginType
        
        # Test creating default config
        config = PluginFactory.createDefaultConfig(PluginType::BRAIN_REGION)
        self.assertTrue(config.hasValue("region_type"))
        self.assertTrue(config.hasValue("neuron_count"))
        self.assertTrue(config.hasValue("synapse_density"))
        
        # Test creating plugin config
        plugin_config = PluginFactory.createPluginConfig("test_plugin", "1.0.0")
        self.assertEqual(plugin_config.get_value("name"), "test_plugin")
        self.assertEqual(plugin_config.get_value("version"), "1.0.0")
        
        # Test getting config schema
        schema = PluginFactory.getPluginConfigSchema(PluginType::BRAIN_REGION)
        self.assertIsInstance(schema, list)
        self.assertTrue(len(schema) > 0)

class TestPluginLoader(unittest.TestCase):
    """Test PluginLoader class"""
    
    def test_plugin_loader_creation(self):
        """Test creating a PluginLoader"""
        from plugin import PluginLoader
        loader = PluginLoader()
        self.assertIsInstance(loader, PluginLoader)
    
    def test_plugin_file_detection(self):
        """Test plugin file detection"""
        from plugin import PluginLoader
        
        # Test with various file extensions
        self.assertTrue(PluginLoader.isPluginFile("plugin.dll"))
        self.assertTrue(PluginLoader.isPluginFile("plugin.so"))
        self.assertTrue(PluginLoader.isPluginFile("plugin.plugin"))
        self.assertFalse(PluginLoader.isPluginFile("plugin.txt"))
        self.assertFalse(PluginLoader.isPluginFile("plugin"))
        
        # Test file format detection
        # This would require actual files to test properly
        # self.assertEqual(PluginLoader.detectPluginFileFormat("plugin.dll"), PluginFileFormat::BINARY)

if __name__ == "__main__":
    unittest.main()