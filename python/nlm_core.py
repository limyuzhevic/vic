# Enhanced Python Bindings for NLM (Neural Learning Machine)
# 
# This module provides a comprehensive Python interface to the NLM C++ neural simulation framework.
# It includes both direct bindings to the C++ API and enhanced Python utilities for easier usage.

from typing import List, Dict, Any, Optional, Union, Callable
import warnings
import sys
import os

# Import the C++ bindings
try:
    # Try to import from pynlm module
    import pynlm._pynlm as _pynlm
    HAS_PYNLM = True
except ImportError:
    try:
        # Try to import directly from compiled module
        import pynlm as _pynlm
        HAS_PYNLM = True
    except ImportError:
        HAS_PYNLM = False
        raise ImportError("NLM C++ bindings not found. Please build the package first.")

# Re-export all C++ types for convenience
from pynlm import (
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    SimulationStep,
    Timestamp,
)

# Import Core Classes
from pynlm import (
    Config,
    Brain,
    SimpleWorld,
    AgentBrain,
)

# Import Sensory Classes
from pynlm import (
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
)

# Import Motor Classes
from pynlm import (
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
)

# Import Agent Classes
from pynlm import (
    SensoryPercept,
)

# ==============================================================================
# Enhanced Python Types and Wrappers
# ==============================================================================

class WorkingMemoryEntry:
    """Entry in working memory with enhanced Python features"""
    def __init__(self, content: str, activation: float = 1.0, timestamp: float = 0.0):
        self.content = content
        self.activation = activation
        self.timestamp = timestamp
    
    def __repr__(self):
        return f"WorkingMemoryEntry(content='{self.content}', activation={self.activation:.3f}, timestamp={self.timestamp})"
    
    def __eq__(self, other):
        return (isinstance(other, WorkingMemoryEntry) and 
                self.content == other.content and 
                abs(self.activation - other.activation) < 0.001)

class EpisodicEntry:
    """Entry in episodic memory with enhanced tracking"""
    def __init__(self, episode_id: str, context: Dict[str, Any], outcome: Dict[str, Any], timestamp: float = 0.0):
        self.episode_id = episode_id
        self.context = context
        self.outcome = outcome
        self.timestamp = timestamp
    
    def __repr__(self):
        return f"EpisodicEntry(id='{self.episode_id}', context={self.context}, outcome={self.outcome})"
    
    def get_reward(self) -> float:
        """Extract reward from outcome"""
        return self.outcome.get('reward', 0.0)

class AssociativeMemoryEntry:
    """Entry in associative memory with Hebbian-style strengthening"""
    def __init__(self, key: str, value: str, strength: float = 0.5, timestamp: float = 0.0):
        self.key = key
        self.value = value
        self.strength = strength
        self.timestamp = timestamp
    
    def __repr__(self):
        return f"AssociativeMemoryEntry(key='{self.key}', value='{self.value}', strength={self.strength:.3f})"
    
    def strengthen(self, amount: float = 0.1):
        """Strengthen the association"""
        self.strength = min(1.0, self.strength + amount)
    
    def weaken(self, amount: float = 0.1):
        """Weaken the association"""
        self.strength = max(0.0, self.strength - amount)

class NeuromodulationState:
    """Current neuromodulation state with validation"""
    def __init__(self):
        self.dopamine = 0.1
        self.curiosity = 0.5
        self.novelty = 0.3
        self.prediction_error = 0.2
        self.rewards = []
    
    def __repr__(self):
        return (f"NeuromodulationState(dopamine={self.dopamine:.3f}, "
                f"curiosity={self.curiosity:.3f}, novelty={self.novelty:.3f}, "
                f"prediction_error={self.prediction_error:.3f})")
    
    def validate(self):
        """Validate neuromodulation values are in expected range"""
        if not 0.0 <= self.dopamine <= 1.0:
            warnings.warn(f"Dopamine value {self.dopamine} outside expected range [0.0, 1.0]")
        if not 0.0 <= self.curiosity <= 1.0:
            warnings.warn(f"Curiosity value {self.curiosity} outside expected range [0.0, 1.0]")
        if not 0.0 <= self.novelty <= 1.0:
            warnings.warn(f"Novelty value {self.novelty} outside expected range [0.0, 1.0]")
        if not 0.0 <= self.prediction_error <= 1.0:
            warnings.warn(f"Prediction error value {self.prediction_error} outside expected range [0.0, 1.0]")

