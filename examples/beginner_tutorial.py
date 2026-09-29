#!/usr/bin/env python3
"""
NLM Beginner Tutorial

This tutorial introduces the basics of NLM (Neural Learning Machine)
for beginners who want to understand how to create and run neural simulations.

Prerequisites:
- Python 3.8 or higher
- NLM Python bindings installed (pynlm)
- Basic understanding of neural networks (not required)

What you'll learn:
1. Creating a simple brain with configuration
2. Basic simulation loop
3. Sensory input and motor output
4. Understanding the core concepts
"""

import sys
import os
import time

# Add the src directory to the path for direct imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    from brain.Brain import Brain
    from sensory.SensoryInput import SensoryInput
    from agent.AgentBrain import AgentBrain
    from world.SimpleWorld import SimpleWorld
    from core.Logger.Logger import NLM_LOG_INFO, NLM_LOG_ERROR
except ImportError as e:
    print(f"Error importing NLM modules: {e}")
    print("Make sure NLM is properly built and the Python bindings are installed.")
    sys.exit(1)

def print_header(title):
    """Print formatted header"""
    print("\n" + "=" * 70)
    print(f"  {title}")
    print("=" * 70)

def step_1_create_basic_brain():
    """Step 1: Create a basic brain with minimal configuration"""
    print_header("Step 1: Creating Basic Brain")
    
    print("\n1.1 Creating configuration for a simple brain...")
    config = Config()
    
    print("\n1.2 Setting basic parameters...")
    config.set("neuron_count", 500)
    config.set("plasticity_rate", 0.1f)
    config.set("working_memory_capacity", 100)
    config.set("prediction_horizon", 3)
    
    print("\n1.3 Creating brain instance...")
    brain = Brain(config)
    
    print("\n1.4 Initializing brain...")
    if brain.initialize():
        print("   ✓ Brain initialized successfully")
        print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"   ✓ {brain.getTotalSynapseCount()} synapses available")
    else:
        print("   ✗ Brain initialization failed")
        return None
    
    return brain

def step_2_create_sensory_input(brain):
    """Step 2: Create and process sensory input"""
    print_header("Step 2: Creating and Processing Sensory Input")
    
    print("\n2.1 Creating sensory input (10x10 grayscale)...")
    input_ = SensoryInput(10, 10)
    
    print("\n2.2 Adding visual pattern (diagonal bright pixels)...")
    for i in range(10):
        input_.setPixel(i, i, 1.0f)  # Diagonal pixels
        input_.setPixel(9-i, i, 0.5f)  # Anti-diagonal
    
    print("\n2.3 Displaying input pattern...")
    for y in range(10):
        line = "   "
        for x in range(10):
            pixel = input_.getPixel(x, y)
            line += "*" if pixel > 0.5 else "-"
        print(f"      {line}")
    
    print("\n2.4 Processing input through brain...")
    brain.receiveSensoryInput(input_)
    print("   ✓ Input processed successfully")
    
    return input_

