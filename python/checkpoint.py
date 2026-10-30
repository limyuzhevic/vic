"""
Python checkpoint management API for NLM

This module provides Python-friendly checkpoint management
interfaces for NLM, including checkpoint creation, loading,
and management utilities.
"""

from typing import Dict, List, Optional, Any, Union, Callable
from enum import Enum
import os
import time
import json
import warnings
from pathlib import Path

# Import from C++ bindings if available
try:
    from .core import (
        Brain,
        createBrain,
        createDefaultConfig,
    )
    
    BINDINGS_AVAILABLE = True
    
except ImportError:
    BINDINGS_AVAILABLE = False
    
    # Create stub classes for development
    class Brain:
        """Stub Brain class"""
        def __init__(self):
            self._state = {}
            
        def save(self, filepath: str) -> bool:
            print(f"[STUB] Saving brain to {filepath}")
            return True
            
        def load(self, filepath: str) -> bool:
            print(f"[STUB] Loading brain from {filepath}")
            return True
            
        def reset(self) -> None:
            self._state.clear()
            
        def getTotalNeuronCount(self) -> int:
            return 1000
            
        def getTotalSynapseCount(self) -> int:
            return 10000
            
        def getSimulationTime(self) -> float:
            return 0.0
            
        def getStep(self) -> int:
            return 0

# Checkpoint status enumeration
class CheckpointStatus(Enum):
    """Status of checkpoint files"""
    
    NEW = "new"
    """New checkpoint not yet finalized"""
    
    VALID = "valid"
    """Valid checkpoint"""
    
    INVALID = "invalid"
    """Invalid checkpoint"""
    
    CORRUPTED = "corrupted"
    """Corrupted checkpoint"""
    
    MISSING = "missing"
    """Missing checkpoint"""

