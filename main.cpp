#include <windows.h>
#include <format>
#include <fstream>

#include "nya_commonhooklib.h"
#include "nya_commonmath.h"
#include "nya_commontimer.h"
#include "nfsmw.h"
#include "chloemenulib.h"

#include "util.h"
#include "components/customcamera.h"

void WriteLog(const std::string& str) {
	static auto file = std::ofstream("NFSMWSecondPerson_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

float fMaxDistance = 250.0;
IVehicle* GetCarInCameraRange() {
	auto closest = GetClosestActiveVehicle(GetLocalPlayerVehicle());
	if (!closest || (*closest->GetPosition() - *GetLocalPlayerVehicle()->GetPosition()).length() > fMaxDistance) return nullptr;
	return closest;
}

void OnCameraTick(CameraMover* pMover) {
	static CNyaTimer gTimer;
	gTimer.Process();

	auto closest = GetCarInCameraRange();
	if (!closest) return;
	CustomCamera::SetTargetCar(closest, GetLocalPlayerVehicle());
	CustomCamera::ProcessCam(pMover->pCamera, gTimer.fDeltaTime);
	Sim::SetStream(GetLocalPlayerVehicle()->GetPosition(), false);
}

void DebugMenu() {
	ChloeMenuLib::BeginMenu();

	QuickValueEditor("Maximum Camera Distance", fMaxDistance);

	//QuickValueEditor("Player Bounty", FEDatabase->mUserProfile->PlayersCarStable.SoldHistoryBounty);
	//QuickValueEditor("Player Cash", FEDatabase->mUserProfile->TheCareerSettings.CurrentCash);
	//QuickValueEditor("UnlockAllThings", UnlockAllThings);

	ChloeMenuLib::EndMenu();
}

float TrafficDensityHooked() {
	if (GRaceStatus::fObj && GRaceStatus::fObj->mRaceParms && GRaceStatus::fObj->mRacerCount > 1) return AITrafficManager::ComputeDensity();

	// always enable traffic in races without opponents
	return 1.0;
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x3C4040) {
				MessageBoxA(nullptr, "Unsupported game version! Make sure you're using v1.3 (.exe size of 6029312 bytes)", "nya?!~", MB_ICONERROR);
				return TRUE;
			}

			NyaHooks::CameraMoverHook::Init();
			NyaHooks::CameraMoverHook::aFunctions.push_back(OnCameraTick);

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x439BBD, &TrafficDensityHooked);

			ChloeMenuLib::RegisterMenu("Second Person Camera", &DebugMenu);

			WriteLog("Mod initialized");
		} break;
		default:
			break;
	}
	return TRUE;
}