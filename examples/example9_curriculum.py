#!/usr/bin/env python3
"""
Example 9: Curriculum Learning

This example demonstrates curriculum learning where the agent learns from easy 
tasks to difficult tasks, improving learning efficiency and generalization.
"""

import pynlm
import numpy as np

def curriculum_learning_demo():
    """Demonstrate curriculum learning functionality."""
    
    print("=== Example 9: Curriculum Learning ===")
    
    # Create advanced configuration with curriculum support
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    print("Curriculum learning configuration:")
    print(f"  Brain neurons: {config.get('brain.neuron_count')}")
    print(f"  Learning rate: {config.get('plasticity.stdp.learning_rate')}")
    
    # Create trainer for curriculum learning
    class CurriculumTrainer:
        def __init__(self, brain, world, agent):
            self.brain = brain
            self.world = world
            self.agent = agent
            self.current_difficulty = 0
            self.performance_history = []
            self.adaptation_threshold = 0.8  # Performance threshold for difficulty increase
        
        def define_curriculum(self):
            """Define curriculum tasks with increasing difficulty."""
            curriculum = [
                {
                    'name': 'Simple Navigation',
                    'difficulty': 1,
                    'steps_per_task': 50,
                    'world_params': {
                        'width': 10, 'height': 10,
                        'visionWidth': 5, 'visionHeight': 5
                    },
                    'goal': 'move from start to center'
                },
                {
                    'name': 'Object Finding',
                    'difficulty': 2,
                    'steps_per_task': 75,
                    'world_params': {
                        'width': 15, 'height': 15,
                        'visionWidth': 6, 'visionHeight': 6
                    },
                    'goal': 'find and collect items'
                },
                {
                    'name': 'Complex Navigation',
                    'difficulty': 3,
                    'steps_per_task': 100,
                    'world_params': {
                        'width': 20, 'height': 20,
                        'visionWidth': 8, 'visionHeight': 8
                    },
                    'goal': 'navigate obstacles efficiently'
                },
                {
                    'name': 'Multi-Goal Tasks',
                    'difficulty': 4,
                    'steps_per_task': 80,
                    'world_params': {
                        'width': 25, 'height': 25,
                        'visionWidth': 10, 'visionHeight': 10
                    },
                    'goal': 'complete multiple objectives'
                },
                {
                    'name': 'Advanced Decision Making',
                    'difficulty': 5,
                    'steps_per_task': 60,
                    'world_params': {
                        'width': 30, 'height': 30,
                        'visionWidth': 12, 'visionHeight': 12
                    },
                    'goal': 'plan and execute complex strategies'
                }
            ]
            return curriculum
        
        def evaluate_performance(self, task_rewards, steps_completed):
            """Evaluate task performance and determine if ready for next difficulty."""
            if not task_rewards:
                return 0.0
            
            # Calculate performance metrics
            avg_reward = np.mean(task_rewards)
            reward_std = np.std(task_rewards)
            steps_efficiency = steps_completed / (len(task_rewards) * 10)  # normalize
            
            # Normalize metrics
            normalized_reward = min(avg_reward / 0.5, 1.0)  # target 0.5 reward
            normalized_efficiency = min(steps_efficiency, 1.0)
            
            # Overall performance score
            performance = (0.6 * normalized_reward + 0.4 * normalized_efficiency)
            
            return performance
        
        def adapt_difficulty(self, performance, curriculum, current_task_index):
            """Adapt difficulty based on performance."""
            if performance >= self.adaptation_threshold and current_task_index < len(curriculum) - 1:
                # Move to next difficulty level
                next_difficulty = curriculum[current_task_index + 1]['difficulty']
                print(f"  🎉 Performance sufficient ({performance:.2f}), moving to next difficulty: {next_difficulty}")
                return True
            elif performance < 0.5 and current_task_index > 0:
                # Stay or move to easier task
                prev_difficulty = curriculum[current_task_index - 1]['difficulty']
                print(f"  📈 Performance low ({performance:.2f}), maintaining difficulty: {prev_difficulty}")
                return False
            else:
                print(f"  ➡️  Performance moderate ({performance:.2f}), maintaining current difficulty")
                return False
        
        def run_curriculum_training(self, curriculum):
            """Run complete curriculum training."""
            
            print("\n--- Starting Curriculum Training ---")
            
            for task_idx, task in enumerate(curriculum):
                print(f"\n🎯 Task {task_idx + 1}/{len(curriculum)}: {task['name']}")
                print(f"   Difficulty: {task['difficulty']}")
                print(f"   Goal: {task['goal']}")
                print(f"   Steps: {task['steps_per_task']}")
                
                # Configure environment for this task
                self.world.configure(**task['world_params'])
                self.world.reset()
                self.agent.initialize(self.world)
                
                # Set learning rate based on task difficulty
                config.set("plasticity.stdp.learning_rate", 
                         0.0005 * task['difficulty'])
                
                # Run the task
                task_rewards = []
                
                for step in range(task['steps_per_task']):
                    # Update world
                    self.world.update(0.1)
                    
                    # Get percept and process
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    
                    # Run brain step
                    self.brain.step(step + task_idx * 100)  # unique step numbers
                    
                    # Get action and execute
                    action = self.agent.decodeMotorCommand()
                    result = self.world.applyMotorCommand(
                        action, self.world.getSimulationTime()
                    )
                    
                    # Get reward
                    reward = result.reward if hasattr(result, 'reward') else np.random.random() * 0.3
                    task_rewards.append(reward)
                    
                    # Apply reward modulation
                    self.agent.applyRewardModulation(reward, 0.0)
                    
                    # Print progress
                    if step % 20 == 0:
                        agent_pos = self.world.getAgentPosition()
                        curiosity = self.agent.getCuriosityLevel()
                        print(f"     Step {step:3d}: Pos({agent_pos.x:5.1f},{agent_pos.y:5.1f}), "
                              f"Curiosity:{curiosity:4.2f}, Reward:{reward:4.2f}")
                
                # Evaluate task performance
                performance = self.evaluate_performance(task_rewards, task['steps_per_task'])
                print(f"   📊 Task Performance: {performance:.3f}")
                
                # Store performance history
                self.performance_history.append({
                    'task': task['name'],
                    'difficulty': task['difficulty'],
                    'performance': performance,
                    'avg_reward': np.mean(task_rewards),
                    'reward_std': np.std(task_rewards)
                })
                
                # Adapt difficulty for next task
                if task_idx < len(curriculum) - 1:
                    self.adapt_difficulty(performance, curriculum, task_idx)
                
                print(f"   ✅ Task {task['name']} completed")
            
            print(f"\n🎉 Curriculum training complete!")
            print(f"   Total tasks: {len(self.performance_history)}")
            print(f"   Average performance: {np.mean([p['performance'] for p in self.performance_history]):.3f}")
            
            return self.performance_history
    
    # Create curriculum trainer
    trainer = CurriculumTrainer(brain, world, agent)
    
    # Define curriculum
    curriculum = trainer.define_curriculum()
    
    # Run curriculum training
    performance_history = trainer.run_curriculum_training(curriculum)
    
    # Analyze curriculum results
    print("\n--- Curriculum Analysis ---")
    
    print("Task Performance Summary:")
    for i, task_data in enumerate(performance_history):
        status = "✅ PASSED" if task_data['performance'] >= 0.8 else "⚠️  NEEDS IMPROVEMENT" if task_data['performance'] >= 0.6 else "❌ FAILED"
        print(f"  {i+1}. {task_data['task']} (Difficulty {task_data['difficulty']}): "
              f"{task_data['performance']:.3f} {status}")
    
    # Check for learning progression
    print("\n--- Learning Progression Analysis ---")
    
    performances = [task['performance'] for task in performance_history]
    difficulties = [task['difficulty'] for task in performance_history]
    
    # Calculate correlation between difficulty and performance
    if len(set(difficulties)) > 1:
        correlation = np.corrcoef(performances, difficulties)[0, 1]
        print(f"Performance vs Difficulty Correlation: {correlation:.3f}")
        
        if correlation > 0.5:
            print("   🎯 Good: Performance increases with difficulty (proper challenge)")
        elif correlation < -0.3:
            print("   ⚠️  Caution: Performance decreases with difficulty (too challenging)")
        else:
            print("   ➡️  Moderate: Performance relationship with difficulty")
    
    # Check for learning curve
    print("\nLearning Curve:")
    print("Task progression shows how the agent learns and adapts to increasing challenges")
    
    # Show final curriculum results
    final_performance = performances[-1]
    if final_performance > 0.8:
        print(f"\n🏆 Curriculum completed successfully!")
        print(f"   Final performance: {final_performance:.3f}")
        print(f"   The agent successfully learned to handle increasing complexity")
    elif final_performance > 0.6:
        print(f"\n📈 Curriculum mostly successful!")
        print(f"   Final performance: {final_performance:.3f}")
        print(f"   The agent showed good learning but needs improvement")
    else:
        print(f"\n⚠️  Curriculum needs adjustment")
        print(f"   Final performance: {final_performance:.3f}")
        print(f"   The agent struggled with curriculum difficulty")
    
    # Recommendation for curriculum optimization
    print("\n--- Curriculum Optimization Recommendations ---")
    
    if final_performance > 0.8:
        print("✅ Current curriculum is well-designed")
        print("   • Keep the current difficulty progression")
        print("   • Add more complex tasks for advanced training")
    elif final_performance > 0.6:
        print("📈 Curriculum needs minor adjustments")
        print("   • Reduce task difficulty if performance < 0.7")
        print("   • Increase task difficulty if performance > 0.7")
        print("   • Add intermediate tasks between difficulty levels")
    else:
        print("⚠️  Curriculum needs significant redesign")
        print("   • Start with easier tasks")
        print("   • Increase the adaptation threshold")
        print("   • Add scaffolding for difficult tasks")
    
    print("\nCurriculum learning demonstration complete!")
    return brain, world, agent, trainer

if __name__ == "__main__":
    curriculum_learning_demo()