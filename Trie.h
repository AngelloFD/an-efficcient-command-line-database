#pragma once
#include <iostream>
#include <string>

using namespace std;

// Clase del árbol de prefijos
class Trie
{
private:
	// Nodo del árbol de prefijos
	struct TrieNode
	{
		long posicion = -1;			 // posición de la palabra en el archivo, 8 bytes
		TrieNode *children[10] = {}; // array de punteros a los nodos hijos, 80 bytes

		// Destructor para liberar memoria de los nodos hijos
		~TrieNode()
		{
			for (auto &child : children)
			{
				delete child;
			}
		}
	};

	TrieNode *root; // nodo raíz del Trie

	// Función para liberar memoria de un nodo y sus hijos
	void clearNode(TrieNode *node)
	{
		if (node != nullptr)
		{
			delete node;
		}
	}

public:
	Trie() : root(new TrieNode()) {}

	~Trie()
	{
		clear();
	}

	/**
	 * @brief Insertar una palabra en el árbol de prefijos
	 * @param palabra Palabra a insertar
	 * @param posicion Posición de la palabra en el archivo
	 */
	void insert(const string &palabra, long posicion)
	{
		TrieNode *node = root; // comenzamos el recorrido desde la raíz
		// recorremos cada caracter de la palabra
		for (char c : palabra)
		{
			int index = c - '0'; // Convertir el carácter a índice (0-9)
			// si el caracter no está en el recorrido (hijos) del nodo actual lo agregamos
			if (node->children[index] == nullptr)
			{
				node->children[index] = new TrieNode(); // Crear un nuevo nodo hijo
			}
			node = node->children[index]; // moverse al nodo hijo correspondiente
		}
		node->posicion = posicion; // guardar la posicion de la palabra en el archivo
	}

	/**
	 * @brief Buscar una palabra en el árbol de prefijos
	 * @param palabra Palabra a buscar
	 * @return Posición de la palabra en el archivo si existe, -1 en caso contrario
	 */
	long searchAndGetOffset(const string &palabra)
	{
		TrieNode *node = root;
		for (char c : palabra)
		{
			int index = c - '0'; // Conversion de caracter a índice (0-9) en ascii
			// Si el caracter no está presente entre los hijos
			if (node->children[index] == nullptr)
			{
				return -1; // la palabra no existe en el árbol
			}
			node = node->children[index];
		}
		return node->posicion; // la palabra existe en el árbol
	}

	// Funcion para borrar el trie y todos sus nodos para liberar memoria
	void clear()
	{
		clearNode(root);
		root = new TrieNode();
	}
};
