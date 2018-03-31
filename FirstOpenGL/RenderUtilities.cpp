#include "RenderUtilities.h"
#include "cShaderManager.h"
#include "cLightManager.h"
//#include "Camera.h"
#include "cMesh.h"
#include "cVAOMeshManager.h"
#include "Utilities.h"
#include "CTextureManager.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

extern cShaderManager*	g_pShaderManager;		// Heap, new (and delete)
extern cLightManager*	g_pLightManager;
//extern Camera* g_pTheCamera;
extern bool drawDebugInfo;
extern cVAOMeshManager* g_pVAOManager;
extern glm::vec3 g_cameraXYZ;
extern glm::vec3 g_cameraTarget_XYZ;

//TODO::
//will have to delete later
cMesh* tempMesh = new cMesh();

bool renderScene(std::vector<cGameObject*> theObjects, GLFWwindow* theWindow) {
	float ratio;
	int width, height;
	glm::mat4x4 matProjection;			// was "p"

	glfwGetFramebufferSize(theWindow, &width, &height);
	ratio = width / (float)height;
	glViewport(0, 0, width, height);

	//		// Clear colour AND depth buffer
	//		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glEnable(GL_DEPTH_TEST);

	//        glUseProgram(program);
	::g_pShaderManager->useShaderProgram("mySexyShader");
	GLint curShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	// Update all the light uniforms...
	// (for the whole scene)
	::g_pLightManager->CopyLightInformationToCurrentShader();

	// Projection and view don't change per scene (maybe)
	matProjection = glm::perspective(0.6f,			// FOV
		ratio,		// Aspect ratio
		1.0f,		// Near (as big as possible)
		10000.0f);	// Far (as small as possible)


					// View or "camera" matrix
	glm::mat4 matView = glm::mat4(1.0f);	// was "v"

											// Now the veiw matrix is taken right from the camera class
	//matView = ::g_pTheCamera->getViewMatrix();
	matView = glm::lookAt( g_cameraXYZ,						// "eye" or "camera" position
						   g_cameraTarget_XYZ,		// "At" or "target" 
						   glm::vec3( 0.0f, 1.0f, 0.0f ) );	// "up" vector

	GLint uniLoc_mView = glGetUniformLocation(curShaderID, "mView");
	GLint uniLoc_mProjection = glGetUniformLocation(curShaderID, "mProjection");

	glUniformMatrix4fv(uniLoc_mView, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(matView));
	glUniformMatrix4fv(uniLoc_mProjection, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(matProjection));

	// Enable blend ("alpha") transparency for the scene
	// NOTE: You "should" turn this OFF, then draw all NON-Transparent objects
	//       Then turn ON, sort objects from far to near ACCORDING TO THE CAMERA
	//       and draw them
	glEnable(GL_BLEND);		// Enables "blending"
							//glDisable( GL_BLEND );
							// Source == already on framebuffer
							// Dest == what you're about to draw
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Draw the scene
	unsigned int sizeOfVector = (unsigned int)theObjects.size();	//*****//
	for (int index = 0; index != sizeOfVector; index++)
	{
		// This is the top level vector, so they are all "parents" 
		DrawObject(theObjects[index], curShaderID);
	}//for ( int index = 0...

	return true;
}


