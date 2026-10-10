#!/usr/bin/env python3
"""
Advanced Phase 6 Examples for NLM (Neural Learning Machine)

This file contains comprehensive examples demonstrating all Phase 6 capabilities:
- Working Memory and Episodic Memory
- Neuromodulation Systems (Dopamine, ACh, Curiosity)
- Concept Formation
- Neural Planning
- Advanced Python Features

These examples are designed for advanced users who want to leverage
the full integrated brain capabilities of Phase 6.
"""

import pynlm
import time
import math
from typing import Dict, List, Optional

print("=== Advanced Phase 6 Examples for NLM ===")
print()

# ============================================================================
# 1. WORKING MEMORY AND EPISODIC MEMORY EXAMPLES
# ============================================================================

def example_1_working_memory_advanced():
    """Advanced working memory demonstration with sensory integration."""
    
    print("1. Advanced Working Memory Example")
    print("-" * 40)
    
    # Configure brain with enhanced memory capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    config.set("working_memory.capacity_neuron_count", 800)
    config.set("working_memory.persistence_strength", 0.9)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Working memory capacity: {config.get('working_memory.capacity_neuron_count')} neurons")
    print()
    
    # Run demonstration with working memory tracking
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            wm_neurons = len(brain.getWorkingMemory().getMemoryNeurons()) if brain.getWorkingMemory() else 0
            activity = brain.getWorkingMemory().getMemoryActivity() if brain.getWorkingMemory() else 0.0
            print(f"Step {step}: Working memory neurons={wm_neurons}, activity={activity:.3f}")
    
    print("Working memory demonstration complete!")
    return brain, agent, world

# ============================================================================
# 2. NEUROMODULATION AND DOPAMINE-DRIVEN LEARNING
# ============================================================================

def example_2_dopamine_advanced():
    """Advanced dopamine neuromodulation with reward prediction."""
    
    print("\n2. Advanced Dopamine Neuromodulation Example")
    print("-" * 45)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.15)
    config.set("neuromod.dopamine.scale", 1.8)
    config.set("neuromod.dopamine.working_memory_boost", 0.7)
    config.set("neuromod.dopamine.memory_consolidation_strength", 1.3)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    print("Dopamine level tracking:")
    print("  - Higher dopamine = stronger learning")
    print("  - Dopamine modulates working memory")
    print("  - Dopamine affects memory consolidation")
    print()
    
    # Simulate different reward scenarios
    reward_phases = [
        (0, 100, 0.0, "Exploration - no reward"),
        (100, 300, 0.5, "Learning - moderate rewards"),
        (300, 500, 1.0, "Optimization - high rewards"),
        (500, 600, 0.2, "Testing - intermittent rewards")
    ]
    
    for phase_start, phase_end, base_reward, description in reward_phases:
        print(f"Phase: {description}")
        
        for step in range(phase_start, phase_end):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            predicted = 0.5  # Simple prediction
            agent.applyRewardModulation(base_reward, predicted)
            
            # Track dopamine effects
            dopamine = agent.getNeuromodulationLevel()
            
            if step % 50 == phase_start:
                print(f"  Step {step}: Reward={base_reward:.1f}, Dopamine={dopamine:.3f}")
    
    print("Dopamine neuromodulation demonstration complete!")
    return brain, agent, world

# ============================================================================
# 3. CONCEPT FORMATION AND PATTERN DISCOVERY
# ============================================================================

def example_3_concept_formation():
    """Advanced concept formation from sensory patterns."""
    
    print("\n3. Advanced Concept Formation Example")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2500)
    config.set("brain.synapse_density", 0.18)
    config.set("concept_formation.enable_pattern_clustering", True)
    config.set("concept_formation.prototype_creation_threshold", 0.6)
    config.set("concept_formation.stability_threshold", 0.8)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    print("Concept formation capabilities:")
    print("  - Pattern clustering from sensory input")
    print("  - Prototype creation through averaging")
    print("  - Stability measurement for concept persistence")
    print()
    
    # Create diverse environment for concept learning
    # Resources at different positions
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0))
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, value=2.0))
    world.addObject(pynlm.WorldObject(25.0, 5.0, pynlm.WorldObjectType.Resource, value=1.5))
    
    # Hazards at different positions  
    world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, value=0.5))
    world.addObject(pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectType.Hazard, value=0.8))
    
    # Explore environment to form concepts
    for step in range(400):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on outcomes
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 100 == 0:
            print(f"Step {step}: Agent exploring for concepts...")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    
    print("Concept formation demonstration complete!")
    return brain, agent, world

# ============================================================================
# 4. NEURAL PLANNING AND GOAL-DIRECTED BEHAVIOR
# ============================================================================

def example_4_neural_planning():
    """Advanced neural planning with multi-step action sequences."""
    
    print("\n4. Advanced Neural Planning Example")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.22)
    config.set("neural_planner.planning_depth", 5)
    config.set("neural_planner.action_evaluation_threshold", 0.6)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    print("Neural planning capabilities:")
    print("  - Multi-step action sequence evaluation")
    print("  - Planning depth configuration")
    print("  - Action quality from experience")
    print("  - Plan success tracking")
    print()
    
    # Create goal at a distance
    goal_x, goal_y = 35.0, 35.0
    world.addObject(pynlm.WorldObject(goal_x, goal_y, pynlm.WorldObjectType.Resource, value=5.0))
    
    print(f"Goal set at coordinates ({goal_x}, {goal_y})")
    
    # Plan and execute towards goal
    for step in range(600):
        # Get sensory input for planning
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Neural planning (if available)
        if brain.getNeuralPlanner():
            planned_actions = brain.getNeuralPlanner().getCurrentPlan()
            print(f"  Planned {planned_actions.size()} actions for step {step}")
        
        # Execute action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check progress towards goal
        if step % 100 == 0:
            print(f"Step {step}: Planning and executing behavior...")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            
            # Check if we've reached the goal area
            agent_body = world.getAgentBody()
            distance_to_goal = math.sqrt((agent_body.x - goal_x)**2 + (agent_body.y - goal_y)**2)
            print(f"  Distance to goal: {distance_to_goal:.1f}")
    
    print("Neural planning demonstration complete!")
    return brain, agent, world

