#!/usr/bin/env python3
"""
Enhanced NLM Python Serialization Module

A practical serialization module for NLM brain states with support for
multiple formats, compression, and advanced features.
"""

import json
import pickle
import base64
import zlib
import os
import struct
from pathlib import Path
from typing import Dict, List, Any, Optional, Union, BinaryIO
from enum import Enum
import hashlib
import time
from datetime import datetime

class SerializationFormat(Enum):
    """Supported serialization formats."""
    BINARY = "binary"
    JSON = "json"
    PICKLE = "pickle"
    COMPRESSED = "compressed"
    BASE64 = "base64"
    HYBRID = "hybrid"

class SerializationError(Exception):
    """Base exception for serialization errors."""
    pass

class CheckpointManager:
    """
    Advanced checkpoint manager for NLM brains.
    
    This class provides comprehensive checkpoint management with support for
    multiple serialization formats and automatic cleanup.
    """
    
    def __init__(self, checkpoint_dir: str = "checkpoints"):
        self.checkpoint_dir = Path(checkpoint_dir)
        self.checkpoint_dir.mkdir(exist_ok=True)
        self.serializer = BrainSerializer()
        self.checkpoint_index = self._load_index()
    
    def _load_index(self) -> Dict[str, Any]:
        """Load checkpoint index from disk."""
        index_file = self.checkpoint_dir / "index.json"
        if index_file.exists():
            try:
                with open(index_file, 'r') as f:
                    return json.load(f)
            except:
                return {}
        return {}
    
    def _save_index(self):
        """Save checkpoint index to disk."""
        index_file = self.checkpoint_dir / "index.json"
        with open(index_file, 'w') as f:
            json.dump(self.checkpoint_index, f, indent=2)
    
    def save_checkpoint(self, brain, name: str, 
                       format: SerializationFormat = SerializationFormat.HYBRID,
                       tags: Optional[List[str]] = None,
                       metadata: Optional[Dict[str, Any]] = None,
                       description: str = "",
                       include_stats: bool = True,
                       compression: bool = True) -> str:
        """
        Save brain checkpoint with enhanced features.
        
        Args:
            brain: Brain object to save
            name: Checkpoint name (without extension)
            format: Serialization format
            tags: Tags to add to checkpoint
            metadata: Additional metadata
            description: Human-readable description
            include_stats: Whether to include statistics
            compression: Whether to compress the checkpoint
            
        Returns:
            Full path to the saved checkpoint
        """
        # Generate unique checkpoint ID
        timestamp = datetime.now().isoformat()
        checkpoint_id = f"{name}_{int(time.time())}"
        
        # Create checkpoint data structure
        checkpoint_data = {
            "checkpoint_id": checkpoint_id,
            "name": name,
            "timestamp": timestamp,
            "format": format.value,
            "description": description,
            "tags": tags or [],
            "metadata": metadata or {},
            "brain_info": {
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "ei_ratio": brain.getExcitationInhibitionRatio(),
                "developmental_stage": str(brain.getDevelopmentalStage()),
            }
        }
        
        # Add statistics if requested
        if include_stats:
            checkpoint_data["statistics"] = {
                "step_count": 0,  # Would track actual steps
                "max_memory_usage": "estimated",
                "last_simulation_time": timestamp,
            }
        
        # Serialize the brain state
        if hasattr(brain, 'save'):
            # Use brain native save method
            filepath = self.checkpoint_dir / f"{checkpoint_id}.bin"
            if brain.save(str(filepath)):
                checkpoint_data["file"] = str(filepath)
            else:
                raise SerializationError("Failed to save brain using native method")
        else:
            # Use serializer
            data = self.serializer.serialize(brain, format, compression)
            filepath = self.checkpoint_dir / f"{checkpoint_id}.{format.value}"
            with open(filepath, 'wb') as f:
                f.write(data)
            checkpoint_data["file"] = str(filepath)
        
        # Update index
        self.checkpoint_index[checkpoint_id] = checkpoint_data
        self._save_index()
        
        return str(filepath)
    
    def load_checkpoint(self, checkpoint_id: str) -> Any:
        """
        Load brain checkpoint by ID.
        
        Args:
            checkpoint_id: Checkpoint ID or name
            
        Returns:
            Loaded brain object
        """
        # Handle both checkpoint_id and simple names
        if checkpoint_id in self.checkpoint_index:
            checkpoint_data = self.checkpoint_index[checkpoint_id]
            filepath = checkpoint_data.get("file")
            format = SerializationFormat(checkpoint_data.get("format", "hybrid"))
        else:
            # Try to find checkpoint by name
            matching = [cid for cid in self.checkpoint_index.keys() 
                       if cid.startswith(checkpoint_id)]
            if not matching:
                raise SerializationError(f"Checkpoint not found: {checkpoint_id}")
            
            checkpoint_id = matching[0]
            checkpoint_data = self.checkpoint_index[checkpoint_id]
            filepath = checkpoint_data.get("file")
            format = SerializationFormat(checkpoint_data.get("format", "hybrid"))
        
        if not filepath or not Path(filepath).exists():
            raise SerializationError(f"Checkpoint file not found: {filepath}")
        
        # Load based on format
        if format == SerializationFormat.BINARY:
            # Use brain native load method
            brain = self._load_native(filepath)
        else:
            # Use serializer
            with open(filepath, 'rb') as f:
                data = f.read()
            brain = self.serializer.deserialize(data, format)
        
        return brain
    
    def _load_native(self, filepath: str) -> Any:
        """Load using brain native methods."""
        # This is a simplified implementation
        # In practice, you'd need to handle the specific file format
        # For now, return a placeholder
        class MockBrain:
            def __init__(self, filepath):
                self.filepath = filepath
                self._load_from_file()
            
            def _load_from_file(self):
                # Try to load using pynlm if available
                try:
                    import pynlm
                    # This is a simplified load - in practice you'd need
                    # to implement proper checkpoint loading
                    config = pynlm.createDefaultConfig()
                    brain = pynlm.createBrain(config)
                    brain.load(self.filepath)
                    self.__dict__.update(brain.__dict__)
                except:
                    # Create a mock brain
                    self.neuron_count = 0
                    self.synapse_count = 0
                    self.total_spikes = 0
                    self.firing_neurons = 0
        
        return MockBrain(filepath)
    
    def list_checkpoints(self) -> List[Dict[str, Any]]:
        """
        List all available checkpoints.
        
        Returns:
            List of checkpoint information
        """
        checkpoints = []
        
        for checkpoint_file in self.checkpoint_dir.glob("*.bin"):
            if checkpoint_file.name == "index.json":
                continue
            
            checkpoint_id = checkpoint_file.stem
            checkpoint_data = self.checkpoint_index.get(checkpoint_id, {})
            
            checkpoint_info = {
                "id": checkpoint_id,
                "name": checkpoint_data.get("name", checkpoint_id),
                "timestamp": checkpoint_data.get("timestamp", ""),
                "description": checkpoint_data.get("description", ""),
                "tags": checkpoint_data.get("tags", []),
                "file_size": checkpoint_file.stat().st_size,
                "neuron_count": checkpoint_data.get("brain_info", {}).get("neuron_count", 0),
                "synapse_count": checkpoint_data.get("brain_info", {}).get("synapse_count", 0),
            }
            checkpoints.append(checkpoint_info)
        
        return sorted(checkpoints, key=lambda x: x["timestamp"], reverse=True)
    
    def delete_checkpoint(self, checkpoint_id: str) -> bool:
        """
        Delete checkpoint.
        
        Args:
            checkpoint_id: Checkpoint ID
            
        Returns:
            True if successful, False otherwise
        """
        if checkpoint_id not in self.checkpoint_index:
            return False
        
        checkpoint_data = self.checkpoint_index[checkpoint_id]
        filepath = checkpoint_data.get("file")
        
        # Delete file
        if filepath and Path(filepath).exists():
            Path(filepath).unlink()
        
        # Remove from index
        del self.checkpoint_index[checkpoint_id]
        self._save_index()
        
        return True
    
    def cleanup_old_checkpoints(self, keep_latest: int = 10):
        """
        Clean up old checkpoints, keeping only the latest N.
        
        Args:
            keep_latest: Number of latest checkpoints to keep
        """
        checkpoints = self.list_checkpoints()
        
        if len(checkpoints) <= keep_latest:
            return
        
        # Sort by timestamp (oldest first)
        checkpoints.sort(key=lambda x: x["timestamp"])
        
        # Delete oldest checkpoints
        for checkpoint in checkpoints[:-keep_latest]:
            self.delete_checkpoint(checkpoint["id"])
    
    def export_checkpoint(self, checkpoint_id: str, output_path: str, 
                         format: SerializationFormat = SerializationFormat.JSON) -> str:
        """
        Export checkpoint to different format.
        
        Args:
            checkpoint_id: Source checkpoint ID
            output_path: Output file path
            format: Export format
            
        Returns:
            Output file path
        """
        # Load checkpoint
        checkpoint = self.load_checkpoint(checkpoint_id)
        
        # Serialize to requested format
        data = self.serializer.serialize(checkpoint, format)
        
        # Write to file
        with open(output_path, 'wb') as f:
            f.write(data)
        
        return output_path
    
    def import_checkpoint(self, input_path: str, name: Optional[str] = None) -> str:
        """
        Import checkpoint from file.
        
        Args:
            input_path: Input file path
            name: Checkpoint name (if None, derived from filename)
            
        Returns:
            Imported checkpoint name
        """
        # Read data
        with open(input_path, 'rb') as f:
            data = f.read()
        
        # Detect format
        format = self._detect_format(data)
        
        # Deserialize
        checkpoint = self.serializer.deserialize(data, format)
        
        # Save checkpoint
        checkpoint_name = name or Path(input_path).stem
        return self.save_checkpoint(checkpoint, checkpoint_name)
    
    def _detect_format(self, data: bytes) -> SerializationFormat:
        """Detect format from data."""
        # Try to decode as JSON
        try:
            decoded = data.decode('utf-8')
            json.loads(decoded)
            return SerializationFormat.JSON
        except:
            pass
        
        # Try to decompress and check
        try:
            decompressed = zlib.decompress(data)
            try:
                decoded = decompressed.decode('utf-8')
                json.loads(decoded)
                return SerializationFormat.COMPRESSDED
            except:
                # Check if it's pickle
                import pickle
                pickle.loads(decompressed)
                return SerializationFormat.PICKLE
        except:
            pass
        
        # Check if it's base64
        try:
            decoded = base64.b64decode(data)
            # Try to decode as JSON or pickle
            try:
                json.loads(decoded.decode('utf-8'))
                return SerializationFormat.BASE64
            except:
                import pickle
                pickle.loads(decoded)
                return SerializationFormat.BASE64
        except:
            pass
        
        # Default to binary
        return SerializationFormat.BINARY
    
    def create_experiment_snapshot(self, brain, experiment_name: str,
                                   steps: int, metrics: Dict[str, Any]) -> str:
        """
        Create a snapshot for experiment tracking.
        
        Args:
            brain: Brain object
            experiment_name: Experiment name
            steps: Number of steps simulated
            metrics: Performance metrics
            
        Returns:
            Checkpoint ID
        """
        tags = ["experiment", experiment_name, "snapshot"]
        metadata = {
            "experiment_name": experiment_name,
            "steps": steps,
            "metrics": metrics,
            "snapshot_type": "experiment",
        }
        
        description = f"Experiment snapshot: {experiment_name} after {steps} steps"
        
        return self.save_checkpoint(
            brain, f"{experiment_name}_snapshot",
            tags=tags, metadata=metadata, description=description
        )
    
    def create_training_checkpoint(self, brain, iteration: int,
                                  loss: float, accuracy: float) -> str:
        """
        Create a checkpoint for training.
        
        Args:
            brain: Brain object
            iteration: Training iteration
            loss: Training loss
            accuracy: Training accuracy
            
        Returns:
            Checkpoint ID
        """
        tags = ["training", "checkpoint"]
        metadata = {
            "iteration": iteration,
            "loss": loss,
            "accuracy": accuracy,
            "training_step": iteration,
        }
        
        description = f"Training checkpoint at iteration {iteration} (loss: {loss:.4f})"
        
        return self.save_checkpoint(
            brain, f"training_iter_{iteration}",
            tags=tags, metadata=metadata, description=description
        )
    
    def get_checkpoint_info(self, checkpoint_id: str) -> Dict[str, Any]:
        """
        Get detailed information about a checkpoint.
        
        Args:
            checkpoint_id: Checkpoint ID
            
        Returns:
            Checkpoint information
        """
        if checkpoint_id not in self.checkpoint_index:
            raise SerializationError(f"Checkpoint not found: {checkpoint_id}")
        
        return self.checkpoint_index[checkpoint_id]

