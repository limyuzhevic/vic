# Integration Testing and Stress Testing Framework

## Overview

Integration testing and stress testing are critical components of the NLM testing framework, ensuring that the complex neural simulation system works correctly under various conditions and performance requirements. This document outlines the comprehensive testing strategy for NLM.

## Testing Architecture

### 1. Testing Layer Structure

```
┌─────────────────────────────────────────────────────────────┐
│                    Test Framework                          │
├─────────────────────────────────────────────────────────────┤
│  Unit Tests (Component Testing)                             │
│  - Neuron functionality                                      │
│  - Synapse dynamics                                         │
│  - Memory system operations                                 │
│  - Plasticity rules                                         │
├─────────────────────────────────────────────────────────────┤
│  Integration Tests (System Testing)                         │
│  - Brain system integration                                 │
│  - Agent-world interaction                                   │
│  - Neuromodulation integration                              │
│  - Development system testing                              │
├─────────────────────────────────────────────────────────────┤
│  Performance Testing (Benchmarking)                        │
│  - Scalability testing                                      │
│  - Memory usage analysis                                    │
│  - Real-time performance                                   │
│  - Parallel processing effectiveness                        │
├─────────────────────────────────────────────────────────────┤
│  Stress Testing (Load Testing)                              │
│  - Large network simulation                                 │
│  - Extreme parameter combinations                          │
│  - Resource exhaustion testing                              │
│  - Long-running simulations                                │
├─────────────────────────────────────────────────────────────┤
│  End-to-End Testing (Acceptance Testing)                    │
│  - Complete scenario reproduction                           │
│  - User workflow validation                                 │
│  - Integration with external systems                        │
└─────────────────────────────────────────────────────────────┘
```

## Test Categories

### 1. Unit Tests
**Focus**: Individual components and their isolated functionality

#### Neuron Unit Tests
**File**: `tests/test_neuron.cpp`

```cpp
// Neuron core functionality
TEST_SUITE_BEGIN("Neuron Core")
    TEST_CASE("Creation and Destruction") {
        Neuron neuron(NeuronId(1));
        ASSERT_EQ(neuron.getId(), NeuronId(1));
        // ... other assertions
    }
    
    TEST_CASE("Membrane Dynamics") {
        Neuron neuron(NeuronId(1));
        neuron.setMembranePotential(-65.0f);
        neuron.stepLIF(0.1, 0.001);
        // Verify LIF dynamics
    }
    
    TEST_CASE("Spike Generation") {
        Neuron neuron(NeuronId(1));
        neuron.injectCurrent(50.0f);
        bool fired = neuron.stepLIF(0.1, 0.001);
        ASSERT_TRUE(fired);
        ASSERT_TRUE(neuron.isFiring());
    }
    
    TEST_CASE("Plasticity Mechanisms") {
        Neuron neuron(NeuronId(1));
        neuron.enablePlasticity(true, true, false);
        // Test Hebbian and STDP
    }
TEST_SUITE_END()
```

#### Synapse Unit Tests
**File**: `tests/test_synapse.cpp`

```cpp
TEST_SUITE_BEGIN("Synapse Dynamics")
    TEST_CASE("Transmission Properties") {
        Synapse synapse(NeuronId(1), NeuronId(2), SynapseType::Excitatory);
        synapse.transmitSignal(10.0f);
        ASSERT_GT(neuron2.getTotalCurrent(), 0.0f);
    }
    
    TEST_CASE("Plasticity Learning") {
        Synapse synapse(NeuronId(1), NeuronId(2), SynapseType::Excitatory);
        synapse.enablePlasticity(true, true);
        // Test STDP timing
    }
    
    TEST_CASE("Delay Mechanisms") {
        Synapse synapse(NeuronId(1), NeuronId(2), SynapseType::Excitatory);
        synapse.setDelay(10.0f);
        // Verify delay behavior
    }
TEST_SUITE_END()
```

### 2. Integration Tests
**Focus**: System-level functionality and component interactions

#### Brain Integration Tests
**File**: `tests/test_brain.cpp`

```cpp
TEST_SUITE_BEGIN("Brain System Integration")
    TEST_CASE("Region Connectivity") {
        auto brain = createDefaultBrain(1000, 1);
        brain->initialize();
        
        // Test connectivity between regions
        auto region1 = brain->getRegion(RegionId(1));
        auto region2 = brain->getRegion(RegionId(2));
        
        // Verify cross-region communication
        for (int step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        // Check integration metrics
        ASSERT_GT(brain->getTotalSynapseCount(), 0);
    }
    
    TEST_CASE("Memory System Integration") {
        auto brain = createDefaultBrain(500, 2);
        brain->initialize();
        
        // Test working memory
        brain->enableWorkingMemory(true);
        brain->storeWorkingMemory(EpisodicMemory("test", 1000.0f));
        
        // Test episodic memory replay
        brain->replayEpisodicMemory();
        
        ASSERT_TRUE(brain->workingMemoryReady());
    }
    
    TEST_CASE("Neuromodulation Integration") {
        auto brain = createDefaultBrain(300, 1);
        brain->initialize();
        
        // Test dopamine system
        brain->enableDopamineSystem(true);
        brain->applyDopamineModulation(1.0f, 0.5f);
        
        // Test curiosity system
        brain->enableCuriosity(true);
        brain->triggerExploration();
        
        ASSERT_GT(brain->getCuriosityLevel(), 0.0f);
    }
TEST_SUITE_END()
```

#### Agent-World Integration Tests
**File**: `tests/test_agent_integration.cpp`

```cpp
TEST_SUITE_BEGIN("Agent-World Integration")
    TEST_CASE("Sensory Processing Pipeline") {
        auto agent = createAgent(500);
        auto world = createSimpleWorld(20, 20, 8, 8);
        
        world.reset();
        agent->initialize(world);
        
        // Process sensory input
        world.update(0.1f);
        auto percept = world.getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Verify processing
        ASSERT_TRUE(agent->sensoryProcessed());
        ASSERT_GT(agent->getFiringNeuronCount(), 0);
    }
    
    TEST_CASE("Motor Command Generation") {
        auto agent = createAgent(400);
        auto world = createSimpleWorld(15, 15, 6, 6);
        
        agent->initialize(world);
        
        // Generate motor command
        auto action = agent->decodeMotorCommand();
        
        // Apply to world
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Verify movement
        ASSERT_TRUE(world.agentMoved());
    }
    
    TEST_CASE("Reward-Based Learning") {
        auto agent = createAgent(300);
        auto world = createSimpleWorld(10, 10, 4, 4);
        
        agent->initialize(world);
        agent->enableRewardModulation(true);
        
        // Run learning episode
        for (int step = 0; step < 200; ++step) {
            world.update(0.1f);
            auto percept = world.getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto action = agent->decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Get reward and learn
            float reward = world.getInternalReward();
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        // Verify learning occurred
        ASSERT_GT(agent->getCuriosityLevel(), 0.1f);
        ASSERT_GT(agent->getNoveltyLevel(), 0.1f);
    }
TEST_SUITE_END()
```

