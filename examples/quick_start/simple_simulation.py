#!/usr/bin/env python3
"""
Simple Simulation Example

This example demonstrates a basic NLM brain simulation with a SimpleWorld.
It shows how to create a world, initialize an agent, and run a simulation loop.

Author: NLM Examples Team
Date: 2026-10-19
Version: 1.0
"""

import sys
import os
import time
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import pynlm
import numpy as np
from datetime import datetime

class SimpleSimulationExample:
    """Example class demonstrating a simple NLM brain simulation."""
    
    def __init__(self):
        """Initialize the simple simulation example."""
        self.brain = None
        self.world = None
        self.agent = None
        self.config = None
        self.example_name = "Simple Simulation"
        
    def setup_simulation_environment(self, width=20, height=20, vision_width=12, vision_height=12):
        """
        Set up the simulation environment with world configuration.
        
        Args:
            width: World width in cells
            height: World height in cells
            vision_width: Agent's vision width
            vision_height: Agent's vision height
        """
        print("Setting up simulation environment...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        print(f"  - World created with dimensions {width}x{height}")
        
        # Configure world
        self.world.configure(width, height, vision_width, vision_height)
        print(f"  - World configured: {vision_width}x{vision_height} vision")
        
        # Reset world to initial state
        self.world.reset()
        print(f"  - World reset to initial state")
        
        # Create agent start position (center)
        start_x = width // 2
        start_y = height // 2
        self.world.setAgentStart(start_x, start_y)
        print(f"  - Agent start position set to ({start_x}, {start_y})")
        
    def initialize_brain_and_agent(self):
        """Initialize brain and agent for the simulation."""
        print("Initializing brain and agent...")
        
        # Create configuration
        self.config = pynlm.createDefaultConfig()
        print(f"  - Configuration created")
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        print(f"  - Brain created")
        
        # Initialize brain
        self.brain.initialize()
        print(f"  - Brain initialized")
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        print(f"  - Agent brain interface created")
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        print(f"  - Agent initialized with world")
        
    def display_initial_state(self):
        """Display the initial state of the simulation."""
        print("Initial Simulation State:")
        print("-" * 40)
        
        # Get world state
        agent_body = self.world.getAgentBody()
        print(f"Agent Position: ({agent_body.x}, {agent_body.y})")
        print(f"Agent Orientation: {agent_body.orientation}")
        print(f"Agent Velocity: ({agent_body.velocityX}, {agent_body.velocityY})")
        print(f"Agent Energy: {agent_body.energy:.2f}")
        print(f"Agent Health: {agent_body.health:.2f}")
        
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        vision = percept.getVision()
        vision_height, vision_width, channels = len(vision), len(vision[0]) if vision else 0, 3
        print(f"Vision Dimensions: {vision_width}x{vision_height}x{channels}")
        
        # Get brain state
        print(f"Brain State:")
        print(f"  - Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  - Developmental Stage: {self.brain.getDevelopmentalStage()}")
        
    def run_simulation_steps(self, steps=100, timestep=0.1, verbose=True):
        """
        Run the simulation loop for a specified number of steps.
        
        Args:
            steps: Number of simulation steps to run
            timestep: Time duration per step in seconds
            verbose: Whether to display progress information
        """
        print(f"Running simulation for {steps} steps (timestep={timestep}s)...")
        
        for step in range(steps):
            # Update world physics
            self.world.update(timestep)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Process sensory input through agent and brain
            self.agent.processSensoryInput(percept)
            
            # Perform brain computation
            self.brain.step(step)
            
            # Get action and apply to world
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Display progress for first few steps and every 20 steps after
            if verbose and (step < 5 or step % 20 == 0 or step == steps - 1):
                agent_body = self.world.getAgentBody()
                print(f"  Step {step:4d}: Pos({agent_body.x:3.0f}, {agent_body.y:3.0f}) "
                      f"Vel({agent_body.velocityX:5.2f}, {agent_body.velocityY:5.2f}) "
                      f"Energy({agent_body.energy:5.2f})")
        
        print(f"Simulation completed: {steps} steps executed")
        
    def display_final_statistics(self):
        """Display final simulation statistics."""
        print("\nFinal Simulation Statistics:")
        print("-" * 40)
        
        # World statistics
        agent_body = self.world.getAgentBody()
        print(f"Agent Final State:")
        print(f"  - Position: ({agent_body.x:.2f}, {agent_body.y:.2f})")
        print(f"  - Orientation: {agent_body.orientation:.2f} radians")
        print(f"  - Final Velocity: ({agent_body.velocityX:.3f}, {agent_body.velocityY:.3f})")
        print(f"  - Final Energy: {agent_body.energy:.3f}")
        print(f"  - Final Health: {agent_body.health:.3f}")
        print(f"  - Age: {agent_body.age:.2f}")
        
        # Brain statistics
        print(f"\nBrain Statistics:")
        print(f"  - Total Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  - Total Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  - Active Neurons: {self.brain.getActiveNeuronCount()}")
        print(f"  - Firing Neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  - Average Firing Rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"  - E/I Ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        print(f"  - Total Spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  - Developmental Stage: {self.brain.getDevelopmentalStage()}")
        
        # Time information
        print(f"\nTime Information:")
        print(f"  - Simulation Time: {self.world.getSimulationTime():.2f} seconds")
        
    def save_simulation_results(self, brain_file="simulation_brain.nld", world_file=None):
        """
        Save simulation results to files.
        
        Args:
            brain_file: File path to save brain state
            world_file: File path to save world state (if needed)
        """
        print("\nSaving simulation results...")
        
        # Save brain state
        if brain_file:
            self.brain.save(brain_file)
            print(f"  - Brain saved to: {brain_file}")
        
        # Note: World state is not typically saved as it's derived from brain state
        # For complex worlds, you might want to implement world serialization
        
    def run_example(self, steps=100, timestep=0.1, save_results=False):
        """
        Run the complete simple simulation example.
        
        Args:
            steps: Number of simulation steps to run
            timestep: Time duration per step
            save_results: Whether to save simulation results
        """
        print("=" * 60)
        print(f"NLM Simple Simulation Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Setup simulation environment
            self.setup_simulation_environment()
            print()
            
            # Initialize brain and agent
            self.initialize_brain_and_agent()
            print()
            
            # Display initial state
            self.display_initial_state()
            print()
            
            # Run simulation
            self.run_simulation_steps(steps, timestep)
            print()
            
            # Display final statistics
            self.display_final_statistics()
            print()
            
            # Save results if requested
            if save_results:
                self.save_simulation_results()
                print()
            
            print("=" * 60)
            print("Simple Simulation Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain, self.world, self.agent
            
        except Exception as e:
            print(f"Error in simple simulation example: {e}")
            import traceback
            traceback.print_exc()
            return None, None, None

if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Simple Simulation Example")
    parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser.add_argument("--timestep", type=float, default=0.1, help="Time step duration in seconds")
    parser.add_argument("--save", action="store_true", help="Save simulation results")
    
    args = parser.parse_args()
    
    # Run the simple simulation example
    example = SimpleSimulationExample()
    result = example.run_example(args.steps, args.timestep, args.save)
    
    if result[0] is not None:
        print("\nExample completed successfully. Simulation is ready for analysis.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)