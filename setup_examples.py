#!/usr/bin/env python3
"""
Setup script for NLM enhanced Python API examples.

This script helps set up the environment and run the basic examples
to demonstrate the new enhanced Python bindings.
"""

import sys
import os
import subprocess
import platform

def check_python_version():
    """Check if Python version is compatible"""
    version = sys.version_info
    if version.major < 3 or (version.major == 3 and version.minor < 8):
        print("❌ ERROR: Python 3.8+ is required")
        return False
    print(f"✅ Python {version.major}.{version.minor}.{version.micro} is compatible")
    return True

def check_dependencies():
    """Check required dependencies"""
    required_packages = [
        "pybind11",
        "numpy", 
        "matplotlib",
        "scikit-learn"
    ]
    
    missing_packages = []
    for package in required_packages:
        try:
            __import__(package.replace("-", "_"))
            print(f"✅ {package} is available")
        except ImportError:
            missing_packages.append(package)
            print(f"❌ {package} is not available")
    
    if missing_packages:
        print(f"\n❌ Missing packages: {', '.join(missing_packages)}")
        print("\nTo install missing packages:")
        if platform.system() == "Windows":
            print("  pip install", " ".join(missing_packages))
        else:
            print("  pip install", " ".join(missing_packages))
        return False
    
    return True

def check_nlm_module():
    """Check if NLM module is available"""
    try:
        import pynlm
        print("✅ NLM module is available")
        
        # Check for enhanced functions
        enhanced_functions = [
            "run_simulation",
            "train_agent", 
            "create_trained_brain",
            "evaluate_brain",
            "save_model",
            "load_model",
            "export_weights",
            "enable_learning",
            "set_exploration_rate",
            "add_reward_callback",
            "plot_activity",
            "plot_connectivity",
            "plot_development",
            "get_performance_stats",
            "benchmark",
            "profile",
            "create_experimental_config",
            "create_production_config",
            "optimize_config",
            "analyze_patterns",
            "detect_anomalies",
            "summarize_performance",
            "validate_config",
            "check_brain_initialization",
            "safe_operation"
        ]
        
        available_functions = []
        for func in enhanced_functions:
            if hasattr(pynlm, func):
                available_functions.append(func)
        
        print(f"✅ {len(available_functions)}/{len(enhanced_functions)} enhanced functions available")
        
        if len(available_functions) < len(enhanced_functions):
            print(f"⚠️  Missing functions: {set(enhanced_functions) - set(available_functions)}")
        
        return True
    except ImportError as e:
        print(f"❌ NLM module not available: {e}")
        return False

def run_basic_examples():
    """Run the basic examples"""
    print("\n" + "="*60)
    print("Running Basic Examples...")
    print("="*60)
    
    example_path = os.path.join(os.path.dirname(__file__), "examples", "basic_examples.py")
    
    if not os.path.exists(example_path):
        print(f"❌ Example file not found: {example_path}")
        return False
    
    try:
        print(f"Running: {example_path}")
        result = subprocess.run([sys.executable, example_path], 
                              capture_output=True, text=True, timeout=60)
        
        if result.returncode == 0:
            print("✅ Examples completed successfully!")
            print("\n" + result.stdout)
        else:
            print(f"❌ Examples failed with error: {result.stderr}")
            
        return result.returncode == 0
    except subprocess.TimeoutExpired:
        print("❌ Examples timed out after 60 seconds")
        return False
    except Exception as e:
        print(f"❌ Error running examples: {e}")
        return False

def create_test_brain():
    """Create a simple test brain to verify functionality"""
    print("\n" + "="*60)
    print("Creating Test Brain...")
    print("="*60)
    
    try:
        import pynlm
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for i in range(50):
            brain.step(i)
        
        # Check basic properties
        print(f"✅ Brain created successfully")
        print(f"   Neurons: {brain.getTotalNeuronCount()}")
        print(f"   Synapses: {brain.getTotalSynapseCount()}")
        print(f"   Firing rate: {brain.getAverageFiringRate():.4f}")
        
        return True
    except Exception as e:
        print(f"❌ Error creating test brain: {e}")
        return False

def main():
    """Main setup and verification function"""
    print("="*60)
    print("NLM Enhanced Python API - Setup and Verification")
    print("="*60)
    
    checks = [
        ("Python Version", check_python_version),
        ("Dependencies", check_dependencies),
        ("NLM Module", check_nlm_module),
        ("Test Brain", create_test_brain),
    ]
    
    results = []
    for check_name, check_func in checks:
        print(f"\n🔍 Checking: {check_name}")
        result = check_func()
        results.append((check_name, result))
    
    print("\n" + "="*60)
    print("Summary")
    print("="*60)
    
    passed = sum(1 for _, result in results if result)
    total = len(results)
    
    for check_name, result in results:
        status = "✅ PASS" if result else "❌ FAIL"
        print(f"{status}: {check_name}")
    
    print(f"\nResults: {passed}/{total} checks passed")
    
    if passed == total:
        print("\n🎉 All checks passed! Ready to run examples.")
        
        # Ask if user wants to run examples
        try:
            run_examples = input("\nRun basic examples? (y/n): ").lower().strip()
            if run_examples in ['y', 'yes']:
                run_basic_examples()
        except KeyboardInterrupt:
            print("\n⚠️  Setup interrupted by user")
        except Exception as e:
            print(f"❌ Error running examples: {e}")
    else:
        print(f"\n❌ {total - passed} checks failed. Please fix the issues above.")
        
        if passed >= 2:  # If at least Python version and NLM module are working
            print("\n⚠️  Some non-critical checks failed, but you can still try running examples.")
            try:
                run_examples = input("Try running examples anyway? (y/n): ").lower().strip()
                if run_examples in ['y', 'yes']:
                    run_basic_examples()
            except:
                pass

if __name__ == "__main__":
    main()
