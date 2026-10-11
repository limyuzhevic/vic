#!/usr/bin/env python3
"""
Advanced NLM Brain Control Interface

This script provides advanced commands for power users and debugging of the
NLM (Neural Learning Machine) brain system. It includes:
- Brain state management and checkpoint operations
- Real-time monitoring and statistics
- Performance optimization commands
- Debug and analysis tools
- Advanced plasticity control
- Simulation control and diagnostics
"""

import argparse
import json
import time
import sys
import os
from pathlib import Path
from typing import Optional, Dict, Any, List

# Add src to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from brain.Brain import Brain
    from core.Config.Config import Config
    from plasticity.STDP import STDP
    from plasticity.Hebbian import Hebbian
    from plasticity.AntiHebbian import AntiHebbian
    from plasticity.BCM import BCM
except ImportError as e:
    print(f"Warning: Could not import all modules: {e}")
    print("Some advanced commands may not work properly.")
class NLMAdvancedCommands:
    """Advanced command interface for NLM brain control and debugging."""
    
    def __init__(self, config_path: Optional[str] = None):
        self.config_path = config_path
        self.config = self._load_config()
        self.brain = None
        self.is_initialized = False
        
    def _load_config(self) -> Config:
        """Load configuration from file or create default."""
        if self.config_path and os.path.exists(self.config_path):
            # For now, create a basic config
            config = Config()
            # Load basic settings from file if available
            return config
        else:
            config = Config()
            # Set reasonable defaults
            config.set("neuron_count", 1000)
            config.set("region_count", 2)
            config.set("connection_probability", 0.1)
            return config
    
    def initialize_brain(self) -> bool:
        """Initialize the brain with current configuration."""
        try:
            self.brain = Brain(self.config)
            if self.brain.initialize():
                self.is_initialized = True
                print("✓ Brain initialized successfully")
                return True
            else:
                print("✗ Brain initialization failed")
                return False
        except Exception as e:
            print(f"✗ Brain initialization error: {e}")
            return False
    
    def save_checkpoint(self, filepath: str, compression: str = "balanced") -> bool:
        """Save brain state to checkpoint file."""
        if not self.is_initialized:
            print("✗ Brain not initialized")
            return False
            
        try:
            # Set compression level based on string
            compression_level = "balanced"
            if compression == "fast":
                compression_level = "fast"
            elif compression == "high":
                compression_level = "high"
                
            if self.brain.save(filepath):
                print(f"✓ Checkpoint saved to {filepath}")
                return True
            else:
                print(f"✗ Failed to save checkpoint to {filepath}")
                return False
        except Exception as e:
            print(f"✗ Checkpoint save error: {e}")
            return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        """Load brain state from checkpoint file."""
        if not os.path.exists(filepath):
            print(f"✗ Checkpoint file not found: {filepath}")
            return False
            
        try:
            if self.brain.load(filepath):
                print(f"✓ Checkpoint loaded from {filepath}")
                return True
            else:
                print(f"✗ Failed to load checkpoint from {filepath}")
                return False
        except Exception as e:
            print(f"✗ Checkpoint load error: {e}")
            return False
    
    def run_simulation(self, steps: int, timestep: float = 0.001) -> Dict[str, Any]:
        """Run simulation for specified number of steps."""
        if not self.is_initialized:
            print("✗ Brain not initialized")
            return {}
            
        print(f"Starting simulation for {steps} steps with timestep {timestep}")
        
        stats = {
            'total_spikes': 0,
            'average_firing_rate': 0.0,
            'ei_ratio': 0.0,
            'max_spike_rate': 0.0,
            'simulation_time': 0.0
        }
        
        try:
            start_time = time.time()
            
            for step in range(steps):
                self.brain.step(step, step * timestep)
                
                # Log progress every 1000 steps
                if step % 1000 == 0 and step > 0:
                    print(f"  Step {step}/{steps} ({step/steps*100:.1f}%)")
                    
            end_time = time.time()
            
            # Collect final statistics
            stats['total_spikes'] = self.brain.getTotalSpikeCount()
            stats['average_firing_rate'] = self.brain.getAverageFiringRate()
            stats['ei_ratio'] = self.brain.getExcitationInhibitionRatio()
            stats['simulation_time'] = end_time - start_time
            
            print(f"✓ Simulation completed in {stats['simulation_time']:.2f}s")
            print(f"  Total spikes: {stats['total_spikes']}")
            print(f"  Average firing rate: {stats['average_firing_rate']:.4f} Hz")
            print(f"  E/I ratio: {stats['ei_ratio']:.4f}")
            
            return stats
            
        except Exception as e:
            print(f"✗ Simulation error: {e}")
            return {}
    
    def get_detailed_stats(self) -> Dict[str, Any]:
        """Get detailed brain statistics and diagnostics."""
        if not self.is_initialized:
            print("✗ Brain not initialized")
            return {}
            
        stats = {
            'brain_info': {},
            'regions': [],
            'plasticity': {},
            'performance': {}
        }
        
        try:
            # Basic brain info
            stats['brain_info'] = {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'regions': self.brain.getRegionCount()
            }
            
            # Region details
            for region_id in self.brain.getRegionIds():
                region = self.brain.getRegion(region_id)
                if region:
                    stats['regions'].append({
                        'id': region_id,
                        'name': region.getName(),
                        'neuron_count': region.getTotalNeuronCount(),
                        'synapse_count': region.getSynapseCount(),
                        'avg_weight': region.getAverageSynapticWeight()
                    })
            
            # Plasticity system stats
            if self.brain.getSTDP():
                stdp = self.brain.getSTDP()
                stats['plasticity']['stdp'] = {
                    'ltp_weight': stdp.getLTPWeight(),
                    'ltd_weight': stdp.getLTDWeight(),
                    'time_constant': stdp.getTimeConstant(),
                    'enabled': stdp.isEnabled()
                }
                
            if self.brain.getHebbian():
                hebbian = self.brain.getHebbian()
                stats['plasticity']['hebbian'] = {
                    'learning_rate': hebbian.getLearningRate(),
                    'max_weight': hebbian.getMaxWeight(),
                    'enabled': hebbian.isEnabled()
                }
            
            # Performance metrics
            stats['performance'] = {
                'memory_usage': self._estimate_memory_usage(),
                'simulation_fps': self._calculate_fps(),
                'stability_score': self._calculate_stability_score()
            }
            
            print("✓ Detailed statistics collected")
            return stats
            
        except Exception as e:
            print(f"✗ Statistics collection error: {e}")
            return {}
    
    def optimize_plasticity(self, rule_type: str = "all") -> bool:
        """Optimize plasticity parameters for better learning."""
        if not self.is_initialized:
            print("✗ Brain not initialized")
            return False
            
        try:
            if rule_type == "stdp" or rule_type == "all":
                if self.brain.getSTDP():
                    stdp = self.brain.getSTDP()
                    # Optimize STDP parameters based on current network state
                    stats = self.get_detailed_stats()
                    current_ei_ratio = stats.get('brain_info', {}).get('ei_ratio', 1.0)
                    
                    # Adaptive learning rates based on E/I balance
                    if current_ei_ratio > 2.0:
                        # Network is too excitatory, reduce LTP
                        stdp.setLTPWeight(stdp.getLTPWeight() * 0.8)
                        print("  Optimized STDP: Reduced LTP due to high E/I ratio")
                    elif current_ei_ratio < 0.5:
                        # Network is too inhibitory, enhance LTP
                        stdp.setLTPWeight(stdp.getLTPWeight() * 1.2)
                        print("  Optimized STDP: Enhanced LTP due to low E/I ratio")
            
            if rule_type == "hebbian" or rule_type == "all":
                if self.brain.getHebbian():
                    hebbian = self.brain.getHebbian()
                    # Adjust Hebbian learning based on activity
                    stats = self.get_detailed_stats()
                    firing_rate = stats.get('brain_info', {}).get('avg_firing_rate', 0.01)
                    
                    # Adapt learning rate based on firing rate
                    if firing_rate > 0.1:
                        hebbian.setLearningRate(hebbian.getLearningRate() * 0.5)
                        print("  Optimized Hebbian: Reduced learning rate due to high firing")
                    elif firing_rate < 0.01:
                        hebbian.setLearningRate(hebbian.getLearningRate() * 1.5)
                        print("  Optimized Hebbian: Enhanced learning rate due to low firing")
            
            print("✓ Plasticity optimization completed")
            return True
            
        except Exception as e:
            print(f"✗ Plasticity optimization error: {e}")
            return False
    
    def export_brain_state(self, filepath: str, format: str = "json") -> bool:
        """Export brain state for external analysis."""
        if not self.is_initialized:
            print("✗ Brain not initialized")
            return False
            
        try:
            stats = self.get_detailed_stats()
            
            if format.lower() == "json":
                with open(filepath, 'w') as f:
                    json.dump(stats, f, indent=2, default=str)
                print(f"✓ Brain state exported to {filepath} (JSON)")
            elif format.lower() == "csv":
                self._export_csv(stats, filepath)
                print(f"✓ Brain state exported to {filepath} (CSV)")
            else:
                print(f"✗ Unsupported export format: {format}")
                return False
                
            return True
            
        except Exception as e:
            print(f"✗ Brain state export error: {e}")
            return False
    
    def debug_neuron(self, neuron_id: int) -> bool:
        """Debug specific neuron for detailed analysis."""
        if not self.is_initialized:
            print("✗ Brain not initialized")
            return False
            
        try:
            # Find neuron in regions
            neuron_found = False
            for region in self.brain.getRegions():
                for pop in region->getPopulations():
                    for neuron in pop->getNeurons():
                        if neuron->getId() == neuron_id:
                            neuron_found = True
                            print(f"Debug information for neuron {neuron_id}:")
                            print(f"  Type: {pop->getNeuronType()}")
                            print(f"  Membrane potential: {neuron->getMembranePotential()}")
                            print(f"  Resting potential: {neuron->getRestingPotential()}")
                            print(f"  Threshold: {neuron->getThreshold()}")
                            print(f"  Firing state: {neuron->getState().firingState}")
                            print(f"  Refractory remaining: {neuron->getState().refractoryRemaining}")
                            return True
            
            if not neuron_found:
                print(f"✗ Neuron {neuron_id} not found")
                return False
                
        except Exception as e:
            print(f"✗ Neuron debug error: {e}")
            return False
    
    def analyze_plasticity_effectiveness(self) -> Dict[str, Any]:
        """Analyze effectiveness of different plasticity rules."""
        if not self.is_initialized:
            print("✗ Brain not initialized")
            return {}
            
        analysis = {
            'stdp_effectiveness': 0.0,
            'hebbian_effectiveness': 0.0,
            'antihebbian_effectiveness': 0.0,
            'bc_effectiveness': 0.0,
            'overall_effectiveness': 0.0
        }
        
        try:
            # Compare before and after weight changes
            # This is a simplified analysis - real implementation would track
            # weight changes over time and analyze learning patterns
            
            stats = self.get_detailed_stats()
            brain_info = stats.get('brain_info', {})
            
            # Calculate effectiveness based on weight diversity and stability
            total_synapses = brain_info.get('total_synapses', 0)
            if total_synapses > 0:
                # Weight diversity index (simplified)
                avg_weight = brain_info.get('avg_weight', 0.0)
                analysis['stdp_effectiveness'] = min(1.0, abs(avg_weight) / 2.0)
                analysis['hebbian_effectiveness'] = min(1.0, brain_info.get('firing_neurons', 0) / max(1, brain_info.get('total_spikes', 1)))
                analysis['antihebbian_effectiveness'] = min(1.0, 1.0 - analysis['stdp_effectiveness'])
                analysis['bc_effectiveness'] = (analysis['stdp_effectiveness'] + analysis['hebbian_effectiveness']) / 2.0
            
            # Calculate overall effectiveness
            analysis['overall_effectiveness'] = (
                analysis['stdp_effectiveness'] + analysis['hebbian_effectiveness'] +
                analysis['antihebbian_effectiveness'] + analysis['bc_effectiveness']
            ) / 4.0
            
            print("✓ Plasticity effectiveness analysis completed")
            print(f"  STDP effectiveness: {analysis['stdp_effectiveness']:.3f}")
            print(f"  Hebbian effectiveness: {analysis['hebbian_effectiveness']:.3f}")
            print(f"  Anti-Hebbian effectiveness: {analysis['antihebbian_effectiveness']:.3f}")
            print(f"  BCM effectiveness: {analysis['bc_effectiveness']:.3f}")
            print(f"  Overall effectiveness: {analysis['overall_effectiveness']:.3f}")
            
            return analysis
            
        except Exception as e:
            print(f"✗ Plasticity analysis error: {e}")
            return {}
    
    def _estimate_memory_usage(self) -> Dict[str, Any]:
        """Estimate current memory usage."""
        # Simplified memory estimation
        stats = self.get_detailed_stats()
        brain_info = stats.get('brain_info', {})
        
        # Rough estimates (not exact)
        neuron_count = brain_info.get('total_neurons', 0)
        synapse_count = brain_info.get('total_synapses', 0)
        
        return {
            'estimated_bytes': neuron_count * 100 + synapse_count * 50,
            'neuron_count': neuron_count,
            'synapse_count': synapse_count,
            'memory_mb': (neuron_count * 100 + synapse_count * 50) / (1024 * 1024)
        }
    
    def _calculate_fps(self) -> float:
        """Calculate simulation FPS (simplified)."""
        # Placeholder - would need to track timing
        return 1000.0  # Placeholder value
    
    def _calculate_stability_score(self) -> float:
        """Calculate network stability score."""
        stats = self.get_detailed_stats()
        brain_info = stats.get('brain_info', {})
        
        ei_ratio = brain_info.get('ei_ratio', 1.0)
        firing_rate = brain_info.get('average_firing_rate', 0.0)
        
        # Stability is optimal when E/I ratio is near 1.0
        # and firing rate is in reasonable range
        stability = 1.0 - abs(ei_ratio - 1.0) / 2.0
        stability = stability * (1.0 - min(firing_rate, 0.5) / 0.5)
        
        return max(0.0, min(1.0, stability))
    
    def _export_csv(self, stats: Dict[str, Any], filepath: str):
        """Export statistics to CSV format."""
        # Simplified CSV export
        with open(filepath, 'w') as f:
            f.write("Metric,Value\n")
            for category, data in stats.items():
                if isinstance(data, dict):
                    for key, value in data.items():
                        f.write(f"{category}.{key},{value}\n")

