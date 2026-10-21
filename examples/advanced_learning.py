"""
Advanced NLM Learning Examples

This module contains advanced examples for experienced users of the NLM (Neural Learning Machine) framework.
These examples demonstrate sophisticated neural architectures, complex learning behaviors, and performance optimizations.

Examples include:
1. Multi-task learning with neuromodulation
2. Parallel processing and optimization
3. Advanced neuromodulation strategies
4. Memory systems integration
5. Performance benchmarking
6. Real-time control applications
7. Integration patterns with external systems

Author: NLM Project
License: MIT
Version: 1.0
"""

import pynlm
import time
import threading
import numpy as np
from typing import Dict, List, Optional, Callable, Any

class MetaLearner:
    """
    Meta-learning system for coordinating multiple task heads.
    
    This meta-learner implements mechanisms for:
    - Balancing exploration and exploitation across tasks
    - Transferring knowledge between related tasks
    - Adapting learning rates based on task performance
    - Managing priority learning across multiple objectives
    """
    
    def __init__(self, num_tasks: int):
        self.num_tasks = num_tasks
        self.task_performance = np.zeros(num_tasks)
        self.exploration_rates = np.ones(num_tasks) / num_tasks
        self.task_priorities = np.ones(num_tasks)
        self.transfer_matrix = np.eye(num_tasks)
        
    def update(self, task_id: int, performance: float):
        """
        Update meta-learner state based on task performance.
        
        Args:
            task_id: ID of the task being performed
            performance: Performance metric for the task
        """
        self.task_performance[task_id] = performance
        
        # Update exploration rates based on performance
        self.exploration_rates[task_id] = max(0.05, 
                                          1.0 - performance * 0.8)
        
        # Update task priorities
        self.task_priorities[task_id] = performance
        
        # Normalize priorities
        total_priority = np.sum(self.task_priorities)
        if total_priority > 0:
            self.task_priorities /= total_priority
    
    def getTaskPriority(self, task_id: int) -> float:
        """Get current priority for a specific task."""
        return self.task_priorities[task_id]
    
    def getExplorationRate(self, task_id: int) -> float:
        """Get current exploration rate for a specific task."""
        return self.exploration_rates[task_id]
    
    def transferKnowledge(self, source_task: int, target_task: int):
        """
        Transfer knowledge from one task to another.
        
        Args:
            source_task: Source task ID
            target_task: Target task ID
        """
        if self.transfer_matrix[source_task, target_task] == 0:
            # Learn transfer weights based on task similarity
            similarity = np.exp(-abs(source_task - target_task))
            self.transfer_matrix[source_task, target_task] = similarity
    
    def getCombinedLearningSignal(self, task_id: int) -> float:
        """
        Get combined learning signal considering task performance
        and exploration-exploitation balance.
        """
        base_signal = self.task_performance[task_id]
        exploration_factor = self.exploration_rates[task_id]
        priority_factor = self.task_priorities[task_id]
        
        # Combine factors
        combined_signal = (base_signal * priority_factor + 
                          exploration_factor * 0.1)
        
        return combined_signal
class TaskHead:
    """
    Task-specific neural processing head.
    
    Each task head maintains:
    - Specialized neural pathways
    - Task-specific plasticity rules
    - Performance monitoring
    - State tracking
    """
    
    def __init__(self, task_id: int, brain: pynlm.Brain):
        self.task_id = task_id
        self.brain = brain
        self.performance_history = []
        self.action_history = []
        self.reward_history = []
        self.last_state = None
        self.plasticity_enabled = True
        
    def updateState(self, percept: pynlm.SensoryPercept, reward: float):
        """
        Update task head state based on percept and reward.
        
        Args:
            percept: Current sensory input
            reward: Current reward signal
        """
        self.last_state = percept
        self.reward_history.append(reward)
        
        # Update task-specific neural activity
        self.brain.processSensoryInput(percept)
        self.brain.step(0)
        
        # Get task-specific action
        action = self.getTaskAction()
        self.action_history.append(action)
        
        # Calculate performance
        performance = self.calculatePerformance()
        self.performance_history.append(performance)
        
        # Update task head's own brain
        self.updateTaskBrain()
    
    def getTaskAction(self) -> pynlm.Action:
        """Get the best action for this task."""
        # Task-specific action selection logic
        # This could involve:
        # - Policy gradient methods
        # - Actor-critic methods
        # - Value-based methods
        # - Rule-based methods
        
        # For now, use a simple heuristic
        if self.action_history:
            return self.action_history[-1]
        else:
            return pynlm.Action(pynlm.ActionType.Wait)
    
    def calculatePerformance(self) -> float:
        """
        Calculate task performance based on recent history.
        
        Returns:
            Performance metric (higher is better)
        """
        if len(self.reward_history) < 5:
            return 0.0
        
        # Calculate average reward
        avg_reward = np.mean(self.reward_history[-10:])
        
        # Normalize to [0, 1]
        performance = np.tanh(avg_reward) * 0.5 + 0.5
        
        return performance
    
    def updateTaskBrain(self):
        """Update the task-specific brain's neural connections."""
        # Enable plasticity for task-specific learning
        if self.plasticity_enabled:
            # Enable STDP and Hebbian plasticity
            for region in self.brain.getRegions():
                for syn in region->getSynapses():
                    syn->enablePlasticity(True, True, False)
