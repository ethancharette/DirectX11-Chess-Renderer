#include "ChessScene.h"
#include <Windowsx.h> // for GET__LPARAM macros
#include <d3d11_1.h>
#include <SimpleMath.h>
#include <DirectXColors.h>
#include <sstream>
#include <CommonStates.h>


using namespace DirectX;
using namespace DirectX::SimpleMath;

// helper function
//
// returns a random float between 0 & 1
float RandFloat() { return float(rand())/float(RAND_MAX); }

static const float CAMERA_SPEED = XM_PI * 0.2f;

// starting camera values (also used when the view is reset)
static const float CAMERA_START_RADIUS = 32.0f;
static const float CAMERA_START_PITCH = XM_PI / 10.0f;

//----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	// WIC (used to load .png/.jpg textures) requires COM to be initialized
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		return 0;

	ChessScene application(hInstance);    // Create the class variable

	if( application.InitWindowsApp(L"DirectX 11 Chess Renderer", nShowCmd) == false )    // Initialize the window, if all is well show and update it so it displays
	{
		return 0;                   // Error creating the window, terminate application
	}

	if( application.InitializeDirect3D() )
	{
		application.SetDepthStencil(true);      // Tell DirectX class to create and maintain a depth stencil buffer
		application.InitializeObjects();
		application.MessageLoop();				// Window has been successfully created, start the application message loop
	}

	return 0;
}

//----------------------------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------------------------
ChessScene::ChessScene(HINSTANCE hInstance)
	: DirectXClass(hInstance)
{
	spritebatch = NULL;
	mousePos = Vector2(clientWidth * 0.5f, clientHeight * 0.5f);
	buttonDown = false;

	ClearColor = Color(DirectX::Colors::Black.v);

	cameraPos = Vector3(0, 0, 6);

	cameraRadius = CAMERA_START_RADIUS;
	cameraRadiusSpeed = 0;

	cameraRotation = Vector2(0, CAMERA_START_PITCH);
	cameraRotationSpeed = Vector2::Zero;

	runTime = 0;
	inMenu = true;
}

//----------------------------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------------------------
ChessScene::~ChessScene()
{
	delete spritebatch;
}

