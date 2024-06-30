#pragma once

#include "Entity.h"

#include <vector>
#include <map>

using EntityVector = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVector>;

class EntityManager
{
public:
	EntityManager();

	void update();
	std::shared_ptr<Entity> addEntity(const std::string& tag);

	const EntityVector& getEntities();	// Shouldn't this return a non-const reference?
	const EntityVector& getEntities(const std::string& tag);
	const std::map<std::string, EntityVector>& getEntityMap();

private:
	EntityVector m_entities;
	EntityVector m_entitiesToAdd;
	EntityMap m_entityMap;
	size_t m_totalEntities = 0;

	void removeDeadEntities(EntityVector& vector);
};