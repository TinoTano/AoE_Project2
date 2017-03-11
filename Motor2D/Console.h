#ifndef _CONSOLE_H
#define _CONSOLE_H_

#include "Module.h"
#include "Fonts.h"
#include "Gui.h"
#include <list>
using namespace std;

class Console : public Module
{
public:
	Console();

	// Destructor
	virtual ~Console();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Load and save before
	bool Save(pugi::xml_node&) const;

public:
	bool				show = false;
	uint				window_w, window_h;
	list<string>		ctext;
	list<UIElement*>	console_elements;
	Quad*				cbackground;
	Quad*				cinput;
	InputText*			ctextinput;

private:
	void				SetConsoleText();

};




#endif // CONSOLE_H
