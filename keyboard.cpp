#include "keyboard.h"

void KeyboardListener::PressKey(unsigned char key)    
{ 
	if (!keys[key]) {
		keys[key] = true;
		fresh_keys[key] = true;
	}
}

void KeyboardListener::ReleaseKey(unsigned char key)  
{ 
	keys[key] = false;
	fresh_keys[key] = false;
}

bool KeyboardListener::NewlyPressed(unsigned char key) const
{
	return fresh_keys[key];
}

void KeyboardListener::ClearNewKeys()
{
	memset(fresh_keys, 0, sizeof(fresh_keys));
}
