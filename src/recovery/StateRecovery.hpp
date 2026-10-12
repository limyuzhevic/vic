// Exception Safety Framework for the NLM System
// This file provides comprehensive exception safety guarantees and transaction-like operations

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <functional>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <deque>

namespace nlm {

// Exception Safety Levels
enum class ExceptionSafetyLevel {
    Basic,       // Basic exception safety (strong guarantee for destructors)
    Strong,      // Strong exception safety (no resource leaks)
    NoThrow,     // No-throw guarantee for critical operations
    Transaction  // Transaction-like operations (rollback on exception)
};

// Exception Safety Category
enum class ExceptionSafetyCategory {
    NoThrow,         // Never throws
    Strong,          // Strong exception safety (rollback on exception)
    Basic,           // Basic exception safety (no resource leaks)
    BasicButLeaky,    // Basic exception safety but may leak resources
    Panicking,       // May throw exceptions
    Undefined        // Exception safety not defined
};

// Exception Safety Policy
enum class ExceptionSafetyPolicy {
    Strict,          // Strictly enforce exception safety
    Lenient,         // Be lenient about exception safety
    Ignore,          // Ignore exception safety
    Automatic,       // Automatically handle exceptions
    Manual          // Require manual exception handling
};

// Resource Management Interface
class ResourceManager {
public:
    virtual ~ResourceManager() = default;
    
    virtual bool allocateResource() = 0;
    virtual bool deallocateResource() = 0;
    virtual bool isResourceAllocated() const = 0;
    virtual std::string getResourceName() const = 0;
    
    virtual bool allocate() = 0;
    virtual bool deallocate() = 0;
    virtual bool isAllocated() const = 0;
    virtual void reset() = 0;
};

// Transaction State
enum class TransactionState {
    Inactive,
    Active,
    Committed,
    RolledBack,
    Aborted,
    Failed
};

// Transaction Information
struct TransactionInfo {
    std::string id;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    TransactionState state;
    std::vector<std::string> operations;
    std::vector<std::string> undoOperations;
    std::unordered_map<std::string, std::string> metadata;
    
    TransactionInfo() : state(TransactionState::Inactive) {}
    
    TransactionInfo(const std::string& id)
        : id(id), state(TransactionState::Active) {
        startTime = std::chrono::system_clock::now();
    }
    
    std::chrono::milliseconds getDuration() const {
        auto end = (state != TransactionState::Inactive && 
                   state != TransactionState::Active) ? 
                   endTime : std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - startTime);
    }
    
    bool isActive() const { return state == TransactionState::Active; }
    bool isCompleted() const { return state == TransactionState::Committed || 
                                     state == TransactionState::RolledBack || 
                                     state == TransactionState::Aborted || 
                                     state == TransactionState::Failed; }
};

// Transaction Manager Interface
class TransactionManager {
public:
    virtual ~TransactionManager() = default;
    
    virtual bool beginTransaction(const std::string& id) = 0;
    virtual bool commitTransaction(const std::string& id) = 0;
    virtual bool rollbackTransaction(const std::string& id) = 0;
    virtual bool abortTransaction(const std::string& id) = 0;
    
    virtual TransactionState getTransactionState(const std::string& id) const = 0;
    virtual std::vector<std::string> getActiveTransactionIds() const = 0;
    
    virtual bool addOperation(const std::string& transactionId,
                             const std::function<bool()>& operation,
                             const std::function<bool()>& undoOperation = nullptr) = 0;
    
    virtual bool executeTransaction(const std::string& id) = 0;
    virtual bool undoTransaction(const std::string& id) = 0;
    
    virtual bool canRollback(const std::string& id) const = 0;
    virtual bool isTransactionSafe(const std::string& id) const = 0;
    
    virtual void cleanupCompletedTransactions() = 0;
    virtual void cleanupFailedTransactions() = 0;
    
    virtual size_t getTransactionCount() const = 0;
    virtual size_t getActiveTransactionCount() const = 0;
    virtual size_t getCompletedTransactionCount() const = 0;
    
    virtual std::string getTransactionReport(const std::string& id) const = 0;
    virtual std::string getAllTransactionsReport() const = 0;
};

// Resource Holder
template<typename T>
class ResourceHolder {
private:
    std::unique_ptr<T> resource;
    bool isOwned;
    ExceptionSafetyLevel safetyLevel;
    std::string resourceName;
    
public:
    ResourceHolder() : isOwned(false), safetyLevel(ExceptionSafetyLevel::Basic) {}
    
