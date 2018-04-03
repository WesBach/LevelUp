#ifndef _ModelUtilities_HG_
#define _ModelUtilities_HG_

#include <fstream>
#include <string>
#include "cMesh.h"
class cVAOMeshManager;

// Note the & symbol which is "by reference" which means
//	it's NOT a copy, but referring to the original object.
// ALSO note that this is a C++ thing (& isn't a thing in C)
bool LoadPlyFileIntoMesh( std::string filename, cMesh &theMesh );
bool LoadPlyFileIntoMeshWithNormals( std::string filename, cMesh &theMesh );
bool LoadPlyFileIntoMeshWith_Normals_and_UV(std::string filename, cMesh &theMesh);
bool LoadTexturesFromFile(std::string& fileName);
bool MapTexturesToProperObjects();

bool LoadSceneFromFileWithNormalAndUV(std::string& fileName);
bool LoadSceneFromFileWithRigidBodies(std::string& fileName);
bool LoadSceneFromFileWithNormal(std::string& fileName);

bool Load3DModelsFromModelFileWithNormalsAndUV(int shaderID, cVAOMeshManager* pVAOManager, std::string &error);
bool Load3DModelsFromModelFileWithNormals(int shaderID, cVAOMeshManager* pVAOManager, std::string &error);

bool Load3DModelsFromModelFile(int shaderID, cVAOMeshManager* pVAOManager, std::string &error);
void loadObjectData(std::ifstream& theFile, std::vector<cGameObject*>& theVector);
#endif
