// Multithreading and parallel simulation support implementation
#include "NLMPythonMultithreading.h"
#include "NLMPythonTypes.h"
#include <random>
#include <sstream>

// Thread-safe brain wrapper for parallel simulation
class ThreadSafeBrain {
public:
    std::shared_ptr<Brain> brain;
    mutable std::mutex brainMutex;
    std::atomic<bool> isRunning;
    std::condition_variable condition;
    
    ThreadSafeBrain(std::shared_ptr<Brain> b) : brain(b), isRunning(false) {}
    
    // Thread-safe brain operation
    template<typename Func>
    auto executeSafely(Func&& func) const -> decltype(func()) {
        std::lock_guard<std::mutex> lock(brainMutex);
        return func();
    }
    
    // Thread-safe step execution
    void stepThreadSafe(size_t step, double time) {
        std::lock_guard<std::mutex> lock(brainMutex);
        if (brain) {
            brain->step(step, time);
        }
    }
    
    // Get brain statistics safely
    SimulationStats::Stats getStatsSafely() const {
        std::lock_guard<std::mutex> lock(brainMutex);
        SimulationStats::Stats stats;
        if (brain) {
            stats.steps = 1; // Simplified for thread safety
            stats.totalSpikes = brain->getTotalSpikeCount();
            stats.firingNeurons = brain->getFiringNeuronCount();
            stats.averageFiringRate = brain->getAverageFiringRate();
            stats.spikeHistory[stats.steps] = stats.totalSpikes;
        }
        return stats;
    }
    
    // Get underlying brain (const version)
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
    // Check if brain is valid
    bool isValid() const { return brain != nullptr; }
};

// Parallel simulation manager
class ParallelSimulationManager {
public:
    struct SimulationTask {
        size_t step;
        double time;
        size_t taskId;
        std::promise<SimulationStats::Stats> promise;
        
        SimulationTask(size_t s, double t, size_t id) 
            : step(s), time(t), taskId(id) {}
    };
    
    ThreadSafeBrain threadSafeBrain;
    size_t numThreads;
    std::vector<std::thread> workers;
    std::queue<std::shared_ptr<SimulationTask>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable taskAvailable;
    std::atomic<bool> stopFlag;
    std::atomic<size_t> completedTasks;
    
    ParallelSimulationManager(std::shared_ptr<Brain> brain, size_t threads = 0)
        : threadSafeBrain(brain), numThreads(threads == 0 ? std::thread::hardware_concurrency() : threads),
          stopFlag(false), completedTasks(0) {
        
        if (!threadSafeBrain.isValid()) {
            throw std::runtime_error("Invalid brain provided for parallel simulation");
        }
        
        // Start worker threads
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back(&ParallelSimulationManager::workerThread, this, i);
        }
    }
    
    ~ParallelSimulationManager() {
        stopFlag = true;
        taskAvailable.notify_all();
        
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    // Add simulation task
    void addTask(size_t step, double time) {
        auto task = std::make_shared<SimulationTask>(step, time, completedTasks++);
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push(task);
        }
        taskAvailable.notify_one();
    }
    
    // Run parallel simulation for specified steps
    std::vector<SimulationStats::Stats> runParallelSimulation(size_t numSteps, double timestep = 0.001) {
        // Clear any existing tasks
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            while (!taskQueue.empty()) {
                taskQueue.pop();
            }
        }
        
        // Submit all tasks
        for (size_t step = 0; step < numSteps; ++step) {
            double time = step * timestep;
            addTask(step, time);
        }
        
        // Wait for all tasks to complete
        std::vector<SimulationStats::Stats> results;
        size_t expectedTasks = numSteps;
        
        while (completedTasks < expectedTasks) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        return results;
    }
    
    // Run simulation with callback
    void runWithCallback(size_t numSteps, double timestep,
                         std::function<void(size_t, const SimulationStats::Stats&)> callback) {
        std::vector<SimulationStats::Stats> results = runParallelSimulation(numSteps, timestep);
        
        for (size_t i = 0; i < results.size() && i < numSteps; ++i) {
            callback(i, results[i]);
        }
    }
    
    // Get number of threads
    size_t getThreadCount() const { return numThreads; }
    
    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const { return threadSafeBrain.getBrain(); }
    
