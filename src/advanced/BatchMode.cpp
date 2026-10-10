// NLM Advanced Features - Batch Mode Implementation
// Batch processing mode for handling large volumes of data and tasks

#include "BatchMode.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace nlm {
namespace advanced {

// Implementation of BatchMode

struct BatchMode::Impl {
    BatchConfig config;
    bool active;
    bool running;
    std::vector<BatchJob> jobs;
    std::map<std::string, std::vector<BatchItem>> jobItems;
    BatchStatistics statistics;
    
    std::vector<std::thread> workerThreads;
    std::atomic<bool> stopWorkers;
    std::mutex jobsMutex;
    std::mutex itemsMutex;
    std::mutex configMutex;
    std::condition_variable jobsCondition;
    
    std::function<bool(const BatchJob&)> jobFilter;
    std::function<void(const BatchJob&)> jobPreProcessor;
    std::function<void(const BatchJob&)> jobPostProcessor;
    std::function<void(const BatchItem&)> itemCallback;
    std::function<void(const BatchJob&)> jobCallback;
    std::function<void(double)> progressCallback;
    
    Impl() : active(false), running(false), stopWorkers(false) {
        statistics.firstJobTime = std::chrono::system_clock::now();
    }
    
    ~Impl() {
        stop();
    }
    
