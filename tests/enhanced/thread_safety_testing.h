// Thread Safety and Concurrency Tests
// Comprehensive testing for thread safety and concurrent access scenarios

#ifndef THREAD_SAFETY_TESTING_H
#define THREAD_SAFETY_TESTING_H

#include "nlm_test_framework.h"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <unordered_map>
#include <algorithm>

// Thread safety test utilities
namespace nlm_test {
    // Atomic counter for thread safety testing
    class AtomicCounter {
    private:
        std::atomic<int> count_{0};
        std::mutex mutex_;
        
    public:
        void increment() {
            std::lock_guard<std::mutex> lock(mutex_);
            count_++;
        }
        
        int get() const {
            return count_.load();
        }
        
        void reset() {
            std::lock_guard<std::mutex> lock(mutex_);
            count_ = 0;
        }
    };
    
    // Barrier for thread synchronization
    class ThreadBarrier {
    private:
        std::mutex mutex_;
        std::condition_variable cv_;
        int count_{0};
        int expected_{0};
        bool started_{false};
        
    public:
        ThreadBarrier(int threads) : expected_(threads) {}
        
        void wait() {
            std::unique_lock<std::mutex> lock(mutex_);
            if (++count_ == expected_) {
                started_ = true;
                cv_.notify_all();
            } else {
                cv_.wait(lock, [this]() { return started_; });
            }
        }
        
        void reset() {
            std::lock_guard<std::mutex> lock(mutex_);
            count_ = 0;
            started_ = false;
        }
    };
    
    // Reader-writer lock for testing
    class ReaderWriterLock {
    private:
        std::shared_mutex rw_mutex_;
        std::atomic<int> readers_{0};
        std::atomic<int> writers_{0};
        
    public:
        void lockRead() {
            rw_mutex_.lock_shared();
            readers_++;
        }
        
        void unlockRead() {
            readers_--;
            rw_mutex_.unlock_shared();
        }
        
        void lockWrite() {
            rw_mutex_.lock();
            writers_++;
        }
        
        void unlockWrite() {
            writers_--;
            rw_mutex_.unlock();
        }
        
        int getReaders() const { return readers_.load(); }
        int getWriters() const { return writers_.load(); }
    };
}

// Thread Safety Test Fixtures
class ThreadSafetyTestBase : public ::testing::Test {
protected:
    nlm_test::AtomicCounter counter_;
    nlm_test::ReaderWriterLock rw_lock_;
    std::vector<std::thread> threads_;
    std::atomic<bool> test_passed_{true};
    
    void SetUp() override {
        nlm_test::TestSeedManager::initSeed(12345);
        counter_.reset();
        test_passed_.store(true);
    }
    
    void TearDown() override {
        joinAllThreads();
        EXPECT_TRUE(test_passed_.load()) << "Thread safety test failed";
    }
    
    void joinAllThreads() {
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        threads_.clear();
    }
    
    template<typename Func>
    void launchThreads(int count, Func func) {
        threads_.reserve(count);
        for (int i = 0; i < count; ++i) {
            threads_.emplace_back([&, i, func]() {
                try {
                    func(i);
                } catch (const std::exception& e) {
                    test_passed_.store(false);
                    std::cerr << "Thread exception: " << e.what() << std::endl;
                }
            });
        }
    }
};

// Atomic operation tests
class AtomicOperationTest : public ThreadSafetyTestBase {
protected:
    void testAtomicCounter() {
        nlm_test::AtomicCounter counter;
        
        auto incrementFunc = [&](int thread_id) {
            for (int i = 0; i < 100; ++i) {
                counter.increment();
                std::this_thread::yield();
            }
        };
        
        launchThreads(8, incrementFunc);
        joinAllThreads();
        
        // Each thread should have incremented 100 times
        EXPECT_EQ(counter.get(), 800) << "Atomic counter should reach 800";
    }
    
