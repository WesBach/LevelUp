#ifndef _cSoundManager_HG_
#define _cSoundManager_HG_
#include <fmod\fmod.hpp>
#include <fmod\fmod_errors.h>
#include <fstream>
#include <map>

struct SoundHelper {
public:
	int groupIndex;
	FMOD::Channel *theChannel;
	FMOD::Sound *theSound;
};

class cSoundManager {
public:
	void playEnemyDamagedSound();
	void playPlayerDamagedSound();
	void changeBackGroundMusic(std::string levelName);
	void playExplosionSound();
	void initSoundManager();

	void readSoundsFromSoundFile(std::string filename);

	std::map<std::string, SoundHelper> mSounds;
private:
	void checkForFMODError(FMOD_RESULT result);
	//Global variables
	FMOD_RESULT g_result;
	FMOD::System *g_system = nullptr;
	FMOD::ChannelGroup *group_actionSounds, *group_backgroundSongs, *group_master;
};

#endif // !_cSoundManager_HG_
