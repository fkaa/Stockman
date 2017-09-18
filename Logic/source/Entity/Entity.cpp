#include <Entity/Entity.h>

using namespace Logic;

Entity::Entity(btRigidBody* body)
{
	m_body = body;
	m_transform = &m_body->getWorldTransform();
	m_addProjectile = nullptr;
}

Entity::Entity(btRigidBody* body, std::function<void(ProjectileData* pData, btVector3 forward)> addProjectilePtr)
{
	m_body = body;
	m_transform = &m_body->getWorldTransform();
	m_addProjectile = addProjectilePtr;
}

Entity::~Entity() 
{
	// ALL physics is getting cleared by the Physics class, 
}

void Entity::clear() { }

void Entity::update(float deltaTime)
{
	for (auto &effectPair : m_statusManager.getActiveEffects()) //opt
		affect(effectPair.first, *effectPair.second, deltaTime);
	m_statusManager.update(deltaTime);
	updateSpecific(deltaTime);

    setWorldMatrix(getTransformMatrix());
}

void Entity::collision(Entity& other)
{
	onCollision(other);
}

void Entity::affect(int stacks, Effect const &effect, float dt) {}

btRigidBody* Entity::getRigidbody()
{
	return m_body;
}

// JUST FOR TESTING, REMOVE
void Entity::consoleWritePosition()
{
	printf("Position = { %f, %f, %f }\n", m_transform->getOrigin().getX(), m_transform->getOrigin().getY(), m_transform->getOrigin().getZ());
}

void Logic::Entity::spawnProjectile(ProjectileData* pData, btVector3 forward)
{
	if(m_addProjectile)
		m_addProjectile(pData, forward);
}

DirectX::SimpleMath::Vector3 Entity::getPosition() const
{
	return DirectX::SimpleMath::Vector3(m_transform->getOrigin());
}

DirectX::SimpleMath::Quaternion Entity::getRotation() const
{
	return DirectX::SimpleMath::Quaternion(m_transform->getRotation());
}

DirectX::SimpleMath::Vector3 Entity::getScale() const
{
	return DirectX::SimpleMath::Vector3(m_body->getCollisionShape()->getLocalScaling());
}

DirectX::SimpleMath::Matrix Logic::Entity::getTransformMatrix() const
{
	// Making memory for a matrix
	float* m = new float[4 * 16];

	// Getting this entity's matrix
	m_transform->getOpenGLMatrix((btScalar*)(m));

	// Translating to DirectX Math and assigning the variables
	DirectX::SimpleMath::Matrix transformMatrix(m);

	// Deleting the old created variables from memory
	delete m;

	return transformMatrix;
}
