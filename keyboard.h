#pragma once

#include "defines.h"

class KeyboardListener
{
	bool keys[256];
	bool fresh_keys[256];
public:
	void PressKey(unsigned char key);
	void ReleaseKey(unsigned char key);
	const bool& operator[] (unsigned char key) const { return keys[key]; }
	bool NewlyPressed(unsigned char key) const;
	void ClearNewKeys();
};
