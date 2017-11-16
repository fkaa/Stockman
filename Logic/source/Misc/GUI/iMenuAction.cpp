#include <Misc\GUI\iMenuAction.h>
#include <StateMachine\StateBuffer.h>
#include <StateMachine\StatePrimary.h>
#include <StateMachine\StateSecondary.h>
#include <StatePlaying.h>
#include <Misc\GUI\iMenuMachine.h>
#include <Misc\GUI\iMenuSkillPick.h>
#include <Engine\Typing.h>
#include <Engine\Settings.h>
#include <State.h>

using namespace Logic;

// Switches both main-states to in-game, will unload & load everything
void ButtonFunction::startGame()
{
    if (Action::Get().m_stateBuffer->currentPrimaryState)
        if (StatePrimary* primary = dynamic_cast<StatePrimary*>(Action::Get().m_stateBuffer->currentPrimaryState))
            primary->queueState(StateType::State_Playing);
    if (Action::Get().m_stateBuffer->currentSecondaryState)
        if (StateSecondary* secondary = dynamic_cast<StateSecondary*>(Action::Get().m_stateBuffer->currentSecondaryState))
            secondary->queueState(StateType::Nothing);
}

// Switches the current menu-machine to settings screen
void ButtonFunction::startSettings()
{
    if (Action::Get().m_menuMachine)
        Action::Get().m_menuMachine->queueMenu(iMenu::MenuGroup::Settings);
}

// Switches the current menu-machine to start screen
void ButtonFunction::startMainMenu()
{
    if (Action::Get().m_menuMachine)
        Action::Get().m_menuMachine->queueMenu(iMenu::MenuGroup::Start);
}

// Switches the current menu-machine to highscore screen
void ButtonFunction::showHighscore()
{
    if (Action::Get().m_menuMachine)
        Action::Get().m_menuMachine->queueMenu(iMenu::MenuGroup::Highscore);
}

// Unloads all program data and quits the game
void ButtonFunction::quitGame()
{

}

void ButtonFunction::writing()
{
    Typing* theChar = Typing::getInstance(); //might need to be deleted
    char trashThis = theChar->getSymbol();
}

void ButtonFunction::chooseUpgrade1()
{
    chooseUpgrade(0);
}

void ButtonFunction::chooseUpgrade2()
{
    chooseUpgrade(1);
}

void ButtonFunction::chooseUpgrade3()
{
    chooseUpgrade(2);
}

void Logic::chooseUpgrade(int index)
{
    if (StatePrimary* primary = dynamic_cast<StatePrimary*>(Action::Get().m_stateBuffer->currentPrimaryState))
    {
        if (StatePlaying* playing = dynamic_cast<StatePlaying*>(primary->getCurrentState()))
        {
            if (playing->getCardManager()->pickAndApplyCard(*playing->getPlayer(), index))
            {
                Action::Get().m_menuMachine->queueMenu(iMenu::MenuGroup::Empty);

                // Enabling mouse movement
                DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
            }
        }
    }
}

// Just ignore how ugly this is
void ButtonFunction::confirmSkillPicks()
{
    if (StatePrimary* primary = dynamic_cast<StatePrimary*>(Action::Get().m_stateBuffer->currentPrimaryState))
    {
        if (StatePlaying* playing = dynamic_cast<StatePlaying*>(primary->getCurrentState()))
        {
            if (iMenuSkillPick* skillpick = dynamic_cast<iMenuSkillPick*>(Action::Get().m_menuMachine->getActiveMenu()))
            {
                // Check if skills are picked correcly
                int primary = skillpick->getPrimarySkill();
                int secondary = skillpick->getSecondarySkill();
                if (primary == -1 || secondary == -1)
                    return;

                // Set the skills on the player
                playing->getPlayer()->setCurrentSkills(primary, secondary);

                // Removing active menu
                Action::Get().m_menuMachine->queueMenu(iMenu::MenuGroup::Empty);

                // Enabling mouse movement
                DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
            }
        }
    }
}

void ButtonFunction::plusSense()
{
    Settings settings = Settings::getInstance();
    settings.setFOV(settings.getFOV() + 0.01f);
}

void ButtonFunction::minusSense()
{
    Settings settings = Settings::getInstance();
    settings.setFOV(settings.getFOV() - 0.01f);
}

void ButtonFunction::plusMaster()
{
    Settings settings = Settings::getInstance();
    settings.setMasterSound(settings.getMasterSound() + 0.01f);
}

void ButtonFunction::minusMaster()
{
    Settings settings = Settings::getInstance();
    settings.setMasterSound(settings.getMasterSound() - 0.01f);
}

void ButtonFunction::plusSFX()
{
    Settings settings = Settings::getInstance();
    settings.setSFX(settings.getSFX() + 0.05f);
}

void ButtonFunction::minusSFX()
{
    Settings settings = Settings::getInstance();
    settings.setSFX(settings.getSFX() - 0.05f);
}

void ButtonFunction::muteUnmute()
{

}

void ButtonFunction::plusFOV()
{
    Settings settings = Settings::getInstance();
    settings.setFOV(settings.getFOV() + 1);
}

void ButtonFunction::minusFOV()
{
    Settings settings = Settings::getInstance();
    settings.setFOV(settings.getFOV() - 1);
}

void ButtonFunction::windowed()
{
    Settings settings = Settings::getInstance();
    settings.setWindowed(!settings.getWindowed());
}