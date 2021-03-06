#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "Scene.h"

class GuiElement;
class GuiImage;
class GuiText;
class GuiButton;
class GuiInputBox;
class GuiScrollbar;

class LogoScene : public Scene 
{
public:
	LogoScene();
	~LogoScene();										// Destructor

	bool Awake(pugi::xml_node&);						// Called before render is available
	bool Start();										// Called before the first frame
	bool PreUpdate();									// Called before all Updates
	bool Update(float dt);								// Called each loop iteration
	bool PostUpdate();									// Called before all Updates
	bool CleanUp();										// Called before quitting

public:
	void InitScene();

	void LoadGuiElements();

	void OnEventCall(GuiElement* element, GUI_EVENT ui_event);

	void ExecuteTransition();

public:
	//BACKGROUND
	SDL_Rect			background_rect;
	SDL_Texture*		background_texture;
	SDL_Rect			logo_rect;
	SDL_Texture*		logo_texture;

private:
	float				logo_scene_duration;
	float				accumulated_time;
	
	//Audio
	uint				logo_fx;
	uint				logo_channel;

};
#endif // !_LOGO_SCENE_H
