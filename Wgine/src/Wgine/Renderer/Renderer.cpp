#include "WginePCH.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Vertex.h"

#include "Wgine/Core/Core.h"
#include "Wgine/Core/Time.h"
#include "Wgine/Renderer/Mesh.h"
#include "Wgine/Renderer/Vertex.h"
#include "Wgine/Renderer/Material.h"
#include "Wgine/Renderer/Light.h"
#include "Wgine/Renderer/RendererDebug.h"

namespace Wgine
{
	Renderer::API Renderer::s_API = Renderer::API::OpenGL;
	const uint32_t Renderer::s_TextureSlotsCount = 32;
	int Renderer::s_TextureSlots[s_TextureSlotsCount] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
	
	class RendererData
	{
	public:
		RendererData()
		{
			ActiveScene = nullptr;
			m_TextureSlot = 0;
		}

		void Init()
		{
			PointLightsSSBO = StorageBuffer::Create(sizeof(PointLightGPU) * 16);
		}

		// returns texture slot beginning at 0
		uint8_t BindTexture(Ref<Texture2D> texture)
		{
			auto slot = FindTextureSlot(texture);
			if (slot == -1) // texture is not bound yet
			{
				m_Textures[m_TextureSlot++] = texture;
				texture->Bind(m_TextureSlot - 1);
				return m_TextureSlot - 1;
			}
			else // texture already bound
				return slot;
		}

		void ResetTextures()
		{
			m_TextureSlot = 0;
		}

		uint32_t GetFreeSlotsCount() const { return 32 - m_TextureSlot; }


		void ResetLights() { m_PointLights.clear(); }
		void AddPointLight(PointLight *l) { m_PointLights.push_back(*l); }
		void UploadLights() { PointLightsSSBO->SetData(m_PointLights.data(), sizeof(PointLightGPU) * m_PointLights.size()); }

	private:
		// returns unsigned -1 (largest uint32) in case texture is not bound
		uint32_t FindTextureSlot(Ref<Texture2D> texture)
		{
			auto it = std::find(m_Textures.begin(), m_Textures.end(), texture);
			return it == m_Textures.end() ? -1 : it - m_Textures.begin();
		}

	public:
		Scene *ActiveScene = nullptr;
		Ref<StorageBuffer> PointLightsSSBO;

	private:
		uint32_t m_TextureSlot;
		std::array<Ref<Texture2D>, Renderer::s_TextureSlotsCount> m_Textures;

		std::vector<PointLightGPU> m_PointLights;
	};

	static RendererData s_RendererData = RendererData();

	struct PerShaderData
	{
		PerShaderData()
		{}

		PerShaderData(Ref<Shader> s)
			: Shader(s), VAO(VertexArray::Create())
		{
			Vertices.reserve(500000);
			Indices.reserve(500000);
			Materials.reserve(500);
			MaterialIDs.reserve(500000);
			Transforms.reserve(5000);
			TransformIDs.reserve(500000);
			Meshes.reserve(5000);

			VAO = VertexArray::Create();
			VBO = VertexBuffer::Create(sizeof(Vertex) * 500000);
			VBO->SetLayout(Vertex::GetLayout());
			IBO = IndexBuffer::Create(500000);
			VAO->AddVertexBuffer(VBO);
			VAO->SetIndexBuffer(IBO);

			TransformSSBO = StorageBuffer::Create(sizeof(TransformGPU) * 5000);
			TransformIDSSBO = StorageBuffer::Create(sizeof(int32_t) * 500000);
			MaterialSSBO = StorageBuffer::Create(sizeof(MaterialGPU) * 100);
			MaterialIDSSBO = StorageBuffer::Create(sizeof(uint32_t) * 500000);

			Reset();
		}

		void Reset()
		{
			Meshes.clear();
			Transforms.clear();
			TransformIDs.clear();
			Materials.clear();
			MaterialIDs.clear();
			Vertices.clear();
			Indices.clear();
		}

		void Flush()
		{
			VBO->SetData(Vertices.data(), sizeof(Vertex) * Vertices.size());
			IBO->SetData(Indices.data(), Indices.size());
			TransformSSBO->SetData(Transforms.data(), sizeof(TransformGPU) * Transforms.size());
			TransformIDSSBO->SetData(TransformIDs.data(), sizeof(int32_t) * TransformIDs.size());

			Scope<MaterialGPU[]> materialsData(new MaterialGPU[Materials.size()]);
			
			for (int i = 0; i < Materials.size(); i++)
			{
				if (s_RendererData.GetFreeSlotsCount() < 2) // TODO: adjust to include dynamic texture count
				{
					// TODO: flush and reset textures (probably we should not reset the vbo, ibo etc. but render it partially)
					WGINE_CORE_ASSERT(false, "TODO: Implement flushing when exceeded texture slots limit!");
				}
				else
				{
					materialsData[i] = *Materials[i].get();
					materialsData[i].Textures[0] = s_RendererData.BindTexture(Materials[i]->DiffuseTex);
					materialsData[i].Textures[1] = s_RendererData.BindTexture(Materials[i]->SpecularTex);
				}
			}

			MaterialSSBO->SetData(materialsData.get(), sizeof(MaterialGPU) * Materials.size());
			MaterialIDSSBO->SetData(MaterialIDs.data(), sizeof(int32_t) * MaterialIDs.size());
			
			Shader->Bind();
			Shader->UploadUniformMat4("u_ViewProjection", s_RendererData.ActiveScene->GetViewProjectionMatrix());
			Shader->UploadUniformFloat3("u_CameraLocation", s_RendererData.ActiveScene->GetActiveCamera()->GetLocation());

			// TODO: this should be uploaded only once at the start?
			Shader->UploadUniformIntArray("u_Texture", Renderer::s_TextureSlots, Renderer::s_TextureSlotsCount);

			VAO->Bind();
			IBO->Bind();
			VBO->Bind();

			RenderCommand::DrawIndexed(VAO, Indices.size());
		}

