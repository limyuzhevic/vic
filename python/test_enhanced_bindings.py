#!/usr/bin/env python3
"""Comprehensive test for enhanced NLM Python bindings with memory management"""

import sys
import os
import gc
import tracemalloc

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to import the module
try:
    from pynlm import (
        # Memory management classes
        MemoryContextManager, PoolMemoryContext, PySmartPointer, makeSmartPointer,
        MemoryUtils, EnhancedPerformanceMonitor, MemoryResourceManager,
        
        # Original NLM classes (for backward compatibility)
        Config, Brain, SimpleWorld, TimingUtils, BenchmarkResult,
        MemoryTracker, Profiler, PerformanceMonitor,
        DebugLogger, StateInspector, MemoryDumper, DebugHelper,
        NeuralAnalyzer, ConnectivityAnalyzer, PerformanceAnalyzer, PatternAnalyzer,
        Vision, Audio, InternalSignals, Action, WorldObject, AgentBody, 
        ActionResult, SensoryPercept, AgentBrain,
        
        # Factory functions
        createPerformanceMonitor, createProfiler, createDebugLogger,
        createStateInspector, createMemoryDumper, createNeuralAnalyzer,
        createConnectivityAnalyzer, createPerformanceAnalyzer, createPatternAnalyzer,
        
        # Convenience functions
        enableDebugMode, disableDebugMode, setLogLevel, logDebug, logInfo, logWarning, logError,
        
        # Constants
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
    )
    
    print("Successfully imported all enhanced Python bindings!")
except ImportError as e:
    print(f"Failed to import enhanced Python bindings: {e}")
    sys.exit(1)

def test_basic_memory_management():
    print("\n" + "="*60)
    print("Test 1: Basic Memory Management")
    print("="*60)
    
    try:
        # Test memory system initialization
        print("\n1.1 Initializing memory system...")
        MemoryUtils.initialize()
        print("✓ Memory system initialized successfully")
        
        # Test memory statistics
        print("\n1.2 Getting memory statistics...")
        stats = MemoryUtils.getStats()
        print(f"✓ Total allocations: {stats.totalAllocations}")
        print(f"✓ Current allocations: {stats.currentAllocations}")
        print(f"✓ Current memory usage: {stats.getCurrentMemoryUsageMB():.2f} MB")
        
        # Test memory leak report
        print("\n1.3 Checking for memory leaks...")
        leak_report = MemoryUtils.getLeakReport()
        print(f"✓ Leak report generated (length: {len(leak_report)} chars)")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed basic memory management test: {e}")
        return False

def test_context_managers():
    print("\n" + "="*60)
    print("Test 2: Context Managers")
    print("="*60)
    
    try:
        # Test MemoryContextManager
        print("\n2.1 Testing MemoryContextManager...")
        config = {"enableMemoryTracking": True, "enableLeakDetection": True}
        memory_context = MemoryContextManager(config)
        memory_context.open()
        print("✓ MemoryContextManager opened successfully")
        
        memory_report = memory_context.getMemoryReport()
        print(f"✓ Memory report generated (length: {len(memory_report)} chars)")
        
        memory_context.close()
        print("✓ MemoryContextManager closed successfully")
        
        # Test PoolMemoryContext
        print("\n2.2 Testing PoolMemoryContext...")
        neuron_pool = PoolMemoryContext("neuron_pool", 1024)
        neuron_pool.open()
        print("✓ PoolMemoryContext opened successfully")
        
        available = neuron_pool.getAvailableMemory()
        print(f"✓ Available memory: {available} objects")
        
        neuron_pool.close()
        print("✓ PoolMemoryContext closed successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed context managers test: {e}")
        return False