class PredictionEntry:
    """Prediction made by the prediction system"""
    def __init__(self, action: str, predicted_reward: float, confidence: float, timestep: int):
        self.action = action
        self.predicted_reward = predicted_reward
        self.confidence = confidence
        self.timestep = timestep
    
    def __repr__(self):
        return f"PredictionEntry(action='{self.action}', predicted_reward={self.predicted_reward:.3f}, confidence={self.confidence:.3f})"

class DevelopmentalStageInfo:
    """Enhanced developmental stage information"""
    def __init__(self, stage: DevelopmentalStage, age: int, plasticity_multiplier: float, critical_period: bool = False):
        self.stage = stage
        self.age = age
        self.plasticity_multiplier = plasticity_multiplier
        self.critical_period = critical_period
    
    def __repr__(self):
        return (f"DevelopmentalStageInfo(stage={self.stage.name}, age={self.age}, "
                f"plasticity_multiplier={self.plasticity_multiplier:.3f}, "
                f"critical_period={self.critical_period})")
    
    def is_critical(self) -> bool:
        """Check if current stage is a critical period"""
        return self.critical_period or self.stage.name == "CriticalPeriod"

# ==============================================================================
# Enhanced Wrapper Functions
# ==============================================================================

def createDefaultConfig() -> Config:
    """Create a default configuration with validation"""
    config = Config(_pynlm.createDefaultConfig())
    print("✓ Default configuration created")
    return config

def createBrain(config: Config) -> Brain:
    """Create a new brain with configuration and enhanced initialization"""
    if not config:
        raise ValueError("Config cannot be None")
    brain = Brain(_pynlm.createBrain(config._impl), config)
    brain.initialize()
    print("✓ Brain initialized successfully")
    return brain

def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world"""
    world = SimpleWorld(_pynlm.createSimpleWorld())
    print("✓ Simple world created")
    return world

def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create a new agent brain interface"""
    if not brain:
        raise ValueError("Brain cannot be None")
    agent_brain = AgentBrain(_pynlm.createAgentBrain(brain._impl), brain)
    agent_brain.initialize()
    print("✓ Agent brain initialized")
    return agent_brain

# ==============================================================================
# Convenience and Utility Functions
# ==============================================================================

