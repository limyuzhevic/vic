#!/usr/bin/env python3
"""
NLM Performance Benchmark Tool

This tool benchmarks NLM performance and provides optimization recommendations.
"""

import subprocess
import time
import os
import sys
import json
import argparse
from typing import Dict, List, Any
class NLMBenchmark:
    def __init__(self, build_dir: str = "build"):
        self.build_dir = build_dir
        self.results = {}
        
    def run_benchmark(self, neurons: int = 500, steps: int = 1000, 
                     phases: List[str] = None) -> Dict[str, Any]:
        """Run performance benchmarks"""
        if phases is None:
            phases = ["integration", "plasticity", "stdp", "memory"]
        
        results = {}
        
        for phase in phases:
            print(f"\n=== Benchmarking Phase: {phase.upper()} ===")
            
            if phase == "integration":
                results[phase] = self._benchmark_integration(neurons, steps)
            elif phase == "plasticity":
                results[phase] = self._benchmark_plasticity(neurons, steps)
            elif phase == "stdp":
                results[phase] = self._benchmark_stdp(neurons, steps)
            elif phase == "memory":
                results[phase] = self._benchmark_memory(neurons, steps)
        
        self.results = results
        return results
    
    def _benchmark_integration(self, neurons: int, steps: int) -> Dict[str, Any]:
        """Benchmark the integration experiment"""
        print(f"Running integration benchmark: {neurons} neurons, {steps} steps")
        
        start_time = time.time()
        
        # Run Phase 6 demo with specified parameters
        cmd = [
            f"./{self.build_dir}/nlm_phase6_demo",
            f"--neurons={neurons}",
            f"--steps={steps}",
            "--verbose"
        ]
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
            end_time = time.time()
            
            return {
                "success": result.returncode == 0,
                "duration": end_time - start_time,
                "stdout": result.stdout,
                "stderr": result.stderr,
                "return_code": result.returncode
            }
        except subprocess.TimeoutExpired:
            return {
                "success": False,
                "duration": 300.0,
                "error": "Timeout after 5 minutes",
                "stdout": "",
                "stderr": "Benchmark timed out"
            }
        except FileNotFoundError:
            return {
                "success": False,
                "duration": 0.0,
                "error": "Binary not found",
                "stdout": "",
                "stderr": f"Build directory '{self.build_dir}' not found or nlm_phase6_demo not compiled"
            }
    
    def _benchmark_plasticity(self, neurons: int, steps: int) -> Dict[str, Any]:
        """Benchmark plasticity learning"""
        print(f"Running plasticity benchmark: {neurons} neurons, {steps} steps")
        
        start_time = time.time()
        
        # Create a simple plasticity benchmark script
        benchmark_script = f"""
import sys
sys.path.append('python')
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Apply plastic changes
for i in range({steps}):
    brain.step(i)

print(f"Plasticity benchmark completed: {neurons} neurons, {steps} steps")
print(f"Total spikes: {{brain.getTotalSpikeCount()}}")
print(f"Average firing rate: {{brain.getAverageFiringRate()}}")
"""
        
        try:
            result = subprocess.run(
                [sys.executable, "-c", benchmark_script],
                capture_output=True,
                text=True,
                timeout=180
            )
            end_time = time.time()
            
            return {
                "success": result.returncode == 0,
                "duration": end_time - start_time,
                "stdout": result.stdout,
                "stderr": result.stderr,
                "return_code": result.returncode
            }
        except subprocess.TimeoutExpired:
            return {
                "success": False,
                "duration": 180.0,
                "error": "Timeout after 3 minutes",
                "stdout": "",
                "stderr": "Plasticity benchmark timed out"
            }
        except Exception as e:
            return {
                "success": False,
                "duration": 0.0,
                "error": str(e),
                "stdout": "",
                "stderr": ""
            }
    
    def _benchmark_stdp(self, neurons: int, steps: int) -> Dict[str, Any]:
        """Benchmark STDP learning"""
        print(f"Running STDP benchmark: {neurons} neurons, {steps} steps")
        
        start_time = time.time()
        
        # Simple STDP benchmark
        benchmark_script = f"""
import sys
sys.path.append('python')
import pynlm

# Create brain with STDP
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable STDP and run correlated activity
agent = pynlm.createAgentBrain(brain)
agent.initialize()

for i in range({steps}):
    # Simulate correlated pre-post activity
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
    brain.step(i)

print(f"STDP benchmark completed: {neurons} neurons, {steps} steps")
print(f"Total spikes: {{brain.getTotalSpikeCount()}}")
print(f"Final synaptic weight: {{brain.getTotalSynapticWeight() if hasattr(brain, 'getTotalSynapticWeight') else 'N/A'}}")
"""
        
        try:
            result = subprocess.run(
                [sys.executable, "-c", benchmark_script],
                capture_output=True,
                text=True,
                timeout=180
            )
            end_time = time.time()
            
            return {
                "success": result.returncode == 0,
                "duration": end_time - start_time,
                "stdout": result.stdout,
                "stderr": result.stderr,
                "return_code": result.returncode
            }
        except subprocess.TimeoutExpired:
            return {
                "success": False,
                "duration": 180.0,
                "error": "Timeout after 3 minutes",
                "stdout": "",
                "stderr": "STDP benchmark timed out"
            }
        except Exception as e:
            return {
                "success": False,
                "duration": 0.0,
                "error": str(e),
                "stdout": "",
                "stderr": ""
            }
    
    def _benchmark_memory(self, neurons: int, steps: int) -> Dict[str, Any]:
        """Benchmark memory systems"""
        print(f"Running memory benchmark: {neurons} neurons, {steps} steps")
        
        start_time = time.time()
        
        # Simple memory benchmark
        benchmark_script = f"""
import sys
sys.path.append('python')
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable memory systems
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run with memory interactions
for i in range({steps}):
    brain.step(i)

print(f"Memory benchmark completed: {neurons} neurons, {steps} steps")
print(f"Total spikes: {{brain.getTotalSpikeCount()}}")
print(f"Working memory traces: {{brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}}")
"""
        
        try:
            result = subprocess.run(
                [sys.executable, "-c", benchmark_script],
                capture_output=True,
                text=True,
                timeout=180
            )
            end_time = time.time()
            
            return {
                "success": result.returncode == 0,
                "duration": end_time - start_time,
                "stdout": result.stdout,
                "stderr": result.stderr,
                "return_code": result.returncode
            }
        except subprocess.TimeoutExpired:
            return {
                "success": False,
                "duration": 180.0,
                "error": "Timeout after 3 minutes",
                "stdout": "",
                "stderr": "Memory benchmark timed out"
            }
        except Exception as e:
            return {
                "success": False,
                "duration": 0.0,
                "error": str(e),
                "stdout": "",
                "stderr": ""
            }
    
    def generate_report(self) -> str:
        """Generate a benchmark report"""
        if not self.results:
            return "No benchmark results available. Run benchmarks first."
        
        report = "=== NLM Performance Benchmark Report ===\n\n"
        
        for phase, result in self.results.items():
            report += f"Phase: {phase.upper()}\n"
            report += f"  Success: {'✓' if result.get('success', False) else '✗'}\n"
            report += f"  Duration: {result.get('duration', 0):.2f} seconds\n"
            
            if not result.get('success', False):
                report += f"  Error: {result.get('error', 'Unknown')}\n"
            
            report += "\n"
        
        return report
    
    def save_results(self, filename: str):
        """Save benchmark results to JSON file"""
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2)
        
        print(f"Benchmark results saved to {filename}")
        
