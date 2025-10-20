#include "computeengine/compute_engine.h"

#include <webgpu/webgpu.h>


#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <atomic>
#include <chrono>

ComputeEngine::ComputeEngine(): 
m_device(nullptr), m_limits({}) {}

ComputeEngine::~ComputeEngine() {
    Shutdown();
    m_device = nullptr;
    m_limits = {};
}

WGPUAdapter ComputeEngine::createInstanceAndAdapter() {
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    this->m_instance = wgpuCreateInstance(nullptr);
#else //  WEBGPU_BACKEND_EMSCRIPTEN
    this->m_instance = wgpuCreateInstance(&desc);
#endif //  WEBGPU_BACKEND_EMSCRIPTEN

    if (!this->m_instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        throw std::runtime_error("Could not initialize WebGPU instance");
    }
    std::cout << "WebGPU instance " << std::hex << this->m_instance << std::dec << " created successfully." << std::endl;
    
	WGPURequestAdapterOptions adapterOpts = {};
	adapterOpts.nextInChain = nullptr;
    adapterOpts.backendType = WGPUBackendType_Vulkan;  // or Metal / D3D12 / OpenGL
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
    callbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
    callbackInfo.callback = onAdapterRequestEnded;
    callbackInfo.userdata1 = (void*)&adapter;
    // We request the adapter
	wgpuInstanceRequestAdapter(
		this->m_instance /* equivalent of navigator.gpu */,
		&adapterOpts,
        callbackInfo
	);

    if (!adapter) {
        std::cerr << "Could not initialize the Adapter!" << std::endl;
        throw std::runtime_error("Could not initialize WebGPU adapter");
    }

	std::cout << "Got adapter: " << adapter << std::endl;

    return adapter;
}


WGPUDevice ComputeEngine::getDevice(WGPUAdapter& adapter) {
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

    auto uncapturedErrorCallback = [](WGPUDevice const * device, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void* userdata1, WGPU_NULLABLE void* userdata2) {
        std::cout << "Device " << std::hex << device << std::dec << " uncaptured error callback invoked, type: " << std::hex << type << std::dec;
        std::cout << ", message: " << std::string(message.data, message.length);
        if(userdata1 || userdata2) {
            std::cout << ", userdata not null!";
        }
        std::cout << std::endl;
    };
    WGPUUncapturedErrorCallbackInfo uncapturedErrorCallbackInfo = {};
    uncapturedErrorCallbackInfo.nextInChain = nullptr;
    uncapturedErrorCallbackInfo.callback = uncapturedErrorCallback;
    uncapturedErrorCallbackInfo.userdata1 = nullptr;
    uncapturedErrorCallbackInfo.userdata2 = nullptr;

    deviceDesc.deviceLostCallbackInfo = deviceLostCallbackInfo;
    deviceDesc.uncapturedErrorCallbackInfo = uncapturedErrorCallbackInfo;

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


    WGPURequestDeviceCallbackInfo callbackInfoDevice = {};
    callbackInfoDevice.nextInChain = nullptr;
    callbackInfoDevice.mode = WGPUCallbackMode_AllowSpontaneous;
    callbackInfoDevice.callback = onDeviceRequestEnded;
    callbackInfoDevice.userdata1 = (void*)&this->m_device;
    callbackInfoDevice.userdata2 = nullptr;
	wgpuAdapterRequestDevice(
		adapter,
		&deviceDesc,
		callbackInfoDevice
	);

    if (!this->m_device) {
        std::cerr << "Could not initialize the Device!" << std::endl;
        throw std::runtime_error("Could not initialize WebGPU device");
    }

    std::cout << "WebGPU device " << std::hex << this->m_device << std::dec << " created successfully." << std::endl;

    wgpuAdapterRelease(adapter);

    return m_device;
}

