#include "Renderer.h"
#include <stdio.h>
#include <Graphics\include\ThrowIfFailed.h>
#include <Engine\Constants.h>
#include "Utility\sizeofv.h"
#include <Keyboard.h>
#include <Singletons\DebugWindow.h>

#include <Singletons\Profiler.h>
#include "RenderQueue.h"

#include "Particles\ParticleSystem.h"
#include "Utility\TextureLoader.h"
#include "CommonStates.h"
#include "MainCamera.h"

#include "RenderPass\AORenderPass.h"
#include "RenderPass\ForwardPlusRenderPass.h"
#include "RenderPass\DepthRenderPass.h"
#include "RenderPass\LightCullRenderPass.h"
#include "RenderPass\ShadowRenderPass.h"
#include "RenderPass\SkyBoxRenderPass.h"
#include "RenderPass\GlowRenderPass.h"
#include "RenderPass\ParticleRenderPass.h"
#include "RenderPass\DepthOfFieldRenderPass.h"
#include "RenderPass\SnowRenderPass.h"
#include "RenderPass\PostFXRenderPass.h"
#include "RenderPass\FancyRenderPass.h"
#include "RenderPass\DebugRenderPass.h"
#include "RenderPass\FogRenderPass.h"
#include "RenderPass\GpuQueryRenderPass.h"
#include <vector>

#include "Utility\DebugDraw.h"


#define ANIMATION_HIJACK_RENDER false
#define USE_OLD_RENDER false

#define MAX_DEBUG_POINTS 10000
#define _INSTANCE_CAP 300

namespace Graphics
{
    std::vector<LightRenderInfo> m_TestLights;
    GpuQueryEnd *m_LightSortQuery;
    GpuQueryEnd *m_ForwardQuery;
    std::vector<float> m_SortTimings;
    std::vector<float> m_ForwardTimings;
    bool Active = false;

    void AddRandomLight()
    {
        LightRenderInfo info = {};
        info.color = DirectX::Colors::Crimson;
        info.intensity = 1;
        info.range = RandomFloat(3.f, 20.f);
        info.position = DirectX::SimpleMath::Vector3 { RandomFloat(-140, 140), RandomFloat(0, 5), RandomFloat(-140, 140) };

        m_TestLights.push_back(info);
    }

    void AddTiming()
    {
        m_SortTimings.push_back(m_LightSortQuery->GetAverage());
        m_ForwardTimings.push_back(m_ForwardQuery->GetAverage());
    }

    void PrintTimings()
    {
        printf("Light Sort:\n");
        for (auto t : m_SortTimings)
            printf("%f\n", t);

        printf("Forward Pass:\n");
        for (auto t : m_ForwardTimings)
            printf("%f\n", t);
    }

