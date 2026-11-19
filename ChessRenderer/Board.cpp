#include "Board.h"

struct MaterialBuffer
{
	Color ambient;
	Color diffuse;
	Color spec; // alpha in w component

};

// called to initialize the object
void Board::Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader)
{
	pShader = pLitShader;

	pWhiteMatBuffer = MakeMaterialBuffer(pDevice, Colors::White.v, Colors::White.v, Colors::White.v, 32);
	pBlackMatBuffer = MakeMaterialBuffer(pDevice, Colors::Black.v, Colors::Black.v, Colors::White.v, 32);

	float tileSize = 2;
	float offset = (8 * tileSize) * 0.5f - tileSize * 0.5f;
	m_tileScale = Matrix::CreateScale(tileSize, 0.5, tileSize);

	for (int x = 0; x < 8; ++x)
	{
		for (int z = 0; z < 8; ++z)
		{
			Vector3 pos = Vector3(x * tileSize - offset, 0, z * tileSize - offset);
			tilePositions[x][z] = pos;
		}
	}

	cube.InitializeGeometry(pDevice, Cube);
	cube.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());

}

// called to draw the object
// The parent matrix allows the user to pass in a matrix to transform the entire piece
void Board::Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
{
	// set all 3 to the diffuse
	pDeviceContext->PSSetShaderResources(0, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(1, 1, &pDiffuse);

	pDeviceContext->PSSetConstantBuffers(2, 1, &pWhiteMatBuffer);
	pShader->SetAmbientLight(Colors::White.v);
	pShader->SetSpecularLight(Colors::White.v, 32);
	pShader->SetDirectionalLight(Colors::White.v, Vector3(0,1,1));
	for (int x = 0; x < 8; ++x)
	{
		for (int y = (x % 2 == 0 ? 0 : 1); y < 8; y += 2)
		{
			Vector3 pos = tilePositions[x][y];
			pShader->SetShaders(pDeviceContext, m_tileScale * Matrix::CreateTranslation(pos) * parentMatrix, viewMatrix, projMatrix);
			cube.Draw(pDeviceContext);
		}
	}
	pDeviceContext->PSSetConstantBuffers(2,1, &pBlackMatBuffer);
	for (int x = 0; x < 8; ++x)
	{
		for (int y = (x % 2 == 0 ? 1 : 0); y < 8; y += 2)
		{
			Vector3 pos = tilePositions[x][y];
			pShader->SetShaders(pDeviceContext, m_tileScale * Matrix::CreateTranslation(pos) * parentMatrix, viewMatrix, projMatrix);
			cube.Draw(pDeviceContext);
		}
	}

}

Vector3 Board::GetBoardPosition(int x, int y) const
{
	if (x < 0 || x >= 8 || y < 0 || y >= 8) return Vector3::Zero;

	return tilePositions[x][y];
}

// Helper to make a buffer from the given materials
//
ID3D11Buffer* Board::MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower)
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
Board::Board()
{
	pShader = nullptr;
	pWhiteMatBuffer = nullptr;
	pBlackMatBuffer = nullptr;
	pDiffuse = nullptr;
	pSpec = nullptr;
}

// destructor
Board::~Board()
{
	if (pWhiteMatBuffer) pWhiteMatBuffer->Release();
	if (pBlackMatBuffer) pBlackMatBuffer->Release();
}
