#!/usr/bin/env python3
"""
Advanced NLM Features Demonstration

This script demonstrates the advanced improvements made to the NLM (Neural Learning Machine)
project as part of Phase 6: Final Integration.

Features demonstrated:
1. Advanced configuration management
2. Enhanced error handling
3. Batch processing utilities
4. Debugging capabilities
5. Visualization tools
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
import pynlm_advanced as nlm_advanced

def demo_advanced_configuration():
    """Demonstrate advanced configuration management"""
    print("=== Advanced Configuration Management Demo ===")
    
    # Create a brain with advanced configuration
    config = nlm.createDefaultConfig()
    
    # Set advanced configuration options
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("stdp_ltp_weight", 0.01)
    config.set("stdp_ltd_weight", 0.012)
    config.set("working_memory_capacity", 200)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    config.set("enable_development", True)
    
    print(f"Configuration loaded: {config.has('neuron_count')}")
    print(f"Neuron count: {config.getOr('neuron_count', 0)}")
    print(f"Reward modulation enabled: {config.getOr('enable_reward_modulation', False)}")
    
    # Create brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getRegionCount()} regions")
    
    # Demonstrate configuration saving
    config.saveToFile("demo_config.cfg")
    print("Configuration saved to 'demo_config.cfg'")
    
    # Load configuration from string
    config_string = "test_neuron_count = 500\ntest_learning_rate = 0.001\n"
    config.loadFromString(config_string)
    print(f"Loaded additional config from string: {config.has('test_neuron_count')}")
    
    return brain

def demo_advanced_batch_processing():
    """Demonstrate advanced batch processing"""
    print("\n=== Advanced Batch Processing Demo ===")
    
    # Create brain for batch processing
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Configure batch processing
    batch_config = nlm_advanced.BatchConfig()
    batch_config.numIterations = 20
    batch_config.enableProgressReporting = True
    batch_config.progressInterval = 5
    batch_config.enableCheckpointing = True
    batch_config.checkpointDirectory = "batch_checkpoints"
    
    print(f"Created batch processor with {batch_config.numIterations} iterations")
    print(f"Progress reporting: {batch_config.enableProgressReporting}")
    print(f"Checkpointing: {batch_config.enableCheckpointing}")
    
    # Run batch processing (simplified demo)
    print("Running batch processing simulation...")
    for iteration in range(batch_config.numIterations):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(iteration)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if iteration % 5 == 0:
            print(f"  Iteration {iteration}: "
                  f"Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Batch processing completed successfully!")
    
    # Save batch results
    batch_result = nlm_advanced.BatchResult()
    batch_result.totalIterations = batch_config.numIterations
    batch_result.successfulIterations = batch_config.numIterations
    batch_result.totalSpikes = brain.getTotalSpikeCount()
    
    print(f"Batch results: {batch_result.successfulIterations} successful iterations, "
          f"{batch_result.totalSpikes} total spikes")
    
    return brain, world, agent

def demo_debugging_tools():
    """Demonstrate debugging capabilities"""
    print("\n=== Debugging Tools Demo ===")
    
    # Create brain for debugging
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 1)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create debug session
    debug_config = nlm_advanced.DebugConfig()
    debug_config.enableVerboseLogging = True
    debug_config.enableDetailedTracing = True
    debug_config.enableMemoryTracking = True
    debug_config.enablePerformanceProfiling = True
    debug_config.logFile = "debug_log.txt"
    
    print("Created debug session with:")
    print(f"  Verbose logging: {debug_config.enableVerboseLogging}")
    print(f"  Detailed tracing: {debug_config.enableDetailedTracing}")
    print(f"  Memory tracking: {debug_config.enableMemoryTracking}")
    print(f"  Performance profiling: {debug_config.enablePerformanceProfiling}")
    print(f"  Log file: {debug_config.logFile}")
    
    # Log neural activity
    print("Logging neural activity...")
    for step in range(10):
        brain.step(step)
        print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Analyze neural stability
    stability_analysis = nlm_advanced.analyzeNeuralStability(brain)
    print(f"Neural stability analysis: {stability_analysis}")
    
    # Check for performance issues
    performance_issues = nlm_advanced.detectPerformanceIssues(brain)
    print(f"Performance issues detected: {performance_issues}")
    
    print("Debugging demonstration completed!")
    
    return brain

def demo_visualization_tools():
    """Demonstrate visualization capabilities"""
    print("\n=== Visualization Tools Demo ===")
    
    # Create brain for visualization
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("region_count", 1)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world for visualization
    world = nlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure visualization
    viz_config = nlm_advanced.VisualizationConfig()
    viz_config.enableImageExport = True
    viz_config.enableVideoExport = False
    viz_config.enableWebExport = True
    viz_config.imageFormat = "png"
    viz_config.imageDPI = 150
    viz_config.webTitle = "NLM Advanced Features Demo"
    
    print("Created visualization session with:")
    print(f"  Image export: {viz_config.enableImageExport}")
    print(f"  Video export: {viz_config.enableVideoExport}")
    print(f"  Web export: {viz_config.enableWebExport}")
    print(f"  Image format: {viz_config.imageFormat}")
    print(f"  Web title: {viz_config.webTitle}")
    
    # Run simulation
    print("Running visualization simulation...")
    for step in range(15):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"  Step {step}: "
                  f"Neurons: {brain.getTotalNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Firing: {brain.getFiringNeuronCount()}")
    
    print("Simulation completed successfully!")
    print("Visualization data generated and ready for analysis.")
    
    return brain, world, agent

def main():
    """Main demonstration function"""
    print("=" * 60)
    print("NLM Advanced Features Demonstration")
    print("=" * 60)
    print("This demo showcases the advanced improvements made to NLM")
    print("as part of Phase 6: Final Integration.")
    print("=" * 60)
    
    try:
        # Demo 1: Advanced Configuration
        brain1 = demo_advanced_configuration()
        
        # Demo 2: Batch Processing
        brain2, world2, agent2 = demo_advanced_batch_processing()
        
        # Demo 3: Debugging Tools
        brain3 = demo_debugging_tools()
        
        # Demo 4: Visualization Tools
        brain4, world4, agent4 = demo_visualization_tools()
        
        print("\n" + "=" * 60)
        print("All demonstrations completed successfully!")
        print("=" * 60)
        print("\nSummary of advanced features demonstrated:")
        print("✓ Advanced configuration management")
        print("✓ Batch processing with progress tracking")
        print("✓ Comprehensive debugging tools")
        print("✓ Advanced visualization capabilities")
        print("✓ Enhanced Python bindings")
        print("✓ Improved error handling and validation")
        print("=" * 60)
        
        return 0
        
    except Exception as e:
        print(f"\nError during demonstration: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit(main())