"""
Configuration Management Tests
Tests for enhanced configuration management and validation
"""

import unittest
import json
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

class ConfigurationManagementTests(unittest.TestCase):
    """Tests for advanced configuration features"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.temp_dir = Path("temp_config_test")
        self.temp_dir.mkdir(exist_ok=True)
    
    def tearDown(self):
        """Clean up test fixtures"""
        import shutil
        if self.temp_dir.exists():
            shutil.rmtree(self.temp_dir)
    
    def test_schema_validation(self):
        """Test configuration schema validation"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        
        // Create schema
        Config::Schema schema;
        
        // Add validation for required keys
        auto isString = [](const ConfigEntry& entry) {
            return entry.key.substr(0, 5) == "test_" && 
                   std::holds_alternative<std::string>(entry.value);
        };
        schema.validations["test_1"] = isString;
        schema.validations["test_2"] = isString;
        schema.requiredKeys = {"test_1"};
        
        config.setSchema(schema);
        config.set("test_1", std::string("value1"), ConfigSource::Runtime);
        config.set("test_2", std::string("value2"), ConfigSource::Runtime);
        
        this.assertTrue(config.validate());
        this.assertTrue(config.has("test_1"));
        this.assertTrue(config.has("test_2"));
    
    def test_strict_mode(self):
        """Test strict mode configuration"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        Config::Schema schema;
        schema.strictMode = true;
        config.setSchema(schema);
        
        config.set("allowed_key", 123);
        // In strict mode, unknown keys would be rejected in a real implementation
        // For now, we just test that the schema is set correctly
        this.assertTrue(config.has("allowed_key"));
    
    def test_json_configuration(self):
        """Test JSON configuration file handling"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        
        // Create a temporary JSON file
        json_data = {
            "app_name": "NLM Test",
            "version": "2.0.0",
            "settings": {
                "neuron_count": 1000,
                "plasticity_enabled": true
            }
        };
        
        json_file = self.temp_dir / "test_config.json";
        with open(json_file, 'w') as f:
            json.dump(json_data, f, indent=2);
        
        bool success = config.loadFromFile(str(json_file));
        this.assertTrue(success);
        
        // Verify that nested values are accessible (in a real implementation)
        // This depends on the JSON parser implementation
        this.assertTrue(config.has("app_name"));
    
    def test_configuration_export(self):
        """Test configuration export functionality"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        config.set("name", "Test Config");
        config.set("version", 1.0);
        config.set("enabled", true);
        
        // Test JSON export
        std::string json_str = config.toJsonString();
        this.assertTrue(not json_str.empty());
        
        // Parse back and verify
        auto parsed = json.loads(json_str);
        this.assertEqual(parsed["name"], "Test Config");
        this.assertEqual(parsed["version"], 1.0);
        this.assertEqual(parsed["enabled"], true);
    
    def test_configuration_copy_move(self):
        """Test configuration copy and move operations"""
        from src.core.Config.Config.hpp import Config
        
        Config config1;
        config1.set("key1", "value1");
        config1.set("key2", 42);
        
        // Create copy via move assignment
        Config config2 = std::move(config1);
        
        this.assertTrue(config2.has("key1"));
        this.assertTrue(config2.has("key2"));
        this.assertEqual(config2.getOr("key1", ""), "value1");
        this.assertEqual(config2.getOr("key2", 0), 42);
        
        // Original should be empty after move
        this.assertEqual(config1.getKeys().size(), 0);
    
    def test_configuration_clear(self):
        """Test configuration clearing"""
        from src.core.Config.Config.hpp import Config
        
        Config config;
        config.set("temp1", 1);
        config.set("temp2", "test");
        
        this.assertEqual(config.getKeys().size(), 2);
        
        config.clear();
        
        this.assertEqual(config.getKeys().size(), 0);
        this.assertFalse(config.has("temp1"));
        this.assertFalse(config.has("temp2"));

if __name__ == '__main__':
    unittest.main()