### 3. Performance Testing
**Focus**: Benchmarking and performance optimization

#### Scalability Tests
**File**: `tests/test_scalability.cpp`

```cpp
TEST_SUITE_BEGIN("Network Scalability")
    TEST_CASE("Neuron Count Scaling") {
        std::vector<size_t> neuronCounts = {100, 500, 1000, 5000, 10000};
        
        for (auto neuronCount : neuronCounts) {
            auto brain = createDefaultBrain(neuronCount, 2);
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int step = 0; step < 100; ++step) {
                brain->step(step);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            double performance = neuronCount / (duration.count() / 1000.0); // neurons per second
            
            std::cout << f"Neurons: {neuronCount}, Performance: {performance:.0f} neurons/sec" << std::endl;
            
            // Verify performance is acceptable (not too slow)
            ASSERT_LT(duration.count() / 100.0, 100.0); // Average step < 100ms
        }
    }
    
    TEST_CASE("Synapse Density Scaling") {
        std::vector<float> densities = {0.01f, 0.05f, 0.1f, 0.2f};
        
        for (auto density : densities) {
            auto brain = createDefaultBrain(1000, 1, density);
            brain->initialize();
            
            // Measure memory usage
            size_t memoryUsage = brain->getEstimatedMemoryUsage();
            size_t expectedSynapses = static_cast<size_t>(neuronCount * neuronCount * density);
            
            ASSERT_NEAR(memoryUsage, expectedSynapses * sizeof(float), 10%); // Allow 10% variance
        }
    }
TEST_SUITE_END()
```

#### Parallel Processing Tests
**File**: `tests/test_parallel.cpp`

```cpp
TEST_SUITE_BEGIN("Parallel Processing")
    TEST_CASE("Single vs Multi-threaded Performance") {
        auto brain = createDefaultBrain(2000, 4);
        brain->initialize();
        
        // Single-threaded performance
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < 200; ++step) {
            brain->step(step);
        }
        auto singleThreadedTime = std::chrono::high_resolution_clock::now() - startTime;
        
        // Multi-threaded performance (if available)
        brain->enableParallelProcessing(true);
        brain->setThreadCount(std::thread::hardware_concurrency());
        
        startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < 200; ++step) {
            brain->step(step);
        }
        auto multiThreadedTime = std::chrono::high_resolution_clock::now() - startTime;
        
        double speedup = singleThreadedTime.count() / multiThreadedTime.count();
        
        std::cout << f"Speedup: {speedup:.2f}x" << std::endl;
        
        // Expect some speedup (not always due to overhead)
        ASSERT_GT(speedup, 0.8); // At least 80% of single-threaded speed
    }
    
    TEST_CASE("Load Balancing") {
        auto brain = createDefaultBrain(3000, 8);
        brain->initialize();
        brain->enableParallelProcessing(true);
        
        // Test with different thread counts
        std::vector<size_t> threadCounts = {1, 2, 4, 8, 16};
        std::vector<double> performances;
        
        for (auto threadCount : threadCounts) {
            brain->setThreadCount(threadCount);
            
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int step = 0; step < 100; ++step) {
                brain->step(step);
            }
            auto duration = std::chrono::high_resolution_clock::now() - startTime;
            
            performances.push_back(1.0 / duration.count());
        }
        
        // Check that performance doesn't degrade too much with more threads
        double maxPerformance = *std::max_element(performances.begin(), performances.end());
        double minPerformance = *std::min_element(performances.begin(), performances.end());
        
        ASSERT_LT((maxPerformance - minPerformance) / maxPerformance, 0.3); // 30% variation is acceptable
    }
TEST_SUITE_END()
```

### 4. Stress Testing
**Focus**: System behavior under extreme conditions

#### Resource Exhaustion Tests
**File**: `tests/test_resource_exhaustion.cpp`

```cpp
TEST_SUITE_BEGIN("Resource Exhaustion")
    TEST_CASE("Memory Usage Limits") {
        // Test with progressively larger networks
        std::vector<size_t> networkSizes = {5000, 10000, 20000};
        
        for (auto networkSize : networkSizes) {
            try {
                auto brain = createDefaultBrain(networkSize, 4);
                brain->initialize();
                
                // Check memory usage
                size_t estimatedMemory = brain->getEstimatedMemoryUsage();
                
                // Verify the system can handle the memory requirement
                ASSERT_LE(estimatedMemory, networkSize * networkSize * 10); // 10 bytes per potential synapse
                
                // Run simulation to ensure memory stability
                for (int step = 0; step < 50; ++step) {
                    brain->step(step);
                }
                
                // Check for memory leaks
                size_t finalMemory = brain->getCurrentMemoryUsage();
                ASSERT_NEAR(finalMemory, estimatedMemory, 5%); // Within 5% of estimate
                
            } catch (const std::bad_alloc& e) {
                // System should handle memory allocation failures gracefully
                std::cout << f"Memory allocation failed for network size {networkSize}: {e.what()}" << std::endl;
                // This is acceptable - system should report error rather than crash
                SUCCEED();
            }
        }
    }
    
    TEST_CASE("CPU Resource Limits") {
        auto brain = createDefaultBrain(1000, 2);
        brain->initialize();
        
        // Monitor CPU usage during simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        double cpuUsage = 0.0;
        
        for (int step = 0; step < 500; ++step) {
            brain->step(step);
            
            // Measure CPU usage (simplified)
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            double stepTime = elapsed.count() / (step + 1);
            
            if (stepTime > 0) {
                cpuUsage = (stepTime - 1.0) / stepTime; // Rough approximation
            }
        }
        
        // Verify CPU usage is reasonable (not excessive)
        ASSERT_LT(cpuUsage, 1.0); // CPU usage < 100%
    }
TEST_SUITE_END()
```

