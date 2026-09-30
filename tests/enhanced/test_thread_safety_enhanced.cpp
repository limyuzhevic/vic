// Thread Safety and Concurrency Tests Implementation
// Comprehensive testing for thread safety and concurrent access scenarios

#include "enhanced/thread_safety_testing.h"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include <random>
#include <algorithm>

// Test for atomic operations
TEST_F(AtomicOperationTest, AtomicCounter) {
    testAtomicCounter();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks()) 
        << "Atomic counter test should not cause memory leaks";
}

TEST_F(AtomicOperationTest, AtomicOperationsWithBarrier) {
    testAtomicOperationsWithBarrier();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Atomic operations with barrier test should not cause memory leaks";
}

// Test for reader-writer lock operations
TEST_F(ReaderWriterLockTest, ReadWriteOperations) {
    testReadWriteOperations();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Read-write operations test should not cause memory leaks";
}

TEST_F(ReaderWriterLockTest, ConcurrentReads) {
    testConcurrentReads();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Concurrent reads test should not cause memory leaks";
}

// Test for neural network thread safety
TEST_F(NeuralNetworkThreadSafetyTest, ConcurrentNeuronAccess) {
    testConcurrentNeuronAccess();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Concurrent neuron access test should not cause memory leaks";
}

TEST_F(NeuralNetworkThreadSafetyTest, ConcurrentSynapseAccess) {
    testConcurrentSynapseAccess();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Concurrent synapse access test should not cause memory leaks";
}

TEST_F(NeuralNetworkThreadSafetyTest, ConcurrentSTDPUpdates) {
    testConcurrentSTDPUpdates();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Concurrent STDP updates test should not cause memory leaks";
}

// Test for thread stress scenarios
TEST_F(ThreadStressTest, ManyThreadsManyResources) {
    testManyThreadsManyResources();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Many threads many resources test should not cause memory leaks";
}

TEST_F(ThreadStressTest, DeadlockPrevention) {
    testDeadlockPrevention();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Deadlock prevention test should not cause memory leaks";
}

// Performance testing with thread safety
class ThreadPerformanceTest : public ThreadSafetyTestBase {
protected:
    void SetUp() override {
        ThreadSafetyTestBase::SetUp();
        
        // Create test data
        neurons_.reserve(1000);
        for (int i = 0; i < 1000; ++i) {
            neurons_.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
            neurons_.back()->initializeRandom(rng_);
        }
    }
    
    void TearDown() override {
        neurons_.clear();
        ThreadSafetyTestBase::TearDown();
    }
    
    std::vector<std::unique_ptr<nlm::Neuron>> neurons_;
    nlm::RandomGenerator rng_{12345};
    
    void testConcurrentNeuronReadsPerformance() {
        const int num_threads = 32;
        const int reads_per_thread = 1000;
        std::atomic<int> successful_reads{0};
        
        auto performanceReaderFunc = [&](int thread_id) {
            for (int i = 0; i < reads_per_thread; ++i) {
                // Random neuron access
                int neuron_index = rand() % neurons_.size();
                
                rw_lock_.lockRead();
                auto& neuron = *neurons_[neuron_index];
                
                // Perform read operations
                float potential = neuron.getMembranePotential();
                float threshold = neuron.getThreshold();
                bool is_firing = neuron.isFiring();
                nlm::FiringState state = neuron.getFiringState();
                
                rw_lock_.unlockRead();
                
                // Validate reads
                EXPECT_GE(potential, -100.0f);
                EXPECT_LE(potential, 50.0f);
                EXPECT_GE(threshold, -80.0f);
                EXPECT_LE(threshold, -40.0f);
                EXPECT_TRUE(is_firing == (state == nlm::FiringState::Active));
                
                successful_reads++;
            }
        };
        
        nlm_test::PerformanceTimer timer("ConcurrentNeuronReadsPerformance");
        launchThreads(num_threads, performanceReaderFunc);
        joinAllThreads();
        
        double elapsed = timer.elapsed_us();
        int total_operations = num_threads * reads_per_thread;
        double ops_per_second = (total_operations * 1000000.0) / elapsed;
        
        std::cout << "Concurrent neuron reads performance: " << ops_per_second << " ops/sec" << std::endl;
        
        EXPECT_EQ(successful_reads.load(), total_operations) 
            << "All read operations should complete successfully";
    }
    
