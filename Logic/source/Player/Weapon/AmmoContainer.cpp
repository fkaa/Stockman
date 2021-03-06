#include <Player\Weapon\AmmoContainer.h>

using namespace Logic;

AmmoContainer::AmmoContainer()
{
    m_aInfo = AmmoInfo();
}

AmmoContainer::AmmoContainer(AmmoInfo ammoInfo)
{
    m_aInfo = ammoInfo;
}

AmmoContainer::~AmmoContainer()
{
}

void Logic::AmmoContainer::reset()
{
    m_aInfo.enhancedAmmo = 0;
    m_aInfo.magAmmo = m_aInfo.magSize;
}

const AmmoContainer::AmmoInfo& AmmoContainer::getAmmoInfo() const
{
    return m_aInfo;
}

bool AmmoContainer::removeAmmo(int weapon)
{
    if (m_aInfo.ammoConsumption[weapon] > m_aInfo.magAmmo)
        m_aInfo.magAmmo = 0;
    else
        m_aInfo.magAmmo -= m_aInfo.ammoConsumption[weapon];

    if (m_aInfo.enhancedAmmoConsumption[weapon] != 0 && m_aInfo.enhancedAmmo > 0)
    {
        if (m_aInfo.enhancedAmmoConsumption[weapon] > m_aInfo.enhancedAmmo)
            m_aInfo.enhancedAmmo = 0;
        else
            m_aInfo.enhancedAmmo -= m_aInfo.enhancedAmmoConsumption[weapon];

        return true; // return true if using enhanced ammo
    }
    
    return false; // return false if just using endless ammo
}

void AmmoContainer::setEnhancedAmmo(int ammo)
{
    m_aInfo.enhancedAmmo = ammo;
}

bool AmmoContainer::getIsEnhanced()
{
    return (m_aInfo.enhancedAmmo != 0);
}

void AmmoContainer::fillMag(float modifier)
{
    m_aInfo.magAmmo = static_cast<int>(m_aInfo.magSize * modifier);

    // *OLD*
    /*int toAdd = m_aInfo.magSize + modifier - m_aInfo.magAmmo;

    if (m_aInfo.ammo >= toAdd)
    {
        m_aInfo.ammo -= toAdd;		// Remove ammo from total
        m_aInfo.magAmmo = m_aInfo.magSize + modifier;	// Add ammo to mag
    }
    else
    {
        m_aInfo.magAmmo += m_aInfo.ammo;	// Add rest of ammo to mag
        m_aInfo.magAmmo = m_aInfo.magSize + modifier;				// Remove rest of ammo from total
    }*/
}