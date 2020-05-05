#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Player.h"
#include "GuiManager.h"
#include "UI.h"
#include "UI_Healthbar.h"
#include "EntityManager.h"

#include "EnemyInfantry.h"


EnemyInfantry::EnemyInfantry(int x, int y, ENTITY_TYPE type, int level) : DynamicObject(x, y, type, level)  //Constructor. Called at the first frame.
{
	InitEntity();
};

EnemyInfantry::~EnemyInfantry()  //Destructor. Called at the last frame.
{

};

bool EnemyInfantry::Awake(pugi::xml_node& config)
{
	return true;
};

bool EnemyInfantry::Start()
{
	return true;
};

bool EnemyInfantry::PreUpdate()
{	
	return true;
};

bool EnemyInfantry::Update(float dt, bool do_logic)
{
	HandleMovement(dt);

	DataMapSafetyCheck();

	if (path_full)
	{
		UpdateUnitSpriteSection();
	}
	else
	{
		UpdateUnitOrientation();
	}

	selection_collider.x = (int)pixel_position.x;
	selection_collider.y = (int)pixel_position.y;

	if (do_logic)
	{
		if (target == nullptr && !path_full)
		{
			SetEntityTargetByProximity();
		}
	}

	if (target != nullptr)
	{
		if (TargetIsInRange())
		{
			DealDamage();
		}
		else
		{
			if (!path_full)
			{
				ChaseTarget();
			}
		}
	}

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height / 2);

	return true;
};

bool EnemyInfantry::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}
	
	return true;
};

bool EnemyInfantry::CleanUp()
{
	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}

	App->gui_manager->DeleteGuiElement(healthbar);
	
	return true;
};

void EnemyInfantry::Draw()
{
	App->render->Blit(this->entity_sprite, (int)pixel_position.x, (int)pixel_position.y, &entity_sprite_section);

	if (App->player->god_mode)
	{
		App->render->DrawQuad(selection_collider, 255, 255, 0, 100);
	}
}

void EnemyInfantry::InitEntity()
{
	entity_sprite = App->entity_manager->GetEnemyTexture();

	InitUnitSpriteSections();

	is_selectable = false;
	is_selected = false;
	path_full = false;

	target = nullptr;
	attack_in_cooldown = false;
	accumulated_cooldown = 0.0f;

	speed = 350.0f;

	max_health = 300;
	current_health = max_health;
	attack_damage = 30;

	attack_speed = 0.75f;
	attack_range = 5;

	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
	}

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height * 0.5f);
}

void EnemyInfantry::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;										//Magic
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 967, 1, MAX_UNIT_HEALTHBAR_WIDTH, 6 };
	healthbar_rect = { 967, 7, MAX_UNIT_HEALTHBAR_WIDTH, 6 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui_manager->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void EnemyInfantry::InitUnitSpriteSections()
{
	entity_sprite_section		= { 58, 0, 58, 47 };

	pathing_up_section			= { 0, 47, 70, 52 };
	pathing_down_section		= { 71, 47, 70, 52 };
	pathing_rigth_section		= { 202, 47, 59, 52 };
	pathing_left_section		= { 142, 47, 59, 52 };
	pathing_up_right_section	= { 116, 0, 60, 47 };
	pathing_up_left_section		= { 176, 0, 59, 47 };
	pathing_down_right_section	= { 58, 0, 58, 47 };
	pathing_down_left_section	= { 0, 0, 58, 47 };
}

void EnemyInfantry::UpdateUnitSpriteSection()
{
	//change section according to pathing. 
	switch (unit_state)
	{
	case ENTITY_STATE::PATHING_UP:
		entity_sprite_section = pathing_up_section;
		break;
	case ENTITY_STATE::PATHING_DOWN:
		entity_sprite_section = pathing_down_section;
		break;
	case ENTITY_STATE::PATHING_RIGHT:
		entity_sprite_section = pathing_rigth_section;
		break;
	case ENTITY_STATE::PATHING_LEFT:
		entity_sprite_section = pathing_left_section;
		break;
	case ENTITY_STATE::PATHING_UP_RIGHT:
		entity_sprite_section = pathing_up_right_section;
		break;
	case ENTITY_STATE::PATHING_UP_LEFT:
		entity_sprite_section = pathing_up_left_section;
		break;
	case ENTITY_STATE::PATHING_DOWN_RIGHT:
		entity_sprite_section = pathing_down_right_section;
		break;
	case ENTITY_STATE::PATHING_DOWN_LEFT:
		entity_sprite_section = pathing_down_left_section;
		break;
	}
}

void EnemyInfantry::SetEntityTargetByProximity()
{
	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsAllyEntity((*item)))
		{
			if (App->pathfinding->DistanceInTiles(tile_position, (*item)->tile_position) <= attack_range)
			{
				target = (*item);
				break;
			}
		}
	}
}

