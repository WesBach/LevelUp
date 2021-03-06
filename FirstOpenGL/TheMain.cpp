#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>			// C++ cin, cout, etc.
//#include "linmath.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <random>
#include <time.h>

#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>		
#include <string>

#include <vector>	
#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "cShader.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"
#include "Physics.h"	// Physics collision detection functions
#include "cLightManager.h"
#include "Keyboard.h"
#include "DebugRenderer.h"
#include"CommandManager.h"
#include "OrientToTime.h"
#include "CTextureManager.h"
#include "RenderUtilities.h"
#include <iPhysicsFactory.h>
#include <enums.h>
#include "cPathingManager.h"
#include "XMLReader.h"
#include "cTextRenderer.h"
#include "cFBO.h" 
#include "cPlayer.h"
#include "cSceneManager.h"
#include "cEnemy.h"
#include "cParticleManager.h"
#include "cPowerUp.h"
#include "cSoundManager.h"

bool loadTextures = false;
int height = 480;	/* default */
int width = 640;	// default

bool LoadLightsAttributesFromFile(std::string& fileName, std::vector<cLight>& theLights);
void renderPlayerInfo(int winWidth, int winHeight);
void printError(const std::string& error);
void createProjectiles();
void updateProjectilePositions(double deltaTime);
void checkForProjectileCollisions(float deltaTime);
void checkObjectBounds();
void removeEnemyOnEnemyDeath();
void restartOnPlayerDeath();
void loadNextLevel();
void handleExplosions(float deltaTime);
void checkForPowerUpCollisions();
void checkPlayerProjectileRanges();
void checkPlayerHealth();

std::string modelAndLightFile = "assets/GameInfoFiles/SceneInfo.txt";

bool KeysPressed[Keyboard::KeyCount];
cVAOMeshManager*	g_pVAOManager = 0;		// or NULL, or nullptr
cShaderManager*		g_pShaderManager;		// Heap, new (and delete)
cLightManager*		g_pLightManager;
DebugRenderer*		g_pTheDebugrender;
CTextureManager*	g_pTextureManager;
cParticleManager*	g_pParticleManager;
cSoundManager*		g_pSoundManager;
std::vector<cEnemy> g_vecEnemies;
std::vector<cEnemy> g_vecExplodedEnemies;
std::vector<cPowerUp*> g_vecPowerUps;

cPlayer* g_pThePlayer;
sScene* g_pCurrentScene;
cPathingManager* g_thePathingManager;
cFBO* g_pFBO;
cSceneManager* g_pSceneManager;
cTextRenderer* g_ptheTextRenderer;
XMLReader* g_pTheXMLReader;

int g_languageNum = 6;
//physics
nPhysics::iPhysicsWorld* theWorld;
nPhysics::iPhysicsFactory* theFactory;

GLint sexyShaderID;

glm::vec3 g_cameraXYZ = glm::vec3(0.0f);
glm::vec3 g_cameraTarget_XYZ = glm::vec3(0.0f);

int g_GameObjectIndex = 0;
cGameObject *g_curGameObject;

//RANDOM
std::random_device randd; // obtain a random number from hardware
std::mt19937 engine(rand()); // seed the generator
std::uniform_int_distribution<> distribution(1, 40);

double g_lastTimeStep = 0.0;

//for the movement of the teapot
CommandManager* theParallelCommands;
CommandManager* theSerialCommands = new CommandManager(false);

glm::vec3 maxDimensions(35.0f, 0.0f, 35.0f);
glm::vec3 minDimensions(-35.0f, 0.0f, -35.0f);

