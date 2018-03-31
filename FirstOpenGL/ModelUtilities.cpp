#include "ModelUtilities.h" 
#include "cVAOMeshManager.h"
#include "cMesh.h"
#include <sstream>
#include "cGameObject.h"
#include "cLightManager.h"
#include "CTextureManager.h"
#include "globalOpenGL_GLFW.h"
#include "cVAOMeshManager.h"
#include "cPathingManager.h"

#include "cAngryState.h"
#include "cCuriousState.h"
#include "cFollowState.h"
#include "cPlayer.h"
#include "cSceneManager.h"

#include <glm\vec3.hpp>
#include <glm\glm.hpp>
#include <iPhysicsFactory.h>



extern glm::vec3 g_cameraXYZ;
extern cPathingManager* g_thePathingManager;

extern nPhysics::iPhysicsWorld* theWorld;
extern nPhysics::iPhysicsFactory* theFactory;
extern cVAOMeshManager*	g_pVAOManager;

//extern std::vector< cGameObject* >  g_vecTerrain;
//extern std::vector< cGameObject* >  g_vecEnemiesToDraw;
//extern std::vector<cGameObject*> g_vecThePlayersToDraw;
extern sScene* g_pCurrentScene;

extern cPlayer* g_pThePlayer;

extern glm::vec3 maxDimensions;
extern glm::vec3 minDimensions;

void ReadFileToToken(std::ifstream &file, std::string token)
{
	bool bKeepReading = true;
	std::string garbage;
	do
	{
		file >> garbage;		// Title_End??
		if (garbage == token)
		{
			return;
		}
	} while (bKeepReading);
	return;
}

bool LoadLightsAttributesFromFile(std::string& fileName, std::vector<cLight>& theLights) {
	//open the file
	std::ifstream modelAndSceneFile(fileName.c_str());
	cMesh* testMesh;
	int numLights = 0;

	if (!modelAndSceneFile.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(modelAndSceneFile, "NUM_LIGHTS_TO_LOAD");
	modelAndSceneFile >> numLights;

	ReadFileToToken(modelAndSceneFile, "LIGHTS_BEGIN");

	std::string tempType;
	for (int i = 0; i < numLights; i++)
	{

		modelAndSceneFile >> theLights[i].position.x;
		modelAndSceneFile >> theLights[i].position.y;
		modelAndSceneFile >> theLights[i].position.z;
		modelAndSceneFile >> theLights[i].attenuation.x;
		modelAndSceneFile >> theLights[i].attenuation.y;
		modelAndSceneFile >> theLights[i].attenuation.z;
		modelAndSceneFile >> theLights[i].diffuse.r;
		modelAndSceneFile >> theLights[i].diffuse.g;
		modelAndSceneFile >> theLights[i].diffuse.b;
		modelAndSceneFile >> tempType;
		if (tempType == "DIRECTIONAL")
			theLights[i].setLightParamType(cLight::DIRECTIONAL);
		else if (tempType == "POINT")
			theLights[i].setLightParamType(cLight::POINT);
		else if (tempType == "SPOT")
		{
			theLights[i].setLightParamType(cLight::SPOT);
			theLights[i].direction = glm::vec3(0.0f, -1.0f, 0.0f);
			theLights[i].setLightParamSpotPrenumAngleInner(glm::radians(15.0f));
			theLights[i].setLightParamSpotPrenumAngleOuter(glm::radians(25.0f));
		}

	}

	//close the file stream
	modelAndSceneFile.close();
	return true;
}

bool LoadTexturesFromFile(std::string& fileName) {
	std::ifstream modelAndSceneFile(fileName.c_str());
	int numTextures = 0;
	if (!modelAndSceneFile.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(modelAndSceneFile, "NUM_TEXTURES");
	modelAndSceneFile >> numTextures;

	ReadFileToToken(modelAndSceneFile, "TEXTURE_NAMES");
	for (int i = 0; i < numTextures; i++)
	{
		std::string temp = "";
		modelAndSceneFile >> temp;
		//load the textures into the texture manager
		g_pTextureManager->Create2DTextureFromBMPFile(temp, true);
		temp = "";
	}
	return true;
}

bool MapTexturesToProperObjects() {

	for (int i = 0; i < g_pCurrentScene->enemies.size(); i++)
	{

		if (g_pCurrentScene->enemies[i]->meshName == "mig29_xyz.ply")
		{
			g_pCurrentScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("leaves.bmp", 1.0f));
		}
		else if (g_pCurrentScene->enemies[i]->meshName == "Raider_ASCII_UVtex.ply")
		{
			g_pCurrentScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("lava.bmp", 1.0f));
		}
		else if (g_pCurrentScene->enemies[i]->meshName == "Viper_MkVII_ASCII_UVTex.ply")
		{
			g_pCurrentScene->enemies[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("rust.bmp", 0.5f));
		}
	}


	for (int i = 0; i <  g_pCurrentScene->terrain.size(); i++)
	{
		if (g_pCurrentScene->terrain[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv.ply")
		{
			g_pCurrentScene->terrain[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("gravel.bmp", 0.5f));
		}
		else if (g_pCurrentScene->terrain[i]->meshName == "SmoothSphere_Inverted_Normals_xyz_n.ply")
		{
			g_pCurrentScene->terrain[i]->vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
			g_pCurrentScene->terrain[i]->bIsSkyBoxObject = true;
		}
		else if (g_pCurrentScene->terrain[i]->meshName == "SmoothSphere_Inverted_Normals_xyz_n_uv.ply")
		{
			g_pCurrentScene->terrain[i]->vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
			g_pCurrentScene->terrain[i]->bIsSkyBoxObject = true;
		}
	}

	for (int i = 0; i < g_pCurrentScene->players.size(); i++)
	{
		if (g_pCurrentScene->players[i]->meshName == "Utah_Teapot_xyz_n_uv.ply")
		{
			g_pCurrentScene->players[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("TropicalSunnyDayBack2048.bmp", 0.5f));
		}
	}
	return true;
}

void loadObjectData(std::ifstream& theFile,std::vector<cGameObject*>& theVector) {

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
