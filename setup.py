#!/usr/bin/env python3
"""
Quick setup script for NLM Python bindings.

This script provides a simple way to install and test the NLM Python bindings
without requiring complex build processes.
"""

import subprocess
import sys
import os
import platform
from pathlib import Path

def print_header(title):
    print(f"\n{'=' * 60}")
    print(f"{title}")
    print(f"{'=' * 60}")

def run_command(cmd, cwd=None):
    try:
        print(f"Running: {' '.join(cmd)}")
        result = subprocess.run(cmd, cwd=cwd, check=True, 
                              capture_output=True, text=True)
        if result.stdout:
            print("Output:", result.stdout.strip())
        return True
    except subprocess.CalledProcessError as e:
        print(f"ERROR: Command failed with exit code {e.returncode}")
        if e.stderr:
            print("Error:", e.stderr.strip())
        return False

def main():
    print_header("NLM Python Bindings Setup")
    
    # Check Python version
    python_version = sys.version_info
    if python_version.major < 3 or (python_version.major == 3 and python_version.minor < 8):
        print("ERROR: Python 3.8 or higher is required")
        return 1
    
    print(f"Python version: {python_version.major}.{python_version.minor}.{python_version.micro}")
    
    # Check if we're in the NLM directory
    if not os.path.exists("pyproject.toml"):
        print("ERROR: pyproject.toml not found. Please run this script from the NLM root directory.")
        return 1
    
    # Install package
    print_header("Installing NLM Python bindings")
    if not run_command([sys.executable, "-m", "pip", "install", "-e", ".[dev]"]):
        return 1
    
    # Run quick tests
    print_header("Running quick functionality tests")
    
    # Test 1: Basic import
    test_code = '''
import pynlm
print(f"✓ Imported pynlm version: {pynlm.__version__}")

# Test basic functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")

# Test simulation
for i in range(10):
    brain.step(i)
print(f"✓ Simulated {i + 1} steps")
print(f"✓ Average firing rate: {brain.getAverageFiringRate():.3f} Hz")

# Test agent system
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
print(f"✓ Created agent with {agent.getSensoryInputSize()} sensory inputs")

# Test learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
print(f"✓ Enabled reward modulation: {agent.isRewardModulationEnabled()}")
print(f"✓ Enabled curiosity: {agent.isCuriosityEnabled()}")

print("\\nAll basic tests passed! 🎉")
'''
    
    if not run_command([sys.executable, "-c", test_code]):
        return 1
    
    # Show example usage
    print_header("Example Usage")
    print("""
You can now use NLM Python bindings with these examples:

1. Simple brain simulation:
   ```python
   import pynlm
   brain = pynlm.createBrain()
   brain.initialize()
   for i in range(100):
       brain.step(i)
   ```

2. Complete agent system:
   ```python
   import pynlm
   brain, world, agent = pynlm.create_advanced_agent()
   for step in range(50):
       world.update(0.1)
       agent.processSensoryInput(world.getSensoryPercept())
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())
   ```

3. Advanced training:
   ```python
   import pynlm
   brain, stats = pynlm.train_agent(10, {"brain.neuron_count": 2000})
   ```

4. Performance benchmarking:
   ```bash
   python -m benchmarks.performance_test
   ```

For more examples, see:
- examples/advanced_usage.py - Advanced usage patterns
- examples/tutorial.py - Step-by-step tutorial
- benchmarks/performance_test.py - Performance benchmarks
""")
    
    # Show next steps
    print_header("Next Steps")
    print("""
1. Run the test suite:
   pytest tests/ -v

2. Try advanced examples:
   python -m examples.advanced_usage

3. Build documentation:
   sphinx-build -b html docs/ docs/_build/html/

4. For more information:
   - Check the docs/API.md for complete API reference
   - Visit the NLM project documentation
   - Join the NLM community for support
""")
    
    print_header("Setup Complete!")
    print("\n🎉 NLM Python bindings are ready to use!")
    print("\nKey features available:")
    print("  • Enhanced Python API with comprehensive documentation")
    print("  • Advanced learning systems (reward, curiosity, development)")
    print("  • Memory systems (working, episodic, associative)")
    print("  • Prediction and planning modules")
    print("  • Neuromodulation systems")
    print("  • Performance monitoring and benchmarking")
    print("  • Comprehensive test suite")
    
    return 0

if __name__ == "__main__":
    exit(main())
