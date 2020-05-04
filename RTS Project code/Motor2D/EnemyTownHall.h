#ifndef __ENEMY_TOWNHALL_H__
#define __ENEMY_TOWNHALL_H__

#include "Building.h"

class EnemyTownHall : public Building
{
public:
	EnemyTownHall(int x, int y, ENTITY_TYPE type, int level);
	~EnemyTownHall();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();
	bool CleanUp();

	void Draw();

public:
	void InitEntity();
	void AttachHealthbarToEntity();
	void AttachCreationBarToEntity();

	void StartUnitCreation();

	void GenerateUnit(ENTITY_TYPE type, int level);

	void LevelChanges();

public:
	SDL_Rect	hall_rect;
	SDL_Rect	hall_rect_1;
	SDL_Rect	hall_rect_2;

	int			unit_level;

	float		enemy_gatherer_creation_time;

	ENTITY_TYPE created_unit_type;
	bool		creating_unit;
};

#endif // !__ENEMY_TOWNHALL_H__