    void start() {
        std::lock_guard<std::mutex> lock(configMutex);
        
        if (active || jobs.empty()) {
            return;
        }
        
        active = true;
        running = true;
        stopWorkers = false;
        
        // Start worker threads
        startWorkerThreads();
        
        // Notify all jobs are ready
        jobsCondition.notify_all();
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(configMutex);
        
        if (!active) {
            return;
        }
        
        running = false;
        stopWorkers = true;
        
        // Wake up all worker threads
        jobsCondition.notify_all();
        
        // Wait for worker threads to finish
        for (auto& thread : workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        active = false;
    }
    
    void startWorkerThreads() {
        size_t numWorkers = std::min(static_cast<size_t>(config.maxConcurrentJobs), jobs.size());
        if (numWorkers == 0) numWorkers = 1;
        
        workerThreads.clear();
        for (size_t i = 0; i < numWorkers; ++i) {
            workerThreads.emplace_back(&Impl::workerThread, this);
        }
    }
    
    void stopWorkerThreads() {
        for (auto& thread : workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        workerThreads.clear();
    }
    
    void workerThread() {
        while (!stopWorkers.load() && running.load()) {
            std::unique_lock<std::mutex> lock(jobsMutex);
            
            // Find a job to process
            BatchJob* jobToProcess = nullptr;
            
            // Sort jobs by priority
            std::sort(jobs.begin(), jobs.end(),
                      [](const BatchJob& a, const BatchJob& b) {
                          return static_cast<int>(a.priority) > static_cast<int>(b.priority);
                      });
            
            for (auto& job : jobs) {
                if (job.status == BatchJobStatus::Pending || job.status == BatchJobStatus::Running) {
                    jobToProcess = &job;
                    break;
                }
            }
            
            if (jobToProcess == nullptr) {
                // No jobs to process, wait for notification
                jobsCondition.wait_for(lock, std::chrono::milliseconds(100),
                                      [this]() {
                                          return stopWorkers.load() || !running.load() || 
                                                 (jobFilter && std::any_of(jobs.begin(), jobs.end(), jobFilter)) ||
                                                 std::any_of(jobs.begin(), jobs.end(),
                                                            [](const BatchJob& j) { return j.status == BatchJobStatus::Pending; });
                                      });
                continue;
            }
            
            // Process the job
            lock.unlock();
            
            if (config.dryRun) {
                // Dry run: just mark as completed
                std::lock_guard<std::mutex> jobLock(jobsMutex);
                auto& job = *jobToProcess;
                job.status = BatchJobStatus::Completed;
                job.endTime = std::chrono::system_clock::now();
                job.actualTime = 0.0;
                job.processedItems = job.totalItems;
                
                if (jobCallback) jobCallback(job);
            } else {
                // Actual processing
                processJobInternal(*jobToProcess);
            }
            
            lock.lock();
            jobsCondition.notify_all();
        }
    }
    
    void processJobInternal(BatchJob& job) {
        auto startTime = std::chrono::system_clock::now();
        
        // Update job status
        job.status = BatchJobStatus::Running;
        job.startTime = startTime;
        
        // Pre-process job
        if (jobPreProcessor) {
            jobPreProcessor(job);
        }
        
        // Get items for this job
        std::vector<BatchItem> items;
        {
            std::lock_guard<std::mutex> lock(itemsMutex);
            auto it = jobItems.find(job.id);
            if (it != jobItems.end()) {
                items = it->second;
            }
        }
        
        // Validate job
        if (config.skipExisting) {
            // Filter out already processed items
            std::vector<BatchItem> remainingItems;
            for (const auto& item : items) {
                if (item.status != BatchJobStatus::Completed && 
                    item.status != BatchJobStatus::Skipped) {
                    remainingItems.push_back(item);
                }
            }
            items = remainingItems;
        }
        
        // Process each item
        uint64_t successfulItems = 0;
        uint64_t failedItems = 0;
        uint64_t skippedItems = 0;
        
        for (auto& item : items) {
            if (!running) break; // Stop if batch processing was stopped
            
            if (config.dryRun) {
                item.status = BatchJobStatus::Completed;
                item.endTime = std::chrono::system_clock::now();
                item.processingTime = 0.0;
                successfulItems++;
            } else {
                // Process the item
                bool success = processItem(item);
                if (success) {
                    item.status = BatchJobStatus::Completed;
                    successfulItems++;
                } else {
                    if (config.continueOnFailure) {
                        item.status = BatchJobStatus::Failed;
                        failedItems++;
                    } else {
                        item.status = BatchJobStatus::Skipped;
                        skippedItems++;
                    }
                }
            }
            
            item.endTime = std::chrono::system_clock::now();
            
            if (itemCallback) {
                itemCallback(item);
            }
            
            // Update progress
            updateJobProgress(job.id);
            
            if (progressCallback) {
                double progress = (static_cast<double>(job.processedItems) / job.totalItems) * 100.0;
                progressCallback(progress);
            }
        }
        
        // Update job status
        job.endTime = std::chrono::system_clock::now();
        job.actualTime = std::chrono::duration<double>(job.endTime - startTime).count();
        job.processedItems = successfulItems + failedItems + skippedItems;
        
        if (failedItems == 0) {
            job.status = BatchJobStatus::Completed;
        } else if (failedItems < job.totalItems) {
            job.status = BatchJobStatus::Completed; // Some failures but continued
        } else {
            job.status = BatchJobStatus::Failed;
        }
        
        // Post-process job
        if (jobPostProcessor) {
            jobPostProcessor(job);
        }
        
        if (jobCallback) {
            jobCallback(job);
        }
        
        // Update statistics
        updateStatistics(job, successfulItems, failedItems, skippedItems);
    }
    
    bool processItem(BatchItem& item) {
        if (!item.itemProcessor) {
            return false;
        }
        
        auto startTime = std::chrono::system_clock::now();
        
        try {
            bool success = item.itemProcessor(item);
            
            auto endTime = std::chrono::system_clock::now();
            item.processingTime = std::chrono::duration<double>(endTime - startTime).count();
            item.endTime = endTime;
            
            return success;
        } catch (const std::exception& e) {
            item.errorMessage = e.what();
            return false;
        }
    }
    
    bool validateJob(const BatchJob& job) const {
        if (job.id.empty() || job.type == BatchJobType::Custom) {
            return false;
        }
        
        if (job.status == BatchJobStatus::Completed || job.status == BatchJobStatus::Failed) {
            return true; // Already validated
        }
        
        return true;
    }
    
    void updateJobStatus(const std::string& jobId, BatchJobStatus status) {
        std::lock_guard<std::mutex> lock(jobsMutex);
        
        for (auto& job : jobs) {
            if (job.id == jobId) {
                job.status = status;
                break;
            }
        }
    }
    
    void updateJobProgress(const std::string& jobId) {
        std::lock_guard<std::mutex> lock(jobsMutex);
        
        for (auto& job : jobs) {
            if (job.id == jobId) {
                uint64_t completedItems = 0;
                auto it = jobItems.find(job.id);
                if (it != jobItems.end()) {
                    for (const auto& item : it->second) {
                        if (item.status == BatchJobStatus::Completed || 
                            item.status == BatchJobStatus::Skipped) {
                            completedItems++;
                        }
                    }
                }
                
                job.processedItems = completedItems;
                break;
            }
        }
    }
    
    void updateStatistics(const BatchJob& job, uint64_t successful, uint64_t failed, uint64_t skipped) {
        statistics.totalJobs++;
        statistics.jobsByType[static_cast<int>(job.type)]++;
        statistics.jobsByStatus[static_cast<int>(job.status)]++;
        statistics.jobsByPriority[static_cast<int>(job.priority)]++;
        statistics.totalItems += job.totalItems;
        statistics.processedItems += (successful + failed + skipped);
        statistics.failedItems += failed;
        statistics.skippedItems += skipped;
        
        auto now = std::chrono::system_clock::now();
        statistics.lastJobTime = now;
        
        if (statistics.totalJobs == 1) {
            statistics.firstJobTime = now;
        }
        
        // Calculate job rate
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - statistics.firstJobTime);
        if (duration.count() > 0) {
            statistics.jobRatePerMinute = (static_cast<double>(statistics.totalJobs) / duration.count()) * 60.0;
        }
        
        // Calculate success rate
        uint64_t totalCompleted = successful + failed + skipped;
        if (totalCompleted > 0) {
            statistics.successRate = (static_cast<double>(successful) / totalCompleted) * 100.0;
        }
        
        // Calculate average processing time
        if (statistics.totalJobs > 0) {
            statistics.averageProcessingTime = statistics.totalProcessingTime / statistics.totalJobs;
        }
        
        // Update most frequent job
        statistics.jobFrequency[job.description]++;
        if (statistics.jobFrequency[job.description] > statistics.mostFrequentJobCount) {
            statistics.mostFrequentJobCount = statistics.jobFrequency[job.description];
            statistics.mostFrequentJob = job.description;
        }
        
        // Update recent jobs
        statistics.recentJobs.push_back(job.description);
        if (statistics.recentJobs.size() > 100) {
            statistics.recentJobs.pop_front();
        }
    }
    
    std::string generateJobId() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1000, 9999);
        
