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

struct sScene {
	int id;
	std::vector<cGameObject*> players;
	std::vector<cGameObject*> enemies;
	std::vector<cGameObject*> terrain;
};

class cSceneManager {
public :
	bool LoadSceneFromFileIntoSceneMap(std::string& fileName,int mapIndex);
	bool LoadModelsFromModelInfoFile(std::string& filename, cVAOMeshManager* theMeshManager,int shaderId);

	sScene getSceneById(int id);//copy the vector, we dont want to change the original
	void copySceneFromCopyToPointer(const sScene& copyFrom, sScene* copyTo);
	void populateEnemies(std::vector<cEnemy>& enemies,sScene* theScene);
private:
	void loadObjectData(std::ifstream& theFile, std::vector<cGameObject*>& theVector);
	void ReadFileToToken(std::ifstream &file, std::string token);
	bool LoadPlyFileIntoMeshWith_Normals_and_UV(std::string filename, cMesh &theMesh);
	bool LoadPlyFileIntoMeshWithNormals(std::string filename, cMesh &theMesh);
	std::map<int, sScene*> mSceneMap;
};

#endif // !_cSceneManager_HG_

