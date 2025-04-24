#include "pch.h"
#include "hook.h"

void Render(State* state) {
	auto targetFps = Carbon::SM::GameSettings::GetTargetFPS();
    if (state->recording) {
        if (state->copyTexture && state->srv) {
            // Create a staging texture to copy the current texture into
            ID3D11DeviceContext* context = nullptr;
            ID3D11Device* pDevice = nullptr;
            state->copyTexture->GetDevice(&pDevice);
            pDevice->GetImmediateContext(&context);

            // Create a staging texture description
            D3D11_TEXTURE2D_DESC textureDesc;
            state->copyTexture->GetDesc(&textureDesc);

            textureDesc.Usage = D3D11_USAGE_STAGING;
            textureDesc.BindFlags = 0;
            textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

            // Create the staging texture
            ID3D11Texture2D* stagingTexture = nullptr;
            HRESULT hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &stagingTexture);
            if (FAILED(hr)) {
                // Handle failure
                return;
            }

            // Copy the current texture to the staging texture
            context->CopyResource(stagingTexture, state->copyTexture);

            // Map the staging texture to access its data
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            hr = context->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
            if (FAILED(hr)) {
                // Handle failure
                stagingTexture->Release();
                return;
            }

            DirectX::ScratchImage scratchImage;
            hr = scratchImage.Initialize2D(
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
                textureDesc.Width,
                textureDesc.Height,
                1,                          // arraySize (typically 1)
                1,                          // mipLevels (1 for no mipmaps)
                DirectX::CP_FLAGS_NONE);    // flags
            if (FAILED(hr)) {
                // Handle failure
                context->Unmap(stagingTexture, 0);
                stagingTexture->Release();
                return;
            }

            const uint8_t* srcData = reinterpret_cast<const uint8_t*>(mappedResource.pData);
            size_t rowPitch = mappedResource.RowPitch;

            const DirectX::Image* image = scratchImage.GetImage(0, 0, 0);
            uint8_t* destData = image->pixels;
            size_t destRowPitch = image->rowPitch;

            for (size_t y = 0; y < textureDesc.Height; ++y) {
                memcpy(destData + y * destRowPitch, srcData + y * rowPitch, textureDesc.Width * 4);
            }

            // Push the loaded frame into the queue
            state->frames.push(std::move(scratchImage));

            // Unmap the staging texture and release it
            context->Unmap(stagingTexture, 0);
            stagingTexture->Release();
        }
    }

	if (!state->showMenu) return;

	auto contraption = Carbon::SM::Contraption::GetInstance();

	auto w = contraption->width / 8;
	auto h = contraption->height / 8;

	float titleBarHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y;
	ImGui::SetNextWindowSize(ImVec2(w, h + titleBarHeight));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::Begin("SM-Recorder##Preview", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
	ImGui::Image(state->srv, ImVec2(w, h));
	ImGui::End();
	ImGui::PopStyleVar(3);

	ImGui::Begin("SM-Recorder");

	if (ImGui::Button(state->recording ? "Stop Recording" : "Start Recording")) {
		state->recording = !state->recording;
	}

    if (ImGui::Button("Reset recording")) {
		state->frameCount = 0;
		while (!state->frames.empty()) {
			state->frames.pop();
		}

        for (const auto& entry : std::filesystem::directory_iterator(state->recordingPath)) {
            if (entry.is_regular_file()) {
                std::filesystem::remove(entry.path());
            }
        }
    }

	ImGui::Text("Target FPS: %.0f", targetFps);
	ImGui::End();

	auto bgDl = ImGui::GetBackgroundDrawList();
	auto& io = ImGui::GetIO();
	bgDl->AddCircleFilled(io.MousePos, 5.0f, IM_COL32(255, 255, 255, 200), 4);
}

void Update(State* state) {
	//CINFO("Frames remaining: {}", state->frames.size());
	size_t frameCount = state->frames.size();
	if (frameCount > 10) {
		CWARN("Frame debt: {} frames", frameCount);
    }

	if (state->frames.empty()) return;

	auto& frame = state->frames.front();

	// Save the frame to a file
	auto saveDir = state->recordingPath;
    auto fileName = std::format(L"{}/frame_{}.png", saveDir.wstring(), state->frameCount);
	DirectX::SaveToWICFile( // WIC = Windows Imaging Component (will save as a .png)
		frame.GetImages(),
		frame.GetImageCount(),
        DirectX::WIC_FLAGS_NONE,
		GUID_ContainerFormatPng,
		fileName.c_str()
	);
	state->frameCount++;
	state->frames.pop();
}

void Setup(State* state) {
	std::filesystem::create_directories(state->recordingPath);
}
