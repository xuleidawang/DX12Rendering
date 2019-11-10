#include <iostream>
#include "Common/d3dApp.h"
#include <DirectXColors.h>

using namespace DirectX;

class InitDirect3DApp : public D3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
};

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance) :D3DApp(hInstance) {
}

InitDirect3DApp::~InitDirect3DApp(){
}

bool InitDirect3DApp::Initialize() {
	if (!D3DApp::Initialize())
		return false;
	return true;
}

void InitDirect3DApp::OnResize() {
	D3DApp::OnResize();
}

void InitDirect3DApp::Update(const GameTimer& gt) {

}

void InitDirect3DApp::Draw(const GameTimer& gt) {
	//Reuse the memory associated with command recoring
	//only reset the associated command list have finished execution on the GPU
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	//Indicate a state transition on the resource usage
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//set the viewport and scissor rect. 
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	//Clear back buffer and depth buffer
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr); 
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	//Specify the target buffer 
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	//Indicate a state transition on the resource usage. 
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//Done recording commands
	ThrowIfFailed(mCommandList->Close());

	//Add command list to the queue for execution
	ID3D12CommandList* cmdsListsp[] = { mCommandList.Get() };

	//swap back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	//Wait Until frame commands are complete. This waiting is inefficient and is done for simplicity. 
	FlushCommandQueue();

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmlLine, int showCmd) {
	//Enable runtime memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	try {
		InitDirect3DApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;
		return theApp.Run();
	}
	catch (DxException & e) {
		MessageBox(nullptr, (LPCSTR)e.ToString().c_str(), (LPCSTR)L"HR Failed", MB_OK);
		return 0;
	}
}