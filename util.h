bool IsInLoadingScreen() {
	if (FadeScreen::IsFadeScreenOn()) return true;
	if (cFEng::IsPackagePushed(cFEng::mInstance, "Loading.fng")) return true;
	if (cFEng::IsPackagePushed(cFEng::mInstance, "WS_Loading.fng")) return true;
	return false;
}

bool IsInNIS() {
	return INIS::mInstance && INIS::mInstance->IsPlaying();
}

bool IsInMovie() {
	return gMoviePlayer;
}

IPlayer* GetLocalPlayer() {
	auto& list = PLAYER_LIST::GetList(PLAYER_LOCAL);
	if (list.empty()) return nullptr;
	return list[0];
}

ISimable* GetLocalPlayerSimable() {
	auto ply = GetLocalPlayer();
	if (!ply) return nullptr;
	return ply->GetSimable();
}

template<typename T>
T* GetLocalPlayerInterface() {
	auto ply = GetLocalPlayerSimable();
	if (!ply) return nullptr;
	T* out;
	if (!ply->QueryInterface<T>(&out)) return nullptr;
	return out;
}

auto GetLocalPlayerVehicle() { return GetLocalPlayerInterface<IVehicle>(); }
auto GetLocalPlayerEngine() { return GetLocalPlayerInterface<IEngine>(); }

std::vector<IVehicle*> GetActiveVehicles(int driverClass = -1) {
	auto& list = VEHICLE_LIST::GetList(VEHICLE_ALL);
	std::vector<IVehicle*> cars;
	for (int i = 0; i < list.size(); i++) {
		if (!list[i]->IsActive()) continue;
		if (list[i]->IsLoading()) continue;
		if (driverClass >= 0 && list[i]->GetDriverClass() != driverClass) continue;
		cars.push_back(list[i]);
	}
	return cars;
}

bool IsVehicleValidAndActive(IVehicle* vehicle) {
	auto cars = GetActiveVehicles();
	for (auto& car : cars) {
		if (car == vehicle) return true;
	}
	return false;
}

IVehicle* GetClosestActiveVehicle(IVehicle* toVehicle) {
	auto sourcePos = *toVehicle->GetPosition();
	UMath::Vector3 sourceFwd;
	toVehicle->mCOMObject->Find<IRigidBody>()->GetForwardVector(&sourceFwd);
	IVehicle* out = nullptr;
	float distance = 99999;
	auto cars = GetActiveVehicles();
	for (auto& car : cars) {
		if (car == toVehicle) continue;
		auto targetPos = *car->GetPosition();
		if ((sourcePos - targetPos).length() < distance) {
			out = car;
			distance = (sourcePos - targetPos).length();
		}
	}
	return out;
}

NyaVec3 WorldToRenderCoords(NyaVec3 world) {
	return {world.z, -world.x, world.y};
}

NyaVec3 RenderToWorldCoords(NyaVec3 render) {
	return {-render.y, render.z, render.x};
}

NyaMat4x4 WorldToRenderMatrix(NyaMat4x4 world) {
	NyaMat4x4 out;
	out.x = WorldToRenderCoords(world.x);
	out.y = -WorldToRenderCoords(world.y); // v1, up
	out.z = WorldToRenderCoords(world.z);
	out.p = WorldToRenderCoords(world.p);
	return out;
}

// view to world
NyaMat4x4 PrepareCameraMatrix(Camera* pCamera) {
	return pCamera->CurrentKey.Matrix.Invert();
}

// world to view
void ApplyCameraMatrix(Camera* pCamera, NyaMat4x4 mat) {
	auto inv = mat.Invert();
	pCamera->bClearVelocity = true;
	Camera::SetCameraMatrix(pCamera, (bMatrix4*)&inv, 0);
}

void ValueEditorMenu(float& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stof(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
}

void ValueEditorMenu(int& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stoi(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
}

void QuickValueEditor(const char* name, float& value) {
	if (DrawMenuOption(std::format("{} - {}", name, value))) { ValueEditorMenu(value); }
}

void QuickValueEditor(const char* name, int& value) {
	if (DrawMenuOption(std::format("{} - {}", name, value))) { ValueEditorMenu(value); }
}

void QuickValueEditor(const char* name, bool& value) {
	if (DrawMenuOption(std::format("{} - {}", name, value))) { value = !value; }
}