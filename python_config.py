#!/usr/bin/env python3
"""
Python Binding Configuration for NLM

This module provides configuration management for Python bindings,
optimization settings, and runtime configuration for NLM's Python interface.

Key Features:
- Runtime configuration management
- Python binding optimization settings
- Feature toggle management
- Cross-platform compatibility
"""

import os
import sys
import json
import platform
from typing import Dict, Any, Optional, Union
from pathlib import Path

# Global configuration dictionary
_config = {}

# Default configuration values
_DEFAULT_CONFIG = {
    "bindings": {
        "module_name": "nlm",
        "python_module": "pynlm",
        "extension_name": "nlm",
        "enable_pybind11": True,
        "enable_python_tests": True,
        "enable_python_examples": True,
        "enable_python_docs": False,
        "python_version_min": "3.8",
        "python_version_max": "3.12",
        "threading_model": "dynamic",  # "static", "dynamic", "auto"
        "gil_state_management": "enabled",  # "enabled", "disabled", "permissive"
        "numpy_support": True,
        "scipy_support": False,
    },
    "optimization": {
        "enable_simd": True,
        "enable_openmp": True,
        "enable_cuda": False,
        "enable_mpi": False,
        "optimization_level": "2",  # "0", "1", "2", "3"
        "warnings_as_errors": False,
        "verbose_logging": False,
        "profile_bindings": False,
        "coverage_enabled": False,
        "sanitizers_enabled": False,
        "asan_enabled": False,
        "ubsan_enabled": False,
    },
    "platform": {
        "system": platform.system(),
        "architecture": platform.machine(),
        "python_implementation": platform.python_implementation(),
        "python_version": platform.python_version(),
        "compiler": "unknown",
    },
    "paths": {
        "build_dir": "build",
        "install_dir": "python_install",
        "include_dir": "python/include",
        "library_dir": "lib",
        "config_file": "python_config.json",
    },
    "runtime": {
        "max_threads": os.cpu_count() or 4,
        "memory_limit_mb": 1024,
        "simulation_step": 0.001,
        "random_seed": 42,
        "enable_logging": True,
        "log_level": "INFO",
        "log_file": "nlm_python.log",
    },
    "testing": {
        "test_paths": ["tests", "python/tests"],
        "pytest_options": ["-v", "--tb=short"],
        "coverage_threshold": 80.0,
        "benchmark_enabled": False,
    },
    "development": {
        "enable_linting": True,
        "enable_type_checking": True,
        "black_config": {
            "line_length": 88,
            "include": '*.py',
            "exclude": '''(
                /(
                    \.eggs
                    | \.git
                    | \.hg
                    | \.mypy_cache
                    | \.tox
                    | \.venv
                    | _build
                    | buck-out
                    | build
                    | dist
                )/
            )''',
        },
        "isort_config": {
            "profile": "black",
            "multi_line_output": 3,
            "include_trailing_comma": True,
            "force_grid_wrap": 0,
            "use_parentheses": True,
            "ensure_newline_before_comments": True,
            "line_length": 88,
        },
        "mypy_config": {
            "python_version": "3.8",
            "warn_return_any": True,
            "warn_unused_configs": True,
            "disallow_untyped_defs": True,
            "ignore_missing_imports": True,
        },
    },
}


def load_config(config_file: Optional[str] = None) -> Dict[str, Any]:
    """
    Load configuration from file or use defaults.
    
    Args:
        config_file: Path to configuration file (optional)
    
    Returns:
        Configuration dictionary
    """
    global _config
    
    # Start with defaults
    _config = json.loads(json.dumps(_DEFAULT_CONFIG))
    
    # Load from file if specified
    if config_file and os.path.exists(config_file):
        with open(config_file, 'r') as f:
            file_config = json.load(f)
        
        # Merge file config with defaults
        _merge_config(_config, file_config)
    
    return _config


def _merge_config(default: Dict[str, Any], override: Dict[str, Any]) -> None:
    """
    Recursively merge configuration dictionaries.
    
    Args:
        default: Default configuration
        override: Configuration to merge
    """
    for key, value in override.items():
        if key in default and isinstance(default[key], dict) and isinstance(value, dict):
            _merge_config(default[key], value)
        else:
            default[key] = value


def save_config(config_file: str) -> None:
    """
    Save current configuration to file.
    
    Args:
        config_file: Path to save configuration
    """
    with open(config_file, 'w') as f:
        json.dump(_config, f, indent=2)


def get_config(section: str, key: Optional[str] = None) -> Union[Any, Dict[str, Any]]:
    """
    Get configuration value(s).
    
    Args:
        section: Configuration section name
        key: Optional specific key within section
    
    Returns:
        Configuration value or entire section
    """
    if section not in _config:
        raise ValueError(f"Unknown configuration section: {section}")
    
    if key:
        if key not in _config[section]:
            raise ValueError(f"Unknown configuration key: {key} in section {section}")
        return _config[section][key]
    
    return _config[section]


def set_config(section: str, key: str, value: Any) -> None:
    """
    Set configuration value.
    
    Args:
        section: Configuration section name
        key: Configuration key
        value: Configuration value
    """
    if section not in _config:
        raise ValueError(f"Unknown configuration section: {section}")
    
    _config[section][key] = value


