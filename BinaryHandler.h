#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include "Trie.h"

using std::filesystem::path;

#define MAX_SIZE 80000

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

    void writeTo(std::ofstream &file)
    {
        size_t dniSize = dni.size();
        file.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
        file.write(dni.c_str(), dniSize);

        size_t datosSize = datos.size();
        file.write(reinterpret_cast<const char *>(&datosSize), sizeof(datosSize));
        file.write(datos.c_str(), datosSize);
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

void serializarRegistro(std::ofstream &posBinFile, std::ofstream &binFile, const std::string &dni, const std::string &line)
{
    size_t dniSize = dni.size();
    long currentPos = binFile.tellp();
    posBinFile.write(reinterpret_cast<const char *>(&currentPos), sizeof(currentPos));
    posBinFile.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
    posBinFile.write(dni.c_str(), dniSize);

    // Serializamos los registros para el archivo principal
    // Primero, serializamos el dni
    binFile.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
    binFile.write(dni.c_str(), dniSize);

    // Luego, serializamos la línea
    size_t lineSize = line.size();
    binFile.write(reinterpret_cast<const char *>(&lineSize), sizeof(lineSize));
    binFile.write(line.c_str(), lineSize);
}

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
            throw std::runtime_error("Error: archivo no encontrado");
        }

        std::ofstream binFile(binname, std::ios::binary);
        std::ofstream posBinFile(posbinname, std::ios::binary);
        if (!binFile.is_open() || !posBinFile.is_open())
        {
            throw std::runtime_error("Error: no se pudo abrir el archivo binario");
        }

        binFile.seekp(sizeof(Cabecera) - 1);
        binFile.write("", 1);
        posBinFile.seekp(sizeof(Cabecera) - 1);
        posBinFile.write("", 1);

        std::string line;
        std::getline(file, line); // Saltamos la primera linea del csv
        while (std::getline(file, line))
        {
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            std::string dni = line.substr(0, line.find(','));
            long currentPos = binFile.tellp();

            serializarRegistro(posBinFile, binFile, dni, line);

            // // Serializamos los registros
            // size_t dniSize = dni.size();
            // posBinFile.write(reinterpret_cast<const char *>(&currentPos), sizeof(currentPos));
            // posBinFile.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
            // posBinFile.write(dni.c_str(), dniSize);

            // // Serializamos los registros
            // // Primero, serializamos el dni
            // binFile.write(reinterpret_cast<const char *>(&dniSize), sizeof(dniSize));
            // binFile.write(dni.c_str(), dniSize);

            // // Luego, serializamos la linea
            // size_t lineSize = line.size();
            // binFile.write(reinterpret_cast<const char *>(&lineSize), sizeof(lineSize));
            // binFile.write(line.c_str(), lineSize);

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
        throw std::runtime_error("Error: archivo binario no encontrado");
    }

    std::ifstream binFile(binname, std::ios::binary);
    std::ifstream posBinFile(posbinname, std::ios::binary);
    if (!binFile.is_open() || !posBinFile.is_open())
    {
        throw std::runtime_error("Error: no se pudo abrir el archivo binario");
    }

    Cabecera cabeceraBin, cabeceraPosBin;
    if (!binFile.read(reinterpret_cast<char *>(&cabeceraBin), sizeof(Cabecera)) || !posBinFile.read(reinterpret_cast<char *>(&cabeceraPosBin), sizeof(Cabecera)))
    {
        throw std::runtime_error("Error: no se pudo leer los headers");
    }

    Trie trie;
    size_t registrosLeidos = 0;
    size_t cantRegistros = MAX_SIZE;
    while (registrosLeidos < cabeceraPosBin.num_registros)
    {
        for (size_t i = 0; i < cantRegistros && registrosLeidos < cabeceraPosBin.num_registros; i++)
        {
            std::unique_ptr<RegistroPos> regPos = std::make_unique<RegistroPos>(RegistroPos::readFrom(posBinFile));
            if (!posBinFile.good())
            {
                throw std::runtime_error("Error: no se pudo leer el archivo de posiciones");
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
        throw std::runtime_error("Error: no se pudo abrir el archivo binario");
    }

    binFile.seekg(offset);
    RegistroBin reg = RegistroBin::readFrom(binFile);
    std::cout << "Registro encontrado: " << reg.datos << std::endl;
}

/**
 * @brief Funcion para agregar un nuevo registro al archivo binario
 * @param filename Nombre del archivo csv
 * @param dni DNI a agregar
 * @param newData Datos a agregar
 */
void addRegistro(const std::string &filename, Cabecera &cabeceraMain, Cabecera &cabeceraPos, const std::string &dni, const std::string &newData)
{
    if (buscarOffsetDelRegistro(filename, dni) != -1)
    {
        std::cout << "Registro ya existe" << std::endl;
        return;
    }

    path binname = filename.substr(0, filename.find_last_of('.')) + ".bin";
    path posbinname = filename.substr(0, filename.find_last_of('.')) + "_pos.bin";

    std::ifstream readBinFile(binname, std::ios::binary);
    std::ifstream readPosBinFile(posbinname, std::ios::binary);
    if (!readBinFile.is_open() || !readPosBinFile.is_open())
    {
        throw std::runtime_error("Error: no se pudo abrir el archivo binario");
    }

    if (!readBinFile.read(reinterpret_cast<char *>(&cabeceraMain), sizeof(Cabecera)) || !readPosBinFile.read(reinterpret_cast<char *>(&cabeceraPos), sizeof(Cabecera)))
    {
        throw std::runtime_error("Error: no se pudo leer los headers");
    }

    std::ofstream binFile(binname, std::ios::binary | std::ios::out | std::ios::app);
    std::ofstream posBinFile(posbinname, std::ios::binary | std::ios::out | std::ios::app);
    if (!binFile.is_open() || !posBinFile.is_open())
    {
        throw std::runtime_error("Error: no se pudo abrir el archivo binario");
    }

    // Ir a la ultima posicion del archivo, moverse al final de la linea y escribir el nuevo registro

    serializarRegistro(posBinFile, binFile, dni, newData);
    binFile.close();
    posBinFile.close();

    cabeceraMain.num_registros++;
    cabeceraPos.num_registros++;

    // Reescribir las cabeceras actualizadas
    binFile.open(binname, std::ios::binary | std::ios::out | std::ios::in);
    posBinFile.open(posbinname, std::ios::binary | std::ios::out | std::ios::in);
    if (!binFile.is_open() || !posBinFile.is_open())
    {
        throw std::runtime_error("Error: no se pudo abrir el archivo binario");
    }

    binFile.seekp(0, std::ios::beg);
    binFile.write(reinterpret_cast<const char *>(&cabeceraMain), sizeof(cabeceraMain));
    posBinFile.seekp(0, std::ios::beg);
    posBinFile.write(reinterpret_cast<const char *>(&cabeceraPos), sizeof(cabeceraPos));

    std::cout << "Registro agregado exitosamente" << std::endl;

    binFile.close();
    posBinFile.close();
}

void noteRegistro(const std::string &filename, Cabecera &cabeceraMain, Cabecera &cabeceraPos, const std::string &dni)
{
    long offset = buscarOffsetDelRegistro(filename, dni);
    if (offset == -1)
    {
        std::cout << "Registro no encontrado" << std::endl;
        return;
    }

    path binname = filename.substr(0, filename.find_last_of('.')) + ".bin";

    std::ifstream binFile(binname, std::ios::binary | std::ios::in | std::ios::out);
    if (!binFile.is_open())
    {
        throw std::runtime_error("Error: no se pudo abrir el archivo binario");
    }

    binFile.seekg(offset);
    RegistroBin reg = RegistroBin::readFrom(binFile);
    binFile.close();

    std::ofstream writeBinFile(binname, std::ios::binary | std::ios::in | std::ios::out);
    reg.datos.append(",DESACTIVADO");

    writeBinFile.seekp(offset);
    reg.writeTo(writeBinFile);

    std::cout << "Registro anotado exitosamente" << std::endl;

    writeBinFile.close();
}