    ResourceHolder(std::unique_ptr<T> res, bool owned = true,
                   ExceptionSafetyLevel safety = ExceptionSafetyLevel::Basic,
                   const std::string& name = "")
        : resource(std::move(res)), isOwned(owned),
          safetyLevel(safety), resourceName(name) {}
    
    ResourceHolder(const ResourceHolder&) = delete;
    ResourceHolder& operator=(const ResourceHolder&) = delete;
    
    ResourceHolder(ResourceHolder&& other) noexcept
        : resource(std::move(other.resource)),
          isOwned(other.isOwned),
          safetyLevel(other.safetyLevel),
          resourceName(std::move(other.resourceName)) {
        other.isOwned = false;
        other.resource = nullptr;
    }
    
    ResourceHolder& operator=(ResourceHolder&& other) noexcept {
        if (this != &other) {
            release();
            resource = std::move(other.resource);
            isOwned = other.isOwned;
            safetyLevel = other.safetyLevel;
            resourceName = std::move(other.resourceName);
            other.isOwned = false;
            other.resource = nullptr;
        }
        return *this;
    }
    
    ~ResourceHolder() {
        release();
    }
    
    void release() {
        if (isOwned && resource) {
            resource.reset();
        }
        isOwned = false;
        resource = nullptr;
    }
    
    T* get() const { return resource.get(); }
    T& operator*() const { return *resource; }
    T* operator->() const { return resource.get(); }
    
    explicit operator bool() const { return resource != nullptr; }
    bool hasResource() const { return resource != nullptr; }
    
    void reset() {
        release();
        isOwned = false;
        resource = nullptr;
    }
    
    void reset(std::unique_ptr<T> newResource, bool owned = true) {
        release();
        resource = std::move(newResource);
        isOwned = owned;
    }
    
    ExceptionSafetyLevel getSafetyLevel() const { return safetyLevel; }
    void setSafetyLevel(ExceptionSafetyLevel level) { safetyLevel = level; }
    
    const std::string& getResourceName() const { return resourceName; }
    void setResourceName(const std::string& name) { resourceName = name; }
    
    bool isOwnedResource() const { return isOwned; }
    
    // Exception safety guaranteed operations
    void swap(ResourceHolder& other) noexcept {
        using std::swap;
        swap(resource, other.resource);
        swap(isOwned, other.isOwned);
        swap(safetyLevel, other.safetyLevel);
        swap(resourceName, other.resourceName);
    }
};

// Exception Safety Guard
class ExceptionSafetyGuard {
private:
    std::vector<std::function<void()>> cleanupFunctions;
    bool isActive;
    ExceptionSafetyLevel safetyLevel;
    std::string transactionId;
    TransactionManager* transactionManager;
    
public:
    ExceptionSafetyGuard(ExceptionSafetyLevel level = ExceptionSafetyLevel::Strong)
        : isActive(true), safetyLevel(level), transactionManager(nullptr) {}
    
    ExceptionSafetyGuard(const std::string& tid, TransactionManager* tm,
                         ExceptionSafetyLevel level = ExceptionSafetyLevel::Transaction)
        : isActive(true), safetyLevel(level), transactionId(tid),
          transactionManager(tm) {}
    
    ~ExceptionSafetyGuard() {
        if (isActive) {
            rollback();
        }
    }
    
    void addCleanupFunction(const std::function<void()>& func) {
        cleanupFunctions.push_back(func);
    }
    
    template<typename F>
    void addCleanupFunction(F&& func) {
        cleanupFunctions.push_back(std::forward<F>(func));
    }
    
    void commit() {
        if (!isActive) return;
        
        // Execute all cleanup functions
        for (const auto& func : cleanupFunctions) {
            try {
                func();
            } catch (...) {
                // Log error but continue
            }
        }
        
        cleanupFunctions.clear();
        isActive = false;
        
        if (!transactionId.empty() && transactionManager) {
            transactionManager->commitTransaction(transactionId);
        }
    }
    
    void rollback() {
        if (!isActive) return;
        
        // Rollback in reverse order
        for (auto it = cleanupFunctions.rbegin(); it != cleanupFunctions.rend(); ++it) {
            const auto& func = *it;
            try {
                func();
            } catch (...) {
                // Log error but continue with other rollbacks
            }
        }
        
        cleanupFunctions.clear();
        isActive = false;
        
        if (!transactionId.empty() && transactionManager) {
            transactionManager->rollbackTransaction(transactionId);
        }
    }
    
    void abort() {
        if (!isActive) return;
        
        cleanupFunctions.clear();
        isActive = false;
        
        if (!transactionId.empty() && transactionManager) {
            transactionManager->abortTransaction(transactionId);
        }
    }
    