//----------------------------------------------------------------------------------------------
// Initialize any fonts we need to use here
//----------------------------------------------------------------------------------------------
void ChessScene::InitializeObjects()
{
	spritebatch = new DirectX::SpriteBatch(DeviceContext);

	buttonTex.Load(D3DDevice, DeviceContext, L"..\\Textures\\panel.png");
	button.Initialize(&buttonTex, Vector2(clientWidth * 0.5f, clientHeight * 0.5f), 0, 5, Colors::Black.v, 0);
	text.InitializeFont(D3DDevice, DeviceContext, L"..\\Font\\Arial16.spritefont");

	skyBox.Initialize(D3DDevice, DeviceContext, L"..\\Textures\\envMap.dds", 64 );

	// load the shader
	shader.LoadShader(D3DDevice);

	leftColor = Colors::DarkGray.v;
	rightColor = Colors::Black.v;

	// load board and pieces
	chessBoard.Initialize(D3DDevice, &shader);
	w_pawn.Initialize(D3DDevice, &shader, &leftColor);
	w_rook.Initialize(D3DDevice, &shader, &leftColor);
	w_knight.Initialize(D3DDevice, &shader, &leftColor);
	w_bishop.Initialize(D3DDevice, &shader, &leftColor);
	w_king.Initialize(D3DDevice, &shader, &leftColor);
	w_queen.Initialize(D3DDevice, &shader, &leftColor);
	b_pawn.Initialize(D3DDevice, &shader, &rightColor);
	b_rook.Initialize(D3DDevice, &shader, &rightColor);
	b_knight.Initialize(D3DDevice, &shader, &rightColor);
	b_bishop.Initialize(D3DDevice, &shader, &rightColor);
	b_king.Initialize(D3DDevice, &shader, &rightColor);
	b_queen.Initialize(D3DDevice, &shader, &rightColor);

	// load the textures
	diffuseTexWhite.Load(D3DDevice, DeviceContext, L"..\\Textures\\marblewhite.png");
	diffuseTexBlack.Load(D3DDevice, DeviceContext, L"..\\Textures\\marbleblack.png");
	specTex.Load(D3DDevice, DeviceContext, L"..\\Textures\\MarbleSpec.jpg");

	// set textures
	chessBoard.SetTextures(diffuseTexWhite.GetResourceView(), specTex.GetResourceView());
	w_pawn.SetTextures(diffuseTexWhite.GetResourceView(), specTex.GetResourceView());
	w_rook.SetTextures(diffuseTexWhite.GetResourceView(), specTex.GetResourceView());
	w_knight.SetTextures(diffuseTexWhite.GetResourceView(), specTex.GetResourceView());
	w_bishop.SetTextures(diffuseTexWhite.GetResourceView(), specTex.GetResourceView());
	w_king.SetTextures(diffuseTexWhite.GetResourceView(), specTex.GetResourceView());
	w_queen.SetTextures(diffuseTexWhite.GetResourceView(), specTex.GetResourceView());
	b_pawn.SetTextures(diffuseTexBlack.GetResourceView(), specTex.GetResourceView());
	b_rook.SetTextures(diffuseTexBlack.GetResourceView(), specTex.GetResourceView());
	b_knight.SetTextures(diffuseTexBlack.GetResourceView(), specTex.GetResourceView());
	b_bishop.SetTextures(diffuseTexBlack.GetResourceView(), specTex.GetResourceView());
	b_king.SetTextures(diffuseTexBlack.GetResourceView(), specTex.GetResourceView());
	b_queen.SetTextures(diffuseTexBlack.GetResourceView(), specTex.GetResourceView());

	shader.SetDirectionalLight(Colors::White.v, Vector3(1,0,1));
	shader.SetSpecularLight(Colors::White.v, 32);

	// Pawn matrices
	for (int i = 0; i < 8; i++)
	{
		m_WhitePieces[i] = Matrix::CreateRotationY(90 * (XM_PI / 180)) * Matrix::CreateTranslation(chessBoard.GetBoardPosition(1, i));
		m_BlackPieces[i] = Matrix::CreateRotationY(270 * (XM_PI / 180)) * Matrix::CreateTranslation(chessBoard.GetBoardPosition(6, i));
	}
	// other pieces
	for (int i = 0; i < 8; i++)
	{
		m_WhitePieces[i + 8] = Matrix::CreateRotationY(90 * (XM_PI / 180)) * Matrix::CreateTranslation(chessBoard.GetBoardPosition(0, i));
		m_BlackPieces[i + 8] = Matrix::CreateRotationY(270 * (XM_PI / 180)) * Matrix::CreateTranslation(chessBoard.GetBoardPosition(7, i));
	}

}

//----------------------------------------------------------------------------------------------
// Window message handler
//----------------------------------------------------------------------------------------------
LRESULT ChessScene::ProcessWindowMessages(UINT msg, WPARAM wParam, LPARAM lParam)
{
	// for mouse positions
	Vector2 pos((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));

	switch (msg )
	{
	case WM_MOUSEMOVE:
		mouseDelta = pos - mousePos;
		mousePos = pos;
		OnMouseMove();
		return 0;
	case WM_LBUTTONUP:
		buttonDown = false;
		mouseDelta = pos - mousePos;
		mousePos = pos;
		break;
	case WM_LBUTTONDOWN:
		buttonDown = true;
		mouseDelta = pos - mousePos;
		mousePos = pos;
		OnMouseDown();
		break;
	case WM_KEYUP:
		if (wParam >= '0' && wParam <= '4')
		{
			PresentInterval = wParam - '0';
		}
		else if (wParam == VK_UP)	{	cameraRotationSpeed.y = 0;	}
		else if (wParam == VK_DOWN) {	cameraRotationSpeed.y = 0; }
		else if (wParam == VK_LEFT) {	cameraRotationSpeed.x = 0; }
		else if (wParam == VK_RIGHT){	cameraRotationSpeed.x = 0; }
		else if (wParam == VK_ADD)  {	cameraRadiusSpeed = 0; }
		else if (wParam == VK_SUBTRACT)  { cameraRadiusSpeed = 0; }
		else if (wParam == VK_SPACE)
		{
			// reset to the starting view
			cameraRotation = Vector2(0, CAMERA_START_PITCH);
			cameraRadius   = CAMERA_START_RADIUS;
		}

		break;
	case WM_KEYDOWN:
		if (wParam == VK_UP)	{ cameraRotationSpeed.y = CAMERA_SPEED; }
		else if (wParam == VK_DOWN) { cameraRotationSpeed.y = -CAMERA_SPEED; }
		else if (wParam == VK_LEFT) { cameraRotationSpeed.x = -CAMERA_SPEED; }
		else if (wParam == VK_RIGHT){ cameraRotationSpeed.x = CAMERA_SPEED; }
		else if (wParam == VK_ADD)  { cameraRadiusSpeed = -1.0f; }
		else if (wParam == VK_SUBTRACT)  { cameraRadiusSpeed = 1.0f; }
		break;

	}

	// let the base class handle remaining messages
	return DirectXClass::ProcessWindowMessages(msg, wParam, lParam);
}

