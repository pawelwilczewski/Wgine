#include "WginePCH.h"
#include "Application.h"

#include "Input.h"
#include "Renderer/Buffer.h"

#include "Wgine/Renderer/Renderer.h"
#include "Wgine/Renderer/RenderCommand.h"

#include "Wgine/Camera.h"

#include "Wgine/KeyCodes.h"
#include "Wgine/Core/Time.h"

namespace Wgine {

	Application *Application::s_Instance = nullptr;

	Wgine::Application::Application()
		: m_Window(Window::Create())
	{
		WGINE_CORE_ASSERT(s_Instance == nullptr, "Attempting to create multiple applications!");
		s_Instance = this;
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_Window->SetEventCallback(WGINE_BIND_EVENT_FN(Application::OnEvent));

		//m_Window->SetVSync(false);
	}

	Wgine::Application::~Application()
	{
	}

	void Wgine::Application::Run()
	{
		Time::Init();
		while (m_Running)
		{
			Time::FrameBegin();

			for (auto layer : m_LayerStack)
				layer->OnUpdate(Time::GetDeltaSeconds());

			// ImGui rendering
			m_ImGuiLayer->Begin();
			for (Layer *layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event &e)
	{
		EventDispatcher d(e);
		d.Dispatch<WindowCloseEvent>(WGINE_BIND_EVENT_FN(Application::OnWindowClosed));

		for (auto i = m_LayerStack.end(); i != m_LayerStack.begin(); )
		{
			(*--i)->OnEvent(e);
			if (e.WasHandled())
				break;
		}
	}

	void Application::PushLayer(Layer *layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer *overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	bool Application::OnWindowClosed(WindowCloseEvent &e)
	{
		m_Running = false;

		return true;
	}
}
