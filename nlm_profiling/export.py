#!/usr/bin/env python3
"""
NLM Profiling Utilities Export Module

Export utilities and integration functions for NLM profiling modules.
Provides comprehensive export capabilities and integration with existing NLM tools.
"""

import time
import json
import csv
import logging
from typing import Dict, List, Optional, Any, Union
from pathlib import Path
import sys

from .base import OutputFormat, OutputFormatter

logger = logging.getLogger("nlm_profiling.export")


class ExportConfig:
    """Configuration for export operations."""
    
    def __init__(self, output_format: OutputFormat = OutputFormat.JSON,
                 pretty_print: bool = False, include_metadata: bool = True,
                 file_path: Optional[str] = None):
        self.output_format = output_format
        self.pretty_print = pretty_print
        self.include_metadata = include_metadata
        self.file_path = file_path
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "output_format": self.output_format.value,
            "pretty_print": self.pretty_print,
            "include_metadata": self.include_metadata,
            "file_path": self.file_path
        }


class ProfilerExporter:
    """Export utilities for profiling results."""
    
    def __init__(self):
        self.logger = logging.getLogger("nlm_profiling.export")
    
    def export_profiling_results(self, results: Any, config: ExportConfig) -> str:
        """Export profiling results in specified format."""
        try:
            formatter = OutputFormatter()
            
            if config.output_format == OutputFormat.TEXT:
                output = formatter.format_text(results)
            elif config.output_format == OutputFormat.JSON:
                output = formatter.format_json(results, pretty=config.pretty_print)
            elif config.output_format == OutputFormat.CSV:
                data = results.to_dict() if hasattr(results, 'to_dict') else results
                headers = list(data.keys()) if data and isinstance(data, dict) else None
                output = formatter.format_csv([data] if isinstance(data, dict) else data, headers)
            elif config.output_format == OutputFormat.PRETTY_JSON:
                output = formatter.format_json(results, pretty=True)
            else:
                raise ValueError(f"Unsupported format: {config.output_format}")
            
            # Export to file if specified
            if config.file_path:
                self._write_to_file(output, config.file_path)
            
            return output
            
        except Exception as e:
            self.logger.error(f"Export failed: {e}")
            raise
    
    def _write_to_file(self, content: str, file_path: str) -> None:
        """Write content to file."""
        try:
            path = Path(file_path)
            path.parent.mkdir(parents=True, exist_ok=True)
            
            with open(path, 'w', encoding='utf-8') as f:
                f.write(content)
            
            self.logger.info(f"Results exported to: {file_path}")
            
        except Exception as e:
            self.logger.error(f"Failed to write to file: {e}")
            raise

    def export_multiple_formats(self, results: Any, base_path: str,
                              formats: List[OutputFormat] = None) -> Dict[str, str]:
        """Export results in multiple formats."""
        if formats is None:
            formats = [OutputFormat.TEXT, OutputFormat.JSON, OutputFormat.CSV]
        
        export_results = {}
        
        for format_type in formats:
            config = ExportConfig(
                output_format=format_type,
                file_path=f"{base_path}.{format_type.value}"
            )
            
            try:
                output = self.export_profiling_results(results, config)
                export_results[format_type.value] = output
            except Exception as e:
                self.logger.error(f"Failed to export in format {format_type.value}: {e}")
                export_results[format_type.value] = f"Export failed: {e}"
        
        return export_results

    def export_with_insights(self, results: Any, config: ExportConfig,
                           insights: Dict[str, Any] = None) -> str:
        """Export results with additional insights."""
        try:
            # Merge results with insights
            export_data = {
                "results": results.to_dict() if hasattr(results, 'to_dict') else results,
                "insights": insights or {},
                "export_timestamp": time.time(),
                "export_config": config.to_dict()
            }
            
            # Export the combined data
            output = self.export_profiling_results(export_data, config)
            return output
            
        except Exception as e:
            self.logger.error(f"Failed to export with insights: {e}")
            raise

    def create_report_template(self, template_type: str = "standard") -> Dict[str, Any]:
        """Create a report template for specific use cases."""
        templates = {
            "standard": {
                "title": "NLM Performance Profiling Report",
                "sections": [
                    "Executive Summary",
                    "Performance Metrics",
                    "Benchmark Results",
                    "Learning Analysis",
                    "Timeline Analysis",
                    "Recommendations"
                ],
                "format": "comprehensive"
            },
            "quick": {
                "title": "NLM Quick Performance Summary",
                "sections": [
                    "Performance Summary",
                    "Key Metrics",
                    "Recommendations"
                ],
                "format": "concise"
            },
            "detailed": {
                "title": "NLM Detailed Performance Analysis",
                "sections": [
                    "Executive Summary",
                    "Performance Measurement",
                    "Benchmark Harness",
                    "Memory Profiling",
                    "Spike Rate Analysis",
                    "Learning Performance",
                    "Timeline Analysis",
                    "Comparative Analysis",
                    "Optimization Recommendations"
                ],
                "format": "comprehensive"
            },
            "api": {
                "title": "NLM Profiling API Report",
                "sections": [
                    "API Performance",
                    "Integration Metrics",
                    "Scalability Analysis",
                    "Best Practices"
                ],
                "format": "technical"
            }
        }
        
        return templates.get(template_type, templates["standard"])

    def export_comparison_report(self, results_list: List[Any], comparison_name: str,
                              output_path: str) -> Dict[str, Any]:
        """Export a comparative report across multiple profiling sessions."""
        try:
            comparison_report = {
                "report_title": f"Comparative Analysis: {comparison_name}",
                "timestamp": time.time(),
                "total_sessions": len(results_list),
                "successful_sessions": sum(1 for result in results_list if result.success),
                "failed_sessions": sum(1 for result in results_list if not result.success),
                "comparison_metrics": self._extract_comparison_metrics(results_list),
                "detailed_results": [result.to_dict() for result in results_list if hasattr(result, 'to_dict')]
            }
            
            # Create exporter and export
            exporter = ProfilerExporter()
            config = ExportConfig(
                output_format=OutputFormat.JSON,
                file_path=f"{output_path}_comparison.json"
            )
            
            output = exporter.export_profiling_results(comparison_report, config)
            
            return {
                "success": True,
                "output_path": config.file_path,
                "report": comparison_report,
                "output": output
            }
            
        except Exception as e:
            self.logger.error(f"Failed to create comparison report: {e}")
            return {
                "success": False,
                "error": str(e)
            }
    
    def _extract_comparison_metrics(self, results_list: List[Any]) -> Dict[str, Any]:
        """Extract comparison metrics from multiple results."""
        if not results_list:
            return {}
        
        metrics = {}
        
        # Aggregate metrics across all results
        all_metrics = []
        for result in results_list:
            if hasattr(result, 'metrics'):
                for metric in result.metrics:
                    all_metrics.append(metric.to_dict() if hasattr(metric, 'to_dict') else metric)
        
        # Calculate aggregates
        if all_metrics:
            metric_groups = defaultdict(list)
            for metric in all_metrics:
                metric_groups[metric.get('metric_name', 'unknown')].append(metric.get('value', 0))
            
            for metric_name, values in metric_groups.items():
                if values:
                    metrics[f"{metric_name}_average"] = sum(values) / len(values)
                    metrics[f"{metric_name}_min"] = min(values)
                    metrics[f"{metric_name}_max"] = max(values)
                    metrics[f"{metric_name}_count"] = len(values)
        
        return metrics

    def export_recommendations(self, recommendations: List[str], config: ExportConfig) -> str:
        """Export recommendations in specified format."""
        try:
            report = {
                "recommendations": recommendations,
                "total_recommendations": len(recommendations),
                "generated_at": time.time(),
                "config": config.to_dict()
            }
            
            output = self.export_profiling_results(report, config)
            return output
            
        except Exception as e:
            self.logger.error(f"Failed to export recommendations: {e}")
            raise

    def create_integration_template(self, template_type: str = "cli") -> Dict[str, Any]:
        """Create integration template for specific use cases."""
        templates = {
            "cli": {
                "description": "CLI Integration Template",
                "file_structure": {
                    "main.py": "Entry point for CLI interface",
                    "config.py": "Configuration management",
                    "utils.py": "Utility functions",
                    "export.py": "Export functionality"
                },
                "commands": [
                    "python -m nlm_profiling <utility> --steps <n> --output <file>"
                ]
            },
            "library": {
                "description": "Library Integration Template",
                "file_structure": {
                    "nlm_profiling/__init__.py": "Package initialization",
                    "nlm_profiling/base.py": "Core profiling framework",
                    "nlm_profiling/performance.py": "Performance measurement module",
                    "nlm_profiling/benchmark.py": "Benchmark harness module",
                    "nlm_profiling/memory.py": "Memory profiler module",
                    "nlm_profiling/spike.py": "Spike rate analyzer module",
                    "nlm_profiling/learning.py": "Learning performance metrics module",
                    "nlm_profiling/timeline.py": "Timeline profiler module"
                },
                "usage_example": """
from nlm_profiling import PerformanceMeasurementModule, OutputFormat

# Create performance measurement module
module = PerformanceMeasurementModule()
result = module.run_profiling(steps=100)

# Export results
output = module.export_results(OutputFormat.JSON, "results.json")
"""
            },
            "web": {
                "description": "Web Integration Template",
                "file_structure": {
                    "api.py": "REST API endpoints",
                    "dashboard.py": "Web dashboard interface",
                    "export.py": "Export functionality",
                    "reports.py": "Report generation"
                },
                "endpoints": [
                    "/api/profile/performance",
                    "/api/profile/benchmark",
                    "/api/profile/memory",
                    "/api/export/<format>"
                ]
            }
        }
        
        return templates.get(template_type, templates["cli"])


