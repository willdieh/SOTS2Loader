#pragma once

// Include GLM core features
#include "../glm/vec2.hpp" // vec2
#include "../glm/vec3.hpp" // vec3
#include "../glm/mat4x4.hpp" // mat4
#include "../glm/trigonometric.hpp" //radians

// Include GLM extensions
#include "../glm/ext/matrix_transform.hpp" // perspective, translate, rotate
#include "../glm/gtc/type_ptr.hpp" // retrieves pointer to struct data 

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "Materials.h"


struct Vertex
{
	float x, y, z;
	float unknown1;		// always 0.0f??!
	float a, b, c, d;	// always 1.0f??!
	float u, v;
	int i1, i2, i3;		// no IDEA what these are. Not floats. RGBA colors? doesn't seem like it.
	int i4, i5, i6;		// always 0??!
	
	void print();
};

struct TextureInfo
{	
	int unknown, size, fileOffset; 
	std::string name;
};

struct NodeObject
{
	void PrintIndexedVertices();

	// Texture Indexes and Vertexes
	std::vector<int>		nodeIndexes;
	std::vector<Vertex>		nodeVertexes;
	std::vector<TextureInfo>	textures;

	// No idea what this stuff is yet. Trails texture name
	float x1, y1, z1;
	float x2, y2, z2;
	float u1, v1;
	float u2, v2;
};

struct Model
{
	bool Load(std::string& filename, const MaterialDictionary* dictionary);
	bool SaveAsOBJ();

	std::string filename;
	const MaterialDictionary* dictionary;

	// Kerberos specific header stuff (seems useless)
	std::string						kerbLeader;			// "SHYL"
	int								kerbSerialNumber;	// INT? UINT? Two CHARs?
	std::string						kerbType;			// "Scene", etc
		
	// First part of the file contains Nodes and Node info, loaded in this order:
	int								nodeCount;			// Number of Nodes	
	std::vector<int>				nodeParents;		// Parent of each Node, parent's numeric value (RootNode Parent always = -1)
	std::vector<glm::mat4>			nodeMatrixes;		// Transform matrices for each node? Might need to be transposed for glm col order
	std::vector<std::string>		nodeNames;			// Names of Nodes, in order (RootNode always = position 0)

	std::vector<int>		nodeObjectParents;	// Parent(s) of the node object(s)
	std::vector<NodeObject>	nodeObjects;		// Each node object
};

