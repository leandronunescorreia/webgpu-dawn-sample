// Includes
#include <webgpu/webgpu.h>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif // __EMSCRIPTEN__

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdio>


const char* WGPUFeatureNameToString(WGPUFeatureName name) {
    switch (name) {
        case WGPUFeatureName_Undefined: return "WGPUFeatureName_Undefined";
        case WGPUFeatureName_DepthClipControl: return "WGPUFeatureName_DepthClipControl";
        case WGPUFeatureName_Depth32FloatStencil8: return "WGPUFeatureName_Depth32FloatStencil8";
        case WGPUFeatureName_TimestampQuery: return "WGPUFeatureName_TimestampQuery";
        case WGPUFeatureName_TextureCompressionBC: return "WGPUFeatureName_TextureCompressionBC";
        case WGPUFeatureName_TextureCompressionBCSliced3D: return "WGPUFeatureName_TextureCompressionBCSliced3D";
        case WGPUFeatureName_TextureCompressionETC2: return "WGPUFeatureName_TextureCompressionETC2";
        case WGPUFeatureName_TextureCompressionASTC: return "WGPUFeatureName_TextureCompressionASTC";
        case WGPUFeatureName_TextureCompressionASTCSliced3D: return "WGPUFeatureName_TextureCompressionASTCSliced3D";
        case WGPUFeatureName_IndirectFirstInstance: return "WGPUFeatureName_IndirectFirstInstance";
        case WGPUFeatureName_ShaderF16: return "WGPUFeatureName_ShaderF16";
        case WGPUFeatureName_RG11B10UfloatRenderable: return "WGPUFeatureName_RG11B10UfloatRenderable";
        case WGPUFeatureName_BGRA8UnormStorage: return "WGPUFeatureName_BGRA8UnormStorage";
        case WGPUFeatureName_Float32Filterable: return "WGPUFeatureName_Float32Filterable";
        case WGPUFeatureName_Float32Blendable: return "WGPUFeatureName_Float32Blendable";
        case WGPUFeatureName_ClipDistances: return "WGPUFeatureName_ClipDistances";
        case WGPUFeatureName_DualSourceBlending: return "WGPUFeatureName_DualSourceBlending";
        case WGPUFeatureName_Force32: return "WGPUFeatureName_Force32";
        default: return "Unknown";
    }
}

