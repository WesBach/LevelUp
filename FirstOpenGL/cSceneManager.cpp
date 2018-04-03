#include "cSceneManager.h"
#include "cVAOMeshManager.h"
#include "cGameObject.h"
#include "cMesh.h"

#include "cAngryState.h"
#include "cCuriousState.h"
#include "cFollowState.h"
#include "cPlayer.h"
#include "cEnemy.h"
#include "cPowerUp.h"
#include "cEmitter.h"
#include "Utilities.h"
#include <iostream>

extern glm::vec3 g_cameraXYZ;

bool  cSceneManager::LoadSceneFromFileIntoSceneMap(std::string& fileName, int mapIndex) {
	sScene* tempScene = new sScene();

	std::ifstream sceneInfo(fileName.c_str());
	int numPlayers = 0;
	int numEnemies = 0;
	int numTerrain = 0;
	if (!sceneInfo.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(sceneInfo, "CAMERA_POSITION");
	sceneInfo >> g_cameraXYZ.x;
	sceneInfo >> g_cameraXYZ.y;
	sceneInfo >> g_cameraXYZ.z;

	//popualate the players
	ReadFileToToken(sceneInfo, "NUMBER_OF_PLAYERS");
	sceneInfo >> numPlayers;
	ReadFileToToken(sceneInfo, "PLAYERS");
	for (int i = 0; i < numPlayers; i++)
	{
		loadObjectData(sceneInfo, tempScene->players);
	}
	ReadFileToToken(sceneInfo, "PLAYER_SPEED");
	float playerSpeed;
	sceneInfo >> playerSpeed;

	//populate the terrain
	ReadFileToToken(sceneInfo, "NUMBER_OF_TERRAIN");
	sceneInfo >> numTerrain;
	ReadFileToToken(sceneInfo, "TERRAIN");
	for (int i = 0; i < numTerrain; i++)
	{
		loadObjectData(sceneInfo, tempScene->terrain);
	}

	//populate the enemies
	ReadFileToToken(sceneInfo, "NUMBER_OF_ENEMIES");
	sceneInfo >> numEnemies;
	ReadFileToToken(sceneInfo, "ENEMIES");

	for (int i = 0; i < numEnemies; i++)
	{
		loadObjectData(sceneInfo, tempScene->enemies);
	}

	//populate the enemies
	ReadFileToToken(sceneInfo, "ENEMY_SPEED");
	float enemySpeed;
	sceneInfo >> enemySpeed;

	for (int i = 0; i < tempScene->enemies.size(); i++) {
		StateType type = tempScene->enemies[i]->theState->getStateType();
		if (type == StateType::ANGRY) {
			cAngryState* theState = dynamic_cast<cAngryState*>(tempScene->enemies[i]->theState);
			theState->speed = enemySpeed;
		}
		else if (type == StateType::FOLLOWER) {
			cFollowState* theState = dynamic_cast<cFollowState*>(tempScene->enemies[i]->theState);
			theState->speed = enemySpeed;
		}
		else if (type == StateType::CURIOUS) {
			cCuriousState* theState = dynamic_cast<cCuriousState*>(tempScene->enemies[i]->theState);
			theState->speed = enemySpeed;
		}
	}

	//populate the enemies
	int numPowerups;
	ReadFileToToken(sceneInfo, "NUMBER_OF_POWERUPS");
	sceneInfo >> numPowerups;
	ReadFileToToken(sceneInfo, "POWERUPS");

	for (int i = 0; i < numPowerups; i++)
	{
		loadObjectData(sceneInfo, tempScene->powerUps);
	}

	//close the file stream
	sceneInfo.close();
	//add the scene to the map
	this->mSceneMap[mapIndex] = tempScene;

	this->numLevels = this->mSceneMap.size()-1;
	return true;
}

bool cSceneManager::LoadModelsFromModelInfoFile(std::string& filename, cVAOMeshManager* theMeshManager,int shaderId) {
	std::ifstream modelNameFile(filename.c_str());
	std::string tempString;
	int numModels = 0;
	int numModelsUV;
	bool succeeded = true;

	if (!modelNameFile.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(modelNameFile, "NUM_MODELS_UV");
	modelNameFile >> numModelsUV;
	ReadFileToToken(modelNameFile, "MODEL_NAME_UV_START");

	for (int i = 0; i < numModelsUV; i++)
	{
		modelNameFile >> tempString;
		cMesh* testMesh;
		testMesh = new cMesh();
		testMesh->name = tempString;


		if (testMesh->name == "SmoothSphere_Inverted_Normals_xyz_n.ply")
		{
			if (!LoadPlyFileIntoMeshWithNormals(tempString, *testMesh))
			{
				//std::cout << "Didn't load model" << std::endl;
				std::cout << "Didn't load model >" << testMesh->name << "<" << std::endl;
				succeeded = false;
			}
		}
		else {
			if (!LoadPlyFileIntoMeshWith_Normals_and_UV(tempString, *testMesh))
			{
				std::cout << "Didn't load model >" << testMesh->name << "<" << std::endl;
				succeeded = false;
			}
		}

		if (!theMeshManager->loadMeshIntoVAO(*testMesh, shaderId, true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			std::cout << "Could not load mesh >" << testMesh->name << "< into VAO" << std::endl;
			succeeded = false;

		}

		tempString = "";
	}


	return succeeded;
}

sScene cSceneManager::getSceneById(int id) {
	std::vector<cGameObject> objects;
	sScene theScene;

	//make sure the map has a size greater 0
	if (this->mSceneMap.size() > 0)
	{
		theScene = *this->mSceneMap[id];
	}

	return theScene;
}

void cSceneManager::copySceneFromCopyToPointer(const sScene& copyFrom, sScene* copyTo) {
	
	sScene temp = copyFrom;
	//clear the previous scene
	copyTo->enemies.clear();
	copyTo->players.clear();
	copyTo->terrain.clear();
	//make new objects with the game object copies
	for (int i = 0; i < temp.enemies.size(); i++)
	{
		copyTo->enemies.push_back(new cGameObject(*temp.enemies[i]));
	}

	for (int i = 0; i < temp.terrain.size(); i++)
	{
		copyTo->terrain.push_back(new cGameObject(*temp.terrain[i]));
	}

	for (int i = 0; i < temp.players.size(); i++)
	{
		copyTo->players.push_back(new cGameObject(*temp.players[i]));
	}

	for (int i = 0; i < temp.powerUps.size(); i++)
	{
		copyTo->powerUps.push_back(new cGameObject(*temp.powerUps[i]));
	}

}

void cSceneManager::loadObjectData(std::ifstream& theFile, std::vector<cGameObject*>& theVector) {

	std::string physicsObjectType = "";
	std::string stateType = "";
	cGameObject* pTempGO = new cGameObject();
	//cPathingNode* pTempNode = new cPathingNode();
	float tempFloat = 0.0f;
	float mass = 0.0f;
	theFile >> pTempGO->meshName;
	theFile >> pTempGO->position.x;
	theFile >> pTempGO->position.y;
	theFile >> pTempGO->position.z;
	theFile >> pTempGO->scale;
	theFile >> pTempGO->vel.x;
	theFile >> pTempGO->vel.y;
	theFile >> pTempGO->vel.z;
	theFile >> pTempGO->radius;
	theFile >> mass;
	theFile >> pTempGO->diffuseColour.x;
	theFile >> pTempGO->diffuseColour.y;
	theFile >> pTempGO->diffuseColour.z;
	theFile >> pTempGO->diffuseColour.a;
	theFile >> pTempGO->bIsWireFrame;
	theFile >> tempFloat;
	pTempGO->orientation2.x = glm::radians(tempFloat);
	theFile >> tempFloat;
	pTempGO->orientation2.y = glm::radians(tempFloat);
	theFile >> tempFloat;
	pTempGO->orientation2.z = glm::radians(tempFloat);
	theFile >> physicsObjectType;
	theFile >> pTempGO->bHasAABB;
	theFile >> pTempGO->hasTexture;
	theFile >> stateType;


	if (physicsObjectType == "UNKNOWN")
	{
		pTempGO->typeOfObject = eTypeOfObject::UNKNOWN;
	}
	else if (physicsObjectType == "SPHERE")
	{
		pTempGO->typeOfObject = eTypeOfObject::SPHERE;
	}
	else
	{
		pTempGO->typeOfObject = eTypeOfObject::PLANE;
	}

	if (stateType == "ANGRY") {
		pTempGO->theState = new cAngryState();
	}
	else if (stateType == "FOLLOWER") {
		pTempGO->theState = new cFollowState();
	}
	else if (stateType == "CURIOUS") {
		pTempGO->theState = new cCuriousState();
	}

	theVector.push_back(pTempGO);
}



bool cSceneManager::LoadPlyFileIntoMeshWith_Normals_and_UV(std::string filename, cMesh &theMesh)
{
	// Load the vertices
	std::string file = "assets\\models\\" + filename;
	// c_str() changes a string to a "c style char* string"
	std::ifstream plyFile(file.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return
		return false;
	}
	// File is open, let's read it

	ReadFileToToken(plyFile, "vertex");
	//	int numVertices = 0;
	plyFile >> theMesh.numberOfVertices;

	ReadFileToToken(plyFile, "face");
	//	int numTriangles = 0;
	plyFile >> theMesh.numberOfTriangles;

	ReadFileToToken(plyFile, "end_header");

	// Allocate the appropriate sized array (+a little bit)
	//theMesh.pVertices = new cVertex_xyz_rgb_n[theMesh.numberOfVertices];
	theMesh.pVertices = new sVertex_xyz_rgba_n_uv2_bt_4Bones[theMesh.numberOfVertices];
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Read vertices
	for (int index = 0; index < theMesh.numberOfVertices; index++)
	{
		//end_header
		//-0.0312216 0.126304 0.00514924 0.850855 0.5 		
		float x, y, z, nx, ny, nz;
		// Added
		float u, v;		// Model now has texture coordinate

						// Typical vertex is now... 
						// 29.3068 -5e-006 24.3079 -0.949597 0.1875 -0.251216 0.684492 0.5

		plyFile >> x >> y >> z;
		plyFile >> nx >> ny >> nz;
		// 
		plyFile >> u >> v;			// ADDED


		theMesh.pVertices[index].x = x;	// vertices[index].x = x;
		theMesh.pVertices[index].y = y;	// vertices[index].y = y;
		theMesh.pVertices[index].z = z;
		theMesh.pVertices[index].r = 1.0f;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].g = 1.0f;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].b = 1.0f;	// vertices[index].r = 1.0f;
		theMesh.pVertices[index].nx = nx;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].ny = ny;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].nz = nz;	// vertices[index].r = 1.0f;

											// Only store the 1st UV.
		theMesh.pVertices[index].u1 = u;
		theMesh.pVertices[index].v1 = v;
	}

	// Load the triangle (or face) information, too
	for (int count = 0; count < theMesh.numberOfTriangles; count++)
	{
		// 3 164 94 98 
		int discard = 0;
		plyFile >> discard;									// 3
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;	// 164
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;	// 94
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;	// 98
	}

	//	theMesh.CalculateNormals();

	return true;
}