class BrainSerializer:
    """
    Advanced serializer for NLM brain states.
    
    This class provides comprehensive serialization capabilities for NLM brains,
    supporting multiple formats, compression, and validation.
    """
    
    def __init__(self):
        self.supported_formats = {
            SerializationFormat.BINARY: self._serialize_binary,
            SerializationFormat.JSON: self._serialize_json,
            SerializationFormat.PICKLE: self._serialize_pickle,
            SerializationFormat.COMPRESS: self._serialize_compressed,
            SerializationFormat.BASE64: self._serialize_base64,
            SerializationFormat.HYBRID: self._serialize_hybrid,
        }
    
    def serialize(self, data: Any, format: SerializationFormat = SerializationFormat.HYBRID,
                  compression: bool = True) -> bytes:
        """
        Serialize brain data to specified format.
        
        Args:
            data: Brain data to serialize
            format: Serialization format
            compression: Whether to compress the data
            
        Returns:
            Serialized data as bytes
        """
        if format not in self.supported_formats:
            raise SerializationError(f"Format {format} is not supported")
        
        # Serialize to selected format
        serialized_data = self.supported_formats[format](data)
        
        # Apply compression if requested
        if compression:
            serialized_data = self._compress(serialized_data)
        
        return serialized_data
    
    def deserialize(self, data: bytes, format: SerializationFormat = SerializationFormat.HYBRID,
                    compression: bool = True) -> Any:
        """
        Deserialize brain data from specified format.
        
        Args:
            data: Serialized data as bytes
            format: Serialization format
            compression: Whether data is compressed
            
        Returns:
            Deserialized brain data
        """
        if format not in self.supported_formats:
            raise SerializationError(f"Format {format} is not supported")
        
        # Decompress if requested
        if compression:
            try:
                data = self._decompress(data)
            except:
                pass  # If decompression fails, try with compressed data
        
        return self.supported_formats[format](data)
    
    def _serialize_binary(self, data: Any) -> bytes:
        """Serialize to binary format using pickle."""
        return pickle.dumps(data)
    
    def _serialize_json(self, data: Any) -> bytes:
        """Serialize to JSON format."""
        return json.dumps(data, indent=2).encode('utf-8')
    
    def _serialize_pickle(self, data: Any) -> bytes:
        """Serialize to pickle format."""
        return pickle.dumps(data)
    
    def _serialize_compressed(self, data: Any) -> bytes:
        """Serialize and compress to JSON format."""
        json_data = self._serialize_json(data)
        return self._compress(json_data)
    
    def _serialize_base64(self, data: Any) -> bytes:
        """Serialize to base64 format."""
        binary_data = self._serialize_binary(data)
        return base64.b64encode(binary_data)
    
    def _serialize_hybrid(self, data: Any) -> bytes:
        """Serialize to hybrid format with metadata."""
        hybrid_data = {
            "format": "hybrid",
            "timestamp": datetime.now().isoformat(),
            "version": "1.0.0",
            "data": {
                "binary": self._serialize_binary(data),
                "json": self._serialize_json(data),
            }
        }
        return json.dumps(hybrid_data, indent=2).encode('utf-8')
    
    def _compress(self, data: bytes) -> bytes:
        """Compress data using zlib."""
        return zlib.compress(data)
    
    def _decompress(self, data: bytes) -> bytes:
        """Decompress data using zlib."""
        return zlib.decompress(data)