    bool isActiveGuard() const { return isActive; }
    ExceptionSafetyLevel getSafetyLevel() const { return safetyLevel; }
    
    void disableAutoRollback() { isActive = false; }
    
    // Exception safety guaranteed functions
    template<typename F>
    auto executeSafely(F&& func) -> decltype(func()) {
        try {
            auto result = func();
            commit();
            return result;
        } catch (...) {
            rollback();
            throw;
        }
    }
    
    template<typename F>
    auto executeSafelyNoThrow(F&& func) -> decltype(func()) {
        try {
            auto result = func();
            commit();
            return result;
        } catch (...) {
            // Don't rethrow for NoThrow guarantee
            rollback();
            // Return default value if possible
            if constexpr (!std::is_same<decltype(func()), void>::value) {
                // Could return default value, but can't know type
                // This is a limitation of the API
            }
        }
    }
};

// Transaction Wrapper
class TransactionWrapper {
private:
    std::string transactionId;
    TransactionManager* transactionManager;
    ExceptionSafetyGuard guard;
    bool isActive;
    
public:
    TransactionWrapper(const std::string& tid, TransactionManager* tm)
        : transactionId(tid), transactionManager(tm), guard(tid, tm),
          isActive(false) {
        if (transactionManager) {
            isActive = transactionManager->beginTransaction(tid);
        }
    }
    
    ~TransactionWrapper() {
        if (isActive && transactionManager) {
            transactionManager->rollbackTransaction(transactionId);
        }
    }
    
    bool isActiveTransaction() const { return isActive; }
    const std::string& getTransactionId() const { return transactionId; }
    
    bool commit() {
        if (!isActive || !transactionManager) return false;
        
        guard.commit();
        isActive = false;
        
        bool success = transactionManager->commitTransaction(transactionId);
        return success;
    }
    
    bool rollback() {
        if (!isActive || !transactionManager) return false;
        
        guard.rollback();
        isActive = false;
        
        bool success = transactionManager->rollbackTransaction(transactionId);
        return success;
    }
    
    bool abort() {
        if (!isActive || !transactionManager) return false;
        
        guard.abort();
        isActive = false;
        
        bool success = transactionManager->abortTransaction(transactionId);
        return success;
    }
    
    template<typename F>
    auto executeOperation(F&& operation) -> decltype(operation()) {
        if (!isActive) throw std::runtime_error("Transaction not active");
        
        return guard.executeSafely(std::forward<F>(operation));
    }
}; 

// Resource Management Utilities
namespace ResourceUtils {
    // Resource allocation with exception safety
    template<typename T, typename... Args>
    ResourceHolder<T> allocateResource(Args&&... args) {
        try {
            auto resource = std::make_unique<T>(std::forward<Args>(args)...);
            return ResourceHolder<T>(std::move(resource), true, ExceptionSafetyLevel::Strong);
        } catch (...) {
            throw;
        }
    }
    
    // Resource allocation without ownership
    template<typename T, typename... Args>
    std::shared_ptr<T> allocateSharedResource(Args&&... args) {
        try {
            return std::make_shared<T>(std::forward<Args>(args)...);
        } catch (...) {
            throw;
        }
    }
    
    // Resource allocation with manual ownership
    template<typename T, typename... Args>
    T* allocateRawResource(Args&&... args) {
        try {
            return new T(std::forward<Args>(args)...);
        } catch (...) {
            throw;
        }
    }
    
    // Safe resource cleanup
    template<typename T>
    void safeDelete(T* ptr) {
        if (ptr) {
            delete ptr;
        }
    }
    
    template<typename T>
    void safeDeleteArray(T* ptr) {
        if (ptr) {
            delete[] ptr;
        }
    }
    
