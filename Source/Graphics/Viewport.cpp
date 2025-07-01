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

    m_texture.display();
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::Resize(unsigned int width, unsigned int height)
{
    sf::Vector2u size = m_texture.getSize();

    if (size.x == width && size.y == height)
    {
        return;
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

    RenderGrid();
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

    if (!m_grid.create(size.x, size.y))
    {
        return;
    }

    GameState& gs = GameState::GetInstance();
    auto [width, height] = gs.GetDimensions();

    m_grid.clear(sf::Color::Transparent);

    m_grid.display();
}

} // !namespace Zappy
