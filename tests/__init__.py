import pytest
import sys
import os
import tempfile
import shutil
import time
from pathlib import Path

# Add the path to import pynlm
try:
    sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_9f55cb1a-1e5d-4e86-b53a-94257876359f')
    import pynlm
except ImportError as e:
    pytest.skip(f"pynlm module not available: {e}")

# Check that pynlm was imported successfully
assert pynlm is not None, "Failed to import pynlm"

# Constants for testing
TEST_NAME = "pynlm test suite"
pytestmark = pytest.mark.skipif(
    not hasattr(sys.modules.get('pynlm'), '__version__'),
    reason="pynlm module is not properly installed"
)

# Import test modules for collection
from .test_basic_bindings import TestBasicBindings
from .test_integration import TestIntegration
from .test_memory_systems import TestMemorySystems
from .test_neuromodulation import TestNeuromodulation
from .test_checkpoints import TestCheckpoints
from .test_error_handling import TestErrorHandling
from .test_performance import TestPerformance
