"""
Safety and Transaction Management for Power User Operations

This module provides safety and transaction management for power user operations,
including rollback points, transaction handling, and operation mode control.
"""

import time
import threading
import hashlib
import json
from typing import Dict, List, Optional, Any
from enum import Enum, auto

class PowerUserError(Exception):
    """Base exception for power user operations"""
    pass

class RollbackPointError(PowerUserError):
    """Raised when rollback point operations fail"""
    pass

class TransactionError(PowerUserError):
    """Raised when transaction operations fail"""
    pass

class SafetyError(PowerUserError):
    """Raised when safety checks fail"""
    pass

class PermissionError(PowerUserError):
    """Raised when insufficient permissions for operation"""
    pass

class ValidationError(PowerUserError):
    """Raised when validation fails"""
    pass

class OperationMode(Enum):
    """Operation modes for different safety levels"""
    SAFE = auto()           # Read-only operations only
    CAUTIOUS = auto()       # Write operations with safety checks
    ADVANCED = auto()       # Full write operations with rollback
    EXPERT = auto()         # Unrestricted operations with backup

class RollbackPoint:
    """Rollback point for transaction operations"""
    
    def __init__(self, name: str, brain_state: Dict, timestamp: float):
        self.name = name
        self.brain_state = brain_state
        self.timestamp = timestamp
        self.checksum = self._calculate_checksum(brain_state)
    
    def _calculate_checksum(self, state: Dict) -> str:
        """Calculate checksum of state for validation"""
        state_str = json.dumps(state, sort_keys=True)
        return hashlib.sha256(state_str.encode()).hexdigest()
    
    def validate(self, current_state: Dict) -> bool:
        """Validate that state hasn't been modified"""
        current_checksum = self._calculate_checksum(current_state)
        return current_checksum == self.checksum
class SafetyManager:
    """Safety and transaction management for power user operations"""
    
    def __init__(self):
        self.operation_mode = OperationMode.CAUTIOUS
        self.rollback_points: Dict[str, RollbackPoint] = {}
        self.transaction_stack: List[Dict] = []
        self.safety_lock = threading.RLock()
        self.backup_enabled = True
        self.auto_rollback = True
        self.max_rollback_depth = 100
    
    def set_operation_mode(self, mode: OperationMode):
        """Set operation mode for safety control"""
        with self.safety_lock:
            if mode == OperationMode.EXPERT:
                import warnings
                warnings.warn("EXPERT mode enabled - dangerous operations allowed!")
            self.operation_mode = mode
    
    def create_rollback_point(self, name: str, brain_state: Dict) -> str:
        """Create a rollback point for current brain state"""
        with self.safety_lock:
            if len(self.rollback_points) >= self.max_rollback_depth:
                # Remove oldest rollback point
                oldest_key = next(iter(self.rollback_points))
                del self.rollback_points[oldest_key]
            
            rollback_point = RollbackPoint(name, brain_state, time.time())
            self.rollback_points[name] = rollback_point
            return name
    
    def rollback(self, name: str) -> bool:
        """Rollback to a previous state"""
        with self.safety_lock:
            if name not in self.rollback_points:
                raise RollbackPointError(f"Rollback point '{name}' not found")
            
            rollback_point = self.rollback_points[name]
            # Note: Actual rollback would require integration with brain state
            # This is a simplified implementation
            del self.rollback_points[name]
            return True
    
    def begin_transaction(self, name: str) -> Dict:
        """Begin a transaction for a series of operations"""
        with self.safety_lock:
            if self.operation_mode == OperationMode.SAFE:
                raise PermissionError("Transaction operations not allowed in SAFE mode")
            
            transaction = {
                'name': name,
                'start_time': time.time(),
                'operations': [],
                'rollback_point': None
            }
            
            if self.backup_enabled:
                transaction['rollback_point'] = self.create_rollback_point(
                    f"transaction_{name}", {}
                )
            
            self.transaction_stack.append(transaction)
            return transaction
    
    def commit_transaction(self, transaction: Dict) -> bool:
        """Commit a transaction"""
        with self.safety_lock:
            if not self.transaction_stack or self.transaction_stack[-1] != transaction:
                raise TransactionError("Invalid transaction state")
            
            self.transaction_stack.pop()
            return True
    
    def rollback_transaction(self, transaction: Dict) -> bool:
        """Rollback a transaction"""
        with self.safety_lock:
            if not self.transaction_stack or self.transaction_stack[-1] != transaction:
                raise TransactionError("Invalid transaction state")
            
            # Rollback operations in reverse order
            for operation in reversed(transaction['operations']):
                try:
                    operation['undo']()
                except Exception:
                    pass  # Ignore errors during rollback
            
            self.transaction_stack.pop()
            return True
    
    def validate_safety(self, operation: str, params: Dict) -> bool:
        """Validate safety of an operation"""
        with self.safety_lock:
            if self.operation_mode == OperationMode.SAFE:
                # Only allow read-only operations in SAFE mode
                read_only_ops = ['get_neuron_state', 'get_synapse_state', 
                               'query_memory', 'get_simulation_state']
                if operation not in read_only_ops:
                    raise SafetyError(f"Operation '{operation}' not allowed in SAFE mode")
            
            return True
    
    def validate_operation(self, operation: str, params: Dict) -> bool:
        """Validate safety of an operation"""
        with self.safety_lock:
            if self.operation_mode == OperationMode.SAFE:
                # Only allow read-only operations in SAFE mode
                read_only_ops = ['get_neuron_state', 'get_synapse_state', 
                               'query_memory', 'get_simulation_state']
                if operation not in read_only_ops:
                    raise SafetyError(f"Operation '{operation}' not allowed in SAFE mode")
            
            return True