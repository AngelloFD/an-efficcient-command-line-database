#include <iostream>
#include <algorithm>
#include "BinaryHandler.h"

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
        std::cout << "Ingrese una opcion: " << '\n';
        std::cout << "1. Buscar registro" << '\n';
        std::cout << "2. Agregar registro" << '\n';
        std::cout << "3. Anotar registro" << '\n';
        std::cout << "0. Salir" << '\n';
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
        } while (dni.size() < 8 || !std::all_of(dni.begin(), dni.end(), ::isdigit)); // Validar que el dni tenga 8 caracteres (numeros)
        std::cout << "Ingrese los datos a agregar separados por comas: ";
        std::cin.ignore();
        std::getline(std::cin, line);
        addRegistro(filename, cabeceraMain, cabeceraPos, dni, dni + "," + line);
        break;
    }
    case 3:
    {
        std::string dni;
        do
        {
            std::cout << "Ingrese el dni: ";
            std::cin >> dni;
        } while (dni.size() < 8 || !std::all_of(dni.begin(), dni.end(), ::isdigit)); // Validar que el dni tenga 8 caracteres (numeros)
        noteRegistro(filename, cabeceraMain, cabeceraPos, dni);
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
