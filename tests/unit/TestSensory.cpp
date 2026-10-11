// NLM Unit Testing Framework
// Sensory Tests

#include "framework/TestFramework.hpp"
#include "brain/SensorySystem.hpp"
#include "brain/SensoryPercept.hpp"
#include <cassert>
#include <cmath>

class TestSensory : public TestCase {
public:
    TestSensory() : TestCase("Sensory") {}
    
    void testSensoryPerceptCreation() {
        SensoryPercept percept;
        
        // Create percept from raw input
        std::vector<double> input = {0.8, 0.3, 0.6, 0.2, 0.9};
        percept.createPercept(input, SensoryModality::VISUAL);
        
        assert(percept.getModality() == SensoryModality::VISUAL);
        assert(percept.getFeatures().size() == input.size());
        assert(std::abs(percept.getFeatures()[0] - input[0]) < 0.001);
    }
    
    void testSensoryProcessing() {
        SensorySystem sensory;
        
        // Test multi-sensory integration
        VisualInput visual = {0.8, 0.3, 0.6}; // RGB
        AuditoryInput auditory = {0.5, 0.2, 0.4}; // Frequency spectrum
        
        Percept visual_percept = sensory.processInput(visual, SensoryModality::VISUAL);
        Percept auditory_percept = sensory.processInput(auditory, SensoryModality::AUDITORY);
        
        assert(visual_percept.getStrength() > 0.0);
        assert(auditory_percept.getStrength() > 0.0);
        
        // Test cross-modal integration
        CombinedPercept combined = sensory.integrateModalities(
            visual_percept, auditory_percept);
        
        assert(combined.overall_strength > 0.0);
    }
    
    void testFeatureExtraction() {
        SensorySystem sensory;
        
        // Test edge detection (visual)
        std::vector<std::vector<double>> image = {
            {0.1, 0.2, 0.3},
            {0.4, 0.5, 0.6},
            {0.7, 0.8, 0.9}
        };
        
        std::vector<double> edges = sensory.detectEdges(image);
        assert(edges.size() == image.size() * image[0].size());
        
        // Test pattern recognition
        Pattern pattern = sensory.recognizePattern(image);
        assert(pattern.weights.size() > 0);
    }
    
    void testSensoryAdaptation() {
        SensorySystem sensory;
        
        // Test sensory adaptation to stimuli
        double initial_intensity = 1.0;
        double adapted_intensity = sensory.adaptToStimulus(
            initial_intensity, AdaptationType::PHOTOPIC);
        
        assert(adapted_intensity < initial_intensity); // Adaptation reduces response
        assert(adapted_intensity >= 0.0);
        
        // Test sensory recovery
        sensory.recoverFromStimulus(AdaptationType::PHOTOPIC);
        double recovered_intensity = sensory.getBaselineSensitivity();
        assert(recovered_intensity > adapted_intensity);
    }
    
    void testSensoryAttention() {
        SensorySystem sensory;
        
        // Test selective attention
        std::vector<Percept> percepts = {
            sensory.processInput(0.8, SensoryModality::VISUAL),
            sensory.processInput(0.3, SensoryModality::AUDITORY),
            sensory.processInput(0.6, SensoryModality::VISUAL)
        };
        
        Percept attended_percept = sensory.applySelectiveAttention(percepts);
        assert(attended_percept.getStrength() > 0.0);
        
        // Should attend to highest strength percept
        assert(attended_percept.getStrength() >= 0.6);
    }
};