bool drawDebugInfo = true;
GLFWwindow* g_pGLFWWindow = NULL;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char** argv)
{
	//set the initial keys to false
	Keyboard::setKeys();
	g_curGameObject = new cGameObject();

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	std::string title = "OpenGL Rocks";

	loadConfigFile(width, height, title);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	g_pGLFWWindow = glfwCreateWindow(width, height,
		title.c_str(),
		NULL, NULL);

	if (!g_pGLFWWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//static key_callback is from Keyboard.h
	glfwSetKeyCallback(g_pGLFWWindow, Keyboard::key_callback);
	glfwMakeContextCurrent(g_pGLFWWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	//print versions
	std::cout << glGetString(GL_VENDOR) << " "
		<< glGetString(GL_RENDERER) << ", "
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	::g_pShaderManager = new cShaderManager();
	cShader vertShader;
	cShader fragShader;
	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";
	::g_pShaderManager->setBasePath("assets//shaders//");

	// Shader Manager
	if (!::g_pShaderManager->createProgramFromFile(
		"mySexyShader", vertShader, fragShader))
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;
		//		exit(
	}
	//////////////////
	//Scene manager///
	/////////////////
	::g_pSceneManager = new cSceneManager();
	::g_pVAOManager = new cVAOMeshManager();
	sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
	//Load models 
	std::string error;

	if (g_pSceneManager->LoadModelsFromModelInfoFile(std::string("assets/GameInfoFiles/ModelInfo.txt"), g_pVAOManager, sexyShaderID) == false) {
		std::cout << "Failed Loading Files Into Mesh Manager!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;
	}
	g_pSceneManager->LoadSceneFromFileIntoSceneMap(std::string("assets/GameInfoFiles/Scene1.txt"), 0);
	g_pSceneManager->LoadSceneFromFileIntoSceneMap(std::string("assets/GameInfoFiles/Scene2.txt"), 1);
	g_pSceneManager->LoadSceneFromFileIntoSceneMap(std::string("assets/GameInfoFiles/Scene3.txt"), 2);
	g_pSceneManager->LoadSceneFromFileIntoSceneMap(std::string("assets/GameInfoFiles/Scene4.txt"), 3);

	//populate scene appropriately
	sScene tempScene = g_pSceneManager->getSceneById(0);
	g_pCurrentScene = new sScene();
	g_pSceneManager->copySceneFromCopyToPointer(tempScene, g_pCurrentScene);

	//populate the player
	g_pThePlayer = new cPlayer();
	g_pThePlayer->thePlayerObject = g_pCurrentScene->players[0];
	g_pThePlayer->currentHealth = 100;
	g_pThePlayer->playerSpeed = 4.0f;
	g_pThePlayer->rotationSpeed = 2.0f;
	createProjectiles();
	//populate the enemies
	g_pSceneManager->populateEnemies(g_vecEnemies,g_pCurrentScene);
	g_pSceneManager->configurePowerUpsForScene(g_pCurrentScene, g_vecPowerUps);

	////////////////////////
	////PARTICLE MANAGER///
	//////////////////////
	//create the particle manager
	g_pParticleManager = new cParticleManager();
	//create enough emitters for each enemy
	g_pParticleManager->createEmitters(g_vecEnemies.size());
	g_pParticleManager->connectEmittersWithEntities(g_vecEnemies);

	  /////////////////////
	 //	Debug Renderer	//
	/////////////////////
	::g_pTheDebugrender = new DebugRenderer();
	if(!::g_pTheDebugrender->initialize(error))
	{
		printError(error);
	}

	  /////////////////////
	 //	Text Renderer	//
	/////////////////////
	g_ptheTextRenderer = new cTextRenderer();
	if (!g_ptheTextRenderer->initializeTextRenderer(error))
	{
		printError(error);
	}
	
	  /////////////////
	 //	XML Reader	//
	/////////////////
	g_pTheXMLReader = new XMLReader();
	g_pTheXMLReader->load_languages_from_xml(L"assets/xml/localization.xml");

	  /////////////////
	 //	Light Loader	//
	/////////////////
	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
	//getUniformLocations(mvp_location, currentProgID);

	//load lights
	::g_pLightManager = new cLightManager();
	::g_pLightManager->CreateLights(20);
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);
	if(!LoadLightsAttributesFromFile(std::string("assets/GameInfoFiles/LightInfo.txt"), ::g_pLightManager->vecLights))
	{
		error = "Lights not loaded properly! Line 249:main";
		printError(error);
	}

	  ////////////////////
	 //	Texture Loader //
	////////////////////

	//do the texture loading
	g_pTextureManager = new CTextureManager();
	g_pTextureManager->setBasePath("assets//textures");

	loadTextures = LoadTexturesFromFile(std::string("assets/GameInfoFiles/TextureInfo.txt"));

	::g_pTextureManager->setBasePath("assets/textures/skybox");
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles(
		"space",
		"SpaceBox_right1_posX.bmp",
		"SpaceBox_left2_negX.bmp",
		"SpaceBox_top3_posY.bmp",
		"SpaceBox_bottom4_negY.bmp",
		"SpaceBox_front5_posZ.bmp",
		"SpaceBox_back6_negZ.bmp", true, true))
	{
		std::cout << "Didn't load skybox" << std::endl;
	}

	::g_pTextureManager->setBasePath("assets/textures/skybox");
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles(
		"sunny",
		"TropicalSunnyDayRight2048.bmp",
		"TropicalSunnyDayLeft2048.bmp",
		"TropicalSunnyDayDown2048.bmp",//TropicalSunnyDayUp2048
		"TropicalSunnyDayUp2048.bmp",//TropicalSunnyDayDown2048
		"TropicalSunnyDayFront2048.bmp",
		"TropicalSunnyDayBack2048.bmp", true, true))
	{
		std::cout << "Didn't load skybox" << std::endl;
	}

	//load the textures for the scene
	g_pSceneManager->loadLevelTextures(g_pCurrentScene);

	//////////////////
	//SOUND MANAGER//
	/////////////////
	g_pSoundManager = new cSoundManager();
	g_pSoundManager->initSoundManager();
	g_pSoundManager->readSoundsFromSoundFile("assets/GameInfoFiles/SoundInfo.txt");
	g_pSoundManager->setInitialBackgroundSound();

	glEnable(GL_DEPTH);
	glCullFace(GL_BACK);
	// Gets the "current" time "tick" or "step"
	g_lastTimeStep = glfwGetTime();

	g_pFBO = new cFBO();

	if (!g_pFBO->init(1920, 1080, error))
	{
		std::cout << "FBO error: " << error << std::endl;
	}
	else
	{
		std::cout << "FBO is good." << std::endl;
		std::cout << "\tFBO ID = " << g_pFBO->ID << std::endl;
		std::cout << "\tcolour texture ID = " << g_pFBO->colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << g_pFBO->normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << g_pFBO->getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << g_pFBO->getMaxDrawBuffers() << std::endl;
	}



	// Main game or application loop
	while (!glfwWindowShouldClose(g_pGLFWWindow))
	{
		float ratio;
		int width, height;
		g_cameraTarget_XYZ = g_pCurrentScene->players[0]->position;
		//make the camera follow the player objects
		g_cameraXYZ.x = g_pThePlayer->thePlayerObject->position.x;

		glfwGetFramebufferSize(g_pGLFWWindow, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear colour AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		::g_pShaderManager->useShaderProgram("mySexyShader");
		GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

		// Update all the light uniforms...
		// (for the whole scene)
		::g_pLightManager->CopyLightInformationToCurrentShader();

		// Projection and view don't change per scene (maybe)
		glm::mat4x4 p = glm::perspective(0.6f,			// FOV
			ratio,		// Aspect ratio
			0.1f,			// Near (as big as possible)
			1000.0f);	// Far (as small as possible)

		//glm::mat4x4 p = glm::ortho(0, width, 0, height);
		// View or "camera" matrix
		glm::mat4 v = glm::mat4(1.0f);	// identity

		v = glm::lookAt(g_cameraXYZ,		// "eye" or "camera" position
			g_cameraTarget_XYZ,				// "At" or "target" 
			glm::vec3(0.0f, 1.0f, 0.0f));	// "up" vector

		GLuint uniLoc_mView = glGetUniformLocation(currentProgID, "mView");
		GLuint uniLoc_mProjection = glGetUniformLocation(currentProgID, "mProjection");
		glUniformMatrix4fv(uniLoc_mView, 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(v));
		glUniformMatrix4fv(uniLoc_mProjection, 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(p));

		for (int i = 0; i < Keyboard::KeyCount; i++) {
			if (KeysPressed[i] == true) {
				Keyboard::key_resolution(i);
			}
		}

		double curTime = glfwGetTime();
		double deltaTime = curTime - g_lastTimeStep;

		//make sure the objects dont go outside the bounds
		checkObjectBounds();


		//render the scene
		renderScene(::g_pCurrentScene->terrain, g_pGLFWWindow);
		renderScene(::g_pCurrentScene->enemies, g_pGLFWWindow);
		renderScene(::g_pCurrentScene->players, g_pGLFWWindow);
		renderScene(::g_pCurrentScene->powerUps, g_pGLFWWindow);

		//Render the enemy projectiles
		for (int i = 0; i < g_vecEnemies.size(); i++)
		{
			renderScene(g_vecEnemies[i].projectilesToDraw, g_pGLFWWindow);
		}

		if (g_pThePlayer->projectilesToDraw.size() > 0)
		{
			renderScene(::g_pThePlayer->projectilesToDraw, g_pGLFWWindow);
		}

		//draw explosions
		for (int i = 0; i < g_vecExplodedEnemies.size(); i++) {
			renderScene(::g_vecExplodedEnemies[i].explosion, g_pGLFWWindow);
		}

		//Deal with the ai actions
		for (int i = 0; i < g_pCurrentScene->enemies.size(); i++) {
			if (g_pCurrentScene->enemies[i]->theState != NULL) {
				//of the type is a follower make sure to check all the objects in the scene to see what one is closest
				StateType type = g_pCurrentScene->enemies[i]->theState->getStateType();
				if (type == StateType::FOLLOWER) {
					//have all the objects in the same vector
					std::vector<cGameObject*> theCopyVec = g_pCurrentScene->enemies;
					std::vector<glm::vec3> thePositions;
					//sort the vector and get the closest one 
					for (int copyIndex = 0; copyIndex < theCopyVec.size(); copyIndex++) {
						if (copyIndex == i) {
							continue;
						}

						thePositions.push_back(theCopyVec[copyIndex]->position);
					}

					glm::vec3 closest(0.0f);
					float distance = 0.0f;

					//if it's the only enemy left in the vector jsut test against the player
					if (thePositions.size() == 0) {
						//perform action against the player
						g_pCurrentScene->enemies[i]->theState->performAction(g_pCurrentScene->players[0], g_pCurrentScene->enemies[i], (float)1 / 60);
						continue;
					}

					//otherwise find the closest enemy
					distance = abs(glm::distance(g_pCurrentScene->enemies[i]->position, thePositions[0]));
					//get the closest position
					for (int distIndex = 1; distIndex < thePositions.size(); distIndex++){
						float currDist = abs(glm::distance(g_pCurrentScene->enemies[i]->position, thePositions[distIndex]));
						
						if (currDist < distance) {
							distance = currDist;
							closest = thePositions[distIndex];
						}
					}

					//test against closest enemy
					for (int objectIndex = 0; objectIndex < theCopyVec.size(); objectIndex++) {
						//find the closest enemy adn perform an action
						if (closest == theCopyVec[objectIndex]->position) {
							g_pCurrentScene->enemies[i]->theState->performAction(theCopyVec[objectIndex], g_pCurrentScene->enemies[i], (float)1 / 60);
							break;
						}
					}

				}
				g_pCurrentScene->enemies[i]->theState->performAction(g_pCurrentScene->players[0], g_pCurrentScene->enemies[i], (float)1 / 60);
			}
		}//end ai actions

		std::stringstream ssTitle;
		ssTitle << "Camera (xyz): "
			<< g_cameraXYZ.x << ", "
			<< g_cameraXYZ.y << ", "
			<< g_cameraXYZ.z;
		glfwSetWindowTitle(g_pGLFWWindow, ssTitle.str().c_str());

		//update projectile positions
		updateProjectilePositions(deltaTime);
		//handle explosions
		handleExplosions(deltaTime);
		//check for collisions between projectiles entities	
		checkForProjectileCollisions(deltaTime);
		checkForPowerUpCollisions();

		//render the particles last so that transparency works properly
		g_pParticleManager->updateEmitterPositions(g_vecEnemies, g_pThePlayer);
		g_pParticleManager->drawActiveParticles(sexyShaderID);
		//update particles 
		g_pParticleManager->updateLivingParticles(deltaTime);

		//make sure the player projectiles don't go too far
		checkPlayerProjectileRanges();
		//deal with death
		removeEnemyOnEnemyDeath();
		restartOnPlayerDeath();

		//draw the bounding boxes
		::g_pTheDebugrender->RenderDebugObjects(v, p, deltaTime);

		//render screen text
		renderPlayerInfo(width, height);

		//check to see if the enemies have died and move to next level
		loadNextLevel();
		checkPlayerHealth();
		//switch back
		g_lastTimeStep = curTime;
		glfwSwapBuffers(g_pGLFWWindow);
		glfwPollEvents();
	}// while ( ! glfwWindowShouldClose(window) )

	glfwDestroyWindow(g_pGLFWWindow);
	glfwTerminate();
	delete ::g_pTheDebugrender;
	delete ::g_pTextureManager;
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;
	delete ::g_pLightManager;
	delete ::g_pThePlayer;
	delete ::g_curGameObject;
	delete ::g_thePathingManager;
	delete ::g_pParticleManager;
	delete ::g_pSceneManager;
	delete ::g_pCurrentScene;
	return 0;
}

