#pragma once
#include "RenderPass.h"
#include "../Utility/ShaderResource.h"
#include "../Utility/StructuredBuffer.h"
#include "../Utility/Shader.h"
#include <CommonStates.h>
#include <SimpleMath.h>

namespace Graphics
{

    class GUIRenderPass : public RenderPass
    {
    public:
        GUIRenderPass(ID3D11RenderTargetView * renderTarget);


        // Inherited via RenderPass
        virtual void render() override;

    private:
        Shader spriteShader;

        ID3D11RenderTargetView * renderTarget;
        ID3D11BlendState * blendState;

        struct Vertex
        {
            DirectX::SimpleMath::Vector2 position;
            DirectX::SimpleMath::Vector2 uv;
        };

        StructuredBuffer<Vertex> vertexBuffer;
    };
}