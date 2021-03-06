#ifndef __INFANTRY_H__
#define __INFANTRY_H__

#include "AllyUnit.h"

class Infantry : public AllyUnit
{
public:
	Infantry(int x, int y, ENTITY_TYPE type, int level);
	~Infantry();

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

	void InitUnitSpriteSections();
	void UpdateUnitSpriteSection();
	
	void SetEntityTargetByProximity();
	void UpdateUnitOrientation();

	bool TargetIsInRange();
	void ChaseTarget();
	void DealDamage();

	Entity* GetTarget();
	int GetAttackRange();

public:
	/*int							attack_range;							// A unit's attack range in tiles.
	float						attack_speed;							// A unit's attack speed in attacks/second.*/

	//const std::vector<iPoint>*	entity_path;
};

#endif // __INFANTRY_H__