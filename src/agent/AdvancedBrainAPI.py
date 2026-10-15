"""
NLM - Enhanced Python API for Neural Learning Machine

This module provides advanced functionality for NLM users, including:
- Enhanced configuration options
- Advanced memory management
- Prediction system integration
- Development tracking
- Performance monitoring
- Experiment management
"""

import numpy as np
import json
import time
from typing import Dict, List, Optional, Union, Any
from dataclasses import dataclass, field

# Try to import the main pynlm module
try:
    import pynlm
    _pynlm_available = True
except ImportError:
    _pynlm_available = False
    print("Warning: pynlm module not available. Enhanced features will be limited.")

@dataclass
class AdvancedConfig:
    """Advanced configuration for NLM with enhanced options"""
    # Core brain parameters
    neuron_count: int = 1000
    region_count: int = 2
    connection_probability: float = 0.1
    simulation_timestep: float = 0.001
    random_seed: int = 42
    
    # Memory system parameters
    enable_working_memory: bool = True
    enable_episodic_memory: bool = True
    enable_associative_memory: bool = True
    working_memory_capacity: int = 1000
    episodic_memory_limit: int = 100
    
    # Prediction system parameters
    enable_prediction: bool = True
    prediction_horizon: int = 10
    prediction_error_weight: float = 0.5
    confidence_threshold: float = 0.7
    
    # Neuromodulation parameters
    dopamine_baseline: float = 0.1
    curiosity_threshold: float = 0.3
    novelty_decay: float = 0.99
    
    # Development parameters
    enable_development: bool = True
    critical_period_duration: float = 60.0
    maturation_age: float = 300.0
    structural_plasticity_enabled: bool = True
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Performance parameters
    enable_memory_pools: bool = True
    enable_event_queues: bool = True
    enable_sparse_connectivity: bool = True
    enable_simd_optimization: bool = True
    enable_parallel_processing: bool = False
    
    # Advanced features
    enable_curiosity_driven: bool = True
    enable_novelty_detection: bool = True
    enable_attention: bool = True
    enable_planning: bool = True
    enable_social_learning: bool = True
    enable_self_model: bool = True
    
    # Persistence
    enable_checkpointing: bool = True
    checkpoint_interval: int = 1000
    max_checkpoints: int = 10
    
    # Visualization
    enable_visualization: bool = False
    visualization_fps: int = 30
    visualization_mode: str = "neural"
    
    # Experiment tracking
    experiment_tracking: bool = True
    auto_save_metrics: bool = True
    metric_log_interval: int = 100
    
    # Logging and monitoring
    log_level: str = "INFO"
    enable_performance_profiling: bool = False
    profile_interval: int = 1000
    enable_memory_monitoring: bool = True
    
    # Environment integration
    enable_world_simulation: bool = True
    world_width: int = 20
    world_height: int = 20
    world_vision_width: int = 8
    world_vision_height: int = 8
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary"""
        return {
            "brain": {
                "neuron_count": self.neuron_count,
                "region_count": self.region_count,
                "connection_probability": self.connection_probability,
                "simulation_timestep": self.simulation_timestep,
                "random_seed": self.random_seed,
            },
            "memory": {
                "enable_working_memory": self.enable_working_memory,
                "enable_episodic_memory": self.enable_episodic_memory,
                "enable_associative_memory": self.enable_associative_memory,
                "working_memory_capacity": self.working_memory_capacity,
                "episodic_memory_limit": self.episodic_memory_limit,
            },
            "prediction": {
                "enable_prediction": self.enable_prediction,
                "prediction_horizon": self.prediction_horizon,
                "prediction_error_weight": self.prediction_error_weight,
                "confidence_threshold": self.confidence_threshold,
            },
            "neuromodulation": {
                "dopamine_baseline": self.dopamine_baseline,
                "curiosity_threshold": self.curiosity_threshold,
                "novelty_decay": self.novelty_decay,
            },
            "development": {
                "enable_development": self.enable_development,
                "critical_period_duration": self.critical_period_duration,
                "maturation_age": self.maturation_age,
                "structural_plasticity_enabled": self.structural_plasticity_enabled,
                "synaptogenesis_rate": self.synaptogenesis_rate,
                "pruning_rate": self.pruning_rate,
            },
            "performance": {
                "enable_memory_pools": self.enable_memory_pools,
                "enable_event_queues": self.enable_event_queues,
                "enable_sparse_connectivity": self.enable_sparse_connectivity,
                "enable_simd_optimization": self.enable_simd_optimization,
                "enable_parallel_processing": self.enable_parallel_processing,
            },
            "advanced_features": {
                "enable_curiosity_driven": self.enable_curiosity_driven,
                "enable_novelty_detection": self.enable_novelty_detection,
                "enable_attention": self.enable_attention,
                "enable_planning": self.enable_planning,
                "enable_social_learning": self.enable_social_learning,
                "enable_self_model": self.enable_self_model,
            },
            "persistence": {
                "enable_checkpointing": self.enable_checkpointing,
                "checkpoint_interval": self.checkpoint_interval,
                "max_checkpoints": self.max_checkpoints,
            },
            "visualization": {
                "enable_visualization": self.enable_visualization,
                "visualization_fps": self.visualization_fps,
                "visualization_mode": self.visualization_mode,
            },
            "experiment": {
                "experiment_tracking": self.experiment_tracking,
                "auto_save_metrics": self.auto_save_metrics,
                "metric_log_interval": self.metric_log_interval,
            },
            "logging": {
                "log_level": self.log_level,
                "enable_performance_profiling": self.enable_performance_profiling,
                "profile_interval": self.profile_interval,
                "enable_memory_monitoring": self.enable_memory_monitoring,
            },
            "world": {
                "enable_world_simulation": self.enable_world_simulation,
                "world_width": self.world_width,
                "world_height": self.world_height,
                "world_vision_width": self.world_vision_width,
                "world_vision_height": self.world_vision_height,
            },
        }
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> "AdvancedConfig":
        """Create configuration from dictionary"""
        # Flatten nested dictionary for direct attribute assignment
        flat_dict = {}
        for section, values in config_dict.items():
            for key, value in values.items():
                flat_dict[f"{section}_{key}" if section != "advanced_features" else key] = value
        
        # Filter out keys that don't exist in the dataclass
        valid_keys = {f.name for f in cls.__dataclass_fields__.values()}
        filtered_dict = {k: v for k, v in flat_dict.items() if k in valid_keys}
        
        return cls(**filtered_dict)


class AdvancedBrain:
    """Advanced brain interface with enhanced functionality"""
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        self.config = config or AdvancedConfig()
        self._brain = None
        self._metrics = {}
        self._experiment_data = {}
        self._memory_stats = {}
        self._performance_data = {}
        self._is_initialized = False
        
        # Initialize metrics tracking
        self._metrics = {
            "neurons_created": 0,
            "synapses_created": 0,
            "spikes_generated": 0,
            "learning_events": 0,
            "memory_stores": 0,
            "prediction_attempts": 0,
            "prediction_correct": 0,
            "curiosity_events": 0,
            "novelty_events": 0,
            "development_stages": [],
            "performance_samples": [],
            "memory_usage": [],
            "cpu_usage": [],
        }
    
    def initialize(self) -> bool:
        """Initialize the brain with advanced configuration"""
        if not _pynlm_available:
            raise RuntimeError("pynlm module is required for advanced brain functionality")
        
        # Convert AdvancedConfig to pynlm config format
        pynlm_config = pynlm.createDefaultConfig()
        
        # Apply advanced configuration
        brain_config = self.config.to_dict()
        
        # Core brain settings
        pynlm_config.set("brain.neuron_count", brain_config["brain"]["neuron_count"])
        pynlm_config.set("brain.region_count", brain_config["brain"]["region_count"])
        pynlm_config.set("brain.connection_probability", brain_config["brain"]["connection_probability"])
        pynlm_config.set("simulation_timestep", brain_config["brain"]["simulation_timestep"])
        pynlm_config.set("random_seed", brain_config["brain"]["random_seed"])
        
        # Plasticity settings
        pynlm_config.set("plasticity.learning_rate", 0.01)
        pynlm_config.set("plasticity.stdp.enable", True)
        pynlm_config.set("plasticity.stdp.learning_rate", 0.001)
        pynlm_config.set("plasticity.hebbian.enable", True)
        
        # Neuromodulation settings
        pynlm_config.set("neuromod.dopamine.scale", brain_config["neuromodulation"]["dopamine_baseline"])
        pynlm_config.set("neuromod.curiosity.enable", brain_config["advanced_features"]["enable_curiosity_driven"])
        pynlm_config.set("neuromod.novelty.enable", brain_config["advanced_features"]["enable_novelty_detection"])
        
        # Development settings
        pynlm_config.set("development.synaptogenesis_rate", brain_config["development"]["synaptogenesis_rate"])
        pynlm_config.set("development.pruning_rate", brain_config["development"]["pruning_rate"])
        
        # Performance settings
        if brain_config["performance"]["enable_memory_pools"]:
            pynlm_config.set("performance.memory_pool_size", 1000)
        
        # Create and initialize brain
        self._brain = pynlm.createBrain(pynlm_config)
        self._brain.initialize()
        
        # Initialize agent if configured
        if self.config.enable_world_simulation:
            from world import SimpleWorld
            self._world = SimpleWorld()
            self._world.configure(
                width=self.config.world_width,
                height=self.config.world_height,
                visionWidth=self.config.world_vision_width,
                visionHeight=self.config.world_vision_height
            )
            self._agent = pynlm.createAgentBrain(self._brain)
            self._agent.initialize(self._world)
        
        # Enable advanced subsystems
        self._enable_advanced_subsystems()
        
        self._is_initialized = True
        
        # Start metrics collection
        if self.config.experiment_tracking:
            self._start_metrics_collection()
        
        return True
    
    def _enable_advanced_subsystems(self):
        """Enable advanced cognitive and memory subsystems"""
        if not self._brain or not self._agent:
            return
        
        # Enable advanced agent features
        self._agent.enableRewardModulation(self.config.advanced_features["enable_curiosity_driven"])
        self._agent.enableStructuralPlasticity(self.config.development["structural_plasticity_enabled"])
        self._agent.enableDevelopment(self.config.advanced_features["enable_curiosity_driven"])
        self._agent.enableCuriosity(self.config.advanced_features["enable_curiosity_driven"])
        
        # Set development stages
        if self.config.development["enable_development"]:
            # Enable developmental stages
            pass
    
    def _start_metrics_collection(self):
        """Start collecting performance metrics"""
        # Start background metrics collection thread
        import threading
        self._metrics_thread = threading.Thread(target=self._collect_metrics, daemon=True)
        self._metrics_thread.start()
    
    def _collect_metrics(self):
        """Collect performance and neural metrics"""
        while self._is_initialized:
            try:
                # Neural metrics
                self._metrics["neurons_created"] = self._brain.getTotalNeuronCount()
                self._metrics["synapses_created"] = self._brain.getTotalSynapseCount()
                self._metrics["spikes_generated"] = self._brain.getTotalSpikeCount()
                self._metrics["firing_neurons"] = self._brain.getFiringNeuronCount()
                self._metrics["average_firing_rate"] = self._brain.getAverageFiringRate()
                
                # Memory metrics
                if self.config.memory["enable_working_memory"]:
                    # Add working memory metrics
                    pass
                
                # Performance metrics
                if self.config.performance["enable_performance_profiling"]:
                    import psutil
                    process = psutil.Process()
                    self._metrics["cpu_usage"].append(process.cpu_percent())
                    self._metrics["memory_usage"].append(process.memory_info().rss / (1024 * 1024))  # MB
                
                # Development metrics
                if self.config.development["enable_development"]:
                    dev_stage = self._brain.getDevelopmentalStage()
                    self._metrics["development_stages"].append((time.time(), dev_stage.value))
                
                time.sleep(1.0)  # Collect metrics every second
                
            except Exception as e:
                # Log error but continue
                pass
    
    def run_advanced_simulation(self, num_steps: int = 1000, 
                               enable_checkpointing: bool = None) -> Dict[str, Any]:
        """Run advanced simulation with enhanced monitoring"""
        if not self._is_initialized:
            raise RuntimeError("Brain not initialized. Call initialize() first.")
        
        checkpoint_interval = enable_checkpointing if enable_checkpointing is not None else self.config.checkpoint_interval
        
        simulation_results = {
            "steps": [],
            "metrics": self._metrics.copy(),
            "checkpoints": [],
            "events": [],
            "memory_usage": [],
            "performance_samples": [],
            "development_timeline": [],
        }
        
        for step in range(num_steps):
            step_start_time = time.time()
            
            # Update world if enabled
            if self.config.enable_world_simulation and self._world:
                self._world.update(0.1)
                
                # Process sensory input
                percept = self._world.getSensoryPercept()
                self._agent.processSensoryInput(percept)
                
                # Apply motor command
                action = self._agent.decodeMotorCommand()
                self._world.applyMotorCommand(action, self._world.getSimulationTime())
            
            # Run brain step
            self._brain.step(step)
            
            # Record metrics
            step_metrics = {
                "step": step,
                "timestamp": time.time(),
                "neurons_firing": self._brain.getFiringNeuronCount(),
                "total_spikes": self._brain.getTotalSpikeCount(),
                "average_firing_rate": self._brain.getAverageFiringRate(),
                "development_stage": self._agent.getDevelopmentalStage().value if self._agent else None,
                "curiosity_level": self._agent.getCuriosityLevel() if self._agent else 0.0,
                "novelty_level": self._agent.getNoveltyLevel() if self._agent else 0.0,
                "prediction_error": self._agent.getPredictionError() if self._agent else 0.0,
            }
            
            simulation_results["steps"].append(step_metrics)
            
            # Save checkpoint if needed
            if checkpoint_interval > 0 and step % checkpoint_interval == 0 and self.config.enable_checkpointing:
                checkpoint_path = f"checkpoint_step_{step}.bin"
                if self._brain.save(checkpoint_path):
                    simulation_results["checkpoints"].append({
                        "step": step,
                        "path": checkpoint_path,
                        "size": self._get_checkpoint_size(checkpoint_path),
                    })
            
            # Collect performance data
            if step % self.config.metric_log_interval == 0:
                if self.config.experiment_tracking:
                    simulation_results["performance_samples"].append({
                        "step": step,
                        "cpu_usage": np.mean(self._metrics["cpu_usage"]) if self._metrics["cpu_usage"] else 0.0,
                        "memory_usage": np.mean(self._metrics["memory_usage"]) if self._metrics["memory_usage"] else 0.0,
                    })
            
            # Log development stage changes
            if step % 100 == 0 and self.config.development["enable_development"]:
                simulation_results["development_timeline"].append({
                    "step": step,
                    "stage": self._agent.getDevelopmentalStage().value if self._agent else None,
                    "plasticity_modifier": self._agent.getNeuromodulationLevel() if self._agent else 0.0,
                })
            
            # Record execution time
            step_duration = time.time() - step_start_time
            simulation_results["steps"][-1]["step_duration_ms"] = step_duration * 1000
        
        return simulation_results
    
    def _get_checkpoint_size(self, path: str) -> int:
        """Get size of checkpoint file in bytes"""
        try:
            return len(open(path, 'rb').read())
        except:
            return 0
    
    def export_metrics(self, filepath: str = None) -> str:
        """Export collected metrics to file"""
        if not self.config.experiment_tracking:
            raise RuntimeError("Experiment tracking is not enabled")
        
        # Prepare comprehensive metrics report
        report = {
            "summary": {
                "total_steps": len(self._metrics["steps"]) if "steps" in self._metrics else 0,
                "final_neurons": self._brain.getTotalNeuronCount(),
                "final_synapses": self._brain.getTotalSynapseCount(),
                "total_spikes": self._brain.getTotalSpikeCount(),
                "average_firing_rate": self._brain.getAverageFiringRate(),
            },
            "detailed_metrics": self._metrics,
            "memory_stats": self._memory_stats,
            "performance_data": self._performance_data,
        }
        
        if filepath:
            with open(filepath, 'w') as f:
                json.dump(report, f, indent=2, default=str)
            return filepath
        else:
            # Return as JSON string
            return json.dumps(report, indent=2, default=str)
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics"""
        if not self._is_initialized:
            raise RuntimeError("Brain not initialized")
        
        stats = {
            "basic_info": {
                "neuron_count": self._brain.getTotalNeuronCount(),
                "synapse_count": self._brain.getTotalSynapseCount(),
                "firing_neurons": self._brain.getFiringNeuronCount(),
                "average_firing_rate": self._brain.getAverageFiringRate(),
                "excitatory_inhibitory_ratio": self._brain.getExcitationInhibitionRatio(),
            },
            "advanced_stats": {
                "development_stage": self._agent.getDevelopmentalStage().value if self._agent else None,
                "neuromodulation_level": self._agent.getNeuromodulationLevel() if self._agent else 0.0,
                "curiosity_level": self._agent.getCuriosityLevel() if self._agent else 0.0,
                "novelty_level": self._agent.getNoveltyLevel() if self._agent else 0.0,
                "prediction_error": self._agent.getPredictionError() if self._agent else 0.0,
                "total_spikes": self._brain.getTotalSpikeCount(),
                "memory_metrics": self._memory_stats,
                "performance_metrics": self._performance_data,
            },
        }
        
        return stats
    
    def enable_feature(self, feature_name: str, parameters: Optional[Dict[str, Any]] = None):
        """Enable an advanced feature dynamically"""
        if not parameters:
            parameters = {}
        
        if feature_name == "working_memory" and self._agent:
            # Enable working memory
            pass
        elif feature_name == "episodic_memory" and self._agent:
            # Enable episodic memory
            pass
        elif feature_name == "prediction" and self._agent:
            # Enable prediction system
            pass
        elif feature_name == "curiosity" and self._agent:
            # Enable curiosity-driven exploration
            self._agent.enableCuriosity(True)
        elif feature_name == "novelty" and self._agent:
            # Enable novelty detection
            pass
        elif feature_name == "planning" and self._agent:
            # Enable planning
            pass
        elif feature_name == "social_learning" and self._agent:
            # Enable social learning
            pass
        elif feature_name == "self_model" and self._agent:
            # Enable self-model
            pass
        else:
            raise ValueError(f"Unknown feature: {feature_name}")
    
    def disable_feature(self, feature_name: str):
        """Disable an advanced feature dynamically"""
        if feature_name == "working_memory":
            # Disable working memory
            pass
        elif feature_name == "episodic_memory":
            # Disable episodic memory
            pass
        elif feature_name == "prediction":
            # Disable prediction system
            pass
        elif feature_name == "curiosity" and self._agent:
            self._agent.enableCuriosity(False)
        elif feature_name == "novelty":
            # Disable novelty detection
            pass
        elif feature_name == "planning":
            # Disable planning
            pass
        elif feature_name == "social_learning":
            # Disable social learning
            pass
        elif feature_name == "self_model":
            # Disable self-model
            pass
        else:
            raise ValueError(f"Unknown feature: {feature_name}")
    
    def get_feature_status(self) -> Dict[str, bool]:
        """Get status of all advanced features"""
        return {
            "working_memory": self.config.memory["enable_working_memory"],
            "episodic_memory": self.config.memory["enable_episodic_memory"],
            "associative_memory": self.config.memory["enable_associative_memory"],
            "prediction": self.config.prediction["enable_prediction"],
            "curiosity": self.config.advanced_features["enable_curiosity_driven"],
            "novelty": self.config.advanced_features["enable_novelty_detection"],
            "attention": self.config.advanced_features["enable_attention"],
            "planning": self.config.advanced_features["enable_planning"],
            "social_learning": self.config.advanced_features["enable_social_learning"],
            "self_model": self.config.advanced_features["enable_self_model"],
            "development": self.config.development["enable_development"],
            "checkpointing": self.config.persistence["enable_checkpointing"],
            "visualization": self.config.visualization["enable_visualization"],
            "experiment_tracking": self.config.experiment["experiment_tracking"],
            "performance_profiling": self.config.logging["enable_performance_profiling"],
        }
    
    def save_checkpoint(self, path: str = None) -> str:
        """Save brain state to checkpoint"""
        if not path:
            path = f"checkpoint_{int(time.time())}.bin"
        
        if self._brain.save(path):
            return path
        else:
            raise RuntimeError(f"Failed to save checkpoint to {path}")
    
    def load_checkpoint(self, path: str) -> bool:
        """Load brain state from checkpoint"""
        if self._brain.load(path):
            self._is_initialized = True
            return True
        else:
            raise RuntimeError(f"Failed to load checkpoint from {path}")
    
    def run_experiment(self, experiment_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run a specific experiment with given configuration"""
        if not self._is_initialized:
            raise RuntimeError("Brain not initialized")
        
        # Store current state
        current_config = self.config.to_dict()
        
        # Apply experiment configuration
        self._apply_experiment_config(experiment_config)
        
        # Run experiment
        num_steps = experiment_config.get("num_steps", 1000)
        experiment_results = self.run_advanced_simulation(num_steps)
        
        # Restore configuration
        self.config = AdvancedConfig.from_dict(current_config)
        self._reinitialize()
        
        return experiment_results
    
    def _apply_experiment_config(self, experiment_config: Dict[str, Any]):
        """Apply experiment-specific configuration"""
        # Temporarily modify configuration for this experiment
        pass
    
    def _reinitialize(self):
        """Reinitialize brain with current configuration"""
        self.initialize()
    
    def cleanup(self):
        """Clean up resources and stop metrics collection"""
        self._is_initialized = False
        if hasattr(self, '_metrics_thread'):
            self._metrics_thread.join(timeout=5.0)
        
        # Clean up
        if self._world:
            del self._world
        if self._agent:
            del self._agent
        if self._brain:
            del self._brain
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.cleanup()


# Factory functions for easy creation of advanced brains

def create_advanced_brain(config: Optional[AdvancedConfig] = None) -> AdvancedBrain:
    """Create an advanced brain with enhanced features"""
    return AdvancedBrain(config)


def create_experimental_brain(config_dict: Optional[Dict[str, Any]] = None) -> AdvancedBrain:
    """Create a brain optimized for experiments with given configuration dictionary"""
    if config_dict:
        config = AdvancedConfig.from_dict(config_dict)
    else:
        config = AdvancedConfig()
    
    return AdvancedBrain(config)


def create_developmental_brain(initial_neurons: int = 100, enable_development: bool = True) -> AdvancedBrain:
    """Create a brain specifically designed for developmental studies"""
    config = AdvancedConfig()
    config.neuron_count = initial_neurons
    config.development["enable_development"] = enable_development
    config.development["critical_period_duration"] = 120.0
    config.development["maturation_age"] = 480.0
    config.advanced_features["enable_curiosity_driven"] = True
    config.advanced_features["enable_novelty_detection"] = True
    
    return AdvancedBrain(config)


def create_social_brain(num_agents: int = 5) -> List[AdvancedBrain]:
    """Create multiple interacting agents for social learning experiments"""
    brains = []
    config = AdvancedConfig()
    config.advanced_features["enable_social_learning"] = True
    config.advanced_features["enable_self_model"] = True
    config.world["enable_world_simulation"] = True
    
    for i in range(num_agents):
        brain = AdvancedBrain(config)
        brain.initialize()
        brains.append(brain)
    
    return brains


def create_prediction_brain(prediction_horizon: int = 20, confidence_threshold: float = 0.8) -> AdvancedBrain:
    """Create a brain optimized for prediction and sequence learning"""
    config = AdvancedConfig()
    config.prediction["enable_prediction"] = True
    config.prediction["prediction_horizon"] = prediction_horizon
    config.prediction["confidence_threshold"] = confidence_threshold
    config.prediction["prediction_error_weight"] = 0.8
    config.advanced_features["enable_attention"] = True
    config.advanced_features["enable_planning"] = True
    
    return AdvancedBrain(config)


# Utility functions

def benchmark_brain(config: AdvancedConfig, num_steps: int = 1000) -> Dict[str, Any]:
    """Benchmark brain performance with given configuration"""
    start_time = time.time()
    
    brain = AdvancedBrain(config)
    brain.initialize()
    
    results = brain.run_advanced_simulation(num_steps)
    
    end_time = time.time()
    
    benchmark_results = {
        "execution_time": end_time - start_time,
        "steps_per_second": num_steps / (end_time - start_time),
        "final_metrics": brain.get_brain_stats(),
        "simulation_results": results,
    }
    
    brain.cleanup()
    return benchmark_results


def compare_configs(configs: List[AdvancedConfig], num_steps: int = 1000) -> Dict[str, Any]:
    """Compare multiple brain configurations"""
    results = {}
    
    for i, config in enumerate(configs):
        print(f"Benchmarking configuration {i+1}/{len(configs)}...")
        results[f"config_{i+1}"] = benchmark_brain(config, num_steps)
    
    return results


# Example usage and presets

def get_preset_config(preset_name: str) -> AdvancedConfig:
    """Get a preset configuration by name"""
    presets = {
        "minimal": lambda: AdvancedConfig(
            neuron_count=100,
            region_count=1,
            enable_prediction=False,
            enable_attention=False,
            enable_planning=False,
            enable_social_learning=False,
        ),
        "balanced": lambda: AdvancedConfig(
            neuron_count=500,
            region_count=2,
            enable_prediction=True,
            enable_attention=True,
            enable_planning=True,
            enable_social_learning=False,
        ),
        "advanced": lambda: AdvancedConfig(
            neuron_count=1000,
            region_count=4,
            enable_prediction=True,
            enable_attention=True,
            enable_planning=True,
            enable_social_learning=True,
            enable_self_model=True,
            enable_development=True,
        ),
        "developmental": create_developmental_brain(),
        "social": create_social_brain(),
        "prediction": create_prediction_brain(),
    }
    
    if preset_name not in presets:
        raise ValueError(f"Unknown preset: {preset_name}. Available presets: {list(presets.keys())}")
    
    return presets[preset_name]()


# Main execution example
if __name__ == "__main__":
    # Example: Create and run an advanced simulation
    print("NLM Advanced Python API Example")
    print("=" * 50)
    
    # Create advanced brain with balanced configuration
    brain = AdvancedBrain(get_preset_config("advanced"))
    
    print(f"Created brain with {brain.config.neuron_count} neurons")
    print(f"Enabled features: {brain.get_feature_status()}")
    
    # Initialize and run simulation
    if brain.initialize():
        print("Brain initialized successfully")
        
        # Run simulation
        print("Running simulation...")
        results = brain.run_advanced_simulation(500)
        
        # Get statistics
        stats = brain.get_brain_stats()
        print(f"Final neuron count: {stats['basic_info']['neuron_count']}")
        print(f"Final firing rate: {stats['basic_info']['average_firing_rate']:.3f} Hz")
        
        # Export results
        brain.export_metrics("simulation_results.json")
        print("Results exported to simulation_results.json")
        
        # Cleanup
        brain.cleanup()
        print("Brain cleanup completed")
    
    print("\nExample completed successfully!")
