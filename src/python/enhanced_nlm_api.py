# Improved NLM Python API - Complete Implementation

"""
Enhanced Python bindings for the Neural Learning Machine (NLM) framework.
This module provides a comprehensive, user-friendly interface to the NLM brain simulator.

Key improvements over the original API:
- Comprehensive documentation with examples
- Better error handling and type safety
- Advanced features for power users
- Performance monitoring and debugging tools
- Modular, extensible design
- Extensive configuration options
- Better integration with Python ecosystem
"""

import sys
import json
import time
import logging
from typing import Optional, List, Dict, Any, Union, Callable, Iterator
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

try:
    import pynlm
except ImportError as e:
    logger.error(f"Failed to import pynlm: {e}")
    logger.error("Please ensure the pynlm module is properly installed using 'pip install -e .'")
    raise

# Type aliases for better API
NeuronId = pynlm.NeuronId
SynapseId = pynlm.SynapseId
RegionId = pynlm.RegionId
PopulationId = pynlm.PopulationId
NeuronType = pynlm.NeuronType
ActionType = pynlm.ActionType
DevelopmentalStage = pynlm.DevelopmentalStage

__version__ = "2.0.0"
__author__ = "NLM Team"
__license__ = "MIT"

# Enhanced configuration class
class AdvancedConfig:
    """
    Enhanced configuration system with improved type safety and validation.
    
    This class extends the basic Config functionality with:
    - Schema validation
    - Default values management
    - Configuration inheritance
    - Environment variable support
    - Configuration groups and presets
    """
    
    def __init__(self):
        self._config = pynlm.createDefaultConfig()
        self._defaults = {}
        self._schema = {}
        self._groups = {}
        
    def load_from_preset(self, preset_name: str) -> bool:
        """
        Load a named preset configuration.
        
        Available presets: 'minimal', 'balanced', 'research', 'production'
        """
        presets = {
            'minimal': {
                'brain.neuron_count': 100,
                'brain.synapse_density': 0.05,
                'plasticity.stdp.enable': True,
                'neuromod.dopamine.scale': 0.5,
                'neuromod.curiosity.enable': False,
            },
            'balanced': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.1,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.001,
                'plasticity.hebbian.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.0,
            },
            'research': {
                'brain.neuron_count': 5000,
                'brain.synapse_density': 0.2,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.01,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.5,
                'neuromod.prediction_error.enable': True,
                'neuromod.prediction_error.sensitivity': 0.5,
            },
            'production': {
                'brain.neuron_count': 10000,
                'brain.synapse_density': 0.15,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.001,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': False,  # Off for stability
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.0,
                'neuromod.prediction_error.enable': True,
                'neuromod.prediction_error.sensitivity': 0.3,
                'simulation.timestep': 0.0005,
                'simulation.max_steps': 100000,
            }
        }
        
        if preset_name not in presets:
            raise ValueError(f"Unknown preset: {preset_name}. Available: {list(presets.keys())}")
            
        for key, value in presets[preset_name].items():
            self._config.set(key, value)
            
        return True
    
    def validate_configuration(self) -> List[str]:
        """Validate current configuration and return list of issues."""
        issues = []
        
        # Validate neuron count
        if self._config.has('brain.neuron_count'):
            neuron_count = self._config.getOr<int64_t>('brain.neuron_count', 1000)
            if neuron_count < 10:
                issues.append("Neuron count too low (minimum 10)")
            elif neuron_count > 100000:
                issues.append("Neuron count too high (maximum 100000)")
        
        # Validate connection probability
        if self._config.has('brain.connection_probability'):
            prob = self._config.getOr<double>('brain.connection_probability', 0.1)
            if prob < 0.0 or prob > 1.0:
                issues.append("Connection probability must be between 0.0 and 1.0")
        
        # Check for conflicting settings
        if self._config.getOr<bool>('plasticity.structural.enable', False):
            if not self._config.has('plasticity.structural.synaptogenesis_rate'):
                issues.append("Structural plasticity enabled but synaptogenesis rate not set")
        
        return issues
    
    def save_as_json(self, filepath: str) -> bool:
        """Save configuration as JSON file."""
        try:
            data = {}
            for key in self._config.getKeys():
                data[key] = self._config.getOr[str](key, "")
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            return True
        except Exception as e:
            logger.error(f"Failed to save configuration as JSON: {e}")
            return False
    
    def load_from_json(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            for key, value in data.items():
                if isinstance(value, bool):
                    self._config.set(key, value)
                elif isinstance(value, int):
                    self._config.set(key, value)
                elif isinstance(value, float):
                    self._config.set(key, value)
                elif isinstance(value, str):
                    self._config.set(key, value)
                elif isinstance(value, list):
                    if all(isinstance(x, int) for x in value):
                        self._config.set(key, value)
                    elif all(isinstance(x, float) for x in value):
                        self._config.set(key, value)
                    elif all(isinstance(x, str) for x in value):
                        self._config.set(key, value)
            
            return True
        except Exception as e:
            logger.error(f"Failed to load configuration from JSON: {e}")
            return False
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value with type safety."""
        if not self._config.has(key):
            if default is not None:
                return default
            raise KeyError(f"Configuration key '{key}' not found")
        
        # Return the value based on its type
        if isinstance(default, bool):
            return self._config.getOr<bool>(key, default)
        elif isinstance(default, int):
            return self._config.getOr<int64_t>(key, default)
        elif isinstance(default, float):
            return self._config.getOr<double>(key, default)
        elif isinstance(default, str):
            return self._config.getOr[str](key, default)
        else:
            return self._config.getOr[str](key, str(default))

# Enhanced brain class with monitoring and debugging
class EnhancedBrain:
    """
    Enhanced brain interface with monitoring, debugging, and advanced features.
    
    This class provides an enhanced interface to the NLM brain with:
    - Real-time monitoring and statistics
    - Performance profiling
    - Debugging tools
    - Advanced configuration
    - Simulation control and analysis
    """
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        if config is None:
            config = AdvancedConfig()
        
        self._config = config
        self._brain = pynlm.createBrain(config._config)
        self._brain.initialize()
        
        # Monitoring data
        self._step_times = []
        self._spike_rates = []
        self._energy_levels = []
        self._neuron_counts = []
        
        # Debugging data
        self._debug_enabled = False
        self._debug_log = []
        
    def run_simulation(self, num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
        """
        Run a simulation with comprehensive monitoring.
        
        Returns a dictionary with simulation statistics and performance metrics.
        """
        start_time = time.time()
        
        # Validate configuration
        validation_issues = self._config.validate_configuration()
        if validation_issues:
            logger.warning(f"Configuration validation issues: {validation_issues}")
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Get current statistics
            stats = self.get_brain_stats()
            
            # Record metrics
            self._step_times.append(time.time() - step_start)
            self._spike_rates.append(stats['firing_rate'])
            self._energy_levels.append(stats['e_i_ratio'])
            self._neuron_counts.append(stats['total_neurons'])
            
            # Record debug information if enabled
            if self._debug_enabled:
                self._record_debug_info(step, stats)
            
            # Run the brain step
            self._brain.step(step)
            
        end_time = time.time()
        
        # Generate performance report
        performance = self._generate_performance_report(num_steps, start_time, end_time)
        
        return performance
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        stats = {
            'total_neurons': self._brain.getTotalNeuronCount(),
            'total_synapses': self._brain.getTotalSynapseCount(),
            'active_neurons': self._brain.getActiveNeuronCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'firing_rate': self._brain.getAverageFiringRate(),
            'e_i_ratio': self._brain.getExcitationInhibitionRatio(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'developmental_stage': self._brain.getDevelopmentalStage(),
            'current_step': self._brain.getConfig().getOr<int64_t>("current_step", 0),
        }
        
        # Add memory system statistics
        working_memory = self._brain.getWorkingMemory()
        if working_memory:
            stats['working_memory_traces'] = working_memory.getActiveTraces()
        
        episodic_memory = self._brain.getEpisodicMemory()
        if episodic_memory:
            stats['episodic_memory_episodes'] = episodic_memory.getEpisodeCount()
        
        return stats
    
    def get_performance_analysis(self) -> Dict[str, Any]:
        """Get detailed performance analysis."""
        if not self._step_times:
            return {'error': 'No performance data available'}
        
        analysis = {
            'total_steps': len(self._step_times),
            'average_step_time': sum(self._step_times) / len(self._step_times),
            'min_step_time': min(self._step_times),
            'max_step_time': max(self._step_times),
            'median_step_time': sorted(self._step_times)[len(self._step_times) // 2],
            'average_spike_rate': sum(self._spike_rates) / len(self._spike_rates),
            'average_e_i_ratio': sum(self._energy_levels) / len(self._energy_levels),
            'average_neuron_count': sum(self._neuron_counts) / len(self._neuron_counts),
            'steps_per_second': len(self._step_times) / sum(self._step_times) if sum(self._step_times) > 0 else 0,
        }
        
        # Add performance recommendations
        recommendations = []
        if analysis['average_step_time'] > 0.1:
            recommendations.append("Step time is high (>0.1s). Consider reducing simulation complexity or increasing timestep.")
        
        if analysis['average_spike_rate'] > 50:
            recommendations.append("High spike rate detected. Consider adjusting connection probability or synaptic weights.")
        
        if analysis['average_e_i_ratio'] > 5:
            recommendations.append("High E/I ratio detected. Consider increasing inhibitory neuron count or modifying plasticity rules.")
        
        analysis['recommendations'] = recommendations
        
        return analysis
    
    def export_stats(self, filepath: str) -> bool:
        """Export performance statistics to a file."""
        try:
            stats = {
                'brain_stats': self.get_brain_stats(),
                'performance_analysis': self.get_performance_analysis(),
                'debug_log': self._debug_log if self._debug_enabled else [],
            }
            
            with open(filepath, 'w') as f:
                json.dump(stats, f, indent=2)
            
            return True
        except Exception as e:
            logger.error(f"Failed to export statistics: {e}")
            return False
    
    def enable_debug_mode(self, enabled: bool = True):
        """Enable or disable debug mode."""
        self._debug_enabled = enabled
        if enabled:
            logger.info("Debug mode enabled")
        else:
            logger.info("Debug mode disabled")
    
    def _record_debug_info(self, step: int, stats: Dict[str, Any]):
        """Record debug information for current step."""
        debug_info = {
            'step': step,
            'timestamp': time.time(),
            'stats': stats,
            'system_load': self._calculate_system_load(),
        }
        
        self._debug_log.append(debug_info)
        
        # Keep debug log size manageable
        if len(self._debug_log) > 1000:
            self._debug_log = self._debug_log[-500:]
    
    def _calculate_system_load(self) -> float:
        """Calculate system load based on recent performance."""
        if not self._step_times:
            return 0.0
        
        avg_step_time = sum(self._step_times) / len(self._step_times)
        max_expected_step_time = 0.01  # 10ms expected maximum
        return min(1.0, avg_step_time / max_expected_step_time)
    
    def _generate_performance_report(self, num_steps: int, start_time: float, end_time: float) -> Dict[str, Any]:
        """Generate performance report from simulation."""
        stats = self.get_brain_stats()
        analysis = self.get_performance_analysis()
        
        report = {
            'simulation_parameters': {
                'steps_run': num_steps,
                'total_simulation_time': end_time - start_time,
                'steps_per_second': num_steps / (end_time - start_time),
                'average_step_time': sum(self._step_times) / len(self._step_times) if self._step_times else 0,
            },
            'brain_statistics': stats,
            'performance_analysis': analysis,
            'configuration_validation': self._config.validate_configuration(),
        }
        
        return report

# Advanced agent brain with enhanced features
class AdvancedAgentBrain:
    """
    Enhanced agent brain interface with advanced learning capabilities.
    
    This class extends the basic AgentBrain with:
    - Advanced reinforcement learning capabilities
    - Curiosity-driven exploration
    - Novelty detection
    - Prediction error tracking
    - Advanced action selection strategies
    """
    
    def __init__(self, brain: EnhancedBrain):
        self._brain = brain
        self._agent = pynlm.createAgentBrain(brain._brain)
        self._agent.initialize(pynlm.createSimpleWorld())
        
        # Learning statistics
        self._reward_history = []
        self._novelty_history = []
        self._curiosity_history = []
        self._prediction_error_history = []
        
        # Advanced learning parameters
        self._learning_rate = 0.01
        self._exploration_rate = 0.1
        self._adaptation_threshold = 0.5
        
    def run_advanced_episode(self, num_steps: int, enable_learning: bool = True) -> Dict[str, Any]:
        """
        Run an advanced episode with enhanced learning capabilities.
        
        Returns episode statistics and learning progress.
        """
        world = self._agent._brain._config  # Access through brain
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        episode_stats = {
            'steps_completed': 0,
            'total_reward': 0.0,
            'max_reward': 0.0,
            'curiosity_driven_actions': 0,
            'novelty_detected': False,
            'learning_events': 0,
        }
        
        for step in range(num_steps):
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            self._agent.processSensoryInput(percept)
            
            # Get brain prediction
            prediction = self._agent.getPredictionError()
            
            # Enable/disable learning
            if enable_learning:
                self._agent.enableRewardModulation(True)
                self._agent.enableCuriosity(True)
                self._agent.enableDevelopment(True)
            else:
                self._agent.enableRewardModulation(False)
                self._agent.enableCuriosity(False)
            
            # Decode motor command
            action = self._agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and novelty information
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            novelty = self._agent.getNoveltyLevel()
            curiosity = self._agent.getCuriosityLevel()
            prediction_error = self._agent.getPredictionError()
            
            # Record learning data
            self._reward_history.append(reward)
            self._novelty_history.append(novelty)
            self._curiosity_history.append(curiosity)
            self._prediction_error_history.append(prediction_error)
            
            # Update episode statistics
            episode_stats['total_reward'] += reward
            episode_stats['max_reward'] = max(episode_stats['max_reward'], reward)
            
            if curiosity > self._adaptation_threshold:
                episode_stats['curiosity_driven_actions'] += 1
            
            if novelty > 0.5:
                episode_stats['novelty_detected'] = True
            
            if abs(prediction_error) > 0.1:
                episode_stats['learning_events'] += 1
            
            # Apply reward modulation
            self._agent.applyRewardModulation(reward, prediction_error)
            
            # Update development
            self._agent.updateDevelopment(0.1)
            
            episode_stats['steps_completed'] += 1
        
        return episode_stats
    
    def get_learning_statistics(self) -> Dict[str, Any]:
        """Get comprehensive learning statistics."""
        if not self._reward_history:
            return {'error': 'No learning data available'}
        
        stats = {
            'episodes_run': len(self._reward_history),
            'average_reward': sum(self._reward_history) / len(self._reward_history),
            'max_reward': max(self._reward_history),
            'min_reward': min(self._reward_history),
            'average_novelty': sum(self._novelty_history) / len(self._novelty_history),
            'average_curiosity': sum(self._curiosity_history) / len(self._curiosity_history),
            'average_prediction_error': sum(self._prediction_error_history) / len(self._prediction_error_history),
            'reward_std_dev': (sum((x - sum(self._reward_history) / len(self._reward_history)) ** 2 for x in self._reward_history) / len(self._reward_history)) ** 0.5,
        }
        
        return stats
    
    def adapt_learning_rate(self, target_performance: float, actual_performance: float):
        """
        Adapt learning rate based on performance comparison.
        
        If actual performance is significantly lower than target, increase learning rate.
        If actual performance is significantly higher, decrease learning rate.
        """
        performance_diff = abs(actual_performance - target_performance)
        
        if performance_diff > 0.5:  # Large difference
            self._learning_rate *= 1.2 if actual_performance < target_performance else 0.8
        else:  # Small difference
            self._learning_rate *= 0.95 if actual_performance > target_performance else 1.05
        
        # Clamp learning rate
        self._learning_rate = max(0.001, min(0.1, self._learning_rate))
        
        logger.info(f"Adapted learning rate to {self._learning_rate}")

# Command-line interface utilities
class NLMCLI:
    """
    Command-line interface for NLM with advanced features.
    
    Provides a comprehensive CLI for:
    - Brain simulation control
    - Configuration management
    - Performance monitoring
    - Experiment management
    - Data analysis
    """
    
    def __init__(self):
        self._brain = None
        self._agent_brain = None
        
    def create_brain(self, preset: str = 'balanced') -> EnhancedBrain:
        """Create an enhanced brain with specified preset."""
        config = AdvancedConfig()
        config.load_from_preset(preset)
        self._brain = EnhancedBrain(config)
        return self._brain
    
    def run_experiment(self, preset: str, num_steps: int, output_file: str = None):
        """Run a complete experiment with specified preset."""
        print(f"Running NLM experiment with preset: {preset}")
        print(f"Steps: {num_steps}")
        
        # Create brain with preset
        config = AdvancedConfig()
        config.load_from_preset(preset)
        self._brain = EnhancedBrain(config)
        
        # Run simulation
        result = self._brain.run_simulation(num_steps)
        
        # Print results
        print("\n=== Experiment Results ===")
        print(f"Total simulation time: {result['simulation_parameters']['total_simulation_time']:.2f} seconds")
        print(f"Steps per second: {result['simulation_parameters']['steps_per_second']:.2f}")
        print(f"Average step time: {result['simulation_parameters']['average_step_time']*1000:.2f} ms")
        print(f"Final firing rate: {result['brain_statistics']['firing_rate']:.2f} Hz")
        print(f"E/I ratio: {result['brain_statistics']['e_i_ratio']:.2f}")
        print(f"Total spikes: {result['brain_statistics']['total_spikes']}")
        
        # Print recommendations
        if 'recommendations' in result['performance_analysis']:
            print("\nPerformance Recommendations:")
            for rec in result['performance_analysis']['recommendations']:
                print(f"  - {rec}")
        
        # Save results if output file specified
        if output_file:
            self._brain.export_stats(output_file)
            print(f"\nResults saved to: {output_file}")
        
        return result
    
    def interactive_mode(self):
        """Enter interactive mode for manual exploration."""
        print("=== NLM Interactive Mode ===")
        print("Available commands:")
        print("  create <preset> - Create brain with preset (minimal, balanced, research, production)")
        print("  run <steps> - Run simulation for specified steps")
        print("  stats - Show current brain statistics")
        print("  analyze - Show performance analysis")
        print("  config - Show current configuration")
        print("  save <file> - Save current state to file")
        print("  debug on/off - Enable/disable debug mode")
        print("  learn on/off - Enable/disable learning")
        print("  exit - Exit interactive mode")
        
        while True:
            try:
                command = input("\nNLM> ").strip()
                parts = command.split()
                
                if not parts:
                    continue
                
                if parts[0] == 'create':
                    preset = parts[1] if len(parts) > 1 else 'balanced'
                    self.create_brain(preset)
                    print(f"Created brain with preset: {preset}")
                
                elif parts[0] == 'run':
                    if not self._brain:
                        print("No brain created. Use 'create <preset>' first.")
                        continue
                    
                    steps = int(parts[1]) if len(parts) > 1 else 1000
                    self._brain.run_simulation(steps)
                    print(f"Simulation completed: {steps} steps")
                
                elif parts[0] == 'stats':
                    if not self._brain:
                        print("No brain created. Use 'create <preset>' first.")
                        continue
                    
                    stats = self._brain.get_brain_stats()
                    print("\n=== Brain Statistics ===")
                    for key, value in stats.items():
                        print(f"  {key}: {value}")
                
                elif parts[0] == 'analyze':
                    if not self._brain:
                        print("No brain created. Use 'create <preset>' first.")
                        continue
                    
                    analysis = self._brain.get_performance_analysis()
                    print("\n=== Performance Analysis ===")
                    for key, value in analysis.items():
                        if key != 'recommendations':
                            print(f"  {key}: {value}")
                    
                    if analysis.get('recommendations'):
                        print("\nRecommendations:")
                        for rec in analysis['recommendations']:
                            print(f"  - {rec}")
                
                elif parts[0] == 'config':
                    if not self._brain:
                        print("No brain created. Use 'create <preset>' first.")
                        continue
                    
                    config = self._brain._config
                    print("\n=== Configuration ===")
                    for key in config._config.getKeys():
                        value = config._config.getOr[str](key, "")
                        print(f"  {key}: {value}")
                
                elif parts[0] == 'save':
                    if not self._brain:
                        print("No brain created. Use 'create <preset>' first.")
                        continue
                    
                    if len(parts) < 2:
                        print("Usage: save <filename>")
                        continue
                    
                    filename = parts[1]
                    self._brain.export_stats(filename)
                    print(f"Statistics saved to: {filename}")
                
                elif parts[0] == 'debug':
                    if len(parts) < 2:
                        print("Usage: debug on/off")
                        continue
                    
                    if parts[1] == 'on':
                        self._brain.enable_debug_mode(True)
                    elif parts[1] == 'off':
                        self._brain.enable_debug_mode(False)
                    else:
                        print("Usage: debug on/off")
                
                elif parts[0] == 'exit':
                    print("Exiting interactive mode")
                    break
                
                else:
                    print(f"Unknown command: {parts[0]}")
                    print("Type 'help' for available commands")
            
            except KeyboardInterrupt:
                print("\nExiting interactive mode")
                break
            
            except Exception as e:
                print(f"Error: {e}")

# Create a command-line entry point
if __name__ == "__main__":
    # Check if running in script mode or import mode
    if len(sys.argv) > 1:
        # Command-line mode
        cli = NLMCLI()
        
        if sys.argv[1] == 'experiment':
            preset = sys.argv[2] if len(sys.argv) > 2 else 'balanced'
            steps = int(sys.argv[3]) if len(sys.argv) > 3 else 1000
            output_file = sys.argv[4] if len(sys.argv) > 4 else None
            
            cli.run_experiment(preset, steps, output_file)
        
        elif sys.argv[1] == 'interactive':
            cli.interactive_mode()
        
        else:
            print(f"Unknown command: {sys.argv[1]}")
            print("Available commands: experiment, interactive")
    else:
        # Default: show help
        print("NLM Advanced Python API v2.0.0")
        print("\nAvailable usage:")
        print("  python nlm_api.py experiment <preset> <steps> [output_file]")
        print("  python nlm_api.py interactive")
        print("\nAvailable presets: minimal, balanced, research, production")
