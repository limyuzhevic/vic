#!/usr/bin/env python3
"""
NLM Python Bindings - Setup Script

This script helps users install and set up the NLM Python bindings
for use in their projects.
"""

import sys
import os
import subprocess
import platform

def print_header():
    """Print the script header."""
    print("NLM Python Bindings - Setup Script")
    print("=" * 60)
    print()
    print("This script will help you install and set up the NLM Python bindings")
    print("for use in your Python projects.")
    print()

def check_python_version():
    """Check Python version and requirements."""
    print("1. Checking Python version...")
    
    # Check Python version
    python_version = sys.version_info
    print(f"   Python {python_version.major}.{python_version.minor}.{python_version.micro}")
    
    if python_version.major < 3 or (python_version.major == 3 and python_version.minor < 8):
        print("   WARNING: Python 3.8 or higher is recommended.")
    else:
        print("   ✓ Python version is compatible.")
    
    return True

def check_dependencies():
    """Check if required dependencies are available."""
    print("\n2. Checking dependencies...")
    
    dependencies = [
        ('pybind11', 'Pybind11 is required for the C++ bindings'),
        ('numpy', 'NumPy is recommended for scientific computing'),
        ('pytest', 'Pytest is recommended for testing')
    ]
    
    all_good = True
    
    for dep_name, dep_desc in dependencies:
        try:
            __import__(dep_name)
            print(f"   ✓ {dep_name} - {dep_desc}")
        except ImportError:
            print(f"   ✗ {dep_name} - {dep_desc}")
            all_good = False
    
    if not all_good:
        print("\n   WARNING: Some dependencies are missing.")
        print("   Please install them using: pip install pybind11 numpy pytest")
    else:
        print("   ✓ All dependencies are available.")
    
    return all_good

def build_bindings():
    """Build the Python bindings."""
    print("\n3. Building Python bindings...")
    
    # Check if we can build with scikit-build-core
    try:
        # Try to import scikit-build-core
        import scikit_build_core
        print("   ✓ scikit-build-core is available")
        
        # Try to build the package
        print("   Building package with scikit-build-core...")
        
        # Run pip install in development mode
        cmd = [sys.executable, '-m', 'pip', 'install', '-e', '.']
        result = subprocess.run(cmd, capture_output=True, text=True, cwd=os.path.dirname(__file__))
        
        if result.returncode == 0:
            print("   ✓ Successfully built and installed NLM Python bindings")
            return True
        else:
            print(f"   ✗ Failed to build: {result.stderr}")
            return False
            
    except ImportError:
        print("   ✗ scikit-build-core is not available")
        print("   Trying alternative build method...")
        
        # Try alternative method
        try:
            # Check if we're in a development environment
            if os.path.exists('setup.py'):
                cmd = [sys.executable, 'setup.py', 'build_ext', '--inplace']
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=os.path.dirname(__file__))
                
                if result.returncode == 0:
                    print("   ✓ Successfully built NLM Python bindings")
                    return True
                else:
                    print(f"   ✗ Alternative build failed: {result.stderr}")
                    return False
            else:
                print("   ✗ setup.py not found")
                return False
                
        except Exception as e:
            print(f"   ✗ Alternative build failed: {e}")
            return False

def create_test_environment():
    """Create a test environment and run basic tests."""
    print("\n4. Creating test environment...")
    
    try:
        # Try to import and run basic tests
        import importlib.util
        
        # Check if there are test files
        test_files = []
        for root, dirs, files in os.walk('.'):
            for file in files:
                if file.startswith('test_') and file.endswith('.py'):
                    test_files.append(os.path.join(root, file))
        
        if test_files:
            print(f"   Found {len(test_files)} test file(s)")
            
            # Try to run a simple test
            for test_file in test_files[:1]:  # Just test the first one
                try:
                    spec = importlib.util.spec_from_file_location(
                        os.path.splitext(os.path.basename(test_file))[0], test_file
                    )
                    test_module = importlib.util.module_from_spec(spec)
                    spec.loader.exec_module(test_module)
                    
                    print(f"   ✓ Successfully loaded test file: {test_file}")
                    break
                    
                except Exception as e:
                    print(f"   Warning: Could not load test file: {e}")
        else:
            print("   No test files found")
        
        # Create a simple test to verify bindings
        print("   Testing basic functionality...")
        
        # Try to import the main module
        try:
            import pynlm
            print("   ✓ Successfully imported pynlm")
            
            # Test basic functionality
            config = pynlm.createDefaultConfig()
            print("   ✓ Created default configuration")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            print(f"   ✓ Created and initialized brain with {brain.getTotalNeuronCount()} neurons")
            
            # Run a few steps
            for step in range(5):
                brain.step(step)
            
            print(f"   ✓ Successfully ran simulation steps, total spikes: {brain.getTotalSpikeCount()}")
            
            return True
            
        except Exception as e:
            print(f"   ✗ Basic functionality test failed: {e}")
            return False
            
    except Exception as e:
        print(f"   ✗ Test environment creation failed: {e}")
        return False

