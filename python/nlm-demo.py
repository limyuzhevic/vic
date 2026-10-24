#!/usr/bin/env python3
"""
NLM Main Demo - Interactive Command Line Interface

A comprehensive command-line interface for the NLM (Neural Learning Machine) framework.
Provides easy access to all NLM functionality with intuitive commands and options.

Usage:
    nlm-demo [COMMAND] [OPTIONS]

Commands:
    run           Run a brain simulation
    test          Run integration tests
    explore       Explore brain components
    benchmark     Run performance benchmarks
    help          Show help information
    version       Show version information
"""

import argparse
import sys
import subprocess
import os
from pathlib import Path

def print_banner():
    """Print the NLM demo banner."""
    print("=" * 70)
    print("NLM Phase 6 Demo - Interactive Command Line Interface")
    print("Neural Learning Machine - Experimental Artificial Brain")
    print("=" * 70)
    print()

def handle_version(args):
    """Show version information."""
    print("NLM Phase 6 Demo v0.1.0")
    print("Python Bindings: Available")
    print("C++ Framework: Available")
    print("Build Type: Release")
    print("Phase: Final Integration")
    print()
    print("Key Features:")
    print("  ✓ Real spiking neural computation")
    print("  ✓ Event-driven spike propagation")
    print("  ✓ STDP and Hebbian plasticity")
    print("  ✓ Memory systems integration")
    print("  ✓ Neuromodulation systems")
    print("  ✓ Prediction and cognition")
    print("  ✓ Development and structural plasticity")
    print("  ✓ Checkpoint and replay capabilities")
    print()

def handle_help(args):
    """Show help information."""
    print("NLM Phase 6 Demo - Commands:")
    print()
    print("run [OPTIONS]")
    print("  Run a brain simulation with specified options")
    print()
    print("    --neurons N          Number of neurons (default: 500)")
    print("    --steps S            Number of simulation steps (default: 1000)")
    print("    --verbose            Enable verbose output")
    print("    --quick              Run quick test only")
    print("    --full               Run full integration test")
    print("    --diagnostics        Run system diagnostics")
    print("    --export DIR         Export results to directory")
    print()
    print("test [OPTIONS]")
    print("  Run integration tests")
    print()
    print("    --mode MODE         Test mode: 'quick', 'full', 'all' (default: 'all')")
    print("    --output FILE        Output file for test results")
    print()
    print("explore [OPTIONS]")
    print("  Explore brain components and systems")
    print()
    print("    --component COMP     Component to explore: 'memory', 'neuro', 'cognition' (default: 'all')")
    print("    --details            Show detailed information")
    print()
    print("benchmark [OPTIONS]")
    print("  Run performance benchmarks")
    print()
    print("    --duration T        Benchmark duration in seconds (default: 60)")
    print("    --output FILE        Output file for benchmark results")
    print()
    print("help [COMMAND]       Show help for specific command")
    print("version              Show version information")
    print()
    print("Examples:")
    print("  nlm-demo run --neurons 1000 --steps 5000 --verbose")
    print("  nlm-demo test --mode full --output results.txt")
    print("  nlm-demo explore --component memory --details")
    print("  nlm-demo benchmark --duration 30")
    print()

def handle_run(args):
    """Run a brain simulation."""
    print(f"Running NLM simulation...")
    print(f"  - Neurons: {args.neurons}")
    print(f"  - Steps: {args.steps}")
    print(f"  - Regions: {args.regions}")
    print(f"  - Connection Probability: {args.prob}")
    
    if args.verbose:
        print("  - Verbose mode: enabled")
    
    # Build command for Phase 6 demo
    cmd = ["./nlm_phase6_demo",
          "--neurons", str(args.neurons),
          "--steps", str(args.steps)]
    
    if args.quick:
        cmd.append("--quick")
    elif args.full:
        cmd.append("--full")
    elif args.diagnostics:
        cmd.append("--diagnostics")
    
    if args.no_checkpointing:
        cmd.append("--no-checkpointing")
    if args.no_replay:
        cmd.append("--no-replay")
    if args.no_development:
        cmd.append("--no-development")
    
    if args.verbose:
        print(f"  - Command: {' '.join(cmd)}")
        print()
    
    # Run the simulation
    try:
        result = subprocess.run(cmd, check=True, capture_output=not args.verbose)
        if result.returncode == 0:
            print("✓ Simulation completed successfully!")
            if args.output:
                with open(args.output, "w") as f:
                    f.write(result.stdout.decode() if result.stdout else "Simulation completed successfully\n")
                print(f"  Results saved to: {args.output}")
        else:
            print("✗ Simulation failed!")
            if not args.verbose and result.stderr:
                print(f"Error: {result.stderr.decode().strip()}")
            return 1
    except FileNotFoundError:
        print("ERROR: nlm_phase6_demo executable not found!")
        print("Please build the project first:")
        print("  mkdir build")
        print("  cd build")
        print("  cmake .. -DCMAKE_BUILD_TYPE=Release")
        print("  make")
        return 1
    except subprocess.CalledProcessError as e:
        print(f"✗ Simulation failed with error: {e}")
        if args.verbose:
            print(f"  Output: {e.output}")
        return 1
    
    return 0

