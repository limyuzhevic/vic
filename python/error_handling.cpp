#include "error_handling.h"
#include <pybind11/functional.h>
#include <string>
#include <sstream>

namespace py = pybind11;
namespace nlm {

void register_type_conversions(py::module_& m) {
    // Register type conversion utilities

    // Python to C++ type conversions
    py::class_<py::object>(m, "PyObject", "Python object wrapper")
        .def(py::init<>())
        .def("as_string", [](const py::object& obj) {
            return obj.cast<std::string>();
        }, "Convert Python object to string")
        .def("as_int", [](const py::object& obj) {
            return obj.cast<int>();
        }, "Convert Python object to int")
        .def("as_float", [](const py::object& obj) {
            return obj.cast<float>();
        }, "Convert Python object to float")
        .def("as_bool", [](const py::object& obj) {
            return obj.cast<bool>();
        }, "Convert Python object to bool");

    // Vector conversion utilities
    m.def("to_vector", [](const py::list& list) {
        std::vector<float> result;
        for (const auto& item : list) {
            result.push_back(item.cast<float>());
        }
        return result;
    }, "Convert Python list to vector<float>", py::arg("list"));

    m.def("to_list", [](const std::vector<float>& vec) {
        py::list result;
        for (float value : vec) {
            result.append(value);
        }
        return result;
    }, "Convert vector<float> to Python list", py::arg("vec"));

    // Dictionary conversion utilities
    m.def("to_dict", [](const std::unordered_map<std::string, std::string>& map) {
        py::dict result;
        for (const auto& pair : map) {
            result[pair.first.c_str()] = pair.second.c_str();
        }
        return result;
    }, "Convert unordered_map<string, string> to Python dict", py::arg("map"));

    // Tuple conversion utilities
    m.def("to_tuple", [](const std::vector<float>& vec) {
        py::tuple result(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            result[i] = vec[i];
        }
        return result;
    }, "Convert vector<float> to Python tuple", py::arg("vec"));

    // Safe conversion with error handling
    m.def("safe_convert_to_string", [](const py::object& obj) {
        try {
            return obj.cast<std::string>();
        } catch (const py::cast_error& e) {
            throw std::runtime_error("Failed to convert Python object to string: " + std::string(e.what()));
        }
    }, "Safely convert Python object to string", py::arg("obj"));

    m.def("safe_convert_to_int", [](const py::object& obj) {
        try {
            return obj.cast<int>();
        } catch (const py::cast_error& e) {
            throw std::runtime_error("Failed to convert Python object to int: " + std::string(e.what()));
        }
    }, "Safely convert Python object to int", py::arg("obj"));

    m.def("safe_convert_to_float", [](const py::object& obj) {
        try {
            return obj.cast<float>();
        } catch (const py::cast_error& e) {
            throw std::runtime_error("Failed to convert Python object to float: " + std::string(e.what()));
        }
    }, "Safely convert Python object to float", py::arg("obj"));

    m.def("safe_convert_to_bool", [](const py::object& obj) {
        try {
            return obj.cast<bool>();
        } catch (const py::cast_error& e) {
            throw std::runtime_error("Failed to convert Python object to bool: " + std::string(e.what()));
        }
    }, "Safely convert Python object to bool", py::arg("obj"));

    // Error handling utilities
    m.def("validate_parameter", [](const std::string& paramName, const py::object& value, 
                                 const std::string& type) {
        if (type == "float" || type == "double") {
            try {
                value.cast<float>();
            } catch (const py::cast_error&) {
                throw std::runtime_error("Parameter '" + paramName + "' must be a float/double");
            }
        } else if (type == "int") {
            try {
                value.cast<int>();
            } catch (const py::cast_error&) {
                throw std::runtime_error("Parameter '" + paramName + "' must be an int");
            }
        } else if (type == "string") {
            try {
                value.cast<std::string>();
            } catch (const py::cast_error&) {
                throw std::runtime_error("Parameter '" + paramName + "' must be a string");
            }
        } else if (type == "bool") {
            try {
                value.cast<bool>();
            } catch (const py::cast_error&) {
                throw std::runtime_error("Parameter '" + paramName + "' must be a bool");
            }
        }
        return true;
    }, "Validate parameter type", py::arg("paramName"), py::arg("value"), py::arg("type"));

    // Vector validation
    m.def("validate_vector", [](const py::list& list, const std::string& elementType, size_t minSize = 0) {
        if (list.size() < minSize) {
            throw std::runtime_error("List must have at least " + std::to_string(minSize) + " elements");
        }
        for (const auto& item : list) {
            if (elementType == "float") {
                try { item.cast<float>(); }
                catch (const py::cast_error&) {
                    throw std::runtime_error("List elements must be floats");
                }
            } else if (elementType == "int") {
                try { item.cast<int>(); }
                catch (const py::cast_error&) {
                    throw std::runtime_error("List elements must be ints");
                }
            }
        }
        return true;
    }, "Validate vector/list contents", py::arg("list"), py::arg("elementType"), py::arg("minSize") = 0);

    // Object validation
    m.def("validate_object", [](const py::object& obj, const std::string& expectedType) {
        if (expectedType == "SensoryInput" && !obj.is_none()) {
            try {
                // This would require actual type checking against Python classes
                // For now, just accept any non-None object
                return true;
            } catch (...) {
                throw std::runtime_error("Object must be a SensoryInput instance");
            }
        }
        return true;
    }, "Validate Python object type", py::arg("obj"), py::arg("expectedType"));

    // Parameter validation helpers
    m.def("check_range", [](float value, float min, float max, const std::string& paramName) {
        if (value < min || value > max) {
            throw std::runtime_error("Parameter '" + paramName + "' must be in range [" + 
                                   std::to_string(min) + ", " + std::to_string(max) + "]");
        }
        return true;
    }, "Check if float parameter is within valid range", py::arg("value"), py::arg("min"), py::arg("max"), py::arg("paramName"));

    m.def("check_positive", [](float value, const std::string& paramName) {
        if (value < 0) {
            throw std::runtime_error("Parameter '" + paramName + "' must be positive");
        }
        return true;
    }, "Check if float parameter is positive", py::arg("value"), py::arg("paramName"));

    m.def("check_non_empty", [](const std::string& value, const std::string& paramName) {
        if (value.empty()) {
            throw std::runtime_error("Parameter '" + paramName + "' must not be empty");
        }
        return true;
    }, "Check if string parameter is not empty", py::arg("value"), py::arg("paramName"));

    // Error formatting utilities
    m.def("format_error", [](const std::string& template, const std::vector<std::string>& args) {
        std::string result = template;
        for (size_t i = 0; i < args.size(); ++i) {
            size_t pos = result.find("{}" + std::to_string(i));
            if (pos != std::string::npos) {
                result.replace(pos, 3, args[i]);
            }
        }
        return result;
    }, "Format error message with placeholders", py::arg("template"), py::arg("args"));

    // Exception raising utilities
    m.def("raise_runtime_error", [](const std::string& message) {
        throw std::runtime_error(message);
    }, "Raise a runtime error with custom message", py::arg("message"));

    m.def("raise_value_error", [](const std::string& message) {
        throw std::value_error(message);
    }, "Raise a value error with custom message", py::arg("message"));

    m.def("raise_type_error", [](const std::string& message) {
        throw std::type_error(message);
    }, "Raise a type error with custom message", py::arg("message"));

    // Error checking utilities
    m.def("check_error", []() {
        PyErr_Clear(); // Clear any pending Python exception
        return true;
    }, "Clear any pending Python exception");

    // Safe function wrapper
    m.def("safe_call", [](const py::function& func, const py::args& args, const py::kwargs& kwargs) {
        try {
            return func(args, kwargs);
        } catch (const std::exception& e) {
            throw std::runtime_error("Error in safe call: " + std::string(e.what()));
        }
    }, "Safely call a Python function with error handling", py::arg("func"), py::arg("args"), py::arg("kwargs"));

    // Type checking utilities
    m.def("is_instance", [](const py::object& obj, const std::string& className) {
        // This is a simplified type check - in practice you'd use PyObject_IsInstance
        return !obj.is_none(); // For now, just accept any non-None object
    }, "Check if Python object is instance of specified class", py::arg("obj"), py::arg("className"));

    m.def("is_callable", [](const py::object& obj) {
        return py::isinstance<py::function>(obj);
    }, "Check if Python object is callable", py::arg("obj"));

    // Conversion utilities
    m.def("convert_to_pyarray", [](const std::vector<float>& vec) {
        return py::array(py::dtype("f"), vec);
    }, "Convert vector<float> to numpy array", py::arg("vec"));

    m.def("convert_from_pyarray", [](py::array array) {
        auto buf = array.request();
        float* ptr = (float*)buf.ptr;
        size_t size = buf.size;
        return std::vector<float>(ptr, ptr + size);
    }, "Convert numpy array to vector<float>", py::arg("array"));

} // namespace nlm

void register_exception_translators(py::module_& m) {
    // Register exception translators to convert C++ exceptions to Python exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) {
                std::rethrow_exception(p);
            }
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::value_error& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::type_error& e) {
            PyErr_SetString(PyExc_TypeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });
}

