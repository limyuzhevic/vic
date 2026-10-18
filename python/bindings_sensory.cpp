// python/bindings_sensory.cpp

void bindSensoryInput(py::module_& m) {
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
    
    Base class for all sensory input data types.
    Provides common interface for accessing sensory information.
    
    Subclasses:
        Vision: Visual sensor data
        Audio: Auditory sensor data  
        InternalSignals: Internal brain signals
    
    Usage:
        >>> percept = pynlm.SensoryPercept()
        >>> vision = percept.getVision()
        >>> data = vision.getData()
    
    See Also:
        Vision, Audio, InternalSignals
    )pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input)
        
        Returns:
            str: String representation of the sensory input type
            
        Example:
            >>> percept = pynlm.SensoryPercept()
            >>> percept.getVision().getType()
            'vision'
        ")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a vector)
        
        Returns:
            list: Raw sensory data as a list of floats
            
        Example:
            >>> percept = pynlm.SensoryPercept()
            >>> percept.getVision().setData([0.1, 0.5, 0.9])
            >>> percept.getVision().getData()
            [0.1, 0.5, 0.9]
        ")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality)
        
        Returns:
            int: Number of dimensions in the data
            
        Example:
            >>> percept = pynlm.SensoryPercept()
            >>> percept.getVision().configure(8, 8, 3)
            >>> percept.getVision().getDimensions()
            192
        ")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp)
        
        Returns:
            float: Simulation timestamp when this data was created
            
        Example:
            >>> percept = pynlm.SensoryPercept()
            >>> percept.getVision().setTimestamp(1.5)
            >>> percept.getVision().getTimestamp()
            1.5
        ")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set the timestamp)
             
             Args:
                 timestamp: Simulation timestamp
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> percept.getVision().setTimestamp(2.0)
            ")
        .def("getAge", &SensoryInput::getAge, R"pbdoc(Get the age of this sensory data)
        
        Returns:
            float: Age of the data in simulation time
            
        Example:
            >>> percept = pynlm.SensoryPercept()
            >>> percept.getVision().setTimestamp(0.0)
            >>> # Age depends on current simulation time
            >>> percept.getVision().getAge()
            1.0
        ")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(input.getType()) + 
                   ", dimensions=" + std::to_string(input.getDimensions()) + ">";
        });

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
    
    Visual sensor data representing camera-like input.
    Data is typically organized as a flattened array of RGB values.
    
    Common Uses:
        - Simulated camera input
        - Visual scene representation
        - Feature extraction input
    
    Data Layout:
        Width × Height × Channels (typically 8×8×3 = 192 values)
        
    Example:
        >>> vision = pynlm.Vision(8, 8, 3)
        >>> data = [0.1 * (i % 3) for i in range(192)]
        >>> vision.setData(data)
        >>> vision.getWidth() * vision.getHeight() * vision.getChannels()
        192
    
    Attributes:
        width: Image width in pixels
        height: Image height in pixels  
        channels: Color channels (typically 3 for RGB)
        total_pixels: Total number of data points
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), 
             py::arg("height"), py::arg("channels") = 3,
             R"pbdoc(Create vision input with specified dimensions)
             
             Args:
                 width: Image width in pixels (e.g., 8 for 8x8)
                 height: Image height in pixels (e.g., 8 for 8x8)
                 channels: Number of color channels (default: 3 for RGB)
                 
             Example:
                 >>> vision = pynlm.Vision(16, 16, 3)  # 16x16 RGB image
                 >>> vision.getWidth()
                 16
                 >>> vision.getChannels()
                 3
             ")
        .def("configure", &Vision::configure, py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, R"pbdoc(Configure vision dimensions)
             
             Args:
                 width: Image width in pixels
                 height: Image height in pixels
                 channels: Number of color channels (default: 3)
                 
             Example:
                 >>> vision = pynlm.Vision()
                 >>> vision.configure(10, 10, 3)
                 >>> vision.getDimensions()
                 300
             ")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() != getExpectedSize(self)) {
                throw nlm::InvalidParameterError(
                    "data size",
                    "expected " + std::to_string(getExpectedSize(self)) + 
                    " but got " + std::to_string(data.size())
                );
            }
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set vision data with validation)
        
        Args:
            data: Vision data as list of floats. Must match width × height × channels.
            
        Raises:
            InvalidParameterError: If data size doesn't match dimensions
            
        Example:
            >>> vision = pynlm.Vision(4, 4, 3)  # 48 values
            >>> data = [0.1 * i for i in range(48)]
            >>> vision.setData(data)  # OK
            >>> bad_data = [0.1] * 10  # Wrong size
            >>> vision.setData(bad_data)  # Raises InvalidParameterError
        ")
        .def("getData", &Vision::getData, R"pbdoc(Get vision data)
        
        Returns:
            list: Vision data as list of floats
            
        Example:
            >>> vision = pynlm.Vision(2, 2, 3)
            >>> vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 0.0])
            >>> vision.getData()
            [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 0.0]
        ")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get vision image width)
             
             Returns:
                 int: Image width in pixels
                 
             Example:
                 >>> vision = pynlm.Vision(20, 15, 3)
                 >>> vision.getWidth()
                 20
            ")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get vision image height)
             
             Returns:
                 int: Image height in pixels
                 
             Example:
                 >>> vision = pynlm.Vision(20, 15, 3)
                 >>> vision.getHeight()
                 15
            ")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get number of color channels)
             
             Returns:
                 int: Number of color channels
                 
             Example:
                 >>> vision = pynlm.Vision(8, 8, 1)  # Grayscale
                 >>> vision.getChannels()
                 1
                 >>> vision2 = pynlm.Vision(8, 8, 3)  # RGB
                 >>> vision2.getChannels()
                 3
            ")
        .def("getExpectedSize", &Vision::getExpectedSize,
             R"pbdoc(Get expected data size for current dimensions)
             
             Returns:
                 int: Expected number of data points
                 
             Example:
                 >>> vision = pynlm.Vision(10, 10, 3)
                 >>> vision.getExpectedSize()
                 300
             ")
        .def("isValidDataSize", [](const Vision& self, size_t size) {
            return size == self.getExpectedSize();
        }, py::arg("size"), R"pbdoc(Check if data size matches dimensions)
        
        Args:
            size: Data size to check
            
        Returns:
            bool: True if size matches width × height × channels
            
        Example:
            >>> vision = pynlm.Vision(8, 8, 3)
            >>> vision.isValidDataSize(192)
            True
            >>> vision.isValidDataSize(100)
            False
        ")
        .def("getExpectedSize", [](const Audio& self) {
            return self.getNumSamples();
        }, R"pbdoc(Get expected data size for audio)
        
        Returns:
            int: Expected number of audio samples
            
        Example:
            >>> audio = pynlm.Audio(16000, 100)
            >>> audio.getExpectedSize()
            100
        ")
        .def("normalizeData", [](Vision& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > 0.0f) {
                for (auto& val : data) val /= max_val;
            }
            self.setData(data);
        }, R"pbdoc(Normalize vision data to range [0, 1])
        
        Normalizes all vision data to range [0, 1] by dividing by maximum value.
        
        Example:
            >>> vision = pynlm.Vision(2, 2, 1)
            >>> vision.setData([0.5, 1.0, 1.5, 2.0])
            >>> vision.getData()
            [0.5, 1.0, 1.5, 2.0]
            >>> vision.normalizeData()
            >>> vision.getData()
            [0.25, 0.5, 0.75, 1.0]
        ")
        .def("getCenterPixel", [](Vision& self) {
            size_t w = self.getWidth();
            size_t h = self.getHeight();
            size_t center_idx = (h/2) * w + (w/2);
            if (center_idx < self.getData().size()) {
                return self.getData()[center_idx];
            }
            return 0.0f;
        }, R"pbdoc(Get center pixel value)
        
        Returns:
            float: Value at the center of the image
            
        Example:
            >>> vision = pynlm.Vision(5, 5, 1)
            >>> vision.setData(list(range(25)))
            >>> vision.getCenterPixel()
            12.0
        ")
        .def("getAverageBrightness", [](Vision& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, R"pbdoc(Get average brightness of vision data)
        
        Returns:
            float: Average value across all pixels
            
        Example:
            >>> vision = pynlm.Vision(2, 2, 1)
            >>> vision.setData([0.1, 0.5, 0.3, 0.8])
            >>> vision.getAverageBrightness()
            0.425
        ")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" +
                   std::to_string(v.getHeight()) + "x" +
                   std::to_string(v.getChannels()) + ">";
        });

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
    
    Auditory sensor data representing sound waveforms.
    Used for audio processing and speech recognition tasks.
    
    Common Uses:
        - Sound source localization
        - Speech recognition
        - Music processing
        
    Data Layout:
        Sample rate × number of samples
        
    Example:
        >>> audio = pynlm.Audio(44100, 1000)  # CD quality
        >>> data = [0.5 * math.sin(2 * math.pi * f * t / 44100) for f, t in enumerate(range(1000))]
        >>> audio.setData(data)
        >>> audio.getSampleRate()
        44100
    
    Attributes:
        sample_rate: Samples per second (Hz)
        num_samples: Number of audio samples
        duration: Audio duration in seconds
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(Create audio input with sample rate and duration)
             
             Args:
                 sampleRate: Samples per second (Hz)
                 numSamples: Number of audio samples
                 
             Example:
                 >>> audio = pynlm.Audio(8000, 100)  # 8kHz, 100ms at 1kHz
                 >>> audio.getSampleRate()
                 8000
                 >>> audio.getNumSamples()
                 100
             ")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() != self.getNumSamples()) {
                throw nlm::InvalidParameterError(
                    "data size", 
                    "expected " + std::to_string(self.getNumSamples()) +
                    " samples but got " + std::to_string(data.size())
                );
            }
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data with validation)
        
        Args:
            data: Audio samples as list of floats. Must match numSamples.
            
        Raises:
            InvalidParameterError: If data size doesn't match expected samples
            
        Example:
            >>> audio = pynlm.Audio(16000, 50)  # 1 second at 16kHz
            >>> data = [0.0] * 50  # Silence
            >>> audio.setData(data)
            >>> audio.getNumSamples()
            50
        ")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set audio sample rate)
             
             Args:
                 sampleRate: New sample rate in Hz
                 
             Example:
                 >>> audio = pynlm.Audio(44100, 100)
                 >>> audio.setSampleRate(16000)
                 >>> audio.getSampleRate()
                 16000
             ")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get audio sample rate)
             
             Returns:
                 int: Sample rate in Hz
                 
             Example:
                 >>> audio = pynlm.Audio(22050, 200)
                 >>> audio.getSampleRate()
                 22050
             ")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get number of audio samples)
             
             Returns:
                 int: Number of samples
                 
             Example:
                 >>> audio = pynlm.Audio(16000, 50)
                 >>> audio.getNumSamples()
                 50
             ")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, R"pbdoc(Get audio duration in seconds)
        
        Returns:
            float: Audio duration in seconds
            
        Example:
            >>> audio = pynlm.Audio(8000, 200)  # 200 samples at 8kHz
            >>> audio.getDuration()
            0.025
        ")
        .def("getFrequency", [](Audio& self, int sampleIndex) {
            if (sampleIndex < 0 || sampleIndex >= self.getNumSamples()) {
                throw nlm::InvalidParameterError(
                    "sample index", 
                    "index must be between 0 and " + std::to_string(self.getNumSamples() - 1)
                );
            }
            // Simple frequency extraction (would use FFT in real implementation)
            return self.getData()[sampleIndex % self.getData().size()];
        }, py::arg("sample_index"), R"pbdoc(Get amplitude at specific sample index)
        
        Args:
            sample_index: Sample index (0-based)
            
        Returns:
            float: Amplitude at that sample
            
        Raises:
            InvalidParameterError: If index out of range
            
        Example:
            >>> audio = pynlm.Audio(16000, 10)
            >>> audio.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0])
            >>> audio.getFrequency(0)
            0.1
            >>> audio.getFrequency(5)
            0.4
        ")
        .def("normalizeAmplitude", [](Audio& self, float targetMax = 1.0f) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end(),
                                             [](float a, float b) { return std::abs(a) < std::abs(b); });
            if (max_val > 0.0f) {
                float scale = targetMax / max_val;
                for (auto& sample : data) sample *= scale;
            }
            self.setData(data);
        }, py::arg("target_max") = 1.0f,
             R"pbdoc(Normalize audio amplitude)
             
             Args:
                 target_max: Target maximum amplitude (default: 1.0)
                 
             Example:
                 >>> audio = pynlm.Audio(16000, 5)
                 >>> audio.setData([0.5, 1.0, 1.5, 2.0, 2.5])
                 >>> audio.normalizeAmplitude()
                 >>> audio.getData()
                 [0.2, 0.4, 0.6, 0.8, 1.0]
             ")
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + "Hz, " +
                   std::to_string(a.getNumSamples()) + " samples>";
        });

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
    
    Internal brain signals representing cognitive states.
    Used for neuromodulation, prediction errors, and internal state monitoring.
    
    Common Uses:
        - Reward prediction errors
        - Neuromodulation signals
        - Cognitive state tracking
        
    Example:
        >>> signals = pynlm.InternalSignals()
        >>> signals.addSignal(0.8)  # Dopamine level
        >>> signals.addSignal(0.3)  # Novelty
        >>> signals.addSignal(0.9)  # Curiosity
        >>> signals.getInternal()
        [0.8, 0.3, 0.9]
    
    Attributes:
        signal_count: Number of internal signals
        max_signals: Maximum number of signals (default: 100)
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add an internal signal)
             
             Args:
                 value: Signal value (typically between 0.0 and 1.0)
                 
             Example:
                 >>> signals = pynlm.InternalSignals()
                 >>> signals.addSignal(0.5)  # Curiosity level
                 >>> signals.addSignal(0.8)  # Reward prediction
            ")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            auto signals = self.getInternal();
            if (index >= signals.size()) {
                throw nlm::InvalidParameterError(
                    "signal index",
                    "index must be between 0 and " + std::to_string(signals.size() - 1)
                );
            }
            return signals[index];
        }, py::arg("index"), R"pbdoc(Get signal at specific index)
        
        Args:
            index: Signal index (0-based)
            
        Returns:
            float: Signal value
            
        Raises:
            InvalidParameterError: If index out of range
            
        Example:
            >>> signals = pynlm.InternalSignals()
            >>> signals.addSignal(0.7)
            >>> signals.addSignal(0.9)
            >>> signals.getSignal(0)
            0.7
            >>> signals.getSignal(1)
            0.9
        ")
        .def("getInternal", &InternalSignals::getInternal,
             R"pbdoc(Get all internal signals)
             
             Returns:
                 list: List of internal signal values
                 
             Example:
                 >>> signals = pynlm.InternalSignals()
                 >>> signals.addSignal(0.5)
                 >>> signals.addSignal(0.8)
                 >>> signals.getInternal()
                 [0.5, 0.8]
             ")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(Clear all internal signals)
             
             Example:
                 >>> signals = pynlm.InternalSignals()
                 >>> signals.addSignal(0.5)
                 >>> signals.addSignal(0.8)
                 >>> signals.clearSignals()
                 >>> signals.getInternal()
                 []
             ")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getInternal().size();
        }, R"pbdoc(Get number of internal signals)
        
        Returns:
            int: Number of signals
            
        Example:
            >>> signals = pynlm.InternalSignals()
            >>> len(signals.getInternal())
            0
        ")
        .def("getAverageSignal", [](const InternalSignals& self) {
            auto signals = self.getInternal();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, R"pbdoc(Get average of all internal signals)
        
        Returns:
            float: Average signal value
            
        Example:
            >>> signals = pynlm.InternalSignals()
            >>> signals.addSignal(0.5)
            >>> signals.addSignal(0.8)
            >>> signals.addSignal(0.9)
            >>> signals.getAverageSignal()
            0.73333335
        ")
        .def("getMaxSignal", [](const InternalSignals& self) {
            auto signals = self.getInternal();
            if (signals.empty()) return 0.0f;
            return *std::max_element(signals.begin(), signals.end());
        }, R"pbdoc(Get maximum signal value)
        
        Returns:
            float: Maximum signal value
            
        Example:
            >>> signals = pynlm.InternalSignals()
            >>> signals.addSignal(0.3)
            >>> signals.addSignal(0.8)
            >>> signals.addSignal(0.5)
            >>> signals.getMaxSignal()
            0.8
        ")
        .def("getSignalAtTime", [](const InternalSignals& self, float time) {
            auto signals = self.getInternal();
            size_t index = static_cast<size_t>(time * 10.0f);  // Assuming 10Hz sampling
            if (index < signals.size()) {
                return signals[index];
            }
            return 0.0f;
        }, py::arg("time"), R"pbdoc(Get signal value at specific time)
        
        Args:
            time: Time in simulation units
            
        Returns:
            float: Signal value at that time
            
        Example:
            >>> signals = pynlm.InternalSignals()
            >>> for i in range(10): signals.addSignal(i * 0.1)
            >>> signals.getSignalAtTime(0.5)
            0.5
        ")
        .def("__repr__", [](const InternalSignals& s) {
            return "<InternalSignals: " + std::to_string(s.getInternal().size()) + 
                   " signals>";
        });
}

// Helper functions for sensory processing
namespace py {
    // Helper for expected vision data size
    inline size_t getExpectedSize(const Vision& vision) {
        return vision.getWidth() * vision.getHeight() * vision.getChannels();
    }
}