def validate_config() -> bool:
    """
    Validate configuration for consistency.
    
    Returns:
        True if configuration is valid, False otherwise
    """
    config = _config
    
    # Validate Python version
    py_version = config["bindings"]["python_version_min"]
    py_version_max = config["bindings"]["python_version_max"]
    
    # Validate optimization level
    opt_level = config["optimization"]["optimization_level"]
    if opt_level not in ["0", "1", "2", "3"]:
        print(f"Warning: Invalid optimization level '{opt_level}'")
        return False
    
    # Validate threading model
    threading_model = config["bindings"]["threading_model"]
    if threading_model not in ["static", "dynamic", "auto"]:
        print(f"Warning: Invalid threading model '{threading_model}'")
        return False
    
    # Validate GIL state management
    gil_state = config["bindings"]["gil_state_management"]
    if gil_state not in ["enabled", "disabled", "permissive"]:
        print(f"Warning: Invalid GIL state management '{gil_state}'")
        return False
    
    # Validate log level
    log_level = config["runtime"]["log_level"]
    if log_level not in ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]:
        print(f"Warning: Invalid log level '{log_level}'")
        return False
    
    return True


def get_build_args() -> list:
    """
    Generate build arguments based on configuration.
    
    Returns:
        List of build arguments
    """
    config = _config
    build_args = []
    
    # Optimization
    if config["optimization"]["enable_simd"]:
        build_args.extend(["-DNLM_ENABLE_SIMD=ON"])
    
    if config["optimization"]["enable_openmp"]:
        build_args.extend(["-DNLM_ENABLE_OPENMP=ON"])
    
    if config["optimization"]["enable_cuda"]:
        build_args.extend(["-DNLM_ENABLE_CUDA=ON"])
    
    if config["optimization"]["enable_mpi"]:
        build_args.extend(["-DNLM_ENABLE_MPI=ON"])
    
    if config["optimization"]["optimization_level"]:
        build_args.extend([f"-DNLM_BUILD_OPTIMIZATION_LEVEL={config['optimization']['optimization_level']}"])
    
    if config["optimization"]["warnings_as_errors"]:
        build_args.extend(["-DNLM_BUILD_WARNINGS_AS_ERRORS=ON"])
    
    if config["optimization"]["verbose_logging"]:
        build_args.extend(["-DNLM_BUILD_VERBOSE_LOGGING=ON"])
    
    # Bindings
    if not config["bindings"]["enable_pybind11"]:
        build_args.extend(["-DNLM_ENABLE_PYTHON_BINDINGS=OFF"])
    
    if config["bindings"]["enable_python_tests"]:
        build_args.extend(["-DNLM_ENABLE_TESTS=ON"])
    
    if config["bindings"]["enable_python_examples"]:
        build_args.extend(["-DNLM_ENABLE_EXAMPLES=ON"])
    
    if config["bindings"]["enable_python_docs"]:
        build_args.extend(["-DNLM_ENABLE_DOCS=ON"])
    
    return build_args


def get_test_args() -> list:
    """
    Generate test arguments based on configuration.
    
    Returns:
        List of test arguments
    """
    config = _config
    test_args = []
    
    test_args.extend(config["testing"]["pytest_options"])
    
    if config["testing"]["benchmark_enabled"]:
        test_args.extend(["--benchmark"])
    
    return test_args


def print_config() -> None:
    """Print current configuration."""
    print("\n" + "="*60)
    print("NLM Python Configuration")
    print("="*60)
    
    for section_name, section_config in _config.items():
        print(f"\n[{section_name.upper()}]")
        if isinstance(section_config, dict):
            for key, value in section_config.items():
                print(f"  {key}: {value}")
        else:
            print(f"  {section_config}")
    
    print("\n" + "="*60 + "\n")


def main():
    """Main function for command-line interface."""
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Python Configuration Manager")
    parser.add_argument("--config", help="Path to configuration file")
    parser.add_argument("--set", nargs=2, action="append", help="Set configuration value")
    parser.add_argument("--get", nargs=2, action="append", help="Get configuration value")
    parser.add_argument("--validate", action="store_true", help="Validate configuration")
    parser.add_argument("--print", action="store_true", dest="print_config", help="Print configuration")
    parser.add_argument("--save", help="Save configuration to file")
    parser.add_argument("--build-args", action="store_true", help="Generate build arguments")
    parser.add_argument("--test-args", action="store_true", help="Generate test arguments")
    
    args = parser.parse_args()
    
    # Load configuration
    load_config(args.config)
    
    # Execute commands
    if args.set:
        for section, key in args.set:
            value = input(f"Enter value for {section}.{key}: ")
            set_config(section, key, value)
    
    if args.get:
        for section, key in args.get:
            try:
                value = get_config(section, key)
                print(f"{section}.{key} = {value}")
            except ValueError as e:
                print(f"Error: {e}")
    
    if args.validate:
        if validate_config():
            print("Configuration is valid")
        else:
            print("Configuration is invalid")
            sys.exit(1)
    
    if args.print_config:
        print_config()
    
    if args.save:
        save_config(args.save)
        print(f"Configuration saved to {args.save}")
    
    if args.build_args:
        build_args = get_build_args()
        print("Build arguments:")
        for arg in build_args:
            print(f"  {arg}")
    
    if args.test_args:
        test_args = get_test_args()
        print("Test arguments:")
        for arg in test_args:
            print(f"  {arg}")


if __name__ == "__main__":
    main()