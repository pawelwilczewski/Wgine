#pragma once

#include "Wgine/Core/Core.h"
#include "Wgine/Event/Event.h"

#include <string>

namespace Wgine
{
	class WGINE_API Layer
	{
	public:
		Layer(const std::string &name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(const float &deltaSeconds) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event &event) {}

		inline const std::string &GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}
