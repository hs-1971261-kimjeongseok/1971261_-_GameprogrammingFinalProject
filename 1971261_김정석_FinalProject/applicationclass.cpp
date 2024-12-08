////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"
#include <random>



ApplicationClass::ApplicationClass()
{
    answer = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model[0] = 0; m_Model[1] = 0; m_Model[2] = 0;
    m_NormalMapShader = 0;

    m_WindowModel = 0; m_WindowModel2 = 0;
    m_GlassShader = 0;

	m_Light = 0; m_Light2 = 0;
	m_RenderTexture = 0; m_RenderTexture2 = 0;
    m_ChoosePanel[0] = 0; m_ChoosePanel[1] = 0; m_ChoosePanel[2] = 0;
	m_TextureShader = 0;
	m_DisplayPlane = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;
    this->hwnd = hwnd;

    char textureFilename1[128], textureFilename2[128];
	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

    m_Light = new LightClass;
    m_Light2 = new LightClass;


	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the camera object.
	m_Camera = new CameraClass;
	m_Camera->Render();

    // Create and initialize the normal map shader object.
    m_NormalMapShader = new NormalMapShaderClass;

    result = m_NormalMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the normal map shader object.", L"Error", MB_OK);
        return false;
    }

    // Set the file name of the model.
    strcpy_s(modelFilename, "data/cube.txt");

    // 선택지로 사용될 보기들의 경로를 할당
    chooseMap[1] = (WCHAR*)L"data/stone01.jpg"; // 아무 문양도 없는 보기
    for (int i = 2; i <= 6; i++) {
        std::wstring path = L"data/choose/choose" + std::to_wstring(i - 1) + L".jpg";
        chooseMap[i] = _wcsdup(path.c_str());
    }



    // 사용될 3개의 보기를 정함(중복되지 않게)
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    used[0] = rand() % 6 + 1;
    used[1] = rand() % 6 + 1;
    while (used[1] == used[0]) { used[1] = rand() % 6 + 1; }
    used[2] = rand() % 6 + 1;
    while (used[2] == used[0] || used[2] == used[1]) { used[2] = rand() % 6 + 1; }

    // 보기에 사용될 3개의 모델을 각각 할당
    for (int i = 0; i < 3; i++) {
        m_Model[i] = new ModelClass;
        result = m_Model[i]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename,
            chooseMap[used[i]], (WCHAR*)L"data/normal01.jpg");
        if (!result) {
            return false;
        }
    }

    answer = rand() % 4; // 정답 결정

    // Set the file name of the window model.
    strcpy_s(modelFilename, "data/square.txt");

    // Create and initialize the window model object.
    m_WindowModel = new GlassModelClass;

    result = m_WindowModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, (char*)"data/glass01.tga", (char*)"data/normal03.tga");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the window model object.", L"Error", MB_OK);
        return false;
    }

    strcpy_s(textureFilename1, "data/ice01.tga");
    strcpy_s(textureFilename2, "data/icebump01.tga");

    // Create and initialize the window model object.
    m_WindowModel2 = new GlassModelClass;

    result = m_WindowModel2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, (char*)"data/ice01.tga", (char*)"data/icebump01.tga");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the window model object.", L"Error", MB_OK);
        return false;
    }

    // Create and initialize the glass shader object.
    m_GlassShader = new GlassShaderClass;

    result = m_GlassShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the glass shader object.", L"Error", MB_OK);
        return false;
    }


	// Create and initialize the render to wtexture object.
	m_RenderTexture = new RenderTextureClass;

	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), 256, 256, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		return false;
	}

    // Create and initialize the render to texture object.
    m_RenderTexture2 = new RenderTextureClass;

    result = m_RenderTexture2->Initialize(m_Direct3D->GetDevice(), 256, 256, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        return false;
    }

    // Create and initialize the render to texture object.
    m_ChoosePanel[0] = new RenderTextureClass;

    result = m_ChoosePanel[0]->Initialize(m_Direct3D->GetDevice(), 256, 256, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        return false;
    }

    // Create and initialize the render to texture object.
    m_ChoosePanel[1] = new RenderTextureClass;

    result = m_ChoosePanel[1]->Initialize(m_Direct3D->GetDevice(), 256, 256, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        return false;
    }

    // Create and initialize the render to texture object.
    m_ChoosePanel[2] = new RenderTextureClass;

    result = m_ChoosePanel[2]->Initialize(m_Direct3D->GetDevice(), 256, 256, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        return false;
    }

	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

	// Create and initialize the display plane object.
	m_DisplayPlane = new DisplayPlaneClass;

	result = m_DisplayPlane->Initialize(m_Direct3D->GetDevice(), 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the display plane object.
	if (m_DisplayPlane)
	{
		m_DisplayPlane->Shutdown();
		delete m_DisplayPlane;
		m_DisplayPlane = 0;
	}

	// Release the texture shader.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the render to texture object.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}
    // Release the render to texture object.
    if (m_RenderTexture2)
    {
        m_RenderTexture2->Shutdown();
        delete m_RenderTexture2;
        m_RenderTexture2 = 0;
    }

    // Release the window model object.
    if (m_WindowModel)
    {
        m_WindowModel->Shutdown();
        delete m_WindowModel;
        m_WindowModel = 0;
    }

    if (m_WindowModel2)
    {
        m_WindowModel2->Shutdown();
        delete m_WindowModel2;
        m_WindowModel2 = 0;
    }

    if (m_NormalMapShader)
    {
        m_NormalMapShader->Shutdown();
        delete m_NormalMapShader;
        m_NormalMapShader = 0;
    }

    // Release the glass shader object.
    if (m_GlassShader)
    {
        m_GlassShader->Shutdown();
        delete m_GlassShader;
        m_GlassShader = 0;
    }

    for (int i = 0; i < 3; i++) {
        // Release the render to texture object.
        if (m_ChoosePanel[i])
        {
            m_ChoosePanel[i]->Shutdown();
            delete m_ChoosePanel[i];
            m_ChoosePanel[i] = 0;
        }

        // Release the model object.
        if (m_Model[i])
        {
            m_Model[i]->Shutdown();
            delete m_Model[i];
            m_Model[i] = 0;
        }
    }

	

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}
	if (m_Light2) {
		delete m_Light2;
		m_Light2 = 0;
	}

	return;
}

