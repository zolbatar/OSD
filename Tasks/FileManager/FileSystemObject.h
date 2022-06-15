#pragma once

enum class FileNamingFormat {
	Acorn
};

class FileSystem {
public:
	void SetCurrentDirectory(std::string directory);
private:
	FileNamingFormat format = FileNamingFormat::Acorn;
};

