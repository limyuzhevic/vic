#!/usr/bin/env python3
"""
NLM Example 3: Learning and Development

This script demonstrates how brains can learn, develop over time, and change
their behavior based on experience.

Key concepts covered:
- Developmental stages
- Reward-based learning
- Curiosity-driven exploration
- Structural plasticity
- Long-term learning effects
"""
import pynlm
import sys

def main():
    """Demonstrate learning and development over time"""
    print("=== Learning and Development Example ===")
    
    # 1. Setup learning brain
    print("1. Setting up learning brain...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    config.set("enable_curiosity", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_development", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.setAgentStart(7, 7)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures()
    
    # 2. Create learning environment
    print("2. Creating learning environment...")
    # Add resources that can be learned
    for x in range(3, 12, 3):
        for y in range(3, 12, 3):
            if (x != 7 or y != 7):  # Away from start
                world.addObject(pynlm.WorldObject(x, y, 
                                                 pynlm.WorldObjectType.Resource, 
                                                 5))
    
    # 3. Track learning progress
    print("3. Tracking learning progress...")
    learning_log = {
        "steps": [],
        "rewards": [],
        "firing_rates": [],
        "developmental_stages": [],
        "actions_taken": []
    }
    
    # 4. Run learning simulation
    print("4. Running learning simulation...")
    num_steps = 300
    for step in range(num_steps):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and learns
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        action_result = world.getActionResult()
        
        # Apply reward-based learning
        agent.applyRewardModulation(action_result.reward, 
                                   agent.getExpectedReward())
        
        # Log learning progress
        learning_log["steps"].append(step)
        learning_log["rewards"].append(action_result.reward)
        learning_log["firing_rates"].append(brain.getAverageFiringRate())
        learning_log["developmental_stages"].append(brain.getDevelopmentalStage())
        learning_log["actions_taken"].append(action)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            avg_reward = sum(learning_log["rewards"]) / len(learning_log["rewards"])
            avg_firing = sum(learning_log["firing_rates"]) / len(learning_log["firing_rates"])
            print(f"   Step {step}: Avg reward={avg_reward:.2f}, "
                  f"Avg firing={avg_firing:.3f}, "
                  f"Stage={brain.getDevelopmentalStage()}")
    
    # 5. Analyze learning results
    print("5. Analyzing learning results...")
    final_stage = brain.getDevelopmentalStage()
    avg_reward = sum(learning_log["rewards"]) / len(learning_log["rewards"])
    max_reward = max(learning_log["rewards"])
    
    print(f"   - Developmental stages reached: {set(learning_log['developmental_stages'])}")
    print(f"   - Final developmental stage: {final_stage}")
    print(f"   - Average reward per step: {avg_reward:.3f}")
    print(f"   - Maximum reward: {max_reward:.3f}")
    
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)