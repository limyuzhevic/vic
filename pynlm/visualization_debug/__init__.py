"""
pynlm.visualization_debug - Debugging tools for visualization

This module provides:
- Visualization state inspection
- Rendering debugging tools
- Data validation for visualization
- Interactive debugging interfaces
"""

import time
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, Any, List, Optional, Tuple, Union, Callable
from dataclasses import dataclass, field, asdict
from abc import ABC, abstractmethod
from pathlib import Path
import copy
import threading
from collections import defaultdict, deque
import warnings
import json

# Import logging components
from pynlm.logging import (
    get_visualization_logger, get_brain_logger, get_simulation_logger
)

logger = get_visualization_logger()


@dataclass
class VisualizationState:
    """State information for visualization."""
    timestamp: float = field(default_factory=time.time)
    visualization_type: str = "unknown"
    data_source: str = ""
    parameters: Dict[str, Any] = field(default_factory=dict)
    rendering_settings: Dict[str, Any] = field(default_factory=dict)
    active_regions: List[str] = field(default_factory=list)
    selected_neurons: List[str] = field(default_factory=list)
    color_map: Optional[str] = None
    time_series_data: Optional[Dict[str, Any]] = None
    plot_configuration: Optional[Dict[str, Any]] = None
    debug_info: Dict[str, Any] = field(default_factory=dict)


