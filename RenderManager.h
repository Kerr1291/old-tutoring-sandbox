#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <SandboxGlobals.h>
#include <string>
#include <list>
#include <map>


struct GraphicData;
struct RenderObject;

const unsigned DEFAULT_RESERVED_GRAPHIC_SPACE = 64;

class RenderManager
{
	friend struct GraphicData;
	friend struct RenderObject;
	public:

		RenderManager();
		~RenderManager();

		void SetViewCullingScalar(float cullingScalar);
		truth CreateShaderObjects();
		
		void Draw();
		void SetWireframes(truth enabled);

		RenderObject* CreateCube(vec3* pos, mat4* rot, float* scale, const std::string& name = "");
		RenderObject* CreateBox(vec3* pos, mat4* rot, float* scale, float sx, float sy, float sz, const std::string& name = "");
		RenderObject* CreateSphere(vec3* pos, mat4* rot, float* scale, const std::string& name = "");
		RenderObject* CreateRenderObject(vec3* verts, unsigned _numVerts, unsigned* indices, unsigned _numIndices, const std::string& meshname,
						        vec3* pos, mat4* rot, float* scale, const std::string& name = "");
		RenderObject* FindRenderObject(const std::string& nameToFind);
		truth RemoveRenderObject( RenderObject* objectToRemove );

	private:

		RenderObject* MakeRenderObject( GraphicData* g, vec3* pos, mat4* rot, float* scale, const std::string& name );
		void RenderManager::Render(std::vector<RenderObject*>& objects);

		truth IsNearCamera(const vec2& pos, const float area_size);

		void CreateTexture(unsigned &texture);
		void CreateRenderbuffer(unsigned &buffer, unsigned &framebuffer, unsigned texture0, unsigned texture1 = 0);

		truth CreateShader(const std::string& filename, unsigned handle);
		truth LoadShaderFile(const std::string& filename, char** out_shaderStrings);
		truth CompileShader(unsigned shaderHandle, char* shader_data);
		truth LinkShaders(unsigned program, unsigned vertex, unsigned fragment);

		truth AddGfx( GraphicData* g, std::string filename );
		truth HasGfx( std::string filename );
		void AllocateGraphicSpace( unsigned size );
		void RemoveRenderObject(unsigned i);

		std::vector<RenderObject*> renderObjects;

		std::map<std::string,unsigned> loadedGraphics;

		unsigned maxStorage;
		unsigned currentStorage;
		GraphicData** graphics;
		float nearby_scalar;

		//shader data
		unsigned vertexShader;
		unsigned fragmentShader;
		unsigned basicProgram;

		unsigned normalRenderbuffer;
		unsigned normalTexture;
		unsigned normalFramebuffer;
		
		truth drawWireframes;
};


#endif