#ifndef __MAP_H__
#define __MAP_H__

#include "Log.h"					// Added due to ~Properties (#57)

#include "Module.h"
#include "Point.h"
#include "TileQuadTree.h"

class Entity;

// Collider Types -------------------------
enum Object_Type					//Care with enum class. If enum class-> Object_Type::HAZARD and not just HAZARD (i.e)
{
	ENEMY_BARRACKS,
	BARRACKS,
	ENEMY,
	INFANTRY,
	GATHERER,
	HALL,
	ENEMY_HALL,
	ROCK,
	TREE,
	OBELISK,
	NONE,
	UNKNOWN = -1
};

// Properties and Properties methods of a layer -------------------------
union values																//A union makes the elements of a struct share the same memory space (int = 1 0 0 0 0 0 0 0, float = 1 0 0 0 0 0 0 0 ...). It is really efficient.
{
	const char* un_string;													//Union string.
	int			un_int;														//Union int.
	float		un_float;													//Union float.
};

//Information / Data members of the properties of a layer.
struct Properties
{
	struct Property															//Property struct that will hold the data members of the properties of a layer.
	{
		Property()
		{
			intValue = 0;
		}

		std::string	name;													//Name of the property in a layer.
		//values		value;													//Value of the property in a layer.
		int			intValue;												//Int value of the property in a layer. Used mainly for pathfinding
	};

	Properties::~Properties()															//Deletes every property and frees all allocated memory.
	{
		LOG("The Properties' destructor has been called");
		LOG("property_list has %d elements", property_list.size());

		std::vector<Property*>::iterator item = property_list.begin();

		for (; item != property_list.end(); ++item)
		{
			RELEASE((*item));										//Deletes all data members of a property and frees all allocated memory.
		}
		property_list.clear();												//Clears poperty_list by deleting all items in the list and freeing all allocated memory.
	}

	//values default_values = { 0 };
	
	int Get(std::string name, int default_value = 0);						//Will get a specified property's data members. //This version will be used exclusively for pathfinding. (Draw / Nodraw)
	//Changed to non const because of list unknown problem

	std::vector<Property*>	property_list;
};

//Information of a specific object in the map.
struct ObjectData
{
	uint				id;							//Object's id.
	std::string			name;						//Object's name.
	Object_Type			type;						//Type of collider associated with the object.
	SDL_Rect*			collider;					//Rectangle that represents the object. As x, y, w and h are object properties, they can be grouped in a SDL_Rect.
	float				rotation;					//Rotation of the object in degrees clockwise.
	SDL_Texture*		texture;					//Visible image of the object.
};

//Object layer that has all the objects in the same "plane".
struct ObjectGroup
{
	uint				id;							//ObjectGroup layer id.
	std::string			name;						//ObjectGroup's name.
	ObjectData*			object;						//Individual info of each object in the ObjectGroup layer.
	std::string			type;						//ObjectGroup's type. It's an arbitrary string added in Tiled to the Object/ObjectGroup.
	uint				num_objects;				//Quantity of objects. Treure per obj def.
};

// Map Layer data members --------------------------------
struct MapLayer
{
	std::string			name;			//Map name.
	uchar*				gid;			//Tile Id.					// Mind that uchar only allows up to 255 ID's.
	uint				width;			//Layer width in tiles.
	uint				height;			//Layer height in tiles.
	uint				size;			//width * height.
	float				speed;			//Parallax speed.
	Properties			properties;		//Properties of a layer.

	MapLayer();
	~MapLayer();

	//Get id of tile in position x, y from gid[] array
	inline uchar Get(uint x, uint y) const 
	{
		return gid[(y * width) + x]; 
		//return x + y * width;
	}
};

// Tileset Data Members and methods ----------------------------------------------------
struct TileSet
{	
	//This method calculates the position of each tile when given a tile id. 
	SDL_Rect GetTileRect(uint tile_id) const;

