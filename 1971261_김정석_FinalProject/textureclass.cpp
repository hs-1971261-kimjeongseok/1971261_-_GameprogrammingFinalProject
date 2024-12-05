////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"


TextureClass::TextureClass() {
    m_texture = 0;
}
TextureClass::TextureClass(const TextureClass&) {

}
TextureClass::~TextureClass() {

}
#include "WICTextureLoader.h"
bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* filename)
{

    DirectX::CreateWICTextureFromFile(device, filename, NULL, &m_textureView);

    return true;
}
void TextureClass::Shutdown() {
    if (m_texture) {
        m_texture->Release();
        m_texture = 0;
    }
}
ID3D11ShaderResourceView* TextureClass::GetTexture() {
    return m_textureView;
}
