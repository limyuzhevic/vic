namespace nlm {

// Implementation of helper methods for biologically realistic vision processing

void VisionProcessor::processRetinalLayers(const std::vector<float>& input) {
    // Process through biological retinal layers
    // Photoreceptors -> Bipolar cells -> Horizontal cells -> Amacrine cells -> Ganglion cells
    
    size_t inputSize = input.size();
    if (inputSize == 0) return;
    
    // Photoreceptor layer: convert light to neural signals
    // Simulate logarithmic response of photoreceptors (Weber-Fechner law)
    std::vector<float> photoreceptorOutput(inputSize);
    float maxInput = *std::max_element(input.begin(), input.end());
    float minInput = *std::min_element(input.begin(), input.end());
    float inputRange = maxInput - minInput;
    
    if (inputRange > 0.0f) {
        for (size_t i = 0; i < inputSize; ++i) {
            // Logarithmic response with baseline
            float normalized = (input[i] - minInput) / inputRange;
            photoreceptorOutput[i] = std::log(1.0f + normalized * 100.0f) / std::log(101.0f);
        }
    } else {
        std::fill(photoreceptorOutput.begin(), photoreceptorOutput.end(), 0.5f);
    }
    
    // Bipolar cell layer: center-surround receptive fields
    // Direction-selective and motion-sensitive
    std::vector<float> bipolarOutput(inputSize, 0.0f);
    for (size_t i = 1; i < inputSize - 1; ++i) {
        // Center-surround filtering (Mexican hat)
        float center = photoreceptorOutput[i];
        float surround = (photoreceptorOutput[i-1] + photoreceptorOutput[i+1]) * 0.5f;
        float response = center - 0.5f * surround;
        
        // Apply gain control and nonlinearity
        float normalized = (response + 1.0f) / 2.0f;
        bipolarOutput[i] = normalized * normalized * 4.0f - 1.0f;  // Quadratic response
    }
    
    // Horizontal cell layer: lateral inhibition for contrast enhancement
    std::vector<float> horizontalOutput = bipolarOutput;
    for (size_t i = 2; i < inputSize - 2; ++i) {
        // Lateral inhibition based on local contrast
        float localContrast = std::abs(bipolarOutput[i] - bipolarOutput[i-2]) +
                           std::abs(bipolarOutput[i] - bipolarOutput[i+2]);
        float inhibition = 0.1f * localContrast;
        horizontalOutput[i] = std::max(-1.0f, std::min(1.0f, horizontalOutput[i] * (1.0f - inhibition)));
    }
    
    // Amacrine cell layer: motion detection and temporal processing
    std::vector<float> amacrineOutput(inputSize, 0.0f);
    for (size_t i = 1; i < inputSize; ++i) {
        // Motion sensitivity based on temporal changes
        float temporalChange = std::abs(input[i] - input[i-1]);
        float motionSignal = horizontalOutput[i] * temporalChange * 2.0f;
        amacrineOutput[i] = std::tanh(motionSignal);
    }
    
    // Ganglion cell layer: final output with adaptive receptive fields
    pImpl->features = amacrineOutput;
    
    NLM_LOG_INFO("Vision processing: Retinal layers - processed " + std::to_string(inputSize) + " pixels");
}

void VisionProcessor::processV1SimpleCells() {
    // Process through simple cells in primary visual cortex (V1)
    // Orientation-selective receptive fields using Gabor filters
    
    size_t inputSize = pImpl->featureDimensions;
    if (inputSize < 16) return;  // Need minimum size for filtering
    
    // Simulate 8 orientation channels (0°, 45°, 90°, 135°, etc.)
    size_t numOrientations = 8;
    std::vector<std::vector<float>> orientationResponses(numOrientations, std::vector<float>(inputSize, 0.0f));
    
    // Gabor filter parameters for biological realism
    float sigma = 2.0f;  // Receptive field size
    float lambda = 4.0f;  // Wavelength
    float gamma = 0.5f;   // Spatial aspect ratio
    
    for (size_t orientation = 0; orientation < numOrientations; ++orientation) {
        float angle = orientation * M_PI / numOrientations;
        
        for (size_t i = 0; i < inputSize; ++i) {
            // Convert linear index to 2D coordinates for filtering
            size_t x = i % static_cast<size_t>(std::sqrt(inputSize));
            size_t y = i / static_cast<size_t>(std::sqrt(inputSize));
            
            // Calculate Gabor filter response
            float x_theta = x * std::cos(angle) + y * std::sin(angle);
            float y_theta = -x * std::sin(angle) + y * std::cos(angle);
            
            float gaussian = std::exp(-(x_theta*x_theta + gamma*y_theta*gamma*y_theta) / (2*sigma*sigma));
            float sinusoid = std::cos(2 * M_PI * x_theta / lambda);
            
            float response = gaussian * sinusoid;
            orientationResponses[orientation][i] = response * pImpl->features[i];  // Modulate by retinal output
        }
        
        // Apply nonlinearity to simulate simple cell response
        for (size_t i = 0; i < inputSize; ++i) {
            orientationResponses[orientation][i] = std::max(0.0f, orientationResponses[orientation][i]);
        }
    }
    
    // Combine orientation channels (max pooling across orientations)
    for (size_t i = 0; i < inputSize; ++i) {
        float maxResponse = 0.0f;
        for (size_t orientation = 0; orientation < numOrientations; ++orientation) {
            maxResponse = std::max(maxResponse, orientationResponses[orientation][i]);
        }
        pImpl->features[i] = maxResponse;
    }
    
    NLM_LOG_INFO("Vision processing: V1 simple cells - extracted orientation features");
}

void VisionProcessor::processV1ComplexCells() {
    // Process through complex cells in V1
    // Position-invariant, movement-selective neurons
    
    size_t inputSize = pImpl->featureDimensions;
    if (inputSize < 4) return;
    
    std::vector<float> complexOutput(inputSize, 0.0f);
    
    // Complex cells respond to patterns across multiple simple cells
    // Simulate squaring operation (energy model)
    for (size_t i = 0; i < inputSize; ++i) {
        // Average response across neighboring simple cells
        float localAverage = 0.0f;
        size_t count = 0;
        
        for (size_t j = std::max(size_t(0), i-1); j <= std::min(i+1, inputSize-1); ++j) {
            localAverage += pImpl->features[j];
            count++;
        }
        
        if (count > 0) {
            localAverage /= count;
        }
        
        // Energy model: sum of squares of simple cell responses
        float energy = 0.0f;
        for (size_t j = std::max(size_t(0), i-1); j <= std::min(i+1, inputSize-1); ++j) {
            energy += pImpl->features[j] * pImpl->features[j];
        }
        
        // Complex cell response is square root of energy
        float response = std::sqrt(energy);
        
        // Modulate by spatial context and lateral inhibition
        float lateralInhibition = 0.0f;
        if (i > 0 && i < inputSize-1) {
            lateralInhibition = std::abs(pImpl->features[i-1] - pImpl->features[i+1]) * 0.1f;
        }
        
        complexOutput[i] = response * (1.0f - lateralInhibition);
    }
    
    pImpl->features = complexOutput;
    
    NLM_LOG_INFO("Vision processing: V1 complex cells - extracted position-invariant features");
}

void VisionProcessor::processSpatialPooling() {
    // Apply spatial pooling to reduce resolution and increase receptive field size
    // Simulates feedforward processing in V2 and V4
    
    size_t inputSize = pImpl->featureDimensions;
    if (inputSize < 16) return;
    
    // Calculate pooling size based on input dimensions
    size_t poolSize = static_cast<size_t>(std::sqrt(inputSize));
    if (poolSize < 2) poolSize = 2;
    
    // Resize to pooled representation
    size_t outputSize = static_cast<size_t>(std::sqrt(static_cast<float>(inputSize) / static_cast<float>(poolSize * poolSize)));
    if (outputSize < 1) outputSize = 1;
    
    pImpl->features.resize(outputSize * outputSize, 0.0f);
    pImpl->featureDimensions = outputSize * outputSize;
    
    // Max pooling across spatial locations
    for (size_t y = 0; y < outputSize; ++y) {
        for (size_t x = 0; x < outputSize; ++x) {
            size_t inputX = x * poolSize;
            size_t inputY = y * poolSize;
            
            float maxResponse = 0.0f;
            for (size_t py = 0; py < poolSize && inputY + py < static_cast<size_t>(std::sqrt(inputSize)); ++py) {
                for (size_t px = 0; px < poolSize && inputX + px < static_cast<size_t>(std::sqrt(inputSize)); ++px) {
                    size_t inputIdx = (inputY + py) * static_cast<size_t>(std::sqrt(inputSize)) + (inputX + px);
                    if (inputIdx < inputSize) {
                        maxResponse = std::max(maxResponse, pImpl->features[inputIdx]);
                    }
                }
            }
            
            pImpl->features[y * outputSize + x] = maxResponse;
        }
    }
    
    NLM_LOG_INFO("Vision processing: Spatial pooling - reduced from " + std::to_string(inputSize) +
                 " to " + std::to_string(pImpl->features.size()) + " features");
}

void VisionProcessor::processHigherVisualFeatures() {
    // Process higher-order visual features in V2, V4, and prefrontal cortex
    // Color processing, shape recognition, object detection
    
    size_t inputSize = pImpl->featureDimensions;
    if (inputSize == 0) return;
    
    // Color feature extraction (simplified opponent process theory)
    std::vector<float> colorFeatures;
    if (inputSize >= 3) {
        // Extract opponent colors: L-M, S-(L+M)
        colorFeatures.resize(inputSize, 0.0f);
        for (size_t i = 0; i < inputSize; ++i) {
            float lightness = pImpl->features[i];
            float hue = (i % 3) * 2.094f;  // 0, 120, 240 degrees in radians
            float saturation = 0.5f + 0.5f * std::sin(hue);
            
            // Opponent process responses
            float lumGreenRed = lightness * std::sin(hue);
            float lumBlueYellow = lightness * std::cos(hue);
            float blueYellow = saturation * std::cos(hue + M_PI/3);
            
            colorFeatures[i] = (lumGreenRed + blueYellow) * 0.5f;
        }
    } else {
        colorFeatures = pImpl->features;
    }
    
    // Shape feature extraction
    std::vector<float> shapeFeatures(inputSize, 0.0f);
    for (size_t i = 0; i < inputSize; ++i) {
        // Edge density and orientation
        float edgeStrength = pImpl->features[i];
        float curvature = 0.0f;
        
        // Estimate curvature from local neighborhood
        if (i > 0 && i < inputSize-1) {
            float left = pImpl->features[i-1];
            float right = pImpl->features[i+1];
            curvature = std::abs(right - left) / (std::abs(left) + std::abs(right) + 1e-6f);
        }
        
        shapeFeatures[i] = edgeStrength * (1.0f - curvature * 0.5f);
    }
    
    // Objectness and saliency
    std::vector<float> objectFeatures(inputSize, 0.0f);
    for (size_t i = 0; i < inputSize; ++i) {
        // Combine multiple features for object detection
        float colorVote = std::abs(colorFeatures[i]);
        float shapeVote = shapeFeatures[i];
        float contextVote = 1.0f - std::abs(pImpl->features[i]);  // Contradiction detection
        
        float objectness = (colorVote * 0.4f + shapeVote * 0.4f + contextVote * 0.2f);
        objectFeatures[i] = objectness;
    }
    
    // Integrate all features
    pImpl->features.resize(inputSize);
    for (size_t i = 0; i < inputSize; ++i) {
        float integratedFeature = 0.4f * colorFeatures[i] +
                                 0.3f * shapeFeatures[i] +
                                 0.3f * objectFeatures[i];
        pImpl->features[i] = std::tanh(integratedFeature);
    }
    
    NLM_LOG_INFO("Vision processing: Higher visual features - extracted color, shape, and object information");
}

void VisionProcessor::processFeatureBinding() {
    // Feature binding and attention mechanisms
    // Synchronizes activity across different feature maps
    
    size_t inputSize = pImpl->featureDimensions;
    if (inputSize == 0) return;
    
    // Calculate feature coherence and binding strength
    std::vector<float> coherenceMap(inputSize, 0.0f);
    
    for (size_t i = 0; i < inputSize; ++i) {
        // Measure local feature similarity
        float similarity = 1.0f;
        
        // Compare with neighbors
        if (i > 0) {
            float diff = std::abs(pImpl->features[i] - pImpl->features[i-1]);
            similarity *= std::exp(-diff * 5.0f);
        }
        
        if (i < inputSize-1) {
            float diff = std::abs(pImpl->features[i] - pImpl->features[i+1]);
            similarity *= std::exp(-diff * 5.0f);
        }
        
        // Apply attention modulation
        float attention = std::min(1.0f, 0.5f + pImpl->features[i] * 0.5f);  // Higher feature strength = more attention
        
        // Binding strength is product of similarity and attention
        coherenceMap[i] = similarity * attention;
    }
    
    // Apply binding to final feature map
    for (size_t i = 0; i < inputSize; ++i) {
        pImpl->features[i] *= coherenceMap[i];
    }
    
    // Find and enhance salient features
    float maxCoherence = *std::max_element(coherenceMap.begin(), coherenceMap.end());
    if (maxCoherence > 0.5f) {
        // Enhance the most coherent feature (bound feature)
        size_t maxIndex = std::max_element(coherenceMap.begin(), coherenceMap.end()) - coherenceMap.begin();
        pImpl->features[maxIndex] = std::min(1.0f, pImpl->features[maxIndex] * 1.5f);
        
        NLM_LOG_INFO("Vision processing: Feature binding - enhanced salient feature at index " + std::to_string(maxIndex));
    }
    
    NLM_LOG_INFO("Vision processing: Feature binding - processed " + std::to_string(inputSize) + " feature locations");
}

} // namespace nlm