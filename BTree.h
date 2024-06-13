#pragma once
#include <cstdlib>
#include <iostream>

#define G 50                 // grado minimo del arbol. G-1 = # minimo de claves en un nodo y 2*G-1 = maximo
#define CLAVES_MAX 2 * G - 1 // cantidad maxima de claves en un nodo

struct BTree
{
    int keys[CLAVES_MAX]; // arreglo para almacenar hasta 2*G-1 claves en cada nodo
    BTree *C[2 * G];      // arreglo de punteros hacia los hijos del nodo
    int n;                // numero actual de claves
    bool hoja;            // indica si el nodo actual es o no una hoja
    std::string line;     // linea de texto asociada a la clave
};

/**
 * @brief Inicializa un nodo
 * @param hoja Indica si el nodo es hoja o no
 */
BTree *initNode(bool hoja)
{
    BTree *node = new BTree; // un nuevo nodo
    node->hoja = hoja;       // establece si el nuevo nodo es una hoja o no
    node->n = 0;             // cantidad inicial de claves del nodo (valores llenos)

    // Inicializamos todos los hijos del nodo en null
    for (int i = 0; i < 2 * G; i++)
    {
        node->C[i] = nullptr;
    }
    return node; // Devuelve el nuevo nodo inicializado
}

/**
 * @brief Busca una clave en el arbol
 * @param root Raiz del arbol
 * @param k Clave a buscar
 */
bool boolSearch(BTree *root, int k)
{
    // Busca una clave en el arbol
    int i = 0;                               // indice de la primera clave mayor o igual a k
    while (i < root->n && k > root->keys[i]) // Si el indice de la clave es menor que el numero de claves y la clave es mayor que la clave actual, avanza
    {
        i++;
    }
    if (i < root->n && k == root->keys[i]) // Si el indice es menor que el numero de claves y la clave es igual a la clave actual, se ha encontrado
    {
        // Escribe la linea de texto asociada a la clave
        std::cout << root->line << std::endl;
        return true;
    }
    if (root->hoja) // Si el nodo es una hoja, la clave no se encuentra
    {
        std::cout << "El DNI " << k << " no se encuentra en la base de datos\n";
        return false;
    }
    return boolSearch(root->C[i], k); // Si no es una hoja, busca en el hijo correspondiente
}

/**
 * @brief Divide un hijo lleno de un nodo teniendo en cuenta las lineas de texto de cada clave
 * @param x Nodo padre
 * @param i Indice del hijo lleno
 * @param y Nodo hijo lleno
 */
void splitChild(BTree *x, int i, BTree *y)
{
    // Crea un nuevo nodo que contendra (G-1) claves de y
    BTree *z = initNode(y->hoja);
    z->n = G - 1; // Establece la cantidad de claves en el nuevo nodo

    // Copia las ultimas (G-1) claves de y a z
    for (int j = 0; j < G - 1; j++)
    {
        z->keys[j] = y->keys[j + G];
        z->line = y->line;
    }

    // Copia los ultimos G hijos de y a z
    if (!y->hoja)
    {
        for (int j = 0; j < G; j++)
        {
            z->C[j] = y->C[j + G];
        }
    }

    // Reduce la cantidad de claves en y
    y->n = G - 1;

    // Crea espacio para un nuevo hijo en x
    for (int j = x->n; j >= i + 1; j--)
    {
        x->C[j + 1] = x->C[j];
    }

    // Conecta el nuevo hijo a x
    x->C[i + 1] = z;

    // Un elemento de y se mueve a x
    // Encuentra la posicion de la clave que se movera a x
    for (int j = x->n - 1; j >= i; j--)
    {
        x->keys[j + 1] = x->keys[j];
    }

    // Copia la clave media de y a x
    x->keys[i] = y->keys[G - 1];
    x->line = y->line;

    // Incrementa la cantidad de claves en x
    x->n++;
}

/**
 * @brief Inserta una clave en un nodo no lleno
 * @param x Nodo en el que se insertara la clave
 * @param k Clave a insertar
 */
