#!/usr/bin/env python3

"""
NLM Configuration Module - Configuration Management and Templates

This module provides advanced configuration management:
- Configuration management with template support
- Template-based configuration with variables and inheritance
- Configuration profiling and optimization
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

# Re-export configuration classes for easy access
from nlm.config.manager import ConfigManager
from nlm.config.templates import ConfigTemplate
from nlm.config.profiling import ConfigProfiler

__all__ = [
    "ConfigManager", "ConfigTemplate", "ConfigProfiler",
    "__version__", "__author__"
]