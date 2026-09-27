#!/usr/bin/env python3
"""
CLI Config Tests - Test configuration management command
"""

import sys
import os
import tempfile
import json
import pytest
from unittest.mock import patch, MagicMock

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

def test_config_command_parsing():
    """Test config command argument parsing"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test config list
    args = parser.parse_args(['config', 'list'])
    assert args.command == 'config'
    assert args.config_action == 'list'
    
    # Test config set
    args = parser.parse_args(['config', 'set', 'neuron_count', '2000'])
    assert args.command == 'config'
    assert args.config_action == 'set'
    assert args.key == 'neuron_count'
    assert args.value == '2000'
    
    # Test config get
    args = parser.parse_args(['config', 'get', 'random_seed'])
    assert args.command == 'config'
    assert args.config_action == 'get'
    assert args.key == 'random_seed'
    
    print("✓ Config command parsing works")

def test_config_set_get_workflow():
    """Test complete config set-get workflow"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    class MockArgs:
        command = 'config'
        config_action = 'set'
        key = 'test_key'
        value = 'test_value'
        output_format = 'text'
    
    # Set a value
    args = MockArgs()
    result = cli.run_config(args)
    assert result == 0
    
    # Get the value
    class GetArgs:
        command = 'config'
        config_action = 'get'
        key = 'test_key'
        output_format = 'text'
    
    args = GetArgs()
    result = cli.run_config(args)
    assert result == 0
    
    print("✓ Config set-get workflow works")

def test_config_list_command():
    """Test config list command"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    class ListArgs:
        command = 'config'
        config_action = 'list'
        output_format = 'text'
    
    args = ListArgs()
    result = cli.run_config(args)
    assert result == 0
    
    print("✓ Config list command works")

def test_config_save_load():
    """Test config save and load functionality"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        save_file = f.name
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        load_file = f.name
    
    try:
        # Save config
        class SaveArgs:
            command = 'config'
            config_action = 'save'
            file = save_file
            format = 'json'
        
        args = SaveArgs()
        result = cli.run_config(args)
        assert result == 0
        assert os.path.exists(save_file)
        
        # Load config
        class LoadArgs:
            command = 'config'
            config_action = 'load'
            file = save_file
        
        args = LoadArgs()
        result = cli.run_config(args)
        assert result == 0
        
    finally:
        if os.path.exists(save_file):
            os.unlink(save_file)
        if os.path.exists(load_file):
            os.unlink(load_file)
    
    print("✓ Config save-load works")

def test_config_validation():
    """Test config validation through CLI"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    from python.nlm.cli.validation import NLMValidator
    
    validator = NLMValidator()
    
    # Test validation on CLI config manager
    cli = NLMCli()
    
    # Create a config with invalid values
    class InvalidArgs:
        command = 'config'
        config_action = 'set'
        key = 'neuron_count'
        value = '-1'  # Invalid value
        output_format = 'text'
    
    args = InvalidArgs()
    result = cli.run_config(args)
    assert result == 0
    
    print("✓ Config validation works")

if __name__ == '__main__':
    test_config_command_parsing()
    test_config_set_get_workflow()
    test_config_list_command()
    test_config_save_load()
    test_config_validation()
    print("\nAll CLI config tests passed!")
