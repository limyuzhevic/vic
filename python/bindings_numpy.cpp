// NumPy integration for enhanced Python API
// Simplified NumPy support for common use cases

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"

// Forward declarations
namespace nlm {
    class Vision;
    class Audio;
}

namespace py = pybind11;
namespace nlm {

/**
 * Helper functions for NumPy integration
 */

// Convert Python list to std::vector<float>
std::vector<float> python_list_to_float_vector(const py::list& list) {
    std::vector<float> result;
    result.reserve(list.size());
    for (size_t i = 0; i < list.size(); ++i) {
        result.push_back(list[i].cast<float>());
    }
    return result;
}

// Convert std::vector<float> to Python list
py::list float_vector_to_python_list(const std::vector<float>& vec) {
    py::list result;
    for (const auto& item : vec) {
        result.append(item);
    }
    return result;
}

// Convert py::array_t to std::vector<float>
std::vector<float> numpy_array_to_float_vector(const py::array_t<float>& array) {
    py::buffer_info info = array.request();
    float* ptr = (float*)info.ptr;
    size_t size = info.size;
    
    std::vector<float> result(ptr, ptr + size);
    return result;
}

// Convert std::vector<float> to py::array_t<float>
py::array_t<float> float_vector_to_numpy_array(const std::vector<float>& vec) {
    py::buffer_info info{
        vec.data(),
        sizeof(float),
        py::format_descriptor<float>::value,
        1,
        {static_cast<ssize_t>(vec.size())},
        {sizeof(float)}
    };
    return py::array_t<float>(info);
}

// Enhanced Vision class with NumPy support
void addNumPyEnhancedBindings(py::module& m) {
    try {
        // Check if numpy is available by trying to import it
        py::module numpy = py::module::import("numpy");
        
        // Enhanced Vision class
        py::class_<Vision, SensoryInput>(m, "Vision")
            .def(py::init<>())
            .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
            
            // Enhanced setData methods that accept NumPy arrays
            .def("setDataNumPy", [](Vision& self, const py::array_t<float>& array) {
                auto data = numpy_array_to_float_vector(array);
                self.setData(data);
            }, py::arg("array"), "Set data from NumPy array")
            
            .def("setDataNumPy1D", [](Vision& self, const py::array_t<float>& array, 
                                     size_t width, size_t height, size_t channels) {
                auto data = numpy_array_to_float_vector(array);
                if (data.size() != width * height * channels) {
                    throw std::runtime_error("NumPy array size doesn't match image dimensions");
                }
                self.setData(data);
            }, py::arg("array"), py::arg("width"), py::arg("height"), py::arg("channels"), "Set data from 1D NumPy array with dimensions")
            
            // Conversion methods to NumPy
            .def("toNumPyArray", [](const Vision& self) {
                return float_vector_to_numpy_array(self.getData());
            }, "Convert data to NumPy array")
            
            // NumPy-specific attributes
            .def_property_readonly("shape", [](const Vision& self) {
                py::tuple result;
                result = py::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
                return result;
            }, "Get array shape (width, height, channels)")
            
            .def_property_readonly("size", [](const Vision& self) {
                return self.getWidth() * self.getHeight() * self.getChannels();
            }, "Get total number of elements");
        
        // Enhanced Audio class
        py::class_<Audio, SensoryInput>(m, "Audio")
            .def(py::init<>())
            .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
            
            // Enhanced setData methods that accept NumPy arrays
            .def("setDataNumPy", [](Audio& self, const py::array_t<float>& array) {
                auto data = numpy_array_to_float_vector(array);
                self.setData(data);
            }, py::arg("array"), "Set data from NumPy array")
            
            // Conversion methods to NumPy
            .def("toNumPyArray", [](const Audio& self) {
                return float_vector_to_numpy_array(self.getData());
            }, "Convert data to NumPy array");
        
        // Convenience functions for NumPy users
        m.def("loadImageFromNumPy", [](const py::array_t<float>& array, size_t width, size_t height, size_t channels) {
            auto data = numpy_array_to_float_vector(array);
            if (data.size() != width * height * channels) {
                throw std::runtime_error("NumPy array size doesn't match image dimensions");
            }
            Vision vision(width, height, channels);
            vision.setData(data);
            return vision;
        }, py::arg("array"), py::arg("width"), py::arg("height"), py::arg("channels"), "Create Vision from NumPy array");
        
        m.def("loadAudioFromNumPy", [](const py::array_t<float>& array, size_t sampleRate) {
            auto data = numpy_array_to_float_vector(array);
            Audio audio(sampleRate, data.size());
            audio.setData(data);
            return audio;
        }, py::arg("array"), py::arg("sampleRate"), "Create Audio from NumPy array");
        
        // Enhanced configuration loading from NumPy
        m.def("loadConfigFromNumPy", [](const py::array_t<double>& configArray) {
            auto config = std::make_shared<Config>();
            auto data = numpy_array_to_float_vector(configArray);
            
            if (data.size() >= 3) {
                config->set("neuron_count", static_cast<int64_t>(data[0]));
                config->set("region_count", static_cast<int64_t>(data[1]));
                config->set("connection_probability", data[2]);
            }
            
            return config;
        }, py::arg("configArray"), "Load configuration from NumPy array");
        
        std::cout << "NumPy enhanced bindings added successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "NumPy not available, using basic bindings: " << e.what() << std::endl;
        // NumPy is not available, continue with basic bindings
    }
}

} // namespace nlm
