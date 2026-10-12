#!/usr/bin/env python3
"""
NLM Example 3: World Interaction

This example demonstrates how an agent interacts with a world environment.
It shows the complete cycle: perception -> processing -> action -> feedback.

Learning Objectives:
- Understand world-environment interaction
- Learn agent-brain-world integration
- Master sensory input and motor output
- See how agents make decisions

Time Complexity: O(n_steps × (world_processing + neural_processing))

Expected Output:
```
=== World Interaction Example ===
World created (20x20) with vision (8x8)
Agent initialized with brain
Starting interaction loop...
Step 0: Vision=...x... Action=MoveForward
Step 1: Vision=...x... Action=TurnLeft
...
Final statistics:
  Agent position: (18.5, 12.3)
  Total steps: 100
  Curiosity level: 0.234
  Novelty level: 0.156
  Development stage: Maturation (stage 2)
=== Example Complete ===
```

Tips for Customization:
- Change world size and vision parameters
- Add more agent capabilities
- Enable curiosity and learning
- Modify agent starting position

Key Concepts:
1. Sensorimotor loop: world → agent → brain → world
2. Visual processing and decision making
3. Action selection and execution
4. Learning from experience
"""

import pynlm
import time
import random

def main():
    print("=== World Interaction Example ===")
    print("This example demonstrates how an agent interacts with a world.")
    print("We'll create a complete agent system that perceives, decides,")
    print("and acts in a simulated environment.\n")
    
    # Step 1: Setup components
    print("Step 1: Creating agent, brain, and world components...")
    
    # Create brain with advanced capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with vision
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    
    # Set agent starting position
    start_x = random.uniform(5.0, 15.0)
    start_y = random.uniform(5.0, 15.0)
    world.setAgentStart(start_x, start_y)
    
    world.reset()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"World configured: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent starting position: ({start_x:.1f}, {start_y:.1f})")
    print(f"Vision field: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"Learning systems enabled: Reward={agent.getRewardModulationLevel():.1f}, "
          f"Curiosity={agent.getCuriosityLevel():.1f}")
    
    # Step 2: Run interaction loop
    print("\nStep 2: Running interaction loop...")
    print("The agent will perceive, decide, and act in the world.\n")
    
    # Statistics collection
    positions = []
    curiosity_levels = []
    novelty_levels = []
    action_counts = {}
    
    for step in range(100):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory percept (what agent sees)
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # 4. Run brain step
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation (agent learns from experience)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        body = world.getAgentBody()
        positions.append((body.x, body.y))
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        
        action_name = action.getActionType().name
        action_counts[action_name] = action_counts.get(action_name, 0) + 1
        
        # Log progress
        if step % 20 == 0:
            print(f"Step {step}: Pos=({body.x:.1f},{body.y:.1f}) "
                  f"Action={action_name} "
                  f"Curiosity={agent.getCuriosityLevel():.3f} "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    # Step 3: Analyze results
    print("\nStep 3: Analyzing interaction results...")
    
    # Position analysis
    final_pos = positions[-1]
    start_pos = positions[0]
    distance_traveled = ((final_pos[0] - start_pos[0])**2 + 
                        (final_pos[1] - start_pos[1])**2)**0.5
    
    # Statistics
    avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
    avg_novelty = sum(novelty_levels) / len(novelty_levels)
    
    print(f"\n=== Final Statistics ===")
    print(f"Agent position: ({final_pos[0]:.1f}, {final_pos[1]:.1f})")
    print(f"Distance traveled: {distance_traveled:.1f} units")
    print(f"Total steps: 100")
    print(f"Average curiosity: {avg_curiosity:.3f}")
    print(f"Average novelty: {avg_novelty:.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage().name}")
    
    # Action analysis
    print(f"\nAction breakdown:")
    for action, count in sorted(action_counts.items(), key=lambda x: x[1], reverse=True):
        print(f"  {action}: {count} times")
    
    # Step 4: Observations
    print("\n=== Understanding the Interaction ===")
    print("Key observations:")
    print("1. The agent successfully perceived its environment")
    print("2. The brain processed sensory information")
    print("3. The agent made decisions based on neural activity")
    print("4. Actions affected the world state")
    print("5. The brain learned from rewards and novelty")
    
    print("\nSensorimotor loop working:")
    print("World → Vision → Brain → Decision → Action → World")
    print("           ↑                 ↑")
    print("      Learning ←←← Reward")
    
    print("\n=== Example Complete ===")
    print("You now understand how agents interact with worlds!")
    print("Next: Try examples/plasticity_learning.py for learning examples.")

if __name__ == "__main__":
    start_time = time.time()
    main()
    end_time = time.time()
    print(f"\nExecution time: {end_time - start_time:.2f} seconds")
