#include "VisionProcessor.hpp"

namespace nlm {

struct VisionProcessor::Impl {
    // VisionFeatures storage
    VisionFeatures features;
    std::vector<float> processedFeatures;
    
    // Neural components
    std::unique_ptr<NeuralRegion> retinaRegion;
    std::unique_ptr<SpikeSystem> spikeSystem;
    
    // Biological parameters
    float edgeThreshold;
    float textureSensitivity;
    float maxSpikeRate;
    float inhibitionStrength;
    
    // Pre-computed kernels for edge detection
    std::vector<float> sobelXKernel;
    std::vector<float> sobelYKernel;
    
    Impl(size_t width, size_t height) 
        : features(width / 2, height / 2)
        , edgeThreshold(0.5f)
        , textureSensitivity(1.0f)
        , maxSpikeRate(100.0f)
        , inhibitionStrength(0.3f) {
        
        // Initialize Sobel kernels for edge detection
        sobelXKernel = {-1.0f, 0.0f, 1.0f, -2.0f, 0.0f, 2.0f, -1.0f, 0.0f, 1.0f};
        sobelYKernel = {-1.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 1.0f};
    }
};

VisionProcessor::VisionProcessor() : pImpl(new Impl(16, 16)) {
    initializeNeuralComponents(16, 16);
}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::initializeNeuralComponents(size_t width, size_t height) {
    // Create retina region with LIF neurons
    pImpl->retinaRegion = std::make_unique<NeuralRegion>(RegionId(1));
    
    // Add excitatory and inhibitory neuron populations
    // Horizontal visual processing
    PopulationId horizPop = pImpl->retinaRegion->addPopulation(width / 2, NeuronType::Excitatory);
    pImpl->retinaRegion->addPopulation(height / 2, NeuronType::Inhibitory);
    
    // Vertical visual processing  
    PopulationId vertPop = pImpl->retinaRegion->addPopulation(height / 2, NeuronType::Excitatory);
    pImpl->retinaRegion->addPopulation(width / 2, NeuronType::Inhibitory);
    
    // Orientation-selective neurons
    pImpl->retinaRegion->addPopulation(width / 2, NeuronType::Internal);
    
    // Initialize spike system for event-driven processing
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    
    // Register spike handler for neural activity recording
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        if (event.populationId == PopulationId(1) || event.populationId == PopulationId(2)) {
            // Vision feature neuron - record spike rate
            size_t idx = event.populationId.value - 1;
            if (idx < pImpl->features.spikeRates.size()) {
                pImpl->features.spikeRates[idx] += 1.0f;
            }
        }
    });
}

void VisionProcessor::process(const Vision& input) {
    reset();
    
    const auto& inputData = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t channels = input.getChannels();
    
    // Convert to grayscale for processing
    std::vector<float> grayScale(width * height);
    for (size_t i = 0; i < width * height; ++i) {
        size_t r = (channels > 0) ? i * channels : 0;
        size_t g = (channels > 1) ? i * channels + 1 : 0;
        size_t b = (channels > 2) ? i * channels + 2 : 0;
        
        // Simple luminance conversion
        float gray = 0.299f * inputData[r] + 0.587f * inputData[g] + 0.114f * inputData[b];
        grayScale[i] = gray;
    }
    
    // Process through retinal neural circuit
    processThroughRetina(grayScale);
    
    // Extract edge features using biologically plausible computation
    extractEdgeFeatures(grayScale);
    
    // Compute texture features using local variance
    computeTextureFeatures(grayScale);
    
    // Convert features to spike patterns
    convertToSpikes(grayScale);
    
    // Apply lateral inhibition for contrast enhancement
    applyInhibition(pImpl->features.horizontalEdges, pImpl->inhibitionStrength);
    applyInhibition(pImpl->features.verticalEdges, pImpl->inhibitionStrength);
    
    // Combine all features into final representation
    combineFeatures(pImpl->features);
    
    // Generate output feature vector
    generateOutputFeatures();
}

void VisionProcessor::processThroughRetina(const std::vector<float>& inputData) {
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    
    // Simulate retinal processing through spiking neurons
    // Each neuron responds to local input patterns
    
    // Create retinal population for horizontal processing
    auto neurons = pImpl->retinaRegion->getAllNeurons();
    
    // Inject visual input as current into sensory neurons
    float maxInput = *std::max_element(inputData.begin(), inputData.end());
    float minInput = *std::min_element(inputData.begin(), inputData.end());
    float inputRange = maxInput - minInput;
    
    // Distribute input to appropriate neuron populations
    size_t neuronCount = neurons.size();
    if (neuronCount > 0) {
        for (size_t i = 0; i < inputData.size() && i < neuronCount; ++i) {
            float normalizedInput = (inputData[i] - minInput) / (inputRange + 0.001f);
            float current = normalizedInput * 10.0f;  // Scale to neural current
            neurons[i]->injectCurrent(current);
        }
    }
}

