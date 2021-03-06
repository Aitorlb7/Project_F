#include "Dependencies\Brofiler\Brofiler.h"

#include "Application.h"
#include "Textures.h"
#include "Input.h"
#include "Fonts.h"
#include "GuiManager.h"

#include "GuiText.h"


//GuiText can be interactible and draggable. Can potentially have all events.
//This element can receive up to 5 different strings (one for each possible event).
GuiText::GuiText(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, GuiElement* parent,
			std::string* string, std::string* hoverString, std::string* leftClickString, std::string* rightClickString): GuiElement(type, x, y, hitbox, listener, parent),
			idle_texture(nullptr), hover_texture(nullptr), left_click_texture(nullptr), right_click_texture(nullptr), input_text_texture (nullptr), current_texture (nullptr)
{	
	// --- Setting this element's flags to the ones passed as argument.
	this->is_visible = is_visible;												//Sets the isVisible flag to the one passed as argument.
	this->is_interactible = is_interactible;									//Sets the isInteractible flag to the one passed as argument. 
	this->is_draggable = is_draggable;											//Sets the isDraggable flag to the one passed as argument.
	this->drag_x_axis = is_draggable;											//Sets the dragXaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	this->drag_y_axis = is_draggable;											//Sets the dragYaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	previous_mouse_position = iPoint(0, 0);										//Initializes prevMousePos for this UI Element. Safety measure to avoid weird dragging behaviour.
	initial_position = GetScreenPos();											//Records the initial position where the element is at at app execution start.
	// ---------------------------------------------------------------

	//Loading all strings. Print() generates a texture with the given string with the a font and a colour.
	if (string != nullptr)
	{
		idle_texture = App->font->Print(string->c_str(), fontColour, font);						//Creates the texture for the idle state.
	}

	if (hoverString != nullptr)
	{
		hover_texture = App->font->Print(hoverString->c_str(), fontColour, font);				//Creates the texture for the hover state.
	}
	
	if (leftClickString != nullptr)
	{
		left_click_texture = App->font->Print(leftClickString->c_str(), fontColour, font);		//Creates the texture for the left-clicked state.
	}

	if (rightClickString != nullptr)
	{
		right_click_texture = App->font->Print(rightClickString->c_str(), fontColour, font);	//Crates the texture for the right_clicked state.
	}
	// ----------------------------------------------------------------------------------------------------

	if (this->is_interactible)																	//If the Image element is interactible.
	{
		this->listener = nullptr;																//This Text element's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != nullptr)																	//If a parent is passed as argument.
	{
		int localPosX = x - parent->GetScreenPos().x;											//Gets the local position of the Text element in the X Axis.
		int localPosY = y - parent->GetScreenPos().y;											//Gets the local position of the Text element in the Y Axis.

		iPoint localPos = { localPosX, localPosY };												//Buffer iPoint to pass it as argument to SetLocalPos().

		SetLocalPos(localPos);																	//Sets the local poisition of this Text Element to the given localPos.
	}

	this->font = font;
	this->font_colour = fontColour;

	text_rect = { 0, 0, 0, 0 };			//
}

GuiText::GuiText() : GuiElement ()		//Default Constructor
{}

bool GuiText::Draw()
{	
	CheckInput();

	if (current_texture != nullptr)
	{
		SDL_QueryTexture(current_texture, nullptr, nullptr, &text_rect.w, &text_rect.h);			//REVISE  THIS  LATER

		SetHitbox({ GetHitbox().x, GetHitbox().y, text_rect.w, text_rect.h });

		BlitElement(current_texture, GetScreenPos().x, GetScreenPos().y, nullptr, 0.0f, 1.0f);
	}

	return true;
}

// --- This Method checks for any inputs that the GuiText element might have received and "returns" an event.
void GuiText::CheckInput()
{	
	BROFILER_CATEGORY("Text_CheckInput", Profiler::Color::Indigo);

	if (!is_visible)
	{
		current_texture = NULL;
	}
	
	if (is_visible)																						//If the Text element is visible.
	{	
		GetMousePos();																					//Gets the mouse's position on the screen.

		// --- IDLE EVENT
		if (!IsHovered() && ui_event != GUI_EVENT::FOCUSED)												//If the mouse is not on the text.
		{
			ui_event = GUI_EVENT::IDLE;

			if (input_text_texture == NULL)																//This sometimes crashes the Query_texture.
			{
				current_texture = idle_texture;															//Blit the idle text.
			}
			else
			{
				current_texture = input_text_texture;
			}
		}

		/*if (ui_event == UI_Event::FOCUSED)
		{
			currentTex = inputTextTex;
		}*/

		if (is_interactible)																			//If the Text element is interactible.
		{
			// --- HOVER EVENT
			if ((IsHovered() && IsForemostElement()) /*|| IsFocused()*/)								//If the mouse is on the text.
			{
				ui_event = GUI_EVENT::HOVER;

				if (hover_texture != NULL)
				{
					current_texture = hover_texture;													//Blit the hover text.
				}
			}

			// --- CLICKED EVENT (Left Click)
			if (IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)				//If the mouse is on the text and the left mouse button is pressed.
			{
				if (IsForemostElement())
				{
					previous_mouse_position = GetMousePos();											//Sets the initial position where the mouse was before starting to drag the element.
					initial_position = GetScreenPos();													//Sets initialPosition with the current position at mouse KEY_DOWN.
					is_drag_target = true;																//Sets the element as the drag target.
				}
			}

			if ((IsHovered() || is_drag_target) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)		//If the mouse is on the text and the left mouse button is being pressed.
			{
				if (IsForemostElement() || is_drag_target)												//If the UI Text element is the foremost element under the mouse. 
				{
					ui_event = GUI_EVENT::CLICKED;

					if (left_click_texture != NULL)
					{
						current_texture = left_click_texture;											//Blit the left click text.
					}

					if (ElementCanBeDragged())															//If the UI Text element is draggable and is the foremost element under the mouse. 
					{
						DragElement();																	//The Text element is dragged.
						
						CheckElementChilds();															//Checks if this Text element has any childs and updates them in consequence.

						previous_mouse_position = GetMousePos();										//Updates prevMousePos so it can be dragged again next frame.
					}
				}
			}

			// --- UNCLICKED EVENT (Left Click)
			if ((IsHovered() || is_drag_target) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_UP)		//If the mouse is on the text and the left mouse button is released.
			{
				if (IsForemostElement() && ElementRemainedInPlace())									//If the UI Text element is the foremost element under the mouse and has not been dragged. 
				{
					ui_event = GUI_EVENT::UNCLICKED;
				}

				if (is_drag_target)
				{
					is_drag_target = false;
					initial_position = GetScreenPos();
				}

				//currentRect = clicked;																//Button Hover sprite.
			}

			// --- CLICKED EVENT (Right Click)
			if (IsHovered() == true && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_STATE::KEY_DOWN)	//If the mouse is on the button and the right mouse button is pressed.
			{
				if (IsForemostElement())																//If the UI Text element is the foremost element under the mouse. 
				{
					ui_event = GUI_EVENT::CLICKED;

					if (right_click_texture != NULL)
					{
						current_texture = right_click_texture;											//Blit the right click text.
					}
				}
			}

			if (listener != nullptr)
			{
				listener->OnEventCall(this, ui_event);													//The listener call the OnEventCall() method passing the current event as argument.
			}
		}
	}
}

void GuiText::CleanUp()
{
	if (idle_texture != nullptr)
	{
		App->tex->UnLoad(idle_texture);
	}

	if (hover_texture != nullptr)
	{
		App->tex->UnLoad(hover_texture);
	}

	if (left_click_texture != nullptr)
	{
		App->tex->UnLoad(left_click_texture);
	}

	if (right_click_texture != nullptr)
	{
		App->tex->UnLoad(right_click_texture);
	}

	if (input_text_texture != nullptr)
	{
		App->tex->UnLoad(input_text_texture);
	}

	if (current_texture != nullptr)
	{
		App->tex->UnLoad(current_texture);
	}
}

SDL_Texture* GuiText::GetTexture() const
{
	return current_texture;
}

SDL_Texture* GuiText::GetCurrentStringTex()
{
	return current_texture;
}

void GuiText::DeleteCurrentStringTex()
{
	current_texture = NULL;
}

std::string* GuiText::GetString()
{
	return string;
}

// ----------------------------------------- INPUT TEXT METHODS -----------------------------------------
void GuiText::RefreshTextInput(const char* newString)
{
	if (input_text_texture == NULL)
	{
		input_text_texture = App->font->Print(newString, font_colour, font);
	}
	else
	{
		App->tex->UnLoad(input_text_texture);
		input_text_texture = App->font->Print(newString, font_colour, font);
	}

	current_texture = input_text_texture;
}