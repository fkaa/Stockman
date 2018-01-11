#pragma once
#include "RenderPass.h"
#include "../Utility/Shader.h"
#define SHADOW_RESOLUTION 2048

namespace Graphics {
    class ShadowRenderPass : public RenderPass
    {
    public:
        enum ResourcesSlots {
            staticInstanceBuffer, 
            animatedInstanceBuffer, 
            animatedJointsBuffer,
            foliageInstanceBuffer,
            newAnimatedInstanceBuffer,
            newAnimatedJointsBuffer,
        };

        ShadowRenderPass(
            std::initializer_list<ID3D11RenderTargetView*> targets,
            std::initializer_list<ID3D11ShaderResourceView*> resources = {},
            std::initializer_list<ID3D11Buffer*> buffers = {},
            ID3D11DepthStencilView * depthStencil = nullptr);
        virtual ~ShadowRenderPass() {};

        virtual wchar_t* name() const override {
            return L"ShadowRenderPass";
        }

        void update(float deltaTime);
        void render() ;

    private:
        D3D11_VIEWPORT viewport;
        D3D11_VIEWPORT viewportReset;
        Shader forward_plus_vs_static;
        Shader forward_plus_vs_animated;

    };
}