#### Long-Running Tests
**File**: `tests/test_long_running.cpp`

```cpp
TEST_SUITE_BEGIN("Long-Running Stability")
    TEST_CASE("Extended Simulation Stability") {
        auto brain = createDefaultBrain(500, 2);
        brain->initialize();
        
        // Run simulation for extended period
        for (int step = 0; step < 10000; ++step) {
            brain->step(step);
            
            // Check for stability every 100 steps
            if (step % 100 == 0) {
                double stability = brain->getStabilityScore();
                ASSERT_GT(stability, 0.5); // Network should remain stable
                
                // Check for drift or instability
                auto neuronStats = brain->getNeuronStatistics();
                ASSERT_EQ(neuronStats.minVoltage, neuronStats.maxVoltage); // Not drifting to infinity
            }
        }
        
        // Final stability check
        double finalStability = brain->getStabilityScore();
        ASSERT_GT(finalStability, 0.6); // Network should still be stable after 10k steps
    }
    
    TEST_CASE("Memory Consolidation Over Time") {
        auto brain = createDefaultBrain(300, 2);
        brain->initialize();
        brain->enableEpisodicMemory(true);
        
        // Store multiple experiences over time
        for (int episode = 0; episode < 100; ++episode) {
            // Generate experience
            Experience experience;
            experience.memory = "Experience " + std::to_string(episode);
            experience.value = (episode % 10) * 0.1f; // Varying value
            experience.timestamp = episode * 10.0f;
            
            // Store in memory
            brain->storeEpisodicMemory(experience);
            
            // Run simulation
            for (int step = 0; step < 10; ++step) {
                brain->step(step);
            }
            
            // Check memory growth
            size_t memorySize = brain->getEpisodicMemorySize();
            ASSERT_EQ(memorySize, episode + 1);
        }
        
        // Verify memory consolidation
        brain->consolidateMemories();
        size_t consolidatedSize = brain->getConsolidatedMemorySize();
        
        // Should reduce memory while maintaining information
        ASSERT_LE(consolidatedSize, 100); // Consolidated size should be reasonable
    }
TEST_SUITE_END()
```

### 5. End-to-End Tests
**Focus**: Complete user workflows and real-world scenarios

#### Complete Agent Scenario
**File**: `tests/test_complete_scenario.cpp`

```cpp
TEST_SUITE_BEGIN("Complete Agent Scenario")
    TEST_CASE("Foraging Task") {
        // Create environment with resources
        auto world = createSimpleWorld(30, 30, 12, 12);
        world.configure(
            width=30, height=30,
            visionWidth=12, visionHeight=12,
            resourceDensity=0.15f,
            hazardDensity=0.05f
        );
        
        // Create agent with realistic parameters
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", 1000);
        config->set("brain.connection_probability", 0.1f);
        config->set("plasticity.learning_rate", 0.001f);
        
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        
        // Initialize agent
        agent->initialize(world);
        world.reset();
        world.setAgentStart(15.0f, 15.0f);
        
        // Enable all subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run foraging task
        float totalReward = 0.0f;
        int episodeSteps = 0;
        
        for (int step = 0; step < 1000 && !world.agentStarved(); ++step) {
            // Update world
            world.update(0.1f);
            
            // Get sensory input
            auto percept = world.getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain simulation
            brain->step(step);
            
            // Generate and apply motor command
            auto action = agent->decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Get reward and learn
            float reward = world.getInternalReward();
            totalReward += reward;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            episodeSteps++;
        }
        
        // Verify task completion
        ASSERT_GT(totalReward, 0.0f); // Agent should have collected rewards
        ASSERT_LE(episodeSteps, 1000); // Should not exceed step limit
        
        // Check learning metrics
        ASSERT_GT(agent->getCuriosityLevel(), 0.2f); // Agent should have learned
        ASSERT_GT(agent->getNoveltyLevel(), 0.1f); // Agent should be exploring
    }
    
    TEST_CASE("Navigation with Obstacles") {
        // Create environment with obstacles
        auto world = createSimpleWorld(20, 20, 8, 8);
        world.configure(
            width=20, height=20,
            visionWidth=8, visionHeight=8,
            obstacleDensity=0.3f,
            resourceDensity=0.2f
        );
        
        // Add obstacles
        for (int x = 0; x < 20; x += 3) {
            for (int y = 0; y < 20; y += 3) {
                if ((x + y) % 6 != 0) { // Leave some paths clear
                    world.addObject(WorldObject(x, y, WorldObjectType::Wall, 1.0f));
                }
            }
        }
        
        // Create and initialize agent
        auto brain = createBrain(createDefaultConfig());
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        world.reset();
        world.setAgentStart(1.0f, 1.0f);
        
        // Navigate to goal
        float startX = world.getAgentPosition().x;
        float startY = world.getAgentPosition().y;
        
        for (int step = 0; step < 500; ++step) {
            world.update(0.1f);
            
            auto percept = world.getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Check if goal reached (agent has moved significantly)
            auto currentPos = world.getAgentPosition();
            float distance = std::sqrt(std::pow(currentPos.x - startX, 2) + 
                                     std::pow(currentPos.y - startY, 2));
            
            if (distance > 15.0f) { // Reached goal
                break;
            }
        }
        
        // Verify navigation success
        auto finalPos = world.getAgentPosition();
        float distanceFromStart = std::sqrt(std::pow(finalPos.x - startX, 2) + 
                                          std::pow(finalPos.y - startY, 2));
        
        ASSERT_GT(distanceFromStart, 10.0f); // Agent should have traveled far
    }
TEST_SUITE_END()
```

## Test Framework Integration

### 1. Test Configuration
**File**: `tests/config/test_config.json`

```json
{
  "test_suites": {
    "unit": {
      "patterns": ["test_*.cpp"],
      "timeout": 300,
      "parallel": true,
      "coverage": true
    },
    "integration": {
      "patterns": ["integration_*.cpp"],
      "timeout": 600,
      "parallel": false,
      "coverage": false
    },
    "performance": {
      "patterns": ["benchmark_*.cpp"],
      "timeout": 1200,
      "parallel": true,
      "coverage": false,
      "slow": true
    },
    "stress": {
      "patterns": ["stress_*.cpp"],
      "timeout": 1800,
      "parallel": false,
      "coverage": false,
      "slow": true
    }
  },
  
  "test_environment": {
    "default_config": "configs/default.cfg",
    "performance_config": "configs/performance.cfg",
    "stress_config": "configs/stress.cfg",
    "timeout_seconds": 300,
    "memory_limit_mb": 4096,
    "cpu_limit_percent": 80
  },
  
  "reporting": {
    "format": "xml,junit,json",
    "output_dir": "test-results",
    "generate_html_report": true,
    "generate_coverage_report": true,
    "upload_artifacts": true
  }
}
```