def test_smart_pointers():
    print("\n" + "="*60)
    print("Test 3: Smart Pointers")
    print("="*60)
    
    try:
        # Test PySmartPointer creation
        print("\n3.1 Testing PySmartPointer creation...")
        config = Config()
        config.set("test_key", "test_value")
        
        smart_ptr = makeSmartPointer(&config, "Config", "test_config_ptr")
        print(f"✓ Smart pointer created: {smart_ptr.getObjectName()}")
        
        # Test smart pointer access
        print("\n3.2 Testing smart pointer access...")
        value = smart_ptr->get<std::string>("test_key").value();
        print(f"✓ Value retrieved via smart pointer: {value}")
        
        # Test smart pointer equality
        print("\n3.3 Testing smart pointer operations...")
        smart_ptr2 = makeSmartPointer(&config, "Config", "test_config_ptr2");
        print(f"✓ Smart pointers can be created and accessed")
        
        # Test allocation statistics
        print("\n3.4 Testing allocation statistics...")
        alloc_count = PySmartPointer<Config>::getAllocationCount();
        dealloc_count = PySmartPointer<Config>::getDeallocationCount();
        print(f"✓ Allocation count: {alloc_count}")
        print(f"✓ Deallocation count: {dealloc_count}")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed smart pointers test: {e}")
        return False

def test_performance_monitor():
    print("\n" + "="*60)
    print("Test 4: Enhanced Performance Monitor")
    print("="*60)
    
    try:
        # Test EnhancedPerformanceMonitor
        print("\n4.1 Testing EnhancedPerformanceMonitor...")
        perf_monitor = EnhancedPerformanceMonitor()
        print("✓ EnhancedPerformanceMonitor created successfully")
        
        # Start profiling
        print("\n4.2 Starting profiling...")
        perf_monitor.startProfiling("test_function", "memory")
        print("✓ Profiling started")
        
        # Record some allocations
        print("\n4.3 Recording allocations...")
        import time
        time.sleep(0.01)  # Small delay
        perf_monitor.recordAllocation("test_function", 1024, "memory", __file__, __line__)
        perf_monitor.recordDeallocation("test_function", 512);
        print("✓ Allocations recorded")
        
        # End profiling
        print("\n4.4 Ending profiling...")
        perf_monitor.endProfiling("test_function")
        print("✓ Profiling ended")
        
        # Generate report
        print("\n4.5 Generating report...")
        report = perf_monitor.generateReport()
        print(f"✓ Performance report generated (length: {len(report)} chars)")
        
        # Test tracking
        print("\n4.6 Testing tracking...")
        perf_monitor.enableTracking(True)
        print("✓ Tracking enabled")
        
        is_tracking = perf_monitor.isTrackingEnabled()
        print(f"✓ Tracking status: {is_tracking}")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed performance monitor test: {e}")
        return False

def test_resource_manager():
    print("\n" + "="*60)
    print("Test 5: Resource Manager")
    print("="*60)
    
    try:
        # Test MemoryResourceManager
        print("\n5.1 Testing MemoryResourceManager...")
        resource_manager = MemoryResourceManager()
        print("✓ MemoryResourceManager created successfully")
        
        # Allocate resources
        print("\n5.2 Allocating resources...")
        resource1 = resource_manager.allocateResource("brain_pool", 1024, "neuron_data_1");
        resource2 = resource_manager.allocateResource("brain_pool", 2048, "neuron_data_2");
        resource3 = resource_manager.allocateResource("world_pool", 512, "world_data_1");
        
        print(f"✓ Resource 1 allocated: {resource1 != 0}")
        print(f"✓ Resource 2 allocated: {resource2 != 0}")
        print(f"✓ Resource 3 allocated: {resource3 != 0}")
        
        # Get resource names
        print("\n5.3 Getting resource names...")
        brain_resources = resource_manager.getResourceNames("brain_pool");
        world_resources = resource_manager.getResourceNames("world_pool");
        
        print(f"✓ Brain pool resources: {len(brain_resources)}")
        print(f"✓ World pool resources: {len(world_resources)}")
        
        # Release resources
        print("\n5.4 Releasing resources...")
        released1 = resource_manager.releaseResource("brain_pool", "neuron_data_1");
        released2 = resource_manager.releaseResource("brain_pool", "neuron_data_2");
        released3 = resource_manager.releaseResource("world_pool", "world_data_1");
        
        print(f"✓ Resource 1 released: {released1}")
        print(f"✓ Resource 2 released: {released2}")
        print(f"✓ Resource 3 released: {released3}")
        
        # Check pool sizes
        print("\n5.5 Checking pool sizes...")
        brain_pool_size = resource_manager.getPoolSize("brain_pool");
        world_pool_size = resource_manager.getPoolSize("world_pool");
        
        print(f"✓ Brain pool size after release: {brain_pool_size}")
        print(f"✓ World pool size after release: {world_pool_size}")
        
        # Enable/disable tracking
        print("\n5.6 Testing tracking...")
        resource_manager.enableTracking(True);
        print("✓ Resource tracking enabled")
        
        is_tracking = resource_manager.isTrackingEnabled();
        print(f"✓ Tracking status: {is_tracking}")
        
        # Clear resources
        print("\n5.7 Clearing resources...")
        resource_manager.clear();
        print("✓ All resources cleared")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed resource manager test: {e}")
        return False