# Factory function for creating exporters
class ExportFactory:
    """Factory for creating export utilities."""
    
    @staticmethod
    def create_exporter() -> ProfilerExporter:
        """Create a profiler exporter."""
        return ProfilerExporter()
    
    @staticmethod
    def create_config(format_type: str = "json", **kwargs) -> ExportConfig:
        """Create an export config."""
        try:
            output_format = OutputFormat(format_type.lower())
        except ValueError:
            output_format = OutputFormat.JSON
        
        return ExportConfig(
            output_format=output_format,
            pretty_print=kwargs.get('pretty_print', False),
            include_metadata=kwargs.get('include_metadata', True),
            file_path=kwargs.get('file_path')
        )


if __name__ == "__main__":
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        description="NLM Profiling Export Utilities",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python -m nlm_profiling.export --input results.json --output analysis.txt --format text
  python -m nlm_profiling.export --create-template library
  python -m nlm_profiling.export --create-template web
        """
    )
    
    parser.add_argument("--input", type=str, help="Input file path")
    parser.add_argument("--output", type=str, help="Output file path")
    parser.add_argument("--format", choices=["text", "json", "csv"], default="json",
                      help="Output format")
    parser.add_argument("--template", type=str, choices=["cli", "library", "web"],
                      help="Create integration template")
    parser.add_argument("--multiple", action="store_true", help="Export in multiple formats")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Setup logging
    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=level)
    
    # Handle template creation
    if args.template:
        exporter = ProfilerExporter()
        template = exporter.create_integration_template(args.template)
        print(json.dumps(template, indent=2))
        sys.exit(0)
    
    # Handle export operations
    if args.input:
        try:
            # Load input data
            with open(args.input, 'r') as f:
                data = json.load(f)
            
            # Create exporter and export
            exporter = ProfilerExporter()
            config = ExportFactory.create_config(
                format_type=args.format,
                file_path=args.output
            )
            
            output = exporter.export_profiling_results(data, config)
            print(output)
            
        except Exception as e:
            logging.error(f"Export failed: {e}")
            sys.exit(1)
    else:
        parser.print_help()
        sys.exit(0)