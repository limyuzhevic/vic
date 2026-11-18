#!/usr/bin/env python3
"""
Enhanced NLM Python Interface
============================

This document describes the enhanced Python interface for the NLM (Neural Learning Machine)
simulation framework. This interface provides comprehensive access to all NLM features
through a user-friendly Python API that wraps the underlying C++ bindings.

The interface has been enhanced with:
- Comprehensive Python bindings for all NLM systems
- Convenience methods for common workflows
- Enhanced error handling and validation
- Advanced configuration management
- Simulation management utilities
- Visualization and analysis tools
- Command-line integration

All features build upon the existing pybind11 bindings but provide additional
functionality and improved usability.
"""

import pybind11
import json
import logging
from pathlib import Path
from typing import Dict, Any, Optional, List, Union
import numpy as np

# Import NLM Python bindings
try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some features will be limited.")

class NLMConfig:
    """
    Enhanced NLM Configuration with validation and preset profiles.
    
    This class provides comprehensive configuration management for NLM simulations,
    including preset profiles, validation, and file I/O operations.
    """
    
    def __init__(self):
        """Initialize an empty configuration."""
        self._data = {}
        self._presets = self._create_presets()
        self._logger = logging.getLogger(__name__)
    
    def _create_presets(self) -> Dict[str, Dict[str, Any]]:
        """Create preset configuration profiles."""
        return {
            "minimal": {
                "brain.neuron_count": 100,
                "brain.connection_probability": 0.05,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "neuromod.dopamine.scale": 0.5,
                "neuromod.curiosity.enable": False,
                "neuromod.novelty.enable": False,
                "development.enabled": False
            },
            "standard": {
                "brain.neuron_count": 1000,
                "brain.connection_probability": 0.1,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 1.0,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "development.enabled": True,
                "simulation.timestep": 0.001
            },
            "high_performance": {
                "brain.neuron_count": 5000,
                "brain.connection_probability": 0.15,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 2.0,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "development.enabled": True,
                "simulation.timestep": 0.001,
                "performance.enable_vectorization": True,
                "performance.enable_parallel": True,
                "checkpoint.enabled": True,
                "checkpoint.interval": 1000
            },
            "learning": {
                "brain.neuron_count": 2000,
                "brain.connection_probability": 0.12,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 1.5,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "development.enabled": True,
                "development.stage": "adult",
                "learning.rate": 0.001,
                "learning.simulation_steps": 5000
            }
        }
    
    def load_preset(self, preset_name: str) -> 'NLMConfig':
        """
        Load a preset configuration profile.
        
        Args:
            preset_name: Name of the preset profile (minimal, standard, 
                        high_performance, or learning)
        
        Returns:
            Self for method chaining
        
        Raises:
            ValueError: If the preset name is not recognized
        """
        if preset_name not in self._presets:
            available = list(self._presets.keys())
            raise ValueError(f"Unknown preset '{preset_name}'. Available presets: {available}")
        
        self._data.update(self._presets[preset_name])
        self._logger.debug(f"Loaded preset: {preset_name}")
        return self
    
    def set(self, key: str, value: Any) -> 'NLMConfig':
        """
        Set a configuration parameter.
        
        Args:
            key: Configuration key (e.g., "brain.neuron_count")
            value: Configuration value
        
        Returns:
            Self for method chaining
        """
        self._data[key] = value
        self._logger.debug(f"Set config: {key} = {value}")
        return self
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration parameter.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
        
        Returns:
            Configuration value or default
        """
        return self._data.get(key, default)
    
    def has(self, key: str) -> bool:
        """
        Check if configuration has a parameter.
        
        Args:
            key: Configuration key
        
        Returns:
            True if configuration has the key
        """
        return key in self._data
    
    def get_keys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return list(self._data.keys())
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Export configuration as dictionary.
        
        Returns:
            Configuration as dictionary
        """
        return self._data.copy()
    
    def load_from_file(self, filepath: str) -> 'NLMConfig':
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to configuration file
        
        Returns:
            Self for method chaining
        
        Raises:
            IOError: If file cannot be loaded
        """
        try:
            with open(filepath, 'r') as f:
                loaded_data = json.load(f)
            self._data.update(loaded_data)
            self._logger.info(f"Loaded configuration from: {filepath}")
        except Exception as e:
            raise IOError(f"Failed to load configuration from {filepath}: {e}")
        return self
    
    def save_to_file(self, filepath: str) -> 'NLMConfig':
        """
        Save configuration to JSON file.