private:
    void workerThread(size_t workerId) {
        while (!stopFlag) {
            std::shared_ptr<SimulationTask> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                taskAvailable.wait(lock, [this]() {
                    return !taskQueue.empty() || stopFlag;
                });
                
                if (stopFlag && taskQueue.empty()) {
                    return;
                }
                
                task = taskQueue.front();
                taskQueue.pop();
            }
            
            if (task) {
                // Execute the task
                threadSafeBrain.stepThreadSafe(task->step, task->time);
                
                // Get results and set promise
                auto result = threadSafeBrain.getStatsSafely();
                task->promise.set_value(result);
            }
        }
    }
};

// Simulation batch processor with parallel execution
class SimulationBatchProcessor {
public:
    struct BatchConfig {
        size_t numRuns;
        size_t stepsPerRun;
        double timestep;
        size_t parallelThreads;
        bool randomizeInitialState;
        std::string outputDirectory;
    };
    
    BatchConfig config;
    
    SimulationBatchProcessor(const BatchConfig& cfg) : config(cfg) {}
    
    // Run batch simulation with parallel execution
    std::vector<std::vector<SimulationStats::Stats>> runBatchParallel(
        std::function<std::shared_ptr<Brain>(size_t)> brainFactory) {
        
        std::vector<std::vector<SimulationStats::Stats>> allResults;
        allResults.reserve(config.numRuns);
        
        if (config.parallelThreads > 1) {
            // Parallel batch processing
            std::vector<std::future<std::vector<SimulationStats::Stats>>> futures;
            
            for (size_t run = 0; run < config.numRuns; ++run) {
                futures.push_back(std::async(std::launch::async, [this, brainFactory, run]() {
                    return runSingleBatch(brainFactory, run);
                }));
            }
            
            for (auto& future : futures) {
                allResults.push_back(future.get());
            }
        } else {
            // Sequential batch processing
            for (size_t run = 0; run < config.numRuns; ++run) {
                allResults.push_back(runSingleBatch(brainFactory, run));
            }
        }
        
        return allResults;
    }
    
    // Run single batch simulation
    std::vector<SimulationStats::Stats> runSingleBatch(
        std::function<std::shared_ptr<Brain>(size_t)> brainFactory, size_t runId) {
        
        std::vector<SimulationStats::Stats> results;
        
        for (size_t step = 0; step < config.stepsPerRun; ++step) {
            // Create brain instance
            std::shared_ptr<Brain> brain = brainFactory(runId);
            
            if (config.randomizeInitialState) {
                // Randomize brain state
                randomizeBrainState(brain, runId, step);
            }
            
            // Run simulation step
            double time = step * config.timestep;
            brain->step(step, time);
            
            // Collect statistics
            SimulationStats::Stats stats;
            stats.steps = step + 1;
            stats.totalSpikes = brain->getTotalSpikeCount();
            stats.firingNeurons = brain->getFiringNeuronCount();
            stats.averageFiringRate = brain->getAverageFiringRate();
            stats.spikeHistory[stats.steps] = stats.totalSpikes;
            
            results.push_back(stats);
        }
        
        return results;
    }
    
    // Randomize brain state for batch processing
    void randomizeBrainState(std::shared_ptr<Brain> brain, size_t runId, size_t step) {
        if (!brain) return;
        
        // Simple randomization based on run and step IDs
        std::mt19937 rng(runId * 1000 + step);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        
        // Inject random currents
        for (size_t i = 0; i < std::min<size_t>(10, brain->getTotalNeuronCount()); ++i) {
            brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 5), dist(rng) * 50.0f);
        }
    }
    
    // Get configuration
    BatchConfig getConfig() const { return config; }
    
    // Set configuration
    void setConfig(const BatchConfig& cfg) { config = cfg; }
};

