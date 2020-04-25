#include <memory>
#include <vector>
#include <string>
#include "Materials.h"
#include "Model.h"

//Forward Declaration of our help function
void ShowHelp(void);

int main(int argc, char* argv[])
{
    std::string baseInstallDir;
    std::string sceneFilename;
    std::string materialDictionaryFilename;

    //If user specified more than just the executable path on the command line, parse arguments
    if (argc >= 2)
    {
        std::vector<std::string> arguments;
        for (int i = 1; i < argc; i++)
        {
            arguments.push_back(argv[i]);
        }

        //If the user specified "Help" argument at any point in the command, just show help and exit
        for (unsigned int i = 1; i < arguments.size(); i++)
        {
            if (arguments.at(i) == "-H" || arguments.at(i) == "/H" || arguments.at(i) == "-h" || arguments.at(i) == "/h" || arguments.at(i) == "-?" || arguments.at(i) == "/?")
            {
                ShowHelp();
                return 0;
            }
        }

        //If no help requested, parse the arguments normally
        for (int i = 0; i + 1 <= (int) arguments.size(); i++)
        {
            //if it looks like a parameter switch, test it. 
            if (arguments.at(i).at(0) == '-' || arguments.at(i).at(0) == '/')	//Check for a switch prefix character (/ or -)
            {
                if (arguments.at(i).length() != 2)
                {
                    std::cout << "Error: Invalid Parameter " << arguments.at(i) << "\n";
                    return(1);
                }
                else if (arguments.at(i).at(1) == 'b' or arguments.at(i).at(1) == 'B')
                {
                    if (i < (argc - 1))
                    {
                        if (arguments.at(i + 1).length() > _MAX_PATH)
                        {
                            std::cout << "Error: SOTS2 Install Path exceeds " << _MAX_PATH << "characters\n";
                            return(1);
                        }
                        else
                        {
                            ++i;
                            baseInstallDir = arguments.at(i);
                            
                        }
                    }
                    else
                    {
                        std::cout << "Error: SOTS2 Install Path parameter specified but no path provided\n";
                    }
                }
                else if (arguments.at(i).at(1) == 'd' or arguments.at(i).at(1) == 'D')
                {
                    if (i < (argc - 1))
                    {
                        if (arguments.at(i + 1).length() > _MAX_PATH)
                        {
                            std::cout << "Error: SOTS2 material path exceeds " << _MAX_PATH << "characters\n";
                            return(1);
                        }
                        else
                        {
                            ++i;
                            materialDictionaryFilename = arguments.at(i);
                        }
                    }
                    else
                    {
                        std::cout << "Error: SOTS2 Material Dictionary parameter specified but no path provided\n";
                    }
                }
                else
                {
                    std::cout << "Error: Invalid Parameter " << arguments.at(i) << "\n";
                }
            }
            else if (sceneFilename.empty() == true)
            {
                sceneFilename = arguments.at(i);
            }
        }

    }
    else ShowHelp();

    if (sceneFilename.empty() == false)
    {
        if (baseInstallDir.empty() == false)
        {
            std::unique_ptr<MaterialDictionary> materialDictionary = std::make_unique<MaterialDictionary>();

            if (materialDictionaryFilename.empty() == false)
            {
                materialDictionary->Load(materialDictionaryFilename, baseInstallDir);
            }

            std::unique_ptr<Model> model = std::make_unique<Model>();
            model->Load(sceneFilename, materialDictionary.get());

            model->SaveAsOBJ();
        }      
        else
        {
            std::cout << "Missing required parameter: SOTS2 Installation Directory\n";
            return 1;
        }
    }
    else
    {
        std::cout << "Missing required parameter: SOTS2 Scene Filename\n";
        return 1;
    }

    return 0;
}

void ShowHelp(void)
{
    std::cout << "\n";
    std::cout << "Sword of the Stars 2 Model Loader" << "\n";
    std::cout << "\n";
    std::cout << "This utility can convert SOTS2 .SCENE~ files into Wavefront (.OBJ) model files.\n";
    std::cout << "If a SOTS2 materials.xml~ file is provided, will also create Material (.MTL) files\n";
    std::cout << "and attempt to convert each referenced .tga~ texture to a DDS2 (.DXT) file.";
    std::cout << "\n";
    std::cout << "Once converted, OBJ + MTL files should be importable in popular 3D rendering software\n";
    std::cout << "such as Blender.\n";
    std::cout << "\n";
    std::cout << "Command Syntax: SOTS2Loader [-H|-?] InputSceneFile -B BaseSOTS2InstallDir [-D InputMaterialsFile]" << "\n";
    std::cout << "\n";
    std::cout << "-?, -H                   This Help" << "\n";
    std::cout << "\n";
    std::cout << "InputSceneFile           Input File Name.. (Sword of the Stars 2 .scene~ file)" << "\n";
    std::cout << "\n";
    std::cout << " -B BaseSots2InstallDir  Installation directory of SOTS2\n";
    std::cout << "    eg: \"C:\\Program Files(x86)\\Steam\\steamapps\\common\\Sword of the Stars II\"\n";
    std::cout << "\n";
    std::cout << " -D InputMaterialsFile   Path to Materials Dictionary\n";
    std::cout << "    eg: \"C:\\Program Files(x86)\\Steam\\steamapps\\common\\Sword of the Stars II\\assets\\base\\factions\\hiver\\materials.xml~\"\n";
    std::cout << "    If no SOTS2 materials.xml~ file provided, no textures will be included with generated model\n";
    std::cout << "\n";
    std::cout << "\n";
    exit(1);
}
