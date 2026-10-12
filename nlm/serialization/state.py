#!/usr/bin/env python3
"""
NLM State Module - Advanced State Management

This module provides sophisticated state management with version control,
validation, and rollback capabilities.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Union, Tuple
import time
import json
import hashlib
from pathlib import Path
from copy import deepcopy

from nlm.core.brain import Brain

class StateManager:
    """Advanced state management for neural simulation.
    
    Provides sophisticated state management with version control,
    validation, and rollback capabilities.
    
    Features:
    * State versioning
    * State validation
    * Rollback support
    * State comparison
    * State hashing
    * Metadata tracking
    """
    
    def __init__(self):
        self.states = {}
        self.state_versions = {}
        self.current_state_name = ""
        self.metadata = {}
        self.version_history = []
    
    def save_state(self, name: str, data: Dict[str, Any], metadata: Optional[Dict] = None):
        """Save state with metadata.
        
        Args:
            name: State name
            data: State data to save
            metadata: Optional metadata to store with state
        """
        print(f"Saving state: {name}")
        
        # Generate version number
        version = self._get_next_version(name)
        
        # Create state object
        state = {
            "name": name,
            "version": version,
            "timestamp": time.time(),
            "data": deepcopy(data),
            "metadata": metadata or {}
        }
        
        # Add checksum for integrity validation
        state["checksum"] = self._calculate_checksum(state)
        
        # Save state
        self.states[name] = state
        self.state_versions[name] = version
        self.current_state_name = name
        
        # Track version history
        self.version_history.append({
            "name": name,
            "version": version,
            "timestamp": time.time(),
            "action": "save"
        })
        
        print(f"State saved: {name} v{version}")
        return version
    
    def load_state(self, name: str) -> Dict[str, Any]:
        """Load state by name.
        
        Args:
            name: State name to load
            
        Returns:
            State data without metadata and checksum
        """
        if name not in self.states:
            raise ValueError(f"State not found: {name}")
        
        state = self.states[name]
        
        # Validate integrity
        if not self._validate_state_integrity(state):
            raise ValueError(f"State integrity validation failed: {name}")
        
        # Track version history
        self.version_history.append({
            "name": name,
            "version": state["version"],
            "timestamp": time.time(),
            "action": "load"
        })
        
        print(f"State loaded: {name} v{state['version']}")
        return state["data"]
    
    def has_state(self, name: str) -> bool:
        """Check if state exists.
        
        Args:
            name: State name to check
            
        Returns:
            True if state exists, False otherwise
        """
        return name in self.states
    
    def delete_state(self, name: str):
        """Delete state by name.
        
        Args:
            name: State name to delete
        """
        if name in self.states:
            version = self.states[name]["version"]
            del self.states[name]
            del self.state_versions[name]
            
            # Track version history
            self.version_history.append({
                "name": name,
                "version": version,
                "timestamp": time.time(),
                "action": "delete"
            })
            
            print(f"State deleted: {name} v{version}")
    
    def list_states(self) -> List[str]:
        """List all saved states."""
        return sorted(self.states.keys())
    
    def compare_states(self, name1: str, name2: str) -> Dict[str, Any]:
        """Compare two states.
        
        Args:
            name1: First state name
            name2: Second state name
            
        Returns:
            Comparison results
        """
        if name1 not in self.states or name2 not in self.states:
            raise ValueError(f"One or both states not found: {name1}, {name2}")
        
        state1 = self.states[name1]
        state2 = self.states[name2]
        
        # Compare versions
        version1 = state1["version"]
        version2 = state2["version"]
        
        # Compare data (basic comparison)
        data1 = state1["data"]
        data2 = state2["data"]
        
        differences = self._compare_data(data1, data2)
        
        comparison = {
            "state1": {
                "name": name1,
                "version": version1,
                "timestamp": state1["timestamp"],
                "data_keys": list(data1.keys()) if isinstance(data1, dict) else "not_dict"
            },
            "state2": {
                "name": name2,
                "version": version2,
                "timestamp": state2["timestamp"],
                "data_keys": list(data2.keys()) if isinstance(data2, dict) else "not_dict"
            },
            "differences": differences,
            "similarity_score": self._calculate_similarity_score(data1, data2)
        }
        
        return comparison
    
    def rollback_state(self, name: str, version: Optional[int] = None):
        """Rollback state to specific version.
        
        Args:
            name: State name
            version: Version to rollback to (latest if None)
        """
        if name not in self.states:
            raise ValueError(f"State not found: {name}")
        
        state_versions = [s for s in self.states[name].get("all_versions", [])]
        if not state_versions:
            raise ValueError(f"No versions available for state: {name}")
        
        if version is None:
            # Rollback to latest version (just return current state)
            target_state = self.states[name]
        else:
            # Find specific version
            target_state = None
            for state in state_versions:
                if state["version"] == version:
                    target_state = state
                    break
            
            if target_state is None:
                raise ValueError(f"Version {version} not found for state: {name}")
        
        # Save current state as a snapshot before rollback
        current_name = f"{name}_backup_{int(time.time())}"
        self.save_state(current_name, self.states[name]["data"], {
            "type": "backup",
            "original_name": name
        })
        
        # Update current state
        self.states[name] = target_state
        self.current_state_name = name
        
        # Track version history
        self.version_history.append({
            "name": name,
            "version": target_state["version"],
            "timestamp": time.time(),
            "action": "rollback"
        })
        
        print(f"State rolled back: {name} to v{target_state['version']}")
    
    def get_state_version(self, name: str) -> int:
        """Get state version.
        
        Args:
            name: State name
            
        Returns:
            Version number
        """
        if name not in self.states:
            raise ValueError(f"State not found: {name}")
        
        return self.states[name]["version"]
    
    def get_state_metadata(self, name: str) -> Dict[str, Any]:
        """Get state metadata.
        
        Args:
            name: State name
            
        Returns:
            State metadata
        """
        if name not in self.states:
            raise ValueError(f"State not found: {name}")
        
        return self.states[name]["metadata"].copy()
    
    def validate_state(self, name: str) -> bool:
        """Validate state integrity.
        
        Args:
            name: State name
            
        Returns:
            True if state is valid, False otherwise
        """
        if name not in self.states:
            return False
        
        return self._validate_state_integrity(self.states[name])
    
    def compute_state_hash(self, name: str) -> str:
        """Compute hash of state.
        
        Args:
            name: State name
            
        Returns:
            State hash
        """
        if name not in self.states:
            raise ValueError(f"State not found: {name}")
        
        return self._calculate_checksum(self.states[name])
    
    def get_version_history(self, name: str) -> List[Dict[str, Any]]:
        """Get version history for state.
        
        Args:
            name: State name
            
        Returns:
            Version history entries
        """
        return [entry for entry in self.version_history if entry["name"] == name]
    
    def clear_state(self, name: str = ""):
        """Clear specific state or all states.
        
        Args:
            name: State name to clear (all if empty)
        """
        if name:
            if name in self.states:
                del self.states[name]
                print(f"State cleared: {name}")
        else:
            self.states.clear()
            print("All states cleared")
    
    def get_current_state(self) -> str:
        """Get current state name."""
        return self.current_state_name
    
    def __repr__(self):
        return f"StateManager(states={len(self.states)}, current_state='{self.current_state_name}')"
    
    def _get_next_version(self, name: str) -> int:
        """Get next version number for state."""
        if name not in self.state_versions:
            return 1
        
        # Check if current state has all_versions
        if name in self.states and "all_versions" in self.states[name]:
            versions = [s["version"] for s in self.states[name]["all_versions"]]
            return max(versions) + 1
        
        # Otherwise use version counter
        return self.state_versions[name] + 1
    
    def _calculate_checksum(self, state: Dict[str, Any]) -> str:
        """Calculate checksum for state."""
        data_to_check = {k: v for k, v in state.items() if k != "checksum"}
        json_str = json.dumps(data_to_check, sort_keys=True)
        return hashlib.md5(json_str.encode()).hexdigest()
    
    def _validate_state_integrity(self, state: Dict[str, Any]) -> bool:
        """Validate state integrity."""
        # Check required fields
        required_fields = ["name", "version", "timestamp", "data", "metadata"]
        for field in required_fields:
            if field not in state:
                return False
        
        # Verify checksum
        if "checksum" in state:
            calculated_checksum = self._calculate_checksum(state)
            if calculated_checksum != state["checksum"]:
                return False
        
        return True
    
    def _compare_data(self, data1: Any, data2: Any) -> Dict[str, Any]:
        """Compare two data structures."""
        differences = {
            "types_match": type(data1) == type(data2),
            "values_match": data1 == data2
        }
        
        if isinstance(data1, dict) and isinstance(data2, dict):
            keys1 = set(data1.keys())
            keys2 = set(data2.keys())
            
            differences["keys1_only"] = list(keys1 - keys2)
            differences["keys2_only"] = list(keys2 - keys1)
            
            common_keys = keys1 & keys2
            for key in common_keys:
                differences[f"diff_{key}"] = self._compare_data(data1[key], data2[key])
        
        return differences
    
    def _calculate_similarity_score(self, data1: Any, data2: Any) -> float:
        """Calculate similarity score between two data structures."""
        if data1 == data2:
            return 100.0
        
        if isinstance(data1, dict) and isinstance(data2, dict):
            keys1 = set(data1.keys())
            keys2 = set(data2.keys())
            
            if not keys1 and not keys2:
                return 100.0
            
            common_keys = keys1 & keys2
            total_keys = len(keys1) + len(keys2)
            
            if total_keys == 0:
                return 100.0
            
            similarity = (len(common_keys) / total_keys) * 100
            
            # Add similarity of common values
            for key in common_keys:
                value_similarity = self._calculate_similarity_score(data1[key], data2[key])
                similarity += value_similarity / len(common_keys)
            
            return similarity
        
        return 0.0

if __name__ == "__main__":
    # Example usage
    print("Running State Manager Example")
    
    # Create state manager
    state_manager = StateManager()
    
    # Save some states
    state_manager.save_state("config1", {"neurons": 1000, "rate": 0.1}, {"source": "default"})
    state_manager.save_state("config2", {"neurons": 2000, "rate": 0.05}, {"source": "optimized"})
    
    # Save more versions of first state
    state_manager.save_state("config1", {"neurons": 1000, "rate": 0.15}, {"source": "updated", "version": 2})
    state_manager.save_state("config1", {"neurons": 1000, "rate": 0.2}, {"source": "optimized", "version": 3})
    
    # List states
    print("Available states:", state_manager.list_states())
    
    # Load a state
    config = state_manager.load_state("config2")
    print(f"Loaded config: neurons={config['neurons']}, rate={config['rate']}")
    
    # Compare states
    comparison = state_manager.compare_states("config1", "config2")
    print(f"Similarity score: {comparison['similarity_score']:.1f}%")
    
    # Check state version
    print(f"config1 version: {state_manager.get_state_version('config1')}")
    
    # Get metadata
    metadata = state_manager.get_state_metadata("config1")
    print(f"config1 metadata: {metadata}")
    
    # Validate state
    is_valid = state_manager.validate_state("config1")
    print(f"config1 is valid: {is_valid}")
    
    # Compute hash
    state_hash = state_manager.compute_state_hash("config1")
    print(f"config1 hash: {state_hash}")
    
    # Get version history
    history = state_manager.get_version_history("config1")
    print(f"config1 version history: {len(history)} entries")
    
    # Test rollback (commented out as it would delete current state)
    # state_manager.rollback_state("config1", 2)
    
    # Clear state
    state_manager.clear_state("config1")
    
    print("State manager example completed!")
