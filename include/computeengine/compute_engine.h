#include <webgpu/webgpu.h>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif // __EMSCRIPTEN__


class ComputeEngine {
public:
    ComputeEngine();
    ~ComputeEngine();

    void getInstance();
    WGPUAdapter getAdapter();
    WGPUDevice getDevice(WGPUAdapter adapter);
    WGPULimits getDeviceLimits(WGPUDevice device);
    
    void initBindGroupLayout();
    void initComputePipeline();
    void initBuffers();
    void BindGroups();

    bool Initialize();
    void Shutdown();
    void Run();
private:
    WGPUInstance m_instance;
    WGPUDevice m_device;
    WGPULimits m_limits;

    WGPUBindGroup m_bindGroup;
    WGPUBindGroupLayout m_bindGroupLayout;
    WGPUComputePipeline m_pipeline;
    WGPUPipelineLayout m_pipelineLayout;

    WGPUBuffer m_aBuffer;
    WGPUBuffer m_resBuffer;
    WGPUBuffer m_mapBuffer;
};