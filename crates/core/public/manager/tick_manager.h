#pragma once


#include "avalanche_core_export.h"
#include <cstdint>
#include <cstddef>


namespace avalanche::core {

    using tick_group_t = float;

    namespace TickGroup {
        // Before physics update
        constexpr tick_group_t PrePhysics = 0.f;
        // Launch physics
        constexpr tick_group_t StartPhysics = 1.f;
        // Synchronizing physics update
        constexpr tick_group_t DuringPhysics = 2.f;
        // Physics update ended
        constexpr tick_group_t EndPhysics = 3.f;
        // All physics jobs has been done. Updating other states here.
        constexpr tick_group_t PostPhysics = 4.f;
        // Post update
        constexpr tick_group_t PostUpdate = 5.f;
        // Before rendering
        constexpr tick_group_t PreRendering = 6.f;
        // Collecting rendering resource
        constexpr tick_group_t CollectRenderingResource = 7.f;
        // Tick newly spawned
        constexpr tick_group_t NewlySpawned = 8.f;
    } // namespace TickGroup

    /**
     * @brief The tickable interface
     */
    class AVALANCHE_CORE_API ITickable {
    public:
        using duration_type = float;

        virtual ~ITickable() = default;

        /**
         * @brief Tick function body
         * @param delta_time duration time in millisecond
         */
        virtual void tick(duration_type delta_time) = 0;
    };

    class AVALANCHE_CORE_API ITickManager {
    public:
        virtual ~ITickManager() = default;

        virtual void register_tickable(ITickable* tickable, tick_group_t group) = 0;

        virtual void unregister_tickable(ITickable* tickable) = 0;

        virtual bool tick_frame() = 0;

        /**
         * @brief Shutdown down the event loop
         */
        virtual void shutdown() = 0;

        static ITickManager& get();
    };

    template <typename Self, tick_group_t TG = TickGroup::PostUpdate>
    class TickableCRTPBase : public ITickable {
    public:
        static constexpr tick_group_t tick_group = TG;

        using self_type = Self;

        TickableCRTPBase() {
            ITickManager::get().register_tickable(this, TG);
        }

        ~TickableCRTPBase() override {
            ITickManager::get().unregister_tickable(this);
        }

    };

} // namespace avalanche::core