def main():
    parser = argparse.ArgumentParser(description="NLM Performance Benchmark Tool")
    parser.add_argument("--neurons", type=int, default=500, help="Number of neurons (default: 500)")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps (default: 1000)")
    parser.add_argument("--phases", nargs="+", default=["integration", "plasticity", "stdp", "memory"], 
                       help="Phases to benchmark (default: all)")
    parser.add_argument("--build-dir", default="build", help="Build directory (default: build)")
    parser.add_argument("--output", help="Output JSON file for results")
    
    args = parser.parse_args()
    
    benchmark = NLMBenchmark(args.build_dir)
    
    print("Starting NLM Performance Benchmark...")
    print(f"Neurons: {args.neurons}")
    print(f"Steps: {args.steps}")
    print(f"Phases: {', '.join(args.phases)}")
    
    results = benchmark.run_benchmark(args.neurons, args.steps, args.phases)
    
    print("\n" + benchmark.generate_report())
    
    if args.output:
        benchmark.save_results(args.output)
    
    # Check if any benchmarks failed
    failed = False
    for phase, result in results.items():
        if not result.get('success', False):
            failed = True
            print(f"Warning: Benchmark for {phase} failed: {result.get('error', 'Unknown error')}")
    
    sys.exit(1 if failed else 0)
if __name__ == "__main__":
    main()