void insertNotFull(BTree *x, int k, std::string line)
{
    int i = x->n - 1; // indice del ultimo elemento del nodo

    // Si el nodo en el que estamos es una hoja
    if (x->hoja)
    {
        // Mueve todas las claves mayores que el valor k una posicion hacia
        // adelante para crear un espacio para la nueva clave k
        while (i >= 0 && x->keys[i] > k)
        {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        x->keys[i + 1] = k; // Insertamos la nueva clave k en la posicion correcta que encontramos
        x->n++;             // Incrementamos el contador de claves del nodo
        x->line = line;     // Asignamos la linea de texto
    }
    else
    { // si el nodo no es una hoja, encuentra el hijo que debe contener la clave
        while (i >= 0 && x->keys[i] > k)
        {
            i--; // retrocedemos hasta encontrar el indice correcto
        }
        // i+1 es el indice del hijo que podra contener a la nueva clave k
        // comprobamos si este hijo esta lleno
        if (x->C[i + 1]->n == CLAVES_MAX)
        {
            // si el hijo esta lleno, debemos dividirlo
            splitChild(x, i + 1, x->C[i + 1]);
            // Despues de dividir, el valor medio se sube
            // actualizamos el indice de insercion:
            if (k > x->keys[i + 1])
            {
                i++;
            }
        }
        // Realiza la insercion recursiva
        insertNotFull(x->C[i + 1], k, line);
    }
}

/**
 * @brief Inserta una clave en el arbol
 * @param root Raiz del arbol
 * @param k Clave a insertar
 * @todo Antes de insertar, se debe verificar si la clave ya existe en el arbol
 */
void insert(BTree *&root, int k, std::string line)
{
    // Si el arbol esta vacio
    if (root == nullptr)
    {
        // Crea un nuevo nodo
        root = initNode(true);
        root->keys[0] = k; // Inserta la clave k en el nodo
        root->n = 1;       // Incrementa la cantidad de claves en el nodo
        root->line = line; // Asigna la linea de texto
    }
    else
    {
        // Si la raiz esta llena, el arbol crece en altura
        if (root->n == CLAVES_MAX)
        {
            // Crea un nuevo nodo
            BTree *s = initNode(false);
            // Hace que el antiguo nodo sea hijo del nuevo nodo
            s->C[0] = root;
            // Divide el nodo antiguo y mueve una clave a s
            splitChild(s, 0, root);
            // Ahora s tiene dos hijos. Decide cual de los dos hijos tendra la nueva clave
            int i = 0;
            if (s->keys[0] < k)
            {
                i++;
            }
            insertNotFull(s, k, line);
            // Cambia la raiz
            root = s;
        }
        else
        {
            // Si la raiz no esta llena, inserta la clave en la raiz
            insertNotFull(root, k, line);
        }
    }
}

// void search(BTree *root, int k);

/**
 * @brief Busca una clave en el arbol y devuelve la linea de texto asociada
 * @param root Raiz del arbol
 * @param k Clave a buscar
 * @return Linea de texto asociada a la clave
 */
// void BTree::search(BTree *root, int k)
// {
//     // Busca una clave en el arbol
//     int i = 0;                               // indice de la primera clave mayor o igual a k
//     while (i < root->n && k > root->keys[i]) // Si el indice de la clave es menor que el numero de claves y la clave es mayor que la clave actual, avanza
//     {
//         i++;
//     }
//     if (i < root->n && k == root->keys[i]) // Si el indice es menor que el numero de claves y la clave es igual a la clave actual, se ha encontrado
//     {
//         std::cout << "Linea: " << root->line << std::endl;
//         return;
//     }
//     if (root->hoja) // Si el nodo es una hoja, la clave no se encuentra
//     {
//         std::cout << "La clave " << k << " no se encuentra en el arbol\n";
//         return;
//     }
//     search(root->C[i], k); // Si no es una hoja, busca en el hijo correspondiente
// }
