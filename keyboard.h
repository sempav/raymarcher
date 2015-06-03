#pragma once

#include "defines.h"

#include <vector>
#include <GLFW/glfw3.h>

class KeyboardListener
{
    std::vector<bool> keys;
    std::vector<bool> fresh_keys;
public:
    KeyboardListener(): keys(GLFW_KEY_LAST),
                        fresh_keys(GLFW_KEY_LAST)
    { }

	void PressKey(int key) {
        fresh_keys[key] = !keys[key];
        keys[key] = true;
    }
	void ReleaseKey(int key) { 
        keys[key] = false;
        fresh_keys[key] = false;
    }
	bool operator[] (int key)  const { return keys[key]; }
	bool NewlyPressed(int key) const { return fresh_keys[key]; }
	void ClearNewKeys() {
        fresh_keys.assign(fresh_keys.size(), false);
    }
};
