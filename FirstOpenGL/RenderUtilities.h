#ifndef  _RenderUtilities_HG_
#define _RenderUtilities_HG_
#include "globalOpenGL_GLFW.h"
#include "cGameObject.h"


bool renderScene(std::vector<cGameObject*> theObjects, GLFWwindow* theWindow);

void DrawObject(cGameObject* pTheGO, GLint curShaderProgramID);


#endif // ! _RenderUtilities_HG_
