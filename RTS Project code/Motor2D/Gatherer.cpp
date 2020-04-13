#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Gui.h"
#include "EntityManager.h"

#include "Gatherer.h"


Gatherer::Gatherer(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)
{
	target_to_gather = nullptr;

	entity_sprite = App->entity_manager->GetGathererTexture();

	entity_sprite_section = { 52,0,52,49 };

	is_selectable = true;
	path_full = false;

	speed = 500;
}

Gatherer::~Gatherer()
{

}

bool Gatherer::Awake(pugi::xml_node&)
{
	return true;
}

bool Gatherer::Start()
{
	return true;
}

bool Gatherer::PreUpdate()
{
	return true;
}

bool Gatherer::Update(float dt, bool doLogic)
{
	//MoveInput();

	/*if ( path_full )
	{
		target_tile = entity_path.back();
	}*/

	//LOG("target_tile %d %d", target_tile.x, target_tile.y);

	HandleMovement(dt);

	//change section according to pathing. 
	switch (this->unit_state) {
	case ENTITY_STATE::PATHING_DOWN:
		entity_sprite_section = { 39,49,38,45 };
		break;
	case ENTITY_STATE::PATHING_RIGHT:
		entity_sprite_section = { 123,49,43,42 };
		break;
	case ENTITY_STATE::PATHING_LEFT:
		entity_sprite_section = { 78,49,43,42 };
		break;
	case ENTITY_STATE::PATHING_UP:
		entity_sprite_section = { 0,49,39,42 };
		break;
	case ENTITY_STATE::PATHING_DOWN_RIGHT:
		entity_sprite_section = { 52,0,52,49 };
		break;
	case ENTITY_STATE::PATHING_DOWN_LEFT:
		entity_sprite_section = { 0,0,52,49 };
		break;
	case ENTITY_STATE::PATHING_UP_RIGHT:
		entity_sprite_section = { 104,0,52,49 };
		break;
	case ENTITY_STATE::PATHING_UP_LEFT:
		entity_sprite_section = { 156,0,52,49 };
		break;
	}


	selection_collider.x = pixel_position.x + 10;
	selection_collider.y = pixel_position.y + 10;

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y - 14, &entity_sprite_section);


	//App->render->DrawQuad(selection_collider, 255, 255, 0, 100);

	return true;
}

bool Gatherer::PostUpdate()
{
	return true;
}

bool Gatherer::CleanUp()
{
	App->tex->UnLoad(entity_sprite);

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}
	
	return true;
}

void Gatherer::OnCollision(Collider* C1, Collider* C2)
{

}