#ifndef LEVEL1_H
#define LEVEL1_H

#include "Scene.h"
#include <vector>

class Level1 : public DungeonLevel
{
private:
	std::vector<unsigned int> mLevelData;
	static const int kWidth = 20;
	static const int kHeight = 14;

public:
	Level1(Vector2 origin, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas, Texture2D ghostAtlas,
	       Texture2D chargerAtlas);
	void initialise() override;
	void render() override;
	void shutdown() override;
};

#endif
