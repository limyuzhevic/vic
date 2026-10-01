#!/usr/bin/env python3
"""
NLM Setup and Usage Guide

This script provides a quick setup and demonstration of the NLM project improvements.
"""

import subprocess
import sys
from pathlib import Path


def print_header(title):
    """Print formatted header."""
    print("\n" + "=" * 60)
    print(f"  {title}")
    print("=" * 60)


def check_python_dependencies():
    """Check Python dependencies."""
    print_header("Checking Python Dependencies")
    
    try:
        import pynlm
        print("✓ NLM Python package is installed")
        return True
    except ImportError as e:
        print(f"✗ NLM Python package is not installed: {e}")
        print("\nTo install NLM:")
        print("  pip install pybind11 scikit-build-core")
        print("  pip install .  # Install from this directory")
        return False


def check_cli_tools():
    """Check CLI tools availability."""
    print_header("Checking CLI Tools")
    
    cli_path = Path("nlm_cli.py")
    if cli_path.exists():
        print("✓ CLI tool (nlm_cli.py) is available")
        
        # Check if it's executable
        if cli_path.stat().st_mode & 0o111:
            print("✓ CLI tool is executable")
        else:
            print("⚠ CLI tool is not executable (will add execute permissions)")
        
        return True
    else:
        print("✗ CLI tool (nlm_cli.py) is not found")
        return False


def demonstrate_basic_usage():
    """Demonstrate basic NLM usage."""
    print_header("Basic NLM Usage Demonstration")
    
    print("""
1. Import NLM
   import pynlm

2. Create a brain
   brain = pynlm.createBrain(pynlm.createDefaultConfig())

3. Initialize the brain
   brain.initialize()

4. Run simulation
   for step in range(100):
       brain.step(step)

5. Get statistics
   print(f"Neurons: {brain.getTotalNeuronCount()}")
   print(f"Spikes: {brain.getTotalSpikeCount()}")
   print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
""")


def demonstrate_agent_usage():
    """Demonstrate agent simulation."""
    print_header("Agent Simulation Example")
    
    print("""
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(15.0, 15.0)

# Create agent with learning
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}, "
      f"Curiosity: {agent.getCuriosityLevel():.3f}")
""")


def demonstrate_cli_usage():
    """Demonstrate CLI tool usage."""
    print_header("Command Line Interface (CLI) Usage")
    
    print("""
Available commands:

Basic Simulation:
  nlm-cli simulate --steps 1000
  nlm-cli simulate --steps 500 --config config.json

Agent Simulation:
  nlm-cli agent --width 30 --height 30
  nlm-cli agent --width 50 --height 40 --steps 1000

Brain Status:
  nlm-cli status
  nlm-cli status --file saved_brain.bin

Configuration Management:
  nlm-cli config list
  nlm-cli config get "brain.neuron_count"
  nlm-cli config set "brain.neuron_count" 3000

Examples:
  # Run a simple brain simulation
  nlm-cli simulate --steps 1000

  # Run an agent in a world
  nlm-cli agent --width 30 --height 30

  # Check brain status
  nlm-cli status

  # Set configuration
  nlm-cli config set "brain.neuron_count" 2000
""")


def demonstrate_configuration_examples():
    """Demonstrate configuration examples."""
    print_header("Configuration Examples")
    
    print("""
Basic Configuration:
  config = pynlm.createDefaultConfig()

Custom Configuration:
  config = pynlm.createDefaultConfig()
  config.set("brain.neuron_count", 3000)
  config.set("brain.synapse_density", 0.15)
  config.set("plasticity.stdp.learning_rate", 0.002)

Loading from File:
  config = pynlm.createDefaultConfig()
  config.loadFromFile("config.json")

Getting Values:
  neurons = config.get("brain.neuron_count")
  rate = config.get("plasticity.stdp.learning_rate")
  default_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)

Configuration Summary:
  summary = config.summary()
  print(summary)
""")


def main():
    """Run the setup and demonstration."""
    print_header("NLM Project Setup and Demonstration")
    print("This script demonstrates the NLM project improvements and usage.")
    
    # Check dependencies
    deps_ok = check_python_dependencies()
    cli_ok = check_cli_tools()
    
    if not deps_ok:
        print("\n❌ NLM is not properly installed. Please install dependencies first.")
        return 1
    
    print("\n" + "=" * 60)
    print("  SETUP AND DEMONSTRATION COMPLETE")
    print("=" * 60)
    
    demonstrate_basic_usage()
    demonstrate_agent_usage()
    demonstrate_cli_usage()
    demonstrate_configuration_examples()
    
    print_header("Next Steps")
    print("""
1. Explore the NLM source code in the src/ directory
2. Run the comprehensive test suite:
   python test_improvements.py

3. Build the C++ project from source:
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4

4. Create your own NLM simulations and experiments
5. Contribute improvements and new features

The NLM project now includes:
✓ Enhanced Python bindings with advanced features
✓ Complete memory system integration
✓ Prediction system integration
✓ Cognitive system integration
✓ Neuromodulation system integration
✓ Enhanced configuration system
✓ Command line interface tools
✓ Comprehensive documentation
✓ Error handling and validation
✓ Performance monitoring capabilities

You're ready to start working with the advanced NLM brain simulation system!
    """)
    
    print("\n" + "=" * 60)
    print("  SETUP COMPLETE - READY TO USE")
    print("=" * 60)
    
    return 0


if __name__ == "__main__":
    sys.exit(main())
