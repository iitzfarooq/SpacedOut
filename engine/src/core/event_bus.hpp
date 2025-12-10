#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <functional>

#include "utils/types.hpp"

namespace ii {
    struct Event {
        enum class Type {};

        Type type;

        explicit inline Event(Event::Type type) : type(type) {}
    };

    using EventTypeHasher = EnumClassHasher<Event::Type>;

    class EventBus {
    public:
        using Callback = std::function<bool(Event&)>;

        EventBus() = default;
        ~EventBus() = default;

        void publish(const Event& event);
        void subscribe(const Event::Type& type, const Callback& callback);
        void process_events();

    private:
        std::mutex mutex;
        std::queue<Event> event_queue;
        std::unordered_map<
            Event::Type,
            std::vector<Callback>,
            EventTypeHasher
        > callbacks;
    };

}