	std::string			name;						//Tileset name.
	int					firstgid;					//First global tile id. Maps to the first id in the tileset.		// THIS Change to uchar?
	int					tile_width;					//Maximum width of tiles in a given tileset.
	int					tile_height;				//Maximum height of tiles in a given tilesset.
	int					spacing;					//Space in pixels between the tiles in a given tileset.
	int					margin;						//Margin around the tiles in a given tileset.
	SDL_Texture*		texture;					//Image that will be embedded on the tileset.
	int					tex_width;					//Image width in pixels.
	int					tex_height;					//Image height in pixels.
	int					num_tiles_width;			//Number of tiles at the X axis that will have a given texture. Ex: num_tiles_width = tile_width / tex_width; 
	int					num_tiles_height;			//Number of tiles at the Y axis that will have a given texture. Ex: num_tiles_height = tile_height / tex_height;
	int					offset_x;					//Horizontal offset in pixels.
	int					offset_y;					//Vertical offset in pixels.
};

// Different supported MapTypes ----------------------------------------------------
enum class MAP_TYPE
{
	UNKNOWN = 0,
	ORTHOGONAL,
	ISOMETRIC,
	STAGGERED,
	HEXAGONAL
};

// ----------------------------------------------------
struct MapData
{
	int							width;				//Map width in tiles.
	int							height;				//Map height in tiles.
	int							tile_width;			//Tile width in pixels.
	int							tile_height;		//Tile height in pixels.
	SDL_Color					background_color;	//Background colours.
	MAP_TYPE					type;				//Type of map (Orthogonal, Isometric, Staggered or Hexagonal)
	std::vector<TileSet*>		tilesets;			//List that accesses all tilesets and their data members/properties.
	std::vector<MapLayer*>		layers;				//List that accesses all layers and their data members/properties.
	std::vector<ObjectGroup*>	objectGroups;		//List that accesses all object groups and their data members/properties.

	std::string music_File;
};

// ----------------------------------------------------
class Map : public Module
{
public:

	Map();
	virtual ~Map();																	// Destructor
	
	bool Awake(pugi::xml_node& conf);												// Called before render is available
	void Draw();																	// Called each loop iteration
	bool CleanUp();																	// Called before quitting

	bool Load(std::string path);													// Load new map

public:
	iPoint MapToWorld(int x, int y) const;											//This method translates the position of the tile on the map to its equivalent position on screen.
	iPoint WorldToMap(int x, int y) const;											//This method translates the position of the tile on the screen to its equivalent position on the map.
	iPoint TiledIsoCoordsToMap(int x, int y) const;
	
	bool CheckMapBoundaries(const iPoint& tile_position);							//This method will check whether or not a given tile position is within the map's bounds.
	bool HasMapTile(const iPoint& tile_position);									//Will check the gid of every layer to check whether or not the given tile position has a drawn tile.

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer);				//Changed to non const because of list unknown problem
	bool CreateEntityMap(int& width, int& height);									//This method will allocate the necessary memory and initialize the entities map. Not need for it to be here.
	bool CreateVisibilityMap(int& width, int& height, uchar** buffer);

	void DataMapDebug();
	void Restart_Cam();
	void GetMapSize(int& w, int& h) const;

	void DrawMapGrid();

	TileSet* GetTilesetFromTileId(int id);											//Changed to non const because of list unknown problem

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadObjectLayers(pugi::xml_node& node, ObjectGroup* group);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	
	void LoadMetaDataMaps();												// Will load the walkability, entity and visibility maps.

public:

	MapData				data;
	Point<float>		spawn_position_cam;

	int					window_width;					// Declared to store the window's width.
	int					window_height;					// Declared to store the window's height.
														   
	iPoint				cam_tilePos;					// Position of the camera in the X and Y axis in tiles.
	int					tile_index;						// Will store the tile's index number so the correct tile is loaded.
														   
	bool				pathfinding_meta_debug;			// Keeps track of whether to load the PathfindingCollisions layer. Temporally declared here.
	bool				walkability_debug;				// Will track whether or not the walkability map debug is active or not.
	bool				entity_map_debug;				// Will track whether or not the entity map debug is active or not.

	bool				map_loaded;

	std::vector<Entity*> tutorial_boulders;

public:													// Camera Culling Variables
	bool				smaller_camera;
	
	iPoint				camera_pos_in_pixels;

	int					bottom_right_x;
	int					bottom_right_y;

	int					min_x_row;
	int					max_x_row;

	int					min_y_row;						// Top right corner
	int					max_y_row;						// Bottom left corner

private:
	pugi::xml_document	map_file;
	std::string			folder;
};

#endif // __MAP_H__