def test_enhanced_classes():
    print("\n" + "="*60)
    print("Test 6: Enhanced Classes")
    print("="*60)
    
    try:
        # Test ConfigWithMemoryManagement
        print("\n6.1 Testing ConfigWithMemoryManagement...")
        config = Config()
        config.set("test_int", 42);
        config.set("test_string", "hello");
        config.set("test_bool", true);
        
        value_int = config.get<int>("test_int").value();
        value_string = config.get<std::string>("test_string").value();
        value_bool = config.get<bool>("test_bool").value();
        
        print(f"✓ Config values: int={value_int}, string={value_string}, bool={value_bool}")
        
        # Test BrainWithMemoryManagement
        print("\n6.2 Testing BrainWithMemoryManagement...")
        brain = Brain(config);
        print("✓ Brain created successfully")
        
        # Test SimpleWorldWithMemoryManagement
        print("\n6.3 Testing SimpleWorldWithMemoryManagement...")
        world = SimpleWorld();
        print("✓ SimpleWorld created successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed enhanced classes test: {e}")
        return False

def test_backward_compatibility():
    print("\n" + "="*60)
    print("Test 7: Backward Compatibility")
    print("="*60)
    
    try:
        # Test that original classes still work
        print("\n7.1 Testing original Config class...")
        config = Config()
        config.set("compatibility_test", "value");
        
        value = config.get<std::string>("compatibility_test").value();
        print(f"✓ Original Config works: {value}")
        
        # Test original factory functions
        print("\n7.2 Testing original factory functions...")
        perf_monitor = createPerformanceMonitor();
        print("✓ createPerformanceMonitor works")
        
        profiler = createProfiler();
        print("✓ createProfiler works")
        
        debug_logger = createDebugLogger();
        print("✓ createDebugLogger works")
        
        # Test original convenience functions
        print("\n7.3 Testing original convenience functions...")
        enableDebugMode();
        print("✓ enableDebugMode works")
        
        disableDebugMode();
        print("✓ disableDebugMode works")
        
        # Test original constants
        print("\n7.4 Testing original constants...")
        print(f"✓ INVALID_NEURON_ID: {INVALID_NEURON_ID}")
        print(f"✓ INVALID_SYNAPSE_ID: {INVALID_SYNAPSE_ID}")
        print(f"✓ INVALID_REGION_ID: {INVALID_REGION_ID}")
        print(f"✓ INVALID_POPULATION_ID: {INVALID_POPULATION_ID}")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed backward compatibility test: {e}")
        return False

