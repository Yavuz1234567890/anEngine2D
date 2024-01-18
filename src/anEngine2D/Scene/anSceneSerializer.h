#ifndef AN_SCENE_SERIALIZER_H_
#define AN_SCENE_SERIALIZER_H_

#include "anScene.h"
#include "Core/anFileSystem.h"

#include <tinyxml2.h>

class anSceneSerializer
{
public:
	anSceneSerializer();
	~anSceneSerializer();

	anEntity DeserializeEntity(const anFileSystem::path& location, tinyxml2::XMLElement* element, anScene* scene);
	void SerializeEntity(const anFileSystem::path& location, const anEntity& ent, tinyxml2::XMLPrinter& printer);
	anScene* DeserializeScene(const anFileSystem::path& location, const anFileSystem::path& filePath);
	void SerializeScene(const anFileSystem::path& location, anScene* scene, const anFileSystem::path& filePath);
};

#endif
