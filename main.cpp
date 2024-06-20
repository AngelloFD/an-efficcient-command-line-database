#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include "trie completo.h"

// Cabecera tiene un tamaño, dicho tamaño se usa para caminar por el archivo. Se tiene que poner toda la estructura del nodo del arbol b en ella.
// sizeof(cabecera) * offset siendo offset el numero de posicion de lo que quieras encontrar.
// https://aprendeaprogramar.com/cursos/verApartado.php?id=16801
// https://pastebin.com/A450dNmE
// Tabla hash encadenada tal que pueda localizar el offset

using std::filesystem::path;

struct RegistroBin
{
    std::string dni;
    std::string datos;
    // Constructor para inicializar los atributos de la estructura
    RegistroBin(const std::string &dni, const std::string &datos) : dni(dni), datos(datos) {}
    RegistroBin() = default;
    ~RegistroBin(){};

    // Deserializacion de un RegistroBin desde un archivo binario
    static RegistroBin readFrom(std::ifstream &file)
    {
        RegistroBin reg;
        size_t dniSize, datosSize;
        file.read(reinterpret_cast<char *>(&dniSize), sizeof(dniSize)); // Se lee el tamaño del dni
        reg.dni.resize(dniSize);                                        // Se redimensiona el string dni
        file.read(&reg.dni[0], dniSize);                                // Se lee el dni
        file.read(reinterpret_cast<char *>(&datosSize), sizeof(datosSize));
        reg.datos.resize(datosSize);
        file.read(&reg.datos[0], datosSize);
        return reg;
    }
};

struct RegistroPos
{
    long offset;
    std::string dni;
    // Constructor para inicializar los atributos de la estructura
    RegistroPos(std::streampos offset, const std::string &dni) : offset(offset), dni(dni) {}
    RegistroPos() = default;
    ~RegistroPos(){};

    // Deserializacion de un RegistroPos desde un archivo binario
    static RegistroPos readFrom(std::ifstream &file)
    {
        RegistroPos reg;
        size_t dniSize;
        file.read(reinterpret_cast<char *>(&dniSize), sizeof(dniSize));       // Se lee el tamaño del dni
        reg.dni.resize(dniSize);                                              // Se redimensiona el string dni
        file.read(&reg.dni[0], dniSize);                                      // Se lee el dni
        file.read(reinterpret_cast<char *>(&reg.offset), sizeof(reg.offset)); // Se lee el offset
        return reg;
    }
};

struct Cabecera
{
    uint64_t num_registros;
    Cabecera(){};
    Cabecera(long num_registros) : num_registros(num_registros) {}
};

// Funcion para obtener el tamaño de un archivo
std::uintmax_t GetFileSize(const std::string &filename)
{
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    if (!in.is_open())
    {
        std::cerr << "Error al abrir el archivo para obtener el tamanio." << std::endl;
        return -1;
    }
    std::uintmax_t fileSize = in.tellg();
    in.close();
    std::cout << "Tamanio del archivo: " << fileSize << " bytes" << std::endl;
    return fileSize;
}

void escribirArchivosBinario(const std::string &filename, Cabecera &cabeceraMain, Cabecera &cabeceraPos)
{
    path binname = filename.substr(0, filename.find_last_of('.')) + ".bin";        // Archivo binario con los registros
    path posbinname = filename.substr(0, filename.find_last_of('.')) + "_pos.bin"; //  Archivo binario con las posición de los registros
    if (!std::filesystem::exists(binname))
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: file not found" << std::endl;
            return;
        }

        std::ofstream binFile(binname, std::ios::binary);
        std::ofstream posBinFile(posbinname, std::ios::binary);
        if (!binFile.is_open() || !posBinFile.is_open())
        {
            std::cerr << "Error: could not create binary file" << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line); // Saltamos la primera linea del csv
        while (std::getline(file, line))
        {
            std::string dni = line.substr(0, line.find(','));
            long currentPos = binFile.tellp();

            // Serializamos los registros
            size_t dniSize = dni.size();
            posBinFile.write(reinterpret_cast<const char *>(&currentPos), sizeof(currentPos));
            posBinFile.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
            posBinFile.write(dni.c_str(), dniSize);

            // Serializamos los registros
            // Primero, serializamos el dni
            binFile.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
            binFile.write(dni.c_str(), dniSize);

            // Luego, serializamos la linea
            size_t lineSize = line.size();
            binFile.write(reinterpret_cast<const char *>(&lineSize), sizeof(lineSize));
            binFile.write(line.c_str(), lineSize);

            cabeceraMain.num_registros++;
            cabeceraPos.num_registros++;
        }
        // Volvemos al inicio del archivo para escribir la cabecera
        binFile.seekp(0, std::ios::beg);
        binFile.write(reinterpret_cast<const char *>(&cabeceraMain), sizeof(cabeceraMain));
        posBinFile.seekp(0, std::ios::beg);
        posBinFile.write(reinterpret_cast<const char *>(&cabeceraPos), sizeof(cabeceraPos));
        binFile.close();
        posBinFile.close();
    }
    return;
}

