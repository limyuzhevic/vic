#!/usr/bin/env python3
"""
Example 10: Meta-Learning (Model-Agnostic Meta-Learning)

This example demonstrates meta-learning where the agent learns how to learn,
adapting its learning strategy based on task performance and experience.
"""

import pynlm
import numpy as np

def meta_learning_demo():
    """Demonstrate meta-learning capabilities."""
    
    print("=== Example 10: Meta-Learning (Model-Agnostic Meta-Learning) ===")
    
    # Create advanced configuration for meta-learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    print("Meta-learning configuration:")
    print(f"  Brain neurons: {config.get('brain.neuron_count')}")
    print(f"  Learning rate: {config.get('plasticity.stdp.learning_rate')}")
    
    # Create meta-learning agent
    class MetaLearningAgent:
        def __init__(self, brain, world, agent):
            self.brain = brain
            self.world = world
            self.agent = agent
            self.meta_learner = True
            self.task_history = []
            self.meta_parameters = {
                'base_learning_rate': 0.001,
                'curiosity_weight': 0.5,
                'novelty_threshold': 0.5,
                'adaptation_rate': 0.1,
                'task_diversity': 0.0
            }
            self.current_task_type = 'exploration'  # meta-learning will adapt this
        
        def detect_task_type(self, percept, step, history):
            """Detect what type of task the agent is currently in."""
            
            # Analyze recent experience for task type detection
            if len(history) < 5:
                return 'exploration'  # Initial phase
            
            # Analyze patterns to determine task type
            recent_rewards = [h['reward'] for h in history[-10:]]
            recent_curiosity = [h['curiosity'] for h in history[-10:]]
            recent_performance = [h['performance'] for h in history[-10:]]
            
            # Calculate metrics
            avg_reward = np.mean(recent_rewards)
            reward_variance = np.std(recent_rewards)
            avg_curiosity = np.mean(recent_curiosity)
            avg_performance = np.mean(recent_performance)
            
            # Determine task type based on metrics
            if avg_reward < 0.1 and avg_curiosity > 0.5:
                return 'exploration'  # Need to explore for rewards
            elif reward_variance > 0.3 and avg_performance > 0.7:
                return 'exploitation'  # Stable environment, exploit known strategies
            elif avg_reward > 0.3 and avg_curiosity < 0.3:
                return 'optimization'  # Good rewards, optimize current strategy
            elif avg_reward < 0.2 and avg_curiosity < 0.2:
                return 'deadlock'  # Stuck, need new approach
            else:
                return 'balance'  # Need to balance exploration and exploitation
        
        def adapt_meta_parameters(self, task_type, performance, experience):
            """Adapt meta-learning parameters based on task type and performance."""
            
            adaptation_rate = self.meta_parameters['adaptation_rate']
            
            if task_type == 'exploration':
                # Increase curiosity and novelty for exploration tasks
                self.meta_parameters['curiosity_weight'] = min(
                    1.0, self.meta_parameters['curiosity_weight'] + adaptation_rate
                )
                self.meta_parameters['novelty_threshold'] = max(
                    0.2, self.meta_parameters['novelty_threshold'] - adaptation_rate
                )
                self.meta_parameters['base_learning_rate'] *= 0.9
                print(f"     📈 Exploration adaptation: Increased curiosity, decreased novelty threshold")
                
            elif task_type == 'exploitation':
                # Reduce exploration for stable environments
                self.meta_parameters['curiosity_weight'] = max(
                    0.1, self.meta_parameters['curiosity_weight'] - adaptation_rate
                )
                self.meta_parameters['base_learning_rate'] *= 1.1
                print(f"     📉 Exploitation adaptation: Decreased curiosity, increased learning rate")
                
            elif task_type == 'optimization':
                # Fine-tune current strategy
                self.meta_parameters['base_learning_rate'] *= 1.05
                print(f"     🎯 Optimization adaptation: Fine-tuning learning rate")
                
            elif task_type == 'deadlock':
                # Dramatically change approach
                self.meta_parameters['curiosity_weight'] = 0.9
                self.meta_parameters['novelty_threshold'] = 0.3
                self.meta_parameters['base_learning_rate'] *= 0.8
                print(f"     🚨 Deadlock recovery: Major adaptation strategy change")
                
            elif task_type == 'balance':
                # Maintain balanced state
                self.meta_parameters['base_learning_rate'] *= 1.02
                print(f"     ⚖️  Balance maintenance: Maintaining balanced approach")
            
            # Update task diversity based on experience
            if experience:
                unique_tasks = len(set([e['task_type'] for e in experience]))
                self.meta_parameters['task_diversity'] = min(
                    1.0, unique_tasks / len(experience) * 10
                )
            
            return self.meta_parameters
        
        def run_meta_learning_episode(self, episode_num):
            """Run a single meta-learning episode."""
            
            # Determine current task type
            task_type = self.detect_task_type(
                None, episode_num, self.task_history
            )
            
            print(f"     📊 Episode {episode_num}: Task type = {task_type}")
            
            # Apply adapted meta-parameters
            adapted_params = self.adapt_meta_parameters(
                task_type, 0.0, self.task_history
            )
            
            # Set current meta-configuration
            config.set("plasticity.stdp.learning_rate", 
                      adapted_params['base_learning_rate'])
            config.set("neuromod.curiosity.enable", adapted_params['curiosity_weight'] > 0.3)
            
            # Configure world based on task type
            if task_type == 'exploration':
                # High curiosity environment
                world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
                print(f"     🌍 Exploration environment: Large world, high curiosity")
            elif task_type == 'exploitation':
                # Stable environment
                world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
                print(f"     🏠 Exploitation environment: Medium world, stable")
            elif task_type == 'optimization':
                # Resource-rich environment
                world.configure(width=18, height=18, visionWidth=9, visionHeight=9)
                print(f"     💎 Optimization environment: Resource-rich, optimized")
            else:
                # Variable environment
                world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
                print(f"     🎲 Variable environment: Mixed challenge")
            
            world.reset()
            self.agent.initialize(world)
            
            # Run episode
            episode_metrics = {
                'episode': episode_num,
                'task_type': task_type,
                'steps': 0,
                'total_reward': 0.0,
                'max_reward': 0.0,
                'curiosity_history': [],
                'reward_history': [],
                'performance': 0.0
            }
            
            for step in range(50):
                world.update(0.1)
                percept = world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                self.brain.step(step + episode_num * 100)
                
                action = self.agent.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = result.reward if hasattr(result, 'reward') else np.random.random() * 0.3
                self.agent.applyRewardModulation(reward, 0.0)
                
                episode_metrics['steps'] += 1
                episode_metrics['total_reward'] += reward
                episode_metrics['max_reward'] = max(episode_metrics['max_reward'], reward)
                episode_metrics['curiosity_history'].append(self.agent.getCuriosityLevel())
                episode_metrics['reward_history'].append(reward)
            
            # Calculate episode performance
            episode_metrics['performance'] = (
                min(episode_metrics['total_reward'] / 12.5, 1.0) * 0.7 +
                min(np.mean(episode_metrics['curiosity_history']), 1.0) * 0.3
            )
            
            return episode_metrics
    
    # Create meta-learning agent
    meta_agent = MetaLearningAgent(brain, world, agent)
    
    print("\n--- Meta-Learning Phase ---")
    print("The agent will learn how to learn by adapting to different task types...")
    
    # Run meta-learning episodes
    meta_data = []
    
    for episode in range(1, 21):
        episode_data = meta_agent.run_meta_learning_episode(episode)
        meta_data.append(episode_data)
        
        # Store in task history for meta-learning adaptation
        meta_agent.task_history.append({
            'episode': episode,
            'task_type': episode_data['task_type'],
            'performance': episode_data['performance'],
            'reward': episode_data['total_reward'] / episode_data['steps'],
            'curiosity': np.mean(episode_data['curiosity_history'])
        })
        
        # Print progress
        if episode % 5 == 0:
            print(f"     📈 Meta-learning progress: Episode {episode}/20")
    
    # Analyze meta-learning results
    print("\n--- Meta-Learning Analysis ---")
    
    # Task type distribution
    task_types = [d['task_type'] for d in meta_data]
    task_type_counts = {}
    for task_type in task_types:
        task_type_counts[task_type] = task_type_counts.get(task_type, 0) + 1
    
    print("Task type distribution:")
    for task_type, count in task_type_counts.items():
        percentage = (count / len(task_types)) * 100
        print(f"  {task_type}: {count} episodes ({percentage:.1f}%)")
    
    # Performance evolution
    performances = [d['performance'] for d in meta_data]
    
    print(f"\nPerformance Evolution:")
    print(f"  Initial performance: {performances[0]:.3f}")
    print(f"  Final performance: {performances[-1]:.3f}")
    print(f"  Mean performance: {np.mean(performances):.3f}")
    print(f"  Performance trend: {np.polyfit(range(len(performances)), performances, 1)[0]:.4f} per episode")
    
    # Adaptation analysis
    print(f"\nMeta-Parameter Adaptation:")
    print(f"  Final curiosity weight: {meta_agent.meta_parameters['curiosity_weight']:.3f}")
    print(f"  Final learning rate: {meta_agent.meta_parameters['base_learning_rate']:.6f}")
    print(f"  Final task diversity: {meta_agent.meta_parameters['task_diversity']:.3f}")
    
    # Meta-learning success criteria
    print(f"\n--- Meta-Learning Success Criteria ---")
    
    # 1. Performance improvement over time
    if performances[-1] > performances[0]:
        print("  ✅ Performance improvement: Agent learned and improved over time")
        performance_growth = performances[-1] - performances[0]
        print(f"     Performance growth: {performance_growth:.3f}")
    else:
        print("  ❌ Performance improvement: Agent did not show learning progress")
        performance_growth = performances[-1] - performances[0]
        print(f"     Performance change: {performance_growth:.3f}")
    
    # 2. Task type adaptation
    unique_task_types = len(set(task_types))
    if unique_task_types > 1:
        print(f"  ✅ Task type adaptation: Agent adapted to {unique_task_types} different task types")
    else:
        print(f"  ❌ Task type adaptation: Agent only handled one task type")
    
    # 3. Parameter adaptation
    if meta_agent.meta_parameters['curiosity_weight'] > 0.3:
        print(f"  ✅ Parameter adaptation: Agent maintained adaptive curiosity levels")
    else:
        print(f"  ❌ Parameter adaptation: Agent may have become rigid")
    
    # 4. Learning rate adaptation
    if meta_agent.meta_parameters['base_learning_rate'] < 0.0005 or > 0.005:
        print(f"  ✅ Learning rate adaptation: Agent adapted learning rates")
        print(f"     Final learning rate: {meta_agent.meta_parameters['base_learning_rate']:.6f}")
    else:
        print(f"  ❌ Learning rate adaptation: Limited parameter adaptation")
    
    # 5. Overall meta-learning assessment
    print(f"\n--- Overall Meta-Learning Assessment ---")
    
    # Calculate meta-learning score
    performance_score = min((performances[-1] - performances[0]) / 0.5, 1.0)  # Normalize to 0-1
    adaptation_score = min(unique_task_types / 3, 1.0)  # Normalized to 0-1
    parameter_score = min((meta_agent.meta_parameters['curiosity_weight'] > 0.3) +
                         (meta_agent.meta_parameters['base_learning_rate'] < 0.0005 or > 0.005), 1.0)
    
    meta_learning_score = (performance_score + adaptation_score + parameter_score) / 3
    
    print(f"Meta-learning score: {meta_learning_score:.3f}/1.0")
    
    if meta_learning_score > 0.7:
        print("  🏆 Excellent meta-learning! The agent successfully learned how to learn.")
        print("     • Performance improved over time")
        print("     • Adapted to different task types")
        print("     • Maintained flexible parameters")
    elif meta_learning_score > 0.4:
        print("  📈 Good meta-learning with room for improvement.")
        print("     • Some learning progress observed")
        print("     • Basic adaptation to task types")
        print("     • Limited parameter flexibility")
    else:
        print("  ⚠️  Limited meta-learning capabilities.")
        print("     • Minimal learning progress")
        print("     • Rigid response to different tasks")
        print("     • Fixed parameter adaptation")
    
    # Meta-learning recommendations
    print(f"\n--- Meta-Learning Recommendations ---")
    
    if meta_learning_score > 0.7:
        print("✅ Current meta-learning approach is effective")
        print("   • Continue with the current adaptation strategy")
        print("   • Consider adding more task diversity for further learning")
        print("   • Monitor for overfitting to specific task types")
    elif meta_learning_score > 0.4:
        print("📈 Meta-learning needs refinement")
        print("   • Increase task diversity for better adaptation")
        print("   • Adjust adaptation rates for more flexible learning")
        print("   • Consider adding novelty detection for exploration")
    else:
        print("⚠️  Meta-learning requires significant redesign")
        print("   • Implement more sophisticated task detection")
        print("   • Increase exploration mechanisms")
        print("   • Add more adaptive parameter tuning")
    
    print("\nMeta-learning demonstration complete!")
    return brain, world, agent, meta_agent

if __name__ == "__main__":
    meta_learning_demo()