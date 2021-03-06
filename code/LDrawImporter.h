/** @file  LDrawImporter.h
 *  @brief Declaration of the LDraw importer class
 */
#ifndef INCLUDED_AI_LDR_IMPORTER_H
#define INCLUDED_AI_LDR_IMPORTER_H

#include "AssimpPCH.h"
#include "ParsingUtils.h"
#include "fast_atof.h"
#include "TinyFormatter.h"
#include "Importer.h"
#include "GenericProperty.h"
#include "SceneCombiner.h"
#include "LineSplitter.h"

namespace Assimp{

	namespace LDraw{
		typedef unsigned int ColorIndex;
		struct LDrawMaterial
		{
			LDrawMaterial(std::string name, int code, aiColor3D color, aiColor3D edge) :
			name(name), code(code), color(color), edge(edge)
			{}
			//human readable name
			std::string name;
			//identification of the color in LDraw files
			ColorIndex code;
			//the main color of the material
			aiColor3D color;
			//the contrast color of the material
			aiColor3D edge;
			//opacity
			float alpha = 1.0f;
			//factor of light emmision
			float luminance = 0.0f;
		};

		struct LDrawMesh
		{
			std::vector<aiVector3D> vertices;
			std::vector<aiFace> faces;
			unsigned int primitivesType = 0;
		};

		struct SubFileReference
		{
			aiMatrix4x4 transformation;
			std::string path;
			ColorIndex color;
			bool variableColor = false;
		};

		struct LDrawNode
		{
			SubFileReference file;
			std::vector<LDrawNode> children;
		};

		struct LDrawFile
		{
			std::map<ColorIndex, LDrawMesh> meshes;
			LDrawNode subtree;
		};
	}

	using namespace LDraw;

	class LDrawImporter : public BaseImporter
	{
	public:
		LDrawImporter();
		~LDrawImporter();

		// -------------------------------------------------------------------

		bool CanRead(const std::string& pFile, IOSystem* pIOHandler,
			bool checkSig) const;

		// -------------------------------------------------------------------

		const aiImporterDesc* GetInfo() const;

		// -------------------------------------------------------------------

		void SetupProperties(const Importer* pImp);

		// -------------------------------------------------------------------
	protected:
		void InternReadFile(const std::string& pFile, aiScene* pScene,
			IOSystem* pIOHandler);

		// -------------------------------------------------------------------

	private:

		// Read num floats from the buffer and store it in the out array
		bool ReadNumFloats(const char* line, float* &out, unsigned int num);

		// -------------------------------------------------------------------

		//throw an DeadlyImportError with the specified Message
		void ThrowException(const std::string &Message){
			throw DeadlyImportError("LDraw: " + Message);
		}

		// -------------------------------------------------------------------

		//try to read the LDraw materials from _libPath/ldconfig.ldr
		void ReadMaterials(std::string filename);

		// -------------------------------------------------------------------

		//Build up a scene structure tree of LDrawNode 
		void ProcessNode(std::string file, LDrawNode* current, unsigned int colorindex);

		// -------------------------------------------------------------------

		//Convert the LDrawNode structure to assimps scene structure
		void ConvertNode(aiNode* node, LDrawNode* current, std::vector<aiMesh*>* aiMeshes, std::vector<aiMaterial*>* aiMaterials);

		// -------------------------------------------------------------------

		//recursive assignment of color value to LDrawNodes
		void ColorNode(LDrawNode* current, ColorIndex color);

		// -------------------------------------------------------------------
		//try to find the full path of file in the LDrawLibrary
		//returns "" if unsuccessful
		std::string FindPath(std::string subpath);

		//IOSystem short reference
		IOSystem* pIOHandler = NULL;

		//path to the root folder of the library
		std::string _libPath = "";

		//container for the LDraw color definitions
		std::map<ColorIndex, LDraw::LDrawMaterial> materials = std::map<ColorIndex,LDraw::LDrawMaterial>();

		//cache for already loaded files
		std::map<std::string, LDrawFile> fileCache = std::map<std::string, LDrawFile>();

	}; //end of class LDrawImporter

} // end of namespace Assimp
#endif // !INCLUDED_AI_LDR_IMPORTER_H