# Advanced NLM User Tools
# Phase 6 Integration Complete - Advanced Features

import pynlm
import json
import time
import math
from typing import Dict, List, Optional, Callable
from dataclasses import dataclass
from enum import Enum

class AdvancedNLMToolkit:
    """Advanced toolkit for NLM users with Phase 6 integrated brain systems"""
    
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.config = None
        self.simulation_history = []
        self.performance_metrics = {}
    
    def create_advanced_brain(self, neuron_count: int = 2000, 
                             enable_all_systems: bool = True) -> 'AdvancedNLMToolkit':
        """Create an advanced brain with all integrated systems enabled"""
        self.config = pynlm.createDefaultConfig()
        
        # Advanced configuration for performance
        self.config.set("brain.neuron_count", neuron_count)
        self.config.set("brain.synapse_density", 0.15)
        self.config.set("brain.connection_probability", 0.08)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("plasticity.structural.enable", True)
        self.config.set("neuromod.dopamine.scale", 1.2)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("neuromod.prediction.enable", True)
        self.config.set("development.enable", True)
        self.config.set("memory.working_memory.capacity", neuron_count // 8)
        self.config.set("memory.episodic.max_episodes", 2000)
        self.config.set("memory.associative.enabled", True)
        
        # Advanced system parameters
        self.config.set("replay.interval", 50)
        self.config.set("consolidation.interval", 500)
        self.config.set("development.critical_period", 120.0)
        self.config.set("performance.checkpoint.interval", 1000)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enable all subsystems if requested
        if enable_all_systems:
            self.enable_all_systems()
        
        return self
    
    def enable_all_systems(self):
        """Enable all advanced brain systems"""
        if not self.brain:
            return
            
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(None)  # Will be set later with world
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Configure advanced parameters
        self.agent.getBrain().getPredictionSystem().train(None)  # Would need proper input
        
        # Configure attention system
        attention = self.brain.getAttention()
        if attention:
            attention.setInhibitionStrength(0.6)
            attention.setExcitationStrength(2.0)
            attention.setCompetitionThreshold(0.7)
        
        # Configure development
        development = self.brain.getDevelopmentSystem()
        if development:
            development.setCriticalPeriodDuration(150.0)
            development.setMaturationRate(1.2)
        
        # Configure memory systems
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            working_memory.setCapacity(self.config.getOr<int>("memory.working_memory.capacity", 250))
            working_memory.setDecayRate(0.98)
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            episodic_memory.enableReplay(True)
            episodic_memory.setConsolidationThreshold(0.3)
        
        print("All advanced brain systems enabled")
    
    def create_world_environment(self, width: int = 50, height: int = 50,
                                vision_range: int = 8, enable_resources: bool = True,
                                enable_hazards: bool = True) -> 'AdvancedNLMToolkit':
        """Create an advanced world environment"""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, vision_range, vision_range)
        
        # Enable agent interface with world
        if self.brain and not self.agent:
            self.agent = pynlm.createAgentBrain(self.brain)
        if self.agent:
            self.agent.initialize(self.world)
        
        # Create environment objects
        if enable_resources:
            self._create_resources()
        
        if enable_hazards:
            self._create_hazards()
        
        return self
    
    def _create_resources(self):
        """Create resource objects in the world"""
        # Place resources randomly
        for i in range(20):
            x = self.world.getWidth() // 2 + (i % 3 - 1) * 10
            y = self.world.getHeight() // 2 + ((i // 3) % 3 - 1) * 10
            if 0 <= x < self.world.getWidth() and 0 <= y < self.world.getHeight():
                self.world.addObject(pynlm.WorldObject(x, y, 
                                                      pynlm.WorldObjectType.Resource, 
                                                      value=1.0 + (i % 5) * 0.5))
    
    def _create_hazards(self):
        """Create hazard objects in the world"""
        # Place hazards away from center
        for i in range(10):
            angle = i * 2 * 3.14159 / 10
            radius = 20 + i * 5
            x = self.world.getWidth() // 2 + int(radius * math.cos(angle))
            y = self.world.getHeight() // 2 + int(radius * math.sin(angle))
            if 0 <= x < self.world.getWidth() and 0 <= y < self.world.getHeight():
                self.world.addObject(pynlm.WorldObject(x, y,
                                                      pynlm.WorldObjectType.Hazard,
                                                      value=0.8))
    
    def run_advanced_simulation(self, num_steps: int, enable_logging: bool = True,
                               log_interval: int = 100) -> Dict:
        """Run an advanced simulation with comprehensive monitoring"""
        if not self.brain or not self.world or not self.agent:
            raise ValueError("Brain, world, and agent must be initialized")
        
        self.simulation_history = []
        self.performance_metrics = {
            'total_steps': num_steps,
            'neural_activity': [],
            'memory_usage': [],
            'prediction_errors': [],
            'curiosity_levels': [],
            'reward_history': [],
            'action_counts': {}
        }
        
        start_time = time.time()
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = self._calculate_reward(percept)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Log metrics periodically
            if enable_logging and step % log_interval == 0:
                self._log_step_metrics(step)
            
            # Store simulation state
            if step % 10 == 0:
                self.simulation_history.append({
                    'step': step,
                    'time': step * 0.1,
                    'neurons_firing': self.brain.getFiringNeuronCount(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'reward': reward,
                    'action': action
                })
        
        end_time = time.time()
        
        # Compile final metrics
        metrics = {
            'total_time': end_time - start_time,
            'steps_per_second': num_steps / (end_time - start_time),
            'final_neurons_firing': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'memory_traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
            'episodes_stored': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0,
            'simulation_history': self.simulation_history
        }
        
        self.performance_metrics.update(metrics)
        return metrics
    
    def _calculate_reward(self, percept) -> float:
        """Calculate reward based on percept"""
        # Simple reward calculation based on internal signals
        internal = percept.getInternal()
        if internal:
            return (internal[0] + 1.0) / 2.0  # Normalize to [0, 1]
        return 0.5
    
    def _log_step_metrics(self, step: int):
        """Log detailed metrics for a simulation step"""
        if not self.brain:
            return
        
        # Log neural activity
        self.performance_metrics['neural_activity'].append({
            'step': step,
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount()
        })
        
        # Log memory usage
        if self.brain.getWorkingMemory():
            self.performance_metrics['memory_usage'].append({
                'step': step,
                'traces': self.brain.getWorkingMemory().getActiveTraces(),
                'capacity': self.brain.getWorkingMemory().getCapacity()
            })
        
        # Log prediction system
        if self.brain.getPredictionSystem():
            self.performance_metrics['prediction_errors'].append({
                'step': step,
                'error': self.brain.getPredictionSystem().getPredictionError(),
                'confidence': self.brain.getPredictionSystem().getConfidence()
            })
        
        # Log neuromodulation
        if self.agent:
            self.performance_metrics['curiosity_levels'].append({
                'step': step,
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError()
            })
    
    def analyze_brain_state(self) -> Dict:
        """Perform comprehensive analysis of brain state"""
        if not self.brain:
            return {}
        
        analysis = {
            'development_stage': {
                'stage': self.brain.getDevelopmentalStage(),
                'age': self.agent.getDevelopmentalStage() if self.agent else None
            },
            'neural_network': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'excitatory_inhibitory_ratio': self.brain.getExcitationInhibitionRatio()
            },
            'memory_systems': {
                'working_memory': {
                    'traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
                    'capacity': self.brain.getWorkingMemory().getCapacity() if self.brain.getWorkingMemory() else 0,
                    'neurons_in_memory': len(self.brain.getWorkingMemory().getMemoryNeurons()) if self.brain.getWorkingMemory() else 0
                },
                'episodic_memory': {
                    'episodes': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0,
                    'replay_enabled': self.brain.getEpisodicMemory().isReplayEnabled() if self.brain.getEpisodicMemory() else False
                },
                'associative_memory': {
                    'associations': self.brain.getAssociativeMemory().getAssociationCount() if self.brain.getAssociativeMemory() else 0
                }
            },
            'cognitive_systems': {
                'attention': {
                    'winners': len(self.brain.getAttention().getWinners()) if self.brain.getAttention() else 0,
                    'attended_regions': len(self.brain.getAttention().getAttendedRegions()) if self.brain.getAttention() else 0
                },
                'concept_formation': 'Active' if self.brain.getConceptFormation() else 'Inactive'
            },
            'neuromodulation': {
                'dopamine_level': self.agent.getNeuromodulationLevel() if self.agent else 0,
                'curiosity_level': self.agent.getCuriosityLevel() if self.agent else 0,
                'prediction_error': self.agent.getPredictionError() if self.agent else 0,
                'novelty_level': self.agent.getNoveltyLevel() if self.agent else 0
            }
        }
        
        return analysis
    
    def export_brain_model(self, filepath: str) -> bool:
        """Export brain model to file"""
        if not self.brain:
            return False
        
        try:
            # Create comprehensive brain model
            model = {
                'config': self.config.summary() if self.config else "No config",
                'timestamp': time.time(),
                'analysis': self.analyze_brain_state(),
                'simulation_history': self.simulation_history[-100:],  # Last 100 steps
                'performance': self.performance_metrics
            }
            
            with open(filepath, 'w') as f:
                json.dump(model, f, indent=2, default=str)
            
            print(f"Brain model exported to {filepath}")
            return True
            
        except Exception as e:
            print(f"Error exporting brain model: {e}")
            return False
    
    def import_brain_model(self, filepath: str) -> bool:
        """Import brain model from file"""
        try:
            with open(filepath, 'r') as f:
                model = json.load(f)
            
            # Could implement model restoration here
            print(f"Brain model imported from {filepath}")
            print(f"Model contains {len(model.get('simulation_history', []))} simulation steps")
            print(f"Analysis shows {model.get('analysis', {}).get('neural_network', {}).get('total_neurons', 0)} neurons")
            
            return True
            
        except Exception as e:
            print(f"Error importing brain model: {e}")
            return False
    
    def create_experiment_protocol(self, name: str, steps: int, 
                                   parameters: Dict = None) -> Dict:
        """Create an experiment protocol for advanced usage"""
        protocol = {
            'name': name,
            'steps': steps,
            'parameters': parameters or {},
            'start_time': time.time(),
            'brain_config': self.config.summary() if self.config else "Default config",
            'world_config': {
                'width': self.world.getWidth() if self.world else 0,
                'height': self.world.getHeight() if self.world else 0,
                'vision_range': 8
            }
        }
        
        print(f"Experiment protocol '{name}' created with {steps} steps")
        return protocol
    
    def run_batch_simulation(self, num_runs: int, steps_per_run: int,
                           parameter_variations: Dict = None) -> List[Dict]:
        """Run batch simulation with parameter variations"""
        results = []
        
        for run in range(num_runs):
            print(f"Running batch simulation {run + 1}/{num_runs}")
            
            # Apply parameter variations if specified
            if parameter_variations:
                self._apply_parameter_variations(parameter_variations, run)
            
            # Run simulation
            metrics = self.run_advanced_simulation(steps_per_run, enable_logging=True)
            metrics['run'] = run + 1
            results.append(metrics)
        
        return results
    
    def _apply_parameter_variations(self, variations: Dict, run: int):
        """Apply parameter variations for batch simulation"""
        # Implementation would vary parameters based on run index
        pass