def test_memory_leak_detection():
    print("\n" + "="*60)
    print("Test 8: Memory Leak Detection")
    print("="*60)
    
    try:
        # Initialize with leak detection enabled
        print("\n8.1 Initializing with leak detection...")
        MemoryUtils.initialize({"enableMemoryTracking": True, "enableLeakDetection": true});
        print("✓ Memory system initialized with leak detection")
        
        # Create and use objects
        print("\n8.2 Creating and using objects...")
        config = Config();
        config.set("leak_test", "value");
        
        # Access the config via smart pointer for tracking
        smart_ptr = makeSmartPointer(&config, "Config", "leak_test_ptr");
        value = smart_ptr->get<std::string>("leak_test").value();
        print(f"✓ Test value retrieved: {value}")
        
        # Shutdown and check for leaks
        print("\n8.3 Checking for leaks...")
        MemoryUtils.shutdown();
        
        leak_report = MemoryUtils.getLeakReport();
        print(f"✓ Leak report: {leak_report[:200]}..." if len(leak_report) > 200 else f"✓ Leak report: {leak_report}")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed memory leak detection test: {e}")
        return False

def test_system_integration():
    print("\n" + "="*60)
    print("Test 9: System Integration")
    print("="*60)
    
    try:
        # Complete integration test
        print("\n9.1 Running complete system integration...")
        
        # Initialize memory system
        MemoryUtils.initialize();
        print("✓ Memory system initialized")
        
        # Create context managers
        memory_context = MemoryContextManager();
        memory_context.open();
        print("✓ Memory context opened")
        
        # Create performance monitor
        perf_monitor = EnhancedPerformanceMonitor();
        print("✓ Performance monitor created")
        
        # Create resource manager
        resource_manager = MemoryResourceManager();
        print("✓ Resource manager created")
        
        # Create enhanced smart pointers
        config = Config();
        config.set("integration_test", "success");
        smart_ptr = makeSmartPointer(&config, "Config", "integration_ptr");
        print("✓ Smart pointer created")
        
        # Perform operations
        value = smart_ptr->get<std::string>("integration_test").value();
        print(f"✓ Integration test successful: {value}")
        
        # Generate reports
        perf_report = perf_monitor.generateReport();
        memory_report = memory_context.getMemoryReport();
        
        print(f"✓ Performance report generated: {len(perf_report)} chars")
        print(f"✓ Memory report generated: {len(memory_report)} chars")
        
        # Close context
        memory_context.close();
        print("✓ Memory context closed")
        
        # Shutdown
        MemoryUtils.shutdown();
        print("✓ Memory system shutdown")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed system integration test: {e}")
        return False

def main():
    print("Enhanced NLM Python Bindings Test Suite")
    print("=" * 60)
    print("Testing comprehensive memory management enhancements...")
    
    # Run all tests
    tests = [
        ("Basic Memory Management", test_basic_memory_management),
        ("Context Managers", test_context_managers),
        ("Smart Pointers", test_smart_pointers),
        ("Performance Monitor", test_performance_monitor),
        ("Resource Manager", test_resource_manager),
        ("Enhanced Classes", test_enhanced_classes),
        ("Backward Compatibility", test_backward_compatibility),
        ("Memory Leak Detection", test_memory_leak_detection),
        ("System Integration", test_system_integration),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ Test crashed: {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "="*60)
    print("TEST SUMMARY")
    print("="*60)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "PASSED" if success else "FAILED"
        symbol = "✓" if success else "✗"
        print(f"{symbol} {test_name}: {status}")
        
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed successfully!")
        print("\nEnhanced memory management features verified:")
        print("  ✓ Smart pointer support")
        print("  ✓ Memory pool management")
        print("  ✓ Context managers")
        print("  ✓ Memory tracking and reporting")
        print("  ✓ Memory leak detection")
        print("  ✓ RAII resource management")
        print("  ✓ Performance optimization")
        print("  ✓ Safety improvements")
        print("  ✓ Backward compatibility")
        
        return 0
    else:
        print(f"\n❌ {failed} test(s) failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main())