bool cSceneManager::LoadPlyFileIntoMeshWithNormals(std::string filename, cMesh &theMesh)
{
	// Load the vertices
	// c_str() changes a string to a "c style char* string"
	std::string file = "assets\\models\\" + filename;
	std::ifstream plyFile(file.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return
		return false;
	}
	// File is open, let's read it

	ReadFileToToken(plyFile, "vertex");
	//	int numVertices = 0;
	plyFile >> theMesh.numberOfVertices;

	ReadFileToToken(plyFile, "face");
	//	int numTriangles = 0;
	plyFile >> theMesh.numberOfTriangles;

	ReadFileToToken(plyFile, "end_header");

	// Allocate the appropriate sized array (+a little bit)
	//theMesh.pVertices = new cVertex_xyz_rgb_n[theMesh.numberOfVertices];
	theMesh.pVertices = new sVertex_xyz_rgba_n_uv2_bt_4Bones[theMesh.numberOfVertices];
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Read vertices
	for (int index = 0; index < theMesh.numberOfVertices; index++)
	{
		//end_header
		//-0.0312216 0.126304 0.00514924 0.850855 0.5 		
		float x, y, z, nx, ny, nz;//, confidence, intensity;

		plyFile >> x;
		plyFile >> y;
		plyFile >> z;
		plyFile >> nx >> ny >> nz;
		//		plyFile >> confidence;
		//		plyFile >> intensity;

		theMesh.pVertices[index].x = x;	// vertices[index].x = x;
		theMesh.pVertices[index].y = y;	// vertices[index].y = y;
		theMesh.pVertices[index].z = z;
		theMesh.pVertices[index].r = 1.0f;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].g = 1.0f;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].b = 1.0f;	// vertices[index].r = 1.0f;
		theMesh.pVertices[index].nx = nx;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].ny = ny;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].nz = nz;	// vertices[index].r = 1.0f;
	}

	// Load the triangle (or face) information, too
	for (int count = 0; count < theMesh.numberOfTriangles; count++)
	{
		// 3 164 94 98 
		int discard = 0;
		plyFile >> discard;									// 3
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;	// 164
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;	// 94
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;	// 98
	}

	//	theMesh.CalculateNormals();

	return true;
}

