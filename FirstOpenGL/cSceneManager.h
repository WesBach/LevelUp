#ifndef _cSceneManager_HG_
#define _cSceneManager_HG_

#include <map>
#include <vector>
#include <string>

//#include "ModelUtilities.h"

class cGameObject;
class cVAOMeshManager;
class cMesh;
class cEnemy;
class cPlayer;
class cPowerUp;

struct sScene {
	std::vector<cGameObject*> players;
	std::vector<cGameObject*> enemies;
	std::vector<cGameObject*> terrain;
	std::vector<cGameObject*> powerUps;
};

class cSceneManager {
public :
	cSceneManager();
	bool LoadSceneFromFileIntoSceneMap(std::string& fileName,int mapIndex);
	bool LoadModelsFromModelInfoFile(std::string& filename, cVAOMeshManager* theMeshManager,int shaderId);

	sScene getSceneById(int id);//copy the vector, we dont want to change the original
	void copySceneFromCopyToPointer(const sScene& copyFrom, sScene* copyTo);
	void populateEnemies(std::vector<cEnemy>& enemies,sScene* theScene);
	void loadLevelTextures(sScene* theScene);
	void loadNextLevel(sScene* g_pCurrentScene, cPlayer* thePlayer);
	void configurePowerUpsForScene(sScene* theScene, std::vector<cPowerUp*>& thePowerUps);

	void setCurrentLevel(int levelIndex);
private:
	int currentLevel;
	int numLevels;

	void loadObjectData(std::ifstream& theFile, std::vector<cGameObject*>& theVector);
	bool LoadPlyFileIntoMeshWith_Normals_and_UV(std::string filename, cMesh &theMesh);
	bool LoadPlyFileIntoMeshWithNormals(std::string filename, cMesh &theMesh);
	std::map<int, sScene*> mSceneMap;
};

#endif // !_cSceneManager_HG_

