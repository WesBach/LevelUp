#include "cSoundManager.h"
#include "Utilities.h"
#include <iostream>
#include <algorithm>
#include <vector>

//Name:		checkForFMODError
//Purpose:	Checks to see the outcome of the FMOD_RESULT and prints an error then exits
//Return:	void
void cSoundManager::checkForFMODError(FMOD_RESULT result) {
	if (result != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		//exit(-1);
	}
}

void  cSoundManager::playEnemyDamagedSound() {
	//TODO::play enemy damaged sound
}

void  cSoundManager::playPlayerDamagedSound() {
	//TODO:: play player damaged sound
}

void cSoundManager::playExplosionSound() {
	//TODO:: play explosion sound
}

void  cSoundManager::changeBackGroundMusic(std::string levelName) {
	//TODO:: play the correct background music for the level
}

void  cSoundManager::initSoundManager() {
	// Create the main system object.
	g_result = FMOD::System_Create(&g_system);
	checkForFMODError(g_result);

	//Initializes the system object, and the msound device.
	g_result = g_system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	checkForFMODError(g_result);
}

void cSoundManager::readSoundsFromSoundFile(std::string filename) {
	//TODO:: read sounds from sound file into sound map
	std::ifstream configFile(filename.c_str());
	std::string tempString = "";
	int numberOfFiles = 0;
	std::vector<std::string> soundTitles;

	if (!configFile.is_open())
	{	// Didn't open file, so return
		std::cout << "Error Opening Sound Info File!" << std::endl;
	}
	// File is open, let's read it

	ReadFileToToken(configFile, "SOUND_TYPE_SELECTION");
	//	int numVertices = 0;
	configFile >> tempString;
	//set the string to lower case
	transform(tempString.begin(), tempString.end(), tempString.begin(), ::tolower);

	//if the sound type read in is compressed or uncompressed
	if (tempString == "uncompressed")
	{
		//get the number of files to read 
		ReadFileToToken(configFile, "numberOfUncompressedFiles");
		configFile >> numberOfFiles;
		ReadFileToToken(configFile, "START_UNCOMPRESSED");
		//read that many files in
		for (int i = 0; i < numberOfFiles; i++) {
			tempString = "";
			configFile >> tempString;
			//tell the program where to find the file
			soundTitles.push_back("assets/" + tempString);

		}
	}

	//if the sound titles vector is greater than 0
	if (soundTitles.size() > 0)
	{
		FMOD::Sound *tempSound;
		FMOD::Channel *tempChannel;
		SoundHelper* tempSoundHelper;
		int tempIndex = 0;

		for (unsigned int i = 0; i < soundTitles.size(); i++)
		{
			tempSoundHelper = new SoundHelper();

			g_result = g_system->createStream(soundTitles[i].c_str(), FMOD_LOOP_NORMAL | FMOD_DEFAULT, 0, &tempSound);
			checkForFMODError(g_result);

			//pause all the sounds
			g_result = g_system->playSound(tempSound, 0, true, &tempChannel);

			//check for errors
			checkForFMODError(g_result);
			//set up the temp sound helper 
			tempSoundHelper->groupIndex = tempIndex;
			tempSoundHelper->theChannel = tempChannel;
			tempSoundHelper->theSound = tempSound;
			//insert the temp sound helper into the map
			this->mSounds[soundTitles[i]] = *tempSoundHelper;
		}
	}
}
