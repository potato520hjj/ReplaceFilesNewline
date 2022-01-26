#include <iostream>
#include <filesystem>
#include <algorithm>
#include <string>
namespace fs = std::filesystem;
void replace_all(std::string& str, const std::string& oldkey, const std::string& newKey) {
	size_t start_pos = 0;
	while ((start_pos = str.find(oldkey, start_pos)) != std::string::npos) {
		str.replace(start_pos, oldkey.length(), newKey);
		start_pos += newKey.length();
	}
}
int main()
{
    auto path = "./";
    std::error_code err;

    for (auto& file : fs::recursive_directory_iterator(path, err)) {
        if (file.status(err).type() == fs::file_type::regular && file.exists(err)) {
            const auto& path = file.path();
            const auto& ex = path.extension();
            if (ex == ".h" || ex == ".cpp" || ex == ".hpp") {
				const auto& filePath = path.string();
				auto pFile = fopen(filePath.c_str(), "rb");
				if (nullptr != pFile) {
                    //read file
					fseek(pFile, 0L, SEEK_END);
					auto size = ftell(pFile);
					fseek(pFile, 0L, SEEK_SET);
					std::string buffStr(size, 0);
					size = fread((void*)buffStr.c_str(), sizeof(char), size, pFile);

					auto tmpStr = buffStr;
                    //replace file
					if (buffStr.find(std::string("\r\0\n\0", 4)) != std::string::npos) {
						replace_all(buffStr, std::string("\r\0\n\0", 4), std::string("\n\0", 2));
					}
					else {
						replace_all(buffStr, "\r\n", "\n");
					}

					if (buffStr.find(std::string("\r\0", 2)) != std::string::npos) {
						replace_all(buffStr, std::string("\r\0", 2), std::string("\n\0", 2));
					}
					else {
						replace_all(buffStr, "\r", "\n");
					}

					if (buffStr.find(std::string("\n\0", 2)) != std::string::npos) {
						replace_all(buffStr, std::string("\n\0", 2), std::string("\r\0\n\0", 4));
					}
					else {
						replace_all(buffStr, "\n", "\r\n");
					}
					
					
					
                    fclose(pFile);

                    //save file
                    if (!buffStr.empty() && tmpStr != buffStr) {
						pFile = fopen(filePath.c_str(), "wb");
						if (nullptr != pFile) {
                            fwrite(buffStr.c_str(), sizeof(char), buffStr.size(), pFile);
                            fclose(pFile);
							std::cout << path << std::endl;
						}
                    }
				}
            }
            
        }
    }
}