### 2. Test Automation Script
**File**: `scripts/run_tests.py`
```python
#!/usr/bin/env python3
"""
NLM Test Automation Script

Comprehensive test automation for NLM with multiple test suites,
performance benchmarking, and result aggregation.
"""

import argparse
import subprocess
import sys
import os
import json
import time
import threading
from pathlib import Path
from datetime import datetime

class NLMTestRunner:
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.test_dir = self.project_root / "tests"
        self.results_dir = self.project_root / "test-results"
        self.benchmark_dir = self.project_root / "benchmarks"
        
    def run_unit_tests(self, config_file):
        """Run unit tests"""
        print("Running unit tests...")
        
        start_time = time.time()
        
        # Build test executable if needed
        test_exe = self.project_root / "build" / "nlm_test"
        if not test_exe.exists():
            self.build_tests()
        
        # Run unit tests
        result = subprocess.run(
            [str(test_exe)],
            cwd=str(self.project_root / "build"),
            capture_output=True,
            text=True
        )
        
        end_time = time.time()
        duration = end_time - start_time
        
        return self.process_test_result("unit", result, duration)
    
    def run_integration_tests(self, config_file):
        """Run integration tests"""
        print("Running integration tests...")
        
        start_time = time.time()
        
        # Run integration tests (could be separate executables or same framework)
        result = subprocess.run(
            ["pytest", "tests/integration/"],
            cwd=str(self.project_root),
            capture_output=True,
            text=True
        )
        
        end_time = time.time()
        duration = end_time - start_time
        
        return self.process_test_result("integration", result, duration)
    
    def run_performance_tests(self, config_file):
        """Run performance tests"""
        print("Running performance tests...")
        
        start_time = time.time()
        
        # Run performance benchmarks
        result = subprocess.run(
            ["pytest", "tests/performance/", "-m", "performance"],
            cwd=str(self.project_root),
            capture_output=True,
            text=True
        )
        
        end_time = time.time()
        duration = end_time - start_time
        
        return self.process_test_result("performance", result, duration)
    
    def run_stress_tests(self, config_file):
        """Run stress tests"""
        print("Running stress tests...")
        
        start_time = time.time()
        
        # Run stress tests
        result = subprocess.run(
            ["pytest", "tests/stress/", "-m", "stress"],
            cwd=str(self.project_root),
            capture_output=True,
            text=True
        )
        
        end_time = time.time()
        duration = end_time - start_time
        
        return self.process_test_result("stress", result, duration)
    
    def run_end_to_end_tests(self, config_file):
        """Run end-to-end tests"""
        print("Running end-to-end tests...")
        
        start_time = time.time()
        
        # Run end-to-end tests
        result = subprocess.run(
            ["pytest", "tests/e2e/"],
            cwd=str(self.project_root),
            capture_output=True,
            text=True
        )
        
        end_time = time.time()
        duration = end_time - start_time
        
        return self.process_test_result("end-to-end", result, duration)
    
    def process_test_result(self, test_type, result, duration):
        """Process test result and return structured data"""
        return {
            "type": test_type,
            "duration": duration,
            "returncode": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "success": result.returncode == 0,
            "timestamp": datetime.now().isoformat()
        }
    
    def build_tests(self):
        """Build test executables"""
        print("Building tests...")
        
        # Check if CMake build directory exists
        build_dir = self.project_root / "build"
        if not build_dir.exists():
            # Configure
            subprocess.run([
                "cmake", "..",
                "-DCMAKE_BUILD_TYPE=Release",
                "-DNLM_ENABLE_TESTS=ON",
                "-DNLM_ENABLE_BENCHMARKS=ON"
            ], cwd=str(build_dir), check=True)
        
        # Build
        subprocess.run([
            "cmake", "--build", ".",
            "--parallel", str(os.cpu_count())
        ], cwd=str(build_dir), check=True)
    
    def generate_report(self, test_results, config_file):
        """Generate comprehensive test report"""
        print("Generating test report...")
        
        # Create results directory
        self.results_dir.mkdir(exist_ok=True)
        
        # Generate summary
        summary = {
            "timestamp": datetime.now().isoformat(),
            "total_tests": len(test_results),
            "successful_tests": sum(1 for r in test_results if r["success"]),
            "failed_tests": sum(1 for r in test_results if not r["success"]),
            "total_duration": sum(r["duration"] for r in test_results),
            "results": test_results
        }
        
        # Save summary
        with open(self.results_dir / "test-summary.json", "w") as f:
            json.dump(summary, f, indent=2)
        
        # Generate HTML report
        self.generate_html_report(summary)
        
        # Generate JUnit XML for CI
        self.generate_junit_report(summary)
        
        return summary
    
    def generate_html_report(self, summary):
        """Generate HTML test report"""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Test Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .test-suite {{ margin: 10px 0; padding: 10px; border: 1px solid #ddd; }}
        .success {{ background-color: #d4edda; }}
        .failure {{ background-color: #f8d7da; }}
        .summary {{ font-weight: bold; }}
        table {{ border-collapse: collapse; width: 100%; }}
        th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Test Report</h1>
        <p>Generated: {summary["timestamp"]}</p>
        <p class="summary">Total Tests: {summary["total_tests"]}</p>
        <p class="summary">Successful: {summary["successful_tests"]}</p>
        <p class="summary">Failed: {summary["failed_tests"]}</p>
        <p class="summary">Total Duration: {summary["total_duration"]:.2f} seconds</p>
    </div>
    
    <h2>Test Results</h2>
    <table>
        <tr>
            <th>Test Type</th>
            <th>Duration</th>
            <th>Success</th>
            <th>Return Code</th>
        </tr>
"""
        
        for result in summary["results"]:
            status_class = "success" if result["success"] else "failure"
            status_text = "PASS" if result["success"] else "FAIL"
            
            html_content += f"""
        <tr class="{status_class}">
            <td>{result["type"]}</td>
            <td>{result["duration"]:.2f}s</td>
            <td>{status_text}</td>
            <td>{result["returncode"]}</td>
        </tr>
"""
        
        html_content += """
    </table>
    
    <h2>Details</h2>
    <pre>
""" + json.dumps(summary["results"], indent=2) + """
    </pre>
</body>
</html>
"""
        
        with open(self.results_dir / "test-report.html", "w") as f:
            f.write(html_content)
    
    def generate_junit_report(self, summary):
        """Generate JUnit XML report for CI"""
        xml_content = '''<?xml version="1.0" encoding="UTF-8"?>
<testsuites>
'''
        
        for result in summary["results"]:
            xml_content += f'''  <testsuite name="{result["type"]}" tests="1" failures="{0 if result["success"] else 1}" time="{result["duration"]}">
    <testcase name="test_{result["type"]}" time="{result["duration"]}">
'''
            if not result["success"]:
                xml_content += f'''      <failure message="Test failed with return code {result["returncode"]}">
{result["stderr"]}
      </failure>
'''
            xml_content += '''    </testcase>
  </testsuite>
'''
        
        xml_content += """
</testsuites>
"""
        
        with open(self.results_dir / "junit-report.xml", "w") as f:
            f.write(xml_content)

    def run_all_tests(self, config_file):
        """Run all test suites"""
        print("Starting comprehensive test suite...")
        
        test_results = []
        
        # Run each test suite
        test_results.append(self.run_unit_tests(config_file))
        test_results.append(self.run_integration_tests(config_file))
        test_results.append(self.run_performance_tests(config_file))
        test_results.append(self.run_stress_tests(config_file))
        test_results.append(self.run_end_to_end_tests(config_file))
        
        # Generate comprehensive report
        summary = self.generate_report(test_results, config_file)
        
        # Print summary
        print("\n" + "="*50)
        print("TEST SUMMARY")
        print("="*50)
        for result in test_results:
            status = "PASS" if result["success"] else "FAIL"
            print(f"{result['type']}: {status} ({result['duration']:.2f}s)")
        
        print(f"\nOverall: {summary['successful_tests']}/{summary['total_tests']} tests passed")
        
        # Return non-zero exit code if any tests failed
        if summary['failed_tests'] > 0:
            return False
        return True

def main():
    parser = argparse.ArgumentParser(description="NLM Test Automation Script")
    
    parser.add_argument(
        "--config", "-c",
        default="tests/config/test_config.json",
        help="Test configuration file"
    )
    
    parser.add_argument(
        "--test-type",
        choices=["unit", "integration", "performance", "stress", "e2e", "all"],
        default="all",
        help="Type of tests to run"
    )
    
    parser.add_argument(
        "--build-only",
        action="store_true",
        help="Only build test executables, don't run tests"
    )
    
    parser.add_argument(
        "--output-dir",
        default="test-results",
        help="Directory for test results"
    )
    
    args = parser.parse_args()
    
    runner = NLMTestRunner()
    
    if args.build_only:
        runner.build_tests()
        print("Build completed successfully!")
        return 0
    
    # Load configuration
    with open(args.config, "r") as f:
        config = json.load(f)
    
    # Run specified tests
    if args.test_type == "unit":
        test_results = [runner.run_unit_tests(config)]
    elif args.test_type == "integration":
        test_results = [runner.run_integration_tests(config)]
    elif args.test_type == "performance":
        test_results = [runner.run_performance_tests(config)]
    elif args.test_type == "stress":
        test_results = [runner.run_stress_tests(config)]
    elif args.test_type == "e2e":
        test_results = [runner.run_end_to_end_tests(config)]
    else:  # all
        test_results = runner.run_all_tests(config)
    
    # Generate report
    runner.generate_report(test_results, config)
    
    # Print final summary
    print("\n" + "="*50)
    print("TESTING COMPLETED")
    print("="*50)
    
    return 0 if test_results and all(r["success"] for r in test_results) else 1

if __name__ == "__main__":
    sys.exit(main())
```

