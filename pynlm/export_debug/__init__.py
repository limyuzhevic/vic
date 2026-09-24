"""
pynlm.export_debug - Tools for exporting debug information

This module provides:
- Debug report generation
- State export for analysis
- Configuration export
- Performance report generation
"""

import time
import json
import pandas as pd
import numpy as np
from typing import Dict, Any, List, Optional, Tuple, Union
from dataclasses import dataclass, field, asdict
from abc import ABC, abstractmethod
from pathlib import Path
import copy
from collections import defaultdict
import warnings

# Optional dependency handling
try:
    import yaml
    HAS_YAML = True
except ImportError:
    HAS_YAML = False
    yaml = None
    warnings.warn("PyYAML is not installed. YAML export functionality is disabled.")
import csv
from datetime import datetime

# Import logging components
from pynlm.logging import (
    get_brain_logger, get_memory_logger, get_simulation_logger,
    get_plastic_logger, get_visualization_logger, get_checkpoint_logger
)

logger = get_simulation_logger()


@dataclass
class DebugReport:
    """Container for debug report data."""
    timestamp: float = field(default_factory=time.time)
    report_id: str = ""
    session_id: str = ""
    report_type: str = "comprehensive"
    generated_at: str = field(default_factory=lambda: datetime.now().isoformat())
    summary: Dict[str, Any] = field(default_factory=dict)
    brain_state: Dict[str, Any] = field(default_factory=dict)
    simulation_metrics: Dict[str, Any] = field(default_factory=dict)
    memory_usage: Dict[str, Any] = field(default_factory=dict)
    performance_analysis: Dict[str, Any] = field(default_factory=dict)
    errors_and_warnings: Dict[str, Any] = field(default_factory=dict)
    visualizations: Dict[str, Any] = field(default_factory=dict)
    recommendations: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)


