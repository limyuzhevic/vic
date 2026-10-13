#pragma once

namespace nlm {

/**
 * PimplMoveOperations - A template that provides standardized move constructor
 * and assignment operator implementations for Pimpl-based classes.
 * 
 * This template addresses R1.1 from the analysis, providing a consistent
 * and optimized approach to move operations for all Pimpl-based classes.
 * 
 * Usage:
 * class MyClass {
 * public:
 *     // ... other declarations ...
 *     // Disable copying
 *     MyClass(const MyClass&) = delete;
 *     MyClass& operator=(const MyClass&) = delete;
 *     
 *     // Enable moving using the template
 *     PimplMoveOperations<MyClass>;
 *     // ... rest of class definition ...
 * };
 */

template <typename Derived>
class PimplMoveOperations {
public:
    // Move constructor
    PimplMoveOperations(Derived&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    // Move assignment operator
    Derived& operator=(Derived&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return static_cast<Derived&>(*this);
    }
    
protected:
    // Protected constructor - derived classes must explicitly call it
    PimplMoveOperations() = default;
    
private:
    // The pImpl pointer is accessed by derived classes
    typename Derived::Impl* pImpl;
};

} // namespace nlm