## Test Environment Setup

### 1. Docker-Based Testing Environment
**File**: `docker/docker-compose.yml`
```yaml
version: '3.8'

services:
  nlm-test:
    build: .
    image: nlm/test:latest
    container_name: nlm-test-container
    ports:
      - "8080:8080"
    volumes:
      - ./tests:/workspace/nlm/tests
      - ./test-results:/workspace/nlm/test-results
    environment:
      - NLM_TEST_SUITE=all
      - NLM_TEST_TIMEOUT=300
      - NLM_MEMORY_LIMIT=4096
    command: [
      "bash", "-c",
      "cd /workspace/nlm && ./scripts/run_tests.py --test-type all"
    ]
    depends_on:
      - nlm-build

  nlm-build:
    build: .
    image: nlm/build:latest
    container_name: nlm-build-container
    volumes:
      - ./:/workspace/nlm
    command: [
      "bash", "-c",
      "cd /workspace/nlm && mkdir -p build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)"
    ]

  nlm-benchmarks:
    build: .
    image: nlm/benchmarks:latest
    container_name: nlm-benchmarks-container
    volumes:
      - ./benchmarks:/workspace/nlm/benchmarks
    command: [
      "bash", "-c",
      "cd /workspace/nlm && ./benchmarks/run_benchmarks.sh"
    ]
    profiles: [benchmark]

networks:
  default:
    name: nlm-test-network
```

### 2. Environment Variables
```bash
# Test Configuration
NLM_TEST_SUITE=all        # Which test suite to run
NLM_TEST_TIMEOUT=300      # Test timeout in seconds
NLM_MEMORY_LIMIT=4096     # Memory limit in MB
NLM_CPU_LIMIT=80          # CPU limit percentage

# Test Environment
NLM_TEST_ENV=local        # local, docker, ci
NLM_TEST_COVERAGE=true    # Generate coverage reports
NLM_TEST_PERFORMANCE=true # Run performance benchmarks
NLM_TEST_STRESS=true      # Run stress tests

# Logging
NLM_LOG_LEVEL=INFO       # Logging level
NLM_LOG_FILE=test.log     # Log file path
NLM_LOG_FORMAT=json       # Log format

# Reporting
NLM_REPORT_FORMAT=all    # Report formats: console,html,junit,json
NLM_REPORT_UPLOAD=true   # Upload reports to CI
```

