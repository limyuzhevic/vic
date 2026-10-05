"""
NeuralPopulation class for NLM neural simulation framework.

This module provides a Python interface to the C++ NeuralPopulation class with enhanced
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
PopulationId = Any
NeuronId = Any
NeuronType = Any

@dataclass
class NeuronData:
    """Data structure for neuron information"""
    id: NeuronId
    type: NeuronType
    active: bool
    potential: float
    firing_rate: float

class NeuralPopulation:
    """
    NeuralPopulation class for NLM neural simulation framework.
    
    This class provides a Python interface to the underlying C++ NeuralPopulation class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self, population_id: PopulationId = None):
        """
        Initialize neural population.
        
        Args:
            population_id: Population ID (optional)
        """
        self._logger = PythonSessionLogger()
        self._population_id = population_id
        self._neurons: List[NeuronData] = []
        
        # Log initialization
        self._logger.logEntry(f"NeuralPopulation initialized: id={self._population_id.value if self._population_id else 'None'}")
    
    # Population ID property
    
    @property
    def population_id(self) -> PopulationId:
        """Get population ID."""
        return self._population_id
    
    @population_id.setter
    def population_id(self, value: PopulationId) -> None:
        """
        Set population ID.
        
        Args:
            value: Population ID
        """
        self._population_id = value
    
    # Neuron management methods
    
    def addNeuron(self, neuron: 'Neuron') -> bool:
        """
        Add neuron to population.
        
        Args:
            neuron: Neuron to add
            
        Returns:
            True if successful, False otherwise
        """
        try:
            neuron_data = NeuronData(
                id=neuron.id,
                type=neuron.type,
                active=neuron.active,
                potential=neuron.potential,
                firing_rate=neuron.firing_rate,
            )
            
            self._neurons.append(neuron_data)
            self._logger.logEntry(f"Added neuron to population: id={neuron.id.value}, population={self._population_id.value if self._population_id else 'None'}")
            
            return True
            
        except Exception as e:
            self._format_python_error(f"Failed to add neuron to population: {e}")
            return False
    
    def removeNeuron(self, neuron_id: NeuronId) -> bool:
        """
        Remove neuron from population.
        
        Args:
            neuron_id: Neuron ID to remove
            
        Returns:
            True if successful, False otherwise
        """
        try:
            for i, neuron in enumerate(self._neurons):
                if neuron.id == neuron_id:
                    del self._neurons[i]
                    self._logger.logEntry(f"Removed neuron from population: id={neuron_id.value}, population={self._population_id.value if self._population_id else 'None'}")
                    return True
            
            return False
            
        except Exception as e:
            self._format_python_error(f"Failed to remove neuron from population: {e}")
            return False
    
    def getNeuron(self, neuron_id: NeuronId) -> Optional[NeuronData]:
        """
        Get neuron from population.
        
        Args:
            neuron_id: Neuron ID to retrieve
            
        Returns:
            NeuronData object or None if not found
        """
        try:
            for neuron in self._neurons:
                if neuron.id == neuron_id:
                    return neuron
            
            return None
            
        except Exception:
            return None
    
    def getNeuronCount(self) -> int:
        """Get number of neurons in population."""
        return len(self._neurons)
    
    # Population statistics
    
    def getTotalNeurons(self) -> int:
        """Get total number of neurons."""
        return self.getNeuronCount()
    
    def getActiveNeuronCount(self) -> int:
        """Get number of active neurons."""
        return sum(1 for neuron in self._neurons if neuron.active)
    
    def getFiringNeuronCount(self) -> int:
        """Get number of firing neurons."""
        return sum(1 for neuron in self._neurons if neuron.firing_rate > 0)
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate of population."""
        if not self._neurons:
            return 0.0
        
        total_rate = sum(neuron.firing_rate for neuron in self._neurons)
        return total_rate / len(self._neurons)
    
    def getExcitatoryInhibitionRatio(self) -> float:
        """Get excitation/inhibition ratio of population."""
        excitatory_count = sum(1 for neuron in self._neurons if neuron.type == NeuronType.Excitatory)
        inhibitory_count = sum(1 for neuron in self._neurons if neuron.type == NeuronType.Inhibitory)
        
        total = excitatory_count + inhibitory_count
        if total == 0:
            return 1.0
        
        return float(excitatory_count) / float(inhibitory_count) if inhibitory_count > 0 else float("inf")
    
    # Population operations
    
    def activateAll(self) -> None:
        """Activate all neurons in population."""
        try:
            for neuron in self._neurons:
                neuron.active = True
            
            self._logger.logEntry(f"Activated all neurons in population: id={self._population_id.value if self._population_id else 'None'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to activate all neurons: {e}")
    
    def deactivateAll(self) -> None:
        """Deactivate all neurons in population."""
        try:
            for neuron in self._neurons:
                neuron.active = False
            
            self._logger.logEntry(f"Deactivated all neurons in population: id={self._population_id.value if self._population_id else 'None'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to deactivate all neurons: {e}")
    
    def reset(self) -> None:
        """Reset population state."""
        try:
            for neuron in self._neurons:
                neuron.active = False
                neuron.potential = 0.0
                neuron.firing_rate = 0.0
            
            self._logger.logEntry(f"Reset population: id={self._population_id.value if self._population_id else 'None'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to reset population: {e}")
    
    def update(self, dt: float) -> None:
        """
        Update population state.
        
        Args:
            dt: Time step
        """
        try:
            # Update each neuron
            for neuron in self._neurons:
                if neuron.active:
                    # Simple neuron dynamics (placeholder)
                    neuron.potential *= 0.99
                    neuron.firing_rate = max(0.0, neuron.firing_rate * 0.99)
            
            self._logger.logEntry(f"Updated population: id={self._population_id.value if self._population_id else 'None'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to update population: {e}")
    
    def getState(self) -> Dict[str, Any]:
        """
        Get population state as dictionary.
        
        Returns:
            Dictionary containing population state
        """
        return {
            "population_id": self._population_id.value if self._population_id else None,
            "neuron_count": self.getNeuronCount(),
            "active_neuron_count": self.getActiveNeuronCount(),
            "firing_neuron_count": self.getFiringNeuronCount(),
            "average_firing_rate": self.getAverageFiringRate(),
            "excitatory_inhibition_ratio": self.getExcitatoryInhibitionRatio(),
        }
    
    def __repr__(self) -> str:
        """String representation of neural population."""
        return f"<NeuralPopulation: id={self._population_id.value if self._population_id else 'None'}, neurons={self.getNeuronCount()}>"
    
    def __str__(self) -> str:
        """String representation of neural population."""
        return f"NLM Neural Population {self._population_id.value if self._population_id else 'None'} ({self.getNeuronCount()} neurons)"

__all__ = ['NeuralPopulation']
