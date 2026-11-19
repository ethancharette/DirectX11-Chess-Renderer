#include "Knight.h"

struct MaterialBuffer
{
	Color ambient;
	Color diffuse;
	Color spec; // alpha in w component

};

// called to initialize the object
void Knight::Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader, Color* color)
{
	SetColor(color);
	pShader = pLitShader;

	torus.InitializeGeometry(pDevice, Torus);
	torus.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());

	cylinder.InitializeGeometry(pDevice, Cylinder);
	cylinder.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());

	sphere.InitializeGeometry(pDevice, Sphere);
	sphere.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());

	cube.InitializeGeometry(pDevice, Cube);
	cube.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());


	baseMatrix = Matrix::CreateScale(1, 0.5, 1) * Matrix::CreateTranslation(0, 0.25, 0);
	bodyMatrix = Matrix::CreateScale(0.5f, 1.75, 0.7f) * Matrix::CreateRotationX(-25 * (XM_PI / 180)) * Matrix::CreateTranslation(0, 01, -0.2);
	headMatrix = Matrix::CreateScale(1.25) * Matrix::CreateTranslation(0, 2, -0.5);
	faceMatrix = Matrix::CreateScale(0.6f, 1.2, 0.5f) * Matrix::CreateRotationX(115 * (XM_PI / 180)) * Matrix::CreateTranslation(0, 2, 0);


	pBaseBuffer = MakeMaterialBuffer(pDevice, *color, Colors::White.v, Colors::White.v, 2);
}

// called to draw the object
// The parent matrix allows the user to pass in a matrix to transform the entire piece
void Knight::Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
{
	// set all 3 to the diffuse
	pDeviceContext->PSSetShaderResources(0, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(1, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(2, 1, &pDiffuse);

	pDeviceContext->PSSetConstantBuffers(2, 1, &pBaseBuffer);
	pShader->SetAmbientLight(*color);

	pShader->SetShaders(pDeviceContext, baseMatrix * parentMatrix, viewMatrix, projMatrix);
	cylinder.Draw(pDeviceContext);
	torus.Draw(pDeviceContext);

	pShader->SetShaders(pDeviceContext, bodyMatrix * parentMatrix, viewMatrix, projMatrix);
	cube.Draw(pDeviceContext);

	pShader->SetShaders(pDeviceContext, headMatrix * parentMatrix, viewMatrix, projMatrix);
	sphere.Draw(pDeviceContext);

	pShader->SetShaders(pDeviceContext, faceMatrix * parentMatrix, viewMatrix, projMatrix);
	cylinder.Draw(pDeviceContext);
}

// update the object
void Knight::Update(float deltaTime)
{
}

// Helper to make a buffer from the given materials
//
ID3D11Buffer* Knight::MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower)
{
	MaterialBuffer mat;
	mat.ambient = ambient;
	mat.diffuse = diffuse;
	mat.spec = spec;
	mat.spec.w = specPower;

	// Create the constant buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(MaterialBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &mat;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	ID3D11Buffer* pConstantBuffer = nullptr;
	pDevice->CreateBuffer(&bufferDesc, &data, &pConstantBuffer);
	return pConstantBuffer;
}

// constructor
Knight::Knight()
{
	pShader = nullptr;
	pBaseBuffer = nullptr;
	pMiddleBuffer = nullptr;
	pTopBuffer = nullptr;
	pDiffuse = nullptr;
	pSpec = nullptr;
	color = nullptr;	// set in Initialize()
}

// destructor
Knight::~Knight()
{
	if (pBaseBuffer) pBaseBuffer->Release();
	if (pMiddleBuffer) pMiddleBuffer->Release();
	if (pTopBuffer) pTopBuffer->Release();
}
