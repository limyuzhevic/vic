#!/usr/bin/env python3
#
# NLM Visualization System - Utils
#

import sys
import os
import json
from pathlib import Path

def create_test_brain():
    """Create a test brain configuration for visualization"""
    brain_config = {
        "name": "TestBrain",
        "version": "1.0.0",
        "description": "Test brain for visualization demonstration",
        "simulation": {
            "steps": 1000,
            "dt": 0.001,
            "neurons": {
                "excitatory": 1000,
                "inhibitory": 200,
                "sensory": 100,
                "motor": 50
            }
        },
        "regions": {
            "cortex": {
                "neurons": 1500,
                "connections": 5000,
                "type": "association"
            },
            "thalamus": {
                "neurons": 500,
                "connections": 1000,
                "type": "relay"
            },
            "hypothalamus": {
                "neurons": 200,
                "connections": 300,
                "type": "modulatory"
            }
        }
    }
    
    with open('test_brain.json', 'w') as f:
        json.dump(brain_config, f, indent=2)
    
    print("Created test_brain.json")

def show_visualization_usage():
    """Show visualization usage examples"""
    print("\n" + "="*60)
    print("NLM Visualization System - Usage Examples")
    print("="*60)
    
    examples = [
        ("Basic Network Visualization", "./nlm visualize --network test_brain.json"),
        ("Real-time Activity Visualization", "./nlm visualize --activity --live --update-rate 60"),
        ("Spike Raster Plot", "./nlm visualize --spikes"),
        ("Weight Matrix Visualization", "./nlm visualize --weights"),
        ("Web-based Visualization", "./nlm visualize --web --port 8080"),
        ("Multiple Views", "./nlm visualize --activity --spikes --weights --live"),
        ("Export to SVG", "./nlm visualize --spikes --output svg"),
        ("Export to PDF", "./nlm visualize --spikes --output pdf"),
    ]
    
    for description, command in examples:
        print(f"\n{description}:")
        print(f"  {command}")

def show_quick_start():
    """Show quick start guide"""
    print("\n" + "="*60)
    print("NLM Visualization System - Quick Start")
    print("="*60)
    
    print("\n1. Build the project:")
    print("   mkdir build && cd build")
    print("   cmake .. && make -j$(nproc)")
    print("   cd ..")
    
    print("\n2. Run the demo script:")
    print("   ./examples/demo_visualization.sh --help")
    
    print("\n3. Create a test brain:")
    print("   python3 examples/visualization_utils.py --create-test-brain")
    
    print("\n4. Run visualization:")
    print("   ./examples/demo_visualization.sh --network test_brain.json --activity --live")
    
    print("\n5. Access web interface:")
    print("   http://localhost:8080")

def show_architecture():
    """Show visualization system architecture"""
    print("\n" + "="*60)
    print("NLM Visualization System - Architecture")
    print("="*60)
    
    print("\nCore Components:")
    print("  • VisualizationInterfaceAdvanced.hpp - Main interface class")
    print("  • VisualizationEngine.hpp/cpp - Core rendering engine")
    print("  • VisualizationScene.hpp/cpp - Scene management")
    print("  • VisualizationRenderer.hpp/cpp - Rendering system")
    print("  • VisualizationController.hpp/cpp - User interaction")
    
    print("\nVisualization Types:")
    print("  • NetworkView.hpp/cpp - Neural network visualization")
    print("  • ActivityView.hpp/cpp - Neural activity heatmap")
    print("  • SpikeView.hpp/cpp - Spike raster plot")
    print("  • WeightView.hpp/cpp - Synaptic weight visualization")
    print("  • 3DView.hpp/cpp - Three-dimensional visualization")
    print("  • TimeSeriesView.hpp/cpp - Time series visualization")
    print("  • MemoryView.hpp/cpp - Memory system visualization")
    print("  • PredictionView.hpp/cpp - Prediction system visualization")
    
    print("\nOutput Technologies:")
    print("  • ConsoleRenderer.hpp/cpp - Console-based visualization")
    print("  • SVGRenderer.hpp/cpp - Scalable vector graphics")
    print("  • PDFRenderer.hpp/cpp - PDF generation")
    print("  • WebRenderer.hpp/cpp - Web-based visualization")
    print("  • ScientificRenderer.hpp/cpp - Scientific plotting")
    
    print("\nBackend Technologies:")
    print("  • OpenGLBackend.hpp/cpp - OpenGL rendering")
    print("  • SDLBackend.hpp/cpp - SDL window management")
    print("  • WebGLBackend.hpp/cpp - WebGL for web applications")
    print("  • CairoBackend.hpp/cpp - Cairo 2D graphics")
    
    print("\nIntegration:")
    print("  • NLMIntegration.hpp/cpp - Integration with NLM components")

def main():
    """Main function"""
    if len(sys.argv) > 1:
        command = sys.argv[1]
        
        if command == "--create-test-brain":
            create_test_brain()
            return 0
            
        elif command == "--usage":
            show_visualization_usage()
            return 0
            
        elif command == "--quick-start":
            show_quick_start()
            return 0
            
        elif command == "--architecture":
            show_architecture()
            return 0
            
        elif command == "--help":
            print("\n" + "="*60)
            print("NLM Visualization System - Python Utility")
            print("="*60)
            print("\nThis script provides utilities for the NLM visualization system.")
            print("\nAvailable commands:")
            print("  --create-test-brain    Create a test brain configuration")
            print("  --usage               Show visualization usage examples")
            print("  --quick-start        Show quick start guide")
            print("  --architecture       Show system architecture")
            print("  --help               Show this help message")
            return 0
    
    # Default: show help
    print("\n" + "="*60)
    print("NLM Visualization System - Python Utility")
    print("="*60)
    print("\nUsage: python3 visualization_utils.py [command]")
    print("\nFor more information, use --help")
    return 0

if __name__ == "__main__":
    sys.exit(main())