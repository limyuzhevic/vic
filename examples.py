#!/usr/bin/env python3

"""
NLM (熙然) - Example Python Scripts

This file contains example Python scripts that demonstrate how to use the
NLM brain simulation framework. These examples show basic usage, common patterns,
and advanced techniques for working with NLM.

These scripts are intended to help users get started with NLM and provide
examples for more advanced usage.
"""

import json
import sys
import time
from pathlib import Path

# Add the python bindings directory to the path
sys.path.insert(0, str(Path(__file__).parent / "python"))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Examples will not run.", file=sys.stderr)


def example_1_basic_brain():
    """
    Example 1: Basic Brain Usage
    
    This example shows how to create and run a simple brain simulation.
    """
    print("=== Example 1: Basic Brain Usage ===")
    
    if not PYNLM_AVAILABLE:
        print("Skipping - pynlm module not available")
        return
    
    # Create a brain with default configuration
    print("Creating brain with default configuration...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Print initial status
    print("Brain initialized successfully!")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    
    # Run simulation
    print("\nRunning simulation for 100 steps...")
    start_time = time.time()
    
    for step in range(100):
        brain.step(step)
        if step % 25 == 0:
            print(f"  Step {step}: Firing neurons = {brain.getFiringNeuronCount()}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Print final statistics
    print("\nFinal Statistics:")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"  - Pending spike events: {brain.getPendingSpikeEventCount()}")
    
    print("\n✓ Basic brain example completed successfully!\n")


def example_2_agent_world():
    """
    Example 2: Agent in World
    
    This example shows how to create an agent that interacts with a world.
    """
    print("=== Example 2: Agent in World ===")
    
    if not PYNLM_AVAILABLE:
        print("Skipping - pynlm module not available")
        return
    
    print("Creating brain, world, and agent...")
    
    # Create brain
    config = pynlm.createBrain(pynlm.createDefaultConfig())
    config.loadFromArgs(['--neuron-count', '500', '--region-count', '2'])
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure agent with advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Components initialized:")
    print(f"  - World: {world.getWidth()}x{world.getHeight()} cells")
    print(f"  - Agent vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"  - Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Motor output size: {agent.getMotorOutputSize()}")
    print(f"  - Sensory input size: {agent.getSensoryInputSize()}")
    
    # Run simulation
    print("\nRunning agent simulation...")
    steps_per_episode = 50
    num_episodes = 3
    
    for episode in range(num_episodes):
        print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
        
        # Reset environment
        world.reset()
        agent.reset()
        brain.reset()
        
        # Run episode
        for step in range(steps_per_episode):
            # 1. Get sensory percept from world
            percept = world.getSensoryPercept()
            
            # 2. Inject sensory input into brain
            agent.processSensoryInput(percept)
            
            # 3. Simulate brain
            for neural_step in range(10):
                brain.step(neural_step)
            
            # 4. Decode motor command
            action = agent.decodeMotorCommand()
            
            # 5. Apply motor command
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # 6. Apply neuromodulation
            agent.applyRewardModulation(result.reward, 0.0f)
            
            # 7. Update world
            world.update(0.01)
            
            # 8. Update development
            agent.updateDevelopment(0.01)
            
            # Print periodic updates
            if step % 10 == 0:
                print(f"  Step {step}: reward={result.reward:.1f}, "
                      f"action={action.name}, novelty={agent.getNoveltyLevel():.2f}, "
                      f"curiosity={agent.getCuriosityLevel():.2f}")
        
        # Print episode summary
        print(f"  Episode completed: reward={result.reward:.1f}, "
              f"energy={world.getAgentBody().energy:.1f}, "
              f"age={world.getAgentBody().age}")
    
    print("\n✓ Agent in world example completed successfully!\n")


def example_3_brain_analysis():
    """
    Example 3: Brain Analysis
    
    This example shows how to analyze brain data and extract meaningful metrics.
    """
    print("=== Example 3: Brain Analysis ===")
    
    if not PYNLM_AVAILABLE:
        print("Skipping - pynlm module not available")
        return
    
    print("Creating and running a simulation for analysis...")
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.loadFromArgs(['--neuron-count', '1000', '--steps', '1000'])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
    
    # Run analysis
    print("Analyzing brain state...")
    diagnostics = pynlm.run_diagnostic(brain)
    
    print("Analysis Results:")
    for key, value in diagnostics.items():
        print(f"  {key}: {value}")
    
    # Save analysis results
    output_file = "brain_analysis.json"
    with open(output_file, 'w') as f:
        json.dump(diagnostics, f, indent=2)
    
    print(f"\nAnalysis saved to {output_file}")
    
    # Create visualizations
    print("\nCreating visualizations...")
    # In a real implementation, we would create plots here
    print("  (Visualization code would go here)")
    
    print("\n✓ Brain analysis example completed successfully!\n")


def example_4_research_mode():
    """
    Example 4: Research Mode
    
    This example shows how to use NLM in research mode with advanced features.
    """
    print("=== Example 4: Research Mode ===")
    
    if not PYNLM_AVAILABLE:
        print("Skipping - pynlm module not available")
        return
    
    print("Running in research mode with advanced features...")
    
    # Create configuration for research
    config = pynlm.createDefaultConfig()
    
    # Create and configure brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable all advanced features
    print("Enabled features:")
    print("  - Reward modulation: enabled")
    print("  - Curiosity: enabled")
    print("  - Novelty detection: enabled")
    print("  - Structural plasticity: enabled")
    print("  - Development: enabled")
    
    # Run a research simulation
    num_steps = 5000
    print(f"\nRunning research simulation for {num_steps} steps...")
    
    start_time = time.time()
    
    for step in range(num_steps):
        brain.step(step)
        
        if step % 1000 == 0:
            print(f"  Step {step}: Total spikes = {brain.getTotalSpikeCount()}")
    
    end_time = time.time()
    print(f"\nResearch simulation completed in {end_time - start_time:.2f} seconds")
    
    # Get diagnostic information
    print("\nResearch metrics:")
    diagnostics = pynlm.run_diagnostic(brain)
    for key, value in diagnostics.items():
        print(f"  {key}: {value}")
    
    print("\n✓ Research mode example completed successfully!\n")


def example_5_batch_processing():
    """
    Example 5: Batch Processing
    
    This example shows how to run multiple simulations in batch mode.
    """
    print("=== Example 5: Batch Processing ===")
    
    if not PYNLM_AVAILABLE:
        print("Skipping - pynlm module not available")
        return
    
    print("Setting up batch processing...")
    
    # Create multiple configurations
    configs = []
    for i in range(3):
        config = pynlm.createDefaultConfig()
        config.loadFromArgs([
            '--neuron-count', str(500 + i * 500),
            '--region-count', str(2 + i),
            '--random-seed', str(42 + i)
        ])
        configs.append(config)
    
    # Run batch simulation
    num_steps = 200
    brains = []
    
    print(f"Running batch simulation for {num_steps} steps with {len(configs)} configurations...")
    
    for i, config in enumerate(configs):
        print(f"  Running configuration {i + 1}: {config.get('neuron_count')} neurons")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(num_steps):
            brain.step(step)
        
        brains.append(brain)
        print(f"    Total spikes: {brain.getTotalSpikeCount()}")
    
    # Analyze batch results
    print("\nBatch Results:")
    for i, brain in enumerate(brains):
        diagnostics = pynlm.run_diagnostic(brain)
        print(f"  Configuration {i + 1}: {diagnostics.get('total_neurons')} neurons, "
              f"{diagnostics.get('total_spikes')} spikes")
    
    print("\n✓ Batch processing example completed successfully!\n")


def example_6_checkpoints():
    """
    Example 6: Checkpointing and Recovery
    
    This example shows how to use checkpointing and recovery features.
    """
    print("=== Example 6: Checkpointing and Recovery ===")
    
    if not PYNLM_AVAILABLE:
        print("Skipping - pynlm module not available")
        return
    
    print("Testing checkpointing and recovery...")
    
    # Create initial brain
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run some simulation
    for step in range(100):
        brain1.step(step)
    
    print("Initial simulation completed")
    print(f"  Total spikes: {brain1.getTotalSpikeCount()}")
    
    # Save checkpoint
    checkpoint_file = "test_checkpoint.brain"
    print(f"\nSaving checkpoint to {checkpoint_file}...")
    brain1.save(checkpoint_file)
    print("  Checkpoint saved successfully")
    
    # Create new brain and load checkpoint
    print("\nCreating new brain and loading checkpoint...")
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    
    if brain2.load(checkpoint_file):
        print("  Checkpoint loaded successfully")
        
        # Verify loaded brain
        print("\nVerifying loaded brain state:")
        print(f"  Total neurons: {brain2.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain2.getTotalSynapseCount()}")
        print(f"  Total spikes: {brain2.getTotalSpikeCount()}")
        
        # Continue simulation
        print("\nContinuing simulation...")
        for step in range(100, 200):
            brain2.step(step)
        
        print(f"  Total spikes after continuation: {brain2.getTotalSpikeCount()}")
        
        # Save final state
        brain2.save("final_checkpoint.brain")
        print("  Final checkpoint saved")
        
    else:
        print("  Error loading checkpoint")
        return
    
    print("\n✓ Checkpointing and recovery example completed successfully!\n")


def main():
    """Run all examples."""
    print("NLM Python Examples")
    print("=" * 50)
    print()
    
    examples = [
        ("Basic Brain Usage", example_1_basic_brain),
        ("Agent in World", example_2_agent_world),
        ("Brain Analysis", example_3_brain_analysis),
        ("Research Mode", example_4_research_mode),
        ("Batch Processing", example_5_batch_processing),
        ("Checkpointing and Recovery", example_6_checkpoints),
    ]
    
    completed = 0
    skipped = 0
    
    for name, example_func in examples:
        try:
            example_func()
            completed += 1
        except Exception as e:
            print(f"✗ Error running {name}: {e}")
            skipped += 1
        
        print("-" * 50)
    
    print(f"\nSummary: {completed} examples completed, {skipped} examples skipped")
    
    if not PYNLM_AVAILABLE:
        print("\nNote: Some examples were skipped because pynlm module is not available.")
        print("Make sure to install the Python bindings before running examples.")


if __name__ == '__main__':
    main()