class NLMUtils:
    """Utility functions for NLM with enhanced features"""
    
    @staticmethod
    def print_banner():
        """Print the NLM banner with enhanced formatting"""
        print("""
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     Enhanced Python API with utilities.                       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
        """)
    
    @staticmethod
    def is_learning_detected(brain: Brain, initial_mean: float, final_mean: float, 
                           strengthened: int, weakened: int) -> bool:
        """Determine if learning occurred based on enhanced metrics"""
        return (abs(final_mean - initial_mean) > 0.001 or 
               (strengthened > 0 or weakened > 0))
    
    @staticmethod
    def create_experiment_config(**kwargs) -> Config:
        """Create a configuration optimized for experiments with validation"""
        config = createDefaultConfig()
        
        # Enhanced parameter mapping with validation
        param_mapping = {
            'neuron_count': 'neuron_count',
            'simulation_timestep': 'simulation_timestep',
            'random_seed': 'random_seed',
            'plasticity_learning_rate': 'plasticity_learning_rate',
            'stdp_ltp_weight': 'stdp_ltp_weight',
            'stdp_ltd_weight': 'stdp_ltd_weight',
            'working_memory_capacity': 'working_memory_capacity',
            'episodic_memory_max_episodes': 'episodic_memory_max_episodes',
            'neuromod_curiosity': 'neuromod_curiosity',
            'environment_width': 'environment_width',
            'environment_height': 'environment_height',
        }
        
        for key, config_key in param_mapping.items():
            if key in kwargs:
                value = kwargs[key]
                config.set(config_key, value)
                
                # Validate parameter ranges
                if key == 'neuron_count' and value <= 0:
                    raise ValueError("neuron_count must be positive")
                if key == 'simulation_timestep' and value <= 0:
                    raise ValueError("simulation_timestep must be positive")
                if key == 'working_memory_capacity' and value < 0:
                    raise ValueError("working_memory_capacity must be non-negative")
        
        return config
    
    @staticmethod
    def validate_config(config: Config) -> List[str]:
        """Validate configuration and return list of warnings"""
        warnings_list = []
        
        # Check for common configuration issues
        if config.get('neuron_count') and config.get('neuron_count') < 10:
            warnings_list.append("Low neuron count may affect network dynamics")
        
        if config.get('simulation_timestep') and config.get('simulation_timestep') > 0.1:
            warnings_list.append("Large timestep may cause numerical instability")
        
        if config.get('connection_probability') and config.get('connection_probability') > 0.5:
            warnings_list.append("High connection probability may create overly dense network")
        
        return warnings_list

# ==============================================================================
# Enhanced Memory System Integration
# ==============================================================================

