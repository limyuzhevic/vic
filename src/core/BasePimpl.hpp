#pragma once

#include <memory>
#include <utility>

namespace nlm {

// Base Pimpl template for forward-declared implementation classes
// Handles the most common pImpl pattern: BasePimpl<Class, Impl>
template<typename T, typename Impl>
class BasePimpl {
public:
    // Default constructor creates implementation
    BasePimpl() : pImpl(std::make_unique<Impl>()) {}
    
    // Constructor with arguments (forwarded to Impl)
    template<typename... Args>
    BasePimpl(Args&&... args) : pImpl(std::make_unique<Impl>(std::forward<Args>(args)...)) {}
    
    // Move constructor
    BasePimpl(BasePimpl&& other) noexcept : pImpl(std::move(other.pImpl)) {}
    
    // Move assignment operator
    BasePimpl& operator=(BasePimpl&& other) noexcept {
        if (this != &other) {
            pImpl = std::move(other.pImpl);
        }
        return *this;
    }
    
    // Delete copy operations
    BasePimpl(const BasePimpl&) = delete;
    BasePimpl& operator=(const BasePimpl&) = delete;
    
    // Access to implementation
    Impl* operator->() { return pImpl.get(); }
    const Impl* operator->() const { return pImpl.get(); }
    
    // Get raw pointer (for pImpl member)
    Impl* get() { return pImpl.get(); }
    const Impl* get() const { return pImpl.get(); }
    
    // Unique pointer access
    std::unique_ptr<Impl>& getPtr() { return pImpl; }
    const std::unique_ptr<Impl>& getPtr() const { return pImpl; }
    
private:
    std::unique_ptr<Impl> pImpl;
};

// Base Pimpl template with unique_ptr for Config-like classes
// Handles: BasePimpl<Config, std::unique_ptr<Impl>>
template<typename T, typename Impl>
class BasePimplUnique {
public:
    // Default constructor creates implementation
    BasePimplUnique() : pImpl(std::make_unique<Impl>()) {}
    
    // Constructor with arguments (forwarded to Impl)
    template<typename... Args>
    BasePimplUnique(Args&&... args) : pImpl(std::make_unique<Impl>(std::forward<Args>(args)...)) {}
    
    // Move constructor
    BasePimplUnique(BasePimplUnique&& other) noexcept : pImpl(std::move(other.pImpl)) {}
    
    // Move assignment operator
    BasePimplUnique& operator=(BasePimplUnique&& other) noexcept {
        if (this != &other) {
            pImpl = std::move(other.pImpl);
        }
        return *this;
    }
    
    // Delete copy operations
    BasePimplUnique(const BasePimplUnique&) = delete;
    BasePimplUnique& operator=(const BasePimplUnique&) = delete;
    
    // Access to implementation
    Impl* operator->() { return pImpl.get(); }
    const Impl* operator->() const { return pImpl.get(); }
    
    // Get raw pointer (for pImpl member)
    Impl* get() { return pImpl.get(); }
    const Impl* get() const { return pImpl.get(); }
    
    // Unique pointer access
    std::unique_ptr<Impl>& getPtr() { return pImpl; }
    const std::unique_ptr<Impl>& getPtr() const { return pImpl; }
    
private:
    std::unique_ptr<Impl> pImpl;
};

// Simple pImpl pattern for legacy classes
// Handles: Impl* pImpl (used by Dopamine, other neuromodulators)
template<typename T, typename Impl>
class SimplePimpl {
public:
    // Default constructor creates implementation
    SimplePimpl() : pImpl(new Impl()) {}
    
    // Constructor with arguments (forwarded to Impl)
    template<typename... Args>
    SimplePimpl(Args&&... args) : pImpl(new Impl(std::forward<Args>(args)...)) {}
    
    // Move constructor
    SimplePimpl(SimplePimpl&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    // Move assignment operator
    SimplePimpl& operator=(SimplePimpl&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    // Delete copy operations
    SimplePimpl(const SimplePimpl&) = delete;
    SimplePimpl& operator=(const SimplePimpl&) = delete;
    
    // Destructor
    ~SimplePimpl() {
        delete pImpl;
    }
    
    // Access to implementation
    Impl* operator->() { return pImpl; }
    const Impl* operator->() const { return pImpl; }
    
    // Get raw pointer (for pImpl member)
    Impl* get() { return pImpl; }
    const Impl* get() const { return pImpl; }
    
    // Set raw pointer
    void set(Impl* impl) {
        delete pImpl;
        pImpl = impl;
    }
    
    // Reset to nullptr
    void reset() {
        delete pImpl;
        pImpl = nullptr;
    }
    
private:
    Impl* pImpl;
};

// Helper functions for common initialization patterns
namespace pimpl_helpers {
    // Common constructor with initializer list for impl classes
    template<typename T, typename Impl, typename... Args>
    void initializeImpl(SimplePimpl<T, Impl>& pimpl, Args&&... args) {
        static_cast<T*>(this)->SimplePimpl<T, Impl>::SimplePimpl(std::forward<Args>(args)...);
    }
    
    // Common move constructor helper
    template<typename T>
    void moveConstructHelper(T& self, T&& other) {
        self.pImpl = std::move(other.pImpl);
    }
    
    // Common move assignment helper
    template<typename T>
    T& moveAssignHelper(T& self, T&& other) {
        if (&self != &other) {
            delete self.pImpl;
            self.pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return self;
    }
}

} // namespace nlm
