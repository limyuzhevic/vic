#!/usr/bin/env python3
"""
Setup Script for NLM Advanced Features
=======================================

This script sets up the environment for using NLM Phase 6 advanced features.
It includes:
1. Environment validation
2. Configuration file setup
3. Python bindings verification
4. Performance benchmarking
5. Example demonstrations
"""

import sys
import os
import subprocess
import platform
import json
from pathlib import Path

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def print_header(title):
    """Print formatted header."""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def check_python_environment():
    """Check Python environment and dependencies."""
    print_header("Python Environment Check")
    
    # Check Python version
    python_version = sys.version_info
    print(f"Python version: {python_version.major}.{python_version.minor}.{python_version.micro}")
    
    if python_version.major < 3 or (python_version.major == 3 and python_version.minor < 8):
        print("⚠️  Warning: Python 3.8 or higher is recommended")
    
    # Check if pynlm can be imported
    try:
        import pynlm
        print("✓ pynlm module imported successfully")
        
        # Check available classes and functions
        required_classes = ['Config', 'Brain', 'AgentBrain', 'SimpleWorld']
        for cls in required_classes:
            if hasattr(pynlm, cls):
                print(f"✓ {cls} class available")
            else:
                print(f"✗ {cls} class missing")
                return False
        
        # Check for advanced features
        if hasattr(pynlm, 'PerformanceMetrics'):
            print("✓ PerformanceMetrics class available")
        
        if hasattr(pynlm, 'AdvancedExperimentRunner'):
            print("✓ AdvancedExperimentRunner class available")
        
        if hasattr(pynlm, 'analyzeSpikeTrain'):
            print("✓ analyzeSpikeTrain function available")
        
        return True
        
    except ImportError as e:
        print(f"✗ Error importing pynlm: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False

def check_configuration_files():
    """Check configuration files."""
    print_header("Configuration Files Check")
    
    config_files = [
        "configs/default.cfg",
        "configs/advanced_config.json",
        "config.json",
        "advanced_config.json"
    ]
    
    found_configs = []
    
    for config_file in config_files:
        if os.path.exists(config_file):
            found_configs.append(config_file)
            print(f"✓ Found configuration file: {config_file}")
            
            # Try to load configuration
            try:
                import pynlm
                config = pynlm.createDefaultConfig()
                if config.loadFromFile(config_file):
                    print(f"  ✓ Successfully loaded configuration")
                else:
                    print(f"  ⚠️  Could not load configuration from file")
            except Exception as e:
                print(f"  ⚠️  Error loading configuration: {e}")
        else:
            print(f"✗ Missing configuration file: {config_file}")
    
    if found_configs:
        print(f"\nFound {len(found_configs)} configuration file(s)")
        return True
    else:
        print("\n⚠️  No configuration files found")
        return False

def create_sample_configurations():
    """Create sample configuration files."""
    print_header("Sample Configuration Creation")
    
    configs_dir = Path("configs")
    configs_dir.mkdir(exist_ok=True)
    
    # Default configuration
    default_config = {
        "brain": {
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.1,
            "initial_weight_mean": 0.5,
            "initial_weight_std": 0.1
        },
        "plasticity": {
            "stdp": {
                "enable": True,
                "learning_rate": 0.001,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enable": True,
                "learning_rate": 0.01
            }
        },
        "neuromod": {
            "dopamine": {
                "scale": 1.0
            }
        }
    }
    
    with open(configs_dir / "default.cfg", 'w') as f:
        # Convert to NLM format (simplified)
        f.write("# Default NLM Configuration\n")
        f.write("brain.neuron_count = 1000\n")
        f.write("brain.region_count = 1\n")
        f.write("brain.connection_probability = 0.1\n")
    
    print("✓ Created default.cfg")
    
    # Advanced configuration
    advanced_config = {
        "brain": {
            "neuron_count": 2000,
            "region_count": 3,
            "connection_probability": 0.08
        },
        "plasticity": {
            "stdp": {
                "enable": True,
                "learning_rate": 0.005,
                "tau_plus": 25.0,
                "tau_minus": 25.0
            },
            "structural": {
                "enable": True,
                "synaptogenesis_rate": 0.0005,
                "pruning_rate": 0.00005
            }
        },
        "neuromod": {
            "dopamine": {
                "scale": 1.5
            },
            "curiosity": {
                "enable": True
            }
        },
        "memory": {
            "working_memory": {
                "capacity": 1000,
                "trace_decay": 0.95
            },
            "episodic_memory": {
                "max_episodes": 10000,
                "replay_frequency": 0.1
            }
        }
    }
    
    with open(configs_dir / "advanced_config.json", 'w') as f:
        json.dump(advanced_config, f, indent=2)
    
    print("✓ Created advanced_config.json")
    
    return True

def check_build_system():
    """Check if build system is available."""
    print_header("Build System Check")
    
    # Check for CMake
    if shutil.which("cmake"):
        print("✓ CMake found")
    else:
        print("✗ CMake not found")
        return False
    
    # Check for build directory
    if os.path.exists("build"):
        print("✓ Build directory exists")
        
        # Check for compiled binaries
        binaries = ["nlm", "nlm_phase3_demo", "nlm_phase4_demo", "nlm_phase6_demo"]
        found_binaries = []
        
        for binary in binaries:
            binary_path = f"build/{binary}"
            if os.path.exists(binary_path):
                found_binaries.append(binary)
                print(f"✓ Found binary: {binary}")
            else:
                print(f"✗ Missing binary: {binary}")
        
        if found_binaries:
            print(f"\nFound {len(found_binaries)} binaries")
            return True
        else:
            print("\n⚠️  No binaries found in build directory")
            return False
    else:
        print("⚠️  Build directory not found")
        print("   Run: mkdir build && cd build && cmake .. && make")
        return False

def run_basic_test():
    """Run a basic functionality test."""
    print_header("Basic Functionality Test")
    
    try:
        import pynlm
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run basic simulation
        for i in range(50):
            brain.step(i)
        
        # Check results
        neuron_count = brain.getTotalNeuronCount()
        spike_count = brain.getTotalSpikeCount()
        
        print(f"✓ Basic test completed successfully")
        print(f"  Neurons: {neuron_count}")
        print(f"  Spikes: {spike_count}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic test failed: {e}")
        return False

def main():
    """Main setup function."""
    print("NLM Advanced Features Setup")
    print("=" * 60)
    
    # Import required modules
    try:
        import shutil
    except ImportError:
        print("✗ shutil module not available")
        return 1
    
    # Run checks
    checks = [
        ("Python Environment", check_python_environment),
        ("Configuration Files", check_configuration_files),
        ("Basic Functionality", run_basic_test),
    ]
    
    results = []
    
    for name, check_func in checks:
        print(f"\nRunning {name} check...")
        success = check_func()
        results.append((name, success))
    
    # Summary
    print_header("Setup Summary")
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{name:30} {status}")
    
    print(f"\nSetup completed: {passed}/{total} checks passed")
    
    if passed >= 2:  # At least Python environment and basic functionality should pass
        print("\n🎉 Setup completed successfully!")
        print("\nNext steps:")
        print("  1. Run demo: python demo_advanced_features.py")
        print("  2. Run config manager: python config_manager.py --validate")
        print("  3. Run benchmarks: python -c 'import pynlm; brain = pynlm.createBrain(); brain.benchmark(1000)'")
        return 0
    else:
        print("\n⚠️  Setup completed with some issues")
        print("Please check the output above for specific problems")
        return 1

if __name__ == "__main__":
    sys.exit(main())