    float CameraConfigs[60][2][3] = {
        { { -2.396, 7.113, 109.997 },{ 0.274, -0.044, -0.961 } },
    { { 0.069, 7.113, 89.631 },{ -0.159, -0.110, -0.981 } },
    { { -4.728, 7.113, 78.170 },{ -0.542, -0.117, -0.832 } },
    { { -17.230, 7.113, 69.147 },{ -0.982, -0.120, -0.143 } },
    { { -30.401, 7.113, 69.619 },{ -0.975, -0.125, 0.183 } },
    { { -42.751, 7.113, 71.020 },{ -0.982, -0.134, -0.136 } },
    { { -53.894, 7.113, 65.760 },{ -0.726, -0.124, -0.677 } },
    { { -61.785, 7.113, 54.731 },{ -0.368, -0.125, -0.921 } },
    { { -64.425, 7.113, 42.331 },{ -0.054, -0.125, -0.991 } },
    { { -62.964, 7.113, 29.725 },{ 0.237, -0.125, -0.963 } },
    { { -58.366, 7.113, 18.696 },{ 0.470, -0.125, -0.874 } },
    { { -52.342, 7.113, 9.971 },{ 0.650, -0.125, -0.750 } },
    { { -39.871, 7.113, 2.660 },{ 0.988, -0.125, -0.095 } },
    { { -27.660, 7.113, 5.610 },{ 0.901, -0.125, 0.416 } },
    { { -16.412, 7.113, 12.929 },{ 0.748, -0.125, 0.652 } },
    { { -7.465, 7.113, 21.193 },{ 0.726, -0.071, 0.684 } },
    { { 6.828, 7.113, 35.110 },{ 0.702, 0.181, 0.689 } },
    { { 17.893, 8.592, 42.516 },{ 0.935, -0.077, 0.346 } },
    { { 30.536, 11.955, 44.689 },{ 0.974, -0.222, -0.051 } },
    { { 45.376, 16.221, 38.741 },{ 0.695, -0.314, -0.646 } },
    { { 53.646, 20.406, 25.865 },{ 0.364, -0.371, -0.854 } },
    { { 57.163, 24.024, 13.412 },{ 0.146, -0.432, -0.890 } },
    { { 57.992, 27.762, 0.011 },{ -0.012, -0.486, -0.874 } },
    { { 56.507, 31.338, -12.611 },{ -0.121, -0.527, -0.841 } },
    { { 55.345, 33.462, -20.202 },{ -0.169, -0.555, -0.815 } },
    { { 50.690, 37.533, -33.306 },{ -0.324, -0.705, -0.631 } },
    { { 44.057, 41.268, -44.318 },{ -0.385, -0.773, -0.505 } },
    { { 34.216, 45.291, -53.769 },{ -0.456, -0.833, -0.313 } },
    { { 22.030, 49.668, -59.822 },{ -0.490, -0.858, -0.154 } },
    { { 9.244, 53.518, -61.650 },{ -0.512, -0.859, 0.005 } },
    { { -4.207, 58.120, -57.612 },{ -0.437, -0.859, 0.268 } },
    { { -14.836, 63.169, -48.307 },{ -0.312, -0.860, 0.404 } },
    { { -19.039, 64.744, -39.853 },{ -0.155, -0.861, 0.485 } },
    { { -21.065, 63.988, -27.739 },{ -0.064, -0.862, 0.504 } },
    { { -22.338, 59.812, -15.567 },{ -0.039, -0.862, 0.506 } },
    { { -22.656, 55.633, -3.366 },{ -0.054, -0.854, 0.518 } },
    { { -26.279, 51.181, 11.038 },{ -0.159, -0.846, 0.508 } },
    { { -30.759, 46.839, 25.259 },{ -0.161, -0.845, 0.510 } },
    { { -36.685, 42.690, 38.334 },{ -0.307, -0.840, 0.448 } },
    { { -49.411, 38.238, 47.642 },{ -0.614, -0.787, -0.060 } },
    { { -63.501, 34.488, 42.006 },{ -0.575, -0.745, -0.336 } },
    { { -75.629, 30.717, 33.062 },{ -0.498, -0.644, -0.581 } },
    { { -80.776, 27.090, 20.941 },{ -0.084, -0.539, -0.838 } },
    { { -79.835, 23.136, 7.632 },{ 0.284, -0.438, -0.853 } },
    { { -72.112, 18.711, -3.782 },{ 0.669, -0.394, -0.630 } },
    { { -59.413, 13.932, -11.487 },{ 0.821, -0.342, -0.457 } },
    { { -46.572, 9.399, -20.800 },{ 0.782, -0.239, -0.576 } },
    { { -38.585, 7.062, -29.578 },{ 0.494, -0.071, -0.866 } },
    { { -33.697, 7.062, -42.789 },{ 0.342, -0.037, -0.939 } },
    { { -30.040, 7.062, -59.276 },{ -0.066, -0.087, -0.994 } },
    { { -32.297, 7.062, -69.879 },{ -0.293, -0.087, -0.952 } },
    { { -39.334, 7.062, -83.759 },{ -0.187, -0.134, -0.973 } },
    { { -45.013, 7.062, -89.843 },{ 0.304, -0.175, -0.936 } },
    { { -43.050, 7.062, -103.541 },{ 0.968, -0.187, -0.166 } },
    { { -32.807, 7.062, -112.445 },{ 0.935, -0.182, 0.305 } },
    { { -22.383, 7.062, -113.100 },{ 0.501, -0.175, 0.847 } },
    { { -17.418, 7.062, -99.645 },{ 0.306, -0.170, 0.937 } },
    { { -12.683, 7.062, -86.625 },{ 0.403, -0.162, 0.901 } },
    { { -5.453, 7.062, -76.531 },{ 0.729, -0.156, 0.666 } },
    { { 6.428, 7.062, -69.862 },{ 0.936, -0.156, 0.315 } }
 };

