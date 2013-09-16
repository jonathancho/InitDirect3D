//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************

#include "Precompiled.h"
#include "d3dApp.h"
#include "Console.h"
//#include "d3dx11effect.h"

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

class InitDirect3DApp : public D3DApp
{
	public:
		InitDirect3DApp(HINSTANCE hInstance);
		~InitDirect3DApp();

		bool Init();
		void OnResize();
		void UpdateScene(float dt);
		void DrawScene(); 

	private:
		void BuildVertexBuffer();
		void BuildIndexBuffer();
		void BuildEffect();
		void BuildVertexLayout(); 

		// private variables
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;

		ID3DX11Effect* effect;
		ID3DX11EffectTechnique* technique;
		ID3DX11EffectMatrixVariable* worldViewProj;

		ID3D11InputLayout* inputLayout;

		XMMATRIX worldMtx;
		XMMATRIX viewMtx;
		XMMATRIX projMtx;

		float mTheta;
		float mPhi;
		float mRadius;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	SetupConsole();

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	InitDirect3DApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
: D3DApp(hInstance) 
{
	vertexBuffer = 0;
	indexBuffer  = 0;
	effect = 0;
	technique = 0;
	worldViewProj = 0;
	inputLayout  = 0;

	mTheta = 1.5f * PI;
	mPhi = 0.25f * PI;
	mRadius = 5.0f;

	// Initialize all matrices to the identity
	XMMATRIX identity = XMMatrixIdentity();
	worldMtx = identity;
	viewMtx = identity;
	projMtx = identity;
}

InitDirect3DApp::~InitDirect3DApp()
{
	ReleaseCOM(vertexBuffer);
	ReleaseCOM(indexBuffer);
	ReleaseCOM(effect);
	ReleaseCOM(inputLayout);
}

bool InitDirect3DApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Set up the vertex and index buffers for the shape we are drawing
	BuildVertexBuffer();
	BuildIndexBuffer();
	BuildEffect();
	BuildVertexLayout();

	return true;
}

void InitDirect3DApp::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX newProjMtx = XMMatrixPerspectiveFovLH(0.25f * PI, AspectRatio(), 1.0f, 1000.0f);
	projMtx = newProjMtx;
}

void InitDirect3DApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(0.0f, 0.0f, 10.0f, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	viewMtx = V;
	//XMStoreFloat4x4(&viewMtx, V);
}

void InitDirect3DApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	// Clear all the old stuff before rendering the new stuff
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set the input layout for vertices before drawing
	md3dImmediateContext->IASetInputLayout(inputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, offset);

	// Update the worldViewProjMtx and set up
	XMMATRIX worldViewProjMtx = worldMtx * viewMtx * projMtx;
	worldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProjMtx));

	// Draw the indices
	D3DX11_TECHNIQUE_DESC techniqueDesc;
	technique->GetDesc(&techniqueDesc);

	for (unsigned int i = 0; i < techniqueDesc.Passes; ++i)
	{
		technique->GetPassByIndex(i)->Apply(0, md3dImmediateContext);

		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}

	// Present the back buffer to the screen
	HR(mSwapChain->Present(0, 0));
}

void InitDirect3DApp::BuildVertexBuffer()
{
	Vertex vertices[] = 
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White   },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black   },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};

	// Describes the buffer we are going to create
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(vertices);			// Size in bytes of the buffer we are going to create
	vbd.Usage = D3D11_USAGE_IMMUTABLE;			// The contents will not change after creation
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// Making a vertex buffer
	vbd.CPUAccessFlags = 0;                     // CPU does not require read or write access after the buffer has been created
	vbd.MiscFlags = 0;                          // We need nothing here
	vbd.StructureByteStride = 0;                // For structured buffers only

	// Specifies the data we want to initialize the buffer with
	D3D11_SUBRESOURCE_DATA vertexBufferInitData;
	vertexBufferInitData.pSysMem = vertices;

	// Create the vertex buffer
	HR(md3dDevice->CreateBuffer(&vbd, &vertexBufferInitData, &vertexBuffer));
}

void InitDirect3DApp::BuildIndexBuffer()
{
	UINT indices[] =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	// Describes the buffer we are going to create
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(indices);			// Size in bytes of the buffer we are going to create
	ibd.Usage = D3D11_USAGE_IMMUTABLE;			// The contents will not change after creation
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// Making a vertex buffer
	ibd.CPUAccessFlags = 0;                     // CPU does not require read or write access after the buffer has been created
	ibd.MiscFlags = 0;                          // We need nothing here
	ibd.StructureByteStride = 0;                // For structured buffers only

	// Specifies the data we want to initialize the buffer with
	D3D11_SUBRESOURCE_DATA indexBufferInitData;
	indexBufferInitData.pSysMem = indices;

	// Create the index buffer
	HR(md3dDevice->CreateBuffer(&ibd, &indexBufferInitData, &indexBuffer));
}

void InitDirect3DApp::BuildEffect()
{
	DWORD shaderFlags = 0;

	#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationErrors = 0;

	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx",		// Name of the .fx file
									   0,					// Advanced option
									   0,					// Advanced option
									   0,					// Using the effects framework so set it to null
									   "fx_5_0",			// Shader version we are using
									   shaderFlags,			// Flags to specify how the code should be compiled
									   0,					// Advanced effect compilation options
									   0,					// Advanced option to compile the shader asynchronously
									   &compiledShader,		// Pointer to the compiled shader
									   &compilationErrors,	// Pointer to compilation errors if there are any
									   0);                  // Return error code for compiling asynchronously
	
	// Check for any compilation errors
	if (compilationErrors != 0)
	{
		MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationErrors);
	}

	// Check for any other errors with compiling the shader
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	// Shader compiled successfully, time to create the effect
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, md3dDevice, &effect));

	ReleaseCOM(compiledShader);

	// Get the technique location from the effect
	technique = effect->GetTechniqueByName("ColorTechnique");

	// Get the World View Proj Matrix location from the effect
	worldViewProj = effect->GetVariableByName("WorldViewProj")->AsMatrix();
}

void InitDirect3DApp::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDescription[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDescription;

	technique->GetPassByIndex(0)->GetDesc(&passDescription);
	HR(md3dDevice->CreateInputLayout(vertexDescription, 2, passDescription.pIAInputSignature, passDescription.IAInputSignatureSize, &inputLayout));
}