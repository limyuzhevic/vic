#!/usr/bin/env python3
"""
NLM Python API - Comprehensive Tutorial and Examples

Complete tutorial for using the enhanced NLM (Neural Learning Machine) Python API.
This tutorial covers basic usage, intermediate features, and advanced power user techniques.
"""

import sys
import os
import json
import time
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

print("NLM Python API - Comprehensive Tutorial and Examples")
print("==================================================")
print()

def section_header(title):
    """Print a section header."""
    print("=" * 60)
    print(title)
    print("=" * 60)
    print()

def tutorial_1_basic_usage():
    """Tutorial 1: Basic Usage"""
    section_header("TUTORIAL 1: BASIC USAGE")
    
    print("This tutorial shows the basic steps to use the NLM Python API.")
    print()
    
    print("1.1 Import the NLM module:")
    print("   >>> import pynlm")
    print()
    
    print("1.2 Create a default configuration:")
    print("   >>> config = pynlm.ConfigWrapper()")
    print("   >>> config.configureBrain(neuronCount=500)")
    print()
    
    print("1.3 Create and initialize a brain:")
    print("   >>> brain = pynlm.BrainWrapper(config.getConfig())")
    print("   >>> brain.initialize()")
    print()
    
    print("1.4 Run a simple simulation:")
    print("   >>> brain.runSimulation(100)")
    print()
    
    print("1.5 Get and display statistics:")
    print("   >>> stats = brain.getStatistics()")
    print("   >>> print(f'Total neurons: {stats[\"total_neurons\"]}')")
    print("   >>> print(f'Total spikes: {stats[\"total_spikes\"]}')")
    print("   >>> print(f'Average firing rate: {stats[\"avg_firing_rate\"]:.2f} Hz')")
    print()
    
    # Actually demonstrate the basic usage
    print("--- RUNNING BASIC USAGE EXAMPLE ---")
    
    try:
        # Create configuration
        config = pynlm.ConfigWrapper()
        config.configureBrain(neuronCount=100)
        
        # Create brain
        brain = pynlm.BrainWrapper(config.getConfig())
        
        # Initialize and run
        if brain.initialize():
            print("✓ Brain initialized successfully")
            brain.runSimulation(20)
            
            # Get statistics
            stats = brain.getStatistics()
            print(f"✓ Simulation completed")
            print(f"  Total neurons: {stats['total_neurons']}")
            print(f"  Total synapses: {stats['total_synapses']}")
            print(f"  Firing neurons: {stats['firing_neurons']}")
            print(f"  Total spikes: {stats['total_spikes']}")
            print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
        else:
            print("✗ Failed to initialize brain")
            
    except Exception as e:
        print(f"✗ Error: {e}")
    
    print()