def handle_test(args):
    """Run integration tests."""
    print(f"Running integration tests...")
    print(f"  - Mode: {args.mode}")
    
    # Test the Python bindings
    test_script = Path("python/test_demo.py")
    
    if not test_script.exists():
        print("Creating test script...")
        create_test_script()
    
    # Run tests
    try:
        result = subprocess.run([
            sys.executable, str(test_script),
            "--mode", args.mode
        ], check=True, capture_output=not args.verbose)
        
        if result.returncode == 0:
            print("✓ Integration tests completed successfully!")
            if args.output:
                with open(args.output, "w") as f:
                    f.write(result.stdout.decode() if result.stdout else "All tests passed\n")
                print(f"  Results saved to: {args.output}")
        else:
            print("✗ Integration tests failed!")
            if not args.verbose and result.stderr:
                print(f"Error: {result.stderr.decode().strip()}")
            return 1
            
    except FileNotFoundError:
        print("ERROR: Python test script not found!")
        return 1
    except subprocess.CalledProcessError as e:
        print(f"✗ Integration tests failed: {e}")
        return 1
    
    return 0

def create_test_script():
    """Create a comprehensive test script for NLM."""
    test_content = '''#!/usr/bin/env python3
"""
NLM Integration Test Script

Comprehensive integration tests for the NLM framework.
"""

import sys
import os
import argparse
import json
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

import pynlm

def run_integration_test(mode="all"):
    """Run integration tests based on mode."""
    results = {
        "timestamp": time.time(),
        "mode": mode,
        "tests": {},
        "summary": {}
    }
    
    print(f"Running integration test in '{mode}' mode...")
    
    if mode in ["all", "brain"]:
        results["tests"]["brain"] = test_brain_creation()
    
    if mode in ["all", "simulation"]:
        results["tests"]["simulation"] = test_simulation()
    
    if mode in ["all", "memory"]:
        results["tests"]["memory"] = test_memory_systems()
    
    if mode in ["all", "neuromod"]:
        results["tests"]["neuromod"] = test_neuromodulation()
    
    if mode in ["all", "python"]:
        results["tests"]["python"] = test_python_interface()
    
    # Calculate summary
    results["summary"] = {
        "total_tests": len(results["tests"]),
        "passed": sum(1 for test in results["tests"].values() if test.get("passed", False)),
        "failed": sum(1 for test in results["tests"].values() if not test.get("passed", False)),
        "success_rate": 0.0
    }
    
    if results["summary"]["total_tests"] > 0:
        results["summary"]["success_rate"] = (
            results["summary"]["passed"] / results["summary"]["total_tests"] * 100
        )
    
    return results

def test_brain_creation():
    """Test brain creation and initialization."""
    print("  Testing brain creation...")
    
    try:
        # Test basic brain creation
        brain = pynlm.Brain(neuron_count=100, region_count=1)
        if not brain.initialize():
            return {"passed": False, "error": "Brain initialization failed"}
        
        # Test brain properties
        if brain.get_total_neuron_count() != 100:
            return {"passed": False, "error": f"Expected 100 neurons, got {brain.get_total_neuron_count()}"}
        
        # Test memory access
        if brain.get_working_memory() is None:
            return {"passed": False, "error": "Working memory not available"}
        
        print("    ✓ Brain creation test passed")
        return {"passed": True, "details": "Brain created and initialized successfully"}
        
    except Exception as e:
        return {"passed": False, "error": f"Brain creation test failed: {e}"}

def test_simulation():
    """Test basic simulation."""
    print("  Testing simulation...")
    
    try:
        brain = pynlm.Brain(neuron_count=50, region_count=1)
        if not brain.initialize():
            return {"passed": False, "error": "Brain initialization failed"}
        
        # Run a few simulation steps
        for step in range(10):
            brain.step(step, step * 0.001)
        
        # Check that simulation produced results
        if brain.get_total_spike_count() < 0:
            return {"passed": False, "error": "Invalid spike count"}
        
        print("    ✓ Simulation test passed")
        return {"passed": True, "details": f"Simulation completed with {brain.get_total_spike_count()} spikes"}
        
    except Exception as e:
        return {"passed": False, "error": f"Simulation test failed: {e}"}

def test_memory_systems():
    """Test memory systems."""
    print("  Testing memory systems...")
    
    try:
        brain = pynlm.Brain(neuron_count=100, region_count=1)
        if not brain.initialize():
            return {"passed": False, "error": "Brain initialization failed"}
        
        # Test working memory
        working_memory = brain.get_working_memory()
        if working_memory is None:
            return {"passed": False, "error": "Working memory not available"}
        
        traces = working_memory.getActiveTraces()
        if traces < 0:
            return {"passed": False, "error": "Invalid working memory traces"}
        
        # Test episodic memory
        episodic_memory = brain.get_episodic_memory()
        if episodic_memory is None:
            return {"passed": False, "error": "Episodic memory not available"}
        
        episodes = episodic_memory.getEpisodeCount()
        if episodes < 0:
            return {"passed": False, "error": "Invalid episode count"}
        
        print("    ✓ Memory systems test passed")
        return {
            "passed": True,
            "details": f"Working memory: {traces} traces, Episodic memory: {episodes} episodes"
        }
        
    except Exception as e:
        return {"passed": False, "error": f"Memory systems test failed: {e}"}

def test_neuromodulation():
    """Test neuromodulation systems."""
    print("  Testing neuromodulation systems...")
    
    try:
        brain = pynlm.Brain(neuron_count=100, region_count=1)
        if not brain.initialize():
            return {"passed": False, "error": "Brain initialization failed"}
        
        # Test dopamine system
        dopamine = brain.get_dopamine()
        if dopamine is None:
            return {"passed": False, "error": "Dopamine system not available"}
        
        dopamine_level = dopamine.getLevel()
        if dopamine_level < 0:
            return {"passed": False, "error": "Invalid dopamine level"}
        
        # Test curiosity system
        curiosity = brain.get_curiosity()
        if curiosity is None:
            return {"passed": False, "error": "Curiosity system not available"}
        
        curiosity_level = curiosity.getLevel()
        if curiosity_level < 0:
            return {"passed": False, "error": "Invalid curiosity level"}
        
        print("    ✓ Neuromodulation systems test passed")
        return {
            "passed": True,
            "details": f"Dopamine: {dopamine_level:.3f}, Curiosity: {curiosity_level:.3f}"
        }
        
    except Exception as e:
        return {"passed": False, "error": f"Neuromodulation test failed: {e}"}

def test_python_interface():
    """Test Python interface functionality."""
    print("  Testing Python interface...")
    
    try:
        # Test configuration
        config = pynlm.Config()
        config.set("test_key", "test_value")
        value = config.get_string("test_key", "")
        if value != "test_value":
            return {"passed": False, "error": "Configuration get/set test failed"}
        
        # Test constants
        if not hasattr(pynlm, "VERSION"):
            return {"passed": False, "error": "VERSION constant not available"}
        
        # Test helper functions
        if not hasattr(pynlm, "get_version"):
            return {"passed": False, "error": "get_version function not available"}
        
        print("    ✓ Python interface test passed")
        return {"passed": True, "details": "Python interface fully functional"}
        
    except Exception as e:
        return {"passed": False, "error": f"Python interface test failed: {e}"}

def main():
    """Main test function."""
    parser = argparse.ArgumentParser(description="NLM Integration Test Script")
    parser.add_argument("--mode", choices=["all", "brain", "simulation", "memory", "neuromod", "python"], 
                       default="all", help="Test mode")
    
    args = parser.parse_args()
    
    print("=" * 60)
    print("NLM Integration Test Script")
    print("=" * 60)
    
    results = run_integration_test(args.mode)
    
    # Print results
    print("\n" + "=" * 60)
    print("Test Results Summary")
    print("=" * 60)
    
    for test_name, test_result in results["tests"].items():
        status = "✓ PASS" if test_result["passed"] else "✗ FAIL"
        print(f"{test_name:20} {status}")
        if not test_result["passed"]:
            print(f"  Error: {test_result.get('error', 'Unknown error')}")
    
    print(f"\nSummary:")
    print(f"  Total Tests: {results['summary']['total_tests']}")
    print(f"  Passed: {results['summary']['passed']}")
    print(f"  Failed: {results['summary']['failed']}")
    print(f"  Success Rate: {results['summary']['success_rate']:.1f}%")
    
    # Save results to file
    output_file = Path("integration_test_results.json")
    with open(output_file, "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\nResults saved to: {output_file}")
    
    if results["summary"]["failed"] > 0:
        return 1
    else:
        return 0

if __name__ == "__main__":
    exit(main())
'''
    
    with open(test_script, "w") as f:
        f.write(test_content)
    
    os.chmod(test_script, 0o755)
    print(f"✓ Created test script: {test_script}")

