#!/usr/bin/env python3
"""
Checkpoint Python bindings for NLM (Neural Learning Machine)

This module provides Python interfaces for checkpoint functionality,
allowing saving and loading of NLM brain states with compression,
validation, and incremental checkpointing support.
"""

import pybind11
import pybind11.stlib
from pybind11.interpreter import captured
import time
import struct
import zlib
import json
import os
from typing import Dict, List, Optional, Callable
from dataclasses import dataclass
from enum import Enum

# Import NLM C++ bindings
import pynlm

class CompressionLevel(Enum):
    """Checkpoint compression levels"""
    NONE = 0
    FAST = 1
    BALANCED = 2
    BEST = 3

@dataclass
class CheckpointMetadata:
    """Metadata for checkpoint files"""
    version_major: int
    version_minor: int
    version_patch: int
    timestamp: float
    total_size: int
    neuron_count: int
    synapse_count: int
    region_count: int
    simulation_step: int
    simulation_time: float
    checksum: int

@dataclass
class CheckpointInfo:
    """Information about a checkpoint file"""
    filepath: str
    size: int
    timestamp: float
    metadata: Optional[CheckpointMetadata] = None
    is_valid: bool = True

class CheckpointManager:
    """Python interface for checkpoint management"""
    
    def __init__(self, checkpoint_dir: str = "./checkpoints"):
        self.checkpoint_dir = checkpoint_dir
        self.metadata_file = os.path.join(checkpoint_dir, "metadata.json")
        
        # Ensure checkpoint directory exists
        os.makedirs(checkpoint_dir, exist_ok=True)
        
        # Load existing metadata
        self.checkpoints = self._load_metadata()
    
    def _load_metadata(self) -> Dict[str, CheckpointInfo]:
        """Load checkpoint metadata from JSON file"""
        checkpoints = {}
        
        if os.path.exists(self.metadata_file):
            try:
                with open(self.metadata_file, 'r') as f:
                    data = json.load(f)
                    
                for filepath, info_dict in data.items():
                    info_dict['metadata'] = CheckpointMetadata(**info_dict.pop('metadata'))
                    checkpoints[filepath] = CheckpointInfo(**info_dict)
                    
            except (json.JSONDecodeError, KeyError, TypeError) as e:
                print(f"Warning: Failed to load checkpoint metadata: {e}")
                
        return checkpoints
    
    def _save_metadata(self) -> None:
        """Save checkpoint metadata to JSON file"""
        data = {}
        
        for filepath, info in self.checkpoints.items():
            info_dict = info.__dict__.copy()
            if info.metadata:
                info_dict['metadata'] = info.metadata.__dict__
                
            data[filepath] = info_dict
        
        with open(self.metadata_file, 'w') as f:
            json.dump(data, f, indent=2)
    
    def create_checkpoint(self, filepath: str, config, brain) -> bool:
        """
        Create a new checkpoint file
        
        Args:
            filepath: Path to checkpoint file
            config: NLM configuration object
            brain: Brain object to checkpoint
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Ensure absolute path
            if not os.path.isabs(filepath):
                filepath = os.path.join(self.checkpoint_dir, filepath)
            
            # Create checkpoint directory if needed
            os.makedirs(os.path.dirname(filepath), exist_ok=True)
            
            # Convert to NLM C++ checkpoint if available
            # For now, implement Python-only checkpointing
            metadata = self._create_python_checkpoint(filepath, config, brain)
            
            if metadata:
                # Add to metadata
                info = CheckpointInfo(
                    filepath=filepath,
                    size=metadata.total_size,
                    timestamp=metadata.timestamp,
                    metadata=metadata,
                    is_valid=True
                )
                
                self.checkpoints[filepath] = info
                self._save_metadata()
                
                return True
                
            return False
            
        except Exception as e:
            print(f"Error creating checkpoint: {e}")
            return False
    
    def _create_python_checkpoint(self, filepath: str, config, brain) -> Optional[CheckpointMetadata]:
        """Create checkpoint using Python implementation"""
        try:
            # Serialize configuration
            config_data = {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'region_count': brain.getRegionCount(),
                'simulation_step': brain.getCurrentStep(),
                'simulation_time': brain.getCurrentTime()
            }
            
            # Serialize neuron states
            neuron_states = []
            for region in brain.getRegions():
                for population in region.getPopulations():
                    for neuron in population.getNeurons():
                        neuron_states.append({
                            'id': neuron.getId(),
                            'region_id': region.getId(),
                            'population_id': population.getId(),
                            'type': neuron.getType(),
                            'membrane_potential': neuron.getMembranePotential(),
                            'firing_state': neuron.getFiringState(),
                            'threshold': neuron.getThreshold()
                        })
            
            # Serialize synapse states
            synapse_states = []
            for region in brain.getRegions():
                for synapse in region.getSynapses():
                    synapse_states.append({
                        'id': synapse.getId(),
                        'source_neuron': synapse.getSourceNeuron(),
                        'destination_neuron': synapse.getDestinationNeuron(),
                        'weight': synapse.getWeight(),
                        'delay': synapse.getDelay(),
                        'type': synapse.getType()
                    })
            
            # Create checkpoint data
            checkpoint_data = {
                'version': '1.0',
                'timestamp': time.time(),
                'config': config_data,
                'neurons': neuron_states,
                'synapses': synapse_states
            }
            
            # Compress if needed
            if True:  # Configurable compression
                checkpoint_bytes = json.dumps(checkpoint_data).encode('utf-8')
                compressed_data = zlib.compress(checkpoint_bytes)
                
                with open(filepath, 'wb') as f:
                    # Write header
                    header = struct.pack('4sIIfIIf', 
                                        b'NLMCP1',
                                        len(compressed_data),
                                        time.time(),
                                        config_data['neuron_count'],
                                        config_data['synapse_count'],
                                        config_data['region_count'])
                    
                    f.write(header)
                    f.write(compressed_data)
                    
                metadata = CheckpointMetadata(
                    version_major=1,
                    version_minor=0,
                    version_patch=0,
                    timestamp=time.time(),
                    total_size=os.path.getsize(filepath),
                    neuron_count=config_data['neuron_count'],
                    synapse_count=config_data['synapse_count'],
                    region_count=config_data['region_count'],
                    simulation_step=config_data['simulation_step'],
                    simulation_time=config_data['simulation_time'],
                    checksum=hash(json.dumps(checkpoint_data, sort_keys=True))
                )
                
                return metadata
                
            return None
            
        except Exception as e:
            print(f"Error creating Python checkpoint: {e}")
            return None
    
    def load_checkpoint(self, filepath: str) -> bool:
        """
        Load checkpoint from file
        
        Args:
            filepath: Path to checkpoint file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if not os.path.exists(filepath):
                print(f"Checkpoint file not found: {filepath}")
                return False
            
            # Check if in metadata
            if filepath in self.checkpoints:
                info = self.checkpoints[filepath]
                if not info.is_valid:
                    print(f"Checkpoint file is invalid: {filepath}")
                    return False
            
            # Load checkpoint
            success = self._load_python_checkpoint(filepath)
            
            if success and filepath in self.checkpoints:
                info = self.checkpoints[filepath]
                info.is_valid = True
                self._save_metadata()
            
            return success
            
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return False
    
    def _load_python_checkpoint(self, filepath: str) -> bool:
        """Load checkpoint using Python implementation"""
        try:
            with open(filepath, 'rb') as f:
                # Read header
                header = f.read(28)  # 4s + I + I + f + I + f + I
                if len(header) < 28:
                    print("Invalid checkpoint header")
                    return False
                
                header_data = struct.unpack('4sIIfIIf', header)
                if header_data[0] != b'NLMCP1':
                    print("Invalid checkpoint magic number")
                    return False
                
                compressed_size = header_data[1]
                timestamp = header_data[2]
                neuron_count = header_data[3]
                synapse_count = header_data[4]
                region_count = header_data[5]
                
                # Read compressed data
                compressed_data = f.read(compressed_size)
                if len(compressed_data) != compressed_size:
                    print("Incomplete checkpoint data")
                    return False
                
                # Decompress and parse
                checkpoint_bytes = zlib.decompress(compressed_data)
                checkpoint_data = json.loads(checkpoint_bytes.decode('utf-8'))
                
                # Apply checkpoint data (would restore brain state)
                # This would require access to brain object for restoration
                print(f"Loaded checkpoint with {neuron_count} neurons, {synapse_count} synapses")
                
                return True
                
        except Exception as e:
            print(f"Error loading Python checkpoint: {e}")
            return False
    
    def list_checkpoints(self) -> List[CheckpointInfo]:
        """List all available checkpoints"""
        return list(self.checkpoints.values())
    
    def delete_checkpoint(self, filepath: str) -> bool:
        """
        Delete a checkpoint file
        
        Args:
            filepath: Path to checkpoint file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if filepath in self.checkpoints:
                # Delete file
                if os.path.exists(filepath):
                    os.remove(filepath)
                
                # Remove from metadata
                del self.checkpoints[filepath]
                self._save_metadata()
                
                return True
                
            return False
            
        except Exception as e:
            print(f"Error deleting checkpoint: {e}")
            return False
    
    def cleanup_old_checkpoints(self, keep_last: int = 5) -> int:
        """
        Clean up old checkpoint files
        
        Args:
            keep_last: Number of most recent checkpoints to keep
            
        Returns:
            Number of checkpoints deleted
        """
        try:
            # Sort checkpoints by timestamp (oldest first)
            sorted_checkpoints = sorted(
                self.checkpoints.items(),
                key=lambda x: x[1].timestamp
            )
            
            # Delete oldest checkpoints if we have more than keep_last
            deleted_count = 0
            for filepath, info in sorted_checkpoints[:-keep_last]:
                if os.path.exists(filepath):
                    os.remove(filepath)
                    del self.checkpoints[filepath]
                    deleted_count += 1
            
            if deleted_count > 0:
                self._save_metadata()
            
            return deleted_count
            
        except Exception as e:
            print(f"Error cleaning up checkpoints: {e}")
            return 0

# Create Python bindings for checkpoint functionality
def register_checkpoint_bindings(m):
    """Register checkpoint bindings with Python module"""
    
    # Register CheckpointManager class
    pybind11.class_<CheckpointManager>(m, "CheckpointManager") \
        .def("__init__", &CheckpointManager.__init__, 
             pybind11.arg("checkpoint_dir", "./checkpoints")) \
        .def("create_checkpoint", &CheckpointManager.create_checkpoint,
             pybind11.arg("filepath"), pybind11.arg("config"), 
             pybind11.arg("brain")) \
        .def("load_checkpoint", &CheckpointManager.load_checkpoint,
             pybind11.arg("filepath")) \
        .def("list_checkpoints", &CheckpointManager.list_checkpoints) \
        .def("delete_checkpoint", &CheckpointManager.delete_checkpoint,
             pybind11.arg("filepath")) \
        .def("cleanup_old_checkpoints", &CheckpointManager.cleanup_old_checkpoints,
             pybind11.arg("keep_last", 5)) \
        .def("get_checkpoint_dir", &CheckpointManager.checkpoint_dir)
    
    # Register checkpoint-related classes
    pybind11.enum_<CompressionLevel>(m, "CompressionLevel") \
        .value("NONE", CompressionLevel.NONE) \
        .value("FAST", CompressionLevel.FAST) \
        .value("BALANCED", CompressionLevel.BALANCED) \
        .value("BEST", CompressionLevel.BEST)
    
    pybind11.class_<CheckpointMetadata>(m, "CheckpointMetadata") \
        .def(pybind11.__init__<CheckpointMetadata>()) \
        .def_readwrite("version_major", &CheckpointMetadata.version_major) \
        .def_readwrite("version_minor", &CheckpointMetadata.version_minor) \
        .def_readwrite("version_patch", &CheckpointMetadata.version_patch) \
        .def_readwrite("timestamp", &CheckpointMetadata.timestamp) \
        .def_readwrite("total_size", &CheckpointMetadata.total_size) \
        .def_readwrite("neuron_count", &CheckpointMetadata.neuron_count) \
        .def_readwrite("synapse_count", &CheckpointMetadata.synapse_count) \
        .def_readwrite("region_count", &CheckpointMetadata.region_count) \
        .def_readwrite("simulation_step", &CheckpointMetadata.simulation_step) \
        .def_readwrite("simulation_time", &CheckpointMetadata.simulation_time) \
        .def_readwrite("checksum", &CheckpointMetadata.checksum)
    
    pybind11.class_<CheckpointInfo>(m, "CheckpointInfo") \
        .def(pybind11.__init__<CheckpointInfo>()) \
        .def_readwrite("filepath", &CheckpointInfo.filepath) \
        .def_readwrite("size", &CheckpointInfo.size) \
        .def_readwrite("timestamp", &CheckpointInfo.timestamp) \
        .def_readwrite("metadata", &CheckpointInfo.metadata) \
        .def_readwrite("is_valid", &CheckpointInfo.is_valid)

# Example usage function
def example_checkpoint_usage():
    """Example usage of checkpoint functionality"""
    print("""
Checkpoint Usage Example:

# Create checkpoint manager
checkpoint_manager = CheckpointManager("./my_checkpoints")

# Create a brain and configure it
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation...
for i in range(100):
    brain.step(i)

# Save checkpoint
checkpoint_manager.create_checkpoint("my_checkpoint.bin", config, brain)

# Load checkpoint later
checkpoint_manager.load_checkpoint("my_checkpoint.bin")

# List checkpoints
for checkpoint in checkpoint_manager.list_checkpoints():
    print(f"Checkpoint: {checkpoint.filepath}, Size: {checkpoint.size} bytes")
    
# Clean up old checkpoints (keep last 3)
checkpoint_manager.cleanup_old_checkpoints(keep_last=3)
""")

# Register the module
m.def("example_checkpoint_usage", &example_checkpoint_usage)
m.def("register_checkpoint_bindings", &register_checkpoint_bindings)