class DebugReportGenerator:
    """Generate comprehensive debug reports."""

    def __init__(self, output_dir: Optional[str] = None):
        """
        Initialize debug report generator.

        Parameters
        ----------
        output_dir : str, optional
            Directory for output reports
        """
        self.output_dir = Path(output_dir) if output_dir else None
        if self.output_dir:
            self.output_dir.mkdir(parents=True, exist_ok=True)

    def generate_comprehensive_report(self, session_id: str, 
                                    data_sources: Dict[str, Any]) -> DebugReport:
        """
        Generate a comprehensive debug report.

        Parameters
        ----------
        session_id : str
            Session identifier
        data_sources : Dict[str, Any]
            Data sources for the report

        Returnsn        -------
        DebugReport
            Generated debug report
        """
        report_id = f"debug_report_{int(time.time())}_{session_id}"
        report = DebugReport(
            report_id=report_id,
            session_id=session_id,
            report_type="comprehensive",
            metadata={"generated_by": "NLM Debug Report Generator"}
        )

        try:
            # Generate report components
            report.summary = self._generate_summary(data_sources)
            report.brain_state = self._extract_brain_state(data_sources)
            report.simulation_metrics = self._extract_simulation_metrics(data_sources)
            report.memory_usage = self._extract_memory_usage(data_sources)
            report.performance_analysis = self._extract_performance_analysis(data_sources)
            report.errors_and_warnings = self._extract_errors_and_warnings(data_sources)
            report.visualizations = self._extract_visualizations(data_sources)
            report.recommendations = self._generate_recommendations(report)

            # Save report
            self._save_report(report)

            logger.info(f"Generated comprehensive debug report: {report_id}")
            return report

        except Exception as e:
            logger.error(f"Failed to generate debug report: {e}")
            raise

    def _generate_summary(self, data_sources: Dict[str, Any]) -> Dict[str, Any]:
        """Generate report summary."""
        summary = {
            "session_id": data_sources.get("session_id", "unknown"),
            "timestamp": time.time(),
            "data_sources": list(data_sources.keys()),
            "report_generation_time": 0,
            "report_size_estimate": 0
        }

        # Estimate report size
        estimated_size = sum(
            len(str(data)) if data else 0
            for data in data_sources.values()
            if not isinstance(data, (list, dict))
        )
        summary["report_size_estimate"] = f"{estimated_size / 1024:.2f} KB"

        return summary

    def _extract_brain_state(self, data_sources: Dict[str, Any]) -> Dict[str, Any]:
        """Extract brain state information."""
        brain_state = {
            "timestamp": time.time(),
            "brain_regions": {},
            "neuron_populations": {},
            "connectivity": {},
            "activity_levels": {},
            "state_snapshots": []
        }

        # Extract from brain state inspector
        if "brain_state_inspector" in data_sources:
            inspector = data_sources["brain_state_inspector"]
            if hasattr(inspector, "get_recent_snapshots"):
                snapshots = inspector.get_recent_snapshots(5)
                brain_state["state_snapshots"] = snapshots

        # Extract from simulation logger
        if "simulation_logger" in data_sources:
            # This would extract simulation-specific brain state data
            pass

        return brain_state

    def _extract_simulation_metrics(self, data_sources: Dict[str, Any]) -> Dict[str, Any]:
        """Extract simulation metrics."""
        metrics = {
            "timestamp": time.time(),
            "simulation_steps": 0,
            "simulation_duration": 0,
            "steps_per_second": 0,
            "simulation_phases": {},
            "performance_stats": {}
        }

        # Extract from simulation debugger
        if "simulation_debugger" in data_sources:
            debugger = data_sources["simulation_debugger"]
            if hasattr(debugger, "get_step_history"):
                history = debugger.get_step_history()
                metrics["simulation_steps"] = len(history)
                if history:
                    first_step = history[0].get("step", 0)
                    last_step = history[-1].get("step", 0)
                    metrics["simulation_steps"] = last_step - first_step + 1
                    metrics["simulation_duration"] = history[-1].get("timestamp", 0) - history[0].get("timestamp", 0)

        # Extract performance analysis
        if "performance_profiler" in data_sources:
            profiler = data_sources["performance_profiler"]
            if hasattr(profiler, "profiling_data"):
                metrics["performance_stats"] = profiler.profiling_data

        if metrics["simulation_duration"] > 0:
            metrics["steps_per_second"] = metrics["simulation_steps"] / metrics["simulation_duration"]

        return metrics

    def _extract_memory_usage(self, data_sources: Dict[str, Any]) -> Dict[str, Any]:
        """Extract memory usage information."""
        memory = {
            "timestamp": time.time(),
            "memory_monitor": None,
            "memory_analysis": {},
            "leak_detection": {}
        }

        # Extract from memory monitor
        if "memory_monitor" in data_sources:
            monitor = data_sources["memory_monitor"]
            if hasattr(monitor, "get_memory_stats"):
                memory["memory_monitor"] = monitor.get_memory_stats()
            if hasattr(monitor, "detect_memory_leak"):
                memory["leak_detection"] = monitor.detect_memory_leak()

        return memory

    def _extract_performance_analysis(self, data_sources: Dict[str, Any]) -> Dict[str, Any]:
        """Extract performance analysis."""
        performance = {
            "timestamp": time.time(),
            "bottlenecks": [],
            "optimization_opportunities": [],
            "component_performance": {}
        }

        # Extract from performance profiler
        if "performance_profiler" in data_sources:
            profiler = data_sources["performance_profiler"]
            if hasattr(profiler, "analyze_bottlenecks"):
                performance["bottlenecks"] = profiler.analyze_bottlenecks()

        return performance

    def _extract_errors_and_warnings(self, data_sources: Dict[str, Any]) -> Dict[str, Any]:
        """Extract errors and warnings information."""
        errors = {
            "timestamp": time.time(),
            "error_reporter": None,
            "error_statistics": {},
            "recent_errors": []
        }

        # Extract from error reporter
        if "error_reporter" in data_sources:
            reporter = data_sources["error_reporter"]
            if hasattr(reporter, "get_error_statistics"):
                errors["error_statistics"] = reporter.get_error_statistics()
            if hasattr(reporter, "get_recent_errors"):
                errors["recent_errors"] = reporter.get_recent_errors(10)

        return errors

    def _extract_visualizations(self, data_sources: Dict[str, Any]) -> Dict[str, Any]:
        """Extract visualization information."""
        visualizations = {
            "timestamp": time.time(),
            "visualization_data": [],
            "visualization_stats": {}
        }

        # Extract from visualization debugger
        if "visualization_debugger" in data_sources:
            debugger = data_sources["visualization_debugger"]
            if hasattr(debugger, "debug_sessions"):
                visualizations["visualization_data"] = debugger.debug_sessions

        return visualizations

    def _generate_recommendations(self, report: DebugReport) -> List[str]:
        """Generate recommendations based on report data."""
        recommendations = []

        # Memory-related recommendations
        if report.memory_usage.get("leak_detection", {}).get("status") == "leak_detected":
            recommendations.append(
                "Memory leak detected - investigate and fix memory management issues"
            )

        # Performance recommendations
        if report.performance_analysis.get("bottlenecks"):
            for bottleneck in report.performance_analysis["bottlenecks"]:
                if bottleneck.get("severity") == "high":
                    recommendations.append(
                        f"High severity bottleneck in {bottleneck.get('component', 'unknown')}: {bottleneck.get('issue', 'unknown')}"
                    )

        # Error-related recommendations
        if report.errors_and_warnings.get("error_statistics", {}).get("total_errors", 0) > 10:
            recommendations.append(
                "High error rate - review error patterns and implement error handling improvements"
            )

        # Simulation recommendations
        if report.simulation_metrics.get("steps_per_second", 0) < 1.0:
            recommendations.append(
                "Slow simulation performance - consider performance optimization"
            )

        if not recommendations:
            recommendations.append("All systems operating within normal parameters")

        return recommendations

    def _save_report(self, report: DebugReport) -> None:
        """Save debug report to file."""
        if not self.output_dir:
            return

        # Save as JSON
        with open(json_path, 'w') as f:
            # Convert to serializable format
            report_dict = asdict(report)
            # Convert datetime objects to strings
            if "generated_at" in report_dict:
                report_dict["generated_at"] = report.generated_at

            json.dump(report_dict, f, indent=2, default=str)

        # Save as YAML
        if HAS_YAML:
            yaml_path = self.output_dir / f"{report.report_id}.yaml"
            with open(yaml_path, 'w') as f:
                yaml.dump(asdict(report), f, default_flow_style=False)
        else:
            warnings.warn("PyYAML is not installed. Skipping YAML export.")

        logger.info(f"Saved debug report to {json_path}")
        if HAS_YAML:
            logger.info(f" and {yaml_path}")


