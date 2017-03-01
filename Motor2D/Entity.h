#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"
#include "SDL\include\SDL.h"

using namespace std;

struct Collider;

class Entity
{
public:
	Entity();
	~Entity();
	iPoint GetPosition() const;
	int GetEntityID() const;
	void SetActive(bool active);

	virtual bool Update(float dt);
	virtual bool Draw();
	virtual bool HandleInput();

public:
	int entityID;
	bool isActive = false;
	SDL_Texture* entityTexture;
	iPoint entityPosition;
	Collider* collider;
};

#endif // !__ENTITY_H__


