#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include "root_directory.h"

class FileSystemUtil
{
private:
	typedef std::string(*Build) (const std::string& path);

public:
	static std::string getPath(const std::string& path)
	{
		static std::string(*pathBuilder)(const std::string&) = getPathBuilder();
		return (*pathBuilder)(path);
	}

private:
	static const std::string& getRoot()
	{
		char* envRoot = nullptr;
		size_t sz = 0;
		if (_dupenv_s(&envRoot, &sz, "LOGL_ROOT_PATH") == 0 && envRoot != nullptr)
		{
			std::cerr << "EnvVarName = '" << envRoot << "'.\n";
		}
		static const char* givenRoot = (envRoot != nullptr ? envRoot : RESOURCE_PATH_DIR);
		static std::string root = (givenRoot != nullptr ? givenRoot : "");
		if (envRoot)
		{
			free(envRoot);
		}
		return root;
	}

	static Build getPathBuilder()
	{
		if (getRoot() != "")
		{
			return &FileSystemUtil::getPathRelativeRoot;
		}
		else 
		{
			return &FileSystemUtil::getPathRelativeBinary;
		}
	}

	static std::string getPathRelativeRoot(const std::string& path)
	{
		return getRoot() + std::string("/") + path;
	}

	static std::string getPathRelativeBinary(const std::string& path)
	{
		return "../../" + path;
	}
};