class StateExporter:
    """Export simulation state for analysis."""

    def __init__(self, export_dir: Optional[str] = None):
        """
        Initialize state exporter.

        Parameters
        ----------
        export_dir : str, optional
            Directory for exported state files
        """
        self.export_dir = Path(export_dir) if export_dir else None
        if self.export_dir:
            self.export_dir.mkdir(parents=True, exist_ok=True)

    def export_brain_state(self, brain_state_data: Dict[str, Any],
                           export_format: str = "json",
                           filename: Optional[str] = None) -> str:
        """
        Export brain state data.

        Parameters
        ----------
        brain_state_data : Dict[str, Any]
            Brain state data to export
        export_format : str
            Export format ("json", "yaml", "npz")
        filename : str, optional
            Custom filename

        Returns
        -------
        str
            Path to exported file
        """
        if not self.export_dir:
            raise ValueError("Export directory not specified")

        if filename is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"brain_state_{timestamp}"

        export_path = self.export_dir / f"{filename}.{export_format}"

        if export_format == "json":
            with open(export_path, 'w') as f:
                json.dump(brain_state_data, f, indent=2, default=str)
        elif export_format == "yaml":
            if not HAS_YAML:
                raise ImportError("YAML export requires PyYAML. Install it with 'pip install pyyaml'.")
            with open(export_path, 'w') as f:
                yaml.dump(brain_state_data, f, default_flow_style=False)
        elif export_format == "npz":
            # Convert numpy arrays to a compressed format
            npz_data = self._convert_to_npz(brain_state_data)
            np.savez_compressed(export_path, **npz_data)
        else:
            raise ValueError(f"Unsupported export format: {export_format}")

        logger.info(f"Exported brain state to {export_path}")
        return str(export_path)

    def export_simulation_state(self, simulation_data: Dict[str, Any],
                               include_snapshots: bool = True,
                               format: str = "json") -> Dict[str, str]:
        """
        Export complete simulation state.

        Parameters
        ----------
        simulation_data : Dict[str, Any]
            Simulation data to export
        include_snapshots : bool
            Whether to include state snapshots
        format : str
            Export format

        Returns
        -------
        Dict[str, str]
            Paths to exported files
        """
        if not self.export_dir:
            raise ValueError("Export directory not specified")

        exported_files = {}
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

        # Export main simulation data
        main_filename = f"simulation_state_{timestamp}"
        main_path = self.export_dir / f"{main_filename}.{format}"

        simulation_export = simulation_data.copy()
        if not include_snapshots and "snapshots" in simulation_export:
            del simulation_export["snapshots"]

        if format == "json":
            with open(main_path, 'w') as f:
                json.dump(simulation_export, f, indent=2, default=str)
        elif format == "yaml":
            if not HAS_YAML:
                raise ImportError("YAML export requires PyYAML. Install it with 'pip install pyyaml'.")
            with open(main_path, 'w') as f:
                yaml.dump(simulation_export, f, default_flow_style=False)

        exported_files["main"] = str(main_path)

        # Export snapshots if requested and available
        if include_snapshots and "snapshots" in simulation_data:
            snapshots_path = self.export_dir / f"simulation_snapshots_{timestamp}.{format}"
            if format == "json":
                with open(snapshots_path, 'w') as f:
                    json.dump({"snapshots": simulation_data["snapshots"]}, f, indent=2)
            elif format == "yaml":
                if not HAS_YAML:
                    raise ImportError("YAML export requires PyYAML. Install it with 'pip install pyyaml'.")
                with open(snapshots_path, 'w') as f:
                    yaml.dump({"snapshots": simulation_data["snapshots"]}, f, indent=2)

            exported_files["snapshots"] = str(snapshots_path)

        # Export metadata
        metadata_path = self.export_dir / f"simulation_metadata_{timestamp}.json"
        with open(metadata_path, 'w') as f:
            metadata = {
                "export_time": time.time(),
                "export_format": format,
                "include_snapshots": include_snapshots,
                "simulation_steps": simulation_data.get("steps", 0),
                "brain_regions_count": len(simulation_data.get("brain_regions", {})),
                "neuron_count": sum(
                    len(pop) if isinstance(pop, (list, dict)) else 1
                    for pop in simulation_data.get("neuron_populations", {}).values()
                )
            }
            json.dump(metadata, f, indent=2)

        exported_files["metadata"] = str(metadata_path)

        logger.info(f"Exported simulation state to {len(exported_files)} files")
        return exported_files

    def _convert_to_npz(self, data: Dict[str, Any]) -> Dict[str, np.ndarray]:
        """Convert data to numpy arrays for NPZ export."""
        npz_data = {}

        for key, value in data.items():
            if isinstance(value, np.ndarray):
                npz_data[key] = value
            elif isinstance(value, (list, tuple)):
                npz_data[key] = np.array(value)
            elif isinstance(value, dict):
                # Recursively convert nested dicts
                for sub_key, sub_value in value.items():
                    if isinstance(sub_value, np.ndarray):
                        npz_data[f"{key}_{sub_key}"] = sub_value
                    elif isinstance(sub_value, (list, tuple)):
                        npz_data[f"{key}_{sub_key}"] = np.array(sub_value)

        return npz_data


