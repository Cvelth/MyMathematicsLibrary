#pragma once
#include "mml/matrix.hpp"

#include "mml/exceptions.hpp"
DefineNewMMLException(TransformationError)

namespace mml {
	template <typename T, size_t S> class basic_transformation;
	template <typename T> basic_transformation<T, 2> rotation(T const& angle);
	template <typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T, T_O>::value>::type> 
		basic_transformation<T, 3> rotation(T const& angle, basic_vector<T, 3> const& axis);

	template <typename T, size_t S>
	class basic_transformation : public basic_matrix<T, S + 1, S + 1> {
	protected:
		using basic_matrix<T, S + 1, S + 1>::data;
	public:
		using basic_matrix<T, S + 1, S + 1>::basic_matrix;

		template <typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_transformation<T, S> translate(basic_vector<T_O, S> const& direction) {
			for (size_t r = 0; r < S + 1; r++)
				data[r][S] += dot(data[r], direction);
			return *this;
		}
		template <typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_transformation<T, S> scale(basic_vector<T_O, S> const& direction) {
			for (size_t r = 0; r < S + 1; r++)
				data[r] *= direction;
			return *this;
		}
		template <typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<S == 2>::type>
		basic_transformation<T, S> rotate(T_O const& angle) {
			auto res = (rotation<T>(angle) * *this);
			for (size_t r = 0; r < S + 1; r++)
				data[r] = res[r];
			return *this;
		}
		template <typename T_O, typename T_OO, typename = typename std::enable_if<std::is_convertible<T, T_O>::value>::type, 
			typename = typename std::enable_if<std::is_convertible<T, T_OO>::value>::type, typename = typename std::enable_if<S == 3>::type>
		basic_transformation<T, S> rotate(T_O const& angle, basic_vector<T_OO, S> const& axis) {
			auto res = (rotation<T>(angle, axis) * *this);
			for (size_t r = 0; r < S + 1; r++)
				data[r] = res[r];
			return *this;
		}
	};
	
	template <typename T, size_t S, typename = typename std::enable_if<(S > 1)>::type>
	inline basic_transformation<T, S> translation(basic_vector<T, S> const& direction) {
		basic_transformation<T, S> res;
		for (size_t i = 0; i < S; i++)
			res[i][S] = direction[i];
		return res;
	}
	template <typename T, size_t S, typename = typename std::enable_if<(S > 1)>::type>
	inline basic_transformation<T, S> scaling(basic_vector<T, S> const& direction) {
		basic_transformation<T, S> res;
		for (size_t i = 0; i < S; i++)
			res[i][i] = direction[i];
		return res;
	}
	template <typename T>
	inline basic_transformation<T, 3> rotation_x(T const& angle) {
		return {{T(1), T(0), T(0), T(0)},
		{T(0), std::cos(angle), -std::sin(angle), T(0)},
		{T(0), std::sin(angle), std::cos(angle), T(0)},
		{T(0), T(0), T(0), T(1)}};
	}
	template <typename T>
	inline basic_transformation<T, 3> rotation_y(T const& angle) {
		return {{std::cos(angle), T(0), std::sin(angle), T(0)},
		{ T(0), T(1), T(0), T(0) },
		{-std::sin(angle), T(0), std::cos(angle), T(0)},
		{T(0), T(0), T(0), T(1)}};
	}
	template <typename T>
	inline basic_transformation<T, 3> rotation_z(T const& angle) {
		return {{std::cos(angle), -std::sin(angle), T(0), T(0)},
		{ std::sin(angle), std::cos(angle), T(0), T(0) },
		{T(0), T(0), T(1), T(0)},
		{T(0), T(0), T(0), T(1)}};
	}
	template <typename T>
	inline basic_transformation<T, 3> rotation(T const& angle, basic_vector<T, 3> const& axis) {
		T const c = cos(angle);
		T const s = sin(angle);
		auto a = axis.normalized();
		auto t = ((T(1) - c) * a);

		basic_transformation<T, 3> r;
		r[0][0] = c + t[0] * a[0];
		r[1][1] = c + t[1] * a[1];
		r[2][2] = c + t[2] * a[2];
		r[3][3] = T(1);

		r[0][1] = t[1] * a[0] - s * a[2];
		r[0][2] = t[2] * a[0] + s * a[1];
		r[1][0] = t[0] * a[1] + s * a[2];
		r[1][2] = t[2] * a[1] - s * a[0];
		r[2][0] = t[0] * a[2] - s * a[1];
		r[2][1] = t[1] * a[2] + s * a[0];

		return r;
	}
	template <typename T>
	inline basic_transformation<T, 2> rotation(T const& angle) {
		return basic_transformation<T, 2>(rotation_z<T>(angle));
	}