def step_3_run_simulation(brain, steps=50):
    """Step 3: Run simulation loop"""
    print_header(f"Step 3: Running Simulation ({steps} Steps)")
    
    print(f"\n3.1 Starting simulation with {steps} steps...")
    
    for step in range(steps):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"   Step {step:3d}: {brain.getTotalSpikeCount()} spikes total")
            print(f"            Active neurons: {brain.getActiveNeuronCount()}")
    
    print("\n3.2 Simulation completed successfully")
    print(f"   ✓ Total spikes processed: {brain.getTotalSpikeCount()}")
    print(f"   ✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   ✓ Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    return steps

def step_4_get_output(brain):
    """Step 4: Get motor output from brain"""
    print_header("Step 4: Getting Motor Output")
    
    print("\n4.1 Extracting action from brain...")
    action = brain.produceAction()
    
    if action:
        print("\n4.2 Action details:")
        print(f"   ✓ Action type: {action.getType()}")
        print(f"   ✓ Confidence: {action.getConfidence():.2f}")
        print(f"   ✓ Duration: {action.getDuration()} ms")
        
        # Show what this action means
        action_types = ["MOVE_FORWARD", "MOVE_BACKWARD", "TURN_LEFT", "TURN_RIGHT", "INTERACT", "WAIT"]
        action_type = action.getType()
        if hasattr(action, 'getDirection'):
            print(f"   ✓ Direction: {action.getDirection()}")
        
        print(f"\n   Interpretation: This action means the agent will {get_action_description(action)}")
    else:
        print("\n4.2 No action selected (brain in inactive state)")
    
    return action

def get_action_description(action):
    """Get human-readable description of action"""
    action_type = action.getType()
    
    if "forward" in action_type.lower():
        return "move forward"
    elif "backward" in action_type.lower():
        return "move backward"
    elif "left" in action_type.lower():
        return "turn left"
    elif "right" in action_type.lower():
        return "turn right"
    elif "interact" in action_type.lower():
        return "interact with environment"
    elif "wait" in action_type.lower():
        return "wait/idle"
    else:
        return f"perform {action_type}"

def step_5_understanding_memory(brain):
    """Step 5: Explore working memory"""
    print_header("Step 5: Understanding Working Memory")
    
    print("\n5.1 Checking working memory...")
    working_memory = brain.getWorkingMemory()
    
    if working_memory:
        print(f"   ✓ Working memory exists")
        print(f"   ✓ Current capacity: {working_memory.getCapacity()}")
        print(f"   ✓ Current entries: {working_memory.getEntryCount()}")
        
        print("\n5.2 Storing information in working memory...")
        working_memory.store("current_time", 0.0f)
        working_memory.store("goal", 1.0f)
        working_memory.store("reward", 0.5f)
        
        print("\n5.3 Retrieving information...")
        time_val = working_memory.retrieve("current_time")
        goal = working_memory.retrieve("goal")
        reward = working_memory.retrieve("reward")
        
        print(f"   ✓ Retrieved time: {time_val}")
        print(f"   ✓ Retrieved goal: {goal}")
        print(f"   ✓ Retrieved reward: {reward}")
        
        print("\n5.4 Memory contents:")
        entries = working_memory.getAllEntries()
        for key, value in entries.items():
            print(f"      {key}: {value}")
    else:
        print("   ✗ Working memory not available")
    
    return working_memory

def step_6_complete_example():
    """Complete example combining all steps"""
    print_header("Complete Beginner Example: Simple Neural Simulation")
    
    print("\n6.1 This example demonstrates a complete neural simulation...")
    print("   We'll create a brain, process sensory input, and get motor output.")
    print("   This is a self-contained example that beginners can run and understand.")
    
    # Step 1: Create brain
    brain = step_1_create_basic_brain()
    if not brain:
        return
    
    # Step 2: Create and process input
    input_ = step_2_create_sensory_input(brain)
    
    # Step 3: Run simulation
    step_3_run_simulation(brain, 30)
    
    # Step 4: Get output
    action = step_4_get_output(brain)
    
    # Step 5: Explore memory
    working_memory = step_5_understanding_memory(brain)
    
    print("\n6.2 Example completed successfully!")
    print("\nKey takeaways:")
    print("   ✓ Brains are created with configuration")
    print("   ✓ Sensory input drives neural activity")
    print("   ✓ Simulation produces motor output")
    print("   ✓ Working memory stores and retrieves information")
    print("   ✓ Neural computation is driven by spikes and plasticity")
    
    return brain, input_, action, working_memory

def demonstrate_agent_world_integration():
    """Demonstrate agent-world integration (optional extension)"""
    print_header("Optional: Agent-World Integration")
    
    print("\nThis section shows how to connect the brain to a world environment...")
    print("\n7.1 Creating world environment...")
    
    try:
        world = SimpleWorld(32, 32)
        world.initialize()
        print("   ✓ World created and initialized")
        
        print("\n7.2 Creating agent brain...")
        config = Config()
        config.set("neuron_count", 200)
        
        brain = Brain(config)
        brain.initialize()
        
        agentBrain = AgentBrain(brain)
        agentBrain.initialize(world)
        print("   ✓ Agent brain created and initialized")
        
        print("\n7.3 Running agent in world...")
        print("      This would typically involve:")
        print("      1. Processing sensory input from world")
        print("      2. Getting motor output from brain")
        print("      3. Applying action to world")
        print("      4. Receiving feedback (reward, state change)")
        print("      5. Learning from experience")
        
        print("\n   For complete agent-world example, see intermediate_tutorial.py")
        
    except Exception as e:
        print(f"   Note: Agent-world example requires more setup: {e}")

def main():
    """Main tutorial function"""
    print_header("NLM Beginner Tutorial")
    print("\nThis tutorial introduces the basics of NLM (Neural Learning Machine)")
    print("for beginners who want to understand neural simulation.")
    
    print("\nPrerequisites:")
    print("  • Python 3.8 or higher")
    print("  • NLM Python bindings installed")
    print("  • C++ compiler for NLM core")
    
    choice = input("\nChoose tutorial option:")
    print("\n1. Step-by-step tutorial")
    print("2. Complete example")
    print("3. Agent-world integration demo")
    print("4. Exit")
    
    choice = input("\nEnter your choice (1-4): ").strip()
    
    if choice == "1":
        # Step-by-step tutorial
        brain = step_1_create_basic_brain()
        if brain:
            step_2_create_sensory_input(brain)
            step_3_run_simulation(brain, 20)
            step_4_get_output(brain)
            step_5_understanding_memory(brain)
            
            print("\n" + "=" * 70)
            print("Beginner tutorial completed successfully!")
            print("=" * 70)
            print("\nYou have learned:")
            print("  ✓ How to create a neural brain")
            print("  ✓ How to process sensory input")
            print("  ✓ How to run neural simulations")
            print("  ✓ How to get motor output")
            print("  ✓ How working memory works")
            
    elif choice == "2":
        # Complete example
        step_6_complete_example()
    
    elif choice == "3":
        # Agent-world demo
        demonstrate_agent_world_integration()
    
    elif choice == "4" or choice == "":
        print("\nTutorial exited by user.")
    
    else:
        print(f"\nInvalid choice: {choice}")
        print("Please run the tutorial again and select a valid option.")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nTutorial interrupted by user.")
        sys.exit(0)
    except Exception as e:
        print(f"\n\nTutorial error: {e}")
        print("Please check your NLM installation.")
        sys.exit(1)