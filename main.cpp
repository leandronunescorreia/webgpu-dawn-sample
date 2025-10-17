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

    // We clean up the WebGPU instance
    wgpuInstanceRelease(instance);
    wgpuAdapterRelease(adapter);


    return 0;
}