bool choose(int input, int answer) { return answer == input; }

static float timeLeft = 0.0f;
static float timeRight = 0.0f;
static float stop = 0.0f;
static bool usedStop = false;
bool ApplicationClass::Frame(InputClass* Input)
{
	static float rotation = 0.0f;
    
    bool result;

    //왼쪽 모델의 조명 세기 
    timeLeft += 0.0174532925f * 1.5f;
    if (timeLeft > 360.0f) { timeLeft = 0.0f; }

    //오른쪽 모델의 조명 세기
    timeRight = timeLeft - 180.0f;

	// Check if the user pressed escape and wants to exit the application.
	if (Input->IsEscapePressed())
	{
		return false;
	}


    if (Input->IsNumOnePressed())
    {
        std::wstring message = L"Wrong. Answer is " + std::to_wstring(answer);
        bool res = choose(1, answer);
        if (!res) {
            MessageBox(hwnd, message.c_str(), L"Error", MB_OK);
        }
        else {
            MessageBox(hwnd, L"Correct!!", L"Information", MB_OK);
        }
        return false;
    }


    if (Input->IsNumTwoPressed())
    {
        std::wstring message = L"Wrong. Answer is " + std::to_wstring(answer);
        bool res = choose(2, answer);
        if (!res) {
            MessageBox(hwnd, message.c_str(), L"Error", MB_OK);
        }
        else {
            MessageBox(hwnd, L"Correct!!", L"Information", MB_OK);
        }
        return false;
    }


    if (Input->IsNumThreePressed())
    {
        std::wstring message = L"Wrong. Answer is " + std::to_wstring(answer);
        bool res = choose(3, answer);
        if (!res) {
            MessageBox(hwnd, message.c_str(), L"Error", MB_OK);
        }
        else {
            MessageBox(hwnd, L"Correct!!", L"Information", MB_OK);
        }
        return false;
    }

    
    

    if (Input->IsSpacebarPressed() && !usedStop) {
        usedStop = true;
    }
	
    if (usedStop) {
        if (stop < 10.0f) {
            stop += 0.0174532925f * 4.5f;
        }
        else {
            rotation -= 0.0174532925f * 4.5f;
        }
    }
    else {
        rotation -= 0.0174532925f * 4.5f;
    }
	// Update the rotation variable each frame.
	
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

    float refractionScale = 0.0f;
    refractionScale -= 0.0174532925f * 5.5f;
    // Update the rotation variable each frame.

    if (refractionScale < 0.0f)
    {
        refractionScale += 360.0f;
    }

	// Render the scene to a render texture.
	result = RenderSceneToTexture(rotation, m_RenderTexture, 1, 0.0f, 0.0f, 0.0f, 0);
	if (!result)
	{
		return false;
	}

	// 두번째 도화지에 그림 그리기

	result = RenderSceneToTexture(-rotation, m_RenderTexture2, 2, 0.0f, 0.0f, 0.0f, 0);
	if (!result)
	{
		return false;
	}

    int answerused = 1;
    // 선택지 3개를 각각 그리기
    for (int i = 0; i < 3; i++) {
        float red = 0.0f; float green = 0.0f; float blue = 0.0f;
        if (i == 0) { red = 0.2f; }
        if (i == 1) { green = 0.2f; }
        if (i == 2) { blue = 0.2f; }

        if (i == answer - 1) {// 정답 번호 판넬이라면 위 2개와 같은 문양
            result = RenderSceneToTexture(0, m_ChoosePanel[i], 0, red, blue, green, 0);
        }
        else {
            result = RenderSceneToTexture(0, m_ChoosePanel[i], 0, red, blue, green, answerused);
            answerused++;
        }
        
        if (!result)
        {
            return false;
        }
    }

	// Render the final graphics scene.

    //refractionscale은 0 ~ 0.05
	result = Render(rotation, (sinf(refractionScale) + 1) / 40.0f);
	if (!result)
	{
		return false;
	}
	return true;
}

