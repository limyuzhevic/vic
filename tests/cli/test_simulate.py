#!/usr/bin/env python3
"""
CLI Simulate Tests - Test brain simulation command
"""

import sys
import os
import tempfile
import pytest
from unittest.mock import patch, MagicMock

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

def test_simulate_command_parsing():
    """Test simulate command argument parsing"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test minimal arguments
    args = parser.parse_args(['simulate', '--steps', '100'])
    assert args.command == 'simulate'
    assert args.steps == 100
    assert args.output is None
    
    # Test with output
    args = parser.parse_args(['simulate', '--steps', '500', '--output', 'results.json'])
    assert args.steps == 500
    assert args.output == 'results.json'
    
    print("✓ Simulate command parsing works")

def test_simulate_with_config():
    """Test simulate command with configuration"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test with config file
    args = parser.parse_args(['simulate', '--config', 'brain.cfg', '--steps', '200'])
    assert args.command == 'simulate'
    assert args.config == 'brain.cfg'
    assert args.steps == 200
    
    print("✓ Simulate with config works")

def test_simulate_output_formats():
    """Test different output formats for simulate command"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test text output (default)
    args = parser.parse_args(['simulate', '--steps', '100', '--format', 'text'])
    assert args.format == 'text'
    
    # Test JSON output
    args = parser.parse_args(['simulate', '--steps', '100', '--format', 'json'])
    assert args.format == 'json'
    
    # Test YAML output
    args = parser.parse_args(['simulate', '--steps', '100', '--format', 'yaml'])
    assert args.format == 'yaml'
    
    print("✓ Simulate output formats work")

def test_simulate_with_neuron_count():
    """Test simulate command with neuron count override"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    args = parser.parse_args(['simulate', '--steps', '100', '--neuron-count', '2000'])
    assert args.neuron_count == 2000
    
    print("✓ Simulate with neuron count works")

if __name__ == '__main__':
    test_simulate_command_parsing()
    test_simulate_with_config()
    test_simulate_output_formats()
    test_simulate_with_neuron_count()
    print("\nAll CLI simulate tests passed!")
