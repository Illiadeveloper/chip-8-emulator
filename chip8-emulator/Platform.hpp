#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

#pragma once

class Platform {
public:
	Platform(const char* title, unsigned int windowWidth, unsigned int windowHeight, int videoScale, int textureWidth, int textureHeight);

	void Update(void const* buffer);

	bool ProcessInput(uint8_t* key);
private:
	sf::RenderWindow window;
	GLuint framebuffer_texture;
	sf::Texture texture;
	int Scale;
};