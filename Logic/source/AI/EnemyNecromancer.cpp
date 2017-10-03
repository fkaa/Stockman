#include <AI\EnemyNecromancer.h>
#include <AI\Behavior\RangedBehavior.h>
using namespace Logic;

EnemyNecromancer::EnemyNecromancer(Graphics::ModelID modelID,
	btRigidBody* body, btVector3 halfExtent)
	: Enemy(modelID, body, halfExtent, 10, 5, 0, 0) {
	setBehavior(RANGED);
}

EnemyNecromancer::~EnemyNecromancer()
{
}

void EnemyNecromancer::clear()
{
}

void EnemyNecromancer::onCollision(Entity & other)
{
}

void EnemyNecromancer::onCollision(Player & other)
{
}

void EnemyNecromancer::updateSpecific(Player const & player, float deltaTime)
{
}

void EnemyNecromancer::updateDead(float deltaTime)
{
}
