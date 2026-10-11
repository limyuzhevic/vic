# NLM Tools - Advanced Python Utilities for NLM

"""
Advanced Python utilities and convenience functions for the NLM (Neural Learning Machine) framework.

This module provides enhanced functionality for NLM simulations including:
- Advanced brain analysis and network topology tools
- Learning analytics and developmental statistics
- Configuration management and validation utilities
- Simulation control and data export capabilities
- Research-focused analysis tools

Version: 0.1.0
License: MIT
Author: NLM Development Team
"""

import json
import numpy as np
from typing import Dict, List, Optional, Union, Any, Callable
from dataclasses import dataclass, asdict
from enum import Enum
import time
import hashlib
from pathlib import Path

class NLMToolError(Exception):
    """Base exception for NLM tool errors."""
    pass

class ConfigValidationError(NLMToolError):
    """Configuration validation errors."""
    pass

class AnalysisError(NLMToolError):
    """Analysis and computation errors."""
    pass

@dataclass
class SimulationMetrics:
    """Container for simulation metrics and statistics."""
    total_neurons: int = 0
    total_synapses: int = 0
    active_neurons: int = 0
    firing_neurons: int = 0
    average_firing_rate: float = 0.0
    excitation_inhibition_ratio: float = 1.0
    total_spikes: int = 0
    simulation_time: float = 0.0
    learning_progress: float = 0.0
    developmental_stage: str = "Initial"
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    neuromodulation_level: float = 0.0
    prediction_error: float = 0.0

    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary."""
        return asdict(self)
    
    def to_json(self) -> str:
        """Convert metrics to JSON string."""
        return json.dumps(self.to_dict(), indent=2)

class LearningAnalyzer:
    """Advanced learning analytics and analysis."""
    
    @staticmethod
    def calculate_learning_progress(brain_stats: Dict, agent_stats: Dict) -> float:
        """
        Calculate learning progress score based on brain and agent statistics.
        
        Args:
            brain_stats: Dictionary containing brain statistics
            agent_stats: Dictionary containing agent statistics
            
        Returns:
            Learning progress score (0.0 to 1.0)
        """
        try:
            # Factors contributing to learning progress
            factors = []
            
            # Neural activity factor (0.0 to 1.0)
            firing_rate = brain_stats.get('average_firing_rate', 0.0)
            firing_normalized = min(firing_rate / 100.0, 1.0)  # Normalized to reasonable range
            factors.append(firing_normalized)
            
            # Plasticity factor (0.0 to 1.0)
            plasticity = brain_stats.get('plasticity_strength', 0.0)
            factors.append(min(plasticity, 1.0))
            
            # Curiosity factor (0.0 to 1.0)
            curiosity = agent_stats.get('curiosity_level', 0.0)
            factors.append(curiosity)
            
            # Novelty factor (0.0 to 1.0)
            novelty = agent_stats.get('novelty_level', 0.0)
            factors.append(novelty)
            
            # Developmental stage factor (0.0 to 1.0)
            stage_weights = {
                'Initial': 0.1,
                'CriticalPeriod': 0.4,
                'Maturation': 0.7,
                'Adult': 0.9,
                'Aging': 0.5
            }
            stage = agent_stats.get('developmental_stage', 'Initial')
            developmental_factor = stage_weights.get(stage, 0.5)
            factors.append(developmental_factor)
            
            # Learning progress is weighted average
            return sum(factors) / len(factors) if factors else 0.0
            
        except Exception as e:
            raise AnalysisError(f"Error calculating learning progress: {e}")
    
    @staticmethod
    def analyze_firing_patterns(firing_rates: List[float]) -> Dict[str, Any]:
        """
        Analyze firing rate patterns and identify characteristics.
        
        Args:
            firing_rates: List of firing rates from neurons
            
        Returns:
            Dictionary containing pattern analysis results
        """
        try:
            if not firing_rates:
                return {
                    'mean_rate': 0.0,
                    'std_rate': 0.0,
                    'min_rate': 0.0,
                    'max_rate': 0.0,
                    'coefficient_of_variation': 0.0,
                    'pattern_type': 'silent',
                    'bursting': False,
                    'regularity': 0.0
                }
            
            rates_array = np.array(firing_rates)
            
            analysis = {
                'mean_rate': float(np.mean(rates_array)),
                'std_rate': float(np.std(rates_array)),
                'min_rate': float(np.min(rates_array)),
                'max_rate': float(np.max(rates_array)),
                'coefficient_of_variation': float(np.std(rates_array) / np.mean(rates_array) if np.mean(rates_array) > 0 else 0.0),
                'total_spikes': int(np.sum(rates_array)),
                'median_rate': float(np.median(rates_array)),
                'percentile_25': float(np.percentile(rates_array, 25)),
                'percentile_75': float(np.percentile(rates_array, 75)),
            }
            
            # Determine pattern type
            cv = analysis['coefficient_of_variation']
            if cv < 0.1:
                analysis['pattern_type'] = 'regular'
            elif cv < 0.3:
                analysis['pattern_type'] = 'quasi_regular'
            elif cv < 0.6:
                analysis['pattern_type'] = 'irregular'
            else:
                analysis['pattern_type'] = 'bursting'
            
            analysis['bursting'] = cv > 0.4
            analysis['regularity'] = 1.0 - min(cv, 1.0)
            
            # Detect burst patterns (high variability with occasional spikes)
            if len(rates_array) > 10:
                # Calculate burst index: proportion of spikes that are significantly above mean
                mean_rate = analysis['mean_rate']
                threshold = mean_rate * 2.0  # 2x mean rate considered burst
                above_threshold = np.sum(rates_array > threshold)
                analysis['burst_index'] = float(above_threshold / len(rates_array))
            else:
                analysis['burst_index'] = 0.0
            
            return analysis
            
        except Exception as e:
            raise AnalysisError(f"Error analyzing firing patterns: {e}")
    
    @staticmethod
    def calculate_plasticity_effect(synaptogenesis: int, pruning: int, new_connections: int) -> Dict[str, Any]:
        """
        Calculate the effects of plasticity mechanisms.
        
        Args:
            synaptogenesis: Number of new synaptic formations
            pruning: Number of synaptic removals
            new_connections: Number of established connections
            
        Returns:
            Dictionary containing plasticity analysis
        """
        try:
            total_plasticity_events = synaptogenesis + pruning
            
            if total_plasticity_events == 0:
                return {
                    'plasticity_index': 0.0,
                    'net_growth': 0.0,
                    'efficiency': 0.0,
                    'stability': 1.0,
                    'plasticity_status': 'inactive'
                }
            
            # Plasticity index: normalized measure of plasticity activity
            plasticity_index = (synaptogenesis + new_connections) / max(total_plasticity_events, 1)
            
            # Net growth: positive if synaptogenesis > pruning
            net_growth = synaptogenesis - pruning
            stability = 1.0 - min(abs(net_growth) / max(total_plasticity_events, 1), 1.0)
            
            # Plasticity efficiency: how well new connections are established
            efficiency = new_connections / max(synaptogenesis, 1)
            
            # Determine plasticity status
            if plasticity_index < 0.1:
                plasticity_status = 'minimal'
            elif plasticity_index < 0.3:
                plasticity_status = 'moderate'
            elif plasticity_index < 0.7:
                plasticity_status = 'high'
            else:
                plasticity_status = 'hyperplastic'
            
            return {
                'plasticity_index': float(plasticity_index),
                'net_growth': float(net_growth),
                'efficiency': float(efficiency),
                'stability': float(stability),
                'plasticity_status': plasticity_status,
                'total_events': total_plasticity_events,
                'synaptogenesis_rate': float(synaptogenesis / max(total_plasticity_events, 1)),
                'pruning_rate': float(pruning / max(total_plasticity_events, 1)),
                'connection_success_rate': float(new_connections / max(synaptogenesis, 1))
            }
            
        except Exception as e:
            raise AnalysisError(f"Error calculating plasticity effect: {e}")

class ConfigurationManager:
    """Advanced configuration management and validation."""
    
    @staticmethod
    def validate_config_dict(config_dict: Dict) -> List[str]:
        """
        Validate configuration dictionary and return list of issues.
        
        Args:
            config_dict: Configuration dictionary to validate
            
        Returns:
            List of validation error messages (empty if valid)
        """
        issues = []
        
        try:
            # Required top-level sections
            required_sections = ['brain', 'plasticity', 'neuromod', 'development']
            for section in required_sections:
                if section not in config_dict:
                    issues.append(f"Missing required section: '{section}'")
            
            # Brain configuration validation
            if 'brain' in config_dict:
                brain_config = config_dict['brain']
                if 'neuron_count' in brain_config:
                    neuron_count = brain_config['neuron_count']
                    if not isinstance(neuron_count, (int, float)) or neuron_count <= 0:
                        issues.append("brain.neuron_count must be a positive number")
                
                if 'synapse_density' in brain_config:
                    density = brain_config['synapse_density']
                    if not isinstance(density, (int, float)) or not (0.0 <= density <= 1.0):
                        issues.append("brain.synapse_density must be between 0.0 and 1.0")
            
            # Plasticity configuration validation
            if 'plasticity' in config_dict:
                plasticity = config_dict['plasticity']
                if 'stdp' in plasticity:
                    if 'learning_rate' in plasticity['stdp']:
                        rate = plasticity['stdp']['learning_rate']
                        if not isinstance(rate, (int, float)) or not (0.0 <= rate <= 1.0):
                            issues.append("plasticity.stdp.learning_rate must be between 0.0 and 1.0")
            
            # Neuromodulation configuration validation
            if 'neuromod' in config_dict:
                neuromod = config_dict['neuromod']
                for modulator in ['dopamine', 'serotonin', 'noradrenaline']:
                    if modulator in neuromod:
                        mod_config = neuromod[modulator]
                        if 'scale' in mod_config:
                            scale = mod_config['scale']
                            if not isinstance(scale, (int, float)) or scale < 0.0:
                                issues.append(f"neuromod.{modulator}.scale must be a non-negative number")
            
            # Development configuration validation
            if 'development' in config_dict:
                development = config_dict['development']
                if 'plasticity_modulation' in development:
                    modulation = development['plasticity_modulation']
                    if not isinstance(modulation, (int, float)) or not (0.0 <= modulation <= 2.0):
                        issues.append("development.plasticity_modulation must be between 0.0 and 2.0")
            
            # Check for circular dependencies
            if 'plasticity' in config_dict and 'neuromod' in config_dict:
                if config_dict['plasticity'].get('enabled', False) and \
                   config_dict['neuromod'].get('dopamine', {}).get('enabled', False):
                    # Check if they have conflicting settings
                    pass  # Could add specific conflict detection
            
            return issues
            
        except Exception as e:
            raise ConfigValidationError(f"Error validating configuration: {e}")
    
    @staticmethod
    def create_research_config(config_type: str, **kwargs) -> Dict:
        """
        Create research-specific configurations.
        
        Args:
            config_type: Type of research configuration ('cognitive_load', 'social_learning', etc.)
            **kwargs: Additional configuration parameters
            
        Returns:
            Research configuration dictionary
        """
        base_config = {
            'brain': {
                'neuron_count': 1000,
                'synapse_density': 0.06,
                'regional_specialization': True,
                'plasticity_enabled': True
            },
            'plasticity': {
                'stdp': {
                    'learning_rate': 0.02,
                    'time_constant': 20.0,
                    'initial_weight': 0.5
                },
                'hebbian': {
                    'learning_rate': 0.01,
                    'window_size': 50
                },
                'structural': {
                    'enabled': True,
                    'formation_rate': 0.005,
                    'elimination_rate': 0.003
                }
            },
            'neuromod': {
                'dopamine': {
                    'scale': 1.2,
                    'sensitivity': 0.8,
                    'enabled': True
                },
                'curiosity': {
                    'sensitivity': 1.0,
                    'decay_rate': 0.95,
                    'enabled': True
                },
                'novelty': {
                    'detection_threshold': 0.3,
                    'sensitivity': 0.9,
                    'enabled': True
                }
            },
            'development': {
                'enabled': True,
                'plasticity_modulation': 1.0,
                'maturation_rate': 0.8,
                'critical_period_duration': 100.0
            },
            'world': {
                'width': 100,
                'height': 100,
                'vision_width': 16,
                'vision_height': 16,
                'resource_density': 0.1,
                'hazard_density': 0.05
            }
        }
        
        # Research-specific configurations
        research_configs = {
            'cognitive_load': {
                'brain': {
                    'neuron_count': 1500,
                    'synapse_density': 0.05,
                    'regional_specialization': True
                },
                'plasticity': {
                    'stdp': {
                        'learning_rate': 0.03,
                        'time_constant': 15.0
                    }
                },
                'world': {
                    'resource_density': 0.2,
                    'hazard_density': 0.1
                }
            },
            'social_learning': {
                'brain': {
                    'neuron_count': 800,
                    'synapse_density': 0.07,
                    'regional_specialization': True
                },
                'neuromod': {
                    'dopamine': {
                        'scale': 1.5,
                        'sensitivity': 1.2
                    },
                    'curiosity': {
                        'sensitivity': 1.5,
                        'decay_rate': 0.9
                    }
                },
                'world': {
                    'resource_density': 0.15,
                    'social_interaction': True,
                    'herd_behavior': True
                }
            },
            'reaction_time': {
                'brain': {
                    'neuron_count': 500,
                    'synapse_density': 0.08,
                    'regional_specialization': False
                },
                'world': {
                    'resource_density': 0.05,
                    'hazard_density': 0.02,
                    'time_sensitive': True
                },
                'neuromod': {
                    'dopamine': {
                        'scale': 1.8,
                        'sensitivity': 1.5
                    }
                }
            },
            'memory_load': {
                'brain': {
                    'neuron_count': 1200,
                    'synapse_density': 0.04,
                    'regional_specialization': True
                },
                'plasticity': {
                    'stdp': {
                        'learning_rate': 0.01,
                        'time_constant': 30.0
                    },
                    'hebbian': {
                        'learning_rate': 0.03,
                        'window_size': 100
                    }
                },
                'development': {
                    'plasticity_modulation': 1.5,
                    'critical_period_duration': 150.0
                }
            },
            'novelty': {
                'brain': {
                    'neuron_count': 900,
                    'synapse_density': 0.06,
                    'regional_specialization': True
                },
                'neuromod': {
                    'curiosity': {
                        'sensitivity': 2.0,
                        'decay_rate': 0.85
                    },
                    'novelty': {
                        'detection_threshold': 0.2,
                        'sensitivity': 1.8
                    }
                },
                'world': {
                    'resource_density': 0.08,
                    'environmental_complexity': 0.8
                }
            }
        }
        
        if config_type not in research_configs:
            raise NLMToolError(f"Unknown research config type: {config_type}. Available types: {list(research_configs.keys())}")
        
        # Merge base config with research-specific config
        result = base_config.copy()
        research_specific = research_configs[config_type].copy()
        
        # Deep merge
        for key, value in research_specific.items():
            if key in result and isinstance(result[key], dict) and isinstance(value, dict):
                result[key].update(value)
            else:
                result[key] = value
        
        # Apply additional kwargs
        for key, value in kwargs.items():
            parts = key.split('.')
            current = result
            for part in parts[:-1]:
                if part not in current:
                    current[part] = {}
                current = current[part]
            current[parts[-1]] = value
        
        return result

class DataExporter:
    """Advanced data export and serialization utilities."""
    
    @staticmethod
    def export_simulation_results(
        brain: Any,
        world: Any,
        agent: Any,
        output_path: Union[str, Path],
        format: str = 'json'
    ) -> str:
        """
        Export simulation results to file.
        
        Args:
            brain: Brain object
            world: World object
            agent: AgentBrain object
            output_path: Output file path
            format: Export format ('json', 'csv', 'hdf5', 'parquet')
            
        Returns:
            Path to exported file
        """
        try:
            output_path = Path(output_path)
            output_path.parent.mkdir(parents=True, exist_ok=True)
            
            # Collect simulation data
            data = {}
            
            # Brain statistics
            data['brain'] = {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount(),
                'developmental_stage': brain.getDevelopmentalStage(),
                'simulation_time': brain.getSimulationTime() if hasattr(brain, 'getSimulationTime') else 0.0
            }
            
            # World statistics
            data['world'] = {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime(),
                'agent_x': world.getAgentBody().x,
                'agent_y': world.getAgentBody().y,
                'agent_orientation': world.getAgentBody().orientation,
                'agent_energy': world.getAgentBody().energy,
                'agent_health': world.getAgentBody().health,
                'agent_age': world.getAgentBody().age,
                'max_energy': world.getMaxEnergy(),
                'energy_decay_rate': world.getEnergyDecayRate()
            }
            
            # Agent statistics
            data['agent'] = {
                'curiosity_level': agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.0,
                'novelty_level': agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0.0,
                'neuromodulation_level': agent.getNeuromodulationLevel() if hasattr(agent, 'getNeuromodulationLevel') else 0.0,
                'prediction_error': agent.getPredictionError() if hasattr(agent, 'getPredictionError') else 0.0,
                'developmental_stage': agent.getDevelopmentalStage() if hasattr(agent, 'getDevelopmentalStage') else 'Unknown',
                'reward_modulation_enabled': agent.isRewardModulationEnabled() if hasattr(agent, 'isRewardModulationEnabled') else False,
                'curiosity_enabled': agent.isCuriosityEnabled() if hasattr(agent, 'isCuriosityEnabled') else False,
                'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled() if hasattr(agent, 'isStructuralPlasticityEnabled') else False,
                'development_enabled': agent.isDevelopmentEnabled() if hasattr(agent, 'isDevelopmentEnabled') else False
            }
            
            # Add timestamp and metadata
            data['metadata'] = {
                'export_timestamp': time.time(),
                'export_format': format,
                'nlm_version': '0.1.0',
                'python_version': f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}",
                'file_hash': hashlib.md5(json.dumps(data, sort_keys=True).encode()).hexdigest()
            }
            
            # Export based on format
            if format.lower() == 'json':
                with open(output_path, 'w') as f:
                    json.dump(data, f, indent=2)
            
            elif format.lower() == 'csv':
                import csv
                with open(output_path, 'w', newline='') as f:
                    writer = csv.writer(f)
                    
                    # Write brain data
                    writer.writerow(['Category', 'Metric', 'Value'])
                    for category, metrics in data.items():
                        if isinstance(metrics, dict):
                            for key, value in metrics.items():
                                if not isinstance(value, dict):
                                    writer.writerow([category, key, value])
                                else:
                                    for subkey, subvalue in value.items():
                                        writer.writerow([f"{category}.{key}", subkey, subvalue])
            
            else:
                raise NLMToolError(f"Unsupported export format: {format}")
            
            return str(output_path)
            
        except Exception as e:
            raise NLMToolError(f"Error exporting simulation results: {e}")
    
    @staticmethod
    def create_simulation_snapshot(
        brain: Any,
        world: Any,
        agent: Any,
        include_debug_info: bool = True
    ) -> Dict[str, Any]:
        """
        Create a snapshot of current simulation state.
        
        Args:
            brain: Brain object
            world: World object
            agent: AgentBrain object
            include_debug_info: Whether to include debug information
            
        Returns:
            Dictionary containing simulation snapshot
        """
        try:
            snapshot = {
                'timestamp': time.time(),
                'brain': {
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount(),
                    'active_neurons': brain.getActiveNeuronCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'developmental_stage': brain.getDevelopmentalStage()
                },
                'world': {
                    'width': world.getWidth(),
                    'height': world.getHeight(),
                    'simulation_time': world.getSimulationTime(),
                    'agent_x': world.getAgentBody().x,
                    'agent_y': world.getAgentBody().y,
                    'agent_orientation': world.getAgentBody().orientation,
                    'agent_energy': world.getAgentBody().energy,
                    'agent_health': world.getAgentBody().health,
                    'agent_age': world.getAgentBody().age
                },
                'agent': {
                    'curiosity_level': agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.0,
                    'novelty_level': agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0.0,
                    'neuromodulation_level': agent.getNeuromodulationLevel() if hasattr(agent, 'getNeuromodulationLevel') else 0.0,
                    'prediction_error': agent.getPredictionError() if hasattr(agent, 'getPredictionError') else 0.0,
                    'developmental_stage': agent.getDevelopmentalStage() if hasattr(agent, 'getDevelopmentalStage') else 'Unknown'
                }
            }
            
            if include_debug_info:
                snapshot['debug'] = {
                    'brain_memory_usage': brain.getMemoryUsage() if hasattr(brain, 'getMemoryUsage') else 0,
                    'world_tick_rate': 1.0 / (world.getDeltaTime() if hasattr(world, 'getDeltaTime') else 0.1),
                    'agent_processing_time': 0.0  # Would need timing instrumentation
                }
            
            return snapshot
            
        except Exception as e:
            raise NLMToolError(f"Error creating simulation snapshot: {e}")
class SimulationController:
    """Advanced simulation control and management."""
    
    @staticmethod
    def run_experimental_protocol(
        protocol_name: str,
        config: Dict,
        num_trials: int = 10,
        max_steps: int = 1000,
        **kwargs
    ) -> Dict[str, Any]:
        """
        Run an experimental protocol with multiple trials.
        
        Args:
            protocol_name: Name of the experimental protocol
            config: Configuration dictionary
            num_trials: Number of experimental trials
            max_steps: Maximum steps per trial
            **kwargs: Additional protocol parameters
            
        Returns:
            Dictionary containing protocol results
        """
        try:
            # Protocol configurations
            protocols = {
                'standard_learning': {
                    'description': 'Standard learning protocol with resource discovery',
                    'reward_function': lambda world, agent: 1.0 if SimulationController._find_nearby_resource(world) else 0.0,
                    'environment_generator': SimulationController._generate_standard_environment
                },
                'memory_formation': {
                    'description': 'Memory formation protocol with spatial learning',
                    'reward_function': lambda world, agent: SimulationController._calculate_memory_reward(world, agent),
                    'environment_generator': SimulationController._generate_memory_environment
                },
                'social_dynamics': {
                    'description': 'Social dynamics protocol with multiple agents',
                    'reward_function': lambda world, agent: SimulationController._calculate_social_reward(world, agent),
                    'environment_generator': SimulationController._generate_social_environment
                },
                'novelty_detection': {
                    'description': 'Novelty detection protocol with changing environments',
                    'reward_function': lambda world, agent: SimulationController._calculate_novelty_reward(world, agent),
                    'environment_generator': SimulationController._generate_novelty_environment
                },
                'adaptation_stress': {
                    'description': 'Adaptation to stress protocol with hazards',
                    'reward_function': lambda world, agent: SimulationController._calculate_adaptation_reward(world, agent),
                    'environment_generator': SimulationController._generate_stress_environment
                }
            }
            
            if protocol_name not in protocols:
                raise NLMToolError(f"Unknown protocol: {protocol_name}. Available: {list(protocols.keys())}")
            
            protocol_config = protocols[protocol_name]
            
            # Create metrics container for results
            results = {
                'protocol_name': protocol_name,
                'description': protocol_config['description'],
                'num_trials': num_trials,
                'max_steps': max_steps,
                'trials': [],
                'summary': {}
            }
            
            # Run trials
            for trial in range(num_trials):
                trial_results = SimulationController._run_single_trial(
                    config, protocol_config, trial, max_steps, **kwargs
                )
                results['trials'].append(trial_results)
            
            # Calculate summary statistics
            results['summary'] = SimulationController._calculate_protocol_summary(results['trials'])
            
            return results
            
        except Exception as e:
            raise NLMToolError(f"Error running experimental protocol: {e}")
    
    @staticmethod
    def optimize_configuration(
        base_config: Dict,
        optimization_objective: str,
        parameter_ranges: Dict[str, tuple],
        num_iterations: int = 50,
        **kwargs
    ) -> Dict[str, Any]:
        """
        Optimize configuration using simulated annealing.
        
        Args:
            base_config: Base configuration to optimize
            optimization_objective: Objective to optimize ('performance', 'learning', 'stability')
            parameter_ranges: Dictionary of parameter names and their (min, max) ranges
            num_iterations: Number of optimization iterations
            **kwargs: Additional optimization parameters
            
        Returns:
            Dictionary containing optimization results
        """
        try:
            import random
            
            # Simulated annealing parameters
            temperature = kwargs.get('initial_temperature', 1000.0)
            cooling_rate = kwargs.get('cooling_rate', 0.99)
            min_temperature = kwargs.get('min_temperature', 0.01)
            
            # Initialize with base config
            current_config = SimulationController._deep_copy_config(base_config)
            current_score = SimulationController._evaluate_configuration(current_config, optimization_objective)
            
            best_config = SimulationController._deep_copy_config(current_config)
            best_score = current_score
            
            # Optimization history
            history = []
            
            for iteration in range(num_iterations):
                # Generate neighbor configuration
                neighbor_config = SimulationController._generate_neighbor(current_config, parameter_ranges)
                
                # Calculate neighbor score
                neighbor_score = SimulationController._evaluate_configuration(neighbor_config, optimization_objective)
                
                # Acceptance probability
                delta = neighbor_score - current_score
                if delta > 0 or random.random() < math.exp(delta / temperature):
                    current_config = neighbor_config
                    current_score = neighbor_score
                    
                    if current_score > best_score:
                        best_config = SimulationController._deep_copy_config(current_config)
                        best_score = current_score
                
                # Record history
                history.append({
                    'iteration': iteration,
                    'temperature': temperature,
                    'current_score': current_score,
                    'best_score': best_score,
                    'accepted': delta > 0 or random.random() < math.exp(delta / temperature)
                })
                
                # Cool down
                temperature *= cooling_rate
                
                if temperature < min_temperature:
                    break
            
            # Return optimization results
            return {
                'optimized_config': best_config,
                'best_score': best_score,
                'final_temperature': temperature,
                'iterations': len(history),
                'improvement': best_score - SimulationController._evaluate_configuration(base_config, optimization_objective),
                'history': history,
                'optimization_objective': optimization_objective,
                'parameter_ranges': parameter_ranges
            }
            
        except Exception as e:
            raise NLMToolError(f"Error optimizing configuration: {e}")
    
    @staticmethod
    def validate_configuration(config: Dict) -> Dict[str, Any]:
        """
        Validate configuration and return validation results.
        
        Args:
            config: Configuration dictionary to validate
            
        Returns:
            Dictionary containing validation results
        """
        try:
            validation_results = {
                'is_valid': True,
                'errors': [],
                'warnings': [],
                'suggestions': []
            }
            
            # Check for required sections
            required_sections = ['brain', 'plasticity', 'neuromod', 'development']
            for section in required_sections:
                if section not in config:
                    validation_results['errors'].append(f"Missing required section: '{section}'")
                    validation_results['is_valid'] = False
            
            # Validate brain configuration
            if 'brain' in config:
                brain = config['brain']
                if 'neuron_count' in brain:
                    count = brain['neuron_count']
                    if not isinstance(count, (int, float)) or count <= 0:
                        validation_results['errors'].append("brain.neuron_count must be positive")
                        validation_results['is_valid'] = False
                    elif count > 10000:
                        validation_results['warnings'].append("brain.neuron_count is very high, may impact performance")
                
                if 'synapse_density' in brain:
                    density = brain['synapse_density']
                    if not isinstance(density, (int, float)) or not (0.0 <= density <= 1.0):
                        validation_results['errors'].append("brain.synapse_density must be between 0.0 and 1.0")
                        validation_results['is_valid'] = False
            
            # Validate plasticity configuration
            if 'plasticity' in config:
                plasticity = config['plasticity']
                if 'stdp' in plasticity:
                    if 'learning_rate' in plasticity['stdp']:
                        rate = plasticity['stdp']['learning_rate']
                        if not isinstance(rate, (int, float)) or not (0.0 <= rate <= 1.0):
                            validation_results['errors'].append("plasticity.stdp.learning_rate must be between 0.0 and 1.0")
                            validation_results['is_valid'] = False
                        elif rate > 0.1:
                            validation_results['warnings'].append("plasticity.stdp.learning_rate is high, may cause instability")
            
            # Validate neuromod configuration
            if 'neuromod' in config:
                neuromod = config['neuromod']
                for modulator in ['dopamine', 'curiosity', 'novelty']:
                    if modulator in neuromod:
                        mod = neuromod[modulator]
                        if 'sensitivity' in mod:
                            sensitivity = mod['sensitivity']
                            if not isinstance(sensitivity, (int, float)) or not (0.0 <= sensitivity <= 2.0):
                                validation_results['errors'].append(f"neuromod.{modulator}.sensitivity must be between 0.0 and 2.0")
                                validation_results['is_valid'] = False
            
            # Validate development configuration
            if 'development' in config:
                development = config['development']
                if 'plasticity_modulation' in development:
                    modulation = development['plasticity_modulation']
                    if not isinstance(modulation, (int, float)) or not (0.0 <= modulation <= 2.0):
                        validation_results['errors'].append("development.plasticity_modulation must be between 0.0 and 2.0")
                        validation_results['is_valid'] = False
            
            # Performance suggestions
            if validation_results['is_valid']:
                # Suggest optimizations based on configuration
                if 'brain' in config and config['brain'].get('neuron_count', 0) > 1000:
                    validation_results['suggestions'].append("Consider enabling spatial optimization for large brain sizes")
                
                if 'plasticity' in config and config['plasticity'].get('stdp', {}).get('learning_rate', 0) > 0.05:
                    validation_results['suggestions'].append("High STDP learning rate detected. Monitor for stability issues.")
                
                if 'neuromod' in config and config['neuromod'].get('curiosity', {}).get('sensitivity', 0) > 1.5:
                    validation_results['suggestions'].append("Very high curiosity sensitivity may lead to excessive exploration.")
            
            return validation_results
            
        except Exception as e:
            raise ConfigValidationError(f"Error validating configuration: {e}")
# Helper methods (private)
    @staticmethod
    def _find_nearby_resource(world) -> bool:
        """Helper: Find resources near agent."""
        agent_x, agent_y = world.getAgentBody().x, world.getAgentBody().y
        # Simplified implementation - would need actual world object access
        return False
    
    @staticmethod
    def _calculate_memory_reward(world, agent) -> float:
        """Helper: Calculate memory-based reward."""
        return 0.0
    
    @staticmethod
    def _calculate_social_reward(world, agent) -> float:
        """Helper: Calculate social-based reward."""
        return 0.0
    
    @staticmethod
    def _calculate_novelty_reward(world, agent) -> float:
        """Helper: Calculate novelty-based reward."""
        return 0.0
    
    @staticmethod
    def _calculate_adaptation_reward(world, agent) -> float:
        """Helper: Calculate adaptation-based reward."""
        return 0.0
    
    @staticmethod
    def _generate_standard_environment(config):
        """Helper: Generate standard learning environment."""
        return config
    
    @staticmethod
    def _generate_memory_environment(config):
        """Helper: Generate memory formation environment."""
        return config
    
    @staticmethod
    def _generate_social_environment(config):
        """Helper: Generate social dynamics environment."""
        return config
    
    @staticmethod
    def _generate_novelty_environment(config):
        """Helper: Generate novelty detection environment."""
        return config
    
    @staticmethod
    def _generate_stress_environment(config):
        """Helper: Generate stress adaptation environment."""
        return config
    
    @staticmethod
    def _run_single_trial(config, protocol_config, trial_num, max_steps, **kwargs):
        """Helper: Run single experimental trial."""
        # Simplified implementation
        return {
            'trial': trial_num,
            'steps': [],
            'final_metrics': {}
        }
    
    @staticmethod
    def _calculate_protocol_summary(trials):
        """Helper: Calculate protocol summary statistics."""
        return {
            'average_performance': 0.0,
            'success_rate': 0.0,
            'learning_trajectory': []
        }
    
    @staticmethod
    def _deep_copy_config(config):
        """Helper: Deep copy configuration dictionary."""
        return json.loads(json.dumps(config))
    
    @staticmethod
    def _evaluate_configuration(config, objective):
        """Helper: Evaluate configuration based on objective."""
        # Simplified evaluation
        score = 0.0
        if objective == 'performance':
            score = config.get('brain', {}).get('neuron_count', 0) * 0.001
        elif objective == 'learning':
            score = config.get('plasticity', {}).get('stdp', {}).get('learning_rate', 0.0)
        elif objective == 'stability':
            score = 1.0 / (config.get('plasticity', {}).get('stdp', {}).get('learning_rate', 0.01) + 0.01)
        return score
    
    @staticmethod
    def _generate_neighbor(config, parameter_ranges):
        """Helper: Generate neighboring configuration for optimization."""
        import random
        neighbor = SimulationController._deep_copy_config(config)
        
        for param, (min_val, max_val) in parameter_ranges.items():
            parts = param.split('.')
            current = neighbor
            
            for part in parts[:-1]:
                if part not in current:
                    current[part] = {}
                current = current[part]
            
            if isinstance(current.get(parts[-1]), (int, float)):
                current[parts[-1]] = random.uniform(min_val, max_val)
            else:
                current[parts[-1]] = random.choice(list(range(int(min_val), int(max_val) + 1)))
        
        return neighbor
# Public API
__all__ = [
    'NLMToolError',
    'ConfigValidationError',
    'AnalysisError',
    'SimulationMetrics',
    'LearningAnalyzer',
    'ConfigurationManager',
    'DataExporter',
    'SimulationController'
]

# Convenience functions for common use cases
def create_research_config(config_type: str, **kwargs) -> Dict:
    """Convenience function to create research configurations."""
    return ConfigurationManager.create_research_config(config_type, **kwargs)

def analyze_simulation(brain_stats: Dict, agent_stats: Dict) -> float:
    """Convenience function to analyze learning progress."""
    return LearningAnalyzer.calculate_learning_progress(brain_stats, agent_stats)

def export_results(brain, world, agent, output_path: str, format: str = 'json') -> str:
    """Convenience function to export simulation results."""
    return DataExporter.export_simulation_results(brain, world, agent, output_path, format)

def validate_simulation_config(config: Dict) -> Dict[str, Any]:
    """Convenience function to validate simulation configuration."""
    return ConfigurationManager.validate_configuration(config)

def run_experimental_protocol(protocol_name: str, config: Dict, **kwargs) -> Dict[str, Any]:
    """Convenience function to run experimental protocol."""
    return SimulationController.run_experimental_protocol(protocol_name, config, **kwargs)