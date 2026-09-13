#include "ParallelBatchProcessor.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <cmath>

namespace nlm {

namespace batch_utils {

std::string generateJobId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    return "job_" + std::to_string(dis(gen));
}

bool compareJobsByPriority(const std::shared_ptr<BatchJob>& a, const std::shared_ptr<BatchJob>& b) {
    return static_cast<int>(a->priority) < static_cast<int>(b->priority);
}

std::string jobStatusToString(JobStatus status) {
    switch (status) {
        case JobStatus::Pending: return "Pending";
        case JobStatus::Running: return "Running";
        case JobStatus::Completed: return "Completed";
        case JobStatus::Failed: return "Failed";
        case JobStatus::Cancelled: return "Cancelled";
        default: return "Unknown";
    }
}

std::string batchPriorityToString(BatchPriority priority) {
    switch (priority) {
        case BatchPriority::Low: return "Low";
        case BatchPriority::Normal: return "Normal";
        case BatchPriority::High: return "High";
        case BatchPriority::Critical: return "Critical";
        default: return "Normal";
    }
}

double calculateJobProgress(const BatchJob& job) {
    // Basic progress calculation based on time
    if (job.startTime.time_since_epoch().count() == 0) {
        return 0.0;
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - job.startTime).count();
    
    // Assume each job has an estimated duration based on maxSteps
    // Simple estimation: 1 step = 0.001 seconds for simulation
    double estimatedDuration = static_cast<double>(job.maxSteps) * 0.001;
    
    if (estimatedDuration <= 0) return 0.0;
    
    double progress = (elapsed / estimatedDuration) * 100.0;
    return std::min(progress, 100.0);
}

std::string formatDuration(std::chrono::duration<double> duration) {
    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto hours = totalSeconds / 3600;
    auto minutes = (totalSeconds % 3600) / 60;
    auto seconds = totalSeconds % 60;
    
    std::stringstream ss;
    if (hours > 0) ss << hours << "h ";
    ss << minutes << "m " << seconds << "s";
    return ss.str();
}

std::string formatTimePoint(std::chrono::system_clock::time_point tp) {
    auto time_t_now = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ThreadPool implementation
ThreadPool::ThreadPool(size_t numThreads) : running(true), completedTasks(0) {
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([this]() {
            while (running) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this]() {
                        return !this->running || !this->tasks.empty();
                    });
                    if (!this->running && this->tasks.empty()) {
                        return;
                    }
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
                {
                    std::lock_guard<std::mutex> lock(this->queueMutex);
                    completedTasks++;
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop();
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

template<typename F>
std::future<typename std::result_of<F()>::type> ThreadPool::enqueue(F&& f) {
    using return_type = typename std::result_of<F()>::type;
    
    auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
    std::future<return_type> future = task_ptr->get_future();
    
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (!running) {
            throw std::runtime_error("Cannot enqueue tasks on stopped thread pool");
        }
        tasks.emplace([task_ptr]() {
            (*task_ptr)();
        });
    }
    
    condition.notify_one();
    return future;
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        running = false;
    }
    condition.notify_all();
}

void ThreadPool::waitForCompletion() {
    std::unique_lock<std::mutex> lock(queueMutex);
    condition.wait(lock, [this]() {
        return tasks.empty();
    });
}

} // namespace batch_utils

struct ParallelBatchProcessor::Impl {
    std::shared_ptr<ResearchConfigManager> configManager;
    size_t maxWorkers;
    size_t maxQueueSize;
    bool progressMonitoringEnabled;
    size_t checkpointFrequency;
    
    // Thread pool and job management
    batch_utils::ThreadPool threadPool;
    std::vector<std::shared_ptr<BatchJob>> pendingJobs;
    std::vector<std::shared_ptr<BatchJob>> runningJobs;
    std::vector<std::shared_ptr<BatchJob>> completedJobs;
    std::vector<std::shared_ptr<BatchJob>> failedJobs;
    std::vector<std::shared_ptr<BatchJob>> cancelledJobs;
    
    // Worker threads
    std::vector<std::thread> workers;
    std::vector<WorkerStatus> workerStatuses;
    
    // Control flags
    std::atomic<bool> running;
    std::atomic<bool> paused;
    std::atomic<bool> stopping;
    
    // Synchronization
    std::mutex jobsMutex;
    std::mutex workersMutex;
    std::condition_variable jobsCondition;
    std::condition_variable workersCondition;
    
