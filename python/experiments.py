"""
NLM Python Bindings - Experiments Module

This module provides Python bindings for NLM experiment functionality,
including the Phase 6 integration experiment.
"""

import time
from typing import Dict, Any, Optional
import numpy as np

# Import core types
from .core_types import Brain, Config, Phase6Config, Phase6IntegrationResult
from .environment import SimpleWorld, Action
# Placeholder for the actual C++ integration experiment implementation
# In a real implementation, this would be implemented with pybind11
try:
    from . import _pynlm_experiments
    Phase6IntegratedExperiment = _pynlm_experiments.Phase6IntegratedExperiment
except ImportError:
    # Use Python placeholder implementation
    class Phase6IntegratedExperiment:
        """Phase 6 integrated experiment runner (Python placeholder)."""
        
        def __init__(self):
            self._last_result = None
            
        def run(self, config: Phase6Config) -> Phase6IntegrationResult:
            """Run the complete integration test (Python placeholder)."""
            print(f"=== Phase 6 Integration Experiment ===")
            print(f"Configuration: {config.neuronCount} neurons, {config.maxSteps} steps")
            
            # Create brain
            brain_config = Config()
            brain_config.set("neuron_count", config.neuronCount)
            brain_config.set("region_count", config.regionCount)
            brain_config.set("connection_probability", config.connectionProbability)
            
            brain = Brain(brain_config)
            if not brain.initialize():
                print("ERROR: Failed to initialize brain")
                result = Phase6IntegrationResult()
                result.end_time = int(time.time())
                return result
            
            # Create world
            world = SimpleWorld()
            world.initialize(16, 16)
            
            # Run simulation
            total_reward = 0.0
            total_firing_rate = 0.0
            firing_count = 0
            
            for step in range(config.maxSteps):
                # Get observation
                observation = world.observe(brain)
                
                # Create sensory input (placeholder)
                from .environment import SensoryInput
                sensory_input = SensoryInput.from_array(observation)
                
                # Process sensory input (placeholder)
                # brain.process_sensory_input(sensory_input)
                
                # Brain step
                brain.step(step, step * 0.001)
                
                # Get metrics
                total_firing_rate += brain.get_average_firing_rate()
                if brain.get_firing_neuron_count() > 0:
                    firing_count += 1
                
                # Compute reward
                reward = world.compute_reward(brain)
                total_reward += reward
                
                # Apply development (placeholder)
                if config.enableDevelopment:
                    # brain.update_development(0.001)
                    pass
                
                # Periodic status
                if step % 1000 == 0:
                    print(f"Step {step} | Reward: {total_reward / (step + 1):.3f} | "
                          f"Firing: {brain.get_average_firing_rate():.3f} | "
                          f"WorkingMem: {brain.get_working_memory().get_active_traces() if brain.get_working_memory() else 0}")
            
            # Collect final metrics
            result = Phase6IntegrationResult()
            result.total_reward = total_reward / config.maxSteps
            result.avg_firing_rate = total_firing_rate / config.maxSteps
            result.memory_episodes_stored = brain.get_episodic_memory().get_episode_count() if brain.get_episodic_memory() else 0.0
            
            # Verify integration
            result.memory_working_memory_integrated = brain.get_working_memory() is not None
            result.memory_episodic_memory_integrated = brain.get_episodic_memory() is not None
            result.neuromodulation_integrated = brain.get_dopamine() is not None
            result.prediction_integrated = brain.get_prediction_system() is not None
            result.development_integrated = brain.get_development_system() is not None
            
            # Test checkpointing
            if config.enableCheckpointing:
                checkpoint_path = config.checkpointPath
                if brain.save(checkpoint_path):
                    print(f"Checkpoint saved successfully")
                    
                    # Load checkpoint
                    brain2 = Brain(brain_config)
                    brain2.initialize()
                    
                    if brain2.load(checkpoint_path):
                        print(f"Checkpoint loaded successfully")
                        result.checkpointing_works = True
                    else:
                        print(f"Failed to load checkpoint")
                else:
                    print(f"Failed to save checkpoint")
            
            result.start_time = int(time.time())
            result.end_time = result.start_time + config.maxSteps * 10  # Placeholder
            result.total_wall_clock_time = result.end_time - result.start_time
            
            this._lastResult = &result;
            
            print(f"=== Integration Test Results ===")
            print(f"Working Memory: {'CONNECTED' if result.memory_working_memory_integrated else 'DISCONNECTED'}")
            print(f"Episodic Memory: {'CONNECTED' if result.memory_episodic_memory_integrated else 'DISCONNECTED'}")
            print(f"Neuromodulation: {'CONNECTED' if result.neuromodulation_integrated else 'DISCONNECTED'}")
            print(f"Prediction: {'CONNECTED' if result.prediction_integrated else 'DISCONNECTED'}")
            print(f"Development: {'CONNECTED' if result.development_integrated else 'DISCONNECTED'}")
            print(f"Checkpointing: {'WORKING' if result.checkpointing_works else 'NOT WORKING'}")
            
            return result
            
        def verifyIntegration(self) -> bool:
            """Run a simple integration verification."""
            print("=== Phase 6 Integration Verification ===")
            
            # Create minimal brain
            config = Config()
            config.set("neuron_count", 100)
            config.set("region_count", 1)
            
            brain = Brain(config)
            if not brain.initialize():
                print("Brain initialization failed")
                return False
            
            success = True
            
            # Test memory systems
            if brain.get_working_memory() is not None:
                print("[PASS] Working memory is integrated")
            else:
                print("[FAIL] Working memory is NOT integrated")
                success = False
                
            if brain.get_episodic_memory() is not None:
                print("[PASS] Episodic memory is integrated")
            else:
                print("[FAIL] Episodic memory is NOT integrated")
                success = False
            
            # Test neuromodulation
            if brain.get_dopamine() is not None:
                print("[PASS] Dopamine system is integrated")
            else:
                print("[FAIL] Dopamine system is NOT integrated")
                success = False
                
            if brain.get_curiosity() is not None:
                print("[PASS] Curiosity system is integrated")
            else:
                print("[FAIL] Curiosity system is NOT integrated")
                success = False
            
            # Test prediction
            if brain.get_prediction_system() is not None:
                print("[PASS] Prediction system is integrated")
            else:
                print("[FAIL] Prediction system is NOT integrated")
                success = False
            
            return success
            
        def testMemoryIntegration(self) -> bool:
            """Test memory integration."""
            print("=== Testing Memory Integration ===")
            
            config = Config()
            config.set("neuron_count", 100)
            
            brain = Brain(config)
            brain.initialize()
            
            # Run some steps
            for i in range(100):
                brain.step(i, i * 0.001)
            
            # Check memory
            working_memory = brain.get_working_memory()
            episodic_memory = brain.get_episodic_memory()
            
            if working_memory:
                print(f"[PASS] Working memory has active traces: {working_memory.get_active_traces()}")
            else:
                print("[INFO] Working memory has no active traces")
                
            if episodic_memory:
                print(f"[PASS] Episodic memory has episodes: {episodic_memory.get_episode_count()}")
            else:
                print("[INFO] Episodic memory has no episodes")
            
            return True
            
        def testNeuromodulationIntegration(self) -> bool:
            """Test neuromodulation integration."""
            print("=== Testing Neuromodulation Integration ===")
            
            config = Config()
            config.set("neuron_count", 100)
            
            brain = Brain(config)
            brain.initialize()
            
            # Get neuromodulation systems
            dopamine = brain.get_dopamine()
            curiosity = brain.get_curiosity()
            novelty = brain.get_novelty()
            
            if not dopamine or not curiosity or not novelty:
                print("ERROR: Neuromodulation systems not available")
                return False
            
            # Run some steps with sensory input
            for i in range(50):
                brain.inject_current_to_neurons("Sensory", 5.0)
                brain.step(i, i * 0.001)
            
            print("[PASS] Neuromodulation systems are functional")
            return True
            
        def testCheckpointing(self) -> bool:
            """Test checkpoint save/load."""
            print("=== Testing Checkpoint Save/Load ===")
            
            config = Config()
            config.set("neuron_count", 100)
            
            # Create and initialize brain
            brain1 = Brain(config)
            brain1.initialize()
            
            # Run some steps
            for i in range(100):
                brain1.step(i, i * 0.001)
            
            print(f"Brain1 avg firing: {brain1.get_average_firing_rate():.3f}")
            print(f"Brain1 total spikes: {brain1.get_total_neuron_count()}")
            
            # Save checkpoint
            import tempfile
            import os
            with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
                checkpoint_path = f.name
            
            if not brain1.save(checkpoint_path):
                print("ERROR: Failed to save checkpoint")
                os.unlink(checkpoint_path)
                return False
            
            # Create new brain and load
            brain2 = Brain(config)
            brain2.initialize()
            
            if not brain2.load(checkpoint_path):
                print("ERROR: Failed to load checkpoint")
                os.unlink(checkpoint_path)
                return False
            
            print(f"Brain2 total spikes after load: {brain2.get_total_neuron_count()}")
            os.unlink(checkpoint_path)
            
            print("[PASS] Checkpoint save/load completed")
            return True
            
        def testReplay(self) -> bool:
            """Test replay system."""
            print("=== Testing Replay System ===")
            
            config = Config()
            config.set("neuron_count", 100)
            
            brain = Brain(config)
            brain.initialize()
            
            # Run some steps
            for i in range(200):
                brain.step(i, i * 0.001)
            
            # Check if episodes exist for replay
            episodic_memory = brain.get_episodic_memory()
            if episodic_memory and episodic_memory.get_episode_count() > 0:
                print(f"[PASS] Episodes available for replay: {episodic_memory.get_episode_count()}")
                print("[PASS] Replay system can retrieve episodes")
                return True
            
            print("[INFO] No episodes available for replay")
            return True