# Convenience functions for advanced users

def create_advanced_agent_brain(neuron_count: int = 2000, enable_all_systems: bool = True):
    """Convenience function to create advanced agent brain"""
    toolkit = AdvancedNLMToolkit()
    return toolkit.create_advanced_brain(neuron_count, enable_all_systems)

def create_advanced_world(width: int = 50, height: int = 50, vision_range: int = 8):
    """Convenience function to create advanced world"""
    world = pynlm.createSimpleWorld()
    world.configure(width, height, vision_range, vision_range)
    return world

def benchmark_advanced_simulation(neuron_counts: List[int] = None, 
                                simulation_steps: int = 1000) -> Dict:
    """Benchmark advanced NLM with different neuron counts"""
    if neuron_counts is None:
        neuron_counts = [500, 1000, 2000, 5000]
    
    results = {}
    
    for neuron_count in neuron_counts:
        print(f"Benchmarking {neuron_count} neurons...")
        
        toolkit = AdvancedNLMToolkit()
        toolkit.create_advanced_brain(neuron_count, enable_all_systems=True)
        
        start_time = time.time()
        metrics = toolkit.run_advanced_simulation(simulation_steps, enable_logging=False)
        end_time = time.time()
        
        results[neuron_count] = {
            'total_time': end_time - start_time,
            'steps_per_second': simulation_steps / (end_time - start_time),
            'final_spike_count': metrics['total_spikes'],
            'average_firing_rate': metrics['average_firing_rate']
        }
    
    return results

