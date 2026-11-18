#!/usr/bin/env python3
"""
Demo script showcasing the improved NLM Python bindings.
This script demonstrates the new features and improvements.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def demo_basic_functionality():
    """Demonstrate basic NLM functionality."""
    print("=== Basic NLM Functionality Demo ===")
    
    # Create a simple brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a simple simulation
    for step in range(10):
        brain.step(step)
    
    print(f"Brain created successfully!")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
    print()

def demo_enhanced_configuration():
    """Demonstrate enhanced configuration utilities."""
    print("=== Enhanced Configuration Demo ===")
    
    # Create a training environment
    print("Creating training environment...")
    training_config = pynlm.createLearningConfig("curiosity")
    print(f"Training config keys: {training_config.getKeys()}")
    
    # Create a developing brain
    print("\nCreating developing brain...")
    brain, agent = pynlm.createDevelopingBrain(True, True, 0.05f, 0.7f)
    print(f"Brain created with neurogenesis: {agent.isDevelopmentEnabled()}")
    
    # Create visualization config
    print("\nCreating visualization config...")
    viz_config = pynlm.createDebugVisualizationConfig()
    print(f"Visualization config keys: {viz_config.getKeys()}")
    print()

def demo_validation():
    """Demonstrate configuration validation."""
    print("=== Configuration Validation Demo ===")
    
    # Test valid configuration
    print("Testing valid configuration...")
    config1 = pynlm.createDefaultConfig()
    config1.set("working_memory.enabled", True)
    config1.set("prediction.enabled", True)
    
    errors1, warnings1 = pynlm.validateConfig(config1)
    print(f"Valid config - Errors: {len(errors1)}, Warnings: {len(warnings1)}")
    
    # Test potentially problematic configuration
    print("\nTesting configuration with potential issues...")
    config2 = pynlm.createDefaultConfig()
    config2.set("working_memory.enabled", True)
    config2.set("prediction.enabled", False)
    
    errors2, warnings2 = pynlm.validateConfig(config2)
    print(f"Problematic config - Errors: {len(errors2)}, Warnings: {len(warnings2)}")
    if warnings2:
        print("Warnings:", warnings2)
    print()

def demo_benchmarking():
    """Demonstrate benchmarking utilities."""
    print("=== Benchmarking Demo ===")
    
    print("Running standard benchmark...")
    spike_count, firing_rate, ei_ratio = pynlm.runStandardBenchmark()
    print(f"Benchmark results:")
    print(f"  Total spikes: {spike_count}")
    print(f"  Firing rate: {firing_rate:.3f}")
    print(f"  Excitation/Inhibition ratio: {ei_ratio:.3f}")
    print()

def demo_custom_simulation():
    """Demonstrate custom simulation framework."""
    print("=== Custom Simulation Demo ===")
    
    def setup(brain, world):
        print("  Setting up simulation...")
        brain.initialize()
        world.configure(15, 15, 6, 6)
        world.reset()
    
    def custom_step(step_num, brain, world):
        world.update(0.1f)
        if step_num % 50 == 0:
            print(f"    Step {step_num}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("Running custom comparison simulation...")
    spike_count, firing_rate, ei_ratio = pynlm.runComparisonSimulation(setup, custom_step, 200)
    print(f"Custom simulation results:")
    print(f"  Total spikes: {spike_count}")
    print(f"  Firing rate: {firing_rate:.3f}")
    print(f"  Excitation/Inhibition ratio: {ei_ratio:.3f}")
    print()

def demo_learning_modes():
    """Demonstrate different learning mode configurations."""
    print("=== Learning Mode Presets Demo ===")
    
    modes = ["curiosity", "efficient", "exploration"]
    
    for mode in modes:
        print(f"\n{mode.title()} mode configuration:")
        config = pynlm.createLearningConfig(mode)
        print(f"  Working memory: {config.get<bool>('working_memory.enabled', False)}")
        print(f"  Prediction: {config.get<bool>('prediction.enabled', False)}")
        print(f"  Curiosity: {config.get<bool>('curiosity.enabled', False)}")
        print(f"  Novelty: {config.get<bool>('novelty.enabled', False)}")
        print(f"  Structural plasticity: {config.get<bool>('structural_plasticity.enabled', False)}")
        print(f"  Development: {config.get<bool>('development.enabled', False)}")
    
    print()

def demo_complete_agent_example():
    """Demonstrate a complete agent example using all new features."""
    print("=== Complete Agent Example ===")
    
    print("Creating advanced agent with enhanced features...")
    
    # Create configuration with all learning features
    config = pynlm.createLearningConfig("curiosity")
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    print("Running simulation...")
    
    # Run simulation with enhanced monitoring
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply learning
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        if step % 10 == 0:
            stats = {
                'step': step,
                'neurons': brain.getTotalNeuronCount(),
                'spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'development': brain.getDevelopmentalStage().name,
                'reward': result.reward,
                'prediction_error': agent.getPredictionError()
            }
            print(f"  Step {step}: {stats}")
    
    print(f"\nFinal statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage().name}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print()

def main():
    """Run all demonstrations."""
    print("NLM Python Bindings - Enhanced Features Demo")
    print("=" * 50)
    print()
    
    demo_basic_functionality()
    demo_enhanced_configuration()
    demo_validation()
    demo_benchmarking()
    demo_custom_simulation()
    demo_learning_modes()
    demo_complete_agent_example()
    
    print("=" * 50)
    print("All demonstrations completed successfully!")
    print()
    print("Key features demonstrated:")
    print("  ✓ Enhanced configuration utilities")
    print("  ✓ Configuration validation")
    print("  ✓ Benchmarking tools")
    print("  ✓ Custom simulation framework")
    print("  ✓ Learning mode presets")
    print("  ✓ Complete agent example")
    print()
    print("The NLM Python bindings have been significantly enhanced with:")
    print("  - Better documentation and examples")
    print("  - New utility functions")
    print("  - Advanced learning configurations")
    print("  - Improved error handling")
    print("  - Performance monitoring tools")

if __name__ == "__main__":
    main()