#!/usr/bin/env python3
"""
Test script for NLM CLI
"""

import sys
import os
import tempfile
from pathlib import Path

# Add the CLI module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_cli_help():
    """Test CLI help functionality"""
    print("Testing CLI help...")
    
    from nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test help
    try:
        parser.parse_args(['--help'])
    except SystemExit:
        print("✓ Help command works")
    
    # Test version
    try:
        parser.parse_args(['--version'])
    except SystemExit:
        print("✓ Version command works")

def test_config_manager():
    """Test configuration manager functionality"""
    print("\nTesting configuration manager...")
    
    from nlm.cli.config_manager import NLMConfigManager
    
    config = NLMConfigManager()
    
    # Test default config
    defaults = config.defaults
    print(f"✓ Default config loaded with {len(defaults)} keys")
    
    # Test setting values
    config.set('test_key', 'test_value')
    assert config.get('test_key') == 'test_value'
    print("✓ Config set/get works")
    
    # Test list
    config_list = config.list()
    assert 'test_key' in config_list
    print("✓ Config list works")
    
    # Test save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        temp_file = f.name
    
    try:
        config.save_to_file(temp_file, 'cfg')
        new_config = NLMConfigManager()
        new_config.load_from_file(temp_file)
        assert new_config.get('test_key') == 'test_value'
        print("✓ Config save/load works")
    finally:
        os.unlink(temp_file)

def test_validation():
    """Test validation functionality"""
    print("\nTesting validation...")
    
    from nlm.cli.validation import NLMValidator
    
    validator = NLMValidator()
    
    # Test file validation
    valid, errors = validator.validate_file_path('nonexistent.txt', must_exist=True)
    assert not valid
    print("✓ File validation works")
    
    # Test config validation
    config = {'steps': 100, 'neuron_count': 1000, 'region_count': 2}
    valid, errors, warnings = validator.validate_config_values(config)
    assert valid
    print("✓ Config validation works")
    
    # Test command validation
    valid, errors, warnings = validator.validate_command_args('simulate', {'steps': 100})
    assert valid
    print("✓ Command validation works")

def test_presets():
    """Test preset functionality"""
    print("\nTesting presets...")
    
    from nlm.cli.presets import NLMPresets
    
    presets = NLMPresets()
    
    # Test listing presets
    preset_list = presets.list_presets()
    assert isinstance(preset_list, list)
    print(f"✓ Found {len(preset_list)} presets")
    
    # Test loading built-in presets
    built_in = NLMPresets().get_quick_test_config()
    assert 'random_seed' in built_in
    print("✓ Built-in presets work")

def main():
    """Run all tests"""
    print("NLM CLI Test Suite")
    print("=" * 50)
    
    try:
        test_cli_help()
        test_config_manager()
        test_validation()
        test_presets()
        
        print("\n" + "=" * 50)
        print("All tests passed! ✓")
        return 0
        
    except Exception as e:
        print(f"\nTest failed: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == '__main__':
    sys.exit(main())
