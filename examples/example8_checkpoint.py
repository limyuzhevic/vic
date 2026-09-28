#!/usr/bin/env python3
"""
Example 8: Checkpoint and Persistence

This example demonstrates how to use NLM's checkpoint system for saving and
loading brain states, enabling long-running simulations and experiment reproducibility.
"""

import pynlm
import numpy as np
import os
import time

def checkpoint_persistence_demo():
    """Demonstrate checkpoint and persistence functionality."""
    
    print("=== Example 8: Checkpoint and Persistence ===")
    
    # Create configuration with persistence options
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Create checkpoint manager
    checkpoint_manager = pynlm.createCheckpointManager()
    checkpoint_manager.setAutoSaveInterval(50)  # Auto-save every 50 steps
    
    print("Checkpoint system configuration:")
    print(f"  Auto-save interval: {checkpoint_manager.getAutoSaveInterval()} steps")
    print(f"  Initial checkpoints: {len(checkpoint_manager.getCheckpointList())}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=18, height=18, visionWidth=7, visionHeight=7)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("\n--- Checkpoint Management Demo ---")
    
    # Create initial checkpoint
    initial_checkpoint = checkpoint_manager.createCheckpoint("initial_state")
    print(f"Created initial checkpoint: {initial_checkpoint}")
    
    # Save initial state
    checkpoint_manager.saveCheckpoint("initial_state", brain)
    print("✓ Initial state saved")
    
    # Run simulation for a while
    print("\nRunning initial simulation...")
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply modulation
        reward = np.random.random() * 0.2
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 25 == 0:
            print(f"  Step {step}: Position ({world.getAgentPosition().x:.1f}, {world.getAgentPosition().y:.1f})")
    
    # Check available checkpoints
    checkpoints = checkpoint_manager.getCheckpointList()
    print(f"\nAvailable checkpoints: {checkpoints}")
    
    # Create intermediate checkpoint
    intermediate_checkpoint = checkpoint_manager.createCheckpoint("intermediate_state")
    checkpoint_manager.saveCheckpoint("intermediate_state", brain)
    print(f"✓ Intermediate checkpoint '{intermediate_checkpoint}' created and saved")
    
    # Run more simulation
    print("\nContinuing simulation...")
    for step in range(100, 200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = np.random.random() * 0.2
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 25 == 0:
            print(f"  Step {step}: Position ({world.getAgentPosition().x:.1f}, {world.getAgentPosition().y:.1f})")
    
    # Create final checkpoint
    final_checkpoint = checkpoint_manager.createCheckpoint("final_state")
    checkpoint_manager.saveCheckpoint("final_state", brain)
    print(f"✓ Final checkpoint '{final_checkpoint}' created and saved")
    
    # Get checkpoint information
    checkpoint_info = checkpoint_manager.getCheckpointInfo("initial_state")
    print(f"\nInitial checkpoint info: {checkpoint_info}")
    
    # Test checkpoint restoration
    print("\n--- Checkpoint Restoration Demo ---")
    
    # Create a new brain (fresh)
    new_brain = pynlm.createBrain(config)
    new_brain.initialize()
    
    print("Restoring initial state...")
    checkpoint_manager.loadCheckpoint("initial_state", new_brain)
    print("✓ State restored successfully")
    
    # Verify restoration
    new_firing = new_brain.getFiringNeuronCount()
    initial_firing = brain.getFiringNeuronCount()
    
    print(f"Original brain firing: {initial_firing} neurons")
    print(f"Restored brain firing: {new_firing} neurons")
    
    if new_firing == initial_firing:
        print("✓ Checkpoint restoration successful!")
    else:
        print(f"⚠️  Restoration completed but with differences: {abs(new_firing - initial_firing)}")
    
    # Test checkpoint deletion
    print("\n--- Checkpoint Deletion Demo ---")
    print(f"Checkpoints before deletion: {len(checkpoint_manager.getCheckpointList())}")
    
    checkpoint_manager.deleteCheckpoint("intermediate_state")
    print("✓ Intermediate checkpoint deleted")
    
    print(f"Checkpoints after deletion: {len(checkpoint_manager.getCheckpointList())}")
    
    # Test auto-save functionality
    print("\n--- Auto-Save Demo ---")
    
    # Set different auto-save interval
    checkpoint_manager.setAutoSaveInterval(30)
    print(f"Auto-save interval changed to: {checkpoint_manager.getAutoSaveInterval()} steps")
    
    # Run more steps to trigger auto-save
    print("Running additional steps to trigger auto-save...")
    for step in range(200, 250):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = np.random.random() * 0.2
        agent.applyRewardModulation(reward, 0.0)
    
    # Check if auto-saves were created
    current_checkpoints = checkpoint_manager.getCheckpointList()
    print(f"Checkpoints after auto-save simulation: {current_checkpoints}")
    
    # Show checkpoint details
    for checkpoint in current_checkpoints:
        info = checkpoint_manager.getCheckpointInfo(checkpoint)
        print(f"  {checkpoint}: {info}")
    
    # Demonstrate checkpoint management workflows
    print("\n--- Checkpoint Management Workflows ---")
    
    # Workflow 1: Experiment reproducibility
    print("\n1. Experiment Reproducibility Workflow:")
    print("   Step 1: Run experiment to completion")
    print("   Step 2: Save final checkpoint")
    print("   Step 3: Archive checkpoint for future reference")
    print("   Step 4: Restore in new environment for analysis")
    
    # Workflow 2: Long-running training
    print("\n2. Long-Running Training Workflow:")
    print("   Step 1: Set appropriate auto-save interval")
    print("   Step 2: Train until completion or interruption")
    print("   Step 3: Continue training from checkpoint")
    print("   Step 4: Resume training without data loss")
    
    # Workflow 3: Multi-environment testing
    print("\n3. Multi-Environment Testing Workflow:")
    print("   Step 1: Train on environment A")
    print("   Step 2: Save checkpoint")
    print("   Step 3: Load in environment B for transfer learning")
    print("   Step 4: Fine-tune on new environment")
    
    print("\nCheckpoint and persistence demonstration complete!")
    return brain, world, agent, checkpoint_manager

if __name__ == "__main__":
    checkpoint_persistence_demo()