WGPULimits ComputeEngine::getDeviceLimits(WGPUDevice& device) {
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
    
    WGPUStatus status = WGPUStatus_Error;
#ifdef WEBGPU_BACKEND_DAWN
	status = wgpuDeviceGetLimits(device, &limits);
#else
	status = wgpuDeviceGetLimits(device, &limits);
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

    return limits;
}

bool ComputeEngine::Initialize() {

    WGPUAdapter adapter = createInstanceAndAdapter();
    this->m_device = getDevice(adapter);
    this->m_limits = getDeviceLimits(this->m_device);

    if (!this->m_device) {
        std::cerr << "Failed to initialize WebGPU device!" << std::endl;
        return false;
    }

    std::cout << "Device " << std::hex << this->m_device << std::dec << " initialized successfully." << std::endl;

	this->initBindGroupLayout();
	this->initComputePipeline();
	this->initBuffers();
	this->BindGroups();

    return true;
}

void ComputeEngine::initBindGroupLayout() {

    WGPUBindGroupLayoutEntry layoutEntryA = {};
    layoutEntryA.binding = 0;
    layoutEntryA.visibility = WGPUShaderStage_Compute;
    layoutEntryA.buffer.type = WGPUBufferBindingType_ReadOnlyStorage;

    WGPUBindGroupLayoutEntry layoutEntryC = {};
    layoutEntryC.binding = 1;
    layoutEntryC.visibility = WGPUShaderStage_Compute;
    layoutEntryC.buffer.type = WGPUBufferBindingType_Storage;

    WGPUBindGroupLayoutEntry bindings[] = {layoutEntryA, layoutEntryC};

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc;
	bindGroupLayoutDesc.entryCount = (uint32_t)std::size(bindings);
	bindGroupLayoutDesc.entries = bindings;
    
	this->m_bindGroupLayout = wgpuDeviceCreateBindGroupLayout(this->m_device, &bindGroupLayoutDesc);
}

void ComputeEngine::initComputePipeline() {
static const char shaderCode[] = R"(
@group(0) @binding(0) var<storage, read> inputBuffer: array<f32>;
@group(0) @binding(1) var<storage, read_write> outputBuffer: array<f32>;

fn f(x: f32) -> f32 {
    return 2.0 * x + 1.0;
}

@compute @workgroup_size(32)
fn computeMain(@builtin(global_invocation_id) id: vec3<u32>) {
    let idx = id.x;
    outputBuffer[idx] = f(inputBuffer[idx]);
}
)";

    WGPUShaderModuleWGSLDescriptor wgslDesc = {};
    wgslDesc.code = WGPUStringView{shaderCode, static_cast<uint32_t>(std::strlen(shaderCode))};

    WGPUShaderModuleDescriptor smDesc = {};
    smDesc.label = WGPUStringView{"Compute Shader Module", 22};
    smDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&wgslDesc);
    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(this->m_device, &smDesc);


    WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.nextInChain = nullptr;
    pipelineLayoutDesc.label = WGPUStringView{"Compute Pipeline Layout", 24};
    pipelineLayoutDesc.bindGroupLayoutCount = 1;
    pipelineLayoutDesc.bindGroupLayouts = &this->m_bindGroupLayout;

    this->m_pipelineLayout = wgpuDeviceCreatePipelineLayout(this->m_device, &pipelineLayoutDesc);


    WGPUConstantEntry constantEntry = {};
    constantEntry.nextInChain = nullptr;
    constantEntry.key = WGPUStringView{"group_size", 256};
    constantEntry.value = static_cast<double>(256);

    WGPUComputeState computeState = {};
    computeState.nextInChain = nullptr;
    computeState.module = shaderModule;
    computeState.entryPoint = WGPUStringView{"computeMain", 10};
    computeState.constants = &constantEntry;
    computeState.constantCount = 1;

    WGPUComputePipelineDescriptor pipelineDesc = {
        .nextInChain = nullptr,
        .label = WGPUStringView{"Compute Pipeline", 16},
        .layout = this->m_pipelineLayout,
        .compute = computeState
    };

    this->m_pipeline = wgpuDeviceCreateComputePipeline(this->m_device, &pipelineDesc);    
}