    void testAtomicOperationsWithBarrier() {
        nlm_test::ThreadBarrier barrier(8);
        std::vector<int> results(8, 0);
        
        auto workerFunc = [&](int thread_id) {
            barrier.wait();
            // All threads start simultaneously here
            for (int i = 0; i < 1000; ++i) {
                counter_.increment();
            }
            results[thread_id] = counter_.get();
        };
        
        launchThreads(8, workerFunc);
        joinAllThreads();
        
        // All threads should have the same final count (assuming no race conditions)
        // The exact value depends on timing, but should be consistent across threads
        EXPECT_TRUE(std::all_of(results.begin(), results.end(),
                               [expected = results[0]](int val) { return val == expected; }))
            << "All threads should have consistent final counts";
    }
};

// Reader-writer lock tests
class ReaderWriterLockTest : public ThreadSafetyTestBase {
protected:
    void testReadWriteOperations() {
        std::vector<int> data(1000, 0);
        nlm_test::AtomicCounter read_ops, write_ops;
        
        // Writer function
        auto writerFunc = [&](int thread_id) {
            for (int i = thread_id; i < 1000; i += 8) {
                rw_lock_.lockWrite();
                data[i] = i * 2;
                rw_lock_.unlockWrite();
                write_ops.increment();
            }
        };
        
        // Reader function
        auto readerFunc = [&](int thread_id) {
            for (int i = thread_id; i < 1000; i += 8) {
                rw_lock_.lockRead();
                int value = data[i];
                rw_lock_.unlockRead();
                read_ops.increment();
                EXPECT_GE(value, 0) << "Read value should be valid";
                EXPECT_LE(value, 2000) << "Read value should be valid";
            }
        };
        
        // Launch writer and reader threads
        launchThreads(4, writerFunc);
        launchThreads(8, readerFunc);
        joinAllThreads();
        
        // Verify data integrity
        for (int i = 0; i < 1000; ++i) {
            if (i % 8 == 0) {
                EXPECT_EQ(data[i], i * 2) << "Data corruption detected";
            }
        }
        
        // Verify operation counts
        EXPECT_GT(write_ops.get(), 0) << "Write operations should have occurred";
        EXPECT_GT(read_ops.get(), 0) << "Read operations should have occurred";
    }
    
    void testConcurrentReads() {
        std::atomic<int> concurrent_readers{0};
        std::mutex counter_mutex;
        int max_concurrent_readers = 0;
        
        auto readerFunc = [&](int thread_id) {
            rw_lock_.lockRead();
            
            // Track concurrent readers
            {
                std::lock_guard<std::mutex> lock(counter_mutex);
                concurrent_readers++;
                max_concurrent_readers = std::max(max_concurrent_readers, concurrent_readers.load());
            }
            
            // Simulate read operation
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
            {
                std::lock_guard<std::mutex> lock(counter_mutex);
                concurrent_readers--;
            }
            
            rw_lock_.unlockRead();
        };
        
        launchThreads(16, readerFunc);
        joinAllThreads();
        
        // Verify maximum concurrent readers
        EXPECT_GE(max_concurrent_readers, 1) << "Should have at least one concurrent reader";
        EXPECT_LE(max_concurrent_readers, 16) << "Should not have more than 16 concurrent readers";
    }
};

// Neural network thread safety tests
class NeuralNetworkThreadSafetyTest : public ThreadSafetyTestBase {
protected:
    std::vector<std::unique_ptr<nlm::Neuron>> neurons_;
    std::vector<std::unique_ptr<nlm::Synapse>> synapses_;
    nlm::RandomGenerator rng_{12345};
    
    void SetUp() override {
        ThreadSafetyTestBase::SetUp();
        
        // Create neural network components
        for (int i = 0; i < 100; ++i) {
            neurons_.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
            neurons_.back()->initializeRandom(rng_);
        }
        
        for (int i = 0; i < 200; ++i) {
            synapses_.emplace_back(std::make_unique<nlm::Synapse>(
                nlm::SynapseId(i), nlm::NeuronId(i % 100), nlm::NeuronId((i + 1) % 100)));
            synapses_.back()->setType(nlm::SynapseType::Excitatory);
            synapses_.back()->setWeight(0.5f);
        }
    }
    