void printError(const std::string& error) {
	std::cout << error << std::endl;
}

void checkForPowerUpCollisions() {
	std::vector<bool> powerupsToRemove;

	for (int i = 0; i < g_vecPowerUps.size(); i++) {
		if (glm::distance(g_vecPowerUps[i]->thePowerUp->position, g_pThePlayer->thePlayerObject->position) < g_pThePlayer->thePlayerObject->scale/2.f) {
			//get the power up type 
			ePickupType type = g_vecPowerUps[i]->getPowerUpType();
			if (type == ePickupType::PICKUP_HEALTH) {
				g_pThePlayer->currentHealth += g_vecPowerUps[i]->modifierValue;
				g_pSoundManager->playHealthSound();
			}
			else if (type == ePickupType::PICKUP_RANGE_INCREASE) {
				g_pThePlayer->projectileRange += g_vecPowerUps[i]->modifierValue;
				g_pThePlayer->setProjectileRange();
				g_pSoundManager->playRangeSound();
			}
			//set the power up to be removed
			powerupsToRemove.push_back(true);
		}
		else {
			powerupsToRemove.push_back(false);
		}
	}

	//remove the pickup from the draw vector and pickup vector
	for (int i = 0; i < powerupsToRemove.size(); i++) {
		if (powerupsToRemove[i] == true) {
			//remove from both vectors
			g_vecPowerUps.erase(g_vecPowerUps.begin() + i);
			g_pCurrentScene->powerUps.erase(g_pCurrentScene->powerUps.begin() + i);
		}
	}
}

