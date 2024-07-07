#include "pch.h"

namespace Z
{
	Draw::Draw() {}

	Draw::~Draw() {}

	void Draw::DrawLine(float x1, float y1, float x2, float y2, float thickness, ImVec4 color)
	{
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::GetColorU32(color), thickness);
	}

	void Draw::DrawCircle(float x1, float y1, float radius, ImVec4 color)
	{
		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(x1, y1), radius, ImGui::GetColorU32(color));
	}

	void Draw::DrawCircleFilled(float x1, float y1, float radius, ImVec4 color)
	{
		ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(x1, y1), radius, ImGui::GetColorU32(color));
	}

	void Draw::DrawTriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3, ImVec4 color)
	{
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::GetColorU32(color));
	}

	void Draw::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, bool filled, ImVec4 color, float thickness)
	{
		if (filled)
			ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.5f)));

		ImGui::GetBackgroundDrawList()->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::GetColorU32(color), thickness);
	}

	void Draw::DrawBox(float x, float y, float w, float h, bool filled, ImVec4 color)
	{
		if (filled)
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(w, h), ImGui::GetColorU32(ImVec4(color.x, color.y, color.z, 0.2f)));

		DrawLine(x, y, w, y, 1.5f, color);
		DrawLine(x, h, w, h, 1.5f, color);
		DrawLine(x, y, x, h, 1.5f, color);
		DrawLine(w, y, w, h, 1.5f, color);
	}

	void Draw::DrawCornersBox(float x, float y, float w, float h, bool shadow, bool filled, ImVec4 color)
	{
		float hlength = (w - x) / 3;
		float vlength = (h - y) / 3;

		if (filled)
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(w, h), ImGui::GetColorU32(ImVec4(color.x, color.y, color.z, 0.3f)));

		if (shadow)
		{
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x - 1.0f, y - 1.0f), ImVec2(x + hlength + 2.0f, y + 2.0f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.4f)));
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x - 1.0f, y - 1.0f), ImVec2(x + 2.0f, y + vlength + 2.0f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.4f)));

			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(w - hlength - 1.0f, y - 1.0f), ImVec2(w + 1.0f, y + 2.0f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.4f)));
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(w - 2.0f, y - 1.0f), ImVec2(w + 1.0f, y + vlength + 2.0f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.4f)));

			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x - 1.0f, h - 2.0f), ImVec2(x + hlength + 2.0f, h + 1.0f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.4f)));
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x - 1.0f, h - vlength - 1.0f), ImVec2(x + 2.0f, h + 1.0f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.4f)));

			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(w - hlength - 1.0f, h - 2.0f), ImVec2(w + 1.0f, h + 1.0f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.4f)));
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(w - 2.0f, h - vlength - 1.0f), ImVec2(w + 1.0f, h + 1.0f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.4f)));
		}

		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + hlength + 1.0f, y + 1.0f), ImGui::GetColorU32(color));
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + 1.0f, y + vlength + 1.0f), ImGui::GetColorU32(color));

		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(w - hlength, y), ImVec2(w, y + 1.0f), ImGui::GetColorU32(color));
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(w - 1.0f, y), ImVec2(w, y + vlength + 1.0f), ImGui::GetColorU32(color));

		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, h - 1.0f), ImVec2(x + hlength + 1.0f, h), ImGui::GetColorU32(color));
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, h - vlength), ImVec2(x + 1.0f, h), ImGui::GetColorU32(color));

		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(w - hlength, h - 1.0f), ImVec2(w, h), ImGui::GetColorU32(color));
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(w - 1.0f, h - vlength), ImVec2(w, h), ImGui::GetColorU32(color));
	}

	void Draw::DrawString(ImFont* font, std::string text, float x, float y, float size, bool shadow, ImVec4 color)
	{
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;

		if (shadow)
		{
			ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2 - 1.0f, y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2, y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))), text.c_str());

			ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2 + 1.0f, y - 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2 + 1.0f, y), ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))), text.c_str());

			ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2 + 1.0f, y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2, y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))), text.c_str());

			ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2 - 1.0f, y + 1.0f), ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2 - 1.0f, y), ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))), text.c_str());
		}

		ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(x - textWidth / 2, y), ImGui::GetColorU32(color), text.c_str());
	}

	void Draw::VerticalHealthBar(int x, int y, int w, int h, int Health, bool Outlined)
	{
		auto vList = ImGui::GetBackgroundDrawList();

		int healthValue = max(0, min(Health, 100));

		ImColor barColor = ImColor
		(
			min(510 * (100 - healthValue) / 100, 255), min(510 * healthValue / 100, 255),
			25,
			255
		);
		if (Outlined)
			vList->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1), ImColor(0.f, 0.f, 0.f), 0.0f, 0, 1.0f);

		vList->AddRectFilled(ImVec2(x, y + (h - (int)(((float)h / 100.0f) * (float)Health))), ImVec2(x + w, y + h), barColor, 0.0f, 0);
	}
}