# ============================================================================
# 5. ADVANCED PYTHON FEATURES AND PERFORMANCE
# ============================================================================

def example_5_advanced_features():
    """Demonstrate advanced Python features and performance profiling."""
    
    print("\n5. Advanced Python Features Example")
    print("-" * 40)
    
    # Create highly optimized configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.25)
    config.set("brain.initial_weight_mean", 0.6)
    config.set("brain.initial_weight_std", 0.15)
    config.set("performance.enable_profiling", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Performance profiling capabilities:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print()
    
    # Performance benchmarking
    steps_to_test = [100, 500, 1000]
    
    for target_steps in steps_to_test:
        print(f"Benchmarking {target_steps} steps...")
        
        start_time = time.time()
        
        for step in range(target_steps):
            brain.step(step)
            
            # Collect performance metrics
            if step % 100 == 0:
                firing_rate = brain.getAverageFiringRate()
                neuron_count = brain.getFiringNeuronCount()
                e_i_ratio = brain.getExcitationInhibitionRatio()
                
                print(f"    Step {step}: FR={firing_rate:.3f}, Firing={neuron_count}, E/I={e_i_ratio:.3f}")
        
        end_time = time.time()
        elapsed = end_time - start_time
        steps_per_second = target_steps / elapsed
        
        print(f"  Performance: {steps_per_second:.1f} steps/second")
        print()
    
    print("Advanced Python features demonstration complete!")
    return brain

# ============================================================================
# 6. MEMORY CONSOLIDATION AND REPLAY
# ============================================================================

def example_6_memory_consolidation():
    """Demonstrate memory consolidation and replay mechanisms."""
    
    print("\n6. Memory Consolidation and Replay Example")
    print("-" * 45)
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.18)
    config.set("episodic_memory.consolidation_interval", 200)
    config.set("episodic_memory.replay_interval", 300)
    config.set("episodic_memory.replay_episodes", 3)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    print("Memory consolidation and replay capabilities:")
    print("  - Periodic memory consolidation")
    print("  - Replay of important experiences")
    print("  - Working memory integration with episodic memory")
    print()
    
    # Track memory events
    memory_stats = {
        'episodes_stored': 0,
        'consolidation_events': 0,
        'replay_events': 0,
        'important_experiences': 0
    }
    
    for step in range(800):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Phase 6 memory integration
        if brain.getEpisodicMemory():
            # Check for memory events
            episodes = brain.getEpisodicMemory().getAllEpisodes()
            memory_stats['episodes_stored'] = len(episodes)
            
            # Trigger consolidation periodically
            if step % config.get("episodic_memory.consolidation_interval") == 0:
                brain.getEpisodicMemory().consolidate(0.3f)
                memory_stats['consolidation_events'] += 1
                print(f"  Memory consolidation at step {step}")
            
            # Trigger replay periodically
            if step % config.get("episodic_memory.replay_interval") == 0:
                replay_episodes = brain.getEpisodicMemory().getEpisodesForReplay(config.get("episodic_memory.replay_episodes"))
                memory_stats['replay_events'] += 1
                memory_stats['important_experiences'] = len(replay_episodes)
                print(f"  Memory replay at step {step}: {len(replay_episodes)} episodes replayed")
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"Step {step}: Memory consolidation and replay active")
            print(f"  Stored episodes: {memory_stats['episodes_stored']}")
            print(f"  Consolidation events: {memory_stats['consolidation_events']}")
            print(f"  Replay events: {memory_stats['replay_events']}")
    
    print("\nMemory consolidation and replay demonstration complete!")
    print(f"Final memory stats: {memory_stats}")
    return brain, agent, world

# ============================================================================
# MAIN EXECUTION
# ============================================================================

if __name__ == "__main__":
    print("=" * 60)
    print("ADVANCED PHASE 6 EXAMPLES FOR NLM")
    print("=" * 60)
    print()
    
    try:
        # Run all examples (comment out any you don't want to run)
        print("Running Example 1: Advanced Working Memory")
        example_1_working_memory_advanced()
        
        print("\nRunning Example 2: Advanced Dopamine Neuromodulation")
        example_2_dopamine_advanced()
        
        print("\nRunning Example 3: Advanced Concept Formation")
        example_3_concept_formation()
        
        print("\nRunning Example 4: Advanced Neural Planning")
        example_4_neural_planning()
        
        print("\nRunning Example 5: Advanced Python Features")
        example_5_advanced_features()
        
        print("\nRunning Example 6: Memory Consolidation and Replay")
        example_6_memory_consolidation()
        
        print("\n" + "=" * 60)
        print("ALL ADVANCED PHASE 6 EXAMPLES COMPLETED SUCCESSFULLY!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()