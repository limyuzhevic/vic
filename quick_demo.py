#!/usr/bin/env python3
"""
Quick demonstration of the enhanced NLM Python API

This script demonstrates the key features of the enhanced Python API
for NLM (Neural Learning Machine) in a simple, accessible way.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'pynlm'))

from pynlm import create_default_nlm, NLM
from pynlm.utils import get_config_template, run_multiple_experiments

def main():
    print("=" * 60)
    print("NLM Enhanced Python API Quick Demo")
    print("=" * 60)
    print()
    
    print("This demo showcases the enhanced Python API for NLM")
    print("(Neural Learning Machine) with configuration templates")
    print("and batch experiment execution.")
    print()
    
    # Demo 1: Basic usage
    print("1. Creating NLM with default configuration...")
    nlm = create_default_nlm()
    print(f"   ✓ Created NLM: {nlm}")
    
    # Demo 2: Configuration templates
    print("\n2. Available configuration templates...")
    templates = ['small', 'medium', 'large']
    for template in templates:
        try:
            config = get_config_template(template)
            print(f"   - {template.upper()}: {config['neuron_count']:,} neurons")
        except ValueError:
            print(f"   - {template.upper()}: Not available")
    
    # Demo 3: Batch experiment
    print("\n3. Running batch experiments...")
    experiments = [
        {'name': 'quick_test', 'steps': 500},
        {'name': 'medium_test', 'steps': 1000},
    ]
    
    results = run_multiple_experiments(experiments, on_progress=lambda c, t, n: print(f"     Processing: {n}"))
    
    successful = sum(1 for r in results if r.get('success', True))
    print(f"   ✓ Completed {successful}/{len(results)} experiments successfully")
    
    print("\n" + "=" * 60)
    print("Demo completed successfully!")
    print("=" * 60)
    print("\nThe enhanced NLM Python API provides:")
    print("  • Easy configuration management with templates")
    print("  • Batch experiment execution")
    print("  • Performance monitoring")
    print("  • Result analysis and visualization")
    print("  • File operations for experiment management")
    print("\nFor more comprehensive examples, see the examples.py file.")

if __name__ == "__main__":
    main()
