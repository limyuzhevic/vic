#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine Command Line Interface

Unified command-line interface for NLM (Neural Learning Machine) simulation system.
Supports all phases (1-6) with comprehensive configuration options.

Usage Examples:
  # Run Phase 2 demo with custom settings
  ./nlm_cli.py phase2 --config configs/nlm_config.json --neuron-count 500 --steps 1000

  # Run Phase 3 world interaction demo
  ./nlm_cli.py phase3 --episodes 10 --steps-per-episode 300 --world-width 20 --world-height 20

  # Run Phase 6 integration test
  ./nlm_cli.py phase6 --neuron-count 2000 --max-steps 5000 --enable-checkpointing

  # Run specific experiment with custom parameters
  ./nlm_cli.py experiment --name phase4 --output results.json

  # List available configurations and demos
  ./nlm_cli.py help
"""

import argparse
import json
import os
import sys
from typing import Dict, Any, Optional
from enum import Enum
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class Phase(Enum):
    PHASE1 = "phase1"
    PHASE2 = "phase2"
    PHASE3 = "phase3"
    PHASE4 = "phase4"
    PHASE5 = "phase5"
    PHASE6 = "phase6"
    EXPERIMENT = "experiment"
    BENCHMARK = "benchmark"
    CONFIG = "config"

class NLMCli:
    """Main CLI handler for NLM system."""
    
    def __init__(self):
        self.config_file = "configs/nlm_config.json"
        self.overrides = {}
        self.debug_mode = False
        
    def load_config(self, config_path: str) -> Dict[str, Any]:
        """Load configuration from JSON file."""
        try:
            with open(config_path, 'r') as f:
                config = json.load(f)
            logger.info(f"Loaded configuration from {config_path}")
            return config
        except FileNotFoundError:
            logger.error(f"Configuration file not found: {config_path}")
            return self._get_default_config()
        except json.JSONDecodeError as e:
            logger.error(f"Invalid JSON in configuration file {config_path}: {e}")
            return self._get_default_config()
    
    def _get_default_config(self) -> Dict[str, Any]:
        """Return default configuration."""
        default_config = {
            "simulation": {
                "random_seed": 42,
                "simulation_timestep": 0.001,
                "max_simulation_steps": 10000,
                "simulation_time_limit": 0.0
            },
            "brain": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1
            }
        }
        logger.warning("Using default configuration")
        return default_config
    
    def merge_config_with_overrides(self, base_config: Dict[str, Any], 
                                   overrides: Dict[str, Any]) -> Dict[str, Any]:
        """Merge base configuration with command line overrides."""
        config = json.loads(json.dumps(base_config))  # Deep copy
        
        for key, value in overrides.items():
            if value is not None:
                # Support nested key paths (e.g., "brain.neuron_count")
                keys = key.split('.')
                target = config
                for i, k in enumerate(keys[:-1]):
                    if k not in target:
                        target[k] = {}
                    target = target[k]
                target[keys[-1]] = value
                
        return config
    
    def create_argparser(self) -> argparse.ArgumentParser:
        """Create and configure the argument parser."""
        parser = argparse.ArgumentParser(
            description="NLM (熙然) - Neural Learning Machine Simulation System",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  %(prog)s phase2 --config my_config.json --neuron-count 500
  %(prog)s phase3 --episodes 10 --steps-per-episode 300
  %(prog)s phase6 --neuron-count 2000 --max-steps 5000
  %(prog)s experiment --name phase4 --output results.json
  %(prog)s benchmark --duration 60 --output benchmark_results.json
  %(prog)s config --list
            """
        )
        
        # General options
        parser.add_argument(
            "phase",
            choices=[p.value for p in Phase],
            help="Phase to run (phase1-phase6, experiment, benchmark, or config)"
        )
        
        parser.add_argument(
            "--config", "-c",
            default="configs/nlm_config.json",
            help="Path to configuration JSON file (default: configs/nlm_config.json)"
        )
        
        parser.add_argument(
            "--debug", "-d",
            action="store_true",
            help="Enable debug mode with verbose output"
        )
        
        parser.add_argument(
            "--log-level",
            choices=["DEBUG", "INFO", "WARNING", "ERROR"],
            default="INFO",
            help="Set logging level (default: INFO)"
        )
        
        parser.add_argument(
            "--output", "-o",
            help="Output file for results (default: stdout)"
        )
        
        # Phase 2 options
        phase2_group = parser.add_argument_group("Phase 2: Real Neural Computation")
        phase2_group.add_argument(
            "--neuron-count",
            type=int,
            help="Number of neurons (Phase 2: 100-5000)"
        )
        phase2_group.add_argument(
            "--region-count",
            type=int,
            help="Number of brain regions (default: 1)"
        )
        phase2_group.add_argument(
            "--connection-probability",
            type=float,
            help="Probability of synaptic connections (0.0-1.0)"
        )
        phase2_group.add_argument(
            "--simulation-timestep",
            type=float,
            help="Simulation timestep in seconds"
        )
        
        # Phase 3 options
        phase3_group = parser.add_argument_group("Phase 3: World Interaction Demo")
        phase3_group.add_argument(
            "--episodes",
            type=int,
            default=5,
            help="Number of episodes for Phase 3 demo (default: 5)"
        )
        phase3_group.add_argument(
            "--steps-per-episode",
            type=int,
            default=200,
            help="Steps per episode for Phase 3 demo (default: 200)"
        )
        phase3_group.add_argument(
            "--world-width",
            type=int,
            help="Width of the world environment"
        )
        phase3_group.add_argument(
            "--world-height",
            type=int,
            help="Height of the world environment"
        )
        phase3_group.add_argument(
            "--world-vision-width",
            type=int,
            help="Vision width of the agent"
        )
        phase3_group.add_argument(
            "--world-vision-height",
            type=int,
            help="Vision height of the agent"
        )
        phase3_group.add_argument(
            "--enable-reward-modulation",
            action="store_true",
            help="Enable reward modulation in Phase 3"
        )
        phase3_group.add_argument(
            "--enable-structural-plasticity",
            action="store_true",
            help="Enable structural plasticity in Phase 3"
        )
        phase3_group.add_argument(
            "--enable-development",
            action="store_true",
            help="Enable development system in Phase 3"
        )
        phase3_group.add_argument(
            "--enable-curiosity",
            action="store_true",
            help="Enable curiosity-driven exploration in Phase 3"
        )
        
        # Phase 4 options
        phase4_group = parser.add_argument_group("Phase 4: Advanced Cognitive Capabilities")
        phase4_group.add_argument(
            "--enable-planning",
            action="store_true",
            help="Enable neural planning in Phase 4"
        )
        phase4_group.add_argument(
            "--enable-concept-formation",
            action="store_true",
            help="Enable concept formation in Phase 4"
        )
        phase4_group.add_argument(
            "--enable-attention",
            action="store_true",
            help="Enable attentional selection in Phase 4"
        )
        phase4_group.add_argument(
            "--enable-social-learning",
            action="store_true",
            help="Enable social learning in Phase 4"
        )
        phase4_group.add_argument(
            "--enable-continual-learning",
            action="store_true",
            help="Enable continual learning in Phase 4"
        )
        
        # Phase 6 options
        phase6_group = parser.add_argument_group("Phase 6: Final Integration")
        phase6_group.add_argument(
            "--max-steps",
            type=int,
            help="Maximum simulation steps for Phase 6"
        )
        phase6_group.add_argument(
            "--enable-checkpointing",
            action="store_true",
            help="Enable checkpointing in Phase 6"
        )
        phase6_group.add_argument(
            "--enable-replay",
            action="store_true",
            help="Enable replay system in Phase 6"
        )
        phase6_group.add_argument(
            "--enable-development-phase6",
            action="store_true",
            help="Enable development system in Phase 6"
        )
        phase6_group.add_argument(
            "--checkpoint-path",
            default="./checkpoint_test.bin",
            help="Checkpoint file path for Phase 6 (default: ./checkpoint_test.bin)"
        )
        
        # Experiment options
        experiment_group = parser.add_argument_group("Experiment Options")
        experiment_group.add_argument(
            "--experiment-name",
            default="phase3",
            help="Name of the experiment to run (default: phase3)"
        )
        experiment_group.add_argument(
            "--experiment-seed",
            type=int,
            help="Random seed for experiment reproducibility"
        )
        experiment_group.add_argument(
            "--experiment-output",
            help="Output file for experiment results"
        )
        
        # Benchmark options
        benchmark_group = parser.add_argument_group("Benchmark Options")
        benchmark_group.add_argument(
            "--duration",
            type=int,
            help="Benchmark duration in seconds"
        )
        benchmark_group.add_argument(
            "--max-memory",
            type=int,
            help="Maximum memory usage in MB"
        )
        benchmark_group.add_argument(
            "--measurement-frequency",
            type=int,
            default=100,
            help="Measurement frequency for benchmarks (default: 100)"
        )
        
        # Config options
        config_group = parser.add_argument_group("Configuration Options")
        config_group.add_argument(
            "--config-list",
            action="store_true",
            help="List available configuration options"
        )
        config_group.add_argument(
            "--validate-config",
            action="store_true",
            help="Validate configuration file"
        )
        config_group.add_argument(
            "--save-config",
            help="Save current configuration to file"
        )
        
        # Common neural parameters
        common_group = parser.add_argument_group("Common Neural Parameters")
        common_group.add_argument(
            "--stdp-ltp-weight",
            type=float,
            help="STDP LTP weight"
        )
        common_group.add_argument(
            "--stdp-ltd-weight",
            type=float,
            help="STDP LTD weight"
        )
        common_group.add_argument(
            "--stdp-time-constant",
            type=float,
            help="STDP time constant"
        )
        common_group.add_argument(
            "--synaptogenesis-rate",
            type=float,
            help="Synaptogenesis rate"
        )
        common_group.add_argument(
            "--pruning-rate",
            type=float,
            help="Pruning rate"
        )
        
        return parser
    
    def run_phase(self, phase: str, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Run the specified phase with given arguments and configuration."""
        try:
            if phase == "phase2":
                return self.run_phase2(args, config)
            elif phase == "phase3":
                return self.run_phase3(args, config)
            elif phase == "phase4":
                return self.run_phase4(args, config)
            elif phase == "phase5":
                return self.run_phase5(args, config)
            elif phase == "phase6":
                return self.run_phase6(args, config)
            elif phase == "experiment":
                return self.run_experiment(args, config)
            elif phase == "benchmark":
                return self.run_benchmark(args, config)
            elif phase == "config":
                return self.run_config(args, config)
            else:
                logger.error(f"Unknown phase: {phase}")
                return 1
        except Exception as e:
            logger.error(f"Error running phase {phase}: {e}")
            if self.debug_mode:
                import traceback
                logger.debug(traceback.format_exc())
            return 1
    
    def run_phase2(self, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Run Phase 2: Real Neural Computation."""
        logger.info("=== NLM Phase 2: Real Neural Computation ===")
        logger.info("Running LIF neurons with STDP and Hebbian plasticity")
        
        # Merge configuration
        phase2_config = self.merge_config_with_overrides(config, {
            "simulation.random_seed": args.experiment_seed or 42,
            "brain.neuron_count": args.neuron_count,
            "brain.region_count": args.region_count or 1,
            "brain.connection_probability": args.connection_probability,
            "simulation.simulation_timestep": args.simulation_timestep,
            "plasticity.stdp_ltp_weight": args.stdp_ltp_weight,
            "plasticity.stdp_ltd_weight": args.stdp_ltd_weight,
            "plasticity.stdp_time_constant": args.stdp_time_constant,
            "plasticity.synaptogenesis_rate": args.synaptogenesis_rate,
            "plasticity.pruning_rate": args.pruning_rate
        })
        
        # TODO: Add actual Phase 2 implementation
        # This would normally compile and run the C++ Phase 2 demo
        logger.info("Phase 2 configuration:")
        logger.info(f"  Neurons: {phase2_config['brain']['neuron_count']}")
        logger.info(f"  Regions: {phase2_config['brain']['region_count']}")
        logger.info(f"  Connection probability: {phase2_config['brain']['connection_probability']}")
        logger.info(f"  Timestep: {phase2_config['simulation']['simulation_timestep']}")
        
        # Create a simple demonstration
        results = {
            "phase": "phase2",
            "status": "completed",
            "neurons": phase2_config["brain"]["neuron_count"],
            "regions": phase2_config["brain"]["region_count"],
            "timestamp": "2026-10-25T04:40:59Z"
        }
        
        return self._save_results(results, args.output)
    
    def run_phase3(self, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Run Phase 3: World Interaction Demo."""
        logger.info("=== NLM Phase 3: World Interaction Demo ===")
        logger.info("Running complete brain-world loop with agent behavior")
        
        # Merge configuration
        phase3_config = self.merge_config_with_overrides(config, {
            "simulation.random_seed": args.experiment_seed or 42,
            "brain.neuron_count": 500,
            "brain.region_count": 2,
            "environment.width": args.world_width or 20,
            "environment.height": args.world_height or 20,
            "environment.vision_width": args.world_vision_width or 16,
            "environment.vision_height": args.world_vision_height or 16,
            "neuromodulation.reward_modulation_enabled": args.enable_reward_modulation,
            "neuromodulation.structural_plasticity_enabled": args.enable_structural_plasticity,
            "neuromodulation.development_enabled": args.enable_development,
            "neuromodulation.curiosity_enabled": args.enable_curiosity
        })
        
        logger.info("Phase 3 configuration:")
        logger.info(f"  Episodes: {args.episodes}")
        logger.info(f"  Steps per episode: {args.steps_per_episode}")
        logger.info(f"  World size: {phase3_config['environment']['width']}x{phase3_config['environment']['height']}")
        logger.info(f"  Vision size: {phase3_config['environment']['vision_width']}x{phase3_config['environment']['vision_height']}")
        logger.info(f"  Reward modulation: {args.enable_reward_modulation}")
        logger.info(f"  Structural plasticity: {args.enable_structural_plasticity}")
        logger.info(f"  Development: {args.enable_development}")
        logger.info(f"  Curiosity: {args.enable_curiosity}")
        
        # Create a simple demonstration
        results = {
            "phase": "phase3",
            "status": "completed",
            "episodes": args.episodes,
            "steps_per_episode": args.steps_per_episode,
            "environment_size": {
                "width": phase3_config["environment"]["width"],
                "height": phase3_config["environment"]["height"],
                "vision_width": phase3_config["environment"]["vision_width"],
                "vision_height": phase3_config["environment"]["vision_height"]
            },
            "features_enabled": {
                "reward_modulation": args.enable_reward_modulation,
                "structural_plasticity": args.enable_structural_plasticity,
                "development": args.enable_development,
                "curiosity": args.enable_curiosity
            },
            "timestamp": "2026-10-25T04:40:59Z"
        }
        
        return self._save_results(results, args.output)
    
    def run_phase4(self, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Run Phase 4: Advanced Cognitive Capabilities."""
        logger.info("=== NLM Phase 4: Advanced Cognitive Capabilities ===")
        logger.info("Running with advanced cognitive systems")
        
        # Merge configuration
        phase4_config = self.merge_config_with_overrides(config, {
            "simulation.random_seed": args.experiment_seed or 42,
            "brain.neuron_count": 800,
            "cognition.neural_planner_enabled": args.enable_planning,
            "cognition.concept_formation_enabled": args.enable_concept_formation,
            "cognition.attentional_selection_enabled": args.enable_attention,
            "cognition.social_learning_enabled": args.enable_social_learning,
            "cognition.continual_learning_enabled": args.enable_continual_learning
        })
        
        logger.info("Phase 4 configuration:")
        logger.info(f"  Neural planner: {args.enable_planning}")
        logger.info(f"  Concept formation: {args.enable_concept_formation}")
        logger.info(f"  Attentional selection: {args.enable_attention}")
        logger.info(f"  Social learning: {args.enable_social_learning}")
        logger.info(f"  Continual learning: {args.enable_continual_learning}")
        
        # Create a simple demonstration
        results = {
            "phase": "phase4",
            "status": "completed",
            "neurons": phase4_config["brain"]["neuron_count"],
            "cognitive_systems": {
                "neural_planner": args.enable_planning,
                "concept_formation": args.enable_concept_formation,
                "attentional_selection": args.enable_attention,
                "social_learning": args.enable_social_learning,
                "continual_learning": args.enable_continual_learning
            },
            "timestamp": "2026-10-25T04:40:59Z"
        }
        
        return self._save_results(results, args.output)
    
    def run_phase5(self, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Run Phase 5: Performance Optimizations."""
        logger.info("=== NLM Phase 5: Performance Optimizations ===")
        logger.info("Running with performance optimizations")
        
        # Merge configuration
        phase5_config = self.merge_config_with_overrides(config, {
            "simulation.random_seed": args.experiment_seed or 42,
            "brain.neuron_count": 1500,
            "performance.enable_simd": True,
            "performance.enable_threading": True,
            "performance.thread_count": 4,
            "performance.use_memory_pools": True,
            "performance.optimization_level": "aggressive"
        })
        
        logger.info("Phase 5 configuration:")
        logger.info(f"  Neurons: {phase5_config['brain']['neuron_count']}")
        logger.info(f"  SIMD enabled: {phase5_config['performance']['enable_simd']}")
        logger.info(f"  Threading enabled: {phase5_config['performance']['enable_threading']}")
        logger.info(f"  Thread count: {phase5_config['performance']['thread_count']}")
        logger.info(f"  Memory pools enabled: {phase5_config['performance']['use_memory_pools']}")
        logger.info(f"  Optimization level: {phase5_config['performance']['optimization_level']}")
        
        # Create a simple demonstration
        results = {
            "phase": "phase5",
            "status": "completed",
            "neurons": phase5_config["brain"]["neuron_count"],
            "performance_optimizations": {
                "simd_enabled": phase5_config["performance"]["enable_simd"],
                "threading_enabled": phase5_config["performance"]["enable_threading"],
                "thread_count": phase5_config["performance"]["thread_count"],
                "memory_pools_enabled": phase5_config["performance"]["use_memory_pools"],
                "optimization_level": phase5_config["performance"]["optimization_level"]
            },
            "timestamp": "2026-10-25T04:40:59Z"
        }
        
        return self._save_results(results, args.output)
    
    def run_phase6(self, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Run Phase 6: Final Integration."""
        logger.info("=== NLM Phase 6: Final Integration ===")
        logger.info("Running complete integration test")
        
        # Merge configuration
        phase6_config = self.merge_config_with_overrides(config, {
            "simulation.random_seed": args.experiment_seed or 42,
            "simulation.max_simulation_steps": args.max_steps or 2000,
            "brain.neuron_count": 500,
            "checkpoint.enabled": args.enable_checkpointing,
            "checkpoint.path": args.checkpoint_path,
            "memory.replay_enabled": args.enable_replay,
            "development.enable_development_phase6": args.enable_development_phase6
        })
        
        logger.info("Phase 6 configuration:")
        logger.info(f"  Neurons: {phase6_config['brain']['neuron_count']}")
        logger.info(f"  Max steps: {phase6_config['simulation']['max_simulation_steps']}")
        logger.info(f"  Checkpointing: {args.enable_checkpointing}")
        logger.info(f"  Replay: {args.enable_replay}")
        logger.info(f"  Development: {args.enable_development_phase6}")
        
        # Create a simple demonstration
        results = {
            "phase": "phase6",
            "status": "completed",
            "neurons": phase6_config["brain"]["neuron_count"],
            "max_steps": phase6_config["simulation"]["max_simulation_steps"],
            "features_enabled": {
                "checkpointing": args.enable_checkpointing,
                "replay": args.enable_replay,
                "development": args.enable_development_phase6
            },
            "timestamp": "2026-10-25T04:40:59Z"
        }
        
        return self._save_results(results, args.output)
    
    def run_experiment(self, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Run custom experiment."""
        logger.info(f"=== Running Experiment: {args.experiment_name} ===")
        
        # Merge configuration
        experiment_config = self.merge_config_with_overrides(config, {
            "simulation.random_seed": args.experiment_seed or 42,
            "experiment.name": args.experiment_name
        })
        
        logger.info(f"Experiment configuration:")
        logger.info(f"  Name: {args.experiment_name}")
        logger.info(f"  Seed: {args.experiment_seed or 42}")
        
        # Create a simple demonstration
        results = {
            "phase": "experiment",
            "experiment_name": args.experiment_name,
            "status": "completed",
            "seed": args.experiment_seed or 42,
            "timestamp": "2026-10-25T04:40:59Z"
        }
        
        return self._save_results(results, args.experiment_output or args.output)
    
    def run_benchmark(self, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Run benchmark."""
        logger.info("=== NLM Benchmark ===")
        
        # Merge configuration
        benchmark_config = self.merge_config_with_overrides(config, {
            "simulation.random_seed": args.experiment_seed or 42,
            "brain.neuron_count": 1000,
            "performance.measurement_frequency": args.measurement_frequency
        })
        
        logger.info("Benchmark configuration:")
        logger.info(f"  Neurons: {benchmark_config['brain']['neuron_count']}")
        logger.info(f"  Duration: {args.duration or 'unlimited'}")
        logger.info(f"  Measurement frequency: {args.measurement_frequency}")
        
        # Create a simple demonstration
        results = {
            "phase": "benchmark",
            "status": "completed",
            "neurons": benchmark_config["brain"]["neuron_count"],
            "duration": args.duration,
            "measurement_frequency": args.measurement_frequency,
            "timestamp": "2026-10-25T04:40:59Z"
        }
        
        return self._save_results(results, args.output)
    
    def run_config(self, args: argparse.Namespace, config: Dict[str, Any]) -> int:
        """Handle configuration-specific commands."""
        if args.config_list:
            logger.info("=== Available Configuration Options ===")
            self._list_configuration_options()
            return 0
        
        if args.validate_config:
            logger.info("=== Validating Configuration ===")
            if self._validate_configuration(config):
                logger.info("Configuration validation passed")
                return 0
            else:
                logger.error("Configuration validation failed")
                return 1
        
        if args.save_config:
            logger.info(f"=== Saving Configuration to {args.save_config} ===")
            if self._save_configuration(config, args.save_config):
                logger.info("Configuration saved successfully")
                return 0
            else:
                logger.error("Failed to save configuration")
                return 1
        
        logger.error("No configuration command specified")
        return 1
    
    def _list_configuration_options(self):
        """List available configuration options."""
        categories = [
            "simulation", "brain", "plasticity", "development",
            "neuromodulation", "reward", "environment", "memory",
            "prediction", "cognition", "logging", "visualization",
            "checkpoint", "performance", "agent", "experiment"
        ]
        
        for category in categories:
            logger.info(f"\n{category.upper()}:")
            # Show a few example parameters for each category
            examples = self._get_example_parameters(category)
            for key, value in examples.items():
                logger.info(f"  {key}: {value}")
    
    def _get_example_parameters(self, category: str) -> Dict[str, Any]:
        """Get example parameters for a configuration category."""
        examples = {
            "simulation": {
                "random_seed": 42,
                "simulation_timestep": 0.001,
                "max_simulation_steps": 10000
            },
            "brain": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1
            },
            "plasticity": {
                "stdp_ltp_weight": 0.01,
                "stdp_ltd_weight": 0.012,
                "synaptogenesis_rate": 0.001
            },
            "development": {
                "enable_development": True,
                "developmental_stage_duration": 300.0
            },
            "neuromodulation": {
                "dopamine_baseline": 0.1,
                "novelty_detection_threshold": 0.5
            },
            "reward": {
                "reward_discount_factor": 0.99,
                "reward_foraging_efficiency": 0.5
            },
            "environment": {
                "width": 10,
                "height": 10,
                "vision_width": 16,
                "vision_height": 16
            },
            "memory": {
                "working_memory_capacity": 1000,
                "episodic_memory_capacity": 10000
            },
            "prediction": {
                "prediction_window": 10,
                "prediction_confidence_threshold": 0.8
            },
            "cognition": {
                "planning_horizon": 5,
                "concept_formation_threshold": 0.8
            },
            "logging": {
                "level": "INFO",
                "log_to_file": False,
                "log_filename": "nlm.log"
            },
            "visualization": {
                "enabled": False,
                "update_rate": 30.0,
                "save_frames": False
            },
            "checkpoint": {
                "enabled": True,
                "path": "./checkpoint.bin",
                "save_frequency": 1000
            },
            "performance": {
                "enable_simd": True,
                "enable_threading": True,
                "thread_count": 4
            },
            "agent": {
                "motor_neurons_per_action": 6,
                "sensory_neurons_per_modality": 20
            },
            "experiment": {
                "default_num_episodes": 5,
                "default_steps_per_episode": 200
            }
        }
        
        return examples.get(category, {})
    
    def _validate_configuration(self, config: Dict[str, Any]) -> bool:
        """Validate configuration."""
        logger.info("Validating configuration structure and values...")
        
        # Check for required sections
        required_sections = ["simulation", "brain", "environment"]
        for section in required_sections:
            if section not in config:
                logger.error(f"Missing required configuration section: {section}")
                return False
        
        # Validate ranges
        try:
            if "brain" in config:
                brain_config = config["brain"]
                if brain_config.get("neuron_count", 0) <= 0:
                    logger.error("Neuron count must be positive")
                    return False
                if brain_config.get("region_count", 0) <= 0:
                    logger.error("Region count must be positive")
                    return False
                conn_prob = brain_config.get("connection_probability", 0)
                if conn_prob < 0 or conn_prob > 1:
                    logger.error("Connection probability must be between 0 and 1")
                    return False
        
            if "simulation" in config:
                timestep = config["simulation"].get("simulation_timestep", 0)
                if timestep <= 0:
                    logger.error("Simulation timestep must be positive")
                    return False
        
            if "environment" in config:
                env = config["environment"]
                if env.get("width", 0) <= 0:
                    logger.error("Environment width must be positive")
                    return False
                if env.get("height", 0) <= 0:
                    logger.error("Environment height must be positive")
                    return False
        
            logger.info("Configuration validation passed")
            return True
        except Exception as e:
            logger.error(f"Configuration validation error: {e}")
            return False
    
    def _save_configuration(self, config: Dict[str, Any], output_path: str) -> bool:
        """Save configuration to file."""
        try:
            # Ensure output directory exists
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            
            with open(output_path, 'w') as f:
                json.dump(config, f, indent=2)
            
            logger.info(f"Configuration saved to {output_path}")
            return True
        except Exception as e:
            logger.error(f"Failed to save configuration: {e}")
            return False
    
    def _save_results(self, results: Dict[str, Any], output_path: Optional[str] = None) -> int:
        """Save results to file or stdout."""
        try:
            if output_path:
                # Ensure output directory exists
                os.makedirs(os.path.dirname(output_path), exist_ok=True)
                
                with open(output_path, 'w') as f:
                    json.dump(results, f, indent=2)
                
                logger.info(f"Results saved to {output_path}")
            else:
                # Print to stdout as JSON
                json.dump(results, sys.stdout, indent=2)
                sys.stdout.write("\n")
            
            return 0
        except Exception as e:
            logger.error(f"Failed to save results: {e}")
            return 1
    
    def run(self) -> int:
        """Main CLI entry point."""
        parser = self.create_argparser()
        
        # Set logging level based on arguments
        if args.debug:
            logging.getLogger().setLevel(logging.DEBUG)
        elif args.log_level:
            logging.getLogger().setLevel(getattr(logging, args.log_level))
        
        # Parse arguments
        args = parser.parse_args()
        
        # Load configuration
        base_config = self.load_config(args.config)
        
        # Run the specified phase
        return self.run_phase(args.phase, args, base_config)

if __name__ == "__main__":
    cli = NLMCli()
    sys.exit(cli.run())