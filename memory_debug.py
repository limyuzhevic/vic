#!/usr/bin/env python3
# NLM Memory Pool Debugging Utilities
# Advanced memory debugging tools for NLM neural simulation

import sys
import os
import json
import time
import tracemalloc
import psutil
from datetime import datetime
from pathlib import Path

# Add the parent directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError:
    print("ERROR: NLM Python bindings not found!")
    print("Please build and install NLM Python bindings first.")
    sys.exit(1)

class MemoryTracker:
    """Track memory usage of NLM components"""
    
    def __init__(self):
        self.snapshots = []
        self.start_time = time.time()
        self.process = psutil.Process()
        
    def take_snapshot(self, label=""):
        """Take a memory snapshot"""
        current = tracemalloc.take_snapshot()
        self.snapshots.append({
            "timestamp": time.time() - self.start_time,
            "label": label,
            "snapshot": current
        })
        
    def get_memory_stats(self):
        """Get current memory statistics"""
        return {
            "rss": self.process.memory_info().rss,
            "vms": self.process.memory_info().vms,
            "shared": self.process.memory_info().shared,
            "text": self.process.memory_info().text,
            "data": self.process.memory_info().data,
            "dirty": self.process.memory_info().dirty,
            "uss": self.process.memory_info().uss if hasattr(self.process.memory_info(), 'uss') else None
        }
    
    def analyze_snapshot_diff(self, snapshot1, snapshot2):
        """Analyze memory difference between two snapshots"""
        top_stats = snapshot2.compare_to(snapshot1, 'lineno')
        
        stats = []
        for stat in top_stats[:10]:
            stats.append({
                "filename": stat.traceback.format()[0] if stat.traceback else "unknown",
                "lineno": stat.traceback[0].lineno if stat.traceback else 0,
                "size": stat.size,
                "count": stat.count,
                "traceback": [frame.filename + ":" + str(frame.lineno) for frame in stat.traceback]
            })
        
        return stats