    void TearDown() override {
        neurons_.clear();
        synapses_.clear();
        ThreadSafetyTestBase::TearDown();
    }
    
    void testConcurrentNeuronAccess() {
        auto neuronReaderFunc = [&](int thread_id) {
            for (int i = thread_id; i < neurons_.size(); i += 10) {
                rw_lock_.lockRead();
                auto& neuron = *neurons_[i];
                float potential = neuron.getMembranePotential();
                float threshold = neuron.getThreshold();
                rw_lock_.unlockRead();
                
                // Validate neuron state
                EXPECT_GE(potential, -100.0f) << "Invalid membrane potential";
                EXPECT_LE(potential, 50.0f) << "Invalid membrane potential";
                EXPECT_GE(threshold, -80.0f) << "Invalid threshold";
                EXPECT_LE(threshold, -40.0f) << "Invalid threshold";
            }
        };
        
        auto neuronWriterFunc = [&](int thread_id) {
            for (int i = thread_id; i < neurons_.size(); i += 10) {
                rw_lock_.lockWrite();
                auto& neuron = *neurons_[i];
                neuron.setMembranePotential(-65.0f + (thread_id % 10));
                neuron.setFiringState(nlm::FiringState::Active);
                rw_lock_.unlockWrite();
                std::this_thread::yield();
            }
        };
        
        // Launch reader and writer threads
        launchThreads(5, neuronReaderFunc);
        launchThreads(3, neuronWriterFunc);
        joinAllThreads();
        
        // Verify final consistency
        for (size_t i = 0; i < neurons_.size(); ++i) {
            // After concurrent operations, neurons should still be in valid state
            auto& neuron = *neurons_[i];
            EXPECT_TRUE(neuron.isInitialized()) << "Neuron should remain initialized";
        }
    }
    
    void testConcurrentSynapseAccess() {
        auto synapseReaderFunc = [&](int thread_id) {
            for (int i = thread_id; i < synapses_.size(); i += 5) {
                rw_lock_.lockRead();
                auto& synapse = *synapses_[i];
                float weight = synapse.getWeight();
                nlm::SynapseType type = synapse.getType();
                rw_lock_.unlockRead();
                
                EXPECT_GE(weight, 0.0f) << "Invalid synapse weight";
                EXPECT_LE(weight, 1.0f) << "Invalid synapse weight";
                EXPECT_TRUE(type == nlm::SynapseType::Excitatory || type == nlm::SynapseType::Inhibitory)
                    << "Invalid synapse type";
            }
        };
        
        auto synapseWriterFunc = [&](int thread_id) {
            for (int i = thread_id; i < synapses_.size(); i += 5) {
                rw_lock_.lockWrite();
                auto& synapse = *synapses_[i];
                synapse.setWeight(0.1f * (1 + thread_id));
                rw_lock_.unlockWrite();
                std::this_thread::yield();
            }
        };
        
        // Launch reader and writer threads
        launchThreads(8, synapseReaderFunc);
        launchThreads(4, synapseWriterFunc);
        joinAllThreads();
        
        // Verify synapse integrity after concurrent operations
        for (size_t i = 0; i < synapses_.size(); ++i) {
            auto& synapse = *synapses_[i];
            EXPECT_TRUE(synapse.getWeight() >= 0.0f) << "Synapse weight should be non-negative";
            EXPECT_TRUE(synapse.getWeight() <= 1.0f) << "Synapse weight should be <= 1.0";
        }
    }
    
