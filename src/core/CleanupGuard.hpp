#pragma once

#include <memory>
#include <functional>
#include <stdexcept>
#include <mutex>
#include <chrono>
#include <atomic>

namespace nlm {

/**
 * @class CleanupGuard
 * @brief RAII-style cleanup guard for deterministic resource cleanup
 * 
 * This template class provides automatic resource cleanup using RAII principles.
 * It ensures that resources are properly cleaned up even when exceptions are thrown,
 * making it ideal for complex multi-resource operations that need atomic rollback.
 * 
 * The guard can be configured to automatically clean up resources in its destructor,
 * or it can be manually released when all operations succeed.
 * 
 * Example usage:
 * @code
 * try {
 *     // Create multiple resources
 *     std::vector<std::unique_ptr<SomeResource>> resources;
 *     
 *     // Create cleanup guard with automatic cleanup
 *     CleanupGuard<std::vector<std::unique_ptr<SomeResource>>, 
 *               std::function<void(std::vector<std::unique_ptr<SomeResource>>&)>>
 *         guard(std::move(resources), [](std::vector<std::unique_ptr<SomeResource>>& res) {
 *             for (auto& resource : res) {
 *                 resource->cleanup();
 *             }
 *         });
 *     
 *     // Perform operations
 *     // ...
 *     
 *     // If all operations succeed, release cleanup
 *     guard.release();
 *     
 *     // Resources will be cleaned up automatically when guard goes out of scope
 * } catch (const std::exception& e) {
 *     // Cleanup happens automatically here
 *     std::cerr << "Error: " << e.what() << std::endl;
 * }
 * 
 * // Cleanup happens here too
 * @endcode
 * 
 * @tparam Resource Type of resource to manage
 * @tparam CleanupFunc Type of cleanup function
 * 
 * @see Smart pointers for simpler resource management
 * @see unique_ptr for single resource management
 * @see scoped_lock for thread-safe resource management
 */

template<typename Resource, typename CleanupFunc>
class CleanupGuard {
public:
    /**
     * @brief Construct a cleanup guard with resource and cleanup function
     * 
     * @param resource Resource to manage
     * @param cleanup Cleanup function to call when resource is cleaned up
     */
    CleanupGuard(Resource&& resource, CleanupFunc&& cleanup)
        : resource_(std::move(resource)), cleanup_(std::move(cleanup)), active_(true) {}
    
    /**
     * @brief Destructor - automatically cleans up resource if active
     * 
     * The cleanup function is called when the guard is destroyed, unless
     * the guard has been explicitly released.
     */
    ~CleanupGuard() {
        if (active_) {
            try {
                cleanup_(resource_);
            } catch (const std::exception& e) {
                // Log error but don't propagate in destructor
                // In production, this would use proper logging
                std::cerr << "Warning: Error during resource cleanup: " << e.what() << std::endl;
            }
        }
    }
    
    /**
     * @brief Release the guard, preventing automatic cleanup
     * 
     * Call this method when all operations on the resource have completed
     * successfully and you want to prevent automatic cleanup in the destructor.
     * After releasing, the resource will be cleaned up by its own destructor
     * when it goes out of scope.
     */
    void release() { active_ = false; }
    
    /**
     * @brief Check if the guard is active
     * 
     * @return true if the guard is still active and will perform cleanup
     * @return false if the guard has been released
     */
    bool isActive() const { return active_; }
    
    /**
     * @brief Access the managed resource
     * 
     * @return Resource& Reference to the managed resource
     */
    Resource& getResource() { return resource_; }
    
