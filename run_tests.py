# Updated test file that properly tests the Python bindings
"""
Python bindings tests for NLM (Neural Learning Machine)

These tests cover the enhanced Python bindings with improved error handling,
Pythonic APIs, and performance optimizations.
"""

import sys
import os
import tempfile
import time
from typing import Dict, Any, List

# Add the Python module to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python', 'pynlm'))

# Import the test module
from test_python_bindings import (
    test_basic_imports,
    test_version_and_metadata,
    test_enhanced_neuron_id,
    test_enhanced_config,
    test_enhanced_agent_brain,
    test_legacy_backwards_compatibility,
    test_brain_context_manager,
    test_batch_operations,
    test_benchmarking,
    test_enum_values,
    test_error_handling,
    run_all_tests,
)

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
