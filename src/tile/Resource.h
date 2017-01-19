#pragma once

#include <memory>

#include "engine/Sprite.h"

class Resource{

	Sprite sprite;
public:
	Resource(){};
	Resource(Sprite&& sprite_) noexcept;
	Resource(Resource&& other) noexcept;
	Resource(const Resource& other);

	~Resource();
	const Sprite& GetSprite() const;

	Resource& operator=(Resource&& other) noexcept;
};