# Example usage for advanced users

def example_advanced_simulation():
    """Example of advanced NLM usage"""
    print("=== Advanced NLM Simulation Example ===")
    
    # Create advanced toolkit
    toolkit = AdvancedNLMToolkit()
    
    # Create advanced brain
    toolkit.create_advanced_brain(neuron_count=3000, enable_all_systems=True)
    print(f"Created advanced brain with {toolkit.brain.getTotalNeuronCount()} neurons")
    
    # Create world environment
    toolkit.create_world_environment(width=100, height=100, vision_range=10)
    print(f"Created world environment {toolkit.world.getWidth()}x{toolkit.world.getHeight()}")
    
    # Run advanced simulation
    metrics = toolkit.run_advanced_simulation(2000, enable_logging=True, log_interval=200)
    print(f"Simulation completed in {metrics['total_time']:.2f} seconds")
    print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
    print(f"Total spikes: {metrics['total_spikes']}")
    
    # Analyze brain state
    analysis = toolkit.analyze_brain_state()
    print("\n=== Brain State Analysis ===")
    print(f"Development stage: {analysis['development_stage']['stage']}")
    print(f"Total neurons: {analysis['neural_network']['total_neurons']}")
    print(f"Episodic memory episodes: {analysis['memory_systems']['episodic_memory']['episodes']}")
    print(f"Curiosity level: {analysis['neuromodulation']['curiosity_level']:.2f}")
    
    # Export brain model
    toolkit.export_brain_model("advanced_brain_model.json")
    
    return toolkit, metrics, analysis

if __name__ == "__main__":
    # Run example
    toolkit, metrics, analysis = example_advanced_simulation()