#include "Gui.h"
#include <windows.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>
#include <d3d11.h>
#include <dxgi.h>
#include <detours/detours.h>

#include <iostream>

typedef HRESULT(WINAPI* SwapChainPresent)(IDXGISwapChain* pChain, UINT syncInterval, UINT flags);
typedef HRESULT(WINAPI* SwapChainResizeBuffers)(IDXGISwapChain* pChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

bool bInitialized;
bool bMenuOpen = true;

float* pOriginalNormalX;
float* pOriginalNormalZ;
float* pModifiedNormalX;
float* pModifiedNormalZ;
bool* pApplyNewNormal;

float newXAngle;
float newYAngle;

IDXGISwapChain* hookedSwapChain;
ID3D11Device* hookedDevice;
ID3D11DeviceContext* hookedContext;
ID3D11RenderTargetView* renderTarget;
WNDPROC originalWndProc;
HWND hookedWindow;

SwapChainPresent pHookD3D11Present = nullptr;
SwapChainResizeBuffers pHookD3D11ResizeBuffers = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
		if (wParam == VK_INSERT) {
			bMenuOpen = !bMenuOpen;
		}
		if (wParam == 0x47) { // G key
			*pApplyNewNormal = !*pApplyNewNormal;
		}
		if (wParam == 0x54) { // T key
			*pModifiedNormalX = *pOriginalNormalX;
			*pModifiedNormalZ = *pOriginalNormalZ;
		}
		break;
	}
	}
	ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
	return CallWindowProc(originalWndProc, hwnd, uMsg, wParam, lParam);
}

void SetupRenderTarget(IDXGISwapChain* pChain) {
	ID3D11Texture2D* pBackBuffer;
	pChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	hookedDevice->CreateRenderTargetView(pBackBuffer, nullptr, &renderTarget);
	pBackBuffer->Release();

	hookedContext->OMSetRenderTargets(1, &renderTarget, nullptr);
}

HRESULT Present(IDXGISwapChain* pChain, UINT syncInterval, UINT flags) {
	if (!bInitialized) {
		std::cout << "Initializing DX11 hook" << std::endl;
		hookedSwapChain = pChain;

		if (SUCCEEDED(pChain->GetDevice(IID_PPV_ARGS(&hookedDevice)))) {
			hookedDevice->GetImmediateContext(&hookedContext);

			DXGI_SWAP_CHAIN_DESC desc;
			pChain->GetDesc(&desc);
			hookedWindow = desc.OutputWindow;

			SetupRenderTarget(pChain);

			originalWndProc = (WNDPROC)SetWindowLongPtr(hookedWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init(hookedWindow);
			ImGui_ImplDX11_Init(hookedDevice, hookedContext);
			bInitialized = true;
		}
		else {
			std::cout << "Failed to initialize DX11 hook! Retrying..." << std::endl;
		}
	}

	if (!bInitialized) return pHookD3D11Present(pChain, syncInterval, flags);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (bMenuOpen) {
		ImGui::Begin("Deform Tool", &bMenuOpen, ImGuiWindowFlags_None);
		
		if (ImGui::CollapsingHeader("Debug")) {
			ImGui::Text("Normal X: %f", *pOriginalNormalX);
			ImGui::SameLine();
			ImGui::Text("New Normal X: %f", *pModifiedNormalX);

			ImGui::Text("Normal Z: %f", *pOriginalNormalZ);
			ImGui::SameLine();
			ImGui::Text("New Normal Z: %f", *pModifiedNormalZ);
		}
		ImGui::InputFloat("X", pModifiedNormalX);
		ImGui::InputFloat("Z", pModifiedNormalZ);

		ImGui::Checkbox("Enable", pApplyNewNormal);

		ImGui::Text("Insert - Show/Hide this menu");
		ImGui::Text("T - Save the angle of the surface you are pointing at");
		ImGui::Text("G - Enable/Disable freezing angle");
		
		ImGui::End();
	}
	
	ImGui::EndFrame();
	ImGui::Render();

	hookedContext->OMSetRenderTargets(1, &renderTarget, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return pHookD3D11Present(pChain, syncInterval, flags);
}

HRESULT ResizeBuffers(IDXGISwapChain* pChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	if (renderTarget) {
		hookedContext->OMSetRenderTargets(0, nullptr, nullptr);
		renderTarget->Release();
	}

	HRESULT hr = pHookD3D11ResizeBuffers(pChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	SetupRenderTarget(pChain);
	return hr;
}

void Gui::UpdatePointers(float* originalNormalX, float* originalNormalZ, float* modifiedNormalX, float* modifiedNormalZ, bool* applyNewNormal) {
	pOriginalNormalX = originalNormalX;
	pOriginalNormalZ = originalNormalZ;
	pModifiedNormalX = modifiedNormalX;
	pModifiedNormalZ = modifiedNormalZ;
	pApplyNewNormal = applyNewNormal;
}

bool Gui::InitGui() {
	HWND hWnd = GetForegroundWindow();
	std::cout << "Window: " << hWnd << std::endl;

	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc{ 0 };
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = (GetWindowLong(hWnd, GWL_STYLE) & WS_POPUP) == 0;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, nullptr, &pContext);
	if (FAILED(res)) {
		std::cerr << "Failed to create swapchain: 0x" << std::hex << res << std::dec << std::endl;
		return false;
	}

	DWORD_PTR* pSwapChainVtable = nullptr;

	pSwapChainVtable = (DWORD_PTR*)pSwapChain;
	pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];

	pHookD3D11Present = (HRESULT(*)(IDXGISwapChain*, UINT, UINT))pSwapChainVtable[8];
	pHookD3D11ResizeBuffers = (HRESULT(*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT))pSwapChainVtable[13];

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pHookD3D11Present, Present);
	DetourAttach(&(PVOID&)pHookD3D11ResizeBuffers, ResizeBuffers);
	DetourTransactionCommit();

	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();
	return true;
}

void Gui::DestroyGui() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	if (pHookD3D11Present) DetourDetach(&(PVOID&)pHookD3D11Present, Present);
	if (pHookD3D11ResizeBuffers) DetourDetach(&(PVOID&)pHookD3D11ResizeBuffers, ResizeBuffers);
	DetourTransactionCommit();

	if (originalWndProc) SetWindowLongPtr(hookedWindow, GWLP_WNDPROC, (LONG_PTR)originalWndProc);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (hookedContext) hookedContext->OMSetRenderTargets(0, nullptr, nullptr);
	if (renderTarget) renderTarget->Release();
	bInitialized = false;
}