// Estrategia para la busqueda de un registro
// 1. Se lee la cabecera del archivo de posiciones
// 2. Se lee la cabecera del archivo de registros
// 3. Se busca el dni en el archivo de posiciones
// 4. Se lee el registro en la posición encontrada
// 5. Se muestra el registro

void buscarRegistro(const std::string &filename, const std::string &dni)
{
    path binname = filename.substr(0, filename.find_last_of('.')) + ".bin";        // Archivo binario con los registros
    path posbinname = filename.substr(0, filename.find_last_of('.')) + "_pos.bin"; //  Archivo binario con las posición de los registros
    if (!std::filesystem::exists(binname) || !std::filesystem::exists(posbinname))
    {
        std::cerr << "Error: binary files not found" << std::endl;
        return;
    }

    std::ifstream binFile(binname, std::ios::binary);
    std::ifstream posBinFile(posbinname, std::ios::binary);
    if (!binFile.is_open() || !posBinFile.is_open())
    {
        std::cerr << "Error: could not open binary files" << std::endl;
        return;
    }

    // Lectura de cabeceras
    Cabecera cabeceraBin, cabeceraPosBin;
    if (!binFile.read(reinterpret_cast<char *>(&cabeceraBin), sizeof(Cabecera)) || !posBinFile.read(reinterpret_cast<char *>(&cabeceraPosBin), sizeof(Cabecera)))
    {
        std::cerr << "Error: could not read binary file" << std::endl;
        return;
    }
    std::cout << "Numero de registros: " << cabeceraBin.num_registros << std::endl;
    std::cout << "Numero de registros de posicion: " << cabeceraPosBin.num_registros << std::endl;

    // Estrategia de busqueda
    // 1. Se lee la cabecera del archivo de posiciones
    // 2. Se lee la cabecera del archivo de registros
    // 3. Se crea un buffer para almacenar una cantidad de registros de posicion
    // 4. Se coloca cada registro de posicion en el buffer
    // 5. Se coloca cada registro de posicion en el buffer en un arbol trie
    // 6. Se busca el dni en el arbol trie
    // 7. Si es que se encuentra, se lee la posicion y muestra el registro en la posición encontrada
    // 8. Si no se encuentra, se vacia el buffer y se vuelve a rellenar con la siguiente cantidad de registros de posicion
    // 9. Se repite el proceso hasta que se encuentre el dni o se haya recorrido todos los registros de posicion, cuyo caso devuelve un mensaje de que no fue encontrado el dni

    // Lectura de registros de posicion
    Trie trie;
    size_t registrosLeidos = 0;
    size_t bufferSize = 125000000; // 1GB de registros de posicion
    bool encontrado = false;
    while (registrosLeidos < cabeceraPosBin.num_registros)
    {
        std::vector<RegistroPos> buffer;
        buffer.reserve(bufferSize);
        for (size_t i = 0; i < bufferSize && registrosLeidos < cabeceraPosBin.num_registros; i++)
        {
            RegistroPos regPos = RegistroPos::readFrom(posBinFile);
            if (!posBinFile.good())
            {
                std::cerr << "Error: could not read binary file" << std::endl;
                break;
            }
            buffer.emplace_back(regPos);
            registrosLeidos++;
        }
        trie.clear();
        for (const auto &regPos : buffer)
        {
            trie.insert(regPos.dni, regPos.offset);
        }
        // Busqueda del dni
        long offset = trie.searchAndGetOffset(dni);
        if (offset != -1)
        {
            binFile.seekg(offset);
            RegistroBin reg = RegistroBin::readFrom(binFile);
            std::cout << "Registro encontrado: " << reg.datos << std::endl;
            encontrado = true;
            break;
        }
        buffer.clear();
    }
    if (!encontrado)
    {
        std::cout << "Registro no encontrado" << std::endl;
    }
    posBinFile.close();
    binFile.close();
    return;
}

int main()
{

    std::cout << "Ingrese nombre y extension del archivo a leer: ";
    std::string filename;
    std::cin >> filename;

    Cabecera cabeceraMain, cabeceraPos;

    escribirArchivosBinario(filename, cabeceraMain, cabeceraPos);

    int n;
    do
    {
        std::cout << "Ingrese una opcion: " << std::endl;
        std::cout << "1. Buscar registro" << std::endl;
        std::cout << "2. Agregar registro" << std::endl;
        std::cout << "3. Eliminar registro" << std::endl;
        std::cout << "0. Salir" << std::endl;
        std::cin >> n;
    } while (n < 0 || n > 3);

    switch (n)
    {
    case 1:
    {
        std::string dni;
        std::cout << "Ingrese el dni a buscar: ";
        std::cin >> dni;
        buscarRegistro(filename, dni);
        break;
    }
    case 2:
    {
        // Agregar registro
        break;
    }
    case 3:
    {
        // Eliminar registro
        break;
    }
    case 0:
    {
        break;
    }
    default:
        break;
    }

    return 0;
}
