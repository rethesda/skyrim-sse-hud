#include "main.h"
#include <hook/d3d11hook.h>
#include <hook/dinputhook.h>
#include <hook/hudhook.h>
#include <memory/memory.h>
#include <menu/menu.h>
#include <event/BSTEquipEvent.h>
#include <setting/setting.h>


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	setting::settings_path = "data\\skse\\plugins\\sse-hud.json";
	setting::settings_path_gallery = "data\\skse\\plugins\\1.json";
	fontFilePath = "data\\skse\\plugins\\xyght3.0-62354202.ttf";
	//fontFilePath2 = "data\\skse\\plugins\\xyght3.0-62354202.ttf";

	//MessageBox(nullptr, TEXT("测试中文."), nullptr, MB_OK);
	setting::load_settings();
	setting::load_settings_gallery();
	try {
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

#else
		auto path = logger::log_directory();
		if (!path) {
			return false;
		}

		//*path /= Version::PROJECT;
		*path /= "imgui hud";
		*path += ".log"sv;
		auto sink = make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif
		auto log = make_shared<spdlog::logger>("global log"s, move(sink));

#ifndef NDEBUG
		log->set_level(spdlog::level::trace);
#else
		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);
#endif

		spdlog::set_default_logger(move(log));
		spdlog::set_pattern("[%H:%M:%S.%f] %s(%#) [%^%l%$] %v"s);

		logger::info(FMT_STRING("{} v{}"), "sse-hud", "sse-hud");

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "sse-hud";
		a_info->version = 1;

		if (a_skse->IsEditor()) {
			logger::critical("Loaded in editor, marking as incompatible"sv);
			return false;
		}

		const auto ver = a_skse->RuntimeVersion();
		if (ver < SKSE::RUNTIME_1_5_39) {
			logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
			return false;
		}
	} catch (const std::exception& e) {
		logger::critical("failed, cause {}"sv, e.what());
		return false;
	}

	return true;
}

int SehFilter(DWORD dwExceptionCode)
{
	switch (dwExceptionCode) {
	case EXCEPTION_ACCESS_VIOLATION:
		return EXCEPTION_EXECUTE_HANDLER;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}


void __cdecl installimgui(void*);


//void delayedExecution()
//{
//	std::this_thread::sleep_for(std::chrono::seconds(3));
//	isGameLoading = false;
//}


void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		{
			logger::info("PostLoad"sv);


			auto messaging = SKSE::GetMessagingInterface();
	
			
			break;
		}
	case SKSE::MessagingInterface::kPostPostLoad:
		{
			logger::info("PostPostLoad"sv);
			break;
		}
	case SKSE::MessagingInterface::kPreLoadGame:
		{
			logger::info("kPreLoadGame"sv);
			isGameLoading = true;
			//startflag = true;
			break;
		}
	case SKSE::MessagingInterface::kPostLoadGame:
		{
			logger::info("kPostLoadGame"sv);
			isGameLoading = false;
			startflag = true;
			if (trackPtrs.size() > 0) {
				trackPtrs.clear();
			}
			if (trackActorPtrs.size() > 0) {
				trackActorPtrs.clear();
			}
			//std::thread t(delayedExecution);
			
			break;
		}
	case SKSE::MessagingInterface::kSaveGame:
		{
			logger::info("kSaveGame"sv);
			break;
		}
	case SKSE::MessagingInterface::kDeleteGame:
		{
			logger::info("kDeleteGame"sv);
			break;
		}
	case SKSE::MessagingInterface::kInputLoaded:
		{
			logger::info("kInputLoaded"sv);
			break;
		}
	case SKSE::MessagingInterface::kDataLoaded:
		{
			logger::info("kDataLoaded"sv);
			//MessageBox(nullptr, "SKSE kDataLoaded", nullptr, MB_OK);

			/*		RE::GFxValue obj;
		

			RE::GFxValue fxValue;
			fxValue.SetNumber(90);
			obj.SetMember("goldNumber", fxValue);

			RE::GFxValue fxValue2;
			fxValue2.SetNumber(100);
			obj.SetMember("encumbranceNumber", fxValue);*/

			break;
		}
	default:
		logger::info("??"sv);
		break;

	}
}

void __cdecl installimgui(void*)
{
	d3d11hook::Install(1);
	dinputhook ::Install();
	
	MenuOpenCloseEvent::Register();
	EquipEvent::Register();
	//BSTCrosshairRefEvent::Register();  
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("SKSEPlugin_Load"sv);
#ifndef NDEBUG
	MessageBoxA(nullptr, "SKSEPlugin_Load", nullptr, MB_OK);
	Sleep(1000);
#endif

	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
	SKSE::Init(a_skse);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		logger::critical("Could not register MessageHandler"sv);
		return false;
	}

	SKSE::AllocTrampoline(1 << 6);

	logger::info("registered listener"sv);


	_beginthread(RefreshGameInfo, 0, NULL);
	_beginthread(RefreshActorInfo, 0, NULL);
	_beginthread(RefreshItemInfo, 0, NULL);
	_beginthread(installimgui, 0, NULL);

	return true;
}

