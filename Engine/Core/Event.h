#pragma once
#include <functional>
#include <list>
#include <algorithm>

template<typename... ArgTypes>
class MulticastEvent {
public:
	struct Listener {
		Listener() = default;
		~Listener() {
			//Disconnect();
		}

		MulticastEvent<ArgTypes...>* event = nullptr;
		std::function<void(ArgTypes...)> callback = nullptr;
		bool isBound = false;
		bool isOnce = false;

		void Disconnect() {
			if (event) event->Disconnect(this);
		}
	};

	~MulticastEvent() {
		isDestructing = true;
		listeners.clear();
		pending.clear();
	}

	Listener& Connect(std::function<void(ArgTypes...)> callback) {
		Listener listener;
		listener.event = this;
		listener.callback = callback;
		listener.isBound = true;
		listener.isOnce = false;

		if (!isFiring) {
			listeners.push_back(listener);
			return listeners.back();
		} else {
			pending.push_back(listener);
			return pending.back();
		}
	}

	Listener& Once(std::function<void(ArgTypes...)> callback) {
		Listener listener;
		listener.event = this;
		listener.callback = callback;
		listener.isBound = true;
		listener.isOnce = true;

		if (!isFiring) {
			listeners.push_back(listener);
			return listeners.back();
		} else {
			pending.push_back(listener);
			return pending.back();
		}
	}

	void Disconnect(Listener* listener) {
		if (!listener) return;
		listener->isBound = false;
		if (isFiring) {
			//do not alter the list while firing the event.
			//simply mark it as unbound.
			return;
		}
		cleanupUnbound();
	}

	void Fire(ArgTypes... args) {
		isFiring = true;
		for (auto& listener : listeners) {
			if (listener.isBound) {
				listener.callback(args...);
			}
			if (listener.isOnce) {
				listener.isBound = false;
			}
		}
		isFiring = false;

		cleanupPending();
		cleanupUnbound();
	}

	inline bool HasAnyListeners() const { return !listeners.empty(); }
private:
	std::list<Listener> listeners;
	std::list<Listener> pending;
	bool isFiring = false;
	bool isDestructing = false;

	void cleanupPending() {
		if (isFiring) return;
		listeners.splice(listeners.end(), pending);
	}
	void cleanupUnbound() {
		if (isFiring) return;
		listeners.remove_if([](const Listener& l) { return !l.isBound; });
	}
};

template<>
class MulticastEvent<void> : public MulticastEvent<> {};
