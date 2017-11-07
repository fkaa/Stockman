#include "GUIRenderPass.h"
#include "../RenderQueue.h"
#include "../Device.h"
#include "../Utility/sizeofv.h"
#include "../Utility/TextureLoader.h"
#include "../CommonStates.h"

Graphics::GUIRenderPass::GUIRenderPass(std::initializer_list<ID3D11RenderTargetView*> targets, std::initializer_list<ID3D11ShaderResourceView*> resources, std::initializer_list<ID3D11Buffer*> buffers, ID3D11DepthStencilView * depthStencil)
    : RenderPass(targets, resources, buffers, depthStencil)
    , spriteShader(Resources::Shaders::SpriteShader)
    , vertexBuffer(CpuAccess::Write, INSTANCE_CAP(SpriteRenderInfo) * 4)
{
    this->renderTarget = renderTarget;

    this->sBatch = std::make_unique<DirectX::SpriteBatch>(context);

    this->sFonts[0] = std::make_unique<DirectX::SpriteFont>(device, convertFontFilePath(Resources::Fonts::KG14).c_str());
    this->sFonts[1] = std::make_unique<DirectX::SpriteFont>(device, convertFontFilePath(Resources::Fonts::KG18).c_str());
    this->sFonts[2] = std::make_unique<DirectX::SpriteFont>(device, convertFontFilePath(Resources::Fonts::KG26).c_str());
}

Graphics::GUIRenderPass::~GUIRenderPass()
{
    sBatch.reset();
    for (size_t i = 0; i < NR_OF_FONTS; i++)
    {
        sFonts[i].reset();
    }
    
}

void Graphics::GUIRenderPass::render() const
{
    enum { TL, TR, BL, BR};
    Global::context->IASetInputLayout(nullptr);
    Global::context->PSSetShader(spriteShader, nullptr, 0);
    Global::context->VSSetShader(spriteShader, nullptr, 0);
    Global::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Global::context->RSSetState(Global::cStates->CullNone());

    Global::context->VSSetShaderResources(0, 1, vertexBuffer);

    ID3D11SamplerState * linear = Global::cStates->LinearClamp();
    Global::context->PSSetSamplers(0, 1, &linear);

    Global::context->OMSetRenderTargets(1, &renderTarget, nullptr);
    Global::context->OMSetBlendState(Global::cStates->AlphaBlend(), NULL, -1);

    std::vector<Vertex> vertices(4);
    size_t offset = 0;
    for (auto & info : RenderQueue::get().getQueue<SpriteRenderInfo>())
    {
        Global::context->PSSetShaderResources(2, 1, *TextureLoader::get().getTexture(info->texture));
        Global::context->Draw(4, offset);
        offset += 4;
    }

    for (auto & info : RenderQueue::get().getQueue<TextRenderInfo>())
    {
    }
}

void Graphics::GUIRenderPass::update(float deltaTime)
{
    enum { TL, TR, BL, BR};

    std::vector<Vertex> vertices(4);


    size_t offset = 0;
    auto ptr = vertexBuffer.map(Global::context);
    for (auto & info : RenderQueue::get().getQueue<SpriteRenderInfo>())
    {
        Global::context->PSSetShaderResources(2, 1, *TextureLoader::get().getTexture(info->texture));

        using namespace DirectX::SimpleMath;

        float TL_X = (info->screenRect.topLeft.x * 2 - 1);
        float TL_Y = (info->screenRect.topLeft.y * 2 - 1);
        float BR_X = (info->screenRect.bottomRight.x * 2 - 1);
        float BR_Y = (info->screenRect.bottomRight.y * 2 - 1);
        vertices[TL].position = Vector2(TL_X, TL_Y * -1);
        vertices[TR].position = Vector2(BR_X, TL_Y * -1);
        vertices[BL].position = Vector2(TL_X, BR_Y * -1);
        vertices[BR].position = Vector2(BR_X, BR_Y * -1);

        float TL_UV_X = info->textureRect.topLeft.x;
        float TL_UV_Y = info->textureRect.topLeft.y;
        float BR_UV_X = info->textureRect.bottomRight.x;
        float BR_UV_Y = info->textureRect.bottomRight.y;
        vertices[TL].uv = Vector2(TL_UV_X, TL_UV_Y);
        vertices[TR].uv = Vector2(BR_UV_X, TL_UV_Y);
        vertices[BL].uv = Vector2(TL_UV_X, BR_UV_Y);
        vertices[BR].uv = Vector2(BR_UV_X, BR_UV_Y);

        vertexBuffer.write(context, vertices.data(), sizeofv(vertices));
        context->Draw(4, 0);
    }

    //textRender();
}

//render the queued text
void Graphics::GUIRenderPass::textRender()
{
    sBatch->Begin(DirectX::SpriteSortMode_Deferred);
    for (auto & info : RenderQueue::get().getQueue<TextRenderInfo>())
    {
        if (isDrawableString(info->text))
        {
            sFonts[info->font]->DrawString(sBatch.get(), info->text, info->position, info->color);
        }
        
    }
    sBatch->End();
}

std::wstring Graphics::GUIRenderPass::convertFontFilePath(Resources::Fonts::Files input)
{

    const char* orig = Resources::Fonts::Paths.at(input);
    size_t size = std::strlen(orig);
    std::wstring converted;
    converted.resize(size);
    std::mbstowcs(&converted[0], orig, size);
    return converted;
}

//checks fow unallowed characters that chrashed the text draw
bool Graphics::GUIRenderPass::isDrawableString(const wchar_t * text) const
{
    for (size_t i = 0; i < std::wcslen(text); i++)
    {
        if (text[i] > 127 || text[i] < 32)
        {
            return false;
        }
    }
    return true;
}