void ComputeEngine::initBuffers() {
    const int32_t numVals = 1024;

    WGPUBufferDescriptor aBufferDesc = {
        .nextInChain = nullptr,
        .label = WGPUStringView{"A Buffer", 8},
        .usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst,
        .size = numVals * sizeof(uint32_t),
        .mappedAtCreation = false
    };
    this->m_aBuffer = wgpuDeviceCreateBuffer(this->m_device, &aBufferDesc);


    WGPUBufferDescriptor resBufferDesc = {
        .nextInChain = nullptr,
        .label = WGPUStringView{"Result Buffer", 13},
        .usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc,
        .size = numVals * sizeof(uint32_t),
        .mappedAtCreation = false
    };
    this->m_resBuffer = wgpuDeviceCreateBuffer(this->m_device, &resBufferDesc);

	// Create an intermediary buffer to which we copy the output and that can be
	// used for reading into the CPU memory.
	resBufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
	this->m_mapBuffer = wgpuDeviceCreateBuffer(this->m_device, &resBufferDesc);

}

void ComputeEngine::BindGroups() {
    const int32_t numVals = 1024;

    WGPUBindGroupEntry aBufBinding = {};
    aBufBinding.binding = 0;
    aBufBinding.buffer = this->m_aBuffer;
    aBufBinding.offset = 0;
    aBufBinding.size =  (size_t)numVals * sizeof(uint32_t);

    WGPUBindGroupEntry resBufBinding = {};
    resBufBinding.binding = 1;
    resBufBinding.buffer = this->m_resBuffer;
    resBufBinding.offset = 0;
    resBufBinding.size = (size_t)numVals * sizeof(uint32_t);

    WGPUBindGroupEntry entries[] = {aBufBinding, resBufBinding};

    WGPUBindGroupDescriptor bindGroupDesc = {};
    bindGroupDesc.layout = m_bindGroupLayout;
    bindGroupDesc.entryCount = (uint32_t)std::size(entries);
    bindGroupDesc.entries = entries;
	this->m_bindGroup = wgpuDeviceCreateBindGroup(m_device, &bindGroupDesc);
}

