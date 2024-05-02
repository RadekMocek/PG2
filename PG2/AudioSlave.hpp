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
	void UpdateMusicPosition(glm::vec3 position);
	
	void UpdateMusicVolume(float amount);

	void Play2DOneShot(std::string sound_name);
	void Play3DOneShot(std::string sound_name, glm::vec3 position);
	void PlayWalk();

	void PlayMusic3D();

	~AudioSlave();
};