    // Statistics
    BatchStats stats;
    std::atomic<size_t> nextJobId;
    
    Impl() : maxWorkers(2), maxQueueSize(100), progressMonitoringEnabled(true),
             checkpointFrequency(100), running(false), paused(false), stopping(false),
             nextJobId(0), threadPool(2) {}
    
    ~Impl() {
        stop();
    }
};

ParallelBatchProcessor::ParallelBatchProcessor() : pImpl(std::make_unique<Impl>()) {
}

ParallelBatchProcessor::~ParallelBatchProcessor() = default;

void ParallelBatchProcessor::setConfig(std::shared_ptr<ResearchConfigManager> configManager) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    pImpl->configManager = configManager;
}

void ParallelBatchProcessor::setMaxWorkers(size_t maxWorkers) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    pImpl->maxWorkers = maxWorkers;
    pImpl->threadPool = batch_utils::ThreadPool(maxWorkers);
}

void ParallelBatchProcessor::setQueueSize(size_t maxQueueSize) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    pImpl->maxQueueSize = maxQueueSize;
}

void ParallelBatchProcessor::enableProgressMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    pImpl->progressMonitoringEnabled = enable;
}

void ParallelBatchProcessor::setCheckpointFrequency(size_t frequency) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    pImpl->checkpointFrequency = frequency;
}

std::string ParallelBatchProcessor::submitJob(const ResearchConfig& config, uint64_t seed, SimulationStep maxSteps) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    // Check queue size
    if (pImpl->pendingJobs.size() + pImpl->runningJobs.size() >= pImpl->maxQueueSize) {
        throw std::runtime_error("Batch queue is full");
    }
    
    auto job = std::make_shared<BatchJob>();
    job->jobId = generateJobId();
    job->config = config;
    job->configName = config.name;
    job->seed = seed;
    job->maxSteps = maxSteps;
    job->priority = BatchPriority::Normal;
    job->status = JobStatus::Pending;
    
    pImpl->pendingJobs.push_back(job);
    pImpl->stats.totalJobs++;
    pImpl->stats.pendingJobs++;
    
    pImpl->jobsCondition.notify_one();
    return job->jobId;
}

std::string ParallelBatchProcessor::submitJob(const std::string& configName, uint64_t seed, SimulationStep maxSteps) {
    if (!pImpl->configManager) {
        throw std::runtime_error("No config manager available");
    }
    
    if (!pImpl->configManager->isPreset(configName)) {
        throw std::runtime_error("Unknown preset configuration: " + configName);
    }
    
    ResearchConfig config = pImpl->configManager->getPresetConfig(configName);
    return submitJob(config, seed, maxSteps);
}

bool ParallelBatchProcessor::cancelJob(const std::string& jobId) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    // Check all job lists
    auto cancelJob = [&](auto& jobs) {
        for (auto& job : jobs) {
            if (job->jobId == jobId) {
                job->status = JobStatus::Cancelled;
                return true;
            }
        }
        return false;
    };
    
    if (cancelJob(pImpl->pendingJobs)) {
        pImpl->stats.pendingJobs--;
        return true;
    } else if (cancelJob(pImpl->runningJobs)) {
        pImpl->stats.runningJobs--;
        pImpl->stats.pendingJobs++;
        return true;
    } else if (cancelJob(pImpl->completedJobs)) {
        // Already completed, just mark as cancelled
        for (auto& job : pImpl->completedJobs) {
            if (job->jobId == jobId) {
                job->status = JobStatus::Cancelled;
                return true;
            }
        }
    } else if (cancelJob(pImpl->failedJobs)) {
        for (auto& job : pImpl->failedJobs) {
            if (job->jobId == jobId) {
                job->status = JobStatus::Cancelled;
                return true;
            }
        }
    } else if (cancelJob(pImpl->cancelledJobs)) {
        return true;
    }
    
    return false;
}

bool ParallelBatchProcessor::pauseJob(const std::string& jobId) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    auto pauseJob = [&](auto& jobs) {
        for (auto& job : jobs) {
            if (job->jobId == jobId && job->status == JobStatus::Running) {
                job->status = JobStatus::Pending;
                pImpl->stats.runningJobs--;
                pImpl->stats.pendingJobs++;
                return true;
            }
        }
        return false;
    };
    
    return pauseJob(pImpl->runningJobs);
}