void ComputeEngine::Run() {
    const int32_t numVals = 1024;
    // const uint64_t groupSize = 256;
    // bool timeSupport = false;

    // Create the data arrays
    std::vector<uint32_t> aArray(static_cast<size_t>(numVals));
    std::vector<uint32_t> res(1u, 0u);

    // Initialize arrays
    for (int32_t i = 0; i < numVals; ++i) {
        aArray[static_cast<size_t>(i)] = static_cast<uint32_t>(i + 1);
    }


    WGPUQueue queue = wgpuDeviceGetQueue(this->m_device);
    std::cout << "Using queue " << std::hex << queue << std::dec << std::endl;
    wgpuQueueWriteBuffer(queue, m_aBuffer, 0, aArray.data(), aArray.size() * sizeof(uint32_t));


    WGPUCommandEncoderDescriptor cmdEncDesc = {
        .nextInChain = nullptr,
        .label = WGPUStringView{"My Command Encoder", 18}
    };

    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(this->m_device, &cmdEncDesc);
    std::cout << "Created command encoder " << std::hex << encoder << std::dec << std::endl;

    WGPUComputePassDescriptor computePassDesc = {};
    computePassDesc.nextInChain = nullptr;
    computePassDesc.timestampWrites = nullptr;
    computePassDesc.label = WGPUStringView{"Compute Pass", 12};

    WGPUComputePassEncoder computePass = wgpuCommandEncoderBeginComputePass(encoder, &computePassDesc);
    std::cout << "Created compute pass encoder " << std::hex << computePass << std::dec << std::endl;

    wgpuComputePassEncoderSetPipeline(computePass, this->m_pipeline);
    wgpuComputePassEncoderSetBindGroup(computePass, 0, this->m_bindGroup, 0, nullptr);

	uint32_t invocationCount = size_t(numVals) * sizeof(uint32_t) / sizeof(float);
	uint32_t workgroupSize = 32;
	// This ceils invocationCount / workgroupSize
	uint32_t workgroupCount = (invocationCount + workgroupSize - 1) / workgroupSize;
	wgpuComputePassEncoderDispatchWorkgroups(computePass, workgroupCount, 1, 1);

    wgpuComputePassEncoderEnd(computePass);
    wgpuCommandEncoderCopyBufferToBuffer(encoder, this->m_resBuffer, 0, this->m_mapBuffer, 0, size_t(numVals) * sizeof(uint32_t));

    WGPUCommandBufferDescriptor cmdBufferDesc = {};
    cmdBufferDesc.label = WGPUStringView{"CommandBuffer", 13};
    WGPUCommandBuffer commands = wgpuCommandEncoderFinish(encoder, &cmdBufferDesc);

    // -----------------------------------------------------------------------------
    // Map the result buffer to CPU
    // -----------------------------------------------------------------------------

    bool done = false;

    // Create callback info struct
    WGPUBufferMapCallbackInfo callbackInfo = {};
    callbackInfo.nextInChain = nullptr;
    callbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;

    // The callback will be invoked when mapping is done
    callbackInfo.callback =
        +[](WGPUMapAsyncStatus status,
            WGPUStringView message,
            void* userdata1,
            void* userdata2) {
            (void)userdata2;
            bool* done = reinterpret_cast<bool*>(userdata1);
            if (status == WGPUMapAsyncStatus_Success) {
                std::cout << "Buffer mapping successful!" << std::endl;
            } else {
                std::cerr << "Buffer mapping failed: "
                        << std::string(message.data, message.length)
                        << std::endl;
            }
            *done = true;
        };
    callbackInfo.userdata1 = &done;

    // Request async map
    wgpuBufferMapAsync(this->m_mapBuffer, WGPUMapMode_Read, 0,
                    numVals * sizeof(uint32_t), callbackInfo);

    // Poll Dawn’s event loop until done
    while (!done) {
    #ifdef WEBGPU_BACKEND_WGPU
        wgpuQueueSubmit(queue, 0, nullptr);
    #else
        wgpuInstanceProcessEvents(this->m_instance);
    #endif
    }

    // -----------------------------------------------------------------------------
    // Once mapping succeeds, read the mapped range
    // -----------------------------------------------------------------------------
    const uint32_t* outputData =
        static_cast<const uint32_t*>(
            wgpuBufferGetConstMappedRange(this->m_mapBuffer, 0,
                                        numVals * sizeof(uint32_t)));

    for (int i = 0; i < numVals; ++i) {
        std::cout << "input[" << i << "] processed -> " << outputData[i] << std::endl;
    }

    // Unmap to release the mapping
    wgpuBufferUnmap(this->m_mapBuffer);

    // -----------------------------------------------------------------------------
    // Cleanup (optional if you're using smart wrappers)
    // -----------------------------------------------------------------------------
    #if !defined(WEBGPU_BACKEND_WGPU)
    wgpuCommandBufferRelease(commands);
    wgpuCommandEncoderRelease(encoder);
    wgpuQueueRelease(queue);
    #endif
    }


    void ComputeEngine::Shutdown() {

        wgpuBindGroupRelease(this->m_bindGroup);
        wgpuBindGroupLayoutRelease(this->m_bindGroupLayout);
        wgpuComputePipelineRelease(this->m_pipeline);
        wgpuPipelineLayoutRelease(this->m_pipelineLayout);

        wgpuBufferDestroy(m_mapBuffer);
        wgpuBufferRelease(m_mapBuffer);
        wgpuBufferRelease(m_aBuffer);

        wgpuBufferDestroy(m_resBuffer);
        wgpuBufferRelease(m_resBuffer);

        wgpuInstanceRelease(this->m_instance);
        wgpuDeviceRelease(this->m_device);
}