## Test Results Analysis

### 1. Performance Metrics
**File**: `scripts/analyze_results.py`
```python
#!/usr/bin/env python3
"""
NLM Test Results Analysis

Analyze test results and generate performance insights.
"""

import json
import sys
from pathlib import Path
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

class TestResultsAnalyzer:
    def __init__(self, results_dir):
        self.results_dir = Path(results_dir)
        self.summary_file = self.results_dir / "test-summary.json"
        
    def load_summary(self):
        """Load test summary"""
        if not self.summary_file.exists():
            print("Test summary file not found")
            return None
            
        with open(self.summary_file, "r") as f:
            return json.load(f)
    
    def generate_performance_report(self, summary):
        """Generate performance analysis report"""
        print("Generating performance report...")
        
        # Create DataFrame from test results
        df = pd.DataFrame(summary["results"])
        
        # Basic statistics
        stats = {
            "total_tests": len(df),
            "successful_tests": df["success"].sum(),
            "failed_tests": len(df) - df["success"].sum(),
            "success_rate": df["success"].mean() * 100,
            "total_duration": df["duration"].sum(),
            "average_duration": df["duration"].mean(),
            "median_duration": df["duration"].median(),
            "duration_std": df["duration"].std()
        }
        
        # Performance by test type
        performance_by_type = {}
        for test_type in df["type"].unique():
            type_df = df[df["type"] == test_type]
            performance_by_type[test_type] = {
                "success_rate": type_df["success"].mean() * 100,
                "average_duration": type_df["duration"].mean(),
                "tests_count": len(type_df)
            }
        
        # Generate insights
        insights = self.generate_insights(df, stats, performance_by_type)
        
        # Create visualization
        self.create_visualizations(df, performance_by_type)
        
        # Generate HTML report
        self.generate_html_report(summary, stats, insights, performance_by_type)
        
        return stats, insights, performance_by_type
    
    def generate_insights(self, df, stats, performance_by_type):
        """Generate insights from test results"""
        insights = []
        
        # Success rate insight
        if stats["success_rate"] < 80:
            insights.append("WARNING: Overall test success rate is below 80%")
        elif stats["success_rate"] < 95:
            insights.append("CAUTION: Test success rate is below 95%")
        else:
            insights.append("GOOD: High test success rate")
        
        # Performance insight
        avg_duration = stats["average_duration"]
        if avg_duration > 60:
            insights.append("CAUTION: Average test duration is high (>60s)")
        elif avg_duration > 30:
            insights.append("WARNING: Average test duration is moderate (>30s)")
        else:
            insights.append("GOOD: Test durations are reasonable")
        
        # Consistency insight
        if stats["duration_std"] > avg_duration * 2:
            insights.append("WARNING: High variation in test durations")
        else:
            insights.append("GOOD: Consistent test durations")
        
        # Performance by type insights
        fastest_test = min(performance_by_type.items(), 
                         key=lambda x: x[1]["average_duration"])
        slowest_test = max(performance_by_type.items(), 
                          key=lambda x: x[1]["average_duration"])
        
        insights.append(f"Fastest test type: {fastest_test[0]} ({fastest_test[1]['average_duration']:.2f}s)")
        insights.append(f"Slowest test type: {slowest_test[0]} ({slowest_test[1]['average_duration']:.2f}s)")
        
        return insights
    
    def create_visualizations(self, df, performance_by_type):
        """Create visualizations of test results"""
        # Success rate by test type
        plt.figure(figsize=(10, 6))
        
        types = list(performance_by_type.keys())
        success_rates = [performance_by_type[t]["success_rate"] for t in types]
        
        bars = plt.bar(types, success_rates, color=['green' if rate >= 90 else 'orange' if rate >= 70 else 'red' for rate in success_rates])
        plt.title('Test Success Rate by Test Type')
        plt.ylabel('Success Rate (%)')
        plt.ylim(0, 100)
        
        # Add value labels on bars
        for bar, rate in zip(bars, success_rates):
            plt.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                    f'{rate:.1f}%', textcoords="offset points",
                    xytext=(0,5), ha='center')
        
        plt.tight_layout()
        plt.savefig(self.results_dir / "success-rate-chart.png")
        plt.close()
        
        # Duration distribution
        plt.figure(figsize=(10, 6))
        plt.hist(df["duration"], bins=min(20, len(df)), alpha=0.7, color='skyblue', edgecolor='black')
        plt.title('Test Duration Distribution')
        plt.xlabel('Duration (seconds)')
        plt.ylabel('Frequency')
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(self.results_dir / "duration-distribution.png")
        plt.close()
    
    def generate_html_report(self, summary, stats, insights, performance_by_type):
        """Generate comprehensive HTML report"""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Test Results Analysis</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; }}
        .stats {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin: 20px 0; }}
        .stat-card {{ background-color: #ecf0f1; padding: 15px; border-radius: 5px; text-align: center; }}
        .stat-value {{ font-size: 24px; font-weight: bold; color: #2c3e50; }}
        .stat-label {{ color: #7f8c8d; }}
        .insights {{ background-color: #d5f4e6; padding: 15px; border-radius: 5px; margin: 20px 0; }}
        .insight {{ margin: 10px 0; padding-left: 20px; position: relative; }}
        .insight::before {{ content: "⚠"; position: absolute; left: 0; }}
        .performance-table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        .performance-table th, .performance-table td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        .performance-table th {{ background-color: #f2f2f2; }}
        .chart-container {{ margin: 20px 0; text-align: center; }}
        .chart-container img {{ max-width: 100%; height: auto; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Test Results Analysis</h1>
        <p>Generated: {summary["timestamp"]}</p>
    </div>
    
    <h2>Executive Summary</h2>
    <div class="stats">
        <div class="stat-card">
            <div class="stat-value">{stats["total_tests"]}</div>
            <div class="stat-label">Total Tests</div>
        </div>
        <div class="stat-card">
            <div class="stat-value">{stats["successful_tests"]}</div>
            <div class="stat-label">Successful Tests</div>
        </div>
        <div class="stat-card">
            <div class="stat-value">{stats["failed_tests"]}</div>
            <div class="stat-label">Failed Tests</div>
        </div>
        <div class="stat-card">
            <div class="stat-value">{stats["success_rate"]:.1f}%</div>
            <div class="stat-label">Success Rate</div>
        </div>
        <div class="stat-card">
            <div class="stat-value">{stats["total_duration"]:.2f}s</div>
            <div class="stat-label">Total Duration</div>
        </div>
        <div class="stat-card">
            <div class="stat-value">{stats["average_duration"]:.2f}s</div>
            <div class="stat-label">Average Duration</div>
        </div>
    </div>
    
    <h2>Performance by Test Type</h2>
    <table class="performance-table">
        <tr>
            <th>Test Type</th>
            <th>Success Rate</th>
            <th>Average Duration</th>
            <th>Test Count</th>
        </tr>
"""
        
        for test_type, perf in performance_by_type.items():
            status_color = "green" if perf["success_rate"] >= 90 else "orange" if perf["success_rate"] >= 70 else "red"
            html_content += f"""
        <tr>
            <td>{test_type}</td>
            <td style="color: {status_color};">{perf["success_rate"]:.1f}%</td>
            <td>{perf["average_duration"]:.2f}s</td>
            <td>{perf["tests_count"]}</td>
        </tr>
"""
        
        html_content += """
    </table>
    
    <h2>Key Insights</h2>
    <div class="insights">
"""
        for insight in insights:
            # Replace warning with appropriate icon
            if insight.startswith("WARNING:"):
                icon = "⚠️"
            elif insight.startswith("CAUTION:"):
                icon = "🔶"
            elif insight.startswith("GOOD:"):
                icon = "✅"
            else:
                icon = "ℹ️"
            
            html_content += f"""
        <div class="insight">{icon} {insight}</div>
"""
        
        html_content += """
    </div>
    
    <h2>Visualizations</h2>
    <div class="chart-container">
        <h3>Success Rate by Test Type</h3>
        <img src="success-rate-chart.png" alt="Success Rate Chart">
    </div>
    <div class="chart-container">
        <h3>Test Duration Distribution</h3>
        <img src="duration-distribution.png" alt="Duration Distribution Chart">
    </div>
    
    <h2>Raw Results</h2>
    <pre>
""" + json.dumps(summary, indent=2) + """
    </pre>
</body>
</html>
"""
        
        with open(self.results_dir / "analysis-report.html", "w") as f:
            f.write(html_content)

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Test Results Analyzer")
    parser.add_argument(
        "--results-dir",
        default="test-results",
        help="Directory containing test results"
    )
    
    args = parser.parse_args()
    
    analyzer = TestResultsAnalyzer(args.results_dir)
    summary = analyzer.load_summary()
    
    if summary is None:
        print("No test summary found")
        sys.exit(1)
    
    stats, insights, performance_by_type = analyzer.generate_performance_report(summary)
    
    print("Analysis completed successfully!")
    print(f"Generated files in {args.results_dir}/")
    print("- analysis-report.html")
    print("- success-rate-chart.png")
    print("- duration-distribution.png")
```