        std::time_t now = std::time(nullptr);
        char timeBuf[20];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y%m%d%H%M%S", std::localtime(&now));
        
        return std::string(timeBuf) + "_" + std::to_string(dis(gen));
    }
    
    std::string generateItemId() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10000, 99999);
        
        return "item_" + std::to_string(dis(gen));
    }
    
    std::string formatTime(double seconds) const {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        int millisecs = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
        
        std::ostringstream oss;
        if (hours > 0) oss << hours << "h ";
        oss << minutes << "m " << secs << "." << std::setw(3) << std::setfill('0') << millisecs << "s";
        return oss.str();
    }
    
    std::string formatBytes(uint64_t bytes) const {
        if (bytes < 1024) return std::to_string(bytes) + " B";
        if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
        if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
    
    std::string getTimeString(const std::chrono::system_clock::time_point& time) const {
        auto time_t = std::chrono::system_clock::to_time_t(time);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
        return ss.str();
    }
};

BatchMode::BatchMode() : pImpl(std::make_unique<Impl>()) {
}

BatchMode::~BatchMode() {
    stop();
}

void BatchMode::configure(const BatchConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->config = config;
}

BatchConfig BatchMode::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->config;
}

bool BatchMode::loadJobs(const std::string& filename) {
    std::vector<std::string> filenames = {filename};
    return loadJobs(filenames);
}

bool BatchMode::loadJobs(const std::vector<std::string>& filenames) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    for (const auto& filename : filenames) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple JSON parsing (placeholder)
        // In a real implementation, you would use a proper JSON parser
        // This is a simplified version for demonstration
        
        file.close();
    }
    
    return true;
}

