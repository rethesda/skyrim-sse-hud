#include "d3d11hook.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <MinHook.h>
#include <fonts/IconsMaterialDesignIcons.h>
#include <hook/BSRenderManager.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <memory/memory.h>
#include <menu/menu.h>
#include <utils/utils.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace d3d11hook
{
	ID3D11Device* g_pd3dDevice = nullptr;
	IDXGISwapChain* g_pSwapChain = nullptr;
	ID3D11DeviceContext* g_pd3dContext = nullptr;
	ID3D11RenderTargetView* D3D11RenderView = nullptr;
	HWND g_hwnd = nullptr;

	typedef HRESULT(__stdcall* D3D11PresentHook)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	D3D11PresentHook phookD3D11Present = nullptr;

	using WndProcFunc = std::add_pointer_t<LRESULT((__stdcall)(HWND, UINT, WPARAM, LPARAM))>;
	WndProcFunc OldWndProc;

	LRESULT __stdcall WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
	{
		//LRESULT ret = 0;
		//if (active) {
		//	//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
		//	ret = ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//}
		//return ret;

		//switch (uMsg) {
		//case WM_LBUTTONDOWN:
		//	ImGui::GetIO().MouseDown[0] = true;
		//	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	break;
		//case WM_LBUTTONUP:
		//	ImGui::GetIO().MouseDown[0] = false;
		//	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	break;
		//case WM_RBUTTONDOWN:
		//	ImGui::GetIO().MouseDown[1] = true;
		//	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	break;
		//case WM_RBUTTONUP:
		//	ImGui::GetIO().MouseDown[1] = false;
		//	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	break;
		//case WM_MBUTTONDOWN:
		//	ImGui::GetIO().MouseDown[2] = true;
		//	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	break;
		//case WM_MBUTTONUP:
		//	ImGui::GetIO().MouseDown[2] = false;
		//	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	break;
		//case WM_MOUSEWHEEL:
		//	ImGui::GetIO().MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		//	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	break;
		//case WM_MOUSEMOVE:
		//	ImGui::GetIO().MousePos.x = (signed short)(lParam);
		//	ImGui::GetIO().MousePos.y = (signed short)(lParam >> 16);
		//	return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	break;
		//}

		//return CallWindowProc(OldWndProc, hwnd, uMsg, wParam, lParam);

		//if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
		//	return true;
		//}
		//return CallWindowProc(OldWndProc, hwnd, uMsg, wParam, lParam);

		return OldWndProc(hwnd, uMsg, wParam, lParam);
	}

	HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		static std::once_flag ImguiInit;
		std::call_once(ImguiInit, [&]() {
			g_pSwapChain = pSwapChain;
			if (SUCCEEDED(g_pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice)))) {
				g_pd3dDevice->GetImmediateContext(&g_pd3dContext);

				ID3D11Texture2D* backBuffer;
				g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
				g_pd3dDevice->CreateRenderTargetView(backBuffer, nullptr, &D3D11RenderView);
				backBuffer->Release();

				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);
				auto window = sd.OutputWindow;

				OldWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc));

				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
				if (std::filesystem::exists(fontFilePath)) {
					float baseFontSize = 18.0f;

					io.Fonts->AddFontFromFileTTF("data\\skse\\plugins\\xyght3.0-62354202.ttf", baseFontSize, 0, io.Fonts->GetGlyphRangesChineseFull());

					float iconFontSize = baseFontSize * 2.0f / 3.0f;

					ImFontConfig config;
					config.MergeMode = true;
					config.GlyphMinAdvanceX = iconFontSize;
					static const ImWchar icon_ranges[] = { 0xf000, 0xf3ff, 0 };
					io.Fonts->AddFontFromFileTTF("data\\skse\\plugins\\fontawesome-webfont.ttf", iconFontSize, &config, icon_ranges);
					SKSE::log::info("AddFontFromFileTTF");

					/*io.Fonts*/
					ImFontConfig config2;
					config2.MergeMode = true;
					config2.PixelSnapH = true;
					config2.GlyphMinAdvanceX = iconFontSize;
					static const ImWchar icon_ranges2[] = { static_cast<ImWchar>(ICON_MIN_MDI), static_cast<ImWchar>(ICON_MAX_MDI), static_cast<ImWchar>(0) };
					io.Fonts->AddFontFromFileTTF("data\\skse\\plugins\\materialdesignicons-webfont.ttf", iconFontSize, &config2, icon_ranges2);

					// 初始化缩放
					io.FontGlobalScale = menu::font_scale;
				}

				if (!ImGui_ImplWin32_Init(g_hwnd)) {
					SKSE::log::warn("ImGui_ImplWin32_Init fail");
				}
				if (!ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext)) {
					SKSE::log::warn("ImGui_ImplDX11_Init fail");
				}
				SKSE::log::info("ImGui_ImplDX11_Init");
			} else {
				SKSE::log::info("g_pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice) Fail");
			}
		});

		//ImGui::GetIO().WantCaptureMouse = active;
		ImGui::GetIO().MouseDrawCursor = active || activeItems;
		//ImGui::GetIO().WantSetMousePos = true;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		menu::render();

		ImGui::EndFrame();

		ImGui::Render();

		g_pd3dContext->OMSetRenderTargets(1, &D3D11RenderView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return phookD3D11Present(pSwapChain, SyncInterval, Flags);
	}

	using TDXGISwapChainPresent = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);

	TDXGISwapChainPresent RealDXGISwapChainPresent = nullptr;

	HRESULT __stdcall HookDXGISwapChainPresent(IDXGISwapChain* This, UINT SyncInterval, UINT Flags)
	{
		static std::once_flag ImguiInit;
		std::call_once(ImguiInit, [&]() {
			SKSE::log::info("HookDXGISwapChainPresent call_once start");
			g_pSwapChain = This;
			if (SUCCEEDED(g_pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice)))) {
				g_pd3dDevice->GetImmediateContext(&g_pd3dContext);

				ID3D11Texture2D* backBuffer;
				g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
				g_pd3dDevice->CreateRenderTargetView(backBuffer, nullptr, &D3D11RenderView);
				backBuffer->Release();

				DXGI_SWAP_CHAIN_DESC sd;
				This->GetDesc(&sd);
				auto hwnd = sd.OutputWindow;

				OldWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc));

				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
				if (std::filesystem::exists(fontFilePath)) {
					SKSE::log::info("HookDXGISwapChainPresent addfont start");
					float baseFontSize = 18.0f;

					io.Fonts->AddFontFromFileTTF("data\\skse\\plugins\\xyght3.0-62354202.ttf", baseFontSize, 0, io.Fonts->GetGlyphRangesChineseFull());

					float iconFontSize = baseFontSize * 2.0f / 3.0f;

					ImFontConfig config;
					config.MergeMode = true;
					config.GlyphMinAdvanceX = iconFontSize;
					static const ImWchar icon_ranges[] = { 0xf000, 0xf3ff, 0 };
					io.Fonts->AddFontFromFileTTF("data\\skse\\plugins\\fontawesome-webfont.ttf", iconFontSize, &config, icon_ranges);
					SKSE::log::info("AddFontFromFileTTF");

					/*io.Fonts*/
					ImFontConfig config2;
					config2.MergeMode = true;
					config2.PixelSnapH = true;
					config2.GlyphMinAdvanceX = iconFontSize;
					static const ImWchar icon_ranges2[] = { static_cast<ImWchar>(ICON_MIN_MDI), static_cast<ImWchar>(ICON_MAX_MDI), static_cast<ImWchar>(0) };
					io.Fonts->AddFontFromFileTTF("data\\skse\\plugins\\materialdesignicons-webfont.ttf", iconFontSize, &config2, icon_ranges2);

					// 初始化缩放
					io.FontGlobalScale = menu::font_scale;
				}

				ImGui_ImplWin32_Init(hwnd);
				//ImGui_ImplWin32_Init(g_hwnd);
				ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext);
				SKSE::log::info("ImGui_ImplDX11_Init");
			} else {
				SKSE::log::info("g_pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice) Fail");
			}
		});

		//ImGui::GetIO().WantCaptureMouse = active;
		ImGui::GetIO().MouseDrawCursor = active || activeItems;
		//ImGui::GetIO().WantSetMousePos = true;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		menu::render();

		ImGui::EndFrame();

		ImGui::Render();

		g_pd3dContext->OMSetRenderTargets(1, &D3D11RenderView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		const auto result = RealDXGISwapChainPresent(This, SyncInterval, Flags);
		//if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {

		//}

		return result;
	}

	bool TryD3D11()
	{
		while (true) {
			SKSE::log::info("FindWindow..");
			g_hwnd = FindWindow(0, "Skyrim Special Edition");
			if (g_hwnd) {
				break;
			}
			Sleep(2000);
		}

		D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
		DXGI_SWAP_CHAIN_DESC sd{};
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.OutputWindow = g_hwnd;
		sd.SampleDesc.Count = 1;

		sd.Windowed = ((GetWindowLongPtr(g_hwnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//sd.Windowed = true;

		sd.BufferDesc.Width = 1;
		sd.BufferDesc.Height = 1;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;

		D3D_FEATURE_LEVEL featureLevel{};

		SKSE::log::info("D3D11CreateDeviceAndSwapChain start");
		const auto hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, nullptr);
		SKSE::log::info("D3D11CreateDeviceAndSwapChain end");
		if (FAILED(hr)) {
			SKSE::log::error("D3D11CreateDeviceAndSwapChain Fail {}", (int)g_hwnd);
			return false;
		}
		return true;
	}

	decltype(&D3D11CreateDeviceAndSwapChain) ptrD3D11CreateDeviceAndSwapChain;

	HRESULT WINAPI hk_D3D11CreateDeviceAndSwapChain(
		IDXGIAdapter* pAdapter,
		D3D_DRIVER_TYPE DriverType,
		HMODULE Software,
		UINT Flags,
		const D3D_FEATURE_LEVEL* pFeatureLevels,
		UINT FeatureLevels,
		UINT SDKVersion,
		const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
		IDXGISwapChain** ppSwapChain,
		ID3D11Device** ppDevice,
		D3D_FEATURE_LEVEL* pFeatureLevel,
		ID3D11DeviceContext** ppImmediateContext)
	{
		SKSE::log::info("hk_D3D11CreateDeviceAndSwapChain start");
		const auto result = (*ptrD3D11CreateDeviceAndSwapChain)(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
		//const auto result = RealD3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);

		// 测试
		//auto manager = RE::BSRenderManager::GetSingleton();
		//if (!manager) {
		//	logger::critical("Failed to retrieve BSRenderManager");
		//}

		// reinterpret_cast<IDXGISwapChain*>(manager->swapChain);

		if (RealDXGISwapChainPresent == nullptr && ppSwapChain) {
			SKSE::log::info("HookDXGISwapChainPresent ready");
#pragma warning(suppress: 6001)
			auto pVTable = **reinterpret_cast<uintptr_t***>(ppSwapChain);

			RealDXGISwapChainPresent = reinterpret_cast<TDXGISwapChainPresent>(pVTable[8]);

			auto pHook = &HookDXGISwapChainPresent;
			REL::safe_write(reinterpret_cast<uintptr_t>(&pVTable[8]), &pHook, sizeof(uintptr_t));
			SKSE::log::info("HookDXGISwapChainPresent over");
		}

		return result;

		DXGI_SWAP_CHAIN_DESC sDesc;

		sDesc = *pSwapChainDesc;

		logger::info("Calling original D3D11CreateDeviceAndSwapChain");
		HRESULT hr = (*ptrD3D11CreateDeviceAndSwapChain)(pAdapter,
			DriverType,
			Software,
			Flags,
			pFeatureLevels,
			FeatureLevels,
			SDKVersion,
			&sDesc,
			ppSwapChain,
			ppDevice,
			pFeatureLevel,
			ppImmediateContext);

		//auto device = *ppDevice;
		//auto deviceContext = *ppImmediateContext;
		IDXGISwapChain* pSwapChain = *ppSwapChain;

		//DWORD_PTR* pSwapChainVTable = nullptr;
		//pSwapChainVTable = (DWORD_PTR*)(swapChain);
		//pSwapChainVTable = (DWORD_PTR*)(pSwapChainVTable[0]);

		// void* presentFunc = *(reinterpret_cast<void***>(pSwapChain))[8];

		auto pVTable = **reinterpret_cast<uintptr_t***>(ppSwapChain);
		D3D11PresentHook presentFunc = reinterpret_cast<D3D11PresentHook>(pVTable[8]);

		if (MH_CreateHook(presentFunc, &PresentHook, reinterpret_cast<void**>(&phookD3D11Present)) != MH_OK) {
			SKSE::log::error("MH_CreateHook pSwapChainVTable Fail");
			return 1;
		}
		// 启用 hook
		MH_EnableHook(presentFunc);

		/*	if (MH_CreateHook((DWORD_PTR*)pSwapChainVTable[8], PresentHook, reinterpret_cast<void**>(&phookD3D11Present)) != MH_OK) {
			SKSE::log::error("MH_CreateHook pSwapChainVTable Fail");
			return 1;
		}
		if (MH_EnableHook((DWORD_PTR*)pSwapChainVTable[8]) != MH_OK) {
			SKSE::log::error("MH_EnableHook pSwapChainVTable Fail");
			return 1;
		}*/
		return hr;
	}

	//struct RendererInitOSData
	//	{
	//		HWND hWnd;
	//		HINSTANCE hInstance;
	//		WNDPROC pWndProc;
	//		HICON hIcon;
	//		const char* pClassName;
	//		uint32_t uiAdapter;
	//		int bCreateSwapChainRenderTarget;
	//	};
	//
	//
	//	using TRendererInit = void(void*, RendererInitOSData*, void*, void*);
	//	static TRendererInit* RealRendererInit = nullptr;
	//	static WNDPROC RealWndProc = nullptr;
	//	void HookRendererInit(void* apThis, RendererInitOSData* apOSData, void* apFBData, void* apOut)
	//	{
	//		RealWndProc = apOSData->pWndProc;
	//		//apOSData->pWndProc = HookWndProc;
	//		RealRendererInit(apThis, apOSData, apFBData, apOut);
	//	}

	decltype(&D3D11CreateDeviceAndSwapChain) pD3D11CreateDeviceAndSwapChain = nullptr;

	void Install(int type)
	{
		//HMODULE hD3D11 = GetModuleHandleA("d3d11.dll");
		//pD3D11CreateDeviceAndSwapChain = reinterpret_cast<decltype(&D3D11CreateDeviceAndSwapChain)>(
		//	GetProcAddress(hD3D11, "D3D11CreateDeviceAndSwapChain"));
		//if (MH_Initialize() != MH_OK) {
		//	SKSE::log::error("MH_Initialize Fail");
		//	return ;
		//}
		//if (MH_CreateHook(
		//	pD3D11CreateDeviceAndSwapChain,
		//	&hk_D3D11CreateDeviceAndSwapChain,
		//	reinterpret_cast<LPVOID*>(&pD3D11CreateDeviceAndSwapChain)) != MH_OK) {
		//	SKSE::log::error("MH_CreateHook Fail");
		//	return;
		//}

		//if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		//	SKSE::log::error("MH_EnableHook Fail");
		//	return;
		//}
		//MH_EnableHook(MH_ALL_HOOKS);

		//Sleep(1000);

		if (type == 1) {
			char* ptr = nullptr;
			auto moduleBase = (uintptr_t)GetModuleHandle(ptr);
			auto dllD3D11 = GetModuleHandleA("d3d11.dll");
			*(FARPROC*)&ptrD3D11CreateDeviceAndSwapChain = GetProcAddress(dllD3D11, "D3D11CreateDeviceAndSwapChain");
			SKSE::log::info("GetProcAddress: " + std::to_string((int)ptrD3D11CreateDeviceAndSwapChain));
			Detours::IATHook(moduleBase, "d3d11.dll", "D3D11CreateDeviceAndSwapChain", (uintptr_t)hk_D3D11CreateDeviceAndSwapChain);
		} else if (type == 3) {
			// 测试
			auto manager = RE::BSRenderManager::GetSingleton();
			if (!manager) {
				logger::critical("Failed to retrieve BSRenderManager");
			}
			IDXGISwapChain* pswapchain = nullptr;

			while (!pswapchain) {
				pswapchain = reinterpret_cast<IDXGISwapChain*>(manager->swapChain);
				Sleep(1000);
			}


			if (RealDXGISwapChainPresent == nullptr && pswapchain) {
				SKSE::log::info("HookDXGISwapChainPresent ready");
#pragma warning(suppress: 6001)
				auto pVTable = *reinterpret_cast<uintptr_t**>(pswapchain);

				RealDXGISwapChainPresent = reinterpret_cast<TDXGISwapChainPresent>(pVTable[8]);

				auto pHook = &HookDXGISwapChainPresent;
				REL::safe_write(reinterpret_cast<uintptr_t>(&pVTable[8]), &pHook, sizeof(uintptr_t));
				SKSE::log::info("HookDXGISwapChainPresent over");
			}

		} else {
			static std::once_flag D3DInit;
			std::call_once(D3DInit, [&]() {
				if (TryD3D11()) {
					SKSE::log::info("MH_Initialize");
					if (MH_Initialize() != MH_OK) {
						SKSE::log::error("MH_Initialize Fail");
						return 1;
					}

					DWORD_PTR* pSwapChainVTable = nullptr;
					pSwapChainVTable = (DWORD_PTR*)(g_pSwapChain);
					pSwapChainVTable = (DWORD_PTR*)(pSwapChainVTable[0]);

					SKSE::log::info("MH_CreateHook pSwapChainVTable start");
					if (MH_CreateHook((DWORD_PTR*)pSwapChainVTable[8], PresentHook, reinterpret_cast<void**>(&phookD3D11Present)) != MH_OK) {
						SKSE::log::error("MH_CreateHook pSwapChainVTable Fail");
						return 1;
					}
					SKSE::log::info("MH_CreateHook pSwapChainVTable success");
					if (MH_EnableHook((DWORD_PTR*)pSwapChainVTable[8]) != MH_OK) {
						SKSE::log::error("MH_EnableHook pSwapChainVTable Fail");
						return 1;
					}
					SKSE::log::info("MH_EnableHook pSwapChainVTable success");
				} else {
					return 0;
				}
			});
		}
	}
}
