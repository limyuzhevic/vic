#pragma once

#include <pybind11/pybind11.h>
#include <memory>
#include <string>
#include <type_traits>
#include <stdexcept>

namespace py = pybind11;

namespace nlm {

// Base exception class for Python bindings
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

// Specific exception types
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException("Config Error: " + message) {}
};

class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) : NLMException("Brain Error: " + message) {}
};

class SimpleWorldException : public NLMException {
public:
    explicit SimpleWorldException(const std::string& message) : NLMException("SimpleWorld Error: " + message) {}
};

// Generic ResourceManager template for custom context managers
template<typename T>
class ResourceManager {
private:
    std::shared_ptr<T> resource_;
    bool acquired_ = false;
    std::string name_;
    
public:
    ResourceManager(const std::string& name = "Resource") : name_(name) {}
    
    ResourceManager(std::shared_ptr<T> resource, const std::string& name = "Resource")
        : resource_(std::move(resource)), acquired_(true), name_(name) {}
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    ResourceManager(ResourceManager&& other) noexcept
        : resource_(std::move(other.resource_)), acquired_(other.acquired_), name_(std::move(other.name_)) {
        other.acquired_ = false;
    }
    
    ResourceManager& operator=(ResourceManager&& other) noexcept {
        if (this != &other) {
            resource_ = std::move(other.resource_);
            acquired_ = other.acquired_;
            name_ = std::move(other.name_);
            other.acquired_ = false;
        }
        return *this;
    }
    
    ~ResourceManager() {
        if (acquired_) {
            release();
        }
    }
    
    std::shared_ptr<T> acquire(std::shared_ptr<T> resource) {
        if (acquired_) {
            throw NLMException("Resource already acquired");
        }
        resource_ = std::move(resource);
        acquired_ = true;
        return resource_;
    }
    
    std::shared_ptr<T> release() {
        if (!acquired_) {
            throw NLMException("Resource not acquired");
        }
        acquired_ = false;
        auto temp = resource_;
        resource_.reset();
        return temp;
    }
    
    bool isAcquired() const { return acquired_; }
    
    std::shared_ptr<T> getResource() const { return resource_; }
    
    T* operator->() { return resource_.get(); }
    
    const T* operator->() const { return resource_.get(); }
    
    T& operator*() { return *resource_; }
    
    const T& operator*() const { return *resource_; }
};