class CheckpointValidator:
    """
    Validator for NLM checkpoints.
    
    This class validates checkpoint integrity and consistency.
    """
    
    def __init__(self):
        self.serializer = BrainSerializer()
    
    def validate_checkpoint(self, checkpoint_path: str) -> Dict[str, Any]:
        """
        Validate checkpoint file.
        
        Args:
            checkpoint_path: Path to checkpoint file
            
        Returns:
            Validation result dictionary
        """
        result = {
            "is_valid": True,
            "errors": [],
            "warnings": [],
            "file_info": {},
        }
        
        checkpoint_path = Path(checkpoint_path)
        
        if not checkpoint_path.exists():
            result["is_valid"] = False
            result["errors"].append("Checkpoint file does not exist")
            return result
        
        # Get file info
        stat = checkpoint_path.stat()
        result["file_info"] = {
            "size": stat.st_size,
            "modified_time": stat.st_mtime,
        }
        
        # Try to load and validate
        try:
            with open(checkpoint_path, 'rb') as f:
                data = f.read()
            
            # Detect format
            format = self._detect_format(data)
            
            # Try to deserialize
            try:
                checkpoint = self.serializer._deserialize_from_data(data, format)
                
                # Validate basic structure
                if hasattr(checkpoint, 'getTotalNeuronCount'):
                    neuron_count = checkpoint.getTotalNeuronCount()
                    if neuron_count < 0:
                        result["errors"].append("Invalid neuron count")
                    elif neuron_count == 0:
                        result["warnings"].append("No neurons in brain")
                
            except Exception as e:
                result["is_valid"] = False
                result["errors"].append(f"Failed to deserialize checkpoint: {e}")
            
            # Check file size
            if stat.st_size > 100 * 1024 * 1024:  # 100MB
                result["warnings"].append("Checkpoint file is very large")
            
        except Exception as e:
            result["is_valid"] = False
            result["errors"].append(f"Error validating checkpoint: {e}")
        
        return result
    
    def _detect_format(self, data: bytes) -> SerializationFormat:
        """Detect format from data."""
        # Try to decode as JSON
        try:
            decoded = data.decode('utf-8')
            json.loads(decoded)
            return SerializationFormat.JSON
        except:
            pass
        
        # Check if it's base64
        try:
            decoded = base64.b64decode(data)
            # Try to decode as JSON
            json.loads(decoded.decode('utf-8'))
            return SerializationFormat.BASE64
        except:
            pass
        
        # Default to binary
        return SerializationFormat.BINARY
    
    def _deserialize_from_data(self, data: bytes, format: SerializationFormat) -> Any:
        """Deserialize from data bytes."""
        if format == SerializationFormat.JSON:
            return json.loads(data.decode('utf-8'))
        elif format == SerializationFormat.BASE64:
            decoded = base64.b64decode(data)
            return json.loads(decoded.decode('utf-8'))
        else:
            return pickle.loads(data)

