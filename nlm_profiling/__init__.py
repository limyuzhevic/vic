"""
NLM Profiling Utilities

Comprehensive Performance Profiling Framework for the NLM (Neural Learning Machine) Project

This package provides a complete set of profiling utilities designed to help users:
- Identify performance bottlenecks
- Optimize brain configurations  
- Understand learning dynamics
- Compare different simulation approaches
- Make data-driven optimization decisions

Each profiling utility is designed to:
- Use modern Python profiling and measurement techniques
- Provide detailed, actionable performance insights
- Support multiple output formats (text, JSON, CSV)
- Integrate with the brain simulation lifecycle
- Provide actionable optimization recommendations
- Include comprehensive documentation
- Be easily extensible for new metrics
"""

__version__ = "1.0.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Core modules
from .base import (
    ProfilingFramework,
    PerformanceMetrics,
    BenchmarkResult,
    OutputFormat,
    OutputFormatter
)

# Profiling utilities
from .performance import (
    PerformanceMeasurementModule,
    PerformanceMeasurementConfig,
    PerformanceMeasurementCLI
)

from .benchmark import (
    BenchmarkHarnessModule,
    BenchmarkConfig,
    Configuration,
    BenchmarkCLInterface
)

from .memory import (
    MemoryProfilerModule,
    MemoryTrackingConfig,
    MemoryAnalysisMode,
    MemoryProfilerCLI
)

from .spike import (
    SpikeRateAnalyzerModule,
    SpikeRateAnalyzerConfig,
    SpikeAnalysisMode,
    SpikePattern,
    SpikeRateAnalyzerCLI
)

from .learning import (
    LearningPerformanceMetricsModule,
    LearningPerformanceMetricsConfig,
    LearningAnalysisMode,
    LearningProgressTracker,
    LearningPerformanceMetricsCLI
)

from .timeline import (
    TimelineProfilerModule,
    TimelineAnalysisConfig,
    TimelineEvent,
    TimelineOperation,
    TimelineProfilerCLI
)

# Export utilities
from .export import (
    ProfilerExporter,
    ExportConfig,
    ExportFactory
)

# Factory for creating utilities
def create_profiling_utility(utility_type: str, **kwargs):
    """
    Factory function to create profiling utilities.
    
    Args:
        utility_type: Type of utility to create
        **kwargs: Configuration parameters
        
    Returns:
        Profiling utility instance
        
    Raises:
        ValueError: If utility type is not recognized
    """
    from .base import ProfilingFactory
    return ProfilingFactory.create_utility(utility_type)


def get_all_utilities():
    """
    Get all available profiling utilities.
    
    Returns:
        Dictionary of utility instances
    """
    from .base import ProfilingFactory
    return ProfilingFactory.get_all_utilities()


def create_exporter():
    """
    Create a profiler exporter.
    
    Returns:
        ProfilerExporter instance
    """
    return ExportFactory.create_exporter()


def create_export_config(format_type: str = "json", **kwargs):
    """
    Create an export configuration.
    
    Args:
        format_type: Output format type
        **kwargs: Additional configuration parameters
        
    Returns:
        ExportConfig instance
    """
    return ExportFactory.create_config(format_type, **kwargs)


# Version and metadata
__all__ = [
    # Core modules
    'ProfilingFramework',
    'PerformanceMetrics', 
    'BenchmarkResult',
    'OutputFormat',
    'OutputFormatter',
    
    # Profiling utilities
    'PerformanceMeasurementModule',
    'PerformanceMeasurementConfig',
    'PerformanceMeasurementCLI',
    
    'BenchmarkHarnessModule',
    'BenchmarkConfig',
    'Configuration',
    'BenchmarkCLInterface',
    
    'MemoryProfilerModule',
    'MemoryTrackingConfig',
    'MemoryAnalysisMode',
    'MemoryProfilerCLI',
    
    'SpikeRateAnalyzerModule',
    'SpikeRateAnalyzerConfig',
    'SpikeAnalysisMode',
    'SpikePattern',
    'SpikeRateAnalyzerCLI',
    
    'LearningPerformanceMetricsModule',
    'LearningPerformanceMetricsConfig',
    'LearningAnalysisMode',
    'LearningProgressTracker',
    'LearningPerformanceMetricsCLI',
    
    'TimelineProfilerModule',
    'TimelineAnalysisConfig',
    'TimelineEvent',
    'TimelineOperation',
    'TimelineProfilerCLI',
    
    # Export utilities
    'ProfilerExporter',
    'ExportConfig',
    'ExportFactory',
    
    # Factory functions
    'create_profiling_utility',
    'get_all_utilities',
    'create_exporter',
    'create_export_config',
    
    # Package metadata
    '__version__',
    '__author__',
    '__license__'
]

# Package metadata
__metadata__ = {
    'version': __version__,
    'author': __author__,
    'license': __license__,
    'description': "Comprehensive Performance Profiling Framework for NLM",
    'homepage': "https://github.com/nlm-project/nlm-profiling",
    'features': [
        "Performance Measurement Framework",
        "Benchmark Harness",
        "Memory Profiler", 
        "Spike Rate Analyzer",
        "Learning Performance Metrics",
        "Timeline Profiler",
        "Multiple Output Formats",
        "CLI Integration",
        "Comprehensive Documentation",
        "Extensible Architecture"
    ],
    'supported_formats': ["text", "json", "csv", "pretty_json"],
    'dependencies': [
        "pynlm >= 0.1.0",
        "numpy >= 1.20",
        "psutil >= 5.8",
        "pandas >= 1.3"
    ]
}

# Initialize logging
import logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

logger = logging.getLogger("nlm_profiling")

# Log package initialization
logger.info(f"NLM Profiling Utilities v{__version__} initialized successfully")
logger.info(f"Package loaded with {len(__all__)} modules and utilities")

# Provide help function
def help():
    """Display help information about NLM Profiling Utilities."""
    help_text = """
NLM Profiling Utilities v{version}

Purpose:
  Comprehensive performance profiling framework for the NLM (Neural Learning Machine) project.

Available Modules:
  - Performance Measurement Framework: Core utilities for measuring brain performance metrics
  - Benchmark Harness: System for comparing different brain configurations
  - Memory Profiler: Memory usage monitoring and analysis
  - Spike Rate Analyzer: Tools for analyzing neural spike patterns
  - Learning Performance Metrics: Tools for measuring learning efficiency and convergence
  - Timeline Profiler: Detailed timeline analysis of brain operations
  - Export Utilities: Export and reporting functionality

Usage Examples:
  # Create and run performance measurement
  from nlm_profiling import PerformanceMeasurementModule
  module = PerformanceMeasurementModule(steps=100)
  result = module.run_profiling()
  
  # Export results in different formats
  from nlm_profiling import create_exporter, OutputFormat
  exporter = create_exporter()
  exporter.export_profiling_results(result, OutputFormat.JSON)
  
  # Use CLI interface
  python -m nlm_profiling.performance_measurement --steps 100 --json
  python -m nlm_profiling.benchmark --config-type high_performance
  python -m nlm_profiling.memory --steps 500
  python -m nlm_profiling.spike --analysis-mode statistical
  python -m nlm_profiling.learning --analysis-mode convergence
  python -m nlm_profiling.timeline --analysis-depth detailed

For more information, visit: {homepage}
    """.format(**__metadata__)
    print(help_text)