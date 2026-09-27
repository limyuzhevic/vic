"""
Comparison utilities for NLM CLI
"""

import json
import yaml
import numpy as np
from pathlib import Path
from typing import Dict, Any, List, Tuple
from datetime import datetime

class NLMComparison:
    def __init__(self):
        self.results = []
    
    def compare_brain_results(self, result1: Dict[str, Any], result2: Dict[str, Any], 
                             result1_name: str = "Brain 1", result2_name: str = "Brain 2") -> Dict[str, Any]:
        """Compare two brain simulation results"""
        comparison = {
            "comparison_info": {
                "timestamp": datetime.now().isoformat(),
                "result1_name": result1_name,
                "result2_name": result2_name
            },
            "metrics": {},
            "summary": {}
        }
        
        # Compare key metrics
        metrics_to_compare = [
            'total_neurons',
            'total_spikes', 
            'firing_neurons',
            'average_firing_rate',
            'development_stage',
            'total_synapses'
        ]
        
        for metric in metrics_to_compare:
            val1 = result1.get('brain_status', {}).get(metric, 0)
            val2 = result2.get('brain_status', {}).get(metric, 0)
            
            if isinstance(val1, (int, float)) and isinstance(val2, (int, float)):
                diff = val2 - val1
                percentage = (diff / val1 * 100) if val1 != 0 else 0
                
                comparison['metrics'][metric] = {
                    'brain1': val1,
                    'brain2': val2,
                    'difference': diff,
                    'percentage_change': percentage,
                    'winner': self._determine_winner(val1, val2, metric)
                }
        
        # Generate summary
        comparison['summary'] = {
            'total_metrics_compared': len(comparison['metrics']),
            'identical_results': sum(1 for m in comparison['metrics'].values() if m['difference'] == 0),
            'brain1_better': sum(1 for m in comparison['metrics'].values() if m['winner'] == result1_name),
            'brain2_better': sum(1 for m in comparison['metrics'].values() if m['winner'] == result2_name),
            'overall_winner': self._determine_overall_winner(comparison['metrics'])
        }
        
        return comparison
    
    def _determine_winner(self, value1: float, value2: float, metric: str) -> str:
        """Determine which result won for a specific metric"""
        # For most metrics, higher is better
        if metric in ['total_spikes', 'firing_neurons', 'average_firing_rate', 'total_synapses']:
            if value2 > value1:
                return "Brain 2"
            elif value1 > value2:
                return "Brain 1"
            else:
                return "Tie"
        # For development_stage, higher is better
        elif metric == 'development_stage':
            if value2 > value1:
                return "Brain 2"
            elif value1 > value2:
                return "Brain 1"
            else:
                return "Tie"
        # For neuron count, higher is generally better
        elif metric == 'total_neurons':
            if value2 > value1:
                return "Brain 2"
            elif value1 > value2:
                return "Brain 1"
            else:
                return "Tie"
        else:
            # For unknown metrics, use difference
            return "Tie"
    
    def _determine_overall_winner(self, metrics: Dict[str, Any]) -> str:
        """Determine overall winner based on multiple metrics"""
        brain1_score = 0
        brain2_score = 0
        
        for metric_data in metrics.values():
            winner = metric_data['winner']
            if winner == "Brain 1":
                brain1_score += 1
            elif winner == "Brain 2":
                brain2_score += 1
        
        if brain1_score > brain2_score:
            return "Brain 1"
        elif brain2_score > brain1_score:
            return "Brain 2"
        else:
            return "Tie"
    
    def compare_parameter_sets(self, param_set1: Dict[str, Any], param_set2: Dict[str, Any],
                              name1: str = "Config 1", name2: str = "Config 2") -> Dict[str, Any]:
        """Compare two parameter sets"""
        comparison = {
            "parameter_comparison": {
                "timestamp": datetime.now().isoformat(),
                "config1_name": name1,
                "config2_name": name2
            },
            "parameters": {},
            "differences": {},
            "similarities": []
        }
        
        # Get all keys from both parameter sets
        all_keys = set(param_set1.keys()) | set(param_set2.keys())
        
        for key in sorted(all_keys):
            val1 = param_set1.get(key)
            val2 = param_set2.get(key)
            
            if key not in param_set1:
                comparison['parameters'][key] = {
                    'config1': None,
                    'config2': val2,
                    'status': 'only_in_config2'
                }
                comparison['differences'][key] = f"Only in {name2}"
            elif key not in param_set2:
                comparison['parameters'][key] = {
                    'config1': val1,
                    'config2': None,
                    'status': 'only_in_config1'
                }
                comparison['differences'][key] = f"Only in {name1}"
            else:
                comparison['parameters'][key] = {
                    'config1': val1,
                    'config2': val2,
                    'status': 'both'
                }
                
                # Compare values
                if val1 == val2:
                    comparison['similarities'].append(key)
                else:
                    comparison['differences'][key] = self._format_difference(key, val1, val2)
        
        return comparison
    
    def _format_difference(self, key: str, value1: Any, value2: Any) -> str:
        """Format a difference between two values"""
        if isinstance(value1, (int, float)) and isinstance(value2, (int, float)):
            diff = value2 - value1
            return f"Config 2 has {diff:+.2f} ({value2:.2f} vs {value1:.2f})"
        else:
            return f"Config 2: {value2} vs Config 1: {value1}"
    
    def compare_simulation_performance(self, performance_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare performance across multiple simulations"""
        if not performance_data:
            return {
                "error": "No performance data to compare",
                "timestamp": datetime.now().isoformat()
            }
        
        comparison = {
            "performance_comparison": {
                "timestamp": datetime.now().isoformat(),
                "total_simulations": len(performance_data)
            },
            "metrics": {},
            "rankings": {}
        }
        
        # Extract performance metrics
        metrics = {}
        for i, data in enumerate(performance_data):
            sim_id = data.get('simulation_id', f"Simulation {i+1}")
            
            sim_metrics = {
                'steps': data.get('steps', 0),
                'duration': data.get('duration_seconds', 0),
                'neurons': data.get('neuron_count', 0),
                'total_spikes': data.get('total_spikes', 0),
                'firing_rate': data.get('average_firing_rate', 0)
            }
            
            metrics[sim_id] = sim_metrics
        
        # Compare metrics
        metric_categories = {
            'speed': ['steps_per_second'],
            'efficiency': ['firing_rate_per_neuron'],
            'output': ['total_spikes'],
            'scalability': ['duration_per_neuron']
        }
        
        for category, metric_list in metric_categories.items():
            comparison['metrics'][category] = {}
            
            for sim_id, metrics_data in metrics.items():
                computed_metrics = {}
                
                for metric in metric_list:
                    if metric == 'steps_per_second':
                        computed_metrics[metric] = metrics_data['steps'] / metrics_data['duration'] if metrics_data['duration'] > 0 else 0
                    elif metric == 'firing_rate_per_neuron':
                        computed_metrics[metric] = metrics_data['firing_rate'] / metrics_data['neurons'] if metrics_data['neurons'] > 0 else 0
                    elif metric == 'duration_per_neuron':
                        computed_metrics[metric] = metrics_data['duration'] / metrics_data['neurons'] if metrics_data['neurons'] > 0 else 0
                
                comparison['metrics'][category][sim_id] = computed_metrics
        
        # Generate rankings
        comparison['rankings'] = self._rank_simulations(comparison['metrics'])
        
        return comparison
    
    def _rank_simulations(self, metrics: Dict[str, Dict[str, Dict[str, float]]]) -> Dict[str, Any]:
        """Rank simulations based on performance metrics"""
        rankings = {
            "overall_ranking": [],
            "category_rankings": {},
            "best_performers": {}
        }
        
        # Get all simulation IDs
        sim_ids = list(metrics['speed'].keys())
        
        # Calculate scores for each simulation
        sim_scores = {}
        for sim_id in sim_ids:
            total_score = 0
            category_scores = {}
            
            for category, category_metrics in metrics.items():
                if sim_id in category_metrics:
                    # Calculate normalized score for this category
                    scores = [v for v in category_metrics[sim_id].values() if isinstance(v, (int, float))]
                    if scores:
                        avg_score = sum(scores) / len(scores)
                        category_scores[category] = avg_score
                        total_score += avg_score
            
            sim_scores[sim_id] = {
                'total_score': total_score,
                'category_scores': category_scores
            }
        
        # Create overall ranking
        sorted_sims = sorted(sim_scores.items(), key=lambda x: x[1]['total_score'], reverse=True)
        rankings['overall_ranking'] = [
            {
                'simulation_id': sim_id,
                'total_score': score_data['total_score'],
                'category_scores': score_data['category_scores']
            }
            for sim_id, score_data in sorted_sims
        ]
        
        # Create category rankings
        for category, category_metrics in metrics.items():
            category_ranking = []
            for sim_id, sim_metrics in category_metrics.items():
                avg_score = np.mean(list(sim_metrics.values())) if sim_metrics else 0
                category_ranking.append((sim_id, avg_score))
            
            category_ranking.sort(key=lambda x: x[1], reverse=True)
            rankings['category_rankings'][category] = [
                {'simulation_id': sim_id, 'score': score} for sim_id, score in category_ranking
            ]
        
        # Identify best performers
        rankings['best_performers'] = {
            'fastest': rankings['category_rankings']['speed'][0] if rankings['category_rankings'].get('speed') else None,
            'most_efficient': rankings['category_rankings']['efficiency'][0] if rankings['category_rankings'].get('efficiency') else None,
            'highest_output': rankings['category_rankings']['output'][0] if rankings['category_rankings'].get('output') else None,
            'best_overall': rankings['overall_ranking'][0] if rankings['overall_ranking'] else None
        }
        
        return rankings
