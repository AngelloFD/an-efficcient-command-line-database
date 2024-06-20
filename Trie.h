#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

// Nodo del árbol de prefijos
class Trie
{
private:
	struct TrieNode
	{
		long posicion = -1;						  // posición de la palabra en el archivo
		unordered_map<char, TrieNode *> children; // mapa para almacenar los nodos hijos

		// Destructor para liberar memoria de los nodos hijos
		~TrieNode()
		{
			for (auto &p : children)
			{
				delete p.second;
			}
		}
	};

	TrieNode *root; // nodo raíz del Trie

	void clearNode(TrieNode *node)
	{
		if (node != nullptr)
		{
			delete node;
		}
	}

public:
	// Constructor
	Trie() : root(new TrieNode()) {}

	~Trie()
	{
		clear();
	}

	// insertar una palabra en el trie
	void insert(const string &palabra, long posicion)
	{
		TrieNode *node = root; // comenzamos el recorrido desde la raíz
		// recorremos cada caracter de la palabra
		for (char c : palabra)
		{
			// si el caracter no está en el recorrido (hijos) del nodo actual
			// lo agregamos
			if (node->children.find(c) == node->children.end())
			{
				node->children[c] = new TrieNode(); // Crear un nuevo nodo hijo
			}
			node = node->children[c]; // moverse al nodo hijo correspondiente
		}
		node->posicion = posicion; // guardar la posicion de la palabra en el archivo
	}

	bool search(const string &palabra)
	{
		TrieNode *node = root;
		for (char c : palabra)
		{
			// Si el caracter no está presente entre los hijos
			if (node->children.find(c) == node->children.end())
			{
				return false; // la palabra no existe en el árbol
			}
			node = node->children[c];
		}
		return node != nullptr && node->posicion != -1; // la palabra existe en el árbol
	}

	long searchAndGetOffset(const string &palabra)
	{
		TrieNode *node = root;
		for (char c : palabra)
		{
			// Si el caracter no está presente entre los hijos
			if (node->children.find(c) == node->children.end())
			{
				return -1; // la palabra no existe en el árbol
			}
			node = node->children[c];
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
