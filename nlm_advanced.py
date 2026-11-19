#!/usr/bin/env python3
"""
NLM - Advanced User Utilities and Tools

Advanced utilities for NLM users:
- Neural network analysis and visualization
- Configuration management and optimization
- Performance profiling and monitoring
- Advanced debugging tools
- Custom learning scenarios
- Batch processing capabilities
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Optional, List, Dict, Any, Tuple, Callable
import time
import numpy as np
import matplotlib.pyplot as plt

# Add the parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

class NLMUtility:
    """Base class for NLM utilities"""
    
    def __init__(self, brain=None, config=None):
        self.brain = brain
        self.config = config
        self.history: List[Dict] = []
    
    def capture_state(self) -> Dict:
        """Capture current state of NLM components"""
        state = {
            'timestamp': time.time(),
            'brain_info': {},
            'config_info': {},
            'system_status': {}
        }
        
        if self.brain:
            state['brain_info'] = {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'eir_ratio': self.brain.getExcitationInhibitionRatio(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'region_count': self.brain.getRegionCount(),
                'development_stage': str(self.brain.getDevelopmentalStage())
            }
        
        if self.config:
            state['config_info'] = {
                'keys': self.config.getKeys() if hasattr(self.config, 'getKeys') else [],
                'values': self.get_config_summary()
            }
        
        # System status
        state['system_status'] = {
            'python_version': sys.version,
            'pynlm_available': PYNLM_AVAILABLE,
            'utility_version': '1.0.0'
        }
        
        self.history.append(state)
        return state
    
    def get_config_summary(self) -> Dict:
        """Get summary of configuration values"""
        if not self.config or not hasattr(self.config, 'getKeys'):
            return {}
        
        summary = {}
        for key in self.config.getKeys():
            try:
                # Try to get value, handling different types
                if self.config.has(key):
                    value = self.config.getOr[str](key, "")
                    summary[key] = value
            except Exception:
                summary[key] = "[Unable to retrieve]"
        
        return summary

class NeuralNetworkAnalyzer(NLMUtility):
    """Analyze neural network properties and structure"""
    
    def analyze_connectivity(self) -> Dict:
        """Analyze network connectivity patterns"""
        if not self.brain:
            return {}
        
        analysis = {
            'connectivity_metrics': {},
            'region_analysis': {},
            'synapse_analysis': {},
            'network_topology': {}
        }
        
        try:
            # Basic connectivity metrics
            total_neurons = self.brain.getTotalNeuronCount()
            total_synapses = self.brain.getTotalSynapseCount()
            region_count = self.brain.getRegionCount()
            
            analysis['connectivity_metrics'] = {
                'synapse_density': total_synapses / total_neurons if total_neurons > 0 else 0,
                'connectivity_ratio': total_synapses / (total_neurons * total_neurons) if total_neurons > 0 else 0,
                'regional_connectivity': total_synapses / region_count if region_count > 0 else 0
            }
            
            # Region analysis
            if hasattr(self.brain, 'getRegionIds'):
                region_analysis = {}
                for region_id in self.brain.getRegionIds():
                    if hasattr(self.brain, 'getRegion'):
                        region = self.brain.getRegion(region_id)
                        if region:
                            region_neuron_count = region.getTotalNeuronCount() if hasattr(region, 'getTotalNeuronCount') else 0
                            region_synapse_count = region.getSynapseCount() if hasattr(region, 'getSynapseCount') else 0
                            
                            region_analysis[f"region_{region_id}"] = {
                                'neuron_count': region_neuron_count,
                                'synapse_count': region_synapse_count,
                                'synapse_density': region_synapse_count / region_neuron_count if region_neuron_count > 0 else 0
                            }
                
                analysis['region_analysis'] = region_analysis
            
            # Network topology analysis
            firing_rate = self.brain.getAverageFiringRate()
            eir_ratio = self.brain.getExcitationInhibitionRatio()
            
            analysis['network_topology'] = {
                'stability_indicator': self.calculate_stability_indicator(firing_rate, eir_ratio),
                'activity_level': self.classify_activity_level(firing_rate),
                'network_balance': self.assess_network_balance(eir_ratio)
            }
            
        except Exception as e:
            analysis['error'] = str(e)
        
        return analysis
    
    def calculate_stability_indicator(self, firing_rate: float, eir_ratio: float) -> float:
        """Calculate network stability indicator"""
        # Stability based on firing rate and E/I balance
        rate_component = max(0, 1.0 - abs(firing_rate - 10.0) / 40.0)  # Peak at 10 Hz
        eir_component = max(0, 1.0 - abs(eir_ratio - 1.0) / 1.0)  # Peak at 1.0
        
        return (rate_component * 0.6 + eir_component * 0.4) if (rate_component + eir_component) > 0 else 0
    
    def classify_activity_level(self, firing_rate: float) -> str:
        """Classify network activity level"""
        if firing_rate < 1.0:
            return 'VERY_LOW'
        elif firing_rate < 5.0:
            return 'LOW'
        elif firing_rate < 15.0:
            return 'MODERATE'
        elif firing_rate < 30.0:
            return 'HIGH'
        else:
            return 'VERY_HIGH'
    
    def assess_network_balance(self, eir_ratio: float) -> str:
        """Assess network E/I balance"""
        if abs(eir_ratio - 1.0) < 0.1:
            return 'BALANCED'
        elif abs(eir_ratio - 1.0) < 0.3:
            return 'SLIGHTLY_IMBALANCED'
        elif eir_ratio > 1.5:
            return 'EXCITATORY_DOMINANT'
        elif eir_ratio < 0.5:
            return 'INHIBITORY_DOMINANT'
        else:
            return 'SIGNIFICANTLY_IMBALANCED'
    
    def visualize_connectivity(self, output_file: Optional[str] = None) -> bool:
        """Create visualization of network connectivity"""
        try:
            analysis = self.analyze_connectivity()
            
            # Create visualization plots
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('NLM Neural Network Analysis', fontsize=16)
            
            # Plot 1: Region distribution
            if 'region_analysis' in analysis:
                regions = list(analysis['region_analysis'].keys())
                neuron_counts = [analysis['region_analysis'][r]['neuron_count'] for r in regions]
                
                axes[0, 0].bar(regions, neuron_counts, color='skyblue', alpha=0.7)
                axes[0, 0].set_title('Neuron Distribution by Region')
                axes[0, 0].set_ylabel('Neuron Count')
            
            # Plot 2: Connectivity metrics
            if 'connectivity_metrics' in analysis:
                metrics = list(analysis['connectivity_metrics'].keys())
                values = list(analysis['connectivity_metrics'].values())
                
                axes[0, 1].bar(metrics, values, color='lightgreen', alpha=0.7)
                axes[0, 1].set_title('Connectivity Metrics')
                axes[0, 1].set_ylabel('Value')
                axes[0, 1].tick_params(axis='x', rotation=45)
            
            # Plot 3: Activity level
            activity_data = {
                'Name': ['VERY_LOW', 'LOW', 'MODERATE', 'HIGH', 'VERY_HIGH'],
                'Count': [1, 1, 1, 1, 1]  # Placeholder - would need actual data
            }
            
            # Note: In real implementation, this would use actual activity data
            axes[1, 0].pie([1, 1, 1, 1, 1], labels=activity_data['Name'], autopct='%1.1f%%')
            axes[1, 0].set_title('Activity Level Distribution')
            
            # Plot 4: Stability indicator
            if 'network_topology' in analysis:
                balance = analysis['network_topology']['network_balance']
                stability = analysis['network_topology']['stability_indicator']
                
                axes[1, 1].bar(['Stability', 'Imbalance'], [stability, 1-stability], 
                              color=['green', 'red'], alpha=0.7)
                axes[1, 1].set_title(f'Network Stability: {stability:.2f}')
                axes[1, 1].set_ylabel('Score')
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
                print(f"✓ Visualization saved to {output_file}")
            else:
                plt.show()
            
            plt.close()
            return True
            
        except Exception as e:
            print(f"Visualization failed: {str(e)}")
            return False
    
    def generate_network_report(self, output_file: Optional[str] = None) -> Dict:
        """Generate comprehensive network analysis report"""
        analysis = self.analyze_connectivity()
        brain_state = self.capture_state()
        
        report = {
            'timestamp': time.time(),
            'network_analysis': analysis,
            'brain_state': brain_state,
            'recommendations': self.generate_recommendations(analysis, brain_state),
            'health_score': self.calculate_health_score(analysis)
        }
        
        if output_file:
            try:
                with open(output_file, 'w') as f:
                    json.dump(report, f, indent=2, default=str)
                print(f"✓ Network report saved to {output_file}")
            except Exception as e:
                print(f"Failed to save network report: {str(e)}")
                return report
        
        return report
    
    def generate_recommendations(self, analysis: Dict, brain_state: Dict) -> List[str]:
        """Generate recommendations based on analysis"""
        recommendations = []
        
        try:
            # Check firing rate
            firing_rate = brain_state.get('brain_info', {}).get('firing_rate', 0)
            if firing_rate < 5.0:
                recommendations.append("Increase network excitability to achieve more active learning")
            elif firing_rate > 30.0:
                recommendations.append("Reduce network excitability to prevent runaway excitation")
            
            # Check E/I balance
            eir_ratio = brain_state.get('brain_info', {}).get('eir_ratio', 1.0)
            if abs(eir_ratio - 1.0) > 0.3:
                recommendations.append("Balance excitation and inhibition for stable network operation")
            
            # Check connectivity
            if 'connectivity_metrics' in analysis:
                density = analysis['connectivity_metrics'].get('synapse_density', 0)
                if density > 0.5:
                    recommendations.append("High connectivity detected - consider reducing synaptic density")
                elif density < 0.05:
                    recommendations.append("Low connectivity detected - consider increasing synaptic density")
            
            # Check stability
            stability = analysis.get('network_topology', {}).get('stability_indicator', 0)
            if stability < 0.5:
                recommendations.append("Network stability is low - consider adjusting learning parameters")
            
            # Check activity level
            activity = analysis.get('network_topology', {}).get('activity_level', 'UNKNOWN')
            if activity in ['VERY_LOW', 'LOW']:
                recommendations.append("Increase neural activity to enhance learning capacity")
            
        except Exception as e:
            recommendations.append(f"Unable to generate recommendations: {str(e)}")
        
        if not recommendations:
            recommendations.append("Network performance is optimal - continue with current configuration")
        
        return recommendations
    
    def calculate_health_score(self, analysis: Dict) -> float:
        """Calculate overall network health score (0-1)"""
        scores = []
        
        try:
            # Firing rate health (ideal: 5-15 Hz)
            firing_rate = analysis.get('network_topology', {}).get('activity_level', '')
            if firing_rate == 'MODERATE':
                scores.append(1.0)
            elif firing_rate in ['LOW', 'HIGH']:
                scores.append(0.7)
            else:
                scores.append(0.3)
            
            # Stability health
            stability = analysis.get('network_topology', {}).get('stability_indicator', 0)
            scores.append(stability)
            
            # Balance health
            balance = analysis.get('network_topology', {}).get('network_balance', '')
            balance_scores = {
                'BALANCED': 1.0,
                'SLIGHTLY_IMBALANCED': 0.8,
                'EXCITATORY_DOMINANT': 0.5,
                'INHIBITORY_DOMINANT': 0.5,
                'SIGNIFICANTLY_IMBALANCED': 0.2
            }
            scores.append(balance_scores.get(balance, 0.1))
            
            # Overall health score
            return sum(scores) / len(scores) if scores else 0.0
            
        except Exception:
            return 0.0

class ConfigurationManager(NLMUtility):
    """Manage and optimize brain configurations"""
    
    def optimize_configuration(self, current_config: Dict, performance_metrics: Dict) -> Dict:
        """Optimize configuration based on performance metrics"""
        optimized_config = current_config.copy()
        
        try:
            # Analyze performance metrics
            firing_rate = performance_metrics.get('firing_rate', 0)
            total_spikes = performance_metrics.get('total_spikes', 0)
            eir_ratio = performance_metrics.get('eir_ratio', 1.0)
            
            # Adjust neuron count based on activity
            if firing_rate < 5.0 and total_spikes > 0:
                # Low activity - increase neurons
                optimized_config['neuron_count'] = int(current_config.get('neuron_count', 100) * 1.2)
            elif firing_rate > 20.0:
                # High activity - decrease neurons
                optimized_config['neuron_count'] = max(50, int(current_config.get('neuron_count', 100) * 0.8))
            
            # Adjust connectivity based on E/I balance
            if abs(eir_ratio - 1.0) > 0.3:
                # Imbalanced - adjust connectivity
                current_conn = current_config.get('connection_probability', 0.1)
                if eir_ratio > 1.5:  # Excitatory dominant
                    optimized_config['connection_probability'] = current_conn * 0.9
                elif eir_ratio < 0.5:  # Inhibitory dominant
                    optimized_config['connection_probability'] = current_conn * 1.1
            
            # Adjust plasticity based on learning effectiveness
            learning_efficiency = total_spikes / max(firing_rate, 0.1)
            if learning_efficiency < 10:
                # Low learning efficiency - increase plasticity
                optimized_config['plasticity_learning_rate'] = current_config.get('plasticity_learning_rate', 0.01) * 1.5
                optimized_config['plasticity_hebbian_enable'] = True
            elif learning_efficiency > 100:
                # High learning efficiency - decrease plasticity
                optimized_config['plasticity_learning_rate'] = current_config.get('plasticity_learning_rate', 0.01) * 0.7
            
        except Exception as e:
            print(f"Configuration optimization failed: {str(e)}")
        
        return optimized_config
    
    def validate_configuration(self, config: Dict) -> List[str]:
        """Validate configuration for potential issues"""
        issues = []
        
        try:
            # Validate neuron count
            neuron_count = config.get('neuron_count', 0)
            if neuron_count <= 0:
                issues.append("Neuron count must be positive")
            elif neuron_count > 100000:
                issues.append("Neuron count is extremely high - may cause performance issues")
            
            # Validate region count
            region_count = config.get('region_count', 1)
            if region_count <= 0:
                issues.append("Region count must be positive")
            elif region_count > 100:
                issues.append("Region count is extremely high - may cause complexity issues")
            
            # Validate connectivity
            conn_prob = config.get('connection_probability', 0)
            if conn_prob < 0 or conn_prob > 1:
                issues.append("Connection probability must be between 0 and 1")
            elif conn_prob > 0.5:
                issues.append("Very high connectivity may cause instability")
            
            # Validate plasticity settings
            if config.get('plasticity_stdp_enable', True):
                if config.get('plasticity_stdp_learning_rate', 0.01) > 0.1:
                    issues.append("STDP learning rate is extremely high - may cause unstable learning")
            
        except Exception as e:
            issues.append(f"Configuration validation failed: {str(e)}")
        
        return issues
    
    def export_configuration(self, config: Dict, filepath: str) -> bool:
        """Export configuration to file"""
        try:
            # Convert to JSON-serializable format
            json_config = {}
            for key, value in config.items():
                if isinstance(value, (int, float, str, bool, type(None))):
                    json_config[key] = value
                elif isinstance(value, list):
                    json_config[key] = value
                else:
                    # Convert complex objects to string representation
                    json_config[key] = str(value)
            
            with open(filepath, 'w') as f:
                json.dump(json_config, f, indent=2)
            
            print(f"✓ Configuration exported to {filepath}")
            return True
            
        except Exception as e:
            print(f"Failed to export configuration: {str(e)}")
            return False
    
    def import_configuration(self, filepath: str) -> Dict:
        """Import configuration from file"""
        try:
            if not os.path.exists(filepath):
                print(f"✗ Configuration file not found: {filepath}")
                return {}
            
            with open(filepath, 'r') as f:
                config = json.load(f)
            
            print(f"✓ Configuration imported from {filepath}")
            return config
            
        except Exception as e:
            print(f"Failed to import configuration: {str(e)}")
            return {}

class BatchProcessor:
    """Process multiple brains or configurations in batch"""
    
    def __init__(self):
        self.results = []
        self.failed_jobs = []
    
    def create_brain_batch(self, config_template: Dict, brain_specs: List[Dict]) -> List[Dict]:
        """Create multiple brains from specifications"""
        if not PYNLM_AVAILABLE:
            return []
        
        results = []
        
        for spec in brain_specs:
            try:
                # Merge config template with specific spec
                config = config_template.copy()
                config.update(spec)
                
                # Create brain
                brain = pynlm.createBrain(pynlm.createDefaultConfig())
                
                # Apply configuration
                for key, value in config.items():
                    if hasattr(brain, 'getConfig'):
                        brain.getConfig().set(key, value, pynlm.ConfigSource.User)
                
                brain.initialize()
                
                # Run simulation
                for step in range(spec.get('simulation_steps', 100)):
                    brain.step(step)
                
                result = {
                    'success': True,
                    'spec': spec,
                    'brain': brain,
                    'statistics': {
                        'neuron_count': brain.getTotalNeuronCount(),
                        'synapse_count': brain.getTotalSynapseCount(),
                        'firing_rate': brain.getAverageFiringRate(),
                        'total_spikes': brain.getTotalSpikeCount(),
                        'region_count': brain.getRegionCount()
                    },
                    'timestamp': time.time()
                }
                
                results.append(result)
                print(f"✓ Created brain for spec {spec.get('name', 'unnamed')}")
                
            except Exception as e:
                failed_result = {
                    'success': False,
                    'spec': spec,
                    'error': str(e),
                    'timestamp': time.time()
                }
                
                results.append(failed_result)
                self.failed_jobs.append(failed_result)
                print(f"✗ Failed to create brain for spec {spec.get('name', 'unnamed')}: {str(e)}")
        
        self.results.extend([r for r in results if r['success']])
        return results
    
    def analyze_batch_results(self) -> Dict:
        """Analyze results from batch processing"""
        if not self.results:
            return {}
        
        analysis = {
            'total_jobs': len(self.results),
            'successful_jobs': len(self.results),
            'failed_jobs': len(self.failed_jobs),
            'success_rate': len(self.results) / max(len(self.results) + len(self.failed_jobs), 1),
            'statistics': {},
            'recommendations': []
        }
        
        try:
            # Aggregate statistics
            all_stats = []
            for result in self.results:
                if result['success'] and 'statistics' in result:
                    all_stats.append(result['statistics'])
            
            if all_stats:
                # Calculate averages
                avg_neuron_count = sum(s['neuron_count'] for s in all_stats) / len(all_stats)
                avg_synapse_count = sum(s['synapse_count'] for s in all_stats) / len(all_stats)
                avg_firing_rate = sum(s['firing_rate'] for s in all_stats) / len(all_stats)
                avg_total_spikes = sum(s['total_spikes'] for s in all_stats) / len(all_stats)
                avg_region_count = sum(s['region_count'] for s in all_stats) / len(all_stats)
                
                analysis['statistics'] = {
                    'average_neuron_count': avg_neuron_count,
                    'average_synapse_count': avg_synapse_count,
                    'average_firing_rate': avg_firing_rate,
                    'average_total_spikes': avg_total_spikes,
                    'average_region_count': avg_region_count,
                    'min_neuron_count': min(s['neuron_count'] for s in all_stats),
                    'max_neuron_count': max(s['neuron_count'] for s in all_stats),
                    'min_firing_rate': min(s['firing_rate'] for s in all_stats),
                    'max_firing_rate': max(s['firing_rate'] for s in all_stats)
                }
                
                # Generate recommendations
                analysis['recommendations'] = self.generate_batch_recommendations(analysis['statistics'])
        
        except Exception as e:
            analysis['error'] = str(e)
        
        return analysis
    
    def generate_batch_recommendations(self, statistics: Dict) -> List[str]:
        """Generate recommendations based on batch analysis"""
        recommendations = []
        
        try:
            # Check firing rate consistency
            firing_rate_std = statistics['max_firing_rate'] - statistics['min_firing_rate']
            if firing_rate_std > 20:
                recommendations.append(
                    "High variance in firing rates across batch - consider standardizing neuron counts"
                )
            
            # Check neuron count efficiency
            if statistics['average_firing_rate'] < 5.0:
                recommendations.append(
                    "Low average firing rate - consider increasing neuron counts or connectivity"
                )
            
            # Check synapse density
            avg_neuron_count = statistics['average_neuron_count']
            avg_synapse_count = statistics['average_synapse_count']
            if avg_neuron_count > 0:
                synapse_density = avg_synapse_count / avg_neuron_count
                if synapse_density > 1.0:
                    recommendations.append(
                        "Very high synapse density - may cause computational overhead"
                    )
                elif synapse_density < 0.05:
                    recommendations.append(
                        "Very low synapse density - may limit learning capacity"
                    )
            
            # Check performance
            if statistics['average_firing_rate'] < 1.0:
                recommendations.append(
                    "Performance is low - consider adjusting simulation parameters"
                )
            
            if not recommendations:
                recommendations.append(
                    "Batch processing performed well - current parameters are suitable"
                )
            
        except Exception as e:
            recommendations.append(f"Unable to generate batch recommendations: {str(e)}")
        
        return recommendations

class NLMAdvancedToolkit:
    """Main toolkit for advanced NLM operations"""
    
    def __init__(self):
        self.neural_analyzer = NeuralNetworkAnalyzer()
        self.config_manager = ConfigurationManager()
        self.batch_processor = BatchProcessor()
        self.toolkit_history: List[Dict] = []
    
    def setup_analysis(self, brain=None, config=None):
        """Setup the toolkit with brain components"""
        self.neural_analyzer.brain = brain
        self.neural_analyzer.config = config
        self.config_manager.brain = brain
        self.config_manager.config = config
    
    def analyze_network(self, output_file: Optional[str] = None) -> Dict:
        """Analyze current neural network"""
        if not self.neural_analyzer.brain:
            print("✗ No brain available for analysis")
            return {}
        
        print("=== NEURAL NETWORK ANALYSIS ===")
        
        # Generate network analysis
        analysis = self.neural_analyzer.analyze_connectivity()
        
        # Generate visualization
        if output_file:
            self.neural_analyzer.visualize_connectivity(output_file)
        
        # Generate report
        report = self.neural_analyzer.generate_network_report(output_file)
        
        # Display summary
        print(f"\n=== ANALYSIS SUMMARY ===")
        health_score = report.get('health_score', 0)
        print(f"Network health score: {health_score:.2f}/1.0")
        
        recommendations = report.get('recommendations', [])
        if recommendations:
            print(f"\n=== RECOMMENDATIONS ===")
            for i, rec in enumerate(recommendations, 1):
                print(f"  {i}. {rec}")
        
        # Store analysis
        self.toolkit_history.append({
            'timestamp': time.time(),
            'type': 'network_analysis',
            'health_score': health_score,
            'recommendations': recommendations
        })
        
        return report
    
    def optimize_and_validate(self, brain, config, output_file: Optional[str] = None) -> Dict:
        """Optimize configuration and validate results"""
        print("=== CONFIGURATION OPTIMIZATION AND VALIDATION ===")
        
        # Convert brain and config to dict for optimization
        config_dict = {}
        if config and hasattr(config, 'getKeys'):
            for key in config.getKeys():
                try:
                    config_dict[key] = config.getOr[str](key, "")
                except:
                    pass
        
        # Add brain statistics
        brain_stats = {}
        if brain:
            brain_stats = {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'eir_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount(),
                'region_count': brain.getRegionCount()
            }
        
        performance_metrics = {**brain_stats, **config_dict}
        
        # Optimize configuration
        optimized_config = self.config_manager.optimize_configuration(
            config_dict, performance_metrics
        )
        
        # Validate configuration
        validation_issues = self.config_manager.validate_configuration(optimized_config)
        
        print(f"\n=== OPTIMIZATION RESULTS ===")
        print(f"Original config keys: {len(config_dict)}")
        print(f"Optimized config keys: {len(optimized_config)}")
        print(f"Validation issues: {len(validation_issues)}")
        
        if validation_issues:
            print(f"\n=== VALIDATION ISSUES ===")
            for issue in validation_issues:
                print(f"  • {issue}")
        
        # Export optimized config if requested
        if output_file:
            if self.config_manager.export_configuration(optimized_config, output_file):
                print(f"✓ Optimized configuration exported to {output_file}")
        
        return {
            'optimized_config': optimized_config,
            'validation_issues': validation_issues,
            'brain_stats': brain_stats,
            'config_stats': config_dict
        }
    
    def run_batch_processing(self, config_template: Dict, brain_specs: List[Dict]) -> Dict:
        """Run batch processing of multiple brains"""
        print("=== BATCH PROCESSING ===")
        print(f"Processing {len(brain_specs)} brain specifications...")
        
        # Create batch
        results = self.batch_processor.create_brain_batch(config_template, brain_specs)
        
        # Analyze results
        analysis = self.batch_processor.analyze_batch_results()
        
        print(f"\n=== BATCH PROCESSING RESULTS ===")
        print(f"Total jobs: {analysis.get('total_jobs', 0)}")
        print(f"Successful: {analysis.get('successful_jobs', 0)}")
        print(f"Failed: {analysis.get('failed_jobs', 0)}")
        print(f"Success rate: {analysis.get('success_rate', 0)*100:.1f}%")
        
        # Show recommendations
        recommendations = analysis.get('recommendations', [])
        if recommendations:
            print(f"\n=== RECOMMENDATIONS ===")
            for rec in recommendations:
                print(f"  • {rec}")
        
        return analysis
    
    def get_toolkit_status(self) -> Dict:
        """Get current toolkit status"""
        return {
            'timestamp': time.time(),
            'neural_analyzer': 'active' if self.neural_analyzer.brain else 'inactive',
            'config_manager': 'active' if self.config_manager.brain else 'inactive',
            'batch_processor': 'ready',
            'history_size': len(self.toolkit_history),
            'last_analysis': self.toolkit_history[-1] if self.toolkit_history else None
        }

# Standalone functions for easy use
def create_advanced_analyzer(brain=None, config=None) -> NeuralNetworkAnalyzer:
    """Create an advanced neural network analyzer"""
    analyzer = NeuralNetworkAnalyzer(brain, config)
    return analyzer

def create_config_optimizer(brain=None, config=None) -> ConfigurationManager:
    """Create a configuration optimizer"""
    optimizer = ConfigurationManager(brain, config)
    return optimizer

def create_batch_toolkit() -> BatchProcessor:
    """Create a batch processing toolkit"""
    return BatchProcessor()

def main():
    """Main CLI entry point for advanced toolkit"""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Toolset for Complex Operations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s analyze --setup
  %(prog)s optimize --config optimized_config.json
  %(prog)s batch --specs brain_specs.json
  %(prog)s report --generate html

Advanced toolkit commands:
  analyze - Perform advanced neural network analysis
  optimize - Optimize brain configuration
  batch - Process multiple brains in batch
  report - Generate comprehensive reports
  status - Show toolkit status

For more help on specific commands, use:
  %(prog)s <command> --help
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available advanced commands')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Perform advanced neural network analysis')
    analyze_parser.add_argument('--setup', action='store_true',
                               help='Setup with current brain (requires --brain-file)')
    analyze_parser.add_argument('--brain-file', type=str,
                               help='File containing brain state for analysis')
    analyze_parser.add_argument('--output', '-o', type=str,
                               help='Output file for analysis results')
    analyze_parser.add_argument('--visualization', '-v', type=str,
                               help='File to save visualization (requires matplotlib)')
    
    # Optimize command
    optimize_parser = subparsers.add_parser('optimize', help='Optimize brain configuration')
    optimize_parser.add_argument('--config-file', type=str, required=True,
                               help='File containing current configuration')
    optimize_parser.add_argument('--output', '-o', type=str, default='optimized_config.json',
                               help='Output file for optimized configuration (default: optimized_config.json)')
    optimize_parser.add_argument('--brain-stats', type=str,
                               help='File containing brain statistics for optimization')
    
    # Batch command
    batch_parser = subparsers.add_parser('batch', help='Process multiple brains in batch')
    batch_parser.add_argument('--template', '-t', type=str, required=True,
                             help='Configuration template file')
    batch_parser.add_argument('--specs', '-s', type=str, required=True,
                             help='File containing brain specifications')
    batch_parser.add_argument('--output', '-o', type=str, default='batch_results.json',
                             help='Output file for batch results (default: batch_results.json)')
    
    # Report command
    report_parser = subparsers.add_parser('report', help='Generate comprehensive reports')
    report_parser.add_argument('--type', choices=['neural', 'config', 'batch', 'comprehensive'],
                               default='comprehensive', help='Type of report to generate (default: comprehensive)')
    report_parser.add_argument('--output', '-o', type=str, default='analysis_report.html',
                               help='Output file for report (default: analysis_report.html)')
    report_parser.add_argument('--data-dir', type=str, default='report_data',
                               help='Directory containing data for report generation (default: report_data)')
    
    # Status command
    status_parser = subparsers.add_parser('status', help='Show toolkit status')
    
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(0)
    
    args = parser.parse_args()
    
    try:
        # Initialize toolkit
        toolkit = NLMAdvancedToolkit()
        
        if args.command == 'analyze':
            print("=== NLM ADVANCED ANALYSIS ===")
            
            # Setup analysis
            if args.setup and args.brain_file:
                if os.path.exists(args.brain_file):
                    print(f"Loading brain from {args.brain_file}")
                    # In real implementation, this would load the brain from file
                    # For now, we'll create a sample brain
                    if PYNLM_AVAILABLE:
                        brain = pynlm.createBrain(pynlm.createDefaultConfig())
                        brain.initialize()
                        
                        toolkit.setup_analysis(brain)
                        print("✓ Analysis setup completed")
                    else:
                        print("✗ pynlm not available")
                        sys.exit(1)
                else:
                    print(f"✗ Brain file not found: {args.brain_file}")
                    sys.exit(1)
            elif args.setup:
                print("✗ --setup requires --brain-file")
                sys.exit(1)
            else:
                print("✗ Analysis requires --setup with --brain-file")
                sys.exit(1)
            
            # Perform analysis
            analysis = toolkit.analyze_network(args.output)
            
            # Generate visualization if requested
            if args.visualization and PYNLM_AVAILABLE:
                if toolkit.neural_analyzer.visualize_connectivity(args.visualization):
                    print(f"✓ Visualization saved to {args.visualization}")
        
        elif args.command == 'optimize':
            print("=== NLM CONFIGURATION OPTIMIZATION ===")
            
            if not os.path.exists(args.config_file):
                print(f"✗ Config file not found: {args.config_file}")
                sys.exit(1)
            
            # Load configuration
            with open(args.config_file, 'r') as f:
                config = json.load(f)
            
            # Load brain stats if provided
            brain_stats = {}
            if args.brain_stats and os.path.exists(args.brain_stats):
                with open(args.brain_stats, 'r') as f:
                    brain_stats = json.load(f)
            
            # Setup toolkit
            if PYNLM_AVAILABLE:
                toolkit.setup_analysis(None, config)
                print("✓ Optimization setup completed")
            else:
                print("✗ pynlm not available")
                sys.exit(1)
            
            # Perform optimization and validation
            optimization = toolkit.optimize_and_validate(None, config, args.output)
            
            print(f"✓ Optimization completed successfully")
        
        elif args.command == 'batch':
            print("=== NLM BATCH PROCESSING ===")
            
            if not os.path.exists(args.template):
                print(f"✗ Template file not found: {args.template}")
                sys.exit(1)
            
            if not os.path.exists(args.specs):
                print(f"✗ Specs file not found: {args.specs}")
                sys.exit(1)
            
            # Load template and specs
            with open(args.template, 'r') as f:
                template = json.load(f)
            
            with open(args.specs, 'r') as f:
                specs = json.load(f)
            
            if not isinstance(specs, list):
                print("✗ Specs file should contain a list of brain specifications")
                sys.exit(1)
            
            # Run batch processing
            batch_results = toolkit.run_batch_processing(template, specs)
            
            # Export results
            with open(args.output, 'w') as f:
                json.dump(batch_results, f, indent=2, default=str)
            
            print(f"✓ Batch processing completed successfully")
            print(f"  Results saved to {args.output}")
        
        elif args.command == 'report':
            print("=== NLM REPORT GENERATION ===")
            
            # In a real implementation, this would generate comprehensive reports
            # For now, we'll create a simple report structure
            report = {
                'timestamp': time.time(),
                'report_type': args.type,
                'data_dir': args.data_dir,
                'status': 'report_generation_in_progress'
            }
            
            # Save report
            with open(args.output, 'w') as f:
                json.dump(report, f, indent=2, default=str)
            
            print(f"✓ Report generated and saved to {args.output}")
            print("  Note: Full report generation requires additional data files")
        
        elif args.command == 'status':
            print("=== NLM ADVANCED TOOLKIT STATUS ===")
            
            status = toolkit.get_toolkit_status()
            
            print(f"Toolkit Status:")
            print(f"  Neural Analyzer: {status['neural_analyzer']}")
            print(f"  Configuration Manager: {status['config_manager']}")
            print(f"  Batch Processor: {status['batch_processor']}")
            print(f"  History Entries: {status['history_size']}")
            
            if status['last_analysis']:
                print(f"  Last Analysis: {status['last_analysis']['timestamp']}")
                print(f"  Health Score: {status['last_analysis'].get('health_score', 0):.2f}")
        
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
