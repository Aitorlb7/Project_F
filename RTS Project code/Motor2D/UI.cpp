#include "Application.h"
#include "Render.h"
#include "Input.h"
#include "GuiManager.h"

#include "UI.h"

UI::UI(UI_ELEMENT element, int x, int y, SDL_Rect rect, Module* listener, UI* parent) :
	element(element),position(x, y),rect(rect),listener(listener),parent(parent),dragXAxis(false),dragYAxis(false),isDraggable(false), isDragTarget(false), isFilled(false),
	 isInteractible(false), isVisible(false), localHitbox({ 0,0,0,0 }), localRect({0,0,0,0}),ui_event(UI_EVENT::IDLE),hitbox({0,0,0,0})
	
{
	//hitbox = {GetPosition().x, GetPosition().y, GetRect().w, GetRect().h };
	hitbox = {position.x, position.y, rect.w, rect.h };
}

UI::UI() : element(UI_ELEMENT::EMPTY), position(0, 0), rect({ 0,0,0,0 }), listener(nullptr), parent(nullptr), dragXAxis(false), dragYAxis(false), isDraggable(false),
	isDragTarget(false), isFilled(false), isInteractible(false), isVisible(false), localHitbox({ 0,0,0,0 }), localRect({ 0,0,0,0 }), ui_event(UI_EVENT::IDLE),hitbox({0,0,0,0})
{

}

UI::~UI()
{

}

bool UI::Draw()
{	
	return true;
}

void UI::CheckInput()
{
	return;
}

void UI::CleanUp()
{
	return;
}

void UI::BlitElement(SDL_Texture* texture, int x, int y, SDL_Rect* rect, float speed, float render_scale)
{
	App->render->Blit(texture, x, y, rect, false, speed, render_scale);		//Setting the blit's speed argument to 0 will make the UI Element remain static in relation to the camera.
}

// ----------------------------------------- SET/GET METHODS -----------------------------------------
void UI::SetScreenPos(iPoint position)
{
	this->position = position;							//this-> Allows to overload the position variable name as it declares that the "position" we are setting is the one declared in UI.h.
}

iPoint UI::GetScreenPos() const
{
	return position;									//Returns the position of a UI element.
}

void UI::SetScreenRect(SDL_Rect rect)
{
	this->rect = rect;									//this-> Sets the pointer of a UI_Element so only that UI element's rect is changed. Moreover it allows to overload the rect variable name.
}

SDL_Rect UI::GetScreenRect() const
{
	return rect;										//Returns the rect of a UI element.
}

void UI::SetHitbox(SDL_Rect hitbox)
{
	this->hitbox = hitbox;								//this-> Sets the pointer of a UI_Element so only that UI element's hitbox is changed. Moreover it allows to overload the hitbox variable name.
}

SDL_Rect UI::GetHitbox() const
{
	return hitbox;										//Returns the hitbox of a UI element.
}

void UI::SetLocalPos(iPoint local_position)
{
	this->local_position = local_position;
}

iPoint UI::GetLocalPos() const
{
	return local_position;
}

void UI::SetLocalRect(SDL_Rect local_rect)
{
	int localPosX = local_rect.x - this->parent->rect.x;
	int localPosY = local_rect.y - this->parent->rect.y;
	
	SDL_Rect newLocalRect = { localPosX, localPosY, local_rect.w, local_rect.h };

	this->local_rect = newLocalRect;

	//SetScreenRect(newLocalRect);
}

SDL_Rect UI::GetLocalRect() const
{
	if (parent != NULL)
	{
		//SDL_Rect localRect = { rect.x - parent->rect.x, rect.y - parent->rect.y, rect.w, rect.h };
		//SDL_Rect localRect = { localPosition.x, localPosition.y, rect.w, rect.h };

		return local_rect;
	}

	return { 0,0,0,0 };
}

void UI::SetLocalHitbox(SDL_Rect local_hitbox)
{
	int localPosX = local_rect.x - this->parent->rect.x;
	int localPosY = local_rect.y - this->parent->rect.y;
	
	SDL_Rect newLocalHitbox = { localPosX, localPosY, local_hitbox.w, local_hitbox.h };

	this->local_hitbox = newLocalHitbox;
}

// -------------------------------- UI ELEMENT INTERACTIONS --------------------------------
iPoint UI::GetMousePos() /*const*/
{
	App->input->GetMousePosition(mouse_position.x, mouse_position.y);

	return mouse_position;
}

