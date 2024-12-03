////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

    void SetDiffuseColor(float, float, float, float);
    void SetSpecularColor(float, float, float, float);
	void SetDirection(float, float, float);
	
	XMFLOAT4 GetDiffuseColor();
    XMFLOAT4 GetSpecularColor();
	XMFLOAT3 GetDirection();
	
private:
    XMFLOAT4 m_diffuseColor;
    XMFLOAT4 m_specularColor;
	XMFLOAT3 m_direction;
};

#endif