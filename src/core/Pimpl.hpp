#pragma once

#include <memory>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <cassert>

namespace nlm {

// Standard Pimpl implementation template
// Follows the "Schmidt-Builder" pattern for clean separation

template<typename T>
class Pimpl {
public:
    // Default constructor
    Pimpl() : pImpl(new Impl()) {}
    
    // Constructor with arguments (delegate construction)
    template<typename... Args>
    explicit Pimpl(Args&&... args) : pImpl(new Impl(std::forward<Args>(args)...)) {}
    
    // Copy constructor - deleted
    Pimpl(const Pimpl&) = delete;
    Pimpl& operator=(const Pimpl&) = delete;
    
    // Move constructor
    Pimpl(Pimpl&& other) noexcept : pImpl(std::move(other.pImpl)) {}
    
    // Move assignment operator
    Pimpl& operator=(Pimpl&& other) noexcept {
        if (this != &other) {
            pImpl = std::move(other.pImpl);
        }
        return *this;
    }
    
    // Destructor
    ~Pimpl() = default;
    
    // Access to the implementation
    T* get() { return pImpl ? pImpl->get() : nullptr; }
    const T* get() const { return pImpl ? pImpl->get() : nullptr; }
    
    // Access by reference (with bounds checking)
    T& operator*() {
        assert(pImpl && "Pimpl is null");
        assert(pImpl->get() && "Implementation is null");
        return *static_cast<T*>(pImpl->get());
    }
    
    const T& operator*() const {
        assert(pImpl && "Pimpl is null");
        assert(pImpl->get() && "Implementation is null");
        return *static_cast<const T*>(pImpl->get());
    }
    
    // Check if implementation exists
    explicit operator bool() const { return pImpl != nullptr && pImpl->get() != nullptr; }
    
    // Reset implementation
    void reset() { pImpl.reset(); }
    
    // Replace implementation
    template<typename... Args>
    void reset(Args&&... args) {
        pImpl.reset(new Impl(std::forward<Args>(args)...));
    }
    
protected:
    // Internal Impl structure
    struct ImplBase {
        virtual ~ImplBase() = default;
        virtual void* get() = 0;
        virtual const void* get() const = 0;
    };
    
    template<typename U>
    struct ConcreteImpl : public ImplBase {
        U data;
        
        template<typename... Args>
        ConcreteImpl(Args&&... args) : data(std::forward<Args>(args)...) {}
        
        void* get() override { return &data; }
        const void* get() const override { return &data; }
    };
    
    // Type-safe access to implementation
    template<typename U>
    U* getImpl() {
        static_assert(std::is_same_v<U, T>, "Type mismatch in Pimpl access");
        auto* impl = static_cast<ConcreteImpl<U>*>(pImpl.get());
        return impl ? &impl->data : nullptr;
    }
    
    template<typename U>
    const U* getImpl() const {
        static_assert(std::is_same_v<U, T>, "Type mismatch in Pimpl access");
        auto* impl = static_cast<const ConcreteImpl<U>*>(pImpl.get());
        return impl ? &impl->data : nullptr;
    }
    
    std::unique_ptr<ImplBase> pImpl;
};

// Convenience macro for Pimpl implementation
// Use this in .cpp files to define the Pimpl implementation
#define NLM_PIMPL_IMPL(ClassName) \
    struct ClassName::Impl : public nlm::Pimpl<ClassName>::ImplBase { \
        ClassName implementation; \
        Impl() : implementation() {} \
        template<typename... Args> \
        Impl(Args&&... args) : implementation(std::forward<Args>(args)...) {} \
        void* get() override { return &implementation; } \
        const void* get() const override { return &implementation; } \
    }; \
    ClassName::ClassName() : pImpl(new Impl()) {} \
    ClassName::~ClassName() = default; \
    ClassName::ClassName(ClassName&& other) noexcept : pImpl(std::move(other.pImpl)) {} \
    ClassName& ClassName::operator=(ClassName&& other) noexcept { \
        if (this != &other) { \
            pImpl = std::move(other.pImpl); \
        } \
        return *this; \
    }

// Type-safe Pimpl for classes that need multiple implementations
// This is more complex but allows for different implementation types per member function
class TypeSafePimpl {
public:
    TypeSafePimpl() = default;
    ~TypeSafePimpl() = default;
    
    // Delete copy operations
    TypeSafePimpl(const TypeSafePimpl&) = delete;
    TypeSafePimpl& operator=(const TypeSafePimpl&) = delete;
    
    // Move operations
    TypeSafePimpl(TypeSafePimpl&&) noexcept = default;
    TypeSafePimpl& operator=(TypeSafePimpl&&) noexcept = default;
    
    // Set implementation
    template<typename T>
    void setImplementation(std::unique_ptr<T> impl) {
        currentImpl = std::move(impl);
    }
    
    // Get implementation (type-safe)
    template<typename T>
    T* getImplementation() {
        return dynamic_cast<T*>(currentImpl.get());
    }
    
    template<typename T>
    const T* getImplementation() const {
        return dynamic_cast<const T*>(currentImpl.get());
    }
    
    // Check if implementation exists
    bool hasImplementation() const { return currentImpl != nullptr; }
    
    // Reset implementation
    void reset() { currentImpl.reset(); }
    
protected:
    std::unique_ptr<class ImplBase> currentImpl;
};

} // namespace nlm
