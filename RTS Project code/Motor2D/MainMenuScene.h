#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "Scene.h"

class UI;
class UI_Image;
class UI_Text;
class UI_Button;
class UI_InputBox;
class UI_Scrollbar;

class MainMenuScene : public Scene
{
public:

	MainMenuScene();
	virtual ~MainMenuScene();			// Destructor

	bool Awake(pugi::xml_node&);		// Called before render is available
	bool Start();						// Called before the first frame
	bool PreUpdate();					// Called before all Updates
	bool Update(float dt);				// Called each loop iteration
	bool PostUpdate();					// Called before all Updates
	bool CleanUp();						// Called before quitting

public:
	void InitScene();

	void LoadGuiElements();

	void OnEventCall(UI* element, UI_EVENT ui_event);

	void ExecuteTransition();

public:
	//BACKGROUND
	SDL_Rect		background_rect;
	SDL_Texture*	background_texture;

	// Main Screen
	UI_Image*		main_parent;
	UI_Button*		new_game_button;
	UI_Button*		continue_button;
	UI_Button*		options_button;
	UI_Button*		exit_button;

	//Audio
	uint				menu_song;

};

#endif // !__MAIN_SCENE_H__