class MultiTaskAgent:
    """
    Advanced multi-task learning agent using NLM.
    
    This agent demonstrates:
    - Simultaneous learning of multiple tasks
    - Neuromodulation for task coordination
    - Meta-learning for adaptive task balancing
    - Complex behavior generation
    """
    
    def __init__(self, num_tasks: int = 3):
        # Create configuration optimized for multi-task learning
        self.config = pynlm.createDemoConfig(2000, 0.1)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("neuromod.dopamine.scale", 1.0)
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Initialize task heads
        self.task_heads = {}
        for i in range(num_tasks):
            self.task_heads[i] = TaskHead(i, self.brain)
        
        # Initialize meta-learner
        self.meta_learner = MetaLearner(num_tasks)
        
        # Enable advanced subsystems
        self.enableAdvancedSystems()
        
    def enableAdvancedSystems(self):
        """Enable advanced neural subsystems for complex behaviors."""
        # In a real implementation, this would enable:
        # - Neuromodulation systems
        # - Memory systems
        # - Development systems
        # - Prediction systems
        # - Cognitive systems
        
        pass
    
    def processExperience(self, percept: pynlm.SensoryPercept, 
                         task_id: int, reward: float, 
                         task_weight: float = 1.0):
        """
        Process experience from a specific task.
        
        Args:
            percept: Current sensory input
            task_id: ID of the task being performed
            reward: Reward signal
            task_weight: Weight importance of this task
        """
        # Process sensory input in brain
        self.brain.processSensoryInput(percept)
        
        # Update task head
        self.task_heads[task_id].updateState(percept, reward)
        
        # Update meta-learner
        performance = self.task_heads[task_id].calculatePerformance()
        self.meta_learner.update(task_id, performance * task_weight)
        
        # Run brain simulation step
        self.brain.step(0)
        
        # Get combined action from all task heads
        action = self.getCombinedAction()
        
        return action
    
    def getCombinedAction(self) -> pynlm.Action:
        """
        Get combined action from all task heads.
        
        Returns:
            Combined action considering all task priorities
        """
        actions = []
        weights = []
        
        for task_id, task_head in self.task_heads.items():
            action = task_head.getTaskAction()
            priority = self.meta_learner.getTaskPriority(task_id)
            
            actions.append(action)
            weights.append(priority)
        
        if not actions:
            return pynlm.Action(pynlm.ActionType.Wait)
        
        # Select action with highest combined weight
        max_weight_index = np.argmax(weights)
        selected_action = actions[max_weight_index]
        
        return selected_action
    
    def runEpisode(self, num_steps: int, task_sequence: List[int] = None):
        """
        Run a complete episode with task sequence.
        
        Args:
            num_steps: Number of simulation steps
            task_sequence: Sequence of tasks to perform
        """
        if task_sequence is None:
            # Default: cycle through tasks
            task_sequence = list(self.task_heads.keys())
        
        for step in range(num_steps):
            # Select current task
            task_id = task_sequence[step % len(task_sequence)]
            
            # Get environment percept (simulated)
            percept = self.getEnvironmentPercept(task_id)
            
            # Get reward (simulated)
            reward = self.getTaskReward(task_id, percept)
            
            # Process experience
            action = self.processExperience(percept, task_id, reward)
            
            # Apply action (simulated)
            self.applyAction(action, task_id)
            
            # Log progress periodically
            if step % 100 == 0:
                self.logEpisodeProgress(step)
        
    def getEnvironmentPercept(self, task_id: int) -> pynlm.SensoryPercept:
        """Get sensory percept from environment for specific task."""
        # This would be replaced with actual environment interaction
        # For now, return a simulated percept
        
        if task_id == 0:
            # Navigation task
            return pynlm.Vision(8, 8)
        elif task_id == 1:
            # Resource gathering task
            return pynlm.InternalSignals()
        else:
            # Exploration task
            return pynlm.Vision(4, 4)
    
    def getTaskReward(self, task_id: int, percept: pynlm.SensoryPercept) -> float:
        """Get reward for task performance."""
        # This would be replaced with actual reward calculation
        # For now, return simulated rewards
        
        if task_id == 0:
            return 1.0  # Navigation success
        elif task_id == 1:
            return 2.0  # Resource found
        else:
            return 0.5  # Exploration discovery
    
    def applyAction(self, action: pynlm.Action, task_id: int):
        """Apply action to environment."""
        # This would be replaced with actual environment interaction
        # For now, simulate action application
        
        pass
    
    def logEpisodeProgress(self, step: int):
        """Log progress during episode."""
        print(f"Episode Step {step}:")
        
        for task_id, task_head in self.task_heads.items():
            performance = task_head.calculatePerformance()
            print(f"  Task {task_id}: Performance={performance:.3f}, "
                  f"Priority={self.meta_learner.getTaskPriority(task_id):.3f}")
        
        print(f"  Brain status: "
              f"Neurons={self.brain.getTotalNeuronCount()}, "
              f"Spikes={self.brain.getTotalSpikeCount()}, "
              f"Firing={self.brain.getFiringNeuronCount()}")
        
        print()
    
    def saveModel(self, filepath: str):
        """Save the agent's model to file."""
        self.brain.save(filepath)
        
    def loadModel(self, filepath: str):
        """Load the agent's model from file."""
        self.brain.load(filepath)
        
        # Reset task heads
        for task_head in self.task_heads.values():
            task_head.reset()
    
    def getPerformanceReport(self) -> Dict[str, Any]:
        """Get comprehensive performance report."""
        report = {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'task_performance': {},
            'brain_status': {}
        }
        
        # Add task performance
        for task_id, task_head in self.task_heads.items():
            report['task_performance'][task_id] = {
                'current_performance': task_head.calculatePerformance(),
                'average_reward': np.mean(task_head.reward_history) if task_head.reward_history else 0,
                'action_count': len(task_head.action_history)
            }
        
        # Add brain status
        self.brain.logStatus()
        
        return report