bool UI::CheckMousePos() const
{
	return(mouse_position.x > hitbox.x && mouse_position.x < hitbox.x + hitbox.w
		&& mouse_position.y > hitbox.y && mouse_position.y < hitbox.y + hitbox.h);
}

iPoint UI::GetMouseMotion() /*const*/
{
	App->input->GetMouseMotion(mouse_motion.x, mouse_motion.y);

	return mouse_motion;
}

// --- This method checks whether a UI Element is being hovered (The mouse is inside it's hitbox).
bool UI::IsHovered() const
{
	return CheckMousePos();
}

// --- This method checks whether the focused element is the same as the element that called the method.
bool UI::IsFocused() const
{
	return App->gui_manager->focused_element == this;
}

// --- This method checks whether the element that called the method is the foremost element under the mouse.
bool UI::IsForemostElement() const
{
	return App->gui_manager->FirstElementUnderMouse() == this;
}

// --- This method checks whether or not the element that called the method fulfills the conditions to be dragged.
bool UI::ElementCanBeDragged() const
{
	return ((is_draggable && is_drag_target && App->gui_manager->FirstElementUnderMouse() == this) || is_drag_target);
}

// --- This method checks whetheror not the element that called the method has been clicked but not dragged anywhere.
bool UI::ElementRemainedInPlace() const
{
	return (GetScreenPos() == initial_position);
}

// --- Drags an element around taking into account where the mouse was and where it currently is.
void UI::DragElement()
{	
																								// --- Updating the UI Element's position when it is being dragged.
	iPoint origin(0, 0);																		//This prevents sending undragged elements to undesired places when passing 
																								//from dragging one element to another as prevMousePos in undragged elements is (0,0).

	if (previous_mouse_position != GetMousePos() && previous_mouse_position != origin)
	{
		position += GetMousePos() - previous_mouse_position;									//Check this, mouse is not at a fixed point, the UI Element moves too slow.
		SetScreenPos(position);
	}

																								// --- Updating the UI Element's hitbox rect when it is being dragged.
	SDL_Rect newPosRect = { position.x, position.y, rect.w, rect.h };

	this->SetHitbox(newPosRect);

	//iPoint draggingPos = position + GetMouseMotion();
}

void UI::AxisRestrictedDragElement(bool X_Axis, bool Y_Axis)
{
	if (X_Axis == Y_Axis)
	{
		return;
	}
	
	if (X_Axis)
	{
		if (parent == NULL)
		{
			position.x += GetMousePos().x - previous_mouse_position.x;
			SetScreenPos(position);
		}
		else
		{
			if (position.x != parent->position.x && position.x + GetHitbox().w != parent->position.x + parent->GetHitbox().w)
			{
				position.x += GetMousePos().x - previous_mouse_position.x;
				SetScreenPos(position);
			}

			if (position.x <= parent->position.x)
			{
				position.x = parent->position.x + DRAG_LIMIT_OFFSET;
			}

			if (position.x + GetHitbox().w >= parent->position.x + parent->GetHitbox().w)
			{
				position.x = (parent->position.x + parent->GetHitbox().w) - (GetHitbox().w + DRAG_LIMIT_OFFSET);
			}
		}
	}
	
	if (Y_Axis)
	{
		if (parent == NULL)
		{
			position.y += GetMousePos().y - previous_mouse_position.y;
			SetScreenPos(position);
		}
		else
		{
			if (position.y != parent->position.y && position.y + GetHitbox().h != parent->position.y + parent->GetHitbox().h)
			{
				position.y += GetMousePos().y - previous_mouse_position.y;
				SetScreenPos(position);
			}

			if (position.y <= parent->position.y)
			{
				position.y = parent->position.y + DRAG_LIMIT_OFFSET;
			}

			if (position.y + GetHitbox().h >= parent->position.y + parent->GetHitbox().h)
			{
				position.y = (parent->position.y + parent->GetHitbox().h) - (GetHitbox().h + DRAG_LIMIT_OFFSET);
			}
		}
	}

	SDL_Rect newPosRect = { position.x, position.y, rect.w, rect.h };

	this->SetHitbox(newPosRect);
}

// --- This method Checks if a UI Element has childs and updates them in case the UI Element (parent) has been moved/dragged.
void UI::CheckElementChilds()
{
	if (App->gui_manager->ElementHasChilds(this))
	{
		App->gui_manager->UpdateChilds(this);
	}
}