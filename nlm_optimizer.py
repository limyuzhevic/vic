#!/usr/bin/env python3
"""
NLM - Performance Optimization for Better Learning

Performance optimization tools for NLM to improve learning efficiency:
- Activity regulation for better selectivity
- Learning rate optimization
- Memory efficiency improvements
- Parallel processing optimizations
- Dynamic plasticity adjustment
"""

import argparse
import sys
import os
from pathlib import Path
from typing import Optional, List, Dict, Any, Tuple
import time
import threading
import multiprocessing
import numpy as np

# Add the parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

class ActivityRegulator:
    """Regulator to control neural activity for better learning"""
    
    def __init__(self, target_firing_rate: float = 10.0, max_firing_rate: float = 20.0):
        self.target_firing_rate = target_firing_rate
        self.max_firing_rate = max_firing_rate
        self.activity_history: List[float] = []
        self.regulation_enabled = True
    
    def regulate_activity(self, brain, step: int) -> bool:
        """Regulate neural activity to prevent runaway excitation"""
        if not self.regulation_enabled:
            return False
        
        try:
            firing_rate = brain.getAverageFiringRate()
            self.activity_history.append(firing_rate)
            
            # Keep history manageable
            if len(self.activity_history) > 1000:
                self.activity_history.pop(0)
            
            # Check if activity is too high
            if firing_rate > self.max_firing_rate:
                # Apply inhibitory regulation
                self.apply_inhibitory_regulation(brain, firing_rate)
                return True
            
            # Check if activity is too low (but not completely silent)
            if len(self.activity_history) >= 10 and firing_rate < self.target_firing_rate * 0.5:
                # Apply excitatory regulation
                self.apply_excitatory_regulation(brain, firing_rate)
                return True
            
            return False
        except Exception:
            return False
    
    def apply_inhibitory_regulation(self, brain, current_rate: float) -> None:
        """Apply inhibitory signals to reduce activity"""
        try:
            # Inject inhibitory current to reduce firing rate
            inhibition_strength = min((current_rate - self.max_firing_rate) / 10.0, 1.0)
            
            # Get motor neurons for inhibitory control
            motor_neurons = self.get_motor_neurons(brain)
            if not motor_neurons:
                return
            
            # Apply inhibitory current
            for neuron in motor_neurons:
                if inhibition_strength > 0:
                    brain.injectCurrent(neuron, -inhibition_strength * 10.0)
                    
        except Exception:
            pass
    
    def apply_excitatory_regulation(self, brain, current_rate: float) -> None:
        """Apply excitatory signals to increase activity"""
        try:
            # Apply moderate excitation to boost activity
            excitation_strength = min((self.target_firing_rate * 0.5 - current_rate) / 10.0, 1.0)
            
            # Get sensory neurons for excitatory input
            sensory_neurons = self.get_sensory_neurons(brain)
            if not sensory_neurons:
                return
            
            # Apply excitatory current
            for neuron in sensory_neurons[:5]:  # Limit to avoid over-excitation
                if excitation_strength > 0:
                    brain.injectCurrent(neuron, excitation_strength * 15.0)
                    
        except Exception:
            pass
    
    def get_motor_neurons(self, brain) -> List:
        """Get motor neurons for regulation"""
        try:
            # This would typically access brain internals
            # For now, return a placeholder
            return []
        except Exception:
            return []
    
    def get_sensory_neurons(self, brain) -> List:
        """Get sensory neurons for regulation"""
        try:
            # This would typically access brain internals
            # For now, return a placeholder
            return []
        except Exception:
            return []

