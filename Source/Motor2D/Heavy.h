#ifndef __HEAVY_H__
#define __HEAVY_H__

#include "AllyUnit.h"

class Heavy : public AllyUnit
{
public:
	Heavy(int x, int y, ENTITY_TYPE type, int level);
	~Heavy();

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
	void GetShortestPathWithinAttackRange();
	void UpdateUnitOrientation();

	bool TargetIsInRange();
	void ChaseTarget();
	void DealDamage();

	Entity* GetTarget();
	int GetAttackRange();

public:
	/*int							attack_range;							// A unit's attack range in tiles.
	int							attack_radius;							// The unit's effective attack area. The heavy has an AoE attack. Radius in tiles.
	float						attack_speed;							// A unit's attack speed in attacks/second.*/

	//const std::vector<iPoint>*	entity_path;
};

#endif // !__HEAVY_H__