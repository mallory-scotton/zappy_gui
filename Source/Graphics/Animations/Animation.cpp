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
    m_rectangle.setSize(sf::Vector2f(0.f, 0.f));
    m_rectangle.setOrigin(0.f, 0.f);
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

    m_duration -= deltaTime;
    if (m_duration <= 0.f)
    {
        m_isFinished = true;
        return;
    }

    float progress = 1.f - (m_duration / m_maxDist);
    float size = m_maxDist * progress;

    if (m_isCircle)
    {
        m_circle.setRadius(size);
        m_circle.setOrigin(size, size);
    }
    else if (m_isRectangle)
    {
        m_rectangle.setSize(sf::Vector2f(size, size));
        m_rectangle.setOrigin(size / 2.f, size / 2.f);
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
        m_circle.setFillColor(color);
    }
    else if (m_isRectangle)
    {
        m_rectangle.setFillColor(color);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Animation::SetPosition(unsigned int x, unsigned int y)
{
    m_x = x;
    m_y = y;

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

} // !namespace Zappy
