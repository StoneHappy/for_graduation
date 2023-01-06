#include "RCData.h"
#include <stddef.h>
#include <Global/CoreContext.h>
#include <Renderer/VulkanContext.h>
#include <Renderer/VulkanBuffer.h>
namespace GU
{
	static inline int bit(int a, int b)
	{
		return (a & (1 << b)) >> b;
	}
	static inline unsigned int duRGBA(int r, int g, int b, int a)
	{
		return ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) | ((unsigned int)a << 24);
	}

	static unsigned int duIntToCol(int i, int a)
	{
		int	r = bit(i, 1) + bit(i, 3) * 2 + 1;
		int	g = bit(i, 2) + bit(i, 4) * 2 + 1;
		int	b = bit(i, 0) + bit(i, 5) * 2 + 1;
		return duRGBA(r * 63, g * 63, b * 63, a);
	}

	static unsigned int areaToCol(unsigned int area)
	{
		if (area == 0)
		{
			// Treat zero area type as default.
			return duRGBA(0, 192, 255, 255);
		}
		else
		{
			return duIntToCol(area, 255);
		}
	}

	static inline unsigned int duMultCol(const unsigned int col, const unsigned int d)
	{
		const unsigned int r = col & 0xff;
		const unsigned int g = (col >> 8) & 0xff;
		const unsigned int b = (col >> 16) & 0xff;
		const unsigned int a = (col >> 24) & 0xff;
		return duRGBA((r * d) >> 8, (g * d) >> 8, (b * d) >> 8, a);
	}

	static void duCalcBoxColors(unsigned int* colors, unsigned int colTop, unsigned int colSide)
	{
		if (!colors) return;

		colors[0] = duMultCol(colTop, 250);
		colors[1] = duMultCol(colSide, 140);
		colors[2] = duMultCol(colSide, 165);
		colors[3] = duMultCol(colSide, 217);
		colors[4] = duMultCol(colSide, 165);
		colors[5] = duMultCol(colSide, 217);
	}

	static void createVertexBuffer(VulkanContext& vkContext, const std::vector<RCVertex>& vertices, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// map memory to cpu host
		void* data;
		vkMapMemory(vkContext.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(vkContext.logicalDevice, stagingBufferMemory);

		createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

		copyBuffer(vkContext, stagingBuffer, buffer, bufferSize);

		vkDestroyBuffer(vkContext.logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(vkContext.logicalDevice, stagingBufferMemory, nullptr);
	}

	RCMesh::RCMesh(const rcPolyMesh& mesh)
	{
		const int nvp = mesh.nvp;
		const float cs = mesh.cs;
		const float ch = mesh.ch;
		const float* orig = mesh.bmin;

		for (int i = 0; i < mesh.npolys; ++i)
		{
			const unsigned short* p = &mesh.polys[i * nvp * 2];
			const unsigned char area = mesh.areas[i];

			unsigned int color;
			if (area == RC_WALKABLE_AREA)
				color = duRGBA(0, 192, 255, 64);
			else if (area == RC_NULL_AREA)
				color = duRGBA(0, 0, 0, 64);
			else
				color = areaToCol(area);

			unsigned short vi[3];
			for (int j = 2; j < nvp; ++j)
			{
				if (p[j] == RC_MESH_NULL_IDX) break;
				vi[0] = p[0];
				vi[1] = p[j - 1];
				vi[2] = p[j];
				for (int k = 0; k < 3; ++k)
				{
					const unsigned short* v = &mesh.verts[vi[k] * 3];
					const float x = orig[0] + v[0] * cs;
					const float y = orig[1] + (v[1] + 1) * ch;
					const float z = orig[2] + v[2] * cs;
					RCVertex vertex;
					vertex.pos = { x, y, z };
					glm::u8vec4 tmpcolor;
					memcpy(&tmpcolor, &color, 4 * sizeof(uint8_t));
					vertex.color.r = (float)tmpcolor.r;
					vertex.color.g = (float)tmpcolor.g;
					vertex.color.b = (float)tmpcolor.b;
					vertex.color.a = (float)tmpcolor.a;
					m_verts.emplace_back(std::move(vertex));
				}
			}
		}

		// generate render buuffer
		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, m_verts, vertexBuffer, vertexMemory);
	}
	RCMesh::RCMesh(const rcPolyMeshDetail& dmesh)
	{
		for (int i = 0; i < dmesh.nmeshes; ++i)
		{
			const unsigned int* m = &dmesh.meshes[i * 4];
			const unsigned int bverts = m[0];
			const unsigned int btris = m[2];
			const int ntris = (int)m[3];
			const float* verts = &dmesh.verts[bverts * 3];
			const unsigned char* tris = &dmesh.tris[btris * 4];

			unsigned int color = duIntToCol(i, 192);
			glm::u8vec4 tmpcolor;
			memcpy(&tmpcolor, &color, 4 * sizeof(uint8_t));
			glm::vec4 glmcolor = tmpcolor;
			for (int j = 0; j < ntris; ++j)
			{
				m_verts.push_back({ {(&verts[tris[j * 4 + 0] * 3])[0], (&verts[tris[j * 4 + 0] * 3])[1], (&verts[tris[j * 4 + 0] * 3])[2]}, glmcolor });
				m_verts.push_back({ {(&verts[tris[j * 4 + 1] * 3])[0], (&verts[tris[j * 4 + 1] * 3])[1], (&verts[tris[j * 4 + 1] * 3])[2]}, glmcolor });
				m_verts.push_back({ {(&verts[tris[j * 4 + 2] * 3])[0], (&verts[tris[j * 4 + 2] * 3])[1], (&verts[tris[j * 4 + 2] * 3])[2]}, glmcolor});
				/*dd->vertex(&verts[tris[j * 4 + 0] * 3], color);
				dd->vertex(&verts[tris[j * 4 + 1] * 3], color);
				dd->vertex(&verts[tris[j * 4 + 2] * 3], color);*/
			}
		}

		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, m_verts, vertexBuffer, vertexMemory);
	}
	VkVertexInputBindingDescription RCVertex::getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(RCVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	::std::array<VkVertexInputAttributeDescription, 2> RCVertex::getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(RCVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(RCVertex, color);

		return attributeDescriptions;
	}
	RCContour::RCContour(const rcPolyMeshDetail& dmesh)
	{
		const unsigned int coli = duRGBA(0, 0, 0, 64);
		for (int i = 0; i < dmesh.nmeshes; ++i)
		{
			const unsigned int* m = &dmesh.meshes[i * 4];
			const unsigned int bverts = m[0];
			const unsigned int btris = m[2];
			const int ntris = (int)m[3];
			const float* verts = &dmesh.verts[bverts * 3];
			const unsigned char* tris = &dmesh.tris[btris * 4];

			for (int j = 0; j < ntris; ++j)
			{
				const unsigned char* t = &tris[j * 4];
				for (int k = 0, kp = 2; k < 3; kp = k++)
				{
					unsigned char ef = (t[3] >> (kp * 2)) & 0x3;
					if (ef == 0)
					{
						// Internal edge
						if (t[kp] < t[k])
						{
							internalVerts.push_back({ {(&verts[t[kp] * 3])[0], (&verts[t[kp] * 3])[1], (&verts[t[kp] * 3])[2]}, {0, 0, 0, 64} });
							internalVerts.push_back({ {(&verts[t[k] * 3])[0], (&verts[t[k] * 3])[1], (&verts[t[k] * 3])[2]}, {0, 0, 0, 64} });
							/*dd->vertex(&verts[t[kp] * 3], coli);
							dd->vertex(&verts[t[k] * 3], coli);*/
						}
					}
				}
			}
		}
		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, internalVerts, internalVertexBuffer, internalVertexMemory);

		const unsigned int cole = duRGBA(0, 0, 0, 64);
		for (int i = 0; i < dmesh.nmeshes; ++i)
		{
			const unsigned int* m = &dmesh.meshes[i * 4];
			const unsigned int bverts = m[0];
			const unsigned int btris = m[2];
			const int ntris = (int)m[3];
			const float* verts = &dmesh.verts[bverts * 3];
			const unsigned char* tris = &dmesh.tris[btris * 4];

			for (int j = 0; j < ntris; ++j)
			{
				const unsigned char* t = &tris[j * 4];
				for (int k = 0, kp = 2; k < 3; kp = k++)
				{
					unsigned char ef = (t[3] >> (kp * 2)) & 0x3;
					if (ef != 0)
					{
						// Ext edge
						externalVerts.push_back({ {(&verts[t[kp] * 3])[0], (&verts[t[kp] * 3])[1], (&verts[t[kp] * 3])[2]}, {0, 0, 0, 64} });
						externalVerts.push_back({ {(&verts[t[k] * 3])[0], (&verts[t[k] * 3])[1], (&verts[t[k] * 3])[2]}, {0, 0, 0, 64} });
						/*dd->vertex(&verts[t[kp] * 3], cole);
						dd->vertex(&verts[t[k] * 3], cole);*/
					}
				}
			}
		}

		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, externalVerts, externalVertexBuffer, externalVertexMemory);
	}


	void RCHeightfieldSolid::duAppendBox(float minx, float miny, float minz,
		float maxx, float maxy, float maxz, const unsigned int* fcol)
	{
		const float verts[8 * 3] =
		{
			minx, miny, minz,
			maxx, miny, minz,
			maxx, miny, maxz,
			minx, miny, maxz,
			minx, maxy, minz,
			maxx, maxy, minz,
			maxx, maxy, maxz,
			minx, maxy, maxz,
		};
		static const unsigned char inds[6 * 6] =
		{
			7, 6, 5, 4, 7, 5,
			0, 1, 2, 3, 0, 2,
			1, 5, 6, 2, 1, 6,
			3, 7, 4, 0, 3, 4,
			2, 6, 7, 3, 2, 7,
			0, 4, 5, 1, 0, 5
		};

		const unsigned char* in = inds;
		for (int i = 0; i < 6; ++i)
		{
			glm::u8vec4 tmpcolor;
			memcpy(&tmpcolor, &fcol[i], 4 * sizeof(uint8_t));
			glm::vec4 icolor = tmpcolor;
			m_verts.push_back({ {(&verts[*in * 3])[0], (&verts[*in * 3])[1], (&verts[*in * 3])[2]}, icolor }); in++;
			m_verts.push_back({ {(&verts[*in * 3])[0], (&verts[*in * 3])[1], (&verts[*in * 3])[2]}, icolor }); in++;
			m_verts.push_back({ {(&verts[*in * 3])[0], (&verts[*in * 3])[1], (&verts[*in * 3])[2]}, icolor }); in++;
			m_verts.push_back({ {(&verts[*in * 3])[0], (&verts[*in * 3])[1], (&verts[*in * 3])[2]}, icolor }); in++;
			m_verts.push_back({ {(&verts[*in * 3])[0], (&verts[*in * 3])[1], (&verts[*in * 3])[2]}, icolor }); in++;
			m_verts.push_back({ {(&verts[*in * 3])[0], (&verts[*in * 3])[1], (&verts[*in * 3])[2]}, icolor }); in++;

			/*dd->vertex(&verts[*in * 3], fcol[i]); in++;
			dd->vertex(&verts[*in * 3], fcol[i]); in++;
			dd->vertex(&verts[*in * 3], fcol[i]); in++;
			dd->vertex(&verts[*in * 3], fcol[i]); in++;*/
		}
	}

	RCHeightfieldSolid::RCHeightfieldSolid(const rcHeightfield& hf)
	{
		const float* orig = hf.bmin;
		const float cs = hf.cs;
		const float ch = hf.ch;

		const int w = hf.width;
		const int h = hf.height;

		unsigned int fcol[6];
		duCalcBoxColors(fcol, duRGBA(255, 255, 255, 255), duRGBA(255, 255, 255, 255));

		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				float fx = orig[0] + x * cs;
				float fz = orig[2] + y * cs;
				const rcSpan* s = hf.spans[x + y * w];
				while (s)
				{
					duAppendBox(fx, orig[1] + s->smin * ch, fz, fx + cs, orig[1] + s->smax * ch, fz + cs, fcol);
					s = s->next;
				}
			}
		}

		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, m_verts, vertexBuffer, vertexMemory);
	}
	RCAgentPath::RCAgentPath(float* path, int nsmmoth)
	{
		const unsigned int spathCol = duRGBA(0, 0, 0, 220);
		for (int i = 0; i < nsmmoth; ++i)
		{
			RCVertex vertex;
			vertex.pos = { path[i * 3], path[i * 3 + 1] + 0.1f, path[i * 3 + 2] };
			glm::u8vec4 tmpcolor;
			memcpy(&tmpcolor, &spathCol, 4 * sizeof(uint8_t));
			vertex.color.r = (float)tmpcolor.r;
			vertex.color.g = (float)tmpcolor.g;
			vertex.color.b = (float)tmpcolor.b;
			vertex.color.a = (float)tmpcolor.a;
			m_verts.emplace_back(std::move(vertex));
		}

		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, m_verts, vertexBuffer, vertexMemory);
	}
	RCStraightPath::RCStraightPath(float* path, int num)
	{
		const unsigned int spathCol = duRGBA(0, 0, 0, 220);

		glm::u8vec4 tmpcolor;
		memcpy(&tmpcolor, &spathCol, 4 * sizeof(uint8_t));
		for (int i = 0; i < num - 2; ++i)
		{
			unsigned int col;
			col = spathCol;
			RCVertex vertex0;
			vertex0.pos = { path[i * 3], path[i * 3 + 1] + 0.4f, path[i * 3 + 2] };
			vertex0.color.r = (float)tmpcolor.r;
			vertex0.color.g = (float)tmpcolor.g;
			vertex0.color.b = (float)tmpcolor.b;
			vertex0.color.a = (float)tmpcolor.a;

			RCVertex vertex1;
			vertex1.pos = { path[(i + 1) * 3], path[(i + 1) * 3 + 1] + 0.4f, path[(i + 1) * 3 + 2] };
			vertex1.color.r = (float)tmpcolor.r;
			vertex1.color.g = (float)tmpcolor.g;
			vertex1.color.b = (float)tmpcolor.b;
			vertex1.color.a = (float)tmpcolor.a;
			if (vertex1.pos.x < -999) continue;

			m_verts.push_back(vertex0);
			m_verts.push_back(vertex1);
			/*dd.vertex(path[i * 3], path[i * 3 + 1] + 0.4f, path[i * 3 + 2], col);
			dd.vertex(path[(i + 1) * 3], path[(i + 1) * 3 + 1] + 0.4f, path[(i + 1) * 3 + 2], col);*/
		}

		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, m_verts, vertexBuffer, vertexMemory);
	}
	RCAgentSamplePath::RCAgentSamplePath(const std::vector<glm::vec3>& paths)
	{
		const unsigned int spathCol = duRGBA(220, 0, 0, 220);
		for (int i = 0; i < paths.size(); ++i)
		{
			RCVertex vertex;
			vertex.pos = paths[i];
			glm::u8vec4 tmpcolor;
			memcpy(&tmpcolor, &spathCol, 4 * sizeof(uint8_t));
			vertex.color.r = (float)tmpcolor.r;
			vertex.color.g = (float)tmpcolor.g;
			vertex.color.b = (float)tmpcolor.b;
			vertex.color.a = (float)tmpcolor.a;
			m_verts.emplace_back(std::move(vertex));
		}

		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, m_verts, vertexBuffer, vertexMemory);
	}

	glm::vec4 Conver2GLMColor(const unsigned int spathCol)
	{
		glm::u8vec4 tmpcolor;
		glm::vec4 color;
		memcpy(&tmpcolor, &spathCol, 4 * sizeof(uint8_t));
		color.r = (float)tmpcolor.r;
		color.g = (float)tmpcolor.g;
		color.b = (float)tmpcolor.b;
		color.a = (float)tmpcolor.a;
		return color;
	}
	RCTContours::RCTContours(const rcContourSet& cset)
	{
		const float* orig = cset.bmin;
		const float cs = cset.cs;
		const float ch = cset.ch;

		const unsigned char a = 255;

		for (int i = 0; i < cset.nconts; ++i)
		{
			const rcContour& c = cset.conts[i];
			unsigned int color = duIntToCol(c.reg, a);
			for (int j = 0; j < c.nrverts; ++j)
			{
				const int* v = &c.rverts[j * 4];
				float fx = orig[0] + v[0] * cs;
				float fy = orig[1] + (v[1] + 1 + (i & 1)) * ch;
				float fz = orig[2] + v[2] * cs;
				RCVertex vertex; 
				vertex.pos = { fx, fy, fz };
				vertex.color = Conver2GLMColor(color);
				//dd->vertex(fx, fy, fz, color);
				m_verts.push_back(vertex);
				if (j > 0)
				{
					RCVertex vertex1;
					vertex1.pos = { fx, fy, fz };
					vertex1.color = Conver2GLMColor(color);
					m_verts.push_back(vertex1);
				}
			}
			// Loop last segment.
			const int* v = &c.rverts[0];
			float fx = orig[0] + v[0] * cs;
			float fy = orig[1] + (v[1] + 1 + (i & 1)) * ch;
			float fz = orig[2] + v[2] * cs;
			RCVertex vertex;
			vertex.pos = { fx, fy, fz };
			vertex.color = Conver2GLMColor(color);
			//dd->vertex(fx, fy, fz, color);
			m_verts.push_back(vertex);
		}

		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, m_verts, vertexBuffer, vertexMemory);
	}
}