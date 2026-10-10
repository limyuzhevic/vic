"""
Advanced analysis tools for NLM neural simulations.

Provides network topology analysis, plasticity rule effectiveness analysis,
learning curve analysis, and behavioral pattern analysis.
"""

import numpy as np
import pandas as pd
import networkx as nx
from typing import Dict, List, Any, Optional, Tuple
from datetime import datetime
import matplotlib.pyplot as plt
from scipy import stats
from scipy.optimize import curve_fit
import json
import os

class NetworkTopologyAnalyzer:
    """Analyzes neural network topology and connectivity patterns."""
    
    def __init__(self, brain=None):
        """
        Initialize topology analyzer.
        
        Args:
            brain: Reference to the brain object (optional)
        """
        self.brain = brain
        
    def analyze_connectivity_matrix(self) -> Dict[str, Any]:
        """Analyze connectivity matrix properties."""
        if not self.brain:
            raise ValueError("Brain object required for topology analysis")
            
        # Build connectivity matrix
        neuron_count = self.brain.getTotalNeuronCount()
        connectivity = np.zeros((neuron_count, neuron_count))
        
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                source_idx = synapse.getSourceId()
                target_idx = synapse.getTargetId()
                if source_idx < neuron_count and target_idx < neuron_count:
                    connectivity[source_idx, target_idx] = synapse.getWeight()
                    
        # Analyze matrix properties
        analysis = {
            'matrix_shape': connectivity.shape,
            'total_connections': np.count_nonzero(connectivity),
            'connection_density': np.count_nonzero(connectivity) / (neuron_count ** 2),
            'average_weight': np.mean(connectivity[np.nonzero(connectivity)]),
            'max_weight': np.max(connectivity),
            'min_weight': np.min(connectivity[np.nonzero(connectivity)]) if np.any(connectivity) else 0,
            'weight_std': np.std(connectivity[np.nonzero(connectivity)]) if np.any(connectivity) else 0,
            'self_connections': np.trace(connectivity),
            'symmetric_connections': 0
        }
        
        # Check symmetry
        if np.allclose(connectivity, connectivity.T):
            analysis['symmetric'] = True
            analysis['symmetric_connection_count'] = np.count_nonzero(
                np.triu(connectivity, k=1) + np.tril(connectivity, k=-1)
            )
        else:
            analysis['symmetric'] = False
            
        # Degree analysis
        in_degree = np.sum(connectivity > 0, axis=0)
        out_degree = np.sum(connectivity > 0, axis=1)
        
        analysis['in_degree_stats'] = {
            'mean': np.mean(in_degree),
            'std': np.std(in_degree),
            'max': np.max(in_degree),
            'min': np.min(in_degree)
        }
        
        analysis['out_degree_stats'] = {
            'mean': np.mean(out_degree),
            'std': np.std(out_degree),
            'max': np.max(out_degree),
            'min': np.min(out_degree)
        }
        
        return analysis
        
    def analyze_region_topology(self) -> Dict[str, Any]:
        """Analyze topology within each brain region."""
        if not self.brain:
            raise ValueError("Brain object required for topology analysis")
            
        region_analysis = {}
        
        for region in self.brain.getRegions():
            region_id = region.getId()
            
            # Get neurons and synapses in this region
            neurons = region.getNeurons()
            synapses = region.getSynapses()
            
            if not neurons:
                continue
                
            # Build local connectivity matrix
            neuron_count = len(neurons)
            connectivity = np.zeros((neuron_count, neuron_count))
            
            # Map neuron IDs to indices
            neuron_id_to_idx = {neuron.getId(): i for i, neuron in enumerate(neurons)}
            
            for synapse in synapses:
                if (synapse.getSourceId() in neuron_id_to_idx and 
                    synapse.getTargetId() in neuron_id_to_idx):
                    source_idx = neuron_id_to_idx[synapse.getSourceId()]
                    target_idx = neuron_id_to_idx[synapse.getTargetId()]
                    connectivity[source_idx, target_idx] = synapse.getWeight()
                    
            # Analyze local properties
            neuron_types = [neuron.getType() for neuron in neurons]
            
            region_analysis[region_id] = {
                'neuron_count': neuron_count,
                'synapse_count': len(synapses),
                'neuron_types': {
                    'Excitatory': neuron_types.count('Excitatory'),
                    'Inhibitory': neuron_types.count('Inhibitory'),
                    'Sensory': neuron_types.count('Sensory'),
                    'Motor': neuron_types.count('Motor'),
                    'Internal': neuron_types.count('Internal'),
                    'Modulatory': neuron_types.count('Modulatory')
                },
                'connectivity_density': (np.count_nonzero(connectivity) / 
                                       (neuron_count ** 2) if neuron_count > 0 else 0),
                'average_weight': (np.mean(connectivity[np.nonzero(connectivity)]) 
                                 if np.any(connectivity) else 0),
                'inhibition_excitation_ratio': 0.0  # Would need to compute based on synapse types
            }
            
        return region_analysis
        
    def build_connection_graph(self) -> nx.Graph:
        """Build network graph representation."""
        if not self.brain:
            raise ValueError("Brain object required for topology analysis")
            
        graph = nx.Graph()
        
        # Add nodes
        for region in self.brain.getRegions():
            for neuron in region.getNeurons():
                graph.add_node(
                    neuron.getId(),
                    region_id=region.getId(),
                    population_id=neuron.getPopulationId(),
                    neuron_type=neuron.getType(),
                    membrane_potential=neuron.getMembranePotential()
                )
                
        # Add edges
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                graph.add_edge(
                    synapse.getSourceId(),
                    synapse.getTargetId(),
                    weight=synapse.getWeight(),
                    delay=synapse.getDelay(),
                    type=synapse.getType()
                )
                
        return graph
        
    def analyze_graph_metrics(self, graph: nx.Graph) -> Dict[str, Any]:
        """Analyze graph metrics."""
        if graph.number_of_nodes() == 0:
            return {}
            
        metrics = {}
        
        # Basic graph metrics
        metrics['nodes'] = graph.number_of_nodes()
        metrics['edges'] = graph.number_of_edges()
        metrics['density'] = nx.density(graph)
        metrics['average_clustering'] = nx.average_clustering(graph)
        
        if nx.is_connected(graph):
            metrics['connected'] = True
            metrics['diameter'] = nx.diameter(graph)
            metrics['radius'] = nx.radius(graph)
            metrics['average_shortest_path'] = nx.average_shortest_path_length(graph)
        else:
            metrics['connected'] = False
            # Calculate metrics for largest connected component
            largest_cc = max(nx.connected_components(graph), key=len)
            subgraph = graph.subgraph(largest_cc)
            
            if len(subgraph) > 1:
                metrics['largest_component_size'] = len(largest_cc)
                metrics['largest_component_density'] = nx.density(subgraph)
                metrics['largest_component_diameter'] = nx.diameter(subgraph)
                metrics['largest_component_clustering'] = nx.average_clustering(subgraph)
                
        # Degree distribution
        degree_seq = sorted([d for n, d in graph.degree()], reverse=True)
        metrics['degree_distribution'] = degree_seq
        
        # Analysis by neuron type
        neuron_types = nx.get_node_attributes(graph, 'neuron_type')
        if neuron_types:
            type_metrics = {}
            for node_type in set(neuron_types.values()):
                subgraph = graph.subgraph([n for n, t in neuron_types.items() if t == node_type])
                type_metrics[node_type] = {
                    'size': len(subgraph.nodes()),
                    'edges': len(subgraph.edges()),
                    'density': nx.density(subgraph) if len(subgraph) > 1 else 0,
                    'average_clustering': nx.average_clustering(subgraph)
                }
            metrics['by_neuron_type'] = type_metrics
            
        return metrics
        
    def visualize_topology(self, output_file: str,
                          graph: Optional[nx.Graph] = None,
                          layout: str = 'spring')
        """
        Visualize network topology.
        
        Args:
            output_file: Output file path
            graph: Network graph (optional, will build if not provided)
            layout: Layout algorithm ('spring', 'circular', 'random', 'kamada_kawai')
        """
        if graph is None:
            graph = self.build_connection_graph()
            
        plt.figure(figsize=(12, 10))
        
        # Select layout
        if layout == 'spring':
            pos = nx.spring_layout(graph, k=1, iterations=50)
        elif layout == 'circular':
            pos = nx.circular_layout(graph)
        elif layout == 'random':
            pos = nx.random_layout(graph)
        elif layout == 'kamada_kawai':
            pos = nx.kamada_kawai_layout(graph)
        else:
            pos = nx.spring_layout(graph)
            
        # Node colors by neuron type
        neuron_types = nx.get_node_attributes(graph, 'neuron_type')
        node_colors = []
        for node_id in graph.nodes():
            node_type = neuron_types.get(node_id, 'unknown')
            if node_type == 'Excitatory':
                node_colors.append('red')
            elif node_type == 'Inhibitory':
                node_colors.append('blue')
            elif node_type == 'Sensory':
                node_colors.append('green')
            elif node_type == 'Motor':
                node_colors.append('orange')
            else:
                node_colors.append('gray')
                
        # Draw graph
        nx.draw_networkx_nodes(graph, pos, node_color=node_colors, node_size=50)
        nx.draw_networkx_edges(graph, pos, alpha=0.5, width=1)
        nx.draw_networkx_labels(graph, pos, font_size=8)
        
        plt.title('Neural Network Topology')
        plt.axis('off')
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
class PlasticityAnalyzer:
    """Analyzes plasticity rule effectiveness and dynamics."""
    
    def __init__(self, brain=None):
        """
        Initialize plasticity analyzer.
        
        Args:
            brain: Reference to the brain object (optional)
        """
        self.brain = brain
        
    def analyze_stdp_dynamics(self, recording_file: str) -> Dict[str, Any]:
        """
        Analyze STDP (Spike-Timing-Dependent Plasticity) dynamics.
        
        Args:
            recording_file: File containing simulation recording
            
        Returns:
            STDP analysis results
        """
        # Load recording data
        with open(recording_file, 'r') as f:
            data = json.load(f)
            
        # Extract spike timing and weight changes
        spike_times = []
        weight_changes = []
        
        for frame in data['recordings']:
            step = frame['step']
            simulation_time = frame['simulation_time']
            
            # This would need to track actual spike times and weight changes
            # from the simulation
            for neuron_id, state in frame['neuron_states'].items():
                if state.get('spike_detected', False):
                    spike_times.append((neuron_id, simulation_time))
                    
        analysis = {
            'total_spikes': len(spike_times),
            'unique_neurons': len(set(s[0] for s in spike_times)),
            'mean_inter_spike_interval': 0.0,  # Would need calculation
            'weight_change_rate': 0.0  # Would need calculation
        }
        
        return analysis
        
    def analyze_plasticity_effectiveness(self) -> Dict[str, Any]:
        """
        Analyze effectiveness of plasticity rules.
        
        Returns:
            Plasticity effectiveness analysis
        """
        if not self.brain:
            raise ValueError("Brain object required for plasticity analysis")
            
        # This would analyze plasticity rules like STDP, Hebbian, structural plasticity
        # by tracking weight changes over time
        
        analysis = {
            'rules_analyzed': [],
            'weight_change_patterns': {},
            'learning_curves': {}
        }
        
        return analysis