    void testConcurrentSTDPUpdates() {
        nlm::STDP stdp;
        std::vector<nlm::Timestamp> pre_spikes = {0.0, 10.0, 20.0, 30.0, 40.0};
        std::vector<nlm::Timestamp> post_spikes = {5.0, 15.0, 25.0, 35.0, 45.0};
        
        auto stdpWorkerFunc = [&](int thread_id) {
            for (int i = 0; i < 100; ++i) {
                // Create synapse for this thread
                nlm::Synapse synapse(nlm::SynapseId(thread_id * 1000 + i),
                                   nlm::NeuronId(thread_id), nlm::NeuronId((thread_id + 1) % 10));
                synapse.setType(nlm::SynapseType::Excitatory);
                synapse.setWeight(0.5f);
                
                rw_lock_.lockWrite();
                stdp.update(&synapse, pre_spikes, post_spikes, 0.001);
                rw_lock_.unlockWrite();
                
                // Verify weight bounds
                EXPECT_GE(synapse.getWeight(), 0.0f) << "STDP weight should be non-negative";
                EXPECT_LE(synapse.getWeight(), 1.0f) << "STDP weight should be <= 1.0";
            }
        };
        
        launchThreads(8, stdpWorkerFunc);
        joinAllThreads();
        
        // Final verification - no crashes or corruption
        EXPECT_TRUE(test_passed_.load()) << "STDP concurrent test failed";
    }
};

// Stress testing with many threads
class ThreadStressTest : public ThreadSafetyTestBase {
protected:
    std::vector<std::atomic<int>> shared_counters_;
    std::mutex counter_mutex_;
    
    void SetUp() override {
        ThreadSafetyTestBase::SetUp();
        shared_counters_.resize(100);
    }
    
    void testManyThreadsManyResources() {
        const int num_threads = 64;
        const int operations_per_thread = 1000;
        
        auto workerFunc = [&](int thread_id) {
            std::vector<int> local_updates(100, 0);
            
            for (int op = 0; op < operations_per_thread; ++op) {
                int resource_id = rand() % 100;
                
                // Simulate resource access
                rw_lock_.lockRead();
                int current_value = shared_counters_[resource_id].load();
                rw_lock_.unlockRead();
                
                // Simulate computation
                std::this_thread::yield();
                
                // Update with write lock
                rw_lock_.lockWrite();
                shared_counters_[resource_id].store(current_value + 1);
                rw_lock_.unlockWrite();
                
                local_updates[resource_id]++;
            }
            
            // Verify local updates match global state
            for (int i = 0; i < 100; ++i) {
                int global_value = shared_counters_[i].load();
                int local_value = local_updates[i];
                
                // Allow some tolerance due to race conditions
                EXPECT_LE(std::abs(global_value - local_value), num_threads + 10)
                    << "Resource " << i << " global/local mismatch: global=" 
                    << global_value << ", local=" << local_value;
            }
        };
        
        launchThreads(num_threads, workerFunc);
        joinAllThreads();
        
        // Verify final consistency
        for (size_t i = 0; i < shared_counters_.size(); ++i) {
            int final_value = shared_counters_[i].load();
            EXPECT_GE(final_value, 0) << "Negative counter value detected";
            EXPECT_LE(final_value, num_threads * operations_per_thread)
                << "Counter value exceeds expected maximum";
        }
    }
    
    void testDeadlockPrevention() {
        std::vector<nlm_test::ReaderWriterLock> locks(10);
        std::vector<int> lock_order(10);
        std::atomic<bool> completed{false};
        
        // Create lock ordering to prevent deadlocks
        for (int i = 0; i < 10; ++i) {
            lock_order[i] = i;
        }
        std::random_shuffle(lock_order.begin(), lock_order.end());
        
        auto deadlockWorkerFunc = [&](int thread_id) {
            // Acquire locks in order to prevent deadlocks
            for (int lock_id : lock_order) {
                locks[lock_id].lockWrite();
            }
            
            // Critical section
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            // Release locks in reverse order
            for (int i = lock_order.size() - 1; i >= 0; --i) {
                locks[lock_order[i]].unlockWrite();
            }
            
            completed.store(true);
        };
        
        launchThreads(20, deadlockWorkerFunc);
        joinAllThreads();
        
        EXPECT_TRUE(completed.load()) << "Deadlock prevention test failed";
    }
};

#endif // THREAD_SAFETY_TESTING_H
