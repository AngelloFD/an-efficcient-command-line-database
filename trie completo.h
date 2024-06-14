#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

// Nodo del árbol de prefijos
struct TrieNode
{
	int posicion;							  // posición de la palabra en el archivo
	unordered_map<char, TrieNode *> children; // mapa para almacenar los nodos hijos

	TrieNode() : posicion(-1) {} // Constructor
};

// Estructura Trie que manejará el árbol de prefijos:

struct Trie
{
	TrieNode *root; // nodo raíz del Trie

	// Constructor
	Trie()
	{
		root = new TrieNode(); // Inicializa el nodo raíz
	}

	~Trie() {}

	// insertar una palabra en el trie
	void insert(const string &palabra, int posicion)
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
};