void VisionProcessor::extractEdgeFeatures(const std::vector<float>& inputData) {
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t halfWidth = width / 2;
    size_t halfHeight = height / 2;
    
    // Apply Sobel edge detection (biologically plausible)
    for (size_t y = 0; y < halfHeight; ++y) {
        for (size_t x = 0; x < halfWidth; ++x) {
            size_t inputX = x * 2;
            size_t inputY = y * 2;
            
            // Apply Sobel filters to find edges
            float gx = 0.0f, gy = 0.0f;
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int sampleX = inputX + kx;
                    int sampleY = inputY + ky;
                    
                    if (sampleX < width && sampleY < height) {
                        float pixel = inputData[sampleY * width + sampleX];
                        
                        // Convolve with Sobel kernels
                        int kernelIdx = (ky + 1) * 3 + (kx + 1);
                        gx += pixel * sobelXKernel[kernelIdx];
                        gy += pixel * sobelYKernel[kernelIdx];
                    }
                }
            }
            
            // Compute edge magnitude
            float edgeMag = std::sqrt(gx * gx + gy * gy);
            size_t featureIdx = y * halfWidth + x;
            
            // Separate horizontal and vertical edges
            pImpl->features.horizontalEdges[featureIdx] = gy * edgeMag;
            pImpl->features.verticalEdges[featureIdx] = gx * edgeMag;
            
            // Threshold edges based on biological thresholds
            float thresholded = (edgeMag > pImpl->edgeThreshold) ? edgeMag : 0.0f;
            pImpl->features.orientedFeatures[featureIdx] = thresholded;
        }
    }
}

void VisionProcessor::computeTextureFeatures(const std::vector<float>& inputData) {
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t halfWidth = width / 2;
    size_t halfHeight = height / 2;
    
    for (size_t y = 1; y < halfHeight - 1; ++y) {
        for (size_t x = 1; x < halfWidth - 1; ++x) {
            size_t inputX = x * 2;
            size_t inputY = y * 2;
            
            // Compute local statistics for texture analysis
            float center = inputData[inputY * width + inputX];
            
            // Calculate variance for texture contrast
            float sum = 0.0f, sumSq = 0.0f;
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int sampleX = inputX + kx;
                    int sampleY = inputY + ky;
                    
                    if (sampleX < width && sampleY < height) {
                        float pixel = inputData[sampleY * width + sampleX];
                        sum += pixel;
                        sumSq += pixel * pixel;
                    }
                }
            }
            
            float mean = sum / 9.0f;
            float variance = sumSq / 9.0f - mean * mean;
            float contrast = std::sqrt(std::max(0.0f, variance));
            
            size_t featureIdx = y * halfWidth + x;
            pImpl->features.textureContrast[featureIdx] = contrast;
            pImpl->features.textureSmoothness[featureIdx] = 1.0f / (contrast + 0.001f);
            
            // Color features (if RGB)
            if (input.getChannels() >= 3) {
                size_t r = inputX * 3;
                size_t g = inputX * 3 + 1;
                size_t b = inputX * 3 + 2;
                
                float colorDiff = std::abs(inputData[r] - inputData[g]) + 
                                 std::abs(inputData[g] - inputData[b]) +
                                 std::abs(inputData[b] - inputData[r]);
                pImpl->features.colorFeatures[featureIdx] = colorDiff;
            }
        }
    }
}

void VisionProcessor::convertToSpikes(const std::vector<float>& features) {
    // Generate spike patterns based on feature intensities
    // Simulates temporal spiking dynamics
    
    auto neurons = pImpl->retinaRegion->getAllNeurons();
    float totalInput = 0.0f;
    
    for (const auto& neuron : neurons) {
        totalInput += neuron->getState().membranePotential;
    }
    
    // Scale and normalize for spike generation
    for (size_t i = 0; i < neurons.size() && i < pImpl->features.spikeRates.size(); ++i) {
        float potential = neurons[i]->getState().membranePotential;
        float normalized = (potential + 70.0f) / 125.0f;  // Normalize to [0,1]
        normalized = std::min(1.0f, normalized);
        
        // Generate spike rate based on membrane potential
        float spikeRate = normalized * pImpl->maxSpikeRate;
        pImpl->features.spikeRates[i] = spikeRate;
    }
}

void VisionProcessor::applyInhibition(std::vector<float>& features, float strength) {
    // Apply lateral inhibition for contrast enhancement (like retinal processing)
    size_t size = features.size();
    
    // Simple center-surround inhibition
    for (size_t i = 0; i < size; ++i) {
        // Center component
        float center = features[i];
        
        // Surround component (average of neighbors)
        float surround = 0.0f;
        size_t count = 0;
        
        if (i >= 1) surround += features[i - 1]; count++;
        if (i < size - 1) surround += features[i + 1]; count++;
        if (i >= (size_t)sqrt(size)) surround += features[i - (size_t)sqrt(size)]; count++;
        if ((i + (size_t)sqrt(size)) < size) surround += features[i + (size_t)sqrt(size)]; count++;
        
        surround /= static_cast<float>(count);
        
        // Apply center-surround inhibition
        features[i] = center - strength * surround;
    }
}

