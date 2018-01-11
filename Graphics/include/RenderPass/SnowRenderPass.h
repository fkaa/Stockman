#pragma once
#include "RenderPass.h"
#include "../Utility/Shader.h"
#include "../Utility/StructuredBuffer.h"
#include <vector>

namespace Graphics 
{
    class SnowRenderPass : public RenderPass
    {
    public:
        SnowRenderPass(
            std::initializer_list<ID3D11RenderTargetView*> targets,
            std::initializer_list<ID3D11ShaderResourceView*> resources = {},
            std::initializer_list<ID3D11Buffer*> buffers = {},
            ID3D11DepthStencilView * depthStencil = nullptr);
        virtual ~SnowRenderPass() { SAFE_RELEASE(m_SnowFlakeSRV); };

        virtual wchar_t* name() const override {
            return L"SnowRenderPass";
        }

        void update(float deltaTime);
        void render() ;


    private:
        struct SnowFlake
        {
            DirectX::SimpleMath::Vector3 position;
            float randomRot;
            float distance;
        };

        StructuredBuffer<SnowFlake> snowBuffer;
        Shader snowShader;
        int snowFlakeCount;

        ID3D11ShaderResourceView *m_SnowFlakeSRV;
        std::vector<SnowFlake> snowFlakes;
        void addRandomSnowFlake();
        void moveSnowFlake(int snowFlake);
        void clearSnow();
        void initializeSnowflakes();
    };
}