bool BatchMode::saveJobs(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Save jobs in JSON format (simplified)
    file << "[]" << std::endl; // Placeholder
    
    return true;
}

bool BatchMode::start() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    if (pImpl->active || pImpl->jobs.empty()) {
        return false;
    }
    
    pImpl->start();
    return true;
}

void BatchMode::stop() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->stop();
}

bool BatchMode::isActive() const {
    return pImpl->active;
}

bool BatchMode::isRunning() const {
    return pImpl->running;
}

bool BatchMode::addJob(const BatchJob& job) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    if (!validateJob(job)) {
        return false;
    }
    
    pImpl->jobs.push_back(job);
    return true;
}

bool BatchMode::removeJob(const std::string& jobId) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    auto it = std::remove_if(pImpl->jobs.begin(), pImpl->jobs.end(),
                             [&jobId](const BatchJob& job) { return job.id == jobId; });
    
    if (it != pImpl->jobs.end()) {
        pImpl->jobs.erase(it, pImpl->jobs.end());
        
        // Also remove job items
        std::lock_guard<std::mutex> itemsLock(pImpl->itemsMutex);
        pImpl->jobItems.erase(jobId);
        
        return true;
    }
    
    return false;
}

bool BatchMode::updateJob(const BatchJob& job) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    for (auto& existingJob : pImpl->jobs) {
        if (existingJob.id == job.id) {
            existingJob = job;
            return true;
        }
    }
    
    return false;
}

bool BatchMode::pauseJob(const std::string& jobId) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    for (auto& job : pImpl->jobs) {
        if (job.id == jobId) {
            job.status = BatchJobStatus::Pending; // Simplified: pause by setting to pending
            return true;
        }
    }
    
    return false;
}

bool BatchMode::resumeJob(const std::string& jobId) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    for (auto& job : pImpl->jobs) {
        if (job.id == jobId) {
            if (job.status == BatchJobStatus::Pending) {
                job.status = BatchJobStatus::Running;
                return true;
            }
        }
    }
    
    return false;
}

bool BatchMode::cancelJob(const std::string& jobId) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    for (auto& job : pImpl->jobs) {
        if (job.id == jobId) {
            job.status = BatchJobStatus::Cancelled;
            return true;
        }
    }
    
    return false;
}

std::vector<BatchJob> BatchMode::getJobs() const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    return pImpl->jobs;
}

std::vector<BatchJob> BatchMode::getJobsByType(BatchJobType type) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    std::vector<BatchJob> result;
    
    for (const auto& job : pImpl->jobs) {
        if (job.type == type) {
            result.push_back(job);
        }
    }
    
    return result;
}

std::vector<BatchJob> BatchMode::getJobsByStatus(BatchJobStatus status) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    std::vector<BatchJob> result;
    
    for (const auto& job : pImpl->jobs) {
        if (job.status == status) {
            result.push_back(job);
        }
    }
    
    return result;
}

std::vector<BatchJob> BatchMode::getJobsByPriority(BatchJobPriority priority) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    std::vector<BatchJob> result;
    
    for (const auto& job : pImpl->jobs) {
        if (job.priority == priority) {
            result.push_back(job);
        }
    }
    
    return result;
}

BatchJob BatchMode::getJob(const std::string& jobId) const {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    for (const auto& job : pImpl->jobs) {
        if (job.id == jobId) {
            return job;
        }
    }
    
    return BatchJob(); // Return empty job if not found
}

bool BatchMode::addItemToJob(const std::string& jobId, const BatchItem& item) {
    std::lock_guard<std::mutex> lock(pImpl->itemsMutex);
    
    pImpl->jobItems[jobId].push_back(item);
    
    // Update job total items
    std::lock_guard<std::mutex> jobsLock(pImpl->jobsMutex);
    for (auto& job : pImpl->jobs) {
        if (job.id == jobId) {
            job.totalItems++;
            break;
        }
    }
    
    return true;
}

