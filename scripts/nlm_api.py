#!/usr/bin/env python3
"""
NLM Advanced Python API - Direct brain control for advanced users

This provides a comprehensive Python interface for advanced brain manipulation,
including direct access to memory systems, cognitive functions, and neural dynamics.

Key Features:
- Direct memory system access (working, episodic, associative)
- Cognitive system control (planning, concept formation, attention)
- Neural dynamics manipulation
- Real-time monitoring and control
- Advanced simulation control
- Export and import capabilities
- Memory management utilities
"""

import sys
import os
import time
import json
import math
from typing import Optional, List, Dict, Any, Callable
from pathlib import Path

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent))

# Import NLM Python bindings
try:
    import pynlm
    has_pynlm = True
except ImportError:
    print("Warning: pynlm not available. Python API will be limited.")
    has_pynlm = False

# Python type stubs for NLM classes
if has_pynlm:
    from pynlm import (
        Brain, Config, Action, ActionType,
        NeuronType, DevelopmentalStage,
        SensoryInput, MotorCommand,
        NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory,
        NeuralPlanner, ConceptFormation, AttentionalSelection,
        Dopamine, Curiosity, Novelty, PredictionError,
        PredictionSystem, DevelopmentSystem, SpikeSystem
    )
else:
    # Create placeholder classes for type checking
    class Brain:
        pass
    class Config:
        pass
    class Action:
        pass
    class ActionType:
        MoveForward = 0
        MoveBackward = 1
        TurnLeft = 2
        TurnRight = 3
        Interact = 4
        Wait = 5
        LookLeft = 6
        LookRight = 7
    
    class NeuronType:
        Sensory = 0
        Internal = 1
        Motor = 2
    
    class DevelopmentalStage:
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
    
    class NeuralWorkingMemory:
        pass
    class NeuralEpisodicMemory:
        pass
    class NeuralAssociativeMemory:
        pass
    class NeuralPlanner:
        pass
    class ConceptFormation:
        pass
    class AttentionalSelection:
        pass
    class Dopamine:
        pass
    class Curiosity:
        pass
    class Novelty:
        pass
    class PredictionError:
        pass
    class PredictionSystem:
        pass
    class DevelopmentSystem:
        pass
    class SpikeSystem:
        pass
