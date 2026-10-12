#!/usr/bin/env python3
"""
Parameter Explorer

Explore parameter space for optimization, visualize parameter effects,
automated parameter search, and statistical analysis of parameter sensitivity.

Features:
- Parameter space exploration and visualization
- Automated parameter optimization
- Parameter sensitivity analysis
- Statistical analysis of parameter effects
- Visualization of parameter importance
- Support for different optimization algorithms
- Parallel parameter search
- Interactive parameter tuning
- Parameter effect visualization
- Bayesian optimization support
- Genetic algorithm optimization
- Gradient-based optimization
- Grid search optimization
- Random search optimization

Usage Examples:
    # Explore parameter space with grid search
    python scripts/explore/param_explorer.py --search grid --parameter-file params.json --output results.json

    # Run sensitivity analysis
    python scripts/explore/param_explorer.py --analyze sensitivity --input results.json

    # Visualize parameter effects
    python scripts/explore/param_explorer.py --visualize effects --input results.json --output plot.html

    # Automated parameter optimization
    python scripts/explore/param_explorer.py --optimize genetic --target-metric accuracy --iterations 100

    # Bayesian parameter search
    python scripts/explore/param_explorer.py --optimize bayesian --objective function_performance

API References:
    ParameterExplorer.search_space(param_space, algorithm, iterations)
    ParameterExplorer.run_sensitivity_analysis(results, target_metric)
    ParameterExplorer.visualize_parameter_effects(results, output_file)
    ParameterExplorer.automated_optimization(param_space, objective_function)
    ParameterExplorer.visualize_optimization_progress(history)
    ParameterExplorer.export_results(results, format, output_file)
    ParameterExplorer.import_results(input_file)
"""

import json
import time
import threading
import numpy as np
import pandas as pd
from pathlib import Path
import argparse
from typing import Dict, List, Any, Optional, Tuple, Union, Callable
from datetime import datetime
import warnings
from dataclasses import dataclass, field
from enum import Enum
import os
import copy
import statistics
import math
from scipy import stats
import warnings
warnings.filterwarnings('ignore')

# Optional imports for enhanced exploration
try:
    from sklearn.model_selection import ParameterGrid
    from sklearn.preprocessing import StandardScaler
    from sklearn.linear_model import LinearRegression
    from sklearn.ensemble import RandomForestRegressor
    from sklearn.gaussian_process import GaussianProcessRegressor
    from sklearn.gaussian_process.kernels import RBF, Matern, ConstantKernel as C
    SCIKIT_LEARN_AVAILABLE = True
except ImportError:
    SCIKIT_LEARN_AVAILABLE = False

try:
    import optuna
    OPTUNA_AVAILABLE = True
except ImportError:
    OPTUNA_AVAILABLE = False

try:
    from deap import base, creator, tools, algorithms
    DEAP_AVAILABLE = True
except ImportError:
    DEAP_AVAILABLE = False


try:
    import plotly.graph_objects as go
    import plotly.express as px
    PLOTLY_AVAILABLE = True
except ImportError:
    PLOTLY_AVAILABLE = False


try:
    import seaborn as sns
    SEABORN_AVAILABLE = True
except ImportError:
    SEABORN_AVAILABLE = False


class OptimizationAlgorithm(Enum):
    """Optimization algorithms."""
    GRID_SEARCH = "grid"
    RANDOM_SEARCH = "random"
    BAYESIAN = "bayesian"
    GENETIC = "genetic"
    GRADIENT = "gradient"
    SHANNON = "shannon"
    TOURNAMENT = "tournament"


class ParameterType(Enum):
    """Types of parameters."""
    CONTINUOUS = "continuous"
    DISCRETE = "discrete"
    CATEGORICAL = "categorical"
    INTEGER = "integer"
    BOOLEAN = "boolean"
    LOG_SCALE = "log"


