///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Graphics/Viewport.hpp"
#include "Game/GameState.hpp"
#include "Libraries/imgui.h"
#include <iostream>

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
    , m_isDragging(false)
    , m_lastMousePos(0.f, 0.f)
    , m_viewportX(0.f)
    , m_viewportY(0.f)
    , m_forceRender(false)
    , m_fontLoaded(false)
    , m_renderWinner(false)
    , m_indexX(0)
    , m_indexY(0)
{
    Resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    auto appdir = std::getenv("APPDIR");

    if (appdir && m_font.loadFromFile(
        std::string(appdir) + "/usr/share/Assets/Fonts/Arial.ttf"
    ))
    {
        m_fontLoaded = true;
        m_text.setFont(m_font);
    }
    else if (m_font.loadFromFile("Assets/Fonts/Arial.ttf"))
    {
        m_fontLoaded = true;
        m_text.setFont(m_font);
    }
}

///////////////////////////////////////////////////////////////////////////////
unsigned int Viewport::GetTextureID(void) const
{
    return (m_texture.getTexture().getNativeHandle());
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

    GameState& gs = GameState::GetInstance();

    auto [mapWidth, mapHeight] = gs.GetDimensions();

    float gridWidth = mapWidth * TILE_SIZE;
    float gridHeight = mapHeight * TILE_SIZE;

    float scale = std::max(
        gridWidth / static_cast<float>(width),
        gridHeight / static_cast<float>(height)
    );

    m_view.setCenter(
        static_cast<float>(gridWidth) / 2.f,
        static_cast<float>(gridHeight) / 2.f
    );
    m_view.setSize(
        static_cast<float>(width),
        static_cast<float>(height)
    );
    m_texture.create(width, height);

    m_view.zoom(scale);
    m_texture.setView(m_view);
    m_zoom = DEFAULT_ZOOM;

    m_forceRender = true;

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
        ImVec2 pos = ImGui::GetMousePos();

        if (
            pos.x < m_viewportX ||
            pos.y < m_viewportY ||
            pos.x > m_viewportX + m_texture.getSize().x ||
            pos.y > m_viewportY + m_texture.getSize().y
        )
        {
            return;
        }

        if (event.mouseWheelScroll.delta > 0)
        {
            Zoom(1 / 1.1f);
        }
        else if (event.mouseWheelScroll.delta < 0)
        {
            Zoom(1.1f);
        }

        m_texture.setView(m_view);
        m_forceRender = true;
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
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            sf::Vector2i sfmlMousePos(
                static_cast<int>(mousePos.x - m_viewportX),
                static_cast<int>(mousePos.y - m_viewportY)
            );

            if (sfmlMousePos.x >= 0 && sfmlMousePos.y >= 0 &&
                sfmlMousePos.x < static_cast<int>(m_texture.getSize().x) &&
                sfmlMousePos.y < static_cast<int>(m_texture.getSize().y)) {

                sf::Vector2f worldPos = m_texture.mapPixelToCoords(sfmlMousePos, m_view);

                unsigned int tileX = static_cast<unsigned int>(worldPos.x / TILE_SIZE);
                unsigned int tileY = static_cast<unsigned int>(worldPos.y / TILE_SIZE);

                GameState& gs = GameState::GetInstance();
                auto [mapWidth, mapHeight] = gs.GetDimensions();

                if (tileX < static_cast<unsigned int>(mapWidth) &&
                    tileY < static_cast<unsigned int>(mapHeight)) {
                    m_indexX = tileX;
                    m_indexY = tileY;
                    m_forceRender = true;
                }
            }
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

            float gridWidth = mapWidth * TILE_SIZE;
            float gridHeight = mapHeight * TILE_SIZE;

            float minX = -gridWidth * 0.5f;
            float maxX = gridWidth * 1.5f;
            float minY = -gridHeight * 0.5f;
            float maxY = gridHeight * 1.5f;

            viewCenter.x = std::clamp(viewCenter.x, minX, maxX);
            viewCenter.y = std::clamp(viewCenter.y, minY, maxY);

            m_view.setCenter(viewCenter);
            m_texture.setView(m_view);

            m_lastMousePos = newMousePos;
            m_forceRender = true;
        }
    }
    else if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::Space)
        {
            m_renderWinner = !m_renderWinner;
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
void Viewport::Render(void)
{
    GameState& gs = GameState::GetInstance();

    m_forceRender = false;
    m_texture.setView(m_view);

    m_texture.clear(sf::Color(20, 20, 20));
    RenderGrid();
    RenderPlayers();

    ProcessAnimationEvents();
    UpdateAndRenderAnimations();
    if (gs.HasWin() && m_renderWinner)
    {
        RenderWinner(gs.GetWinner());
        m_activeAnimations.clear();
    }

    m_texture.display();
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::RenderGrid(void)
{
    GameState& gs = GameState::GetInstance();
    auto [width, height] = gs.GetDimensions();

    static constexpr float OUTLINE_THICKNESS = 3.f;

    sf::RectangleShape tile(sf::Vector2f(
        TILE_SIZE - OUTLINE_THICKNESS, TILE_SIZE - OUTLINE_THICKNESS
    ));

    tile.setFillColor(sf::Color::Transparent);
    tile.setOutlineThickness(OUTLINE_THICKNESS);
    tile.setOutlineColor(sf::Color(80, 80, 80));

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            float posX = static_cast<float>(x) * TILE_SIZE;
            float posY = static_cast<float>(y) * TILE_SIZE;

            tile.setPosition(posX, posY);
            m_texture.draw(tile);

            const Inventory& tileInventory = gs.GetTileAt(x, y);

            if (m_fontLoaded) {
                float offsetX = TILE_SIZE * 0.03f;  // 3% of tile size
                float offsetY = TILE_SIZE * 0.03f;  // 3% of tile size
                float lineSpacing = TILE_SIZE * 0.125f;  // 12.5% of tile size
                m_text.setCharacterSize(static_cast<unsigned int>(TILE_SIZE * 0.125f));  // 12.5% of tile size

                auto& resources = GetResources(tileInventory);

                float yPos = posY + offsetY;
                for (const auto& res : resources)
                {
                    m_text.setFillColor(res.color);
                    m_text.setString(std::to_string(res.value));
                    m_text.setPosition(posX + offsetX, yPos);
                    m_texture.draw(m_text);
                    yPos += lineSpacing;
                }
            }
        }
    }
    tile.setPosition(static_cast<float>(m_indexX) * TILE_SIZE, static_cast<float>(m_indexY) * TILE_SIZE);
    tile.setOutlineColor(sf::Color(255, 215, 0));
    tile.setOutlineThickness(OUTLINE_THICKNESS + 1.0f);
    m_texture.draw(tile);
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::RenderPlayers(void)
{
    GameState& gs = GameState::GetInstance();
    auto [width, height] = gs.GetDimensions();
    auto teams = gs.GetTeams();
    std::map<std::string, sf::Color> teamColors;

    for (const auto& team : teams)
    {
        teamColors[team.GetName()] = team.GetColor();
    }

    static constexpr float CIRCLE_RADIUS = TILE_SIZE / 4.f;

    sf::CircleShape circle = sf::CircleShape(CIRCLE_RADIUS);
    sf::ConvexShape triangle(3);

    circle.setOrigin(CIRCLE_RADIUS, CIRCLE_RADIUS);
    circle.setOutlineThickness(0.f);

    triangle.setPoint(0, sf::Vector2f(0.f, -CIRCLE_RADIUS));
    triangle.setPoint(1, sf::Vector2f(-CIRCLE_RADIUS, CIRCLE_RADIUS));
    triangle.setPoint(2, sf::Vector2f(CIRCLE_RADIUS, CIRCLE_RADIUS));
    triangle.setOrigin(0.f, CIRCLE_RADIUS);

    float offset = TILE_SIZE / 2.f - 1.5f;

    std::vector<unsigned int> dirs;

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            float posX = static_cast<float>(x) * TILE_SIZE + offset;
            float posY = static_cast<float>(y) * TILE_SIZE + offset;

            auto players = gs.GetPlayersAt(x, y);

            if (players.empty())
            {
                continue;
            }

            dirs.clear();

            for (const auto player : players)
            {
                if (dirs.size() == 4)
                {
                    break;
                }

                unsigned int orientation = player->GetOrientation();

                auto it = std::find(dirs.begin(), dirs.end(), orientation);
                if (it == dirs.end())
                {
                    triangle.setFillColor(teamColors[player->GetTeam()]);
                    triangle.setPosition(posX, posY);
                    triangle.setRotation(
                        90.f * (static_cast<float>(orientation) - 1.f)
                    );
                    m_texture.draw(triangle);
                    dirs.push_back(orientation);
                }
            }

            const auto top = players.back();
            circle.setPosition(posX, posY);
            circle.setFillColor(teamColors[top->GetTeam()]);
            m_texture.draw(circle);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
std::vector<Viewport::ResourceDisplay>& Viewport::GetResources(const Inventory& inv)
{
    m_resources.clear();

    m_resources.push_back({inv.food, sf::Color(128, 204, 128), 'F'});
    m_resources.push_back({inv.linemate, sf::Color(179, 179, 179), 'L'});  // Gray
    m_resources.push_back({inv.deraumere, sf::Color(0, 128, 75), 'D'});  // Deep emerald green
    m_resources.push_back({inv.sibur, sf::Color(147, 112, 219), 'S'});      // Purple ametgyst
    m_resources.push_back({inv.mendiane, sf::Color(224, 17, 95), 'M'});  // Ruby red
    m_resources.push_back({inv.phiras, sf::Color(135, 206, 235), 'P'});    // Light blue topaz
    m_resources.push_back({inv.thystame, sf::Color(255, 204, 0), 'T'});    // Gold

    return (m_resources);
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::RenderWinner(const Team& team)
{
    GameState& gs = GameState::GetInstance();
    auto [mapWidth, mapHeight] = gs.GetDimensions();
    sf::Vector2u size = m_texture.getSize();

    float gridCenterX = static_cast<float>(mapWidth * TILE_SIZE) / 2.0f;
    float gridCenterY = static_cast<float>(mapHeight * TILE_SIZE) / 2.0f;

    sf::View originalView = m_texture.getView();

    sf::View winnerView = m_view;
    winnerView.setCenter(gridCenterX, gridCenterY);
    m_texture.setView(winnerView);

    sf::RectangleShape background;
    background.setSize(sf::Vector2f(size.x * 0.7f, size.y * 0.3f));
    background.setFillColor(sf::Color(0, 0, 0, 200));
    background.setOrigin(background.getSize().x / 2.0f, background.getSize().y / 2.0f);
    background.setPosition(gridCenterX, gridCenterY);
    background.setOutlineThickness(4.0f);
    background.setOutlineColor(team.GetColor());
    m_texture.draw(background);

    sf::Text text;
    if (m_fontLoaded) {
        text.setFont(m_font);
        text.setString("WINNER: " + team.GetName());
        text.setCharacterSize(42);
        text.setFillColor((team.GetColor()));
        m_text.setStyle(sf::Text::Bold);
        text.setPosition(gridCenterX, gridCenterY);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
        m_texture.draw(text);
    }
    m_texture.setView(originalView);
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::ProcessAnimationEvents(void)
{
    GameState& gs = GameState::GetInstance();

    GameState::ScopedLock lock(gs);

    while (const auto& event = gs.PopAnimation())
    {
        Animation animation(
            event->x * TILE_SIZE + (TILE_SIZE/2),
            event->y * TILE_SIZE + (TILE_SIZE/2),
            TILE_SIZE * 1.5625f, event->duration
        );

        switch (event->type)
        {
            case GameState::AnimationType::Broadcast:
                animation.SetCircle();
                animation.SetColor(event->team.GetColor());
                m_activeAnimations.push_back(animation);
                break;

            case GameState::AnimationType::IncantationStart:
                animation.SetRectangle();
                animation.SetColor(sf::Color::Yellow);
                m_activeAnimations.push_back(animation);
                break;

            case GameState::AnimationType::IncantationSuccess:
                animation.SetRectangle();
                animation.SetColor(sf::Color::Green);
                m_activeAnimations.push_back(animation);
                break;

            case GameState::AnimationType::IncantationFail:
                animation.SetRectangle();
                animation.SetColor(sf::Color::Red);
                m_activeAnimations.push_back(animation);
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Viewport::UpdateAndRenderAnimations(void)
{
    auto it = m_activeAnimations.begin();
    while (it != m_activeAnimations.end())
    {
        float deltaTime = ImGui::GetIO().DeltaTime;
        it->Update(deltaTime);

        if (it->IsFinished())
        {
            it = m_activeAnimations.erase(it);
        }
        else
        {
            it->Render(m_texture);
            ++it;
        }
    }
}

} // !namespace Zappy
