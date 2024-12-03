#include "MyMesh.h"
#include "Bezier.h"

MyMesh::MyMesh() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_texture = 0;
}

MyMesh::MyMesh(const MyMesh&) {

}
MyMesh::~MyMesh() {

}

namespace {
	// Helper for flipping winding of geometric primitives for LH vs. RH coords
	inline void ReverseWinding(IndexCollection& indices, VertexCollection& vertices)
	{
		assert((indices.size() % 3) == 0);
		for (auto it = indices.begin(); it != indices.end(); it += 3)
		{
			std::swap(*it, *(it + 2));
		}

		for (auto it = vertices.begin(); it != vertices.end(); ++it)
		{
			it->textureCoordinate.x = (1.f - it->textureCoordinate.x);
		}
	}
#include "TeapotData.inc"

	void TessellatePatch(VertexCollection& vertices, IndexCollection& indices, TeapotPatch const& patch, size_t tessellation, FXMVECTOR scale, bool isMirrored)
	{
		// Look up the 16 control points for this patch.
		XMVECTOR controlPoints[16];

		for (int i = 0; i < 16; i++)
		{
			controlPoints[i] = XMVectorMultiply(TeapotControlPoints[patch.indices[i]], scale);
		}

		// Create the index data.
		size_t vbase = vertices.size();
		Bezier::CreatePatchIndices(tessellation, isMirrored, [&](size_t index)
			{
				indices.push_back(static_cast<uint16_t>(vbase + index));
			});

		// Create the vertex data.
		Bezier::CreatePatchVertices(controlPoints, tessellation, isMirrored, [&](FXMVECTOR position, FXMVECTOR normal, FXMVECTOR textureCoordinate)
			{
				//vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));
				vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));
			});
	}
}
bool MyMesh::Initialize(ID3D11Device* device, const WCHAR* modelFilename, const WCHAR* texFilename)
{
	VertexCollection vertices;
	IndexCollection indices;
	if (!modelFilename) {
		float size = 5;
		size_t tessellation = 10;
		bool rhcoords = false;
		ComputeTeapot(vertices, indices, size, tessellation, rhcoords);
		//ComputeSphere(vertices, indices, size, tessellation*2, rhcoords, false);
		m_indexCount = (unsigned int)indices.size();
	}
	else {
		// ���߿� load file
		//ComputeTriangle(vertices, indices);
		LoadOBJ(vertices, indices, modelFilename);
		m_indexCount = (unsigned int)indices.size();
	}


	bool result = InitializeBuffers(device, vertices, indices);
	if (!result) return false;

	result = LoadTexture(device, texFilename);
	if (!result) return false;




	return true;
}

void MyMesh::Shutdown() {
	ReleaseTexture();
	ShutdownBuffers();
}

void MyMesh::Render(ID3D11DeviceContext* deviceContext) {
	RenderBuffers(deviceContext);
}

int MyMesh::GetIndexCount() {
	return m_indexCount;
}

ID3D11ShaderResourceView* MyMesh::GetTexture() {
	return m_texture->GetTexture();
}
// vertexbuffer�� indexbuffer�� �����.
bool MyMesh::InitializeBuffers(ID3D11Device* device, const VertexCollection& vertices, const IndexCollection& indices) {

	// gpu memory
	// �����͸� �ؼ��ϴ� ���
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (unsigned int)vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// ���� ������, �޸� ��� ����
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices.data();

	// _vertexBuffer �� ������
	HRESULT result = device->CreateBuffer(&vertexBufferDesc,
		&vertexData, &m_vertexBuffer);

	// index buffer�� ��������
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * (unsigned int)indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = indices.data();

	result = device->CreateBuffer(&indexBufferDesc,
		&indexData, &m_indexBuffer);

	return true;
}
void MyMesh::ShutdownBuffers() {
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}
}
void MyMesh::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// IA: Input Assembler
	// ���� vertex buffer�� �迭�� �ѹ��� set�� �� �ִ�.
	// 0�� �迭�� ������ġ, 1�� ����� ����
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// indexbuffer�� �Ѱ��� set�ϴ� ������ ����
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool MyMesh::LoadTexture(ID3D11Device* device, const WCHAR* filename) {

	m_texture = new TextureClass;
	if (!m_texture)
		return false;

	//bool result = m_texture->Initialize(device, filename);
	//if (!result)
	//	return false;

	return true;
}

