#include <iostream>

#include "Matrices.h"

namespace flo {
	glm::mat3 scale(const glm::mat3& matrix, glm::vec2 scale) {
		const glm::mat3 transform = glm::mat3(
			scale.x, 0.0, 0.0,
			0.0, scale.y, 0.0,
			0.0, 0.0, 1.0
			);
		return transform * matrix;
	}

	glm::mat3 rotate(const glm::mat3& matrix, const float angle) {
		return rotate(matrix, std::cos(angle), std::sin(angle));
	}

	glm::mat3 rotate(const glm::mat3& matrix, const float cos, const float sin) {
		const glm::mat3 transform = glm::mat3(
			cos, sin, 0.0,
			-sin, cos, 0.0,
			0.0, 0.0, 1.0
		);
		return transform * matrix;
	}

	glm::mat3 translate(const glm::mat3& matrix, glm::vec2 offset) {
		const glm::mat3 transform = glm::mat3(
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			offset.x, offset.y, 1.0
		);
		return transform * matrix;
	}

	glm::mat3 scale_and_translate(const glm::vec2& scale, const glm::vec2& offset) {
		return glm::mat3(
			scale.x, 0.0, 0.0,
			0.0, scale.y, 0.0,
			offset.x, offset.y, 1.0
		);
	}

	glm::mat3 scale_and_translate_and_rotate(const glm::vec2& scale, const glm::vec2& offset, const glm::vec2& direction) {
		return glm::mat3(
			scale.x * direction.x, direction.y * scale.x, 0.0,
			-direction.y * scale.y, scale.y * direction.x, 0.0,
			offset.x, offset.y, 1.0
		);
	}

	glm::mat3 generate_tsrt_matrix(const glm::vec2& ioffset, const glm::vec2& scale, const glm::vec2& direction, const glm::vec2& offset) {
		glm::vec2 io = scale * ioffset;
		return glm::mat3(
			scale.x * direction.x, direction.y * scale.x, 0.0,
			-direction.y * scale.y, scale.y * direction.x, 0.0,
			offset.x + io.x * direction.x - io.y * direction.y, offset.y + io.y * direction.x + io.x * direction.y, 1.0
		);
	}

	glm::mat3 setDepth(glm::mat3 matrix, float depth) {
		matrix[2].z = depth;
		return matrix;
	}

	ScaleMode::ScaleMode(ScaleModes mode, ScaleModes rounding, short base_width, short base_height, float base_scale, char pixel_size) :
		mode(mode), rounding(rounding), base_width(base_width), base_height(base_height), base_scale(base_scale), pixel_size(pixel_size) {
	}

	glm::vec2 applyWindowScale(const int width, const int height, const ScaleMode& scalemode) {
		float _scale = 1.0;
		const int base_width = scalemode.base_width;
		const int base_height = scalemode.base_height;
		const float w = fixPixelScale(width, scalemode.pixel_size);
		const float h = fixPixelScale(height, scalemode.pixel_size);

		switch (scalemode.mode) {
		case scale_with_width:
			_scale = 1.0;
			break;
		case scale_with_height:
			_scale = h / w;
			break;
		case scale_with_diagonal:
			_scale = std::sqrt(w*w + h*h) / w;
			break;
		case scale_with_largest:
			if (width > height) _scale = 1.0;
			else _scale = h / w;
			break;
		case scale_with_smallest:
			if (width < height) _scale = 1.0;
			else _scale = h / w;
			break;
		case constant_width:
			_scale = (float)base_width / w;
			break;
		case constant_height:
			_scale = (float)base_height / w;
			break;
		}

		const float scale_const = (float)base_width / w;
		float sc = 0.0;
		switch (scalemode.rounding) {
		case round_to_smallest: 
			sc = std::floor(_scale / scale_const);
			_scale = (sc > 0 ? sc : 1) * scale_const;
			break;
		case round_to_largest:
			sc = std::ceil(_scale / scale_const);
			_scale = (sc > 0 ? sc : 1) * scale_const;
			break;
		case round_to_nearest:
			sc = std::ceil(_scale / scale_const - 0.5);
			_scale = (sc > 0 ? sc : 1) * scale_const;
			break;
		}

		glm::vec2 size = glm::vec2(_scale, _scale * w / h) * ((float)scalemode.base_scale);

		return size;
	}

	glm::vec2 centerWindowOrigin(const int width, const int height) {
		return glm::vec2(width % 2 ? 0.5 / (float)width : 0.0, height % 2 ? 0.5 / (float)height : 0.0);
	}

	int getPixelSize(const int width, const int height, const ScaleMode& scalemode) {
		float _scale = 1.0;
		const int base_width = scalemode.base_width;
		const int base_height = scalemode.base_height;
		const float w = fixPixelScale(width, scalemode.pixel_size);
		const float h = fixPixelScale(height, scalemode.pixel_size);

		switch (scalemode.mode) {
		case scale_with_width:
			_scale = 1.0;
			break;
		case scale_with_height:
			_scale = h / w;
			break;
		case scale_with_diagonal:
			_scale = std::sqrt(w * w + h * h) / w;
			break;
		case scale_with_largest:
			if (width > height) _scale = 1.0;
			else _scale = h / w;
			break;
		case scale_with_smallest:
			if (width < height) _scale = 1.0;
			else _scale = h / w;
			break;
		case constant_width:
			_scale = (float)base_width / w;
			break;
		case constant_height:
			_scale = (float)base_height / h;
			break;
		}

		const float scale_const = (float)base_width / w;
		float sc = 0.0;
		switch (scalemode.rounding) {
		case round_to_smallest:
			sc = std::floor(_scale / scale_const);
			break;
		case round_to_largest:
			sc = std::ceil(_scale / scale_const);
			break;
		case round_to_nearest:
			sc = std::ceil(_scale / scale_const - 0.5);
			break;
		}
		return sc > 0 ? sc : 1;
	}

	glm::vec2 fixToPixelPerfection(glm::vec2& pos, int pixelsize, float basescale) {
		return glm::vec2(std::fmod(pos.x, basescale / pixelsize), std::fmod(pos.y, basescale / pixelsize));
	}

	int extractRelevantBase(long long scalemode) {
		const int base_width = (scalemode >> 32) & 0xffff;
		const int base_height = (scalemode >> 48) & 0xffff;
		if (scalemode & 15 == constant_height) return base_height;
		return base_width;
	}

	int fixPixelScale(int width, int pixel_size) {
		if (pixel_size <= 1) return width;
		return (width - (width % pixel_size));
	}
}