class VisualizationDebugger:
    """Debugger for visualization components."""

    def __init__(self, debug_port: Optional[int] = None):
        """
        Initialize visualization debugger.

        Parameters
        ----------
        debug_port : int, optional
            Port for interactive debugging interface
        """
        self.debug_port = debug_port
        self.visualization_history: List[VisualizationState] = []
        self.debug_sessions: Dict[str, Dict[str, Any]] = {}
        self.data_validators: Dict[str, Callable] = {}
        self.rendering_tools: Dict[str, Callable] = {}
        self._debug_server = None
        self._is_debugging = False

    def start_debug_session(self, session_id: str, visualization_config: Dict[str, Any]) -> None:
        """
        Start a debug session for visualization.

        Parameters
        ----------
        session_id : str
            Unique identifier for the debug session
        visualization_config : Dict[str, Any]
            Initial visualization configuration
        """
        if session_id in self.debug_sessions:
            logger.warning(f"Debug session {session_id} already exists")
            return

        self.debug_sessions[session_id] = {
            "session_id": session_id,
            "start_time": time.time(),
            "visualization_config": visualization_config,
            "state_history": [],
            "errors": [],
            "performance_metrics": {}
        }

        logger.info(f"Started debug session: {session_id}")

    def stop_debug_session(self, session_id: str) -> None:
        """
        Stop a debug session.

        Parameters
        ----------
        session_id : str
            Session identifier
        """
        if session_id in self.debug_sessions:
            session = self.debug_sessions[session_id]
            session["end_time"] = time.time()
            session["duration"] = session["end_time"] - session["start_time"]

            # Calculate performance metrics
            if session["state_history"]:
                render_times = [s.get("render_time", 0) for s in session["state_history"]]
                session["performance_metrics"] = {
                    "avg_render_time": np.mean(render_times) if render_times else 0,
                    "max_render_time": max(render_times) if render_times else 0,
                    "render_time_std": np.std(render_times) if len(render_times) > 1 else 0
                }

            logger.info(f"Stopped debug session: {session_id}")

    def update_visualization_state(self, session_id: str, state: VisualizationState) -> None:
        """
        Update visualization state for a debug session.

        Parameters
        ----------
        session_id : str
            Session identifier
        state : VisualizationState
            Visualization state
        """
        if session_id not in self.debug_sessions:
            logger.warning(f"Debug session {session_id} not found")
            return

        # Update session state
        session = self.debug_sessions[session_id]
        session["state_history"].append(asdict(state))

        # Keep only recent history
        if len(session["state_history"]) > 1000:
            session["state_history"] = session["state_history"][-1000:]

        logger.debug(f"Updated visualization state for session {session_id}")

    def get_session_state(self, session_id: str, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """
        Get visualization state for a session.

        Parameters
        ----------
        session_id : str
            Session identifier
        limit : int, optional
            Maximum number of states to return

        Returns
        -------
        List[Dict[str, Any]]
            Visualization states
        """
        if session_id not in self.debug_sessions:
            return []

        states = self.debug_sessions[session_id]["state_history"]
        return states[-limit:] if limit else states

    def add_data_validator(self, name: str, validator_func: Callable,
                          description: str = "") -> None:
        """
        Add a data validator for visualization.

        Parameters
        ----------
        name : str
        validator_func : Callable
            Function that validates visualization data
        description : str
            Description of the validator
        """
        self.data_validators[name] = lambda data: (validator_func(data), description)

    def add_rendering_tool(self, name: str, tool_func: Callable,
                          description: str = "") -> None:
        """
        Add a rendering tool for debugging.

        Parameters
        ----------
        name : str
        tool_func : Callable
            Function that enhances rendering
        description : str
            Description of the tool
        """
        self.rendering_tools[name] = lambda data: (tool_func(data), description)

    def validate_visualization_data(self, session_id: str, data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Validate visualization data using registered validators.

        Parameters
        ----------
        session_id : str
            Session identifier
        data : Dict[str, Any]
            Visualization data to validate

        Returns
        -------
        Dict[str, Any]
            Validation results
        """
        validation_result = {
            "session_id": session_id,
            "timestamp": time.time(),
            "valid": True,
            "validators_applied": [],
            "errors": [],
            "warnings": [],
            "validation_details": {}
        }

        # Apply data validators
        for validator_name, validator_func in self.data_validators.items():
            validation_result["validators_applied"].append(validator_name)
            try:
                is_valid, result = validator_func(data)
                validation_result["validation_details"][validator_name] = {
                    "valid": is_valid,
                    "result": result
                }

                if not is_valid:
                    validation_result["valid"] = False
                    validation_result["errors"].append({
                        "validator": validator_name,
                        "message": result
                    })
                elif result and "warning" in result.lower():
                    validation_result["warnings"].append({
                        "validator": validator_name,
                        "message": result
                    })

            except Exception as e:
                validation_result["valid"] = False
                validation_result["errors"].append({
                    "validator": validator_name,
                    "message": f"Validation error: {str(e)}"
                })

        # Log validation results
        if validation_result["valid"]:
            logger.debug(f"Data validation passed for session {session_id}")
        else:
            logger.warning(f"Data validation failed for session {session_id}: {validation_result['errors']}")

        return validation_result

    def enhance_rendering(self, session_id: str, visualization_data: Dict[str, Any],
                        enhancement: Optional[str] = None) -> Dict[str, Any]:
        """
        Enhance visualization rendering using registered tools.

        Parameters
        ----------
        session_id : str
            Session identifier
        visualization_data : Dict[str, Any]
            Visualization data
        enhancement : str, optional
            Specific enhancement to apply, if None applies all

        Returns
        -------
        Dict[str, Any]
            Enhanced visualization data
        """
        enhanced_data = copy.deepcopy(visualization_data)
        applied_enhancements = []

        # Apply rendering enhancements
        if enhancement:
            if enhancement in self.rendering_tools:
                tool_name = enhancement
                tool_func = self.rendering_tools[tool_name]
                try:
                    result = tool_func(enhanced_data)
                    enhanced_data.update(result)
                    applied_enhancements.append(tool_name)
                except Exception as e:
                    logger.error(f"Rendering enhancement {tool_name} failed: {e}")
        else:
            # Apply all enhancements
            for tool_name, tool_func in self.rendering_tools.items():
                try:
                    result = tool_func(enhanced_data)
                    enhanced_data.update(result)
                    applied_enhancements.append(tool_name)
                except Exception as e:
                    logger.error(f"Rendering enhancement {tool_name} failed: {e}")

        logger.debug(f"Applied {len(applied_enhancements)} rendering enhancements for session {session_id}")
        return enhanced_data

    def debug_rendering_issues(self, session_id: str, visualization_data: Dict[str, Any],
                              debug_info: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Debug rendering issues in visualization.

        Parameters
        ----------
        session_id : str
            Session identifier
        visualization_data : Dict[str, Any]
            Visualization data
        debug_info : Dict[str, Any], optional
            Additional debug information

        Returns
        -------
        Dict[str, Any]
            Debugging results
        """
        debugging_result = {
            "session_id": session_id,
            "timestamp": time.time(),
            "issues": [],
            "diagnostics": {},
            "suggestions": [],
            "rendering_suggestions": []
        }

        # Basic data validation
        if not visualization_data:
            debugging_result["issues"].append({
                "type": "empty_data",
                "severity": "high",
                "description": "Visualization data is empty"
            })

        # Check for required fields based on visualization type
        viz_type = visualization_data.get("type", "unknown")

        if viz_type == "heatmap":
            if "data" not in visualization_data:
                debugging_result["issues"].append({
                    "type": "missing_data",
                    "severity": "high",
                    "description": "Heatmap visualization missing data field"
                })
            elif not isinstance(visualization_data["data"], dict):
                debugging_result["issues"].append({
                    "type": "invalid_data_format",
                    "severity": "medium",
                    "description": "Heatmap data should be a dictionary"
                })

        elif viz_type == "network":
            if "nodes" not in visualization_data:
                debugging_result["issues"].append({
                    "type": "missing_nodes",
                    "severity": "high",
                    "description": "Network visualization missing nodes"
                })
            if "edges" not in visualization_data:
                debugging_result["issues"].append({
                    "type": "missing_edges",
                    "severity": "high",
                    "description": "Network visualization missing edges"
                })

        # Check for rendering performance issues
        if debug_info:
            render_time = debug_info.get("render_time", 0)
            if render_time > 1.0:  # More than 1 second
                debugging_result["issues"].append({
                    "type": "performance",
                    "severity": "medium",
                    "description": f"Slow rendering detected: {render_time:.2f}s"
                })

        # Generate suggestions
        for issue in debugging_result["issues"]:
            if issue["type"] == "empty_data":
                debugging_result["suggestions"].append(
                    "Provide valid visualization data"
                )
            elif issue["type"] == "missing_data" and viz_type == "heatmap":
                debugging_result["suggestions"].append(
                    "Ensure heatmap has 'data' field with neuron/region activity values"
                )
            elif issue["type"] == "missing_nodes" and viz_type == "network":
                debugging_result["suggestions"].append(
                    "Provide 'nodes' list with node identifiers and properties"
                )

        # Rendering suggestions
        if visualization_data:
            # Check for data that might be too large
            data_size = self._estimate_data_size(visualization_data)
            if data_size > 10 * 1024 * 1024:  # More than 10MB
                debugging_result["rendering_suggestions"].append(
                    "Consider downsampling data to improve rendering performance"
                )

            # Check for missing required parameters
            if "parameters" not in visualization_data:
                debugging_result["rendering_suggestions"].append(
                    "Add 'parameters' field for visualization configuration"
                )

        logger.debug(f"Debugged rendering for session {session_id}: {len(debugging_result['issues'])} issues found")
        return debugging_result

    def _estimate_data_size(self, data: Dict[str, Any]) -> int:
        """Estimate size of visualization data in bytes."""
        size = 0
        for key, value in data.items():
            if isinstance(value, (int, float, str, bool)):
                size += len(str(value))
            elif isinstance(value, (list, tuple)):
                size += sum(self._estimate_data_size(item) if isinstance(item, dict) else len(str(item)) for item in value)
            elif isinstance(value, dict):
                size += self._estimate_data_size(value)
            elif isinstance(value, np.ndarray):
                size += value.nbytes
        return size


class RenderingDebugger:
    """Tools for debugging rendering process."""

    def __init__(self):
        """Initialize rendering debugger."""
        self.rendering_history: List[Dict[str, Any]] = []
        self.render_hooks: Dict[str, Callable] = {}
        self.capture_mode = False
        self.captured_frames: List[Dict[str, Any]] = []

    def add_render_hook(self, name: str, hook_func: Callable,
                       hook_type: str = "pre_render") -> None:
        """
        Add a rendering hook.

        Parameters
        ----------
        name : str
        hook_func : Callable
            Hook function to add
        hook_type : str
            Type of hook ("pre_render", "post_render", "error")
        """
        if name not in self.render_hooks:
            self.render_hooks[name] = {}

        self.render_hooks[name][hook_type] = hook_func
        logger.debug(f"Added render hook: {name} ({hook_type})")

    def start_capture_mode(self):
        """Start capturing rendering frames."""
        self.capture_mode = True
        self.captured_frames = []
        logger.info("Started rendering capture mode")

    def stop_capture_mode(self):
        """Stop capturing rendering frames."""
        self.capture_mode = False
        logger.info(f"Stopped rendering capture mode. Captured {len(self.captured_frames)} frames")

    def debug_rendering(self, render_function: Callable, *args, **kwargs) -> Any:
        """
        Debug rendering with hooks and capture mode.

        Parameters
        ----------
        render_function : Callable
            Rendering function to debug
        *args
            Arguments to pass to rendering function
n        **kwargs
            Keyword arguments to pass to rendering function

        Returnsn        -------
        Any
            Rendering result
        """
        debug_info = {
            "timestamp": time.time(),
            "function_name": render_function.__name__,
            "args_count": len(args),
            "kwargs_keys": list(kwargs.keys()),
            "capture_enabled": self.capture_mode,
            "hooks_called": []
        }

        try:
            # Call pre-render hooks
            for hook_name, hooks in self.render_hooks.items():
                if "pre_render" in hooks:
                    hooks["pre_render"](debug_info)
                    debug_info["hooks_called"].append(f"{hook_name}_pre_render")

            # Execute rendering
            render_start_time = time.time()
            result = render_function(*args, **kwargs)
            render_end_time = time.time()

            debug_info["render_time"] = render_end_time - render_start_time

            # Call post-render hooks
            for hook_name, hooks in self.render_hooks.items():
                if "post_render" in hooks:
                    hooks["post_render"](result, debug_info)
                    debug_info["hooks_called"].append(f"{hook_name}_post_render")

            # Capture frame if in capture mode
            if self.capture_mode:
                self.captured_frames.append({
                    "timestamp": debug_info["timestamp"],
                    "render_time": debug_info["render_time"],
                    "hooks_called": debug_info["hooks_called"],
                    "result_type": type(result).__name__,
                    "result_summary": str(result)[:200] if result else None
                })

        except Exception as e:
            # Call error hooks
            for hook_name, hooks in self.render_hooks.items():
                if "error" in hooks:
                    hooks["error"](e, debug_info)
                    debug_info["hooks_called"].append(f"{hook_name}_error")

            debug_info["error"] = str(e)
            debug_info["error_type"] = type(e).__name__

            raise

        self.rendering_history.append(debug_info)
        logger.debug(f"Debugged rendering of {render_function.__name__}: {debug_info['render_time']:.3f}s")

        return result

    def analyze_rendering_performance(self, history_limit: int = 100) -> Dict[str, Any]:
        """
        Analyze rendering performance from history.

        Parameters
        ----------
        history_limit : int
            Number of recent renders to analyze

        Returns
        -------
        Dict[str, Any]
            Performance analysis
        """
        history = self.rendering_history[-history_limit:]
        if not history:
            return {"status": "no_data", "message": "No rendering history available"}

        analysis = {
            "total_renders": len(history),
            "performance_metrics": {
                "avg_render_time": 0,
                "min_render_time": float('inf'),
                "max_render_time": 0,
                "std_render_time": 0,
                "p95_render_time": 0
            },
            "performance_issues": [],
            "optimization_suggestions": []
        }

        render_times = [h["render_time"] for h in history if "render_time" in h]

        if render_times:
            analysis["performance_metrics"]["avg_render_time"] = np.mean(render_times)
            analysis["performance_metrics"]["min_render_time"] = min(render_times)
            analysis["performance_metrics"]["max_render_time"] = max(render_times)
            analysis["performance_metrics"]["std_render_time"] = np.std(render_times)

            # Calculate percentiles
            sorted_times = sorted(render_times)
            idx_95 = int(len(sorted_times) * 0.95)
            analysis["performance_metrics"]["p95_render_time"] = sorted_times[idx_95]

        # Identify performance issues
        avg_time = analysis["performance_metrics"]["avg_render_time"]
        if avg_time > 0.5:  # More than 0.5 seconds
            analysis["performance_issues"].append({
                "type": "slow_average",
                "severity": "medium",
                "description": f"Average render time is high: {avg_time:.3f}s"
            })

        p95_time = analysis["performance_metrics"]["p95_render_time"]
        if p95_time > 1.0:  # More than 1 second
            analysis["performance_issues"].append({
                "type": "slow_p95",
                "severity": "high",
                "description": f"95th percentile render time is very high: {p95_time:.3f}s"
            })

        # Generate optimization suggestions
        if analysis["performance_issues"]:
            analysis["optimization_suggestions"].append(
                "Consider optimizing rendering pipeline for better performance"
            )
            analysis["optimization_suggestions"].append(
                "Profile specific render functions to identify bottlenecks"
            )

        logger.info(f"Analyzed rendering performance: {len(render_times)} renders")
        return analysis

    def compare_captures(self, capture1_idx: int, capture2_idx: int) -> Dict[str, Any]:
        """
        Compare two captured rendering frames.

        Parameters
        ----------
        capture1_idx : int
            Index of first capture
        capture2_idx : int
            Index of second capture

        Returns
        -------
        Dict[str, Any]
            Comparison results
        """
        if capture1_idx >= len(self.captured_frames):
            raise ValueError(f"Capture index {capture1_idx} out of range")
        if capture2_idx >= len(self.captured_frames):
            raise ValueError(f"Capture index {capture2_idx} out of range")

        cap1 = self.captured_frames[capture1_idx]
        cap2 = self.captured_frames[capture2_idx]

        comparison = {
            "capture1_index": capture1_idx,
            "capture2_index": capture2_idx,
            "timestamp_difference": cap2["timestamp"] - cap1["timestamp"],
            "render_time_difference": cap2["render_time"] - cap1["render_time"],
            "differences": {}
        }

        # Compare hooks called
        hooks1 = set(cap1.get("hooks_called", []))
        hooks2 = set(cap2.get("hooks_called", []))
        comparison["differences"]["hooks_called"] = {
            "only_in_1": hooks1 - hooks2,
            "only_in_2": hooks2 - hooks1,
            "common": hooks1 & hooks2
        }

        # Compare result types
        comparison["differences"]["result_types"] = {
            "capture1": cap1.get("result_type"),
            "capture2": cap2.get("result_type")
        }

        logger.info(f"Compared captures {capture1_idx} and {capture2_idx}")
        return comparison


class DataValidator:
    """Validator for visualization data."""

    def __init__(self):
        """Initialize data validator."""
        self.validation_rules: Dict[str, Callable] = {}
        self.validation_history: List[Dict[str, Any]] = []

    def add_validation_rule(self, name: str, rule_func: Callable,
                          data_type: str = "any", required: bool = True,
                          description: str = "") -> None:
        """
        Add a validation rule.

        Parameters
        ----------
        name : str
        rule_func : Callable
            Function that validates data
        data_type : str
            Expected data type ("heatmap", "network", "scatter", "timeseries", "any")
        required : bool
            Whether this rule is required
        description : str
            Description of the rule
        """
        self.validation_rules[name] = {
            "function": rule_func,
            "data_type": data_type,
            "required": required,
            "description": description
        }

    def validate_data(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Validate visualization data.

        Parameters
        ----------
        data : Dict[str, Any]
            Data to validate

        Returns
        -------
        Dict[str, Any]
            Validation results
        """
        validation_result = {
            "timestamp": time.time(),
            "valid": True,
            "data_type": data.get("type", "unknown"),
            "rules_applied": [],
            "errors": [],
            "warnings": [],
            "validation_details": {}
        }

        # Apply validation rules
        data_type = validation_result["data_type"]
        for rule_name, rule_config in self.validation_rules.items():
            # Check if rule applies to data type
            if rule_config["data_type"] != "any" and rule_config["data_type"] != data_type:
                continue

            validation_result["rules_applied"].append(rule_name)
            try:
                is_valid, result = rule_config["function"](data)
                validation_result["validation_details"][rule_name] = {
                    "valid": is_valid,
                    "result": result,
                    "required": rule_config["required"]
                }

                if not is_valid:
                    if rule_config["required"]:
                        validation_result["valid"] = False
                        validation_result["errors"].append({
                            "rule": rule_name,
                            "message": result,
                            "severity": "high"
                        })
                    else:
                        validation_result["warnings"].append({
                            "rule": rule_name,
                            "message": result,
                            "severity": "low"
                        })

            except Exception as e:
                validation_result["valid"] = False
                validation_result["errors"].append({
                    "rule": rule_name,
                    "message": f"Validation error: {str(e)}",
                    "severity": "high"
                })

        # Log validation results
        if validation_result["valid"]:
            logger.debug(f"Data validation passed for {data_type}")
        else:
            logger.warning(f"Data validation failed for {data_type}: {validation_result['errors']}")

        self.validation_history.append(validation_result)
        return validation_result

    def get_validation_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """
        Get validation history.

        Parameters
        ----------
        limit : int
            Maximum number of history entries

        Returnsn        -------
        List[Dict[str, Any]]
            Validation history
        """
        return self.validation_history[-limit:]

    def generate_validation_report(self) -> Dict[str, Any]:
        """
        Generate validation report.

        Returns
        -------
        Dict[str, Any]
            Validation report
        """
        if not self.validation_history:
            return {"status": "no_data", "message": "No validation history available"}

        report = {
            "total_validations": len(self.validation_history),
            "data_types": defaultdict(lambda: {"valid": 0, "invalid": 0}),
            "error_summary": defaultdict(int),
            "warning_summary": defaultdict(int),
            "most_common_errors": [],
            "validation_trends": {}
        }

        for result in self.validation_history:
            data_type = result["data_type"]
            if result["valid"]:
                report["data_types"][data_type]["valid"] += 1
            else:
                report["data_types"][data_type]["invalid"] += 1

            for error in result["errors"]:
                report["error_summary"][error["message"]] += 1
            for warning in result["warnings"]:
                report["warning_summary"][warning["message"]] += 1

        # Find most common errors
        error_items = list(report["error_summary"].items())
        report["most_common_errors"] = sorted(
            error_items, key=lambda x: x[1], reverse=True
        )[:5]

        logger.info(f"Generated validation report: {len(self.validation_history)} validations")
        return report


# Convenience instances
visualization_debugger = VisualizationDebugger()
rendering_debugger = RenderingDebugger()
data_validator = DataValidator()


def get_visualization_debugger() -> VisualizationDebugger:
    """Get visualization debugger instance."""
    return visualization_debugger


def get_rendering_debugger() -> RenderingDebugger:
    """Get rendering debugger instance."""
    return rendering_debugger


def get_data_validator() -> DataValidator:
    """Get data validator instance."""
    return data_validator