class ConfigurationExporter:
    """Export configuration and settings."""

    def __init__(self, config_dir: Optional[str] = None):
        """
        Initialize configuration exporter.

        Parameters
        ----------
        config_dir : str, optional
            Directory for configuration files
        """
        self.config_dir = Path(config_dir) if config_dir else None
        if self.config_dir:
            self.config_dir.mkdir(parents=True, exist_ok=True)

    def export_configuration(self, config_data: Dict[str, Any],
                            filename: Optional[str] = None,
                            format: str = "json") -> str:
        """
        Export configuration data.

        Parameters
        ----------
        config_data : Dict[str, Any]
            Configuration data to export
        filename : str, optional
            Custom filename
        format : str
            Export format ("json", "yaml", "ini", "csv")

        Returnsn        -------
        str
            Path to exported file
        """
        if not self.config_dir:
            raise ValueError("Config directory not specified")

        if filename is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"configuration_{timestamp}"

        export_path = self.config_dir / f"{filename}.{format}"

        if format == "json":
            with open(export_path, 'w') as f:
                json.dump(config_data, f, indent=2, default=str)
        elif format == "yaml":
            if not HAS_YAML:
                raise ImportError("YAML export requires PyYAML. Install it with 'pip install pyyaml'.")
            with open(export_path, 'w') as f:
                yaml.dump(config_data, f, default_flow_style=False)
        elif format == "ini":
            with open(export_path, 'w') as f:
                for section, values in config_data.items():
                    f.write(f"[{section}]\n")
                    if isinstance(values, dict):
                        for key, value in values.items():
                            f.write(f"{key} = {value}\n")
                    f.write("\n")
        elif format == "csv":
            self._export_csv(config_data, export_path)
        else:
            raise ValueError(f"Unsupported export format: {format}")

        logger.info(f"Exported configuration to {export_path}")
        return str(export_path)

    def _export_csv(self, config_data: Dict[str, Any], output_path: Path) -> None:
        """Export configuration to CSV format."""
        with open(output_path, 'w', newline='') as f:
            writer = csv.writer(f)

            # Write header
            writer.writerow(["Section", "Key", "Value", "Type"])

            # Write configuration data
            for section, values in config_data.items():
                if isinstance(values, dict):
                    for key, value in values.items():
                        writer.writerow([
                            section,
                            key,
                            str(value),
                            type(value).__name__
                        ])


