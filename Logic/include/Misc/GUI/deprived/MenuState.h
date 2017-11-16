#ifndef MENUSTATE_H
#define MENUSTATE_H

#pragma region Comment

/*
Class: MenuState
Author: Andreas Eriksson

Description:
This class creates menu states.

*/
#pragma endregion Description of class

#include <vector>
#include <Misc\GUI\Button.h>


#pragma endregion Description of class

namespace Logic
{
	class MenuState
	{
	private:
		Graphics::MenuInfo m_menu;
		std::vector<Button> m_buttons;		//< List of buttons on current menu
	public:

		struct ButtonStruct
		{
			float xPos;
			float yPos;
			float xTexStart;
			float yTexStart;
			float xTexEnd;
			float yTexEnd;
            float activeOffset;
			float height;
			float width;
			int texture;
			std::function<void(void)> m_CallBackFunction; //must change to a function aqquiering
		};

		MenuState();
		~MenuState();
		void initialize(std::vector<ButtonStruct> buttonStruct, int background);
		void updateOnPress(int posX, int posY);
        void hoverOver(int posX, int posY);
		bool animationTransition(float dt, float maxAnimationTime, bool forward);

        virtual void render() const;
        Button* getButton(int index);
	};
}

#endif