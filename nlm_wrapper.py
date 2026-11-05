#!/usr/bin/env python3
"""
NLM Command Line Interface Wrapper
This script provides a simple command-line interface to the NLM advanced CLI.
"""

import os
import sys
from pathlib import Path

# Add the app directory to the path
sys.path.insert(0, str(Path(__file__).parent))

from nlm_cli import NLMCommandLineInterface

if __name__ == "__main__":
    cli = NLMCommandLineInterface()
    cli.main()