void EnemyInfantry::GetShortestPathWithinAttackRange()
{
	std::vector<iPoint> tmp;

	if (target != nullptr)
	{
		for (int i = 0; i < (int)entity_path.size(); ++i)
		{
			tmp.push_back(entity_path[i]);

			if ((entity_path[i].DistanceNoSqrt(target->tile_position) * 0.1f) <= attack_range)
			{
				entity_path.clear();

				entity_path = tmp;

				target_tile = entity_path.back();
				current_path_tile = entity_path.begin();

				tmp.clear();

				break;
			}
		}
	}
}

void EnemyInfantry::UpdateUnitOrientation()
{
	if (target != nullptr)
	{
		if (tile_position.x > target->tile_position.x && tile_position.y > target->tile_position.y)					// next_tile is (--x , --y)
		{
			entity_sprite_section = pathing_up_section;
			return;
		}

		if (tile_position.x < target->tile_position.x && tile_position.y < target->tile_position.y)					// next_tile is (++x , ++y)
		{
			entity_sprite_section = pathing_down_section;
			return;
		}

		if (tile_position.x < target->tile_position.x && tile_position.y > target->tile_position.y)					// next_tile is (--x , ++y)
		{
			entity_sprite_section = pathing_rigth_section;
			return;
		}

		if (tile_position.x > target->tile_position.x && tile_position.y < target->tile_position.y)					// next_tile is (++x, --y)
		{
			entity_sprite_section = pathing_left_section;
			return;
		}

		if (tile_position.x == target->tile_position.x && tile_position.y > target->tile_position.y)				// next_tile is (== , --y)
		{
			entity_sprite_section = pathing_up_right_section;
			return;
		}

		if (tile_position.x > target->tile_position.x && tile_position.y == target->tile_position.y)				// next tile is (--x, ==)
		{
			entity_sprite_section = pathing_up_left_section;
			return;
		}

		if (tile_position.x < target->tile_position.x && tile_position.y == target->tile_position.y)				// next tile is (++x, ==)
		{
			entity_sprite_section = pathing_down_right_section;
			return;
		}

		if (tile_position.x == target->tile_position.x && tile_position.y < target->tile_position.y)				// next tile is (==, ++y)
		{
			entity_sprite_section = pathing_down_left_section;
			return;
		}
	}
}

bool EnemyInfantry::TargetIsInRange()
{
	if (target != nullptr)
	{
		if (App->pathfinding->DistanceInTiles(tile_position, target->tile_position) <= attack_range)
		{
			return true;
		}
	}

	return false;
}

void EnemyInfantry::ChaseTarget()
{
	std::vector<DynamicObject*> tmp;
	tmp.push_back(this);

	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);

	//GiveNewTargetTile(target->tile_position);
	App->pathfinding->AttackOrder(target->tile_position, tmp);
}

void EnemyInfantry::DealDamage()
{
	if (!attack_in_cooldown)
	{
		ApplyDamage(target);
		attack_in_cooldown = true;
	}
	else
	{
		accumulated_cooldown += App->GetDt();

		if (accumulated_cooldown >= attack_speed)
		{
			attack_in_cooldown = false;
			accumulated_cooldown = 0.0f;
		}
	}

	if (target->current_health <= 0)
	{
		target = nullptr;
	}
}

// Collision Handling ---------------------------------------
void EnemyInfantry::OnCollision(Collider* C1, Collider* C2)
{
	return;
}

Entity* EnemyInfantry::GetTarget()
{
	return target;
}

int  EnemyInfantry::GetAttackRange()
{
	return attack_range;
}