class MemoryIntegrator:
    """Enhanced memory system integration with better management"""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self.working_memory = []
        self.episodic_memory = []
        self.associative_memory = {}
        self.memory_stats = {}
    
    def store_working_memory(self, content: str, activation: float = 1.0, metadata: Dict[str, Any] = None):
        """Store information in working memory with metadata"""
        if metadata is None:
            metadata = {}
        
        entry = WorkingMemoryEntry(content, activation, self.brain.getTotalSpikeCount())
        entry.metadata = metadata
        
        # Check if similar content already exists
        for existing in self.working_memory:
            if existing.content == content:
                existing.activation = max(existing.activation, activation)
                existing.timestamp = entry.timestamp
                return
        
        self.working_memory.append(entry)
        
        # Apply capacity limit
        if len(self.working_memory) > 50:
            # Remove lowest activation entries
            self.working_memory.sort(key=lambda x: x.activation)
            self.working_memory = self.working_memory[-50:]
        
        # Update statistics
        self._update_memory_stats()
    
    def store_episodic_memory(self, context: Dict[str, Any], outcome: Dict[str, Any], 
                             additional_data: Dict[str, Any] = None):
        """Store experience in episodic memory with enhanced tracking"""
        if additional_data is None:
            additional_data = {}
        
        episode_id = f"ep_{len(self.episodic_memory)}_{self.brain.getTotalSpikeCount()}"
        context.update(additional_data)
        entry = EpisodicEntry(episode_id, context, outcome, self.brain.getTotalSpikeCount())
        
        self.episodic_memory.append(entry)
        
        # Apply capacity limit
        if len(self.episodic_memory) > 100:
            self.episodic_memory.pop(0)
        
        # Update statistics
        self._update_memory_stats()
    
    def store_associative_memory(self, key: str, value: str, strength: float = 0.5, 
                               context: Dict[str, Any] = None):
        """Store association in memory with context"""
        if context is None:
            context = {}
        
        entry = AssociativeMemoryEntry(key, value, strength, self.brain.getTotalSpikeCount())
        entry.context = context
        
        self.associative_memory[key] = entry
        
        # Apply capacity limit
        if len(self.associative_memory) > 200:
            # Remove weakest associations
            sorted_items = sorted(self.associative_memory.items(), 
                                key=lambda x: x[1].strength)
            self.associative_memory = dict(sorted_items[-200:])
        
        # Update statistics
        self._update_memory_stats()
    
    def retrieve_working_memory(self, threshold: float = 0.1, limit: int = 10) -> List[WorkingMemoryEntry]:
        """Retrieve active working memory entries sorted by activation"""
        filtered = [entry for entry in self.working_memory if entry.activation > threshold]
        filtered.sort(key=lambda x: x.activation, reverse=True)
        return filtered[:limit]
    
    def retrieve_associative_memory(self, key: str) -> Optional[AssociativeMemoryEntry]:
        """Retrieve associative memory entry by key"""
        return self.associative_memory.get(key)
    
    def search_associative_memory(self, pattern: str) -> List[AssociativeMemoryEntry]:
        """Search associative memory for keys matching pattern"""
        results = []
        for entry in self.associative_memory.values():
            if pattern.lower() in entry.key.lower():
                results.append(entry)
        results.sort(key=lambda x: x.strength, reverse=True)
        return results
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get comprehensive memory system statistics"""
        return self.memory_stats.copy()
    
    def _update_memory_stats(self):
        """Update memory statistics"""
        self.memory_stats = {
            'working_memory_count': len(self.working_memory),
            'episodic_memory_count': len(self.episodic_memory),
            'associative_memory_count': len(self.associative_memory),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'learning_rate': self.brain.getAverageFiringRate() / max(1, len(self.working_memory)),
        }

# ==============================================================================
# Enhanced Experiment Runner
# ==============================================================================

class ExperimentRunner:
    """Enhanced experiment runner with comprehensive reporting"""
    
    def __init__(self, brain: Brain, world: SimpleWorld = None):
        self.brain = brain
        self.world = world
        self.results = []
        self.experiment_metadata = {}
    
    def set_metadata(self, **metadata):
        """Set experiment metadata"""
        self.experiment_metadata.update(metadata)
    
    def run_connectivity_test(self, neurons_to_stimulate: int = None, 
                           current_intensity: float = 30.0f) -> Dict[str, Any]:
        """Enhanced connectivity test with configurable parameters"""
        if neurons_to_stimulate is None:
            neurons_to_stimulate = min(20, max(1, self.brain.getTotalNeuronCount() // 4))
        
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Get available neurons
        region = self.brain.getRegion(RegionId(1))
        if region:
            neurons = region.getAllNeurons()
            for i in range(min(neurons_to_stimulate, len(neurons))):
                neurons[i].injectCurrent(current_intensity)
        
        # Run simulation
        for step in range(50):
            self.brain.step(step, step * 0.001)
        
        spikes = self.brain.getTotalSpikeCount() - initial_spikes
        
        result = {
            'type': 'connectivity_test',
            'neurons_stimulated': neurons_to_stimulate,
            'current_intensity': current_intensity,
            'spikes_generated': spikes,
            'success': spikes > 0,
            'firing_rate': self.brain.getAverageFiringRate(),
            'metadata': self.experiment_metadata.copy()
        }
        
        self.results.append(result)
        return result
    
    def run_plasticity_experiment(self, steps: int = 1000, 
                                 input_pattern_size: int = None,
                                 current_intensity: float = 30.0f) -> Dict[str, Any]:
        """Enhanced plasticity experiment with configurable parameters"""
        import time
        import numpy as np
        
        start_time = time.time()
        
        # Record initial state
        initial_synapse_count = self.brain.getTotalSynapseCount()
        initial_weights = []
        if region := self.brain.getRegion(RegionId(1)):
            for syn in region.getSynapses():
                initial_weights.append(syn.getWeight())
        
        initial_mean = np.mean(initial_weights) if initial_weights else 0
        initial_std = np.std(initial_weights) if len(initial_weights) > 1 else 0
        
        # Enable plasticity
        if region := self.brain.getRegion(RegionId(1)):
            for syn in region.getSynapses():
                syn.enablePlasticity(True, True, False)
        
        # Apply enhanced input patterns
        if input_pattern_size is None:
            input_pattern_size = min(20, max(1, self.brain.getTotalNeuronCount() // 4))
        
        for step in range(steps):
            # Create dynamic input pattern
            for i in range(input_pattern_size):
                # Vary input strength over time
                dynamic_intensity = current_intensity * (1 + 0.1 * np.sin(2 * np.pi * step / 100))
                self.brain.injectCurrentToNeurons(NeuronType.Sensory, dynamic_intensity)
            
            self.brain.step(step, step * 0.001)
            
            # Log progress at intervals
            if step % 200 == 0:
                # Store intermediate state
                pass
        
        # Record final state
        final_weights = []
        if region := self.brain.getRegion(RegionId(1)):
            for syn in region.getSynapses():
                final_weights.append(syn.getWeight())
        
        final_mean = np.mean(final_weights) if final_weights else 0
        final_std = np.std(final_weights) if len(final_weights) > 1 else 0
        
        # Count changes with enhanced criteria
        strengthened = weakened = unchanged = 0
        min_size = min(len(initial_weights), len(final_weights))
        
        for i in range(min_size):
            delta = final_weights[i] - initial_weights[i]
            if delta > 0.01:
                strengthened += 1
            elif delta < -0.01:
                weakened += 1
            else:
                unchanged += 1
        
        end_time = time.time()
        
        # Enhanced learning detection
        learning_indicators = {
            'mean_change': final_mean - initial_mean,
            'std_change': final_std - initial_std,
            'strengthened_count': strengthened,
            'weakened_count': weakened,
            'total_changes': strengthened + weakened,
            'stability': unchanged / min_size if min_size > 0 else 1.0
        }
        
        result = {
            'type': 'plasticity_experiment',
            'steps_completed': steps,
            'duration': end_time - start_time,
            'input_pattern_size': input_pattern_size,
            'current_intensity': current_intensity,
            'initial_synapse_count': initial_synapse_count,
            'final_synapse_count': self.brain.getTotalSynapseCount(),
            'initial_mean_weight': initial_mean,
            'final_mean_weight': final_mean,
            'initial_std_weight': initial_std,
            'final_std_weight': final_std,
            'weight_change': final_mean - initial_mean,
            'strengthened': strengthened,
            'weakened': weakened,
            'unchanged': unchanged,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'learning_detected': (abs(final_mean - initial_mean) > 0.001 or 
                                 (strengthened > 0 or weakened > 0)),
            'learning_indicators': learning_indicators,
            'metadata': self.experiment_metadata.copy()
        }
        
        self.results.append(result)
        return result
    
    def run_stdp_verification(self, pre_post_pairs: int = 5) -> Dict[str, Any]:
        """Enhanced STDP verification with multiple neuron pairs"""
        if not (region := self.brain.getRegion(RegionId(1))) or region.getSynapses().size() < pre_post_pairs:
            return {'type': 'stdp_verification', 'error': 'Not enough synapses for test', 
                   'metadata': self.experiment_metadata.copy()}
        
        auto& synapses = region.getSynapses();
        
        # Get neurons for testing
        auto neurons = region.getAllNeurons();
        if neurons.size() < 2:
            return {'type': 'stdp_verification', 'error': 'Not enough neurons for test',
                   'metadata': self.experiment_metadata.copy()}
        
        # Record initial weights for multiple synapses
        std::vector<float> beforeWeights;
        beforeWeights.reserve(pre_post_pairs);
        
        for i in range(pre_post_pairs):
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        
        # Create enhanced correlated activity
        for trial in range(50):
            # Create varied timing between pre and post
            pre_delay = trial * 2;
            post_delay = pre_delay + (1 + 0.1 * (trial % 3 - 1))  # Variable timing
            
            if neurons.size() >= 2:
                // Fire pre-synaptic neuron
                neurons[0]->injectCurrent(60.0f);
                self.brain.step(pre_delay, pre_delay * 0.001);
                
                // Fire post-synaptic neuron
                neurons[1]->injectCurrent(60.0f);
                self.brain.step(post_delay, post_delay * 0.001);
        
        // Record after weights
        float totalDelta = 0.0f;
        std::vector<float> afterWeights;
        afterWeights.reserve(pre_post_pairs);
        
        for i in range(pre_post_pairs) {
            float after = synapses[i]->getWeight();
            float before = beforeWeights[i];
            float delta = after - before;
            totalDelta += delta;
            afterWeights.push_back(after);
        }
        
        return {
            'type': 'stdp_verification',
            'synapses_tested': pre_post_pairs,
            'total_delta': totalDelta,
            'average_delta': totalDelta / pre_post_pairs,
            ' potentiation': totalDelta > 0.001,
            'depression': totalDelta < -0.001,
            'no_change': abs(totalDelta) <= 0.001,
            'metadata': self.experiment_metadata.copy()
        }
    
    def run_complete_experiment_suite(self, save_intermediates: bool = False) -> List[Dict[str, Any]]:
        """Run complete suite of verification experiments with intermediate saving"""
        results = []
        
        // Reset brain between tests
        self.brain.reset();
        self.brain.initialize();
        
        // Save initial state if requested
        if save_intermediates:
            self.brain.save("experiment_initial_state.nlm")
        
        // Test 1: Basic connectivity
        results.append(self.run_connectivity_test())
        
        // Test 2: Reset and run plasticity experiment
        self.brain.reset();
        self.brain.initialize();
        if save_intermediates:
            self.brain.save("experiment_after_connectivity.nlm")
        results.append(self.run_plasticity_experiment())
        
        // Test 3: Reset and run STDP verification
        self.brain.reset();
        self.brain.initialize();
        if save_intermediates:
            self.brain.save("experiment_after_plasticity.nlm")
        results.append(self.run_stdp_verification())
        
        // Save final state if requested
        if save_intermediates:
            self.brain.save("experiment_final_state.nlm")
        
        return results
    
    def get_experiment_summary(self) -> Dict[str, Any]:
        """Get comprehensive experiment summary"""
        if not self.results:
            return {'status': 'no_experiments_run'}
        
        summary = {
            'experiment_count': len(self.results),
            'experiment_types': list(set(r['type'] for r in self.results)),
            'successful_experiments': sum(1 for r in self.results if r.get('success', True)),
            'learning_detected': any(r.get('learning_detected', False) for r in self.results),
            'total_spikes': sum(r.get('total_spikes', 0) for r in self.results),
            'metadata': self.experiment_metadata.copy()
        }
        
        // Add type-specific metrics
        for result in self.results:
            if result['type'] == 'plasticity_experiment':
                summary['plasticity_learning_rate'] = result['learning_detected']
                summary['plasticity_weight_change'] = result['weight_change']
            elif result['type'] == 'stdp_verification':
                summary['stdp_working'] = result.get('potentiation', False) or result.get('depression', False)
        
        return summary

# ==============================================================================
# Main NLM Class (Enhanced Wrapper)
# ==============================================================================

class NLM:
    """Main NLM interface class with enhanced functionality"""
    
    def __init__(self, config: Optional[Config] = None, world: Optional[SimpleWorld] = None):
        """Initialize NLM with enhanced options"""
        self.config = config or createDefaultConfig()
        self.world = world or createSimpleWorld()
        self.brain = None
        self.agent_brain = None
        self.memory_integrator = None
        
        // Initialize brain
        self.brain = createBrain(self.config)
        
        // Initialize agent brain
        self.agent_brain = createAgentBrain(self.brain)
        
        // Initialize memory integrator
        self.memory_integrator = MemoryIntegrator(self.brain)
        
        // Initialize experiment runner
        self.experiment_runner = ExperimentRunner(self.brain, self.world)
        
        print("✓ NLM system initialized successfully")
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup"""
        self.cleanup()
    
    def step(self, step: int, time: Optional[float] = None):
        """Perform a simulation step"""
        if time is None:
            time = step * 0.001
        self.brain.step(step, time)
    
    def get_status(self) -> Dict[str, Any]:
        """Get comprehensive system status"""
        status = {
            'brain_status': self.brain.getStatus(),
            'memory_stats': self.memory_integrator.get_memory_stats() if self.memory_integrator else {},
            'config_summary': self.config.summary(),
            'world_info': {
                'width': self.world.getWidth(),
                'height': self.world.getHeight(),
                'agent_position': (self.world.getAgentBody().x, self.world.getAgentBody().y)
            }
        }
        return status
    
    def run_tutorial(self, steps: int = 100):
        """Run tutorial simulation"""
        print(f"Running tutorial simulation for {steps} steps...")
        
        for i in range(steps):
            // Random sensory input
            self.brain.injectCurrentToNeurons(NeuronType.Sensory, 20.0f)
            
            // Step brain
            self.brain.step(i, i * 0.001)
            
            // Store in working memory
            self.memory_integrator.store_working_memory(f"Step {i} processing", 0.5 + 0.5 * (i % 100) / 100)
            
            if i % 20 == 0:
                print(f"  Step {i}/{steps} completed")
        
        print("✓ Tutorial completed")
    
    def run_advanced_experiment(self, config_overrides: Dict[str, Any] = None):
        """Run advanced experiment with custom configuration"""
        if config_overrides:
            for key, value in config_overrides.items():
                self.config.set(key, value)
        
        print("Running advanced experiment...")
        results = self.experiment_runner.run_complete_experiment_suite()
        
        summary = self.experiment_runner.get_experiment_summary()
        print(f"✓ Advanced experiment completed. Results: {len(results)} tests run.")
        
        return results, summary
    
    def cleanup(self):
        """Clean up resources"""
        if self.memory_integrator:
            self.memory_integrator = None
        if self.agent_brain:
            self.agent_brain = None
        if self.brain:
            self.brain = None
        print("✓ NLM system cleaned up")

