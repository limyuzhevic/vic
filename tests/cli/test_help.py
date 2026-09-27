#!/usr/bin/env python3
"""
CLI Help Tests - Test command-line help and version information
"""

import sys
import os
import subprocess

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

def test_cli_help_command():
    """Test CLI help command"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test help
    try:
        parser.parse_args(['--help'])
        print("✓ Help command works")
    except SystemExit:
        pass
    
    # Test version
    try:
        parser.parse_args(['--version'])
        print("✓ Version command works")
    except SystemExit:
        pass

def test_cli_subcommand_help():
    """Test subcommand help"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test simulate help
    try:
        parser.parse_args(['simulate', '--help'])
        print("✓ Simulate subcommand help works")
    except SystemExit:
        pass
    
    # Test create help
    try:
        parser.parse_args(['create', '--help'])
        print("✓ Create subcommand help works")
    except SystemExit:
        pass

def test_cli_full_help_output():
    """Test that help contains expected content"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    help_text = parser.format_help()
    
    # Check for key help elements
    assert 'usage:' in help_text.lower()
    assert 'optional arguments:' in help_text.lower() or 'options:' in help_text.lower()
    assert 'nlm --help' in help_text or '--help' in help_text
    
    print("✓ Help output contains expected elements")

def test_cli_version_info():
    """Test version information"""
    from python.nlm.cli.nlm_cli import __version__
    
    assert __version__ is not None
    assert isinstance(__version__, str)
    assert len(__version__) > 0
    
    print(f"✓ Version info: {__version__}")

if __name__ == '__main__':
    test_cli_help_command()
    test_cli_subcommand_help()
    test_cli_full_help_output()
    test_cli_version_info()
    print("\nAll CLI help tests passed!")
