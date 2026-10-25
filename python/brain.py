"""Enhanced Brain interface with Pythonic features"""

import numpy as np
from typing import Optional, List, Dict, Any, Union
from contextlib import contextmanager

class Brain:
    """Enhanced Python interface for NLM Brain with Pythonic features."""
    
    def __init__(self, config=None):
        self._brain = None
        self._config = config
        self._step_callbacks = []
        self._statistics = {}
        
        if config is not None:
            self._create_brain(config)
    
    def _create_brain(self, config):
        """Create underlying C++ brain."""
        import pynlm
        if hasattr(config, '_config'):
            self._brain = pynlm.createBrain(config._config)
        else:
            self._brain = pynlm.createBrain(config)
        self._brain.initialize()
    
    def initialize(self):
        """Initialize the brain."""
        if self._brain:
            self._brain.initialize()
        return self
    
    def step(self, step=None, timestamp=None):
        """Execute a simulation step."""
        if self._brain:
            if timestamp is not None:
                self._brain.step(step, timestamp)
            else:
                self._brain.step(step)
            
            # Call callbacks
            for callback in self._step_callbacks:
                callback(step, self)
            
            # Update statistics
            self._update_statistics(step)
        
        return self
    
    def step_batch(self, steps, timestep=0.001):
        """Execute multiple simulation steps efficiently."""
        if self._brain:
            for i in range(steps):
                self._brain.step(i, timestep * i)
                for callback in self._step_callbacks:
                    callback(i, self)
                self._update_statistics(i)
        
        return self
    
    def reset(self):
        """Reset brain state."""
        if self._brain:
            self._brain.reset()
            self._statistics.clear()
        return self
    
    @property
    def is_initialized(self):
        """Check if brain is initialized."""
        return self._brain is not None
    
    @property
    def total_neurons(self):
        """Total number of neurons."""
        return self._brain.getTotalNeuronCount() if self._brain else 0
    
    @property
    def total_synapses(self):
        """Total number of synapses."""
        return self._brain.getTotalSynapseCount() if self._brain else 0
    
    @property
    def active_neurons(self):
        """Number of currently active neurons."""
        return self._brain.getActiveNeuronCount() if self._brain else 0
    
    @property
    def firing_neurons(self):
        """Number of currently firing neurons."""
        return self._brain.getFiringNeuronCount() if self._brain else 0
    
    @property
    def average_firing_rate(self):
        """Average firing rate."""
        return self._brain.getAverageFiringRate() if self._brain else 0.0
    
    @property
    def e_i_ratio(self):
        """Excitatory/inhibitory ratio."""
        return self._brain.getExcitationInhibitionRatio() if self._brain else 0.0
    
    @property
    def total_spikes(self):
        """Total spike count."""
        return self._brain.getTotalSpikeCount() if self._brain else 0
    
    def get_region(self, region_id):
        """Get brain region by ID."""
        if self._brain:
            return self._brain.getRegion(region_id)
        return None
    
    def get_statistics(self):
        """Get comprehensive brain statistics as dictionary."""
        stats = {
            'neurons': self.total_neurons,
            'synapses': self.total_synapses,
            'active_neurons': self.active_neurons,
            'firing_neurons': self.firing_neurons,
            'average_firing_rate': self.average_firing_rate,
            'e_i_ratio': self.e_i_ratio,
            'total_spikes': self.total_spikes,
        }
        
        # Add memory system stats
        if self._brain and self._brain.getWorkingMemory():
            stats['working_memory_traces'] = self._brain.getWorkingMemory().getActiveTraces()
        
        if self._brain and self._brain.getEpisodicMemory():
            stats['episodic_memory_episodes'] = self._brain.getEpisodicMemory().getEpisodeCount()
        
        # Add neuromodulation stats
        if self._brain and self._brain.getDopamine():
            stats['dopamine_level'] = self._brain.getDopamine().getLevel()
        
        if self._brain and self._brain.getCuriosity():
            stats['curiosity_level'] = self._brain.getCuriosity().getLevel()
        
        stats.update(self._statistics)
        return stats
    
    def add_step_callback(self, callback):
        """Add callback function to be called after each step."""
        self._step_callbacks.append(callback)
        return self
    
    def remove_step_callback(self, callback):
        """Remove callback function."""
        if callback in self._step_callbacks:
            self._step_callbacks.remove(callback)
        return self
    
    def wait_for_spikes(self, min_spikes, timeout_steps=1000):
        """Wait until minimum spike count is reached."""
        if not self._brain:
            return False
        
        for step in range(timeout_steps):
            if self._brain.getTotalSpikeCount() >= min_spikes:
                return True
            self.step(step)
        
        return False
    
    def inject_sensory(self, sensory_type, data):
        """Convenience method to inject sensory data by type."""
        if not self._brain:
            return
        
        # This would need to be implemented based on the actual API
        # For now, just a placeholder
        pass
    
    def monitor_neurons(self, type_filter=None):
        """Get monitored neurons of specific type."""
        # This would need to be implemented based on the actual API
        return []
    
    def _update_statistics(self, step):
        """Update internal statistics."""
        self._statistics['last_step'] = step
        self._statistics['timestamp'] = step * 0.001  # Assuming timestep of 0.001
    
    def __enter__(self):
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.reset()
    
    def __repr__(self):
        return f"<Brain(neurons={self.total_neurons}, spikes={self.total_spikes})>"
    
    def __str__(self):
        stats = self.get_statistics()
        return (f"Brain(n={stats['neurons']}, spikes={stats['total_spikes']}, "
                f"active={stats['active_neurons']}, rate={stats['average_firing_rate']:.3f})")


# Alias for backward compatibility
createBrain = Brain