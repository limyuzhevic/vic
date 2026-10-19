#!/usr/bin/env python3
"""
Agent-Brain Example

This example demonstrates the agent-brain interface integration between NLM and the external world.
It shows how to create an agent that can process sensory input, make decisions, and interact with the world.

Author: NLM Examples Team
Date: 2026-10-19
Version: 1.0
"""

import sys
import os
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import pynlm
from datetime import datetime

class AgentBrainExample:
    """Example class demonstrating agent-brain integration."""
    
    def __init__(self):
        """Initialize the agent-brain example."""
        self.brain = None
        self.world = None
        self.agent = None
        self.config = None
        self.example_name = "Agent-Brain Integration"
        
    def create_basic_world(self, width=15, height=15):
        """
        Create a basic world for agent interaction.
        
        Args:
            width: World width in cells
            height: World height in cells
        """
        print("Creating basic world environment...")
        
        # Create simple world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 10, 10)
        self.world.reset()
        
        # Set up some world objects
        # Add a resource in the middle
        mid_x, mid_y = width // 2, height // 2
        resource = pynlm.WorldObject(mid_x, mid_y, pynlm.WorldObjectType.Resource, 1.0, 0.3)
        self.world.addObject(resource)
        
        print(f"  - World created: {width}x{height} cells")
        print(f"  - Resource placed at ({mid_x}, {mid_y})")
        
    def create_and_initialize_brain(self):
        """Create and initialize the brain for the agent."""
        print("Creating and initializing brain...")
        
        # Create default configuration
        self.config = pynlm.createDefaultConfig()
        print(f"  - Default configuration created")
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        print(f"  - Brain created")
        
        # Initialize brain
        self.brain.initialize()
        print(f"  - Brain initialized")
        
    def create_and_initialize_agent(self):
        """Create and initialize the agent brain interface."""
        print("Creating agent brain interface...")
        
        # Create agent brain
        self.agent = pynlm.createAgentBrain(self.brain)
        print(f"  - Agent brain interface created")
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        print(f"  - Agent initialized with world")
        
        # Display agent capabilities
        sensory_size = self.agent.getSensoryInputSize()
        motor_size = self.agent.getMotorOutputSize()
        print(f"  - Sensory input size: {sensory_size}")
        print(f"  - Motor output size: {motor_size}")
        
    def demonstrate_sensorimotor_loop(self, steps=50):
        """
        Demonstrate the sensorimotor loop where agent perceives and acts.
        
        Args:
            steps: Number of steps to run the sensorimotor loop
        """
        print(f"Demonstrating sensorimotor loop ({steps} steps)...")
        
        for step in range(steps):
            # 1. PERCEPTION: Get sensory input from world
            percept = self.world.getSensoryPercept()
            print(f"  Step {step:3d}: Perception - Vision range: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
            
            # 2. PROCESSING: Process sensory input through agent and brain
            self.agent.processSensoryInput(percept)
            print(f"           Processing: Sensory input injected into brain")
            
            # 3. ACTION: Brain produces motor command
            brain_action = self.brain.produceAction()
            print(f"           Brain produced action: {brain_action}")
            
            # 4. ACTIVATION: Agent decodes motor command
            motor_command = self.agent.decodeMotorCommand()
            print(f"           Agent decoded: {motor_command}")
            
            # 5. EXECUTION: Apply action to world
            result = self.world.applyMotorCommand(motor_command, self.world.getSimulationTime())
            print(f"           Action result: Success={result.success}, Reward={result.reward}")
            
            # 6. INTEGRATION: Handle reward and neuromodulation
            if result.reward > 0:
                self.agent.applyRewardModulation(result.reward, result.reward * 0.8)  # Predicted reward
                print(f"           Applied reward modulation: {result.reward}")
            
            # 7. DEVELOPMENT: Update developmental systems
            self.agent.updateDevelopment(step)
            print(f"           Developmental stage updated")
            
            # Check neuromodulation levels
            if step % 10 == 0:
                neuromod_level = self.agent.getNeuromodulationLevel()
                print(f"           Neuromodulation level: {neuromod_level:.3f}")
            
            print()
            
        print(f"Sensorimotor loop completed: {steps} steps")
        
    def demonstrate_learning_capabilities(self, steps=30):
        """
        Demonstrate learning capabilities including neuromodulation and development.
        
        Args:
            steps: Number of learning steps to demonstrate
        """
        print(f"Demonstrating learning capabilities ({steps} steps)...")
        
        # Enable neuromodulation for learning
        print("Enabling neuromodulation systems...")
        self.agent.enable_reward_modulation(True)
        self.agent.enable_curiosity(True)
        print("  - Reward modulation enabled")
        print("  - Curiosity enabled")
        
        # Run learning simulation
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get percept
            percept = self.world.getSensoryPercept()
            
            # Process with agent (triggers learning systems)
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Display learning progress
            if step % 10 == 0:
                stage = self.agent.getDevelopmentalStage()
                neuromod = self.agent.getNeuromodulationLevel()
                print(f"  Step {step:3d}: Stage={stage}, Neuromod={neurom:.3f}")
                
        print(f"Learning demonstration completed: {steps} steps")
        
    def demonstrate_agent_capabilities(self):
        """Demonstrate various agent capabilities and features."""
        print("Demonstrating agent capabilities...")
        
        # 1. Get agent information
        print("1. Agent Information:")
        sensory_size = self.agent.getSensoryInputSize()
        motor_size = self.agent.getMotorOutputSize()
        print(f"   - Sensory input size: {sensory_size}")
        print(f"   - Motor output size: {motor_size}")
        
        # 2. Show current developmental stage
        print("2. Developmental Stage:")
        stage = self.agent.getDevelopmentalStage()
        print(f"   - Current stage: {stage}")
        
        # 3. Check neuromodulation
        print("3. Neuromodulation:")
        neuromod_level = self.agent.getNeuromodulationLevel()
        print(f"   - Current level: {neuromod_level:.3f}")
        
        # 4. Show brain status
        print("4. Brain Status:")
        print(f"   - Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"   - Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"   - Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"   - Developmental stage: {self.brain.getDevelopmentalStage()}")
        
    def save_agent_results(self, brain_file="agent_brain.nld"):
        """
        Save agent simulation results.
        
        Args:
            brain_file: File path to save brain state
        """
        print("Saving agent simulation results...")
        
        # Save brain state
        if self.brain:
            self.brain.save(brain_file)
            print(f"  - Brain saved to: {brain_file}")
            
        # Note: Agent state is typically derived from brain state
        # World state would need custom serialization if needed
        
    def run_example(self, steps=50, learning_demo=True, save_results=False):
        """
        Run the complete agent-brain example.
        
        Args:
            steps: Number of simulation steps
            learning_demo: Whether to demonstrate learning capabilities
            save_results: Whether to save simulation results
        """
        print("=" * 60)
        print(f"NLM Agent-Brain Integration Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Step 1: Create basic world
            self.create_basic_world()
            print()
            
            # Step 2: Create and initialize brain
            self.create_and_initialize_brain()
            print()
            
            # Step 3: Create and initialize agent
            self.create_and_initialize_agent()
            print()
            
            # Step 4: Demonstrate sensorimotor loop
            self.demonstrate_sensorimotor_loop(steps)
            print()
            
            # Step 5: Demonstrate agent capabilities
            self.demonstrate_agent_capabilities()
            print()
            
            # Step 6: Demonstrate learning if requested
            if learning_demo:
                self.demonstrate_learning_capabilities(steps // 2)
                print()
            
            # Step 7: Save results if requested
            if save_results:
                self.save_agent_results()
                print()
            
            print("=" * 60)
            print("Agent-Brain Integration Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain, self.world, self.agent
            
        except Exception as e:
            print(f"Error in agent-brain example: {e}")
            import traceback
            traceback.print_exc()
            return None, None, None

if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Agent-Brain Integration Example")
    parser.add_argument("--steps", type=int, default=50, help="Number of simulation steps")
    parser.add_argument("--no-learning", action="store_true", help="Disable learning demonstration")
    parser.add_argument("--save", action="store_true", help="Save simulation results")
    
    args = parser.parse_args()
    
    # Run the agent-brain example
    example = AgentBrainExample()
    result = example.run_example(args.steps, not args.no_learning, args.save)
    
    if result[0] is not None:
        print("\nExample completed successfully. Agent-brain integration is working correctly.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)