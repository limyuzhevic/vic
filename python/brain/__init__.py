"""NLM Brain module - Core neural simulation functionality"""

import sys
import os
from typing import List, Optional, Dict, Any

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

import pynlm

__all__ = [
    'Brain',
    'RegionId',
    'NeuronId', 
    'SynapseId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'create_brain',
    'SimulationStep',
    'Timestamp',
    'MembranePotential',
    'FiringRate',
]

# Re-export types
RegionId = pynlm.RegionId
NeuronId = pynlm.NeuronId
SynapseId = pynlm.SynapseId
PopulationId = pynlm.PopulationId
NeuronType = pynlm.NeuronType
SynapseType = pynlm.SynapseType
DevelopmentalStage = pynlm.DevelopmentalStage
FiringState = pynlm.FiringState
ActionType = pynlm.ActionType

# Re-export type aliases
SimulationStep = pynlm.SimulationStep
Timestamp = pynlm.Timestamp
MembranePotential = pynlm.MembranePotential
FiringRate = pynlm.FiringRate

# Factory function
Brain = pynlm.Brain

def create_brain(config):
    """Create a brain with the given configuration"""
    return pynlm.createBrain(config)

# Brain convenience methods
class BrainUtils:
    """Utility methods for working with brains"""
    
    @staticmethod
    def get_region_info(brain, region_id):
        """Get information about a brain region"""
        region = brain.getRegion(region_id)
        if region:
            return {
                'id': region_id,
                'name': region.getName(),
                'neuron_count': region.getTotalNeuronCount(),
                'synapse_count': region.getSynapseCount(),
                'population_count': region.getPopulationCount(),
            }
        return None
    
    @staticmethod
    def get_brain_summary(brain):
        """Get a summary of brain state"""
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'regions': len(brain.getRegionIds()),
            'development_stage': str(brain.getDevelopmentalStage()),
        }
    
    @staticmethod
    def reset_and_reinitialize(brain, config):
        """Reset brain and reinitialize with new config"""
        brain.reset()
        # Note: Reinitialization would need to be supported
        # For now, this is a placeholder
        return brain

# Brain statistics collection
class BrainStatsCollector:
    """Collect statistics from brain simulations"""
    
    def __init__(self):
        self.stats = []
    
    def collect_stats(self, brain, step):
        """Collect statistics at current simulation step"""
        stats = {
            'step': step,
            'timestamp': step * 0.001,  # Assuming 1ms timestep
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': str(brain.getDevelopmentalStage()),
        }
        self.stats.append(stats)
        return stats
    
    def get_summary(self):
        """Get summary of collected statistics"""
        if not self.stats:
            return {}
        
        steps = [s['step'] for s in self.stats]
        return {
            'total_steps': len(self.stats),
            'final_step': steps[-1],
            'average_firing_rate': sum(s['average_firing_rate'] for s in self.stats) / len(self.stats),
            'peak_firing_rate': max(s['average_firing_rate'] for s in self.stats),
            'total_spikes_collected': sum(s['total_spikes'] for s in self.stats),
            'final_e_i_ratio': self.stats[-1]['e_i_ratio'],
        }
    
    def reset(self):
        """Reset the statistics collector"""
        self.stats = []
