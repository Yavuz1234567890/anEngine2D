#ifndef AN_SCENE_SERIALIZER_H_
#define AN_SCENE_SERIALIZER_H_

#include "anScene.h"

class anSceneSerializer
{
public:
	anSceneSerializer();
	~anSceneSerializer();

	anScene* DeserializeScene(const anString& filePath);
	void SerializeScene(anScene* scene, const anString& filePath);
};

#endif
