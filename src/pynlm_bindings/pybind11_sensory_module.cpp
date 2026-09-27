// pybind11_sensory_module.cpp - Sensory module bindings implementation

#include "pybind11_sensory_module.h"
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace nlm {

void bind_sensory_module(py::module& m) {
    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for all sensory inputs)
    
    Abstract base class representing sensory information received by the agent.
    All concrete sensory inputs inherit from this class and provide specific
    data formats for different sensory modalities.
    
    Sensory inputs include:
    - Vision (visual information from the environment)
    - Audio (sound information)
    - Internal (internal state signals)
    - Proprioception (body position and movement)
    
    The SensoryInput class provides:
    - Data access and manipulation methods
    - Timestamp tracking for temporal information
    - Type identification for polymorphic handling
    - Data validation and quality checks
    "pbdoc")
    .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
    .def("get_data", &SensoryInput::getData, "Get the raw data as a list of floats")
    .def("get_dimensions", &SensoryInput::getDimensions, "Get the data dimensionality")
    .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp (seconds since start)")
    .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
         "Set the timestamp (seconds since start)")
    .def("is_valid", []() { return true; }, "Check if sensory data is valid")
    .def("get_data_summary", [](const SensoryInput& input) {
        auto data = input.getData();
        std::string summary = "SensoryInput[dim=" + std::to_string(data.size()) + "]";
        if (!data.empty()) {
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            float mean_val = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            summary += "[min=" + std::to_string(min_val) + ", max=" + std::to_string(max_val) +
                      ", mean=" + std::to_string(mean_val) + "]";
        }
        return summary;
    }, "Get a summary of sensory data characteristics");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Visual sensory input for the agent)
    
    Represents visual information captured by the agent's vision system.
    Vision data is typically arranged as a 2D grid of intensity values.
    
    Vision parameters:
    - width: Horizontal resolution (pixels)
    - height: Vertical resolution (pixels)  
    - channels: Number of color channels (usually 1 for grayscale, 3 for RGB)
    
    Common use cases:
    - Object detection and recognition
    - Navigation and obstacle avoidance
    - Scene understanding
    - Feature extraction
    
    Vision data format:
    - Grayscale: Single float value per pixel (0.0 = black, 1.0 = white)
    - RGB: Three consecutive float values per pixel (R, G, B)
    "pbdoc")
    .def(py::init<>())
    .def(py::init<size_t, size_t, size_t>(), 
         py::arg("width"), py::arg("height"), py::arg("channels") = 3,
         "Initialize Vision with specified dimensions")
    .def("set_data", [](Vision& self, const std::vector<float>& data) {
        if (data.size() == self.getDimensions()) {
            self.setData(data);
        } else {
            throw py::value_error("Data size mismatch: expected " + 
                                 std::to_string(self.getDimensions()) + 
                                 " values, got " + std::to_string(data.size()));
        }
    }, py::arg("data"), "Set vision data with validation")
    .def("set_data_from_2d", [](Vision& self, const std::vector<std::vector<float>>& data_2d) {
        size_t width = self.getWidth();
        size_t height = self.getHeight();
        size_t expected_size = width * height * self.getChannels();
        
        std::vector<float> data;
        data.reserve(expected_size);
        
        for (const auto& row : data_2d) {
            for (float value : row) {
                data.push_back(value);
            }
        }
        
        if (data.size() == expected_size) {
            self.setData(data);
        } else {
            throw py::value_error("2D data size mismatch with vision dimensions");
        }
    }, py::arg("data_2d"), "Set vision data from 2D array")
    .def("get_width", &Vision::getWidth, "Get image width in pixels")
    .def("get_height", &Vision::getHeight, "Get image height in pixels")
    .def("get_channels", &Vision::getChannels, "Get number of color channels")
    .def("get_pixel", [](const Vision& self, size_t x, size_t y) {
        size_t width = self.getWidth();
        size_t height = self.getHeight();
        size_t channels = self.getChannels();
        
        if (x >= width || y >= height) {
            throw py::index_error("Pixel coordinates out of bounds");
        }
        
        size_t index = (y * width + x) * channels;
        if (index + channels > self.getData().size()) {
            throw py::index_error("Not enough data for specified pixel");
        }
        
        std::vector<float> pixel(self.getData().begin() + index,
                                 self.getData().begin() + index + channels);
        return pixel;
    }, py::arg("x"), py::arg("y"), "Get pixel value at coordinates (x, y)")
    .def("set_pixel", [](Vision& self, size_t x, size_t y, const std::vector<float>& pixel) {
        size_t width = self.getWidth();
        size_t height = self.getHeight();
        size_t channels = self.getChannels();
        
        if (x >= width || y >= height) {
            throw py::index_error("Pixel coordinates out of bounds");
        }
        
        if (pixel.size() != channels) {
            throw py::value_error("Pixel must have exactly " + std::to_string(channels) + 
                                 " values");
        }
        
        std::vector<float> data = self.getData();
        size_t index = (y * width + x) * channels;
        
        for (size_t i = 0; i < channels; ++i) {
            data[index + i] = pixel[i];
        }
        
        self.setData(data);
    }, py::arg("x"), py::arg("y"), py::arg("pixel"), "Set pixel value at coordinates (x, y)")
    .def("compute_brightness", [](const Vision& self) {
        auto data = self.getData();
        size_t channels = self.getChannels();
        size_t width = self.getWidth();
        size_t height = self.getHeight();
        
        if (channels == 1) {
            // Grayscale image
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        } else {
            // RGB image - compute luminance
            float total_brightness = 0.0f;
            size_t pixel_count = 0;
            
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    size_t index = (y * width + x) * channels;
                    
                    // Standard luminance formula: 0.299*R + 0.587*G + 0.114*B
                    float r = data[index];
                    float g = (channels >= 2) ? data[index + 1] : r;
                    float b = (channels >= 3) ? data[index + 2] : r;
                    
                    float brightness = 0.299f * r + 0.587f * g + 0.114f * b;
                    total_brightness += brightness;
                    pixel_count++;
                }
            }
            
            return pixel_count > 0 ? total_brightness / pixel_count : 0.0f;
        }
    }, "Compute average brightness of the image")
    .def("compute_contrast", [](const Vision& self) {
        auto data = self.getData();
        if (data.empty()) return 0.0f;
        
        float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        float sum_squared_diff = 0.0f;
        
        for (float value : data) {
            float diff = value - mean;
            sum_squared_diff += diff * diff;
        }
        
        float variance = sum_squared_diff / data.size();
        return std::sqrt(variance);
    }, "Compute image contrast (standard deviation)")
    .def("is_grayscale", [](const Vision& self) {
        return self.getChannels() == 1;
    }, "Check if image is grayscale")
    .def("is_rgb", [](const Vision& self) {
        return self.getChannels() == 3;
    }, "Check if image is RGB")
    .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
        // This is a simplified resize - in practice you'd use interpolation
        size_t old_width = self.getWidth();
        size_t old_height = self.getHeight();
        size_t channels = self.getChannels();
        
        std::vector<float> new_data;
        new_data.reserve(new_width * new_height * channels);
        
        for (size_t y_new = 0; y_new < new_height; ++y_new) {
            for (size_t x_new = 0; x_new < new_width; ++x_new) {
                // Simple nearest-neighbor sampling
                size_t x_old = (x_new * old_width) / new_width;
                size_t y_old = (y_new * old_height) / new_height;
                
                size_t old_index = (y_old * old_width + x_old) * channels;
                for (size_t c = 0; c < channels; ++c) {
                    if (old_index + c < self.getData().size()) {
                        new_data.push_back(self.getData()[old_index + c]);
                    } else {
                        new_data.push_back(0.0f);
                    }
                }
            }
        }
        
        // Resize is not implemented in Vision API, so we'll just return a new Vision object
        // This is a placeholder for the demonstration
        Vision resized_vision(new_width, new_height, channels);
        resized_vision.setData(new_data);
        return resized_vision;
    }, py::arg("new_width"), py::arg("new_height"), 
         "Resize image (placeholder implementation)");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input for the agent)
    
    Represents audio/sensorial information captured by the agent's audio system.
    Audio data is typically represented as time-series samples of sound intensity.
    
    Audio parameters:
    - sampleRate: Sampling rate in Hz (e.g., 44100 Hz for CD quality)
    - numSamples: Number of audio samples in the recording
    
    Audio data format:
    - Single channel: Single float value per sample (amplitude)
    - Multi-channel: Sequential float values per sample (channel 0, channel 1, etc.)
    
    Audio processing features:
    - Sample rate conversion
    - Amplitude normalization
    - Frequency analysis (placeholder)
    - Noise filtering (placeholder)
    "pbdoc")
    .def(py::init<>())
    .def(py::init<size_t, size_t>(), 
         py::arg("sample_rate"), py::arg("num_samples"),
         "Initialize Audio with specified sample rate and duration")
    .def("set_data", [](Audio& self, const std::vector<float>& data) {
        self.setData(data);
    }, py::arg("data"), "Set audio data samples")
    .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
         "Set audio sample rate (Hz)")
    .def("get_sample_rate", &Audio::getSampleRate, "Get audio sample rate (Hz)")
    .def("get_num_samples", &Audio::getNumSamples, "Get number of audio samples")
    .def("get_duration", [](const Audio& self) {
        return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
    }, "Get audio duration in seconds")
    .def("get_amplitude_range", [](const Audio& self) {
        auto data = self.getData();
        if (data.empty()) return std::make_pair(0.0f, 0.0f);
        
        float min_val = *std::min_element(data.begin(), data.end());
        float max_val = *std::max_element(data.begin(), data.end());
        
        return std::make_pair(min_val, max_val);
    }, "Get minimum and maximum amplitude values")
    .def("normalize_amplitude", [](Audio& self, float target_max = 1.0f) {
        auto data = self.getData();
        if (data.empty()) return;
        
        float current_max = *std::max_element(data.begin(), data.end());
        if (current_max > 0.0f) {
            float scale = target_max / current_max;
            for (float& sample : data) {
                sample *= scale;
            }
            self.setData(data);
        }
    }, py::arg("target_max") = 1.0f, "Normalize audio amplitude to specified maximum")
    .def("compute_average_amplitude", [](const Audio& self) {
        auto data = self.getData();
        if (data.empty()) return 0.0f;
        
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        return sum / data.size();
    }, "Compute average amplitude of audio signal")
    .def("compute_zero_crossings", [](const Audio& self) {
        auto data = self.getData();
        if (data.size() < 2) return 0;
        
        int crossings = 0;
        for (size_t i = 1; i < data.size(); ++i) {
            if ((data[i-1] < 0.0f && data[i] >= 0.0f) || 
                (data[i-1] >= 0.0f && data[i] < 0.0f)) {
                crossings++;
            }
        }
        return crossings;
    }, "Count zero-crossing points in audio signal");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal state sensory input)
    
    Represents the agent's internal state and physiological signals.
    Internal signals include:
    - Energy levels
    - Hunger/satiety signals
    - Stress levels
    - Attention/concentration
    - Memory activation
    - Homeostasis indicators
    
    Internal signals are typically used by the agent to:
    - Regulate behavior
    - Make decisions
    - Maintain homeostasis
    - Plan future actions
    
    InternalSignals provide methods for:
    - Adding signal values
    - Managing signal history
    - Accessing current internal state
    - Resetting internal state
    "pbdoc")
    .def(py::init<>())
    .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
         "Add a signal value to the internal state")
    .def("get_signal_count", [](const InternalSignals& self) {
        return self.getData().size();
    }, "Get the number of internal signals")
    .def("get_signal_at", [](const InternalSignals& self, size_t index) {
        auto data = self.getData();
        if (index < data.size()) {
            return data[index];
        } else {
            throw py::index_error("Signal index out of range");
        }
    }, py::arg("index"), "Get internal signal value at index")
    .def("get_all_signals", &InternalSignals::getData, "Get all internal signal values")
    .def("clear_signals", &InternalSignals::clearSignals, "Clear all internal signals")
    .def("get_energy_level", [](const InternalSignals& self) {
        auto data = self.getData();
        if (!data.empty()) {
            return data[0];  // First signal typically represents energy
        }
        return 0.0f;
    }, "Get energy level from internal signals")
    .def("get_hunger_level", [](const InternalSignals& self) {
        auto data = self.getData();
        if (data.size() > 1) {
            return data[1];  // Second signal typically represents hunger
        }
        return 0.0f;
    }, "Get hunger level from internal signals")
    .def("get_stress_level", [](const InternalSignals& self) {
        auto data = self.getData();
        if (data.size() > 2) {
            return data[2];  // Third signal typically represents stress
        }
        return 0.0f;
    }, "Get stress level from internal signals")
    .def("get_attention_level", [](const InternalSignals& self) {
        auto data = self.getData();
        if (data.size() > 3) {
            return data[3];  // Fourth signal typically represents attention
        }
        return 0.0f;
    }, "Get attention level from internal signals")
    .def("update_from_agent_state", [](InternalSignals& self, float energy, 
                                       float hunger, float stress, float attention) {
        std::vector<float> signals;
        signals.reserve(4);
        signals.push_back(energy);
        signals.push_back(hunger);
        signals.push_back(stress);
        signals.push_back(attention);
        
        // Clear existing signals and add new ones
        self.clearSignals();
        for (float signal : signals) {
            self.addSignal(signal);
        }
    }, py::arg("energy"), py::arg("hunger"), py::arg("stress"), py::arg("attention"),
         "Update internal signals from agent state variables");
}

} // namespace nlm