bool BatchMode::removeItemFromJob(const std::string& jobId, const std::string& itemId) {
    std::lock_guard<std::mutex> lock(pImpl->itemsMutex);
    
    auto it = jobItems.find(jobId);
    if (it == jobItems.end()) {
        return false;
    }
    
    auto& items = it->second;
    auto itemIt = std::remove_if(items.begin(), items.end(),
                                  [&itemId](const BatchItem& item) { return item.id == itemId; });
    
    if (itemIt != items.end()) {
        items.erase(itemIt, items.end());
        return true;
    }
    
    return false;
}

bool BatchMode::updateItemInJob(const std::string& jobId, const BatchItem& item) {
    std::lock_guard<std::mutex> lock(pImpl->itemsMutex);
    
    auto it = jobItems.find(jobId);
    if (it == jobItems.end()) {
        return false;
    }
    
    auto& items = it->second;
    for (auto& existingItem : items) {
        if (existingItem.id == item.id) {
            existingItem = item;
            return true;
        }
    }
    
    return false;
}

std::vector<BatchItem> BatchMode::getItemsInJob(const std::string& jobId) const {
    std::lock_guard<std::mutex> lock(pImpl->itemsMutex);
    
    auto it = jobItems.find(jobId);
    if (it != jobItems.end()) {
        return it->second;
    }
    
    return std::vector<BatchItem>();
}

BatchItem BatchMode::getItem(const std::string& jobId, const std::string& itemId) const {
    std::lock_guard<std::mutex> lock(pImpl->itemsMutex);
    
    auto jobIt = jobItems.find(jobId);
    if (jobIt == jobItems.end()) {
        return BatchItem();
    }
    
    for (const auto& item : jobIt->second) {
        if (item.id == itemId) {
            return item;
        }
    }
    
    return BatchItem();
}

bool BatchMode::processJob(const std::string& jobId) {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    for (auto& job : pImpl->jobs) {
        if (job.id == jobId) {
            // Start processing thread
            std::thread processThread(&Impl::processJobInternal, pImpl.get(), job);
            processThread.detach();
            return true;
        }
    }
    
    return false;
}

bool BatchMode::processAllJobs() {
    std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
    
    bool anyJobsProcessed = false;
    for (auto& job : pImpl->jobs) {
        if (job.status == BatchJobStatus::Pending || job.status == BatchJobStatus::Running) {
            // Start processing thread
            std::thread processThread(&Impl::processJobInternal, pImpl.get(), job);
            processThread.detach();
            anyJobsProcessed = true;
        }
    }
    
    return anyJobsProcessed;
}

bool BatchMode::waitForJobCompletion(const std::string& jobId, double timeout) {
    auto startTime = std::chrono::system_clock::now();
    
    while (std::chrono::duration<double>(std::chrono::system_clock::now() - startTime).count() < timeout) {
        BatchJob job = getJob(jobId);
        if (job.status == BatchJobStatus::Completed || job.status == BatchJobStatus::Failed) {
            return true;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return false; // Timeout
}

bool BatchMode::waitForAllJobsCompletion(double timeout) {
    auto startTime = std::chrono::system_clock::now();
    
    while (std::chrono::duration<double>(std::chrono::system_clock::now() - startTime).count() < timeout) {
        std::lock_guard<std::mutex> lock(pImpl->jobsMutex);
        
        bool allCompleted = true;
        for (const auto& job : pImpl->jobs) {
            if (job.status != BatchJobStatus::Completed && job.status != BatchJobStatus::Failed && job.status != BatchJobStatus::Cancelled) {
                allCompleted = false;
                break;
            }
        }
        
        if (allCompleted) {
            return true;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return false; // Timeout
}

BatchStatistics BatchMode::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->statistics;
}

double BatchMode::getTotalProcessingTime() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->statistics.totalProcessingTime;
}

uint64_t BatchMode::getTotalItemsProcessed() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->statistics.processedItems;
}

uint64_t BatchMode::getTotalItemsFailed() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->statistics.failedItems;
}

double BatchMode::getSuccessRate() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->statistics.successRate;
}