    int deCam = 0;
    void PrintCam()
    {
        deCam++;
        DebugWindow::getInstance()->addLog("#%d\n", deCam);
        auto fwd = Global::mainCamera->getForward();
        auto pos = Global::mainCamera->getPos();
        printf("{ { %.3f, %.3f, %.3f }, { %.3f, %.3f, %.3f } },\n", pos.x, pos.y, pos.z, fwd.x, fwd.y, fwd.z);
    }

    uint32_t zero = 0;
    Renderer::Renderer(
        ID3D11Device * device,
        ID3D11DeviceContext * deviceContext,
        ID3D11RenderTargetView * backBuffer, 
        Camera *camera
    )
        : depthStencil(device, WIN_WIDTH, WIN_HEIGHT)
        , fog(device)
        , foliageShader(device, SHADER_PATH("FoliageShader.hlsl"), VERTEX_DESC)
        , timeBuffer(device)
#pragma endregion
        , foliageInstanceBuffer(device, CpuAccess::Write, INSTANCE_CAP(FoliageRenderInfo))
        , staticInstanceBuffer( device, CpuAccess::Write, INSTANCE_CAP(StaticRenderInfo))

        , animatedInstanceBuffer(device, CpuAccess::Write, INSTANCE_CAP(AnimatedRenderInfo))
        , animatedJointsBuffer(  device, CpuAccess::Write, INSTANCE_CAP(AnimatedRenderInfo))

        , newAnimatedInstanceBuffer(device, CpuAccess::Write, INSTANCE_CAP(NewAnimatedRenderInfo))
        , newAnimatedJointsBuffer(  device, CpuAccess::Write, INSTANCE_CAP(NewAnimatedRenderInfo))


        , fakeBuffers(WIN_WIDTH, WIN_HEIGHT)
#pragma region Shared Shader Resources
        , colorMap(WIN_WIDTH, WIN_HEIGHT)

        , shadowMap(Global::device, SHADOW_RESOLUTION, SHADOW_RESOLUTION)
        

