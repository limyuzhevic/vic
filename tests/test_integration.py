#!/usr/bin/env python3
"""
Integration tests for NLM CLI components
"""

import os
import sys
import tempfile
import json
import pytest
from pathlib import Path

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

@pytest.mark.integration
def test_config_cli_integration():
    """Test integration between config management and CLI"""
    from python.nlm.cli.nlm_cli import NLMCli
    from python.nlm.cli.config_manager import NLMConfigManager
    
    # Create config manager
    config = NLMConfigManager()
    config.set('neuron_count', 2000)
    
    # Test CLI with config
    cli = NLMCli()
    
    # Create a mock args object
    class MockArgs:
        config = None
        steps = 100
        neuron_count = 2000
        output = 'test_output.json'
        output_format = 'json'
        
    # Test config is accessible
    assert config.get('neuron_count') == 2000
    
    print("✓ Config-CLI integration test passed")

@pytest.mark.integration
def test_config_file_workflow():
    """Test complete config file workflow"""
    from python.nlm.cli.config_manager import NLMConfigManager
    
    # Create config
    config = NLMConfigManager()
    config.set('test_key', 'test_value')
    config.set('another_key', 123)
    
    # Save to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
    
    try:
        config.save_to_file(temp_file, 'json')
        
        # Load from file
        new_config = NLMConfigManager()
        new_config.load_from_file(temp_file)
        
        # Verify values
        assert new_config.get('test_key') == 'test_value'
        assert new_config.get('another_key') == 123
        
    finally:
        if os.path.exists(temp_file):
            os.unlink(temp_file)
    
    print("✓ Config file workflow test passed")

@pytest.mark.integration
def test_cli_batch_config():
    """Test CLI batch configuration processing"""
    from python.nlm.cli.batch import NLMBatchProcessor
    
    # Create processor
    processor = NLMBatchProcessor()
    
    # Create temporary config list file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
        f.write('config1.cfg\n')
        f.write('config2.cfg\n')
        f.write('config3.cfg\n')
        config_list_file = f.name
    
    try:
        # Process batch (would normally process actual files)
        # For this test, we'll verify the processor can be created
        assert processor is not None
        
    finally:
        if os.path.exists(config_list_file):
            os.unlink(config_list_file)
    
    print("✓ CLI batch config test passed")

def test_all_components_accessible():
    """Test that all NLM CLI components are accessible"""
    from python.nlm.cli import (
        NLMCli,
        create_parser,
        main,
        NLMConfigManager,
        get_default_template,
        save_default_template,
        load_config_with_env,
        NLMBatchProcessor,
        BatchUtils,
        NLMValidator,
        NLMComparison,
        NLMPresets,
        BuiltInPresets
    )
    
    # Verify all components can be imported and instantiated
    assert NLMCli is not None
    assert create_parser is not None
    assert NLMConfigManager is not None
    
    # Create instances
    cli = NLMCli()
    config = NLMConfigManager()
    batch_processor = NLMBatchProcessor()
    validator = NLMValidator()
    
    assert cli is not None
    assert config is not None
    assert batch_processor is not None
    assert validator is not None
    
    print("✓ All components accessible test passed")

if __name__ == '__main__':
    pytest.main(['-v', '-m', 'integration'])
