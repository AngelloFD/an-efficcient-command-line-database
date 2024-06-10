#include <iostream>
#include <fstream>
#include <vector>

/**
 * @brief Clase que implementa una tabla hash de tamaño dinámico
 */
class DynmaicHashTable
{
};

int main()
{
    std::ifstream file;
    file.open("testdata.csv");
    if (!file.is_open())
    {
        std::cerr << "Error: file not found" << std::endl;
        return 1;
    }

    std::ofstream binFile;
    binFile.open("testdata.bin", std::ios::binary);

    std::string line;
    std::getline(file, line); // Skip the first line
    while (std::getline(file, line))
    {
        std::string dni = line.substr(0, line.find(','));
        binFile.write(reinterpret_cast<const char *>(&dni), sizeof(dni));
        binFile.write(",", sizeof(char));
        binFile.write(reinterpret_cast<const char *>(&line), sizeof(line));
        binFile.write("\n", sizeof(char));
    }
    binFile.close();
    return 0;
}