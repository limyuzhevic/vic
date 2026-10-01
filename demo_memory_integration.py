#!/usr/bin/env python3

"""
NLM Python Example: Memory-Integrated Agent

This example demonstrates how to use NLM with memory-integrated agent brain
that stores and retrieves experiences to guide behavior.

Key features:
- AgentBrain with memory systems integration
- Working memory for active information storage
- Episodic memory for experience storage
- Memory-based action selection
- Memory-based reward modulation
"""

import pynlm
import time
import numpy as np

def create_memory_integrated_agent():
    """Create an agent brain with memory systems integration"""
    
    print("=== Creating Memory-Integrated Agent ===")
    
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable memory systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons")
    print(f"World configured: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent brain initialized")
    
    # Check memory integration
    workingMemory = agent.getWorkingMemory()
    episodicMemory = agent.getEpisodicMemory()
    associativeMemory = agent.getAssociativeMemory()
    predictionSystem = agent.getPredictionSystem()
    
    print(f"\nMemory Integration Status:")
    print(f"  WorkingMemory: {'ACTIVE' if workingMemory else 'INACTIVE'}")
    print(f"  EpisodicMemory: {'ACTIVE' if episodicMemory else 'INACTIVE'}")
    print(f"  AssociativeMemory: {'ACTIVE' if associativeMemory else 'INACTIVE'}")
    print(f"  PredictionSystem: {'ACTIVE' if predictionSystem else 'INACTIVE'}")
    print(f"  AgentMemoryIntegrated: {agent.isMemoryIntegrated()}")
    
    return brain, world, agent