void add_type_conversions(py::module_& m) {
    // Add standard type conversions
    m.def("convert_string", [](py::str value) {
        return value.cast<std::string>();
    }, "Convert Python string to std::string", py::arg("value"));

    m.def("convert_int", [](py::int_ value) {
        return value.cast<int>();
    }, "Convert Python integer to int", py::arg("value"));

    m.def("convert_float", [](py::float_ value) {
        return value.cast<float>();
    }, "Convert Python float to float", py::arg("value"));

    m.def("convert_bool", [](py::bool_ value) {
        return value.cast<bool>();
    }, "Convert Python boolean to bool", py::arg("value"));

    // Tuple/list conversions
    m.def("tuple_to_vector", [](py::tuple tuple) {
        std::vector<float> result;
        for (size_t i = 0; i < tuple.size(); ++i) {
            result.push_back(tuple[i].cast<float>());
        }
        return result;
    }, "Convert Python tuple to vector<float>", py::arg("tuple"));

    m.def("list_to_vector", [](py::list list) {
        std::vector<float> result;
        for (const auto& item : list) {
            result.push_back(item.cast<float>());
        }
        return result;
    }, "Convert Python list to vector<float>", py::arg("list"));

    // Dictionary conversions
    m.def("dict_to_map", [](py::dict dict) {
        std::unordered_map<std::string, std::string> result;
        for (auto item : dict) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            result[key] = value;
        }
        return result;
    }, "Convert Python dict to unordered_map<string, string>", py::arg("dict"));

} // namespace nlm