void DrawObject(cGameObject* pTheGO, GLint curShaderProgramID)
{

	// Is there a game object? 
	if (pTheGO == 0)	//if ( ::g_GameObjects[index] == 0 )
	{	// Nothing to draw
		return;		// Skip all for loop code and go to next
	}

	if (drawDebugInfo) {
		//build the bounding boxes 
		if (pTheGO->bHasAABB)
		{
			g_pVAOManager->lookupMeshFromName(pTheGO->meshName, *tempMesh);
			pTheGO->theBoundingBox->update(tempMesh, pTheGO->position, pTheGO->scale);
			pTheGO->theBoundingBox->buildBoundingBox(pTheGO->scale);
		}
	}

	std::string meshToDraw = pTheGO->meshName;		//::g_GameObjects[index]->meshName;

	sVAOInfo VAODrawInfo;
	if (::g_pVAOManager->lookupVAOFromName(meshToDraw, VAODrawInfo) == false)
	{	// Didn't find mesh
		return;
	}

	glm::mat4x4 mModel = glm::mat4x4(1.0f);	//		mat4x4_identity(m);

	glm::mat4 matRreRotZ = glm::mat4x4(1.0f);
	matRreRotZ = glm::rotate(matRreRotZ, pTheGO->orientation.z,
		glm::vec3(0.0f, 0.0f, 1.0f));
	mModel = mModel * matRreRotZ;

	glm::mat4 trans = glm::mat4x4(1.0f);
	trans = glm::translate(trans,
		pTheGO->position);
	mModel = mModel * trans;

	glm::mat4 matPostRotZ = glm::mat4x4(1.0f);
	matPostRotZ = glm::rotate(matPostRotZ, pTheGO->orientation2.z,
		glm::vec3(0.0f, 0.0f, 1.0f));
	mModel = mModel * matPostRotZ;


	glm::mat4 matPostRotY = glm::mat4x4(1.0f);
	matPostRotY = glm::rotate(matPostRotY, pTheGO->orientation2.y,
		glm::vec3(0.0f, 1.0f, 0.0f));
	mModel = mModel * matPostRotY;


	glm::mat4 matPostRotX = glm::mat4x4(1.0f);
	matPostRotX = glm::rotate(matPostRotX, pTheGO->orientation2.x,
		glm::vec3(1.0f, 0.0f, 0.0f));
	mModel = mModel * matPostRotX;
	// TODO: add the other rotation matrix (i.e. duplicate code above)
	float finalScale = pTheGO->scale;

	glm::mat4 matScale = glm::mat4x4(1.0f);
	matScale = glm::scale(matScale,
		glm::vec3(finalScale,
			finalScale,
			finalScale));
	mModel = mModel * matScale;

	GLuint uniLoc_mModel = glGetUniformLocation(curShaderProgramID, "mModel");;
	glUniformMatrix4fv(uniLoc_mModel, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(mModel));


	glm::mat4 mWorldInTranpose = glm::inverse(glm::transpose(mModel));

	GLuint uniLoc_materialDiffuse = glGetUniformLocation(curShaderProgramID, "materialDiffuse");
	glUniform4f(uniLoc_materialDiffuse,
		pTheGO->diffuseColour.r,
		pTheGO->diffuseColour.g,
		pTheGO->diffuseColour.b,
		pTheGO->diffuseColour.a);

	///////////////////////////////////////

	//Set ALL the samplers to something (so they don't point to GL_TEXTURE0)
	QnDTexureSamplerUtility::LoadUniformLocationsIfNeeded(curShaderProgramID);
	// There's no way we have 999 texture units...
	QnDTexureSamplerUtility::setAllSamplerUnitsToInvalidNumber(999);
	QnDTexureSamplerUtility::clearAllBlendValuesToZero();

	// Now set our samplers, and blend function to something
	// Basic texture binding setup (assign all loaded textures to samplers)
	g_pTextureManager->UpdateTextureBindingsByTextureNameSimple();
	std::map<std::string, CTexUnitInfoBrief> mapTexNameToTexUnit;

	g_pTextureManager->GetBoundTextureUnitsByTextureNames(mapTexNameToTexUnit);

	//// Now look up what textures our object is using and set the samplers
	QnDTexureSamplerUtility::SetSamplersForMeshTextures(*pTheGO, mapTexNameToTexUnit);

	GLint texHeightMap_UniLoc = glGetUniformLocation(curShaderProgramID, "texHeightMap");
	GLint bIsHeightMap_UniLoc = glGetUniformLocation(curShaderProgramID, "bIsHeightMap");

	{
		glUniform1f(bIsHeightMap_UniLoc, GL_FALSE);
	}

	GLint uniLoc_bIsSkyBoxObject = glGetUniformLocation(curShaderProgramID, "isASkyBox");
	if (pTheGO->bIsSkyBoxObject)
	{
		glUniform1f(uniLoc_bIsSkyBoxObject, GL_TRUE);
	}
	else
	{
		glUniform1f(uniLoc_bIsSkyBoxObject, GL_FALSE);
	}

	GLint uniLoc_bIsDebugWireFrameObject = glGetUniformLocation(curShaderProgramID, "bIsDebugWireFrameObject");

	// Wireframe?
	if (pTheGO->bIsWireFrame)
	{
		glUniform1f(uniLoc_bIsDebugWireFrameObject, 1.0f);	// TRUE
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Default
	}
	else
	{
		glUniform1f(uniLoc_bIsDebugWireFrameObject, 0.0f);	// FALSE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Default
	}
	///////////////////////////////////////////

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glBindVertexArray(VAODrawInfo.VAO_ID);

	glDrawElements(GL_TRIANGLES,
		VAODrawInfo.numberOfIndices,		// testMesh.numberOfTriangles * 3,	// How many vertex indices
		GL_UNSIGNED_INT,					// 32 bit int 
		0);
	// Unbind that VAO
	glBindVertexArray(0);
	return;
}