class NLMMemoryDebugger:
    """Advanced memory debugging for NLM brain simulations"""
    
    def __init__(self):
        self.tracker = MemoryTracker()
        self.brain = None
        self.agent = None
        self.results = {}
        self.monitoring = False
        
    def start_monitoring(self):
        """Start monitoring memory usage"""
        tracemalloc.start(10)  # Track 10 frames
        self.tracker.take_snapshot("monitoring_start")
        self.monitoring = True
        print("Memory monitoring started")
        
    def stop_monitoring(self):
        """Stop monitoring memory usage"""
        self.tracker.take_snapshot("monitoring_stop")
        tracemalloc.stop()
        self.monitoring = False
        print("Memory monitoring stopped")
        
    def create_brain_with_memory_tracking(self, config=None):
        """Create brain with memory tracking enabled"""
        print("Creating brain with memory tracking...")
        
        # Start monitoring before brain creation
        if not self.monitoring:
            self.start_monitoring()
        
        # Take snapshot before brain creation
        self.tracker.take_snapshot("before_brain")
        
        # Create brain
        config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Take snapshot after brain creation
        self.tracker.take_snapshot("after_brain")
        
        # Take snapshot after brain initialization
        self.tracker.take_snapshot("after_init")
        
        # Get initial memory stats
        initial_stats = self.tracker.get_memory_stats()
        
        # Create world and agent if requested
        if config.getOr("create_agent", False):
            print("Creating agent...")
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(world)
            
            # Take snapshot after agent creation
            self.tracker.take_snapshot("after_agent")
            
            # Enable subsystems
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            
        # Get final memory stats
        final_stats = self.tracker.get_memory_stats()
        
        # Analyze memory changes
        if len(self.tracker.snapshots) >= 3:
            brain_memory = self.tracker.analyze_snapshot_diff(
                self.tracker.snapshots[1]["snapshot"],
                self.tracker.snapshots[2]["snapshot"]
            )
            
            self.results["brain_creation"] = {
                "memory_stats_before": initial_stats,
                "memory_stats_after": final_stats,
                "top_memory_allocations": brain_memory,
                "memory_increase_bytes": final_stats["rss"] - initial_stats["rss"]
            }
        
        print(f"Brain created with {self.brain.getTotalNeuronCount()} neurons")
        if self.agent:
            print("Agent created and initialized")
        
        return self.brain
        
    def run_simulation_with_memory_tracking(self, steps=100, enable_learning=True):
        """Run simulation with detailed memory tracking"""
        if not self.brain:
            print("ERROR: No brain created!")
            return
        
        print(f"Running simulation with memory tracking: {steps} steps")
        
        # Take snapshot before simulation
        self.tracker.take_snapshot("before_simulation")
        
        # Run simulation
        for step in range(steps):
            self.brain.step(step)
            
            # Agent processing
            if self.agent and step % 10 == 0:
                world = pynlm.createSimpleWorld()
                world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
                world.reset()
                
                percept = world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                action = self.agent.decodeMotorCommand()
                
                if enable_learning:
                    reward = 0.1
                    self.agent.applyRewardModulation(reward, 0.0)
            
            # Take snapshot at milestones
            if step % 50 == 0:
                self.tracker.take_snapshot(f"step_{step}")
        
        # Take snapshot after simulation
        self.tracker.take_snapshot("after_simulation")
        
        # Get simulation memory stats
        sim_stats = self.tracker.get_memory_stats()
        
        # Analyze memory changes during simulation
        if len(self.tracker.snapshots) >= 4:
            sim_memory = self.tracker.analyze_snapshot_diff(
                self.tracker.snapshots[-2]["snapshot"],
                self.tracker.snapshots[-1]["snapshot"]
            )
            
            self.results["simulation"] = {
                "memory_stats_before": self.tracker.snapshots[-2]["snapshot"],
                "memory_stats_after": self.tracker.snapshots[-1]["snapshot"],
                "top_memory_allocations": sim_memory,
                "memory_increase_bytes": sim_stats["rss"] - self.tracker.get_memory_stats()["rss"]
            }
        
        print(f"Simulation completed: {self.brain.getTotalSpikeCount()} spikes")
        
    def analyze_memory_leaks(self):
        """Analyze potential memory leaks"""
        print("\n" + "="*50)
        print("MEMORY LEAK ANALYSIS")
        print("="*50)
        
        if len(self.tracker.snapshots) < 3:
            print("ERROR: Need at least 3 snapshots for leak analysis")
            return
        
        # Analyze snapshots in chronological order
        for i in range(1, len(self.tracker.snapshots)):
            prev_snapshot = self.tracker.snapshots[i-1]["snapshot"]
            curr_snapshot = self.tracker.snapshots[i]["snapshot"]
            
            memory_diff = curr_snapshot.compare_to(prev_snapshot, 'lineno')
            
            # Check for significant memory increases
            total_increase = sum(stat.size for stat in memory_diff)
            
            if total_increase > 1024 * 1024:  # More than 1MB
                print(f"\nSignificant memory increase between {self.tracker.snapshots[i-1]['label']} and {self.tracker.snapshots[i]['label']}: {total_increase / 1024:.1f} KB")
                
                for stat in memory_diff[:5]:
                    if stat.size > 0:
                        print(f"  {stat.traceback.format()[0] if stat.traceback else 'unknown'}:{stat.traceback[0].lineno if stat.traceback else 0}")
                        print(f"    Size: {stat.size / 1024:.1f} KB, Count: {stat.count}")
    
    def get_memory_efficiency_report(self):
        """Generate memory efficiency report"""
        print("\n" + "="*50)
        print("MEMORY EFFICIENCY REPORT")
        print("="*50)
        
        if not hasattr(self.brain, 'logStatus'):
            print("ERROR: Brain not initialized properly")
            return
        
        # Get current memory stats
        current_stats = self.tracker.get_memory_stats()
        
        # Log brain status (which includes memory usage)
        print("Brain status:")
        self.brain.logStatus()
        
        # Calculate efficiency metrics
        total_objects = self.brain.getTotalNeuronCount() + self.brain.getTotalSynapseCount()
        
        if total_objects > 0:
            memory_per_object = current_stats["rss"] / total_objects
            print(f"\nMemory efficiency metrics:")
            print(f"  Memory per object: {memory_per_object:.1f} bytes")
            print(f"  RSS: {current_stats['rss'] / 1024 / 1024:.1f} MB")
            print(f"  Total objects: {total_objects}")
        
        # Check for memory issues
        if current_stats["rss"] > 500 * 1024 * 1024:  # More than 500MB
            print(f"\nWARNING: High memory usage detected: {current_stats['rss'] / 1024 / 1024:.1f} MB")
        
        return {
            "total_memory_bytes": current_stats["rss"],
            "memory_per_neuron": current_stats["rss"] / self.brain.getTotalNeuronCount() if self.brain.getTotalNeuronCount() > 0 else 0,
            "memory_per_synapse": current_stats["rss"] / self.brain.getTotalSynapseCount() if self.brain.getTotalSynapseCount() > 0 else 0,
            "snapshots_taken": len(self.tracker.snapshots)
        }
    
    def save_debug_data(self, filepath=None):
        """Save debug data to file"""
        if filepath is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filepath = f"nlm_memory_debug_{timestamp}.json"
        
        # Convert snapshots to serializable format
        snapshot_data = []
        for snapshot in self.tracker.snapshots:
            snapshot_data.append({
                "timestamp": snapshot["timestamp"],
                "label": snapshot["label"],
                "stats": self.tracker.get_memory_stats()
            })
        
        # Compile debug data
        debug_data = {
            "timestamp": datetime.now().isoformat(),
            "brain_created": self.brain is not None,
            "agent_created": self.agent is not None,
            "monitoring_active": self.monitoring,
            "snapshots": snapshot_data,
            "results": self.results
        }
        
        with open(filepath, 'w') as f:
            json.dump(debug_data, f, indent=2)
        
        print(f"Debug data saved to: {filepath}")
        return filepath

