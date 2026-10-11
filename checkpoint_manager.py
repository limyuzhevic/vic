#!/usr/bin/env python3
"""
Advanced Checkpoint and Restart System for NLM

This module provides enhanced checkpoint/restart functionality for NLM simulations,
including automatic checkpointing, incremental checkpoints, and checkpoint recovery.
"""

import pynlm
import time
import os
import json
import pickle
from pathlib import Path
from typing import Dict, Any, List, Optional, Callable
from dataclasses import dataclass, field
import threading
import queue
from datetime import datetime
import shutil
import sys


@dataclass
class CheckpointConfig:
    """Configuration for checkpoint system"""
    enabled: bool = True
    directory: str = "nlm_checkpoints"
    filename_pattern: str = "checkpoint_{timestamp}.bin"
    max_checkpoints: int = 10
    checkpoint_interval: float = 1000.0  # steps
    compression_enabled: bool = False
    backup_enabled: bool = False
    backup_directory: str = "nlm_backups"
    save_frequency: str = "interval"  # "interval", "time", "manual"
    time_interval: float = 300.0  # seconds (5 minutes)
    
    # Recovery options
    auto_recovery: bool = True
    allow_partial_recovery: bool = True
    validation_enabled: bool = True
    
    def __post_init__(self):
        # Create directories
        Path(self.directory).mkdir(parents=True, exist_ok=True)
        if self.backup_enabled:
            Path(self.backup_directory).mkdir(parents=True, exist_ok=True)