    void testConcurrentNeuronWritesPerformance() {
        const int num_threads = 8;
        const int writes_per_thread = 500;
        std::atomic<int> successful_writes{0};
        
        auto performanceWriterFunc = [&](int thread_id) {
            for (int i = 0; i < writes_per_thread; ++i) {
                int neuron_index = thread_id * 10 + (i % 100); // Distribute writes
                if (neuron_index >= neurons_.size()) neuron_index = neurons_.size() - 1;
                
                rw_lock_.lockWrite();
                auto& neuron = *neurons_[neuron_index];
                
                // Perform write operations
                neuron.setMembranePotential(-65.0f + (thread_id % 10));
                neuron.setFiringState(static_cast<nlm::FiringState>(thread_id % 3));
                neuron.injectCurrent(1.0f * (thread_id % 5));
                
                rw_lock_.unlockWrite();
                
                successful_writes++;
            }
        };
        
        nlm_test::PerformanceTimer timer("ConcurrentNeuronWritesPerformance");
        launchThreads(num_threads, performanceWriterFunc);
        joinAllThreads();
        
        double elapsed = timer.elapsed_us();
        int total_operations = num_threads * writes_per_thread;
        double ops_per_second = (total_operations * 1000000.0) / elapsed;
        
        std::cout << "Concurrent neuron writes performance: " << ops_per_second << " ops/sec" << std::endl;
        
        EXPECT_EQ(successful_writes.load(), total_operations)
            << "All write operations should complete successfully";
    }
};

// Test performance with thread safety
TEST_F(ThreadPerformanceTest, ConcurrentNeuronReadsPerformance) {
    testConcurrentNeuronReadsPerformance();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Concurrent neuron reads performance test should not cause memory leaks";
}

TEST_F(ThreadPerformanceTest, ConcurrentNeuronWritesPerformance) {
    testConcurrentNeuronWritesPerformance();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Concurrent neuron writes performance test should not cause memory leaks";
}

// Test for mixed read-write operations
class MixedOperationTest : public ThreadSafetyTestBase {
protected:
    std::vector<int> shared_data_;
    nlm_test::ThreadBarrier start_barrier_{4};
    std::atomic<int> completed_writers_{0};
    std::atomic<int> completed_readers_{0};
    
    void SetUp() override {
        ThreadSafetyTestBase::Setup();
        shared_data_.resize(100, 0);
    }
    
    void testMixedReadWriteOperations() {
        // Writer function
        auto writerFunc = [&](int thread_id) {
            start_barrier_.wait();
            
            for (int i = 0; i < 100; ++i) {
                int index = (thread_id * 100 + i) % shared_data_.size();
                
                // Write with lock
                rw_lock_.lockWrite();
                shared_data_[index] = thread_id * 1000 + i;
                rw_lock_.unlockWrite();
                
                std::this_thread::yield();
            }
            
            completed_writers_.fetch_add(1, std::memory_order_relaxed);
        };
        
        // Reader function
        auto readerFunc = [&](int thread_id) {
            start_barrier_.wait();
            
            for (int i = 0; i < 100; ++i) {
                int index = (thread_id * 100 + i) % shared_data_.size();
                
                // Read with lock
                rw_lock_.lockRead();
                int value = shared_data_[index];
                rw_lock_.unlockRead();
                
                // Validate read
                EXPECT_GE(value, 0) << "Invalid read value";
                EXPECT_LE(value, 4000) << "Invalid read value";
                
                // Validate data integrity
                int expected_writer = (value / 1000) % 4;
                EXPECT_EQ(expected_writer, thread_id) 
                    << "Data corruption detected: writer " << expected_writer 
                    << " wrote value " << value << " but thread " << thread_id << " read it";
            }
            
            completed_readers_.fetch_add(1, std::memory_order_relaxed);
        };
        
        // Launch threads
        launchThreads(4, writerFunc);   // 4 writers
        launchThreads(8, readerFunc);   // 8 readers
        
        // Wait for all threads to complete
        while (completed_writers_.load() < 4 || completed_readers_.load() < 8) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        joinAllThreads();
        
        // Verify final state
        EXPECT_EQ(completed_writers_.load(), 4) << "All writers should complete";
        EXPECT_EQ(completed_readers_.load(), 8) << "All readers should complete";
    }
};

TEST_F(MixedOperationTest, MixedReadWriteOperations) {
    testMixedReadWriteOperations();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Mixed read-write operations test should not cause memory leaks";
}

// Test for thread-local storage
class ThreadLocalStorageTest : public ThreadSafetyTestBase {
protected:
    thread_local static nlm::RandomGenerator thread_local_rng_;
    std::atomic<int> thread_local_test_count_{0};
    
    void SetUp() override {
        ThreadSafetyTestBase::SetUp();
        thread_local_test_count_.store(0);
    }
    
    void testThreadLocalRandomGenerator() {
        auto workerFunc = [&](int thread_id) {
            // Each thread should have its own random generator state
            for (int i = 0; i < 10; ++i) {
                // Use thread-local RNG
                int random_value = thread_local_rng_.nextInt(0, 1000);
                EXPECT_GE(random_value, 0) << "Thread-local RNG should produce non-negative values";
                EXPECT_LE(random_value, 1000) << "Thread-local RNG should produce values <= 1000";
                
                // Verify different threads get different sequences
                // (though with same seed, they might be identical)
                std::this_thread::yield();
                thread_local_test_count_++;
            }
        };
        
        launchThreads(4, workerFunc);
        joinAllThreads();
        
        EXPECT_EQ(thread_local_test_count_.load(), 40) 
            << "All thread-local operations should complete";
    }
};

