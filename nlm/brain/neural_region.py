"""
NeuralRegion class for NLM neural simulation framework.

This module provides a Python interface to the C++ NeuralRegion class with enhanced
Python features including automatic numpy conversion, error formatting, and
time measurement utilities.
"""

import time
import numpy as np
from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass
import traceback

# Python utilities
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
    numpy_utils,
)

# Forward references
RegionId = Any
PopulationId = Any
NeuronId = Any

@dataclass
class NeuronData:
    """Data structure for neuron information"""
    id: NeuronId
    type: str
    active: bool
    potential: float
    firing_rate: float

class NeuralRegion:
    """
    NeuralRegion class for NLM neural simulation framework.
    
    This class provides a Python interface to the underlying C++ NeuralRegion class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self, region_id: RegionId = None, name: str = ""):
        """
        Initialize neural region.
        
        Args:
            region_id: Region ID (optional)
            name: Region name (optional)
        """
        self._logger = PythonSessionLogger()
        self._region_id = region_id
        self._name = name
        self._neurons: Dict[RegionId, List[NeuronData]] = {}
        
        # Log initialization
        self._logger.logEntry(f"NeuralRegion initialized: id={self._region_id.value if self._region_id else 'None'}, name='{self._name}'")
    
    # Region ID and name properties
    
    @property
    def region_id(self) -> RegionId:
        """Get region ID."""
        return self._region_id
    
    @region_id.setter
    def region_id(self, value: RegionId) -> None:
        """
        Set region ID.
        
        Args:
            value: Region ID
        """
        self._region_id = value
    
    @property
    def name(self) -> str:
        """Get region name."""
        return self._name
    
    @name.setter
    def name(self, value: str) -> None:
        """
        Set region name.
        
        Args:
            value: Region name
        """
        self._name = value
    
    # Neuron management methods
    
    def addNeuron(self, neuron: 'Neuron') -> bool:
        """
        Add neuron to region.
        
        Args:
            neuron: Neuron to add
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if self._region_id not in self._neurons:
                self._neurons[self._region_id] = []
            
            neuron_data = NeuronData(
                id=neuron.id,
                type=neuron.type.name,
                active=neuron.active,
                potential=neuron.potential,
                firing_rate=neuron.firing_rate,
            )
            
            self._neurons[self._region_id].append(neuron_data)
            self._logger.logEntry(f"Added neuron to region: id={neuron.id.value}, region={self._region_id.value if self._region_id else 'None'}")
            
            return True
            
        except Exception as e:
            self._format_python_error(f"Failed to add neuron to region: {e}")
            return False
    
    def removeNeuron(self, neuron_id: NeuronId) -> bool:
        """
        Remove neuron from region.
        
        Args:
            neuron_id: Neuron ID to remove
            
        Returns:
            True if successful, False otherwise
        """
        try:
            for neuron_list in self._neurons.values():
                for i, neuron in enumerate(neuron_list):
                    if neuron.id == neuron_id:
                        del neuron_list[i]
                        self._logger.logEntry(f"Removed neuron from region: id={neuron_id.value}, region={self._region_id.value if self._region_id else 'None'}")
                        return True
            
            return False
            
        except Exception as e:
            self._format_python_error(f"Failed to remove neuron from region: {e}")
            return False
    
    def getNeuron(self, neuron_id: NeuronId) -> Optional[NeuronData]:
        """
        Get neuron from region.
        
        Args:
            neuron_id: Neuron ID to retrieve
            
        Returns:
            NeuronData object or None if not found
        """
        try:
            for neuron_list in self._neurons.values():
                for neuron in neuron_list:
                    if neuron.id == neuron_id:
                        return neuron
            
            return None
            
        except Exception:
            return None
    
    def getNeuronCount(self) -> int:
        """Get total number of neurons in region."""
        return sum(len(neuron_list) for neuron_list in self._neurons.values())
    
    # Region statistics
    
    def getTotalNeurons(self) -> int:
        """Get total number of neurons."""
        return self.getNeuronCount()
    
    def getActiveNeuronCount(self) -> int:
        """Get number of active neurons."""
        return sum(1 for neuron_list in self._neurons.values() 
                  for neuron in neuron_list if neuron.active)
    
    def getFiringNeuronCount(self) -> int:
        """Get number of firing neurons."""
        return sum(1 for neuron_list in self._neurons.values() 
                  for neuron in neuron_list if neuron.firing_rate > 0)
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate of region."""
        total_neurons = self.getTotalNeurons()
        if total_neurons == 0:
            return 0.0
        
        total_rate = sum(neuron.firing_rate for neuron_list in self._neurons.values() for neuron in neuron_list)
        return total_rate / total_neurons
    
    def getExcitatoryInhibitionRatio(self) -> float:
        """Get excitation/inhibition ratio of region."""
        excitatory_count = sum(1 for neuron_list in self._neurons.values() 
                              for neuron in neuron_list if neuron.type == 'Excitatory')
        inhibitory_count = sum(1 for neuron_list in self._neurons.values() 
                              for neuron in neuron_list if neuron.type == 'Inhibitory')
        
        total = excitatory_count + inhibitory_count
        if total == 0:
            return 1.0
        
        return float(excitatory_count) / float(inhibitory_count) if inhibitory_count > 0 else float("inf")
    
    # Region operations
    
    def activateAll(self) -> None:
        """Activate all neurons in region."""
        try:
            for neuron_list in self._neurons.values():
                for neuron in neuron_list:
                    neuron.active = True
            
            self._logger.logEntry(f"Activated all neurons in region: id={self._region_id.value if self._region_id else 'None'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to activate all neurons: {e}")
    
    def deactivateAll(self) -> None:
        """Deactivate all neurons in region."""
        try:
            for neuron_list in self._neurons.values():
                for neuron in neuron_list:
                    neuron.active = False
            
            self._logger.logEntry(f"Deactivated all neurons in region: id={self._region_id.value if self._region_id else 'None'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to deactivate all neurons: {e}")
    
    def reset(self) -> None:
        """Reset region state."""
        try:
            for neuron_list in self._neurons.values():
                for neuron in neuron_list:
                    neuron.active = False
                    neuron.potential = 0.0
                    neuron.firing_rate = 0.0
            
            self._logger.logEntry(f"Reset region: id={self._region_id.value if self._region_id else 'None'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to reset region: {e}")
    
    def update(self, dt: float) -> None:
        """
        Update region state.
        
        Args:
            dt: Time step
        """
        try:
            # Update each neuron
            for neuron_list in self._neurons.values():
                for neuron in neuron_list:
                    if neuron.active:
                        # Simple neuron dynamics (placeholder)
                        neuron.potential *= 0.99
                        neuron.firing_rate = max(0.0, neuron.firing_rate * 0.99)
            
            self._logger.logEntry(f"Updated region: id={self._region_id.value if self._region_id else 'None'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to update region: {e}")
    
    def getState(self) -> Dict[str, Any]:
        """
        Get region state as dictionary.
        
        Returns:
            Dictionary containing region state
        """
        return {
            "region_id": self._region_id.value if self._region_id else None,
            "name": self._name,
            "neuron_count": self.getTotalNeurons(),
            "active_neuron_count": self.getActiveNeuronCount(),
            "firing_neuron_count": self.getFiringNeuronCount(),
            "average_firing_rate": self.getAverageFiringRate(),
            "excitatory_inhibition_ratio": self.getExcitatoryInhibitionRatio(),
        }
    
    def __repr__(self) -> str:
        """String representation of neural region."""
        return f"<NeuralRegion: id={self._region_id.value if self._region_id else 'None'}, name='{self._name}'>"
    
    def __str__(self) -> str:
        """String representation of neural region."""
        return f"NLM Neural Region {self._region_id.value if self._region_id else 'None'} ('{self._name}')"

__all__ = ['NeuralRegion']