std::string BatchMode::generateReport() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    std::ostringstream oss;
    oss << "=== NLM Batch Processing Report ===" << std::endl;
    oss << "Total Jobs: " << pImpl->statistics.totalJobs << std::endl;
    oss << "Total Items: " << pImpl->statistics.totalItems << std::endl;
    oss << "Processed Items: " << pImpl->statistics.processedItems << std::endl;
    oss << "Failed Items: " << pImpl->statistics.failedItems << std::endl;
    oss << "Skipped Items: " << pImpl->statistics.skippedItems << std::endl;
    oss << "Success Rate: " << pImpl->statistics.successRate << "%" << std::endl;
    oss << "Average Processing Time: " << formatTime(pImpl->statistics.averageProcessingTime) << std::endl;
    oss << std::endl;
    
    // Generate job reports
    oss << "=== Individual Job Reports ===" << std::endl;
    for (const auto& job : getJobs()) {
        oss << generateJobReport(job.id) << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

std::string BatchMode::generateJobReport(const std::string& jobId) const {
    BatchJob job = getJob(jobId);
    if (job.id.empty()) {
        return "Job not found: " + jobId;
    }
    
    std::ostringstream oss;
    oss << "Job: " << job.id << std::endl;
    oss << "  Type: " << batchJobTypeToString(job.type) << std::endl;
    oss << "  Description: " << job.description << std::endl;
    oss << "  Priority: " << batchJobPriorityToString(job.priority) << std::endl;
    oss << "  Status: " << batchJobStatusToString(job.status) << std::endl;
    oss << "  Total Items: " << job.totalItems << std::endl;
    oss << "  Processed Items: " << job.processedItems << std::endl;
    oss << "  Estimated Time: " << formatTime(job.estimatedTime) << std::endl;
    oss << "  Actual Time: " << formatTime(job.actualTime) << std::endl;
    
    if (!job.errorMessage.empty()) {
        oss << "  Error: " << job.errorMessage << std::endl;
    }
    
    return oss.str();
}

std::string BatchMode::generateProgressReport() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    std::ostringstream oss;
    oss << "=== NLM Batch Processing Progress Report ===" << std::endl;
    oss << "Active Jobs: " << getJobsByStatus(BatchJobStatus::Running).size() << std::endl;
    oss << "Pending Jobs: " << getJobsByStatus(BatchJobStatus::Pending).size() << std::endl;
    oss << "Completed Jobs: " << getJobsByStatus(BatchJobStatus::Completed).size() << std::endl;
    oss << "Failed Jobs: " << getJobsByStatus(BatchJobStatus::Failed).size() << std::endl;
    oss << "Cancelled Jobs: " << getJobsByStatus(BatchJobStatus::Cancelled).size() << std::endl;
    oss << std::endl;
    
    oss << "=== Overall Progress ===" << std::endl;
    double overallProgress = 0.0;
    if (pImpl->statistics.totalItems > 0) {
        overallProgress = (static_cast<double>(pImpl->statistics.processedItems) / pImpl->statistics.totalItems) * 100.0;
    }
    oss << "Progress: " << std::fixed << std::setprecision(1) << overallProgress << "%" << std::endl;
    
    return oss.str();
}

bool BatchMode::exportReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateReport() << std::endl;
    file.close();
    
    return true;
}

