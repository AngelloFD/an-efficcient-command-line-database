#include <iostream>
#include <algorithm>
#include "BinaryHandler.h"

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
        std::string dni, line;
        do
        {
            std::cout << "Ingrese el dni a agregar: ";
            std::cin >> dni;
        } while (dni.size() != 8 && !std::all_of(dni.begin(), dni.end(), ::isdigit)); // Validar que el dni tenga 8 caracteres (numeros)
        std::cout << "Ingrese los datos a agregar separados por comas: ";
        std::cin >> line;
        addRegistro(filename, cabeceraMain, cabeceraPos, dni, line);
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
