# CLI module - Command-line interface
# ==============================

import argparse
from typing import List

from .enhanced import create_enhanced_cli as enhanced_cli

__all__ = ['create_cli']

def create_cli():
    """Create a command-line interface for NLM utilities.
    
    Returns:
        argparse.ArgumentParser: CLI argument parser
    """
    parser = enhanced_cli()
    
    return parser

# Backward compatibility
__all__.append('create_cli')