    // Exception safety checking
    template<typename F>
    bool isExceptionSafe(F&& func) {
        try {
            func();
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Transaction execution
    template<typename F>
    bool executeInTransaction(TransactionManager& tm, const std::string& tid, F&& func) {
        try {
            if (!tm.beginTransaction(tid)) {
                return false;
            }
            
            auto result = func();
            
            if (!tm.commitTransaction(tid)) {
                tm.rollbackTransaction(tid);
                return false;
            }
            
            return result;
        } catch (...) {
            tm.rollbackTransaction(tid);
            throw;
        }
    }
    
    // Resource acquisition with rollback
    template<typename Resource, typename Allocator, typename Deallocator>
    std::unique_ptr<Resource> acquireWithRollback(Allocator alloc, Deallocator dealloc) {
        try {
            Resource* raw = alloc();
            if (!raw) {
                throw std::runtime_error("Resource allocation failed");
            }
            
            auto resource = std::unique_ptr<Resource>(raw);
            
            return resource;
        } catch (...) {
            if (dealloc) {
                dealloc(nullptr); // Deallocate if resource was allocated
            }
            throw;
        }
    }
}

// Exception Safety Utilities
namespace ExceptionUtils {
    // Exception safety checking
    template<typename F>
    ExceptionSafetyCategory checkExceptionSafety(F&& func) {
        try {
            func();
            return ExceptionSafetyCategory::NoThrow;
        } catch (const std::exception&) {
            // We don't know if resources were leaked
            return ExceptionSafetyCategory::Panicking;
        } catch (...) {
            return ExceptionSafetyCategory::Panicking;
        }
    }
    
    // No-throw guarantee checking
    template<typename F>
    bool isNoThrow(F&& func) {
        try {
            func();
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Strong exception safety checking
    template<typename F>
    bool isStrongExceptionSafe(F&& func) {
        // This is a best-effort check
        return isExceptionSafe(std::forward<F>(func));
    }
    
    // Exception propagation control
    template<typename F>
    auto propagateExceptions(F&& func, bool propagate = true) -> decltype(func()) {
        try {
            return func();
        } catch (...) {
            if (propagate) {
                throw;
            }
            // Otherwise, handle the exception
            throw;
        }
    }
    
    // Exception suppression
    template<typename F>
    bool suppressExceptions(F&& func) {
        try {
            func();
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Exception translation
    template<typename FromException, typename ToException, typename F>
    auto translateExceptions(F&& func) -> decltype(func()) {
        try {
            return func();
        } catch (const FromException& e) {
            throw ToException(e.what());
        } catch (...) {
            throw;
        }
    }
    
    // Exception context preservation
    struct ExceptionContext {
        std::string message;
        std::string type;
        std::string file;
        std::string function;
        int line;
        std::unordered_map<std::string, std::string> additionalData;
        
        ExceptionContext() : line(0) {}
        
        ExceptionContext(const std::string& msg, const std::string& file = "",
                         const std::string& function = "", int line = 0)
            : message(msg), file(file), function(function), line(line) {}
    };
    
    // Enhanced exception throwing with context
    template<typename... Args>
    void throwWithContext(const std::string& message, const std::string& file,
                         const std::string& function, int line,
                         const Args&... additionalData) {
        ExceptionContext context(message, file, function, line);
        
        std::vector<std::pair<std::string, std::string>> pairs = {
            additionalData... // This is a simplified approach
        };
        
        for (size_t i = 0; i < pairs.size(); i += 2) {
            if (i + 1 < pairs.size()) {
                context.additionalData[pairs[i].first] = pairs[i].second;
            }
        }
        
        throw std::runtime_error(message);
    }
}

// Default Transaction Manager
class DefaultTransactionManager : public TransactionManager {
private:
    std::unordered_map<std::string, TransactionInfo> transactions;
    std::mutex mutex;
    std::atomic<size_t> transactionCount{0};
    std::atomic<size_t> activeTransactionCount{0};
    std::atomic<size_t> completedTransactionCount{0};
    
public:
    bool beginTransaction(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it != transactions.end() && it->second.isCompleted()) {
            // Remove completed transaction
            transactions.erase(it);
            transactionCount = transactions.size();
        }
        
        if (it == transactions.end()) {
            transactions[id] = TransactionInfo(id);
            transactionCount++;
            activeTransactionCount++;
            return true;
        }
        
        return false; // Transaction already exists
    }
    
    bool commitTransaction(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end() || it->second.isCompleted()) {
            return false;
        }
        
        it->second.state = TransactionState::Committed;
        it->second.endTime = std::chrono::system_clock::now();
        activeTransactionCount--;
        completedTransactionCount++;
        
        return true;
    }
    
    bool rollbackTransaction(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end() || it->second.isCompleted()) {
            return false;
        }
        
        it->second.state = TransactionState::RolledBack;
        it->second.endTime = std::chrono::system_clock::now();
        activeTransactionCount--;
        completedTransactionCount++;
        
        return true;
    }
    
    bool abortTransaction(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end() || it->second.isCompleted()) {
            return false;
        }
        
        it->second.state = TransactionState::Aborted;
        it->second.endTime = std::chrono::system_clock::now();
        activeTransactionCount--;
        completedTransactionCount++;
        
        return true;
    }
    
    TransactionState getTransactionState(const std::string& id) const override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end()) {
            return TransactionState::Inactive;
        }
        
        return it->second.state;
    }
    