# ==============================================================================
# Convenience Functions
# ==============================================================================

def print_banner():
    """Print the NLM banner"""
    NLMUtils.print_banner()

def create_default_config() -> Config:
    """Create a default configuration"""
    return createDefaultConfig()

def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new brain with default or provided configuration"""
    if config is None:
        config = createDefaultConfig()
    return createBrain(config)

def create_simple_world() -> SimpleWorld:
    """Create a new simple world"""
    return createSimpleWorld()

# ==============================================================================
# Module Exports
# ==============================================================================

__all__ = [
    // Core classes
    'Config',
    'Brain',
    'SimpleWorld',
    'AgentBrain',
    
    // Sensory classes
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    
    // Motor classes
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    
    // Agent classes
    'SensoryPercept',
    
    // Enhanced Python types
    'WorkingMemoryEntry',
    'EpisodicEntry',
    'AssociativeMemoryEntry',
    'NeuromodulationState',
    'PredictionEntry',
    'DevelopmentalStageInfo',
    
    // Utility classes
    'NLMUtils',
    'MemoryIntegrator',
    'ExperimentRunner',
    'NLM',
    
    // Convenience functions
    'print_banner',
    'create_default_config',
    'create_brain',
    'create_simple_world',
    
    // C++ types
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'SimulationStep',
    'Timestamp',
]

# Print initialization message if module is run directly
if __name__ == "__main__":
    print_banner()
    print("NLM Enhanced Python Bindings")
    print(f"Version: 0.1.0")
    print(f"PyNLM available: {HAS_PYNLM}")
    print("Available for use in Python scripts")
