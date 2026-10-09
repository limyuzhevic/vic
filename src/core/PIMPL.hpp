// ============================================================================
// PIMPL (Pointer-to-Implementation) Template
// ============================================================================

#pragma once

#include <memory>

namespace nlm {

// Forward declaration of PIMPL implementation interface
#define PIMPL_FORWARD(ClassName)                                             \
    struct ClassName##_Impl;                                                 \
    ClassName##_Impl* pImpl

// PIMPL implementation base class template
#define PIMPL_IMPLEMENTATION(ClassName)                                       \
    struct ClassName##_Impl {                                                \
    public:                                                                   \
        ClassName##_Impl();                                                  \
        ~ClassName##_Impl();                                                 \
                                                                                  \
    private:                                                                  \
        ClassName##_Impl(const ClassName##_Impl&) = delete;                   \
        ClassName##_Impl& operator=(const ClassName##_Impl&) = delete;        \
    };                                                                       \
                                                                              \
    ClassName::ClassName() : pImpl(new ClassName##_Impl) {}                 \
    ClassName::~ClassName() { delete pImpl; }                                \

// PIMPL implementation class with custom constructor
#define PIMPL_IMPLEMENTATION_CUSTOM(ClassName, ConstructorArgs)                \
    struct ClassName##_Impl {                                                \
    public:                                                                   \
        ClassName##_Impl(ConstructorArgs);                                    \
        ~ClassName##_Impl();                                                 \
                                                                                  \
    private:                                                                  \
        ClassName##_Impl(const ClassName##_Impl&) = delete;                   \
        ClassName##_Impl& operator=(const ClassName##_Impl&) = delete;        \
    };                                                                       \
                                                                              \
    ClassName::ClassName(ConstructorArgs) : pImpl(new ClassName##_Impl(args)) {}

// PIMPL implementation with default constructor
#define PIMPL_IMPLEMENTATION_DEFAULT(ClassName)                               \
    struct ClassName##_Impl {                                                \
    public:                                                                   \
        ClassName##_Impl();                                                  \
        ~ClassName##_Impl();                                                 \
                                                                                  \
    private:                                                                  \
        ClassName##_Impl(const ClassName##_Impl&) = delete;                   \
        ClassName##_Impl& operator=(const ClassName##_Impl&) = delete;        \
    };                                                                       \
                                                                              \
    ClassName::ClassName() : pImpl(new ClassName##_Impl) {}                 \
    ClassName::~ClassName() noexcept = default

// PIMPL implementation with move semantics
#define PIMPL_IMPLEMENTATION_MOVE(ClassName)                                  \
    struct ClassName##_Impl {                                                \
    public:                                                                   \
        ClassName##_Impl();                                                  \
        ~ClassName##_Impl();                                                 \
        ClassName##_Impl(ClassName##_Impl&&) noexcept = default;             \
        ClassName##_Impl& operator=(ClassName##_Impl&&) noexcept = default;   \
                                                                                  \
    private:                                                                  \
        ClassName##_Impl(const ClassName##_Impl&) = delete;                   \
        ClassName##_Impl& operator=(const ClassName##_Impl&) = delete;        \
    };                                                                       \
                                                                              \
    ClassName::ClassName() : pImpl(new ClassName##_Impl) {}                 \
    ClassName::~ClassName() noexcept = default;                             \
    ClassName::ClassName(ClassName&& other) noexcept : pImpl(other.pImpl) {   \
        other.pImpl = nullptr;                                               \
    }                                                                       \
    ClassName& operator=(ClassName&& other) noexcept {                      \
        if (this != &other) {                                                \
            delete pImpl;                                                    \
            pImpl = other.pImpl;                                             \
            other.pImpl = nullptr;                                           \
        }                                                                   \
        return *this;                                                        \
    }

// PIMPL cleanup macro
#define PIMPL_CLEANUP(ClassName)                                              \
    inline void cleanup_##ClassName(ClassName& obj) {                        \
        delete obj.pImpl;                                                     \
        obj.pImpl = nullptr;                                                 \
    }

// Usage example:
// class MyClass {
// public:
//     PIMPL_FORWARD(MyClass)
//     
//     MyClass();
//     ~MyClass();
//     // ... other methods ...
// };                                            \
//                                              \
// PIMPL_IMPLEMENTATION_DEFAULT(MyClass)

} // namespace nlm

// ============================================================================
// End of PIMPL Template
// ============================================================================