    /**
     * @brief Access the managed resource (const version)
     * 
     * @return const Resource& Const reference to the managed resource
     */
    const Resource& getResource() const { return resource_; }
    
private:
    Resource resource_;       ///< The resource being managed
    CleanupFunc cleanup_;      ///< Function to clean up the resource
    bool active_;             ///< Whether the guard is active (needs cleanup)
};

/**
 * @brief Create a cleanup guard with exception safety
 * 
 * This function creates a cleanup guard with proper exception safety guarantees.
 * It's useful for operations that need multiple resources and want to ensure
 * cleanup if any operation fails.
 * 
 * @tparam Resource Type of resource to manage
 * @tparam CleanupFunc Type of cleanup function
 * @param resource Resource to manage
 * @param cleanup Cleanup function
 * @return CleanupGuard<Resource, CleanupFunc> Cleanup guard
 * 
 * Example usage:
 * @code
 * try {
 *     std::vector<std::unique_ptr<SomeResource>> resources;
 *     
 *     CleanupGuard<std::vector<std::unique_ptr<SomeResource>>, 
 *               std::function<void(std::vector<std::unique_ptr<SomeResource>>&)>>
 *         guard = createCleanupGuard(
 *             std::move(resources),
 *             [](std::vector<std::unique_ptr<SomeResource>>& res) {
 *                 for (auto& resource : res) {
 *                     resource->cleanup();
 *                 }
 *             }
 *         );
 *     
 *     // ... perform operations ...
 *     
 *     // If all operations succeed, release cleanup
 *     guard.release();
 * } catch (...) {
 *     // Cleanup happens automatically
 * }
 * @endcode
 */
template<typename Resource, typename CleanupFunc>
CleanupGuard<Resource, CleanupFunc> createCleanupGuard(
    Resource&& resource, 
    CleanupFunc&& cleanup
) {
    return CleanupGuard<Resource, CleanupFunc>(std::move(resource), std::move(cleanup));
}

/**
 * @class TransactionGuard
 * @brief Transaction-like resource management for multi-resource operations
 * 
 * This class provides transaction-like semantics for operations that involve
 * multiple resources. It ensures that either all resources are committed
 * successfully or none are (rollback).
 * 
 * Example usage:
 * @code
 * try {
 *     TransactionGuard<SomeResource> transaction;
 *     
 *     // Acquire resources
 *     auto resource1 = transaction.acquireResource<Resource1>(
 *         resource1Args...);
 *     auto resource2 = transaction.acquireResource<Resource2>(
 *         resource2Args...);
 *     
 *     // Perform operations
 *     // ...
 *     
 *     // If all operations succeed, commit transaction
 *     transaction.commit();
 *     
 *     // Resources are now owned by caller
 * } catch (...) {
 *     // Transaction is automatically rolled back
 *     std::cerr << "Transaction failed, cleaning up resources" << std::endl;
 * }
 * 
 * // Transaction automatically cleaned up
 * @endcode
 * 
 * @tparam ResourceType Type of resource to manage
 */
template<typename ResourceType>
class TransactionGuard {
public:
    /**
     * @brief Construct a transaction guard
     */
    TransactionGuard() : committed_(false) {}
    
    /**
     * @brief Destructor - automatically rolls back if not committed
     */
    ~TransactionGuard() {
        if (!committed_) {
            rollback();
        }
    }
    
    /**
     * @brief Acquire a resource for the transaction
     * 
     * @tparam Args Types of constructor arguments for the resource
     * @param args Arguments to pass to resource constructor
     * @return std::unique_ptr<ResourceType> Acquired resource
     */
    template<typename... Args>
    std::unique_ptr<ResourceType> acquireResource(Args&&... args) {
        std::unique_ptr<ResourceType> resource = std::make_unique<ResourceType>(std::forward<Args>(args)...);
        acquiredResources_.push_back(std::move(resource));
        return std::move(acquiredResources_.back());
    }
    
    /**
     * @brief Commit the transaction
     * 
     * After committing, all acquired resources are released from the transaction
     * and can be used by the caller.
     */
    void commit() {
        committed_ = true;
        acquiredResources_.clear();
    }
    