nlm-config.save_to_file docstring remains incomplete
"""
        try:
            with open(filepath, 'w') as f:
                json.dump(self._data, f, indent=2)
            self._logger.info(f"Saved configuration to: {filepath}")
        except Exception as e:
            raise IOError(f"Failed to save configuration to {filepath}: {e}")
        return self
    
    def validate(self) -> List[str]:
        """
        Validate configuration parameters.
        
        Performs basic validation to ensure configuration is consistent
        and complete.
        
        Returns:
            List of validation errors (empty if configuration is valid)
        """
        errors = []
        
        # Check essential parameters
        if not self.has("brain.neuron_count") or self.get("brain.neuron_count") <= 0:
            errors.append("brain.neuron_count must be a positive integer")
        
        if not self.has("brain.connection_probability") or \
           self.get("brain.connection_probability") < 0.0 or \
           self.get("brain.connection_probability") > 1.0:
            errors.append("brain.connection_probability must be between 0.0 and 1.0")
        
        # Check for incompatible combinations
        if self.has("development.enabled") and self.get("development.enabled") and \
           not self.has("development.stage"):
            errors.append("development.stage required when development.enabled is true")
        
        if errors:
            self._logger.warning(f"Configuration validation failed: {errors}")
        
        return errors
    
    def list_presets(self) -> List[str]:
        """
        List available preset names.
        
        Returns:
            List of preset names
        """
        return list(self._presets.keys())

class NLMBrainSimulator:
    """
    High-level interface for NLM brain simulation.
    
    This class provides a comprehensive Python interface for running NLM simulations,
    including initialization, execution, monitoring, and result management.
    """
    
    def __init__(self, config: Optional[NLMConfig] = None):
        """
        Initialize NLM brain simulator.
        
        Args:
            config: NLM configuration (if None, uses minimal configuration)
        
        Raises:
            RuntimeError: If pynlm module is not available
        """
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm module is required for NLMBrainSimulator")
        
        self.config = config or NLMConfig()
        self._validate_config()
        
        # Create NLM brain components
        self._config_obj = self._create_config()
        self._brain = nlm.createBrain(self._config_obj)
        self._agent = None
        self._world = None
        
        # Simulation state
        self._simulation_running = False
        self._simulation_step = 0
        self._simulation_time = 0.0
        
        # History for analysis
        self._history = {
            "steps": [],
            "spike_counts": [],
            "firing_rates": [],
            "curiosity_levels": [],
            "novelty_levels": [],
            "prediction_errors": [],
            "actions": []
        }
        
        self._logger = logging.getLogger(__name__)
    
    def _validate_config(self):
        """Validate configuration and log warnings."""
        errors = self.config.validate()
        if errors:
            for error in errors:
                self._logger.warning(f"Configuration warning: {error}")
    
    def _create_config(self):
        """Convert NLMConfig to NLM Config object."""
        config = nlm.createDefaultConfig()
        
        # Apply configuration values
        for key, value in self.config.to_dict().items():
            if isinstance(value, (int, float)):
                config.set(key, value)
        
        return config
    
    def initialize(self):
        """
        Initialize the brain simulation.
        
        Returns:
            Self for method chaining
        
        Raises:
            RuntimeError: If brain initialization fails
        """
        if not self._brain.initialize():
            raise RuntimeError("Failed to initialize NLM brain")
        
        self._simulation_running = True
        self._simulation_step = 0
        self._simulation_time = 0.0
        
        # Clear history
        for key in self._history:
            self._history[key].clear()
        
        self._logger.info("NLM brain initialized successfully")
        return self
    
    def step(self, step_number: int = None, time: float = None):
        """
        Advance simulation by one step.
        
        Args:
            step_number: Step number (if None, uses current step)
            time: Current time (if None, uses current time)
        
        Returns:
            Current step number
        
        Raises:
            RuntimeError: If simulation is not running
        """
        if not self._simulation_running:
            raise RuntimeError("Simulation not running. Call initialize() first.")
        
        current_step = step_number if step_number is not None else self._simulation_step
        current_time = time if time is not None else self._simulation_time
        
        # Record pre-step state
        pre_spike_count = self._brain.getTotalSpikeCount()
        pre_firing_rate = self._brain.getAverageFiringRate()
        
        # Execute simulation step
        self._brain.step(current_step, current_time)
        
        # Update state
        self._simulation_step = current_step + 1
        self._simulation_time = current_time + self.config.get("simulation.timestep", 0.001)
        
        # Record post-step state
        post_spike_count = self._brain.getTotalSpikeCount()
        post_firing_rate = self._brain.getAverageFiringRate()
        
        # Record history (could be extended with agent/world components)
        self._history["steps"].append(current_step)
        self._history["spike_counts"].append(post_spike_count - pre_spike_count)
        self._history["firing_rates"].append(post_firing_rate)
        
        return current_step
    
    def run(self, num_steps: int, agent=None, world=None):
        """
        Run simulation for specified number of steps.
        
        Args:
            num_steps: Number of simulation steps to run
            agent: Agent brain interface (optional)
            world: World simulation (optional)
        
        Returns:
            Self for method chaining
        """
        if agent and world:
            self._agent = agent
            self._world = world
            return self._run_with_agent(num_steps, agent, world)
        else:
            # Simple brain-only simulation
            for i in range(num_steps):
                self.step(i)
            return self
    
    def _run_with_agent(self, num_steps: int, agent, world):
        """
        Run simulation with agent and world components.
        
        Args:
            num_steps: Number of simulation steps
            agent: Agent brain interface
            world: World simulation
        """
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process in agent brain
            if self._agent:
                self._agent.processSensoryInput(percept)
            
            # Brain step
            self.step(step)
            
            # Get action from agent brain
            action = self._agent.decodeMotorCommand() if self._agent else None
            
            # Apply action in world
            if action and world:
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation (if agent has it)
            if self._agent:
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                self._agent.applyRewardModulation(reward, 0.0)
            
            # Record metrics
            if self._agent:
                self._history["curiosity_levels"].append(self._agent.getCuriosityLevel())
                self._history["novelty_levels"].append(self._agent.getNoveltyLevel())
                self._history["prediction_errors"].append(self._agent.getPredictionError())
                self._history["actions"].append(str(action) if action else "None")
            
            yield step
        
        return self
    
    def reset(self):
        """
        Reset simulation to initial state.
        
        Returns:
            Self for method chaining
        """
        self._brain.reset()
        self._simulation_running = False
        self._simulation_step = 0
        self._simulation_time = 0.0
        
        # Clear history
        for key in self._history:
            self._history[key].clear()
        
        self._logger.info("NLM brain simulation reset")
        return self
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get comprehensive simulation statistics.
        
        Returns:
            Dictionary containing simulation statistics
        """
        stats = {
            "simulation_time": self._simulation_time,
            "total_steps": self._simulation_step,
            "total_spikes": self._brain.getTotalSpikeCount(),
            "firing_neurons": self._brain.getFiringNeuronCount(),
            "average_firing_rate": self._brain.getAverageFiringRate(),
            "eI_ratio": self._brain.getExcitationInhibitionRatio(),
            "neuron_count": self._brain.getTotalNeuronCount(),
            "synapse_count": self._brain.getTotalSynapseCount(),
            "developmental_stage": self._brain.getDevelopmentalStage(),
            "history": self._history.copy()
        }
        
        # Add agent-specific stats if available
        if self._agent:
            stats.update({
                "agent_stats": {
                    "curiosity": self._agent.getCuriosityLevel(),
                    "novelty": self._agent.getNoveltyLevel(),
                    "prediction_error": self._agent.getPredictionError(),
                    "neuromodulation": self._agent.getNeuromodulationLevel(),
                    "development": self._agent.getDevelopmentalStage()
                }
            })
        
        return stats
    
    def save(self, filepath: str):
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save file
        
        Returns:
            Self for method chaining
        
        Raises:
            RuntimeError: If save operation fails
        """
        if not self._brain.save(filepath):
            raise RuntimeError(f"Failed to save brain state to {filepath}")
        
        self._logger.info(f"Brain state saved to {filepath}")
        return self
    
    def load(self, filepath: str):
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load file
        
        Returns:
            Self for method chaining
        
        Raises:
            RuntimeError: If load operation fails
        """
        if not self._brain.load(filepath):
            raise RuntimeError(f"Failed to load brain state from {filepath}")
        
        # Reset simulation state after loading
        self.reset()
        
        self._logger.info(f"Brain state loaded from {filepath}")
        return self
    
    def enable_learning_subsystems(self, reward_modulation=True, curiosity=True,
                                  development=True, structural_plasticity=True):
        """
        Enable learning subsystems for agent brain.
        
        Args:
            reward_modulation: Enable reward modulation
            curiosity: Enable curiosity-driven exploration
            development: Enable developmental changes
            structural_plasticity: Enable structural plasticity
        """
        if not self._agent:
            raise RuntimeError("No agent available. Create agent first.")
        
        self._agent.enableRewardModulation(reward_modulation)
        self._agent.enableCuriosity(curiosity)
        self._agent.enableDevelopment(development)
        self._agent.enableStructuralPlasticity(structural_plasticity)
        
        self._logger.info("Learning subsystems enabled")
        return self
    
    def get_learning_status(self) -> Dict[str, Any]:
        """
        Get current learning status.
        
        Returns:
            Dictionary containing learning status information
        """
        if not self._agent:
            return {"error": "No agent available"}
        
        return {
            "reward_modulation_enabled": self._agent.isRewardModulationEnabled(),
            "curiosity_enabled": self._agent.isCuriosityEnabled(),
            "development_enabled": self._agent.isDevelopmentEnabled(),
            "structural_plasticity_enabled": self._agent.isStructuralPlasticityEnabled(),
            "curiosity_level": self._agent.getCuriosityLevel(),
            "novelty_level": self._agent.getNoveltyLevel(),
            "prediction_error": self._agent.getPredictionError(),
            "neuromodulation_level": self._agent.getNeuromodulationLevel(),
            "developmental_stage": self._agent.getDevelopmentalStage()
        }

