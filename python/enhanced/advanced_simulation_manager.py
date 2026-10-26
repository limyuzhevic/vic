"""
Advanced Simulation Manager for NLM

This module provides advanced simulation control with protocol-based execution,
event systems, and enhanced step management for NLM systems.
"""

import sys
from pathlib import Path

# Add parent directory to Python path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

class AdvancedSimulationManager:
    """
    Advanced simulation manager with protocol-based control.
    
    This class provides enhanced simulation capabilities including:
    - Protocol-based execution
    - Event-driven execution
    - Phase markers
    - Conditional execution
    """
    
    def __init__(self):
        """Initialize advanced simulation manager"""
        try:
            from pynlm_enhanced import AdvancedSimulationManager as EnhancedManager
            self._manager = EnhancedManager()
        except ImportError:
            raise ImportError("Enhanced NLM bindings not available")
    
    def initialize(self, brain):
        """
        Initialize the simulation manager with a brain.
        
        Args:
            brain: Brain object to simulate
        """
        self._manager.initialize(brain)
    
    def add_protocol(self, name, steps, phase_markers=None):
        """
        Add a simulation protocol.
        
        Args:
            name: Protocol name
            steps: Number of steps to execute
            phase_markers: List of phase markers (optional)
        """
        if phase_markers is None:
            phase_markers = []
        self._manager.addProtocol(name, steps, phase_markers)
    
    def add_event(self, timestamp, event_type, data, handler):
        """
        Add a simulation event.
        
        Args:
            timestamp: Step at which event should occur
            event_type: Type of event
            data: Event data dictionary
            handler: Event handler function
        """
        self._manager.addEvent(timestamp, event_type, data, handler)
    
    def run_protocol(self, protocol_name):
        """
        Run a simulation protocol.
        
        Args:
            protocol_name: Name of protocol to run
            
        Returns:
            True if protocol completed successfully
        """
        return self._manager.runProtocol(protocol_name)