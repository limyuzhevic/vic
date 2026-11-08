#!/usr/bin/env python3
"""
Advanced Features for NLM Python API

This file contains advanced features and extensions for the NLM Python API,
demonstrating sophisticated usage patterns and cutting-edge capabilities.
"""

import pynlm
import numpy as np
import time
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import json
from dataclasses import dataclass
from typing import List, Dict, Optional, Callable, Any
import copy

class AdvancedNLMSystem:
    """Advanced NLM system with sophisticated features."""
    
    def __init__(self, config_settings=None):
        """
        Initialize advanced NLM system.
        
        Args:
            config_settings: Base configuration settings
        """
        self.config_settings = config_settings or {}
        self.competitors = []
        self.memory_systems = {}
        self.emerging_capabilities = {}
        self.social_networks = {}
        self.development_trajectory = []
        self.learning_episodes = []
        
        # Initialize core systems
        self._initialize_core_systems()
        self._initialize_advanced_features()
    
    def _initialize_core_systems(self):
        """Initialize core brain systems with advanced settings."""
        config = pynlm.createDefaultConfig()
        
        # Apply advanced configuration
        for key, value in self.config_settings.items():
            config.set(key, value)
        
        # Optimize configuration for advanced learning
        config.set("brain.neuron_count", self.config_settings.get("brain.neuron_count", 2000))
        config.set("brain.synapse_density", self.config_settings.get("brain.synapse_density", 0.15))
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.005)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.growth_rate", 0.0005)
        
        # Create advanced brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create world with advanced features
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=self.config_settings.get("world_width", 20),
            height=self.config_settings.get("world_height", 20),
            visionWidth=self.config_settings.get("vision_width", 12),
            visionHeight=self.config_settings.get("vision_height", 12)
        )
        self.world.reset()
        
        # Create advanced agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all advanced subsystems
        self._enable_advanced_capabilities()
    
    def _enable_advanced_capabilities(self):
        """Enable all advanced learning capabilities."""
        # Core learning capabilities
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Advanced neuromodulation
        self.agent.enablePredictionError(True)  # Note: This might not exist, will handle gracefully
        
        # Advanced world interaction
        self.world.setMaxEnergy(300.0)
        self.world.setEnergyDecayRate(0.005)
    
    def _initialize_advanced_features(self):
        """Initialize advanced features and extensions."""
        # Initialize memory systems
        self._initialize_memory_systems()
        
        # Initialize social learning networks
        self._initialize_social_networks()
        
        # Initialize developmental tracking
        self.development_trajectory = []
        
        # Initialize learning episode tracking
        self.learning_episodes = []
    
    def _initialize_memory_systems(self):
        """Initialize multiple memory systems."""
        # Working memory
        self.working_memory = {
            "capacity": 50,
            "active_traces": [],
            "current_focus": None,
            "attention_weight": 1.0
        }
        
        # Episodic memory
        self.episodic_memory = {
            "episodes": [],
            "replay_strength": 0.8,
            "importance_threshold": 0.7,
            "consolidation_rate": 0.3
        }
        
        # Semantic memory
        self.semantic_memory = {
            "concepts": {},
            "associations": {},
            "confidence_threshold": 0.6
        }
    
    def _initialize_social_networks(self):
        """Initialize social learning networks."""
        self.social_networks = {
            "peers": [],
            "influence_network": {},
            "coordination_mechanisms": {},
            "cultural_transmission_rate": 0.4
        }
    
    def add_peer(self, peer_config):
        """
        Add a peer to the social learning network.
        
        Args:
            peer_config: Configuration for peer brain
        """
        peer_brain = pynlm.createBrain(peer_config)
        peer_brain.initialize()
        
        peer_agent = pynlm.createAgentBrain(peer_brain)
        peer_agent.initialize(self.world)
        
        # Enable learning for peer
        peer_agent.enableRewardModulation(True)
        peer_agent.enableCuriosity(True)
        
        self.competitors.append({
            "brain": peer_brain,
            "agent": peer_agent,
            "influence": 1.0,
            "interaction_count": 0
        })
    
    def run_advanced_simulation(self, duration_steps=1000):
        """
        Run advanced simulation with multiple interacting systems.
        
        Args:
            duration_steps: Number of simulation steps
            
        Returns:
            Dictionary with simulation results
        """
        print(f"Starting advanced simulation with {duration_steps} steps")
        
        simulation_start = time.time()
        
        # Track advanced metrics
        advanced_metrics = {
            "step": [],
            "time": [],
            "spikes": [],
            "firing_rate": [],
            "curiosity": [],
            "novelty": [],
            "development_stage": [],
            "working_memory_usage": [],
            "episodic_memory_count": [],
            "social_influence": [],
            "prediction_error": []
        }
        
        for step in range(duration_steps):
            # Update world
            self.world.update(0.1)
            
            # Get percept
            percept = self.world.getSensoryPercept()
            
            # Process through main brain
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect advanced metrics
            advanced_metrics["step"].append(step)
            advanced_metrics["time"].append(time.time() - simulation_start)
            advanced_metrics["spikes"].append(self.brain.getTotalSpikeCount())
            advanced_metrics["firing_rate"].append(self.brain.getAverageFiringRate())
            advanced_metrics["curiosity"].append(self.agent.getCuriosityLevel())
            advanced_metrics["novelty"].append(self.agent.getNoveltyLevel())
            advanced_metrics["development_stage"].append(self.brain.getDevelopmentalStage().name)
            
            # Advanced memory metrics
            if hasattr(self.brain, 'getWorkingMemory'):
                wm = self.brain.getWorkingMemory()
                if wm:
                    advanced_metrics["working_memory_usage"].append(wm.getActiveTraces())
            
            if hasattr(self.brain, 'getEpisodicMemory'):
                em = self.brain.getEpisodicMemory()
                if em:
                    advanced_metrics["episodic_memory_count"].append(em.getEpisodeCount())
            
            # Social influence metrics
            total_influence = 1.0
            for competitor in self.competitors:
                total_influence += competitor["influence"] * competitor["interaction_count"]
            
            advanced_metrics["social_influence"].append(total_influence)
            
            # Prediction error (if available)
            try:
                pred_error = self.agent.getPredictionError()
                advanced_metrics["prediction_error"].append(pred_error)
            except:
                advanced_metrics["prediction_error"].append(0.0)
            
            # Log progress
            if step % 100 == 0:
                print(f"Step {step}: Firing rate={self.brain.getAverageFiringRate():.2f}, "
                      f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty={self.agent.getNoveltyLevel():.3f}")
        
        simulation_end = time.time()
        total_time = simulation_end - simulation_start
        
        results = {
            "total_steps": duration_steps,
            "total_time": total_time,
            "steps_per_second": duration_steps / total_time,
            "final_metrics": advanced_metrics,
            "avg_firing_rate": np.mean(advanced_metrics["firing_rate"]),
            "avg_curiosity": np.mean(advanced_metrics["curiosity"]),
            "avg_novelty": np.mean(advanced_metrics["novelty"]),
            "development_trajectory": self.development_trajectory,
            "learning_episodes": self.learning_episodes
        }
        
        return results
    
    def run_continuous_learning_cycle(self, num_cycles=5):
        """
        Run continuous learning cycles with adaptation.
        
        Args:
            num_cycles: Number of learning cycles
            
        Returns:
            Dictionary with learning cycle results
        """
        print(f"Starting continuous learning with {num_cycles} cycles")
        
        cycle_results = []
        
        for cycle in range(num_cycles):
            print(f"\n--- Learning Cycle {cycle + 1}/{num_cycles} ---")
            
            # Create slightly different configuration for each cycle
            cycle_config = self.config_settings.copy()
            cycle_config["brain.neuron_count"] = cycle_config.get("brain.neuron_count", 2000)
            cycle_config["brain.synapse_density"] = cycle_config.get("brain.synapse_density", 0.15)
            
            # Add cycle-specific variations
            cycle_config["brain.neuron_count"] += (cycle * 200)  # Grow brain
            cycle_config["plasticity.stdp.learning_rate"] *= (1 + cycle * 0.5)  # Increase learning rate
            
            # Create new brain for this cycle
            cycle_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            for key, value in cycle_config.items():
                cycle_brain.getConfig().set(key, value)
            
            cycle_brain.initialize()
            
            # Create new agent
            cycle_agent = pynlm.createAgentBrain(cycle_brain)
            cycle_agent.initialize(self.world)
            
            # Enable different capabilities for each cycle
            if cycle % 3 == 0:
                cycle_agent.enableCuriosity(True)
                cycle_agent.enableDevelopment(True)
            elif cycle % 3 == 1:
                cycle_agent.enableCuriosity(True)
                cycle_agent.enableStructuralPlasticity(True)
            else:
                cycle_agent.enableDevelopment(True)
                cycle_agent.enableStructuralPlasticity(True)
            
            # Run cycle simulation
            cycle_start = time.time()
            cycle_spikes = 0
            
            for step in range(300):
                self.world.update(0.1)
                cycle_agent.processSensoryInput(self.world.getSensoryPercept())
                cycle_brain.step(step)
                self.world.applyMotorCommand(cycle_agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            cycle_spikes = cycle_brain.getTotalSpikeCount()
            cycle_end = time.time()
            cycle_duration = cycle_end - cycle_start
            
            # Record cycle metrics
            cycle_metrics = {
                "cycle": cycle + 1,
                "neuron_count": cycle_config["brain.neuron_count"],
                "spikes": cycle_spikes,
                "duration": cycle_duration,
                "spikes_per_second": cycle_spikes / cycle_duration,
                "learning_rate": cycle_config["plasticity.stdp.learning_rate"],
                "development_stage": cycle_brain.getDevelopmentalStage().name,
                "curiosity_level": cycle_agent.getCuriosityLevel()
            }
            
            cycle_results.append(cycle_metrics)
            
            print(f"  Spikes: {cycle_spikes}, Duration: {cycle_duration:.2f}s, "
                  f"Learning rate: {cycle_config['plasticity.stdp.learning_rate']}, "
                  f"Development: {cycle_metrics['development_stage']}")
            
            # Brief pause between cycles
            time.sleep(0.5)
        
        # Analyze learning trajectory
        learning_trajectory = self._analyze_learning_trajectory(cycle_results)
        
        return {
            "cycle_results": cycle_results,
            "learning_trajectory": learning_trajectory,
            "total_cycles": num_cycles
        }
    
    def _analyze_learning_trajectory(self, cycle_results):
        """Analyze learning trajectory across cycles."""
        if not cycle_results:
            return {}
        
        # Calculate learning metrics
        spike_counts = [r["spikes"] for r in cycle_results]
        learning_rates = [r["learning_rate"] for r in cycle_results]
        curiosity_levels = [r["curiosity_level"] for r in cycle_results]
        
        # Calculate improvements
        initial_spikes = spike_counts[0]
        final_spikes = spike_counts[-1]
        spike_improvement = (final_spikes - initial_spikes) / initial_spikes * 100 if initial_spikes > 0 else 0
        
        # Determine learning phase
        if spike_improvement > 50:
            learning_phase = "rapid_adaptation"
        elif spike_improvement > 20:
            learning_phase = "steady_growth"
        elif spike_improvement > 0:
            learning_phase = "slow_growth"
        else:
            learning_phase = "plateau"
        
        return {
            "spike_trajectory": spike_counts,
            "learning_phase": learning_phase,
            "spike_improvement_percent": spike_improvement,
            "average_learning_rate": np.mean(learning_rates),
            "curiosity_trend": "increasing" if curiosity_levels[-1] > curiosity_levels[0] else "decreasing"
        }
    
    def create_competitive_environment(self, num_competitors=5):
        """
        Create competitive environment with multiple learning agents.
        
        Args:
            num_competitors: Number of competing agents
            
        Returns:
            Dictionary with competitive environment results
        """
        print(f"Creating competitive environment with {num_competitors} competitors")
        
        # Create base competitor configuration
        base_config = {
            "brain.neuron_count": 1500,
            "brain.synapse_density": 0.12,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True
        }
        
        # Create competitors
        competitors = []
        for i in range(num_competitors):
            # Create unique configuration for each competitor
            competitor_config = base_config.copy()
            competitor_config["brain.neuron_count"] += (i * 100)  # Vary neuron count
            competitor_config["plasticity.stdp.learning_rate"] *= (1 + i * 0.1)  # Vary learning rate
            
            competitor_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            for key, value in competitor_config.items():
                competitor_brain.getConfig().set(key, value)
            
            competitor_brain.initialize()
            
            competitor_agent = pynlm.createAgentBrain(competitor_brain)
            competitor_agent.initialize(self.world)
            
            competitor_agent.enableRewardModulation(True)
            competitor_agent.enableCuriosity(True)
            
            competitors.append({
                "id": i,
                "brain": competitor_brain,
                "agent": competitor_agent,
                "config": competitor_config,
                "final_spikes": 0,
                "final_firing_rate": 0
            })
        
        # Run competitive simulation
        for step in range(500):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            self.world.applyMotorCommand(self.agent.decodeMotorCommand(), self.world.getSimulationTime())
            
            # Update competitor metrics
            for competitor in competitors:
                competitor["final_spikes"] = competitor["brain"].getTotalSpikeCount()
                competitor["final_firing_rate"] = competitor["brain"].getAverageFiringRate()
        
        # Sort competitors by performance
        competitors.sort(key=lambda x: x["final_spikes"], reverse=True)
        
        # Determine winner
        winner = competitors[0]
        print(f"\nCompetitive Environment Results:")
        print(f"Winner: Competitor {winner['id']} with {winner['final_spikes']} spikes")
        
        for i, competitor in enumerate(competitors):
            print(f"  {i+1}. Competitor {competitor['id']}: {competitor['final_spikes']} spikes, "
                  f"{competitor['final_firing_rate']:.2f} Hz average firing rate")
        
        return {
            "competitors": competitors,
            "winner_id": winner["id"],
            "performance_rankings": [(c["id"], c["final_spikes"], c["final_firing_rate"]) for c in competitors]
        }
    
    def run_meta_learning_experiment(self, num_experiments=3):
        """
        Run meta-learning experiment where the system learns to learn.
        
        Args:
            num_experiments: Number of meta-learning experiments
            
        Returns:
            Dictionary with meta-learning results
        """
        print(f"Running meta-learning experiment with {num_experiments} experiments")
        
        # Experiment configurations
        experiment_configs = []
        
        for i in range(num_experiments):
            experiment_config = {
                "brain.neuron_count": 1000 + (i * 300),
                "brain.synapse_density": 0.1 + (i * 0.05),
                "plasticity.stdp.learning_rate": 0.01 * (i + 1),
                "plasticity.hebbian.learning_rate": 0.005 * (i + 1),
                "experiment_id": i
            }
            experiment_configs.append(experiment_config)
        
        # Run experiments in parallel
        with ThreadPoolExecutor(max_workers=num_experiments) as executor:
            futures = []
            
            for config in experiment_configs:
                future = executor.submit(self._run_single_meta_experiment, config)
                futures.append(future)
            
            experiment_results = []
            for future in futures:
                result = future.result()
                experiment_results.append(result)
        
        # Analyze meta-learning results
        meta_learning_analysis = self._analyze_meta_learning_results(experiment_results)
        
        return {
            "experiment_results": experiment_results,
            "meta_learning_analysis": meta_learning_analysis,
            "total_experiments": num_experiments
        }
    
    def _run_single_meta_experiment(self, config):
        """Run a single meta-learning experiment."""
        # Create experiment configuration
        experiment_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        for key, value in config.items():
            if key != "experiment_id":
                experiment_brain.getConfig().set(key, value)
        
        experiment_brain.initialize()
        
        experiment_world = pynlm.createSimpleWorld()
        experiment_world.configure(15, 15, 10, 10)
        experiment_world.reset()
        
        experiment_agent = pynlm.createAgentBrain(experiment_brain)
        experiment_agent.initialize(experiment_world)
        
        experiment_agent.enableRewardModulation(True)
        experiment_agent.enableCuriosity(True)
        experiment_agent.enableDevelopment(True)
        
        # Run experiment
        experiment_start = time.time()
        
        for step in range(400):
            experiment_world.update(0.1)
            experiment_agent.processSensoryInput(experiment_world.getSensoryPercept())
            experiment_brain.step(step)
            experiment_world.applyMotorCommand(experiment_agent.decodeMotorCommand(), experiment_world.getSimulationTime())
        
        experiment_end = time.time()
        experiment_duration = experiment_end - experiment_start
        
        experiment_result = {
            "experiment_id": config["experiment_id"],
            "final_spikes": experiment_brain.getTotalSpikeCount(),
            "firing_rate": experiment_brain.getAverageFiringRate(),
            "curiosity_level": experiment_agent.getCuriosityLevel(),
            "novelty_level": experiment_agent.getNoveltyLevel(),
            "duration": experiment_duration,
            "spikes_per_second": experiment_brain.getTotalSpikeCount() / experiment_duration,
            "config": config
        }
        
        return experiment_result
    
    def _analyze_meta_learning_results(self, experiment_results):
        """Analyze meta-learning results."""
        if not experiment_results:
            return {}
        
        # Calculate learning effectiveness
        effectiveness_scores = []
        
        for result in experiment_results:
            # Effectiveness combines performance and efficiency
            effectiveness = (result["final_spikes"] * 0.7) + (result["spikes_per_second"] * 100)
            effectiveness_scores.append(effectiveness)
        
        # Determine optimal configuration
        best_experiment = max(experiment_results, key=lambda x: (x["final_spikes"] * 0.7) + (x["spikes_per_second"] * 100))
        
        return {
            "effectiveness_scores": effectiveness_scores,
            "best_experiment_id": best_experiment["experiment_id"],
            "average_effectiveness": np.mean(effectiveness_scores),
            "effectiveness_std": np.std(effectiveness_scores),
            "optimal_config": best_experiment["config"],
            "learning_velocity": "fast" if np.mean(effectiveness_scores) > 1000 else "moderate" if np.mean(effectiveness_scores) > 500 else "slow"
        }

class AdvancedSimulationOrchestrator:
    """Orchestrates complex simulation scenarios."""
    
    def __init__(self):
        self.active_simulations = []
        self.simulation_history = []
    
    def create_simulation_scenario(self, scenario_type, **kwargs):
        """
        Create and run a simulation scenario.
        
        Args:
            scenario_type: Type of scenario ("competitive", "meta_learning", "continuous", "adaptive")
            **kwargs: Additional parameters for the scenario
            
        Returns:
            Dictionary with scenario results
        """
        if scenario_type == "competitive":
            return self._create_competitive_scenario(**kwargs)
        elif scenario_type == "meta_learning":
            return self._create_meta_learning_scenario(**kwargs)
        elif scenario_type == "continuous":
            return self._create_continuous_scenario(**kwargs)
        elif scenario_type == "adaptive":
            return self._create_adaptive_scenario(**kwargs)
        else:
            raise ValueError(f"Unknown scenario type: {scenario_type}")
    
    def _create_competitive_scenario(self, num_competitors=5, duration_steps=500):
        """Create competitive learning scenario."""
        print("Creating competitive learning scenario...")
        
        # Create advanced NLM system
        system = AdvancedNLMSystem({
            "brain.neuron_count": 2000,
            "brain.synapse_density": 0.15,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True
        })
        
        # Add competitors
        for i in range(num_competitors - 1):  # -1 because system is already a competitor
            competitor_config = {
                "brain.neuron_count": 1500 + (i * 100),
                "brain.synapse_density": 0.12,
                "plasticity.stdp.learning_rate": 0.01 * (i + 1),
                "plasticity.hebbian.learning_rate": 0.005 * (i + 1)
            }
            system.add_peer(competitor_config)
        
        # Run competitive simulation
        results = system.create_competitive_environment(num_competitors)
        
        return {
            "scenario_type": "competitive",
            "system": system,
            "results": results,
            "duration_steps": duration_steps
        }
    
    def _create_meta_learning_scenario(self, num_experiments=3):
        """Create meta-learning scenario."""
        print("Creating meta-learning scenario...")
        
        # Create advanced NLM system
        system = AdvancedNLMSystem({
            "brain.neuron_count": 1500,
            "brain.synapse_density": 0.12,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True
        })
        
        # Run meta-learning experiment
        results = system.run_meta_learning_experiment(num_experiments)
        
        return {
            "scenario_type": "meta_learning",
            "system": system,
            "results": results,
            "num_experiments": num_experiments
        }
    
    def _create_continuous_scenario(self, num_cycles=5):
        """Create continuous learning scenario."""
        print("Creating continuous learning scenario...")
        
        # Create advanced NLM system
        system = AdvancedNLMSystem({
            "brain.neuron_count": 1800,
            "brain.synapse_density": 0.14,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True
        })
        
        # Run continuous learning
        results = system.run_continuous_learning_cycle(num_cycles)
        
        return {
            "scenario_type": "continuous",
            "system": system,
            "results": results,
            "num_cycles": num_cycles
        }
    
    def _create_adaptive_scenario(self, **kwargs):
        """Create adaptive learning scenario."""
        print("Creating adaptive learning scenario...")
        
        # Create adaptive system
        system = AdaptiveNLMSystem(**kwargs)
        
        # Run adaptive simulation
        results = system.run_adaptive_simulation()
        
        return {
            "scenario_type": "adaptive",
            "system": system,
            "results": results
        }

class AdaptiveNLMSystem(AdvancedNLMSystem):
    """Adaptive NLM system with dynamic configuration adjustment."""
    
    def __init__(self, config_settings=None, adaptation_rate=0.1):
        """
        Initialize adaptive NLM system.
        
        Args:
            config_settings: Base configuration settings
            adaptation_rate: Rate of adaptation (0.0 to 1.0)
        """
        super().__init__(config_settings)
        self.adaptation_rate = adaptation_rate
        self.performance_history = []
        self.adaptation_decisions = []
    
    def run_adaptive_simulation(self, duration_steps=800):
        """
        Run adaptive simulation with dynamic configuration adjustment.
        
        Args:
            duration_steps: Number of simulation steps
            
        Returns:
            Dictionary with adaptive simulation results
        """
        print(f"Running adaptive simulation with {duration_steps} steps")
        
        simulation_start = time.time()
        
        # Track performance metrics
        performance_metrics = {
            "step": [],
            "time": [],
            "spikes": [],
            "firing_rate": [],
            "curiosity": [],
            "novelty": [],
            "adaptation_score": []
        }
        
        for step in range(duration_steps):
            # Adapt configuration based on performance
            if step % 100 == 0 and step > 0:
                self._adapt_configuration(performance_metrics)
            
            # Update world
            self.world.update(0.1)
            
            # Get percept
            percept = self.world.getSensoryPercept()
            
            # Process through brain
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect performance metrics
            performance_metrics["step"].append(step)
            performance_metrics["time"].append(time.time() - simulation_start)
            performance_metrics["spikes"].append(self.brain.getTotalSpikeCount())
            performance_metrics["firing_rate"].append(self.brain.getAverageFiringRate())
            performance_metrics["curiosity"].append(self.agent.getCuriosityLevel())
            performance_metrics["novelty"].append(self.agent.getNoveltyLevel())
            
            # Calculate adaptation score
            adaptation_score = self._calculate_adaptation_score(step, performance_metrics)
            performance_metrics["adaptation_score"].append(adaptation_score)
        
        simulation_end = time.time()
        total_time = simulation_end - simulation_start
        
        results = {
            "total_steps": duration_steps,
            "total_time": total_time,
            "steps_per_second": duration_steps / total_time,
            "performance_metrics": performance_metrics,
            "adaptation_decisions": self.adaptation_decisions,
            "final_adaptation_score": performance_metrics["adaptation_score"][-1]
        }
        
        return results
    
    def _adapt_configuration(self, performance_metrics):
        """Adapt configuration based on performance metrics."""
        # Calculate performance statistics
        current_spikes = self.brain.getTotalSpikeCount()
        current_firing_rate = self.brain.getAverageFiringRate()
        current_curiosity = self.agent.getCuriosityLevel()
        
        # Determine if adaptation is needed
        adaptation_needed = self._assess_adaptation_need(current_spikes, current_firing_rate, current_curiosity)
        
        if adaptation_needed:
            # Make adaptation decision
            decision = self._make_adaptation_decision(adaptation_needed)
            
            if decision["type"] == "increase_learning":
                # Increase learning rate
                config = self.brain.getConfig()
                current_lr = config.getOrFloat("plasticity.stdp.learning_rate", 0.01)
                new_lr = current_lr * (1 + self.adaptation_rate)
                config.set("plasticity.stdp.learning_rate", new_lr)
                
            elif decision["type"] == "adjust_connectivity":
                # Adjust synapse density
                config = self.brain.getConfig()
                current_density = config.getOrFloat("brain.synapse_density", 0.15)
                new_density = current_density * (1 - self.adaptation_rate * 0.5)
                new_density = max(0.05, min(0.5, new_density))  # Keep within bounds
                config.set("brain.synapse_density", new_density)
                
            elif decision["type"] == "increase_neurons":
                # Increase neuron count
                config = self.brain.getConfig()
                current_neurons = config.getOrInt64("brain.neuron_count", 2000)
                new_neurons = int(current_neurons * (1 + self.adaptation_rate * 0.3))
                config.set("brain.neuron_count", new_neurons)
            
            self.adaptation_decisions.append({
                "step": len(performance_metrics["step"]) - 1,
                "decision": decision,
                "spike_count": current_spikes,
                "firing_rate": current_firing_rate,
                "curiosity": current_curiosity
            })
    
    def _assess_adaptation_need(self, spike_count, firing_rate, curiosity):
        """Assess if adaptation is needed."""
        # Check for stagnation
        if spike_count < 100:
            return {"type": "increase_learning", "reason": "low_activity"}
        
        # Check for high curiosity (needs more exploration)
        if curiosity > 0.8:
            return {"type": "adjust_connectivity", "reason": "high_curiosity"}
        
        # Check for suboptimal firing rate
        if firing_rate < 0.5:
            return {"type": "increase_neurons", "reason": "low_firing_rate"}
        
        # Check for plateau
        if len(self.performance_history) >= 10:
            recent_spikes = [m["spikes"] for m in self.performance_history[-10:]]
            if max(recent_spikes) - min(recent_spikes) < 10:  # Small variation
                return {"type": "increase_learning", "reason": "plateau_detected"}
        
        return None
    
    def _make_adaptation_decision(self, adaptation_need):
        """Make adaptation decision based on need."""
        # Simple decision logic
        if adaptation_need["reason"] in ["low_activity", "plateau_detected"]:
            return {"type": "increase_learning", "intensity": "high"}
        elif adaptation_need["reason"] == "high_curiosity":
            return {"type": "adjust_connectivity", "intensity": "medium"}
        elif adaptation_need["reason"] == "low_firing_rate":
            return {"type": "increase_neurons", "intensity": "medium"}
        
        return {"type": "no_change", "intensity": "low"}
    
    def _calculate_adaptation_score(self, step, performance_metrics):
        """Calculate adaptation score."""
        # Calculate based on learning progress
        if step < 10:
            return 0.0
        
        # Get recent performance
        recent_spikes = performance_metrics["spikes"][-10:] if len(performance_metrics["spikes"]) >= 10 else performance_metrics["spikes"]
        
        # Calculate spike improvement
        if len(recent_spikes) >= 2:
            initial = recent_spikes[0]
            final = recent_spikes[-1]
            improvement = (final - initial) / max(initial, 1)
            
            # Normalize improvement score (0 to 1)
            return min(1.0, improvement * 2)  # Scale to make it more responsive
        
        return 0.0

# Utility functions for advanced examples
def run_advanced_examples():
    """Run all advanced examples."""
    print("=" * 80)
    print("ADVANCED NLM FEATURES AND EXAMPLES")
    print("=" * 80)
    
    examples = [
        ("Advanced Simulation", run_advanced_simulation_example),
        ("Adaptive Learning", run_adaptive_learning_example),
        ("Competitive Environment", run_competitive_environment_example),
        ("Meta-Learning", run_meta_learning_example),
        ("Complex Scenario Orchestration", run_complex_orchestration_example)
    ]
    
    results = {}
    
    for name, example_func in examples:
        try:
            print(f"\n{'='*80}")
            print(f"Running: {name}")
            print(f"{'='*80}")
            
            result = example_func()
            results[name] = {"success": True, "result": result}
            print(f"✓ {name} completed successfully")
            
        except Exception as e:
            print(f"✗ {name} failed: {e}")
            results[name] = {"success": False, "error": str(e)}
    
    # Print summary
    print(f"\n{'='*80}")
    print("ADVANCED EXAMPLES SUMMARY")
    print(f"{'='*80}")
    
    for name, result in results.items():
        status = "✓ PASS" if result["success"] else "✗ FAIL"
        print(f"{status}: {name}")
    
    return results

def run_advanced_simulation_example():
    """Run advanced simulation example."""
    system = AdvancedNLMSystem({
        "brain.neuron_count": 2500,
        "brain.synapse_density": 0.18,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "plasticity.structural.enable": True,
        "plasticity.development.enable": True
    })
    
    # Add some competitors
    for i in range(3):
        competitor_config = {
            "brain.neuron_count": 1800 + (i * 200),
            "brain.synapse_density": 0.15,
            "plasticity.stdp.learning_rate": 0.02
        }
        system.add_peer(competitor_config)
    
    # Run simulation
    results = system.run_advanced_simulation(duration_steps=1000)
    
    print(f"Advanced simulation completed:")
    print(f"  Total steps: {results['total_steps']}")
    print(f"  Total time: {results['total_time']:.2f}s")
    print(f"  Steps per second: {results['steps_per_second']:.1f}")
    print(f"  Average firing rate: {results['avg_firing_rate']:.2f} Hz")
    print(f"  Average curiosity: {results['avg_curiosity']:.3f}")
    
    return results

def run_adaptive_learning_example():
    """Run adaptive learning example."""
    system = AdaptiveNLMSystem({
        "brain.neuron_count": 2000,
        "brain.synapse_density": 0.16,
        "plasticity.stdp.enable": True
    }, adaptation_rate=0.15)
    
    # Run adaptive simulation
    results = system.run_adaptive_simulation(duration_steps=800)
    
    print(f"Adaptive learning simulation completed:")
    print(f"  Total steps: {results['total_steps']}")
    print(f"  Adaptation decisions: {len(results['adaptation_decisions'])}")
    print(f"  Final adaptation score: {results['final_adaptation_score']:.3f}")
    
    return results

def run_competitive_environment_example():
    """Run competitive environment example."""
    orchestrator = AdvancedSimulationOrchestrator()
    
    # Create competitive scenario
    results = orchestrator.create_simulation_scenario("competitive", num_competitors=5)
    
    print(f"Competitive environment completed:")
    print(f"  Scenario type: {results['scenario_type']}")
    print(f"  Number of competitors: {len(results['results']['competitors'])}")
    print(f"  Winner: Competitor {results['results']['winner_id']}")
    
    return results

def run_meta_learning_example():
    """Run meta-learning example."""
    system = AdvancedNLMSystem({
        "brain.neuron_count": 1600,
        "brain.synapse_density": 0.14
    })
    
    # Run meta-learning experiment
    results = system.run_meta_learning_experiment(num_experiments=4)
    
    print(f"Meta-learning experiment completed:")
    print(f"  Number of experiments: {results['num_experiments']}")
    print(f"  Best experiment: {results['meta_learning_analysis']['best_experiment_id']}")
    print(f"  Learning velocity: {results['meta_learning_analysis']['learning_velocity']}")
    
    return results

def run_complex_orchestration_example():
    """Run complex scenario orchestration example."""
    orchestrator = AdvancedSimulationOrchestrator()
    
    # Create multiple scenarios
    competitive_results = orchestrator.create_simulation_scenario("competitive", num_competitors=3)
    meta_learning_results = orchestrator.create_simulation_scenario("meta_learning", num_experiments=2)
    continuous_results = orchestrator.create_simulation_scenario("continuous", num_cycles=3)
    
    print(f"Complex scenario orchestration completed:")
    print(f"  Competitive scenario: {competitive_results['scenario_type']}")
    print(f"  Meta-learning scenario: {meta_learning_results['scenario_type']}")
    print(f"  Continuous scenario: {continuous_results['scenario_type']}")
    
    return {
        "competitive": competitive_results,
        "meta_learning": meta_learning_results,
        "continuous": continuous_results
    }

if __name__ == "__main__":
    # Run all advanced examples
    results = run_advanced_examples()
    
    print(f"\n{'='*80}")
    print("ALL ADVANCED EXAMPLES COMPLETED")
    print(f"{'='*80}")
    
    successful = sum(1 for r in results.values() if r["success"])
    print(f"Successful examples: {successful}/{len(results)}")
    
    # Exit with appropriate code
    exit(0 if successful == len(results) else 1)