def show_usage():
    """Show usage examples."""
    print("\n5. Usage Examples")
    print("-" * 60)
    
    print("\nBasic Usage:")
    print("  import pynlm")
    print("  ")
    print("  # Create a brain with default configuration")
    print("  config = pynlm.createDefaultConfig()")
    print("  brain = pynlm.createBrain(config)")
    print("  brain.initialize()")
    print("  ")
    print("  # Run simulation")
    print("  for step in range(100):")
    print("      brain.step(step)")
    print("  ")
    print("  # Create a world and agent")
    print("  world = pynlm.createSimpleWorld()")
    print("  world.configure(width=20, height=20, visionWidth=8, visionHeight=8)")
    print("  agent = pynlm.createAgentBrain(brain)")
    print("  agent.initialize(world)")
    print("  ")
    print("  # Run agent episode")
    print("  for step in range(100):")
    print("      world.update(0.1)")
    print("      agent.processSensoryInput(world.getSensoryPercept())")
    print("      brain.step(step)")
    print("      action = agent.decodeMotorCommand()")
    print("      world.applyMotorCommand(action, world.getSimulationTime())")
    print()
    
    print("Convenience Functions:")
    print("  # Run complete agent episode with learning")
    print("  results = pynlm.run_agent_episode(steps=200, enable_features=True)")
    print("  ")
    print("  # Quick test")
    print("  brain = pynlm.quick_test()")
    print("  ")
    print("  # Create complete simulation")
    print("  sim = pynlm.create_simulation({'brain.neuron_count': 500})")
    print()
    
    print("Error Handling:")
    print("  try:")
    print("      config = pynlm.createDefaultConfig()")
    print("      brain = pynlm.createBrain(config)")
    print("      brain.initialize()")
    print("  except pynlm.NLMError as e:")
    print("      print(f'Error: {e}')")
    print()
    
    print("Examples Directory:")
    print("  python -m pynlm.examples.basic    # Basic examples")
    print("  python -m pynlm.examples.advanced # Advanced examples")
    print("  python -m pynlm.cli demo          # CLI demo")
    print("  python -m pynlm.cli tests         # Run tests")
    print()

def main():
    """Main function."""
    print_header()
    
    # Check system information
    print("System Information:")
    print(f"  Platform: {platform.system()} {platform.release()}")
    print(f"  Architecture: {platform.machine()}")
    print(f"  Python executable: {sys.executable}")
    print()
    
    # Run setup steps
    steps_completed = 0
    total_steps = 5
    
    # Step 1: Check Python version
    if check_python_version():
        steps_completed += 1
    
    # Step 2: Check dependencies
    if check_dependencies():
        steps_completed += 1
    
    # Step 3: Build bindings
    if build_bindings():
        steps_completed += 1
    
    # Step 4: Create test environment
    if create_test_environment():
        steps_completed += 1
    
    # Step 5: Show usage
    show_usage()
    
    # Summary
    print("\n" + "=" * 60)
    print("Setup Summary")
    print("=" * 60)
    print(f"Steps completed: {steps_completed}/{total_steps}")
    
    if steps_completed == total_steps:
        print("\n✓ All setup steps completed successfully!")
        print("\nThe NLM Python bindings are ready for use.")
        print("\nTo get started:")
        print("  1. Import the library: import pynlm")
        print("  2. Create a brain: brain = pynlm.createBrain(pynlm.createDefaultConfig())")
        print("  3. Initialize and run simulation: brain.initialize()")
        print("  4. Explore examples: python -m pynlm.examples.basic")
        return 0
    else:
        print(f"\n✗ {total_steps - steps_completed} step(s) failed.")
        print("\nPlease check the output above for errors and fix them.")
        print("\nCommon issues and solutions:")
        print("  - Missing dependencies: pip install pybind11 numpy pytest")
        print("  - Build errors: Ensure you have a C++ compiler installed")
        print("  - Permission errors: Try installing in a virtual environment")
        return 1

if __name__ == "__main__":
    sys.exit(main())
