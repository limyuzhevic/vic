import enhanced_pynlm as nlm
import time

def main():
    print("=== NLM Enhanced API Benchmark ===")
    
    # Benchmark 1: Brain creation and initialization
    print("\nBenchmark 1: Brain creation and initialization")
    start_time = time.time()
    
    for i in range(5):
        brain = nlm.Brain()
        brain.initialize()
    
    end_time = time.time()
    print(f"  5 brain creations: {end_time - start_time:.3f} seconds")
    print(f"  Average per brain: {(end_time - start_time) / 5:.3f} seconds")
    
    # Benchmark 2: Simulation performance
    print("\nBenchmark 2: Simulation performance")
    brain = nlm.Brain()
    brain.initialize()
    
    start_time = time.time()
    brain.simulate(100)
    end_time = time.time()
    
    print(f"  100 simulation steps: {end_time - start_time:.3f} seconds")
    print(f"  Average per step: {(end_time - start_time) / 100:.5f} seconds")
    
    # Benchmark 3: Statistics collection
    print("\nBenchmark 3: Statistics collection")
    
    start_time = time.time()
    for _ in range(100):
        stats = brain.getStats()
    end_time = time.time()
    
    print(f"  100 stats calls: {end_time - start_time:.3f} seconds")
    print(f"  Average per call: {(end_time - start_time) / 100:.5f} seconds")
    
    # Benchmark 4: Save performance
    print("\nBenchmark 4: Save performance")
    
    start_time = time.time()
    for i in range(3):
        brain.save(f"benchmark_{i}")
    end_time = time.time()
    
    print(f"  3 save operations: {end_time - start_time:.3f} seconds")
    print(f"  Average per save: {(end_time - start_time) / 3:.3f} seconds")
    
    print("\n=== Benchmark Complete ===")

if __name__ == "__main__":
    main()