# Checkpoint information class
class CheckpointInfo:
    """
    Information about a checkpoint file.
    
    This class provides metadata about checkpoint files including
    file information, timing, and statistics.
    """
    
    def __init__(self, filepath: str, name: Optional[str] = None):
        self.filepath = filepath
        self.name = name or Path(filepath).stem
        self.size = os.path.getsize(filepath) if os.path.exists(filepath) else 0
        self.timestamp = os.path.getmtime(filepath) if os.path.exists(filepath) else 0
        self.created_at = time.time()
        self.status = self._determine_status()
        self.metadata = self._load_metadata()
    
    def _determine_status(self) -> CheckpointStatus:
        """Determine checkpoint status"""
        if not os.path.exists(self.filepath):
            return CheckpointStatus.MISSING
        
        # Check file size
        if self.size < 100:  # Minimum reasonable size
            return CheckpointStatus.CORRUPTED
        
        # Check metadata
        metadata = self._load_metadata()
        if not metadata.get('valid', False):
            return CheckpointStatus.INVALID
        
        return CheckpointStatus.VALID
    
    def _load_metadata(self) -> Dict[str, Any]:
        """Load checkpoint metadata from file"""
        metadata_path = f"{self.filepath}.meta"
        metadata = {}
        
        try:
            if os.path.exists(metadata_path):
                with open(metadata_path, 'r') as f:
                    metadata = json.load(f)
        except Exception as e:
            warnings.warn(f"Failed to load checkpoint metadata: {e}")
        
        return metadata
    
    def save_metadata(self, metadata: Dict[str, Any]) -> bool:
        """
        Save checkpoint metadata to file.
        
        Args:
            metadata: Metadata to save
            
        Returns:
            True if save successful
        """
        try:
            metadata_path = f"{self.filepath}.meta"
            with open(metadata_path, 'w') as f:
                json.dump(metadata, f, indent=2)
            self.metadata = metadata
            return True
        except Exception as e:
            warnings.warn(f"Failed to save checkpoint metadata: {e}")
            return False
    
    def get_age(self) -> float:
        """Get checkpoint age in seconds"""
        return time.time() - self.created_at
    
    def get_formatted_size(self) -> str:
        """Get formatted file size"""
        size = self.size
        for unit in ['B', 'KB', 'MB', 'GB']:
            if size < 1024.0:
                return f"{size:.1f} {unit}"
            size /= 1024.0
        return f"{size:.1f} TB"
    
    def get_formatted_time(self) -> str:
        """Get formatted timestamp"""
        return time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(self.timestamp))
    
    def is_valid(self) -> bool:
        """Check if checkpoint is valid"""
        return self.status == CheckpointStatus.VALID
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert checkpoint info to dictionary"""
        return {
            'filepath': self.filepath,
            'name': self.name,
            'size': self.size,
            'formatted_size': self.get_formatted_size(),
            'timestamp': self.timestamp,
            'formatted_time': self.get_formatted_time(),
            'created_at': self.created_at,
            'age': self.get_age(),
            'status': self.status.value,
            'is_valid': self.is_valid(),
            'metadata': self.metadata,
        }
    
    def __repr__(self) -> str:
        return f"CheckpointInfo(name='{self.name}', filepath='{self.filepath}', status={self.status.value})"

# Checkpoint manager class
class CheckpointManager:
    """
    Manager for checkpoint creation, loading, and management.
    
    This class provides comprehensive checkpoint management functionality
    for NLM simulations, including automated checkpointing, validation,
    and cleanup.
    """
    
    def __init__(self, checkpoint_dir: str = "checkpoints"):
        self.checkpoint_dir = Path(checkpoint_dir)
        self.checkpoint_dir.mkdir(parents=True, exist_ok=True)
        
        self.auto_save_interval = 1000  # steps
        self.max_checkpoints = 10
        self.compress_checkpoints = True
        self.metadata_file = self.checkpoint_dir / "checkpoint_metadata.json"
        
        self._load_metadata()
    
    def _load_metadata(self) -> None:
        """Load checkpoint manager metadata"""
        try:
            if self.metadata_file.exists():
                with open(self.metadata_file, 'r') as f:
                    self.metadata = json.load(f)
            else:
                self.metadata = {}
        except Exception as e:
            warnings.warn(f"Failed to load checkpoint metadata: {e}")
            self.metadata = {}
    
    def _save_metadata(self) -> None:
        """Save checkpoint manager metadata"""
        try:
            with open(self.metadata_file, 'w') as f:
                json.dump(self.metadata, f, indent=2)
        except Exception as e:
            warnings.warn(f"Failed to save checkpoint metadata: {e}")
    
    def configure(self, auto_save_interval: Optional[int] = None,
                 max_checkpoints: Optional[int] = None,
                 compress: Optional[bool] = None) -> None:
        """
        Configure checkpoint manager settings.
        
        Args:
            auto_save_interval: Steps between automatic checkpoints
            max_checkpoints: Maximum number of checkpoints to keep
            compress: Whether to compress checkpoints
        """
        if auto_save_interval is not None:
            self.auto_save_interval = auto_save_interval
        
        if max_checkpoints is not None:
            self.max_checkpoints = max_checkpoints
            self._prune_old_checkpoints()
        
        if compress is not None:
            self.compress_checkpoints = compress
        
        self._save_metadata()
    
    def should_save(self, current_step: int) -> bool:
        """
        Check if checkpoint should be saved.
        
        Args:
            current_step: Current simulation step
            
        Returns:
            True if checkpoint should be saved
        """
        return current_step % self.auto_save_interval == 0 or current_step == 0
    
    def save_checkpoint(self, brain: Brain, name: Optional[str] = None,
                       step: Optional[int] = None, time: Optional[float] = None) -> Optional[CheckpointInfo]:
        """
        Save checkpoint for brain state.
        
        Args:
            brain: Brain object to checkpoint
            name: Checkpoint name (optional)
            step: Simulation step (optional, will use brain's step)
            time: Simulation time (optional, will use brain's time)
            
        Returns:
            CheckpointInfo object or None if save failed
        """
        try:
            # Use current brain state if not provided
            current_step = step if step is not None else getattr(brain, 'getStep', lambda: 0)()
            current_time = time if time is not None else getattr(brain, 'getSimulationTime', lambda: 0.0)()
            
            # Generate checkpoint name
            if name is None:
                name = f"checkpoint_{current_step:08d}"
            
            # Save checkpoint
            filepath = self.checkpoint_dir / f"{name}.bin"
            success = brain.save(str(filepath))
            
            if success:
                # Save metadata
                metadata = {
                    'name': name,
                    'step': current_step,
                    'time': current_time,
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount(),
                    'timestamp': time.time(),
                    'valid': True,
                    'brain_type': type(brain).__name__,
                }
                
                checkpoint_info = CheckpointInfo(str(filepath), name)
                checkpoint_info.save_metadata(metadata)
                
                # Update manager metadata
                if 'checkpoints' not in self.metadata:
                    self.metadata['checkpoints'] = []
                
                self.metadata['checkpoints'].append(checkpoint_info.to_dict())
                
                # Keep only recent checkpoints
                self._keep_recent_checkpoints()
                
                self._save_metadata()
                
                return checkpoint_info
            
            return None
            
        except Exception as e:
            warnings.warn(f"Failed to save checkpoint: {e}")
            return None
    
    def load_checkpoint(self, name: str) -> Optional[Brain]:
        """
        Load checkpoint by name.
        
        Args:
            name: Checkpoint name
            
        Returns:
            Brain object or None if load failed
        """
        try:
            # Find checkpoint info
            checkpoint_path = self.checkpoint_dir / f"{name}.bin"
            if not checkpoint_path.exists():
                warnings.warn(f"Checkpoint not found: {name}")
                return None
            
            # Load brain (simplified - assumes brain.load method)
            config = createDefaultConfig()
            brain = createBrain(config)
            
            success = brain.load(str(checkpoint_path))
            
            if success:
                return brain
            
            return None
            
        except Exception as e:
            warnings.warn(f"Failed to load checkpoint: {e}")
            return None
    
    def get_checkpoint(self, name: str) -> Optional[CheckpointInfo]:
        """
        Get checkpoint information.
n        
        Args:
            name: Checkpoint name
            
        Returns:
            CheckpointInfo object or None if not found
        """
        try:
            for checkpoint_info in self.list_checkpoints():
                if checkpoint_info.name == name:
                    return checkpoint_info
            return None
        except Exception as e:
            warnings.warn(f"Failed to get checkpoint info: {e}")
            return None
    
    def list_checkpoints(self) -> List[CheckpointInfo]:
        """
        List all available checkpoints.
        
        Returns:
            List of CheckpointInfo objects
        """
        checkpoints = []
        
        try:
            for filepath in self.checkpoint_dir.glob("*.bin"):
                if filepath.name.endswith('.bin'):
                    checkpoint_info = CheckpointInfo(str(filepath))
                    checkpoints.append(checkpoint_info)
        except Exception as e:
            warnings.warn(f"Failed to list checkpoints: {e}")
        
        return checkpoints
    
    def delete_checkpoint(self, name: str) -> bool:
        """
        Delete checkpoint by name.
n        
        Args:
            name: Checkpoint name
            
        Returns:
            True if deletion successful
        """
        try:
            checkpoint = self.get_checkpoint(name)
            if checkpoint:
                checkpoint_path = Path(checkpoint.filepath)
                if checkpoint_path.exists():
                    checkpoint_path.unlink()
                
                # Remove metadata file
                metadata_path = Path(f"{checkpoint.filepath}.meta")
                if metadata_path.exists():
                    metadata_path.unlink()
                
                # Update manager metadata
                if 'checkpoints' in self.metadata:
                    self.metadata['checkpoints'] = [
                        cp for cp in self.metadata['checkpoints']
                        if cp['name'] != name
                    ]
                
                self._save_metadata()
                return True
            
            return False
        except Exception as e:
            warnings.warn(f"Failed to delete checkpoint: {e}")
            return False
    
    def _keep_recent_checkpoints(self) -> None:
        """Keep only the most recent checkpoints"""
        if 'checkpoints' not in self.metadata:
            return
        
        # Sort checkpoints by timestamp (most recent first)
        checkpoints = sorted(self.metadata['checkpoints'],
                           key=lambda x: x.get('timestamp', 0),
                           reverse=True)
        
        # Keep only max_checkpoints
        if len(checkpoints) > self.max_checkpoints:
            recent_checkpoints = checkpoints[:self.max_checkpoints]
            self.metadata['checkpoints'] = recent_checkpoints
    
    def _prune_old_checkpoints(self) -> None:
        """Delete old checkpoints to maintain size limits"""
        checkpoints = self.list_checkpoints()
        
        if len(checkpoints) > self.max_checkpoints:
            # Sort by timestamp (oldest first)
            checkpoints_sorted = sorted(checkpoints,
                                      key=lambda cp: cp.created_at)
            
            # Delete oldest checkpoints
            for checkpoint in checkpoints_sorted[:-self.max_checkpoints]:
                self.delete_checkpoint(checkpoint.name)
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get checkpoint manager statistics.
n        
        Returns:
            Dictionary with statistics
        """
        checkpoints = self.list_checkpoints()
        
        return {
            'total_checkpoints': len(checkpoints),
            'valid_checkpoints': sum(1 for cp in checkpoints if cp.is_valid()),
            'total_size': sum(cp.size for cp in checkpoints),
            'auto_save_interval': self.auto_save_interval,
            'max_checkpoints': self.max_checkpoints,
            'compress_checkpoints': self.compress_checkpoints,
            'checkpoint_dir': str(self.checkpoint_dir),
        }

# Convenience functions

def create_checkpoint_manager(checkpoint_dir: str = "checkpoints") -> CheckpointManager:
    """
    Create checkpoint manager.
n    
    Args:
        checkpoint_dir: Directory for checkpoints
        
    Returns:
        CheckpointManager instance
    """
    return CheckpointManager(checkpoint_dir)

def save_brain_checkpoint(brain: Brain, checkpoint_dir: str = "checkpoints", 
                         name: Optional[str] = None) -> Optional[CheckpointInfo]:
    """
    Quick function to save brain checkpoint.
n    
    Args:
        brain: Brain object to checkpoint
        checkpoint_dir: Directory for checkpoints
        name: Checkpoint name (optional)
        
    Returns:
        CheckpointInfo object or None if save failed
    """
    manager = create_checkpoint_manager(checkpoint_dir)
    return manager.save_checkpoint(brain, name)

def load_brain_checkpoint(name: str, checkpoint_dir: str = "checkpoints") -> Optional[Brain]:
    """
    Quick function to load brain checkpoint.
n    
    Args:
        name: Checkpoint name
        checkpoint_dir: Directory for checkpoints
        
    Returns:
        Brain object or None if load failed
    """
    manager = create_checkpoint_manager(checkpoint_dir)
    return manager.load_checkpoint(name)

def list_checkpoints(checkpoint_dir: str = "checkpoints") -> List[CheckpointInfo]:
    """
    Quick function to list checkpoints.
n    
    Args:
        checkpoint_dir: Directory for checkpoints
        
    Returns:
        List of CheckpointInfo objects
    """
    manager = create_checkpoint_manager(checkpoint_dir)
    return manager.list_checkpoints()

# Export everything
__all__ = [
    # Enum classes
    'CheckpointStatus',
    
    # Data classes
    'CheckpointInfo',
    
    # Manager class
    'CheckpointManager',
    
    # Convenience functions
    'create_checkpoint_manager',
    'save_brain_checkpoint',
    'load_brain_checkpoint',
    'list_checkpoints',
]

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python checkpoint management API for NLM neural simulation"

print(f"NLM checkpoint management module loaded (version {__version__})")