        , lightOpaqueIndexList(CpuAccess::None, INDEX_LIST_SIZE)
        , lightsNew(CpuAccess::Write, INSTANCE_CAP(LightRenderInfo))
        , sun(shadowMap),
#pragma endregion
        DebugAnnotation(nullptr)


#pragma region CaNCeR!
    {
        srand(234234);

        grassTime = 0;
        Global::cStates = newd CommonStates(device);
        Global::comparisonSampler = [&]() {
            ID3D11SamplerState * sampler = nullptr;
            D3D11_SAMPLER_DESC sDesc = {};
            sDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            sDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            sDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
            sDesc.BorderColor[0] = 1;
            sDesc.BorderColor[1] = 1;
            sDesc.BorderColor[2] = 1;
            sDesc.BorderColor[3] = 1;
            sDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
            sDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            sDesc.MaxAnisotropy = 0;
            sDesc.MinLOD = 0;
            sDesc.MaxLOD = D3D11_FLOAT32_MAX;
            sDesc.MipLODBias = 0;

            ThrowIfFailed(device->CreateSamplerState(&sDesc, &sampler));
            return sampler;
        }();
        Global::mirrorSampler = [&]() {
            ID3D11SamplerState * sampler = nullptr;
            D3D11_SAMPLER_DESC sDesc = {};
            sDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
            sDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
            sDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
            sDesc.BorderColor[0] = 1;
            sDesc.BorderColor[1] = 1;
            sDesc.BorderColor[2] = 1;
            sDesc.BorderColor[3] = 1;
            sDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
            sDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            sDesc.MaxAnisotropy = 0;
            sDesc.MinLOD = 0;
            sDesc.MaxLOD = D3D11_FLOAT32_MAX;
            sDesc.MipLODBias = 0;

            ThrowIfFailed(device->CreateSamplerState(&sDesc, &sampler));
            return sampler;
        }();
        Global::transparencyBlendState = [&]() {
            ID3D11BlendState * state = nullptr;
            D3D11_BLEND_DESC blendDesc = { 0 };

            blendDesc.IndependentBlendEnable = false;
            blendDesc.AlphaToCoverageEnable = false;
            blendDesc.RenderTarget[0].BlendEnable = true;
            blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            //blendDesc.RenderTarget[0].LogicOp = D3D11_LOGIC_OP_CLEAR;

            ThrowIfFailed(device->CreateBlendState(&blendDesc, &state));
            return state;
        }();


        { // CaNCeR!
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Format = DXGI_FORMAT_R32G32_UINT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
            desc.Width = (int)ceil(WIN_WIDTH / (float)BLOCK_SIZE);
            desc.Height = (int)ceil(WIN_HEIGHT / (float)BLOCK_SIZE);
            desc.SampleDesc.Count = 1;
            desc.MipLevels = 1;
            desc.ArraySize = 1;

            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            ZeroMemory(&uavDesc, sizeof(uavDesc));
            uavDesc.Format = DXGI_FORMAT_R32G32_UINT;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            ZeroMemory(&srvDesc, sizeof(srvDesc));
            srvDesc.Format = DXGI_FORMAT_R32G32_UINT;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;

            ID3D11Texture2D *texture;
            ThrowIfFailed(device->CreateTexture2D(&desc, nullptr, &texture));
            ThrowIfFailed(device->CreateUnorderedAccessView(texture, &uavDesc, &lightOpaqueGridUAV));
            ThrowIfFailed(device->CreateShaderResourceView(texture, &srvDesc, &lightOpaqueGridSRV));
            SAFE_RELEASE(texture);
        }

        {
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            desc.Width = WIN_WIDTH;
            desc.Height = WIN_HEIGHT;
            desc.SampleDesc.Count = 1;
            desc.MipLevels = 4;
            desc.ArraySize = 1;

            ID3D11Texture2D *texture = nullptr;
            ThrowIfFailed(Global::device->CreateTexture2D(&desc, nullptr, &texture));

            D3D11_RENDER_TARGET_VIEW_DESC renderDesc = {};
            renderDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

            D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc = {};
            resourceDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            resourceDesc.Texture2D.MipLevels = 1;

            for (int i = 0; i < MIP_LEVELS; i++)
            {
                resourceDesc.Texture2D.MostDetailedMip = i;
                renderDesc.Texture2D.MipSlice = i;

                ID3D11ShaderResourceView *srv = nullptr;
                ID3D11RenderTargetView *rtv = nullptr;

                ThrowIfFailed(Global::device->CreateShaderResourceView(texture, &resourceDesc, &srv));
                ThrowIfFailed(Global::device->CreateRenderTargetView(texture, &renderDesc, &rtv));

                m_BloomRTVMipChain.push_back(rtv);
                m_BloomSRVMipChain.push_back(srv);
            }

            resourceDesc.Texture2D.MipLevels = -1;
            resourceDesc.Texture2D.MostDetailedMip = 1;
            renderDesc.Texture2D.MipSlice = 0;
            ThrowIfFailed(Global::device->CreateShaderResourceView(texture, &resourceDesc, &m_BloomSRV));

            SAFE_RELEASE(texture);
        }

        {
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Format = DXGI_FORMAT_R8_UNORM;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            desc.Width = WIN_WIDTH / 2;
            desc.Height = WIN_HEIGHT / 2;
            desc.SampleDesc.Count = 1;
            desc.MipLevels = 1;
            desc.ArraySize = 2;

            ID3D11Texture2D *texture = nullptr;
            ThrowIfFailed(Global::device->CreateTexture2D(&desc, nullptr, &texture));

            D3D11_RENDER_TARGET_VIEW_DESC renderDesc = {};
            renderDesc.Format = DXGI_FORMAT_R8_UNORM;
            renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            renderDesc.Texture2DArray.ArraySize = 2;
            renderDesc.Texture2DArray.FirstArraySlice = 0;
            renderDesc.Texture2DArray.MipSlice = 0;

            D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc = {};
            resourceDesc.Format = DXGI_FORMAT_R8_UNORM;
            resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            resourceDesc.Texture2DArray.ArraySize = 2;
            resourceDesc.Texture2DArray.FirstArraySlice = 0;
            resourceDesc.Texture2DArray.MipLevels = 1;
            resourceDesc.Texture2DArray.MostDetailedMip = 0;

            ThrowIfFailed(Global::device->CreateShaderResourceView(texture, &resourceDesc, &m_AOSlicesSRV));

            for (int i = 0; i < 2; i++)
            {
                resourceDesc.Texture2DArray.ArraySize = 1;
                resourceDesc.Texture2DArray.FirstArraySlice = i;
                renderDesc.Texture2DArray.ArraySize = 1;
                renderDesc.Texture2DArray.FirstArraySlice = i;

                ThrowIfFailed(Global::device->CreateShaderResourceView(texture, &resourceDesc, &m_AOSliceSRV[i]));
                ThrowIfFailed(Global::device->CreateRenderTargetView(texture, &renderDesc, &m_AOSliceRTV[i]));
            }

            SAFE_RELEASE(texture);
        }

#pragma endregion
        HRESULT hr = Global::context->QueryInterface(IID_PPV_ARGS(&DebugAnnotation));
        if (!SUCCEEDED(hr)) {
            // failed to aquire debug annotation
        }

        this->backBuffer = backBuffer;

        viewPort = { 0 };
        viewPort.Width = WIN_WIDTH;
        viewPort.Height = WIN_HEIGHT;
        viewPort.MaxDepth = 1.0f;

        FXSystem = newd ParticleSystem(device, 4096, "Resources/Particles/base.part");

        TextureLoader::get().loadAll();

        DebugWindow::getInstance()->registerCommand("TEST_START", [&](std::vector<std::string> &args)->std::string
        {
            Active = true;

            return "testing";
        });

        DebugWindow::getInstance()->registerCommand("TEST_PRINT", [&](std::vector<std::string> &args)->std::string
        {
            PrintTimings();

            return "printed";
        });

        auto light_sort_query = new GpuQueryBegin();
        auto light_sort_query_end = new GpuQueryEnd(L"Light Sort", light_sort_query);
        auto fwd_query = new GpuQueryBegin();
        auto fwd_query_end = new GpuQueryEnd(L"Forward Pass", fwd_query);

        m_ForwardQuery = fwd_query_end;
        m_LightSortQuery = light_sort_query_end;

		renderPasses =
		{
			newd ParticleDepthRenderPass(depthStencil),
			newd DepthRenderPass(
				{},
				{
					staticInstanceBuffer,
					animatedInstanceBuffer,
					animatedJointsBuffer,
					foliageInstanceBuffer,
					newAnimatedInstanceBuffer,
					newAnimatedJointsBuffer,
				},
				{*Global::mainCamera->getBuffer(), grassTimeBuffer },
				depthStencil
			),
            newd AORenderPass(
                depthStencil,
                std::vector<ID3D11ShaderResourceView *>(m_AOSliceSRV, m_AOSliceSRV + 2),
                std::vector<ID3D11RenderTargetView *>(m_AOSliceRTV, m_AOSliceRTV + 2)
            ),
			newd ShadowRenderPass(
				{},
				{
					staticInstanceBuffer,
					animatedInstanceBuffer,
					animatedJointsBuffer,
					foliageInstanceBuffer,
					newAnimatedInstanceBuffer,
					newAnimatedJointsBuffer,
				},
				{*sun.getLightMatrixBuffer(), grassTimeBuffer },
				shadowMap
			),
            light_sort_query,
			/*newd LightCullRenderPass(
				{},
				{
					depthStencil,
					lightsNew
				},
				{
					*Global::mainCamera->getBuffer()
				},
				nullptr,
				{
					lightOpaqueIndexList,
					lightOpaqueGridUAV
				}
			),*/
            light_sort_query_end,
			newd SkyBoxRenderPass({ fakeBuffers }, {}, { *sun.getGlobalLightBuffer() }, depthStencil, &sun),
			fwd_query,
            newd ForwardPlusRenderPass(
				{
					fakeBuffers,
					m_BloomRTVMipChain[0]
				},
				{
					lightOpaqueIndexList,
					lightOpaqueGridSRV,
					lightsNew,
					shadowMap,
					staticInstanceBuffer,
					animatedInstanceBuffer,
					animatedJointsBuffer,
					foliageInstanceBuffer,
					newAnimatedInstanceBuffer,
					newAnimatedJointsBuffer,
                    m_AOSlicesSRV
				},
				{
					*Global::mainCamera->getBuffer(),
					*sun.getGlobalLightBuffer(),
					*sun.getLightMatrixBuffer(),
					grassTimeBuffer
				},
				depthStencil
			),
			//newd FogRenderPass({fakeBuffers}, {depthStencil}, {}, nullptr),
            newd ParticleRenderPass(
                fakeBuffers,
                lightOpaqueGridSRV, 
                lightOpaqueIndexList, 
                lightsNew, 
                shadowMap, 
                *sun.getGlobalLightBuffer(),
                depthStencil
            ),
            fwd_query_end,

            newd GlowRenderPass(
                m_BloomSRV,
                m_BloomSRVMipChain,
                m_BloomRTVMipChain
            ),
            newd SnowRenderPass(
                {
                    fakeBuffers
                },
                {
                    lightOpaqueIndexList,
                    lightOpaqueGridSRV,
                    lightsNew,
                    shadowMap
                },
                {
                    *sun.getLightMatrixBuffer(),
                    *sun.getGlobalLightBuffer()
                },
                depthStencil
            ),
            newd FancyRenderPass(
                fakeBuffers,
                lightOpaqueGridSRV,
                lightOpaqueIndexList,
                lightsNew,
                shadowMap,
                *sun.getGlobalLightBuffer(),
                depthStencil
            ),
            newd PostFXRenderPass(
                &fakeBuffers,
                backBuffer,
                m_BloomSRV
            ),
            newd DebugRenderPass({backBuffer},{},{*Global::mainCamera->getBuffer()}, depthStencil),
            newd GUIRenderPass({backBuffer}),
        };
    }


