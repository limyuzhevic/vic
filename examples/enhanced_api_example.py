#!/usr/bin/env python3
"""
Example script demonstrating the enhanced NLM Python API.

This script showcases the new Pythonic features including:
- Context managers for resource management
- Fluent API and property-style access
- Configuration builders
- Simulation runners
- Enhanced error handling
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import time

def example_1_basic_usage():
    """Example 1: Basic usage with enhanced Brain interface"""
    print("=== Example 1: Basic Usage with Enhanced Brain ===")
    
    try:
        # Create brain using config builder (recommended approach)
        config = pynlm.ConfigBuilder().setBasicConfig().build()
        
        # Create enhanced PythonBrain with context manager
        with pynlm.PythonBrain(pynlm.createBrain(config)) as brain:
            print(f"Created brain with {brain.getNeuronCount()} neurons")
            
            # Use fluent API
            brain.initialize().step(0).step(1).step(2)
            
            # Access properties
            print(f"Active neurons: {brain.getActiveNeuronCount()}")
            print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            print(f"Developmental stage: {brain.getDevelopmentalStage()}")
            
            # Run multiple steps efficiently
            brain.runSteps(10, 0.1)
            
            print("✓ Basic usage example completed successfully\n")
            
    except Exception as e:
        print(f"✗ Error in Example 1: {e}\n")

def example_2_world_simulation():
    """Example 2: World simulation with enhanced interfaces"""
    print("=== Example 2: World Simulation with Enhanced Interfaces ===")
    
    try:
        # Create components using enhanced interfaces
        config = pynlm.ConfigBuilder().setBasicConfig().build()
        brain = pynlm.PythonBrain(pynlm.createBrain(config))
        world = pynlm.PythonWorld(pynlm.createSimpleWorld())
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setStart(10, 10)
        
        # Create agent
        agent = pynlm.PythonAgentBrain(pynlm.createAgentBrain(brain.getBrain()))
        
        # Use fluent API for initialization
        brain.initialize()
        world.reset()
        agent.initialize(world.getWorld())
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation using fluent API
        print("Running simulation steps...")
        for step in range(5):
            world.update(0.1)
            
            # Use fluent API
            agent.processInput(world.getPercept())
            brain.step(step, world.getSimulationTime())
            
            # Get action
            action = agent.decodeCommand()
            result = world.applyCommand(action, world.getSimulationTime())
            
            print(f"  Step {step}: {action}, Reward: {result.reward}, Success: {result.success}")
            
            if step % 5 == 4:  # Every 5 steps
                print(f"    Status - Neurons: {brain.getActiveNeuronCount()}, "
                      f"Firing: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.2f}")
        
        print("✓ World simulation example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Error in Example 2: {e}\n")

def example_3_simulation_runner():
    """Example 3: Using SimulationRunner helper"""
    print("=== Example 3: Using SimulationRunner Helper ===")
    
    try:
        # Create components
        config = pynlm.ConfigBuilder().setAdvancedConfig().build()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Create simulation runner
        runner = pynlm.SimulationRunner()
        
        # Configure world
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.setAgentStart(7, 7)
        
        # Create enhanced wrappers for better control
        py_brain = pynlm.PythonBrain(brain)
        py_world = pynlm.PythonWorld(world)
        py_agent = pynlm.PythonAgentBrain(agent)
        
        print("Running simulation with runner...")
        results = runner.runSimulation(
            py_brain.getBrain(),
            py_world.getWorld(),
            py_agent.getAgentBrain(),
            numSteps=20,
            timestep=0.1,
            enableLearning=True
        )
        
        # Analyze results
        total_reward = sum(r.reward for r in results)
        success_count = sum(1 for r in results if r.success)
        avg_time = sum(r.time for r in results) / len(results)
        
        print(f"Simulation completed!")
        print(f"  Total steps: {len(results)}")
        print(f"  Total reward: {total_reward:.2f}")
        print(f"  Success rate: {success_count/len(results)*100:.1f}%")
        print(f"  Average time: {avg_time:.2f}")
        print(f"  Final state - Neurons: {py_brain.getActiveNeuronCount()}, "
              f"Firing: {py_brain.getFiringNeuronCount()}")
        
        print("✓ Simulation runner example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Error in Example 3: {e}\n")

def example_4_configuration_builders():
    """Example 4: Using Configuration builders"""
    print("=== Example 4: Using Configuration Builders ===")
    
    try:
        print("Creating different configuration types...")
        
        # Basic config for testing
        basic_config = pynlm.ConfigBuilder().setBasicConfig().build()
        print(f"Basic config keys: {basic_config.getKeys()}")
        
        # Advanced config for research
        advanced_config = pynlm.ConfigBuilder().setAdvancedConfig().build()
        print(f"Advanced config has neuromodulation: {advanced_config.getOr('neuromodulation', False)}")
        
        # Exploration config for learning
        exploration_config = pynlm.ConfigBuilder().setExplorationConfig().build()
        print(f"Exploration config has curiosity: {exploration_config.getOr('curiosity_factor', 0)}")
        
        # Production config for stability
        production_config = pynlm.ConfigBuilder().setProductionConfig().build()
        print(f"Production config has stability: {production_config.getOr('stability_mode', False)}")
        
        # Custom config
        custom_config = pynlm.ConfigBuilder() \
            .setBasicConfig() \
            .setCustom("custom_param", 42) \
            .setCustom("learning_rate", 0.05) \
            .build()
        print(f"Custom config learning_rate: {custom_config.getOr('learning_rate', 0)}")
        
        print("✓ Configuration builders example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Error in Example 4: {e}\n")

def example_5_error_handling():
    """Example 5: Enhanced error handling"""
    print("=== Example 5: Enhanced Error Handling ===")
    
    try:
        print("Testing error handling...")
        
        # Test with invalid config
        config = pynlm.ConfigBuilder().setBasicConfig().build()
        brain = pynlm.PythonBrain(pynlm.createBrain(config))
        
        # Try to use uninitialized brain (should fail gracefully)
        try:
            brain.step(0)
            print("✗ Should have raised an error for uninitialized brain")
        except pynlm.InitializationError as e:
            print(f"✓ Caught expected InitializationError: {e}")
        
        # Test type validation
        neuron_id = pynlm.NeuronId(123)
        try:
            pynlm.validateNeuronId(neuron_id)
            print("✓ NeuronId validation passed")
        except Exception as e:
            print(f"✗ Unexpected error: {e}")
        
        # Test configuration validation
        empty_config = pynlm.Config()
        try:
            pynlm.validateConfig(empty_config)
            print("✗ Should have raised error for empty config")
        except pynlm.ConfigurationError as e:
            print(f"✓ Caught expected ConfigurationError: {e}")
        
        print("✓ Error handling example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Error in Example 5: {e}\n")

def example_6_visualization_helpers():
    """Example 6: Using VisualizationHelpers"""
    print("=== Example 6: Using VisualizationHelpers ===")
    
    try:
        print("Testing visualization helpers...")
        
        # Create some sample data
        times = [i * 0.1 for i in range(10)]
        firing_rates = [0.5 + 0.1 * (i % 5) for i in range(10)]
        spike_counts = [i * 2 for i in range(10)]
        
        # Create plots
        rate_plot = pynlm.VisualizationHelper.createFiringRatePlot(times, firing_rates)
        spike_plot = pynlm.VisualizationHelper.createSpikeCountPlot(times, spike_counts)
        
        print(f"Created firing rate plot: {rate_plot.title}")
        print(f"Created spike count plot: {spike_plot.title}")
        print(f"Plot data points: {len(rate_plot.x)}")
        
        # Test string encoding helpers
        test_text = "Hello World with Unicode: 🚀"
        utf8_text = pynlm.VisualizationHelper.toUtf8(test_text)
        bytes_data = pynlm.VisualizationHelper.toBytes(test_text)
        
        print(f"Original text: {test_text}")
        print(f"UTF-8 encoded: {utf8_text[:50]}...")
        print(f"Bytes encoded: {len(bytes_data)} bytes")
        
        print("✓ Visualization helpers example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Error in Example 6: {e}\n")

def example_7_complete_workflow():
    """Example 7: Complete workflow with all features"""
    print("=== Example 7: Complete Workflow with All Features ===")
    
    try:
        print("Setting up complete experiment...")
        
        # Use config builder for advanced setup
        config = pynlm.ConfigBuilder().setAdvancedConfig().build()
        
        # Create enhanced interfaces
        brain = pynlm.PythonBrain(pynlm.createBrain(config))
        world = pynlm.PythonWorld(pynlm.createSimpleWorld())
        
        # Configure world
        world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        world.setAgentStart(12, 12)
        
        # Create agent
        agent = pynlm.PythonAgentBrain(pynlm.createAgentBrain(brain.getBrain()))
        
        # Initialize everything with fluent API
        brain.initialize()
        world.reset()
        agent.initialize(world.getWorld())
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("Running complete workflow...")
        
        # Use simulation runner for better control
        runner = pynlm.SimulationRunner()
        results = runner.runSimulation(
            brain.getBrain(),
            world.getWorld(),
            agent.getAgentBrain(),
            numSteps=15,
            timestep=0.1,
            enableLearning=True
        )
        
        # Analyze and display results
        print(f"\nExperiment Results:")
        print(f"  Total steps: {len(results)}")
        print(f"  Average reward: {sum(r.reward for r in results)/len(results):.2f}")
        print(f"  Success rate: {sum(1 for r in results if r.success)/len(results)*100:.1f}%")
        print(f"  Average curiosity: {sum(r.time for r in results)/len(results):.2f}")
        
        # Show final state
        print(f"\nFinal State:")
        print(f"  Neurons active: {brain.getActiveNeuronCount()}")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
        
        print("✓ Complete workflow example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Error in Example 7: {e}\n")

def main():
    """Run all examples"""
    print("=" * 60)
    print("NLM Enhanced Python API Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_1_basic_usage,
        example_2_world_simulation,
        example_3_simulation_runner,
        example_4_configuration_builders,
        example_5_error_handling,
        example_6_visualization_helpers,
        example_7_complete_workflow,
    ]
    
    completed = 0
    for example in examples:
        try:
            example()
            completed += 1
        except Exception as e:
            print(f"✗ Example failed with exception: {e}\n")
    
    print("=" * 60)
    print(f"Summary: {completed}/{len(examples)} examples completed successfully")
    print("=" * 60)
    
    if completed == len(examples):
        print("All examples passed! The enhanced Python API is working correctly.")
        return 0
    else:
        print("Some examples failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
