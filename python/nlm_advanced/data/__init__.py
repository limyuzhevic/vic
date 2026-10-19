"""
Data export/import tools for NLM simulations.

Provides multiple export formats, compressed checkpoint management,
simulation recording and playback, result analysis scripts, and automated report generation.
"""

import numpy as np
import json
import h5py
import csv
import pickle
import gzip
import shutil
from typing import Dict, List, Optional, Any, Union
from dataclasses import dataclass, field
import time
import os
from pathlib import Path
import pandas as pd
from datetime import datetime

@dataclass
class ExportConfig:
    """Configuration for data export."""
    export_format: str = "hdf5"
    compression: str = "gzip"
    compression_level: int = 6
    include_metadata: bool = True
    include_visualizations: bool = False
    chunk_size: int = 1000

@dataclass
class CheckpointInfo:
    """Information about a simulation checkpoint."""
    timestamp: float
    step: int
    checkpoint_id: str
    file_size: int
    version: str
    metadata: Dict[str, Any] = field(default_factory=dict)

class DataExporter:
    """Export simulation results in multiple formats."""
    
    def __init__(self, output_dir: str = "./exports"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.export_configs = {}
    
    def add_export_config(self, name: str, config: ExportConfig):
        """Add an export configuration."""
        self.export_configs[name] = config
    
    def export_results(self, results: Dict, config_name: str = "default") -> str:
        """Export results using specified configuration."""
        if config_name not in self.export_configs:
            raise ValueError(f"Unknown export config: {config_name}")
        
        config = self.export_configs[config_name]
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"results_{timestamp}"
        
        if config.export_format == "hdf5":
            return self._export_to_hdf5(results, f"{filename}.h5", config)
        elif config.export_format == "json":
            return self._export_to_json(results, f"{filename}.json", config)
        elif config.export_format == "csv":
            return self._export_to_csv(results, f"{filename}.csv", config)
        elif config.export_format == "pickle":
            return self._export_to_pickle(results, f"{filename}.pkl", config)
        else:
            raise ValueError(f"Unsupported export format: {config.export_format}")
    
    def _export_to_hdf5(self, results: Dict, filename: str, config: ExportConfig) -> str:
        """Export results to HDF5 file."""
        filepath = self.output_dir / filename
        
        with h5py.File(filepath, 'w') as f:
            # Create groups for different data types
            self._create_hdf5_groups(f, results, config)
        
        return str(filepath)
    
    def _create_hdf5_groups(self, h5_file, results: Dict, config: ExportConfig):
        """Create HDF5 groups for simulation data."""
        # Main results group
        if 'final_metrics' in results:
            metrics_group = h5_file.create_group('final_metrics')
            for key, value in results['final_metrics'].items():
                if isinstance(value, (int, float)):
                    metrics_group.attrs[key] = value
                elif isinstance(value, (list, np.ndarray)):
                    metrics_group.create_dataset(key, data=np.array(value))
        
        # Step results group
        if 'step_results' in results:
            steps_group = h5_file.create_group('step_results')
            for i, step_result in enumerate(results['step_results'][:config.chunk_size]):
                step_group = steps_group.create_group(f'step_{i}')
                for key, value in step_result.items():
                    if isinstance(value, (int, float)):
                        step_group.attrs[key] = value
                    elif isinstance(value, (list, np.ndarray)):
                        step_group.create_dataset(key, data=np.array(value))
        
        # Configuration group
        if 'config' in results:
            config_group = h5_file.create_group('config')
            for key, value in results['config'].items():
                config_group.attrs[key] = str(value)
        
        # Metadata group
        if config.include_metadata and 'metadata' in results:
            metadata_group = h5_file.create_group('metadata')
            for key, value in results['metadata'].items():
                metadata_group.attrs[key] = str(value)
        
        # Add attributes to root
        h5_file.attrs['export_timestamp'] = time.time()
        h5_file.attrs['export_format'] = config.export_format
        h5_file.attrs['compression'] = config.compression
    
    def _export_to_json(self, results: Dict, filename: str, config: ExportConfig) -> str:
        """Export results to JSON file."""
        filepath = self.output_dir / filename
        
        # Convert non-serializable objects
        json_results = self._prepare_json_results(results)
        
        with open(filepath, 'w') as f:
            json.dump(json_results, f, indent=2, default=str)
        
        return str(filepath)
    
    def _prepare_json_results(self, results: Dict) -> Dict:
        """Prepare results for JSON serialization."""
        json_results = {}
        
        for key, value in results.items():
            if key == 'step_results':
                # Limit step results to avoid huge files
                json_results[key] = value[:1000]  # First 1000 steps
            elif isinstance(value, (dict, list)):
                json_results[key] = value
            elif isinstance(value, (int, float, str, bool, type(None))):
                json_results[key] = value
            else:
                # Try to convert to string representation
                json_results[key] = str(value)
        
        return json_results
    
    def _export_to_csv(self, results: Dict, filename: str, config: ExportConfig) -> str:
        """Export results to CSV file."""
        filepath = self.output_dir / filename
        
        # Convert to pandas DataFrame
        df_data = []
        
        if 'step_results' in results:
            for step_result in results['step_results']:
                df_data.append(step_result)
        
        if df_data:
            df = pd.DataFrame(df_data)
            df.to_csv(filepath, index=False)
        else:
            # Create empty CSV with expected columns
            df = pd.DataFrame(columns=['step', 'time', 'active_neurons', 'firing_rate', 'e_i_ratio'])
            df.to_csv(filepath, index=False)
        
        return str(filepath)
    
    def _export_to_pickle(self, results: Dict, filename: str, config: ExportConfig) -> str:
        """Export results to pickle file."""
        filepath = self.output_dir / filename
        
        with open(filepath, 'wb') as f:
            pickle.dump(results, f, protocol=pickle.HIGHEST_PROTOCOL)
        
        return str(filepath)
    
    def export_simulation_state(self, brain, step: int, filepath: str):
        """Export current brain state to file."""
        state_data = {
            'step': step,
            'timestamp': time.time(),
            'brain_state': brain.save() if hasattr(brain, 'save') else None,
            'metrics': {
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount()
            }
        }
        
        # Choose export format based on file extension
        if filepath.endswith('.h5'):
            return self._export_to_hdf5(state_data, filepath, ExportConfig())
        elif filepath.endswith('.json'):
            return self._export_to_json(state_data, filepath, ExportConfig())
        else:
            return self._export_to_pickle(state_data, filepath, ExportConfig())

class DataImporter:
    """Import simulation results from various formats."""
    
    def __init__(self, input_dir: str = "./imports"):
        self.input_dir = Path(input_dir)
        self.input_dir.mkdir(parents=True, exist_ok=True)
    
    def import_results(self, filepath: str) -> Dict:
        """Import results from file."""
        path = Path(filepath)
        
        if path.suffix == '.h5':
            return self._import_from_hdf5(filepath)
        elif path.suffix == '.json':
            return self._import_from_json(filepath)
        elif path.suffix == '.csv':
            return self._import_from_csv(filepath)
        elif path.suffix in ['.pkl', '.pickle']:
            return self._import_from_pickle(filepath)
        else:
            raise ValueError(f"Unsupported file format: {path.suffix}")
    
    def _import_from_hdf5(self, filepath: str) -> Dict:
        """Import results from HDF5 file."""
        results = {}
        
        with h5py.File(filepath, 'r') as f:
            # Import final metrics
            if 'final_metrics' in f:
                results['final_metrics'] = dict(f['final_metrics'].attrs)
            
            # Import step results
            if 'step_results' in f:
                results['step_results'] = []
                for step_name in f['step_results']:
                    step_group = f[f'step_results/{step_name}']
                    step_data = {}
                    for key in step_group.attrs:
                        step_data[key] = step_group.attrs[key]
                    results['step_results'].append(step_data)
            
            # Import config
            if 'config' in f:
                results['config'] = dict(f['config'].attrs)
            
            # Import metadata
            if 'metadata' in f:
                results['metadata'] = dict(f['metadata'].attrs)
        
        return results
    
    def _import_from_json(self, filepath: str) -> Dict:
        """Import results from JSON file."""
        with open(filepath, 'r') as f:
            results = json.load(f)
        
        return results
    
    def _import_from_csv(self, filepath: str) -> Dict:
        """Import results from CSV file."""
        df = pd.read_csv(filepath)
        
        results = {}
        
        # Convert to step results format
        if not df.empty:
            results['step_results'] = df.to_dict('records')
            
            # Extract final metrics if available
            final_metrics = {}
            for col in df.columns:
                if col not in ['step', 'time']:
                    final_metrics[col] = df[col].iloc[-1] if not df[col].isna().all() else None
            
            results['final_metrics'] = final_metrics
        
        return results
    
    def _import_from_pickle(self, filepath: str) -> Dict:
        """Import results from pickle file."""
        with open(filepath, 'rb') as f:
            results = pickle.load(f)
        
        return results

class CheckpointManager:
    """Compressed checkpoint management for simulations."""
    
    def __init__(self, checkpoint_dir: str = "./checkpoints"):
        self.checkpoint_dir = Path(checkpoint_dir)
        self.checkpoint_dir.mkdir(parents=True, exist_ok=True)
        self.checkpoint_info = []
    
    def create_checkpoint(self, brain, step: int, metadata: Dict = None) -> str:
        """Create a compressed checkpoint."""
        timestamp = time.time()
        checkpoint_id = f"checkpoint_{step}_{int(timestamp)}"
        
        # Create checkpoint data
        checkpoint_data = {
            'step': step,
            'timestamp': timestamp,
            'checkpoint_id': checkpoint_id,
            'brain_data': brain.save() if hasattr(brain, 'save') else None,
            'metadata': metadata or {}
        }
        
        # Save as compressed file
        filepath = self.checkpoint_dir / f"{checkpoint_id}.gz"
        
        with gzip.open(filepath, 'wb') as f:
            pickle.dump(checkpoint_data, f, protocol=pickle.HIGHEST_PROTOCOL)
        
        # Record checkpoint info
        info = CheckpointInfo(
            timestamp=timestamp,
            step=step,
            checkpoint_id=checkpoint_id,
            file_size=filepath.stat().st_size,
            version="1.0",
            metadata=metadata or {}
        )
        
        self.checkpoint_info.append(info)
        
        # Cleanup old checkpoints if needed
        self._cleanup_old_checkpoints()
        
        return str(filepath)
    
    def load_checkpoint(self, checkpoint_id: str) -> Dict:
        """Load a checkpoint by ID."""
        filepath = self.checkpoint_dir / f"{checkpoint_id}.gz"
        
        if not filepath.exists():
            raise FileNotFoundError(f"Checkpoint not found: {checkpoint_id}")
        
        with gzip.open(filepath, 'rb') as f:
            checkpoint_data = pickle.load(f)
        
        return checkpoint_data
    
    def list_checkpoints(self, limit: int = 10) -> List[CheckpointInfo]:
        """List available checkpoints."""
        # Sort by timestamp (newest first)
        sorted_info = sorted(self.checkpoint_info, key=lambda x: x.timestamp, reverse=True)
        return sorted_info[:limit]
    
    def delete_checkpoint(self, checkpoint_id: str):
        """Delete a checkpoint."""
        filepath = self.checkpoint_dir / f"{checkpoint_id}.gz"
        
        if filepath.exists():
            filepath.unlink()
        
        # Remove from info list
        self.checkpoint_info = [info for info in self.checkpoint_info 
                               if info.checkpoint_id != checkpoint_id]
    
    def _cleanup_old_checkpoints(self, max_checkpoints: int = 100):
        """Clean up old checkpoints to save space."""
        if len(self.checkpoint_info) <= max_checkpoints:
            return
        
        # Sort by timestamp (oldest first)
        sorted_info = sorted(self.checkpoint_info, key=lambda x: x.timestamp)
        
        # Delete oldest checkpoints
        to_delete = sorted_info[:len(self.checkpoint_info) - max_checkpoints]
        
        for info in to_delete:
            filepath = self.checkpoint_dir / f"{info.checkpoint_id}.gz"
            if filepath.exists():
                filepath.unlink()
        
        # Update checkpoint info
        self.checkpoint_info = sorted_info[len(to_delete):]
    
    def restore_latest(self, brain):
        """Restore simulation from latest checkpoint."""
        if not self.checkpoint_info:
            raise ValueError("No checkpoints available")
        
        latest_checkpoint = max(self.checkpoint_info, key=lambda x: x.timestamp)
        
        checkpoint_data = self.load_checkpoint(latest_checkpoint.checkpoint_id)
        
        if checkpoint_data['brain_data'] and hasattr(brain, 'load'):
            brain.load(checkpoint_data['brain_data'])
        
        return checkpoint_data

class RecordingPlayer:
    """Simulation recording and playback."""
    
    def __init__(self, recording_dir: str = "./recordings"):
        self.recording_dir = Path(recording_dir)
        self.recording_dir.mkdir(parents=True, exist_ok=True)
        self.recordings = []
    
    def start_recording(self, brain, filename: str, recording_rate: float = 0.1):
        """Start recording simulation state."""
        recording_id = f"recording_{int(time.time())}"
        recording_path = self.recording_dir / f"{recording_id}.h5"
        
        # Create recording file
        recording_data = {
            'metadata': {
                'filename': filename,
                'start_time': time.time(),
                'recording_rate': recording_rate,
                'brain_type': type(brain).__name__
            },
            'steps': [],
            'metrics': []
        }
        
        with h5py.File(recording_path, 'w') as f:
            # Store metadata
            for key, value in recording_data['metadata'].items():
                f.attrs[key] = str(value)
        
        recording_info = {
            'recording_id': recording_id,
            'filename': filename,
            'filepath': recording_path,
            'start_time': time.time(),
            'recording_rate': recording_rate,
            'brain': brain,
            'active': True
        }
        
        self.recordings.append(recording_info)
        
        # Start recording thread
        recording_info['thread'] = threading.Thread(
            target=self._recording_loop, args=(recording_info,),
            daemon=True
        )
        recording_info['thread'].start()
        
        return recording_id
    
    def stop_recording(self, recording_id: str):
        """Stop recording for a specific recording."""
        for recording in self.recordings:
            if recording['recording_id'] == recording_id:
                recording['active'] = False
                if 'thread' in recording:
                    recording['thread'].join(timeout=5.0)
                
                # Save final data
                self._save_recording_data(recording)
                
                return True
        
        return False
    
    def _recording_loop(self, recording_info: Dict):
        """Main recording loop."""
        brain = recording_info['brain']
        recording_rate = recording_info['recording_rate']
        recording_id = recording_info['recording_id']
        
        step_counter = 0
        next_record_step = 0
        
        while recording_info['active']:
            current_step = step_counter  # This should be from actual simulation
            current_time = time.time()
            
            # Record at specified rate
            if current_step >= next_record_step:
                self._record_step(recording_info, current_step, current_time)
                next_record_step = int(current_step / recording_rate)
            
            step_counter += 1
            time.sleep(0.01)  # Small delay
    
    def _record_step(self, recording_info: Dict, step: int, timestamp: float):
        """Record a single simulation step."""
        brain = recording_info['brain']
        
        step_data = {
            'step': step,
            'timestamp': timestamp,
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        recording_info['steps'].append(step_data)
        
        # Save to file periodically
        if len(recording_info['steps']) % 100 == 0:
            self._save_recording_data(recording_info)
    
    def _save_recording_data(self, recording_info: Dict):
        """Save recording data to file."""
        filepath = recording_info['filepath']
        
        with h5py.File(filepath, 'a') as f:
            # Save steps
            steps_dataset = f.create_dataset(f'steps_{len(recording_info["steps"])}', 
                                           data=np.array(recording_info['steps'], dtype=object))
            
            # Save metrics
            if recording_info['steps']:
                metrics = {
                    'steps': [s['step'] for s in recording_info['steps']],
                    'active_neurons': [s['active_neurons'] for s in recording_info['steps']],
                    'firing_rates': [s['firing_rate'] for s in recording_info['steps']],
                    'e_i_ratios': [s['e_i_ratio'] for s in recording_info['steps']]
                }
                
                metrics_group = f.create_group(f'metrics_{len(recording_info["steps"])}')
                for key, data in metrics.items():
                    metrics_group.create_dataset(key, data=np.array(data))
    
    def playback_recording(self, recording_id: str, brain, speed_factor: float = 1.0):
        """Playback a recording."""
        for recording in self.recordings:
            if recording['recording_id'] == recording_id:
                filepath = recording['filepath']
                
                with h5py.File(filepath, 'r') as f:
                    steps = f['steps'][:]  # This would need proper handling
                
                # Replay steps
                for step_data in steps:
                    # Load brain state
                    # Apply metrics
                    # Wait based on speed factor
                    time.sleep(0.1 / speed_factor)
                
                return True
        
        return False

class ResultAnalyzer:
    """Result analysis scripts and utilities."""
    
    def __init__(self):
        self.analysis_history = []
    
    def analyze_simulation_results(self, results: Dict) -> Dict:
        """Analyze simulation results and provide insights."""
        analysis = {
            'summary': self._generate_summary(results),
            'performance': self._analyze_performance(results),
            'patterns': self._detect_patterns(results),
            'recommendations': self._generate_recommendations(results)
        }
        
        self.analysis_history.append({
            'timestamp': time.time(),
            'results_id': id(results),
            'analysis': analysis
        })
        
        return analysis
    
    def _generate_summary(self, results: Dict) -> Dict:
        """Generate summary statistics."""
        summary = {
            'total_steps': len(results.get('step_results', [])),
            'final_metrics': results.get('final_metrics', {}),
            'config': results.get('config', {})
        }
        
        if 'final_metrics' in results:
            final = results['final_metrics']
            summary['final_state'] = {
                'active_neurons': final.get('final_active_neurons', 0),
                'firing_rate': final.get('final_firing_rate', 0),
                'e_i_ratio': final.get('final_e_i_ratio', 0),
                'total_spikes': final.get('total_spikes', 0)
            }
        
        return summary
    
    def _analyze_performance(self, results: Dict) -> Dict:
        """Analyze simulation performance."""
        performance = {
            'stability': self._calculate_stability(results),
            'efficiency': self._calculate_efficiency(results),
            'quality': self._calculate_quality(results)
        }
        
        return performance
    
    def _calculate_stability(self, results: Dict) -> Dict:
        """Calculate simulation stability metrics."""
        step_results = results.get('step_results', [])
        
        if not step_results:
            return {'stable': False, 'reason': 'No step results available'}
        
        # Calculate coefficient of variation for key metrics
        active_neurons = [step.get('active_neurons', 0) for step in step_results]
        firing_rates = [step.get('firing_rate', 0) for step in step_results]
        
        # Stability score (lower is more stable)
        stability_score = (
            np.std(active_neurons) / (np.mean(active_neurons) + 1e-6) +
            np.std(firing_rates) / (np.mean(firing_rates) + 1e-6)
        )
        
        stable = stability_score < 0.5  # Threshold
        
        return {
            'stable': stable,
            'stability_score': stability_score,
            'cv_active_neurons': np.std(active_neurons) / (np.mean(active_neurons) + 1e-6),
            'cv_firing_rates': np.std(firing_rates) / (np.mean(firing_rates) + 1e-6)
        }
    
    def _calculate_efficiency(self, results: Dict) -> Dict:
        """Calculate simulation efficiency."""
        step_results = results.get('step_results', [])
        
        if not step_results:
            return {'efficient': False, 'reason': 'No step results available'}
        
        # Efficiency based on activity patterns
        active_neurons = [step.get('active_neurons', 0) for step in step_results]
        
        # Ideal efficiency is moderate activity (not too low, not too high)
        mean_activity = np.mean(active_neurons)
        std_activity = np.std(active_neurons)
        
        efficiency = 1.0 / (1.0 + abs(mean_activity - 1000) / 1000.0)
        efficiency = efficiency * (1.0 - min(std_activity / 500.0, 1.0))
        
        return {
            'efficient': efficiency > 0.3,
            'efficiency_score': efficiency,
            'mean_activity': mean_activity,
            'activity_variance': std_activity
        }
    
    def _calculate_quality(self, results: Dict) -> Dict:
        """Calculate simulation quality metrics."""
        final_metrics = results.get('final_metrics', {})
        
        quality_metrics = {
            'firing_rate_quality': self._assess_firing_rate_quality(final_metrics),
            'balance_quality': self._assess_balance_quality(final_metrics),
            'dynamics_quality': self._assess_dynamics_quality(results)
        }
        
        overall_quality = np.mean(list(quality_metrics.values()))
        
        return {
            'overall_quality': overall_quality,
            'details': quality_metrics,
            'assessment': self._assess_overall_quality(quality_metrics)
        }
    
    def _assess_firing_rate_quality(self, metrics: Dict) -> float:
        """Assess firing rate quality."""
        firing_rate = metrics.get('final_firing_rate', 0)
        
        # Ideal firing rate is moderate (not too high, not too low)
        if 0.1 <= firing_rate <= 10:
            return 1.0
        elif 0.05 <= firing_rate < 0.1 or 10 < firing_rate <= 20:
            return 0.7
        elif 0.01 <= firing_rate < 0.05 or 20 < firing_rate <= 50:
            return 0.4
        else:
            return 0.1
    
    def _assess_balance_quality(self, metrics: Dict) -> float:
        """Assess excitation/inhibition balance quality."""
        e_i_ratio = metrics.get('final_e_i_ratio', 0.5)
        
        # Ideal E/I ratio is around 0.5-1.0 for balanced network
        if 0.4 <= e_i_ratio <= 1.0:
            return 1.0
        elif 0.2 <= e_i_ratio < 0.4 or 1.0 < e_i_ratio <= 2.0:
            return 0.7
        elif 0.1 <= e_i_ratio < 0.2 or 2.0 < e_i_ratio <= 5.0:
            return 0.4
        else:
            return 0.1
    
    def _assess_dynamics_quality(self, results: Dict) -> float:
        """Assess dynamics quality from step results."""
        step_results = results.get('step_results', [])
        
        if not step_results:
            return 0.0
        
        # Check for dynamic behavior (changes over time)
        active_neurons = [step.get('active_neurons', 0) for step in step_results]
        
        # Calculate rate of change
        if len(active_neurons) >= 3:
            changes = np.diff(active_neurons)
            change_variance = np.var(changes)
            
            # Moderate variance is good for dynamics
            if change_variance > 0 and change_variance < 10000:
                return min(change_variance / 1000.0, 1.0)
        
        return 0.0
    
    def _assess_overall_quality(self, quality_metrics: Dict) -> str:
        """Assess overall quality."""
        overall = quality_metrics['overall_quality']
        
        if overall >= 0.8:
            return "Excellent"
        elif overall >= 0.6:
            return "Good"
        elif overall >= 0.4:
            return "Fair"
        elif overall >= 0.2:
            return "Poor"
        else:
            return "Very Poor"
    
    def _detect_patterns(self, results: Dict) -> Dict:
        """Detect patterns in simulation results."""
        patterns = {
            'oscillations': self._detect_oscillations(results),
            'bursts': self._detect_bursts(results),
            'cycles': self._detect_cycles(results)
        }
        
        return patterns
    
    def _detect_oscillations(self, results: Dict) -> Dict:
        """Detect oscillatory patterns."""
        step_results = results.get('step_results', [])
        
        if len(step_results) < 20:
            return {'detected': False, 'reason': 'Insufficient data'}
        
        active_neurons = [step.get('active_neurons', 0) for step in step_results]
        
        # Simple oscillation detection using autocorrelation
        try:
            autocorr = np.correlate(active_neurons, active_neurons, mode='full')
            autocorr = autocorr[len(autocorr)//2:]
            
            # Find peaks in autocorrelation
            peaks = np.where((autocorr[1:-1] > autocorr[:-2]) & 
                           (autocorr[1:-1] > autocorr[2:]))[0] + 1
            
            if len(peaks) > 2:
                period = np.mean(np.diff(peaks))
                return {
                    'detected': True,
                    'period': period,
                    'peaks_found': len(peaks)
                }
        except:
            pass
        
        return {'detected': False, 'reason': 'No oscillations detected'}
    
    def _detect_bursts(self, results: Dict) -> Dict:
        """Detect burst patterns."""
        step_results = results.get('step_results', [])
        
        if not step_results:
            return {'detected': False, 'reason': 'No data available'}
        
        active_neurons = [step.get('active_neurons', 0) for step in step_results]
        mean_activity = np.mean(active_neurons)
        threshold = mean_activity + 2 * np.std(active_neurons)
        
        bursts = sum(1 for activity in active_neurons if activity > threshold)
        
        if bursts > 0:
            return {
                'detected': True,
                'burst_count': bursts,
                'burst_percentage': (bursts / len(active_neurons)) * 100,
                'threshold': threshold
            }
        
        return {'detected': False, 'reason': 'No bursts detected'}
    
    def _detect_cycles(self, results: Dict) -> Dict:
        """Detect developmental cycles."""
        step_results = results.get('step_results', [])
        
        if len(step_results) < 10:
            return {'detected': False, 'reason': 'Insufficient data'}
        
        active_neurons = [step.get('active_neurons', 0) for step in step_results]
        
        # Simple cycle detection using peak finding
        peaks = []
        for i in range(1, len(active_neurons) - 1):
            if active_neurons[i] > active_neurons[i-1] and active_neurons[i] > active_neurons[i+1]:
                peaks.append(i)
        
        if len(peaks) >= 2:
            cycles = len(peaks)
            avg_period = np.mean(np.diff(peaks))
            
            return {
                'detected': True,
                'cycle_count': cycles,
                'average_period': avg_period,
                'peak_indices': peaks
            }
        
        return {'detected': False, 'reason': 'No cycles detected'}
    
    def _generate_recommendations(self, results: Dict) -> List[str]:
        """Generate recommendations based on analysis."""
        recommendations = []
        
        analysis = self.analyze_simulation_results(results)
        
        if not analysis['summary']['final_state']['active_neurons']:
            recommendations.append("Simulation produced no active neurons. Check initial conditions.")
        
        if analysis['performance']['stability']['stability_score'] > 0.5:
            recommendations.append("Simulation appears unstable. Consider reducing time step size or increasing synaptic decay.")
        
        if analysis['performance']['efficiency']['efficiency_score'] < 0.3:
            recommendations.append("Simulation efficiency is low. Consider adjusting target firing rates or connectivity parameters.")
        
        if analysis['quality']['overall_quality'] < 0.5:
            recommendations.append("Simulation quality is suboptimal. Review network parameters and initial conditions.")
        
        return recommendations
    
    def export_analysis_report(self, filepath: str):
        """Export analysis report to file."""
        report = {
            'analysis_history': self.analysis_history,
            'summary_statistics': self._calculate_aggregate_statistics()
        }
        
        with open(filepath, 'w') as f:
            json.dump(report, f, indent=2, default=str)

class ReportGenerator:
    """Automated report generation."""
    
    def __init__(self, template_dir: str = "./templates"):
        self.template_dir = Path(template_dir)
        self.template_dir.mkdir(parents=True, exist_ok=True)
        self.templates = {}
    
    def add_template(self, name: str, template: Dict):
        """Add a report template."""
        self.templates[name] = template
    
    def generate_report(self, results: Dict, template_name: str = "default") -> str:
        """Generate a report from results."""
        if template_name not in self.templates:
            raise ValueError(f"Unknown template: {template_name}")
        
        template = self.templates[template_name]
        
        # Generate report content
        report_content = self._generate_content(results, template)
        
        # Save report
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"report_{timestamp}.md"
        filepath = self.template_dir / filename
        
        with open(filepath, 'w') as f:
            f.write(report_content)
        
        return str(filepath)
    
    def _generate_content(self, results: Dict, template: Dict) -> str:
        """Generate report content from template and results."""
        content = "# NLM Simulation Report\n\n"
        
        # Add title and metadata
        content += f"**Generated:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n"
        
        # Add summary section
        content += "## Summary\n\n"
        content += f"- Total Steps: {len(results.get('step_results', []))}\n"
        
        if 'final_metrics' in results:
            final = results['final_metrics']
            content += f"- Final Active Neurons: {final.get('final_active_neurons', 'N/A')}\n"
            content += f"- Final Firing Rate: {final.get('final_firing_rate', 'N/A'):.3f}\n"
            content += f"- Final E/I Ratio: {final.get('final_e_i_ratio', 'N/A'):.3f}\n"
            content += f"- Total Spikes: {final.get('total_spikes', 'N/A')}\n\n"
        
        # Add performance analysis
        content += "## Performance Analysis\n\n"
        # Would include analysis results here
        
        # Add visualizations section
        if template.get('include_visualizations', False):
            content += "## Visualizations\n\n"
            content += "*(Visualizations would be included here if available)*\n\n"
        
        # Add recommendations
        if template.get('include_recommendations', True):
            content += "## Recommendations\n\n"
            content += "*(Recommendations would be included here based on analysis)*\n\n"
        
        return content

# Import required modules
from .. import pynlm