void inspectDevice(WGPUDevice device) {
	WGPUSupportedFeatures* supported = new WGPUSupportedFeatures();
	wgpuDeviceGetFeatures(device, supported);

	std::cout << "Device features:" << std::endl;
	std::cout << std::hex;
	for (size_t i = 0; i < supported->featureCount; ++i) {
		auto f = supported->features[i];
        std::cout << std::hex;
		std::cout << " - 0x" << f << std::endl;
	}
	std::cout << std::dec;

	WGPULimits limits = {};
	limits.nextInChain = nullptr;

#ifdef WEBGPU_BACKEND_DAWN
	bool success = wgpuDeviceGetLimits(device, &limits) == WGPUStatus_Success;
#else
	WGPUStatus status = wgpuDeviceGetLimits(device, &limits);
#endif
	
	if (status == WGPUStatus_Success) {
		std::cout << "Device limits:" << std::endl;
		std::cout << " - maxTextureDimension1D: " << limits.maxTextureDimension1D << std::endl;
		std::cout << " - maxTextureDimension2D: " << limits.maxTextureDimension2D << std::endl;
		std::cout << " - maxTextureDimension3D: " << limits.maxTextureDimension3D << std::endl;
		std::cout << " - maxTextureArrayLayers: " << limits.maxTextureArrayLayers << std::endl;
		std::cout << " - maxBindGroups: " << limits.maxBindGroups << std::endl;
		std::cout << " - maxDynamicUniformBuffersPerPipelineLayout: " << limits.maxDynamicUniformBuffersPerPipelineLayout << std::endl;
		std::cout << " - maxDynamicStorageBuffersPerPipelineLayout: " << limits.maxDynamicStorageBuffersPerPipelineLayout << std::endl;
		std::cout << " - maxSampledTexturesPerShaderStage: " << limits.maxSampledTexturesPerShaderStage << std::endl;
		std::cout << " - maxSamplersPerShaderStage: " << limits.maxSamplersPerShaderStage << std::endl;
		std::cout << " - maxStorageBuffersPerShaderStage: " << limits.maxStorageBuffersPerShaderStage << std::endl;
		std::cout << " - maxStorageTexturesPerShaderStage: " << limits.maxStorageTexturesPerShaderStage << std::endl;
		std::cout << " - maxUniformBuffersPerShaderStage: " << limits.maxUniformBuffersPerShaderStage << std::endl;
		std::cout << " - maxUniformBufferBindingSize: " << limits.maxUniformBufferBindingSize << std::endl;
		std::cout << " - maxStorageBufferBindingSize: " << limits.maxStorageBufferBindingSize << std::endl;
		std::cout << " - minUniformBufferOffsetAlignment: " << limits.minUniformBufferOffsetAlignment << std::endl;
		std::cout << " - minStorageBufferOffsetAlignment: " << limits.minStorageBufferOffsetAlignment << std::endl;
		std::cout << " - maxVertexBuffers: " << limits.maxVertexBuffers << std::endl;
		std::cout << " - maxVertexAttributes: " << limits.maxVertexAttributes << std::endl;
		std::cout << " - maxVertexBufferArrayStride: " << limits.maxVertexBufferArrayStride << std::endl;
		std::cout << " - maxInterStageShaderComponents: " << limits.maxInterStageShaderVariables << std::endl;
		std::cout << " - maxComputeWorkgroupStorageSize: " << limits.maxComputeWorkgroupStorageSize << std::endl;
		std::cout << " - maxComputeInvocationsPerWorkgroup: " << limits.maxComputeInvocationsPerWorkgroup << std::endl;
		std::cout << " - maxComputeWorkgroupSizeX: " << limits.maxComputeWorkgroupSizeX << std::endl;
		std::cout << " - maxComputeWorkgroupSizeY: " << limits.maxComputeWorkgroupSizeY << std::endl;
		std::cout << " - maxComputeWorkgroupSizeZ: " << limits.maxComputeWorkgroupSizeZ << std::endl;
		std::cout << " - maxComputeWorkgroupsPerDimension: " << limits.maxComputeWorkgroupsPerDimension << std::endl;
	}
}

