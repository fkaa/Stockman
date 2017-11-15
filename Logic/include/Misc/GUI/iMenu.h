#ifndef IMENU_H
#define IMENU_H

#include <Misc\NonCopyable.h>
#include <Graphics\include\RenderQueue.h>
#include <Misc\GUI\Button.h>

namespace Logic
{
    class iMenu : public NonCopyable
    {
    public:
        enum MenuGroup
        {
            Empty,
            Intro,
            Start,
            Settings,
            Skill,
            Card,
            Highscore,
            GameOver
        };

        struct ButtonData
        {
            FloatRect                   screenRect;     // Where the button should be drawn
            FloatRect                   texRectNormal;  // The texture-coordinates on the button-map
            FloatRect                   texRectHover;   // The texture-coordinates on the button-map
            FloatRect                   texRectActive;  // The texture-coordinates on the button-map
            Resources::Textures::Files  texture;        // What buttonmap-texture this button is present inside
            std::function<void(void)>   callback;       // What function this button calls
        };

        iMenu(MenuGroup group);
        ~iMenu();

        void addBackground(Resources::Textures::Files texture, float alpha);
        void addButton(ButtonData btn);
        virtual void update(int x, int y);
        virtual void render() const;

        MenuGroup getMenuType() { return m_group; }

    protected:
        void updateClick(int x, int y);
        void updateHover(int x, int y);

        bool m_pressed;
        bool m_drawButtons;
        bool m_drawMenu;
        MenuGroup m_group;
        std::vector<Button> m_buttons;
        SpriteRenderInfo m_background;
    };
}

#endif // !MENU_H
