#pragma once

// PLATFORMS
#ifdef WGINE_PLATFORM_WINDOWS
#if WGINE_DYNAMIC_LINK
	#ifdef WGINE_BUILD_DLL
		#define WGINE_API __declspec(dllexport)
	#else
		#define WGINE_API __declspec(dllimport)
	#endif
#else
	#define WGINE_API
#endif
#else
	#error Unsupported platform!
#endif

// RENDERING APIS
// TODO: ifdef using opengl
// using openglleft handed system
#define GLM_FORCE_LEFT_HANDED
// using clip space from 0 to 1 (like in directx?) instead of -1 to 1
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE

// CONFIGS
#ifdef WGINE_DEBUG
	#define WGINE_ENABLE_ASSERTS
#endif

// ASSERTS
#ifdef WGINE_ENABLE_ASSERTS
	#define WGINE_ASSERT(x, ...) { if(!(x)) { WGINE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define WGINE_CORE_ASSERT(x, ...) { if(!(x)) { WGINE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define WGINE_ASSERT(x, ...)
	#define WGINE_CORE_ASSERT(x, ...)
#endif

//MACROS
#define BIT(x) (1 << x)
#define WGINE_BIND_EVENT_FN(fn) (std::bind(&fn, this, std::placeholders::_1))

#include <memory>
namespace Wgine
{
	template<typename T>
	using Unique = std::unique_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Unique<T> MakeUnique(Args &&...args) { return std::make_unique<T>(std::forward<Args>(args)...); }

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Ref<T> MakeRef(Args &&...args) { return std::make_shared<T>(std::forward<Args>(args)...); }
}

// switches on the Renderer::API to create a desired Ref (buffers/shaders/textures etc.)
// i.e type = VertexBuffer/Shader/Texture2D etc.
// args are the ones that the desired constructor uses
// NOTE: need to include Wgine/Renderer/Renderer.h to compile (as well as any of the returned types files)
#define SWITCH_RENDERAPI_MAKEREF(type, ...) {\
		switch(Renderer::GetAPI())\
		{\
			case Renderer::API::None: WGINE_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;\
			case Renderer::API::OpenGL: return MakeRef<OpenGL ## type>(__VA_ARGS__);\
		}\
		WGINE_CORE_ASSERT(false, "Unknown RendererAPI!");\
		return nullptr;\
	}