void VisionProcessor::combineFeatures(VisionFeatures& output) {
    // Combine all feature maps into coherent representation
    size_t totalFeatures = output.orientedFeatures.size();
    
    // Combine edge features with spike activity
    for (size_t i = 0; i < totalFeatures; ++i) {
        // Weight different features
        float edgeComponent = (output.horizontalEdges[i] + output.verticalEdges[i]) * 0.5f;
        float textureComponent = (output.textureSmoothness[i] + output.textureContrast[i]) * 0.5f;
        
        // Spike rate as timing information
        float temporalComponent = output.spikeRates[i] / pImpl->maxSpikeRate;
        
        // Combine with biological weighting
        output.orientedFeatures[i] = 
            0.4f * edgeComponent + 
            0.3f * textureComponent + 
            0.3f * temporalComponent;
    }
}

void VisionProcessor::generateOutputFeatures() {
    // Generate final feature vector for agent use
    pImpl->processedFeatures.clear();
    pImpl->processedFeatures.reserve(pImpl->features.orientedFeatures.size() * 3);
    
    // Include key features for agent processing
    for (size_t i = 0; i < pImpl->features.orientedFeatures.size(); ++i) {
        pImpl->processedFeatures.push_back(pImpl->features.orientedFeatures[i]);
    }
    
    // Add texture features
    for (size_t i = 0; i < pImpl->features.textureFeatures.size(); ++i) {
        pImpl->processedFeatures.push_back(
            (pImpl->features.textureSmoothness[i] + pImpl->features.textureContrast[i]) * 0.5f
        );
    }
    
    // Add spike rate features for temporal processing
    for (size_t i = 0; i < pImpl->features.spikeRates.size(); ++i) {
        pImpl->processedFeatures.push_back(
            pImpl->features.spikeRates[i] / pImpl->maxSpikeRate
        );
    }
}

void VisionProcessor::reset() {
    // Reset all feature maps
    for (auto& feature : pImpl->features.horizontalEdges) feature = 0.0f;
    for (auto& feature : pImpl->features.verticalEdges) feature = 0.0f;
    for (auto& feature : pImpl->features.textureSmoothness) feature = 0.0f;
    for (auto& feature : pImpl->features.textureContrast) feature = 0.0f;
    for (auto& feature : pImpl->features.colorFeatures) feature = 0.0f;
    for (auto& feature : pImpl->features.orientedFeatures) feature = 0.0f;
    for (auto& feature : pImpl->features.motionFeatures) feature = 0.0f;
    for (auto& feature : pImpl->features.spikeRates) feature = 0.0f;
    
    pImpl->processedFeatures.clear();
    
    // Reset neural components
    if (pImpl->retinaRegion) pImpl->retinaRegion->reset();
    if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
}

void VisionProcessor::setParameters(float edgeThreshold, float textureSensitivity, 
                                   float maxSpikeRate, float inhibitionStrength) {
    pImpl->edgeThreshold = edgeThreshold;
    pImpl->textureSensitivity = textureSensitivity;
    pImpl->maxSpikeRate = maxSpikeRate;
    pImpl->inhibitionStrength = inhibitionStrength;
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->processedFeatures;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->processedFeatures.size();
}

const std::vector<float>& VisionProcessor::getEdgeFeatures() const {
    // Return combined edge features
    static std::vector<float> combinedEdges;
    combinedEdges.clear();
    combinedEdges.reserve(pImpl->features.horizontalEdges.size() + 
                         pImpl->features.verticalEdges.size());
    
    combinedEdges.insert(combinedEdges.end(), 
                        pImpl->features.horizontalEdges.begin(),
                        pImpl->features.horizontalEdges.end());
    combinedEdges.insert(combinedEdges.end(),
                        pImpl->features.verticalEdges.begin(),
                        pImpl->features.verticalEdges.end());
    
    return combinedEdges;
}

const std::vector<float>& VisionProcessor::getTextureFeatures() const {
    // Return combined texture features
    static std::vector<float> combinedTexture;
    combinedTexture.clear();
    combinedTexture.reserve(pImpl->features.textureSmoothness.size() + 
                           pImpl->features.textureContrast.size());
    
    combinedTexture.insert(combinedTexture.end(),
                          pImpl->features.textureSmoothness.begin(),
                          pImpl->features.textureSmoothness.end());
    combinedTexture.insert(combinedTexture.end(),
                          pImpl->features.textureContrast.begin(),
                          pImpl->features.textureContrast.end());
    
    return combinedTexture;
}

const VisionFeatures& VisionProcessor::getInternalFeatures() const {
    return pImpl->features;
}

} // namespace nlm