int main (int, char**) {
    // We create a descriptor
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;
    
    // We create the instance using this descriptor
    #ifdef WEBGPU_BACKEND_EMSCRIPTEN
    WGPUInstance instance = wgpuCreateInstance(nullptr);
    #else //  WEBGPU_BACKEND_EMSCRIPTEN
    WGPUInstance instance = wgpuCreateInstance(&desc);
    #endif //  WEBGPU_BACKEND_EMSCRIPTEN

    // We can check whether there is actually an instance created
    if (!instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return 1;
    }
    
    // Display the object (WGPUInstance is a simple pointer, it may be
    // copied around without worrying about its size).
    std::cout << "WGPU instance: " << instance << std::endl;

	WGPURequestAdapterOptions adapterOpts = {};
	adapterOpts.nextInChain = nullptr;
	WGPUAdapter adapter = {};

	auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, WGPU_NULLABLE void* userdata1, WGPU_NULLABLE void* userdata2) {
		WGPUAdapter& userData = *reinterpret_cast<WGPUAdapter*>(userdata1);
        if(userdata2 || message.length > 0) {
            std::cout << "Userdata2 is not null!" << std::endl;
        }
		if (status == WGPURequestAdapterStatus_Success) {
			userData = adapter;
		} else {
            std::cout << "Could not get WebGPU adapter: " << std::endl;
		}
	};
    WGPURequestAdapterCallbackInfo callbackInfo = {};
    callbackInfo.nextInChain = nullptr;
    callbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
    callbackInfo.callback = onAdapterRequestEnded;
    callbackInfo.userdata1 = (void*)&adapter;
    // We request the adapter
	wgpuInstanceRequestAdapter(
		instance /* equivalent of navigator.gpu */,
		&adapterOpts,
        callbackInfo
	);

    if (!adapter) {
        std::cerr << "Could not initialize the Adapter!" << std::endl;
        return 1;
    }

	std::cout << "Got adapter: " << adapter << std::endl;
	WGPUSupportedFeatures* features = new WGPUSupportedFeatures();

	// Call the function a first time with a null return address, just to get
	// the entry count.
	wgpuAdapterGetFeatures(adapter, features);
    for(size_t i = 0; i < features->featureCount; ++i) {
        printf("%s\n", WGPUFeatureNameToString(features->features[i]));
    }

	std::cout << "Adapter features:" << std::endl;
	std::cout << std::hex; // Write integers as hexadecimal to ease comparison with webgpu.h literals
	for(size_t i = 0; i < features->featureCount; ++i) {
        auto f = features->features[i];
        std::cout << std::hex;
        std::cout << " - 0x" << f << std::endl;
	}    

    delete features;

    WGPUAdapterInfo adapterInfo = {};
    adapterInfo.nextInChain = nullptr;

    wgpuAdapterGetInfo(adapter, &adapterInfo);


    std::cout << "Adapter properties:" << std::endl;
    std::cout << " - vendorID: 0x" << std::hex << adapterInfo.vendorID << std::dec << std::endl;
    std::cout << " - vendor: " << std::string(adapterInfo.vendor.data, adapterInfo.vendor.length) << std::endl;
    std::cout << " - architecture: " << std::string(adapterInfo.architecture.data, adapterInfo.architecture.length) << std::endl;
    std::cout << " - description: " << std::string(adapterInfo.description.data, adapterInfo.description.length) << std::endl;
    std::cout << " - device: " << std::string(adapterInfo.device.data, adapterInfo.device.length) << std::endl;
    std::cout << " - adapterType: 0x" << std::hex << adapterInfo.adapterType << std::dec << std::endl;
    std::cout << " - backendType: 0x" << std::hex << adapterInfo.backendType << std::dec << std::endl;



  
	std::cout << "Requesting device..." << std::endl;
    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = WGPUStringView{"My Device", 9}; // anything works here, that's your call
    deviceDesc.requiredFeatureCount = 0; // we do not require any specific feature
	deviceDesc.requiredLimits = nullptr; // we do not require any specific limit
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = WGPUStringView{"The default queue", 17};


    auto deviceLostCallback = [](WGPUDevice const * device, WGPUDeviceLostReason reason, WGPUStringView message, WGPU_NULLABLE void* userdata1, WGPU_NULLABLE void* userdata2) {
        std::cout << "Device " << std::hex << device << std::dec << " lost callback invoked, reason: " << std::hex << reason << std::dec;
        std::cout << ", message: " << std::string(message.data, message.length);
        if(userdata1 || userdata2) {
            std::cout << ", userdata not null!";
        }
        std::cout << std::endl;
    };

    WGPUDeviceLostCallbackInfo deviceLostCallbackInfo = {};
    deviceLostCallbackInfo.nextInChain = nullptr;
    deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
    deviceLostCallbackInfo.callback = deviceLostCallback;
    deviceLostCallbackInfo.userdata1 = nullptr;
    deviceLostCallbackInfo.userdata2 = nullptr;

    deviceDesc.deviceLostCallbackInfo = deviceLostCallbackInfo;


	// A function that is invoked whenever the device stops being available.
	auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, WGPU_NULLABLE void* userdata1, WGPU_NULLABLE void* userdata2) {
		WGPUDevice& userData = *reinterpret_cast<WGPUDevice*>(userdata1);
		if (status == WGPURequestDeviceStatus_Success) {
			userData = device;
		} else {
			std::cout << "Could not get WebGPU device: " << std::string(message.data, message.length) << std::endl;
		}
        if(userdata2 || message.length > 0) {
            std::cout << "Userdata2 is not null!" << std::endl;
        }        
	};

    WGPUDevice device = {};
    WGPURequestDeviceCallbackInfo callbackInfoDevice = {};
    callbackInfoDevice.nextInChain = nullptr;
    callbackInfoDevice.mode = WGPUCallbackMode_WaitAnyOnly;
    callbackInfoDevice.callback = onDeviceRequestEnded;
    callbackInfoDevice.userdata1 = (void*)&device;
    callbackInfoDevice.userdata2 = nullptr;
	wgpuAdapterRequestDevice(
		adapter,
		&deviceDesc,
		callbackInfoDevice
	);
	std::cout << "Got device: " << device << std::endl;

    inspectDevice(device);

    wgpuInstanceRelease(instance);
    wgpuAdapterRelease(adapter);
	wgpuDeviceRelease(device);

    return 0;
}