def run_memory_demo_simulation(brain, world, agent, num_steps=100):
    """Run a simulation that demonstrates memory integration"""
    
    print(f"\n=== Running Memory Demo Simulation ({num_steps} steps) ===")
    
    sensory_history = []
    reward_history = []
    action_history = []
    memory_activity_history = []
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get observation (sensory input)
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input WITH memory integration
        # This uses AgentBrain::processSensoryInputWithMemory() which stores in working memory
        agent.processSensoryInputWithMemory(*percept)
        
        # 4. Get memory stats before brain step
        workingMemory = agent.getWorkingMemory()
        memory_activity = workingMemory.getMemoryActivity() if workingMemory else 0.0
        memory_activity_history.append(memory_activity)
        
        # 5. Brain thinks (updates working memory)
        brain.step(step, step * 0.001)
        
        # 6. Get action WITH memory integration
        # This uses AgentBrain::decodeMotorCommandWithMemory() which uses memory patterns
        action = agent.decodeMotorCommandWithMemory()
        
        # 7. Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 8. Compute reward
        reward = world.computeReward(*world.getAgentBody())
        
        # 9. Apply reward modulation WITH memory integration
        # This uses AgentBrain::applyRewardModulationWithMemory() which stores experiences
        agent.applyRewardModulationWithMemory(reward, agent.getPredictionError())
        
        # 10. Update memory systems
        agent.updateMemorySystems(0.1)
        
        # Store history
        sensory_history.append(percept.getVision())
        reward_history.append(reward)
        action_history.append(action)
        
        if step % 20 == 0:
            print(f"Step {step:3d}: "
                  f"Reward: {reward:+.3f}, "
                  f"Action: {action}, "
                  f"Memory: {memory_activity:.3f}, "
                  f"Spikes: {brain.getTotalSpikeCount()}")
    
    print(f"\n=== Simulation Complete ===")
    print(f"Total steps: {num_steps}")
    print(f"Average memory activity: {np.mean(memory_activity_history):.3f}")
    print(f"Memory activity std: {np.std(memory_activity_history):.3f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Store experiences at the end
    store_final_experiences(agent, sensory_history, reward_history, action_history)
    
    return sensory_history, reward_history, action_history, memory_activity_history

def store_final_experiences(agent, sensory_history, reward_history, action_history):
    """Store final experiences in episodic memory"""
    
    print(f"\n=== Storing Final Experiences ===")
    
    episodicMemory = agent.getEpisodicMemory()
    if not episodicMemory:
        print("No episodic memory available - skipping experience storage")
        return
    
    # Create experiences from simulation history
    num_episodes = min(len(sensory_history), 20)  # Store last 20 episodes
    
    for i in range(num_episodes):
        episode_idx = len(sensory_history) - 1 - i
        
        # Create experience
        reward = reward_history[episode_idx]
        action = action_history[episode_idx]
        
        # Create sensory patterns
        sensory_state = sensory_history[episode_idx]
        resulting_sensory_state = [] if episode_idx + 1 >= len(sensory_history) else sensory_history[episode_idx + 1]
        
        # Store experience using AgentBrain::storeExperience()
        agent.storeExperience(reward, sensory_state, resulting_sensory_state, action)
        
        if i % 5 == 0:
            print(f"Stored experience {i+1}/{num_episodes}: "
                  f"reward={reward:+.3f}, action={action}, "
                  f"memory_count={episodicMemory.getEpisodeCount()}")
    
    print(f"\nTotal episodes stored: {episodicMemory.getEpisodeCount()}")
    
    # Try to retrieve similar experiences
    if episodicMemory.getEpisodeCount() > 1:
        print(f"\n=== Retrieving Similar Experiences ===")
        
        # Use recent sensory patterns to retrieve similar experiences
        recent_sensory = sensory_history[-5:] if len(sensory_history) >= 5 else sensory_history
        if recent_sensory:
            # Average recent sensory patterns
            avg_sensory = []
            for i in range(min(5, len(recent_sensory[0]))):
                total = 0.0
                count = 0
                for pattern in recent_sensory:
                    if i < len(pattern):
                        total += pattern[i]
                        count += 1
                avg_sensory.append(total / count if count > 0 else 0.0)
            
            similar_episodes = agent.retrieveSimilarExperiences(avg_sensory)
            print(f"Retrieved {len(similar_episodes)} similar experiences")
            
            for i, episode in enumerate(similar_episodes):
                print(f"  Experience {i+1}: reward={episode.reward:+.3f}, "
                      f"action={episode.action}, neurons={len(episode.neurons)}")

def run_advanced_memory_demo():
    """Run an advanced demo showing memory-based decision making""""
    
    print("=== Advanced Memory Demo: Experience-Drive Behavior ===")
    
    
    # Create agent
    brain, world, agent = create_memory_integrated_agent()
    
    # Enable curiosity for exploration
    agent.enableCuriosity(True)
    
    print(f"\nMemory-integrated agent created with curiosity enabled")
    
    # Run simulation
    sensory_history, reward_history, action_history, memory_history = \
        run_memory_demo_simulation(brain, world, agent, 50)
    
    # Analyze memory patterns
    print(f"\n=== Memory Analysis ===")
    print(f"High memory activity episodes (potential learning):")
    high_memory_episodes = [i for i, activity in enumerate(memory_history) if activity > 0.5]
    print(f"  - Episode {i+1}: activity={activity:.2f}")
    
    # Check what the agent learned
    episodicMemory = agent.getEpisodicMemory()
    if episodicMemory:
        print(f"\nEpisodic memory contains {episodicMemory.getEpisodeCount()} experiences")
        
        # Show recent experiences
        recent_episodes = episodicMemory.getRecentEpisodes(3)
        print(f"Recent experiences:")
        for i, episode in enumerate(recent_episodes):
            print(f"  - Reward: {episode.reward:+.2f}, Action: {episode.action}")
    
    print(f"\n=== Advanced Memory Demo Complete ===")
    return brain, world, agent

def demonstrate_python_memory_api():
    """Demonstrate the Python memory API for advanced users"""
    
    print("=== Python Memory API Demonstration ===")
    
    # Create a simple agent for API demo
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 4, 4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all memory features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Get memory systems
    working_memory = agent.getWorkingMemory()
    episodic_memory = agent.getEpisodicMemory()
    associative_memory = agent.getAssociativeMemory()
    prediction_system = agent.getPredictionSystem()
    
    print(f"Memory Systems Available:")
    print(f"  WorkingMemory: {working_memory is not None}")
    print(f"  EpisodicMemory: {episodic_memory is not None}")
    print(f"  AssociativeMemory: {associative_memory is not None}")
    print(f"  PredictionSystem: {prediction_system is not None}")
    print(f"  MemoryIntegrated: {agent.isMemoryIntegrated()}")
    
    # Get memory stats
    memory_stats = agent.getMemoryStats()
    print(f"\nMemory Statistics:")
    for key, value in memory_stats.items():
        print(f"  {key}: {value}")
    
    # Demonstrate storing experiences
    print(f"\n=== Storing Experiences ===")
    
    # Create sample sensory patterns
    sensory1 = [0.1, 0.2, 0.3, 0.4, 0.5]
    sensory2 = [0.6, 0.7, 0.8, 0.9, 1.0]
    
    # Store experiences with different rewards and actions
    agent.storeExperience(1.0, sensory1, sensory2, pynlm.ActionType.MoveForward)
    agent.storeExperience(-0.5, sensory2, sensory1, pynlm.ActionType.TurnLeft)
    
    print(f"Stored 2 experiences")
    
    # Retrieve similar experiences
    similar = agent.retrieveSimilarExperiences(sensory1, maxResults=1)
    print(f"Retrieved {len(similar)} similar experiences")
    
    if similar:
        episode = similar[0]
        print(f"Retrieved experience: reward={episode.reward:+.2f}, action={episode.action}")
    
    print(f"\n=== Python Memory API Demo Complete ===")

def main():
    """Main function to run all demos"""
    
    print("NLM Memory Integration Demos")
    print("=" * 50)
    
    # Demo 1: Basic memory integration
    brain1, world1, agent1 = create_memory_integrated_agent()
    sensory1, reward1, action1, memory1 = run_memory_demo_simulation(brain1, world1, agent1, 50)
    
    # Demo 2: Advanced memory demo
    brain2, world2, agent2 = run_advanced_memory_demo()
    
    # Demo 3: Python API demonstration
    demonstrate_python_memory_api()
    
    print(f"\n{'=' * 50}")
    print(f"All demos completed successfully!")
    print(f"\nKey improvements:")
    print(f"  ✓ AgentBrain now integrates with memory systems")
    print(f"  ✓ Working memory stores sensory patterns for active use")
    print(f"  ✓ Episodic memory stores complete experiences")
    print(f"  ✓ Memory systems guide action selection")
    print(f"  ✓ Memory-based reward modulation stores experiences")
    print(f"  ✓ Python bindings expose memory system functionality")
    print(f"\nAdvanced users can now use the full memory integration API.")
    
    return 0

if __name__ == "__main__":
    main()