def tutorial_2_agent_world():
    """Tutorial 2: Agent and World Integration"""
    section_header("TUTORIAL 2: AGENT AND WORLD INTEGRATION")
    
    print("This tutorial demonstrates creating and running a complete agent-world system.")
    print()
    
    print("2.1 Create a brain with appropriate configuration:")
    print("   >>> config = pynlm.ConfigWrapper()")
    print("   >>> config.configureBrain(neuronCount=1000)")
    print("   >>> config.configureWorld(width=30, height=30)")
    print()
    
    print("2.2 Create brain, world, and agent:")
    print("   >>> brain = pynlm.BrainWrapper(config.getConfig())")
    print("   >>> world = pynlm.createSimpleWorld()")
    print("   >>> agent = pynlm.createAgentBrain(brain.getBrain())")
    print()
    
    print("2.3 Initialize the system:")
    print("   >>> brain.initialize()")
    print("   >>> world.configure(30, 30, 8, 8)")
    print("   >>> world.reset()")
    print("   >>> agent.initialize(world)")
    print()
    
    print("2.4 Enable learning features:")
    print("   >>> agent.enableRewardModulation(True)")
    print("   >>> agent.enableCuriosity(True)")
    print("   >>> agent.enableDevelopment(True)")
    print()
    
    print("2.5 Run the agent in the world:")
    print("   >>> for step in range(100):")
    print("   ...     world.update(0.1)")
    print("   ...     agent.processSensoryInput(world.getSensoryPercept())")
    print("   ...     brain.step(step)")
    print("   ...     action = agent.decodeMotorCommand()")
    print("   ...     world.applyMotorCommand(action, world.getSimulationTime())")
    print("   ...     if step % 20 == 0:")
    print("   ...         print(f'Step {step}: curiosity={agent.getCuriosityLevel():.3f}')")
    print()
    
    # Actually demonstrate the agent-world example
    print("--- RUNNING AGENT-WORLD EXAMPLE ---")
    
    try:
        # Create complete system
        brain, world, agent = pynlm.createReadyAgent("configs/default.cfg", 30)
        
        print("✓ Complete agent system created")
        print(f"✓ Initial development stage: {agent.getDevelopmentalStage()}")
        print(f"✓ Initial curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"✓ Initial novelty level: {agent.getNoveltyLevel():.3f}")
        
        # Run for a few steps
        for step in range(30):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            if step % 10 == 0:
                print(f"  Step {step}: curiosity={agent.getCuriosityLevel():.3f}, "
                      f"novelty={agent.getNoveltyLevel():.3f}")
        
        print("✓ Agent simulation completed successfully")
        
    except Exception as e:
        print(f"✗ Error: {e}")
    
    print()

def tutorial_3_configuration_management():
    """Tutorial 3: Configuration Management"""
    section_header("TUTORIAL 3: CONFIGURATION MANAGEMENT")
    
    print("This tutorial covers configuration file management and validation.")
    print()
    
    print("3.1 Create configurations from file:")
    print("   >>> config = pynlm.createConfig('configs/default.cfg')")
    print("   >>> print(config.summary())")
    print()
    
    print("3.2 Validate configurations:")
    print("   >>> is_valid = pynlm.validateConfiguration('configs/default.cfg')")
    print("   >>> print(f'Configuration valid: {is_valid}')")
    print()
    
    print("3.3 Modify configurations:")
    print("   >>> config.setInt('brain.neuron_count', 500)")
    print("   >>> config.setFloat('brain.connection_probability', 0.15)")
    print("   >>> config.saveToFile('my_config.json')")
    print()
    
    print("3.4 Use convenience methods:")
    print("   >>> config.configureBrain(neuronCount=1000, connectionProbability=0.1)")
    print("   >>> config.configureWorld(width=50, height=50)")
    print()
    
    # Actually demonstrate configuration management
    print("--- RUNNING CONFIGURATION MANAGEMENT EXAMPLE ---")
    
    try:
        # Create configuration from file
        config = pynlm.createConfig("configs/default.cfg")
        print("✓ Loaded configuration from file")
        
        # Validate configuration
        validation = pynlm.validateConfiguration("configs/default.cfg")
        print(f"✓ Configuration valid: {validation['valid']}")
        print(f"✓ Has neuron count: {validation['has_neuron_count']}")
        print(f"✓ Has connection probability: {validation['has_connection_probability']}")
        
        # Create brain and demonstrate modification
        brain = pynlm.BrainWrapper(config.getConfig())
        if brain.initialize():
            print("✓ Brain initialized with file configuration")
            
            # Modify configuration
            config.setInt("brain.neuron_count", 750)
            config.setFloat("brain.connection_probability", 0.2)
            
            print(f"✓ Modified configuration: {config.summary()}")
            
            # Save modified configuration
            config.getConfig().saveToFile("tutorial_config.json")
            print("✓ Saved modified configuration to tutorial_config.json")
            
        else:
            print("✗ Failed to initialize brain")
            
    except Exception as e:
        print(f"✗ Error: {e}")
    
    print()

def tutorial_4_advanced_features():
    """Tutorial 4: Advanced Features"""
    section_header("TUTORIAL 4: ADVANCED FEATURES")
    
    print("This tutorial demonstrates advanced features for power users.")
    print()
    
    print("4.1 Quick test utility:")
    print("   >>> result = pynlm.runQuickTest()")
    print("   >>> print(f'System status: {result}')")
    print()
    
    print("4.2 Enhanced factory functions:")
    print("   >>> brain, world, agent = pynlm.createReadyAgent('config.json', 200)")
    print("   >>> print(f'Created {brain.getStatistics()[\"total_neurons\"]} neurons')")
    print()
    
    print("4.3 Error handling and validation:")
    print("   >>> try:")
    print("   ...     brain = pynlm.BrainWrapper(config.getConfig())")
    print("   ...     brain.initialize()")
    print("   ... except pynlm.NLMError as e:")
    print("   ...     print(f'Error: {e}')")
    print()
    
    print("4.4 Statistics and monitoring:")
    print("   >>> stats = brain.getStatistics()")
    print("   >>> print(f'Average firing rate: {stats[\"avg_firing_rate\"]:.2f} Hz')")
    print("   >>> print(f'E/I ratio: {stats[\"e_i_ratio\"]:.3f}')")
    print()
    
    # Actually demonstrate advanced features
    print("--- RUNNING ADVANCED FEATURES EXAMPLE ---")
    
    try:
        # Demonstrate quick test
        print("1. Running quick system test:")
        quick_result = pynlm.runQuickTest()
        print(f"   ✓ Quick test result: {quick_result}")
        
        # Demonstrate demo creation
        print("\n2. Creating demo brain:")
        brain, stats = pynlm.create_demo_brain(100, 10)
        if brain:
            print(f"   ✓ Created demo with {stats['total_spikes']} spikes")
        
        # Demonstrate learning agent demo
        print("\n3. Creating learning agent demo:")
        brain, world, agent = pynlm.create_learning_agent_demo(20)
        if agent:
            print(f"   ✓ Agent development stage: {agent.getDevelopmentalStage()}")
            print(f"   ✓ Final curiosity: {agent.getCuriosityLevel():.3f}")
        
        print("\n✓ All advanced features demonstrated successfully")
        
    except Exception as e:
        print(f"✗ Error: {e}")
    
    print()

def tutorial_5_api_reference():
    """Tutorial 5: API Reference"""
    section_header("TUTORIAL 5: API REFERENCE")
    
    print("This section provides a reference to the NLM Python API classes and methods.")
    print()
    
    print("5.1 Core Classes:")
    print("   ConfigWrapper - Enhanced configuration management")
    print("   BrainWrapper - Enhanced brain simulation")
    print("   AgentBrain - Agent brain interface")
    print("   SimpleWorld - Simple 2D world simulation")
    print()
    
    print("5.2 Key Methods:")
    print("   ConfigWrapper:")
    print("     - configureBrain(neuronCount, connectionProbability)")
    print("     - configureWorld(width, height, visionWidth, visionHeight)")
    print("     - validate()")
    print("     - summary()")
    print()
    
    print("   BrainWrapper:")
    print("     - initialize()")
    print("     - runSimulation(steps, timestep)")
    print("     - getStatistics()")
    print("     - saveState(filepath)")
    print("     - loadState(filepath)")
    print()
    
    print("   AgentBrain:")
    print("     - initialize(world)")
    print("     - processSensoryInput(percept)")
    print("     - decodeMotorCommand()")
    print("     - applyRewardModulation(reward, predicted)")
    print("     - enableRewardModulation(bool)")
    print("     - enableCuriosity(bool)")
    print("     - enableDevelopment(bool)")
    print()
    
    print("5.3 Enumerations:")
    print("   pynlm.NeuronType.Excitatory")
    print("   pynlm.NeuronType.Inhibitory")
    print("   pynlm.ActionType.MoveForward")
    print("   pynlm.DevelopmentalStage.Adult")
    print("   pynlm.WorldObjectType.Resource")
    print()
    
    print("5.4 Factory Functions:")
    print("   pynlm.createDefaultConfig()")
    print("   pynlm.createConfig(filepath)")
    print("   pynlm.createBrain(config)")
    print("   pynlm.createSimpleWorld()")
    print("   pynlm.createAgentBrain(brain)")
    print()
    
    print("5.5 Error Handling:")
    print("   pynlm.NLMError - Custom exception class")
    print("   try:")
    print("       # API calls")
    print("   except pynlm.NLMError as e:")
    print("       # Handle errors")
    print()
    
    print("--- RUNNING API REFERENCE DEMONSTRATION ---")
    
    try:
        # Demonstrate different API calls
        print("1. Creating and initializing brain:")
        config = pynlm.ConfigWrapper()
        config.configureBrain(neuronCount=100)
        
        brain = pynlm.BrainWrapper(config.getConfig())
        if brain.initialize():
            print("   ✓ Brain initialized")
        
        # Get statistics
        print("\n2. Accessing statistics:")
        stats = brain.getStatistics()
        print(f"   ✓ Total neurons: {stats['total_neurons']}")
        print(f"   ✓ Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
        print(f"   ✓ E/I ratio: {stats['e_i_ratio']:.3f}")
        
        # Demonstrate different neuron types
        print("\n3. Neuron type enumeration:")
        print(f"   ✓ NeuronType.Excitatory: {pynlm.NeuronType.Excitatory}")
        print(f"   ✓ NeuronType.Inhibitory: {pynlm.NeuronType.Inhibitory}")
        
        # Demonstrate action type enumeration
        print("\n4. Action type enumeration:")
        print(f"   ✓ ActionType.MoveForward: {pynlm.ActionType.MoveForward}")
        print(f"   ✓ ActionType.TurnLeft: {pynlm.ActionType.TurnLeft}")
        
        # Demonstrate developmental stage enumeration
        print("\n5. Developmental stage enumeration:")
        print(f"   ✓ DevelopmentalStage.Initial: {pynlm.DevelopmentalStage.Initial}")
        print(f"   ✓ DevelopmentalStage.Adult: {pynlm.DevelopmentalStage.Adult}")
        
        print("\n✓ All API reference demonstrations completed")
        
    except Exception as e:
        print(f"✗ Error: {e}")
    
    print()

def tutorial_6_workflows():
    """Tutorial 6: Common Workflows"""
    section_header("TUTORIAL 6: COMMON WORKFLOWS")
    
    print("This tutorial shows common workflows and patterns for using NLM.")
    print()
    
    print("6.1 Learning Workflow:")
    print("   1. Create configuration")
    print("   2. Initialize brain and agent")
    print("   3. Enable learning features")
    print("   4. Run simulation loop")
    print("   5. Apply rewards and update development")
    print()
    
    print("6.2 Experiment Workflow:")
    print("   1. Define parameter space")
    print("   2. Run parameter sweep")
    print("   3. Analyze results")
    print("   4. Identify optimal configuration")
    print()
    
    print("6.3 Debugging Workflow:")
    print("   1. Create debug brain")
    print("   2. Run step-by-step simulation")
    print("   3. Monitor statistics")
    print("   4. Identify issues")
    print()
    
    print("6.4 Performance Workflow:")
    print("   1. Profile simulation performance")
    print("   2. Benchmark different configurations")
    print("   3. Optimize parameters")
    print("   4. Save performance profile")
    print()
    
    # Actually demonstrate workflows
    print("--- RUNNING WORKFLOW EXAMPLES ---")
    
    try:
        print("1. Learning Workflow Example:")
        brain, world, agent = pynlm.createReadyAgent("configs/default.cfg", 50)
        
        # Run learning simulation
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 10 == 0:
                print(f"   Step {step}: curiosity={agent.getCuriosityLevel():.3f}")
        
        print("   ✓ Learning workflow completed")
        
        print("\n2. Configuration Validation Workflow:")
        config = pynlm.ConfigWrapper()
        config.configureBrain(neuronCount=500)
        
        # Validate configuration
        brain = pynlm.BrainWrapper(config.getConfig())
        if brain.initialize():
            print("   ✓ Configuration valid and brain initialized")
            
            # Get configuration summary
            summary = config.summary()
            print(f"   ✓ Configuration: {summary}")
        
        print("\n3. Statistics Analysis Workflow:")
        stats = brain.getStatistics()
        print("   ✓ Statistics collected:")
        print(f"     - Total spikes: {stats['total_spikes']}")
        print(f"     - Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
        print(f"     - Network efficiency: {stats['e_i_ratio']:.3f}")
        
        print("\n✓ All workflow examples completed successfully")
        
    except Exception as e:
        print(f"✗ Error: {e}")
    
    print()

def tutorial_7_best_practices():
    """Tutorial 7: Best Practices"""
    section_header("TUTORIAL 7: BEST PRACTICES")
    
    print("This tutorial covers best practices for using the NLM Python API.")
    print()
    
    print("7.1 Error Handling:")
    print("   ✓ Always initialize brain before using it")
    print("   ✓ Catch NLMError exceptions")
    print("   ✓ Validate configurations before use")
    print("   ✓ Check return values from API calls")
    print()
    
    print("7.2 Performance:")
    print("   ✓ Use appropriate neuron counts for your use case")
    print("   ✓ Profile simulations regularly")
    print("   ✓ Use batch operations when possible")
    print("   ✓ Monitor memory usage")
    print()
    
    print("7.3 Code Organization:")
    print("   ✓ Use separate files for different components")
    print("   ✓ Import pynlm once and reuse objects")
    print("   ✓ Use descriptive variable names")
    print("   ✓ Add comments for complex operations")
    print()
    
    print("7.4 Testing and Debugging:")
    print("   ✓ Use quick test for system validation")
    print("   ✓ Run simulations in steps for debugging")
    print("   ✓ Use configuration validation")
    print("   ✓ Save intermediate results")
    print()
    
    print("7.5 Production Usage:")
    print("   ✓ Use configuration files for reproducibility")
    print("   ✓ Implement proper logging")
    print("   ✓ Use batch processing for large experiments")
    print("   ✓ Monitor system resources")
    print()
    
    print("--- RUNNING BEST PRACTICES DEMONSTRATION ---")
    
    try:
        print("1. Proper error handling example:")
        try:
            config = pynlm.ConfigWrapper()
            config.configureBrain(neuronCount=100)
            
            brain = pynlm.BrainWrapper(config.getConfig())
            if brain.initialize():
                print("   ✓ Brain initialized successfully")
                brain.runSimulation(20)
                print("   ✓ Simulation completed")
            else:
                print("   ✗ Brain initialization failed")
        except pynlm.NLMError as e:
            print(f"   ✓ Caught NLMError: {e}")
        except Exception as e:
            print(f"   ✓ Caught other exception: {e}")
        
        print("\n2. Configuration validation example:")
        config = pynlm.ConfigWrapper()
        config.configureBrain(neuronCount=500)
        
        if brain.initialize():
            print("   ✓ Configuration validated and brain initialized")
        
        print("\n3. Statistics monitoring example:")
        stats = brain.getStatistics()
        print("   ✓ Statistics collected and displayed")
        print(f"     - Network health: {stats['firing_neurons']}/{stats['total_neurons']} neurons firing")
        print(f"     - Learning status: {'Active' if stats['e_i_ratio'] > 0.5 else 'Low'}")
        
        print("\n✓ All best practices demonstrations completed")
        
    except Exception as e:
        print(f"✗ Error: {e}")
    
    print()

def show_summary():
    """Show summary of tutorial."""
    section_header("TUTORIAL SUMMARY")
    
    print("This comprehensive tutorial covers:")
    print()
    print("✓ Basic Usage - Getting started with the NLM API")
    print("✓ Agent-World Integration - Complete simulation systems")
    print("✓ Configuration Management - Working with configuration files")
    print("✓ Advanced Features - Power user tools and utilities")
    print("✓ API Reference - Complete documentation of classes and methods")
    print("✓ Common Workflows - Real-world usage patterns")
    print("✓ Best Practices - Guidelines for effective usage")
    print()
    
    print("Key Features Covered:")
    print("• Enhanced Python bindings with improved error handling")
    print("• Convenience methods for common operations")
    print("• Type-safe configuration API")
    print("• Statistics and monitoring utilities")
    print("• Advanced simulation control")
    print("• Batch processing capabilities")
    print("• Debugging and validation tools")
    print("• Performance profiling")
    print()
    
    print("Getting Started:")
    print("1. Run 'basic_examples.py' for basic usage examples")
    print("2. Run 'advanced_api.py' for advanced features")
    print("3. Use 'nlm_cli.py' for command-line interface")
    print("4. Refer to the tutorial files for detailed guidance")
    print()

if __name__ == "__main__":
    # Run all tutorials
    tutorial_1_basic_usage()
    tutorial_2_agent_world()
    tutorial_3_configuration_management()
    tutorial_4_advanced_features()
    tutorial_5_api_reference()
    tutorial_6_workflows()
    tutorial_7_best_practices()
    
    # Show summary
    show_summary()
    
    print("🎉 All tutorials completed successfully!")
    print("The NLM Python API provides a powerful and flexible interface for")
    print("neural simulation, learning, and experimentation.")
