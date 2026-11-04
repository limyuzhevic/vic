#!/usr/bin/env python3
"""
Python Bindings Example for NLM Unified Demo System
Demonstrates how to use the Python bindings to create and interact with NLM brains
"""

import sys
import os
import subprocess
import json

def print_usage():
    """Print usage information"""
    print("NLM Python Bindings Example")
    print("=" * 40)
    print("This script demonstrates how to use the Python bindings for NLM.")
    print("")
    print("Usage: python3 python_bindings_example.py [OPTIONS]")
    print("")
    print("Options:")
    print("  --help                Show this help message")
    print("  --test                Run Python bindings test")
    print("  --build               Build Python bindings")
    print("  --example CODE        Run specific example (config, brain, world)")
    print("")
    print("Examples:")
    print("  python3 python_bindings_example.py --test")
    print("  python3 python_bindings_example.py --example config")
    print("  python3 python_bindings_example.py --example brain")
    print("  python3 python_bindings_example.py --example world")
    print("")
    print("The Python bindings provide access to:")
    print("  - Config: Configuration management")
    print("  - Brain: Neural simulation")
    print("  - AgentBrain: Brain-world interface")
    print("  - SimpleWorld: Environment simulation")
    print("  - SensoryInput: Sensory processing")
    print("  - Action: Motor control")

def test_python_bindings():
    """Test if Python bindings are available and working"""
    print("\n" + "=" * 40)
    print("Testing Python Bindings")
    print("=" * 40)
    
    # Try to import pynlm
    try:
        import pynlm
        print("✓ Successfully imported pynlm module")
        
        # Test basic functionality
        print("\nTesting basic functionality:")
        
        # Test createDefaultConfig
        try:
            config = pynlm.createDefaultConfig()
            print("✓ createDefaultConfig() works")
            
            # Test config methods
            config.set("neuron_count", 100)
            config.set("random_seed", 42)
            print("✓ Config methods work")
            
            # Test config summary
            summary = config.summary()
            print(f"✓ Config summary: {summary[:50]}...")
            
        except Exception as e:
            print(f"✗ Config test failed: {e}")
            return False
        
        # Test createBrain
        try:
            brain = pynlm.createBrain(config)
            print("✓ createBrain() works")
            
            # Test brain methods
            brain.initialize()
            print("✓ brain.initialize() works")
            
            # Test brain status
            brain.logStatus()
            print("✓ brain.logStatus() works")
            
        except Exception as e:
            print(f"✗ Brain test failed: {e}")
            return False
        
        # Test createSimpleWorld
        try:
            world = pynlm.createSimpleWorld()
            print("✓ createSimpleWorld() works")
            
            # Test world methods
            world.configure(10.0, 10.0, 8, 8)
            print("✓ world.configure() works")
            
            # Test world state
            body = world.getAgentBody()
            print(f"✓ world.getAgentBody() works (x={body.x}, y={body.y})")
            
        except Exception as e:
            print(f"✗ World test failed: {e}")
            return False
        
        # Test createAgentBrain
        try:
            agent = pynlm.createAgentBrain(brain)
            print("✓ createAgentBrain() works")
            
            # Test agent methods
            agent.initialize(world)
            print("✓ agent.initialize() works")
            
            # Test agent capabilities
            print(f"✓ agent sensory input size: {agent.getSensoryInputSize()}")
            print(f"✓ agent motor output size: {agent.getMotorOutputSize()}")
            
        except Exception as e:
            print(f"✗ AgentBrain test failed: {e}")
            return False
        
        print("\n✓ All Python bindings tests passed!")
        return True
        
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        print("\nTo build Python bindings, run:")
        print("  cd /workspace/app")
        print("  pip install -e .")
        print("")
        print("Or build using scikit-build:")
        print("  pip install scikit-build-core pybind11")
        print("  python -m pip install -e .")
        return False

def run_example_config():
    """Run configuration example"""
    print("\n" + "=" * 40)
    print("Configuration Example")
    print("=" * 40)
    
    try:
        import pynlm
        
        print("Creating a new configuration:")
        config = pynlm.createDefaultConfig()
        
        print(f"Default simulation timestep: {config.getOr('simulation_timestep', 0.001)}")
        print(f"Default neuron count: {config.getOr('neuron_count', 500)}")
        print(f"Default region count: {config.getOr('region_count', 2)}")
        print(f"Default connection probability: {config.getOr('connection_probability', 0.1)}")
        
        print("\nSetting custom configuration:")
        config.set("neuron_count", 1000)
        config.set("random_seed", 12345)
        config.set("simulation_timestep", 0.002)
        
        print(f"Updated neuron count: {config.getOr('neuron_count', 0)}")
        print(f"Updated random seed: {config.getOr('random_seed', 0)}")
        
        print("\nConfiguration summary:")
        print(config.summary())
        
        print("\n✓ Configuration example completed successfully!")
        return True
        
    except ImportError:
        print("✗ Python bindings not available")
        return False
    except Exception as e:
        print(f"✗ Configuration example failed: {e}")
        return False

