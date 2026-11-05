#!/usr/bin/env python3
"""
NLM Demo Script

Advanced demonstration of NLM Python bindings features.
"""

import sys
import os
import argparse

# Add examples directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'examples'))

from basic_brain import main as basic_brain_demo
from agent_world import main as agent_world_demo
from checkpointing import demonstrate_checkpointing, demonstrate_advanced_checkpointing
from analysis import demonstrate_basic_analysis, demonstrate_performance_benchmarking, demonstrate_comprehensive_analysis

def main():
    """Main demo function"""
    parser = argparse.ArgumentParser(
        description="NLM Demo Script - Advanced demonstrations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Available demo types:
  basic   - Basic brain simulation
  agent   - Agent navigation in world
  checkpoint - Save/load brain states
  analysis - Performance analysis and benchmarking
  all     - Run all demonstrations
        """
    )
    
    parser.add_argument(
        "--demo-type",
        "-t",
        choices=["basic", "agent", "checkpoint", "analysis", "all"],
        default="all",
        help="Type of demo to run (default: all)"
    )
    
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--output-dir",
        "-o",
        default="demo_output",
        help="Output directory for demo results (default: demo_output)"
    )
    
    args = parser.parse_args()
    
    # Create output directory
    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)
    
    print("=" * 70)
    print("NLM Python Bindings - Advanced Demo Suite")
    print("=" * 70)
    
    try:
        if args.demo_type == "basic":
            print("\nRunning Basic Brain Simulation Demo...")
            basic_brain_demo()
            
        elif args.demo_type == "agent":
            print("\nRunning Agent Navigation Demo...")
            agent_world_demo()
            
        elif args.demo_type == "checkpoint":
            print("\nRunning Checkpointing Demo...")
            demonstrate_checkpointing()
            demonstrate_advanced_checkpointing()
            
        elif args.demo_type == "analysis":
            print("\nRunning Analysis Demo...")
            demonstrate_basic_analysis()
            demonstrate_performance_benchmarking()
            demonstrate_comprehensive_analysis()
            
        elif args.demo_type == "all":
            print("\nRunning ALL Demos...")
            print("\n1. Basic Brain Simulation Demo:")
            basic_brain_demo()
            
            print("\n2. Agent Navigation Demo:")
            agent_world_demo()
            
            print("\n3. Checkpointing Demo:")
            demonstrate_checkpointing()
            demonstrate_advanced_checkpointing()
            
            print("\n4. Analysis Demo:")
            demonstrate_basic_analysis()
            demonstrate_performance_benchmarking()
            demonstrate_comprehensive_analysis()
        
        print("\n" + "=" * 70)
        print("ALL DEMOS COMPLETED SUCCESSFULLY!")
        print("=" * 70)
        
        # Print summary
        if args.output_dir:
            print(f"\nResults saved to: {os.path.abspath(args.output_dir)}")
        
        return 0
        
    except Exception as e:
        print(f"\nError running demo: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
