"""
Export/import utilities for NLM neural simulations.

Provides neural network export for analysis, brain state export/import,
simulation recording and playback, and data analysis export formats.
"""

import json
import time
import numpy as np
import pandas as pd
from typing import Dict, List, Any, Optional, Tuple
from datetime import datetime
from pathlib import Path
import csv
import pickle
import h5py
import os
from dataclasses import dataclass, asdict
import copy

@dataclass
class BrainState:
    """Represents a complete brain state for export/import."""
    
    step: int
    simulation_time: float
    timestamp: str
    neuron_states: Dict[int, Dict[str, Any]]
    synapse_states: Dict[int, Dict[str, Any]]
    connectivity: Dict[int, List[int]]
    plasticity_state: Dict[str, Any]
    neuromodulation_state: Dict[str, Any]
    memory_states: Dict[str, Any]
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)
        
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'BrainState':
        return cls(**data)
class SimulationRecording:
    """Manages simulation recording and playback."""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.recordings: List[Dict[str, Any]] = []
        self.is_recording = False
        self.recording_interval = 0.1
        self.start_time = 0.0
        self.output_dir = Path("simulations")
        self.output_dir.mkdir(exist_ok=True)
        
    def start_recording(self, output_prefix: str = "simulation"):
        """
        Start recording simulation.
        
        Args:
            output_prefix: Prefix for output files
        """
        self.is_recording = True
        self.output_prefix = output_prefix
        self.start_time = time.time()
        
    def stop_recording(self):
        """Stop recording simulation."""
        self.is_recording = False
        
    def capture_frame(self, step: int, simulation_time: float):
        """
        Capture current simulation frame.
        
        Args:
            step: Current simulation step
            simulation_time: Current simulation time
        """
        if not self.is_recording:
            return
            
        frame = {
            'step': step,
            'simulation_time': simulation_time,
            'timestamp': datetime.now().isoformat(),
            'neuron_states': {},
            'synapse_states': {},
            'connectivity': {},
            'plasticity_state': {},
            'neuromodulation_state': {},
            'memory_states': {}
        }
        
        if self.brain:
            # Capture brain state
            for region in self.brain.getRegions():
                # Neuron states
                for neuron in region.getNeurons():
                    frame['neuron_states'][neuron.getId()] = {
                        'membrane_potential': neuron.getMembranePotential(),
                        'firing_state': neuron.getFiringState(),
                        'region_id': region.getId(),
                        'population_id': neuron.getPopulationId(),
                        'neuron_type': neuron.getType()
                    }
                    
                # Synapse states
                for synapse in region.getSynapses():
                    frame['synapse_states'][synapse.getId()] = {
                        'weight': synapse.getWeight(),
                        'delay': synapse.getDelay(),
                        'source_id': synapse.getSourceId(),
                        'target_id': synapse.getTargetId(),
                        'type': synapse.getType(),
                        'plasticity_flags': synapse.getPlasticityFlags()
                    }
                    
                # Build connectivity map
                for synapse in region.getSynapses():
                    if synapse.getSourceId() not in frame['connectivity']:
                        frame['connectivity'][synapse.getSourceId()] = []
                    frame['connectivity'][synapse.getSourceId()].append(synapse.getTargetId())
                    
        self.recordings.append(frame)
        
    def export_to_json(self, filepath: Optional[str] = None) -> str:
        """
        Export simulation to JSON format.
        
        Args:
            filepath: Output file path (optional)
            
        Returns:
            Path to output file
        """
        if not filepath:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filepath = self.output_dir / f"{self.output_prefix}_{timestamp}.json"
            
        data = {
            'metadata': {
                'format': 'NLM Simulation Recording',
                'version': '1.0',
                'start_time': self.start_time,
                'end_time': time.time(),
                'total_frames': len(self.recordings),
                'simulation_parameters': {}
            },
            'recordings': self.recordings
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
            
        return str(filepath)
        
    def export_to_hdf5(self, filepath: Optional[str] = None) -> str:
        """
        Export simulation to HDF5 format.
        
        Args:
            filepath: Output file path (optional)
            
        Returns:
            Path to output file
        """
        if not filepath:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filepath = self.output_dir / f"{self.output_prefix}_{timestamp}.h5"
            
        with h5py.File(filepath, 'w') as f:
            # Create datasets
            f.attrs['format'] = 'NLM Simulation Recording'
            f.attrs['version'] = '1.0'
            f.attrs['start_time'] = self.start_time
            f.attrs['end_time'] = time.time()
            f.attrs['total_frames'] = len(self.recordings)
            
            # Create groups for different data types
            grp_neuron = f.create_group('neurons')
            grp_synapse = f.create_group('synapses')
            grp_connectivity = f.create_group('connectivity')
            
            for i, frame in enumerate(self.recordings):
                frame_grp = f.create_group(f'frame_{i}')
                
                # Store neuron states
                if frame['neuron_states']:
                    neuron_ids = list(frame['neuron_states'].keys())
                    neuron_data = np.array([list(frame['neuron_states'][id].values()) 
                                          for id in neuron_ids])
                    grp_neuron.create_dataset(f'frame_{i}', data=neuron_data)
                    
                # Store synapse states
                if frame['synapse_states']:
                    synapse_ids = list(frame['synapse_states'].keys())
                    synapse_data = np.array([list(frame['synapse_states'][id].values()) 
                                           for id in synapse_ids])
                    grp_synapse.create_dataset(f'frame_{i}', data=synapse_data)
                    
                # Store connectivity
                if frame['connectivity']:
                    # Flatten connectivity for HDF5 storage
                    conn_data = []
                    for source, targets in frame['connectivity'].items():
                        conn_data.extend([(source, t) for t in targets])
                    conn_array = np.array(conn_data)
                    grp_connectivity.create_dataset(f'frame_{i}', data=conn_array)
                    
        return str(filepath)
        
    def export_to_csv(self, filepath: Optional[str] = None) -> str:
        """
        Export simulation to CSV format.
        
        Args:
            filepath: Output file path (optional)
            
        Returns:
            Path to output file
        """
        if not filepath:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filepath = self.output_dir / f"{self.output_prefix}_{timestamp}.csv"
            
        # Flatten recordings for CSV export
        csv_data = []
        for frame in self.recordings:
            for neuron_id, state in frame['neuron_states'].items():
                csv_row = {
                    'frame': frame['step'],
                    'simulation_time': frame['simulation_time'],
                    'neuron_id': neuron_id,
                    'region_id': state['region_id'],
                    'population_id': state['population_id'],
                    'neuron_type': state['neuron_type'],
                    'membrane_potential': state['membrane_potential'],
                    'firing_state': state['firing_state']
                }
                csv_data.append(csv_row)
                
        df = pd.DataFrame(csv_data)
        df.to_csv(filepath, index=False)
        
        return str(filepath)
        
    def import_from_json(self, filepath: str):
        """
        Import simulation from JSON format.
        
        Args:
            filepath: Input file path
        """
        with open(filepath, 'r') as f:
            data = json.load(f)
            
        self.recordings = data.get('recordings', [])
        self.start_time = data.get('metadata', {}).get('start_time', time.time())
        
    def import_from_hdf5(self, filepath: str):
        """
        Import simulation from HDF5 format.
        
        Args:
            filepath: Input file path
        """
        with h5py.File(filepath, 'r') as f:
            # Reconstruct recordings from HDF5
            self.recordings = []
            for key in sorted(f.keys()):
                if key.startswith('frame_'):
                    frame_num = int(key.split('_')[1])
                    frame_grp = f[key]
                    
                    frame = {
                        'step': frame_num,
                        'simulation_time': frame_num * 0.1,  # Approximate
                        'neuron_states': {},
                        'synapse_states': {},
                        'connectivity': {},
                        'plasticity_state': {},
                        'neuromodulation_state': {},
                        'memory_states': {}
                    }
                    
                    # Reconstruct from stored data
                    if 'neurons' in f:
                        neuron_data = f['neurons'][key][:]
                        # This would need to be expanded based on actual data format
                        
                    self.recordings.append(frame)
                    
        self.start_time = f.attrs.get('start_time', time.time())
        
    def playback(self, brain) -> List[int]:
        """
        Playback simulation to brain.
        
        Args:
            brain: Target brain object
            
        Returns:
            List of played steps
        """
        played_steps = []
        
        for frame in self.recordings:
            # Reset brain state
            brain.reset()
            
            # Restore neuron states
            for neuron_id, state in frame['neuron_states'].items():
                # This would need a method to restore neuron state
                pass
                
            played_steps.append(frame['step'])
            
        return played_steps
class ExportUtils:
    """
    Export/import utilities for NLM neural simulations.
    
    Features:
    - Neural network export for analysis
    - Brain state export/import
    - Simulation recording and playback
    - Data analysis export formats
    """
    
    def __init__(self, brain=None):
        """
        Initialize export utilities.
        
        Args:
            brain: Reference to the brain object (optional)
        """
        self.brain = brain
        self.recording = SimulationRecording(brain)
        
    def export_network(self, output_file: str, format: str = 'json',
                      include_synapses: bool = True,
                      include_connectivity: bool = True) -> str:
        """
        Export neural network structure for analysis.
        
        Args:
            output_file: Output file path
            format: Export format ('json', 'hdf5', 'csv')
            include_synapses: Whether to include synapse information
            include_connectivity: Whether to include connectivity matrix
            
        Returns:
            Path to output file
        """
        if not self.brain:
            raise ValueError("Brain object required for network export")
            
        network_data = {
            'metadata': {
                'format': 'NLM Neural Network Export',
                'version': '1.0',
                'exported_at': datetime.now().isoformat(),
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'region_count': self.brain.getRegionCount()
            },
            'neurons': [],
            'synapses': [],
            'regions': [],
            'connectivity': {}
        }
        
        # Export neuron data
        for region in self.brain.getRegions():
            region_data = {
                'id': region.getId(),
                'name': region.getName(),
                'neuron_count': region.getNeuronCount(),
                'type': region.getType() if hasattr(region, 'getType') else 'default'
            }
            network_data['regions'].append(region_data)
            
            for neuron in region.getNeurons():
                neuron_data = {
                    'id': neuron.getId(),
                    'type': neuron.getType(),
                    'region_id': region.getId(),
                    'population_id': neuron.getPopulationId(),
                    'membrane_potential': neuron.getMembranePotential(),
                    'firing_state': neuron.getFiringState(),
                    'threshold': neuron.getThreshold(),
                    'reset_potential': neuron.getResetPotential()
                }
                network_data['neurons'].append(neuron_data)
                
        # Export synapse data
        if include_synapses:
            for region in self.brain.getRegions():
                for synapse in region.getSynapses():
                    synapse_data = {
                        'id': synapse.getId(),
                        'source_id': synapse.getSourceId(),
                        'target_id': synapse.getTargetId(),
                        'weight': synapse.getWeight(),
                        'delay': synapse.getDelay(),
                        'type': synapse.getType(),
                        'region_id': region.getId()
                    }
                    network_data['synapses'].append(synapse_data)
                    
        # Build connectivity matrix
        if include_connectivity:
            neuron_ids = [neuron['id'] for neuron in network_data['neurons']]
            connectivity_matrix = np.zeros((len(neuron_ids), len(neuron_ids)))
            
            for synapse in network_data['synapses']:
                source_idx = neuron_ids.index(synapse['source_id']) if synapse['source_id'] in neuron_ids else -1
                target_idx = neuron_ids.index(synapse['target_id']) if synapse['target_id'] in neuron_ids else -1
                
                if source_idx >= 0 and target_idx >= 0:
                    connectivity_matrix[source_idx, target_idx] = synapse['weight']
                    
            network_data['connectivity_matrix'] = connectivity_matrix.tolist()
            
        # Export based on format
        if format == 'json':
            with open(output_file, 'w') as f:
                json.dump(network_data, f, indent=2)
        elif format == 'hdf5':
            with h5py.File(output_file, 'w') as f:
                # Store data in HDF5 groups
                f.create_dataset('neuron_count', data=network_data['metadata']['neuron_count'])
                f.create_dataset('synapse_count', data=network_data['metadata']['synapse_count'])
                
                # Store neurons
                neuron_ids = [n['id'] for n in network_data['neurons']]
                f.create_dataset('neuron_ids', data=neuron_ids)
                
                # Store region information
                region_ids = [r['id'] for r in network_data['regions']]
                f.create_dataset('region_ids', data=region_ids)
                
        elif format == 'csv':
            df = pd.DataFrame({
                'neuron_id': [n['id'] for n in network_data['neurons']],
                'neuron_type': [n['type'] for n in network_data['neurons']],
                'region_id': [n['region_id'] for n in network_data['neurons']],
                'membrane_potential': [n['membrane_potential'] for n in network_data['neurons']],
                'firing_state': [n['firing_state'] for n in network_data['neurons']]
            })
            df.to_csv(output_file, index=False)
            
        return output_file
        
    def export_brain_state(self, output_file: str, format: str = 'json',
                          include_plasticity: bool = True,
                          include_neuromodulation: bool = True) -> str:
        """
        Export current brain state.
        
        Args:
            output_file: Output file path
            format: Export format ('json', 'hdf5', 'csv')
            include_plasticity: Whether to include plasticity state
            include_neuromodulation: Whether to include neuromodulation state
            
        Returns:
            Path to output file
        """
        if not self.brain:
            raise ValueError("Brain object required for brain state export")
            
        # Create brain state
        brain_state = BrainState(
            step=0,  # Would need current step from brain
            simulation_time=0.0,  # Would need current simulation time
            timestamp=datetime.now().isoformat(),
            neuron_states={},
            synapse_states={},
            connectivity={},
            plasticity_state={},
            neuromodulation_state={},
            memory_states={}
        )
        
        # Capture current brain state
        for region in self.brain.getRegions():
            # Neuron states
            for neuron in region.getNeurons():
                brain_state.neuron_states[neuron.getId()] = {
                    'membrane_potential': neuron.getMembranePotential(),
                    'firing_state': neuron.getFiringState(),
                    'region_id': region.getId(),
                    'population_id': neuron.getPopulationId(),
                    'neuron_type': neuron.getType()
                }
                
            # Synapse states
            for synapse in region.getSynapses():
                brain_state.synapse_states[synapse.getId()] = {
                    'weight': synapse.getWeight(),
                    'delay': synapse.getDelay(),
                    'source_id': synapse.getSourceId(),
                    'target_id': synapse.getTargetId(),
                    'type': synapse.getType()
                }
                
            # Build connectivity
            for synapse in region.getSynapses():
                if synapse.getSourceId() not in brain_state.connectivity:
                    brain_state.connectivity[synapse.getSourceId()] = []
                brain_state.connectivity[synapse.getSourceId()].append(synapse.getTargetId())
                
        # Export based on format
        if format == 'json':
            with open(output_file, 'w') as f:
                json.dump(brain_state.to_dict(), f, indent=2)
        elif format == 'hdf5':
            with h5py.File(output_file, 'w') as f:
                # Store brain state in HDF5
                f.attrs['step'] = brain_state.step
                f.attrs['simulation_time'] = brain_state.simulation_time
                f.attrs['timestamp'] = brain_state.timestamp
                
                # Store neuron states
                if brain_state.neuron_states:
                    neuron_ids = list(brain_state.neuron_states.keys())
                    f.create_dataset('neuron_ids', data=neuron_ids)
                    
                    # Create group for neuron properties
                    grp_neuron = f.create_group('neuron_properties')
                    for neuron_id in neuron_ids:
                        state = brain_state.neuron_states[neuron_id]
                        neuron_grp = grp_neuron.create_group(f'neuron_{neuron_id}')
                        for key, value in state.items():
                            neuron_grp.attrs[key] = value
                            
        elif format == 'csv':
            # Flatten neuron states for CSV
            csv_data = []
            for neuron_id, state in brain_state.neuron_states.items():
                csv_row = {
                    'neuron_id': neuron_id,
                    'membrane_potential': state['membrane_potential'],
                    'firing_state': state['firing_state'],
                    'region_id': state['region_id'],
                    'population_id': state['population_id'],
                    'neuron_type': state['neuron_type']
                }
                csv_data.append(csv_row)
                
            df = pd.DataFrame(csv_data)
            df.to_csv(output_file, index=False)
            
        return output_file
        
    def import_brain_state(self, input_file: str) -> BrainState:
        """
        Import brain state from file.
        
        Args:
            input_file: Input file path
            
        Returns:
            Imported brain state
        """
        if input_file.endswith('.json'):
            with open(input_file, 'r') as f:
                data = json.load(f)
            return BrainState.from_dict(data)
        elif input_file.endswith('.h5') or input_file.endswith('.hdf5'):
            with h5py.File(input_file, 'r') as f:
                # Reconstruct brain state from HDF5
                brain_state = BrainState(
                    step=int(f.attrs['step']),
                    simulation_time=float(f.attrs['simulation_time']),
                    timestamp=f.attrs['timestamp'],
                    neuron_states={},
                    synapse_states={},
                    connectivity={},
                    plasticity_state={},
                    neuromodulation_state={},
                    memory_states={}
                )
                
                if 'neuron_ids' in f:
                    neuron_ids = f['neuron_ids'][:]
                    if 'neuron_properties' in f:
                        for neuron_id in neuron_ids:
                            neuron_grp = f[f'neuron_properties/neuron_{neuron_id}']
                            state = {key: neuron_grp.attrs[key] for key in neuron_grp.attrs}
                            brain_state.neuron_states[int(neuron_id)] = state
                            
                return brain_state
                
        raise ValueError(f"Unsupported file format: {input_file}")
        
    def start_recording(self, output_prefix: str = "simulation"):
        """
        Start simulation recording.
        
        Args:
            output_prefix: Prefix for output files
        """
        self.recording.start_recording(output_prefix)
        
    def stop_recording(self):
        """Stop simulation recording."""
        self.recording.stop_recording()
        
    def capture_frame(self, step: int, simulation_time: float):
        """
        Capture simulation frame.
        
        Args:
            step: Current simulation step
            simulation_time: Current simulation time
        """
        self.recording.capture_frame(step, simulation_time)
        
    def export_recording(self, filepath: Optional[str] = None, format: str = 'json') -> str:
        """
        Export simulation recording.
        
        Args:
            filepath: Output file path (optional)
            format: Export format ('json', 'hdf5', 'csv')
            
        Returns:
            Path to output file
        """
        if format == 'json':
            return self.recording.export_to_json(filepath)
        elif format == 'hdf5':
            return self.recording.export_to_hdf5(filepath)
        elif format == 'csv':
            return self.recording.export_to_csv(filepath)
        else:
            raise ValueError(f"Unsupported format: {format}")
            
    def import_recording(self, filepath: str):
        """
        Import simulation recording.
        
        Args:
            filepath: Input file path
        """
        if filepath.endswith('.json'):
            self.recording.import_from_json(filepath)
        elif filepath.endswith('.h5') or filepath.endswith('.hdf5'):
            self.recording.import_from_hdf5(filepath)
        else:
            raise ValueError(f"Unsupported file format: {filepath}")