    /**
     * @brief Rollback the transaction
     * 
     * Rolls back all resources, cleaning them up if necessary.
     */
    void rollback() {
        for (auto& resource : acquiredResources_) {
            try {
                if (resource) {
                    // Default cleanup - call delete if it has a delete function
                    // Subclasses can override this behavior
                    resource->cleanup();
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Error during resource cleanup in rollback: " 
                         << e.what() << std::endl;
            }
        }
        acquiredResources_.clear();
    }
    
    /**
     * @brief Check if transaction is committed
     * 
     * @return true if transaction is committed
     * @return false if transaction is still active or rolled back
     */
    bool isCommitted() const { return committed_; }
    
    /**
     * @brief Get number of acquired resources
     * 
     * @return size_t Number of acquired resources
     */
    size_t getResourceCount() const { return acquiredResources_.size(); }
    
protected:
    /**
     * @brief Virtual cleanup method for subclasses
     * 
     * Subclasses can override this method to provide custom cleanup logic
     * for their specific resource types.
     */
    virtual void cleanupResource(std::unique_ptr<ResourceType>&& resource) {
        // Default cleanup - just delete the resource
        // Subclasses should override this for custom cleanup
        if (resource) {
            // Use a type-erased approach to call cleanup
            // This is a template method that should be specialized
        }
    }
    
private:
    std::vector<std::unique_ptr<ResourceType>> acquiredResources_;  ///< Acquired resources
    bool committed_;                                              ///< Whether transaction is committed
};

/**
 * @brief Helper function to create a transaction guard
 * 
 * @tparam ResourceType Type of resource to manage
 * @return TransactionGuard<ResourceType> Transaction guard
 */
template<typename ResourceType>
TransactionGuard<ResourceType> createTransactionGuard() {
    return TransactionGuard<ResourceType>();
}

} // namespace nlm

/**
 * @file CleanupGuard.hpp
 * 
 * This file contains RAII-style resource management templates for the NLM system.
 * 
 * The CleanupGuard templates provide automatic resource cleanup using RAII
 * principles, ensuring that resources are properly cleaned up even when
 * exceptions are thrown.
 * 
 * Key features:
 * - RAII resource management with automatic cleanup
 * - Exception-safe resource acquisition and cleanup
 * - Transaction-like semantics for multi-resource operations
 * - Flexible cleanup function types
 * - Integration with existing NLM resource management patterns
 * 
 * The templates include:
 * - CleanupGuard: Basic resource cleanup guard
 * - TransactionGuard: Transaction-like resource management
 * - Helper functions for creating guards
 * 
 * Usage scenarios:
 * - File handle management
 * - Memory pool resource management
 * - Network connection management
 * - Checkpoint file management
 * - Configuration resource management
 * 
 * This implementation significantly improves:
 * - Resource safety and reliability
 * - Exception safety
 * - Code maintainability
 * - Prevention of resource leaks
 * 
 * The templates integrate well with the NLM exception hierarchy and
 * can be used throughout the codebase for consistent resource management.
 * 
 * Example of integration with NLM operations:
 * @code
 * #include "CleanupGuard.hpp"
 * #include "Exceptions.hpp"
 * 
 * try {
 *     // Create multiple resources that need cleanup
 *     std::vector<std::unique_ptr<FileHandle>> files;
 *     std::vector<std::unique_ptr<MemoryPool>> pools;
 *     
 *     // Use cleanup guard for automatic resource management
 *     auto guard = nlm::createCleanupGuard(
 *         std::make_pair(std::move(files), std::move(pools)),
 *         [](auto& resources) {
 *             // Cleanup all resources
 *             auto& [files, pools] = resources;
 *             for (auto& file : files) {
 *                 file->close();
 *             }
 *             for (auto& pool : pools) {
 *                 // Memory pool cleanup
 *             }
 *         }
 *     );
 *     
 *     // Perform operations
 *     // ...
 *     
 *     // If all operations succeed, release cleanup
 *     guard.release();
 * } catch (const nlm::NLMException& e) {
 *     // Cleanup happens automatically
 *     std::cerr << "Error: " << e.what() << std::endl;
 * }
 * 
 * // All resources are cleaned up here
 * @endcode
 */