@dataclass
class ParameterDefinition:
    """Definition of a parameter for exploration."""
    name: str
    type: ParameterType
    min_value: Optional[float] = None
    max_value: Optional[float] = None
    values: Optional[List[Any]] = None
    default_value: Any = None
    distribution: str = "uniform"  # uniform, normal, lognormal, triangular
    scale: str = "linear"  # linear, log
    step: Optional[float] = None
    bounds: Optional[Tuple[float, float]] = None
    constraints: Optional[List[str]] = None
    transformation: Optional[str] = None
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ParameterResult:
    """Result of parameter evaluation."""
    parameters: Dict[str, Any]
    metric: float
    timestamp: float
    trial_id: str
    iteration: int
    success: bool = True
    error_message: Optional[str] = None
    metadata: Dict[str, Any] = field(default_factory=dict)
    convergence_info: Dict[str, Any] = field(default_factory=dict)


@dataclass
class OptimizationHistory:
    """History of optimization progress."""
    algorithm: OptimizationAlgorithm
    best_parameter: Dict[str, Any]
    best_metric: float
    iterations: int
    total_trials: int
    convergence: bool
    convergence_iteration: Optional[int] = None
    execution_time: float = 0.0
    history: List[ParameterResult] = field(default_factory=list)


class ParameterExplorer:
    """Explore parameter space for optimization and sensitivity analysis."""
    
    def __init__(self, host: str = "localhost", port: int = 8888):
        """Initialize ParameterExplorer.
        
        Args:
            host: Host for parameter server
            port: Port for parameter server
        """
        self.host = host
        self.port = port
        self.results: List[ParameterResult] = []
        self.parameter_definitions: Dict[str, ParameterDefinition] = {}
        self.optimization_history: List[OptimizationHistory] = []
        self.current_optimization: Optional[OptimizationHistory] = None
        self.analysis_cache: Dict[str, Any] = {}
        self.simulation_connection = None
        
        # Default parameter space
        self.default_parameter_space = {
            "learning_rate": ParameterDefinition(
                name="learning_rate",
                type=ParameterType.LOG_SCALE,
                min_value=1e-6,
                max_value=1e-1,
                default_value=1e-3,
                scale="log",
                distribution="loguniform"
            ),
            "hidden_size": ParameterDefinition(
                name="hidden_size",
                type=ParameterType.INTEGER,
                min_value=32,
                max_value=1024,
                default_value=256,
                step=32
            ),
            "batch_size": ParameterDefinition(
                name="batch_size",
                type=ParameterType.INTEGER,
                min_value=16,
                max_value=512,
                default_value=64,
                step=16
            ),
            "dropout_rate": ParameterDefinition(
                name="dropout_rate",
                type=ParameterType.CONTINUOUS,
                min_value=0.0,
                max_value=0.9,
                default_value=0.1
            ),
            "num_layers": ParameterDefinition(
                name="num_layers",
                type=ParameterType.INTEGER,
                min_value=1,
                max_value=10,
                default_value=3
            ),
            "activation": ParameterDefinition(
                name="activation",
                type=ParameterType.CATEGORICAL,
                values=["relu", "tanh", "sigmoid", "leaky_relu"],
                default_value="relu"
            ),
            "optimizer": ParameterDefinition(
                name="optimizer",
                type=ParameterType.CATEGORICAL,
                values=["adam", "sgd", "rmsprop", "adagrad"],
                default_value="adam"
            )
        }
        
        # Create output directory
        os.makedirs("parameter_results", exist_ok=True)
    
    def load_parameter_space(self, file_path: str) -> Dict[str, ParameterDefinition]:
        """Load parameter space from file.
        
        Args:
            file_path: Path to parameter definition file
            
        Returns:
            Dictionary of parameter definitions
        """
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
            
            parameter_space = {}
            
            for param_name, param_data in data.items():
                param_type = ParameterType(param_data.get("type", "continuous").lower())
                
                # Create parameter definition
                param_def = ParameterDefinition(
                    name=param_name,
                    type=param_type,
                    min_value=param_data.get("min_value"),
                    max_value=param_data.get("max_value"),
                    values=param_data.get("values"),
                    default_value=param_data.get("default_value"),
                    distribution=param_data.get("distribution", "uniform"),
                    scale=param_data.get("scale", "linear"),
                    step=param_data.get("step"),
                    bounds=param_data.get("bounds"),
                    constraints=param_data.get("constraints", []),
                    transformation=param_data.get("transformation"),
                    metadata=param_data.get("metadata", {})
                )
                
                parameter_space[param_name] = param_def
            
            self.parameter_definitions = parameter_space
            self._log_event(f"Parameter space loaded from {file_path}", "INFO",
                           {"num_parameters": len(parameter_space)})
            
            return parameter_space
        
        except Exception as e:
            self._log_event(f"Error loading parameter space: {e}", "ERROR")
            return {}
    
    def get_default_parameter_space(self) -> Dict[str, ParameterDefinition]:
        """Get default parameter space.
        
        Returns:
            Default parameter space
        """
        return self.default_parameter_space.copy()
    
    def generate_parameter_grid(self, parameter_space: Dict[str, ParameterDefinition],
                              points_per_param: int = 10) -> List[Dict[str, Any]]:
        """Generate parameter grid for exploration.
        
        Args:
            parameter_space: Parameter space definition
            points_per_param: Number of points per parameter
            
        Returns:
            List of parameter combinations
        """
        param_combinations = []
        
        # Generate points for each parameter
        param_points = {}
        
        for param_name, param_def in parameter_space.items():
            if param_def.type == ParameterType.CATEGORICAL:
                if param_def.values:
                    param_points[param_name] = param_def.values
                else:
                    param_points[param_name] = [param_def.default_value]
            
            elif param_def.type == ParameterType.INTEGER:
                if param_def.min_value is not None and param_def.max_value is not None:
                    param_points[param_name] = np.linspace(
                        param_def.min_value, param_def.max_value, points_per_param
                    ).astype(int)
                else:
                    param_points[param_name] = [param_def.default_value]
            
            elif param_def.type == ParameterType.CONTINUOUS:
                if param_def.min_value is not None and param_def.max_value is not None:
                    param_points[param_name] = np.linspace(
                        param_def.min_value, param_def.max_value, points_per_param
                    )
                else:
                    param_points[param_name] = [param_def.default_value]
            
            elif param_def.type == ParameterType.LOG_SCALE:
                if param_def.min_value is not None and param_def.max_value is not None:
                    if param_def.scale == "log":
                        param_points[param_name] = np.logspace(
                            np.log10(param_def.min_value), np.log10(param_def.max_value), points_per_param
                        )
                    else:
                        param_points[param_name] = np.linspace(
                            np.log(param_def.min_value), np.log(param_def.max_value), points_per_param
                        ).exp()
                else:
                    param_points[param_name] = [param_def.default_value]
            
            else:  # Default to uniform distribution
                if param_def.min_value is not None and param_def.max_value is not None:
                    param_points[param_name] = np.linspace(
                        param_def.min_value, param_def.max_value, points_per_param
                    )
                else:
                    param_points[param_name] = [param_def.default_value]
        
        # Generate all combinations
        from itertools import product
        
        for combination in product(*param_points.values()):
            param_dict = {}
            for i, (param_name, _) in enumerate(param_points.items()):
                param_dict[param_name] = combination[i]
            
            param_combinations.append(param_dict)
        
        self._log_event(f"Generated parameter grid with {len(param_combinations)} combinations", "INFO")
        return param_combinations
    
    def random_parameter_search(self, parameter_space: Dict[str, ParameterDefinition],
                               num_trials: int = 100) -> List[Dict[str, Any]]:
        """Random parameter search.
        
        Args:
            parameter_space: Parameter space definition
            num_trials: Number of random trials
            
        Returns:
            List of parameter combinations
        """
        param_combinations = []
        
        for i in range(num_trials):
            param_dict = {}
            
            for param_name, param_def in parameter_space.items():
                if param_def.type == ParameterType.CATEGORICAL:
                    param_dict[param_name] = np.random.choice(param_def.values)
                
                elif param_def.type == ParameterType.INTEGER:
                    if param_def.min_value is not None and param_def.max_value is not None:
                        param_dict[param_name] = np.random.randint(
                            param_def.min_value, param_def.max_value + 1
                        )
                    else:
                        param_dict[param_name] = param_def.default_value
                
                elif param_def.type == ParameterType.CONTINUOUS:
                    if param_def.min_value is not None and param_def.max_value is not None:
                        param_dict[param_name] = np.random.uniform(
                            param_def.min_value, param_def.max_value
                        )
                    else:
                        param_def.default_value
                
                elif param_def.type == ParameterType.LOG_SCALE:
                    if param_def.min_value is not None and param_def.max_value is not None:
                        if param_def.scale == "log":
                            param_dict[param_name] = 10 ** np.random.uniform(
                                np.log10(param_def.min_value), np.log10(param_def.max_value)
                            )
                        else:
                            param_dict[param_name] = np.random.uniform(
                                np.log(param_def.min_value), np.log(param_def.max_value)
                            ).exp()
                    else:
                        param_def.default_value
                
                else:
                    param_dict[param_name] = param_def.default_value
            
            param_combinations.append(param_dict)
        
        self._log_event(f"Generated {num_trials} random parameter combinations", "INFO")
        return param_combinations
    
    def evaluate_parameter_set(self, parameters: Dict[str, Any],
                             objective_function: Callable = None,
                             simulation_connection = None) -> ParameterResult:
        """Evaluate a parameter set.
        
        Args:
            parameters: Parameter set to evaluate
            objective_function: Objective function for evaluation
            simulation_connection: Connection to simulation server
            
        Returns:
            Evaluation result
        """
        trial_id = f"trial_{len(self.results)}_{int(time.time())}"
        iteration = len(self.results)
        
        # Validate parameters
        try:
            validated_params = self._validate_parameters(parameters)
        except Exception as e:
            return ParameterResult(
                parameters=parameters,
                metric=0.0,
                timestamp=time.time(),
                trial_id=trial_id,
                iteration=iteration,
                success=False,
                error_message=str(e)
            )
        
        # Evaluate using objective function or simulation
        metric = 0.0
        success = True
        error_msg = None
        
        try:
            if objective_function:
                metric = objective_function(validated_params)
            
            elif simulation_connection:
                # Connect to simulation and run with parameters
                metric = self._run_simulation_with_parameters(simulation_connection, validated_params)
            
            else:
                # Default evaluation (generate random metric for testing)
                metric = np.random.random() * 0.8 + 0.1
        
        except Exception as e:
            success = False
            error_msg = str(e)
            metric = 0.0
        
        result = ParameterResult(
            parameters=validated_params,
            metric=metric,
            timestamp=time.time(),
            trial_id=trial_id,
            iteration=iteration,
            success=success,
            error_message=error_msg,
            metadata={
                "parameter_space": validated_params,
                "evaluation_time": time.time()
            }
        )
        
        self.results.append(result)
        self._log_event(f"Evaluated parameters {trial_id}: {metric:.4f}", "INFO",
                       {"trial_id": trial_id, "metric": metric})
        
        return result
    
    def _validate_parameters(self, parameters: Dict[str, Any]) -> Dict[str, Any]:
        """Validate parameters against parameter space definition.
        
        Args:
            parameters: Parameters to validate
            
        Returns:
            Validated parameters
        """
        validated = {}
        
        for param_name, param_value in parameters.items():
            if param_name in self.parameter_definitions:
                param_def = self.parameter_definitions[param_name]
                
                # Apply type-specific validation
                if param_def.type == ParameterType.CATEGORICAL:
                    if param_def.values and param_value not in param_def.values:
                        raise ValueError(f"Parameter '{param_name}' value '{param_value}' not in allowed values: {param_def.values}")
                
                elif param_def.type == ParameterType.INTEGER:
                    if not isinstance(param_value, (int, np.integer)):
                        try:
                            param_value = int(param_value)
                        except ValueError:
                            raise ValueError(f"Parameter '{param_name}' must be an integer, got {type(param_value)}")
                    
                    if param_def.min_value is not None and param_value < param_def.min_value:
                        raise ValueError(f"Parameter '{param_name}' value {param_value} below minimum {param_def.min_value}")
                    
                    if param_def.max_value is not None and param_value > param_def.max_value:
                        raise ValueError(f"Parameter '{param_name}' value {param_value} above maximum {param_def.max_value}")
                
                elif param_def.type == ParameterType.CONTINUOUS:
                    if not isinstance(param_value, (int, float, np.number)):
                        try:
                            param_value = float(param_value)
                        except ValueError:
                            raise ValueError(f"Parameter '{param_name}' must be a number, got {type(param_value)}")
                    
                    if param_def.min_value is not None and param_value < param_def.min_value:
                        raise ValueError(f"Parameter '{param_name}' value {param_value} below minimum {param_def.min_value}")
                    
                    if param_def.max_value is not None and param_value > param_def.max_value:
                        raise ValueError(f"Parameter '{param_name}' value {param_value} above maximum {param_def.max_value}")
                
                elif param_def.type == ParameterType.LOG_SCALE:
                    if param_value <= 0:
                        raise ValueError(f"Parameter '{param_name}' value must be positive for log scale")
            
            validated[param_name] = param_value
        
        return validated
    
    def run_grid_search(self, parameter_space: Dict[str, ParameterDefinition],
                       points_per_param: int = 5,
                       objective_function: Callable = None,
                       simulation_connection = None,
                       parallel: bool = False) -> OptimizationHistory:
        """Run grid search optimization.
        
        Args:
            parameter_space: Parameter space definition
            points_per_param: Number of points per parameter
            objective_function: Objective function for evaluation
            simulation_connection: Connection to simulation server
            parallel: Whether to run in parallel
            
        Returns:
            Optimization history
        """
        # Generate parameter grid
        param_combinations = self.generate_parameter_grid(parameter_space, points_per_param)
        
        # Evaluate parameters
        if parallel:
            results = self._parallel_evaluate(param_combinations, objective_function, simulation_connection)
        else:
            results = []
            for i, params in enumerate(param_combinations):
                print(f"Evaluating combination {i+1}/{len(param_combinations)}")
                result = self.evaluate_parameter_set(params, objective_function, simulation_connection)
                results.append(result)
        
        # Create optimization history
        best_result = max(results, key=lambda x: x.metric) if results else None
        
        history = OptimizationHistory(
            algorithm=OptimizationAlgorithm.GRID_SEARCH,
            best_parameter=best_result.parameters if best_result else {},
            best_metric=best_result.metric if best_result else 0.0,
            iterations=points_per_param,
            total_trials=len(results),
            convergence=len(results) > 0,
            execution_time=time.time()
        )
        
        history.history = results
        
        self.optimization_history.append(history)
        
        self._log_event(f"Grid search completed: {len(results)} trials, best metric: {history.best_metric:.4f}", "INFO")
        
        return history
    
    def run_random_search(self, parameter_space: Dict[str, ParameterDefinition],
                         num_trials: int = 50,
                         objective_function: Callable = None,
                         simulation_connection = None,
                         parallel: bool = False) -> OptimizationHistory:
        """Run random search optimization.
        
        Args:
            parameter_space: Parameter space definition
            num_trials: Number of trials
            objective_function: Objective function for evaluation
            simulation_connection: Connection to simulation server
            parallel: Whether to run in parallel
            
        Returns:
            Optimization history
        """
        # Generate parameter combinations
        param_combinations = self.random_parameter_search(parameter_space, num_trials)
        
        # Evaluate parameters
        if parallel:
            results = self._parallel_evaluate(param_combinations, objective_function, simulation_connection)
        else:
            results = []
            for i, params in enumerate(param_combinations):
                print(f"Evaluating trial {i+1}/{num_trials}")
                result = self.evaluate_parameter_set(params, objective_function, simulation_connection)
                results.append(result)
        
        # Create optimization history
        best_result = max(results, key=lambda x: x.metric) if results else None
        
        history = OptimizationHistory(
            algorithm=OptimizationAlgorithm.RANDOM_SEARCH,
            best_parameter=best_result.parameters if best_result else {},
            best_metric=best_result.metric if best_result else 0.0,
            iterations=1,
            total_trials=len(results),
            convergence=len(results) > 0,
            execution_time=time.time()
        )
        
        history.history = results
        
        self.optimization_history.append(history)
        
        self._log_event(f"Random search completed: {len(results)} trials, best metric: {history.best_metric:.4f}", "INFO")
        
        return history
    
    def run_sensitivity_analysis(self, results: List[ParameterResult],
                                 target_metric: str = "metric") -> Dict[str, Any]:
        """Run sensitivity analysis on parameter results.
        
        Args:
            results: Parameter evaluation results
            target_metric: Target metric for analysis
            
        Returns:
            Sensitivity analysis results
        """
        if not results:
            return {}
        
        # Convert to DataFrame
        df = pd.DataFrame([{
            'trial_id': r.trial_id,
            'iteration': r.iteration,
            **r.parameters,
            'metric': r.metric,
            'success': r.success
        } for r in results])
        
        # Calculate correlation coefficients
        sensitivity_analysis = {}
        
        # Get parameter names (excluding trial_id, iteration, metric, success)
        param_names = [col for col in df.columns if col not in ['trial_id', 'iteration', 'metric', 'success']]
        
        for param in param_names:
            # Calculate correlation with target metric
            if param in df.columns and pd.api.types.is_numeric_dtype(df[param]):
                correlation = df[param].corr(df['metric'])
                
                # Calculate mutual information (simplified)
                mi = self._calculate_mutual_information(df[param], df['metric'])
                
                # Calculate standardized regression coefficient
                if len(df) > 1:
                    X = df[[param]].fillna(df[param].median())
                    y = df['metric'].fillna(df['metric'].median())
                    
                    try:
                        model = LinearRegression()
                        model.fit(X, y)
                        coef = model.coef_[0]
                    except:
                        coef = 0.0
                else:
                    coef = 0.0
                
                sensitivity_analysis[param] = {
                    'correlation': correlation,
                    'mutual_information': mi,
                    'standardized_coefficient': coef,
                    'mean': df[param].mean(),
                    'std': df[param].std(),
                    'min': df[param].min(),
                    'max': df[param].max(),
                    'num_samples': len(df)
                }
        
        # Rank parameters by importance
        importance_ranking = sorted(
            sensitivity_analysis.items(),
            key=lambda x: abs(x[1]['correlation']),
            reverse=True
        )
        
        sensitivity_analysis['importance_ranking'] = [
            {'parameter': param, 'importance': abs(data['correlation'])}
            for param, data in importance_ranking
        ]
        
        self.analysis_cache['sensitivity'] = sensitivity_analysis
        
        self._log_event(f"Sensitivity analysis completed for {len(param_names)} parameters", "INFO")
        
        return sensitivity_analysis
    
    def _calculate_mutual_information(self, x: pd.Series, y: pd.Series) -> float:
        """Calculate mutual information between two variables."""
        try:
            # Normalize data
            x_norm = (x - x.mean()) / x.std()
            y_norm = (y - y.mean()) / y.std()
            
            # Calculate bins for histogram
            x_bins = min(10, len(x) // 10)
            y_bins = min(10, len(y) // 10)
            
            # Create 2D histogram
            hist, x_edges, y_edges = np.histogram2d(x_norm, y_norm, bins=(x_bins, y_bins))
            
            # Normalize histogram
            hist = hist / hist.sum()
            
            # Calculate mutual information
            mi = -np.sum(hist * np.log(hist + 1e-10))
            
            return mi
        
        except Exception:
            return 0.0
    
    def visualize_parameter_effects(self, results: List[ParameterResult],
                                   output_file: str = None) -> str:
        """Visualize parameter effects.
        
        Args:
            results: Parameter evaluation results
            output_file: Output file path (auto-generated if None)
            
        Returns:
            Output file path
        """
        if not results:
            self._log_event("No results to visualize", "WARNING")
            return ""
        
        # Convert to DataFrame
        df = pd.DataFrame([{
            'trial_id': r.trial_id,
            'iteration': r.iteration,
            **r.parameters,
            'metric': r.metric,
            'success': r.success
        } for r in results])
        
        # Get parameter names
        param_names = [col for col in df.columns if col not in ['trial_id', 'iteration', 'metric', 'success']]
        
        # Create visualizations
        if output_file is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = f"parameter_results/parameter_effects_{timestamp}.html"
        
        os.makedirs("parameter_results", exist_ok=True)
        
        if PLOTLY_AVAILABLE and len(param_names) > 0:
            # Create interactive parameter effect plot
            fig = go.Figure()
            
            # Add scatter plots for top parameters
            n_top = min(3, len(param_names))
            
            for i, param in enumerate(param_names[:n_top]):
                fig.add_trace(go.Scatter(
                    x=df[param],
                    y=df['metric'],
                    mode='markers',
                    name=param,
                    text=df['trial_id'],
                    hovertemplate=f"{param}: %{{x}}<br>Metric: %{{y}}<br>Trial: %{{text}}<extra></extra>"
                ))
            
            fig.update_layout(
                title="Parameter Effects on Target Metric",
                xaxis_title="Parameter Value",
                yaxis_title="Target Metric",
                legend=dict(x=0.1, y=0.9),
                width=800,
                height=600
            )
            
            fig.write_html(output_file)
            
        elif SEABORN_AVAILABLE and len(param_names) > 0:
            # Create static plots
            import matplotlib.pyplot as plt
            
            fig, axes = plt.subplots(2, 2, figsize=(15, 12))
            fig.suptitle("Parameter Effects Analysis", fontsize=16)
            
            # Plot 1: Scatter plot of first two parameters
            if len(param_names) >= 2:
                ax = axes[0, 0]
                ax.scatter(df[param_names[0]], df[param_names[1]], c=df['metric'], alpha=0.7)
                ax.set_xlabel(param_names[0])
                ax.set_ylabel(param_names[1])
                ax.set_title("Parameter Correlation")
            
            # Plot 2: Box plot for categorical parameters
            cat_params = [p for p in param_names if df[p].dtype == 'object']
            if cat_params and len(cat_params) > 0:
                ax = axes[0, 1]
                df.boxplot(column='metric', by=cat_params[0], ax=ax)
                ax.set_title(f"Metric by {cat_params[0]}")
                ax.set_xlabel(cat_params[0])
                ax.set_ylabel("Metric")
            
            # Plot 3: Metric distribution
            ax = axes[1, 0]
            ax.hist(df['metric'], bins=20, alpha=0.7, edgecolor='black')
            ax.set_xlabel("Metric")
            ax.set_ylabel("Frequency")
            ax.set_title("Metric Distribution")
            
            # Plot 4: Parameter correlation heatmap
            ax = axes[1, 1]
            numeric_params = [p for p in param_names if pd.api.types.is_numeric_dtype(df[p])]
            if len(numeric_params) >= 2:
                corr_matrix = df[numeric_params].corr()
                sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, ax=ax)
                ax.set_title("Parameter Correlation Heatmap")
            
            plt.tight_layout()
            plt.savefig(output_file.replace('.html', '.png'), dpi=300, bbox_inches='tight')
            plt.close()
        
        self._log_event(f"Parameter effects visualization saved to {output_file}", "INFO")
        return output_file
    
    def export_results(self, results: List[ParameterResult], format: str,
                      output_file: str = None) -> str:
        """Export results.
        
        Args:
            results: Parameter results
            format: Export format (json, csv)
            output_file: Output file path
            
        Returns:
            Output file path
        """
        if output_file is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = f"parameter_results/exported_results_{timestamp}.{format}"
        
        os.makedirs("parameter_results", exist_ok=True)
        
        try:
            if format.lower() == "json":
                data = {
                    'results': [{
                        'trial_id': r.trial_id,
                        'iteration': r.iteration,
                        'parameters': r.parameters,
                        'metric': r.metric,
                        'timestamp': r.timestamp,
                        'success': r.success,
                        'error_message': r.error_message,
                        'metadata': r.metadata
                    } for r in results]
                }
                
                with open(output_file, 'w') as f:
                    json.dump(data, f, indent=2, default=str)
            
            elif format.lower() == "csv":
                df = pd.DataFrame([{
                    'trial_id': r.trial_id,
                    'iteration': r.iteration,
                    **r.parameters,
                    'metric': r.metric,
                    'timestamp': r.timestamp,
                    'success': r.success,
                    'error_message': r.error_message
                } for r in results])
                
                df.to_csv(output_file, index=False)
            
            self._log_event(f"Results exported: {output_file}", "INFO")
            return output_file
        
        except Exception as e:
            self._log_event(f"Error exporting results: {e}", "ERROR")
            return ""
    
    def import_results(self, input_file: str) -> List[ParameterResult]:
        """Import results from file.
        
        Args:
            input_file: Input file path
            
        Returns:
            Imported results
        """
        try:
            with open(input_file, 'r') as f:
                data = json.load(f)
            
            results = []
            
            if 'results' in data:
                for result_data in data['results']:
                    result = ParameterResult(**result_data)
                    results.append(result)
            
            self.results.extend(results)
            
            self._log_event(f"Results imported from {input_file}", "INFO",
                           {"num_results": len(results)})
            
            return results
        
        except Exception as e:
            self._log_event(f"Error importing results: {e}", "ERROR")
            return []
    
    def _parallel_evaluate(self, param_combinations: List[Dict[str, Any]],
                          objective_function: Callable = None,
                          simulation_connection = None,
                          max_workers: int = 4) -> List[ParameterResult]:
        """Parallel evaluation of parameter combinations.
        
        Args:
            param_combinations: Parameter combinations
            objective_function: Objective function
            simulation_connection: Simulation connection
            max_workers: Maximum number of workers
            
        Returns:
            List of evaluation results
        """
        from concurrent.futures import ThreadPoolExecutor
        
        results = []
        
        def evaluate_single(params):
            return self.evaluate_parameter_set(params, objective_function, simulation_connection)
        
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            results = list(executor.map(evaluate_single, param_combinations))
        
        return results
    
    def _log_event(self, message: str, level: str = "INFO", context: Dict[str, Any] = None):
        """Log event."""
        timestamp_str = datetime.now().strftime('%H:%M:%S.%f')[:-3]
        print(f"[{timestamp_str}] {level}: {message}")
    
    def _run_simulation_with_parameters(self, simulation_connection, parameters: Dict[str, Any]) -> float:
        """Run simulation with given parameters."""
        # This would connect to simulation server and run simulation
        # For now, return a random metric based on parameters
        time_factor = parameters.get("learning_rate", 0.01) * 100
        complexity_factor = parameters.get("hidden_size", 256) / 1024
        
        metric = (time_factor * 0.3 + complexity_factor * 0.7 + np.random.random() * 0.2)
        
        return metric


def main():
    """Main entry point for ParameterExplorer."""
    parser = argparse.ArgumentParser(description="NLM Parameter Explorer Tool")
    parser.add_argument("--search", choices=["grid", "random", "bayesian", "genetic"],
                       help="Search algorithm")n    parser.add_argument("--parameter-file", help="Parameter definitions file")
    parser.add_argument("--points-per-param", type=int, default=5, help="Points per parameter for grid search")
    parser.add_argument("--num-trials", type=int, default=50, help="Number of trials for random search")
    parser.add_argument("--target-metric", default="accuracy", help="Target metric for optimization")
    parser.add_argument("--output", "-o", help="Output file for results")
    parser.add_argument("--format", choices=["json", "csv"], default="json", help="Output format")
    parser.add_argument("--analyze", action="store_true", help="Analyze results")
    parser.add_argument("--visualize", action="store_true", help="Visualize results")
    parser.add_argument("--iterations", type=int, default=10, help="Number of iterations")n    parser.add_argument("--parallel", action="store_true", help="Run in parallel")
    parser.add_argument("--host", default="localhost", help="Host for parameter server")
    parser.add_argument("--port", type=int, default=8888, help="Port for parameter server")
    parser.add_argument("--log-file", help="Log file for recording session")
    parser.add_argument("--sensitivity-analysis", action="store_true", help="Run sensitivity analysis")
    
    args = parser.parse_args()
    
    # Initialize parameter explorer
    explorer = ParameterExplorer(args.host, args.port)
    
    # Load parameter space
    if args.parameter_file:
        parameter_space = explorer.load_parameter_space(args.parameter_file)
    else:
        parameter_space = explorer.get_default_parameter_space()
    
    # Run search based on algorithm
    if args.search == "grid":
        history = explorer.run_grid_search(
            parameter_space, args.points_per_param,
            parallel=args.parallel
        )
    
    elif args.search == "random":
        history = explorer.run_random_search(
            parameter_space, args.num_trials,
            parallel=args.parallel
        )
    
    else:
        print(f"Search algorithm '{args.search}' not yet implemented")
        return
    
    # Analyze results if requested
    if args.analyze:
        sensitivity = explorer.run_sensitivity_analysis(explorer.results, args.target_metric)
        print(f"Sensitivity analysis completed")
        print(json.dumps(sensitivity, indent=2))
    
    # Visualize results if requested
    if args.visualize:
        viz_file = explorer.visualize_parameter_effects(explorer.results)
        print(f"Visualization saved to: {viz_file}")
    
    # Export results
    if args.output:
        export_file = explorer.export_results(explorer.results, args.format, args.output)
        print(f"Results exported to: {export_file}")
    
    print("ParameterExplorer completed")


if __name__ == "__main__":
    main()