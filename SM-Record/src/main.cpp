#include "pch.h"
#include "hook.h"

void Render(State* state) {
	if (!state->showMenu) return;

	ImGui::Begin("Hey!");

	ImGui::SeparatorText("Welcome to CarbonTemplateGUI");
	ImGui::Text("This is a template for Scrap Mechanic mods using ImGui based overlays.");
	ImGui::Text("It has support for DirectX 11, window resizing, and uses CarbonLib");
	ImGui::Text("You can press INSERT to toggle this menu");
	ImGui::Separator();
	ImGui::Text("Have fun!");
	ImGui::SameLine();
	if (ImGui::Button("Poke me")) {
		CINFO("You poked me!");
	}
	ImGui::SetItemTooltip("Check the console for the message (only works when compiled in release mode)");

	ImGui::End();
}

void Update(State*) {
	// Update code here
}
