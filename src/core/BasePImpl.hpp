#pragma once

#include <memory>
#include <stdexcept>
#include <string>

namespace nlm {

/**
 * Base class for implementing the Pimpl pattern with consistent error handling
 * and null pointer safety
 */
template<typename T>
class PImplBase {
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PImplBase() : pImpl(nullptr) {}
    
    explicit PImplBase(std::unique_ptr<Impl> impl) : pImpl(std::move(impl)) {}
    
    ~PImplBase() = default;
    
    // Disable copying
    PImplBase(const PImplBase&) = delete;
    PImplBase& operator=(const PImplBase&) = delete;
    
    // Enable moving
    PImplBase(PImplBase&& other) noexcept : pImpl(std::move(other.pImpl)) {
        other.pImpl = nullptr;
    }
    
    PImplBase& operator=(PImplBase&& other) noexcept {
        if (this != &other) {
            deletePImpl();
            pImpl = std::move(other.pImpl);
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    void deletePImpl() {
        if (pImpl) {
            delete pImpl.release();
        }
    }
    
    Impl& getImpl() {
        if (!pImpl) {
            throw std::runtime_error("PImpl not initialized - object is in invalid state");
        }
        return *pImpl;
    }
    
    const Impl& getImpl() const {
        if (!pImpl) {
            throw std::runtime_error("PImpl not initialized - object is in invalid state");
        }
        return *pImpl;
    }
    
    bool hasImpl() const {
        return pImpl != nullptr;
    }
};

} // namespace nlm