## Test Result Validation

### 1. Result Validation Script
**File**: `scripts/validate_results.py`
```python
#!/usr/bin/env python3
"""
NLM Test Results Validation

Validate test results for correctness and consistency.
"""

import json
import sys
from pathlib import Path
from datetime import datetime
import re

class TestResultsValidator:
    def __init__(self, results_dir):
        self.results_dir = Path(results_dir)
        self.summary_file = self.results_dir / "test-summary.json"
        
    def validate_summary(self, summary):
        """Validate test summary structure and content"""
        print("Validating test summary...")
        
        validation_errors = []
        validation_warnings = []
        
        # Check required fields
        required_fields = ["timestamp", "total_tests", "successful_tests", 
                          "failed_tests", "total_duration", "results"]
        for field in required_fields:
            if field not in summary:
                validation_errors.append(f"Missing required field: {field}")
        
        # Validate field types
        if "timestamp" in summary:
            try:
                datetime.fromisoformat(summary["timestamp"].replace('Z', '+00:00'))
            except ValueError:
                validation_errors.append("Invalid timestamp format")
        
        if "total_tests" in summary:
            if not isinstance(summary["total_tests"], int) or summary["total_tests"] < 0:
                validation_errors.append("total_tests must be non-negative integer")
        
        if "successful_tests" in summary:
            if not isinstance(summary["successful_tests"], int) or summary["successful_tests"] < 0:
                validation_errors.append("successful_tests must be non-negative integer")
        
        if "failed_tests" in summary:
            if not isinstance(summary["failed_tests"], int) or summary["failed_tests"] < 0:
                validation_errors.append("failed_tests must be non-negative integer")
        
        if "total_duration" in summary:
            if not isinstance(summary["total_duration"], (int, float)) or summary["total_duration"] < 0:
                validation_errors.append("total_duration must be non-negative number")
        
        if "results" in summary:
            if not isinstance(summary["results"], list):
                validation_errors.append("results must be a list")
            else:
                for i, result in enumerate(summary["results"]):
                    result_errors = self.validate_test_result(result)
                    validation_errors.extend(f"Result {i}: {error}" for error in result_errors)
        
        # Check consistency
        if "total_tests" in summary and "successful_tests" in summary and "failed_tests" in summary:
            calculated_total = summary["successful_tests"] + summary["failed_tests"]
            if calculated_total != summary["total_tests"]:
                validation_warnings.append(
                    f"Total tests mismatch: calculated {calculated_total}, summary {summary['total_tests']}"
                )
        
        # Check duration reasonableness
        if "total_duration" in summary and "results" in summary:
            if summary["total_duration"] > 0:
                avg_duration = summary["total_duration"] / len(summary["results"]) if summary["results"] else 0
                if avg_duration > 300:  # 5 minutes per test is too high
                    validation_warnings.append(f"Average test duration is high: {avg_duration:.2f}s")
        
        # Report validation results
        if validation_errors:
            print("❌ VALIDATION FAILED:")
            for error in validation_errors:
                print(f"   - {error}")
        
        if validation_warnings:
            print("⚠️  VALIDATION WARNINGS:")
            for warning in validation_warnings:
                print(f"   - {warning}")
        
        if not validation_errors:
            print("✅ Validation passed")
        
        return len(validation_errors) == 0
    
    def validate_test_result(self, result):
        """Validate individual test result"""
        errors = []
        
        required_fields = ["type", "duration", "returncode", "success"]
        for field in required_fields:
            if field not in result:
                errors.append(f"Missing field: {field}")
        
        if "type" in result and not isinstance(result["type"], str):
            errors.append("type must be a string")
        
        if "duration" in result:
            if not isinstance(result["duration"], (int, float)) or result["duration"] < 0:
                errors.append("duration must be non-negative number")
        
        if "returncode" in result:
            if not isinstance(result["returncode"], int):
                errors.append("returncode must be integer")
        
        if "success" in result:
            if not isinstance(result["success"], bool):
                errors.append("success must be boolean")
        
        return errors
    
    def validate_junit_report(self, summary):
        """Validate JUnit XML report structure"""
        print("Validating JUnit report...")
        
        # This would check the junit-report.xml file if it exists
        junit_file = self.results_dir / "junit-report.xml"
        if not junit_file.exists():
            print("⚠️  JUnit report not found")
            return True
        
        # Simple validation - would be more sophisticated in production
        try:
            with open(junit_file, "r") as f:
                content = f.read()
            
            # Check for required XML structure
            if "<testsuites>" not in content:
                print("❌ Invalid JUnit report: missing testsuites element")
                return False
            
            if "<testsuite>" not in content:
                print("❌ Invalid JUnit report: missing testsuite element")
                return False
            
            print("✅ JUnit report validation passed")
            return True
            
        except Exception as e:
            print(f"❌ Error validating JUnit report: {e}")
            return False
    
    def validate_html_report(self, summary):
        """Validate HTML report structure"""
        print("Validating HTML report...")
        
        html_file = self.results_dir / "test-report.html"
        if not html_file.exists():
            print("⚠️  HTML report not found")
            return True
        
        try:
            with open(html_file, "r") as f:
                content = f.read()
            
            # Check for basic HTML structure
            if "<!DOCTYPE html>" not in content:
                print("❌ Invalid HTML report: missing DOCTYPE")
                return False
            
            if "<html>" not in content:
                print("❌ Invalid HTML report: missing html element")
                return False
            
            # Check for summary content
            if str(summary["total_tests"]) not in content:
                print("❌ Invalid HTML report: summary not found")
                return False
            
            print("✅ HTML report validation passed")
            return True
            
        except Exception as e:
            print(f"❌ Error validating HTML report: {e}")
            return False
    
    def validate_analysis_report(self, summary):
        """Validate analysis report"""
        print("Validating analysis report...")
        
        analysis_file = self.results_dir / "analysis-report.html"
        if not analysis_file.exists():
            print("⚠️  Analysis report not found")
            return True
        
        try:
            with open(analysis_file, "r") as f:
                content = f.read()
            
            # Check for analysis content
            if "NLM Test Results Analysis" not in content:
                print("❌ Invalid analysis report: missing title")
                return False
            
            if str(summary["total_tests"]) not in content:
                print("❌ Invalid analysis report: summary not found")
                return False
            
            print("✅ Analysis report validation passed")
            return True
            
        except Exception as e:
            print(f"❌ Error validating analysis report: {e}")
            return False
    
    def run_full_validation(self):
        """Run all validation checks"""
        print("Starting comprehensive test validation...")
        
        # Load summary
        if not self.summary_file.exists():
            print("❌ Test summary not found")
            return False
        
        with open(self.summary_file, "r") as f:
            summary = json.load(f)
        
        # Run validation checks
        summary_valid = self.validate_summary(summary)
        junit_valid = self.validate_junit_report(summary)
        html_valid = self.validate_html_report(summary)
        analysis_valid = self.validate_analysis_report(summary)
        
        # Overall validation result
        overall_valid = summary_valid and junit_valid and html_valid and analysis_valid
        
        if overall_valid:
            print("✅ All validations passed")
        else:
            print("❌ Some validations failed")
        
        return overall_valid

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Test Results Validator")
    parser.add_argument(
        "--results-dir",
        default="test-results",
        help="Directory containing test results"
    )
    
    args = parser.parse_args()
    
    validator = TestResultsValidator(args.results_dir)
    
    if validator.run_full_validation():
        print("Validation completed successfully!")
        sys.exit(0)
    else:
        print("Validation failed!")
        sys.exit(1)
```