bool ApplicationClass::RenderSceneToTexture(float rotation, RenderTextureClass* m_RenderTexture, int idx, 
    float red, float green, float blue, int modelnum)
{

    //화면마다 조명을 다르게 설정한다.
    if(idx == 1){ 
        m_Light->SetDiffuseColor(cosf(timeLeft * 5) * 6,cosf(timeLeft * 5) * 5,cosf(timeLeft * 5) * 4, 1.0f);
        m_Light2->SetDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
        m_Light2->SetDirection(-5.0f, 7.0f, 1.0f);
    }
    else if(idx == 2){ 
        m_Light->SetDiffuseColor(cosf(timeRight * 5) * 5,cosf(timeRight * 5) * 4,cosf(timeRight * 5) * 6, 1.0f);
        m_Light2->SetDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
        m_Light2->SetDirection(-5.0f, 7.0f, 1.0f);
    }
    else{
        m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); 
        m_Light2->SetDiffuseColor(0.3f, 0.2f, 0.3f, 1.0f);
        m_Light2->SetDirection(-5.0f, 7.0f, 1.0f);
    }
    m_Light->SetSpecularColor(0.0f, 1.0f, 0.0f, 1.0f);
    m_Light->SetDirection(-1.0f, -2.0f, 3.0f);


    

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Set the render target to be the render texture and clear it.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

    // 배경색 설정
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), red, blue, green, 1.0f);

	// 화면마다 카메라 위치를 다르게 한다.
    if (idx == 1) { // 왼쪽 화면
        m_Camera->SetPosition(2.5f, 16.0f, 0.0f);
        m_Camera->SetRotation(90.0f + cosf(timeLeft) * 15, 0.0f, 0.0f + cosf(timeLeft * 2) * 21);
        m_Camera->Render();
    }
    else if(idx == 2) { // 오른쪽 화면
        m_Camera->SetPosition(0.0f, 0.0f, -16.0f);
        m_Camera->SetRotation(0.0f + cosf(timeRight * 2) * 7, 0.0f + cosf(timeRight) * 15, 0.0f);
        m_Camera->Render();
    }
    else { // 선택지 3개
        m_Camera->SetPosition(0.0f, 0.0f, -12.0f);
        m_Camera->SetRotation(0.0f, 0.0f, 0.0f);
        m_Camera->Render();
    }
	

	// Get the matrices.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_RenderTexture->GetProjectionMatrix(projectionMatrix);

	// Rotate the world matrix by the rotation value so that the cube will spin.
    if (idx == 1) {
        worldMatrix *= XMMatrixRotationRollPitchYaw(-1.3f + rotation, 0.0f, 0.0f);
    }
    else if (idx == 2) {
        worldMatrix *= XMMatrixRotationRollPitchYaw(0.0f, -1.3f + rotation, 0.0f);
    }
	
	// Render the model using the texture shader.
	m_Model[modelnum]->Render(m_Direct3D->GetDeviceContext());


   
    // 입력받은 숫자에 맞는 모델을 렌더링한다.
    result = m_NormalMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model[modelnum]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model[modelnum]->GetTexture(0), m_Model[modelnum]->GetTexture(1), m_Light->GetDirection(), m_Light->GetDiffuseColor(), 
        m_Light->GetSpecularColor(), m_Light2->GetDirection(), m_Light2->GetDiffuseColor());

	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.  And reset the viewport back to the original.
	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	

	return true;
}

