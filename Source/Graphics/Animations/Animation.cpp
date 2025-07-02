///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Animation.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{
///////////////////////////////////////////////////////////////////////////////
Animation::Animation(unsigned int x, unsigned int y, float maxDist, float duration)
    : m_x(x), m_y(y), m_maxDist(maxDist), m_duration(duration),
      m_isFinished(false), m_isCircle(false), m_isRectangle(false)
{
    m_circle.setRadius(0.f);
    m_circle.setOrigin(0.f, 0.f);
    m_circle.setFillColor(sf::Color::Transparent);
    m_circle.setOutlineThickness(2.f);
    m_rectangle.setSize(sf::Vector2f(0.f, 0.f));
    m_rectangle.setOrigin(0.f, 0.f);
    m_rectangle.setFillColor(sf::Color::Transparent);
    m_rectangle.setOutlineThickness(1.f);
}

///////////////////////////////////////////////////////////////////////////////
void Animation::SetCircle(void)
{
    m_isCircle = true;
    m_isRectangle = false;
    m_circle.setRadius(m_maxDist);
    m_circle.setOrigin(m_maxDist, m_maxDist);
}

///////////////////////////////////////////////////////////////////////////////
void Animation::SetRectangle(void)
{
    m_isCircle = false;
    m_isRectangle = true;
    m_rectangle.setSize(sf::Vector2f(m_maxDist, m_maxDist));
    m_rectangle.setOrigin(m_maxDist / 2.f, m_maxDist / 2.f);
}

///////////////////////////////////////////////////////////////////////////////
void Animation::Update(float deltaTime)
{
    if (m_isFinished)
        return;

    m_currentTime += deltaTime;
    if (m_currentTime >= m_duration)
    {
        m_isFinished = true;
        return;
    }

    float progress = m_currentTime / m_duration;
    float size = m_maxDist * progress;

    sf::Color outLineColor;
    int alpha = static_cast<int>(255 * (1.0f - progress));
    if (m_isCircle)
    {
        m_circle.setRadius(size);
        m_circle.setOrigin(size, size);
        outLineColor = m_circle.getOutlineColor();
        outLineColor.a = alpha;
        m_circle.setOutlineColor(outLineColor);
    }
    else if (m_isRectangle)
    {
        m_rectangle.setSize(sf::Vector2f(size, size));
        m_rectangle.setOrigin(size / 2.f, size / 2.f);
        outLineColor = m_rectangle.getOutlineColor();
        outLineColor.a = alpha;
        m_rectangle.setOutlineColor(outLineColor);
    }

}

///////////////////////////////////////////////////////////////////////////////
void Animation::Render(sf::RenderTarget& target)
{
    if (m_isCircle)
    {
        m_circle.setPosition(m_x, m_y);
        target.draw(m_circle);
    }
    else if (m_isRectangle)
    {
        m_rectangle.setPosition(m_x, m_y);
        target.draw(m_rectangle);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Animation::SetColor(const sf::Color& color)
{
    if (m_isCircle)
    {
        m_circle.setOutlineColor(color);
    }
    else if (m_isRectangle)
    {
        m_rectangle.setOutlineColor(color);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Animation::SetPosition(unsigned int x, unsigned int y)
{
    if (m_isCircle)
    {
        m_circle.setPosition(x, y);
    }
    else if (m_isRectangle)
    {
        m_rectangle.setPosition(x, y);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Animation::IsFinished(void) const
{
    return m_isFinished;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int Animation::GetX(void) const
{
    return m_x;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int Animation::GetY(void) const
{
    return m_y;
}

} // !namespace Zappy
