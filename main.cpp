#include <iostream>
#include <fstream>
#include <vector>

// Cabecera tiene un tamaño, dicho tamaño se usa para caminar por el archivo. Se tiene que poner toda la estructura del nodo del arbol b en ella.
// sizeof(cabecera) * offset siendo offset el numero de posicion de lo que quieras encontrar.
// https://aprendeaprogramar.com/cursos/verApartado.php?id=16801
// https://pastebin.com/A450dNmE

// Tabla hash encadenada tal que pueda localizar el offset

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
        binFile.write(reinterpret_cast<const char *>(&line), sizeof(line));
    }
    binFile.close();
    return 0;
}