bool ParallelBatchProcessor::resumeJob(const std::string& jobId) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    auto resumeJob = [&](auto& jobs) {
        for (auto& job : jobs) {
            if (job->jobId == jobId && job->status == JobStatus::Pending) {
                job->status = JobStatus::Running;
                pImpl->stats.pendingJobs--;
                pImpl->stats.runningJobs++;
                return true;
            }
        }
        return false;
    };
    
    return resumeJob(pImpl->pendingJobs);
}

const BatchJob* ParallelBatchProcessor::getJob(const std::string& jobId) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    auto findJob = [&](const auto& jobs) -> const BatchJob* {
        for (const auto& job : jobs) {
            if (job->jobId == jobId) {
                return job.get();
            }
        }
        return nullptr;
    };
    
    const BatchJob* job = findJob(pImpl->pendingJobs);
    if (job) return job;
    job = findJob(pImpl->runningJobs);
    if (job) return job;
    job = findJob(pImpl->completedJobs);
    if (job) return job;
    job = findJob(pImpl->failedJobs);
    if (job) return job;
    job = findJob(pImpl->cancelledJobs);
    
    return job;
}

std::vector<const BatchJob*> ParallelBatchProcessor::getJobs(JobStatus status) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    std::vector<const BatchJob*> result;
    
    auto filterJobs = [&](const auto& jobs) {
        for (const auto& job : jobs) {
            if (job->status == status) {
                result.push_back(job.get());
            }
        }
    };
    
    if (status == JobStatus::All || status == JobStatus::Pending) {
        filterJobs(pImpl->pendingJobs);
    }
    if (status == JobStatus::All || status == JobStatus::Running) {
        filterJobs(pImpl->runningJobs);
    }
    if (status == JobStatus::All || status == JobStatus::Completed) {
        filterJobs(pImpl->completedJobs);
    }
    if (status == JobStatus::All || status == JobStatus::Failed) {
        filterJobs(pImpl->failedJobs);
    }
    if (status == JobStatus::All || status == JobStatus::Cancelled) {
        filterJobs(pImpl->cancelledJobs);
    }
    
    return result;
}

BatchStats ParallelBatchProcessor::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    BatchStats stats = pImpl->stats;
    
    // Update running counts
    stats.pendingJobs = pImpl->pendingJobs.size();
    stats.runningJobs = pImpl->runningJobs.size();
    
    // Calculate average progress
    double totalProgress = 0.0;
    size_t count = 0;
    auto calculateProgress = [&](const auto& jobs) {
        for (const auto& job : jobs) {
            totalProgress += job->progress;
            count++;
        }
    };
    
    calculateProgress(pImpl->pendingJobs);
    calculateProgress(pImpl->runningJobs);
    calculateProgress(pImpl->completedJobs);
    calculateProgress(pImpl->failedJobs);
    
    if (count > 0) {
        stats.averageProgress = totalProgress / count;
    }
    
    stats.parallelWorkers = pImpl->maxWorkers;
    
    return stats;
}

std::vector<WorkerStatus> ParallelBatchProcessor::getWorkerStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->workersMutex);
    return pImpl->workerStatuses;
}

void ParallelBatchProcessor::start() {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    if (pImpl->running) {
        return;
    }
    
    pImpl->running = true;
    pImpl->paused = false;
    pImpl->stopping = false;
    
    // Initialize worker statuses
    pImpl->workerStatuses.clear();
    for (size_t i = 0; i < pImpl->maxWorkers; ++i) {
        WorkerStatus status;
        status.workerId = "worker_" + std::to_string(i);
        status.isActive = true;
        status.tasksCompleted = 0;
        status.tasksFailed = 0;
        status.lastActive = std::chrono::system_clock::now();
        status.currentTask = "Idle";
        pImpl->workerStatuses.push_back(status);
    }
    
    // Start worker threads
    pImpl->workers.clear();
    for (size_t i = 0; i < pImpl->maxWorkers; ++i) {
        pImpl->workers.emplace_back(&ParallelBatchProcessor::workerThread, this, i);
    }
}

