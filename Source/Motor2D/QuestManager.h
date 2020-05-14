#include "Module.h"
#include "Point.h"
#include <vector>
#include <list>
#include "SDL/include/SDL_rect.h"


enum class EVENT_TYPE
{
	NONE = 0,
	KILL_EVENT,
	GATHER_EVENT,
	INTERACT_EVENT
};

class Event
{

public:

	Event(EVENT_TYPE type);
	~Event();

	EVENT_TYPE type = EVENT_TYPE::NONE;
	bool trigger = false;
};

class KillEvent : public Event
{

public:

	KillEvent():Event(EVENT_TYPE::KILL_EVENT){}
	~KillEvent(){}

	int count = 0;
};


class Quest
{
public:

	Quest() {};
	~Quest();

	int id = 0;
	int trigger = 0;	//whether or not a quest is active
	int reward = 0;
	Event* requisites = nullptr;

	std::string title;
	std::string description;

	bool completed = false;
};


class QuestManager : public Module
{
public:

	QuestManager();
	~QuestManager();

	bool Awake(pugi::xml_node& file);
	bool Start();

	pugi::xml_node LoadQuests(pugi::xml_document& quest_file) const;
	Event* createEvent(pugi::xml_node& xmlReader);

	pugi::xml_document quest_data;

	std::list<Quest*> loaded_quests;
	std::list<Quest*> active_quests;
	std::list<Quest*> finished_quests;
};


