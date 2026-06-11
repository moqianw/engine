# Vulkan Backend 完整架构设计


## 设计目标

Vulkan 后端需要同时满足：

- 多窗口 / 多 viewport / 编辑器嵌入视口
- 高性能资源管理
- 安全的 GPU 生命周期
- 清晰的提交路径
- 后续支持 FrameGraph
- 后续支持 Bindless
- 后续支持 GPU-driven rendering
- 后续支持多线程 command recording

核心原则：

```text
Qt / platform 只提供 NativeWindowHandle
qvk / VulkanBackend 不依赖 Qt
Renderer 不直接持有裸 Vulkan 对象
Renderer 不直接 vkDestroy
Submit 统一进入 SubmissionSystem
资源销毁统一进入 LifetimeQueue
PresentationTarget 管理每个可呈现目标
VulkanCore 只管理 Vulkan 设备层核心对象
```

## 总体结构

```text
VulkanBackend
├─ VulkanCore
│  ├─ VulkanInstance
│  ├─ VulkanAdapter
│  ├─ VulkanDevice
│  ├─ QueueRegistry
│  └─ VulkanCapabilities
│
├─ PresentationSystem
│  └─ PresentTargetPool
│     ├─ PresentTarget #0
│     │  ├─ VulkanSurface
│     │  ├─ VulkanSwapchain
│     │  └─ PresentQueueRef
│     ├─ PresentTarget #1
│     │  ├─ VulkanSurface
│     │  ├─ VulkanSwapchain
│     │  └─ PresentQueueRef
│     └─ ...
│
├─ MemorySystem
│  ├─ VulkanAllocator
│  ├─ UploadRing
│  ├─ ReadbackRing
│  ├─ FrameAllocator
│  └─ TransientAllocator
│
├─ ResourceSystem
│  ├─ BufferTable
│  ├─ ImageTable
│  ├─ ImageViewTable
│  ├─ SamplerTable
│  └─ ShaderTable
│
├─ DescriptorSystem
│  ├─ DescriptorLayoutCache
│  ├─ DescriptorAllocator
│  ├─ FrameDescriptorArena
│  ├─ BindlessTextureHeap
│  ├─ BindlessSamplerHeap
│  └─ DescriptorUpdateQueue
│
├─ PipelineSystem
│  ├─ ShaderModuleCache
│  ├─ PipelineLayoutCache
│  ├─ GraphicsPipelineCache
│  ├─ ComputePipelineCache
│  └─ PipelineCacheBlob
│
├─ CommandSystem
│  ├─ FrameCommandContext
│  ├─ ThreadCommandContext[]
│  └─ CommandPoolRegistry
│
├─ SyncSystem
│  ├─ FrameSync
│  ├─ TimelineSemaphore
│  ├─ BarrierBuilder
│  └─ ResourceStateTracker
│
├─ SubmissionSystem
│  ├─ GraphicsQueueSubmitter
│  ├─ ComputeQueueSubmitter
│  ├─ TransferQueueSubmitter
│  └─ PresentSubmitter
│
├─ LifetimeQueue
├─ VulkanGraphExecutor
└─ DebugSystem
   ├─ Validation
   ├─ DebugMessenger
   ├─ ObjectName
   ├─ GpuMarker
   └─ Stats
```

## 目录结构

```text
include/runtime/vk/
├─ VulkanBackend.hpp
│
├─ core/
│  ├─ VulkanInstance.hpp
│  ├─ VulkanAdapter.hpp
│  ├─ VulkanDevice.hpp
│  ├─ VulkanQueue.hpp
│  ├─ QueueRegistry.hpp
│  ├─ VulkanCapabilities.hpp
│  ├─ VulkanFeatureQuery.hpp
│  ├─ types.hpp
│  └─ utils.hpp
│
├─ present/
│  ├─ PresentTypes.hpp
│  ├─ PresentTarget.hpp
│  ├─ PresentationSystem.hpp
│  ├─ VulkanSurface.hpp
│  └─ VulkanSwapchain.hpp
│
├─ memory/
│  ├─ VulkanAllocator.hpp
│  ├─ UploadRing.hpp
│  ├─ ReadbackRing.hpp
│  ├─ FrameAllocator.hpp
│  └─ TransientAllocator.hpp
│
├─ resource/
│  ├─ ResourceHandle.hpp
│  ├─ ResourceTable.hpp
│  ├─ ResourceSystem.hpp
│  ├─ VulkanBuffer.hpp
│  ├─ VulkanImage.hpp
│  ├─ VulkanImageView.hpp
│  ├─ VulkanSampler.hpp
│  └─ VulkanShader.hpp
│
├─ descriptor/
│  ├─ DescriptorSystem.hpp
│  ├─ DescriptorLayoutCache.hpp
│  ├─ DescriptorAllocator.hpp
│  ├─ FrameDescriptorArena.hpp
│  ├─ BindlessTextureHeap.hpp
│  ├─ BindlessSamplerHeap.hpp
│  └─ DescriptorUpdateQueue.hpp
│
├─ pipeline/
│  ├─ PipelineSystem.hpp
│  ├─ ShaderModuleCache.hpp
│  ├─ PipelineLayoutCache.hpp
│  ├─ GraphicsPipelineCache.hpp
│  ├─ ComputePipelineCache.hpp
│  └─ PipelineCacheBlob.hpp
│
├─ command/
│  ├─ CommandSystem.hpp
│  ├─ CommandContext.hpp
│  ├─ CommandPoolRegistry.hpp
│  └─ CommandBatch.hpp
│
├─ sync/
│  ├─ FrameSync.hpp
│  ├─ TimelineSemaphore.hpp
│  ├─ BarrierBuilder.hpp
│  ├─ ResourceStateTracker.hpp
│  └─ LifetimeQueue.hpp
│
├─ submit/
│  ├─ SubmissionSystem.hpp
│  ├─ QueueSubmitter.hpp
│  └─ PresentSubmitter.hpp
│
├─ graph_executor/
│  ├─ VulkanGraphExecutor.hpp
│  ├─ VulkanBarrierCompiler.hpp
│  ├─ VulkanTransientResourceAllocator.hpp
│  └─ VulkanQueueScheduler.hpp
│
└─ debug/
   ├─ VulkanDebugUtils.hpp
   ├─ VulkanObjectName.hpp
   ├─ VulkanGpuMarker.hpp
   └─ VulkanStats.hpp
```

对应 `src/runtime/vk/` 保持同样目录结构。
