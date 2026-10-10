// TestFixture.hpp - Base class for test fixtures
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <set>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <exception>

// Forward declarations for NLM types
namespace nlm {
    class Config;
    class Brain;
    class Neuron;
    class Synapse;
    class PlasticityRule;
    class Neuromodulator;
    class MemorySystem;
    class SensoryInput;
    class MotorSystem;
    class DevelopmentSystem;
    class EventSystem;
}

// Memory management utilities
class MemoryLeakDetector {
public:
    struct AllocationRecord {
        void* address;
        size_t size;
        std::string allocationSite;
        std::chrono::steady_clock::time_point timestamp;
        std::string allocationThread;
        
        AllocationRecord(void* addr, size_t sz, const std::string& site)
            : address(addr), size(sz), allocationSite(site), 
              timestamp(std::chrono::steady_clock::now()),
              allocationThread(getCurrentThreadId()) {}
    
    static MemoryLeakDetector& getInstance() {
        static MemoryLeakDetector instance;
        return instance;
    }
    
    void recordAllocation(void* ptr, size_t size, const std::string& site) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.push_back(AllocationRecord(ptr, size, site));
    }
    
    void recordDeallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = allocations_.begin(); it != allocations_.end(); ++it) {
            if (it->address == ptr) {
                deallocations_.push_back(*it);
                allocations_.erase(it);
                return;
            }
        }
    }
    
    std::vector<AllocationRecord> getActiveAllocations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_;
    }
    
    size_t getTotalAllocations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.size();
    }
    
    size_t getTotalDeallocations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return deallocations_.size();
    }
    
    size_t getLeakedBytes() const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t total = 0;
        for (const auto& alloc : allocations_) {
            total += alloc.size;
        }
        return total;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
        deallocations_.clear();
    }
    
private:
    MemoryLeakDetector() = default;
    
    std::string getCurrentThreadId() {
        auto id = std::this_thread::get_id();
        std::stringstream ss;
        ss << id;
        return ss.str();
    }
    
    mutable std::mutex mutex_;
    std::vector<AllocationRecord> allocations_;
    std::vector<AllocationRecord> deallocations_;
};

// Smart pointer testing utilities
class SmartPointerTester {
public:
    static bool isNull(const std::nullptr_t&) { return true; }
    
    template<typename T>
    static bool isNull(std::shared_ptr<T>& ptr) { return !ptr; }
    
    template<typename T>
    static bool isNull(const std::shared_ptr<T>& ptr) { return !ptr; }
    
    template<typename T>
    static bool isNull(std::unique_ptr<T>& ptr) { return !ptr; }
    
    template<typename T>
    static bool isNull(const std::unique_ptr<T>& ptr) { return !ptr; }
    
    template<typename T, typename U>
    static bool isSame(std::shared_ptr<T> ptr1, std::shared_ptr<U> ptr2) {
        return ptr1.get() == ptr2.get();
    }
    
    template<typename T>
    static size_t useCount(std::shared_ptr<T>& ptr) { return ptr.use_count(); }
    
    template<typename T>
    static size_t useCount(const std::shared_ptr<T>& ptr) { return ptr.use_count(); }
};

// Circular reference detector
class CircularReferenceDetector {
public:
    struct Node {
        std::string id;
        std::vector<std::string> children;
        bool visited = false;
        bool recursionStack = false;
        
        Node(const std::string& nodeId) : id(nodeId) {}
    }
    
    static bool detectCircularReference(const std::vector<Node>& graph) {
        std::unordered_map<std::string, Node> nodes;
        for (const auto& node : graph) {
            nodes[node.id] = node;
        }
        
        std::stack<std::string> stack;
        for (const auto& pair : nodes) {
            if (hasCycle(nodes, pair.first, stack)) {
                return true;
            }
        }
        return false;
    }
    
private:
    static bool hasCycle(std::unordered_map<std::string, Node>& nodes, 
                        const std::string& current, std::stack<std::string>& stack) {
        auto& node = nodes[current];
        
        if (node.recursionStack) {
            return true;
        }
        
        if (node.visited) {
            return false;
        }
        
        node.visited = true;
        node.recursionStack = true;
        stack.push(current);
        
        for (const auto& childId : node.children) {
            if (hasCycle(nodes, childId, stack)) {
                return true;
            }
        }
        
        node.recursionStack = false;
        stack.pop();
        return false;
    }
};

// Resource cleanup tracker
class ResourceCleanupTracker {
public:
    struct Resource {
        std::string name;
        bool allocated;
        std::chrono::steady_clock::time_point allocatedTime;
        std::chrono::steady_clock::time_point cleanupTime;
        
        Resource(const std::string& resourceName) 
            : name(resourceName), allocated(false) {}
    };
    
    static ResourceCleanupTracker& getInstance() {
        static ResourceCleanupTracker instance;
        return instance;
    }
    
    void allocateResource(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        resources_[name] = Resource(name);
        resources_[name].allocated = true;
        resources_[name].allocatedTime = std::chrono::steady_clock::now();
    }
    
    void cleanupResource(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = resources_.find(name);
        if (it != resources_.end()) {
            it->second.cleanupTime = std::chrono::steady_clock::now();
            it->second.allocated = false;
        }
    }
    
    bool isResourceCleaned(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = resources_.find(name);
        if (it != resources_.end()) {
            return !it->second.allocated;
        }
        return true;
    }
    