		Ref<Shader> Shader;
		std::vector<Ref<Mesh>> Meshes;

		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<IndexBuffer> IBO;
		Ref<StorageBuffer> TransformSSBO;
		Ref<StorageBuffer> TransformIDSSBO;
		Ref<StorageBuffer> MaterialSSBO;
		Ref<StorageBuffer> MaterialIDSSBO;

		std::vector<TransformGPU> Transforms;
		std::vector<int32_t> TransformIDs;
		std::vector<Ref<Material>> Materials;
		std::vector<int32_t> MaterialIDs;
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};

	static std::unordered_map<std::string, PerShaderData> s_ShaderData;

	static void SetupShaderSSBOs(Shader *shader)
	{
		shader->SetupStorageBuffer("ss_MaterialIDs", 0, s_ShaderData[shader->GetPath()].MaterialIDSSBO->GetPtr());
		shader->SetupStorageBuffer("ss_Materials", 1, s_ShaderData[shader->GetPath()].MaterialSSBO->GetPtr());
		shader->SetupStorageBuffer("ss_TransformIDs", 2, s_ShaderData[shader->GetPath()].TransformIDSSBO->GetPtr());
		shader->SetupStorageBuffer("ss_Transforms", 3, s_ShaderData[shader->GetPath()].TransformSSBO->GetPtr());
		shader->SetupStorageBuffer("ss_PointLights", 4, s_RendererData.PointLightsSSBO->GetPtr());
	}

	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
		RendererDebug::Init();

		s_ShaderData = std::unordered_map<std::string, PerShaderData>();
		s_RendererData.Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::BeginScene(Scene *scene)
	{
		WGINE_CORE_ASSERT(scene, "Invalid scene for renderer!");

		s_RendererData.ActiveScene = scene;
		RendererDebug::SetCamera(scene->GetActiveCamera());

		// reset shader data
		for (auto &[shaderName, shaderData] : s_ShaderData)
			shaderData.Reset(); // TODO: Im not sure this is where resetting should happen; perhaps after flushing (end scene?) makes sense?

		// reset lights data ^ same thing to consider as above
		s_RendererData.ResetLights();

		for (auto entity : s_RendererData.ActiveScene->m_SceneEntities)
			Submit(*entity);

		for (auto light : s_RendererData.ActiveScene->m_Lights)
			Submit(light);
	}

	void Renderer::Submit(const SceneEntity &entity)
	{
		Submit(entity.ShaderData, entity.MaterialData, entity.MeshData, entity.GetTransform());
	}

	// TODO: no need to keep references? just add to the resultant array that will be sent to the GPU (copying is necessary no matter what?)
	void Renderer::Submit(Ref<Shader> shader, Ref<Material> material, Ref<Mesh> mesh, const Transform &transform)
	{
		WGINE_ASSERT(s_RendererData.ActiveScene, "No active scene for renderer!");

		// TODO: in order to greatly improve performance, only update "dirty" entities (use some array mask to make it more efficient)

		if (!mesh)
			return;

		// new Shader
		if (s_ShaderData.find(shader->GetPath()) == s_ShaderData.end()) // TODO: when switched c++ 20 use .contains instead
		{
			s_ShaderData[shader->GetPath()] = PerShaderData(shader);
			SetupShaderSSBOs(shader.get());
		}

		auto &shaderData = s_ShaderData[shader->GetPath()];

		// push meshes, transforms
		shaderData.Meshes.push_back(mesh);
		int32_t transformID = shaderData.Transforms.size();
		shaderData.Transforms.push_back(transform);

		// push transform ids
		for (int i = 0; i < mesh->GetVertices().size(); i++)
			shaderData.TransformIDs.push_back(transformID);

		// push offset indices
		auto offset = shaderData.Vertices.size();
		auto &indices = mesh->GetIndices();
		for (int i = 0; i < indices.size(); i++)
			shaderData.Indices.push_back(indices[i] + offset);

		// push vertices
		for (int i = 0; i < mesh->GetVertices().size(); i++)
			shaderData.Vertices.push_back(mesh->GetVertices()[i]);

		// material
		uint32_t index;
		auto findMaterial = std::find(shaderData.Materials.begin(), shaderData.Materials.end(), material);
		if (findMaterial != shaderData.Materials.end())
			index = findMaterial - shaderData.Materials.begin();
		else
		{
			index = shaderData.Materials.size();
			shaderData.Materials.push_back(material);
		}

		// material id
		for (int i = 0; i < mesh->GetVertices().size(); i++)
			shaderData.MaterialIDs.push_back(index);
	}

	void Renderer::Submit(Light *light)
	{
		switch (light->GetLightType())
		{
		case Light::LightType::PointLight:
		{
			auto pointLight = static_cast<PointLight *>(light);
			s_RendererData.AddPointLight(pointLight);
			break;
		}
		default:
			WGINE_CORE_ASSERT(false, "Invalid light type / not implemented light type!");
			break;
		}
	}

	void Renderer::EndScene()
	{
		s_RendererData.UploadLights();

		for (auto &[shaderName, shaderData] : s_ShaderData)
			Flush(shaderData);

		RendererDebug::Flush();
	}

	void Renderer::Flush(PerShaderData &data)
	{
		data.Flush();
	}

	void Renderer::OnWindowResized(float width, float height)
	{
		RenderCommand::SetViewport(0, 0, (uint32_t)width, (uint32_t)height);
	}
}
