#!/usr/bin/env python3
"""
Base Module for NLM Profiling Utilities

Core classes and utilities used by all profiling modules.
Provides common functionality for performance measurement, benchmarking, and analysis.
"""

import time
import json
import csv
import logging
from abc import ABC, abstractmethod
from dataclasses import dataclass, asdict
from typing import Any, Dict, List, Optional, Callable, Union
from enum import Enum
from datetime import datetime
from pathlib import Path
import sys

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)
logger = logging.getLogger("nlm_profiling")


class OutputFormat(Enum):
    """Output format enumeration."""
    TEXT = "text"
    JSON = "json"
    CSV = "csv"
    PRETTY_JSON = "pretty_json"


@dataclass
class PerformanceMetrics:
    """Base class for performance metrics."""
    timestamp: float
    metric_name: str
    value: float
    unit: str = ""
    metadata: Dict[str, Any] = None

    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        result = asdict(self)
        if self.metadata:
            result["metadata"] = self.metadata
        return result

    def to_text(self) -> str:
        """Convert to text representation."""
        return f"{self.metric_name}: {self.value} {self.unit}"


@dataclass
class BenchmarkResult:
    """Benchmark test results."""
    test_name: str
    parameters: Dict[str, Any]
    metrics: List[PerformanceMetrics]
    duration: float
    success: bool
    error_message: Optional[str] = None

    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "test_name": self.test_name,
            "parameters": self.parameters,
            "metrics": [m.to_dict() for m in self.metrics],
            "duration": self.duration,
            "success": self.success,
            "error_message": self.error_message
        }


class OutputFormatter:
    """Output formatter for different formats."""
    
    @staticmethod
    def format_text(data: Any) -> str:
        """Format data as text."""
        if isinstance(data, list):
            return "\n".join([item.to_text() if hasattr(item, 'to_text') else str(item) for item in data])
        elif isinstance(data, dict):
            return "\n".join([f"{k}: {v}" for k, v in data.items()])
        elif hasattr(data, 'to_text'):
            return data.to_text()
        else:
            return str(data)
    
    @staticmethod
    def format_json(data: Any, pretty: bool = False) -> str:
        """Format data as JSON."""
        indent = 2 if pretty else None
        return json.dumps(data, indent=indent, default=str)
    
    @staticmethod
    def format_csv(data: List[Dict[str, Any]], headers: List[str] = None) -> str:
        """Format data as CSV."""
        if not data:
            return ""
        
        output = []
        if headers:
            output.append(",".join(headers))
        
        for row in data:
            if headers:
                values = [str(row.get(header, "")) for header in headers]
            else:
                values = [str(value) for value in row.values()]
            output.append(",".join(values))
        
        return "\n".join(output)


class ProfilingFramework:
    """Base class for profiling utilities."""
    
    def __init__(self, name: str):
        self.name = name
        self.results = []
        self.formatter = OutputFormatter()
        self.logger = logging.getLogger(f"nlm_profiling.{name}")
    
    def run_profiling(self, **kwargs) -> BenchmarkResult:
        """Run profiling and return results."""
        start_time = time.time()
        try:
            self.logger.info(f"Starting {self.name} profiling")
            result = self._profile(**kwargs)
            duration = time.time() - start_time
            
            self.logger.info(f"Completed {self.name} profiling in {duration:.2f}s")
            return result
        except Exception as e:
            self.logger.error(f"Error in {self.name} profiling: {e}")
            return BenchmarkResult(
                test_name=self.name,
                parameters=kwargs,
                metrics=[],
                duration=time.time() - start_time,
                success=False,
                error_message=str(e)
            )
    
    @abstractmethod
    def _profile(self, **kwargs) -> BenchmarkResult:
        """Implement profiling logic."""
        pass
    
    def export_results(self, format: OutputFormat, filepath: Optional[str] = None) -> str:
        """Export results in specified format."""
        if not self.results:
            return "No results to export"
        
        if format == OutputFormat.TEXT:
            output = self.formatter.format_text(self.results)
        elif format == OutputFormat.JSON:
            output = self.formatter.format_json(self.results.to_dict() if hasattr(self.results, 'to_dict') else self.results)
        elif format == OutputFormat.CSV:
            data = self.results.to_dict() if hasattr(self.results, 'to_dict') else []
            headers = list(data.keys()) if data and isinstance(data, dict) else None
            output = self.formatter.format_csv([data] if isinstance(data, dict) else data, headers)
        elif format == OutputFormat.PRETTY_JSON:
            output = self.formatter.format_json(self.results.to_dict() if hasattr(self.results, 'to_dict') else self.results, pretty=True)
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        if filepath:
            with open(filepath, 'w') as f:
                f.write(output)
        
        return output


# CLI interface for standalone usage
if __name__ == "__main__":
    import argparse
    
    def main():
        parser = argparse.ArgumentParser(description="NLM Profiling Utilities")
        parser.add_argument("--utility", required=True, choices=["performance_measurement", "benchmark", "memory", "spike", "learning", "timeline"],
                          help="Type of profiling utility to use")
        parser.add_argument("--steps", type=int, default=100, help="Number of steps to profile")
        parser.add_argument("--runs", type=int, default=3, help="Number of runs for benchmarking")
        parser.add_argument("--output", type=str, help="Output file path")
        parser.add_argument("--format", choices=["text", "json", "csv"], default="text", help="Output format")
        parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
        
        args = parser.parse_args()
        
        # Setup logging
        level = logging.DEBUG if args.verbose else logging.INFO
        logging.basicConfig(level=level)
        
        # Create and run the profiling utility
        utility = ProfilingFactory.create_utility(args.utility)
        result = utility.run_profiling(steps=args.steps, runs=args.runs)
        
        # Output results
        if args.format == "text":
            print(OutputFormatter.format_text(result))
        elif args.format == "json":
            print(OutputFormatter.format_json(result.to_dict() if hasattr(result, 'to_dict') else result))
        elif args.format == "csv":
            print(OutputFormatter.format_csv([result.to_dict() if hasattr(result, 'to_dict') else result]))
        
        # Export to file if specified
        if args.output:
            format_enum = OutputFormat(args.format)
            utility.export_results(format_enum, args.output)
    
    main()