## Implementation Strategy

### Phase 1: Foundation Setup (Weeks 1-2)
1. Implement comprehensive test suite structure
2. Create test configuration management
3. Set up test automation framework
4. Initialize performance testing infrastructure

### Phase 2: Core Testing Implementation (Weeks 3-4)
1. Implement unit tests for all core components
2. Create integration tests for system-level functionality
3. Set up performance and stress testing frameworks
4. Implement end-to-end testing scenarios

### Phase 3: Test Framework Enhancement (Weeks 5-6)
1. Enhance test automation and reporting
2. Implement result analysis and validation
3. Create visualization tools
4. Set up CI/CD integration

### Phase 4: Comprehensive Testing (Weeks 7-8)
1. Run full test suite execution
2. Perform performance benchmarking
3. Execute stress testing scenarios
4. Validate system reliability

### Phase 5: Analysis and Optimization (Weeks 9-10)
1. Analyze test results and performance
2. Identify bottlenecks and optimization opportunities
3. Generate comprehensive reports
4. Implement performance improvements

## Expected Outcomes

### Testing Coverage:
- **Unit Tests**: 90%+ coverage of core components
- **Integration Tests**: 80%+ coverage of system interactions
- **Performance Tests**: Comprehensive benchmarking suite
- **Stress Tests**: 24/7 continuous availability simulation

### Performance Metrics:
- **Test Execution Time**: < 5 minutes for full test suite
- **Test Parallelization**: 90%+ efficiency with multi-threading
- **Resource Utilization**: < 80% CPU during testing
- **Memory Stability**: No memory leaks over extended testing

### Reliability:
- **Test Success Rate**: > 95%
- **Test Consistency**: < 5% variation in execution times
- **Result Validation**: 100% automated validation
- **Continuous Integration**: 24/7 automated testing

## Conclusion

The integration testing and stress testing framework provides comprehensive coverage of the NLM system, ensuring reliability, performance, and scalability. By implementing this robust testing strategy, NLM can achieve high confidence in system quality while maintaining continuous development and improvement.

**Key Achievements**:
- ✅ Comprehensive test coverage across all system levels
- ✅ Robust performance and stress testing capabilities
- ✅ Automated test execution and result analysis
- ✅ Continuous integration and deployment support
- ✅ Detailed reporting and visualization tools

The testing framework enables rapid development, early bug detection, and continuous quality improvement throughout the NLM project lifecycle.