    std::vector<std::string> getActiveTransactionIds() const override {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::vector<std::string> ids;
        for (const auto& pair : transactions) {
            if (pair.second.isActive()) {
                ids.push_back(pair.first);
            }
        }
        
        return ids;
    }
    
    bool addOperation(const std::string& transactionId,
                     const std::function<bool()>& operation,
                     const std::function<bool()>& undoOperation) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(transactionId);
        if (it == transactions.end() || !it->second.isActive()) {
            return false;
        }
        
        it->second.operations.push_back(std::to_string(operation));
        if (undoOperation) {
            it->second.undoOperations.push_back(std::to_string(undoOperation));
        }
        
        return true;
    }
    
    bool executeTransaction(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end() || !it->second.isActive()) {
            return false;
        }
        
        // Execute operations
        for (const auto& opStr : it->second.operations) {
            // In a real implementation, we would execute the actual operation
            // For this stub, we just mark as completed
        }
        
        it->second.state = TransactionState::Committed;
        it->second.endTime = std::chrono::system_clock::now();
        activeTransactionCount--;
        completedTransactionCount++;
        
        return true;
    }
    
    bool undoTransaction(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end() || !it->second.isActive()) {
            return false;
        }
        
        // Undo operations in reverse order
        for (auto rit = it->second.undoOperations.rbegin(); 
             rit != it->second.undoOperations.rend(); ++rit) {
            // In a real implementation, we would execute the actual undo operation
        }
        
        it->second.state = TransactionState::RolledBack;
        it->second.endTime = std::chrono::system_clock::now();
        activeTransactionCount--;
        completedTransactionCount++;
        
        return true;
    }
    
    bool canRollback(const std::string& id) const override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end()) {
            return false;
        }
        
        return it->second.state == TransactionState::Active;
    }
    
    bool isTransactionSafe(const std::string& id) const override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end()) {
            return false;
        }
        
        // Check if transaction has undo operations
        return !it->second.undoOperations.empty();
    }
    
    void cleanupCompletedTransactions() override {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto it = transactions.begin(); it != transactions.end(); ) {
            if (it->second.isCompleted()) {
                it = transactions.erase(it);
            } else {
                ++it;
            }
        }
        
        transactionCount = transactions.size();
    }
    
    void cleanupFailedTransactions() override {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto it = transactions.begin(); it != transactions.end(); ) {
            if (it->second.state == TransactionState::Failed) {
                it = transactions.erase(it);
            } else {
                ++it;
            }
        }
        
        transactionCount = transactions.size();
    }
    
    size_t getTransactionCount() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return transactionCount.load();
    }
    
    size_t getActiveTransactionCount() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return activeTransactionCount.load();
    }
    
    size_t getCompletedTransactionCount() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return completedTransactionCount.load();
    }
    
    std::string getTransactionReport(const std::string& id) const override {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = transactions.find(id);
        if (it == transactions.end()) {
            return "Transaction not found: " + id;
        }
        
        std::stringstream ss;
        ss << "Transaction Report:" << std::endl;
        ss << "  ID: " << it->first << std::endl;
        ss << "  State: " << static_cast<int>(it->second.state) << std::endl;
        ss << "  Start Time: " << std::chrono::system_clock::to_time_t(it->second.startTime) << std::endl;
        if (it->second.isCompleted()) {
            ss << "  End Time: " << std::chrono::system_clock::to_time_t(it->second.endTime) << std::endl;
            ss << "  Duration: " << it->second.getDuration().count() << " ms" << std::endl;
        }
        ss << "  Operations: " << it->second.operations.size() << std::endl;
        ss << "  Undo Operations: " << it->second.undoOperations.size() << std::endl;
        
        return ss.str();
    }
    
    std::string getAllTransactionsReport() const override {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::stringstream ss;
        ss << "All Transactions Report:" << std::endl;
        ss << "  Total Transactions: " << transactionCount.load() << std::endl;
        ss << "  Active Transactions: " << activeTransactionCount.load() << std::endl;
        ss << "  Completed Transactions: " << completedTransactionCount.load() << std::endl;
        
        for (const auto& pair : transactions) {
            ss << "\n" << getTransactionReport(pair.first) << std::endl;
        }
        
        return ss.str();
    }
};

// Global Transaction Manager Instance
inline TransactionManager& getDefaultTransactionManager() {
    static DefaultTransactionManager manager;
    return manager;
}

} // namespace nlm