class NLMBrainAPI:
    """
    Advanced Python API for NLM brain control.
    
    This class provides direct access to brain systems and advanced functionality
    for power users and researchers.
    """
    
    def __init__(self, config_file: Optional[str] = None):
        self._brain = None
        self._config_file = config_file or "configs/default.cfg"
        self._callbacks = {}
        self._simulation_running = False
        self._last_update_time = 0.0
        
        if has_pynlm:
            self._load_brain()
        else:
            print("Warning: NLM Python bindings not available")
    
    def _load_brain(self):
        """Load brain from configuration file."""
        try:
            if os.path.exists(self._config_file):
                self._brain = Brain(self._config_file)
                if self._brain.initialize():
                    print(f"✓ Loaded brain from {self._config_file}")
                else:
                    print(f"✗ Failed to initialize brain from {self._config_file}")
            else:
                print(f"✗ Config file not found: {self._config_file}")
        except Exception as e:
            print(f"✗ Failed to load brain: {e}")
    
    # ==================== BRAIN CONTROL ====================
    
    def run(self, steps: int, timestep: float = 0.001) -> bool:
        """Run simulation for specified number of steps."""
        if not self._brain:
            print("✗ No brain loaded")
            return False
        
        print(f"Starting simulation for {steps} steps (dt={timestep}s)...")
        start_time = time.time()
        
        try:
            for i in range(steps):
                current_time = i * timestep
                
                # Update brain state
                self._brain.step(i, current_time)
                
                # Process callbacks
                self._process_callbacks(i, current_time)
                
                # Save checkpoint periodically
                if i > 0 and i % 1000 == 0:
                    self._save_checkpoint(f"checkpoint_step_{i}")
                
                # Progress reporting
                if i % 1000 == 0:
                    self._log_progress(i, steps, time.time() - start_time)
            
            elapsed = time.time() - start_time
            print(f"✓ Simulation completed in {elapsed:.2f}s ({steps/elapsed:.0f} steps/sec)")
            return True
            
        except Exception as e:
            print(f"✗ Simulation failed: {e}")
            return False
    
    def step(self, timestep: float = 0.001) -> bool:
        """Run a single simulation step."""
        if not self._brain:
            print("✗ No brain loaded")
            return False
        
        current_time = time.time()
        
        try:
            self._brain.step(current_time)
            self._process_callbacks("step", current_time)
            return True
        except Exception as e:
            print(f"✗ Step failed: {e}")
            return False
    
    # ==================== MEMORY SYSTEMS ====================
    
    def get_working_memory(self) -> Optional[NeuralWorkingMemory]:
        """Get working memory system."""
        if not self._brain:
            return None
        return self._brain.getWorkingMemory()
    
    def get_episodic_memory(self) -> Optional[NeuralEpisodicMemory]:
        """Get episodic memory system."""
        if not self._brain:
            return None
        return self._brain.getEpisodicMemory()
    
    def get_associative_memory(self) -> Optional[NeuralAssociativeMemory]:
        """Get associative memory system."""
        if not self._brain:
            return None
        return self._brain.getAssociativeMemory()
    
    def store_working_memory(self, pattern: List[float], strength: float = 1.0) -> bool:
        """Store pattern in working memory."""
        working_mem = self.get_working_memory()
        if not working_mem:
            return False
        
        try:
            working_mem.store(pattern, strength)
            return True
        except Exception as e:
            print(f"✗ Failed to store in working memory: {e}")
            return False
    
    def retrieve_working_memory(self) -> List[float]:
        """Retrieve current working memory content."""
        working_mem = self.get_working_memory()
        if not working_mem:
            return []
        
        try:
            return working_mem.retrieve()
        except Exception as e:
            print(f"✗ Failed to retrieve from working memory: {e}")
            return []
    
    def store_episode(self, action: ActionType, reward: float, sensory_state: List[float]) -> bool:
        """Store experience as episodic memory."""
        episodic_mem = self.get_episodic_memory()
        if not episodic_mem:
            return False
        
        try:
            # Create episode item
            episode = EpisodicMemoryItem()
            episode.action = action
            episode.reward = reward
            episode.sensory_state = sensory_state
            episode.timestamp = self.get_current_step()
            
            episodic_mem.storeEpisode(episode)
            return True
        except Exception as e:
            print(f"✗ Failed to store episode: {e}")
            return False
    
    # ==================== COGNITIVE SYSTEMS ====================
    
    def get_planner(self) -> Optional[NeuralPlanner]:
        """Get neural planner system."""
        if not self._brain:
            return None
        return self._brain.getPlanner()
    
    def get_concept_formation(self) -> Optional[ConceptFormation]:
        """Get concept formation system."""
        if not self._brain:
            return None
        return self._brain.getConceptFormation()
    
    def get_attention(self) -> Optional[AttentionalSelection]:
        """Get attentional selection system."""
        if not self._brain:
            return None
        return self._brain.getAttention()
    
    def plan_action(self, current_state: List[float], target_reward: float = 0.5) -> ActionType:
        """Plan next action using neural planner."""
        planner = self.get_planner()
        if not planner:
            return ActionType.Wait
        
        try:
            return planner.planAction(current_state, target_reward)
        except Exception as e:
            print(f"✗ Action planning failed: {e}")
            return ActionType.Wait
    
    def form_concept(self, pattern: List[float], features: List[float], reward: float, current_time: float) -> int:
        """Present experience to concept formation system."""
        concept_formation = self.get_concept_formation()
        if not concept_formation:
            return 0
        
        try:
            return concept_formation.presentExperience(pattern, features, reward, current_time)
        except Exception as e:
            print(f"✗ Concept formation failed: {e}")
            return 0
    
    # ==================== NEUROMODULATION ====================
    
    def get_dopamine(self) -> Optional[Dopamine]:
        """Get dopamine system."""
        if not self._brain:
            return None
        return self._brain.getDopamine()
    
    def get_curiosity(self) -> Optional[Curiosity]:
        """Get curiosity system."""
        if not self._brain:
            return None
        return self._brain.getCuriosity()
    
    def get_novelty(self) -> Optional[Novelty]:
        """Get novelty detection system."""
        if not self._brain:
            return None
        return self._brain.getNovelty()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward prediction error for learning."""
        if not self._brain:
            return
        
        # This would typically be called by agent brain
        # For direct API, we can apply the effect
        prediction_error = reward - predicted_reward
        
        # Apply to synapses
        for region in self._brain.getRegions():
            for syn in region.getSynapses():
                eligibility = syn.getEligibilityTrace()
                if abs(eligibility) > 0.001:
                    delta = eligibility * prediction_error * 1.0  # plasticity factor
                    syn.addToWeight(delta)
                    syn.decayEligibilityTrace(0.1f)
    
    # ==================== NEURAL DYNAMICS ====================
    
    def get_spike_system(self) -> Optional[SpikeSystem]:
        """Get spike system."""
        if not self._brain:
            return None
        return self._brain.getSpikeSystem()
    
    def inject_sensory_input(self, input_data: List[float]):
        """Inject sensory input into brain."""
        if not self._brain:
            return
        
        sensory_input = SensoryInput(input_data)
        self._brain.receiveSensoryInput(sensory_input)
    
    def get_motor_command(self) -> Action:
        """Get motor command from brain."""
        if not self._brain:
            return Action(ActionType.Wait)
        
        return self._brain.produceAction()
    
    # ==================== MONITORING ====================
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        stats = {}
        
        if not self._brain:
            return stats
        
        stats['brain'] = {
            'regions': self._brain.getRegionCount(),
            'neurons': self._brain.getTotalNeuronCount(),
            'synapses': self._brain.getTotalSynapseCount(),
            'active_neurons': self._brain.getActiveNeuronCount(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'firing_rate': self._brain.getAverageFiringRate(),
            'e_i_ratio': self._brain.getExcitationInhibitionRatio(),
            'pending_spikes': self._brain.getPendingSpikeEventCount(),
        }
        
        # Memory system stats
        if self.get_working_memory():
            stats['working_memory'] = {
                'active_traces': self.get_working_memory().getActiveTraces(),
                'capacity': self.get_working_memory().getCapacity(),
                'memory_activity': self.get_working_memory().getMemoryActivity(),
                'memory_neurons': len(self.get_working_memory().getMemoryNeurons()),
            }
        
        if self.get_episodic_memory():
            stats['episodic_memory'] = {
                'episode_count': self.get_episodic_memory().getEpisodeCount(),
                'average_reward': self.get_episodic_memory().getAverageReward(),
                'replay_enabled': self.get_episodic_memory().isReplayEnabled(),
            }
        
        if self.get_associative_memory():
            stats['associative_memory'] = {
                'association_count': self.get_associative_memory().getAssociationCount(),
            }
        
        # Neuromodulation stats
        if self.get_dopamine():
            stats['neuromodulation'] = {
                'dopamine_level': self.get_dopamine().getLevel(),
            }
        
        if self.get_curiosity():
            stats['neuromodulation']['curiosity_level'] = self.get_curiosity().getLevel()
        
        if self.get_novelty():
            stats['neuromodulation']['novelty_level'] = self.get_novelty().getLevel()
        
        # Development status
        if self._brain.getDevelopmentSystem():
            stats['development'] = {
                'stage': self._brain.getDevelopmentalStage(),
                'age': self._brain.getDevelopmentSystem().getAge(),
            }
        
        return stats
    
    def print_stats(self):
        """Print brain statistics in human-readable format."""
        stats = self.get_brain_stats()
        
        print("=== NLM Brain Statistics ===")
        
        if 'brain' in stats:
            b = stats['brain']
            print(f"\nBrain Core:")
            print(f"  Regions: {b['regions']}")
            print(f"  Neurons: {b['neurons']:,}")
            print(f"  Synapses: {b['synapses']:,}")
            print(f"  Active neurons: {b['active_neurons']:,}")
            print(f"  Total spikes: {b['total_spikes']:,}")
            print(f"  Firing rate: {b['firing_rate']:.4f}")
            print(f"  E/I ratio: {b['e_i_ratio']:.4f}")
            print(f"  Pending spikes: {b['pending_spikes']:,}")
        
        if 'working_memory' in stats:
            wm = stats['working_memory']
            print(f"\nWorking Memory:")
            print(f"  Active traces: {wm['active_traces']}")
            print(f"  Capacity: {wm['capacity']}")
            print(f"  Memory activity: {wm['memory_activity']:.4f}")
            print(f"  Memory neurons: {wm['memory_neurons']}")
        
        if 'episodic_memory' in stats:
            em = stats['episodic_memory']
            print(f"\nEpisodic Memory:")
            print(f"  Episode count: {em['episode_count']}")
            print(f"  Average reward: {em['average_reward']:.4f}")
            print(f"  Replay enabled: {em['replay_enabled']}")
        
        if 'associative_memory' in stats:
            am = stats['associative_memory']
            print(f"\nAssociative Memory:")
            print(f"  Association count: {am['association_count']}")
        
        if 'neuromodulation' in stats:
            n = stats['neuromodulation']
            print(f"\nNeuromodulation:")
            if 'dopamine_level' in n:
                print(f"  Dopamine level: {n['dopamine_level']:.4f}")
            if 'curiosity_level' in n:
                print(f"  Curiosity level: {n['curiosity_level']:.4f}")
            if 'novelty_level' in n:
                print(f"  Novelty level: {n['novelty_level']:.4f}")
        
        if 'development' in stats:
            d = stats['development']
            print(f"\nDevelopment:")
            print(f"  Stage: {d['stage']}")
            print(f"  Age: {d['age']:.2f}")
    
    # ==================== CALLBACKS AND EVENTS ====================
    
    def add_callback(self, event_type: str, callback: Callable):
        """Add callback for specific event type."""
        if event_type not in self._callbacks:
            self._callbacks[event_type] = []
        self._callbacks[event_type].append(callback)
    
    def _process_callbacks(self, event, data):
        """Process registered callbacks for event."""
        if not hasattr(data, '__iter__'):
            data = {'event': event, 'data': data}
        
        for callback in self._callbacks.get(str(event), []):
            try:
                callback(data)
            except Exception as e:
                print(f"Callback error: {e}")
    
    # ==================== CHECKPOINTING ====================
    
    def save_checkpoint(self, name: str = "") -> bool:
        """Save brain state to checkpoint file."""
        if not self._brain:
            print("✗ No brain loaded")
            return False
        
        try:
            filename = f"checkpoints/{name}.nlmck" if name else "checkpoints/brain_state.nlmck"
            
            # Ensure checkpoints directory exists
            os.makedirs("checkpoints", exist_ok=True)
            
            if self._brain.save(filename):
                print(f"✓ Saved checkpoint to {filename}")
                return True
            else:
                print(f"✗ Failed to save checkpoint to {filename}")
                return False
                
        except Exception as e:
            print(f"✗ Checkpoint save failed: {e}")
            return False
    
    def load_checkpoint(self, name: str) -> bool:
        """Load brain state from checkpoint file."""
        try:
            filename = f"checkpoints/{name}.nlmck"
            
            if self._brain.load(filename):
                print(f"✓ Loaded checkpoint from {filename}")
                return True
            else:
                print(f"✗ Failed to load checkpoint from {filename}")
                return False
                
        except Exception as e:
            print(f"✗ Checkpoint load failed: {e}")
            return False
    
    # ==================== UTILITY METHODS ====================
    
    def get_current_step(self) -> int:
        """Get current simulation step."""
        if not self._brain:
            return 0
        return self._brain.getCurrentStep() if hasattr(self._brain, 'getCurrentStep') else 0
    
    def get_current_time(self) -> float:
        """Get current simulation time."""
        if not self._brain:
            return 0.0
        return self._brain.getCurrentTime() if hasattr(self._brain, 'getCurrentTime') else 0.0
    
    def reset(self):
        """Reset brain to initial state."""
        if self._brain:
            self._brain.reset()
            print("✓ Brain reset to initial state")
    
    def cleanup(self):
        """Clean up resources."""
        self._callbacks.clear()
        self._simulation_running = False
        
        if self._brain:
            # In full implementation, would clean up brain resources
            pass
    
    def __enter__(self):
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.cleanup()
class EpisodicMemoryItem:
    """Episodic memory item for Python API."""
    def __init__(self):
        self.timestamp = 0
        self.action = ActionType.Wait
        self.reward = 0.0
        self.energy = 0.0
        self.novelty = 0.0
        self.sensory_state = []
        self.resulting_sensory_state = []
        self.resulting_reward = 0.0
        self.active_neurons = []
        self.neuron_activations = []
        self.age = 0
class NLMAdvancedAPI:
    """Main entry point for advanced NLM Python API."""
    
    def __init__(self, config_file: Optional[str] = None):
        self.api = NLMBrainAPI(config_file)
    
    def get_brain(self) -> NLMBrainAPI:
        """Get the underlying brain API."""
        return self.api
    
    def run_simulation(self, steps: int, timestep: float = 0.001, output_file: Optional[str] = None) -> bool:
        """Run a complete simulation."""
        print(f"Running NLM simulation for {steps} steps...")
        return self.api.run(steps, timestep)
    
    def analyze_brain(self) -> Dict[str, Any]:
        """Analyze current brain state."""
        return self.api.get_brain_stats()
    
    def save_analysis(self, filename: str, format_type: str = "json") -> bool:
        """Save brain analysis to file."""
        stats = self.api.get_brain_stats()
        
        try:
            if format_type == "json":
                with open(filename, 'w') as f:
                    json.dump(stats, f, indent=2)
            elif format_type == "csv":
                with open(filename, 'w') as f:
                    if 'brain' in stats:
                        b = stats['brain']
                        f.write("timestamp,regions,neurons,synapses,active_neurons,total_spikes\n")
                        f.write(f"{time.time()},{b['regions']},{b['neurons']},{b['synapses']},"
                               f"{b['active_neurons']},{b['total_spikes']}\n")
            
            print(f"✓ Analysis saved to {filename}")
            return True
        except Exception as e:
            print(f"✗ Failed to save analysis: {e}")
            return False
    
    def __enter__(self):
        """Context manager entry."""
        return self.api
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.api.cleanup()
def createBrain(config_file: Optional[str] = None) -> NLMBrainAPI:
    """Create a new NLM brain instance (convenience function)."""
    return NLMBrainAPI(config_file)
def loadBrain(config_file: Optional[str] = None) -> Optional[NLMBrainAPI]:
    """Load an existing NLM brain instance (convenience function)."""
    if has_pynlm:
        api = NLMBrainAPI(config_file)
        if api.get_brain_stats():
            return api
    return None
def main():
    """Example usage of advanced NLM Python API."""
    print("NLM Advanced Python API - Example Usage")
    print("=" * 50)
    
    # Create brain with custom config
    api = createBrain("configs/default.cfg")
    
    # Example 1: Run simulation
    if api:
        print("\n1. Running simulation for 10,000 steps...")
        api.run(10000, 0.001)
        
        # Example 2: Analyze brain state
        print("\n2. Analyzing brain state...")
        stats = api.get_brain_stats()
        print(f"Brain statistics: {json.dumps(stats, indent=2)}")
        
        # Example 3: Save checkpoint
        print("\n3. Saving checkpoint...")
        api.save_checkpoint("simulation_checkpoint")
        
        # Example 4: Load checkpoint
        print("\n4. Loading checkpoint...")
        api.load_checkpoint("simulation_checkpoint")
        
        # Example 5: Working with memory systems
        print("\n5. Working with memory systems...")
        working_mem = api.get_working_memory()
        if working_mem:
            # Store some data in working memory
            pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
            api.store_working_memory(pattern, 0.8)
            
            # Retrieve and display
            retrieved = api.retrieve_working_memory()
            print(f"Retrieved working memory: {retrieved}")
        
        # Example 6: Cognitive systems
        print("\n6. Using cognitive systems...")
        current_state = [0.5] * 10  # Example state
        planned_action = api.plan_action(current_state)
        print(f"Planned action: {planned_action}")
        
        # Example 7: Neuromodulation
        print("\n7. Neuromodulation effects...")
        api.apply_reward_modulation(0.8, 0.6)
        
        # Example 8: Export data
        print("\n8. Exporting data...")
        api.export_neural_data("brain_state.json")
        
        print("\n✓ All examples completed successfully!")
    
    print("\nAdvanced Python API completed.")
if __name__ == "__main__":
    main()
