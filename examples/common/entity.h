#pragma once

#include <entt/entt.hpp>

extern entt::registry s_registry;

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle) : handle_(handle) {}
    Entity(const Entity& other) = default;

    void Create() {
        handle_ = s_registry.create();
    }

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        T& component = s_registry.emplace<T>(handle_, std::forward<Args>(args)...);
        return component;
    }

    template<typename T>
    T& GetComponent()
    {
        return s_registry.get<T>(handle_);
    }

    template<typename T>
    bool HasComponent()
    {
        return s_registry.any_of<T>(handle_);
    }

    template<typename T>
    void RemoveComponent()
    {
        s_registry.remove<T>(handle_);
    }

    operator bool() const { return handle_ != entt::null; }
    operator entt::entity() const { return handle_; }
    operator uint32_t() const { return (uint32_t)handle_; }

    bool operator==(const Entity& other) const
    {
        return handle_ == other.handle_;
    }

    bool operator!=(const Entity& other) const
    {
        return !(*this == other);
    }

    entt::entity handle_{ entt::null };
};
