#!/usr/bin/env python3
"""
Neuromodulation Example

This example demonstrates the advanced Neuromodulation systems in NLM.
It shows how Dopamine, Curiosity, Novelty, and PredictionError systems work together.

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

class NeuromodulationExample:
    """Example class demonstrating the Neuromodulation systems."""
    
    def __init__(self):
        """Initialize the neuromodulation example."""
        self.brain = None
        self.world = None
        self.agent = None
        self.dopamine = None
        self.curiosity = None
        self.novelty = None
        self.prediction_error = None
        self.example_name = "Neuromodulation Systems"
        
    def setup_simulation_environment(self):
        """Set up the simulation environment."""
        print("Setting up simulation environment...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(18, 18, 14, 14)
        self.world.reset()
        
        # Create brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def create_neuromodulation_systems(self):
        """Create and initialize all neuromodulation systems."""
        print("Creating Neuromodulation systems...")
        
        # Create individual neuromodulators
        self.dopamine = pynlm.createDopamine()
        self.curiosity = pynlm.createCuriosity()
        self.novelty = pynlm.createNovelty()
        self.prediction_error = pynlm.createPredictionError()
        
        print(f"  - Dopamine system created")
        print(f"  - Curiosity system created")
        print(f"  - Novelty system created")
        print(f"  - PredictionError system created")
        
        # Set initial levels
        self.dopamine.setLevel(0.1)
        self.curiosity.update(0.05, 0.0, 0.1)
        self.novelty.setLevel(0.0)
        self.prediction_error.computeError(0.5, 0.5)
        
    def demonstrate_dopamine_system(self, steps=40):
        """
        Demonstrate the Dopamine system for reward and reinforcement learning.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Dopamine System ({steps} steps)...")
        
        # Enable reward modulation
        self.agent.enable_reward_modulation(True)
        
        for step in range(steps):
            # Generate varying rewards (simulating interesting events)
            if step % 7 == 0:  # Every 7th step gives a reward
                reward = 0.8 + (step % 3) * 0.05  # 0.8, 0.85, 0.9
                print(f"  Step {step:3d}: Reward event! Value: {reward:.2f}")
                
                # Signal reward to dopamine system
                self.dopamine.signalReward(reward)
                
                # Apply reward modulation
                self.agent.applyRewardModulation(reward, reward * 0.8)
                
            # Check dopamine levels
            if step % 10 == 0:
                dop_level = self.dopamine.getLevel()
                print(f"  Step {step:3d}: Dopamine level: {dop_level:.3f}")
                
            # Update world
            self.world.update(0.1)
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
        print(f"  - Dopamine system demonstration completed: {steps} steps")
        
    def demonstrate_curiosity_system(self, steps=40):
        """
        Demonstrate the Curiosity system for exploration and learning.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Curiosity System ({steps} steps)...")
        
        # Enable curiosity system
        self.agent.enable_curiosity(True)
        
        # Adjust curiosity weights
        self.curiosity.setNoveltyWeight(0.7)
        self.curiosity.setPredictionErrorWeight(0.3)
        
        print(f"  - Novelty weight: {self.curiosity.getNoveltyWeight():.2f}")
        print(f"  - Prediction error weight: {self.curiosity.getPredictionErrorWeight():.2f}")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent (curiosity affects exploration)
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check curiosity level
            if step % 8 == 0:
                curiosity_level = self.curiosity.getCuriosityLevel()
                print(f"  Step {step:3d}: Curiosity level: {curiosity_level:.3f}")
                
                # Higher curiosity should lead to more exploration
                if curiosity_level > 0.6:
                    print(f"           High curiosity! -> More exploration expected")
        
        print(f"  - Curiosity system demonstration completed: {steps} steps")
        
    def demonstrate_novelty_system(self, steps=30):
        """
        Demonstrate the Novelty detection system.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Novelty System ({steps} steps)...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Get current vision pattern for novelty detection
            vision = percept.getVision()
            current_pattern = []
            if vision:
                # Convert vision to pattern vector
                current_pattern = np.array(vision).flatten().tolist()
            
            # Get previous pattern for comparison
            prev_pattern = getattr(self, '_last_pattern', current_pattern)
            
            # Detect novelty
            novelty_score = self.novelty.detectNovelty(current_pattern, prev_pattern)
            
            print(f"  Step {step:3d}: Novelty score: {novelty_score:.3f}")
            
            # Update novelty level
            self.novelty.setLevel(novelty_score)
            
            # Update for next comparison
            self._last_pattern = current_pattern
            
            # Process with agent
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Display novelty history periodically
            if step % 10 == 0:
                history = self.novelty.getHistory()
                print(f"           Novelty history ({len(history)} samples): {[f'{x:.2f}' for x in history[-5:]]}")
        
        print(f"  - Novelty system demonstration completed: {steps} steps")
        
    def demonstrate_prediction_error_system(self, steps=35):
        """
        Demonstrate the Prediction Error system.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Prediction Error System ({steps} steps)...")
        
        # Track predictions and actuals
        predictions = []
        actuals = []
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Generate prediction (in real system, this would use PredictionSystem)
            # For demonstration, use simple alternating predictions
            if step % 4 == 0:
                predicted = 0.9  # High confidence
            elif step % 4 == 1:
                predicted = 0.5  # Medium confidence
            else:
                predicted = 0.1  # Low confidence
            
            # Simulate actual observation (could differ from prediction)
            actual = predicted + np.random.uniform(-0.2, 0.2)
            actual = max(0.0, min(1.0, actual))  # Clamp to [0, 1]
            
            predictions.append(predicted)
            actuals.append(actual)
            
            # Compute prediction error
            error = self.prediction_error.computeError(predicted, actual)
            
            print(f"  Step {step:3d}: Predicted={predicted:.2f}, Actual={actual:.2f}, Error={error:.3f}")
            
            # Update prediction for next step
            self.prediction_error.updatePrediction(predicted)
            
            # Process with agent
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
        # Display prediction error statistics
        errors = self.prediction_error.getHistory()
        if errors:
            print(f"  - Prediction error statistics:")
            print(f"    - Mean absolute error: {np.mean(np.abs(errors)):.3f}")
            print(f"    - Max error: {np.max(np.abs(errors)):.3f}")
            print(f"    - Min error: {np.min(np.abs(errors)):.3f}")
        
        print(f"  - Prediction error system demonstration completed: {steps} steps")
        
    def demonstrate_neuromodulation_integration(self, steps=40):
        """
        Demonstrate integration of all neuromodulation systems.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Neuromodulation Integration ({steps} steps)...")
        
        # Enable all neuromodulation systems
        self.agent.enable_reward_modulation(True)
        self.agent.enable_curiosity(True)
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent (all neuromodulators active)
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check all neuromodulation levels periodically
            if step % 5 == 0:
                dop = self.dopamine.getLevel()
                cur = self.curiosity.getCuriosityLevel()
                nov = self.novelty.getLevel()
                pred_err = self.prediction_error.getPredictionError()
                
                print(f"  Step {step:3d}: Dopamine={dop:.2f}, "
                      f"Curiosity={cur:.2f}, Novelty={nov:.2f}, "
                      f"PredError={pred_err:.2f}")
                
                # Check if neuromodulators are interacting
                if dop > 0.5 and cur > 0.3:
                    print(f"           High reward + high curiosity!")
                if nov > 0.4 and pred_err > 0.3:
                    print(f"           High novelty + high prediction error!")
        
        print(f"  - Neuromodulation integration completed: {steps} steps")
        
    def display_neuromodulation_statistics(self):
        """Display neuromodulation system statistics."""
        print("\nNeuromodulation System Statistics:")
        print("-" * 40)
        
        if self.dopamine:
            print(f"1. Dopamine System:")
            print(f"   - Current level: {self.dopamine.getLevel():.3f}")
            print(f"   - Plasticity factor: {self.dopamine.getPlasticityFactor():.3f}")
            print(f"   - Reward prediction error: {self.dopamine.getLevel():.3f}")
        
        if self.curiosity:
            print(f"\n2. Curiosity System:")
            print(f"   - Current level: {self.curiosity.getCuriosityLevel():.3f}")
            print(f"   - Novelty weight: {self.curiosity.getNoveltyWeight():.3f}")
            print(f"   - Prediction error weight: {self.curiosity.getPredictionErrorWeight():.3f}")
        
        if self.novelty:
            print(f"\n3. Novelty System:")
            print(f"   - Current level: {self.novelty.getLevel():.3f}")
            history = self.novelty.getHistory()
            if history:
                print(f"   - History samples: {len(history)}")
                print(f"   - Average novelty: {np.mean(history):.3f}")
        
        if self.prediction_error:
            print(f"\n4. Prediction Error System:")
            print(f"   - Current error: {self.prediction_error.getPredictionError():.3f}")
            print(f"   - Error magnitude: {self.prediction_error.getMagnitude():.3f}")
            history = self.prediction_error.getHistory()
            if history:
                print(f"   - History samples: {len(history)}")
                print(f"   - Average error: {np.mean(history):.3f}")
        
        # Agent neuromodulation level
        if self.agent:
            neuromod_level = self.agent.getNeuromodulationLevel()
            print(f"\n5. Agent Neuromodulation:")
            print(f"   - Current level: {neuromod_level:.3f}")
        
    def run_example(self, steps=50):
        """
        Run the complete neuromodulation example.
        
        Args:
            steps: Number of steps for integration demo
        """
        print("=" * 60)
        print(f"NLM Neuromodulation Systems Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Step 1: Setup simulation environment
            self.setup_simulation_environment()
            print()
            
            # Step 2: Create neuromodulation systems
            self.create_neuromodulation_systems()
            print()
            
            # Step 3: Demonstrate dopamine system
            self.demonstrate_dopamine_system(steps // 3)
            print()
            
            # Step 4: Demonstrate curiosity system
            self.demonstrate_curiosity_system(steps // 3)
            print()
            
            # Step 5: Demonstrate novelty system
            self.demonstrate_novelty_system(steps // 5)
            print()
            
            # Step 6: Demonstrate prediction error system
            self.demonstrate_prediction_error_system(steps // 5)
            print()
            
            # Step 7: Demonstrate integration
            self.demonstrate_neuromodulation_integration(steps - (steps // 3) * 2)
            print()
            
            # Step 8: Display statistics
            self.display_neuromodulation_statistics()
            print()
            
            print("=" * 60)
            print("Neuromodulation Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain, self.world, self.agent, self.dopamine, self.curiosity, self.novelty, self.prediction_error
            
        except Exception as e:
            print(f"Error in neuromodulation example: {e}")
            import traceback
            traceback.print_exc()
            return None, None, None, None, None, None, None

if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Neuromodulation Example")
    parser.add_argument("--steps", type=int, default=50, help="Number of steps for integration demo")
    
    args = parser.parse_args()
    
    # Run the neuromodulation example
    example = NeuromodulationExample()
    result = example.run_example(args.steps)
    
    if result[0] is not None:
        print("\nExample completed successfully. Neuromodulation systems are working correctly.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)