void checkForProjectileCollisions( float deltaTime) {

	std::vector<bool> theObjectsForRemoval;
	bool collided = false;

	//check the players projectiles
	for (int i = 0; i < g_pThePlayer->projectiles.size(); i++) {
		for (int index = 0; index < g_pCurrentScene->enemies.size(); index++) {

			if (glm::distance(g_pThePlayer->projectiles[i].object->position, g_pCurrentScene->enemies[index]->position) < g_pCurrentScene->enemies[index]->scale / 3.f)
			{
				//objects are colliding do damage
				g_vecEnemies[index].health -= g_pThePlayer->projectiles[i].damage;

				//play the enemy impact sound
				g_pSoundManager->playEnemyDamagedSound();

				//check to see if the enemy is low health(will only add the emitter if it isn't already present)
				if (g_vecEnemies[index].health < g_vecEnemies[index].maxHealth / 2.f)
				{
					//try to add the emitter to the active emitters
					g_pParticleManager->setEnemyEmitterToActive(g_vecEnemies[index].particleManagerEmitterIndex);
				}
				//remove the projectile from the scene
				collided = true;
				//break out of this loop and add the bool to the vector
				break;
			}
		}
		theObjectsForRemoval.push_back(collided);
		//reset collided
		collided = false;
	}

	//do the player projectile object removal
	for (int i = 0; i < theObjectsForRemoval.size(); i++) {
		//check to see if the object collided
		if (theObjectsForRemoval[i] == true)
		{
			g_pThePlayer->removeProjectile(g_pThePlayer->projectilesToDraw[i]);
		}
	}

	/////////////////////////////////
	//check the enemies projectiles//
	////////////////////////////////
	//go through each enemy
	std::vector<std::vector<bool>> theEnemyProjectilesToRemove;

	for (int i = 0; i < g_vecEnemies.size(); i++) {
		std::vector<bool> removals;
		//go through each projectile for each enemy
		for (int projectilIndex = 0; projectilIndex < g_vecEnemies[i].projectilesToDraw.size(); projectilIndex++) {

			//if the projectile is within the range of the player get rid of it and do damage
			if (glm::distance(g_vecEnemies[i].projectilesToDraw[projectilIndex]->position, g_pThePlayer->thePlayerObject->position) < g_pThePlayer->thePlayerObject->scale /2.f) {
				//do damage to the player
				g_pThePlayer->currentHealth -= g_vecEnemies[i].projectiles[projectilIndex].damage;

				//play the player damaged sound
				g_pSoundManager->playPlayerDamagedSound();

				//set emitter to active
				if (g_pThePlayer->currentHealth < g_pThePlayer->maxHealth / 2.f) {
					g_pParticleManager->setPlayerEmitterToActive();
				}
				//flag this index to be removed
				removals.push_back(true);
			}
			else {
				//flag this index to not be removed
				removals.push_back(false);
			}

			if (projectilIndex == g_vecEnemies[i].projectilesToDraw.size() - 1) {
				//add the vector to the vector for later
				theEnemyProjectilesToRemove.push_back(removals);
			}
		}

		if (g_vecEnemies[i].projectiles.size() == 0) {
			//add a dummy entry to keep the correct structure
			theEnemyProjectilesToRemove.push_back(std::vector<bool>());
		}
	}

	//remove the proper projectiles from the correct enemy vectors
	//go through the first vector
	for (int i = 0; i < theEnemyProjectilesToRemove.size(); i++) {
		//go through the inner vectors
		for (int index = 0; index < theEnemyProjectilesToRemove[i].size();index++) {
			//if this index is true
			if (theEnemyProjectilesToRemove[i][index] == true) {
				//remove the projectile
				g_vecEnemies[i].removeProjectile(g_vecEnemies[i].projectilesToDraw[index]);
			}
		}
	}

	//check for the explosion hitting the player
	for (int i = 0; i < g_vecExplodedEnemies.size(); i++)
	{
		if (glm::distance(g_vecExplodedEnemies[i].explosion[0]->position, g_pThePlayer->thePlayerObject->position) < g_vecExplodedEnemies[i].explosion[0]->scale)
		{
			g_pThePlayer->isInExplosionRadius = true;
			if (g_pThePlayer->timeInExplosion >= 1.0f)
			{
				//the player is inside the explosion for long enough to take damage
				if (g_pThePlayer->currentHealth < 20.f) {
					g_pThePlayer->currentHealth = 0.0f;
				}
				else {
					g_pThePlayer->currentHealth -= 20.f;
				}
				//reset the damage timer
				g_pThePlayer->timeInExplosion = 0.0f;

				if (g_pThePlayer->currentHealth < g_pThePlayer->maxHealth / 2.f) {
					g_pParticleManager->setPlayerEmitterToActive();
				}
			}
			else {
				g_pThePlayer->timeInExplosion += deltaTime;
			}
		}
	}

}

