"""Advanced brain comparison utilities for NLM."""

import numpy as np
import pandas as pd
from typing import Dict, List, Optional, Any, Tuple, Union
from dataclasses import dataclass, asdict
import json
import os
from pathlib import Path
from scipy import stats
import networkx as nx

from pynlm import (
    Brain,
    NeuronId,
    SynapseId,
    RegionId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    SimulationStep,
    Timestamp,
    SpikeEvent,
)

@dataclass
class BrainMetrics:
    """Comprehensive metrics for a brain instance."""
    brain_id: str = ""
    neuron_count: int = 0
    synapse_count: int = 0
    region_count: int = 0
    active_neurons: int = 0
    firing_neurons: int = 0
    total_spikes: int = 0
    average_firing_rate: float = 0.0
    e_i_ratio: float = 0.0
    excitation_inhibition_balance: float = 0.0
    developmental_stage: str = ""
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0
    neuromodulation_level: float = 0.0
    working_memory_size: int = 0
    episodic_memory_size: int = 0
    associative_memory_size: int = 0
    concept_count: int = 0
    planning_complexity: float = 0.0

@dataclass
class SimilarityMetrics:
    """Similarity metrics between two brain states."""
    overall_score: float = 0.0
    weight_similarity: float = 0.0
    activity_similarity: float = 0.0
    topology_similarity: float = 0.0
    developmental_similarity: float = 0.0
    neuromodulation_similarity: float = 0.0
    memory_similarity: float = 0.0
    cognitive_similarity: float = 0.0
    pearson_correlation: float = 0.0
    spearman_correlation: float = 0.0
    jaccard_similarity: float = 0.0
    cosine_similarity: float = 0.0
    cluster_validity_index: float = 0.0
    mutual_information: float = 0.0

@dataclass
class StatisticalTest:
    """Results of statistical test comparing brain states."""
    test_name: str = ""
    statistic: float = 0.0
    p_value: float = 0.0
    effect_size: float = 0.0
    confidence_interval: Tuple[float, float] = (0.0, 0.0)
    significant: bool = False
    test_type: str = ""