class LearningRateOptimizer:
    """Optimizer for learning rates and plasticity parameters"""
    
    def __init__(self):
        self.stdp_learning_rate = 0.01
        self.hebbian_learning_rate = 0.005
        self.stdp_tau = 20.0
        self.plasticity_active = True
    
    def optimize_learning_rates(self, brain, learning_history: List[Dict]) -> Dict:
        """Optimize learning rates based on learning progress"""
        if not learning_history:
            return self.get_current_learning_config()
        
        try:
            # Analyze learning progress
            recent_progress = learning_history[-10:] if len(learning_history) >= 10 else learning_history
            
            # Calculate average learning signal
            learning_signals = [h.get('reward', 0) for h in recent_progress]
            avg_learning_signal = sum(learning_signals) / len(learning_signals) if learning_signals else 0
            
            # Adjust learning rates based on progress
            new_config = self.get_current_learning_config()
            
            # If learning is too high, reduce rates to prevent overfitting
            if avg_learning_signal > 0.8:
                new_config['stdp_learning_rate'] *= 0.8
                new_config['hebbian_learning_rate'] *= 0.8
            
            # If learning is too low, increase rates to encourage exploration
            elif avg_learning_signal < 0.2:
                new_config['stdp_learning_rate'] *= 1.2
                new_config['hebbian_learning_rate'] *= 1.2
            
            # Adapt based on firing rate
            firing_rate = brain.getAverageFiringRate()
            if firing_rate > 50:  # Too high activity
                new_config['stdp_tau'] *= 0.8
            elif firing_rate < 5:   # Too low activity
                new_config['stdp_tau'] *= 1.2
            
            return new_config
            
        except Exception:
            return self.get_current_learning_config()
    
    def get_current_learning_config(self) -> Dict:
        """Get current learning configuration"""
        return {
            'stdp_learning_rate': self.stdp_learning_rate,
            'hebbian_learning_rate': self.hebbian_learning_rate,
            'stdp_tau': self.stdp_tau,
            'plasticity_active': self.plasticity_active
        }

class MemoryEfficiencyOptimizer:
    """Optimizer for memory usage and efficiency"""
    
    def __init__(self):
        self.compression_enabled = True
        self.sampling_rate = 1.0  # Fraction of events to store
        self.prioritization_enabled = True
    
    def optimize_memory_usage(self, brain, world=None) -> Dict:
        """Optimize memory usage for better performance"""
        config = {}
        
        try:
            # Analyze current memory usage
            neuron_count = brain.getTotalNeuronCount()
            synapse_count = brain.getTotalSynapseCount()
            
            # Estimate memory requirements
            estimated_neuron_memory = neuron_count * 64  # bytes per neuron
            estimated_synapse_memory = synapse_count * 32  # bytes per synapse
            
            total_estimated = estimated_neuron_memory + estimated_synapse_memory
            
            # Apply memory optimizations based on size
            if total_estimated > 100 * 1024 * 1024:  # > 100MB
                config['compression'] = True
                config['sampling_rate'] = 0.5
                config['prioritization'] = True
            elif total_estimated > 10 * 1024 * 1024:  # > 10MB
                config['compression'] = True
                config['sampling_rate'] = 0.8
                config['prioritization'] = False
            
            # Optimize for learning efficiency
            if brain.getTotalSpikeCount() < 1000:
                config['learning_efficiency'] = 'high'
                config['event_sampling'] = 'aggressive'
            elif brain.getTotalSpikeCount() < 10000:
                config['learning_efficiency'] = 'medium'
                config['event_sampling'] = 'moderate'
            else:
                config['learning_efficiency'] = 'conservative'
                config['event_sampling'] = 'minimal'
            
            # Memory monitoring
            if world:
                config['world_memory_efficiency'] = True
                config['state_compression'] = True
                config['temporal_sampling'] = True
            
        except Exception:
            config['optimization_failed'] = True
        
        return config

class ParallelProcessor:
    """Parallel processing optimizations for better performance"""
    
    def __init__(self):
        self.num_cores = max(1, multiprocessing.cpu_count() - 1)
        self.parallel_enabled = True
        self.thread_pool = None
    
    def process_in_parallel(self, task_func, data_list: List, max_workers: Optional[int] = None) -> List:
        """Process data in parallel for better performance"""
        if not self.parallel_enabled or len(data_list) < 4:
            # Sequential processing for small datasets
            return [task_func(item) for item in data_list]
        
        try:
            import concurrent.futures
            
            workers = max_workers or self.num_cores
            results = []
            
            with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as executor:
                # Submit all tasks
                future_to_item = {
                    executor.submit(task_func, item): item 
                    for item in data_list
                }
                
                # Collect results as they complete
                for future in concurrent.futures.as_completed(future_to_item):
                    try:
                        result = future.result()
                        results.append(result)
                    except Exception as e:
                        print(f"Parallel processing error: {str(e)}")
            
            return results
            
        except Exception:
            # Fallback to sequential processing
            return [task_func(item) for item in data_list]

