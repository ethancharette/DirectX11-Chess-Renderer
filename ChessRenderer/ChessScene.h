#ifndef _ChessScene_h
#define _ChessScene_h

#include "DirectX.h"
#include "Font.h"
#include "TextureType.h"
#include "Sprite.h"
#include "IndexedPrimitive.h"
#include "LitColourShader.h"
#include "SkyBox.h"

// all pieces
#include "Board.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "King.h"
#include "Queen.h"

// forward declare the sprite batch

using namespace DirectX;
using namespace DirectX::SimpleMath;


//----------------------------------------------------------------------------------------------
// Main scene class
//	Inherits the directx class to help us initalize directX
//----------------------------------------------------------------------------------------------

class ChessScene : public DirectXClass
{
public:
	// constructor
	ChessScene(HINSTANCE hInstance);
	~ChessScene();

	// Initialize any directX objects
	void InitializeObjects();

	// window message handler
	LRESULT ProcessWindowMessages(UINT msg, WPARAM wParam, LPARAM lParam);

	// Called by the render loop to render a single frame
	void Render(void);

	// Called by directX framework to allow you to update any scene objects
	void Update(float deltaTime);

	// Compute the view and projection matrices
	void ComputeViewProjection();

private:

	SpriteBatch* spritebatch;

	TextureType buttonTex;
	Sprite button;
	FontType text;
	bool inMenu;

	LitColourShader shader;
	SkyBox skyBox;

	float runTime;

	Color leftColor;
	Color rightColor;

	Board chessBoard;

	// White pieces
	Pawn w_pawn;
	Rook w_rook;
	Bishop w_bishop;
	Knight w_knight;
	King w_king;
	Queen w_queen;
	// Black pieces
	Pawn b_pawn;
	Rook b_rook;
	Bishop b_bishop;
	Knight b_knight;
	King b_king;
	Queen b_queen;

	Matrix m_BlackPieces[16];
	Matrix m_WhitePieces[16];

	TextureType diffuseTexWhite;
	TextureType diffuseTexBlack;
	TextureType specTex;


	Matrix viewMatrix;
	Matrix projectionMatrix;

	// mouse variables
	Vector2 mousePos;
	Vector2 mouseDelta;
	bool buttonDown;

	// for camera controls
	Vector3 cameraPos;
	Vector2 cameraRotationSpeed;
	Vector2 cameraRotation;
	float cameraRadius;
	float cameraRadiusSpeed;

	void UpdateCamera(float deltaTime);

	// call when the mouse is released
	void OnMouseDown();
	void OnMouseMove();

};

#endif