def handle_explore(args):
    """Explore brain components and systems."""
    print(f"Exploring brain components...")
    print(f"  - Component: {args.component}")
    print(f"  - Details: {'enabled' if args.details else 'disabled'}")
    
    # Create exploration script
    explore_script = Path("explore_nlm.py")
    
    explore_content = f'''
#!/usr/bin/env python3
"""
NLM Exploration Script

Explores specific brain components based on command line arguments.
"""

import sys
import os
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

import pynlm

def explore_memory():
    """Explore memory systems."""
    print("=== Memory System Exploration ===")
    
    brain = pynlm.Brain(neuron_count=100, region_count=1)
    brain.initialize()
    
    # Working memory
    wm = brain.get_working_memory()
    if wm:
        print(f"Working Memory:")
        print(f"  - Active Traces: {{wm.getActiveTraces()}}")
        print(f"  - Capacity: {{wm.getCapacity()}}")
    
    # Episodic memory
    em = brain.get_episodic_memory()
    if em:
        print(f"Episodic Memory:")
        print(f"  - Episodes Stored: {{em.getEpisodeCount()}}")
        print(f"  - Max Storage: {{em.getMaxStorage()}}")
    
    # Semantic memory
    sm = brain.get_semantic_memory()
    if sm:
        print(f"Semantic Memory:")
        print(f"  - Concepts Formed: {{sm.getConceptCount()}}")
    
    # Procedural memory
    pm = brain.get_procedural_memory()
    if pm:
        print(f"Procedural Memory:")
        print(f"  - Skills Learned: {{pm.getSkillCount()}}")
    
    # Associative memory
    am = brain.get_associative_memory()
    if am:
        print(f"Associative Memory:")
        print(f"  - Associations Formed: {{am.getAssociationCount()}}")

def explore_neuromod():
    """Explore neuromodulation systems."""
    print("=== Neuromodulation System Exploration ===")
    
    brain = pynlm.Brain(neuron_count=100, region_count=1)
    brain.initialize()
    
    # Dopamine
    dop = brain.get_dopamine()
    if dop:
        print(f"Dopamine System:")
        print(f"  - Level: {{dop.getLevel()}}")
        print(f"  - Prediction Error: {{dop.getPredictionError()}}")
    
    # Curiosity
    cur = brain.get_curiosity()
    if cur:
        print(f"Curiosity System:")
        print(f"  - Level: {{cur.getLevel()}}")
        print(f"  - Exploration Drive: {{cur.getExplorationDrive()}}")
    
    # Novelty
    nov = brain.get_novelty()
    if nov:
        print(f"Novelty System:")
        print(f"  - Novelty Level: {{nov.getNoveltyLevel()}}")
    
    # Reward
    rew = brain.get_reward()
    if rew:
        print(f"Reward System:")
        print(f"  - Level: {{rew.getLevel()}}")
        print(f"  - Discount Factor: {{rew.getDiscountFactor()}}")

def explore_cognition():
    """Explore cognitive systems."""
    print("=== Cognitive System Exploration ===")
    
    brain = pynlm.Brain(neuron_count=100, region_count=1)
    brain.initialize()
    
    # Planner
    planner = brain.get_planner()
    if planner:
        print(f"Neural Planner:")
        print(f"  - Planning Active: {{planner.isPlanning()}}")
    
    # Concept formation
    cf = brain.get_concept_formation()
    if cf:
        print(f"Concept Formation:")
        print(f"  - Concepts Formed: {{cf.getConceptCount()}}")
    
    # Attention
    attn = brain.get_attention()
    if attn:
        print(f"Attention System:")
        print(f"  - Attention Active: {{attn.isAttentionActive()}}")
        print(f"  - Focus Level: {{attn.getFocusLevel()}}")

def main():
    print("NLM Component Explorer")
    print("=" * 40)
    
    if "{args.component}" == "memory":
        explore_memory()
    elif "{args.component}" == "neuro":
        explore_neuromod()
    elif "{args.component}" == "cognition":
        explore_cognition()
    else:
        print("All Components:")
        explore_memory()
        print()
        explore_neuromod()
        print()
        explore_cognition()

if __name__ == "__main__":
    main()
'''
    
    with open(explore_script, "w") as f:
        f.write(explore_content)
    
    os.chmod(explore_script, 0o755)
    print(f"✓ Created exploration script: {explore_script}")
    
    # Run exploration
    try:
        result = subprocess.run([
            sys.executable, str(explore_script)
        ], check=True, capture_output=not args.verbose)
        
        if result.returncode == 0:
            print("✓ Exploration completed successfully!")
        else:
            print("✗ Exploration failed!")
            return 1
            
    except Exception as e:
        print(f"✗ Exploration failed: {e}")
        return 1
    
    return 0