class DynamicPlasticityController:
    """Dynamic control of plasticity rules for adaptive learning"""
    
    def __init__(self):
        self.plasticity_enabled = True
        self.adaptation_rate = 0.01
        self.critical_period_duration = 1000
        self.current_plasticity = 1.0
    
    def adapt_plasticity(self, brain, step: int) -> Dict:
        """Adapt plasticity rules based on learning progress"""
        config = {}
        
        try:
            firing_rate = brain.getAverageFiringRate()
            total_spikes = brain.getTotalSpikeCount()
            eir_ratio = brain.getExcitationInhibitionRatio()
            
            # Adaptive plasticity based on stability
            if abs(eir_ratio - 1.0) > 0.3:
                # Unstable network - reduce plasticity
                self.current_plasticity *= 0.95
                config['plasticity_reduction'] = True
            elif firing_rate > 30:
                # High activity - moderate plasticity
                self.current_plasticity *= 0.98
                config['activity_based_reduction'] = True
            else:
                # Stable, moderate activity - maintain plasticity
                self.current_plasticity *= 1.01
                config['plasticity_maintenance'] = True
            
            # Critical period handling
            if step < self.critical_period_duration:
                # Critical period - high plasticity
                self.current_plasticity = min(self.current_plasticity * 1.1, 1.5)
                config['critical_period'] = True
            
            # Prevent runaway plasticity
            self.current_plasticity = max(0.1, min(self.current_plasticity, 2.0))
            config['final_plasticity'] = self.current_plasticity
            
            # Store plasticity history for analysis
            if not hasattr(self, 'plasticity_history'):
                self.plasticity_history = []
            self.plasticity_history.append(self.current_plasticity)
            
        except Exception:
            config['adaptation_failed'] = True
        
        return config

