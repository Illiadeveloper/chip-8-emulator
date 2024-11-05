#include "Platform.hpp"

Platform::Platform(const char* title, unsigned int windowWidth, unsigned int windowHeight, int videoScale, int textureWidth, int textureHeight)
    : window(sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight), title, sf::Style::Titlebar | sf::Style::Close)),
    texture()
{
    window.setVerticalSyncEnabled(true);

    texture.create(textureWidth, textureHeight);

    this->Scale = videoScale;
}

void Platform::Update(void const* buffer)
{

    texture.update(static_cast<const uint8_t*>(buffer));

    sf::Sprite sprite(texture);
    float scaleFactor = static_cast<float>(Scale);
    sprite.setScale(scaleFactor, scaleFactor);
    
    window.clear();
    window.draw(sprite);
    window.display();
}





bool Platform::ProcessInput(uint8_t* keys)
{
    sf::Event event;
    bool quit = false;

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            quit = true;
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Escape:
                quit = true;
                break;
            case sf::Keyboard::X:
                keys[0] = 1;
                break;
            case sf::Keyboard::Num1:
                keys[1] = 1;
                break;
            case sf::Keyboard::Num2:
                keys[2] = 1;
                break;
            case sf::Keyboard::Num3:
                keys[3] = 1;
                break;
            case sf::Keyboard::Q:
                keys[4] = 1;
                break;
            case sf::Keyboard::W:
                keys[5] = 1;
                break;
            case sf::Keyboard::E:
                keys[6] = 1;
                break;
            case sf::Keyboard::A:
                keys[7] = 1;
                break;
            case sf::Keyboard::S:
                keys[8] = 1;
                break;
            case sf::Keyboard::D:
                keys[9] = 1;
                break;
            case sf::Keyboard::Z:
                keys[0xA] = 1;
                break;
            case sf::Keyboard::C:
                keys[0xB] = 1;
                break;
            case sf::Keyboard::Num4:
                keys[0xC] = 1;
                break;
            case sf::Keyboard::R:
                keys[0xD] = 1;
                break;
            case sf::Keyboard::F:
                keys[0xE] = 1;
                break;
            case sf::Keyboard::V:
                keys[0xF] = 1;
                break;
            }
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::X:
                keys[0] = 0;
                break;
            case sf::Keyboard::Num1:
                keys[1] = 0;
                break;
            case sf::Keyboard::Num2:
                keys[2] = 0;
                break;
            case sf::Keyboard::Num3:
                keys[3] = 0;
                break;
            case sf::Keyboard::Q:
                keys[4] = 0;
                break;
            case sf::Keyboard::W:
                keys[5] = 0;
                break;
            case sf::Keyboard::E:
                keys[6] = 0;
                break;
            case sf::Keyboard::A:
                keys[7] = 0;
                break;
            case sf::Keyboard::S:
                keys[8] = 0;
                break;
            case sf::Keyboard::D:
                keys[9] = 0;
                break;
            case sf::Keyboard::Z:
                keys[0xA] = 0;
                break;
            case sf::Keyboard::C:
                keys[0xB] = 0;
                break;
            case sf::Keyboard::Num4:
                keys[0xC] = 0;
                break;
            case sf::Keyboard::R:
                keys[0xD] = 0;
                break;
            case sf::Keyboard::F:
                keys[0xE] = 0;
                break;
            case sf::Keyboard::V:
                keys[0xF] = 0;
                break;
            }
        }
    }

    return quit;
}
