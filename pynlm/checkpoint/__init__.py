"""
pynlm.checkpoint - Checkpoint management for NLM project

This module provides checkpoint management including:
- Automatic checkpointing
- State restoration
- Version compatibility
- Backup management
"""

import json
import pickle
import hashlib
import os
import time
from typing import Dict, Any, List, Optional, Union
from pathlib import Path
import shutil
import logging
from datetime import datetime
from enum import Enum

logger = logging.getLogger(__name__)


class CheckpointStatus(Enum):
    """Enumeration of checkpoint status."""
    CREATED = "created"
    SAVED = "saved"
    LOADED = "loaded"
    DELETED = "deleted"
    CORRUPTED = "corrupted"
    INVALID = "invalid"


@dataclass
class CheckpointMetadata:
    """Metadata for a checkpoint."""
    checkpoint_id: str
    timestamp: str
    version: str
    agent_id: str
    episode: int
    step: int
    reward: float
    status: CheckpointStatus
    file_size: int
    checksum: str
    description: Optional[str] = None
    metadata: Dict[str, Any] = field(default_factory=dict)


class Checkpoint:
    """Represents a single checkpoint."""
    
    def __init__(self, metadata: CheckpointMetadata, data: Dict[str, Any]):
        """
        Initialize checkpoint.
        
        Parameters
        ----------
        metadata : CheckpointMetadata
            Checkpoint metadata
        data : Dict[str, Any]
            Checkpoint data
        """
        self.metadata = metadata
        self.data = data
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert checkpoint to dictionary."""
        return {
            "metadata": {
                "checkpoint_id": self.metadata.checkpoint_id,
                "timestamp": self.metadata.timestamp,
                "version": self.metadata.version,
                "agent_id": self.metadata.agent_id,
                "episode": self.metadata.episode,
                "step": self.metadata.step,
                "reward": self.metadata.reward,
                "status": self.metadata.status.value,
                "file_size": self.metadata.file_size,
                "checksum": self.metadata.checksum,
                "description": self.metadata.description,
                "metadata": self.metadata.metadata
            },
            "data": self.data
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Checkpoint':
        """Create checkpoint from dictionary."""
        metadata_dict = data["metadata"]
        metadata = CheckpointMetadata(
            checkpoint_id=metadata_dict["checkpoint_id"],
            timestamp=metadata_dict["timestamp"],
            version=metadata_dict["version"],
            agent_id=metadata_dict["agent_id"],
            episode=metadata_dict["episode"],
            step=metadata_dict["step"],
            reward=metadata_dict["reward"],
            status=CheckpointStatus(metadata_dict["status"]),
            file_size=metadata_dict["file_size"],
            checksum=metadata_dict["checksum"],
            description=metadata_dict.get("description"),
            metadata=metadata_dict.get("metadata", {})
        )
        
        return cls(metadata, data["data"])
    
    def calculate_checksum(self) -> str:
        """Calculate checksum of checkpoint data."""
        data_str = json.dumps(self.data, sort_keys=True)
        checksum = hashlib.sha256(data_str.encode()).hexdigest()
        return checksum
    
    def validate_checksum(self) -> bool:
        """Validate checksum of checkpoint."""
        calculated_checksum = self.calculate_checksum()
        return calculated_checksum == self.metadata.checksum
    
    def is_valid(self) -> bool:
        """Check if checkpoint is valid."""
        try:
            # Check if all required fields are present
            if not all([
                self.metadata.checkpoint_id,
                self.metadata.timestamp,
                self.metadata.version,
                self.metadata.agent_id,
                self.metadata.status
            ]):
                return False
            
            # Check checksum
            if not self.validate_checksum():
                return False
            
            return True
        except Exception as e:
            logger.warning(f"Checkpoint validation error: {e}")
            return False


class CheckpointManager:
    """Manager for checkpoint operations."""
    
    def __init__(self, checkpoint_dir: str = "./checkpoints"):
        """
        Initialize checkpoint manager.
        
        Parameters
        ----------
        checkpoint_dir : str, optional
            Directory to store checkpoints, by default "./checkpoints"
        """
        self.checkpoint_dir = Path(checkpoint_dir)
        self.checkpoint_dir.mkdir(parents=True, exist_ok=True)
        
        # Create index file
        self.index_file = self.checkpoint_dir / "checkpoint_index.json"
        self.index = self._load_index()
    
    def _load_index(self) -> Dict[str, Any]:
        """Load checkpoint index."""
        if self.index_file.exists():
            try:
                with open(self.index_file, 'r') as f:
                    return json.load(f)
            except Exception as e:
                logger.warning(f"Failed to load checkpoint index: {e}")
                return {"checkpoints": [], "versions": set(), "agents": set()}
        else:
            return {"checkpoints": [], "versions": set(), "agents": set()}
    
    def _save_index(self) -> None:
        """Save checkpoint index."""
        with open(self.index_file, 'w') as f:
            json.dump(self.index, f, indent=2)
    
    def create_checkpoint(self, agent_id: str, episode: int, step: int,
                         data: Dict[str, Any], version: str = "1.0.0",
                         description: Optional[str] = None) -> Checkpoint:
        """
        Create a new checkpoint.
        
        Parameters
        ----------
        agent_id : str
            Agent identifier
        episode : int
            Episode number
        step : int
            Step number
        data : Dict[str, Any]
            Checkpoint data
        version : str, optional
            Checkpoint version, by default "1.0.0"
        description : Optional[str], optional
            Checkpoint description, by default None
            
        Returns
        -------
        Checkpoint
            Created checkpoint
        """
        # Generate unique checkpoint ID
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        checksum = hashlib.sha256(json.dumps(data, sort_keys=True).encode()).hexdigest()
        checkpoint_id = f"{agent_id}_{episode}_{step}_{timestamp}"
        
        # Create metadata
        metadata = CheckpointMetadata(
            checkpoint_id=checkpoint_id,
            timestamp=datetime.now().isoformat(),
            version=version,
            agent_id=agent_id,
            episode=episode,
            step=step,
            reward=data.get("reward", 0.0),
            status=CheckpointStatus.CREATED,
            file_size=len(json.dumps(data)),
            checksum=checksum,
            description=description,
            metadata={"created_by": "pynlm", "format": "json"}
        )
        
        # Create checkpoint
        checkpoint = Checkpoint(metadata, data)
        
        # Save checkpoint to file
        self._save_checkpoint(checkpoint)
        
        # Update index
        self._add_to_index(checkpoint)
        
        logger.info(f"Created checkpoint {checkpoint_id} for agent {agent_id}")
        return checkpoint
    
    def _save_checkpoint(self, checkpoint: Checkpoint) -> None:
        """Save checkpoint to file."""
        checkpoint_file = self.checkpoint_dir / f"{checkpoint.metadata.checkpoint_id}.json"
        
        with open(checkpoint_file, 'w') as f:
            json.dump(checkpoint.to_dict(), f, indent=2)
        
        # Update checkpoint status
        checkpoint.metadata.status = CheckpointStatus.SAVED
    
    def _add_to_index(self, checkpoint: Checkpoint) -> None:
        """Add checkpoint to index."""
        checkpoint_info = {
            "checkpoint_id": checkpoint.metadata.checkpoint_id,
            "timestamp": checkpoint.metadata.timestamp,
            "version": checkpoint.metadata.version,
            "agent_id": checkpoint.metadata.agent_id,
            "episode": checkpoint.metadata.episode,
            "step": checkpoint.metadata.step,
            "reward": checkpoint.metadata.reward,
            "status": checkpoint.metadata.status.value,
            "file_size": checkpoint.metadata.file_size,
            "checksum": checkpoint.metadata.checksum,
            "description": checkpoint.metadata.description,
            "valid": checkpoint.is_valid()
        }
        
        self.index["checkpoints"].append(checkpoint_info)
        self.index["versions"].add(checkpoint.metadata.version)
        self.index["agents"].add(checkpoint.metadata.agent_id)
        
        self._save_index()
    
    def load_checkpoint(self, checkpoint_id: str) -> Optional[Checkpoint]:
        """
        Load checkpoint by ID.
        
        Parameters
        ----------
        checkpoint_id : str
            Checkpoint ID
            
        Returns
        -------
        Optional[Checkpoint]
            Loaded checkpoint or None if not found
        """
        # Find checkpoint in index
        checkpoint_info = None
        for info in self.index["checkpoints"]:
            if info["checkpoint_id"] == checkpoint_id:
                checkpoint_info = info
                break
        
        if not checkpoint_info:
            logger.warning(f"Checkpoint not found: {checkpoint_id}")
            return None
        
        # Load checkpoint from file
        checkpoint_file = self.checkpoint_dir / f"{checkpoint_id}.json"
        if not checkpoint_file.exists():
            logger.warning(f"Checkpoint file not found: {checkpoint_file}")
            return None
        
        try:
            with open(checkpoint_file, 'r') as f:
                checkpoint_data = json.load(f)
            
            checkpoint = Checkpoint.from_dict(checkpoint_data)
            
            if checkpoint.is_valid():
                checkpoint.metadata.status = CheckpointStatus.LOADED
                self._save_index()
                logger.info(f"Loaded checkpoint {checkpoint_id}")
                return checkpoint
            else:
                checkpoint.metadata.status = CheckpointStatus.CORRUPTED
                logger.warning(f"Checkpoint is corrupted: {checkpoint_id}")
                self._save_index()
                return None
                
        except Exception as e:
            logger.error(f"Failed to load checkpoint {checkpoint_id}: {e}")
            return None
    
    def delete_checkpoint(self, checkpoint_id: str) -> bool:
        """
        Delete checkpoint.
        
        Parameters
        ----------
        checkpoint_id : str
            Checkpoint ID
            
        Returns
        -------
        bool
            True if checkpoint was deleted, False otherwise
        """
        # Find checkpoint in index
        checkpoint_info = None
        for info in self.index["checkpoints"]:
            if info["checkpoint_id"] == checkpoint_id:
                checkpoint_info = info
                break
        
        if not checkpoint_info:
            logger.warning(f"Checkpoint not found for deletion: {checkpoint_id}")
            return False
        
        # Delete checkpoint file
        checkpoint_file = self.checkpoint_dir / f"{checkpoint_id}.json"
        if checkpoint_file.exists():
            checkpoint_file.unlink()
        
        # Remove from index
        self.index["checkpoints"] = [info for info in self.index["checkpoints"] 
                                    if info["checkpoint_id"] != checkpoint_id]
        self.index["versions"] = {v for v in self.index["versions"] 
                                if self._get_checkpoint_version(checkpoint_id, v) is not None}
        self.index["agents"] = {a for a in self.index["agents"] 
                               if self._get_checkpoint_agent(checkpoint_id, a) is not None}
        
        self._save_index()
        
        logger.info(f"Deleted checkpoint {checkpoint_id}")
        return True
    
    def _get_checkpoint_version(self, checkpoint_id: str, version: str) -> Optional[Dict[str, Any]]:
        """Get checkpoint info for specific version."""
        for info in self.index["checkpoints"]:
            if info["checkpoint_id"] == checkpoint_id and info["version"] == version:
                return info
        return None
    
    def _get_checkpoint_agent(self, checkpoint_id: str, agent_id: str) -> Optional[Dict[str, Any]]:
        """Get checkpoint info for specific agent."""
        for info in self.index["checkpoints"]:
            if info["checkpoint_id"] == checkpoint_id and info["agent_id"] == agent_id:
                return info
        return None
    
    def get_checkpoint(self, checkpoint_id: str) -> Optional[Dict[str, Any]]:
        """
        Get checkpoint information.
        
        Parameters
        ----------
        checkpoint_id : str
            Checkpoint ID
            
        Returns
        -------
        Optional[Dict[str, Any]]
            Checkpoint information or None if not found
        """
        for info in self.index["checkpoints"]:
            if info["checkpoint_id"] == checkpoint_id:
                return info
        return None
    
    def list_checkpoints(self, agent_id: Optional[str] = None, 
                        version: Optional[str] = None) -> List[Dict[str, Any]]:
        """
        List checkpoints with optional filtering.
        
        Parameters
        ----------
        agent_id : Optional[str], optional
            Filter by agent ID, by default None
        version : Optional[str], optional
            Filter by version, by default None
            
        Returns
        -------
        List[Dict[str, Any]]
            List of checkpoint information
        """
        checkpoints = self.index["checkpoints"]
        
        # Apply filters
        if agent_id:
            checkpoints = [c for c in checkpoints if c["agent_id"] == agent_id]
        
        if version:
            checkpoints = [c for c in checkpoints if c["version"] == version]
        
        # Sort by timestamp (descending)
        checkpoints.sort(key=lambda x: x["timestamp"], reverse=True)
        
        return checkpoints
    
    def get_latest_checkpoint(self, agent_id: Optional[str] = None) -> Optional[Dict[str, Any]]:
        """
        Get latest checkpoint for agent.
        
        Parameters
        ----------
        agent_id : Optional[str], optional
            Filter by agent ID, by default None
            
        Returns
        -------
        Optional[Dict[str, Any]]
            Latest checkpoint information or None
        """
        checkpoints = self.list_checkpoints(agent_id=agent_id)
        return checkpoints[0] if checkpoints else None
    
    def backup_checkpoint(self, checkpoint_id: str, backup_dir: str = "./backups") -> bool:
        """
        Create backup of checkpoint.
        
        Parameters
        ----------
        checkpoint_id : str
            Checkpoint ID
        backup_dir : str, optional
            Backup directory, by default "./backups"
            
        Returns
        -------
        bool
            True if backup was created, False otherwise
        """
        backup_path = Path(backup_dir)
        backup_path.mkdir(parents=True, exist_ok=True)
        
        checkpoint_file = self.checkpoint_dir / f"{checkpoint_id}.json"
        backup_file = backup_path / f"{checkpoint_id}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        if checkpoint_file.exists():
            shutil.copy2(checkpoint_file, backup_file)
            logger.info(f"Created backup of checkpoint {checkpoint_id} at {backup_file}")
            return True
        
        logger.warning(f"Could not create backup: checkpoint file not found {checkpoint_id}")
        return False
    
    def restore_backup(self, backup_path: str, restore_to: str = None) -> Optional[str]:
        """
        Restore checkpoint from backup.
        
        Parameters
        ----------
        backup_path : str
            Path to backup file
        restore_to : str, optional
            Restore to this directory, by default None (same directory)
            
        Returns
        -------
        Optional[str]
            Restored checkpoint ID or None if failed
        """
        backup_path = Path(backup_path)
        if not backup_path.exists():
            logger.error(f"Backup file not found: {backup_path}")
            return None
        
        # Extract checkpoint ID from filename
        checkpoint_id = backup_path.stem
        if restore_to:
            target_dir = Path(restore_to)
        else:
            target_dir = self.checkpoint_dir
        
        target_dir.mkdir(parents=True, exist_ok=True)
        target_file = target_dir / backup_path.name
        
        # Copy backup to target location
        shutil.copy2(backup_path, target_file)
        
        # Update index (this is simplified - in reality you'd want to parse the backup)
        self._add_to_index_from_file(target_file)
        
        logger.info(f"Restored checkpoint {checkpoint_id} from backup")
        return checkpoint_id
    
    def _add_to_index_from_file(self, checkpoint_file: Path) -> None:
        """Add checkpoint to index from file."""
        try:
            with open(checkpoint_file, 'r') as f:
                checkpoint_data = json.load(f)
            
            metadata = checkpoint_data["metadata"]
            checkpoint_info = {
                "checkpoint_id": metadata["checkpoint_id"],
                "timestamp": metadata["timestamp"],
                "version": metadata["version"],
                "agent_id": metadata["agent_id"],
                "episode": metadata["episode"],
                "step": metadata["step"],
                "reward": metadata["reward"],
                "status": metadata["status"],
                "file_size": metadata["file_size"],
                "checksum": metadata["checksum"],
                "description": metadata.get("description"),
                "valid": True
            }
            
            # Check if already in index
            for i, info in enumerate(self.index["checkpoints"]):
                if info["checkpoint_id"] == checkpoint_info["checkpoint_id"]:
                    self.index["checkpoints"][i] = checkpoint_info
                    break
            else:
                self.index["checkpoints"].append(checkpoint_info)
                self.index["versions"].add(metadata["version"])
                self.index["agents"].add(metadata["agent_id"])
            
            self._save_index()
            
        except Exception as e:
            logger.error(f"Failed to add checkpoint to index from file: {e}")
    
    def cleanup_old_checkpoints(self, max_checkpoints_per_agent: int = 10,
                              max_age_days: int = 30) -> None:
        """
        Clean up old checkpoints.
        
        Parameters
        ----------
        max_checkpoints_per_agent : int, optional
            Maximum checkpoints per agent, by default 10
        max_age_days : int, optional
            Maximum age in days, by default 30
        """
        current_time = datetime.now()
        to_delete = []
        
        for checkpoint_info in self.index["checkpoints"]:
            checkpoint_id = checkpoint_info["checkpoint_id"]
            
            # Check if checkpoint has exceeded age limit
            try:
                checkpoint_time = datetime.fromisoformat(checkpoint_info["timestamp"])
                age_days = (current_time - checkpoint_time).days
                if age_days > max_age_days:
                    to_delete.append(checkpoint_id)
                    continue
            except Exception as e:
                logger.warning(f"Failed to parse timestamp for checkpoint {checkpoint_id}: {e}")
                continue
            
            # Check if agent has too many checkpoints
            agent_checkpoints = [c for c in self.index["checkpoints"]
                               if c["agent_id"] == checkpoint_info["agent_id"]]
            if len(agent_checkpoints) > max_checkpoints_per_agent:
                # Keep the most recent ones
                agent_checkpoints.sort(key=lambda x: x["timestamp"], reverse=True)
                to_delete.extend([c["checkpoint_id"] for c in agent_checkpoints[max_checkpoints_per_agent:]])
        
        # Delete old checkpoints
        for checkpoint_id in set(to_delete):
            if self.delete_checkpoint(checkpoint_id):
                logger.info(f"Cleaned up old checkpoint: {checkpoint_id}")
    
    def get_statistics(self) -> Dict[str, Any]:
        """
        Get checkpoint statistics.
        
        Returns
        -------
        Dict[str, Any]
            Checkpoint statistics
        """
        total_checkpoints = len(self.index["checkpoints"])
        
        stats = {
            "total_checkpoints": total_checkpoints,
            "num_agents": len(self.index["agents"]),
            "num_versions": len(self.index["versions"]),
            "agents": list(self.index["agents"]),
            "versions": list(self.index["versions"])
        }
        
        # Add per-agent statistics
        agent_stats = {}
        for agent_id in self.index["agents"]:
            agent_checkpoints = self.list_checkpoints(agent_id=agent_id)
            if agent_checkpoints:
                agent_stats[agent_id] = {
                    "checkpoint_count": len(agent_checkpoints),
                    "latest_checkpoint": agent_checkpoints[0]["checkpoint_id"],
                    "latest_timestamp": agent_checkpoints[0]["timestamp"],
                    "latest_reward": agent_checkpoints[0]["reward"],
                    "total_reward": sum(c["reward"] for c in agent_checkpoints)
                }
        
        stats["agent_statistics"] = agent_stats
        
        return stats


class AutoCheckpoint:
    """Automatic checkpointing."""
    
    def __init__(self, checkpoint_manager: CheckpointManager, 
                 save_frequency: int = 100, max_checkpoints: int = 50):
        """
        Initialize automatic checkpointing.
        
        Parameters
        ----------
        checkpoint_manager : CheckpointManager
            Checkpoint manager
        save_frequency : int, optional
            Save frequency, by default 100
        max_checkpoints : int, optional
            Maximum number of checkpoints, by default 50
        """
        self.checkpoint_manager = checkpoint_manager
        self.save_frequency = save_frequency
        self.max_checkpoints = max_checkpoints
        
        # Track last saved step
        self.last_saved_step = -1
        
        # Track checkpoints for cleanup
        self.checkpoint_history = []
    
    def maybe_save_checkpoint(self, agent_id: str, episode: int, step: int,
                             data: Dict[str, Any], force: bool = False) -> Optional[Checkpoint]:
        """
        Save checkpoint if conditions are met.
        
        Parameters
        ----------
        agent_id : str
            Agent identifier
        episode : int
            Episode number
        step : int
            Step number
        data : Dict[str, Any]
            Checkpoint data
        force : bool, optional
            Force save even if not at frequency, by default False
            
        Returns
        -------
        Optional[Checkpoint]
            Saved checkpoint or None
        """
        # Check if we should save
        should_save = (
            force or 
            step % self.save_frequency == 0 or 
            step == 0 or 
            data.get("done", False)
        )
        
        if not should_save:
            return None
        
        # Check if we have too many checkpoints
        agent_checkpoints = self.checkpoint_manager.list_checkpoints(agent_id=agent_id)
        if len(agent_checkpoints) >= self.max_checkpoints:
            # Clean up old checkpoints
            self.checkpoint_manager.cleanup_old_checkpoints(
                max_checkpoints_per_agent=self.max_checkpoints,
                max_age_days=30
            )
        
        # Save checkpoint
        checkpoint = self.checkpoint_manager.create_checkpoint(
            agent_id=agent_id,
            episode=episode,
            step=step,
            data=data,
            description=f"Automatic checkpoint at episode {episode}, step {step}"
        )
        
        # Update history
        self.checkpoint_history.append({
            "checkpoint_id": checkpoint.metadata.checkpoint_id,
            "timestamp": checkpoint.metadata.timestamp,
            "step": checkpoint.metadata.step,
            "episode": checkpoint.metadata.episode,
            "reward": checkpoint.metadata.reward
        })
        
        self.last_saved_step = step
        
        logger.info(f"Automatic checkpoint saved: {checkpoint.metadata.checkpoint_id}")
        return checkpoint
    
    def get_checkpoint_history(self) -> List[Dict[str, Any]]:
        """
        Get checkpoint history.
        
        Returns
        -------
        List[Dict[str, Any]]
            Checkpoint history
        """
        return self.checkpoint_history.copy()
    
    def clear_history(self) -> None:
        """Clear checkpoint history."""
        self.checkpoint_history = []
        self.last_saved_step = -1
        logger.info("Cleared checkpoint history")


# Package information
__version__ = "1.0.0"
__author__ = "NLM Team"
__description__ = "Checkpoint management utilities for NLM project"

# Export all classes
__all__ = [
    'CheckpointStatus',
    'CheckpointMetadata',
    'Checkpoint',
    'CheckpointManager',
    'AutoCheckpoint'
]

# Example usage function
def example_usage():
    """Example usage of the checkpoint module."""
    # Create checkpoint manager
    checkpoint_manager = CheckpointManager("./checkpoints")
    
    # Create sample data
    sample_data = {
        "agent_id": "agent_1",
        "position": [100, 200],
        "velocity": [5, 3],
        "health": 0.8,
        "energy": 0.9,
        "memory": {"last_action": "move_right"},
        "reward": 15.5,
        "done": False
    }
    
    # Create checkpoint
    checkpoint = checkpoint_manager.create_checkpoint(
        agent_id="agent_1",
        episode=10,
        step=100,
        data=sample_data,
        description="Test checkpoint"
    )
    
    # Load checkpoint
    loaded_checkpoint = checkpoint_manager.load_checkpoint(checkpoint.metadata.checkpoint_id)
    
    # Get statistics
    stats = checkpoint_manager.get_statistics()
    
    return {
        "checkpoint_created": checkpoint is not None,
        "checkpoint_loaded": loaded_checkpoint is not None,
        "checkpoint_id": checkpoint.metadata.checkpoint_id if checkpoint else None,
        "statistics": stats
    }

if __name__ == "__main__":
    print("Testing pynlm.checkpoint module...")
    
    # Run example usage
    results = example_usage()
    print("Example usage completed successfully!")
    print(f"Checkpoint created: {results['checkpoint_created']}")
    print(f"Checkpoint loaded: {results['checkpoint_loaded']}")
    print(f"Checkpoint ID: {results['checkpoint_id']}")
    print(f"Statistics: {results['statistics']}")