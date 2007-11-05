#include <iostream>

#include "../inc/DllOpen.h"
#include "../inc/LibraryManager.h"
#include "../inc/Algorithm.h"

namespace Mantid
{
	
	LibraryManager* LibraryManager::instance = 0;
	void* LibraryManager::module = 0;

	LibraryManager::LibraryManager()
	{

	}

	LibraryManager::~LibraryManager()
	{
		//Close lib
		DllOpen::CloseDll(module);
		module = 0;
	}

	LibraryManager* LibraryManager::Initialise(const std::string& libName)
	{
		if (!instance)
		{
			instance = new LibraryManager;
				
			//Load dynamically loaded library
			module = DllOpen::OpenDll(libName.c_str());
			if (!module) 
			{
				std::cout << "Could not open library!\n";
				return 0;
			}
		}
	
		return instance;
	}

	Algorithm* LibraryManager::CreateAlgorithm(const std::string& algName)
	{
		create_alg* createMyAlg = (create_alg*) DllOpen::GetFunction(module, algName.c_str());
	
		return createMyAlg();
	}

	void LibraryManager::DestroyAlgorithm(const std::string& algName, Algorithm* obj)
	{
		destroy_alg* destroyMyAlg = (destroy_alg*) DllOpen::GetFunction(module, algName.c_str());

		destroyMyAlg(obj);
	}
}

