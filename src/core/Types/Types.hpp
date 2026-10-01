#ifndef NLM_CORE_H
#define NLM_CORE_H

// Core component interface library
// Provides fundamental types, utilities, and base classes for all NLM components

#pragma once

#include <memory>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <variant>

namespace nlm {
namespace core {

// Forward declarations for internal use
class Logger;
class SimulationClock;

// Core exception hierarchy
class NLMException : public std::exception {
protected:
    std::string message_;
public:
    explicit NLMException(const std::string& message) : message_(message) {}
    virtual ~NLMException() = default;
    virtual const char* what() const noexcept override { return message_.c_str(); }
};

class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message) : NLMException("ConfigurationError: " + message) {}
};

// Type aliases for common types
using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;
using Duration = std::chrono::duration<double>;
using NodeId = uint64_t;
using Weight = double;
using Probability = double;
using TimeStep = double;

// Thread-safe counter
class AtomicCounter {
private:
    std::atomic<uint64_t> counter_;
public:
    AtomicCounter() : counter_(0) {}
    uint64_t increment() { return ++counter_; }
    uint64_t get() const { return counter_; }
    void reset() { counter_ = 0; }
};

// Smart pointer typedefs
template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// Result type for error handling
enum class Result { SUCCESS, FAILURE, PARTIAL, TIMEOUT };

template<typename T>
class ResultWrapper {
public:
    ResultWrapper(Result result, const T& data) : result_(result), data_(data) {}
    ResultWrapper(Result result) : result_(result) {}
    
    bool success() const { return result_ == Result::SUCCESS; }
    bool failure() const { return result_ == Result::FAILURE; }
    bool partial() const { return result_ == Result::PARTIAL; }
    bool timeout() const { return result_ == Result::TIMEOUT; }
    
    const T& data() const { return data_; }
    T& data() { return data_; }
    
private:
    Result result_;
    T data_;
};

} // namespace core
} // namespace nlm

#endif // NLM_CORE_H