class PerformanceReportGenerator:
    """Generate performance reports."""

    def __init__(self, report_dir: Optional[str] = None):
        """
        Initialize performance report generator.

        Parameters
        ----------
        report_dir : str, optional
            Directory for performance reports
        """
        self.report_dir = Path(report_dir) if report_dir else None
        if self.report_dir:
            self.report_dir.mkdir(parents=True, exist_ok=True)

    def generate_performance_report(self, performance_data: Dict[str, Any],
                                   report_type: str = "detailed") -> Dict[str, str]:
        """
        Generate performance report.

        Parameters
        ----------
        performance_data : Dict[str, Any]
            Performance data to report on
        report_type : str
            Report type ("summary", "detailed", "comprehensive")

        Returnsn        -------
        Dict[str, str]
            Paths to generated report files
        """
        if not self.report_dir:
            raise ValueError("Report directory not specified")

        reports = {}
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

        if report_type == "summary":
            reports["summary"] = self._generate_summary_report(performance_data, timestamp)
        elif report_type == "detailed":
            reports["detailed"] = self._generate_detailed_report(performance_data, timestamp)
        elif report_type == "comprehensive":
            reports["summary"] = self._generate_summary_report(performance_data, timestamp)
            reports["detailed"] = self._generate_detailed_report(performance_data, timestamp)
            reports["analysis"] = self._generate_analysis_report(performance_data, timestamp)

        logger.info(f"Generated performance report: {len(reports)} reports")
        return reports

    def _generate_summary_report(self, data: Dict[str, Any], timestamp: str) -> str:
        """Generate summary performance report."""
        report_path = self.report_dir / f"performance_summary_{timestamp}.json"

        summary = {
            "timestamp": time.time(),
            "report_type": "summary",
            "overall_performance": self._calculate_overall_performance(data),
            "key_metrics": self._extract_key_metrics(data),
            "performance_trends": self._extract_performance_trends(data),
            "recommendations": self._generate_performance_recommendations(data)
        }

        with open(report_path, 'w') as f:
            json.dump(summary, f, indent=2, default=str)

        return str(report_path)

    def _generate_detailed_report(self, data: Dict[str, Any], timestamp: str) -> str:
        """Generate detailed performance report."""
        report_path = self.report_dir / f"performance_detailed_{timestamp}.json"

        detailed = {
            "timestamp": time.time(),
            "report_type": "detailed",
            "component_performance": self._extract_component_performance(data),
            "resource_usage": self._extract_resource_usage(data),
            "performance_by_time": self._extract_performance_by_time(data),
            "bottleneck_analysis": self._analyze_bottlenecks(data),
            "optimization_opportunities": self._identify_optimization_opportunities(data)
        }

        with open(report_path, 'w') as f:
            json.dump(detailed, f, indent=2, default=str)

        return str(report_path)

    def _generate_analysis_report(self, data: Dict[str, Any], timestamp: str) -> str:
        """Generate analysis performance report."""
        report_path = self.report_dir / f"performance_analysis_{timestamp}.json"

        analysis = {
            "timestamp": time.time(),
            "report_type": "analysis",
            "statistical_analysis": self._perform_statistical_analysis(data),
            "comparative_analysis": self._perform_comparative_analysis(data),
            "prediction_analysis": self._perform_prediction_analysis(data),
            "risk_assessment": self._perform_risk_assessment(data)
        }

        with open(report_path, 'w') as f:
            json.dump(analysis, f, indent=2, default=str)

        return str(report_path)

    def _calculate_overall_performance(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate overall performance metrics."""
        # This would calculate composite performance scores
        return {
            "performance_score": 0.0,
            "performance_grade": "N/A",
            "status": "unknown"
        }

    def _extract_key_metrics(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Extract key performance metrics."""
        # Extract key metrics from performance profiler and other sources
        return {}

    def _extract_performance_trends(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Extract performance trends over time."""
        return {}

    def _generate_performance_recommendations(self, data: Dict[str, Any]) -> List[str]:
        """Generate performance recommendations."""
        return []

    def _extract_component_performance(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Extract component performance."""
        return {}

    def _extract_resource_usage(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Extract resource usage."""
        return {}

    def _extract_performance_by_time(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Extract performance by time."""
        return {}

    def _analyze_bottlenecks(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze bottlenecks."""
        return {}

    def _identify_optimization_opportunities(self, data: Dict[str, Any]) -> List[str]:
        """Identify optimization opportunities."""
        return []

    def _perform_statistical_analysis(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform statistical analysis."""
        return {}

    def _perform_comparative_analysis(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform comparative analysis."""
        return {}

    def _perform_prediction_analysis(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform prediction analysis."""
        return {}

    def _perform_risk_assessment(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Perform risk assessment."""
        return {}


# Convenience instances
debug_report_generator = DebugReportGenerator()
state_exporter = StateExporter()
configuration_exporter = ConfigurationExporter()
performance_report_generator = PerformanceReportGenerator()


def get_debug_report_generator() -> DebugReportGenerator:
    """Get debug report generator instance."""
    return debug_report_generator


def get_state_exporter() -> StateExporter:
    """Get state exporter instance."""
    return state_exporter


def get_configuration_exporter() -> ConfigurationExporter:
    """Get configuration exporter instance."""
    return configuration_exporter


def get_performance_report_generator() -> PerformanceReportGenerator:
    """Get performance report generator instance."""
    return performance_report_generator