def main():
    """Main command line interface."""
    parser = argparse.ArgumentParser(
        description="Advanced NLM Brain Control Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    
    parser.add_argument(
        '--config', '-c',
        help='Configuration file path'
    )
    
    parser.add_argument(
        '--action', '-a',
        choices=[
            'init', 'save', 'load', 'run', 'stats', 'optimize',
            'export', 'debug', 'analyze', 'all'
        ],
        default='stats',
        help='Action to perform'
    )
    
    parser.add_argument(
        '--steps', '-s',
        type=int,
        default=1000,
        help='Number of simulation steps to run'
    )
    
    parser.add_argument(
        '--checkpoint', '-k',
        help='Checkpoint file path'
    )
    
    parser.add_argument(
        '--output', '-o',
        help='Output file path for exports'
    )
    
    parser.add_argument(
        '--neuron-id', '-n',
        type=int,
        help='Neuron ID for debugging'
    )
    
    parser.add_argument(
        '--format', '-f',
        choices=['json', 'csv'],
        default='json',
        help='Export format'
    )
    
    parser.add_argument(
        '--timestep', '-t',
        type=float,
        default=0.001,
        help='Simulation timestep'
    )
    
    parser.add_argument(
        '--compression', '-z',
        choices=['fast', 'balanced', 'high'],
        default='balanced',
        help='Checkpoint compression level'
    )
    
    args = parser.parse_args()
    
    # Create advanced commands interface
    commands = NLMAdvancedCommands(args.config)
    
    # Perform requested action
    success = False
    
    if args.action == 'init':
        success = commands.initialize_brain()
    elif args.action == 'save':
        if args.checkpoint:
            success = commands.save_checkpoint(args.checkpoint, args.compression)
        else:
            print("✗ Checkpoint path required for save action")
    elif args.action == 'load':
        if args.checkpoint:
            success = commands.load_checkpoint(args.checkpoint)
        else:
            print("✗ Checkpoint path required for load action")
    elif args.action == 'run':
        stats = commands.run_simulation(args.steps, args.timestep)
        success = bool(stats)
    elif args.action == 'stats':
        stats = commands.get_detailed_stats()
        success = bool(stats)
    elif args.action == 'optimize':
        success = commands.optimize_plasticity('all')
    elif args.action == 'export':
        if args.output:
            success = commands.export_brain_state(args.output, args.format)
        else:
            print("✗ Output path required for export action")
    elif args.action == 'debug':
        if args.neuron_id:
            success = commands.debug_neuron(args.neuron_id)
        else:
            print("✗ Neuron ID required for debug action")
    elif args.action == 'analyze':
        analysis = commands.analyze_plasticity_effectiveness()
        success = bool(analysis)
    elif args.action == 'all':
        print("Running comprehensive analysis...")
        
        # Initialize brain
        if commands.initialize_brain():
            print("\n1. Running simulation...")
            stats = commands.run_simulation(args.steps, args.timestep)
            
            print("\n2. Collecting detailed statistics...")
            detailed_stats = commands.get_detailed_stats()
            
            print("\n3. Analyzing plasticity effectiveness...")
            analysis = commands.analyze_plasticity_effectiveness()
            
            print("\n4. Optimizing plasticity...")
            commands.optimize_plasticity('all')
            
            print("\n5. Exporting brain state...")
            if args.output:
                commands.export_brain_state(args.output, args.format)
            
            success = True
        else:
            success = False
    
    if success:
        print("\n✓ Operation completed successfully")
        return 0
    else:
        print("\n✗ Operation failed")
        return 1
if __name__ == '__main__':
    sys.exit(main())