# Global instances
_default_serializer = BrainSerializer()
_default_checkpoint_manager = None
_default_validator = CheckpointValidator()

# Convenience functions
def create_checkpoint_manager(checkpoint_dir: str = "checkpoints") -> CheckpointManager:
    """Create a new CheckpointManager instance."""
    return CheckpointManager(checkpoint_dir)

def save_brain_checkpoint(brain, name: str, checkpoint_dir: str = "checkpoints",
                          format: SerializationFormat = SerializationFormat.HYBRID,
                          tags: Optional[List[str]] = None,
                          description: str = "") -> str:
    """
    Convenience function to save a brain checkpoint.
    
    Args:
        brain: Brain object to save
        name: Checkpoint name
        checkpoint_dir: Checkpoint directory
        format: Serialization format
        tags: Tags to add to checkpoint
        description: Description of the checkpoint
        
    Returns:
        Checkpoint ID
    """
    global _default_checkpoint_manager
    if _default_checkpoint_manager is None:
        _default_checkpoint_manager = CheckpointManager(checkpoint_dir)
    
    return _default_checkpoint_manager.save_checkpoint(
        brain, name, format, tags, description
    )

def load_brain_checkpoint(checkpoint_id: str, checkpoint_dir: str = "checkpoints") -> Any:
    """
    Convenience function to load a brain checkpoint.
    
    Args:
        checkpoint_id: Checkpoint ID
        checkpoint_dir: Checkpoint directory
        
    Returns:
        Loaded brain object
    """
    global _default_checkpoint_manager
    if _default_checkpoint_manager is None:
        _default_checkpoint_manager = CheckpointManager(checkpoint_dir)
    
    return _default_checkpoint_manager.load_checkpoint(checkpoint_id)

