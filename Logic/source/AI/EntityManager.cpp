#include "AI/EntityManager.h"
using namespace Logic;

EntityManager::EntityManager()
{
	m_currentWave = 0;
}


EntityManager::~EntityManager()
{
}

void EntityManager::update(float deltaTime) {

}

void EntityManager::spawnWave() {

}

int EntityManager::getEnemiesAlive() const {

}

void EntityManager::clear() {

}

void EntityManager::spawnWave() {
	
}

void EntityManager::setCurrentWave(int currentWave) {
	m_currentWave = currentWave;
}

int EntityManager::getCurrentWave() const {
	return m_currentWave;
}