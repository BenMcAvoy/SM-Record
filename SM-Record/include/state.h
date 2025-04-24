#pragma once

#include "pch.h"

struct State {
	bool showMenu = true;

	ID3D11Texture2D* copyTexture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	bool recording = false;
	std::filesystem::path recordingPath = "C:\\Users\\Ben\\Desktop\\recording\\";

	// All frames ready to be saved to disk
	std::queue<DirectX::ScratchImage> frames;
	int frameCount = 0;

	void* d3d11Render = nullptr;
};
