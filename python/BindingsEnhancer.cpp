#include "PythonBindingsEnhancer.hpp"
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>
#include <pybind11/eval.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

class PythonBindingsEnhancer {
public:
    PythonBindingsEnhancer() = default;
    
    // Enhanced Python binding utilities
    static void addAdvancedSensoryAPI(py::module& m);
    static void addVisualizationAPI(py::module& m);
    static void addAdvancedTrainingAPI(py::module& m);
    static void addExpertCommandLineTools(py::module& m);
    static void addPerformanceMonitoring(py::module& m);
    static void addMemoryManagementTools(py::module& m);
    static void addSerializationTools(py::module& m);
    static void addDebugAndAnalysisTools(py::module& m);
    
private:
    static void registerExceptionHandlers(py::module& m);
    static void registerCustomTypes(py::module& m);
    static void registerConversionHelpers(py::module& m);
};

} // namespace nlm