#include "Keyboard.h"
#include "cGameObject.h"
#include <iostream>
#include <vector>
#include <glm\vec3.hpp>
#include "cLightManager.h"
#include "LuaBrain.h"
#include "cPlayer.h"
#include "cSceneManager.h"

extern glm::vec3 g_cameraXYZ;
extern glm::vec3 g_cameraTarget_XYZ;

extern sScene* g_pCurrentScene;
extern cGameObject* g_curGameObject;

extern cPlayer* g_pThePlayer;
extern cLightManager*		g_pLightManager;
extern int g_languageNum;
int objectIndex = 0;
int lightIndex = 0;


glm::vec3 vel;
extern bool KeysPressed[Keyboard::KeyCount];

extern bool drawDebugInfo;
const float CAMERASPEED = 2.0f;

void Keyboard::key_resolution(int index) {
	glm::vec3 cameraPos = g_cameraXYZ;
	glm::vec3 forward = cameraPos - g_pCurrentScene->players[g_pCurrentScene->players.size() - 1]->position;
	forward = glm::normalize(forward);
	forward.y = 0.0f;
	glm::vec3 right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
	sProjectile projectile;

	switch (index) {
	case 65:
		g_pThePlayer->movePlayer(right);
		break;
	case 68:
		g_pThePlayer->movePlayer(-right);
		break;
	case 69:
		g_pThePlayer->rotatePlayer(glm::vec3(0.0f, -1.0f, 0.0f));
		break;
	case 81:
		g_pThePlayer->rotatePlayer(glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 83:
		g_pThePlayer->movePlayer(forward);
		break;
	case 87:
		g_pThePlayer->movePlayer(-forward);
		break;


	case 262:
		g_cameraXYZ.x += CAMERASPEED;
		KeysPressed[262] = false;
		break;
	case 263:
		g_cameraXYZ.x -= CAMERASPEED;
		KeysPressed[263] = false;
		break;

	case 264:
		g_cameraXYZ.y -= CAMERASPEED;
		KeysPressed[264] = false;
		break;
	case 265:
		g_cameraXYZ.y += CAMERASPEED;
		KeysPressed[265] = false;
		break;

	case 321:
		g_cameraXYZ.z -= CAMERASPEED;
		KeysPressed[321] = false;
		break;
	case 344:
		g_cameraXYZ.z += CAMERASPEED;
		KeysPressed[344] = false;
		break;
	}
}

void Keyboard::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{


	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (drawDebugInfo)
			drawDebugInfo = false;
		else
			drawDebugInfo = true;
	}

	if (key == GLFW_KEY_LEFT) {
		KeysPressed[263] = true;
	}

	if (key == GLFW_KEY_RIGHT) {
		KeysPressed[262] = true;
	}

	if (key == GLFW_KEY_RIGHT_SHIFT) {
		KeysPressed[344] = true;
	}

	if (key == GLFW_KEY_KP_1) {
		KeysPressed[321] = true;
	}

	if (key == GLFW_KEY_UP) {
		KeysPressed[265] = true;
	}

	if (key == GLFW_KEY_DOWN) {
		KeysPressed[264] = true;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		KeysPressed[65] = true;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		KeysPressed[68] = true;
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		KeysPressed[87] = true;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		KeysPressed[83] = true;
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		KeysPressed[81] = true;
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		KeysPressed[69] = true;
	}


	//release
	if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		KeysPressed[65] = false;
	}

	if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		KeysPressed[68] = false;
	}

	if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		KeysPressed[87] = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		KeysPressed[83] = false;
	}

	if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
		KeysPressed[81] = false;
	}

	if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
		KeysPressed[69] = false;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		sProjectile projectile;
		projectile = Keyboard::createProjectileBasedOnPlayerStats(g_pThePlayer);
		if (projectile.object != NULL)
		{
			g_pThePlayer->setProjectileInUse(projectile);
			//set the projectile position and direction (push the sphere to the edge of the player)
			projectile.object->position = g_pThePlayer->thePlayerObject->position + g_pThePlayer->forwardVector;
			projectile.direction = g_pThePlayer->forwardVector;
			projectile.projectileInitialPosition = g_pThePlayer->thePlayerObject->position;
			//add the game object to draw
			g_pThePlayer->projectilesToDraw.push_back(projectile.object);
			//add the projectile to projectiles for keeping track of movement
			g_pThePlayer->projectiles.push_back(projectile);
		}
	}
	
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	{
		std::cout << "Object #:" << objectIndex 
			<< " X: "<< g_pCurrentScene->players[objectIndex]->position.x
			<< " Y: " << g_pCurrentScene->players[objectIndex]->position.y
			<< " Z: " << g_pCurrentScene->players[objectIndex]->position.z << std::endl;
	}

	return;
}

void Keyboard::setKeys() {
	for (int i = 0; i < KeyCount; i++) {
		KeysPressed[i] = false;
	}
}

sProjectile Keyboard::createProjectileBasedOnPlayerStats(cPlayer* player) {
	sProjectile tempObject;
	int index = g_pThePlayer->getNextProjectileIndex();

	//get the projectile
	if (index >= 0)
	{
		tempObject = g_pThePlayer->projectilePool[index];
	}
	else {
		return sProjectile();
	}

	return tempObject;
}

