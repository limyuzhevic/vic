#!/usr/bin/env python3

import subprocess
import sys
import os
import tempfile
import json
from pathlib import Path
from typing import Dict, List, Any, Optional, Union

try:
    import pybind11
except ImportError:
    print("pybind11 not found. Please install pybind11 for Python bindings support.")
    sys.exit(1)

class NLMHelperUtilities:
    """
    Python helper utilities for NLM (Neural Learning Machine) users.
    
    This module provides additional functionality to make NLM more user-friendly
    and accessible for both beginners and advanced users.
    """
    
    @staticmethod
    def create_research_config(experiment_type: str = "standard", 
                              neuron_count: int = 1000,
                              enable_learning: bool = True,
                              enable_memory: bool = True) -> Dict[str, Any]:
        """
        Create a pre-configured research setup for different experiment types.
        
        Args:
            experiment_type: Type of experiment ("standard", "social", "curiosity", "development")
            neuron_count: Number of neurons to create
            enable_learning: Enable learning mechanisms
            enable_memory: Enable memory systems
            
        Returns:
            Dictionary with configuration settings
        """
        configs = {
            "standard": {
                "neuron_count": neuron_count,
                "region_count": 1,
                "connection_probability": 0.1,
                "simulation_timestep": 0.001,
                "stdp_ltp_weight": 0.02,
                "stdp_ltd_weight": 0.018,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001,
                "random_seed": 42,
                "enable_stdp": True,
                "enable_hebbian": True,
                "enable_structural_plasticity": enable_learning,
                "enable_development": enable_learning,
                "enable_reward_modulation": enable_learning,
                "enable_curiosity": True,
                "enable_novelty": True,
                "enable_prediction": True,
                "enable_working_memory": enable_memory,
                "enable_episodic_memory": enable_memory,
                "enable_associative_memory": enable_memory,
            },
            "social": {
                "neuron_count": neuron_count * 2,
                "region_count": 2,
                "connection_probability": 0.08,
                "simulation_timestep": 0.001,
                "stdp_ltp_weight": 0.015,
                "stdp_ltd_weight": 0.014,
                "social_coefficient": 0.3,
                "empathy_rate": 0.05,
                "imitation_probability": 0.4,
                "mirror_neuron_activation": True,
                "enable_structural_plasticity": enable_learning,
                "enable_development": enable_learning,
                "enable_reward_modulation": enable_learning,
                "enable_curiosity": True,
                "enable_novelty": True,
                "enable_prediction": True,
                "enable_working_memory": enable_memory,
                "enable_episodic_memory": enable_memory,
            },
            "curiosity": {
                "neuron_count": neuron_count,
                "region_count": 1,
                "connection_probability": 0.12,
                "simulation_timestep": 0.001,
                "stdp_ltp_weight": 0.025,
                "stdp_ltd_weight": 0.022,
                "curiosity_threshold": 0.5,
                "exploration_bonus": 0.3,
                "intrinsic_reward_scale": 0.2,
                "enable_structural_plasticity": enable_learning,
                "enable_development": enable_learning,
                "enable_reward_modulation": True,
                "enable_curiosity": True,
                "enable_novelty": True,
                "enable_prediction": True,
                "enable_working_memory": enable_memory,
                "enable_episodic_memory": enable_memory,
            },
            "development": {
                "neuron_count": neuron_count,
                "region_count": 1,
                "connection_probability": 0.09,
                "simulation_timestep": 0.001,
                "stdp_ltp_weight": 0.018,
                "stdp_ltd_weight": 0.016,
                "plasticity_decay_rate": 0.95,
                "critical_period_multiplier": 1.5,
                "maturation_threshold": 100,
                "enable_structural_plasticity": enable_learning,
                "enable_development": True,
                "enable_reward_modulation": enable_learning,
                "enable_curiosity": True,
                "enable_novelty": True,
                "enable_prediction": True,
                "enable_working_memory": enable_memory,
                "enable_episodic_memory": enable_memory,
            }
        }
        
        return configs.get(experiment_type, configs["standard"])
    
    @staticmethod
    def analyze_brain_state(brain, max_neurons: int = 10) -> Dict[str, Any]:
        """
        Analyze and provide detailed statistics about brain state.
        
        Args:
            brain: NLM Brain object
            max_neurons: Maximum number of neurons to analyze in detail
            
        Returns:
            Dictionary with brain state analysis
        """
        analysis = {
            "neuron_stats": {
                "total": brain.getTotalNeuronCount(),
                "active": brain.getActiveNeuronCount(),
                "firing": brain.getFiringNeuronCount(),
                "excitatory_ratio": brain.getExcitatoryInhibitionRatio(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount(),
            },
            "memory_stats": {
                "working_memory_traces": 0,
                "episodic_episodes": 0,
                "semantic_concepts": 0,
            },
            "neuromodulation": {
                "dopamine_level": 0.0,
                "curiosity_level": 0.0,
                "novelty_level": 0.0,
                "prediction_error": 0.0,
            },
            "developmental": {
                "stage": brain.getDevelopmentalStage(),
                "plasticity_rate": 1.0,
            }
        }
        
        # Try to access more detailed metrics if available
        try:
            if hasattr(brain, 'getWorkingMemory') and brain.getWorkingMemory():
                wm = brain.getWorkingMemory()
                analysis["memory_stats"]["working_memory_traces"] = wm.getActiveTraces()
        except:
            pass
            
        try:
            if hasattr(brain, 'getEpisodicMemory') and brain.getEpisodicMemory():
                em = brain.getEpisodicMemory()
                analysis["memory_stats"]["episodic_episodes"] = em.getEpisodeCount()
        except:
            pass
            
        try:
            analysis["neuromodulation"]["dopamine_level"] = brain.getNeuromodulationLevel()
        except:
            pass
            
        try:
            analysis["neuromodulation"]["curiosity_level"] = brain.getCuriosityLevel()
        except:
            pass
            
        try:
            analysis["neuromodulation"]["novelty_level"] = brain.getNoveltyLevel()
        except:
            pass
            
        try:
            analysis["neuromodulation"]["prediction_error"] = brain.getPredictionError()
        except:
            pass
            
        try:
            analysis["developmental"]["plasticity_rate"] = brain.getPlasticityRate()
        except:
            pass
            
        return analysis
    
    @staticmethod
    def create_experiment_setup(world_width: int = 16,
                               world_height: int = 16,
                               vision_width: int = 8,
                               vision_height: int = 8,
                               num_agents: int = 1) -> Dict[str, Any]:
        """
        Create a complete experiment setup with world, brain, and agent.
        
        Args:
            world_width: World width in cells
            world_height: World height in cells
            vision_width: Vision width in cells
            vision_height: Vision height in cells
            num_agents: Number of agents to create
            
        Returns:
            Dictionary with experiment components
        """
        try:
            import pynlm
            
            # Create default config
            config = pynlm.createDefaultConfig()
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.configure(world_width, world_height, vision_width, vision_height)
            world.reset()
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable advanced features
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            return {
                "config": config,
                "brain": brain,
                "world": world,
                "agent": agent,
                "components": {
                    "brain_count": 1,
                    "world_count": 1,
                    "agent_count": num_agents,
                    "total_neurons": brain.getTotalNeuronCount(),
                }
            }
            
        except ImportError as e:
            raise ImportError("NLM Python bindings not available. "
                            "Please build NLM with Python bindings enabled.") from e
    
    @staticmethod
    def run_learning_simulation(setup: Dict[str, Any], 
                               steps: int = 1000,
                               enable_checkpointing: bool = False,
                               checkpoint_path: str = "./nlm_checkpoint.bin") -> Dict[str, Any]:
        """
        Run a complete learning simulation with the given setup.
        
        Args:
            setup: Experiment setup from create_experiment_setup
            steps: Number of simulation steps
            enable_checkpointing: Enable checkpointing
            checkpoint_path: Path for checkpoint file
            
        Returns:
            Dictionary with simulation results
        """
        brain = setup["brain"]
        world = setup["world"]
        agent = setup["agent"]
        
        results = {
            "steps_completed": 0,
            "total_reward": 0.0,
            "firing_rates": [],
            "brain_states": [],
            "actions_taken": 0,
            "learning_events": 0,
            "avg_firing_rate": 0.0,
            "memory_growth": 0.0,
            "final_state": None
        }
        
        try:
            for step in range(steps):
                # Update world
                world.update(0.1)
                
                # Get percept
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain step
                brain.step(step)
                
                # Decode action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Collect metrics
                results["firing_rates"].append(brain.getAverageFiringRate())
                
                # Try to get detailed brain state
                try:
                    results["brain_states"].append(
                        NLMHelperUtilities.analyze_brain_state(brain)
                    )
                except:
                    pass
                
                # Count actions and learning events
                results["actions_taken"] += 1
                if hasattr(agent, 'getPredictionError') and agent.getPredictionError() > 0.1:
                    results["learning_events"] += 1
                
                # Apply reward modulation
                agent.applyRewardModulation(0.0, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                # Save checkpoint periodically
                if enable_checkpointing and step % 500 == 0:
                    brain.save(checkpoint_path)
            
            # Calculate averages
            if results["firing_rates"]:
                results["avg_firing_rate"] = sum(results["firing_rates"]) / len(results["firing_rates"])
            
            results["steps_completed"] = steps
            results["final_state"] = NLMHelperUtilities.analyze_brain_state(brain)
            
            return results
            
        except Exception as e:
            results["error"] = str(e)
            return results
    
    @staticmethod
    def benchmark_performance(setup: Dict[str, Any], 
                             durations: List[float] = None) -> Dict[str, Any]:
        """
        Benchmark performance of NLM components.
        
        Args:
            setup: Experiment setup
            durations: List of simulation durations to test (in seconds)
            
        Returns:
            Dictionary with benchmark results
        """
        if durations is None:
            durations = [0.1, 0.5, 1.0, 2.0]
            
        brain = setup["brain"]
        world = setup["world"]
        agent = setup["agent"]
        
        results = {
            "benchmarks": [],
            "performance_scaling": {},
            "bottleneck_analysis": {}
        }
        
        for duration in durations:
            steps = int(duration / 0.1)  # Assuming 0.1s timestep
            
            import time
            start_time = time.time()
            
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                agent.applyRewardModulation(0.0, 0.0)
                agent.updateDevelopment(0.1)
            
            elapsed = time.time() - start_time
            steps_per_second = steps / elapsed if elapsed > 0 else 0
            
            benchmark = {
                "duration_sec": duration,
                "steps_completed": steps,
                "elapsed_time": elapsed,
                "steps_per_second": steps_per_second,
                "firing_rate_avg": brain.getAverageFiringRate(),
                "neurons_active": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
            }
            
            results["benchmarks"].append(benchmark)
        
        # Analyze performance scaling
        if len(results["benchmarks"]) > 1:
            first_bench = results["benchmarks"][0]
            last_bench = results["benchmarks"][-1]
            
            results["performance_scaling"] = {
                "time_scaling": (last_bench["steps_per_second"] / first_bench["steps_per_second"]
                               if first_bench["steps_per_second"] > 0 else 0),
                "memory_scaling": (last_bench["neurons_active"] / first_bench["neurons_active"]
                                if first_bench["neurons_active"] > 0 else 0),
                "efficiency_scaling": (last_bench["firing_rate_avg"] / first_bench["firing_rate_avg"]
                                    if first_bench["firing_rate_avg"] > 0 else 0),
            }
        
        # Identify bottlenecks
        max_time_per_step = max(b["
            elapsed_time"] / b["steps_completed"] for b in results["benchmarks"] 
                                  if b["steps_completed"] > 0)
        
        results["bottleneck_analysis"] = {
            "max_time_per_step_ms": max_time_per_step * 1000,
            "recommended_max_steps": int(1.0 / max_time_per_step) if max_time_per_step > 0 else 0,
            "performance_rating": "GOOD" if max_time_per_step < 0.001 else "NEEDS_OPTIMIZATION"
        }
        
        return results

# Create module-level functions for easier use
create_research_config = NLMHelperUtilities.create_research_config
analyze_brain_state = NLMHelperUtilities.analyze_brain_state
create_experiment_setup = NLMHelperUtilities.create_experiment_setup
run_learning_simulation = NLMHelperUtilities.run_learning_simulation
benchmark_performance = NLMHelperUtilities.benchmark_performance

__all__ = [
    'NLMHelperUtilities',
    'create_research_config',
    'analyze_brain_state', 
    'create_experiment_setup',
    'run_learning_simulation',
    'benchmark_performance'
]
