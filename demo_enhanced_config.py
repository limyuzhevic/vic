#!/usr/bin/env python3
"""
NLM Demo Scripts

These scripts demonstrate the improved NLM configuration system and
provide examples of how to use the enhanced features.
"""

import subprocess
import sys
import os

def demo_basic_usage():
    """Demonstrate basic NLM usage with configuration files."""
    print("=== NLM Basic Usage Demo ===\n")
    
    print("1. Running NLM with basic configuration...")
    print("   Command: ./nlm --config=configs/basic.cfg")
    
    # Check if NLM executable exists
    if not os.path.exists("./nlm"):
        print("\n   WARNING: NLM executable not found.")
        print("   Please build the project first:")
        print("   mkdir build && cd build && cmake .. && make -j4")
        return False
    
    try:
        result = subprocess.run(
            ["./nlm", "--config=configs/basic.cfg"],
            capture_output=True,
            text=True,
            timeout=30
        )
        
        print(f"\n   Execution {'SUCCEEDED' if result.returncode == 0 else 'FAILED'}")
        print(f"   Return code: {result.returncode}")
        
        if result.returncode == 0:
            print("\n   Key outputs from the demo:")
            lines = result.stdout.split('\n')
            for line in lines:
                if any(keyword in line.lower() for keyword in 
                      ['learning detected', 'spikes generated', 'total spikes', 'weight statistics']):
                    print(f"     {line}")
        else:
            print(f"\n   Error output:\n{result.stderr}")
            
    except subprocess.TimeoutExpired:
        print("\n   ERROR: NLM execution timed out (30 seconds)")
        return False
    except Exception as e:
        print(f"\n   ERROR: Failed to run NLM: {e}")
        return False
    
    return True

def demo_configuration_examples():
    """Demonstrate different configuration file formats."""
    print("\n=== Configuration Examples Demo ===\n")
    
    print("1. Available configuration files in configs/:")
    
    configs_dir = "configs"
    if os.path.exists(configs_dir):
        for filename in sorted(os.listdir(configs_dir)):
            filepath = os.path.join(configs_dir, filename)
            if os.path.isfile(filepath):
                print(f"\n   {filename}:")
                
                # Show first few lines of the file
                with open(filepath, 'r') as f:
                    lines = f.readlines()[:10]  # First 10 lines
                    
                for line in lines:
                    print(f"     {line.rstrip()}")
                
                if len(lines) < 10:
                    print("     ...")
    else:
        print("   configs/ directory not found")
    
    print("\n2. Configuration format examples:")
    print("\n   Basic Format (configs/basic.cfg):")
    print("     neuron_count = 100")
    print("     region_count = 1")
    print("     simulation_timestep = 0.001")
    print("     random_seed = 42")
    
    print("\n   JSON Format (configs/agent_training.json):")
    print("     {\\")
    print("       \\"neuron_count\\": 1500,")
    print("       \\"region_count\\": 2,")
    print("       \\"simulation_timestep\\": 0.001,")
    print("       ...")
    print("     }")
    
    return True

def demo_python_bindings():
    """Demonstrate enhanced Python bindings."""
    print("\n=== Python Bindings Demo ===\n")
    
    print("1. Enhanced Python API features:")
    print("\n   - Configuration validation:")
    print("     config.validate()  # Throws exception if invalid")
    print("     config.validate(strict=False)  # Logs warnings")
    
    print("\n   - Type-safe getters:")
    print("     neuron_count = config.getInt('neuron_count')")
    print("     random_seed = config.getInt('random_seed', 42)")
    print("     timestep = config.getFloat('simulation_timestep')")
    
    print("\n   - Required key access:")
    print("     required_value = config.getRequired('key')")
    
    print("\n   - Configuration management:")
    print("     config.loadFromFile('config.json')")
    print("     config.saveToFile('my_config.json')")
    print("     config.clear()  # Reset configuration")
    
    print("\n2. Convenience functions:")
    print("\n   - Create brain with default config:")
    print("     brain = pynlm.createBrain()")
    
    print("\n   - Create complete agent system:")
    print("     world, agent_brain = pynlm.createAgentSystem(640, 480)")
    
    print("\n   - Configuration templates:")
    print("     config = pynlm.createDefaultConfig()")
    print("     config.set('neuron_count', 1000)")
    
    return True

def demo_command_line_options():
    """Demonstrate command line options."""
    print("\n=== Command Line Options Demo ===\n")
    
    print("1. Available command line options:")
    print("\n   --help, -h")
    print("     Show help message with usage examples")
    
    print("\n   --config <file>")
    print("     Load configuration from file (JSON or key=value format)")
    print("     Examples:")
    print("       --config=configs/basic.cfg")
    print("       --config=advanced_config.json")
    
    print("\n2. Configuration file support:")
    print("\n   Simple key=value format:")
    print("     random_seed = 123")
    print("     neuron_count = 500")
    print("     simulation_timestep = 0.001")
    
    print("\n   JSON format:")
    print("     {")
    print("       \"random_seed\": 123,")
    print("       \"neuron_count\": 500,")
    print("       \"simulation_timestep\": 0.001")
    print("     }")
    
    print("\n3. Runtime configuration override:")
    print("\n   You can override any configuration parameter at runtime:")
    print("   ./nlm --config=configs/base.cfg --neuron_count=1000 --region_count=3")
    
    return True

def demo_configuration_validation():
    """Demonstrate configuration validation features."""
    print("\n=== Configuration Validation Demo ===\n")
    
    print("1. Configuration validation scenarios:")
    print("\n   Strict validation (throws exception):")
    print("     try:")
    print("         config.validate()")
    print("         print('Configuration is valid')")
    print("     except Exception as e:")
    print("         print(f'Validation failed: {e}')")
    
    print("\n   Non-strict validation (logs warnings):")
    print("     config.validate(strict=False)  # Logs warnings for issues")
    
    print("\n   Type checking:")
    print("     if config.has('neuron_count'):")
    print("         count = config.getInt('neuron_count')")
    print("         if count <= 0:")
    print("             raise ValueError('neuron_count must be positive')")
    
    print("\n   Required parameter access:")
    print("     try:")
    print("         required_param = config.getRequired('essential_param')")
    print("         print(f'Required parameter: {required_param}')")
    print("     except Exception as e:")
    print("         print(f'Missing required parameter: {e}')")
    
    return True

def main():
    print("NLM (熙然) Enhanced Configuration System Demo")
    print("=" * 60)
    
    demos = [
        ("Basic Usage", demo_basic_usage),
        ("Configuration Examples", demo_configuration_examples),
        ("Python Bindings", demo_python_bindings),
        ("Command Line Options", demo_command_line_options),
        ("Configuration Validation", demo_configuration_validation),
    ]
    
    for demo_name, demo_func in demos:
        print(f"\n{'=' * 60}")
        
        try:
            if not demo_func():
                print(f"\n⚠️  {demo_name} demo failed or was skipped")
                
        except Exception as e:
            print(f"\n❌ Error in {demo_name} demo: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'=' * 60}")
    print("\nDemo completed!")
    print("\nNext steps:")
    print("1. Build NLM: mkdir build && cd build && cmake .. && make -j4")
    print("2. Create example configs: python config_examples.py --create-examples")
    print("3. Test configurations: python config_examples.py --test-config configs/basic.cfg")
    print("4. Compare configurations: python config_examples.py --compare configs/basic.cfg configs/agent_training.json")
    
    return True

if __name__ == "__main__":
    main()
