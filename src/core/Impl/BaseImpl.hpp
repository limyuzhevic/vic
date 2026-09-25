// Base PIMPL implementation using smart pointers for better memory management
#pragma once

#include <memory>
#include <stdexcept>

namespace nlm {

// Base implementation structure that uses smart pointers
template<typename T>
struct BaseImpl {
    std::unique_ptr<T> pImpl;
    
    BaseImpl() : pImpl(std::make_unique<T>()) {}
    explicit BaseImpl(std::unique_ptr<T> impl) : pImpl(std::move(impl)) {}
    
    // Disable copying
    BaseImpl(const BaseImpl&) = delete;
    BaseImpl& operator=(const BaseImpl&) = delete;
    
    // Enable moving
    BaseImpl(BaseImpl&& other) noexcept : pImpl(std::move(other.pImpl)) {}
    BaseImpl& operator=(BaseImpl&& other) noexcept {
        if (this != &other) {
            pImpl = std::move(other.pImpl);
        }
        return *this;
    }
    
    ~BaseImpl() = default;
    
    // Access to the implementation
    T* get() { return pImpl.get(); }
    const T* get() const { return pImpl.get(); }
    
    // Dereference operators for convenience
    T& operator*() { return *pImpl; }
    const T& operator*() const { return *pImpl; }
    
    T* operator->() { return pImpl.get(); }
    const T* operator->() const { return pImpl.get(); }
};

} // namespace nlm