class NLMPerformanceOptimizer:
    """Main performance optimizer for NLM"""
    
    def __init__(self):
        self.activity_regulator = ActivityRegulator()
        self.learning_rate_optimizer = LearningRateOptimizer()
        self.memory_efficiency_optimizer = MemoryEfficiencyOptimizer()
        self.parallel_processor = ParallelProcessor()
        self.plasticity_controller = DynamicPlasticityController()
        self.optimization_history: List[Dict] = []
    
    def optimize_learning(self, brain, world=None, steps: int = 100) -> Dict:
        """Run comprehensive performance optimization"""
        start_time = time.time()
        
        optimization_results = {
            'timestamp': time.time(),
            'brain_state_before': self.capture_brain_state(brain),
            'activity_regulation': {},
            'learning_optimization': {},
            'memory_optimization': {},
            'plasticity_control': {},
            'performance_metrics': {},
            'learning_history': []
        }
        
        try:
            # Run optimization steps
            learning_history = []
            
            for step in range(steps):
                # Regulate activity
                activity_regulated = self.activity_regulator.regulate_activity(brain, step)
                
                # Process step
                brain.step(step)
                
                # Collect learning metrics
                learning_metrics = {
                    'step': step,
                    'firing_rate': brain.getAverageFiringRate(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'eir_ratio': brain.getExcitationInhibitionRatio(),
                    'activity_regulated': activity_regulated
                }
                
                if world:
                    learning_metrics['world_time'] = world.getSimulationTime()
                
                learning_history.append(learning_metrics)
                optimization_results['learning_history'].append(learning_metrics)
            
            # Optimize learning rates based on history
            optimization_results['learning_optimization'] = (
                self.learning_rate_optimizer.optimize_learning_rates(brain, learning_history)
            )
            
            # Optimize memory usage
            optimization_results['memory_optimization'] = (
                self.memory_efficiency_optimizer.optimize_memory_usage(brain, world)
            )
            
            # Control plasticity dynamically
            optimization_results['plasticity_control'] = (
                self.plasticity_controller.adapt_plasticity(brain, steps)
            )
            
            # Calculate performance metrics
            end_time = time.time()
            total_time = end_time - start_time
            
            optimization_results['performance_metrics'] = {
                'total_steps': steps,
                'total_time': total_time,
                'steps_per_second': steps / total_time if total_time > 0 else 0,
                'avg_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'final_eir_ratio': brain.getExcitationInhibitionRatio(),
                'activity_regulation_events': len([h for h in learning_history if h['activity_regulated']])
            }
            
            optimization_results['brain_state_after'] = self.capture_brain_state(brain)
            
            # Store optimization results
            self.optimization_history.append(optimization_results)
            
            return optimization_results
            
        except Exception as e:
            optimization_results['optimization_error'] = str(e)
            optimization_results['success'] = False
            return optimization_results
    
    def capture_brain_state(self, brain) -> Dict:
        """Capture current brain state for comparison"""
        return {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'eir_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'region_count': brain.getRegionCount()
        }
    
    def analyze_performance_trends(self) -> Dict:
        """Analyze performance trends from optimization history"""
        if not self.optimization_history:
            return {}
        
        trends = {
            'total_optimizations': len(self.optimization_history),
            'avg_improvement': 0,
            'stability_score': 0,
            'recommendations': []
        }
        
        try:
            # Analyze firing rate trends
            firing_rates = []
            for opt in self.optimization_history:
                firing_rates.append(opt['performance_metrics']['avg_firing_rate'])
            
            if len(firing_rates) >= 2:
                # Calculate trend (simple linear regression)
                n = len(firing_rates)
                x = list(range(n))
                y = firing_rates
                
                x_mean = sum(x) / n
                y_mean = sum(y) / n
                
                numerator = sum((xi - x_mean) * (yi - y_mean) for xi, yi in zip(x, y))
                denominator = sum((xi - x_mean) ** 2 for xi in x)
                
                if denominator != 0:
                    trend_slope = numerator / denominator
                    trends['firing_rate_trend'] = trend_slope
                    
                    if abs(trend_slope) < 0.01:
                        trends['stability_score'] = 1.0
                    elif abs(trend_slope) < 0.05:
                        trends['stability_score'] = 0.7
                    else:
                        trends['stability_score'] = 0.3
                    
                    # Generate recommendations
                    if trend_slope > 0.01:
                        trends['recommendations'].append(
                            "Firing rate is increasing - consider activity regulation"
                        )
                    elif trend_slope < -0.01:
                        trends['recommendations'].append(
                            "Firing rate is decreasing - consider excitatory input"
                        )
            
        except Exception:
            trends['analysis_error'] = True
        
        return trends

def main():
    """Main CLI entry point for performance optimization"""
    parser = argparse.ArgumentParser(
        description="NLM Performance Optimization for Better Learning",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s optimize --steps 200
  %(prog)s analyze --type comprehensive
  %(prog)s setup --regulator-target 15.0

Optimization commands:
  optimize - Run comprehensive performance optimization
  analyze - Analyze optimization trends and performance
  setup - Configure optimization parameters
  monitor - Monitor real-time performance metrics

For more help on specific commands, use:
  %(prog)s <command> --help
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available optimization commands')
    
    # Optimize command
    optimize_parser = subparsers.add_parser('optimize', help='Run comprehensive performance optimization')
    optimize_parser.add_argument('--steps', '-s', type=int, default=200,
                                help='Number of optimization steps (default: 200)')
    optimize_parser.add_argument('--regulator-target', type=float, default=10.0,
                                help='Target firing rate for activity regulator (default: 10.0)')
    optimize_parser.add_argument('--verbose', '-v', action='store_true',
                                help='Enable verbose optimization output')
    optimize_parser.add_argument('--export', '-e', type=str,
                                help='Export optimization results to file')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze optimization trends and performance')
    analyze_parser.add_argument('--type', choices=['basic', 'comprehensive', 'stability'],
                               default='comprehensive', help='Analysis type (default: comprehensive)')
    analyze_parser.add_argument('--export', '-e', type=str,
                               help='Export analysis results to file')
    
    # Setup command
    setup_parser = subparsers.add_parser('setup', help='Configure optimization parameters')
    setup_parser.add_argument('--target-firing', type=float, default=15.0,
                             help='Target firing rate (default: 15.0)')
    setup_parser.add_argument('--max-firing', type=float, default=30.0,
                             help='Maximum firing rate (default: 30.0)')
    setup_parser.add_argument('--plasticity-factor', type=float, default=1.0,
                             help='Initial plasticity factor (default: 1.0)')
    
    # Monitor command
    monitor_parser = subparsers.add_parser('monitor', help='Monitor optimization performance')
    monitor_parser.add_argument('--history-file', type=str, default='optimization_history.json',
                               help='File to save optimization history (default: optimization_history.json)')
    
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(0)
    
    args = parser.parse_args()
    
    try:
        optimizer = NLMPerformanceOptimizer()
        
        if args.command == 'optimize':
            print("=== NLM PERFORMANCE OPTIMIZATION ===")
            print(f"Running optimization for {args.steps} steps...")
            print(f"Activity regulator target: {args.regulator_target} Hz")
            
            # Setup activity regulator
            optimizer.activity_regulator.target_firing_rate = args.regulator_target
            optimizer.activity_regulator.max_firing_rate = args.regulator_target * 2
            
            # Run optimization
            results = optimizer.optimize_learning(None, None, args.steps)
            
            # Display results
            print(f"\n=== OPTIMIZATION RESULTS ===")
            metrics = results['performance_metrics']
            print(f"Optimization completed in {metrics['total_time']:.2f} seconds")
            print(f"Steps per second: {metrics['steps_per_second']:.1f}")
            print(f"Final firing rate: {metrics['avg_firing_rate']:.2f} Hz")
            print(f"Total spikes: {metrics['total_spikes']}")
            print(f"E/I ratio: {metrics['final_eir_ratio']:.3f}")
            print(f"Activity regulation events: {metrics['activity_regulation_events']}")
            
            if args.verbose:
                print(f"\n=== DETAILED METRICS ===")
                print(f"Learning optimization: {results['learning_optimization']}")
                print(f"Memory optimization: {results['memory_optimization']}")
                print(f"Plasticity control: {results['plasticity_control']}")
            
            # Export results if requested
            if args.export:
                import json
                with open(args.export, 'w') as f:
                    json.dump(results, f, indent=2, default=str)
                print(f"✓ Optimization results exported to {args.export}")
        
        elif args.command == 'analyze':
            print("=== PERFORMANCE ANALYSIS ===")
            
            # Analyze trends
            trends = optimizer.analyze_performance_trends()
            
            print(f"\n=== ANALYSIS RESULTS ===")
            print(f"Total optimizations: {trends.get('total_optimizations', 0)}")
            print(f"Stability score: {trends.get('stability_score', 0):.2f}")
            
            if trends.get('firing_rate_trend'):
                trend = trends['firing_rate_trend']
                print(f"Firing rate trend: {trend:+.4f} Hz/step")
                if abs(trend) < 0.01:
                    print("  → Stable (good)")
                elif abs(trend) < 0.05:
                    print("  → Moderate change")
                else:
                    print("  → Significant change (may need adjustment)")
            
            # Show recommendations
            recommendations = trends.get('recommendations', [])
            if recommendations:
                print(f"\n=== RECOMMENDATIONS ===")
                for rec in recommendations:
                    print(f"  • {rec}")
            
            # Export analysis if requested
            if args.export:
                import json
                with open(args.export, 'w') as f:
                    json.dump(trends, f, indent=2, default=str)
                print(f"✓ Analysis exported to {args.export}")
        
        elif args.command == 'setup':
            print("=== OPTIMIZATION SETUP ===")
            print(f"Target firing rate: {args.target_firing} Hz")
            print(f"Maximum firing rate: {args.max_firing} Hz")
            print(f"Initial plasticity factor: {args.plasticity_factor}")
            
            # Configure optimizer
            optimizer.activity_regulator.target_firing_rate = args.target_firing
            optimizer.activity_regulator.max_firing_rate = args.max_firing
            optimizer.plasticity_controller.current_plasticity = args.plasticity_factor
            
            print("✓ Optimization parameters configured")
        
        elif args.command == 'monitor':
            print("=== PERFORMANCE MONITORING ===")
            print(f"Loading optimization history from {args.history_file}")
            
            if os.path.exists(args.history_file):
                import json
                with open(args.history_file, 'r') as f:
                    history = json.load(f)
                
                print(f"Loaded {len(history.get('optimizations', []))} optimization sessions")
                
                # Analyze history
                trends = optimizer.analyze_performance_trends()
                
                print(f"\n=== MONITORING SUMMARY ===")
                print(f"Total optimization sessions: {trends.get('total_optimizations', 0)}")
                print(f"Average stability: {trends.get('stability_score', 0):.2f}")
                
                if trends.get('recommendations'):
                    print(f"\n=== ACTIVE RECOMMENDATIONS ===")
                    for rec in trends['recommendations']:
                        print(f"  • {rec}")
            else:
                print(f"Warning: History file not found: {args.history_file}")
                print("Run 'optimize' command first to generate optimization data")
        
        else:
            print(f"Unknown command: {args.command}")
            sys.exit(1)
        
        print(f"\n✓ Command completed successfully")
        
    except Exception as e:
        print(f"✗ Command failed: {str(e)}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