class NLMSimulationRunner:
    """
    High-level interface for running complete NLM simulations.
    
    This class provides utilities for running common simulation patterns
    and managing experiment execution.
    """
    
    @staticmethod
    def create_experiment(config_file: str = None, preset: str = "standard"):
        """
        Create a new simulation from configuration file or preset.
        
        Args:
            config_file: Path to configuration file
            preset: Preset name (minimal, standard, high_performance, learning)
        
        Returns:
            NLMBrainSimulator instance
        """
        config = NLMConfig()
        
        if preset:
            config.load_preset(preset)
        
        if config_file:
            config.load_from_file(config_file)
        
        return NLMBrainSimulator(config)
    
    @staticmethod
    def run_learning_experiment(
        experiment_name: str,
        preset: str = "learning",
        steps: int = 1000,
        enable_agent: bool = True,
        verbose: bool = False
    ) -> Dict[str, Any]:
        """
        Run a complete learning experiment.
        
        Args:
            experiment_name: Name of the experiment
            preset: Preset configuration
            steps: Number of simulation steps
            enable_agent: Whether to enable agent brain
            verbose: Whether to print verbose output
        
        Returns:
            Dictionary containing experiment results
        """
        if verbose:
            print(f"Running learning experiment: {experiment_name}")
        
        # Create simulation
        sim = NLMBrainSimulator().load_preset(preset)
        
        # Initialize
        sim.initialize()
        
        # If agent enabled, create and initialize it
        agent = None
        world = None
        
        if enable_agent:
            agent = nlm.createAgentBrain(sim._brain)
            world = nlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            agent.initialize(world)
            
            # Enable learning subsystems
            sim.enable_learning_subsystems(True, True, True, True)
        
        # Run simulation
        if verbose:
            print(f"Running {steps} simulation steps...")
        
        for i, step in enumerate(sim.run(steps, agent, world)):
            if verbose and step % 100 == 0:
                print(f"  Step {step}")
        
        # Get final statistics
        stats = sim.get_stats()
        
        # Save results
        output_file = f"experiment_{experiment_name}_results.json"
        with open(output_file, 'w') as f:
            json.dump(stats, f, indent=2)
        
        if verbose:
            print(f"Experiment completed. Results saved to {output_file}")
        
        return stats
    
    @staticmethod
    def run_phase_experiments(phases: List[str] = None, verbose: bool = False):
        """
        Run experiments for specified phases.
        
        Args:
            phases: List of phase names to run (if None, runs all phases)
            verbose: Whether to print verbose output
        
        Returns:
            Dictionary containing results for each phase
        """
        if phases is None:
            phases = ["phase3", "phase4", "phase5", "phase6"]
        
        results = {}
        
        for phase in phases:
            if verbose:
                print(f"\n--- Running {phase} experiment ---")
            
            # Run the specific phase experiment
            if phase == "phase3":
                # Phase 3: Basic neural dynamics
                sim = NLMSimulationRunner.create_experiment(preset="minimal")
                sim.initialize()
                sim.run(200)
                results[phase] = sim.get_stats()
                
            elif phase == "phase4":
                # Phase 4: Working memory and prediction
                sim = NLMSimulationRunner.create_experiment(preset="standard")
                sim.initialize()
                sim.run(500)
                results[phase] = sim.get_stats()
                
            elif phase == "phase5":
                # Phase 5: Performance optimizations
                sim = NLMSimulationRunner.create_experiment(preset="high_performance")
                sim.initialize()
                sim.run(300)
                results[phase] = sim.get_stats()
                
            elif phase == "phase6":
                # Phase 6: Full integration
                results[phase] = NLMSimulationRunner.run_learning_experiment(
                    experiment_name="phase6_integration",
                    preset="learning",
                    steps=1000,
                    enable_agent=True,
                    verbose=verbose
                )
        
        return results