bool ApplicationClass::Render(float rotation, float refractionScale)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    bool result;
	
	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.6f, 0.6f, 0.6f, 1.0f);

	// Set the position of the camera for viewing the display planes with the render textures on them.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_Camera->SetRotation(0.0f, 0.0f, 0.0f);
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);


	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Rotate the world matrix by the rotation value so that the cube will spin.
	worldMatrix *= XMMatrixRotationRollPitchYaw(0.0f, -1.3f + rotation, 0.0f);

	m_Model[0]->Render(m_Direct3D->GetDeviceContext());

	// Setup matrices - Top display plane.
	worldMatrix = XMMatrixTranslation(-1.12f, 0.55f, -6.0f);

    m_WindowModel->Render(m_Direct3D->GetDeviceContext());

    result = m_GlassShader->Render(m_Direct3D->GetDeviceContext(), m_WindowModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_WindowModel->GetTexture(0),
        m_WindowModel->GetTexture(1), m_RenderTexture->GetShaderResourceView(), refractionScale);
    if (!result)
    {
        return false;
    }


    // Set the refraction scale for the glass shader.
    //refractionScale = 0.1f;

	// Setup matrices - Top display plane.
	worldMatrix = XMMatrixTranslation(1.12f, 0.55f, -6.0f);

    m_WindowModel2->Render(m_Direct3D->GetDeviceContext());

    result = m_GlassShader->Render(m_Direct3D->GetDeviceContext(), m_WindowModel2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_WindowModel2->GetTexture(0),
        m_WindowModel2->GetTexture(1), m_RenderTexture2->GetShaderResourceView(), refractionScale);
    if (!result)
    {
        return false;
    }


    for (int i = 0; i < 3; i++) {
        // Setup matrices - Top display plane.
        worldMatrix = XMMatrixTranslation(-3.0f + (i * 3.0f), -2.0f, -2.0f);
        // Render the display plane using the texture shader and the render texture resource.
        m_DisplayPlane->Render(m_Direct3D->GetDeviceContext());

        result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DisplayPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
            m_ChoosePanel[i]->GetShaderResourceView());
        if (!result)
        {
            return false;
        }
    }
    

	m_Direct3D->EndScene();

	return true;
}