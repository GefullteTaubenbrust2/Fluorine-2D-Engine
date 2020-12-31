#pragma once
#include "3D Renderer.h"

namespace meshloader {
	uint read_index = 0;
	std::string data;
	std::vector<glm::vec2> coords_pool;
	std::vector<glm::vec3> pos_pool;

	bool isNumber(char c) {
		return (c >= '0' && c <= '9') || c == '.' || c == '-';
	}

	float readNumber() {
		int start = read_index;
		for (; read_index < data.size(); ++read_index) {
			if (isNumber(data[read_index])) {
				start = read_index;
				break;
			}
		}
		int end = read_index;
		for (; read_index < data.size(); ++read_index) {
			if (!isNumber(data[read_index])) {
				end = read_index;
				break;
			}
		}

		std::string substr = data.substr(start, end - start);
		return std::atof(substr.data());
	}

	int readInt() {
		int start = read_index;
		for (; read_index < data.size(); ++read_index) {
			if (isNumber(data[read_index])) {
				start = read_index;
				break;
			}
		}
		int end = read_index;
		for (; read_index < data.size(); ++read_index) {
			if (!isNumber(data[read_index])) {
				end = read_index;
				break;
			}
		}

		std::string substr = data.substr(start, end - start);
		return std::atoi(substr.data());
	}

	void gotoLineEnd() {
		for (; read_index < data.size(); ++read_index) {
			if (data[read_index] == '\n') {
				++read_index;
				return;
			}
		}
	}

	void readLine(fgr::Mesh& mesh) {
		bool data_incoming = false;
		bool indexing = false;

		switch (data[read_index]) {
		case '#':
			for (; read_index < data.size(); ++read_index) {
				if (data[read_index] == '\n') {
					++read_index;
					return;
				}
			}
			return;
		case '\n':
			++read_index;
			return;
		case 'v':
			data_incoming = true;
			break;
		case 'f':
			indexing = true;
			break;
		}
		++read_index;
		if (read_index >= data.size()) return;

		if (data_incoming) {
			switch (data[read_index]) {
			case 't': {
				float x = readNumber();
				float y = readNumber();
				glm::vec2 coord = glm::vec2(x, y);
				gotoLineEnd();
				coords_pool.push_back(coord);
				break;
			}
			case ' ': {
				float x = readNumber();
				float y = readNumber();
				float z = readNumber();
				glm::vec3 pos = glm::vec3(x, y, z);
				gotoLineEnd();
				pos_pool.push_back(pos);
				break;
			}
			default:
				return;
			}
		} else if (indexing) {
			int i0 = readInt() - 1;
			int t0 = readInt() - 1;
			int i1 = readInt() - 1;
			int t1 = readInt() - 1;
			int i2 = readInt() - 1;
			int t2 = readInt() - 1;
			gotoLineEnd();

			fgr::Vertex3D v0 = fgr::Vertex3D(pos_pool[i0], glm::vec3(1.), glm::vec3(0.), coords_pool[t0]);
			fgr::Vertex3D v1 = fgr::Vertex3D(pos_pool[i1], glm::vec3(1.), glm::vec3(0.), coords_pool[t1]);
			fgr::Vertex3D v2 = fgr::Vertex3D(pos_pool[i2], glm::vec3(1.), glm::vec3(0.), coords_pool[t2]);

			glm::vec3 normal = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));

			v0.normal = normal;
			v1.normal = normal;
			v2.normal = normal;

			mesh.indices.push_back(mesh.vertices.size());
			mesh.indices.push_back(mesh.vertices.size() + 1);
			mesh.indices.push_back(mesh.vertices.size() + 2);
			mesh.vertices.push_back(v0);
			mesh.vertices.push_back(v1);
			mesh.vertices.push_back(v2);
		}
	}

	void loadMesh(fgr::Mesh& mesh, std::string& str) {
		read_index = 0;
		data = str;

		coords_pool.clear();
		pos_pool.clear();

		mesh.vertices.clear();
		mesh.indices.clear();

		while (read_index < data.size()) {
			readLine(mesh);
		}
	}
}