#pragma once
#include <map>

#include <glm/glm.hpp>
#include <irrKlang/irrKlang.h>

class AudioSlave
{
private:
	irrklang::ISoundEngine* engine = nullptr;

	std::map<std::string, irrklang::ISoundSource*> sounds;

	irrklang::ISound* music = nullptr;

	bool walkSwitch = false;
public:
	AudioSlave();

	void UpdateListenerPosition(glm::vec3 position, glm::vec3 front, glm::vec3 world_up);

	void PlayWalk();

	void PlayMusic2D();
	void PlayMusic3D();

	~AudioSlave();
};
