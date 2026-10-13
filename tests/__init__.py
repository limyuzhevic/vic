# Basic test for NLM Python bindings
# Run with: pytest tests/ -v

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm as nlm

def test_version():
    assert nlm.__version__ == "0.1.0"

def test_config_creation():
    config = nlm.Config()
    assert config is not None

def test_simple_world():
    world = nlm.SimpleWorld()
    assert world is not None
    world.configure(10, 10, 5, 5)
    assert world.getWidth() == 10

def test_factory_functions():
    config = nlm.createDefaultConfig()
    assert config is not None
    
    world = nlm.createSimpleWorld()
    assert world is not None
    
    brain = nlm.createBrain()
    assert brain is not None

if __name__ == "__main__":
    test_version()
    test_config_creation()
    test_simple_world()
    test_factory_functions()
    print("All tests passed!")
