#!/usr/bin/env python3
"""
Sensory input test suite for NLM Python bindings.
Tests sensory modalities (Vision, Audio, InternalSignals) and data handling.
"""

import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

class TestVisionSensory:
    """Test vision sensory input."""
    
    def test_vision_creation(self):
        """Test vision sensor creation."""
        # Create vision sensor with default parameters
        vision = nlm.Vision()
        assert vision is not None
        assert isinstance(vision, nlm.Vision)
        assert vision.getType() == nlm.SensoryInput
        
        # Create vision with custom dimensions
        vision2 = nlm.Vision(64, 64, 3)
        assert vision2.getWidth() == 64
        assert vision2.getHeight() == 64
        assert vision2.getChannels() == 3
        
        # Create vision with width/height only (channels default to 3)
        vision3 = nlm.Vision(32, 32)
        assert vision3.getWidth() == 32
        assert vision3.getHeight() == 32
        assert vision3.getChannels() == 3
    
    def test_vision_data_handling(self):
        """Test vision data handling."""
        # Create vision sensor
        vision = nlm.Vision(10, 10, 3)
        
        # Create test data (grayscale image converted to RGB)
        test_data = []
        for y in range(10):
            for x in range(10):
                # Simple gradient pattern
                value = ((x + y) % 10) / 10.0
                test_data.extend([value, value, value])  # RGB
        
        # Test data access (setData is not available in current bindings, so we test existing functionality)
        assert vision.getWidth() == 10
        assert vision.getHeight() == 10
        assert vision.getChannels() == 3
        
        # Test timestamp
        assert vision.getTimestamp() == 0
        vision.setTimestamp(123456789)
        assert vision.getTimestamp() == 123456789
    
    def test_vision_properties(self):
        """Test vision sensor properties."""
        vision = nlm.Vision(32, 32, 1)
        
        # Test basic properties
        assert vision.getWidth() == 32
        assert vision.getHeight() == 32
        assert vision.getChannels() == 1
        assert vision.getType() == nlm.SensoryInput

class TestAudioSensory:
    """Test audio sensory input."""
    
    def test_audio_creation(self):
        """Test audio sensor creation."""
        # Create audio sensor with default parameters
        audio = nlm.Audio()
        assert audio is not None
        assert isinstance(audio, nlm.Audio)
        assert audio.getType() == nlm.SensoryInput
        
        # Create audio with custom parameters
        audio2 = nlm.Audio(48000, 2048)  # 48kHz, 2048 samples
        assert audio2.getSampleRate() == 48000
        assert audio2.getNumSamples() == 2048
        
        # Create audio with sample rate only (numSamples defaults to 1024?)
        audio3 = nlm.Audio(44100)
        assert audio3.getSampleRate() == 44100
        # Note: default numSamples not specified in binding
    
    def test_audio_data_handling(self):
        """Test audio data handling."""
        # Create audio sensor
        audio = nlm.Audio(22050, 512)
        
        # Test properties
        assert audio.getSampleRate() == 22050
        assert audio.getNumSamples() == 512
        assert audio.getType() == nlm.SensoryInput
        
        # Test setting sample rate
        audio.setSampleRate(44100)
        assert audio.getSampleRate() == 44100
        
        # Test setting data
        test_data = [0.1] * 512
        audio.setData(test_data)
        
        # Note: In current bindings, getData may return the raw data
        # or modify the internal data. We'll test basic functionality.
        retrieved_data = audio.getData()
        assert len(retrieved_data) == 512
    
    def test_audio_properties(self):
        """Test audio sensor properties."""
        audio = nlm.Audio(96000, 4096)
        
        # Test basic properties
        assert audio.getSampleRate() == 96000
        assert audio.getNumSamples() == 4096
        assert audio.getType() == nlm.SensoryInput

class TestInternalSignalsSensory:
    """Test internal signals sensory input."""
    
    def test_internal_signals_creation(self):
        """Test internal signals sensor creation."""
        internal = nlm.InternalSignals()
        assert internal is not None
        assert isinstance(internal, nlm.InternalSignals)
        assert internal.getType() == nlm.SensoryInput
    
    def test_internal_signals_data_operations(self):
        """Test internal signals data operations."""
        internal = nlm.InternalSignals()
        
        # Add signals
        internal.addSignal(0.1)
        internal.addSignal(0.5)
        internal.addSignal(0.9)
        internal.addSignal(0.0)
        internal.addSignal(0.3)
        
        # Get data
        signals = internal.getData()
        assert len(signals) == 5
        assert signals[0] == 0.1
        assert signals[1] == 0.5
        assert signals[2] == 0.9
        assert signals[3] == 0.0
        assert signals[4] == 0.3
        
        # Clear signals
        internal.clearSignals()
        assert len(internal.getData()) == 0
        
        # Add more signals after clear
        internal.addSignal(0.7)
        assert len(internal.getData()) == 1
        assert internal.getData()[0] == 0.7
    
    def test_internal_signals_properties(self):
        """Test internal signals sensor properties."""
        internal = nlm.InternalSignals()
        
        # Test type
        assert internal.getType() == nlm.SensoryInput
        
        # Add some signals for testing
        internal.addSignal(0.5)
        
        # Test data access
        signals = internal.getData()
        assert len(signals) == 1
        assert signals[0] == 0.5

