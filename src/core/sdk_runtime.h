#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>
#include "hook/hook_manager.h"
#include "hook/hook_registry.h"
#include "script/script.h"
#include "event/event_bus.h"


namespace GTASA {
	namespace SDK {

		class SDKRuntime {

		public:
			static SDKRuntime& instance();

			void init();
			void shutdown();

			SDKRuntime(const SDKRuntime&) = delete;
			SDKRuntime& operator=(const SDKRuntime&) = delete;

		private:
			SDKRuntime();
			~SDKRuntime();

		private:	
			bool m_initialized;
			std::mutex m_mutex;
		};


		namespace Events {
			class InitializeEvent : public BaseEvent {
				public:
				InitializeEvent() {}
				virtual const char* getName() const { return "InitializeEvent"; }
			};

			class ShutdownEvent : public BaseEvent {
				public:
				ShutdownEvent() {}
				virtual const char* getName() const { return "ShutdownEvent"; }
			};
		}
	} // namespace SDK
} // namespace GTASA