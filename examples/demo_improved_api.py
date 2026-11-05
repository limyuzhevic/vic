#!/usr/bin/env python3
"""
NLM Example Script: Demonstrates the improved Python API

This script shows how to use the enhanced NLM Python bindings,
including new convenience methods and configuration options.
"""

import pynlm
import sys

def print_header(title):
    """Print a formatted header."""
    print("\n" + "="*60)
    print(f"{title}")
    print("="*60)

def example_1_basic_usage():
    """Example 1: Basic usage with new convenience methods."""
    print_header("Example 1: Basic NLM Usage")
    
    # Create a quick-start configuration
    print("Creating quick-start configuration...")
    config = pynlm.createQuickStartConfig()
    
    # Create brain with config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Run a simple simulation
    print("\nRunning simple simulation...")
    for i in range(50):
        brain.step(i)
    
    print(f"✓ Brain processed {brain.getTotalSpikeCount()} spikes")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.3f} Hz")

def example_2_complete_simulation():
    """Example 2: Complete simulation with agent and world."""
    print_header("Example 2: Complete Agent Simulation")
    
    # Create components
    print("Setting up simulation components...")
    brain = pynlm.createBrain(pynlm.createQuickStartConfig())
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Configure agent for balanced performance
    agent.configureForSimulation("balanced")
    agent.enableAllFeatures(True)
    
    print(f"✓ Created agent with {agent.getSensoryInputSize()} sensory inputs")
    print(f"✓ Agent has {agent.getMotorOutputSize()} motor outputs")
    print(f"✓ Agent features:")
    print(f"  - Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  - Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  - Development: {agent.isDevelopmentEnabled()}")
    print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run simulation
    print("\nRunning complete simulation...")
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("✓ Simulation complete!")
    print(f"  Final stats:")
    print(f"    Total spikes: {brain.getTotalSpikeCount()}")
    print(f"    Development stage: {agent.getDevelopmentalStage()}")
    print(f"    Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")

def example_3_configuration_examples():
    """Example 3: Different configuration modes."""
    print_header("Example 3: Configuration Modes")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    
    # Test different simulation modes
    modes = ["fast", "balanced", "accurate", "minimal"]
    
    for mode in modes:
        print(f"\nTesting '{mode}' mode...")
        
        # Reset for clean test
        agent.reset()
        agent.configureForSimulation(mode)
        
        print(f"  ✓ Enabled features: {agent.getFeatureStatus()}")
        print(f"  ✓ Available features: {', '.join(agent.getAvailableFeatures())}")
        
        # Demonstrate feature flag setting
        if mode != "minimal":
            # Set a specific feature flag
            agent.setFeatureFlag("curiosity", True)
            print(f"  ✓ Set curiosity flag: {agent.isCuriosityEnabled()}")
    
    print("\n✓ All configuration modes tested successfully")

def example_4_validation_and_status():
    """Example 4: Setup validation and status reporting."""
    print_header("Example 4: Setup Validation and Status")
    
    # Create a complete setup
    brain = pynlm.createBrain(pynlm.createQuickStartConfig())
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    
    agent.initialize(world)
    
    # Validate setup
    print("Validating simulation setup...")
    is_valid, errors = pynlm.validateBrainSetup(brain, agent, world)
    
    if is_valid:
        print("✓ Setup is valid!")
        
        # Get comprehensive status
        print("\nGetting simulation status...")
        status = pynlm.getSimulationStatus(brain, agent, world)
        
        for key, value in status.items():
            print(f"  {key}: {value}")
            
    else:
        print(f"✗ Setup has errors: {errors}")

def example_5_advanced_features():
    """Example 5: Advanced features like export/import."""
    print_header("Example 5: Advanced Configuration Features")
    
    # Create and save a configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("neuromod.curiosity.enable", True)
    
    print("Created custom configuration")
    
    # Save to JSON
    config_filename = "example_config.json"
    config.saveToFile(config_filename)
    print(f"✓ Saved configuration to {config_filename}")
    
    # Load from JSON
    new_config = pynlm.createDefaultConfig()
    success = pynlm.importConfigFromJSON(new_config, config_filename)
    
    if success:
        print("✓ Successfully loaded configuration from JSON")
        print(f"  - Neuron count: {new_config.getOr(int64_t, 'brain.neuron_count', 0)}")
    else:
        print("✗ Failed to load configuration")
    
    # Demonstrate cleanup
    import os
    if os.path.exists(config_filename):
        os.remove(config_filename)
        print(f"✓ Cleaned up {config_filename}")

def main():
    """Main function to run all examples."""
    print("NLM Python API Examples")
    print("======================\n")
    print("This script demonstrates the improved NLM Python bindings,")
    print("including new convenience methods and configuration options.\n")
    
    examples = [
        example_1_basic_usage,
        example_2_complete_simulation,
        example_3_configuration_examples,
        example_4_validation_and_status,
        example_5_advanced_features,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"\n✗ Error in Example {i}: {e}")
            import traceback
            traceback.print_exc()
    
    print_header("All Examples Complete!")
    print("\nThese examples showcase the following improvements:")
    print("  • Quick-start configuration methods")
    print("  • Complete simulation templates")
    print("  • Multiple simulation modes (fast, balanced, accurate, minimal)")
    print("  • Setup validation and status reporting")
    print("  • Configuration export/import")
    print("  • Enhanced agent configuration options")
    print("\nFor more information, see NLM_QUICK_START.md")

if __name__ == "__main__":
    main()