def run_example_brain():
    """Run brain example"""
    print("\n" + "=" * 40)
    print("Brain Example")
    print("=" * 40)
    
    try:
        import pynlm
        
        print("Creating brain with configuration:")
        config = pynlm.createDefaultConfig()
        
        # Customize configuration
        config.set("neuron_count", 200)
        config.set("random_seed", 42)
        
        print(f"Config: {config.getOr('neuron_count', 0)} neurons, seed {config.getOr('random_seed', 0)}")
        
        print("\nInitializing brain:")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("✓ Brain initialized successfully")
        
        # Get brain statistics
        print(f"\nBrain statistics:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        
        # Test simulation
        print(f"\nRunning simulation steps:")
        for step in range(5):
            brain.step(step, step * 0.001)
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        print("\n✓ Brain example completed successfully!")
        return True
        
    except ImportError:
        print("✗ Python bindings not available")
        return False
    except Exception as e:
        print(f"✗ Brain example failed: {e}")
        return False

def run_example_world():
    """Run world example"""
    print("\n" + "=" * 40)
    print("World Example")
    print("=" * 40)
    
    try:
        import pynlm
        
        print("Creating world:")
        world = pynlm.createSimpleWorld()
        world.configure(20.0, 20.0, 16, 16)
        world.setRandomSeed(42)
        
        print("✓ World configured successfully")
        
        # Get world state
        body = world.getAgentBody()
        print(f"\nWorld state:")
        print(f"  Agent position: ({body.x:.1f}, {body.y:.1f})")
        print(f"  Agent orientation: {body.orientation:.1f} degrees")
        print(f"  Agent energy: {body.energy:.1f}")
        print(f"  Agent health: {body.health:.1f}")
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        print(f"\nSensory percept:")
        print(f"  Vision dimensions: {len(percept.getVision())}")
        print(f"  Touch dimensions: {len(percept.getTouch())}")
        
        # Get world dimensions
        print(f"\nWorld dimensions:")
        print(f"  Width: {world.getWidth()}")
        print(f"  Height: {world.getHeight()}")
        
        print("\n✓ World example completed successfully!")
        return True
        
    except ImportError:
        print("✗ Python bindings not available")
        return False
    except Exception as e:
        print(f"✗ World example failed: {e}")
        return False

def run_example_agent():
    """Run agent example"""
    print("\n" + "=" * 40)
    print("Agent Example")
    print("=" * 40)
    
    try:
        import pynlm
        
        print("Creating brain and agent:")
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("random_seed", 42)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10.0, 10.0, 8, 8)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✓ Agent initialized successfully")
        
        # Get agent capabilities
        print(f"\nAgent capabilities:")
        print(f"  Sensory input size: {agent.getSensoryInputSize()}")
        print(f"  Motor output size: {agent.getMotorOutputSize()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        
        # Test sensory processing
        percept = world.getSensoryPercept()
        print(f"\nProcessing sensory input:")
        print(f"  Vision data size: {len(percept.getVision())}")
        print(f"  Touch data size: {len(percept.getTouch())}")
        
        # Process input through agent
        agent.processSensoryInput(percept)
        print("✓ Sensory input processed successfully")
        
        # Get motor command
        action = agent.decodeMotorCommand()
        print(f"  Motor command: {action}")
        
        print("\n✓ Agent example completed successfully!")
        return True
        
    except ImportError:
        print("✗ Python bindings not available")
        return False
    except Exception as e:
        print(f"✗ Agent example failed: {e}")
        return False

def build_python_bindings():
    """Build Python bindings"""
    print("\n" + "=" * 40)
    print("Building Python Bindings")
    print("=" * 40)
    
    print("To build Python bindings, run one of the following:")
    print("")
    print("1. Using pip (recommended):")
    print("   pip install -e .")
    print("")
    print("2. Using scikit-build:")
    print("   pip install scikit-build-core pybind11")
    print("   python -m pip install -e .")
    print("")
    print("3. Using CMake directly:")
    print("   mkdir build")
    print("   cd build")
    print("   cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON")
    print("   make -j4")
    print("")
    print("Note: You may need to install scikit-build-core and pybind11:")
    print("   pip install scikit-build-core pybind11")
    
    return True

def main():
    # Parse command line arguments
    if len(sys.argv) > 1:
        if sys.argv[1] == '--help':
            print_usage()
            return 0
        elif sys.argv[1] == '--test':
            return 0 if test_python_bindings() else 1
        elif sys.argv[1] == '--build':
            return 0 if build_python_bindings() else 1
        elif sys.argv[1] == '--example':
            if len(sys.argv) > 2:
                if sys.argv[2] == 'config':
                    return 0 if run_example_config() else 1
                elif sys.argv[2] == 'brain':
                    return 0 if run_example_brain() else 1
                elif sys.argv[2] == 'world':
                    return 0 if run_example_world() else 1
                elif sys.argv[2] == 'agent':
                    return 0 if run_example_agent() else 1
                else:
                    print(f"Unknown example: {sys.argv[2]}")
                    return 1
            else:
                print("Please specify an example type (config, brain, world, agent)")
                return 1
    
    # Default: print usage
    print_usage()
    return 0

if __name__ == '__main__':
    sys.exit(main())
