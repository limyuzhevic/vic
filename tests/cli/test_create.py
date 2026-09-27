#!/usr/bin/env python3
"""
CLI Create Tests - Test brain creation command
"""

import sys
import os
import tempfile
import json
import pytest
from unittest.mock import patch, MagicMock

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

def test_create_command_parsing():
    """Test create command argument parsing"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test minimal arguments
    args = parser.parse_args(['create'])
    assert args.command == 'create'
    assert args.config is None
    assert args.output is None
    assert args.neurons is None
    
    # Test with neurons
    args = parser.parse_args(['create', '--neurons', '1500'])
    assert args.neurons == 1500
    
    # Test with config
    args = parser.parse_args(['create', '--config', 'template.cfg'])
    assert args.config == 'template.cfg'
    
    # Test with output
    args = parser.parse_args(['create', '--output', 'newbrain.cfg'])
    assert args.output == 'newbrain.cfg'
    
    print("✓ Create command parsing works")

def test_create_config_file_output():
    """Test create command generates config files"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    parser = create_parser()
    cli = NLMCli()
    
    # Test config generation with different output formats
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        output_file = f.name
    
    try:
        # Create a mock args object
        class MockArgs:
            config = None
            output = output_file
            neurons = 1000
            output_format = 'text'
        
        # Run create command
        result = cli.run_create(MockArgs())
        assert result == 0
        
        # Check that file was created
        assert os.path.exists(output_file)
        
        # Check file content
        with open(output_file, 'r') as f:
            content = f.read()
            assert 'random_seed' in content
            assert 'neuron_count' in content
        
    finally:
        if os.path.exists(output_file):
            os.unlink(output_file)
    
    print("✓ Create command config file output works")

def test_create_json_output():
    """Test create command with JSON output"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        output_file = f.name
    
    try:
        class MockArgs:
            config = None
            output = output_file
            neurons = 2000
            output_format = 'json'
        
        result = cli.run_create(MockArgs())
        assert result == 0
        
        # Check JSON file
        with open(output_file, 'r') as f:
            data = json.load(f)
            assert 'random_seed' in data
            assert data.get('neuron_count') == 2000
        
    finally:
        if os.path.exists(output_file):
            os.unlink(output_file)
    
    print("✓ Create command JSON output works")

def test_create_yaml_output():
    """Test create command with YAML output"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        output_file = f.name
    
    try:
        class MockArgs:
            config = None
            output = output_file
            neurons = 1500
            output_format = 'yaml'
        
        result = cli.run_create(MockArgs())
        assert result == 0
        
        # Check YAML file (basic check)
        with open(output_file, 'r') as f:
            content = f.read()
            assert 'neuron_count' in content or 'neurona_count' in content
        
    finally:
        if os.path.exists(output_file):
            os.unlink(output_file)
    
    print("✓ Create command YAML output works")

def test_create_with_template():
    """Test create command using a template config file"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    # Create a temporary template config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write("""random_seed = 99
neuron_count = 500
region_count = 3""")
        template_file = f.name
    
    try:
        parser = create_parser()
        cli = NLMCli()
        
        class MockArgs:
            config = template_file
            output = None
            neurons = 0  # Use template value
            output_format = 'text'
        
        result = cli.run_create(MockArgs())
        assert result == 0
        
    finally:
        if os.path.exists(template_file):
            os.unlink(template_file)
    
    print("✓ Create command with template works")

if __name__ == '__main__':
    test_create_command_parsing()
    test_create_config_file_output()
    test_create_json_output()
    test_create_yaml_output()
    test_create_with_template()
    print("\nAll CLI create tests passed!")