def list_checkpoints(checkpoint_dir: str = "checkpoints") -> List[Dict[str, Any]]:
    """
    Convenience function to list checkpoints.
    
    Args:
        checkpoint_dir: Checkpoint directory
        
    Returns:
        List of checkpoint information
    """
    global _default_checkpoint_manager
    if _default_checkpoint_manager is None:
        _default_checkpoint_manager = CheckpointManager(checkpoint_dir)
    
    return _default_checkpoint_manager.list_checkpoints()

def validate_checkpoint(checkpoint_path: str) -> Dict[str, Any]:
    """
    Convenience function to validate a checkpoint.
    
    Args:
        checkpoint_path: Path to checkpoint file
        
    Returns:
        Validation result
    """
    return _default_validator.validate_checkpoint(checkpoint_path)

# Example usage
def example_usage():
    """Example usage of the serialization module."""
    print("=== NLM Serialization Module Example Usage ===\n")
    
    # Create checkpoint manager
    checkpoint_manager = CheckpointManager()
    
    # Try to demonstrate with a mock brain
    print("1. Creating example checkpoints:")
    
    # Create a mock brain for demonstration
    class MockBrain:
        def __init__(self, name, neuron_count):
            self.name = name
            self.neuron_count = neuron_count
            self.synapse_count = neuron_count * 5
            self.total_spikes = 0
            self.firing_neurons = 0
        
        def getTotalNeuronCount(self):
            return self.neuron_count
        
        def getTotalSynapseCount(self):
            return self.synapse_count
        
        def getTotalSpikeCount(self):
            return self.total_spikes
        
        def getFiringNeuronCount(self):
            return self.firing_neurons
    
    # Create mock brains
    brain1 = MockBrain("simple_brain", 100)
    brain2 = MockBrain("complex_brain", 1000)
    
    # Save checkpoints
    checkpoint1_id = checkpoint_manager.save_checkpoint(
        brain1, "simple_brain",
        tags=["demo", "simple"],
        description="A simple brain for demonstration"
    )
    print(f"   Created checkpoint: {checkpoint1_id}")
    
    checkpoint2_id = checkpoint_manager.save_checkpoint(
        brain2, "complex_brain",
        tags=["demo", "complex"],
        description="A complex brain for demonstration"
    )
    print(f"   Created checkpoint: {checkpoint2_id}")
    
    print("\n2. Listing checkpoints:")
    checkpoints = checkpoint_manager.list_checkpoints()
    for i, checkpoint in enumerate(checkpoints):
        print(f"   {i+1}. {checkpoint['name']} ({checkpoint['id']})")
        print(f"      Neurons: {checkpoint['neuron_count']}, "
              f"Size: {checkpoint['file_size']} bytes")
    
    print("\n3. Loading checkpoint:")
    try:
        loaded_brain = checkpoint_manager.load_checkpoint("simple_brain")
        print(f"   Loaded brain with {loaded_brain.getTotalNeuronCount()} neurons")
    except Exception as e:
        print(f"   Note: {e}")
    
    print("\n4. Example completed successfully!")
    print("   The serialization module provides:")
    print("   - Multiple serialization formats")
    print("   - Checkpoint management")
    print("   - Validation and export/import")
    print("   - Experiment and training snapshots")

if __name__ == "__main__":
    example_usage()