"""
Long-Running Experiment Examples for NLM

This module demonstrates extended learning and experimentation scenarios
including continuous learning, evolutionary processes, and long-term cognitive development.
"""

import pynlm
import numpy as np
import time
import threading
from typing import Dict, List, Optional, Tuple
import json
from datetime import datetime

class LongRunningExperimentExamples:
    """Long-running experiment examples."""
    
    @staticmethod
    def create_continuous_learning_experiment(weeks: int = 4) -> Dict:
        """
        Example: Continuous learning experiment over extended time periods.
        
        Simulates continuous learning over weeks/months to observe:
        - Long-term memory consolidation
        - Skill retention and decay
        - Adaptive behavior changes
        - Performance evolution over time
        """
        print(f"=== Continuous Learning Experiment ({weeks} weeks) ===")
        
        # Create brain configuration optimized for continuous learning
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.synapse_density", 0.09)
        
        # Configure for continuous learning
        config.set("neuromod.dopamine.scale", 0.7)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # Configure for sustained plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.stdp.tau_plus", 40.0)  # Slower, more stable learning
        config.set("plasticity.stdp.tau_minus", 40.0)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.008)
        config.set("plasticity.structural.enable", True)  # Structural changes for long-term memory
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableNovelty(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        print(f"Created continuous learning environment for {weeks} weeks")
        print(f"  Total simulation steps: {weeks * 7 * 24 * 12} (12 steps per day)")
        
        # Simulate continuous learning
        print("\nStarting continuous learning simulation...")
        
        # Time tracking
        start_time = time.time()
        experiment_start = datetime.now()
        
        # Performance tracking over time
        performance_metrics = {
            "steps": [],
            "reward": [],
            "curiosity": [],
            "novelty": [],
            "firing_rate": [],
            "memory_consolidation": []
        }
        
        # Learning phases
        learning_phases = {
            "week_1": {"goal": 0.6, "phase_length": 7},
            "week_2": {"goal": 0.7, "phase_length": 7},
            "week_3": {"goal": 0.8, "phase_length": 7},
            "week_4": {"goal": 0.85, "phase_length": 7}
        }
        
        current_phase = "week_1"
        phase_step = 0
        
        # Run continuous learning simulation
        total_steps = weeks * 7 * 24 * 12  # 12 steps per day
        for step in range(total_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            # Simulate learning reward (in real scenario this would come from environment)
            learning_reward = 0.2 + (step * 0.0001)  # Gradually improving reward
            agent.applyRewardModulation(learning_reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record metrics
            performance_metrics["steps"].append(step)
            performance_metrics["reward"].append(learning_reward)
            
            if hasattr(agent, 'getCuriosityLevel'):
                performance_metrics["curiosity"].append(agent.getCuriosityLevel())
            else:
                performance_metrics["curiosity"].append(0.5)
                
            if hasattr(agent, 'getNoveltyLevel'):
                performance_metrics["novelty"].append(agent.getNoveltyLevel())
            else:
                performance_metrics["novelty"].append(0.5)
                
            performance_metrics["firing_rate"].append(brain.getAverageFiringRate())
            
            if hasattr(brain, 'getWorkingMemory'):
                working_mem = brain.getWorkingMemory()
                if working_mem:
                    performance_metrics["memory_consolidation"].append(working_mem.getSize())
            
            # Phase tracking
            phase_step += 1
            current_phase_duration = learning_phases[current_phase]["phase_length"]
            
            if phase_step >= current_phase_duration:
                print(f"  Week {current_phase[-1]} completed")
                current_phase_key = list(learning_phases.keys())[list(learning_phases.keys()).index(current_phase) + 1]
                if current_phase_key in learning_phases:
                    current_phase = current_phase_key
            
            # Progress reporting
            if step % (weeks * 24 * 6) == 0:  # Report every 6 days
                elapsed = time.time() - start_time
                days_completed = step / (24 * 12)
                print(f"  Day {days_completed:.1f}: Reward = {learning_reward:.2f}, "
                      f"Firing rate = {brain.getAverageFiringRate():.2f} Hz, "
                      f"Elapsed time = {elapsed/3600:.1f} hours")
        
        total_time = time.time() - start_time
        end_time = datetime.now()
        
        print(f"\nContinuous learning completed!")
        print(f"  Total simulation time: {total_time/3600:.1f} hours")
        print(f"  Experiment duration: {end_time - experiment_start}")
        print(f"  Average daily reward: {np.mean(performance_metrics['reward']):.3f}")
        print(f"  Final firing rate: {performance_metrics['firing_rate'][-1]:.2f} Hz")
        
        return {
            "brain": brain,
            "agent": agent,
            "world": world,
            "performance_metrics": performance_metrics,
            "config": config,
            "experiment_duration": weeks,
            "total_steps": total_steps,
            "total_time": total_time
        }
    
    @staticmethod
    def create_evolutionary_experiment(generation_count: int = 50) -> Dict:
        """
        Example: Evolutionary experiment with population-level learning.
        
        Simulates evolutionary processes including:
        - Genetic selection
        - Cross-generation learning transfer
        - Population adaptation
        - Evolutionary optimization
        """
        print(f"\n=== Evolutionary Experiment ({generation_count} generations) ===")
        
        # Create base configuration
        base_config = pynlm.createDefaultConfig()
        base_config.set("brain.neuron_count", 2000)
        base_config.set("brain.synapse_density", 0.08)
        
        # Configure for evolutionary learning
        base_config.set("neuromod.dopamine.scale", 0.9)
        base_config.set("neuromod.curiosity.enable", True)
        base_config.set("neuromod.novelty.enable", True)
        
        base_config.set("plasticity.stdp.enable", True)
        base_config.set("plasticity.stdp.learning_rate", 0.02)  # Higher for evolution
        base_config.set("plasticity.hebbian.enable", True)
        base_config.set("plasticity.hebbian.learning_rate", 0.015)
        
        print(f"Creating evolutionary population of {generation_count} generations")
        
        # Create population of brains
        population = []
        generation_metrics = {}
        
        for generation in range(generation_count):
            print(f"\nGeneration {generation + 1}/{generation_count}")
            
            # Create new generation
            generation_brains = []
            generation_performance = []
            
            for individual in range(10):  # 10 individuals per generation
                # Create brain with slight variations (evolution)
                config = pynlm.createDefaultConfig()
                
                # Apply evolutionary variations
                neuron_count = base_config.get("brain.neuron_count")
                neuron_count_variation = int(neuron_count * np.random.uniform(0.9, 1.1))
                config.set("brain.neuron_count", neuron_count_variation)
                
                # Copy base configuration with small mutations
                config.set("brain.synapse_density", 
                          base_config.get("brain.synapse_density") * np.random.uniform(0.9, 1.1))
                config.set("neuromod.dopamine.scale", 
                          base_config.get("neuromod.dopamine.scale") * np.random.uniform(0.9, 1.1))
                
                # Create and initialize brain
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Create world and agent for this individual
                world = pynlm.createSimpleWorld()
                world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
                world.reset()
                world.setAgentStart(12.5, 12.5)
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                
                # Enable learning
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                
                # Simulate individual performance
                individual_performance = 0
                for step in range(100):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    
                    # Calculate performance based on activity
                    if step % 25 == 0:
                        individual_performance += brain.getFiringNeuronCount()
                
                generation_brains.append(brain)
                generation_performance.append(individual_performance)
            
            # Calculate generation statistics
            avg_performance = np.mean(generation_performance)
            best_performance = np.max(generation_performance)
            worst_performance = np.min(generation_performance)
            performance_std = np.std(generation_performance)
            
            generation_metrics[generation + 1] = {
                "average_performance": avg_performance,
                "best_performance": best_performance,
                "worst_performance": worst_performance,
                "performance_std": performance_std,
                "individual_performances": generation_performance
            }
            
            print(f"  Average performance: {avg_performance:.1f}")
            print(f"  Best performance: {best_performance:.1f}")
            print(f"  Performance standard deviation: {performance_std:.1f}")
            
            # Evolutionary pressure - keep only top performers for next generation
            if generation < generation_count - 1:
                # Select top 3 individuals for reproduction
                top_indices = np.argsort(generation_performance)[-3:]
                population.extend([generation_brains[i] for i in top_indices])
                print(f"  Selected top {len(top_indices)} individuals for next generation")
        
        # Analyze evolutionary results
        print("\n=== Evolutionary Analysis ===")
        
        generations = list(generation_metrics.keys())
        avg_performance = [generation_metrics[g]["average_performance"] for g in generations]
        best_performance = [generation_metrics[g]["best_performance"] for g in generations]
        
        # Calculate improvement rate
        if len(avg_performance) > 1:
            improvement_rate = (avg_performance[-1] - avg_performance[0]) / (generation_count - 1)
            print(f"Average improvement per generation: {improvement_rate:.2f}")
        
        # Identify convergence
        if len(avg_performance) >= 10:
            recent_avg = np.mean(avg_performance[-10:])
            early_avg = np.mean(avg_performance[:10])
            convergence = (recent_avg - early_avg) / early_avg
            print(f"Convergence rate: {convergence:.2%}")
        
        return {
            "population": population,
            "generation_metrics": generation_metrics,
            "config": base_config,
            "total_generations": generation_count,
            "total_individuals": sum(len(metrics["individual_performances"]) 
                                   for metrics in generation_metrics.values())
        }
    
    @staticmethod
    def create_skill_consolidation_experiment() -> Dict:
        """
        Example: Skill consolidation experiment.
        
        Demonstrates long-term skill retention and consolidation through:
        - Initial skill acquisition
        - Sleep/wake cycles (simulated)
        - Skill refinement and optimization
        - Long-term retention testing
        """
        print("\n=== Skill Consolidation Experiment ===")
        
        # Create brain for skill learning
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2500)
        config.set("brain.synapse_density", 0.11)
        
        # Configure for skill learning
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.025)  # Strong learning for skills
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.02)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=35, height=35, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(17.5, 17.5)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableNovelty(True)
        agent.enableStructuralPlasticity(True)
        
        print("Created skill consolidation environment")
        print("  Focus: Motor skill learning and refinement")
        print("  Protocol: Acquisition -> Consolidation -> Retention testing")
        
        # Skill consolidation phases
        skill_phases = {
            "acquisition": {"steps": 200, "target_performance": 0.7},
            "consolidation": {"steps": 100, "target_performance": 0.85},
            "optimization": {"steps": 150, "target_performance": 0.95}
        }
        
        # Simulate skill learning
        skill_results = {
            "phases": {},
            "performance_history": [],
            "skill_robustness": [],
            "retention_tests": []
        }
        
        current_phase = "acquisition"
        phase_step = 0
        
        for step in range(450):  # Total of 450 steps across all phases
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            # Simulate skill learning reward (higher for better performance)
            base_reward = 0.1
            performance_reward = brain.getAverageFiringRate() * 0.05
            skill_reward = 0.5 if step % 20 == 0 else 0  # Bonus for periodic attempts
            
            total_reward = base_reward + performance_reward + skill_reward
            agent.applyRewardModulation(total_reward, 0.0)
            
            # Record performance
            skill_results["performance_history"].append({
                "step": step,
                "phase": current_phase,
                "reward": total_reward,
                "firing_rate": brain.getAverageFiringRate(),
                "active_neurons": brain.getActiveNeuronCount(),
                "firing_neurons": brain.getFiringNeuronCount()
            })
            
            # Phase tracking
            phase_step += 1
            required_steps = skill_phases[current_phase]["steps"]
            
            if phase_step >= required_steps:
                print(f"  Phase '{current_phase}' completed")
                current_phase_key = list(skill_phases.keys())[list(skill_phases.keys()).index(current_phase) + 1]
                if current_phase_key in skill_phases:
                    current_phase = current_phase_key
                    phase_step = 0
                    
                    # Consolidation checkpoint
                    if current_phase == "consolidation":
                        print(f"  Performing consolidation checkpoint...")
                        # Simulate consolidation (stable memory formation)
                        if hasattr(brain, 'getWorkingMemory'):
                            working_mem = brain.getWorkingMemory()
                            if working_mem:
                                print(f"  Working memory capacity: {working_mem.getCapacity()}")
            
            # Periodic skill robustness testing
            if step % 50 == 0 and step > 0:
                # Simulate retention test with interference
                interference_steps = 20
                retention_performance = []
                
                for interference_step in range(interference_steps):
                    # Add interference to test robustness
                    agent.processSensoryInput(percept)  # Same input with interference
                    brain.step(step + interference_step)
                    
                    retention_performance.append(brain.getAverageFiringRate())
                
                avg_retention = np.mean(retention_performance)
                skill_results["skill_robustness"].append({
                    "step": step,
                    "retention_performance": avg_retention,
                    "interference_duration": interference_steps
                })
                
                print(f"  Step {step}: Skill robustness = {avg_retention:.2f}")
        
        # Calculate final retention performance
        final_retention_test = []
        for test_step in range(50):
            brain.step(450 + test_step)
            final_retention_test.append(brain.getAverageFiringRate())
        
        skill_results["retention_tests"]["final_retention"] = np.mean(final_retention_test)
        skill_results["retention_tests"]["retention_decay"] = (
            skill_results["retention_tests"]["final_retention"] / 
            np.mean([p["firing_rate"] for p in skill_results["performance_history"][-50:]])
        )
        
        print(f"\nSkill consolidation results:")
        print(f"  Final retention performance: {skill_results['retention_tests']['final_retention']:.2f}")
        print(f"  Retention decay factor: {skill_results['retention_tests']['retention_decay']:.3f}")
        print(f"  Average performance during acquisition: {np.mean([p['firing_rate'] for p in skill_results['performance_history'][:200]]):.2f} Hz")
        
        return {
            "brain": brain,
            "agent": agent,
            "world": world,
            "skill_results": skill_results,
            "config": config
        }

if __name__ == "__main__":
    print("Long-Running Experiment Examples")
    print("=" * 30)
    
    # Example 1: Continuous learning
    continuous_result = LongRunningExperimentExamples.create_continuous_learning_experiment(2)
    print(f"\nContinuous learning: {continuous_result['total_steps']} steps completed in {continuous_result['total_time']/3600:.1f} hours")
    
    # Example 2: Evolutionary experiment
    evolutionary_result = LongRunningExperimentExamples.create_evolutionary_experiment(20)
    print(f"\nEvolutionary experiment: {evolutionary_result['total_generations']} generations, {evolutionary_result['total_individuals']} individuals")
    
    # Example 3: Skill consolidation
    consolidation_result = LongRunningExperimentExamples.create_skill_consolidation_experiment()
    print(f"\nSkill consolidation: 3 phases completed with {len(consolidation_result['skill_results']['performance_history'])} data points")
    
    print("\n=== All long-running examples completed ===")
