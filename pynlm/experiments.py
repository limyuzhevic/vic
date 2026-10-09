"""
Enhanced Python interface for NLM experiments and utilities
"""

from typing import List, Dict, Any, Optional, Tuple, Callable
from enum import Enum
from .exceptions import NLMError

class ExperimentType(Enum):
    """Experiment type enumeration"""
    PHASE3 = "phase3"
    PHASE4 = "phase4"
    PHASE5 = "phase5"
    PHASE6 = "phase6"
    INTEGRATED = "integrated"
    BENCHMARK = "benchmark"

class Experiment:
    """Experiment framework for NLM"""
    
    def __init__(self, name: str, exp_type: ExperimentType = ExperimentType.PHASE3):
        self.name = name
        self.type = exp_type
        self.description = ""
        self.objective = ""
        self.setup_function = None
        self.run_function = None
        self.teardown_function = None
        self.results = {}
        self.metrics = {}
        self.metadata = {}
        
    def set_description(self, description: str) -> None:
        """Set experiment description"""
        self.description = description
        
    def set_objective(self, objective: str) -> None:
        """Set experiment objective"""
        self.objective = objective
        
    def set_setup_function(self, func: Callable) -> None:
        """Set setup function"""
        self.setup_function = func
        
    def set_run_function(self, func: Callable) -> None:
        """Set run function"""
        self.run_function = func
        
    def set_teardown_function(self, func: Callable) -> None:
        """Set teardown function"""
        self.teardown_function = func
        
    def add_metadata(self, key: str, value: Any) -> None:
        """Add metadata"""
        self.metadata[key] = value
        
    def run(self) -> Dict[str, Any]:
        """Run experiment"""
        try:
            # Setup
            if self.setup_function:
                self.setup_function()
                
            # Run experiment
            if self.run_function:
                self.results = self.run_function()
                
            # Record metrics
            self._record_metrics()
            
            return self.results
            
        finally:
            # Teardown
            if self.teardown_function:
                self.teardown_function()
                
    def _record_metrics(self) -> None:
        """Record experiment metrics"""
        # Simple metrics recording
        self.metrics = {
            'name': self.name,
            'type': self.type.value,
            'description': self.description,
            'objective': self.objective,
            'metadata': self.metadata,
            'timestamp': None  # Would be filled with actual time
        }
        
    def get_results(self) -> Dict[str, Any]:
        """Get experiment results"""
        return self.results.copy()
        
    def get_metrics(self) -> Dict[str, Any]:
        """Get experiment metrics"""
        return self.metrics.copy()
        
    def get_metadata(self) -> Dict[str, Any]:
        """Get experiment metadata"""
        return self.metadata.copy()
        
    def __repr__(self):
        return f"<Experiment: {self.name} ({self.type.value})>"

class ExperimentRunner:
    """Experiment runner for multiple experiments"""
    
    def __init__(self):
        self.experiments = []
        self.results = {}
        
    def add_experiment(self, experiment: Experiment) -> None:
        """Add experiment"""
        self.experiments.append(experiment)
        
    def run_all(self) -> Dict[str, Any]:
        """Run all experiments"""
        for experiment in self.experiments:
            try:
                result = experiment.run()
                self.results[experiment.name] = {
                    'success': True,
                    'result': result,
                    'experiment': experiment
                }
            except Exception as e:
                self.results[experiment.name] = {
                    'success': False,
                    'error': str(e),
                    'experiment': experiment
                }
                
        return self.results
        
    def run_experiment(self, name: str) -> Dict[str, Any]:
        """Run specific experiment by name"""
        for experiment in self.experiments:
            if experiment.name == name:
                return self.run_experiment(experiment)
        raise KeyError(f"Experiment not found: {name}")
        
    def run_experiment(self, experiment: Experiment) -> Dict[str, Any]:
        """Run specific experiment"""
        try:
            result = experiment.run()
            self.results[experiment.name] = {
                'success': True,
                'result': result,
                'experiment': experiment
            }
            return self.results[experiment.name]
        except Exception as e:
            self.results[experiment.name] = {
                'success': False,
                'error': str(e),
                'experiment': experiment
            }
            raise
            
    def get_results(self) -> Dict[str, Any]:
        """Get all results"""
        return self.results.copy()
        
    def get_successful_experiments(self) -> List[str]:
        """Get names of successful experiments"""
        return [name for name, result in self.results.items() 
                if result.get('success', False)]
        
    def get_failed_experiments(self) -> List[str]:
        """Get names of failed experiments"""
        return [name for name, result in self.results.items() 
                if not result.get('success', True)]
        
    def clear(self) -> None:
        """Clear all results"""
        self.results.clear()
        
    def __repr__(self):
        return f"<ExperimentRunner: {len(self.experiments)} experiments, {len(self.results)} results>"

class Metrics:
    """Metrics collection and analysis"""
    
    def __init__(self):
        self.records = []
        self.aggregates = {}
        
    def record(self, name: str, value: float, timestamp: Optional[float] = None,
               tags: Optional[Dict[str, str]] = None) -> None:
        """Record metric value"""
        record = {
            'name': name,
            'value': value,
            'timestamp': timestamp,
            'tags': tags or {}
        }
        self.records.append(record)
        
    def get_records(self, name: Optional[str] = None, 
                    tags: Optional[Dict[str, str]] = None) -> List[Dict[str, Any]]:
        """Get metric records"""
        filtered = self.records
        if name:
            filtered = [r for r in filtered if r['name'] == name]
        if tags:
            filtered = [r for r in filtered 
                       if all(r['tags'].get(k) == v for k, v in tags.items())]
        return filtered.copy()
        
    def get_values(self, name: Optional[str] = None) -> List[float]:
        """Get metric values"""
        return [r['value'] for r in self.get_records(name)]
        
    def get_mean(self, name: str) -> float:
        """Get mean value for metric"""
        values = self.get_values(name)
        return sum(values) / len(values) if values else 0.0
        
    def get_std(self, name: str) -> float:
        """Get standard deviation for metric"""
        values = self.get_values(name)
        if len(values) <= 1:
            return 0.0
        mean = self.get_mean(name)
        variance = sum((v - mean) ** 2 for v in values) / (len(values) - 1)
        return variance ** 0.5
        
    def get_min(self, name: str) -> float:
        """Get minimum value for metric"""
        values = self.get_values(name)
        return min(values) if values else 0.0
        
    def get_max(self, name: str) -> float:
        """Get maximum value for metric"""
        values = self.get_values(name)
        return max(values) if values else 0.0
        
    def clear(self) -> None:
        """Clear all records"""
        self.records.clear()
        self.aggregates.clear()
        
    def __repr__(self):
        return f"<Metrics: {len(self.records)} records>"
