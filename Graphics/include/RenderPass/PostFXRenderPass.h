#pragma once
#include "RenderPass.h"
#include "../Utility/Shader.h"
#include "../Utility/PingPongBuffer.h"

namespace Graphics
{
    class PostFXRenderPass : public RenderPass
    {
    public:
        PostFXRenderPass(
            PingPongBuffer *backBuffers,
            ID3D11RenderTargetView *target,
            ID3D11ShaderResourceView *bloomSRV
        );

        virtual wchar_t* name() const override {
            return L"PostFXRenderPass";
        }

        void update(float deltaTime);
        void render() ;

        ~PostFXRenderPass() {}


    private:
        struct PostFXConstants {
            float m_BulletTime;
            float _padding[3];
            DirectX::SimpleMath::Vector3 m_Tint;
            float m_TintProgress;
        } m_Constants;

        Shader m_PostFXShader;
        PingPongBuffer *backBuffers;
        ConstantBuffer<PostFXConstants> m_ConstantsBuffer;

        ID3D11ShaderResourceView *m_BloomSRV;
        ID3D11RenderTargetView *m_Target;
    };
}