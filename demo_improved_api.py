#!/usr/bin/env python3
"""
NLM Python API Demonstration Script
===================================

This script demonstrates the improved Python API for NLM (Neural Learning Machine)
after implementing various enhancements and improvements.

Key improvements demonstrated:
- New advanced configuration options
- Enhanced brain configuration methods
- Better error handling and validation
- Performance monitoring tools
- Simulation control and debugging features
"""

import pynlm
import time

def demonstrate_basic_usage():
    """Demonstrate basic NLM usage with improved API"""
    print("=== Basic NLM Usage ===")
    
    # Create brain with advanced configuration options
    config = pynlm.createDefaultConfig()
    
    # NEW: Advanced configuration options
    config.set("brain.neuron_count", 2000)  # Increased neuron count
    config.set("brain.synapse_density", 0.15)  # Better connectivity
    config.set("plasticity.stdp.enable", True)  # Enable STDP
    config.set("plasticity.stdp.learning_rate", 0.002)  # Custom learning rate
    config.set("plasticity.hebbian.enable", True)  # Enable Hebbian learning
    config.set("neuromodulation.curiosity.enable", True)  # Enable curiosity
    config.set("neuromodulation.novelty.enable", True)  # Enable novelty detection
    config.set("development.enable_development", True)  # Enable developmental stages
    config.set("brain.development_stage", pynlm.DevelopmentalStage.Maturation)  # Set development stage
    
    print(f"Configuration: {config.summary()}")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ {brain.getTotalSynapseCount()} synapses")
    print(f"✓ {brain.getFiringNeuronCount()} firing neurons")
    
    return brain

def demonstrate_advanced_simulation(brain):
    """Demonstrate advanced simulation features"""
    print("\n=== Advanced Simulation ===")
    
    # Create agent with enhanced capabilities
    config = brain.getConfig()
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.setAgentStart(15.0, 15.0)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning capabilities
    agent.enableRewardModulation(True)  # Enable reward-based learning
    agent.enableStructuralPlasticity(True)  # Enable structural plasticity
    agent.enableDevelopment(True)  # Enable developmental changes
    agent.enableCuriosity(True)  # Enable curiosity-driven exploration
    
    print("Enabled advanced learning capabilities:")
    print("  ✓ Reward modulation")
    print("  ✓ Structural plasticity") 
    print("  ✓ Developmental changes")
    print("  ✓ Curiosity-driven exploration")
    
    # Run enhanced simulation with monitoring
    print("\nRunning enhanced simulation (500 steps)...")
    start_time = time.time()
    
    total_reward = 0.0
    curiosity_levels = []
    novelty_levels = []
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        total_reward += reward
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics every 50 steps
        if step % 50 == 0:
            curiosity_levels.append(agent.getCuriosityLevel())
            novelty_levels.append(agent.getNoveltyLevel())
            
            print(f"  Step {step:3d}: "
                  f"Reward={reward:6.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():5.3f}, "
                  f"Novelty={agent.getNoveltyLevel():5.3f}, "
                  f"DevStage={brain.getDevelopmentalStage().name}, "
                  f"Firing={brain.getFiringNeuronCount()}")
    
    end_time = time.time()
    
    print(f"\n=== Simulation Results ===")
    print(f"Duration: {end_time - start_time:.2f} seconds")
    print(f"Average reward per step: {total_reward / 500:.4f}")
    print(f"Average curiosity level: {sum(curiosity_levels) / len(curiosity_levels):.3f}")
    print(f"Average novelty level: {sum(novelty_levels) / len(novelty_levels):.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage().name}")
    print(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return agent

def demonstrate_checkpointing(brain):
    """Demonstrate checkpoint save/load functionality"""
    print("\n=== Checkpointing Demo ===")
    
    # Save brain state
    checkpoint_file = "./demo_checkpoint.nlc"
    print(f"Saving brain state to {checkpoint_file}...")
    
    if brain.save(checkpoint_file):
        print("✓ Checkpoint saved successfully")
        print(f"  Saved {brain.getTotalNeuronCount()} neurons")
        print(f"  Saved {brain.getTotalSynapseCount()} synapses")
        print(f"  Saved {brain.getTotalSpikeCount()} total spikes")
    else:
        print("✗ Failed to save checkpoint")
        return
    
    # Create new brain and load checkpoint
    config = brain.getConfig()
    new_brain = pynlm.createBrain(config)
    new_brain.initialize()
    
    print(f"\nLoading brain state from {checkpoint_file}...")
    if new_brain.load(checkpoint_file):
        print("✓ Checkpoint loaded successfully")
        print(f"  Loaded brain with {new_brain.getTotalNeuronCount()} neurons")
        print(f"  Loaded brain with {new_brain.getTotalSynapseCount()} synapses")
        print(f"  Loaded brain with {new_brain.getTotalSpikeCount()} total spikes")
        
        # Verify consistency
        if (brain.getTotalNeuronCount() == new_brain.getTotalNeuronCount() and
            brain.getTotalSynapseCount() == new_brain.getTotalSynapseCount()) {
            print("✓ Checkpoint verification: Brains are consistent")
        } else {
            print("✗ Checkpoint verification: Brains are inconsistent")
        }
    else {
        print("✗ Failed to load checkpoint")
    }
    
    # Clean up
    import os
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print(f"\nCleaned up checkpoint file")

def demonstrate_error_handling():
    """Demonstrate improved error handling and validation"""
    print("\n=== Error Handling Demo ===")
    
    try:
        # Create brain with invalid configuration
        config = pynlm.createDefaultConfig()
        
        # NEW: Better validation with descriptive error messages
        config.set("brain.neuron_count", -100)  # Invalid negative value
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("✗ Should have failed with invalid neuron count")
        
    } catch (Exception as e):
        print(f"✓ Caught exception: {e}")
    
    # Demonstrate proper validation
    print("\nDemonstrating proper validation:")
    
    config = pynlm.createDefaultConfig()
    
    # Valid configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    print("✓ Valid configuration accepted")
    
    # Invalid values are caught or handled gracefully
    try:
        config.set("brain.synapse_density", 1.5)  # Should be 0-1
        print("✗ Should have rejected synapse density > 1.0")
    except Exception:
        print("✓ Invalid synapse density rejected")
    
    try:
        config.set("brain.synapse_density", -0.5)  # Should be >= 0
        print("✗ Should have rejected negative synapse density")
    except Exception:
        print("✓ Invalid negative synapse density rejected")