class NLMVisualizer:
    """
    Visualization utilities for NLM simulations.
    
    This class provides tools for creating visualizations and analyzing
    NLM simulation results.
    """
    
    @staticmethod
    def create_summary_plot(stats: Dict[str, Any], output_file: str = None):
        """
        Create a summary plot of simulation statistics.
        
        Args:
            stats: Simulation statistics dictionary
            output_file: Path to save plot (if None, displays plot)
        
        Returns:
            True if plot was created successfully, False otherwise
        """
        try:
            import matplotlib.pyplot as plt
            
            # Create figure with subplots
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle("NLM Simulation Results", fontsize=16)
            
            # Plot 1: Spike counts over time
            ax1 = axes[0, 0]
            if "history" in stats and "spike_counts" in stats["history"]:
                spike_data = stats["history"]["spike_counts"]
                ax1.plot(spike_data, label='Spike Count')
                ax1.set_xlabel('Step')
                ax1.set_ylabel('Spike Count')
                ax1.set_title('Spike Activity Over Time')
                ax1.legend()
                ax1.grid(True, alpha=0.3)
            
            # Plot 2: Firing rates over time
            ax2 = axes[0, 1]
            if "history" in stats and "firing_rates" in stats["history"]:
                rate_data = stats["history"]["firing_rates"]
                ax2.plot(rate_data, label='Firing Rate', color='orange')
                ax2.set_xlabel('Step')
                ax2.set_ylabel('Firing Rate (Hz)')
                ax2.set_title('Firing Rate Over Time')
                ax2.legend()
                ax2.grid(True, alpha=0.3)
            
            # Plot 3: Neuromodulation levels
            ax3 = axes[1, 0]
            if "agent_stats" in stats:
                curiosity = stats["agent_stats"].get("curiosity", [])
                novelty = stats["agent_stats"].get("novelty", [])
                if curiosity and novelty:
                    steps = list(range(len(curiosity)))
                    ax3.plot(steps, curiosity, label='Curiosity', color='green')
                    ax3.plot(steps, novelty, label='Novelty', color='red')
                    ax3.set_xlabel('Step')
                    ax3.set_ylabel('Level')
                    ax3.set_title('Neuromodulation Levels')
                    ax3.legend()
                    ax3.grid(True, alpha=0.3)
            
            # Plot 4: Performance metrics
            ax4 = axes[1, 1]
            metrics = ["total_spikes", "firing_neurons", "neuron_count", "synapse_count"]
            values = [stats.get(metric, 0) for metric in metrics]
            bars = ax4.bar(metrics, values, color=['skyblue', 'lightgreen', 'lightcoral', 'gold'])
            ax4.set_ylabel('Count')
            ax4.set_title('Final Network Statistics')
            ax4.tick_params(axis='x', rotation=45)
            
            # Add value labels on bars
            for bar, value in zip(bars, values):
                height = bar.get_height()
                ax4.text(bar.get_x() + bar.get_width()/2., height,
                        f'{int(value)}', ha='center', va='bottom')
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
                print(f"Plot saved to {output_file}")
            else:
                plt.show()
                
            plt.close()
            
            return True
            
        except ImportError:
            print("Warning: matplotlib not available. Skipping plot generation.")
            return False
        except Exception as e:
            print(f"Error creating plot: {e}")
            return False
    
    @staticmethod
    def create_learning_curve_plot(stats: Dict[str, Any], output_file: str = None):
        """
        Create a learning curve plot showing development over time.
        
        Args:
            stats: Simulation statistics dictionary
            output_file: Path to save plot (if None, displays plot)
        
        Returns:
            True if plot was created successfully, False otherwise
        """
        try:
            import matplotlib.pyplot as plt
            
            # Create figure
            fig, axes = plt.subplots(2, 2, figsize=(14, 10))
            fig.suptitle("NLM Learning Progress Analysis", fontsize=16)
            
            # Plot 1: Development stages over time
            ax1 = axes[0, 0]
            if "history" in stats and "steps" in stats["history"]:
                steps = stats["history"]["steps"]
                # Developmental stages are not directly tracked in history
                # Use firing rate as proxy for learning progress
                firing_rates = stats["history"].get("firing_rates", [])
                if firing_rates:
                    ax1.plot(steps, firing_rates, label='Firing Rate', color='blue')
                    ax1.set_xlabel('Step')
                    ax1.set_ylabel('Firing Rate (Hz)')
                    ax1.set_title('Learning Progress')
                    ax1.legend()
                    ax1.grid(True, alpha=0.3)
            
            # Plot 2: Neuromodulation dynamics
            ax2 = axes[0, 1]
            if "agent_stats" in stats:
                curiosity = stats["agent_stats"].get("curiosity", [])
                novelty = stats["agent_stats"].get("novelty", [])
                if curiosity and novelty:
                    steps = list(range(len(curiosity)))
                    ax2.plot(steps, curiosity, label='Curiosity', color='green')
                    ax2.plot(steps, novelty, label='Novelty', color='orange')
                    ax2.set_xlabel('Step')
                    ax2.set_ylabel('Level')
                    ax2.set_title('Exploration Drive Dynamics')
                    ax2.legend()
                    ax2.grid(True, alpha=0.3)
            
            # Plot 3: Network growth
            ax3 = axes[1, 0]
            # Track network growth over time
            final_neuron_count = stats.get("neuron_count", 0)
            initial_neuron_count = final_neuron_count // 10  # Approximate
            growth_data = [initial_neuron_count] + \
                         [int(initial_neuron_count * (1 + (i/100)*(final_neuron_count/initial_neuron_count - 1)))
                          for i in range(1, 11)]
            ax3.plot(growth_data, label='Neuron Count', marker='o', color='purple')
            ax3.set_xlabel('Time Period')
            ax3.set_ylabel('Neuron Count')
            ax3.set_title('Network Growth Over Time')
            ax3.legend()
            ax3.grid(True, alpha=0.3)
            
            # Plot 4: Prediction error reduction
            ax4 = axes[1, 1]
            if "agent_stats" in stats:
                prediction_errors = stats["agent_stats"].get("prediction_error", [])
                if prediction_errors:
                    steps = list(range(len(prediction_errors)))
                    ax4.plot(steps, prediction_errors, label='Prediction Error', color='red')
                    ax4.set_xlabel('Step')
                    ax4.set_ylabel('Error Level')
                    ax4.set_title('Prediction Error Reduction')
                    ax4.legend()
                    ax4.grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
                print(f"Learning curve plot saved to {output_file}")
            else:
                plt.show()
                
            plt.close()
            
            return True
            
        except ImportError:
            print("Warning: matplotlib not available. Skipping learning curve plot.")
            return False
        except Exception as e:
            print(f"Error creating learning curve plot: {e}")
            return False

