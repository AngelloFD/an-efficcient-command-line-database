#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>

// Cabecera tiene un tamaño, dicho tamaño se usa para caminar por el archivo. Se tiene que poner toda la estructura del nodo del arbol b en ella.
// sizeof(cabecera) * offset siendo offset el numero de posicion de lo que quieras encontrar.
// https://aprendeaprogramar.com/cursos/verApartado.php?id=16801
// https://pastebin.com/A450dNmE
// Tabla hash encadenada tal que pueda localizar el offset

struct BinHeader
{
    int num_registros;
    int tam_registro;
    BinHeader(int num_registros, int tam_registro) : num_registros(num_registros), tam_registro(tam_registro) {}
};

struct Registro
{
    std::string dni;
    std::string datos;
    // Constructor para inicializar los atributos de la estructura
    Registro(const std::string &dni, const std::string &datos) : dni(dni), datos(datos) {}
    Registro() = default;
    ~Registro(){};
};

void escribirArchivoBinario(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: file not found" << std::endl;
        return;
    }

    std::string binname = filename.substr(0, filename.find_last_of('.')) + ".bin";
    if (!std::filesystem::exists(binname))
    {
        std::ofstream binFile(binname, std::ios::binary);
        if (!binFile.is_open())
        {
            std::cerr << "Error: could not create binary file" << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line); // Saltamos la primera linea del csv
        BinHeader *cabecera = new BinHeader(0, sizeof(Registro));
        // Escribimos el header dentro del archivo binario
        binFile.write(reinterpret_cast<const char *>(&cabecera), sizeof(cabecera));
        while (std::getline(file, line))
        {
            std::string dni = line.substr(0, line.find(','));
            std::unique_ptr<Registro> reg = std::make_unique<Registro>(dni, line); // Uso de smart pointers para eliminar el Registro al salir del scope
            binFile.write(reinterpret_cast<const char *>(&reg), sizeof(Registro));
            cabecera->num_registros++;
        }
        binFile.close();
        delete cabecera;
    }
    return;
}

// Funcion para leer el archivo binario por bloques de registros
void leerArchivoBinario(const std::string &filename)
{
}

int main()
{
    std::cout << "Ingrese nombre y extension del archivo a leer: ";
    std::string filename;
    std::cin >> filename;

    escribirArchivoBinario(filename);

    return 0;
}