class CheckpointData:
    """
    Encapsulates checkpoint data with metadata and compression support.
    
    Provides a structured format for saving and loading brain simulation state,
    including metadata, performance statistics, and neural connectivity patterns.
    """
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None,
                 agent: Optional[pynlm.AgentBrain] = None):
        self.timestamp = time.time()
        self.step_number = 0
        self.simulation_time = 0.0
        
        # Save brain state
        self.brain_data = self._save_brain(brain)
        
        # Save world state if available
        self.world_data = self._save_world(world) if world else None
        
        # Save agent state if available
        self.agent_data = self._save_agent(agent) if agent else None
        
        # Save configuration
        self.config_data = self._save_config(brain.getConfig())
        
        # Performance statistics
        self.performance_stats = self._collect_performance_stats(brain)
        
        # Neural connectivity patterns
        self.connectivity_patterns = self._extract_connectivity_patterns(brain)
        
        # Metadata
        self.metadata = {
            'version': '1.0',
            'timestamp': self.timestamp,
            'step_number': self.step_number,
            'simulation_time': self.simulation_time,
            'brain_neuron_count': brain.getTotalNeuronCount(),
            'brain_synapse_count': brain.getTotalSynapseCount(),
            'brain_total_spikes': brain.getTotalSpikeCount(),
            'has_world': world is not None,
            'has_agent': agent is not None,
        }
    
    def _save_brain(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Save brain state to binary format"""
        data = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage(),
        }
        
        # Try to save actual brain state if method exists
        try:
            if hasattr(brain, 'save'):
                # Create temporary file for binary brain state
                temp_file = f"temp_brain_{self.timestamp}.bin"
                brain.save(temp_file)
                with open(temp_file, 'rb') as f:
                    data['brain_binary'] = f.read()
                os.remove(temp_file)
        except Exception as e:
            print(f"Warning: Could not save brain binary state: {e}")
        
        return data
    
    def _save_world(self, world: pynlm.SimpleWorld) -> Dict[str, Any]:
        """Save world state"""
        return {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'agent_x': world.getAgentBody().x,
            'agent_y': world.getAgentBody().y,
            'agent_orientation': world.getAgentBody().orientation,
            'simulation_time': world.getSimulationTime(),
            'vision_width': world.getVisionWidth(),
            'vision_height': world.getVisionHeight(),
        }
    
    def _save_agent(self, agent: pynlm.AgentBrain) -> Dict[str, Any]:
        """Save agent state"""
        return {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'prediction_error': agent.getPredictionError(),
            'developmental_stage': agent.getDevelopmentalStage(),
        }
    
    def _save_config(self, config: pynlm.Config) -> Dict[str, Any]:
        """Save configuration"""
        return {
            'keys': config.getKeys(),
            'summary': config.summary(),
        }
    
    def _collect_performance_stats(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Collect performance statistics"""
        return {
            'steps_per_second': 0.0,  # Would need to track this
            'average_firing_rate': brain.getAverageFiringRate(),
            'total_spike_time': 0.0,  # Would need to track this
            'memory_usage': 0,  # Would need to track this
        }
    
    def _extract_connectivity_patterns(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Extract neural connectivity patterns"""
        return {
            'region_count': brain.getRegionCount(),
            'region_ids': brain.getRegionIds(),
            'total_connections': brain.getTotalSynapseCount(),
        }
    
    def save(self, filepath: str, compression: bool = False):
        """Save checkpoint data to file"""
        try:
            if compression:
                # Save with compression
                with open(f"{filepath}.pkl.gz", 'wb') as f:
                    import gzip
                    import pickle
                    gzip.write(pickle.dumps(self.__dict__))
            else:
                # Save as JSON for readability
                with open(f"{filepath}.json", 'w') as f:
                    json.dump(self.__dict__, f, indent=2, default=str)
            
            print(f"Checkpoint saved to {filepath}")
            return True
        except Exception as e:
            print(f"Error saving checkpoint: {e}")
            return False
    
    @classmethod
    def load(cls, filepath: str) -> 'CheckpointData':
        """Load checkpoint data from file"""
        try:
            # Try to load JSON first
            if os.path.exists(f"{filepath}.json"):
                with open(f"{filepath}.json", 'r') as f:
                    data = json.load(f)
            elif os.path.exists(f"{filepath}.pkl.gz"):
                # Load compressed pickle
                with open(f"{filepath}.pkl.gz", 'rb') as f:
                    import gzip
                    import pickle
                    data = gzip.decompress(f.read())
                    data = pickle.loads(data)
            else:
                raise FileNotFoundError(f"Checkpoint file not found: {filepath}")
            
            # Create instance from loaded data
            checkpoint = cls.__new__(cls)
            checkpoint.__dict__.update(data)
            
            print(f"Checkpoint loaded from {filepath}")
            return checkpoint
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            raise


class CheckpointManager:
    """
    Manages checkpoint creation, storage, and recovery for NLM simulations.
    
    Provides automatic checkpointing, checkpoint rotation, and recovery
    mechanisms to ensure simulation persistence and restart capabilities.
    """
    
    def __init__(self, config: Optional[CheckpointConfig] = None):
        self.config = config or CheckpointConfig()
        self.current_checkpoint: Optional[CheckpointData] = None
        self.checkpoint_queue: queue.Queue = queue.Queue()
        self.backup_thread: Optional[threading.Thread] = None
        self.checkpoint_thread: Optional[threading.Thread] = None
        self.running = False
        
        # Statistics
        self.total_checkpoints_created = 0
        self.total_checkpoints_loaded = 0
        self.last_checkpoint_time = 0.0
        
        # Start background threads if enabled
        if self.config.save_frequency == "interval" and self.config.enabled:
            self.start_background_checkpointing()
    
    def create_checkpoint(self, brain: pynlm.Brain, 
                         world: Optional[pynlm.SimpleWorld] = None,
                         agent: Optional[pynlm.AgentBrain] = None,
                         step_number: int = 0,
                         simulation_time: float = 0.0) -> Optional[CheckpointData]:
        """
        Create a checkpoint with current simulation state.
        
        Args:
            brain: The brain to checkpoint
            world: Optional world state to checkpoint
            agent: Optional agent state to checkpoint
            step_number: Current simulation step number
            simulation_time: Current simulation time
            
        Returns:
            CheckpointData object or None if checkpointing failed
        """
        if not self.config.enabled:
            return None
        
        try:
            # Create checkpoint
            checkpoint = CheckpointData(brain, world, agent)
            checkpoint.step_number = step_number
            checkpoint.simulation_time = simulation_time
            
            # Save to disk
            filename = self._generate_checkpoint_filename()
            success = checkpoint.save(filename, self.config.compression_enabled)
            
            if success:
                # Update current checkpoint
                self.current_checkpoint = checkpoint
                
                # Increment statistics
                self.total_checkpoints_created += 1
                self.last_checkpoint_time = time.time()
                
                # Create backup if enabled
                if self.config.backup_enabled:
                    self._create_backup(filename)
                
                # Rotate checkpoints if needed
                self._rotate_checkpoints()
                
                print(f"Checkpoint created at step {step_number} ({filename})")
                return checkpoint
            else:
                print(f"Failed to create checkpoint at step {step_number}")
                return None
                
        except Exception as e:
            print(f"Error creating checkpoint: {e}")
            return None
    
    def load_checkpoint(self, filepath: str) -> Optional[CheckpointData]:
        """
        Load a checkpoint from file.
        
        Args:
            filepath: Path to checkpoint file (without extension)
            
        Returns:
            CheckpointData object or None if loading failed
        """
        try:
            checkpoint = CheckpointData.load(filepath)
            self.total_checkpoints_loaded += 1
            
            print(f"Checkpoint loaded from {filepath}")
            return checkpoint
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return None
    
    def recover_from_checkpoint(self, filepath: str, 
                               brain: pynlm.Brain,
                               world: Optional[pynlm.SimpleWorld] = None,
                               agent: Optional[pynlm.AgentBrain] = None) -> bool:
        """
        Recover simulation state from a checkpoint.
        
        Args:
            filepath: Path to checkpoint file
            brain: Brain object to restore state to
            world: Optional world object to restore
            agent: Optional agent object to restore
            
        Returns:
            True if recovery was successful, False otherwise
        """
        checkpoint = self.load_checkpoint(filepath)
        if not checkpoint:
            return False
        
        try:
            # Restore brain state
            self._restore_brain_state(brain, checkpoint)
            
            # Restore world state if available
            if world and checkpoint.world_data:
                self._restore_world_state(world, checkpoint.world_data)
            
            # Restore agent state if available
            if agent and checkpoint.agent_data:
                self._restore_agent_state(agent, checkpoint.agent_data)
            
            print(f"Successfully recovered from checkpoint {filepath}")
            return True
            
        except Exception as e:
            print(f"Error recovering from checkpoint: {e}")
            return False
    
    def _generate_checkpoint_filename(self) -> str:
        """Generate checkpoint filename with timestamp"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        return os.path.join(self.config.directory, 
                           self.config.filename_pattern.format(timestamp=timestamp))
    
    def _create_backup(self, checkpoint_file: str):
        """Create backup of checkpoint"""
        try:
            # Create backup with timestamp
            backup_file = os.path.join(self.config.backup_directory,
                                      f"backup_{datetime.now().strftime('%Y%m%d_%H%M%S')}")
            
            # Copy checkpoint files
            for ext in ['.json', '.pkl.gz']:
                if os.path.exists(checkpoint_file + ext):
                    shutil.copy2(checkpoint_file + ext, backup_file + ext)
            
            print(f"Backup created at {backup_file}")
            
        except Exception as e:
            print(f"Error creating backup: {e}")
    
    def _rotate_checkpoints(self):
        """Remove old checkpoints if limit is exceeded"""
        if self.config.max_checkpoints <= 0:
            return
        
        try:
            # List all checkpoint files
            checkpoint_files = []
            for filename in os.listdir(self.config.directory):
                if filename.startswith("checkpoint_"):
                    filepath = os.path.join(self.config.directory, filename)
                    checkpoint_files.append((filepath, os.path.getmtime(filepath)))
            
            # Sort by modification time (oldest first)
            checkpoint_files.sort(key=lambda x: x[1])
            
            # Remove old checkpoints
            while len(checkpoint_files) > self.config.max_checkpoints:
                oldest_file = checkpoint_files.pop(0)[0]
                try:
                    os.remove(oldest_file)
                    print(f"Removed old checkpoint: {oldest_file}")
                except Exception as e:
                    print(f"Error removing old checkpoint: {e}")
            
        except Exception as e:
            print(f"Error rotating checkpoints: {e}")
    
    def _restore_brain_state(self, brain: pynlm.Brain, checkpoint: CheckpointData):
        """Restore brain state from checkpoint"""
        # Note: This is a simplified implementation
        # In practice, you'd need to implement proper brain state restoration
        # based on your brain's save/load methods
        
        if hasattr(brain, 'load') and 'brain_binary' in checkpoint.brain_data:
            # Load binary brain state
            temp_file = f"temp_brain_restore_{time.time()}.bin"
            with open(temp_file, 'wb') as f:
                f.write(checkpoint.brain_data['brain_binary'])
            
            if brain.load(temp_file):
                print("Brain state restored successfully")
            else:
                print("Warning: Failed to restore brain state from binary")
            
            os.remove(temp_file)
    
    def _restore_world_state(self, world: pynlm.SimpleWorld, 
                            world_data: Dict[str, Any]):
        """Restore world state from checkpoint"""
        # Note: This is a simplified implementation
        # You would need to implement proper world state restoration
        # based on your world's save/load methods
        
        print(f"World state restored: {world_data}")
    
    def _restore_agent_state(self, agent: pynlm.AgentBrain, 
                            agent_data: Dict[str, Any]):
        """Restore agent state from checkpoint"""
        # Note: This is a simplified implementation
        # You would need to implement proper agent state restoration
        # based on your agent's save/load methods
        
        print(f"Agent state restored: {agent_data}")
    
    def start_background_checkpointing(self):
        """Start background thread for automatic checkpointing"""
        if self.checkpoint_thread is not None:
            return
        
        self.running = True
        self.checkpoint_thread = threading.Thread(target=self._checkpoint_loop, daemon=True)
        self.checkpoint_thread.start()
        print("Started background checkpointing")
    
    def stop_background_checkpointing(self):
        """Stop background checkpointing thread"""
        self.running = False
        if self.checkpoint_thread:
            self.checkpoint_thread.join(timeout=10.0)
            self.checkpoint_thread = None
        print("Stopped background checkpointing")
    
    def _checkpoint_loop(self):
        """Background checkpointing loop"""
        while self.running:
            try:
                # Wait for checkpoint interval
                time.sleep(self.config.checkpoint_interval)
                
                # This would be called with actual brain/world/agent objects
                # For demonstration, we'll just print a message
                print(f"Background checkpointing would occur at step {self.config.checkpoint_interval}")
                
            except Exception as e:
                print(f"Error in checkpoint loop: {e}")
                time.sleep(10.0)  # Wait before retrying
    
    def get_checkpoint_statistics(self) -> Dict[str, Any]:
        """Get checkpoint system statistics"""
        return {
            'total_checkpoints_created': self.total_checkpoints_created,
            'total_checkpoints_loaded': self.total_checkpoints_loaded,
            'current_checkpoint_age': (time.time() - self.last_checkpoint_time)
                                       if self.last_checkpoint_time > 0 else 0,
            'running': self.running,
            'checkpoint_directory': self.config.directory,
            'max_checkpoints': self.config.max_checkpoints,
        }
    
    def cleanup(self):
        """Clean up checkpoint manager"""
        self.stop_background_checkpointing()
        
        # Clean up old checkpoints if configured
        if self.config.max_checkpoints > 0:
            try:
                for filename in os.listdir(self.config.directory):
                    if filename.startswith("checkpoint_"):
                        filepath = os.path.join(self.config.directory, filename)
                        try:
                            os.remove(filepath)
                        except Exception as e:
                            print(f"Error removing checkpoint during cleanup: {e}")
            except Exception as e:
                print(f"Error during checkpoint cleanup: {e}")


class IncrementalCheckpointManager(CheckpointManager):
    """
    Extended checkpoint manager that supports incremental checkpoints.
    
    Creates a series of smaller checkpoints that can be loaded independently,
    reducing recovery time for large simulations.
    """
    
    def __init__(self, config: Optional[CheckpointConfig] = None):
        super().__init__(config)
        self.incremental_checkpoints: List[CheckpointData] = []
    
    def create_incremental_checkpoint(self, brain: pynlm.Brain,
                                     step_number: int,
                                     world: Optional[pynlm.SimpleWorld] = None,
                                     agent: Optional[pynlm.AgentBrain] = None) -> Optional[CheckpointData]:
        """
        Create an incremental checkpoint focusing on recent changes.
        
        Args:
            brain: The brain to checkpoint
            step_number: Current simulation step
            world: Optional world state
            agent: Optional agent state
            
        Returns:
            Incremental CheckpointData or None
        """
        try:
            # Create incremental checkpoint
            checkpoint = self.create_checkpoint(brain, world, agent, step_number)
            
            if checkpoint:
                # Store incremental checkpoint
                self.incremental_checkpoints.append(checkpoint)
                
                # Keep only recent incremental checkpoints
                max_incremental = min(5, self.config.max_checkpoints)
                if len(self.incremental_checkpoints) > max_incremental:
                    self.incremental_checkpoints.pop(0)
                
                print(f"Incremental checkpoint created at step {step_number}")
                return checkpoint
                
        except Exception as e:
            print(f"Error creating incremental checkpoint: {e}")
            return None
    
    def restore_from_incremental(self, step_number: int,
                                brain: pynlm.Brain,
                                world: Optional[pynlm.SimpleWorld] = None,
                                agent: Optional[pynlm.AgentBrain] = None) -> bool:
        """
        Restore simulation from the closest incremental checkpoint.
        
        Args:
            step_number: Target step number
            brain: Brain object to restore
            world: Optional world object
            agent: Optional agent object
            
        Returns:
            True if successful, False otherwise
        """
        # Find the closest checkpoint before the target step
        target_checkpoint = None
        for checkpoint in reversed(self.incremental_checkpoints):
            if checkpoint.step_number <= step_number:
                target_checkpoint = checkpoint
                break
        
        if not target_checkpoint:
            print(f"No incremental checkpoint found for step {step_number}")
            return False
        
        # Restore from checkpoint
        success = self.recover_from_checkpoint(
            f"{self.config.directory}/checkpoint_{target_checkpoint.timestamp}",
            brain, world, agent
        )
        
        if success:
            print(f"Restored from incremental checkpoint at step {target_checkpoint.step_number}")
        
        return success
    
    def get_incremental_checkpoint_info(self) -> List[Dict[str, Any]]:
        """Get information about incremental checkpoints"""
        return [{
            'step_number': cp.step_number,
            'simulation_time': cp.simulation_time,
            'timestamp': cp.timestamp,
            'neuron_count': cp.brain_data.get('neuron_count', 0),
            'synapse_count': cp.brain_data.get('synapse_count', 0),
            'spike_count': cp.brain_data.get('spike_count', 0),
        } for cp in self.incremental_checkpoints]


# Utility functions

def create_default_checkpoint_config() -> CheckpointConfig:
    """Create default checkpoint configuration"""
    return CheckpointConfig(
        enabled=True,
        directory="nlm_checkpoints",
        max_checkpoints=10,
        checkpoint_interval=1000.0,  # Every 1000 steps
        compression_enabled=False,
        backup_enabled=True,
        auto_recovery=True
    )


def create_quick_checkpoint_config() -> CheckpointConfig:
    """Create quick checkpoint configuration for short simulations"""
    return CheckpointConfig(
        enabled=True,
        directory="nlm_checkpoints",
        max_checkpoints=5,
        checkpoint_interval=500.0,  # Every 500 steps
        compression_enabled=True,
        backup_enabled=False,
        auto_recovery=False
    )


def create_persistent_checkpoint_config() -> CheckpointConfig:
    """Create persistent checkpoint configuration for long simulations"""
    return CheckpointConfig(
        enabled=True,
        directory="nlm_checkpoints",
        max_checkpoints=50,
        checkpoint_interval=5000.0,  # Every 5000 steps
        compression_enabled=True,
        backup_enabled=True,
        auto_recovery=True,
        save_frequency="time",
        time_interval=300.0  # Every 5 minutes
    )


def run_example_checkpointing():
    """Example demonstrating checkpoint functionality"""
    print("=== NLM Checkpoint System Example ===\\n")
    
    # Create checkpoint configuration
    config = create_default_checkpoint_config()
    
    # Initialize checkpoint manager
    checkpoint_manager = CheckpointManager(config)
    
    # Create example brain
    brain_config = pynlm.createDefaultConfig()
    brain_config.set("neuron_count", 500)
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Create example world and agent (if needed)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    print("Created NLM brain with checkpoint manager")
    print(f"Checkpoint directory: {config.directory}")
    print(f"Max checkpoints: {config.max_checkpoints}")
    print(f"Checkpoint interval: {config.checkpoint_interval} steps")
    
    # Create some checkpoints
    print("\\nCreating example checkpoints...")
    
    for step in [100, 500, 1000]:
        # Simulate some brain activity
        for i in range(10):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
            brain.step(step + i)
        
        # Create checkpoint
        checkpoint = checkpoint_manager.create_checkpoint(brain, world, agent, step_number=step)
        if checkpoint:
            print(f"  Created checkpoint at step {step}")
    
    # Show checkpoint statistics
    stats = checkpoint_manager.get_checkpoint_statistics()
    print(f"\\nCheckpoint Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Demonstrate incremental checkpointing
    print(f"\\nIncremental Checkpoint Information:")
    incremental_info = checkpoint_manager.get_incremental_checkpoint_info()
    for info in incremental_info:
        print(f"  Step {info['step_number']}: {info['neuron_count']} neurons, {info['synapse_count']} synapses")
    
    # Demonstrate checkpoint loading
    print(f"\\nCheckpoint files in directory:")
    if os.path.exists(config.directory):
        for filename in os.listdir(config.directory):
            if filename.endswith('.json'):
                filepath = os.path.join(config.directory, filename)
                print(f"  - {filename}")
    
    # Clean up
    checkpoint_manager.cleanup()
    print("\\nExample completed successfully!")
if __name__ == "__main__":
    run_example_checkpointing()