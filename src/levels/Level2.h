#ifndef LEVEL2_H
#define LEVEL2_H

#include "Scene.h"
#include <vector>

class Level2 : public DungeonLevel
{
private:
	std::vector<unsigned int> mLevelData;
	static const int kWidth = 22;
	static const int kHeight = 16;

public:
	Level2(Vector2 origin, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas, Texture2D ghostAtlas,
	       Texture2D chargerAtlas);
	void initialise() override;
	void render() override;
	void shutdown() override;
};

#endif