void cSceneManager::populateEnemies(std::vector<cEnemy>& enemies, sScene* theScene) {
	//clear the vector
	enemies.clear();

	//populate the enemies
	for (int i = 0; i < theScene->enemies.size(); i++) {
		StateType stateType = theScene->enemies[i]->theState->getStateType();
		eAttackType attackType;
		cEnemy enemy;
		//set the emitter position
		//enemy.theParticleEmitter->position = theScene->enemies[i]->position;

		//check the state of the object
		if (stateType == StateType::FOLLOWER) {
			attackType = eAttackType::EXPLOSION;
			enemy.enemyType = eEnemyType::SUICIDE;
			enemy.health = 100.f;
			enemy.maxHealth = 100.f;
		}
		else {
			enemy.enemyType = eEnemyType::GUNNER;
			attackType = eAttackType::PROJECTILE;
			enemy.health = 150.f;
			enemy.maxHealth = 150.f;
		}

		//set the attack type
		enemy.attackType = attackType;
		enemy.theEnemyObject = theScene->enemies[i];
		//add the enemy
		enemies.push_back(enemy);
	}
}

cSceneManager::cSceneManager() {
	this->currentLevel = 0;
	this->numLevels = 0;
}

void cSceneManager::loadLevelTextures(sScene* theScene) {
	
	//based on current level load textures
	switch (this->currentLevel) {
		//level 1
		case 0:
			for (int i = 0; i < theScene->enemies.size(); i++)
			{

				if (theScene->enemies[i]->meshName == "mig29_xyz.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("leaves.bmp", 1.0f));
				}
				else if (theScene->enemies[i]->meshName == "Raider_ASCII_UVtex.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("lava.bmp", 1.0f));
				}
				else if (theScene->enemies[i]->meshName == "Viper_MkVII_ASCII_UVTex.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("rust.bmp", 0.5f));
				}
			}


			for (int i = 0; i < theScene->terrain.size(); i++)
			{
				if (theScene->terrain[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv.ply")
				{
					theScene->terrain[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("realistic-wet-grassl.bmp", 1.f));
				}
				else if (theScene->terrain[i]->meshName == "SmoothSphere_Inverted_Normals_xyz_n.ply")
				{
					theScene->terrain[i]->vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
					theScene->terrain[i]->bIsSkyBoxObject = true;
				}
			}

			for (int i = 0; i < theScene->players.size(); i++)
			{
				if (theScene->players[i]->meshName == "Sample_Ship.ply")
				{
					theScene->players[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("sh3.bmp", 1.0f));
				}
				//Sample_Ship.ply
			}

			for (int i = 0; i < theScene->powerUps.size(); i++)
			{
				if (theScene->powerUps[i]->meshName == "health_pack.ply")
				{
					theScene->powerUps[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("health.bmp", 1.0f));
				}
				if (theScene->powerUps[i]->meshName == "bullet.ply")
				{
					theScene->powerUps[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("gold.bmp", 1.0f));
				}
			}
			break;

		//level 2
		case 1:
			for (int i = 0; i < theScene->enemies.size(); i++)
			{
				if (theScene->enemies[i]->meshName == "mig29_xyz.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("leaves.bmp", 1.0f));
				}
				else if (theScene->enemies[i]->meshName == "Raider_ASCII_UVtex.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("lava.bmp", 1.0f));
				}
				else if (theScene->enemies[i]->meshName == "Viper_MkVII_ASCII_UVTex.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("rust.bmp", 0.5f));
				}
			}


			for (int i = 0; i < theScene->terrain.size(); i++)
			{
				if (theScene->terrain[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv.ply")
				{
					theScene->terrain[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("ground_grass.bmp", 0.5f));
				}
				else if (theScene->terrain[i]->meshName == "SmoothSphere_Inverted_Normals_xyz_n.ply")
				{
					theScene->terrain[i]->vecMeshCubeMaps.push_back(sTextureBindBlendInfo("sunny", 1.0f));
					theScene->terrain[i]->bIsSkyBoxObject = true;
				}
			}

			for (int i = 0; i < theScene->players.size(); i++)
			{
				if (theScene->players[i]->meshName == "Sample_Ship.ply")
				{
					theScene->players[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("sh3.bmp", 1.0f));
				}
			}


			for (int i = 0; i < theScene->powerUps.size(); i++)
			{
				if (theScene->powerUps[i]->meshName == "health_pack.ply")
				{
					theScene->powerUps[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("health.bmp", 1.0f));
				}
				if (theScene->powerUps[i]->meshName == "bullet.ply")
				{
					theScene->powerUps[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("gold.bmp", 1.0f));
				}
			}
			break;

		//level 3
		case 2:
			for (int i = 0; i < theScene->enemies.size(); i++)
			{
				if (theScene->enemies[i]->meshName == "mig29_xyz.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("leaves.bmp", 1.0f));
				}
				else if (theScene->enemies[i]->meshName == "Raider_ASCII_UVtex.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("lava.bmp", 1.0f));
				}
				else if (theScene->enemies[i]->meshName == "Viper_MkVII_ASCII_UVTex.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("rust.bmp", 0.5f));
				}
			}


			for (int i = 0; i < theScene->terrain.size(); i++)
			{
				if (theScene->terrain[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv.ply")
				{
					theScene->terrain[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("lava.bmp", 0.5f));
				}
				else if (theScene->terrain[i]->meshName == "SmoothSphere_Inverted_Normals_xyz_n.ply")
				{
					theScene->terrain[i]->vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
					theScene->terrain[i]->bIsSkyBoxObject = true;
				}
			}

			for (int i = 0; i < theScene->players.size(); i++)
			{
				if (theScene->players[i]->meshName == "Sample_Ship.ply")
				{
					theScene->players[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("sh3.bmp", 1.0f));
				}
			}


			for (int i = 0; i < theScene->powerUps.size(); i++)
			{
				if (theScene->powerUps[i]->meshName == "health_pack.ply")
				{
					theScene->powerUps[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("health.bmp", 1.0f));
				}
				if (theScene->powerUps[i]->meshName == "bullet.ply")
				{
					theScene->powerUps[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("gold.bmp", 1.0f));
				}
			}
			break;


		case 3:
			for (int i = 0; i < theScene->enemies.size(); i++)
			{
				if (theScene->enemies[i]->meshName == "mig29_xyz.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("leaves.bmp", 1.0f));
				}
				else if (theScene->enemies[i]->meshName == "Raider_ASCII_UVtex.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("lava.bmp", 1.0f));
				}
				else if (theScene->enemies[i]->meshName == "Viper_MkVII_ASCII_UVTex.ply")
				{
					theScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("rust.bmp", 0.5f));
				}
			}


			for (int i = 0; i < theScene->terrain.size(); i++)
			{
				if (theScene->terrain[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv.ply")
				{
					theScene->terrain[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("lava.bmp", 0.5f));
				}
				else if (theScene->terrain[i]->meshName == "SmoothSphere_Inverted_Normals_xyz_n.ply")
				{
					theScene->terrain[i]->vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
					theScene->terrain[i]->bIsSkyBoxObject = true;
				}
			}

			for (int i = 0; i < theScene->players.size(); i++)
			{
				if (theScene->players[i]->meshName == "Sample_Ship.ply")
				{
					theScene->players[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("sh3.bmp", 1.0f));
				}
			}


			for (int i = 0; i < theScene->powerUps.size(); i++)
			{
				if (theScene->powerUps[i]->meshName == "health_pack.ply")
				{
					theScene->powerUps[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("health.bmp", 1.0f));
				}
				if (theScene->powerUps[i]->meshName == "bullet.ply")
				{
					theScene->powerUps[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("gold.bmp", 1.0f));
				}
			}
			break;

	}
}

void cSceneManager::loadNextLevel(sScene* g_pCurrentScene,cPlayer* thePlayer) {

	if (this->currentLevel == this->numLevels) {
		this->currentLevel = 0;
	}
	else {
		this->currentLevel++;
	}

	//load the scene info
	sScene tempScene = this->getSceneById(this->currentLevel);
	this->copySceneFromCopyToPointer(tempScene, g_pCurrentScene);

	//populate the player
	thePlayer->thePlayerObject = g_pCurrentScene->players[0];
	thePlayer->currentHealth = 100;
	thePlayer->playerSpeed = 4.0f;
	thePlayer->rotationSpeed = 2.0f;
	thePlayer->projectiles.clear();
	thePlayer->projectilesToDraw.clear();
	//reset the projectiles
	for (int i = 0; i < thePlayer->projectilePool.size(); i++) {
		thePlayer->projectilePool[i].inUse = false;
	}
}


void cSceneManager::configurePowerUpsForScene(sScene* theScene,std::vector<cPowerUp*>& thePowerups) {

	thePowerups.clear();
	//set the modifiers
	for (int i = 0; i < theScene->powerUps.size(); i++) {
		if (theScene->powerUps[i]->meshName == "health_pack.ply") {			
			thePowerups.push_back(new cPowerUp(eModifierType::MODIFIER_HEALTH,ePickupType::PICKUP_HEALTH, theScene->powerUps[i]));
		}
		else if (theScene->powerUps[i]->meshName == "bullet.ply") {
			thePowerups.push_back(new cPowerUp(eModifierType::MODIFIER_RANGE_INCREASE,ePickupType::PICKUP_RANGE_INCREASE, theScene->powerUps[i]));
		}	
	}
}

void cSceneManager::setCurrentLevel(int levelIndex) {
	this->currentLevel = levelIndex;
}