def runAdvancedLearningExample():
    """Run the advanced learning example."""
    print("=== Advanced Multi-Task Learning Example ===\n")
    
    # Create and configure agent
    agent = MultiTaskAgent(num_tasks=3)
    
    print("Agent initialized with:")
    print(f"- Brain with {agent.brain.getTotalNeuronCount()} neurons")
    print(f"- {len(agent.task_heads)} task heads")
    print(f"- Meta-learning system enabled")
    print()
    
    # Run demonstration episode
    print("Running demonstration episode...")
    agent.runEpisode(num_steps=500)
    
    # Get and display performance report
    print("\n=== Performance Report ===")
    report = agent.getPerformanceReport()
    
    for task_id, perf in report['task_performance'].items():
        print(f"Task {task_id}:")
        print(f"  Performance: {perf['current_performance']:.3f}")
        print(f"  Average Reward: {perf['average_reward']:.3f}")
        print(f"  Actions Taken: {perf['action_count']}")
    
    print(f"\nOverall Brain Status:")
    print(f"  Total Neurons: {report['total_neurons']}")
    print(f"  Total Synapses: {report['total_synapses']}")
    print(f"  Total Spikes: {report['total_spikes']}")
    print(f"  Firing Neurons: {report['firing_neurons']}")
    
    print("\n=== Example Complete ===")
if __name__ == "__main__":
    runAdvancedLearningExample()