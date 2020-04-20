#ifndef __UI_SCROLLBAR_H__
#define __UI_SCROLLBAR_H__

#include "UI.h"

class UI_Image;

class UI_Scrollbar : public UI
{
public:
	UI_Scrollbar(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, SDL_Rect thumbSize, iPoint thumbOffset, SDL_Rect dragArea, float dragFactor, bool dragXAxis = false, bool dragYAxis = true,
		bool invertedScrolling = false, bool isVisible = true, bool isInteractible = false, bool isDraggable = false, Module* listener = nullptr, UI* parent = nullptr,
		SDL_Rect* scrollMask = nullptr, iPoint maskOffset = iPoint(0, 0), bool emptyElements = false);

	bool Draw();

	void CheckInput();

public:
	void DrawScrollbarElements();
	void LinkScroll(UI* element);
	void UpdateLinkedElements();
	bool LinkedElementsBeingHovered();
	bool MouseWithinDragArea();
	float GetDragFactor(UI* element);

	bool GetDragXAxis() const;
	bool GetDragYAxis() const;
	iPoint GetThumbLocalPos();
	SDL_Rect GetThumbHitbox();
	void SetThumbHitbox(SDL_Rect hitbox);

	void PlaceThumbOnMousePos();
	void CheckKeyboardInputs();
	void DragThumbWithMousewheel();
	
	bool ThumbIsWithinVerticalScrollbarBounds();
	bool ThumbIsAtUpperBound();
	bool ThumbIsAtLowerBound();
	bool ThumbIsWithinHorizontalScrollbarBounds();
	bool ThumbIsAtLeftBound();
	bool ThumbIsAtRightBound();
	void CheckScrollbarBounds();

private:
	UI_Image* bar;
	UI_Image* thumb;
	UI_Image* scrollMask;
	
	int scrollbarWidth;
	int scrollbarHeight;

	SDL_Rect	dragArea;
	float		dragFactor;
	iPoint		dragDisplacement;
	iPoint		mouseWheelScroll;
	bool		invertedScrolling;
	float		arrowPosFactor;
	iPoint		newThumbPos;

	std::vector<UI*> linkedElements;
};
#endif // !__UI_SCROLLBAR_H__