#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include "Trie.h"

using std::filesystem::path;

struct RegistroBin // 64 bytes
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

struct RegistroPos // 40 bytes
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
        long offset;
        size_t dniSize;
        file.read(reinterpret_cast<char *>(&offset), sizeof(offset));   // Se lee el offset
        file.read(reinterpret_cast<char *>(&dniSize), sizeof(dniSize)); // Se lee el tamaño del dni
        std::string dni(dniSize, '\0');                                 // Se redimensiona el string dni
        file.read(&dni[0], dniSize);                                    // Se lee el dni
        return RegistroPos(offset, dni);
    }
};

struct Cabecera // 8 bytes
{
    uint64_t num_registros;
    Cabecera(){};
    Cabecera(long num_registros) : num_registros(num_registros) {}
};

/**
 * @brief Funcion para escribir los archivos binarios
 * @param filename Nombre del archivo csv
 * @param cabeceraMain Cabecera del archivo de registros
 * @param cabeceraPos Cabecera del archivo de posiciones
 */
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

        binFile.seekp(sizeof(Cabecera) - 1);
        binFile.write("", 1);
        posBinFile.seekp(sizeof(Cabecera) - 1);
        posBinFile.write("", 1);

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

long buscarOffsetDelRegistro(const std::string &filename, const std::string &dni)
{
    path binname = filename.substr(0, filename.find_last_of('.')) + ".bin";
    path posbinname = filename.substr(0, filename.find_last_of('.')) + "_pos.bin";
    if (!std::filesystem::exists(binname) || !std::filesystem::exists(posbinname))
    {
        std::cerr << "Error: binary files not found" << std::endl;
        return -1;
    }

    std::ifstream binFile(binname, std::ios::binary);
    std::ifstream posBinFile(posbinname, std::ios::binary);
    if (!binFile.is_open() || !posBinFile.is_open())
    {
        std::cerr << "Error: could not open binary files" << std::endl;
        return -1;
    }

    Cabecera cabeceraBin, cabeceraPosBin;
    if (!binFile.read(reinterpret_cast<char *>(&cabeceraBin), sizeof(Cabecera)) || !posBinFile.read(reinterpret_cast<char *>(&cabeceraPosBin), sizeof(Cabecera)))
    {
        std::cerr << "Error: could not read binary file headers" << std::endl;
        return -1;
    }

    Trie trie;
    size_t registrosLeidos = 0;
    size_t cantRegistros = 50000;
    while (registrosLeidos < cabeceraPosBin.num_registros)
    {
        for (size_t i = 0; i < cantRegistros && registrosLeidos < cabeceraPosBin.num_registros; i++)
        {
            std::unique_ptr<RegistroPos> regPos = std::make_unique<RegistroPos>(RegistroPos::readFrom(posBinFile));
            if (!posBinFile.good())
            {
                std::cerr << "Error: could not read position binary file" << std::endl;
                return -1;
            }
            trie.insert(regPos->dni, regPos->offset);
            registrosLeidos++;
        }
        long offset = trie.searchAndGetOffset(dni);
        if (offset != -1)
        {
            return offset;
        }
        trie.clear();
    }
    return -1; // DNI no encontrado
}

// Estrategia para la busqueda de un registro
// 1. Se lee la cabecera del archivo de posiciones
// 2. Se lee la cabecera del archivo de registros
// 3. Se busca el dni en el archivo de posiciones
// 4. Se lee el registro en la posición encontrada
// 5. Se muestra el registro

/**
 * @brief Funcion para buscar y mostrar un registro en el archivo binario
 * @param filename Nombre del archivo csv
 * @param dni DNI a buscar
 */
void buscarRegistro(const std::string &filename, const std::string &dni)
{
    long offset = buscarOffsetDelRegistro(filename, dni);
    if (offset == -1)
    {
        std::cout << "Registro no encontrado" << std::endl;
        return;
    }

    path binname = filename.substr(0, filename.find_last_of('.')) + ".bin";
    std::ifstream binFile(binname, std::ios::binary);
    if (!binFile.is_open())
    {
        std::cerr << "Error: could not open binary file" << std::endl;
        return;
    }

    binFile.seekg(offset);
    RegistroBin reg = RegistroBin::readFrom(binFile);
    std::cout << "Registro encontrado: " << reg.datos << std::endl;
}

// void añadirRegistro(const std::string &filename, const std::string &dni, const std::string &line, Cabecera &cabeceraMain, Cabecera &cabeceraPos)
// {
//     path binname = filename.substr(0, filename.find_last_of('.')) + ".bin";        // Archivo binario con los registros
//     path posbinname = filename.substr(0, filename.find_last_of('.')) + "_pos.bin"; //  Archivo binario con las posición de los registros
//     if (!std::filesystem::exists(binname) || !std::filesystem::exists(posbinname))
//     {
//         std::cerr << "Error: binary files not found" << std::endl;
//         return;
//     }

//     std::ifstream binFile(binname, std::ios::binary);
//     std::ifstream posBinFile(posbinname, std::ios::binary);
//     if (!binFile.is_open() || !posBinFile.is_open())
//     {
//         std::cerr << "Error: could not create binary file" << std::endl;
//         return;
//     }

//     // Lectura de cabeceras
//     if (!binFile.read(reinterpret_cast<char *>(&cabeceraMain), sizeof(Cabecera)) || !posBinFile.read(reinterpret_cast<char *>(&cabeceraPos), sizeof(Cabecera)))
//     {
//         std::cerr << "Error: could not read binary file" << std::endl;
//         return;
//     }
//     binFile.close();
//     posBinFile.close();

//     std::ofstream writeBinFile(binname, std::ios::binary | std::ios::app);
//     std::ofstream writePosBinFile(posbinname, std::ios::binary | std::ios::app);
//     if (!writeBinFile.is_open() || !writePosBinFile.is_open())
//     {
//         std::cerr << "Error: could not create binary file" << std::endl;
//         return;
//     }

//     long currentPos = writeBinFile.tellp();
//     // Serializamos los registros
//     size_t dniSize = dni.size();
//     writePosBinFile.write(reinterpret_cast<const char *>(&currentPos), sizeof(currentPos));
//     writePosBinFile.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
//     writePosBinFile.write(dni.c_str(), dniSize);

//     // Serializamos los registros
//     // Primero, serializamos el dni
//     writeBinFile.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
//     writeBinFile.write(dni.c_str(), dniSize);

//     // Luego, serializamos la linea
//     size_t lineSize = line.size();
//     writeBinFile.write(reinterpret_cast<const char *>(&lineSize), sizeof(lineSize));
//     writeBinFile.write(line.c_str(), lineSize);

//     cabeceraMain.num_registros++;
//     cabeceraPos.num_registros++;
// }