class LearningCurveAnalyzer:
    """Analyzes learning curves and performance over time."""
    
    def __init__(self, brain=None):
        """
        Initialize learning curve analyzer.
        
        Args:
            brain: Reference to the brain object (optional)
        """
        self.brain = brain
        
    def analyze_performance_learning(self, recording_file: str,
                                   metric_function: Optional[Callable] = None) -> Dict[str, Any]:
        """
        Analyze learning curves from simulation recording.
        
        Args:
            recording_file: File containing simulation recording
            metric_function: Function to extract performance metric (optional)
            
        Returns:
            Learning curve analysis
        """
        # Load recording data
        with open(recording_file, 'r') as f:
            data = json.load(f)
            
        # Extract metrics over time
        steps = []
        metrics = []
        
        for frame in data['recordings']:
            step = frame['step']
            
            if metric_function:
                metric = metric_function(frame)
            else:
                # Default: use spike count or firing rate
                metric = len(frame['neuron_states'])
                
            steps.append(step)
            metrics.append(metric)
            
        # Fit learning curve
        if len(steps) > 10:
            try:
                # Try exponential learning curve
                def exp_decay(x, a, b, c):
                    return a * np.exp(-b * x) + c
                    
                popt, pcov = curve_fit(exp_decay, steps, metrics, p0=[max(metrics), 0.01, min(metrics)])
                fitted_curve = exp_decay(np.array(steps), *popt)
                
                learning_analysis = {
                    'steps': steps,
                    'metrics': metrics,
                    'fitted_curve': fitted_curve.tolist(),
                    'parameters': {
                        'asymptote': popt[2],
                        'decay_rate': popt[1],
                        'initial_value': popt[0]
                    },
                    'r_squared': self._calculate_r_squared(np.array(metrics), fitted_curve),
                    'convergence_step': self._find_convergence_point(steps, metrics)
                }
            except:
                # If curve fitting fails, use basic statistics
                learning_analysis = {
                    'steps': steps,
                    'metrics': metrics,
                    'statistics': {
                        'mean': np.mean(metrics),
                        'std': np.std(metrics),
                        'min': np.min(metrics),
                        'max': np.max(metrics),
                        'trend': np.polyfit(steps, metrics, 1)[0]  # Linear trend
                    }
                }
        else:
            learning_analysis = {
                'steps': steps,
                'metrics': metrics,
                'note': 'Insufficient data for curve fitting'
            }
            
        return learning_analysis
        
    def _calculate_r_squared(self, y_true: np.ndarray, y_pred: np.ndarray) -> float:
        """Calculate R-squared value."""
        ss_res = np.sum((y_true - y_pred) ** 2)
        ss_tot = np.sum((y_true - np.mean(y_true)) ** 2)
        return 1 - (ss_res / ss_tot) if ss_tot != 0 else 0.0
        
    def _find_convergence_point(self, steps: List[int], metrics: List[float],
                              threshold: float = 0.01) -> Optional[int]:
        """Find convergence point where learning saturates."""
        if len(metrics) < 10:
            return None
            
        # Calculate rolling average
        window = min(10, len(metrics) // 10)
        if window < 2:
            return None
            
        rolling_avg = np.convolve(metrics, np.ones(window)/window, mode='valid')
        
        # Find point where change is below threshold
        for i in range(1, len(rolling_avg)):
            relative_change = abs(rolling_avg[i] - rolling_avg[i-1]) / rolling_avg[i-1] if rolling_avg[i-1] != 0 else 0
            if relative_change < threshold:
                return steps[i + window//2]  # Adjust index
                
        return None
        
    def analyze_learning_stages(self, recording_file: str) -> Dict[str, Any]:
        """
        Identify learning stages (exploration, exploitation, consolidation).
        
        Args:
            recording_file: File containing simulation recording
            
        Returns:
            Learning stage analysis
        """
        # Load recording data
        with open(recording_file, 'r') as f:
            data = json.load(f)
            
        stages = {
            'exploration': {'start_step': 0, 'end_step': 0, 'activity': 0},
            'exploitation': {'start_step': 0, 'end_step': 0, 'activity': 0},
            'consolidation': {'start_step': 0, 'end_step': 0, 'activity': 0}
        }
        
        # Simple stage detection based on activity patterns
        # This would need to be refined based on specific metrics
        total_activity = [len(frame['neuron_states']) for frame in data['recordings']]
        
        if len(total_activity) > 0:
            max_activity = max(total_activity)
            max_step = total_activity.index(max_activity)
            
            # Simple heuristic: early exploration, middle exploitation, later consolidation
            exploration_end = max(1, len(total_activity) // 3)
            consolidation_start = max(exploration_end + 1, 2 * len(total_activity) // 3)
            
            stages['exploration']['end_step'] = exploration_end
            stages['exploitation']['start_step'] = exploration_end
            stages['exploitation']['end_step'] = consolidation_start
            stages['consolidation']['start_step'] = consolidation_start
            
        return stages
class BehavioralPatternAnalyzer:
    """Analyzes behavioral patterns and actions."""
    
    def __init__(self, brain=None):
        """
        Initialize behavioral pattern analyzer.
        
        Args:
            brain: Reference to the brain object (optional)
        """
        self.brain = brain
        
    def analyze_action_patterns(self, recording_file: str) -> Dict[str, Any]:
        """
        Analyze action/behavioral patterns.
        
        Args:
            recording_file: File containing simulation recording
            
        Returns:
            Behavioral pattern analysis
        """
        # Load recording data
        with open(recording_file, 'r') as f:
            data = json.load(f)
            
        # Extract actions from recording
        actions = []
        for frame in data['recordings']:
            if 'motor_commands' in frame:
                for action in frame['motor_commands']:
                    actions.append(action)
                    
        analysis = {
            'total_actions': len(actions),
            'unique_actions': len(set(action.get('type', 'unknown') for action in actions)),
            'action_frequency': {},
            'sequence_patterns': [],
            'temporal_patterns': {}
        }
        
        # Analyze action frequency
        action_types = {}
        for action in actions:
            action_type = action.get('type', 'unknown')
            if action_type not in action_types:
                action_types[action_type] = []
            action_types[action_type].append(action)
            
        analysis['action_frequency'] = {
            action_type: len(actions_list) / len(actions) if actions else 0
            for action_type, actions_list in action_types.items()
        }
        
        return analysis
        
    def analyze_behavioral_sequences(self, recording_file: str,
                                    sequence_length: int = 3) -> Dict[str, Any]:
        """
        Analyze behavioral sequences and transitions.
        
        Args:
            recording_file: File containing simulation recording
            sequence_length: Length of sequences to analyze
            
        Returns:
            Sequence pattern analysis
        """
        # Load recording data
        with open(recording_file, 'r') as f:
            data = json.load(f)
            
        # Extract action sequences
        all_sequences = []
        for frame in data['recordings']:
            if 'motor_commands' in frame:
                for action in frame['motor_commands']:
                    all_sequences.append(action.get('type', 'unknown'))
                    
        # Find frequent sequences
        sequence_patterns = {}
        if len(all_sequences) >= sequence_length:
            for i in range(len(all_sequences) - sequence_length + 1):
                sequence = tuple(all_sequences[i:i + sequence_length])
                if sequence not in sequence_patterns:
                    sequence_patterns[sequence] = 0
                sequence_patterns[sequence] += 1
                
        analysis = {
            'all_sequences': all_sequences,
            'frequent_sequences': dict(sorted(sequence_patterns.items(), 
                                           key=lambda x: x[1], reverse=True)[:10]),
            'sequence_transition_matrix': self._build_transition_matrix(all_sequences),
            'pattern_statistics': self._analyze_sequence_statistics(all_sequences)
        }
        
        return analysis
        
    def _build_transition_matrix(self, sequences: List[str]) -> Dict[str, Dict[str, int]]:
        """Build transition matrix for sequences."""
        transitions = {}
        
        for i in range(len(sequences) - 1):
            current = sequences[i]
            next_action = sequences[i + 1]
            
            if current not in transitions:
                transitions[current] = {}
            if next_action not in transitions[current]:
                transitions[current][next_action] = 0
                
            transitions[current][next_action] += 1
            
        return transitions
        
    def _analyze_sequence_statistics(self, sequences: List[str]) -> Dict[str, Any]:
        """Analyze sequence statistics."""
        if not sequences:
            return {}
            
        unique_sequences = set(sequences)
        
        return {
            'total_sequences': len(sequences),
            'unique_sequences': len(unique_sequences),
            'sequence_repetition_rate': 1 - (len(unique_sequences) / len(sequences)) if sequences else 0,
            'most_frequent_sequence': max(set(tuple(x) for x in [sequences[i:i+3] for i in range(len(sequences)-2)]), 
                                        key=lambda x: [sequences[i:i+3].count(x) for i in range(len(sequences)-2)].count(x) if x in [sequences[i:i+3] for i in range(len(sequences)-2)] else 0)
        }
        
    def detect_behavioral_regressions(self, recording_file: str) -> Dict[str, Any]:
        """
        Detect behavioral regressions or performance declines.
        
        Args:
            recording_file: File containing simulation recording
            
        Returns:
            Regression detection results
        """
        # Load recording data
        with open(recording_file, 'r') as f:
            data = json.load(f)
            
        # Analyze performance metrics over time
        metrics = []
        for frame in data['recordings']:
            # Extract performance metric (placeholder - would use actual metrics)
            metric = len(frame.get('motor_commands', []))
            metrics.append(metric)
            
        # Detect significant drops
        regressions = []
        if len(metrics) > 10:
            # Calculate rolling average
            window = min(5, len(metrics) // 5)
            if window >= 2:
                rolling_avg = np.convolve(metrics, np.ones(window)/window, mode='valid')
                
                # Detect significant drops
                for i in range(1, len(rolling_avg)):
                    drop = rolling_avg[i-1] - rolling_avg[i]
                    if drop > 0.2 * rolling_avg[i-1]:  # 20% drop threshold
                        regressions.append({
                            'step': i + window//2,
                            'drop_amount': drop,
                            'relative_drop': drop / rolling_avg[i-1] if rolling_avg[i-1] != 0 else 0
                        })
                        
        analysis = {
            'metrics': metrics,
            'regressions_detected': len(regressions),
            'regression_details': regressions,
            'stability_score': self._calculate_stability_score(metrics) if metrics else 0
        }
        
        return analysis
        
    def _calculate_stability_score(self, metrics: List[float]) -> float:
        """Calculate stability score based on metric variance."""
        if len(metrics) < 5:
            return 1.0  # Default stable score
            
        # Calculate coefficient of variation
        mean_metric = np.mean(metrics)
        std_metric = np.std(metrics)
        
        if mean_metric == 0:
            return 0.0
            
        cv = std_metric / mean_metric
        
        # Lower CV means more stable
        stability_score = max(0.0, 1.0 - cv)
        return stability_score
class AnalysisTools:
    """
    Advanced analysis tools for NLM neural simulations.
    
    Features:
    - Network topology analysis
    - Plasticity rule effectiveness analysis
    - Learning curve analysis
    - Behavioral pattern analysis
    """
    
    def __init__(self, brain=None):
        """
        Initialize analysis tools.
        
        Args:
            brain: Reference to the brain object (optional)
        """
        self.brain = brain
        self.topology_analyzer = NetworkTopologyAnalyzer(brain)
        self.plasticity_analyzer = PlasticityAnalyzer(brain)
        self.learning_curve_analyzer = LearningCurveAnalyzer(brain)
        self.behavioral_pattern_analyzer = BehavioralPatternAnalyzer(brain)
        
    def run_complete_analysis(self, recording_file: str) -> Dict[str, Any]:
        """
        Run complete analysis suite on recording.
        
        Args:
            recording_file: File containing simulation recording
            
        Returns:
            Complete analysis results
        """
        analysis_results = {
            'timestamp': datetime.now().isoformat(),
            'network_topology': {},
            'plasticity_analysis': {},
            'learning_curve': {},
            'behavioral_patterns': {}
        }
        
        try:
            # Network topology analysis
            analysis_results['network_topology'] = (
                self.topology_analyzer.analyze_connectivity_matrix()
            )
            
        except Exception as e:
            analysis_results['network_topology'] = {'error': str(e)}
            
        try:
            # Plasticity analysis (requires recording file)
            analysis_results['plasticity_analysis'] = (
                self.plasticity_analyzer.analyze_stdp_dynamics(recording_file)
            )
            
        except Exception as e:
            analysis_results['plasticity_analysis'] = {'error': str(e)}
            
        try:
            # Learning curve analysis
            analysis_results['learning_curve'] = (
                self.learning_curve_analyzer.analyze_performance_learning(recording_file)
            )
            
        except Exception as e:
            analysis_results['learning_curve'] = {'error': str(e)}
            
        try:
            # Behavioral pattern analysis
            analysis_results['behavioral_patterns'] = (
                self.behavioral_pattern_analyzer.analyze_action_patterns(recording_file)
            )
            
        except Exception as e:
            analysis_results['behavioral_patterns'] = {'error': str(e)}
            
        return analysis_results
        
    def export_analysis_results(self, results: Dict[str, Any], output_file: str,
                               format: str = 'json') -> str:
        """
        Export analysis results to file.
        
        Args:
            results: Analysis results dictionary
            output_file: Output file path
            format: Export format ('json', 'csv', 'hdf5')
            
        Returns:
            Path to output file
        """
        # Add timestamp to results
        results['exported_at'] = datetime.now().isoformat()
        
        if format == 'json':
            with open(output_file, 'w') as f:
                json.dump(results, f, indent=2)
        elif format == 'csv':
            # Flatten results for CSV export
            flat_results = self._flatten_results(results)
            df = pd.DataFrame(flat_results)
            df.to_csv(output_file, index=False)
        elif format == 'hdf5':
            with h5py.File(output_file, 'w') as f:
                # Store results in HDF5
                self._store_results_in_hdf5(f, results)
                
        return output_file
        
    def _flatten_results(self, results: Dict[str, Any], prefix: str = '') -> List[Dict[str, Any]]:
        """Flatten nested results for CSV export."""
        flat = []
        
        for key, value in results.items():
            current_key = f"{prefix}_{key}" if prefix else key
            
            if isinstance(value, dict):
                flat.extend(self._flatten_results(value, current_key))
            elif isinstance(value, list):
                for i, item in enumerate(value):
                    if isinstance(item, dict):
                        flat.extend(self._flatten_results(item, f"{current_key}_{i}"))
                    else:
                        flat.append({current_key: item})
            else:
                flat.append({current_key: value})
                
        return flat
        
    def _store_results_in_hdf5(self, hdf5_file, results: Dict[str, Any], group_name: str = ''):
        """Store results in HDF5 file."""
        for key, value in results.items():
            current_path = f"{group_name}/{key}" if group_name else key
            
            if isinstance(value, dict):
                self._store_results_in_hdf5(hdf5_file, value, current_path)
            elif isinstance(value, list):
                # Try to store as dataset
                try:
                    hdf5_file.create_dataset(current_path, data=np.array(value))
                except:
                    # Store as string if can't convert to array
                    hdf5_file.attrs[current_path] = json.dumps(value)
            else:
                hdf5_file.attrs[current_path] = value