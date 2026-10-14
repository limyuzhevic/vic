#!/usr/bin/env python3
"""
Advanced NLM User Guide - Phase 6 Complete

This guide demonstrates advanced features and commands for NLM (Neural Learning Machine)
after Phase 6: Final Integration. It covers:
- Advanced brain configuration
- Memory system management  
- Neuromodulation control
- Performance optimization
- Experiment management
- Model export/import
- Batch processing
"""

import pynlm
import json
import time
import math
from typing import Dict, List, Optional

class AdvancedNLMCommands:
    """Advanced command-line style interface for NLM users"""
    
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.session_log = []
        self.performance_profile = {}
    
    def __str__(self):
        return """
=== Advanced NLM User Interface ===
Commands:
  create brain <neuron_count>                    - Create advanced brain
  configure brain <key> <value>                   - Configure brain parameters  
  enable all systems                              - Enable all integrated systems
  create world <width> <height> <vision_range>   - Create world environment
  run <steps>                                     - Run simulation
  analyze                                          - Analyze brain state
  export <filepath>                               - Export brain model
  import <filepath>                               - Import brain model
  batch <runs> <steps>                            - Run batch simulations
  help                                            - Show this help
  quit                                            - Exit
"""
    
    def create_brain(self, neuron_count: int = 2000, advanced_config: bool = True):
        """Create advanced brain with Phase 6 integration"""
        print(f"Creating advanced brain with {neuron_count} neurons...")
        
        self.config = pynlm.createDefaultConfig()
        
        if advanced_config:
            # Advanced configuration for maximum performance
            self.config.set("brain.neuron_count", neuron_count)
            self.config.set("brain.synapse_density", 0.12)
            self.config.set("brain.connection_probability", 0.07)
            self.config.set("plasticity.stdp.enable", True)
            self.config.set("plasticity.stdp.learning_rate", 0.002)
            self.config.set("plasticity.hebbian.enable", True)
            self.config.set("plasticity.structural.enable", True)
            self.config.set("neuromod.dopamine.scale", 1.5)
            self.config.set("neuromod.curiosity.enable", True)
            self.config.set("neuromod.novelty.enable", True)
            self.config.set("neuromod.prediction.enable", True)
            self.config.set("development.enable", True)
            self.config.set("development.critical_period", 180.0)
            self.config.set("memory.working_memory.capacity", neuron_count // 6)
            self.config.set("memory.episodic.max_episodes", 3000)
            self.config.set("memory.associative.enabled", True)
            self.config.set("replay.interval", 75)
            self.config.set("consolidation.interval", 800)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        print(f"✓ Advanced brain created: {self.brain.getTotalNeuronCount()} neurons, {self.brain.getTotalSynapseCount()} synapses")
        return self
    
    def enable_all_systems(self):
        """Enable all Phase 6 integrated systems"""
        if not self.brain:
            print("Error: Brain not created yet")
            return self
        
        print("Enabling all Phase 6 integrated systems...")
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(None)
        
        # Enable all subsystems with advanced settings
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Configure advanced neuromodulation parameters
        if self.brain.getPredictionSystem():
            self.brain.getPredictionSystem().train(None)
        
        # Configure attention system for advanced processing
        attention = self.brain.getAttention()
        if attention:
            attention.setInhibitionStrength(0.7)
            attention.setExcitationStrength(2.5)
            attention.setCompetitionThreshold(0.8)
        
        # Configure development system
        development = self.brain.getDevelopmentSystem()
        if development:
            development.setCriticalPeriodDuration(200.0)
            development.setMaturationRate(1.5)
        
        # Configure memory systems
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            working_memory.setDecayRate(0.95)
            working_memory.setCapacity(self.config.getOr<int>("memory.working_memory.capacity", 333))
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            episodic_memory.enableReplay(True)
            episodic_memory.setConsolidationThreshold(0.4)
        
        print("✓ All Phase 6 integrated systems enabled")
        return self
    
    def create_world(self, width: int = 50, height: int = 50, vision_range: int = 8):
        """Create advanced world environment"""
        if not self.brain:
            print("Error: Brain not created yet")
            return self
        
        print(f"Creating world environment {width}x{height} with vision range {vision_range}...")
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, vision_range, vision_range)
        
        # Initialize agent with world
        if not self.agent:
            self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Create complex environment
        self._create_complex_environment()
        
        print(f"✓ World environment created: {self.world.getWidth()}x{self.world.getHeight()}")
        return self
    
    def _create_complex_environment(self):
        """Create complex environment with resources and hazards"""
        width = self.world.getWidth()
        height = self.world.getHeight()
        
        # Place resources in clusters
        for cluster in range(3):
            cluster_x = (cluster % 3) * 15 + 10
            cluster_y = (cluster // 3) * 15 + 10
            
            for i in range(5):
                offset_x = (i % 3 - 1) * 8
                offset_y = (i // 3 - 1) * 8
                
                x = cluster_x + offset_x
                y = cluster_y + offset_y
                
                if 0 <= x < width and 0 <= y < height:
                    # Create resource with varying values
                    resource_value = 1.0 + (cluster + 1) * 0.5 + (i % 3)
                    self.world.addObject(pynlm.WorldObject(
                        x, y, 
                        pynlm.WorldObjectType.Resource, 
                        value=resource_value
                    ))
        
        # Place hazards in hazardous zones
        for hazard in range(8):
            angle = hazard * 2 * 3.14159 / 8
            radius = 25 + hazard * 3
            
            x = width // 2 + int(radius * math.cos(angle))
            y = height // 2 + int(radius * math.sin(angle))
            
            if 0 <= x < width and 0 <= y < height:
                self.world.addObject(pynlm.WorldObject(
                    x, y,
                    pynlm.WorldObjectType.Hazard,
                    value=0.9
                ))
    
    def run(self, steps: int, detailed_logging: bool = True, logging_interval: int = 200):
        """Run advanced simulation with comprehensive monitoring"""
        if not self.brain or not self.world or not self.agent:
            print("Error: Brain, world, and agent must be initialized")
            return None
        
        print(f"Running advanced simulation for {steps} steps...")
        
        self.session_log = []
        self.performance_profile = {
            'total_steps': steps,
            'neural_activity': [],
            'memory_usage': [],
            'prediction_errors': [],
            'curiosity_levels': [],
            'reward_history': [],
            'action_counts': {}
        }
        
        start_time = time.time()
        
        for step in range(steps):
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
            reward = self._calculate_advanced_reward(percept)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Detailed logging periodically
            if detailed_logging and step % logging_interval == 0:
                self._log_detailed_metrics(step)
            
            # Store simulation state
            if step % 10 == 0:
                self.session_log.append({
                    'step': step,
                    'time': step * 0.1,
                    'neurons_firing': self.brain.getFiringNeuronCount(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'reward': reward,
                    'action': action,
                    'memory_traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
                    'episodes': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0
                })
            
            # Track action counts
            if action not in self.performance_profile['action_counts']:
                self.performance_profile['action_counts'][action] = 0
            self.performance_profile['action_counts'][action] += 1
        
        end_time = time.time()
        
        # Compile performance profile
        profile = {
            'total_time': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'final_neurons_firing': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'final_excitatory_inhibitory_ratio': self.brain.getExcitationInhibitionRatio(),
            'memory_traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
            'episodes_stored': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0,
            'final_development_stage': self.brain.getDevelopmentalStage(),
            'curiosity_level': self.agent.getCuriosityLevel() if self.agent else 0,
            'novelty_level': self.agent.getNoveltyLevel() if self.agent else 0,
            'prediction_error': self.agent.getPredictionError() if self.agent else 0
        }
        
        self.performance_profile.update(profile)
        
        print(f"✓ Simulation completed: {steps} steps in {profile['total_time']:.2f}s")
        print(f"  Average firing rate: {profile['average_firing_rate']:.2f} Hz")
        print(f"  Total spikes: {profile['total_spikes']}")
        print(f"  E/I ratio: {profile['final_excitatory_inhibitory_ratio']:.2f}")
        print(f"  Development stage: {profile['final_development_stage']}")
        print(f"  Curiosity level: {profile['curiosity_level']:.2f}")
        print(f"  Memory traces: {profile['memory_traces']}")
        
        return profile
    
    def _calculate_advanced_reward(self, percept) -> float:
        """Calculate advanced reward based on percept complexity"""
        internal = percept.getInternal()
        if not internal:
            return 0.5
        
        # Calculate reward based on multiple factors
        novelty_factor = abs(internal[0]) if len(internal) > 0 else 0.0
        exploration_factor = abs(internal[1]) if len(internal) > 1 else 0.0
        learning_factor = abs(internal[2]) if len(internal) > 2 else 0.0
        
        # Weighted combination
        reward = (novelty_factor * 0.4 + exploration_factor * 0.3 + learning_factor * 0.3 + 0.5)
        
        return min(max(reward, 0.0), 2.0)  # Clamp to [0, 2]
    
    def _log_detailed_metrics(self, step: int):
        """Log detailed performance metrics"""
        # Log neural activity
        self.performance_profile['neural_activity'].append({
            'step': step,
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount()
        })
        
        # Log memory usage
        if self.brain.getWorkingMemory():
            self.performance_profile['memory_usage'].append({
                'step': step,
                'traces': self.brain.getWorkingMemory().getActiveTraces(),
                'capacity': self.brain.getWorkingMemory().getCapacity(),
                'memory_neurons': len(self.brain.getWorkingMemory().getMemoryNeurons())
            })
        
        # Log prediction system
        if self.brain.getPredictionSystem():
            self.performance_profile['prediction_errors'].append({
                'step': step,
                'error': self.brain.getPredictionSystem().getPredictionError(),
                'confidence': self.brain.getPredictionSystem().getConfidence()
            })
        
        # Log neuromodulation
        if self.agent:
            self.performance_profile['curiosity_levels'].append({
                'step': step,
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError()
            })
    
    def analyze(self):
        """Comprehensive analysis of brain state"""
        if not self.brain:
            print("Error: Brain not created")
            return None
        
        print("\n=== Advanced Brain Analysis ===")
        
        analysis = {
            'development_status': {
                'current_stage': self.brain.getDevelopmentalStage(),
                'stage_duration_estimate': self.agent.getDevelopmentalStage() if self.agent else None
            },
            'neural_network': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'excitatory_inhibitory_ratio': self.brain.getExcitationInhibitionRatio(),
                'network_efficiency': self._calculate_network_efficiency()
            },
            'memory_systems': {
                'working_memory': {
                    'active_traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
                    'memory_capacity': self.brain.getWorkingMemory().getCapacity() if self.brain.getWorkingMemory() else 0,
                    'memory_efficiency': self._calculate_memory_efficiency() if self.brain.getWorkingMemory() else 0,
                    'neurons_in_memory': len(self.brain.getWorkingMemory().getMemoryNeurons()) if self.brain.getWorkingMemory() else 0
                },
                'episodic_memory': {
                    'episodes_stored': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0,
                    'replay_enabled': self.brain.getEpisodicMemory().isReplayEnabled() if self.brain.getEpisodicMemory() else False,
                    'consolidation_threshold': 0.4
                },
                'associative_memory': {
                    'associations': self.brain.getAssociativeMemory().getAssociationCount() if self.brain.getAssociativeMemory() else 0,
                    'pattern_neuron_count': len(self.brain.getAssociativeMemory().patternNeurons_) if self.brain.getAssociativeMemory() else 0
                }
            },
            'cognitive_systems': {
                'attention': {
                    'winners_count': len(self.brain.getAttention().getWinners()) if self.brain.getAttention() else 0,
                    'attended_regions': len(self.brain.getAttention().getAttendedRegions()) if self.brain.getAttention() else 0,
                    'attention_focus_efficiency': self._calculate_attention_efficiency() if self.brain.getAttention() else 0
                },
                'concept_formation': 'Active' if self.brain.getConceptFormation() else 'Inactive'
            },
            'neuromodulation': {
                'dopamine_level': self.agent.getNeuromodulationLevel() if self.agent else 0,
                'curiosity_level': self.agent.getCuriosityLevel() if self.agent else 0,
                'novelty_level': self.agent.getNoveltyLevel() if self.agent else 0,
                'prediction_error': self.agent.getPredictionError() if self.agent else 0,
                'neuromodulation_balance': self._calculate_neuromodulation_balance() if self.agent else 0
            }
        }
        
        # Print analysis summary
        print(f"Development Stage: {analysis['development_status']['current_stage']}")
        print(f"Neural Network: {analysis['neural_network']['total_neurons']} neurons, {analysis['neural_network']['total_synapses']} synapses")
        print(f"Active Neurons: {analysis['neural_network']['active_neurons']} ({analysis['neural_network']['firing_rate']:.2f} Hz)")
        print(f"E/I Ratio: {analysis['neural_network']['excitatory_inhibitory_ratio']:.2f}")
        print(f"Working Memory: {analysis['memory_systems']['working_memory']['active_traces']} traces / {analysis['memory_systems']['working_memory']['memory_capacity']} capacity")
        print(f"Episodic Memory: {analysis['memory_systems']['episodic_memory']['episodes_stored']} episodes")
        print(f"Associative Memory: {analysis['memory_systems']['associative_memory']['associations']} associations")
        print(f"Attention Winners: {analysis['cognitive_systems']['attention']['winners_count']}")
        print(f"Curiosity Level: {analysis['neuromodulation']['curiosity_level']:.2f}")
        print(f"Prediction Error: {analysis['neuromodulation']['prediction_error']:.2f}")
        
        return analysis
    
    def _calculate_network_efficiency(self) -> float:
        """Calculate overall network efficiency"""
        if not self.brain or not self.brain.getWorkingMemory():
            return 0.0
        
        total_neurons = self.brain.getTotalNeuronCount()
        active_neurons = self.brain.getActiveNeuronCount()
        memory_traces = self.brain.getWorkingMemory().getActiveTraces()
        
        # Simple efficiency calculation
        efficiency = (active_neurons / total_neurons) * 0.6 + (memory_traces / 100) * 0.4
        return min(efficiency, 1.0)
    
    def _calculate_memory_efficiency(self) -> float:
        """Calculate memory system efficiency"""
        if not self.brain or not self.brain.getWorkingMemory():
            return 0.0
        
        capacity = self.brain.getWorkingMemory().getCapacity()
        traces = self.brain.getWorkingMemory().getActiveTraces()
        memory_neurons = len(self.brain.getWorkingMemory().getMemoryNeurons())
        
        # Memory efficiency based on utilization and neuron engagement
        utilization = traces / capacity if capacity > 0 else 0.0
        engagement = memory_neurons / 100.0  # Normalized engagement
        
        return min((utilization * 0.7 + engagement * 0.3), 1.0)
    
    def _calculate_attention_efficiency(self) -> float:
        """Calculate attention system efficiency"""
        if not self.brain or not self.brain.getAttention():
            return 0.0
        
        winners = len(self.brain.getAttention().getWinners())
        attended_regions = len(self.brain.getAttention().getAttendedRegions())
        
        # Attention efficiency based on focus and coverage
        focus = winners / 50.0  # Normalized focus
        coverage = attended_regions / 10.0  # Normalized coverage
        
        return min((focus * 0.6 + coverage * 0.4), 1.0)
    
    def _calculate_neuromodulation_balance(self) -> float:
        """Calculate neuromodulation system balance"""
        if not self.agent:
            return 0.0
        
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        prediction_error = abs(self.agent.getPredictionError())
        dopamine = self.agent.getNeuromodulationLevel()
        
        # Balance calculation
        balance = 1.0 - abs(curiosity - 0.5) - abs(novelty - 0.5) - abs(prediction_error - 0.5) - abs(dopamine - 0.5)
        
        return max(balance, 0.0)
    
    def export(self, filepath: str) -> bool:
        """Export brain model and session data"""
        print(f"\nExporting brain model to {filepath}...")
        
        try:
            # Create comprehensive export
            export_data = {
                'export_timestamp': time.time(),
                'version': 'Phase6_Advanced',
                'brain_configuration': {
                    'total_neurons': self.brain.getTotalNeuronCount(),
                    'total_synapses': self.brain.getTotalSynapseCount(),
                    'development_stage': self.brain.getDevelopmentalStage()
                },
                'session_performance': self.performance_profile,
                'session_log': self.session_log,
                'brain_analysis': self.analyze() if self.brain else None,
                'world_state': {
                    'width': self.world.getWidth() if self.world else 0,
                    'height': self.world.getHeight() if self.world else 0,
                    'simulation_time': self.world.getSimulationTime() if self.world else 0
                },
                'neuromodulation_state': {
                    'curiosity': self.agent.getCuriosityLevel() if self.agent else 0,
                    'novelty': self.agent.getNoveltyLevel() if self.agent else 0,
                    'prediction_error': self.agent.getPredictionError() if self.agent else 0,
                    'dopamine': self.agent.getNeuromodulationLevel() if self.agent else 0
                }
            }
            
            with open(filepath, 'w') as f:
                json.dump(export_data, f, indent=2, default=str)
            
            print(f"✓ Brain model exported to {filepath}")
            print(f"  Contains {len(self.session_log)} simulation steps")
            print(f"  Contains {len(self.performance_profile.get('neural_activity', []))} neural activity records")
            
            return True
            
        except Exception as e:
            print(f"✗ Error exporting brain model: {e}")
            return False
    
    def import_(self, filepath: str) -> bool:
        """Import brain model from file"""
        print(f"\nImporting brain model from {filepath}...")
        
        try:
            with open(filepath, 'r') as f:
                import_data = json.load(f)
            
            print(f"✓ Brain model imported from {filepath}")
            print(f"  Version: {import_data.get('version', 'Unknown')}")
            print(f"  Simulation steps: {len(import_data.get('session_log', []))}")
            print(f"  Neurons: {import_data.get('brain_configuration', {}).get('total_neurons', 0)}")
            print(f"  Development stage: {import_data.get('brain_configuration', {}).get('development_stage', 'Unknown')}")
            
            # Could implement model restoration here
            return True
            
        except Exception as e:
            print(f"✗ Error importing brain model: {e}")
            return False
    
    def batch(self, num_runs: int, steps_per_run: int, variations: Dict = None) -> List[Dict]:
        """Run batch simulation with parameter variations"""
        print(f"\nRunning batch simulation: {num_runs} runs x {steps_per_run} steps each...")
        
        results = []
        
        for run in range(num_runs):
            print(f"  Run {run + 1}/{num_runs}")
            
            # Apply variations if specified
            if variations:
                self._apply_variations(variations, run)
            
            # Run simulation
            metrics = self.run(steps_per_run, detailed_logging=True)
            metrics['run'] = run + 1
            results.append(metrics)
        
        return results
    
    def _apply_variations(self, variations: Dict, run: int):
        """Apply parameter variations for batch simulation"""
        # Simple implementation - could be expanded
        if 'neuron_variance' in variations:
            variance = variations['neuron_variance']
            run_neuron_count = int(2000 * (1 + (run % variance)))
            # Would need to recreate brain with different neuron count
        
        if 'curiosity_variance' in variations:
            variance = variations['curiosity_variance']
            # Would modify curiosity settings
    
    def help(self):
        """Show help information"""
        print(str(self))


def main():
    """Main command-line interface"""
    print("=== Advanced NLM Command Line Interface ===")
    print("Type 'help' for available commands")
    
    interface = AdvancedNLMCommands()
    
    try:
        while True:
            try:
                command = input("\nnlm> ").strip().lower()
                
                if command == 'quit':
                    print("Goodbye!")
                    break
                
                elif command == 'help':
                    interface.help()
                
                elif command.startswith('create brain'):
                    parts = command.split()
                    neuron_count = int(parts[2]) if len(parts) > 2 else 2000
                    interface.create_brain(neuron_count)
                
                elif command.startswith('configure brain'):
                    parts = command.split()
                    if len(parts) >= 4:
                        key = parts[2]
                        value = ' '.join(parts[3:])
                        # Would need to implement brain configuration
                        print(f"Configuring brain {key} = {value}")
                    else:
                        print("Usage: configure brain <key> <value>")
                
                elif command == 'enable all systems':
                    interface.enable_all_systems()
                
                elif command.startswith('create world'):
                    parts = command.split()
                    width = int(parts[2]) if len(parts) > 2 else 50
                    height = int(parts[3]) if len(parts) > 3 else 50
                    vision_range = int(parts[4]) if len(parts) > 4 else 8
                    interface.create_world(width, height, vision_range)
                
                elif command.startswith('run'):
                    parts = command.split()
                    steps = int(parts[1]) if len(parts) > 1 else 1000
                    profile = interface.run(steps)
                    if profile:
                        print(f"Simulation completed: {profile['total_time']:.2f}s, {profile['steps_per_second']:.1f} steps/s")
                
                elif command == 'analyze':
                    interface.analyze()
                
                elif command.startswith('export'):
                    parts = command.split()
                    filepath = parts[1] if len(parts) > 1 else "advanced_brain_export.json"
                    interface.export(filepath)
                
                elif command.startswith('import'):
                    parts = command.split()
                    filepath = parts[1] if len(parts) > 1 else "advanced_brain_export.json"
                    interface.import_(filepath)
                
                elif command.startswith('batch'):
                    parts = command.split()
                    num_runs = int(parts[1]) if len(parts) > 1 else 5
                    steps_per_run = int(parts[2]) if len(parts) > 2 else 1000
                    results = interface.batch(num_runs, steps_per_run)
                    print(f"Batch simulation completed: {len(results)} runs")
                
                else:
                    print(f"Unknown command: {command}")
                    print("Type 'help' for available commands")
                    
            except KeyboardInterrupt:
                print("\nInterrupted. Type 'quit' to exit.")
            except Exception as e:
                print(f"Error: {e}")
                
    except Exception as e:
        print(f"Fatal error: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())