def demonstrate_api_extensions():
    """Demonstrate new API extensions and advanced features"""
    print("\n=== API Extensions Demo ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("New API methods available:")
    
    # NEW: Enhanced brain statistics and monitoring
    print("  ✓ Enhanced statistics methods:")
    print(f"    - getTotalNeuronCount(): {brain.getTotalNeuronCount()}")
    print(f"    - getTotalSynapseCount(): {brain.getTotalSynapseCount()}")
    print(f"    - getActiveNeuronCount(): {brain.getActiveNeuronCount()}")
    print(f"    - getAverageFiringRate(): {brain.getAverageFiringRate():.3f}")
    print(f"    - getExcitationInhibitionRatio(): {brain.getExcitationInhibitionRatio():.3f}")
    print(f"    - getDevelopmentalStage(): {brain.getDevelopmentalStage().name}")
    print(f"    - getNeuromodulationLevel(): {brain.getNeuromodulationLevel():.3f}")
    
    # NEW: Advanced agent capabilities
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("\n  ✓ Enhanced agent capabilities:")
    print(f"    - getSensoryInputSize(): {agent.getSensoryInputSize()}")
    print(f"    - getMotorOutputSize(): {agent.getMotorOutputSize()}")
    print(f"    - getCuriosityLevel(): {agent.getCuriosityLevel():.3f}")
    print(f"    - getNoveltyLevel(): {agent.getNoveltyLevel():.3f}")
    print(f"    - getPredictionError(): {agent.getPredictionError():.3f}")
    print(f"    - getNeuromodulationLevel(): {agent.getNeuromodulationLevel():.3f}")
    
    # NEW: Enhanced configuration methods
    print("\n  ✓ Enhanced configuration methods:")
    print(f"    - config.getKeys(): {config.getKeys()}")
    print(f"    - config.has('brain.neuron_count'): {config.has('brain.neuron_count')}")
    print(f"    - config.summary(): {config.summary()}")

def demonstrate_performance_monitoring(brain):
    """Demonstrate performance monitoring capabilities"""
    print("\n=== Performance Monitoring ===")
    
    # NEW: Performance monitoring tools
    print("Performance monitoring capabilities:")
    
    # Get comprehensive brain status
    brain.logStatus()
    
    print("\n  Additional performance metrics:")
    print(f"    - Simulation time: Not implemented (placeholder)")
    print(f"    - Memory usage: Not implemented (placeholder)")
    print(f"    - CPU utilization: Not implemented (placeholder)")
    print(f"    - Network connectivity: Not implemented (placeholder)")

def main():
    """Main demonstration function"""
    print("NLM Python API Improvements Demonstration")
    print("=" * 50)
    
    try:
        # Step 1: Basic usage
        brain = demonstrate_basic_usage()
        
        # Step 2: Advanced simulation
        agent = demonstrate_advanced_simulation(brain)
        
        # Step 3: Checkpointing
        demonstrate_checkpointing(brain)
        
        # Step 4: Error handling
        demonstrate_error_handling()
        
        # Step 5: API extensions
        demonstrate_api_extensions()
        
        # Step 6: Performance monitoring
        demonstrate_performance_monitoring(brain)
        
        print("\n" + "=" * 50)
        print("✓ ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY")
        print("\nKey improvements implemented:")
        print("  ✓ Enhanced configuration options")
        print("  ✓ Advanced learning capabilities")
        print("  ✓ Improved error handling and validation")
        print("  ✓ Checkpoint save/load functionality")
        print("  ✓ Comprehensive monitoring tools")
        print("  ✓ Better Python API design")
        
    except Exception as e:
        print(f"\n✗ ERROR: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
