#pragma once

#include <memory>
#include <vector>

#include "tileset/BasicTerrainset.h"
#include "tileset/TilesetType.h"
#include "engine/Sprite.h"
#include "engine/SpriteManager.h"
#include "map/Neighbour.h"



class Tile{
	std::shared_ptr<const BasicTerrainset> 	terrainset;
	int						basicSpriteID;
	int 					resourceID;

    int                     x;
    int                     y;

    Tile*                   neighbour[4];
    uint8_t                 neighbourMask;

	std::shared_ptr<const Sprite> sprite;

public:

	explicit				Tile(int x_, int y_);
							Tile(const Tile& other) = delete;
							~Tile() = default;

	void                   RandomizeBaseSprite(std::mt19937& rng);
	void                   RandomizeResource(std::mt19937& rng);
    void                   Update();
    void                   UpdateNeighbourMask();
    void                   SetTerrainset(std::shared_ptr<const BasicTerrainset> terrainset_);
	const BasicTerrainset& GetTerrainset() const;
	uint32_t 			   GetBasicSpriteHash() const;
	uint32_t               GetResourceSpriteHash() const;
	bool                   HasResource() const;

	void                   Render(int x, int y);
    void                   RenderRow(int startX, int startY, int maxX);

    int		               GetX() const;
    int		               GetY() const;
    bool	               IsOdd() const;

    void                   SetNeighbour(Tile *tile, Neighbour dir);
    Tile*                  GetNeighbour(Neighbour dir) const;
    void                   LinkNeighbours(Tile* tile, Neighbour dir);

};

inline const BasicTerrainset& Tile::GetTerrainset() const{
	return *terrainset;
}

inline uint32_t Tile::GetBasicSpriteHash() const{
	return terrainset->GetBasicSpriteHash(basicSpriteID);
}

inline void Tile::Render(int x, int y){
	sprite->Render(x, y);
}

inline int Tile::GetX() const{
    return x;
}

inline int Tile::GetY() const{
    return y;
}

inline bool	Tile::IsOdd() const{
    return GetY() % 2 == 1;
}