void renderPlayerInfo(int winWidth, int winHeight) {
	float sx = 2.0f / winWidth;
	float sy = 2.0f / winHeight;
	GLfloat yoffset = 50.0f;
	GLfloat xoffset = 8 * sx;

	std::wstring healthTitle = L"Health: ";
	std::wstring characterHealth = std::to_wstring(g_pThePlayer->currentHealth);
	healthTitle += characterHealth;
	g_ptheTextRenderer->render_text(healthTitle.c_str(), -1 + xoffset, 1 - yoffset * sy, sx, sy);
	yoffset += 30.0f;
}

void checkPlayerHealth() {
	//if the player picks up health the particles should disappear if above 1/2
	if (g_pThePlayer->currentHealth >= g_pThePlayer->maxHealth / 2.f) {
		g_pParticleManager->setPlayerEmitterToInactive();
	}
}
//converts a string to a wide string
//Return:	wstring
std::wstring s2ws(std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void checkObjectBounds() {
	//check the players
	for (int i = 0; i < g_pCurrentScene->players.size(); i++) {
		if (g_pCurrentScene->players[i]->position.x > maxDimensions.x) {
			g_pCurrentScene->players[i]->position.x = maxDimensions.x;
		}
		else if (g_pCurrentScene->players[i]->position.x < minDimensions.x) {
			g_pCurrentScene->players[i]->position.x = minDimensions.x;
		}

		if (g_pCurrentScene->players[i]->position.y > maxDimensions.y) {
			g_pCurrentScene->players[i]->position.y = maxDimensions.y;
		}
		else if (g_pCurrentScene->players[i]->position.y < minDimensions.y) {
			g_pCurrentScene->players[i]->position.y = minDimensions.y;
		}

		if (g_pCurrentScene->players[i]->position.z > maxDimensions.z) {
			g_pCurrentScene->players[i]->position.z = maxDimensions.z;
		}
		else if (g_pCurrentScene->players[i]->position.z < minDimensions.z) {
			g_pCurrentScene->players[i]->position.z = minDimensions.z;
		}
	}

	//check the enemies
	for (int i = 0; i < g_pCurrentScene->enemies.size(); i++) {
		if (g_pCurrentScene->enemies[i]->position.x > maxDimensions.x) {
			g_pCurrentScene->enemies[i]->position.x = maxDimensions.x;
		}
		else if (g_pCurrentScene->enemies[i]->position.x < minDimensions.x) {
			g_pCurrentScene->enemies[i]->position.x = minDimensions.x;
		}

		if (g_pCurrentScene->enemies[i]->position.y > maxDimensions.y) {
			g_pCurrentScene->enemies[i]->position.y = maxDimensions.y;
		}
		else if (g_pCurrentScene->enemies[i]->position.y < minDimensions.y) {
			g_pCurrentScene->enemies[i]->position.y = minDimensions.y;
		}

		if (g_pCurrentScene->enemies[i]->position.z > maxDimensions.z) {
			g_pCurrentScene->enemies[i]->position.z = maxDimensions.z;
		}
		else if (g_pCurrentScene->enemies[i]->position.z < minDimensions.z) {
			g_pCurrentScene->enemies[i]->position.z = minDimensions.z;
		}
	}

	std::vector<bool> objectsToRemove;

	//check all the player projectiles for out of bounds 
	for (int i = 0; i < g_pThePlayer->projectilesToDraw.size(); i++)
	{
		if (g_pThePlayer->projectilesToDraw[i]->position.x > maxDimensions.x || g_pThePlayer->projectilesToDraw[i]->position.x < minDimensions.x)
		{
			//add it to the vector for removal
			objectsToRemove.push_back(true);
		}
		else if (g_pThePlayer->projectilesToDraw[i]->position.z > maxDimensions.z || g_pThePlayer->projectilesToDraw[i]->position.z < minDimensions.z)
		{
			//add it to the vector for removal
			objectsToRemove.push_back(true);
		}
		else {
			objectsToRemove.push_back(false);
		}
	}

	//remove all the objects that are out of bounds 
	for (int i = 0; i < objectsToRemove.size(); i++) {
		if (objectsToRemove[i] == true) {
			g_pThePlayer->removeProjectile(g_pThePlayer->projectilesToDraw[i]);
		}
	}

	//////////////////////////////////
	//CHECK FOR ENEMY PROJECTILES ////
	/////////////////////////////////
	std::vector<std::vector<bool>> theEnemyProjectilesToRemove;

	for (int i = 0; i < g_vecEnemies.size(); i++) {
		std::vector<bool> removals;
		//go through each projectile for each enemy
		for (int projectilIndex = 0; projectilIndex < g_vecEnemies[i].projectilesToDraw.size(); projectilIndex++) {

			//if the projectile is within the range of the player or a wall get rid of it(do damage if its the player)
			if (g_vecEnemies[i].projectilesToDraw[projectilIndex]->position.x > maxDimensions.x ||
				g_vecEnemies[i].projectilesToDraw[projectilIndex]->position.x < minDimensions.x) {
				//flag this index to be removed
				removals.push_back(true);
			}		
			else if (g_vecEnemies[i].projectilesToDraw[projectilIndex]->position.z > maxDimensions.z ||
				g_vecEnemies[i].projectilesToDraw[projectilIndex]->position.z < minDimensions.z) {
				//flag this index to be removed
				removals.push_back(true);
			}
			else {
				//flag this index to not be removed
				removals.push_back(false);
			}

			if (projectilIndex == g_vecEnemies[i].projectilesToDraw.size() - 1) {
				//add the vector to the vector for later
				theEnemyProjectilesToRemove.push_back(removals);
			}
		}

		//make sure the objects are in the right order
		if (g_vecEnemies[i].projectilesToDraw.size() <= 0)
		{
			theEnemyProjectilesToRemove.push_back(std::vector<bool>());
		}
	}

	//remove the proper projectiles from the correct enemy vectors
	//go through the first vector
	for (int i = 0; i < theEnemyProjectilesToRemove.size(); i++) {
		//go through the inner vectors
		for (int index = 0; index < theEnemyProjectilesToRemove[i].size(); index++) {
			//if this index is true
			if (theEnemyProjectilesToRemove[i][index] == true) {
				//remove the projectile
				g_vecEnemies[i].removeProjectile(g_vecEnemies[i].projectilesToDraw[index]);
			}
		}
	}


}

void checkPlayerProjectileRanges() {
	std::vector<bool> projectilesToRemove;

	for (int i = 0; i < g_pThePlayer->projectiles.size(); i++) {
		//make sure the projectiles don't go over their range
		if (glm::distance(g_pThePlayer->projectiles[i].object->position, g_pThePlayer->projectiles[i].projectileInitialPosition) >= g_pThePlayer->projectiles[i].projectileRange) {
			projectilesToRemove.push_back(true);
		}
		else {
			projectilesToRemove.push_back(false);
		}
	}

	//remove the projectiles
	for (int i = 0; i < projectilesToRemove.size(); i++) {
		if (projectilesToRemove[i] == true) {
			g_pThePlayer->projectiles[i].inUse = false;
			g_pThePlayer->projectiles.erase(g_pThePlayer->projectiles.begin() + i);
			g_pThePlayer->projectilesToDraw.erase(g_pThePlayer->projectilesToDraw.begin() + i);
		}
	}
}

void createProjectiles() {

	cGameObject* tempObject;

	for (int i = 0; i < 100; i++) {
		tempObject = new cGameObject();
		tempObject->meshName = "Sphere_xyz_n_uv.ply";
		tempObject->position = glm::vec3(0.f);
		tempObject->bIsWireFrame = true;
		tempObject->bIsSkyBoxObject = false;
		tempObject->diffuseColour = glm::vec4(1.f);
		tempObject->scale = 0.25f;
		tempObject->radius = 0.125f;

		sProjectile tempProjectile;
		tempProjectile.damage = 10.f;
		tempProjectile.object = tempObject;
		tempProjectile.speed = g_pThePlayer->playerSpeed + 2.f;
		tempProjectile.inUse = false;
		tempProjectile.projectileRange = g_pThePlayer->projectileRange;
		g_pThePlayer->projectilePool.push_back(tempProjectile);
	}
}

void updateProjectilePositions(double deltaTime) {

	//look through the current player projectiles and update their positions
	for (int i = 0; i < g_pThePlayer->projectiles.size(); i++) {
		//move the projectile
		float speed = g_pThePlayer->projectiles[i].speed;
		glm::vec3 direction = g_pThePlayer->projectiles[i].direction;

		g_pThePlayer->projectiles[i].object->position += (direction * speed) * (float)deltaTime;
	}

	//look through the enemy projectiles
	for (int i = 0; i < g_vecEnemies.size(); i++) {
		for (int index = 0; index < g_vecEnemies[i].projectiles.size(); index++) {
			//update the projectile position
			float speed = g_vecEnemies[i].projectiles[index].speed;
			glm::vec3 direction = g_vecEnemies[i].projectiles[index].direction;
			g_vecEnemies[i].projectiles[index].object->position += (direction * speed) * (float)deltaTime;
		}
	}
}

void removeEnemyOnEnemyDeath() {
	std::vector<bool> isDead;

	//check to see if any enemy is dead
	for (int i = 0; i < g_vecEnemies.size(); i++) {
		if (g_vecEnemies[i].health <= 0.f)
		{
			isDead.push_back(true);
		}
		else {
			isDead.push_back(false);
		}
	}

	//remove the dead enemies if any
	for (int i = 0; i < isDead.size(); i++) {
		if (isDead[i] == true) {
			//remove the enemy particles
			g_pParticleManager->deactivateEmitter(g_vecEnemies[i].particleManagerEmitterIndex);
			//remove the enemy from the enemy vector
			g_vecEnemies.erase(g_vecEnemies.begin() + i);
			//remove the enemy from the scene
			g_pCurrentScene->enemies.erase(g_pCurrentScene->enemies.begin() + i);
		}
	}
}

void restartOnPlayerDeath() {

	if (g_pThePlayer->currentHealth <= 0.f)
	{
		//restart at the first level
		sScene tempScene = g_pSceneManager->getSceneById(0);
		g_pSceneManager->copySceneFromCopyToPointer(tempScene, g_pCurrentScene);

		//populate the player
		g_pThePlayer->thePlayerObject = g_pCurrentScene->players[0];
		g_pThePlayer->currentHealth = 100;
		g_pThePlayer->playerSpeed = 4.0f;
		g_pThePlayer->rotationSpeed = 2.0f;
		//populate the enemies
		g_pSceneManager->setCurrentLevel(0);
		g_pSceneManager->populateEnemies(g_vecEnemies, g_pCurrentScene);
		g_pSceneManager->loadLevelTextures(g_pCurrentScene);
		g_pSceneManager->configurePowerUpsForScene(g_pCurrentScene,g_vecPowerUps);
		//reset the particles 
		g_pThePlayer->resetProjectiles();
		g_pSoundManager->changeBackGroundMusic("assets/Songs/Tapestry.wav");
		//clear the emitters
		g_pParticleManager->clearAllEmitters();
		//add the emitters again
		g_pParticleManager->createEmitters(g_vecEnemies.size());
		g_pParticleManager->connectEmittersWithEntities(g_vecEnemies);
		
		//clear explosion vector
		g_vecExplodedEnemies.clear();
		//reload the projectiles into the player
		createProjectiles();
	}
}

void loadNextLevel() {
	if (g_pCurrentScene->enemies.size() == 0 && g_vecExplodedEnemies.size() == 0)
	{
		//reset the particles 
		g_pThePlayer->resetProjectiles();

		//go to next level (make sure everything is loaded for each. player,enemy,powerup)
		g_pSceneManager->loadNextLevel(g_pCurrentScene,g_pThePlayer);

		//play the right level song
		int levelNum = g_pSceneManager->getCurrentLevel();
		if (levelNum == 1) {
			g_pSoundManager->changeBackGroundMusic("assets/Songs/Failure.wav");
		}
		else if (levelNum == 2) {
			g_pSoundManager->changeBackGroundMusic("assets/Songs/ThisIstheTime.wav");
		}
		if (levelNum == 3) {
			g_pSoundManager->changeBackGroundMusic("assets/Songs/I'llBeOk.wav");
		}

		g_pSceneManager->loadLevelTextures(g_pCurrentScene);
		g_pSceneManager->populateEnemies(g_vecEnemies,g_pCurrentScene);
		g_pSceneManager->configurePowerUpsForScene(g_pCurrentScene, g_vecPowerUps);

		//clear the particle system and rebuild for next level
		g_pParticleManager->clearAllEmitters();
		g_pParticleManager->createEmitters(g_vecEnemies.size());
		g_pParticleManager->connectEmittersWithEntities(g_vecEnemies);
	}
}

void handleExplosions(float deltaTime) {
	std::vector<bool> explosionsToRemove;	

	//remove the exploding enemies
	for (int i = 0; i < g_vecEnemies.size(); i++)
	{
		if (g_vecEnemies[i].isExploding)
		{
			explosionsToRemove.push_back(true);
		}
		else {
			explosionsToRemove.push_back(false);
		}
	}

	//remove the enemy from the enemy drawing vector add it to the explosion vector
	for (int i = 0; i < explosionsToRemove.size(); i++)
	{
		if (explosionsToRemove[i] == true)
		{
			//keep track of the enemy that exploded
			g_vecExplodedEnemies.push_back(g_vecEnemies[i]);
			//remove the emitter from the active ones
			g_pParticleManager->deactivateEmitter(g_vecEnemies[i].particleManagerEmitterIndex);
			g_vecEnemies.erase(g_vecEnemies.begin()+i);
			g_pCurrentScene->enemies.erase(g_pCurrentScene->enemies.begin() + i);

			//play the explosion sound
			g_pSoundManager->playExplosionSound();
		}
	}

	//deal with the explosion growth
	for (int i = 0; i < g_vecExplodedEnemies.size(); i++) {
		if (g_vecExplodedEnemies[i].explosion[0]->scale < g_vecExplodedEnemies[i].explosionSize)
		{
			g_vecExplodedEnemies[i].explosion[0]->scale += deltaTime;
			if (g_vecExplodedEnemies[i].explosion[0]->vecMehs2DTextures.size() == 0)
			{
				//add the blue fire texture and set the object to be slightly transparent
				g_vecExplodedEnemies[i].explosion[0]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("bluefire.bmp", 0.75f));
			}
			else if (g_vecExplodedEnemies[i].explosion[0]->scale >= 5.f) {
				//add the explosion texture
				if (g_vecExplodedEnemies[i].explosion[0]->vecMehs2DTextures.size() == 1) {
					g_vecExplodedEnemies[i].explosion[0]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("explosion.bmp", 0.25f));
				}
			}
		}
		else {
			//remove the enemy from the exploding vector
			g_vecExplodedEnemies.erase(g_vecExplodedEnemies.begin()+i);
		}
	}
}