//----------------------------------------------------------------------------------------------
// Called by the render loop to render a single frame
//----------------------------------------------------------------------------------------------
void ChessScene::Render(void)
{
	// calculate camera matrices
	ComputeViewProjection();

	// draw the skybox FIRST
	skyBox.Draw(DeviceContext, viewMatrix, projectionMatrix);

	// draw board
	chessBoard.Draw(DeviceContext, Matrix::CreateTranslation(0, 0, 0), viewMatrix, projectionMatrix);

	// Draw pawns
	for (int i = 0; i < 8; i++)
	{
		w_pawn.Draw(DeviceContext, m_WhitePieces[i], viewMatrix, projectionMatrix);
		b_pawn.Draw(DeviceContext, m_BlackPieces[i], viewMatrix, projectionMatrix);
	}
	// Rooks
	w_rook.Draw(DeviceContext, m_WhitePieces[8], viewMatrix, projectionMatrix);
	b_rook.Draw(DeviceContext, m_BlackPieces[8], viewMatrix, projectionMatrix);
	w_rook.Draw(DeviceContext, m_WhitePieces[15], viewMatrix, projectionMatrix);
	b_rook.Draw(DeviceContext, m_BlackPieces[15], viewMatrix, projectionMatrix);

	// Knights
	w_knight.Draw(DeviceContext, m_WhitePieces[9], viewMatrix, projectionMatrix);
	b_knight.Draw(DeviceContext, m_BlackPieces[9], viewMatrix, projectionMatrix);
	w_knight.Draw(DeviceContext, m_WhitePieces[14], viewMatrix, projectionMatrix);
	b_knight.Draw(DeviceContext, m_BlackPieces[14], viewMatrix, projectionMatrix);

	// Bishops
	w_bishop.Draw(DeviceContext, m_WhitePieces[10], viewMatrix, projectionMatrix);
	b_bishop.Draw(DeviceContext, m_BlackPieces[10], viewMatrix, projectionMatrix);
	w_bishop.Draw(DeviceContext, m_WhitePieces[13], viewMatrix, projectionMatrix);
	b_bishop.Draw(DeviceContext, m_BlackPieces[13], viewMatrix, projectionMatrix);

	// Queens
	w_queen.Draw(DeviceContext, m_WhitePieces[11], viewMatrix, projectionMatrix);
	b_queen.Draw(DeviceContext, m_BlackPieces[12], viewMatrix, projectionMatrix);

	// Kings
	w_king.Draw(DeviceContext, m_WhitePieces[12], viewMatrix, projectionMatrix);
	b_king.Draw(DeviceContext, m_BlackPieces[11], viewMatrix, projectionMatrix);

	// render the base class
	DirectXClass::Render();

	// UI
	spritebatch->Begin(SpriteSortMode_BackToFront);

	button.Draw(spritebatch);
	Vector2 pos = button.GetPosition();

	spritebatch->End();
	if (inMenu) text.PrintMessage(pos.x - 20, pos.y - 10, L"Start", Colors::White.v);
	else text.PrintMessage(pos.x - 15, pos.y - 10, L"Quit", Colors::White.v);
}

