#!/usr/bin/env python3
"""
Pytest configuration and common test fixtures for NLM CLI tests
"""

import os
import sys
import tempfile
import pytest
from pathlib import Path
from unittest.mock import patch, MagicMock

# Add the python module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

@pytest.fixture
def temp_config_file():
    """Create a temporary configuration file"""
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write("""random_seed = 42
neuron_count = 1000
region_count = 2
connection_probability = 0.1""")
        temp_path = f.name
    
    yield temp_path
    
    # Cleanup
    if os.path.exists(temp_path):
        os.unlink(temp_path)

@pytest.fixture
def mock_pynlm():
    """Mock pynlm module for testing"""
    with patch.dict(sys.modules, {'pynlm': MagicMock()}):
        yield

@pytest.fixture
def sample_config():
    """Provide sample configuration data"""
    return {
        'random_seed': 42,
        'neuron_count': 1000,
        'region_count': 2,
        'connection_probability': 0.1,
        'plasticity_learning_rate': 0.01,
        'environment_name': 'GridWorld'
    }

@pytest.fixture
def cli_app():
    """Create NLM CLI application instance"""
    from python.nlm.cli.nlm_cli import NLMCli
    app = NLMCli()
    yield app

# Custom pytest configuration
if __name__ == '__main__':
    pytest.main(['-v', '--tb=short'])
