#include "ErrorHandling/ErrorHierarchy.hpp"
#include <pybind11/pybind11.h>

namespace nlm {
    // Implementation of ErrorHierarchy functions
    std::string ErrorHierarchy::getPerformanceReport() const {
        return metrics_.report();
    }
    
    std::string ErrorHierarchy::getDetailedPerformanceReport() const {
        return metrics_.report();
    }
}
