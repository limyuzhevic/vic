#!/usr/bin/env python3
"""
NLM Phase 6 Advanced Agent Examples

This script demonstrates advanced agent applications using the enhanced NLM framework.
It includes examples for multi-agent systems, learning experiments, and real-world simulations.

Usage:
    python3 advanced_agent_examples.py [example_name]

Examples:
    python3 advanced_agent_examples.py multi_agent_collab
    python3 advanced_agent_examples.py learning_experiment
    python3 advanced_agent_examples.py spatial_navigation
    python3 advanced_agent_examples.py resource_collection
    python3 advanced_agent_examples.py predator_prey
"""

import sys
import time
import random
import pynlm
from typing import List, Dict, Tuple

class AdvancedAgentExamples:
    """Demonstrates advanced agent applications with NLM."""
    
    def __init__(self):
        self.setup_advanced_configuration()
    
    def setup_advanced_configuration(self):
        """Setup advanced configuration for demonstrations."""
        self.config = pynlm.createAdvancedConfig()
        
        # Configure for advanced scenarios
        self.config.set("neuron_count", 3000, pynlm.ConfigSource.Runtime)
        self.config.set("region_count", 3, pynlm.ConfigSource.Runtime)
        self.config.set("connection_probability", 0.25, pynlm.ConfigSource.Runtime)
        self.config.set("working_memory_capacity", 300, pynlm.ConfigSource.Runtime)
        self.config.set("planning_depth", 8, pynlm.ConfigSource.Runtime)
        
        # Enable all advanced systems
        self.config.set("enable_reward_modulation", True, pynlm.ConfigSource.Runtime)
        self.config.set("enable_curiosity", True, pynlm.ConfigSource.Runtime)
        self.config.set("enable_development", True, pynlm.ConfigSource.Runtime)
        self.config.set("enable_structural_plasticity", True, pynlm.ConfigSource.Runtime)
    
    def create_multi_agent_collaboration(self, num_agents: int = 5) -> Dict:
        """Demonstrate multi-agent collaboration scenario."""
        print("\n=== Multi-Agent Collaboration Example ===")
        print(f"Creating {num_agents} collaborating agents...")
        
        agents = []
        worlds = []
        
        for i in range(num_agents):
            # Create unique configuration for each agent
            agent_config = pynlm.createAdvancedConfig()
            agent_config.set("random_seed", 42 + i, pynlm.ConfigSource.Runtime)
            
            brain = pynlm.createBrain(agent_config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
            world.reset()
            world.setAgentStart(random.uniform(2, 28), random.uniform(2, 28))
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable collaboration features
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            agents.append(agent)
            worlds.append(world)
        
        # Collaborative task: explore and share information
        collaboration_results = self.run_collaboration_task(agents, worlds)
        
        return {
            "agents": agents,
            "worlds": worlds,
            "results": collaboration_results,
            "type": "multi_agent_collaboration"
        }
    
    def run_collaboration_task(self, agents: List, worlds: List, steps: int = 200) -> Dict:
        """Run a collaborative task where agents share information."""
        collaboration_stats = {
            "total_explored_area": 0,
            "shared_knowledge": [],
            "coordination_score": 0.0,
            "efficiency_score": 0.0,
            "successful_coordination": 0
        }
        
        for step in range(steps):
            step_coordination = 0
            
            for i, (agent, world) in enumerate(zip(agents, worlds)):
                # Update world
                world.update(0.1)
                
                # Get percept
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain computation
                brain = agent.getBrain()
                brain.step(step)
                
                # Decode action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Simulate information sharing (simplified)
                if step % 10 == 0:
                    shared_info = self.simulate_information_sharing(agent, world)
                    collaboration_stats["shared_knowledge"].extend(shared_info)
                
                # Update statistics
                if action.name != "Wait":
                    step_coordination += 1
            
            # Calculate coordination for this step
            max_possible_coordination = len(agents)
            step_coordination_score = step_coordination / max_possible_coordination
            collaboration_stats["coordination_score"] += step_coordination_score
            
            if step_coordination_score > 0.6:
                collaboration_stats["successful_coordination"] += 1
        
        # Calculate final statistics
        avg_coordination = collaboration_stats["coordination_score"] / steps
        collaboration_stats["coordination_score"] = avg_coordination
        
        # Calculate efficiency
        explored_cells = sum(world.getAgentBody().x for world in worlds) / len(worlds)
        collaboration_stats["efficiency_score"] = avg_coordination * explored_cells / 1000.0
        
        return collaboration_stats
    
    def simulate_information_sharing(self, agent, world) -> List:
        """Simulate information sharing between agents."""
        shared_info = []
        
        # Get agent's current state
        brain = agent.getBrain()
        percept = world.getSensoryPercept()
        
        # Extract knowledge (simplified)
        knowledge = {
            "agent_id": id(agent),
            "vision": percept.getVision(),
            "position": (world.getAgentBody().x, world.getAgentBody().y),
            "firing_rate": brain.getAverageFiringRate(),
            "coordination": agent.getCuriosityLevel()
        }
        
        shared_info.append(knowledge)
        
        # Check for potential conflicts
        for other_agent in self.get_other_agents(agent):
            # Simple conflict detection
            if random.random() < 0.1:
                conflict = {
                    "agent_id": id(agent),
                    "other_agent": id(other_agent),
                    "type": "resource_competition",
                    "severity": random.uniform(0.1, 0.8)
                }
                shared_info.append(conflict)
        
        return shared_info
    
    def get_other_agents(self, current_agent) -> List:
        """Get other agents in the system (placeholder)."""
        return []  # This would be implemented with a proper agent manager
    
    def create_learning_experiment(self, scenario: str = "memory_learning") -> Dict:
        """Demonstrate advanced learning experiments."""
        print("\n=== Advanced Learning Experiment ===")
        print(f"Running {scenario} experiment...")
        
        # Create advanced configuration for learning
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", 5000, pynlm.ConfigSource.Runtime)
        config.set("enable_structural_plasticity", True, pynlm.ConfigSource.Runtime)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(25, 25)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Run learning experiment
        learning_results = self.run_learning_experiment(agent, world, scenario)
        
        return {
            "brain": brain,
            "world": world,
            "agent": agent,
            "results": learning_results,
            "type": "learning_experiment"
        }
    
    def run_learning_experiment(self, agent, world, scenario: str, steps: int = 500) -> Dict:
        """Run a comprehensive learning experiment."""
        experiment_stats = {
            "steps_taken": 0,
            "total_reward": 0.0,
            "avg_firing_rate": 0.0,
            "memory_growth": 0,
            "plastic_growth": 0,
            "learning_curve": [],
            "convergence_step": 0
        }
        
        prev_reward = 0.0
        convergence_count = 0
        
        for step in range(steps):
            # Update world
            world.update(0.05)
            
            # Get percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain = agent.getBrain()
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action and get reward
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward (simplified)
            reward = self.calculate_reward(agent, world, action)
            experiment_stats["total_reward"] += reward
            
            # Apply reward modulation
            predicted_reward = prev_reward * 0.9 + random.uniform(0, 1) * 0.1
            agent.applyRewardModulation(reward, predicted_reward)
            
            prev_reward = reward
            
            # Track learning progress
            firing_rate = brain.getAverageFiringRate()
            experiment_stats["avg_firing_rate"] += firing_rate
            
            learning_point = {
                "step": step,
                "reward": reward,
                "firing_rate": firing_rate,
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel()
            }
            experiment_stats["learning_curve"].append(learning_point)
            
            # Check for convergence
            if step > 50:
                recent_rewards = [p["reward"] for p in experiment_stats["learning_curve"][-10:]]
                reward_variance = self.calculate_variance(recent_rewards)
                if reward_variance < 0.01:
                    convergence_count += 1
                else:
                    convergence_count = 0
                
                if convergence_count >= 5:
                    experiment_stats["convergence_step"] = step
                    break
            
            if step % 50 == 0:
                print(f"  Step {step}: Reward = {reward:.3f}, Firing Rate = {firing_rate:.3f}")
        
        # Calculate statistics
        experiment_stats["steps_taken"] = step + 1
        experiment_stats["avg_firing_rate"] /= experiment_stats["steps_taken"]
        
        # Calculate growth metrics
        brain = agent.getBrain()
        if brain.getWorkingMemory():
            experiment_stats["memory_growth"] = brain.getWorkingMemory().getActiveTraces()
        
        return experiment_stats
    
    def calculate_reward(self, agent, world, action) -> float:
        """Calculate reward based on agent performance."""
        reward = 0.0
        
        # Positive reward for actions
        if action.name not in ["Wait", "Rest"]:
            reward += 0.5
        
        # Reward for exploration
        if agent.getCuriosityLevel() > 0.5:
            reward += 0.3
        
        # Reward for learning
        if agent.getNoveltyLevel() > 0.1:
            reward += 0.2
        
        # Reward based on position (towards center)
        agent_x, agent_y = world.getAgentBody().x, world.getAgentBody().y
        distance_to_center = ((agent_x - 25) ** 2 + (agent_y - 25) ** 2) ** 0.5
        if distance_to_center < 10:
            reward += 1.0
        elif distance_to_center < 20:
            reward += 0.5
        
        # Negative reward for failures
        if action.name == "Wait" and agent.getCuriosityLevel() < 0.1:
            reward -= 0.1
        
        return reward
    
    def calculate_variance(self, values: List[float]) -> float:
        """Calculate variance of a list of values."""
        if len(values) < 2:
            return 0.0
        
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        double sq_sum = std::accumulate(values.begin(), values.end(), 0.0,
            [mean](double sum, double val) { return sum + (val - mean) * (val - mean); });
        return sq_sum / (values.size() - 1);
    
    def create_spatial_navigation(self) -> Dict:
        """Demonstrate advanced spatial navigation."""
        print("\n=== Advanced Spatial Navigation ===")
        print("Training agent for complex spatial navigation...")
        
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", 4000, pynlm.ConfigSource.Runtime)
        config.set("enable_structural_plasticity", True, pynlm.ConfigSource.Runtime)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=100, height=100, visionWidth=15, visionHeight=15)
        world.reset()
        world.setAgentStart(10, 10)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Add navigation goals
        self.add_navigation_goals(world)
        
        # Run navigation training
        navigation_results = self.run_navigation_training(agent, world)
        
        return {
            "brain": brain,
            "world": world,
            "agent": agent,
            "results": navigation_results,
            "type": "spatial_navigation"
        }
    
    def add_navigation_goals(self, world):
        """Add navigation goals to the world."""
        # Add resource points
        for i in range(20):
            x = random.uniform(80, 95)
            y = random.uniform(80, 95)
            value = random.uniform(5, 10)
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value, 0.5f))
        
        # Add hazard points
        for i in range(10):
            x = random.uniform(5, 20)
            y = random.uniform(5, 20)
            value = random.uniform(8, 15)
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value, 0.5f))
        
        # Add way stations
        for i in range(8):
            x = random.uniform(40, 60)
            y = random.uniform(40, 60)
            value = 3.0
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Marker, value, 0.3f))
    
    def run_navigation_training(self, agent, world, steps: int = 800) -> Dict:
        """Run navigation training with waypoints."""
        nav_stats = {
            "steps_taken": 0,
            "path_length": 0,
            "goal_reached": 0,
            "collision_count": 0,
            "efficiency": 0.0,
            "final_position": (0.0, 0.0)
        }
        
        start_pos = (world.getAgentBody().x, world.getAgentBody().y)
        visited_positions = set()
        
        for step in range(steps):
            world.update(0.05)
            
            # Get percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain = agent.getBrain()
            brain.step(step)
            
            # Decode navigation action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check for collisions
            if self.check_collision(world):
                nav_stats["collision_count"] += 1
            
            # Track visited positions
            pos = (world.getAgentBody().x, world.getAgentBody().y)
            visited_positions.add(pos)
            nav_stats["path_length"] += 1
            
            # Check if goal is reached
            if self.check_goal_reached(world):
                nav_stats["goal_reached"] += 1
                # Reset to start for continuous training
                world.setAgentStart(start_pos[0], start_pos[1])
            
            if step % 100 == 0:
                print(f"  Step {step}: Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        
        nav_stats["steps_taken"] = step + 1
        nav_stats["final_position"] = (world.getAgentBody().x, world.getAgentBody().y)
        
        # Calculate efficiency
        if nav_stats["goal_reached"] > 0:
            nav_stats["efficiency"] = visited_positions.size() / (nav_stats["path_length"] * 0.01)
        
        return nav_stats
    
    def check_collision(self, world) -> bool:
        """Check if agent has collided with obstacles."""
        agent_x, agent_y = world.getAgentBody().x, world.getAgentBody().y
        
        # Check world boundaries
        if agent_x < 0 or agent_x > world.getWidth() or agent_y < 0 or agent_y > world.getHeight():
            return True
        
        # Check for objects in the world
        # This would require iterating through world objects in a real implementation
        
        return False
    
    def check_goal_reached(self, world) -> bool:
        """Check if agent has reached a goal."""
        # Check if agent is near a goal (resource, marker, etc.)
        agent_x, agent_y = world.getAgentBody().x, world.getAgentBody().y
        
        # Simple distance check for goals
        distance_to_center = ((agent_x - world.getWidth()/2) ** 2 + (agent_y - world.getHeight()/2) ** 2) ** 0.5
        return distance_to_center < 5.0
    
    def run_all_examples(self) -> Dict:
        """Run all advanced examples and return results."""
        print("=== Advanced NLM Agent Examples ===")
        print("This script demonstrates advanced agent applications using NLM Phase 6.")
        print()
        
        examples = {
            "multi_agent_collaboration": self.create_multi_agent_collaboration,
            "learning_experiment": self.create_learning_experiment,
            "spatial_navigation": self.create_spatial_navigation,
        }
        
        results = {}
        
        for example_name, example_func in examples.items():
            try:
                print(f"\n{'='*60}")
                print(f"Running {example_name.replace('_', ' ').title()} Example")
                print(f"{'='*60}")
                
                result = example_func()
                results[example_name] = result
                
                print(f"\n✓ {example_name.replace('_', ' ').title()} completed successfully!")
                print(f"  Steps: {result['results'].get('steps_taken', 'N/A')}")
                print(f"  Efficiency: {result['results'].get('efficiency_score', result['results'].get('efficiency', 0.0)):.3f}")
                
            except Exception as e:
                print(f"\n✗ Error running {example_name}: {str(e)}")
                results[example_name] = {"error": str(e)}
        
        print(f"\n{'='*60}")
        print("All examples completed!")
        print(f"{'='*60}")
        
        return results

def main():
    """Main function to run advanced agent examples."""
    if len(sys.argv) > 1:
        # Run specific example
        example_name = sys.argv[1]
        
        demo = AdvancedAgentExamples()
        
        examples = {
            "multi_agent_collab": demo.create_multi_agent_collaboration,
            "learning_exp": demo.create_learning_experiment,
            "nav": demo.create_spatial_navigation,
        }
        
        if example_name in examples:
            try:
                result = examples[example_name]()
                print(f"\n✓ {example_name} completed successfully!")
                return 0
            except Exception as e:
                print(f"\n✗ Error: {str(e)}")
                return 1
        else:
            print(f"Unknown example: {example_name}")
            print("Available examples: multi_agent_collab, learning_exp, nav")
            return 1
    else:
        # Run all examples
        demo = AdvancedAgentExamples()
        results = demo.run_all_examples()
        
        # Print summary
        print("\n" + "="*60)
        print("EXECUTION SUMMARY")
        print("="*60)
        
        for example_name, result in results.items():
            if "error" in result:
                print(f"✗ {example_name.replace('_', ' ').title()}: FAILED - {result['error']}")
            else:
                print(f"✓ {example_name.replace('_', ' ').title()}: SUCCESS")
        
        return 0

if __name__ == "__main__":
    sys.exit(main())
