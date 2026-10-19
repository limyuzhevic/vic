#!/usr/bin/env python3
"""
Prediction and Planning Example

This example demonstrates the advanced Prediction and Planning systems in NLM.
It shows how NeuralPlanner and PredictionSystem work together for intelligent behavior.

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

class PredictionPlanningExample:
    """Example class demonstrating Prediction and Planning systems."""
    
    def __init__(self):
        """Initialize the prediction and planning example."""
        self.brain = None
        self.world = None
        self.agent = None
        self.prediction_system = None
        self.planner = None
        self.example_name = "Prediction and Planning Systems"
        
    def setup_simulation_environment(self):
        """Set up the simulation environment."""
        print("Setting up simulation environment...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(25, 25, 16, 16)
        self.world.reset()
        
        # Create brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def create_prediction_and_planning_systems(self):
        """Create and initialize prediction and planning systems."""
        print("Creating Prediction and Planning systems...")
        
        # Create prediction system
        self.prediction_system = pynlm.createPredictionSystem()
        print(f"  - Prediction system created")
        
        # Create neural planner
        self.planner = pynlm.createNeuralPlanner()
        print(f"  - Neural planner created")
        
        # Initialize planner with brain
        self.planner.initialize(self.brain)
        print(f"  - Planner initialized with brain")
        
        # Configure planning parameters
        self.planner.setPlanningDepth(3)
        print(f"  - Planning depth set to: 3")
        
    def demonstrate_prediction_system(self, steps=40):
        """
        Demonstrate the Prediction System.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Prediction System ({steps} steps)...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Get current state for prediction
            current_state = percept.getVision()
            
            # Make prediction for next state
            prediction = self.prediction_system.predict(current_state)
            
            # Get prediction error
            prediction_error = self.prediction_system.getPredictionError()
            
            # Get prediction confidence
            confidence = self.prediction_system.getPredictionConfidence()
            
            print(f"  Step {step:3d}: Prediction made, Error={prediction_error:.3f}, Confidence={confidence:.3f}")
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update predictions with actual observation
            self.prediction_system.update(prediction, percept)
            
        print(f"  - Prediction system demonstration completed: {steps} steps")
        
    def demonstrate_planning_system(self, steps=40):
        """
        Demonstrate the Neural Planner system.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Neural Planner ({steps} steps)...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Get current state for planning
            current_state = percept.getVision()
            
            # Set planning goal
            if step % 20 == 0:
                goal = [0.8, 0.5, 0.3]  # Example goal vector
                self.planner.setCurrentGoal(goal)
                print(f"  Step {step:3d}: New goal set: {goal}")
            
            # Plan action
            action = self.planner.plan(current_state)
            print(f"  Step {step:3d}: Planned action: {action}")
            
            # Get decoded action
            decoded_action = self.agent.decodeMotorCommand()
            print(f"           Decoded action: {decoded_action}")
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check planning metrics
            if step % 10 == 0:
                confidence = self.planner.getActionQuality()
                depth = self.planner.getPlanningDepth()
                print(f"           Planning confidence: {confidence:.3f}, depth: {depth}")
                
        print(f"  - Neural planner demonstration completed: {steps} steps")
        
    def demonstrate_prediction_planning_integration(self, steps=50):
        """
        Demonstrate integration of prediction and planning systems.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Prediction-Planning Integration ({steps} steps)...")
        
        # Track planning outcomes
        planning_results = []
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            current_state = percept.getVision()
            
            # 1. Prediction Phase
            prediction = self.prediction_system.predict(current_state)
            pred_error = self.prediction_system.getPredictionError()
            
            # 2. Planning Phase
            action = self.planner.plan(current_state)
            
            # 3. Execution Phase
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            decoded_action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(decoded_action, self.world.getSimulationTime())
            
            # 4. Evaluation Phase
            # In a real system, we would evaluate the action outcome
            # For demonstration, use random reward based on step
            reward = min(1.0, step / 20.0)
            
            # Update planning quality based on outcome
            # In real system, this would be more sophisticated
            if step % 15 == 0:
                # Reset planning to show adaptation
                self.planner.setPlanningDepth(2)
                print(f"  Step {step:3d}: Planning depth reset to 2")
            elif step % 20 == 5:
                # Increase planning depth
                self.planner.setPlanningDepth(4)
                print(f"  Step {step:3d}: Planning depth increased to 4")
            
            # Display integration metrics
            if step % 5 == 0:
                confidence = self.planner.getActionQuality()
                error = self.prediction_system.getPredictionError()
                planning_results.append((pred_error, confidence))
                
                print(f"  Step {step:3d}: PredError={pred_error:.3f}, "
                      f"Conf={confidence:.3f}, Depth={self.planner.getPlanningDepth()}")
                
        # Display integration statistics
        if planning_results:
            pred_errors, confidences = zip(*planning_results)
            print(f"\n  - Integration statistics:")
            print(f"    - Average prediction error: {np.mean(pred_errors):.3f}")
            print(f"    - Average planning confidence: {np.mean(confidences):.3f}")
            print(f"    - Max prediction error: {np.max(pred_errors):.3f}")
            print(f"    - Min planning confidence: {np.min(confidences):.3f}")
        
        print(f"  - Prediction-planning integration completed: {steps} steps")
        
    def demonstrate_adaptive_planning(self, steps=45):
        """
        Demonstrate adaptive planning with changing goals and strategies.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Adaptive Planning ({steps} steps)...")
        
        # Set initial goals
        self.planner.setCurrentGoal([1.0, 0.0, 0.0])
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            current_state = percept.getVision()
            
            # Adaptive goal setting based on progress
            if step % 15 == 0:
                # Change goals based on situation
                if step % 30 == 0:
                    # Exploration goal
                    self.planner.setCurrentGoal([0.0, 1.0, 0.0])
                    print(f"  Step {step:3d}: Goal changed to exploration")
                else:
                    # Resource-seeking goal
                    self.planner.setCurrentGoal([0.5, 0.5, 1.0])
                    print(f"  Step {step:3d}: Goal changed to resource seeking")
            
            # Plan action with current goal
            action = self.planner.plan(current_state)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            decoded_action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(decoded_action, self.world.getSimulationTime())
            
            # Check goal achievement
            if step % 10 == 0:
                current_goal = self.planner.getCurrentGoal()
                print(f"  Step {step:3d}: Current goal: {current_goal}")
                print(f"           Planning depth: {self.planner.getPlanningDepth()}")
                print(f"           Action quality: {self.planner.getActionQuality():.3f}")
                
        print(f"  - Adaptive planning demonstration completed: {steps} steps")
        
    def demonstrate_prediction_planning_feedback(self, steps=40):
        """
        Demonstrate prediction-planning feedback loop.
        
        Args:
            steps: Number of steps to demonstrate
        """
        print(f"\nDemonstrating Prediction-Planning Feedback Loop ({steps} steps)...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            current_state = percept.getVision()
            
            # 1. Predict current state
            prediction = self.prediction_system.predict(current_state)
            pred_error = self.prediction_system.getPredictionError()
            
            # 2. Plan based on predicted state
            action = self.planner.plan(current_state)
            
            # 3. Execute planned action
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            decoded_action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(decoded_action, self.world.getSimulationTime())
            
            # 4. Feedback loop: update predictions and plans based on actual outcomes
            # In real system, this would use prediction error to adjust planning
            if step % 8 == 0:
                # Clear planning cache to force re-planning
                self.planner.clearCache()
                print(f"  Step {step:3d}: Planning cache cleared")
                
                # Increase planning depth for better accuracy
                new_depth = min(5, self.planner.getPlanningDepth() + 1)
                self.planner.setPlanningDepth(new_depth)
                print(f"           Planning depth increased to: {new_depth}")
            
            # 5. Update planning quality based on performance
            self.planner.updatePlanQuality([action], [decoded_action], 1.0 - pred_error)
            
            # Display feedback metrics
            if step % 6 == 0:
                confidence = self.planner.getActionQuality()
                pred_error = self.prediction_system.getPredictionError()
                print(f"  Step {step:3d}: Feedback - PredError={pred_error:.3f}, "
                      f"Conf={confidence:.3f}, Depth={self.planner.getPlanningDepth()}")
                
        print(f"  - Prediction-planning feedback completed: {steps} steps")
        
    def display_prediction_planning_statistics(self):
        """Display prediction and planning system statistics."""
        print("\nPrediction and Planning System Statistics:")
        print("-" * 40)
        
        print(f"1. Prediction System:")
        if self.prediction_system:
            print(f"   - Current prediction error: {self.prediction_system.getPredictionError():.3f}")
            print(f"   - Current prediction confidence: {self.prediction_system.getPredictionConfidence():.3f}")
            print(f"   - Prediction system enabled: {self.prediction_system.getEnabled():.1f}")
            
            # Get error history
            error_history = self.prediction_system.getErrorHistory()
            if error_history:
                print(f"   - Error history samples: {len(error_history)}")
                print(f"   - Mean absolute error: {np.mean(np.abs(error_history)):.3f}")
        
        print(f"\n2. Neural Planner:")
        if self.planner:
            print(f"   - Current planning depth: {self.planner.getPlanningDepth()}")
            print(f"   - Planning confidence: {self.planner.getActionQuality():.3f}")
            print(f"   - Current goal: {self.planner.getCurrentGoal()}")
            
        print(f"\n3. Integration Metrics:")
        print(f"   - Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"   - Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"   - Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"   - Developmental stage: {self.brain.getDevelopmentalStage()}")
        
    def run_example(self, steps=50):
        """
        Run the complete prediction and planning example.
        
        Args:
            steps: Number of steps for integration demo
        """
        print("=" * 60)
        print(f"NLM Prediction and Planning Systems Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Step 1: Setup simulation environment
            self.setup_simulation_environment()
            print()
            
            # Step 2: Create prediction and planning systems
            self.create_prediction_and_planning_systems()
            print()
            
            # Step 3: Demonstrate prediction system
            self.demonstrate_prediction_system(steps // 4)
            print()
            
            # Step 4: Demonstrate planning system
            self.demonstrate_planning_system(steps // 4)
            print()
            
            # Step 5: Demonstrate integration
            self.demonstrate_prediction_planning_integration(steps // 4)
            print()
            
            # Step 6: Demonstrate adaptive planning
            self.demonstrate_adaptive_planning(steps // 4)
            print()
            
            # Step 7: Demonstrate feedback loop
            self.demonstrate_prediction_planning_feedback(steps - (steps // 4) * 3)
            print()
            
            # Step 8: Display statistics
            self.display_prediction_planning_statistics()
            print()
            
            print("=" * 60)
            print("Prediction and Planning Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain, self.world, self.agent, self.prediction_system, self.planner
            
        except Exception as e:
            print(f"Error in prediction and planning example: {e}")
            import traceback
            traceback.print_exc()
            return None, None, None, None, None

if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Prediction and Planning Example")
    parser.add_argument("--steps", type=int, default=50, help="Number of steps for integration demo")
    
    args = parser.parse_args()
    
    # Run the prediction and planning example
    example = PredictionPlanningExample()
    result = example.run_example(args.steps)
    
    if result[0] is not None:
        print("\nExample completed successfully. Prediction and planning systems are working correctly.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)