def handle_benchmark(args):
    """Run performance benchmarks."""
    print(f"Running performance benchmarks...")
    print(f"  - Duration: {args.duration} seconds")
    
    # Create benchmark script
    benchmark_script = Path("benchmark_nlm.py")
    
    benchmark_content = f'''
#!/usr/bin/env python3
"""
NLM Performance Benchmark

Benchmarks NLM performance with specified duration.
"""

import sys
import os
import time
import argparse
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

import pynlm

def benchmark_brain_creation(num_iter=100):
    """Benchmark brain creation."""
    print(f"  Benchmarking brain creation ({{num_iter}} iterations)...")
    
    start_time = time.time()
    
    for i in range(num_iter):
        brain = pynlm.Brain(neuron_count=100, region_count=1)
        brain.initialize()
    
    end_time = time.time()
    duration = end_time - start_time
    
    avg_time = duration / num_iter
    brains_per_second = num_iter / duration
    
    print(f"    - Total time: {{duration:.3f}} seconds")
    print(f"    - Average time per brain: {{avg_time:.3f}} seconds")
    print(f"    - Brains created per second: {{brains_per_second:.2f}}")
    
    return avg_time, brains_per_second

def benchmark_simulation(brain, steps_per_brain=1000):
    """Benchmark simulation."""
    print(f"  Benchmarking simulation ({{steps_per_brain}} steps per brain)...")
    
    start_time = time.time()
    
    for step in range(steps_per_brain):
        brain.step(step, step * 0.001)
    
    end_time = time.time()
    duration = end_time - start_time
    
    steps_per_second = steps_per_brain / duration
    
    print(f"    - Total time: {{duration:.3f}} seconds")
    print(f"    - Steps per second: {{steps_per_second:.2f}}")
    
    return duration, steps_per_second

def benchmark_memory_operations(brain, num_operations=100):
    """Benchmark memory operations."""
    print(f"  Benchmarking memory operations ({{num_operations}} operations)...")
    
    start_time = time.time()
    
    for i in range(num_operations):
        # Access memory systems
        wm = brain.get_working_memory()
        if wm:
            _ = wm.getActiveTraces()
        
        em = brain.get_episodic_memory()
        if em:
            _ = em.getEpisodeCount()
    
    end_time = time.time()
    duration = end_time - start_time
    
    ops_per_second = num_operations / duration
    
    print(f"    - Total time: {{duration:.3f}} seconds")
    print(f"    - Operations per second: {{ops_per_second:.2f}}")
    
    return duration, ops_per_second

def main():
    print("NLM Performance Benchmark")
    print("=" * 40)
    
    # Parse arguments
    parser = argparse.ArgumentParser(description="NLM Performance Benchmark")
    parser.add_argument("--duration", type=float, default=60.0, help="Benchmark duration in seconds")
    
    args = parser.parse_args()
    
    # Run benchmarks
    results = []
    
    # Brain creation benchmark
    brain_time, brains_per_sec = benchmark_brain_creation(50)
    results.append(("Brain Creation", brain_time, brains_per_sec))
    
    # Simulation benchmark
    brain = pynlm.Brain(neuron_count=100, region_count=1)
    brain.initialize()
    sim_time, steps_per_sec = benchmark_simulation(brain, 500)
    results.append(("Simulation", sim_time, steps_per_sec))
    
    # Memory operations benchmark
    mem_time, mem_ops_per_sec = benchmark_memory_operations(brain, 100)
    results.append(("Memory Operations", mem_time, mem_ops_per_sec))
    
    # Print summary
    print("\nBenchmark Summary")
    print("=" * 40)
    
    for benchmark_name, duration, ops_per_sec in results:
        print(f"{benchmark_name:20} - {duration:6.3f}s, {ops_per_sec:8.2f} ops/sec")
    
    print(f"\nBenchmark completed in {sum(r[1] for r in results):.3f} seconds")

if __name__ == "__main__":
    main()
'''
    
    with open(benchmark_script, "w") as f:
        f.write(benchmark_content)
    
    os.chmod(benchmark_script, 0o755)
    print(f"✓ Created benchmark script: {benchmark_script}")
    
    # Run benchmark
    try:
        result = subprocess.run([
            sys.executable, str(benchmark_script),
            "--duration", str(args.duration)
        ], check=True, capture_output=not args.verbose)
        
        if result.returncode == 0:
            print("✓ Benchmark completed successfully!")
        else:
            print("✗ Benchmark failed!")
            return 1
            
    except Exception as e:
        print(f"✗ Benchmark failed: {e}")
        return 1
    
    return 0