//----------------------------------------------------------------------------------------------
// Given a running value, returns a value between 0 and 1 which 'ping pongs' back and forth
//----------------------------------------------------------------------------------------------
float PingPong(float value)
{
	value = fabs(value);
	int wholeNumber = (int)value;	// integer component
	float decimal = value - (float)wholeNumber; // decimal parts

	// if it's odd, flip it
	if (wholeNumber % 2 == 1)
		decimal = 1.0f - decimal;

	return decimal;
}

//----------------------------------------------------------------------------------------------
// Linear Interpolate Matrices, decomposing if possible to give nondistorting results
//----------------------------------------------------------------------------------------------
Matrix LerpMatrices(Matrix& start, Matrix& end, float t)
{
	Vector3 scaleA, scaleB;
	Quaternion rotA, rotB;
	Vector3 transA, transB;

	Matrix result;
	if (start.Decompose(scaleA, rotA, transA) && end.Decompose(scaleB, rotB, transB))
	{
		Vector3 scale = Vector3::Lerp(scaleA, scaleB, t);
		Quaternion rot = Quaternion::Lerp(rotA, rotB, t);
		Vector3 trans = Vector3::Lerp(transA, transB, t);
		result = Matrix::CreateScale(scale)*Matrix::CreateFromQuaternion(rot)*Matrix::CreateTranslation(trans);
	}
	else
	{
		result = Matrix::Lerp(start, end, t);
	}
	return result;
}

//----------------------------------------------------------------------------------------------
// Called every frame to update objects.
//	deltaTime: how much time in seconds has elapsed since the last frame
//----------------------------------------------------------------------------------------------
void ChessScene::Update(float deltaTime)
{
	runTime += deltaTime;

	// update the camera movement
	UpdateCamera(deltaTime);
}


//----------------------------------------------------------------------------------------------
// Called when the mouse is pressed
//----------------------------------------------------------------------------------------------
void ChessScene::OnMouseDown()
{
	if (button.ContainsPoint(mousePos))
	{
		inMenu = !inMenu;
	}

	if (inMenu)
	{
		button.SetColor(Colors::Black.v);
		button.SetScale(5);
		button.SetPosition(Vector2(clientWidth * 0.5, clientHeight * 0.5));
	}
	else
	{
		button.SetColor(Colors::Red.v);
		button.SetScale(0.25);
		button.SetPosition(Vector2(clientWidth * 0.5, clientHeight * 0.9));
	}
}

//----------------------------------------------------------------------------------------------
// Called when the mouse is moved
//----------------------------------------------------------------------------------------------
void ChessScene::OnMouseMove()
{
}

//----------------------------------------------------------------------------------------------
// Computes the view and camera matrix
//----------------------------------------------------------------------------------------------
void ChessScene::ComputeViewProjection()
{
	viewMatrix = Matrix::CreateLookAt(cameraPos, Vector3::Zero, Vector3::UnitY);
	projectionMatrix = Matrix::CreatePerspectiveFieldOfView(60.0f * XM_PI / 180.0f, (float)clientWidth / (float)clientHeight, 1, 128);
}

//----------------------------------------------------------------------------------------------
// Orbit camera - spherical coordinates around the board
//----------------------------------------------------------------------------------------------
void ChessScene::UpdateCamera(float deltaTime)
{
	const float VERT_LIMIT = XM_PI * 0.2f;

	// update the radius
	cameraRadius += cameraRadiusSpeed * deltaTime;
	if (cameraRadius < 1) cameraRadius = 1;


	// update the rotation amounts
	cameraRotation += cameraRotationSpeed * deltaTime;

	// clamp the vertical rotation
	if (cameraRotation.y < -VERT_LIMIT) cameraRotation.y = -VERT_LIMIT;
	else if (cameraRotation.y > VERT_LIMIT) cameraRotation.y = VERT_LIMIT;

	// calculate the height
	cameraPos.y = cameraRadius * sinf(cameraRotation.y);
	float r = cameraRadius * cosf(cameraRotation.y);

	// calculate the orbit
	cameraPos.x = sinf(cameraRotation.x) * r;
	cameraPos.z = cosf(cameraRotation.x) * r;
}