    std::vector<Resource> getUncleanedResources() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Resource> result;
        for (const auto& pair : resources_) {
            if (pair.second.allocated) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        resources_.clear();
    }
    
private:
    ResourceCleanupTracker() = default;
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, Resource> resources_;
};

// Random number generation for testing
class TestRandomGenerator {
public:
    static TestRandomGenerator& getInstance() {
        static TestRandomGenerator instance;
        return instance;
    }
    
    double generateDouble(double min = 0.0, double max = 1.0) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(rng_);
    }
    
    int generateInt(int min = 0, int max = 100) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng_);
    }
    
    bool generateBool(double probability = 0.5) {
        return generateDouble() < probability;
    }
    
    std::vector<int> generateRandomVector(size_t size, int min = 0, int max = 100) {
        std::vector<int> result(size);
        for (auto& val : result) {
            val = generateInt(min, max);
        }
        return result;
    }
    
    std::vector<double> generateRandomDoubleVector(size_t size, double min = 0.0, double max = 1.0) {
        std::vector<double> result(size);
        for (auto& val : result) {
            val = generateDouble(min, max);
        }
        return result;
    }
    
    void seed(unsigned int seed) {
        rng_.seed(seed);
    }
    
    unsigned int getSeed() const {
        return static_cast<unsigned int>(rng_.seed());
    }
    
private:
    TestRandomGenerator() = default;
    
    std::mt19937 rng_;
};

// Performance measurement utilities
class PerformanceTimer {
public:
    PerformanceTimer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsedMs() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }
    
    double elapsedSeconds() const {
        return elapsedMs() / 1000.0;
    }
    
    void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

// Test fixture base class
class TestFixture {
public:
    TestFixture() {
        // Setup test environment
        setupTestEnvironment();
    }
    
    virtual ~TestFixture() {
        // Cleanup test environment
        cleanupTestEnvironment();
    }
    
    virtual void setUp() {
        // Override in derived classes
    }
    
    virtual void tearDown() {
        // Override in derived classes
    }
    
protected:
    // Configuration management
    std::shared_ptr<nlm::Config> createTestConfig() {
        auto config = std::make_shared<nlm::Config>();
        // Set reasonable defaults for testing
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
        return config;
    }
    
    // Random utilities
    double randomDouble(double min = 0.0, double max = 1.0) {
        return TestRandomGenerator::getInstance().generateDouble(min, max);
    }
    
    int randomInt(int min = 0, int max = 100) {
        return TestRandomGenerator::getInstance().generateInt(min, max);
    }
    
    bool randomBool(double probability = 0.5) {
        return TestRandomGenerator::getInstance().generateBool(probability);
    }
    
    // Simulation utilities
    void runSimulationSteps(nlm::Brain& brain, nlm::SimulationStep steps) {
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
    }
    
    // Memory utilities
    template<typename T>
    std::shared_ptr<T> makeSharedPtrWithLeakTracking(T* ptr) {
        MemoryLeakDetector::getInstance().recordAllocation(
            ptr, sizeof(T), "make_shared_ptr_with_tracking");
        std::shared_ptr<T> result(ptr);
        result->addCleanupHook([ptr]() {
            MemoryLeakDetector::getInstance().recordDeallocation(ptr);
        });
        return result;
    }
    
    // Resource utilities
    void trackResourceAllocation(const std::string& name) {
        ResourceCleanupTracker::getInstance().allocateResource(name);
    }
    
    void trackResourceCleanup(const std::string& name) {
        ResourceCleanupTracker::getInstance().cleanupResource(name);
    }
    
    bool isResourceCleaned(const std::string& name) const {
        return ResourceCleanupTracker::getInstance().isResourceCleaned(name);
    }
    
    // Validation utilities
    void assertNotNull(const void* ptr, const std::string& message = "Pointer is null") {
        if (ptr == nullptr) {
            throw std::runtime_error(message);
        }
    }
    
    void assertGreaterThan(double value, double threshold, const std::string& message = "Value is not greater than threshold") {
        if (value <= threshold) {
            throw std::runtime_error(message);
        }
    }
    
    void assertLessThan(double value, double threshold, const std::string& message = "Value is not less than threshold") {
        if (value >= threshold) {
            throw std::runtime_error(message);
        }
    }
    
    void assertEqual(double value1, double value2, double epsilon = 1e-9, const std::string& message = "Values are not equal") {
        if (std::abs(value1 - value2) > epsilon) {
            throw std::runtime_error(message);
        }
    }
    
    void assertInRange(double value, double min, double max, const std::string& message = "Value is out of range") {
        if (value < min || value > max) {
            throw std::runtime_error(message);
        }
    }
    
    // File utilities
    bool fileExists(const std::string& path) const {
        std::ifstream file(path);
        return file.good();
    }
    
    std::string readFile(const std::string& path) const {
        std::ifstream file(path);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    void writeFile(const std::string& path, const std::string& content) const {
        std::ofstream file(path);
        if (!file) {
            throw std::runtime_error("Cannot write to file: " + path);
        }
        file << content;
    }
    
private:
    void setupTestEnvironment() {
        // Initialize test random generator with fixed seed for reproducibility
        TestRandomGenerator::getInstance().seed(42);
    }
    
    void cleanupTestEnvironment() {
        // Cleanup any remaining resources
        ResourceCleanupTracker::getInstance().clear();
        MemoryLeakDetector::getInstance().clear();
    }
};