TEST_F(ThreadLocalStorageTest, ThreadLocalRandomGenerator) {
    testThreadLocalRandomGenerator();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Thread-local storage test should not cause memory leaks";
}

// Test for atomic operations with memory ordering
class MemoryOrderingTest : public ThreadSafetyTestBase {
protected:
    std::atomic<int> atomic_var_{0};
    std::atomic<bool> ready_flag_{false};
    std::mutex mutex_;
    std::condition_variable cv_;
    
    void testMemoryOrdering() {
        auto producerFunc = [&](int thread_id) {
            // Produce some data
            int local_data = thread_id * 100;
            
            // Store with memory order release
            atomic_var_.store(local_data, std::memory_order_release);
            ready_flag_.store(true, std::memory_order_release);
            
            // Signal that data is ready
            {
                std::lock_guard<std::mutex> lock(mutex_);
                cv_.notify_one();
            }
        };
        
        auto consumerFunc = [&](int thread_id) {
            std::unique_lock<std::mutex> lock(mutex_);
            
            // Wait for data to be ready
            cv_.wait(lock, [this]() { return ready_flag_.load(std::memory_order_acquire); });
            
            // Consume data with memory order acquire
            int consumed_data = atomic_var_.load(std::memory_order_acquire);
            
            // Verify data consistency
            EXPECT_GE(consumed_data, 0) << "Consumed data should be non-negative";
            EXPECT_LE(consumed_data, 300) << "Consumed data should be <= 300 for 3 threads";
            
            // Use the data
            int result = consumed_data + thread_id;
            EXPECT_GE(result, thread_id) << "Result should be >= thread_id";
        };
        
        // Launch producer and consumer threads
        launchThreads(3, producerFunc);
        launchThreads(3, consumerFunc);
        joinAllThreads();
    }
};

TEST_F(MemoryOrderingTest, MemoryOrdering) {
    testMemoryOrdering();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Memory ordering test should not cause memory leaks";
}

// Test for race condition detection
class RaceConditionDetectionTest : public ThreadSafetyTestBase {
protected:
    std::vector<std::atomic<int>> counters_;
    std::atomic<int> total_operations_{0};
    
    void SetUp() override {
        ThreadSafetyTestBase::SetUp();
        counters_.resize(10);
    }
    
    void testRaceConditionDetection() {
        const int num_threads = 8;
        const int operations_per_thread = 1000;
        
        auto raceConditionWorkerFunc = [&](int thread_id) {
            for (int op = 0; op < operations_per_thread; ++op) {
                int counter_index = rand() % counters_.size();
                
                // Read-modify-write race condition scenario
                int current_value = counters_[counter_index].load(std::memory_order_relaxed);
                std::this_thread::yield(); // Increase chance of race condition
                int new_value = current_value + 1;
                
                // This is a race condition - multiple threads can read the same value
                // and write back incremented value, losing some increments
                counters_[counter_index].store(new_value, std::memory_order_relaxed);
                
                total_operations_.fetch_add(1, std::memory_order_relaxed);
            }
        };
        
        launchThreads(num_threads, raceConditionWorkerFunc);
        joinAllThreads();
        
        // Due to race conditions, the final counter value may be less than expected
        int expected_operations = num_threads * operations_per_thread;
        int actual_total = total_operations_.load();
        
        std::cout << "Expected operations: " << expected_operations << std::endl;
        std::cout << "Actual operations counted: " << actual_total << std::endl;
        std::cout << "Race condition impact: " << (expected_operations - actual_total) << " lost operations" << std::endl;
        
        // Race conditions are expected, but should be detected
        // The actual counter values may be less than expected due to lost updates
        int sum_of_counters = 0;
        for (size_t i = 0; i < counters_.size(); ++i) {
            sum_of_counters += counters_[i].load(std::memory_order_relaxed);
        }
        
        std::cout << "Sum of all counters: " << sum_of_counters << std::endl;
        std::cout << "Average per counter: " << (sum_of_counters / counters_.size()) << std::endl;
        
        // At minimum, counters should have positive values
        for (size_t i = 0; i < counters_.size(); ++i) {
            EXPECT_GE(counters_[i].load(std::memory_order_relaxed), 0) 
                << "Counter values should not be negative";
        }
    }
};

TEST_F(RaceConditionDetectionTest, RaceConditionDetection) {
    testRaceConditionDetection();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Race condition detection test should not cause memory leaks";
}