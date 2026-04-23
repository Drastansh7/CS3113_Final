#ifndef LEVEL3_H
#define LEVEL3_H

#include "Scene.h"
#include <vector>

class Level3 : public DungeonLevel
{
private:
	std::vector<unsigned int> mLevelData;
	static const int kWidth = 24;
	static const int kHeight = 18;

public:
	Level3(Vector2 origin, Texture2D tileset, Texture2D playerAtlas, Texture2D slimeAtlas, Texture2D ghostAtlas,
	       Texture2D chargerAtlas);
	void initialise() override;
	void render() override;
	void shutdown() override;
};

#endif
