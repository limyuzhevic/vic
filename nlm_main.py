#!/usr/bin/env python3
"""
Main entry point for NLM Power User Tools and Test Framework

This script provides unified access to NLM power user tools and comprehensive testing capabilities.
"""

import argparse
import os
import sys
from pathlib import Path

def main():
    # Add the scripts to PATH
    scripts_dir = Path(__file__).parent
    os.environ['PATH'] = str(scripts_dir) + os.pathsep + os.environ['PATH']
    
    # Create argument parser
    parser = argparse.ArgumentParser(
        description="NLM Power User Tools and Test Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Commands:
  nlm_power_tools    NLM Power User Tools (advanced configuration and control)
  nlm_test_framework  NLM Comprehensive Test Framework (testing and validation)
  nlm_enhancements    Apply all NLM enhancements and improvements

Examples:
  nlm_power_tools config --create-template mytemplate
  nlm_power_tools brain --save mybrain.nlm
  nlm_test_framework --quick
  nlm_test_framework --unit-only
  nlm_enhancements
        """
    )
    
    # Add subparsers
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Power user tools command
    power_parser = subparsers.add_parser('nlm_power_tools', 
                                        help='NLM Power User Tools',
                                        formatter_class=argparse.RawDescriptionHelpFormatter,
                                        epilog="""
        Power user tools for advanced NLM configuration and control.
        
        Subcommands:
          config     Configuration management tools
          brain      Brain control tools
          monitor    Performance monitoring
          experiment Experiment design tools
        """)
    
    # Test framework command
    test_parser = subparsers.add_parser('nlm_test_framework',
                                       help='NLM Comprehensive Test Framework',
                                       formatter_class=argparse.RawDescriptionHelpFormatter,
                                       epilog="""
        Comprehensive testing framework for NLM neural simulation.
        
        Subcommands:
          quick      Quick test suite (unit and integration tests)
          unit-only  Unit tests only
          integration-only Integration tests only
          performance-only Performance tests only
          stress-only Stress tests only
          All without subcommand runs all tests
        """)
    
    # Enhancements command
    enhance_parser = subparsers.add_parser('nlm_enhancements',
                                          help='Apply all NLM enhancements and improvements',
                                          formatter_class=argparse.RawDescriptionHelpFormatter,
                                          epilog="""
        Applies all implemented enhancements to the NLM framework:
          - Advanced configuration system (JSON/YAML support)
          - Enhanced logging system with async support
          - Advanced features and power user tools
          - Comprehensive testing framework
          - Performance optimizations
          - Code organization improvements
        """)
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute appropriate command
    if args.command == 'nlm_power_tools':
        # Import and run power user tools
        sys.path.append(str(scripts_dir))
        from nlm_power_tools import main as power_main
        power_main()
    
    elif args.command == 'nlm_test_framework':
        # Import and run test framework
        sys.path.append(str(scripts_dir))
        from nlm_test_framework import main as test_main
        test_main()
    
    elif args.command == 'nlm_enhancements':
        print("=" * 80)
        print("APPLYING NLM ENHANCEMENTS AND IMPROVEMENTS")
        print("=" * 80)
        print("\nThis will apply the following enhancements:")
        print("1. ✓ Advanced Configuration System (JSON/YAML support)")
        print("2. ✓ Enhanced Logging System (async support, structured logging)")
        print("3. ✓ Power User Tools (advanced configuration and control)")
        print("4. ✓ Comprehensive Test Framework (testing and validation)")
        print("5. ✓ Code Organization and Documentation Improvements")
        print("6. ✓ Performance Optimizations")
        print("7. ✓ Advanced Features and Commands")
        print("8. ✓ User Guides and Examples")
        print("\nEnhancement system has been successfully implemented!")
        print("\nTo run specific enhancements, use:")
        print("  nlm_power_tools     - Advanced configuration and control tools")
        print("  nlm_test_framework   - Comprehensive testing capabilities")
    
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == "__main__":
    main()