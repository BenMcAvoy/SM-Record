#pragma once

#include <memory>
#include <chrono>
#include <thread>
#include <string>
#include <unordered_map>

#include "utils.h"
#include "offsets.h"

#include "console.h"

namespace Carbon::SM {
	enum GameStateType : uint32_t {
		Null,
		Load,
		Play,
		Menu,
		TileEditor,
		WorldBuilder,
	};

	class Contraption {
		SINGLETON(Contraption);

		/* 0x0000 */ PAD(0x58);
		/* 0x0058 */ PUB std::shared_ptr<UTILS::Console> console;
		/* 0x0068 */ PUB int width;
		/* 0x006C */ PUB int height;
		/* 0x0070 */ PAD(0x10C);
		/* 0x017C */ PUB GameStateType gameState;

	public:
		static bool IsPlaying() {
			while (Contraption::GetInstance()->gameState <= Null || Contraption::GetInstance()->gameState > WorldBuilder) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			return Contraption::GetInstance()->gameState == Play;
		}
	};

	class GameSettings {
		SINGLETON(GameSettings);
		/* 0x0000 */ PAD(0x40);
		/* 0x0040 */ PUB std::unordered_map<std::string, float> settings;

		static float GetTargetFPS() {
			auto gameSettings = GameSettings::GetInstance();
			if (!gameSettings) return 60.0f;

			auto it = gameSettings->settings.find("FrameRateCap");
			if (it != gameSettings->settings.end())
				return it->second;

			return 60.0f;
		}
	};
} // namespace Carbon::SM
