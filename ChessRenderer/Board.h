#ifndef _BOARD_H
#define _BOARD_H

#include "DirectX.h"
#include "IndexedPrimitive.h"
#include "LitColourShader.h"
#include <d3d11_1.h>
#include <SimpleMath.h>

using namespace DirectX;

class Board
{
public:

	Board();
	~Board();

	// called to initialize the object
	void Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader);

	// called to draw the object
	void Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix);

	void SetTextures(ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* spec) {
		pDiffuse = diffuse;
		pSpec = spec;
	}

	Vector3 GetBoardPosition(int x, int y) const;

private:

	ID3D11Buffer* MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower);

	ID3D11ShaderResourceView* pDiffuse;
	ID3D11ShaderResourceView* pSpec;

	// store a copy of the shader
	LitColourShader* pShader;

	// store each object
	IndexedPrimitive cube;

	Matrix m_tileScale;

	ID3D11Buffer* pWhiteMatBuffer;
	ID3D11Buffer* pBlackMatBuffer;

	Vector3 tilePositions[8][8];
};

#endif

