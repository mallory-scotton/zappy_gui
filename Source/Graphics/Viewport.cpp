///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Graphics/Viewport.hpp"
#include "Game/GameState.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{

///////////////////////////////////////////////////////////////////////////////
Viewport::Viewport(void)
    : m_view(sf::FloatRect(0.f, 0.f, DEFAULT_WIDTH, DEFAULT_HEIGHT))
    , m_texture()
    , m_zoom(DEFAULT_ZOOM)
    , m_tileSize(0.f)
{
    Resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int Viewport::GetTextureID(void) const
{
    return (m_texture.getTexture().getNativeHandle());
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::Render(void)
{
    m_texture.clear(sf::Color(20, 20, 20));

    RenderGrid();

    m_texture.display();
}

///////////////////////////////////////////////////////////////////////////////
bool Viewport::Resize(unsigned int width, unsigned int height)
{
    sf::Vector2u size = m_texture.getSize();

    if (size.x == width && size.y == height)
    {
        return (false);
    }

    if (width == 0 || height == 0)
    {
        width = DEFAULT_WIDTH;
        height = DEFAULT_HEIGHT;
    }

    m_view.setSize(
        static_cast<float>(width),
        static_cast<float>(height)
    );
    m_texture.create(width, height);
    m_texture.setView(m_view);

    Render();

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
sf::Vector2u Viewport::GetSize(void) const
{
    return (m_texture.getSize());
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::RenderGrid(void)
{
    sf::Vector2u size = m_texture.getSize();

    GameState& gs = GameState::GetInstance();
    auto [width, height] = gs.GetDimensions();

    float tileWidth = static_cast<float>(size.x) / width;
    float tileHeight = static_cast<float>(size.y) / height;
    m_tileSize = std::min(tileWidth, tileHeight);

    sf::RectangleShape tile(sf::Vector2f(m_tileSize - 2.f, m_tileSize - 2.f));

    tile.setFillColor(sf::Color::Transparent);
    tile.setOutlineThickness(1.f);
    tile.setOutlineColor(sf::Color(80, 80, 80));

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            float posX = static_cast<float>(x) * m_tileSize;
            float posY = static_cast<float>(y) * m_tileSize;

            tile.setPosition(posX, posY);
            m_texture.draw(tile);
        }
    }
}

} // !namespace Zappy