	template <typename T>
	inline basic_transformation<T, 3> perspective_projection(T const& l, T const& r, T const& b, T const& t, T const& n, T const& f) {
		if (l == r || b == t || n == f)
			throw Exceptions::TransformationError();
		return basic_transformation<T, 3>{
			{ n * T(2) / (r - l), T(0), (r + l) / (r - l), T(0) },
			{T(0), n * T(2) / (t - b), (t + b) / (t - b), T(0) },
			{T(0), T(0), -(f + n) / (f - n), -f * n * T(2) / (f - n) },
			{T(0), T(0), T(-1), T(0) }
		};
	}
	template <typename T>
	inline basic_transformation<T, 3> orthographic_projection(T const& l, T const& r, T const& b, T const& t, T const& n, T const& f) {
		if (l == r || b == t || n == f)
			throw Exceptions::TransformationError();
		return basic_transformation<T, 3>{
			{ T(2) / (r - l), T(0), T(0), -(r + l) / (r - l) },
			{ T(0), T(2) / (t - b), T(0), -(t + b) / (t - b) },
			{ T(0), T(0), T(-2) / (f - n), -(f + n) / (f - n) },
			{ T(0), T(0), T(0), T(1) }
		};
	}

	class transformation2f : public basic_transformation<float, 2u> { public: using basic_transformation::basic_transformation; };
	class transformation3f : public basic_transformation<float, 3u> { public: using basic_transformation::basic_transformation; };
	class transformation2d : public basic_transformation<double, 2u> { public: using basic_transformation::basic_transformation; };
	class transformation3d : public basic_transformation<double, 3u> { public: using basic_transformation::basic_transformation; };

	class transformation : public transformation3f { public: using transformation3f::transformation3f; };

	inline auto translation(vector const& direction) {
		return translation<vector::value_type, vector::size_value>(direction);
	}
	inline auto translation(float x, float y, float z) {
		return translation<vector::value_type, vector::size_value>(vector{x,y,z});
	}
	inline auto scaling(vector const& direction) {
		return scaling<vector::value_type, vector::size_value>(direction);
	}
	inline auto scaling(float x, float y, float z) {
		return scaling<vector::value_type, vector::size_value>(vector{x,y,z});
	}
	inline auto rotation(vector::value_type const& angle, vector const& axis) {
		return rotation<vector::value_type, vector::value_type>(angle, axis);
	}
	inline auto rotation(vector::value_type const& angle, float x, float y, float z) {
		return rotation<vector::value_type, vector::value_type>(angle, vector{x,y,z});
	}
	inline auto rotation_x(vector::value_type const& angle) {
		return rotation_x<vector::value_type>(angle);
	}
	inline auto rotation_y(vector::value_type const& angle) {
		return rotation_y<vector::value_type>(angle);
	}
	inline auto rotation_z(vector::value_type const& angle) {
		return rotation_z<vector::value_type>(angle);
	}

	inline auto orthographic_projection(basic_vector<vector2f, 3> const& edges) {
		return orthographic_projection<float>(edges[0][0], edges[0][1], edges[1][0], edges[1][1], edges[2][0], edges[2][1]);
	}
	inline auto orthographic_projection(float left, float right, float bottom, float top, float near, float far) {
		return orthographic_projection<float>(left, right, bottom, top, near, far);
	}
	inline auto perspective_projection(basic_vector<vector2f, 3> const& edges) {
		return perspective_projection<float>(edges[0][0], edges[0][1], edges[1][0], edges[1][1], edges[2][0], edges[2][1]);
	}
	inline auto perspective_projection(float left, float right, float bottom, float top, float near, float far) {
		return orthographic_projection<float>(left, right, bottom, top, near, far);
	}
}