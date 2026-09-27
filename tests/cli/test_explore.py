#!/usr/bin/env python3
"""
CLI Explore Tests - Test brain exploration command
"""

import sys
import os
import tempfile
import pytest
from unittest.mock import patch, MagicMock

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

def test_explore_command_parsing():
    """Test explore command argument parsing"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    # Test minimal arguments
    args = parser.parse_args(['explore'])
    assert args.command == 'explore'
    assert args.file is None
    assert args.stats is False
    assert args.neurons is False
    assert args.synapses is False
    
    # Test with stats
    args = parser.parse_args(['explore', '--stats'])
    assert args.stats is True
    
    # Test with neurons
    args = parser.parse_args(['explore', '--neurons'])
    assert args.neurons is True
    
    # Test with synapses
    args = parser.parse_args(['explore', '--synapses'])
    assert args.synapses is True
    
    # Test with output format
    args = parser.parse_args(['explore', '--format', 'json'])
    assert args.format == 'json'
    
    print("✓ Explore command parsing works")

def test_explore_file_argument():
    """Test explore command with file argument"""
    from python.nlm.cli.nlm_cli import create_parser
    
    parser = create_parser()
    
    args = parser.parse_args(['explore', '--file', 'checkpoint.bin'])
    assert args.file == 'checkpoint.bin'
    
    print("✓ Explore file argument works")

def test_explore_stats_output():
    """Test explore command stats functionality"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    # Create a mock args object with stats
    class MockArgs:
        command = 'explore'
        file = None
        stats = True
        neurons = False
        synapses = False
        format = 'text'
        output_format = 'text'
    
    args = MockArgs()
    
    # Test that exploration with stats runs without error
    result = cli.run_explore(args)
    assert result == 0
    
    print("✓ Explore stats output works")

def test_explore_neurons_output():
    """Test explore command neurons listing"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    class MockArgs:
        command = 'explore'
        file = None
        stats = False
        neurons = True
        synapses = False
        format = 'text'
        output_format = 'text'
    
    args = MockArgs()
    
    result = cli.run_explore(args)
    assert result == 0
    
    print("✓ Explore neurons output works")

def test_explore_synapses_output():
    """Test explore command synapses listing"""
    from python.nlm.cli.nlm_cli import create_parser, NLMCli
    
    cli = NLMCli()
    
    class MockArgs:
        command = 'explore'
        file = None
        stats = False
        neurons = False
        synapses = True
        format = 'text'
        output_format = 'text'
    
    args = MockArgs()
    
    result = cli.run_explore(args)
    assert result == 0
    
    print("✓ Explore synapses output works")

if __name__ == '__main__':
    test_explore_command_parsing()
    test_explore_file_argument()
    test_explore_stats_output()
    test_explore_neurons_output()
    test_explore_synapses_output()
    print("\nAll CLI explore tests passed!")
