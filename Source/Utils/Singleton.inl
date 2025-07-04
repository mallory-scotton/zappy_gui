///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::unique_ptr<T> Singleton<T>::m_instance;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::mutex Singleton<T>::m_mutex;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T& Singleton<T>::GetInstance(void)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_instance)
    {
        m_instance = std::make_unique<T>();
    }
    return (*m_instance);
}

} // namespace Zappy