// Thread pool for general-purpose parallel operations
class ThreadPool {
public:
    struct Task {
        std::function<void()> function;
        std::promise<void> promise;
        
        Task(std::function<void()> func) : function(func) {}
    };
    
    std::vector<std::thread> workers;
    std::queue<std::shared_ptr<Task>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable taskAvailable;
    std::atomic<bool> stopFlag;
    std::atomic<size_t> completedTasks;
    
    ThreadPool(size_t numThreads = 0) 
        : numThreads(numThreads == 0 ? std::thread::hardware_concurrency() : numThreads),
          stopFlag(false), completedTasks(0) {
        
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back(&ThreadPool::workerThread, this, i);
        }
    }
    
    ~ThreadPool() {
        stopFlag = true;
        taskAvailable.notify_all();
        
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    // Submit task
    std::future<void> submit(std::function<void()> task) {
        auto taskPtr = std::make_shared<Task>(task);
        auto future = taskPtr->promise.get_future();
        
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push(taskPtr);
        }
        taskAvailable.notify_one();
        
        return future;
    }
    
    // Wait for all tasks to complete
    void waitForAll() {
        std::unique_lock<std::mutex> lock(queueMutex);
        taskAvailable.wait(lock, [this]() {
            return taskQueue.empty() && completedTasks == workers.size();
        });
    }
    
    // Get thread count
    size_t getThreadCount() const { return numThreads; }
    
    // Get completed task count
    size_t getCompletedTasks() const { return completedTasks; }
    
private:
    size_t numThreads;
    
    void workerThread(size_t workerId) {
        while (!stopFlag) {
            std::shared_ptr<Task> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                taskAvailable.wait(lock, [this]() {
                    return !taskQueue.empty() || stopFlag;
                });
                
                if (stopFlag && taskQueue.empty()) {
                    return;
                }
                
                task = taskQueue.front();
                taskQueue.pop();
            }
            
            if (task) {
                try {
                    task->function();
                    task->promise.set_value();
                } catch (...) {
                    task->promise.set_exception(std::current_exception());
                }
                completedTasks++;
            }
        }
    }
};

