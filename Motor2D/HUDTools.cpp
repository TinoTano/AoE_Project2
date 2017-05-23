#include "Application.h"
#include "Gui.h"
#include "Fonts.h"

void HUD::AlertText(string text, uint duration) {
	App->gui->hud->alert.Start(text, duration, TEXTALERTPOS_X - App->render->camera.x, TEXTALERTPOS_Y - App->render->camera.y);
}

void TextAlert::Start(string text, uint duration, uint x, uint y) {

	active = true;
	if (textalert_lbl == nullptr) {
		textalert_lbl = (Label*)App->gui->CreateLabel(text, x, y, App->font->fonts[TWENTY]);
		textalert_lbl->SetColor({ 255,255,255,255 });
	}
	this->duration = duration;
	text_duration.Start();
	text_flicker.Start();

}
void TextAlert::Update() {
	if (active) {
		if (text_flicker.ReadSec() > 1) {
			switch (color)
			{
			case 0:
				textalert_lbl->SetColor({ 255, 245, 30, 255 });
				color = 1;
				text_flicker.Start();
				break;
			case 1:
				textalert_lbl->SetColor({ 255,255,255,255 });
				text_flicker.Start();
				color = 0;
				break;
			default:
				break;
			}
		}

		if (text_duration.ReadSec() >= duration) {
			CleanUp();
		}
	}
}

void TextAlert::CleanUp() {
	App->gui->DestroyUIElement(textalert_lbl);
	textalert_lbl = nullptr;
	color = 0;
	active = false;
}