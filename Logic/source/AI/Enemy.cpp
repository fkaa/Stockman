#include <AI\Enemy.h>
#include <AI\Behavior\TestBehavior.h>
using namespace Logic;

Enemy::Enemy(Graphics::ModelID modelID, btRigidBody* body, btVector3 halfExtent, float health, float baseDamage, float moveSpeed, int enemyType, int animationId)
: Entity(body, halfExtent, modelID)
{
	m_behavior = nullptr;

	m_health = health;
	m_baseDamage = baseDamage;
	m_moveSpeed = moveSpeed;
	m_enemyType = enemyType;

	//animation todo
}

void Enemy::setBehavior(BEHAVIOR_ID id)
{
	if (m_behavior)
		delete m_behavior;
	m_behavior = nullptr;

	switch (id) 
	{
		case TEST:
			m_behavior = new TestBehavior();
	}
}

Enemy::~Enemy() {
	if (m_behavior)
		delete m_behavior;
}

void Enemy::update(Player const &player, float deltaTime) {
	Entity::update(deltaTime);
	updateSpecific(player, deltaTime);

	if (m_behavior)
		m_behavior->update(*this, player, deltaTime); // BEHAVIOR IS NOT DONE, FIX LATER K

	m_moveSpeedMod = 0.f; // Reset effect variables, should be in function if more variables are added.
}

void Enemy::damage(float damage)
{
	m_health -= damage;
}

void Enemy::affect(int stacks, Effect const &effect, float dt) 
{
	int flags = effect.getStandards()->flags;

	if (flags & Effect::EFFECT_KILL)
		damage(m_health);
	if (flags & Effect::EFFECT_ON_FIRE)
		damage(effect.getModifiers()->modifyDmgTaken * dt);
	if (flags & Effect::EFFECT_MODIFY_MOVEMENTSPEED)
		m_moveSpeedMod += effect.getModifiers()->modifyMovementSpeed;
}

float Enemy::getHealth() const
{
	return m_health;
}

float Enemy::getMaxHealth() const
{
	return m_maxHealth;
}

float Enemy::getBaseDamage() const
{
	return m_baseDamage;
}

float Enemy::getMoveSpeed() const
{
	return m_moveSpeed + m_moveSpeedMod;
}

int Enemy::getEnemyType() const
{
	return m_enemyType;
}