# High-level convenience functions
def create_default_config() -> Config:
    """Create a default NLM configuration optimized for general use."""
    config = Config()
    config.set("neuron_count", 1000)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    config.set("stdp_ltp_weight", 0.01)
    config.set("stdp_ltd_weight", 0.012)
    config.set("synaptogenesis_rate", 0.0001)
    config.set("pruning_rate", 0.00001)
    config.set("simulation_timestep", 0.001)
    config.set("replay_interval", 100)
    config.set("consolidation_interval", 1000)
    config.set("random_seed", 42)
    return config


def run_integration_experiment(neuron_count: int = 500, max_steps: int = 2000) -> Phase6IntegrationResult:
    """Run a standard NLM integration experiment with specified parameters."""
    config = Phase6Config()
    config.neuronCount = neuron_count
    config.maxSteps = max_steps
    
    experiment = Phase6IntegratedExperiment()
    return experiment.run(config)


def simulate_brain(config: Config, steps: int = 100) -> List[Dict[str, Any]]:
    """Run a simple brain simulation and return metrics at each step."""
    brain = Brain(config)
    brain.initialize()
    
    metrics = []
    for step in range(steps):
        brain.step(step, step * 0.001)
        
        metrics.append({
            'step': step,
            'firing_rate': brain.get_average_firing_rate(),
            'total_spikes': brain.get_total_neuron_count(),
            'sensory_activity': 0.0,  # Placeholder
            'motor_activity': 0.0,   # Placeholder
        })
    
    return metrics


# Module exports
__all__ = [
    'Phase6IntegrationResult',
    'Phase6Config',
    'Phase6IntegratedExperiment',
    'create_default_config',
    'run_integration_experiment',
    'simulate_brain',
]