class TestSensoryInputBase:
    """Test base sensory input functionality."""
    
    def test_sensory_input_base_class(self):
        """Test SensoryInput base class functionality."""
        # Create a concrete sensory input instance
        internal = nlm.InternalSignals()
        
        # Test base class methods
        assert internal.getType() == nlm.SensoryInput
        assert isinstance(internal.getData(), list)
        
        # Test timestamp operations
        assert internal.getTimestamp() == 0
        internal.setTimestamp(1000)
        assert internal.getTimestamp() == 1000
        
        # Test dimensions
        assert internal.getDimensions() == 0
        
        # Add signals and test dimensions
        internal.addSignal(0.5)
        internal.addSignal(0.8)
        assert internal.getDimensions() == 2
    
    def test_sensory_input_interface(self):
        """Test sensory input interface."""
        # Test that all sensory inputs implement the same interface
        vision = nlm.Vision(10, 10, 3)
        audio = nlm.Audio(44100, 1024)
        internal = nlm.InternalSignals()
        
        # All should have these methods
        for sensor in [vision, audio, internal]:
            assert hasattr(sensor, 'getType')
            assert hasattr(sensor, 'getData')
            assert hasattr(sensor, 'getDimensions')
            assert hasattr(sensor, 'getTimestamp')
            assert hasattr(sensor, 'setTimestamp')
        
        # Test type methods
        assert vision.getType() == nlm.SensoryInput
        assert audio.getType() == nlm.SensoryInput
        assert internal.getType() == nlm.SensoryInput

class TestSensoryPercept:
    """Test sensory percept data structure."""
    
    def test_sensory_percept_creation(self):
        """Test sensory percept creation."""
        percept = nlm.SensoryPercept()
        assert percept is not None
        assert isinstance(percept, nlm.SensoryPercept)
    
    def test_sensory_percept_vision(self):
        """Test vision data in percept."""
        percept = nlm.SensoryPercept()
        
        # Create test vision data
        vision_data = []
        width = 16
        height = 16
        
        for y in range(height):
            for x in range(width):
                # Simple checkerboard pattern
                value = 1.0 if ((x + y) % 2 == 0) else 0.0
                vision_data.append(value)
        
        # Set vision data
        percept.setVision(vision_data)
        
        # Retrieve vision data
        retrieved_vision = percept.getVision()
        assert retrieved_vision == vision_data
        assert percept.getVisionWidth() == width
        assert percept.getVisionHeight() == height
    
    def test_sensory_percept_touch(self):
        """Test touch data in percept."""
        percept = nlm.SensoryPercept()
        
        # Create test touch data
        touch_data = [0.1, 0.3, 0.5, 0.7, 0.9]
        percept.setTouch(touch_data)
        
        # Retrieve touch data
        retrieved_touch = percept.getTouch()
        assert retrieved_touch == touch_data
    
    def test_sensory_percept_internal(self):
        """Test internal signals data in percept."""
        percept = nlm.SensoryPercept()
        
        # Create test internal data
        internal_data = [0.2, 0.4, 0.6, 0.8]
        percept.setInternal(internal_data)
        
        # Retrieve internal data
        retrieved_internal = percept.getInternal()
        assert retrieved_internal == internal_data
    
    def test_sensory_percept_proprioception(self):
        """Test proprioception data in percept."""
        percept = nlm.SensoryPercept()
        
        # Create test proprioception data
        proprioception_data = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5]
        percept.setProprioception(proprioception_data)
        
        # Retrieve proprioception data
        retrieved_proprioception = percept.getProprioception()
        assert retrieved_proprioception == proprioception_data
    
    def test_sensory_percept_audio(self):
        """Test audio data in percept."""
        percept = nlm.SensoryPercept()
        
        # Create test audio data
        audio_data = [0.0, 0.25, 0.5, 0.75, 1.0]
        percept.setAudio(audio_data)
        
        # Retrieve audio data
        retrieved_audio = percept.getAudio()
        assert retrieved_audio == audio_data
    
    def test_sensory_percept_all_signals(self):
        """Test getting all signals from percept."""
        percept = nlm.SensoryPercept()
        
        # Set all signal types
        vision_data = [0.1] * 10
        touch_data = [0.2] * 5
        internal_data = [0.3] * 3
        proprioception_data = [0.4] * 2
        audio_data = [0.5] * 7
        
        percept.setVision(vision_data)
        percept.setTouch(touch_data)
        percept.setInternal(internal_data)
        percept.setProprioception(proprioception_data)
        percept.setAudio(audio_data)
        
        # Get all signals
        all_signals = percept.getAllSignals()
        assert isinstance(all_signals, list)
        assert len(all_signals) == 10 + 5 + 3 + 2 + 7
        
        # Verify structure
        assert isinstance(all_signals, list)
    
    def test_sensory_percept_timestamp(self):
        """Test timestamp operations in percept."""
        percept = nlm.SensoryPercept()
        
        # Test initial timestamp
        assert percept.getTimestamp() == 0
        
        # Set timestamp
        test_timestamp = 1234567890
        percept.setTimestamp(test_timestamp)
        
        # Verify timestamp
        assert percept.getTimestamp() == test_timestamp

if __name__ == "__main__":
    pytest.main([__file__, "-v"])