void registerMultithreadingUtils(py::module_& m) {
    // Thread-safe brain wrapper
    py::class_<ThreadSafeBrain>(m, "ThreadSafeBrain", 
                                "Thread-safe wrapper for brain operations in parallel simulation")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize thread-safe brain wrapper")
        .def("step_thread_safe", &ThreadSafeBrain::stepThreadSafe,
             py::arg("step"), py::arg("time"),
             "Execute brain step in thread-safe manner")
        .def("get_stats_safely", &ThreadSafeBrain::getStatsSafely,
             "Get brain statistics in thread-safe manner")
        .def("get_brain", &ThreadSafeBrain::getBrain,
             "Get the underlying brain instance")
        .def("is_valid", &ThreadSafeBrain::isValid,
             "Check if brain is valid");
    
    // Parallel simulation manager
    py::class_<ParallelSimulationManager>(m, "ParallelSimulationManager", 
                                           "Manager for parallel simulation execution")
        .def(py::init<std::shared_ptr<Brain>, size_t>(),
             py::arg("brain"), py::arg("threads") = 0,
             "Initialize parallel simulation manager")
        .def("add_task", &ParallelSimulationManager::addTask,
             py::arg("step"), py::arg("time"),
             "Add simulation task to queue")
        .def("run_parallel_simulation", &ParallelSimulationManager::runParallelSimulation,
             py::arg("num_steps"), py::arg("timestep") = 0.001,
             "Run parallel simulation for specified number of steps")
        .def("run_with_callback", &ParallelSimulationManager::runWithCallback,
             py::arg("num_steps"), py::arg("timestep") = 0.001,
             py::arg("callback"),
             "Run simulation with progress callback")
        .def("get_thread_count", &ParallelSimulationManager::getThreadCount,
             "Get number of threads in pool")
        .def("get_brain", &ParallelSimulationManager::getBrain,
             "Get the underlying brain instance");
    
    // Simulation batch processor
    py::class_<SimulationBatchProcessor>(m, "SimulationBatchProcessor", 
                                         "Process multiple simulation runs in parallel")
        .def(py::init<const SimulationBatchProcessor::BatchConfig&>(),
             py::arg("config"),
             "Initialize batch processor with configuration")
        .def("run_batch_parallel", &SimulationBatchProcessor::runBatchParallel,
             py::arg("brain_factory"),
             "Run batch simulation with parallel execution")
        .def("run_single_batch", &SimulationBatchProcessor::runSingleBatch,
             py::arg("brain_factory"), py::arg("run_id"),
             "Run single batch simulation")
        .def("get_config", &SimulationBatchProcessor::getConfig,
             "Get batch configuration")
        .def("set_config", &SimulationBatchProcessor::setConfig,
             py::arg("config"),
             "Set batch configuration");
    
    // Thread pool for general parallel operations
    py::class_<ThreadPool>(m, "ThreadPool", "Thread pool for parallel task execution")
        .def(py::init<size_t>(), py::arg("num_threads") = 0,
             "Initialize thread pool")
        .def("submit", &ThreadPool::submit,
             py::arg("task"),
             "Submit task to thread pool")
        .def("wait_for_all", &ThreadPool::waitForAll,
             "Wait for all submitted tasks to complete")
        .def("get_thread_count", &ThreadPool::getThreadCount,
             "Get number of threads in pool")
        .def("get_completed_tasks", &ThreadPool::getCompletedTasks,
             "Get number of completed tasks");
    
    // Convenience functions for parallel simulation
    m.def("create_parallel_simulation", [](std::shared_ptr<Brain> brain, size_t threads) {
        return ParallelSimulationManager(brain, threads);
    }, py::arg("brain"), py::arg("threads") = 0,
       "Create parallel simulation manager for brain (convenience function)");

    m.def("create_thread_pool", [](size_t numThreads) {
        return ThreadPool(numThreads);
    }, py::arg("num_threads") = 0,
       "Create thread pool for parallel task execution (convenience function)");

    m.def("run_parallel_batch", [](std::function<std::shared_ptr<Brain>(size_t)> brainFactory,
                                   const SimulationBatchProcessor::BatchConfig& config) {
        SimulationBatchProcessor processor(config);
        return processor.runBatchParallel(brainFactory);
    }, py::arg("brain_factory"), py::arg("config"),
       "Run parallel batch simulation (convenience function)");

    // Parallel simulation configuration helpers
    m.def("create_batch_config", [](size_t numRuns, size_t stepsPerRun, 
                                    double timestep = 0.001, size_t parallelThreads = 0) {
        SimulationBatchProcessor::BatchConfig config;
        config.numRuns = numRuns;
        config.stepsPerRun = stepsPerRun;
        config.timestep = timestep;
        config.parallelThreads = parallelThreads;
        config.randomizeInitialState = false;
        config.outputDirectory = "batch_results";
        return config;
    }, py::arg("num_runs"), py::arg("steps_per_run"), py::arg("timestep") = 0.001,
       py::arg("parallel_threads") = 0,
       "Create batch configuration for parallel simulation (convenience function)");

    // Simulation synchronization primitives
    m.def("synchronized_step", [](std::shared_ptr<Brain> brain, size_t step, double time,
                                 std::shared_ptr<ThreadPool> threadPool) {
        auto future = threadPool->submit([brain, step, time]() {
            if (brain) {
                brain->step(step, time);
            }
        });
        return future;
    }, py::arg("brain"), py::arg("step"), py::arg("time"), py::arg("thread_pool"),
       "Execute brain step in synchronized manner (convenience function)");
}