void ParallelBatchProcessor::stop() {
    {
        std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
        if (!pImpl->running) {
            return;
        }
        
        pImpl->stopping = true;
        pImpl->running = false;
        pImpl->paused = false;
    }
    
    pImpl->jobsCondition.notify_all();
    
    for (auto& worker : pImpl->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    pImpl->workers.clear();
    pImpl->threadPool.stop();
}

void ParallelBatchProcessor::pause() {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    pImpl->paused = true;
}

void ParallelBatchProcessor::resume() {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    pImpl->paused = false;
    pImpl->jobsCondition.notify_all();
}

bool ParallelBatchProcessor::isRunning() const {
    return pImpl->running;
}

bool ParallelBatchProcessor::isPaused() const {
    return pImpl->paused;
}

double ParallelBatchProcessor::getOverallProgress() const {
    BatchStats stats = getStats();
    if (stats.totalJobs == 0) {
        return 0.0;
    }
    
    double completedProgress = stats.completedJobs * 100.0;
    double runningProgress = stats.runningJobs * stats.averageProgress;
    
    return (completedProgress + runningProgress) / stats.totalJobs;
}

std::string ParallelBatchProcessor::getCurrentJobId() const {
    BatchStats stats = getStats();
    if (stats.runningJobs == 0) {
        return "";
    }
    
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    for (const auto& job : pImpl->runningJobs) {
        if (job->progress > 0 && job->progress < 100) {
            return job->jobId;
        }
    }
    
    if (!pImpl->runningJobs.empty()) {
        return pImpl->runningJobs[0]->jobId;
    }
    
    return "";
}

std::chrono::duration<double> ParallelBatchProcessor::getEstimatedTimeRemaining() const {
    BatchStats stats = getStats();
    if (stats.completedJobs == 0 || stats.runningJobs == 0) {
        return std::chrono::duration<double>(0.0);
    }
    
    double avgTimePerJob = stats.totalElapsedTime.count() / stats.completedJobs;
    double pendingJobs = stats.totalJobs - stats.completedJobs - stats.runningJobs;
    
    return std::chrono::duration<double>(pendingJobs * avgTimePerJob);
}

std::shared_ptr<Experiment> ParallelBatchProcessor::getExperimentResult(const std::string& jobId) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    auto findJob = [&](const auto& jobs) -> std::shared_ptr<Experiment> {
        for (const auto& job : jobs) {
            if (job->jobId == jobId) {
                return job->experiment;
            }
        }
        return nullptr;
    };
    
    std::shared_ptr<Experiment> result = findJob(pImpl->completedJobs);
    if (result) return result;
    
    result = findJob(pImpl->failedJobs);
    if (result) return result;
    
    return nullptr;
}

std::vector<std::shared_ptr<Experiment>> ParallelBatchProcessor::getAllResults() const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    std::vector<std::shared_ptr<Experiment>> results;
    
    auto collectResults = [&](const auto& jobs) {
        for (const auto& job : jobs) {
            if (job->experiment) {
                results.push_back(job->experiment);
            }
        }
    };
    
    collectResults(pImpl->completedJobs);
    collectResults(pImpl->failedJobs);
    
    return results;
}

