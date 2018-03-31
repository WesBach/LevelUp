#ifndef _Keyboard_HG_
#define _Keyboard_HG_
#include <GLFW/glfw3.h>

struct sProjectile;
class cPlayer;
class Keyboard {
public:
	const static int KeyCount = 356;
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void key_resolution(int index);
	static void setKeys();
	static sProjectile createProjectileBasedOnPlayerStats(cPlayer* player);
};


#endif // !_Keyboard_HG_