void MyMesh::ReleaseTexture() {
	if (m_texture) {
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}

void MyMesh::ComputeTriangle(VertexCollection& vertices, IndexCollection& indices)
{
	vertices.clear();
	vertices.resize(3);

	vertices[0].position = XMFLOAT3(-1, -1, 0);
	vertices[0].normal = XMFLOAT3(0, 0, 1);
	vertices[0].textureCoordinate = XMFLOAT2(0, 1);
	vertices[1].position = XMFLOAT3(0, 1, 0);
	vertices[1].normal = XMFLOAT3(0, 0, 1);
	vertices[1].textureCoordinate = XMFLOAT2(0.5, 0);
	vertices[2].position = XMFLOAT3(1, -1, 0);
	vertices[2].normal = XMFLOAT3(0, 0, 1);
	vertices[2].textureCoordinate = XMFLOAT2(1, 1);

	indices.clear();
	indices.resize(3);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
}
void MyMesh::ComputeTeapot(VertexCollection& vertices, IndexCollection& indices, float size, size_t tessellation, bool rhcoords)
{
	vertices.clear();
	indices.clear();

	//if (tessellation < 1)
	//	throw std::out_of_range("tesselation parameter out of range");

	XMVECTOR scaleVector = XMVectorReplicate(size);

	XMVECTOR scaleNegateX = XMVectorMultiply(scaleVector, g_XMNegateX);
	XMVECTOR scaleNegateZ = XMVectorMultiply(scaleVector, g_XMNegateZ);
	XMVECTOR scaleNegateXZ = XMVectorMultiply(scaleVector, XMVectorMultiply(g_XMNegateX, g_XMNegateZ));

	for (size_t i = 0; i < _countof(TeapotPatches); i++)
	{
		TeapotPatch const& patch = TeapotPatches[i];

		// Because the teapot is symmetrical from left to right, we only store
		// data for one side, then tessellate each patch twice, mirroring in X.
		TessellatePatch(vertices, indices, patch, tessellation, scaleVector, false);
		TessellatePatch(vertices, indices, patch, tessellation, scaleNegateX, true);

		if (patch.mirrorZ)
		{
			// Some parts of the teapot (the body, lid, and rim, but not the
			// handle or spout) are also symmetrical from front to back, so
			// we tessellate them four times, mirroring in Z as well as X.
			TessellatePatch(vertices, indices, patch, tessellation, scaleNegateZ, true);
			TessellatePatch(vertices, indices, patch, tessellation, scaleNegateXZ, false);
		}
	}

	// Built RH above
	if (!rhcoords)
		ReverseWinding(indices, vertices);
}


void MyMesh::ComputeBox(VertexCollection& vertices, IndexCollection& indices, const XMFLOAT3& size, bool rhcoords, bool invertn)
{
	vertices.clear();
	indices.clear();

	// A box has six faces, each one pointing in a different direction.
	const int FaceCount = 6;

	static const XMVECTORF32 faceNormals[FaceCount] =
	{
		{ { {  0,  0,  1, 0 } } },
		{ { {  0,  0, -1, 0 } } },
		{ { {  1,  0,  0, 0 } } },
		{ { { -1,  0,  0, 0 } } },
		{ { {  0,  1,  0, 0 } } },
		{ { {  0, -1,  0, 0 } } },
	};

	static const XMVECTORF32 textureCoordinates[4] =
	{
		{ { { 1, 0, 0, 0 } } },
		{ { { 1, 1, 0, 0 } } },
		{ { { 0, 1, 0, 0 } } },
		{ { { 0, 0, 0, 0 } } },
	};

	XMVECTOR tsize = XMLoadFloat3(&size);
	tsize = XMVectorDivide(tsize, g_XMTwo);

	// Create each face in turn.
	for (int i = 0; i < FaceCount; i++)
	{
		XMVECTOR normal = faceNormals[i];

		// Get two vectors perpendicular both to the face normal and to each other.
		XMVECTOR basis = (i >= 4) ? g_XMIdentityR2 : g_XMIdentityR1;

		XMVECTOR side1 = XMVector3Cross(normal, basis);
		XMVECTOR side2 = XMVector3Cross(normal, side1);

		// Six indices (two triangles) per face.
		size_t vbase = vertices.size();
		indices.push_back((unsigned int)vbase + 0);
		indices.push_back((unsigned int)vbase + 1);
		indices.push_back((unsigned int)vbase + 2);

		indices.push_back((unsigned int)vbase + 0);
		indices.push_back((unsigned int)vbase + 2);
		indices.push_back((unsigned int)vbase + 3);

		// Four vertices per face.
		// (normal - side1 - side2) * tsize // normal // t0
		vertices.push_back(VertexPositionNormalTexture(XMVectorMultiply(XMVectorSubtract(XMVectorSubtract(normal, side1), side2), tsize), normal, textureCoordinates[0]));

		// (normal - side1 + side2) * tsize // normal // t1
		vertices.push_back(VertexPositionNormalTexture(XMVectorMultiply(XMVectorAdd(XMVectorSubtract(normal, side1), side2), tsize), normal, textureCoordinates[1]));

		// (normal + side1 + side2) * tsize // normal // t2
		vertices.push_back(VertexPositionNormalTexture(XMVectorMultiply(XMVectorAdd(normal, XMVectorAdd(side1, side2)), tsize), normal, textureCoordinates[2]));

		// (normal + side1 - side2) * tsize // normal // t3
		vertices.push_back(VertexPositionNormalTexture(XMVectorMultiply(XMVectorSubtract(XMVectorAdd(normal, side1), side2), tsize), normal, textureCoordinates[3]));
	}

	// Build RH above
	if (!rhcoords)
		ReverseWinding(indices, vertices);

	//if (invertn)
	//	InvertNormals(vertices);
}

void MyMesh::ComputeSphere(VertexCollection& vertices, IndexCollection& indices, float diameter, size_t tessellation, bool rhcoords, bool invertn)
{
	vertices.clear();
	indices.clear();

	//if (tessellation < 3)
	//	throw std::out_of_range("tesselation parameter out of range");

	size_t verticalSegments = tessellation;
	size_t horizontalSegments = tessellation * 2;

	float radius = diameter / 2;

	// Create rings of vertices at progressively higher latitudes.
	for (size_t i = 0; i <= verticalSegments; i++)
	{
		float v = 1 - float(i) / verticalSegments;

		float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
		float dy, dxz;

		XMScalarSinCos(&dy, &dxz, latitude);

		// Create a single ring of vertices at this latitude.
		for (size_t j = 0; j <= horizontalSegments; j++)
		{
			float u = float(j) / horizontalSegments;

			float longitude = j * XM_2PI / horizontalSegments;
			float dx, dz;

			XMScalarSinCos(&dx, &dz, longitude);

			dx *= dxz;
			dz *= dxz;

			XMVECTOR normal = XMVectorSet(dx, dy, dz, 0);
			XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

			vertices.push_back(VertexPositionNormalTexture(XMVectorScale(normal, radius), normal, textureCoordinate));
		}
	}

	// Fill the index buffer with triangles joining each pair of latitude rings.
	size_t stride = horizontalSegments + 1;

	for (size_t i = 0; i < verticalSegments; i++)
	{
		for (size_t j = 0; j <= horizontalSegments; j++)
		{
			size_t nextI = i + 1;
			size_t nextJ = (j + 1) % stride;

			indices.push_back((unsigned int)(i * stride + j));
			indices.push_back((unsigned int)(nextI * stride + j));
			indices.push_back((unsigned int)(i * stride + nextJ));

			indices.push_back((unsigned int)(i * stride + nextJ));
			indices.push_back((unsigned int)(nextI * stride + j));
			indices.push_back((unsigned int)(nextI * stride + nextJ));
		}
	}

	// Build RH above
	if (!rhcoords)
		ReverseWinding(indices, vertices);

	//if (invertn)
	//	InvertNormals(vertices);
}

#include "OBJ_Loader.h"

unsigned int MyMesh::LoadOBJ(VertexCollection& vertices, IndexCollection& indices, const WCHAR* modelFilename) {

	vertices.clear();
	indices.clear();

	// Initialize Loader
	objl::Loader Loader;

	// �����ڵ� ��θ� �� ��Ƽ����Ʈ�� ĳ����
	std::wstring message_w = modelFilename;
	std::string path;
	path.assign(message_w.begin(), message_w.end());

	// Load .obj File very very slow because of file io
	bool loadout = Loader.LoadFile(path);
	if (!loadout)
		return 0; // error

	// �������� mesh�� ������ ���
	int nTris = 0;
	for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {

		// Copy one of the loaded meshes to be our current mesh
		objl::Mesh curMesh = Loader.LoadedMeshes[i];
		// ���� ������ŭ loop
		for (int j = 0; j < curMesh.Vertices.size(); j++) {

			// memcpy(v, curMesh.Vertices[j], sizeof(VertexPositionNormalTexture)); // �̰͵� ����
			XMFLOAT3 p(&curMesh.Vertices[j].Position.X);
			XMFLOAT3 normal(&curMesh.Vertices[j].Normal.X);
			XMFLOAT2 texC(&curMesh.Vertices[j].TextureCoordinate.X);
			VertexPositionNormalTexture v(p, normal, texC);

			vertices.push_back(v);
		}

		for (int j = 0; j < curMesh.Indices.size(); j++) {
			indices.push_back(curMesh.Indices[j]);
		}
		nTris = curMesh.Indices.size() / 3;
		// curMesh.MeshMaterial //���� ��� ����
	}

	return nTris;
}