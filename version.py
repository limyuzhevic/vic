# Add the missing __version__ attribute to the pynlm module

# Read version from pyproject.toml
import re
import os

# Parse pyproject.toml to get version
with open(os.path.join(os.path.dirname(__file__), 'pyproject.toml'), 'r') as f:
    content = f.read()
    match = re.search(r'version\s*=\s*"([^"]+)"', content)
    if match:
        __version__ = match.group(1)
    else:
        __version__ = "0.1.0"
