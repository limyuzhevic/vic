#!/usr/bin/env python3
"""
Development Example

This example demonstrates the Development System in NLM.
It shows how developmental stages affect learning and behavior.

Author: NLM Examples Team
Date: 2026-10-19
Version: 1.0
"""

import sys
import os
import time
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import pynlm
from datetime import datetime

class DevelopmentExample:
    """Example class demonstrating the Development System."""
    
    def __init__(self):
        """Initialize the development example."""
        self.brain = None
        self.world = None
        self.agent = None
        self.development_system = None
        self.example_name = "Development System"
        
    def setup_simulation_environment(self):
        """Set up the simulation environment."""
        print("Setting up simulation environment...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(20, 20, 14, 14)
        self.world.reset()
        
        # Create brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def get_development_system(self):
        """Get the development system from the brain."""
        print("Accessing development system...")
        
        # Get development system
        self.development_system = self.brain.getDevelopmentSystem()
        print(f"  - Development system retrieved")
        
        # Display current stage
        current_stage = self.development_system.getCurrentStage()
        print(f"  - Current developmental stage: {current_stage}")
        print(f"  - Stage name: {self.development_system.getStageName()}")
        
    def demonstrate_developmental_stages(self):
        """Demonstrate developmental stages and their effects."""
        print("\nDemonstrating Developmental Stages...")
        
        # Define stage transitions
        stages = [
            (pynlm.DevelopmentalStage.Initial, "Initial"),
            (pynlm.DevelopmentalStage.CriticalPeriod, "Critical Period"),
            (pynlm.DevelopmentalStage.Maturation, "Maturation"),
            (pynlm.DevelopmentalStage.Adult, "Adult"),
            (pynlm.DevelopmentalStage.Aging, "Aging")
        ]
        
        print("1. Developmental Stage Sequence:")
        for stage, name in stages:
            print(f"   - {name} (Stage {stage})")
        
        print("\n2. Stage Effects on Development System:")
        
        # Demonstrate each stage
        for stage, name in stages:
            # Set the stage
            self.development_system.setCurrentStage(stage)
            print(f"\n   Setting stage to: {name}")
            
            # Display stage-specific properties
            if stage == pynlm.DevelopmentalStage.Initial:
                print(f"     * Characteristics: Initial brain development")
                print(f"     * Plasticity: High")
                print(f"     * Critical period: Not yet started")
                
            elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
                print(f"     * Characteristics: Rapid learning and plasticity")
                print(f"     * Plasticity: Very high")
                print(f"     * Critical period: Active")
                print(f"     * Learning rate: Enhanced")
                
            elif stage == pynlm.DevelopmentalStage.Maturation:
                print(f"     * Characteristics: Refinement and specialization")
                print(f"     * Plasticity: Moderate")
                print(f"     * Critical period: Ending")
                print(f"     * Learning rate: Normal")
                
            elif stage == pynlm.DevelopmentalStage.Adult:
                print(f"     * Characteristics: Stable adult functionality")
                print(f"     * Plasticity: Low to moderate")
                print(f"     * Critical period: Ended")
                print(f"     * Learning rate: Baseline")
                
            elif stage == pynlm.DevelopmentalStage.Aging:
                print(f"     * Characteristics: Age-related changes")
                print(f"     * Plasticity: Declining")
                print(f"     * Critical period: Ended")
                print(f"     * Learning rate: Reduced")
                
        # Return to initial stage for next demonstrations
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Initial)
        print(f"\n   Reset to initial stage for continued demonstrations")
        
    def demonstrate_critical_period(self, steps=30):
        """
        Demonstrate critical period learning.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Critical Period Learning ({steps} steps)...")
        
        # Enable development system
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.CriticalPeriod)
        
        # Critical period properties
        print(f"1. Critical Period Properties:")
        print(f"   - Enabled: {self.development_system.isCriticalPeriod()}")
        print(f"   - Plasticity modifier: {self.development_system.getPlasticityModifier():.3f}")
        print(f"   - Critical period progress: {self.development_system.getCriticalPeriodProgress():.3f}")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent (enhanced learning during critical period)
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update development system
            self.development_system.update(self.brain, step)
            
            # Display critical period progress
            if step % 10 == 0:
                progress = self.development_system.getCriticalPeriodProgress()
                plasticity = self.development_system.getPlasticityModifier()
                print(f"   Step {step:3d}: Progress={progress:.2f}, Plasticity={plasticity:.3f}")
                
                # Check if critical period is ending
                if progress > 0.9:
                    print(f"           Critical period ending soon!")
        
        # Complete critical period
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Maturation)
        print(f"\n   Critical period completed, transitioning to Maturation stage")
        
    def demonstrate_development_plasticity(self, steps=25):
        """
        Demonstrate developmental plasticity effects.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Developmental Plasticity ({steps} steps)...")
        
        # Set to maturation stage
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Maturation)
        
        # Plasticity effects
        print(f"1. Plasticity Effects:")
        print(f"   - Current stage: {self.development_system.getStageName()}")
        print(f"   - Plasticity modifier: {self.development_system.getPlasticityModifier():.3f}")
        
        # Simulate neural development
        print(f"2. Neural Development Simulation:")
        
        for step in range(steps):
            # Update development system
            self.development_system.update(self.brain, step)
            
            # Get current plasticity modifier
            plasticity = self.development_system.getPlasticityModifier()
            
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent (affected by plasticity)
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Display development metrics
            if step % 5 == 0:
                stage_name = self.development_system.getStageName()
                stage_age = self.development_system.getDevelopmentalAge()
                print(f"   Step {step:3d}: Stage={stage_name}, Age={stage_age:.1f}, "
                      f"Plasticity={plasticity:.3f}")
                
        # Demonstrate adult stage
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Adult)
        print(f"\n   Transitioned to Adult stage with plasticity: {self.development_system.getPlasticityModifier():.3f}")
        
    def demonstrate_developmental_learning(self, steps=35):
        """
        Demonstrate learning effects across developmental stages.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Developmental Learning ({steps} steps)...")
        
        # Track learning across stages
        learning_metrics = []
        
        # Set initial stage
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Initial)
        
        for step in range(steps):
            # Change stages at specific points
            if step == 10:
                self.development_system.setCurrentStage(pynlm.DevelopmentalStage.CriticalPeriod)
                print(f"   Step {step}: Entering Critical Period")
            elif step == 25:
                self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Maturation)
                print(f"   Step {step}: Entering Maturation")
            elif step == 35:
                self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Adult)
                print(f"   Step {step}: Entering Adult")
            
            # Update development system
            self.development_system.update(self.brain, step)
            
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Record learning metrics
            stage = self.development_system.getCurrentStage()
            stage_name = self.development_system.getStageName()
            plasticity = self.development_system.getPlasticityModifier()
            
            learning_metrics.append((step, stage_name, plasticity))
            
            # Display stage information periodically
            if step % 10 == 0:
                print(f"   Step {step:3d}: Stage={stage_name}, Plasticity={plasticity:.3f}")
        
        # Analyze learning metrics
        print(f"\n   Learning Analysis:")
        stages = list(set([m[1] for m in learning_metrics]))
        for stage_name in stages:
            stage_metrics = [m for m in learning_metrics if m[1] == stage_name]
            avg_plasticity = np.mean([m[2] for m in stage_metrics])
            print(f"     - {stage_name}: Average plasticity={avg_plasticity:.3f}")
        
    def demonstrate_developmental_aging(self, steps=20):
        """
        Demonstrate aging effects on development.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Developmental Aging ({steps} steps)...")
        
        # Set to aging stage
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Aging)
        
        print(f"1. Aging Stage Properties:")
        print(f"   - Current stage: {self.development_system.getStageName()}")
        print(f"   - Plasticity modifier: {self.development_system.getPlasticityModifier():.3f}")
        
        for step in range(steps):
            # Update development system
            self.development_system.update(self.brain, step)
            
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent (aging affects learning)
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Display aging metrics
            if step % 5 == 0:
                stage_name = self.development_system.getStageName()
                plasticity = self.development_system.getPlasticityModifier()
                age = self.development_system.getDevelopmentalAge()
                print(f"   Step {step:3d}: Age={age:.1f}, Stage={stage_name}, "
                      f"Plasticity={plasticity:.3f}")
                
                # Show aging effects
                if plasticity < 0.5:
                    print(f"              - Reduced plasticity affecting learning")
                if age > 100:
                    print(f"              - Advanced aging stage")
        
        print(f"\n   Aging simulation completed")
        
    def display_development_system_statistics(self):
        """Display development system statistics."""
        print("\nDevelopment System Statistics:")
        print("-" * 40)
        
        if self.development_system:
            print(f"1. Development System:")
            print(f"   - Current stage: {self.development_system.getCurrentStage()}")
            print(f"   - Stage name: {self.development_system.getStageName()}")
            print(f"   - Plasticity modifier: {self.development_system.getPlasticityModifier():.3f}")
            print(f"   - Critical period: {self.development_system.isCriticalPeriod()}")
            print(f"   - Critical period progress: {self.development_system.getCriticalPeriodProgress():.3f}")
            print(f"   - Developmental age: {self.development_system.getDevelopmentalAge():.1f}")
            
        print(f"\n2. Neural System:")
        if self.brain:
            print(f"   - Total neurons: {self.brain.getTotalNeuronCount()}")
            print(f"   - Total synapses: {self.brain.getTotalSynapseCount()}")
            print(f"   - Firing neurons: {self.brain.getFiringNeuronCount()}")
            print(f"   - Developmental stage: {self.brain.getDevelopmentalStage()}")
        
        print(f"\n3. Agent System:")
        if self.agent:
            print(f"   - Current developmental stage: {self.agent.getDevelopmentalStage()}")
            neuromod_level = self.agent.getNeuromodulationLevel()
            print(f"   - Neuromodulation level: {neuromod_level:.3f}")
            
    def run_example(self, steps=50):
        """
        Run the complete development example.
        
        Args:
            steps: Number of steps for learning demo
        """
        print("=" * 60)
        print(f"NLM Development System Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Step 1: Setup simulation environment
            self.setup_simulation_environment()
            print()
            
            # Step 2: Get development system
            self.get_development_system()
            print()
            
            # Step 3: Demonstrate developmental stages
            self.demonstrate_developmental_stages()
            print()
            
            # Step 4: Demonstrate critical period
            self.demonstrate_critical_period(steps // 4)
            print()
            
            # Step 5: Demonstrate development plasticity
            self.demonstrate_development_plasticity(steps // 4)
            print()
            
            # Step 6: Demonstrate developmental learning
            self.demonstrate_developmental_learning(steps // 4)
            print()
            
            # Step 7: Demonstrate aging
            self.demonstrate_developmental_aging(steps - (steps // 4) * 3)
            print()
            
            # Step 8: Display statistics
            self.display_development_system_statistics()
            print()
            
            print("=" * 60)
            print("Development System Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain, self.world, self.agent, self.development_system
            
        except Exception as e:
            print(f"Error in development example: {e}")
            import traceback
            traceback.print_exc()
            return None, None, None, None

if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Development System Example")
    parser.add_argument("--steps", type=int, default=50, help="Number of steps for learning demo")
    
    args = parser.parse_args()
    
    # Run the development example
    example = DevelopmentExample()
    result = example.run_example(args.steps)
    
    if result[0] is not None:
        print("\nExample completed successfully. Development system is working correctly.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)