#include "event_bus.hpp"

namespace ii {
    void EventBus::publish(const Event& event) {
        std::lock_guard<std::mutex> lock(mutex);
        event_queue.push(event);
    }

    void EventBus::subscribe(const Event::Type& type, const Callback& callback) {
        std::lock_guard<std::mutex> lock(mutex);
        callbacks[type].push_back(callback);
    }

    void EventBus::process_events() {
        std::lock_guard<std::mutex> lock(mutex);
        while (!event_queue.empty()) {
            Event event = event_queue.front();
            event_queue.pop();

            auto it = callbacks.find(event.type);
            if (it != callbacks.end()) {
                for (const auto& callback : it->second) {
                    callback(event);
                }
            }
        }
    }
}