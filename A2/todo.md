# Order of Implementation
1. [X] Implement Vec2 class -> then test in main.cpp.
2. [X] Implement basic functionality of EntityManager class.
	- [X] Implement EntityManager::addEntity() and ::update() (don't worry about deleting dead Entities yet).
3. Implement basics of Game class.
	- [X] Construct a player Entity using spawnPlayer() function.
	- [X] Implement basic drawing of entities using the Game::sRender function.
	- [X] Construct some enemies using the spawnEnemy() function.
		- [X] Random position
		- [X] Random velocity
		- [X] Random vertices
	- [ I'M HERE ] Construct a bullet using the spawnBullet() function.
4. Implement player movement in Game::sUserInput and Game::sMovement
5. Implement the EntityManager::update() function so it deletes dead enemies
6. Implement the EntityManager::getEntities(tag) functionality
7. Implement collisions in sCollision and entity.destroy() if it's dead
	- When (large) enemies collide with bullet or player, are destroyed, and N small enemies are spawn in its place (where N is number of vertices).
	- Each small enemy has same number of vertices and colour, and are fired off at every (360 / vertices) degrees.
8. [X] Implement rest of game's functionality including config file reading
9. Implement GUI functionality (can do this earlier to help debug).
10. Implement sLifespan using RGBA.

- Where to set m_text position to top left, and make score update?
- [ ] Check the position of bullets are set in sRender(), and their movement in sMovement() (should be fine since they have a Transform and Shape)