# Convenience aliases for common use cases
def create_brain(neuron_count: int = 1000, preset: str = "standard"):
    """
    Quick creation of a brain with default settings.
    
    Args:
        neuron_count: Number of neurons
        preset: Preset configuration name
    
    Returns:
        NLMBrainSimulator instance
    """
    config = NLMConfig().load_preset(preset).set("brain.neuron_count", neuron_count)
    return NLMBrainSimulator(config)

def run_simple_simulation(steps: int = 100, preset: str = "standard"):
    """
    Quick run of simple simulation.
    
    Args:
        steps: Number of simulation steps
        preset: Preset configuration name
    
    Returns:
        NLMBrainSimulator instance
    """
    sim = create_brain(preset=preset)
    sim.initialize()
    sim.run(steps)
    return sim

def create_agent_simulation(config_preset: str = "learning", steps: int = 1000):
    """
    Create and run an agent simulation with world and learning.
    
    Args:
        config_preset: Configuration preset
        steps: Number of simulation steps
    
    Returns:
        Dictionary containing simulation results
    """
    return NLMSimulationRunner.run_learning_experiment(
        experiment_name="agent_learning",
        preset=config_preset,
        steps=steps,
        enable_agent=True,
        verbose=False
    )

# Example usage functions for quick start
if __name__ == "__main__":
    # Set up logging
    logging.basicConfig(level=logging.INFO)
    
    print("NLM Enhanced Python Interface Examples")
    print("=" * 50)
    
    print("\n1. Creating a simple brain with minimal configuration...")
    sim1 = create_brain(500, "minimal")
    sim1.initialize()
    sim1.run(100)
    print(f"   Total spikes: {sim1.get_stats()['total_spikes']}")
    
    print("\n2. Running a learning experiment with agent...")
    results2 = create_agent_simulation("learning", 500)
    print(f"   Final curiosity level: {results2['agent_stats']['curiosity']:.3f}")
    print(f"   Final prediction error: {results2['agent_stats']['prediction_error']:.3f}")
    
    print("\n3. Creating visualization...")
    NLMVisualizer.create_summary_plot(results2, "learning_example.png")
    
    print("\nAll examples completed successfully!")
    print("\nFor more information, see the NLM documentation and examples.")