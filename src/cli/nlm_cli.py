"""
Python Command Line Interface for NLM

This module provides command-line interfaces for using the NLM (Neural Learning Machine)
framework from Python, making it accessible for research, experimentation, and development.
"""

import argparse
import sys
import time
import json
from pathlib import Path
from typing import Optional, List

# Try to import NLM bindings
try:
    from nlm import (
        Brain, Config, Random, Logger, Environment,
        create_brain, create_agent, create_simple_world,
        get_version, get_system_info
    )
    PYNLB_AVAILABLE = True
except ImportError:
    PYNLB_AVAILABLE = False
    print("Warning: NLM Python bindings not available")

# Get version info
try:
    from nlm import get_version
    NLM_VERSION = get_version()
except:
    NLM_VERSION = {"full": "0.1.0-enhanced"}


def print_banner():
    """Print the NLM banner."""
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║                                                               ║")
    print("║     NLM — 熙然                                                ║")
    print("║     Neural Learning Machine                                   ║")
    print("║                                                               ║")
    print("║     Enhanced Python Interface                                  ║")
    print("║                                                               ║")
    print("╚═══════════════════════════════════════════════════════════════╝")
    print()


def main():
    """Main CLI entry point."""
    if not PYNLB_AVAILABLE:
        print("ERROR: NLM Python bindings not available!")
        print("Please build the Python bindings first:")
        print("  pip install -e .")
        return 1
    
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) - Enhanced Python Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm --brain 100  # Create a simple brain with 100 neurons
  nlm --demo        # Run demonstration
  nlm --benchmark   # Run performance benchmark
  nlm --help        # Show detailed help
        """
    )
    
    parser.add_argument(
        "--version", action="store_true",
        help="Show version information and exit"
    )
    
    parser.add_argument(
        "--demo", action="store_true",
        help="Run demonstration of NLM capabilities"
    )
    
    parser.add_argument(
        "--benchmark", action="store_true",
        help="Run performance benchmark"
    )
    
    parser.add_argument(
        "--brain", type=int, nargs="?", const=100,
        help="Create a brain with specified number of neurons (default: 100)"
    )
    
    parser.add_argument(
        "--steps", type=int, default=1000,
        help="Number of simulation steps (default: 1000)"
    )
    
    parser.add_argument(
        "--seed", type=int, default=42,
        help="Random seed for reproducibility (default: 42)"
    )
    
    parser.add_argument(
        "--output", type=str, default="",
        help="Output file for results (default: stdout)"
    )
    
    parser.add_argument(
        "--verbose", action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--quiet", action="store_true",
        help="Suppress progress output"
    )
    
    args = parser.parse_args()
    
    if args.version:
        print_version()
        return 0
    
    if args.demo:
        return run_demo(args)
    
    if args.benchmark:
        return run_benchmark(args)
    
    if args.brain is not None:
        return create_brain_simulation(args)
    
    # Default: show help
    parser.print_help()
    return 1


def print_version():
    """Print version information."""
    print("NLM Python Interface - Version Information")
    print("=" * 50)
    print(f"Version: {NLM_VERSION.get('full', '0.1.0-enhanced')}")
    print(f"Author: NLM Project Team")
    print(f"License: MIT")
    print(f"Python Bindings: Available")
    print(f"C++ Engine: Ready")
    print()
    print("Key Features:")
    print("  • Real spiking neural networks (LIF model)")
    print("  • Event-driven spike propagation")
    print("  • Multiple plasticity rules (STDP, Hebbian)")
    print("  • Memory systems (working, episodic, associative)")
    print("  • Neuromodulation and motivation systems")
    print("  • Prediction and planning")
    print("  • Development and learning mechanisms")
    print("  • Sensorimotor integration")
    print("  • Environment interaction")
    print()


def run_demo(args):
    """Run demonstration of NLM capabilities."""
    print("=== NLM Demonstration ===")
    print(f"Creating demonstration brain with {args.brain} neurons...")
    
    try:
        # Create brain
        config = Config()
        config.setInt("neuron_count", args.brain)
        config.setInt("region_count", 1)
        config.setFloat("connection_probability", 0.15f)
        
        brain = create_brain(config)
        print(f"✓ Brain created successfully")
        
        # Initialize
        if brain.initialize():
            print("✓ Brain initialized")
        else:
            print("✗ Brain initialization failed")
            return 1
        
        # Run simulation
        print(f"Running {args.steps} simulation steps...")
        start_time = time.time()
        
        for i in range(args.steps):
            brain.step(i, i * 0.001)
            
            if args.verbose and i % 100 == 0:
                print(f"  Step {i}: {brain.getTotalSpikeCount()} spikes")
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Print results
        print("\n=== Demonstration Results ===")
        print(f"Total simulation time: {duration:.2f}s")
        print(f"Total steps: {args.steps}")
        print(f"Steps per second: {args.steps/duration:.1f}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Active neurons: {brain.getFiringNeuronCount()}")
        
        # Save results if requested
        if args.output:
            save_results(args.output, brain, args.steps, duration)
            print(f"\nResults saved to {args.output}")
        
        print("\n✓ Demonstration completed successfully")
        return 0
        
    except Exception as e:
        print(f"\n✗ Demonstration failed: {e}")
        return 1


def run_benchmark(args):
    """Run performance benchmark."""
    print("=== NLM Performance Benchmark ===")
    
    try:
        # Create benchmark brain
        config = Config()
        config.setInt("neuron_count", 1000)
        config.setInt("region_count", 2)
        config.setFloat("connection_probability", 0.1f)
        
        brain = create_brain(config)
        if (!brain.initialize()) {
            print("✗ Brain initialization failed for benchmark")
            return 1;
        }
        
        # Run benchmark
        benchmark_steps = args.steps
        print(f"Running {benchmark_steps} benchmark steps...")
        
        start_time = time.time();
        double totalTime = 0.0;
        size_t totalSpikes = 0;
        size_t maxFiring = 0;
        
        for (size_t i = 0; i < benchmark_steps; ++i) {
            double stepStart = time.time();
            brain.step(i, i * 0.001);
            double stepTime = time.time() - stepStart;
            totalTime += stepTime;
            
            size_t spikes = brain.getTotalSpikeCount();
            totalSpikes = spikes;
            size_t firing = brain.getFiringNeuronCount();
            if (firing > maxFiring) maxFiring = firing;
            
            if (args.verbose && i % 200 == 0) {
                print(f"  Step {i}: {spikes} spikes, {firing} firing, {stepTime*1000:.2f}ms/step");
            }
        }
        
        double totalBenchmarkTime = time.time() - startTime;
        
        // Print benchmark results
        print("\n=== Benchmark Results ===");
        print(f"Total benchmark time: {totalBenchmarkTime:.2f}s");
        print(f"Average step time: {totalTime/benchmark_steps*1000:.2f}ms");
        print(f"Steps per second: {benchmark_steps/totalBenchmarkTime:.1f}");
        print(f"Total spikes generated: {totalSpikes}");
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz");
        print(f"Maximum concurrent firing: {maxFiring} neurons");
        print(f"Neuron density: {brain.getTotalNeuronCount()/(brain.getTotalSynapseCount() + 1):.2f} synapses/neuron");
        
        // Save benchmark results if requested
        if (!args.output.empty()) {
            save_benchmark_results(args.output, brain, benchmark_steps, totalBenchmarkTime);
            print(f"\nBenchmark results saved to {args.output}");
        }
        
        print("\n✓ Benchmark completed successfully");
        return 0;
        
    } catch (const std::exception& e) {
        print(f"\n✗ Benchmark failed: {e.what()}");
        return 1;
    }
}


def create_brain_simulation(args) {
    """Create and simulate a brain."""
    print("=== Brain Simulation ===");
    print(f"Creating brain with {args.brain} neurons...");
    
    try {
        // Create random generator
        Random random(args.seed);
        
        // Create brain configuration
        Config config;
        config.setInt("neuron_count", args.brain);
        config.setInt("region_count", 1);
        config.setFloat("connection_probability", 0.15f);
        config.setInt("random_seed", args.seed);
        
        // Create brain
        Brain brain = create_brain(config);
        if (!brain.initialize()) {
            print("✗ Brain initialization failed");
            return 1;
        }
        
        print("✓ Brain created and initialized");
        
        // Run simulation
        print(f"Running {args.steps} simulation steps...");
        size_t maxSpikeActivity = 0;
        
        for (size_t i = 0; i < args.steps; ++i) {
            // Apply random inputs
            for (size_t j = 0; j < args.brain / 10; ++j) {
                brain.injectCurrentToNeurons(NeuronType::Sensory, random.nextFloat(0.0f, 50.0f));
            }
            
            brain.step(i, i * 0.001);
            
            size_t spikeCount = brain.getTotalSpikeCount();
            if (spikeCount > maxSpikeActivity) {
                maxSpikeActivity = spikeCount;
            }
            
            if (args.verbose && i % 100 == 0) {
                print(f"  Step {i}: {spikeCount} total spikes, {brain.getFiringNeuronCount()} firing neurons");
            }
        }
        
        // Print final statistics
        print("\n=== Final Brain Statistics ===");
        print(f"Total neurons: {brain.getNeuronCount()}");
        print(f"Total synapses: {brain.getTotalSynapseCount()}");
        print(f"Total spikes: {brain.getTotalSpikeCount()}");
        print(f"Peak spike activity: {maxSpikeActivity}");
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz");
        print(f"Active neurons (final): {brain.getFiringNeuronCount()}");
        
        // Log brain status
        brain.logStatus();
        
        // Save results if requested
        if (!args.output.empty()) {
            save_simulation_results(args.output, brain, args.steps);
            print(f"\nSimulation results saved to {args.output}");
        }
        
        print("\n✓ Brain simulation completed successfully");
        return 0;
        
    } catch (const std::exception& e) {
        print(f"\n✗ Brain simulation failed: {e.what()}");
        return 1;
    }
}

// Helper functions
def save_results(filename, Brain& brain, size_t steps, double duration) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open output file: " + filename);
    }
    
    file << "// NLM Simulation Results" << std::endl;
    file << "// Generated: " << time(nullptr) << std::endl;
    file << "// Duration: " << duration << "s" << std::endl;
    file << std::endl;
    
    file << "Simulation Parameters:" << std::endl;
    file << "  Steps: " << steps << std::endl;
    file << "  Final Spike Count: " << brain.getTotalSpikeCount() << std::endl;
    file << "  Final Firing Rate: " << brain.getAverageFiringRate() << std::endl;
    file << "  Active Neurons: " << brain.getFiringNeuronCount() << std::endl;
    file << std::endl;
    
    file << "Time Series Data:" << std::endl;
    file << "Step,SpikeCount,FiringRate,ActiveNeurons" << std::endl;
    
    // In a real implementation, we would save the time series
    // For now, just save final statistics
    file << steps << "," << brain.getTotalSpikeCount() << ","
         << brain.getAverageFiringRate() << "," << brain.getFiringNeuronCount() << std::endl;
    
    file.close();
}

void save_benchmark_results(const std::string& filename, Brain& brain, size_t steps, double duration) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open benchmark output file: " + filename);
    }
    
    file << "// NLM Performance Benchmark Results" << std::endl;
    file << "// Generated: " << time(nullptr) << std::endl;
    file << "// Duration: " << duration << "s" << std::endl;
    file << std::endl;
    
    file << "Benchmark Parameters:" << std::endl;
    file << "  Steps: " << steps << std::endl;
    file << "  Steps per second: " << steps/duration << std::endl;
    file << std::endl;
    
    file << "Performance Metrics:" << std::endl;
    file << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    file << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    file << "  Active Neurons: " << brain.getFiringNeuronCount() << std::endl;
    file << "  Network Size: " << brain.getTotalNeuronCount() << " neurons, " 
          << brain.getTotalSynapseCount() << " synapses" << std::endl;
    
    file.close();
}

void save_simulation_results(const std::string& filename, Brain& brain, size_t steps) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open simulation output file: " + filename);
    }
    
    file << "// NLM Simulation Results" << std::endl;
    file << "// Generated: " << time(nullptr) << std::endl;
    file << std::endl;
    
    file << "Final Statistics:" << std::endl;
    file << "  Simulation Steps: " << steps << std::endl;
    file << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    file << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    file << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    file << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    file << "  Active Neurons: " << brain.getFiringNeuronCount() << std::endl;
    
    file.close();
}

// Entry points for console scripts
int main(int argc, char* argv[]) {
    return main(argc, argv);
}
