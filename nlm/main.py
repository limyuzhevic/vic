"""
NLM Python Package Main Entry Point

This module provides the main entry point for the NLM Python package,
including command-line interface and utility functions.
"""

import sys
import argparse
from typing import Optional
from . import Brain, Agent, SimpleWorld, ConfigManager, example_usage


def main():
    """Main entry point for command-line interface."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Python Package",
        prog="nlm-python"
    )
    
    parser.add_argument(
        "--version", action="store_true",
        help="Show version information"
    )
    
    parser.add_argument(
        "--example", action="store_true",
        help="Run example usage"
    )
    
    parser.add_argument(
        "--test", action="store_true",
        help="Run basic tests"
    )
    
    args = parser.parse_args()
    
    if args.version:
        print(f"NLM Python Package v{sys.modules[__name__].__version__}")
        print("Authors: NLM Authors")
        print("Description:", sys.modules[__name__].__description__)
    
    elif args.example:
        example_usage()
    
    elif args.test:
        test_basic_functionality()
    
    else:
        parser.print_help()


def test_basic_functionality():
    """Test basic functionality of the package."""
    print("Testing basic NLM Python functionality...")
    
    try:
        # Test 1: Configuration
        print("1. Testing configuration...")
        config = ConfigManager()
        config.set("brain.neuron_count", 100)
        config.set("brain.connection_probability", 0.1)
        print(f"   ✓ Configuration created with {config.keys()} keys")
        
        # Test 2: Brain
        print("2. Testing brain creation...")
        brain = Brain(config)
        print(f"   ✓ Brain created with {brain.get_total_neuron_count()} neurons")
        
        # Test 3: World
        print("3. Testing world creation...")
        world = SimpleWorld()
        world.configure(width=10, height=10)
        print(f"   ✓ World created with dimensions {world.get_width()}x{world.get_height()}")
        
        # Test 4: Agent
        print("4. Testing agent creation...")
        agent = Agent(brain)
        print(f"   ✓ Agent created")
        
        print("\n✓ All basic tests passed!")
        
    except Exception as e:
        print(f"\n✗ Test failed: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