def main():
    parser = argparse.ArgumentParser(description="NLM Memory Pool Debugging Utilities")
    parser.add_argument("--create-brain", action="store_true", help="Create brain for testing")
    parser.add_argument("--simulation-steps", type=int, default=100, help="Number of simulation steps")
    parser.add_argument("--enable-learning", action="store_true", help="Enable learning in agent")
    parser.add_argument("--output", type=str, help="Output file for debug data")
    parser.add_argument("--analyze-leaks", action="store_true", help="Analyze for memory leaks")
    
    args = parser.parse_args()
    
    # Create debugger
    debugger = NLMMemoryDebugger()
    
    if args.create_brain:
        print("Creating NLM brain with memory tracking...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = debugger.create_brain_with_memory_tracking(config)
        
        if args.simulation_steps > 0:
            print(f"\nRunning simulation with {args.simulation_steps} steps...")
            debugger.run_simulation_with_memory_tracking(args.simulation_steps, args.enable_learning)
        
        # Generate efficiency report
        efficiency_report = debugger.get_memory_efficiency_report()
        
        # Analyze for leaks if requested
        if args.analyze_leaks:
            debugger.analyze_memory_leaks()
        
        # Save debug data
        if args.output:
            debugger.save_debug_data(args.output)
        
        print("\nMemory debugging completed successfully!")
        
    else:
        print("No action specified. Use --create-brain to start debugging.")
        print("Example:")
        print("  python3 memory_debug.py --create-brain --simulation-steps 1000 --enable-learning --output debug_data.json")

if __name__ == "__main__":
    main()