bool BatchMode::exportDetailedReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Generate detailed report with all job and item information
    file << generateReport() << std::endl;
    
    // Add detailed job information
    for (const auto& job : getJobs()) {
        file << "\n=== Job Details ===" << std::endl;
        file << "Job ID: " << job.id << std::endl;
        file << "Type: " << batchJobTypeToString(job.type) << std::endl;
        file << "Description: " << job.description << std::endl;
        file << "Priority: " << batchJobPriorityToString(job.priority) << std::endl;
        file << "Status: " << batchJobStatusToString(job.status) << std::endl;
        file << "Scheduled Time: " << getTimeString(job.scheduledTime) << std::endl;
        
        if (job.startTime.time_since_epoch().count() > 0) {
            file << "Start Time: " << getTimeString(job.startTime) << std::endl;
        }
        
        if (job.endTime.time_since_epoch().count() > 0) {
            file << "End Time: " << getTimeString(job.endTime) << std::endl;
            file << "Actual Time: " << formatTime(job.actualTime) << std::endl;
        }
        
        if (!job.errorMessage.empty()) {
            file << "Error: " << job.errorMessage << std::endl;
        }
        
        file << "Total Items: " << job.totalItems << std::endl;
        file << "Processed Items: " << job.processedItems << std::endl;
        
        // Add item details
        std::vector<BatchItem> items = getItemsInJob(job.id);
        for (const auto& item : items) {
            file << "\n  Item: " << item.id << std::endl;
            file << "    Source: " << item.sourcePath << std::endl;
            file << "    Destination: " << item.destinationPath << std::endl;
            file << "    Status: " << batchJobStatusToString(item.status) << std::endl;
            file << "    Processed Bytes: " << item.processedBytes << std::endl;
            file << "    Processing Time: " << formatTime(item.processingTime) << std::endl;
            if (!item.errorMessage.empty()) {
                file << "    Error: " << item.errorMessage << std::endl;
            }
        }
    }
    
    file.close();
    
    return true;
}

bool BatchMode::exportToDatabase(const std::string& connection) const {
    // Database export implementation would go here
    return false; // Placeholder
}

bool BatchMode::importFromDatabase(const std::string& connection) {
    // Database import implementation would go here
    return false; // Placeholder
}

void BatchMode::setJobFilter(const std::function<bool(const BatchJob&)>& filter) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->jobFilter = filter;
}

void BatchMode::addIncludePattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->config.includePatterns.push_back(pattern);
}

void BatchMode::addExcludePattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->config.excludePatterns.push_back(pattern);
}

void BatchMode::setJobCallback(const std::function<void(const BatchJob&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->jobCallback = callback;
}

void BatchMode::setItemCallback(const std::function<void(const BatchItem&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->itemCallback = callback;
}

void BatchMode::setProgressCallback(const std::function<void(double)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->progressCallback = callback;
}

BatchMode& BatchMode::getInstance() {
    static BatchMode instance;
    return instance;
}

void BatchMode::resetInstance() {
    // This is primarily for testing
}

std::string BatchMode::batchJobTypeToString(BatchJobType type) const {
    static const std::map<BatchJobType, std::string> typeMap = {
        {BatchJobType::Processing, "Processing"},
        {BatchJobType::Transformation, "Transformation"},
        {BatchJobType::Analysis, "Analysis"},
        {BatchJobType::Validation, "Validation"},
        {BatchJobType::Export, "Export"},
        {BatchJobType::Import, "Import"},
        {BatchJobType::Cleanup, "Cleanup"},
        {BatchJobType::Custom, "Custom"}
    };
    
    auto it = typeMap.find(type);
    return (it != typeMap.end()) ? it->second : "Unknown";
}

std::string BatchMode::batchJobPriorityToString(BatchJobPriority priority) const {
    static const std::map<BatchJobPriority, std::string> priorityMap = {
        {BatchJobPriority::Low, "Low"},
        {BatchJobPriority::Normal, "Normal"},
        {BatchJobPriority::High, "High"},
        {BatchJobPriority::Critical, "Critical"}
    };
    
    auto it = priorityMap.find(priority);
    return (it != priorityMap.end()) ? it->second : "Unknown";
}

std::string BatchMode::batchJobStatusToString(BatchJobStatus status) const {
    static const std::map<BatchJobStatus, std::string> statusMap = {
        {BatchJobStatus::Pending, "Pending"},
        {BatchJobStatus::Running, "Running"},
        {BatchJobStatus::Completed, "Completed"},
        {BatchJobStatus::Failed, "Failed"},
        {BatchJobStatus::Cancelled, "Cancelled"},
        {BatchJobStatus::Skipped, "Skipped"}
    };
    
    auto it = statusMap.find(status);
    return (it != statusMap.end()) ? it->second : "Unknown";
}

std::string BatchMode::getTimeString(const std::chrono::system_clock::time_point& time) const {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

} // namespace advanced
} // namespace nlm

// Include necessary headers
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <set>
#include <algorithm>