    Renderer::~Renderer()
    {
        delete FXSystem;

        delete Global::cStates;
        SAFE_RELEASE(Global::comparisonSampler);
        SAFE_RELEASE(Global::mirrorSampler);
        SAFE_RELEASE(Global::transparencyBlendState);
        SAFE_RELEASE(lightOpaqueGridUAV);
        SAFE_RELEASE(lightOpaqueGridSRV);
        SAFE_RELEASE(DebugAnnotation);
        
        SAFE_RELEASE(m_BloomSRV);

        for (auto srv : m_BloomSRVMipChain)
        {
            SAFE_RELEASE(srv);
        }

        for (auto rtv : m_BloomRTVMipChain)
        {
            SAFE_RELEASE(rtv);
        }

        for (auto & renderPass : renderPasses)
        {
            delete renderPass;
        }

        TextureLoader::get().unloadAll();
        ModelLoader::get().unloadAll();
    }

    int timer = 200;
    int limit = 201;
    int counter = 0;
    DirectX::SimpleMath::Vector3 cPos = { 10, 10, 10 };
    DirectX::SimpleMath::Vector3 cFwd = { 1, 0, 0 };

    void Renderer::update(float deltaTime)
    {
        if (Active) {
            for (auto info : m_TestLights)
                QueueRender(info);

            if (counter != limit)
            if (timer < 60) {
                cPos = DirectX::SimpleMath::Vector3(CameraConfigs[timer][0]);
                cFwd = DirectX::SimpleMath::Vector3(CameraConfigs[timer][1]);
            }

            if (counter != limit)
            if (timer >= 0) timer--;

            if (timer < 0 && counter < limit) {
                printf("#%d...\n", counter);
                AddTiming();
                AddRandomLight();

                timer += 100;
                counter++;

                if (counter == limit) {
                    printf("finished.\n", counter);
                }
            }

            if (counter != limit)
            Global::mainCamera->update(cPos, cFwd, Global::context);
        }




        grassTime += deltaTime;
        grassTimeBuffer.write(Global::context, &grassTime, sizeof(grassTime));
        
        /*
        static StaticRenderInfo infotest;
        infotest.model = Resources::Models::Staff;
        infotest.transform = DirectX::SimpleMath::Matrix::CreateTranslation({0, 10, 0});
        QueueRender(&infotest);
        */

        
        LightRenderInfo lightInfo;
        lightInfo.color = DirectX::Colors::WhiteSmoke;
        lightInfo.intensity = 0.1f;
        lightInfo.position = Global::mainCamera->getPos() + SimpleMath::Vector3(0, 0, 0);
        lightInfo.range = 10;
        QueueRender(lightInfo);

       /* QueueRender([](float dt) -> AnimatedRenderInfo
        {
            static float time = 0;
            AnimatedRenderInfo info;
            info.animationName = "Walk";
            info.animationTimeStamp = time;
            info.model = Resources::Models::SummonUnitWithAnim;
            info.transform = SimpleMath::Matrix::CreateTraanslation(0, 3, -3) * SimpleMath::Matrix::CreateScale(1.0f);
            time += dt;
            if (time > 5) time = 0;
            return info;
        }(deltaTime));*/

		//QueueRender([]()
		//{
		//	StaticRenderInfo info;
		//	info.model = Resources::Models::UnitCube;
		//	info.transform = SimpleMath::Matrix::CreateTranslation(0, -1, 0) * SimpleMath::Matrix::CreateScale(1000, 1, 1000);
		//	info.useGridTexture = true;
		//	return info;
		//}());

       

        FXSystem->update(Global::context, Global::mainCamera, deltaTime);

        writeInstanceBuffers();
        sun.update();
   
        for (auto & renderPass : renderPasses)
        {
            PROFILE_BEGIN(__FUNCSIG__);
            renderPass->update(deltaTime);
            PROFILE_END();
        }

        fakeBuffers.reset();
    }

