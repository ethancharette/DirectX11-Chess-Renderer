#include "Rook.h"

struct MaterialBuffer
{
	Color ambient;
	Color diffuse;
	Color spec; // alpha in w component

};

// called to initialize the object
void Rook::Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader, Color* color)
{
	SetColor(color);
	pShader = pLitShader;

	cube.InitializeGeometry(pDevice, Cube);
	cube.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());

	torus.InitializeGeometry(pDevice, Torus);
	torus.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());

	cylinder.InitializeGeometry(pDevice, Cylinder);
	cylinder.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());

	baseMatrix = Matrix::CreateScale(1,0.5,1) * Matrix::CreateTranslation(0, 0.25, 0);
	middleMatrix = Matrix::CreateScale(0.75, 1, 0.75) * Matrix::CreateTranslation(0,0.75,0);
	topMatrix = Matrix::CreateScale(1, 0.25, 1) * Matrix::CreateTranslation(0,1.4,0);

	pBaseBuffer = MakeMaterialBuffer(pDevice, *color, Colors::White.v, Colors::White.v, 2);

}

// called to draw the object
// The parent matrix allows the user to pass in a matrix to transform the entire piece
void Rook::Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
{
	// set all 3 to the diffuse
	pDeviceContext->PSSetShaderResources(0, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(1, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(2, 1, &pDiffuse);

	pDeviceContext->PSSetConstantBuffers(2, 1, &pBaseBuffer);
	pShader->SetAmbientLight(*color);

	pShader->SetShaders(pDeviceContext, baseMatrix * parentMatrix, viewMatrix, projMatrix);
	torus.Draw(pDeviceContext);

	pShader->SetShaders(pDeviceContext, middleMatrix * parentMatrix, viewMatrix, projMatrix);
	cylinder.Draw(pDeviceContext);

	pShader->SetShaders(pDeviceContext, topMatrix * parentMatrix, viewMatrix, projMatrix);
	cube.Draw(pDeviceContext);

}

// update the object
void Rook::Update(float deltaTime)
{
}

// Helper to make a buffer from the given materials
//
ID3D11Buffer* Rook::MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower)
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
Rook::Rook()
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
Rook::~Rook()
{
	if (pBaseBuffer) pBaseBuffer->Release();
	if (pMiddleBuffer) pMiddleBuffer->Release();
	if (pTopBuffer) pTopBuffer->Release();
}
