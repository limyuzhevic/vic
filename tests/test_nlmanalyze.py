#!/usr/bin/env python3
"""
Test NLMAnalyze tool
"""

import os
import sys
import tempfile
import json
import unittest
from pathlib import Path

# Add the tools directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'tools'))

from NLMAnalyze.main import NLMAnalyze

class TestNLMAnalyze(unittest.TestCase):
    def setUp(self):
        self.tool = NLMAnalyze()
        self.temp_dir = tempfile.mkdtemp()
    
    def tearDown(self):
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_parse_simple_config(self):
        content = """
neuron_count = 1000
region_count = 2
connection_probability = 0.1
"""
        config = self.tool._parse_simple_config(content)
        self.assertEqual(config["neuron_count"], 1000)
        self.assertEqual(config["region_count"], 2)
        self.assertEqual(config["connection_probability"], 0.1)
    
    def test_generate_sample_brain_data(self):
        config = {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
        }
        self.tool.config = config
        
        brain_data = self.tool._generate_sample_brain_data()
        self.assertIn("neuron_count", brain_data)
        self.assertIn("region_count", brain_data)
        self.assertIn("connection_probability", brain_data)
        self.assertEqual(brain_data["neuron_count"], 1000)
        self.assertEqual(brain_data["region_count"], 2)
    
    def test_analyze_connectivity(self):
        brain_data = {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "synapse_count": 200,
            "regional_activity": {"region_0": 1.0, "region_1": 0.8},
            "connectivity_matrix": [[0.1, 0.05], [0.05, 0.1]]
        }
        
        results = self.tool.analyze_connectivity(brain_data)
        self.assertIn("neuron_count", results)
        self.assertIn("region_count", results)
        self.assertIn("connection_probability", results)
        self.assertIn("expected_connections", results)
        self.assertIn("actual_connections", results)
        self.assertIn("connectivity_density", results)
        self.assertIn("regional_connectivity", results)
    
    def test_analyze_clustering(self):
        brain_data = {
            "neuron_count": 1000,
            "region_count": 2,
            "connectivity_matrix": [[0.1, 0.05], [0.05, 0.1]]
        }
        
        results = self.tool.analyze_clustering(brain_data)
        self.assertIn("communities", results)
        self.assertIn("modularity", results)
        self.assertIn("clustering_coefficient", results)
        self.assertIn("diameter", results)
        self.assertIn("radius", results)
        self.assertIn("small_world", results)
    
    def test_generate_report(self):
        brain_data = {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
        }
        self.tool.config = brain_data
        self.tool.analyze_connectivity(brain_data)
        self.tool.analyze_clustering(brain_data)
        
        report = self.tool.generate_report("text")
        self.assertIn("NLM Network Analysis Report", report)
        self.assertIn("connectivity", report)
        
        json_report = self.tool.generate_report("json")
        json_data = json.loads(json_report)
        self.assertIn("connectivity", json_data)
    
    def test_save_report(self):
        brain_data = {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
        }
        self.tool.config = brain_data
        self.tool.analyze_connectivity(brain_data)
        
        report_file = os.path.join(self.temp_dir, "analysis.txt")
        self.tool.save_report(report_file, "text")
        
        self.assertTrue(os.path.exists(report_file))
        with open(report_file, 'r') as f:
            content = f.read()
            self.assertIn("NLM Network Analysis Report", content)

if __name__ == '__main__':
    unittest.main()