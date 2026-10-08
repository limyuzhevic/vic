#include "ErrorHandling.hpp"
#include <chrono>
#include <thread>
#include <sstream>
#include <iostream>

namespace nlm {

// Global error handler initialization
int ErrorHandler::logLevel_ = 0;
bool ErrorHandler::verbose_ = false;
bool ErrorHandler::throwOnError_ = false;

// Template implementations for Result
template<typename T>
Result<T> operator|(const Result<T>& result, const std::function<T(Result<T>)>& func) {
    if (result.isSuccess()) {
        return func(result);
    } else {
        return Result<T>(result.status(), result.errorMessage());
    }
}

} // namespace nlm
