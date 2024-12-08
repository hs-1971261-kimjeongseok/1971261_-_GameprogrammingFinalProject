////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "inputclass.h"
#include "cameraclass.h"
#include "PNTShaderClass.h"
#include "MyMesh.h"
#include "lightclass.h"
#include "rendertextureclass.h"
#include "textureshaderclass.h"
#include "displayplaneclass.h"
#include "normalmapshaderclass.h"
#include "modelclass.h"
#include "glassshaderclass.h"
#include "glassmodelclass.h"
#include <map>
#include <cstdlib>
#include <ctime>

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*);

private:
	bool Render(float);
	bool RenderSceneToTexture(float, RenderTextureClass*, int, float, float, float, int);

private:
    int answer; int used[3];
    char modelFilename[128];
    map<int, WCHAR*> chooseMap;

    HWND hwnd;

	D3DClass* m_Direct3D;
	CameraClass* m_Camera;

    NormalMapShaderClass* m_NormalMapShader;
    ModelClass* m_Model[3];
	LightClass* m_Light; LightClass* m_Light2;

    GlassModelClass* m_WindowModel;
    GlassModelClass* m_WindowModel2;
    GlassShaderClass* m_GlassShader;

	RenderTextureClass* m_RenderTexture;
	RenderTextureClass* m_RenderTexture2;
    RenderTextureClass* m_ChoosePanel[3];

	TextureShaderClass* m_TextureShader;
	DisplayPlaneClass* m_DisplayPlane;

};

#endif