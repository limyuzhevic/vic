#!/usr/bin/env python3
"""
CLI Experiment Tests - Test experiment command
"""

import sys
import os
import tempfile
import json
import pytest
from unittest.mock import patch, MagicMock

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

def test_experiment_command_parsing():
    """Test experiment command argument parsing"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test minimal arguments
    args = parser.parse_args(['experiment'])
    assert args.command == 'experiment'
    assert args.phase6 is False
    assert args.config is None
    assert args.output is None
    
    # Test with phase6
    args = parser.parse_args(['experiment', '--phase6'])
    assert args.phase6 is True
    
    # Test with config
    args = parser.parse_args(['experiment', '--config', 'exp_config.json'])
    assert args.config == 'exp_config.json'
    
    # Test with output
    args = parser.parse_args(['experiment', '--output', 'results.json'])
    assert args.output == 'results.json'
    
    print("✓ Experiment command parsing works")

def test_experiment_phase6_execution():
    """Test experiment phase 6 execution"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    class MockArgs:
        command = 'experiment'
        phase6 = True
        config = None
        output = None
        output_format = 'text'
    
    args = MockArgs()
    
    result = cli.run_experiment(args)
    assert result == 0
    
    print("✓ Experiment phase 6 execution works")

def test_experiment_with_config_file():
    """Test experiment with configuration file"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    # Create a temporary config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_data = {
            'experiment_name': 'test_exp',
            'parameters': {
                'steps': 100,
                'neuron_count': 1000
            }
        }
        json.dump(config_data, f)
        config_file = f.name
    
    try:
        cli = NLMCli()
        
        class MockArgs:
            command = 'experiment'
            phase6 = False
            config = config_file
            output = None
            output_format = 'text'
        
        args = MockArgs()
        
        result = cli.run_experiment(args)
        assert result == 0
        
    finally:
        if os.path.exists(config_file):
            os.unlink(config_file)
    
    print("✓ Experiment with config file works")

def test_experiment_json_output():
    """Test experiment JSON output"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        output_file = f.name
    
    try:
        class MockArgs:
            command = 'experiment'
            phase6 = True
            config = None
            output = output_file
            output_format = 'json'
        
        args = MockArgs()
        
        result = cli.run_experiment(args)
        assert result == 0
        
        # Verify JSON output
        with open(output_file, 'r') as f:
            data = json.load(f)
            assert 'experiment_type' in data
            assert 'timestamp' in data
        
    finally:
        if os.path.exists(output_file):
            os.unlink(output_file)
    
    print("✓ Experiment JSON output works")

if __name__ == '__main__':
    test_experiment_command_parsing()
    test_experiment_phase6_execution()
    test_experiment_with_config_file()
    test_experiment_json_output()
    print("\nAll CLI experiment tests passed!")
