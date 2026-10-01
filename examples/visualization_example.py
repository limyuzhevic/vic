#!/usr/bin/env python3
"""
NLM Brain Visualization Examples

This script demonstrates the new visualization and debugging tools for NLM.
It shows how to use the visualization API, debug console, and analysis tools.
"""

import pynlm
import matplotlib.pyplot as plt
import numpy as np
import json
import os
from pathlib import Path

def create_brain_with_visualization():
    """Create a brain with visualization capabilities."""
    print("=== Creating Brain with Visualization ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    
    # Initialize the brain
    if not brain.initialize():
        print("ERROR: Failed to initialize brain")
        return None
    
    print(f"Brain created successfully")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    
    return brain

def demonstrate_visualization_types(brain):
    """Demonstrate different visualization types."""
    print("\n=== Brain Visualization Types ===")
    
    # Get available visualization types
    viz_types = brain.getAvailableVisualizationTypes()
    print(f"Available visualization types: {viz_types}")
    
    # Generate different types of visualizations
    formats = ["png", "json", "csv"]
    
    for viz_type in viz_types:
        for fmt in formats:
            filename = f"brain_visualization_{viz_type}.{fmt}"
            print(f"\nGenerating {viz_type} visualization in {fmt} format...")
            
            try:
                if fmt == "png":
                    success = brain.generateVisualization(filename)
                elif fmt == "json":
                    success = brain.exportBrainData(filename)
                elif fmt == "csv":
                    success = brain.exportTimeSeriesData(filename)
                
                if success:
                    print(f"  ✓ Successfully created {filename}")
                    # Show file info
                    file_size = os.path.getsize(filename) if os.path.exists(filename) else 0
                    print(f"  File size: {file_size} bytes")
                else:
                    print(f"  ✗ Failed to create {filename}")
                    
            except Exception as e:
                print(f"  ✗ Error: {e}")
    
    print(f"\nTotal files created: {len([f for f in os.listdir('.') if f.startswith('brain_visualization_')])}")

def demonstrate_analysis_tools(brain):
    """Demonstrate analysis tools and export formats."""
    print("\n=== Analysis Tools ===")
    
    # Get available export formats
    formats = brain.getAvailableExportFormats()
    print(f"Available export formats: {formats}")
    
    # Demonstrate connectivity matrix export
    print("\n--- Connectivity Matrix Export ---")
    success = brain.exportConnectivityMatrix("connectivity_matrix.csv")
    if success:
        print("✓ Connectivity matrix exported successfully")
        # Read and display first few lines
        with open("connectivity_matrix.csv", "r") as f:
            lines = f.readlines()[:10]  # First 10 lines
            print("  Preview:")
            for line in lines:
                print(f"    {line.strip()}")
    else:
        print("✗ Failed to export connectivity matrix")
    
    # Demonstrate time series export
    print("\n--- Time Series Export ---")
    success = brain.exportTimeSeriesData("time_series_data.csv")
    if success:
        print("✓ Time series data exported successfully")
    else:
        print("✗ Failed to export time series data")

def demonstrate_visualization_control(brain):
    """Demonstrate visualization control and statistics."""
    print("\n=== Visualization Control ===")
    
    # Enable visualization
    print("\n1. Enabling visualization...")
    brain.enableVisualization(True)
    print("   ✓ Visualization enabled")
    
    # Update visualization
    print("\n2. Updating visualization...")
    for i in range(3):
        brain.updateVisualization(0.1)
        brain.renderVisualization()
        print(f"   Update {i+1} complete")
    
    # Get visualization statistics
    print("\n3. Getting visualization statistics...")
    stats = brain.getVisualizationStats()
    print(f"   Frame count: {stats['frame_count']}")
    print(f"   FPS: {stats['fps']:.2f}")
    print(f"   Update time: {stats['update_time']:.4f}s")
    print(f"   Render time: {stats['render_time']:.4f}s")
    
    # Set visualization preferences
    print("\n4. Setting visualization preferences...")
    brain.setVisualizationPreference("theme", "dark")
    brain.setVisualizationPreference("quality", "high")
    brain.setVisualizationPreference("fps_target", "60")
    
    # Get preferences
    print("\n5. Getting visualization preferences...")
    print(f"   Theme: {brain.getVisualizationPreference('theme')}")
    print(f"   Quality: {brain.getVisualizationPreference('quality')}")
    print(f"   FPS target: {brain.getVisualizationPreference('fps_target')}")

def create_visualization_api_example():
    """Example using the VisualizationAPI directly."""
    print("\n=== Visualization API Example ===")
    
    try:
        # Create visualization API
        viz_api = pynlm.createVisualizationAPI()
        print("✓ Visualization API created")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Set brain in visualization API
        viz_api.setBrain(brain)
        print("✓ Brain set in visualization API")
        
        # Start real-time visualization
        viz_api.startRealTimeVisualization(fps=30.0)
        print("✓ Started real-time visualization")
        
        # Update the API
        viz_api.update(0.1)
        print("✓ Updated visualization")
        
        # Render
        viz_api.render()
        print("✓ Rendered visualization")
        
        # Get status
        status = viz_api.getStatus()
        print(f"API status: {status[:100]}...")
        
        # Stop visualization
        viz_api.stopRealTimeVisualization()
        print("✓ Stopped real-time visualization")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in visualization API example: {e}")
        return False

def demonstrate_python_bindings(brain):
    """Demonstrate Python bindings for visualization."""
    print("\n=== Python Bindings Demonstration ===")
    
    # Generate various visualizations using different formats
    test_cases = [
        ("brain_network.png", "png", "Network visualization"),
        ("brain_data.json", "json", "JSON data export"),
        ("brain_stats.csv", "csv", "CSV statistics export"),
    ]
    
    for filename, format, description in test_cases:
        print(f"\n{description}:")
        print(f"  Target file: {filename}")
        
        try:
            if format == "png":
                success = brain.generateVisualization(filename)
            elif format == "json":
                success = brain.exportBrainData(filename)
            elif format == "csv":
                success = brain.exportTimeSeriesData(filename)
            
            if success:
                print(f"  ✓ Successfully created {filename}")
                file_size = os.path.getsize(filename) if os.path.exists(filename) else 0
                print(f"    File size: {file_size} bytes")
            else:
                print(f"  ✗ Failed to create {filename}")
                
        except Exception as e:
            print(f"  ✗ Error: {e}")

def main():
    """Main demonstration function."""
    print("NLM Brain Visualization and Debugging Tools Demonstration")
    print("=" * 60)
    
    # Create brain with visualization
    brain = create_brain_with_visualization()
    if brain is None:
        print("Failed to create brain. Exiting.")
        return
    
    # Demonstrate different visualization features
    demonstrate_visualization_types(brain)
    demonstrate_analysis_tools(brain)
    demonstrate_visualization_control(brain)
    
    # Demonstrate visualization API
    api_success = create_visualization_api_example()
    
    # Demonstrate Python bindings
    demonstrate_python_bindings(brain)
    
    # Clean up visualization files
    print("\n=== Cleanup ===")
    viz_files = [f for f in os.listdir('.') if f.startswith('brain_visualization_') or 
                 f in ['connectivity_matrix.csv', 'time_series_data.csv']]
    for f in viz_files:
        try:
            os.remove(f)
            print(f"✓ Removed {f}")
        except Exception as e:
            print(f"✗ Error removing {f}: {e}")
    
    print("\n" + "=" * 60)
    print("Demonstration completed successfully!")
    print("\nKey features demonstrated:")
    print("  - Brain visualization in multiple formats (PNG, JSON, CSV)")
    print("  - Connectivity matrix export")
    print("  - Time series data export")
    print("  - Visualization control (enable/disable)")
    print("  - Visualization statistics and preferences")
    print("  - Visualization API integration")
    print("  - Python bindings for visualization")

if __name__ == "__main__":
    main()