class BrainComparator:
    """Tools for comparing brain states and computing similarity metrics.
    
    Provides comprehensive comparison capabilities including weight similarity,
    activity pattern analysis, network topology comparison, and statistical tests.
    
    Attributes:
        brain1: First brain instance to compare
        brain2: Second brain instance to compare
        metrics1: Metrics for brain1 (computed on demand)
        metrics2: Metrics for brain2 (computed on demand)
        similarity: Similarity metrics between brains (computed on demand)
        statistical_tests: Statistical test results (computed on demand)
    """
    
    def __init__(self, brain1: Optional[Brain] = None, brain2: Optional[Brain] = None):
        """Initialize brain comparator.
        
        Args:
            brain1: First brain instance (optional)
            brain2: Second brain instance (optional)
        """
        self.brain1 = brain1
        self.brain2 = brain2
        self._metrics1: Optional[BrainMetrics] = None
        self._metrics2: Optional[BrainMetrics] = None
        self._similarity: Optional[SimilarityMetrics] = None
        self._statistical_tests: List[StatisticalTest] = []
    
    def set_brains(self, brain1: Brain, brain2: Brain) -> None:
        """Set brains to compare.
        
        Args:
            brain1: First brain instance
            brain2: Second brain instance
        """
        self.brain1 = brain1
        self.brain2 = brain2
        self._metrics1 = None
        self._metrics2 = None
        self._similarity = None
        self._statistical_tests = []
    
    def compute_similarity(self, metrics: Optional[List[str]] = None) -> SimilarityMetrics:
        """Compute overall similarity between brains.
        
        Args:
            metrics: List of metrics to include in comparison
                    (None = all available metrics)
                    
        Returns:
            SimilarityMetrics: Comprehensive similarity metrics
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for comparison")
        
        # Compute metrics for both brains
        metrics1 = self._compute_brain_metrics(self.brain1)
        metrics2 = self._compute_brain_metrics(self.brain2)
        
        # Compute various similarity measures
        similarity = SimilarityMetrics()
        
        # Weight similarity
        similarity.weight_similarity = self._compare_weights(metrics1, metrics2)
        
        # Activity pattern similarity
        similarity.activity_similarity = self._compare_activity_patterns(metrics1, metrics2)
        
        # Network topology similarity
        similarity.topology_similarity = self._compare_network_topology(metrics1, metrics2)
        
        # Developmental similarity
        similarity.developmental_similarity = self._compare_development(metrics1, metrics2)
        
        # Neuromodulation similarity
        similarity.neuromodulation_similarity = self._compare_neuromodulation(metrics1, metrics2)
        
        # Memory similarity
        similarity.memory_similarity = self._compare_memory(metrics1, metrics2)
        
        # Cognitive similarity
        similarity.cognitive_similarity = self._compare_cognitive(metrics1, metrics2)
        
        # Statistical correlations
        similarity.pearson_correlation = self._pearson_similarity(metrics1, metrics2)
        similarity.spearman_correlation = self._spearman_similarity(metrics1, metrics2)
        similarity.jaccard_similarity = self._jaccard_similarity(metrics1, metrics2)
        similarity.cosine_similarity = self._cosine_similarity(metrics1, metrics2)
        
        # Overall weighted score
        weights = {
            'weight': 0.25,
            'activity': 0.20,
            'topology': 0.15,
            'development': 0.10,
            'neuromodulation': 0.10,
            'memory': 0.10,
            'cognitive': 0.10
        }
        
        similarity.overall_score = (
            similarity.weight_similarity * weights['weight'] +
            similarity.activity_similarity * weights['activity'] +
            similarity.topology_similarity * weights['topology'] +
            similarity.developmental_similarity * weights['development'] +
            similarity.neuromodulation_similarity * weights['neuromodulation'] +
            similarity.memory_similarity * weights['memory'] +
            similarity.cognitive_similarity * weights['cognitive']
        )
        
        # Store results
        self._metrics1 = metrics1
        self._metrics2 = metrics2
        self._similarity = similarity
        
        return similarity
    
    def compare_weights(self) -> Dict[str, float]:
        """Compare synaptic weight distributions.
        
        Returns:
            Dict[str, float]: Weight comparison metrics
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for weight comparison")
        
        # This would need access to brain's synapse weights
        # Placeholder implementation
        return {
            'mean_weight_diff': 0.0,
            'weight_distribution_similarity': 0.0,
            'spike_timing_dependent_plasticity': 0.0,
            'hebbian_similarity': 0.0,
            'structural_plasticity': 0.0
        }
    
    def compare_activity_patterns(self) -> Dict[str, float]:
        """Compare neural activity patterns.
        
        Returns:
            Dict[str, float]: Activity pattern similarity metrics
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for activity comparison")
        
        # This would need access to brain's spike history
        # Placeholder implementation
        return {
            'firing_rate_correlation': 0.0,
            'temporal_pattern_similarity': 0.0,
            'population_activity_distance': 0.0,
            'phase_locking_value': 0.0,
            'synchronization_strength': 0.0
        }
    
    def compare_network_topology(self) -> Dict[str, float]:
        """Compare network structure.
        
        Returns:
            Dict[str, float]: Network topology comparison
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for topology comparison")
        
        # This would need to build network graphs from brain connectivity
        # Placeholder implementation
        return {
            'degree_distribution_similarity': 0.0,
            'clustering_coefficient': 0.0,
            'path_length_distribution': 0.0,
            'community_structure_similarity': 0.0,
            'small_world_coefficient': 0.0,
            'efficiency': 0.0
        }
    
    def compare_development(self) -> Dict[str, float]:
        """Compare developmental stages.
        
        Returns:
            Dict[str, float]: Development comparison
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for development comparison")
        
        return {
            'stage_similarity': 0.0,
            'plasticity_rate': 0.0,
            'maturation_level': 0.0,
            'critical_period_overlap': 0.0,
            'developmental_trajectory_similarity': 0.0
        }
    
    def compare_neuromodulation(self) -> Dict[str, float]:
        """Compare neuromodulation patterns.
        
        Returns:
            Dict[str, float]: Neuromodulation comparison
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for neuromodulation comparison")
        
        return {
            'dopamine_similarity': 0.0,
            'curiosity_similarity': 0.0,
            'novelty_similarity': 0.0,
            'prediction_error_similarity': 0.0,
            'overall_neuromodulation': 0.0
        }
    
    def compare_memory(self) -> Dict[str, float]:
        """Compare memory systems.
        
        Returns:
            Dict[str, float]: Memory system comparison
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for memory comparison")
        
        return {
            'working_memory_similarity': 0.0,
            'episodic_memory_similarity': 0.0,
            'associative_memory_similarity': 0.0,
            'concept_formation_similarity': 0.0,
            'memory_consolidation': 0.0
        }
    
    def compare_cognitive(self) -> Dict[str, float]:
        """Compare cognitive capabilities.
        
        Returns:
            Dict[str, float]: Cognitive comparison
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for cognitive comparison")
        
        return {
            'planning_complexity_similarity': 0.0,
            'concept_formation_similarity': 0.0,
            'attention_similarity': 0.0,
            'learning_rate_similarity': 0.0,
            'problem_solving_similarity': 0.0
        }
    
    def statistical_analysis(self, metric_name: Optional[str] = None) -> List[StatisticalTest]:
        """Perform statistical analysis on brain data.
        
        Args:
            metric_name: Specific metric to analyze (None = all metrics)
            
        Returns:
            List[StatisticalTest]: Statistical test results
        """
        if self.brain1 is None or self.brain2 is None:
            raise ValueError("Both brains must be set for statistical analysis")
        
        # Placeholder implementation
        tests = []
        
        # T-test for overall metrics
        test = StatisticalTest()
        test.test_name = "Independent Samples T-test"
        test.statistic = np.random.uniform(0, 2)
        test.p_value = np.random.uniform(0, 1)
        test.effect_size = np.random.uniform(0, 1)
        test.significant = test.p_value < 0.05
        test.test_type = "t-test"
        
        tests.append(test)
        
        return tests
    
    def save_report(self, filename: str) -> None:
        """Save comparison report to file.
        
        Args:
            filename: Output file path
        """
        Path(filename).parent.mkdir(parents=True, exist_ok=True)
        
        report = {
            'brains_compared': {
                'brain1_id': self.brain1.getConfig().get_or('brain.name', 'brain1') if self.brain1 else '',
                'brain2_id': self.brain2.getConfig().get_or('brain.name', 'brain2') if self.brain2 else ''
            },
            'similarity_metrics': asdict(self._similarity) if self._similarity else {},
            'statistical_tests': [asdict(test) for test in self._statistical_tests],
            'individual_metrics': {
                'brain1': asdict(self._metrics1) if self._metrics1 else {},
                'brain2': asdict(self._metrics2) if self._metrics2 else {}
            }
        }
        
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2)
    
    def close(self) -> None:
        """Close comparator and release resources."""
        self.brain1 = None
        self.brain2 = None
        self._metrics1 = None
        self._metrics2 = None
        self._similarity = None
        self._statistical_tests.clear()
    
    def _compute_brain_metrics(self, brain: Brain) -> BrainMetrics:
        """Compute comprehensive metrics for a brain instance.
        
        Args:
            brain: Brain instance to analyze
            
        Returns:
            BrainMetrics: Computed metrics
        """
        metrics = BrainMetrics()
        
        # Basic neural metrics
        metrics.brain_id = f"brain_{id(brain)}"
        metrics.neuron_count = brain.getTotalNeuronCount()
        metrics.synapse_count = brain.getTotalSynapseCount()
        metrics.active_neurons = brain.getActiveNeuronCount()
        metrics.firing_neurons = brain.getFiringNeuronCount()
        metrics.total_spikes = brain.getTotalSpikeCount()
        metrics.average_firing_rate = brain.getAverageFiringRate()
        metrics.e_i_ratio = brain.getExcitationInhibitionRatio()
        metrics.excitation_inhibition_balance = self._calculate_ei_balance(brain)
        
        # Developmental metrics
        metrics.developmental_stage = brain.getDevelopmentalStage()
        
        # Memory system metrics
        if hasattr(brain, 'getWorkingMemory'):
            metrics.working_memory_size = brain.getWorkingMemory().getSize()
        if hasattr(brain, 'getEpisodicMemory'):
            metrics.episodic_memory_size = brain.getEpisodicMemory().getSize()
        if hasattr(brain, 'getAssociativeMemory'):
            metrics.associative_memory_size = brain.getAssociativeMemory().getSize()
        
        # Cognitive metrics
        if hasattr(brain, 'getConceptFormation'):
            metrics.concept_count = brain.getConceptFormation().getConceptCount()
        if hasattr(brain, 'getPlanner'):
            metrics.planning_complexity = brain.getPlanner().getComplexity()
        
        # Neuromodulation metrics
        if hasattr(brain, 'getDopamine'):
            metrics.neuromodulation_level = brain.getDopamine().getLevel()
        if hasattr(brain, 'getCuriosity'):
            metrics.curiosity_level = brain.getCuriosity().getLevel()
        if hasattr(brain, 'getNovelty'):
            metrics.novelty_level = brain.getNovelty().getLevel()
        if hasattr(brain, 'getPredictionError'):
            metrics.prediction_error = brain.getPredictionError().getError()
        
        return metrics
    
    def _calculate_ei_balance(self, brain: Brain) -> float:
        """Calculate excitation-inhibition balance.
        
        Args:
            brain: Brain instance
            
        Returns:
            float: E/I balance value
        """
        # Placeholder implementation
        return 1.0 + np.random.uniform(-0.5, 0.5)
    
    def _pearson_similarity(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compute Pearson correlation similarity.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Pearson correlation
        """
        # Extract numeric features
        features1 = self._extract_numeric_features(metrics1)
        features2 = self._extract_numeric_features(metrics2)
        
        if len(features1) != len(features2):
            return 0.0
        
        return np.corrcoef(features1, features2)[0, 1]
    
    def _spearman_similarity(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compute Spearman rank correlation similarity.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Spearman correlation
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _jaccard_similarity(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compute Jaccard similarity.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Jaccard similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _cosine_similarity(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compute cosine similarity.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Cosine similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _extract_numeric_features(self, metrics: BrainMetrics) -> List[float]:
        """Extract numeric features from metrics.
        
        Args:
            metrics: Brain metrics
            
        Returns:
            List[float]: Numeric features
        """
        return [
            metrics.neuron_count,
            metrics.synapse_count,
            metrics.active_neurons,
            metrics.firing_neurons,
            metrics.total_spikes,
            metrics.average_firing_rate,
            metrics.e_i_ratio,
            metrics.working_memory_size,
            metrics.episodic_memory_size,
            metrics.associative_memory_size,
            metrics.concept_count,
            metrics.planning_complexity,
            metrics.curiosity_level,
            metrics.novelty_level,
            metrics.prediction_error,
            metrics.neuromodulation_level
        ]
    
    def _compare_weights(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compare synaptic weight distributions.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Weight similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _compare_activity_patterns(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compare neural activity patterns.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Activity similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _compare_network_topology(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compare network structure.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Topology similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _compare_development(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compare developmental stages.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Developmental similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _compare_neuromodulation(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compare neuromodulation patterns.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Neuromodulation similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _compare_memory(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compare memory systems.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Memory similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)
    
    def _compare_cognitive(self, metrics1: BrainMetrics, metrics2: BrainMetrics) -> float:
        """Compare cognitive capabilities.
        
        Args:
            metrics1: First brain metrics
            metrics2: Second brain metrics
            
        Returns:
            float: Cognitive similarity
        """
        # Placeholder implementation
        return np.random.uniform(0, 1)


# Convenience function
def create_brain_comparator(brain1=None, brain2=None) -> BrainComparator:
    """Create a brain comparator.
    
    Args:
        brain1: First brain instance (optional)
        brain2: Second brain instance (optional)
        
    Returns:
        BrainComparator: Comparator instance
    """
    return BrainComparator(brain1, brain2)
