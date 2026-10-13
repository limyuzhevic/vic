"""Example 04: Memory Systems

This example demonstrates the memory systems in NLM: working memory,
episodic memory, and associative memory.
"""

import pynlm
import time

print("=== Example 4: Memory Systems ===")
print()

# Create brain with memory focus
print("Creating brain with enhanced memory capacity...")
config = pynlm.createDefaultConfig()
config.neuron_count = 300
config.working_memory_capacity = 200  # Large working memory
config.episodic_memory_max_episodes = 500
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent brain
agent = pynlm.createAgentBrain(brain)

# Create simple world
world = pynlm.createSimpleWorld()
world.configure(width=60, height=60, vision_width=12, vision_height=12)
agent.initialize(world)

# Add interesting objects to remember
print("Setting up world with memorable objects...")
# Resource (good memory)
resource1 = pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectType.Resource, 50.0, 2.0)
world.addObject(resource1)

# Hazard (negative memory)  
resource2 = pynlm.createWorldObject(40.0, 40.0, pynlm.WorldObjectType.Resource, 30.0, 2.0)
world.addObject(resource2)

# Create a marker for goal
marker = pynlm.createWorldObject(50.0, 50.0, pynlm.WorldObjectType.Marker, 100.0, 3.0)
world.addObject(marker)

print(f"World objects: Resource1, Resource2, Marker at (50, 50)")

# Memory exploration function
def explore_and_memorize(agent_brain, simulation_step, max_steps=50):
    """Explore world and record memories"""
    print(f"\n--- Memory Exploration Phase (Steps {simulation_step} - {simulation_step + max_steps}) ---")
    
    memories = []
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Brain processes
        brain.step(simulation_step + step)
        
        # Get action
        action = agent_brain.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.get_simulation_time())
        
        # Learn from reward
        agent_brain.apply_reward_modulation(result.reward, agent_brain.get_prediction_error())
        
        # Store memory of this experience
        memory = {
            'step': simulation_step + step,
            'action': action,
            'reward': result.reward,
            'success': result.success,
            'vision': percept.get_vision(5, 5),  # Central vision
            'timestamp': time.time(),
        }
        memories.append(memory)
        
        # Log occasionally
        if step % 10 == 0:
            print(f"  Step {simulation_step + step:3d}: Action {action.name:15s} | "
                  f"Reward {result.reward:5.1f} | Success {result.success:5s}")
    
    return memories

# Phase 1: Initial exploration
print("Phase 1: Initial exploration and memory formation...")
initial_memories = explore_and_memorize(agent, 0, 30)

# Analyze initial memories
print("\n--- Initial Memory Analysis ---")
positive_memories = sum(1 for m in initial_memories if m['reward'] > 0)
negative_memories = sum(1 for m in initial_memories if m['reward'] < 0)
neutral_memories = sum(1 for m in initial_memories if m['reward'] == 0)

print(f"Total experiences recorded: {len(initial_memories)}")
print(f"Positive memories (rewards): {positive_memories}")
print(f"Negative memories (penalties): {negative_memories}")
print(f"Neutral memories: {neutral_memories}")
print(f"Average reward per step: {sum(m['reward'] for m in initial_memories) / len(initial_memories):.2f}")

# Phase 2: Repeated exploration (memory consolidation)
print("\nPhase 2: Repeated exploration (memory consolidation)...")
# Reset world for fresh exploration
world.reset()
repeated_memories = explore_and_memorize(agent, 100, 30)

# Compare memories
print("\n--- Memory Comparison ---")
initial_avg_reward = sum(m['reward'] for m in initial_memories) / len(initial_memories)
repeated_avg_reward = sum(m['reward'] for m in repeated_memories) / len(repeated_memories)

print(f"Initial exploration average reward: {initial_avg_reward:.2f}")
print(f"Repeated exploration average reward: {repeated_avg_reward:.2f}")

if abs(initial_avg_reward - repeated_avg_reward) < 5.0:
    print("✓ Memory appears stable (good consolidation)")
else:
    print("⚠ Memory changed significantly (potential instability)")

# Phase 3: Goal-directed exploration
print("\nPhase 3: Goal-directed exploration...")
# Modify agent to be more goal-directed (this would require more sophisticated agent implementation)
# For this demo, we'll just run a simple sequence
for step in range(20):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.process_sensoryInput(percept)
    brain.step(200 + step)
    action = agent.decode_motor_command()
    result = world.apply_motor_command(action, world.get_simulation_time())
    agent.apply_reward_modulation(result.reward, agent.get_prediction_error())
    
    if step % 5 == 0:
        print(f"  Step {200 + step:3d}: Action {action.name:15s} | "
              f"Reward {result.reward:5.1f} | Success {result.success:5s}")

# Memory statistics
print("\n--- Memory System Statistics ---")
working_memory = brain.get_working_memory()
if working_memory:
    print(f"Working memory capacity: {working_memory.get_capacity()}")
    print(f"Working memory active traces: {working_memory.get_active_traces()}")

episodic_memory = brain.get_episodic_memory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.get_episode_count()}")
    print(f"Episodic memory max episodes: {episodic_memory.get_max_episodes()}")

associative_memory = brain.get_associative_memory()
if associative_memory:
    print(f"Associative memory entries: {associative_memory.get_entry_count()}")

# Final assessment
print("\n=== Memory System Assessment ===")
total_memories = len(initial_memories) + len(repeated_memories)
memory_efficiency = (learning_stats['total_reward'] / max(1, total_memories)) if 'learning_stats' in locals() else 0.0

print(f"Total experiences recorded: {total_memories}")
if total_memories > 0:
    print(f"Average reward per experience: {memory_efficiency:.2f}")

print(f"Memory system utilization:")
if working_memory:
    print(f"  Working memory: {working_memory.get_active_traces() / working_memory.get_capacity() * 100:.1f}%")
if episodic_memory:
    print(f"  Episodic memory: {episodic_memory.get_episode_count() / episodic_memory.get_max_episodes() * 100:.1f}%")

print()
print("✓ Example completed successfully!")

# Export memories for analysis
all_memories = initial_memories + repeated_memories
print(f"\nTotal memories exported: {len(all_memories)}")
print("Sample memory entries:")
for i, mem in enumerate(all_memories[:3]):
    print(f"  Memory {i+1}: Step {mem['step']}, Action {mem['action']}, Reward {mem['reward']:.1f}, Success {mem['success']}")
