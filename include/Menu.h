#pragma once

namespace Z
{
	class Menu
	{
	public:
		static void Dark();
		static void Draw();
		static void MainMenu();

		Menu()  noexcept = default;
		~Menu() noexcept = default;
	};
}