    void Renderer::render() const
    {
        clear();
        Global::context->RSSetViewports(1, &viewPort);

        for (auto & renderPass : renderPasses)
        {
            if (DebugAnnotation) DebugAnnotation->BeginEvent(renderPass->name());
              renderPass->render();
            if (DebugAnnotation) DebugAnnotation->EndEvent();
        }
    }

    void Renderer::clear() const
    {
        static float clearColor[4] = { 0 };
        static float white[4] = { 1 };
        Global::context->ClearRenderTargetView(backBuffer, clearColor);
        //fakeBuffers.clear();
        Global::context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH, 1.f, 0);
        Global::context->ClearDepthStencilView(shadowMap, D3D11_CLEAR_DEPTH, 1.f, 0);
        for (auto rtv : m_BloomRTVMipChain)
        {
            Global::context->ClearRenderTargetView(rtv, clearColor);
        }
    }

    void Renderer::writeInstanceBuffers()
    {
        foliageInstanceBuffer.write([](StaticInstance * instanceBuffer)
        {
            for (auto & model_infos : RenderQueue::get().getQueue<FoliageRenderInfo>())
            {
                for (auto & sinfo : model_infos.second)
                {
                    StaticInstance instance = {};
                    instance.world = sinfo.transform;
                    instance.worldInvT = sinfo.transform.Invert().Transpose();
                    instance.color = sinfo.color;
                    instance.useGridTexture = sinfo.useGridTexture;
                    *instanceBuffer++ = instance;
                }
            }
        });

        staticInstanceBuffer.write([](StaticInstance * instanceBuffer)
        {
            auto & queue = RenderQueue::get().getQueue<StaticRenderInfo>();
            for (auto & model_infos : RenderQueue::get().getQueue<StaticRenderInfo>())
            {
                for (auto & sinfo : model_infos.second)
                {
                    StaticInstance instance = {};
                    instance.world = sinfo.transform;
                    instance.worldInvT = sinfo.transform.Invert().Transpose();
                    instance.color = sinfo.color;
                    instance.useGridTexture = sinfo.useGridTexture;
                    *instanceBuffer++ = instance;
                }
            }
        });

        { // Animation Buffers
            auto instanceBuffer = animatedInstanceBuffer.map();
            auto jointsBuffer   = animatedJointsBuffer.map();

            for (auto & model_infos : RenderQueue::get().getQueue<AnimatedRenderInfo>())
            {
                HybrisLoader::Skeleton * skeleton = &ModelLoader::get().getModel((Resources::Models::Files)model_infos.first)->getSkeleton();

                for (auto & info : model_infos.second)
                {
                    StaticInstance instance = {};
                    instance.world = info.transform;
                    instance.worldInvT = info.transform.Invert().Transpose();
                    instance.color = info.color;
                    instance.useGridTexture = info.useGridTexture;

                    AnimatedJoints joints;
                    if (strlen(info.animationName) != 0)
                    {
                        auto jointTransforms = skeleton->evalAnimation(info.animationName, info.animationTimeStamp);
                        for (size_t i = 0; i < jointTransforms.size(); i++)
                        {
                            joints.jointTransforms[i] = jointTransforms[i];
                        }
                    }

                    *instanceBuffer++ = instance;
                    *jointsBuffer++ = joints;
                }
            } // Animation Buffers

            animatedInstanceBuffer.unmap();
            animatedJointsBuffer.unmap();
        }

        { // New Animation Buffers
            auto instanceBuffer = newAnimatedInstanceBuffer.map();
            auto jointsBuffer   = newAnimatedJointsBuffer.map();

            for (auto & model_infos : RenderQueue::get().getQueue<NewAnimatedRenderInfo>())
            {
                HybrisLoader::Skeleton * skeleton = &ModelLoader::get().getModel((Resources::Models::Files)model_infos.first)->getSkeleton();

                for (auto & info : model_infos.second)
                {
                    StaticInstance instance = {};
                    instance.world = info.transform;
                    instance.worldInvT = info.transform.Invert().Transpose();
                    instance.color = info.color;
                    instance.useGridTexture = info.useGridTexture;

                    AnimatedJoints joints;
                    for (size_t i = 0; i < info.joint_transforms->size(); i++)
                    {
                        joints.jointTransforms[i] = info.joint_transforms->at(i);
                    }

                    *instanceBuffer++ = instance;
                    *jointsBuffer++ = joints;
                }
            } // New Animation Buffers

            newAnimatedInstanceBuffer.unmap();
            newAnimatedJointsBuffer.unmap();
        }

        lightsNew.write([](Light * lightBuffer)
        {
            for (auto & info : RenderQueue::get().getQueue<LightRenderInfo>())
            {
                Light light = {};
                light.range = info.range;
                light.intensity = info.intensity;
                light.color = DirectX::SimpleMath::Vector3(info.color.x, info.color.y, info.color.z);

                light.position = info.position;
                light.viewPosition = DirectX::SimpleMath::Vector4::Transform
                (
                    DirectX::SimpleMath::Vector4(info.position.x, info.position.y, info.position.z, 1.f), 
                    Global::mainCamera->getView()
                );

                Debug::PointLight(light);
                *lightBuffer++ = light;
            }

            for (size_t i = RenderQueue::get().getQueue<LightRenderInfo>().size(); i < INSTANCE_CAP(LightRenderInfo); i++)
            {
                Light light;
                ZeroMemory(&light, sizeof(light));
                *lightBuffer++ = light;
            }
        });
    }


}
