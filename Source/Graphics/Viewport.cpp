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
    , m_isDragging(false)
    , m_lastMousePos(0.f, 0.f)
    , m_viewportX(0.f)
    , m_viewportY(0.f)
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

    m_view.setCenter(
        static_cast<float>(width) / 2.f,
        static_cast<float>(height) / 2.f
    );
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
void Viewport::ProcessEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseWheelScrolled)
    {
        sf::Vector2f halfSize = static_cast<sf::Vector2f>
            (m_texture.getSize()) / 2.f;
        sf::Vector2f mousePos = static_cast<sf::Vector2f>
            (sf::Mouse::getPosition()) -
            sf::Vector2f(m_viewportX, m_viewportY) - halfSize;
        sf::Vector2f oldWorldPos = m_texture.mapPixelToCoords(
            static_cast<sf::Vector2i>(mousePos), m_view
        );

        if (event.mouseWheelScroll.delta > 0)
        {
            Zoom(1 / 1.1f);
        }
        else if (event.mouseWheelScroll.delta < 0)
        {
            Zoom(1.1f);
        }

        sf::Vector2f newWorldPos = m_texture.mapPixelToCoords(
            static_cast<sf::Vector2i>(mousePos), m_view
        );
        m_view.move(oldWorldPos - newWorldPos);
        m_texture.setView(m_view);
    }
    else if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
            m_isDragging = true;
            m_lastMousePos = static_cast<sf::Vector2f>(
                sf::Mouse::getPosition()
            );
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
            m_isDragging = false;
        }
    }
    else if (event.type == sf::Event::MouseMoved)
    {
        if (m_isDragging)
        {
            sf::Vector2f newMousePos = static_cast<
                sf::Vector2f>(sf::Mouse::getPosition()
            );
            sf::Vector2f delta = m_lastMousePos - newMousePos;

            m_view.move(delta * m_zoom);

            GameState& gs = GameState::GetInstance();
            auto [mapWidth, mapHeight] = gs.GetDimensions();
            sf::Vector2f viewCenter = m_view.getCenter();

            float gridWidth = mapWidth * m_tileSize;
            float gridHeight = mapHeight * m_tileSize;

            float minX = -gridWidth * 0.5f;
            float maxX = gridWidth * 1.5f;
            float minY = -gridHeight * 0.5f;
            float maxY = gridHeight * 1.5f;

            viewCenter.x = std::clamp(viewCenter.x, minX, maxX);
            viewCenter.y = std::clamp(viewCenter.y, minY, maxY);

            m_view.setCenter(viewCenter);
            m_texture.setView(m_view);

            m_lastMousePos = newMousePos;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::Zoom(float factor)
{
    if (m_zoom * factor < MIN_ZOOM || m_zoom * factor > MAX_ZOOM)
    {
        return;
    }

    m_zoom *= factor;
    m_view.zoom(factor);
    m_texture.setView(m_view);
    Render();
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::SetViewportPosition(float x, float y)
{
    m_viewportX = x;
    m_viewportY = y;
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

    float offsetX = (size.x - (width * m_tileSize)) / 2.f;
    float offsetY = (size.y - (height * m_tileSize)) / 2.f;

    sf::RectangleShape tile(sf::Vector2f(m_tileSize - 1.f, m_tileSize - 1.f));

    tile.setFillColor(sf::Color::Transparent);
    tile.setOutlineThickness(1.f);
    tile.setOutlineColor(sf::Color(80, 80, 80));

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            float posX = static_cast<float>(x) * m_tileSize + offsetX;
            float posY = static_cast<float>(y) * m_tileSize + offsetY;

            tile.setPosition(posX, posY);
            m_texture.draw(tile);
        }
    }
}

} // !namespace Zappy