def main():
    """Main demo function."""
    print_banner()
    
    parser = argparse.ArgumentParser(
        description="NLM Phase 6 Demo - Interactive Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Available Commands:
  run          Run a brain simulation
  test         Run integration tests
  explore      Explore brain components
  benchmark    Run performance benchmarks
  help         Show help information
  version      Show version information

Examples:
  nlm-demo run --neurons 1000 --steps 5000
  nlm-demo test --mode full
  nlm-demo explore --component memory
  nlm-demo benchmark --duration 30
        """
    )
    
    parser.add_argument("command", choices=["run", "test", "explore", "benchmark", "help", "version"],
                       help="Command to execute")
    
    # Common arguments for commands that need them
    parser.add_argument("--verbose", "-v", action="store_true", help="Enable verbose output")
    parser.add_argument("--output", help="Output file for results")
    
    # Run command specific arguments
    run_parser = parser.add_argument_group("run command arguments")
    run_parser.add_argument("--neurons", type=int, default=500, help="Number of neurons (default: 500)")
    run_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps (default: 1000)")
    run_parser.add_argument("--regions", type=int, default=1, help="Number of brain regions (default: 1)")
    run_parser.add_argument("--prob", type=float, default=0.1, help="Connection probability (default: 0.1)")
    run_parser.add_argument("--quick", action="store_true", help="Run quick test only")
    run_parser.add_argument("--full", action="store_true", help="Run full integration test")
    run_parser.add_argument("--diagnostics", action="store_true", help="Run system diagnostics")
    run_parser.add_argument("--no-checkpointing", action="store_true", help="Disable checkpoint testing")
    run_parser.add_argument("--no-replay", action="store_true", help="Disable replay testing")
    run_parser.add_argument("--no-development", action="store_true", help="Disable development system")
    
    # Test command specific arguments
    test_parser = parser.add_argument_group("test command arguments")
    test_parser.add_argument("--mode", choices=["all", "brain", "simulation", "memory", "neuromod", "python"], 
                            default="all", help="Test mode (default: all)")
    
    # Explore command specific arguments
    explore_parser = parser.add_argument_group("explore command arguments")
    explore_parser.add_argument("--component", choices=["memory", "neuro", "cognition", "all"], 
                               default="all", help="Component to explore (default: all)")
    explore_parser.add_argument("--details", action="store_true", help="Show detailed information")
    
    # Benchmark command specific arguments
    benchmark_parser = parser.add_argument_group("benchmark command arguments")
    benchmark_parser.add_argument("--duration", type=float, default=60.0, help="Benchmark duration in seconds (default: 60)")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    if args.command == "help":
        return handle_help(args)
    elif args.command == "version":
        return handle_version(args)
    elif args.command == "run":
        return handle_run(args)
    elif args.command == "test":
        return handle_test(args)
    elif args.command == "explore":
        return handle_explore(args)
    elif args.command == "benchmark":
        return handle_benchmark(args)
    else:
        print(f"ERROR: Unknown command: {args.command}")
        return 1

if __name__ == "__main__":
    sys.exit(main())