bool ParallelBatchProcessor::saveResults(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Batch Processing Results\n";
        file << "# Generated: " << batch_utils::formatTimePoint(std::chrono::system_clock::now()) << "\n\n";
        
        file << "Statistics:\n";
        file << "  Total Jobs: " << pImpl->stats.totalJobs << "\n";
        file << "  Completed: " << pImpl->stats.completedJobs << "\n";
        file << "  Failed: " << pImpl->stats.failedJobs << "\n";
        file << "  Average Progress: " << pImpl->stats.averageProgress << "%\n";
        file << "\n";
        
        file << "Jobs:\n";
        for (const auto& job : pImpl->completedJobs) {
            file << "  Job " << job->jobId << ": " << job->configName << "\n";
            file << "    Seed: " << job->seed << "\n";
            file << "    Max Steps: " << job->maxSteps << "\n";
            file << "    Status: " << batch_utils::jobStatusToString(job->status) << "\n";
            file << "    Progress: " << job->progress << "%\n";
            if (job->experiment) {
                file << "    Experiments: " << (job->experiment ? "Yes" : "No") << "\n";
            }
            file << "\n";
        }
        
        file << "\n";
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string ParallelBatchProcessor::generateProgressReport() const {
    std::ostringstream oss;
    oss << "NLM Batch Processing - Progress Report\n";
    oss << "====================================" << "\n\n";
    
    BatchStats stats = getStats();
    
    oss << "Overall Progress: " << getOverallProgress() << "%\n";
    oss << "Total Jobs: " << stats.totalJobs << "\n";
    oss << "  - Completed: " << stats.completedJobs << "\n";
    oss << "  - Running: " << stats.runningJobs << "\n";
    oss << "  - Pending: " << stats.pendingJobs << "\n";
    oss << "  - Failed: " << stats.failedJobs << "\n";
    oss << "\n";
    
    if (stats.runningJobs > 0) {
        oss << "Currently Running Jobs:\n";
        for (const auto* job : getJobs(JobStatus::Running)) {
            oss << "  - Job " << job->jobId << ": " << job->configName 
                << " (" << job->progress << "% complete)\n";
        }
        oss << "\n";
    }
    
    oss << "Workers Status:\n";
    std::vector<WorkerStatus> workers = getWorkerStatus();n    for (const auto& worker : workers) {
        oss << "  - " << worker.workerId << ": " << worker.currentTask 
            << " (" << worker.tasksCompleted << " completed, " 
            << worker.tasksFailed << " failed)\n";
    }
    oss << "\n";
    
    return oss.str();
}

std::string ParallelBatchProcessor::generateDetailedReport() const {
    std::ostringstream oss;
    oss << "NLM Batch Processing - Detailed Report\n";
    oss << "=======================================" << "\n\n";
    
    BatchStats stats = getStats();
    
    oss << "Job Details:\n";
    oss << "-------------\n";
    
    auto allJobs = getJobs(JobStatus::All);
    for (const auto* job : allJobs) {
        oss << "Job: " << job->jobId << "\n";
        oss << "  Configuration: " << job->configName << "\n";
        oss << "  Seed: " << job->seed << "\n";
        oss << "  Max Steps: " << job->maxSteps << "\n";
        oss << "  Priority: " << batch_utils::batchPriorityToString(job->priority) << "\n";
        oss << "  Status: " << batch_utils::jobStatusToString(job->status) << "\n";
        oss << "  Progress: " << job->progress << "%\n";
        oss << "  Start Time: " << batch_utils::formatTimePoint(job->startTime) << "\n";
        if (job->endTime.time_since_epoch().count() > 0) {
            auto duration = job->endTime - job->startTime;
            oss << "  Duration: " << batch_utils::formatDuration(duration) << "\n";
        }
        if (!job->errorMessage.empty()) {
            oss << "  Error: " << job->errorMessage << "\n";
        }
        oss << "\n";
    }
    
    oss << "\nWorker Details:\n";
    oss << "----------------\n";
    std::vector<WorkerStatus> workers = getWorkerStatus();
    for (const auto& worker : workers) {
        oss << "Worker: " << worker.workerId << "\n";
        oss << "  Active: " << (worker.isActive ? "Yes" : "No") << "\n";
        oss << "  Current Task: " << worker.currentTask << "\n";
        oss << "  Tasks Completed: " << worker.tasksCompleted << "\n";
        oss << "  Tasks Failed: " << worker.tasksFailed << "\n";
        oss << "  Last Active: " << batch_utils::formatTimePoint(worker.lastActive) << "\n";
        oss << "\n";
    }
    
    return oss.str();
}

std::string ParallelBatchProcessor::generateWorkerReport() const {
    std::ostringstream oss;
    oss << "NLM Batch Processing - Worker Report\n";
    oss << "=====================================" << "\n\n";
    
    std::vector<WorkerStatus> workers = getWorkerStatus();
    for (const auto& worker : workers) {
        oss << "Worker: " << worker.workerId << "\n";
        oss << "  Status: " << (worker.isActive ? "Active" : "Inactive") << "\n";
        oss << "  Utilization: " << (worker.tasksCompleted + worker.tasksFailed) << " tasks\n";
        oss << "  Current Task: " << worker.currentTask << "\n";
        oss << "  Last Active: " << batch_utils::formatTimePoint(worker.lastActive) << "\n";
        oss << "\n";
    }
    
    return oss.str();
}

void ParallelBatchProcessor::workerThread(size_t workerId) {
    WorkerStatus& status = pImpl->workerStatuses[workerId];
    status.workerId = "worker_" << std::to_string(workerId);
    status.isActive = true;
    status.lastActive = std::chrono::system_clock::now();
    
    while (!pImpl->stopping) {
        std::shared_ptr<BatchJob> job;
        
        {
            std::unique_lock<std::mutex> lock(pImpl->jobsMutex);
            
            // Wait for work if paused or no jobs
            pImpl->jobsCondition.wait(lock, [this]() {
                return !pImpl->running || pImpl->stopping || 
                       (!pImpl->paused && !pImpl->pendingJobs.empty()) ||
                       (!pImpl->paused && pImpl->runningJobs.size() < pImpl->maxWorkers);
            });
            
            if (pImpl->stopping) {
                break;
            }
            
            if (pImpl->paused) {
                continue;
            }
            
            // Get next job
            if (!pImpl->pendingJobs.empty()) {
                // Sort pending jobs by priority
                std::sort(pImpl->pendingJobs.begin(), pImpl->pendingJobs.end(),
                         batch_utils::compareJobsByPriority);
                
                job = pImpl->pendingJobs.back();
                pImpl->pendingJobs.pop_back();
                
                job->status = JobStatus::Running;
                job->startTime = std::chrono::system_clock::now();
                pImpl->runningJobs.push_back(job);
                pImpl->stats.runningJobs++;
                pImpl->stats.pendingJobs--;
                
                status.currentTask = "Running job: " + job->jobId;
            } else {
                status.currentTask = "Idle";
                continue;
            }
        }
        
        // Update worker status
        status.lastActive = std::chrono::system_clock::now();
        
        // Execute the job
        bool success = executeJob(job);
        
        {
            std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
            
            // Move job from running to completed/failed
            auto jobIt = std::find_if(pImpl->runningJobs.begin(), pImpl->runningJobs.end(),
                                     [&](const std::shared_ptr<BatchJob>& j) { return j->jobId == job->jobId; });
            
            if (jobIt != pImpl->runningJobs.end()) {
                auto completedJob = *jobIt;
                pImpl->runningJobs.erase(jobIt);
                
                if (success) {
                    completedJob->status = JobStatus::Completed;
                    pImpl->completedJobs.push_back(completedJob);
                    pImpl->stats.completedJobs++;
                    status.tasksCompleted++;
                } else {
                    completedJob->status = JobStatus::Failed;
                    pImpl->failedJobs.push_back(completedJob);
                    pImpl->stats.failedJobs++;
                    status.tasksFailed++;
                }
                
                completedJob->endTime = std::chrono::system_clock::now();
                completedJob->progress = 100.0;
                
                pImpl->stats.runningJobs--;
                status.currentTask = "Idle";
            }
            
            pImpl->workersCondition.notify_all();
        }
    }
    
    status.isActive = false;
}

bool ParallelBatchProcessor::executeJob(std::shared_ptr<BatchJob> job) {
    try {
        if (!pImpl->configManager) {
            throw std::runtime_error("No config manager available");
        }
        
        // Convert config to core config for simulation
        nlm::Config coreConfig = pImpl->configManager->toCoreConfig();
        
        // Set random seed from job
        coreConfig.set("random_seed", job->seed, nlm::ConfigSource::Runtime);
        
        // TODO: In a real implementation, we would create a brain and environment
        // based on the configuration and run the experiment
        
        // For now, simulate some work
        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Update progress
            double progress = static_cast<double>(i + 1) / 10.0 * 100.0;
            updateJobProgress(job, progress);
            
            if (pImpl->stopping) {
                return false;
            }
        }
        
        // Create a dummy experiment result
        job->experiment = std::make_shared<Experiment>(job->configName);
        job->experiment->setSeed(job->seed);
        
        // Record some metrics
        job->experiment->recordMetric("total_reward", 100.0 + job->seed * 10.0);
        job->experiment->recordMetric("final_firing_rate", 0.5 + job->seed * 0.01);
        
        return true;
    } catch (const std::exception& e) {
        // Store error message
        std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
        auto jobIt = std::find_if(pImpl->runningJobs.begin(), pImpl->runningJobs.end(),
                                 [&](const std::shared_ptr<BatchJob>& j) { return j->jobId == job->jobId; });
        if (jobIt != pImpl->runningJobs.end()) {
            (*jobIt)->errorMessage = e.what();
        } else {
            job->errorMessage = e.what();
        }
        return false;
    }
}

void ParallelBatchProcessor::updateJobProgress(std::shared_ptr<BatchJob> job, double progress) {
    if (pImpl->progressMonitoringEnabled) {
        job->progress = progress;
    }
}

void ParallelBatchProcessor::completeJob(std::shared_ptr<BatchJob> job, bool success, const std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    job->progress = 100.0;
    job->endTime = std::chrono::system_clock::now();
    
    if (success) {
        job->status = JobStatus::Completed;
        pImpl->completedJobs.push_back(job);
        pImpl->stats.completedJobs++;
    } else {
        job->status = JobStatus::Failed;
        job->errorMessage = errorMsg;
        pImpl->failedJobs.push_back(job);
        pImpl->stats.failedJobs++;
    }
    
    pImpl->stats.runningJobs--;
    pImpl->stats.pendingJobs--;
}

} // namespace nlm