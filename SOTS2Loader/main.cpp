#include <memory>
#include "Materials.h"
#include "Model.h"

int main()
{
	std::string baseInstallDir = "D:\\Steam\\steamapps\\common\\Sword of the Stars II";

	
	std::string filename = "materials.xml~";
	std::unique_ptr<MaterialDictionary> materialDictionary = std::make_unique<MaterialDictionary>();
	if (materialDictionary->Load(filename, baseInstallDir))
	{
		materialDictionary->Print();
	}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	filename = "scene.scene";
	model->Load(filename, materialDictionary.get());

	
	model->SaveAsOBJ();	
}

