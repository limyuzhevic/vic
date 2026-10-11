#!/usr/bin/env python3
"""
Test NLMConfig tool
"""

import os
import sys
import tempfile
import unittest
from pathlib import Path

# Add the tools directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'tools'))

from NLMConfig.main import NLMConfig

class TestNLMConfig(unittest.TestCase):
    def setUp(self):
        self.tool = NLMConfig()
        self.temp_dir = tempfile.mkdtemp()
    
    def tearDown(self):
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_parse_simple_config(self):
        content = """
# Test configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.1
log_level = INFO
true_value = true
false_value = false
"""
        config = self.tool._parse_simple_config(content)
        self.assertEqual(config["neuron_count"], 1000)
        self.assertEqual(config["region_count"], 2)
        self.assertEqual(config["connection_probability"], 0.1)
        self.assertEqual(config["log_level"], "INFO")
        self.assertEqual(config["true_value"], True)
        self.assertEqual(config["false_value"], False)
    
    def test_generate_config(self):
        config = self.tool.generate_config()
        self.assertIn("neuron_count", config)
        self.assertIn("region_count", config)
        self.assertIn("connection_probability", config)
        self.assertEqual(config["neuron_count"], 1000)
        self.assertEqual(config["region_count"], 2)
    
    def test_validate_config(self):
        valid_config = {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
        }
        self.assertTrue(self.tool.validate_config(valid_config))
        
        invalid_config = {
            "neuron_count": -1,
            "region_count": 2,
            "connection_probability": 0.1
        }
        self.assertFalse(self.tool.validate_config(invalid_config))
    
    def test_save_and_load_config(self):
        config = {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
        }
        
        filepath = os.path.join(self.temp_dir, "test_config.cfg")
        self.tool.save_config(config, filepath)
        
        loaded_config = self.tool.load_config(filepath)
        self.assertEqual(loaded_config["neuron_count"], 1000)
        self.assertEqual(loaded_config["region_count"], 2)
        self.assertEqual(loaded_config["connection_probability"], 0.1)
    
    def test_parameter_info(self):
        info = self.tool.get_parameter_info("neuron_count", 1000)
        self.assertIn("network size", info)
        
        info = self.tool.get_parameter_info("unknown_param", "value")
        self.assertIn("General configuration", info)

if __name__ == '__main__':
    unittest.main()