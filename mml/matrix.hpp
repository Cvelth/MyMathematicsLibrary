#pragma once
#include "mml/vector.hpp"

#include "mml/exceptions.hpp"
DefineNewMMLException(MatrixIndexOutOfBounds);

namespace mml {
	enum MatrixValue { ZeroMatrix = 0, IdentityMatrix = 1 };
	template<typename T, size_t R, size_t C>
	class basic_matrix : public basic_vector<basic_vector<T, C>, R> {
	public:
		using row_type = basic_vector<T, C>;
		using base_type = basic_vector<row_type, R>;
	protected:
		using base_type::data;
	protected:
		template <typename... Tail>
		void set_values(size_t r, size_t c) {}
		template <typename... Tail>
		void set_values(size_t r, size_t c, typename std::enable_if<sizeof...(Tail) + 1 <= R * C, T>::type const& head, Tail ...tail) {
			data[r][c] = head;
			if (++c == C) {
				++r;
				c = 0u;
			}
			set_values(r, c, T(tail)...);
		}

		template <typename... Tail>
		void set_rows(size_t r) {}
		template <typename... Tail>
		void set_rows(size_t r, typename std::enable_if<sizeof...(Tail) + 1 <= R, row_type>::type const& head, Tail ...tail) {
			data[r] = head;
			set_rows(++r, row_type(tail)...);
		}
	public:
		basic_matrix(MatrixValue mv = IdentityMatrix) : basic_vector<basic_vector<T, C>, R>() {
			if (mv == IdentityMatrix)
			for (size_t i = 0; i < std::min(R, C); i++)
				data[i][i] = T(1);
		}
		basic_matrix(basic_matrix<T, R, C> const& other) : basic_vector<basic_vector<T, C>, R>(other) {}
		basic_matrix(basic_matrix<T, R, C> &&other) : basic_vector<basic_vector<T, C>, R>(other) {}
		template <typename... Tail>
		basic_matrix(typename std::enable_if<sizeof...(Tail) + 1 <= R * C, T>::type const& head = T(0), Tail... tail) : basic_vector<basic_vector<T, C>, R>() {
			set_values(0, 0, head, tail...);
		}
		template <typename... Tail>
		basic_matrix(typename std::enable_if<sizeof...(Tail) + 1 <= R, row_type>::type const& head = row_type(0), Tail... tail) : basic_vector<basic_vector<T, C>, R>() {
			set_rows(0, head, tail...);
		}
		basic_matrix(std::initializer_list<std::initializer_list<T>> const& list) : basic_vector<basic_vector<T, C>, R>() {
			if (list.size() > R)
				throw Exceptions::MatrixIndexOutOfBounds("Too many inputs.");
			size_t r = 0;
			for (auto &it : list) {
				if (it.size() > C)
					throw Exceptions::MatrixIndexOutOfBounds("Too many inputs.");
				data[r++] = row_type(it);
			}
		}
		basic_matrix(std::initializer_list<std::initializer_list<T>> &&list) : basic_vector<basic_vector<T, C>, R>() {
			if (list.size() > R)
				throw Exceptions::MatrixIndexOutOfBounds("Too many inputs.");
			size_t r = 0;
			for (auto &it : list) {
				if (it.size() > C)
					throw Exceptions::MatrixIndexOutOfBounds("Too many inputs.");
				data[r++] = row_type(it);
			}
		}
		basic_matrix(std::initializer_list<T> const& list) : basic_vector<basic_vector<T, C>, R>() {
			if (list.size() > R)
				throw Exceptions::MatrixIndexOutOfBounds("Too many inputs.");
			size_t r = 0, c = 0;
			for (auto &it : list) {
				data[r][c] = it;
				if (++c == C) {
					++r;
					c = 0;
				}
			}
		}
		basic_matrix(std::initializer_list<T> &&list) : basic_vector<basic_vector<T, C>, R>() {
			if (list.size() > R * C)
				throw Exceptions::MatrixIndexOutOfBounds("Too many inputs.");
			size_t r = 0, c = 0;
			for (auto &it : list) {
				data[r][c] = it;
				if (++c == C) {
					++r;
					c = 0;
				}
			}
		}

		template<typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_matrix(basic_matrix<T_O, R_O, C_O> const& other, typename std::enable_if<(R_O <= R && C_O <= C), void*>::type less = nullptr) : basic_vector<basic_vector<T, C>, R>() {
			for (int r = 0; r < R_O; r++)
				data[r] = basic_vector<T, C>{other[r]};
		}
		template<typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_matrix(basic_matrix<T_O, R_O, C_O> &&other, typename std::enable_if<(R_O <= R && C_O <= C), void*>::type less = nullptr) : basic_vector<basic_vector<T, C>, R>() {
			for (int r = 0; r < R_O; r++)
				data[r] = basic_vector<T, C>{other[r]};
		}
		template<typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		explicit basic_matrix(basic_matrix<T_O, R_O, C_O> const& other, typename std::enable_if<(R_O > R || C_O > C), void*>::type more = nullptr) {
			for (int r = 0; r < std::min(R, R_O); r++)
				data[r] = row_type(other.row(r));
		}
		template<typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		explicit basic_matrix(basic_matrix<T_O, R_O, C_O> &&other, typename std::enable_if<(R_O > R || C_O > C), void*>::type more = nullptr) {
			for (int r = 0; r < std::min(R, R_O); r++)
				data[r] = row_type(other.row(r));
		}
		basic_matrix<T, C, R> const& operator=(basic_matrix<T, C, R> const& other) {
			std::copy(other.begin(), other.end(), data);
			return *this;
		}
		basic_matrix<T, C, R> const& operator=(basic_matrix<T, C, R> &&other) {
			std::move(other.begin(), other.end(), data);
			return *this;
		}

		size_t size() const {
			return C * R;
		}
		T const* begin() const {
			return base_type::begin()->begin();
		}
		T* begin() {
			return base_type::begin()->begin();
		}
		T const* end() const {
			return base_type::end()->begin();
		}
		T* end() {
			return base_type::end()->begin();
		}

		T const& at(size_t r, size_t c) const {
			if (r >= R || c >= C)
				throw Exceptions::MatrixIndexOutOfBounds();
			return data[r][c];
		}
		T& at(size_t r, size_t c) {
			if (r >= R || c >= C)
				throw Exceptions::MatrixIndexOutOfBounds();
			return data[r][c];
		}
		T const& operator()(size_t r, size_t c) const {
			return at(r, c);
		}
		T& operator()(size_t r, size_t c) {
			return at(r, c);
		}
		row_type const& operator[](size_t r) const {
			if (r >= R)
				throw Exceptions::MatrixIndexOutOfBounds();
			return base_type::operator[](r);
		}
		row_type& operator[](size_t r) {
			if (r >= R)
				throw Exceptions::MatrixIndexOutOfBounds();
			return base_type::operator[](r);
		}
		basic_vector<T, C> const& row(size_t r) const {
			return operator[](r);
		}
		basic_vector<T, C>& row(size_t r) {
			return operator[](r);
		}

		void fill(T const& value) {
			for (size_t r = 0; r < R; r++)
				for (size_t c = 0; c < C; c++)
					data[r][c] = value;
		}

		template<typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(R_O <= R && C_O <= C)>::type>
		basic_matrix<T, R, C> const& operator+=(basic_matrix<T_O, R_O, C_O> const& other) {
			for (size_t r = 0; r < std::min(R, R_O); r++)
				for (size_t c = 0; c < std::min(C, C_O); c++)
					data[r][c] += T(other(r, c));
			return *this;
		}
		template<typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(R_O <= R && C_O <= C)>::type>
		basic_matrix<T, R, C> const& operator-=(basic_matrix<T_O, R_O, C_O> const& other) {
			for (size_t r = 0; r < std::min(R, R_O); r++)
				for (size_t c = 0; c < std::min(C, C_O); c++)
					data[r][c] -= T(other(r, c));
			return *this;
		}
		template<typename T_O, size_t R, size_t C, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(R == C)>::type>
		basic_matrix<T, R, C> const& operator*=(basic_matrix<T_O, R, C> const& other) {
			return (*this = *this * other);
		}
		template<typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_matrix<T, R, C> const& operator*=(T_O const& q) {
			for (size_t r = 0; r < R; r++)
				for (size_t c = 0; c < C; c++)
					data[r][c] *= T(q);
			return *this;
		}
		template<typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_matrix<T, R, C> const& operator/=(T_O const& q) {
			for (size_t r = 0; r < R; r++)
				for (size_t c = 0; c < C; c++)
					data[r][c] /= T(q);
			return *this;
		}

		basic_matrix<T, R, C> const operator-() const {
			basic_matrix<T, R, C> res;
			for (size_t i = 0; i < S; i++)
				res.data[i] = -data[i];
			return res;
		}
	};
	
	template<typename T, size_t R, size_t C, typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(R_O == R && C_O == C)>::type>
	bool operator==(basic_matrix<T, R, C> const& v1, basic_matrix<T_O, R_O, C_O> const& v2) {
		for (size_t i = 0; i < R; i++)
			if (v1.row(i) != v2.row(i))
				return false;
		return true;
	}
	template<typename T, size_t R, size_t C, typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(R_O == R && C_O == C)>::type>
	bool operator!=(basic_matrix<T, R, C> const& v1, basic_matrix<T_O, R_O, C_O> const& v2) {
		return !(v1 == v2);
	}

	template<typename T, size_t R, size_t C, typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(R_O == C)>::type>
	auto const operator*(basic_matrix<T, R, C> const& v1, basic_matrix<T_O, R_O, C_O> const& v2) {
		basic_matrix<decltype(v1[0][0] * v2[0][0]), R, C_O> res(ZeroMatrix);
		for (size_t i = 0; i < R; i++)
			for (size_t k = 0; k < C_O; k++)
				for (size_t j = 0; j < C; j++)
					res[i][k] += v1[i][j] * v2[j][k];
		return res;
	}
	template<typename T, size_t R, size_t C, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(S_O == C)>::type>
	auto const operator*(basic_matrix<T, R, C> const& v1, basic_vector<T_O, S_O> const& v2) {
		basic_vector<decltype(v1[0][0] * v2[0]), R> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[i] += v1[i][j] * v2[j];
		return res;
	}
	template<typename T, size_t R, size_t C, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(S_O == R)>::type>
	auto const operator*(basic_vector<T_O, S_O> const& v1, basic_matrix<T, R, C> const& v2) {
		basic_vector<decltype(v1[0] * v2[0][0]), C> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[j] += v1[i] * v2[i][j];
		return res;
	}

	template<typename T, size_t R, size_t C, typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator+(basic_matrix<T, R, C> const& v1, basic_matrix<T_O, R_O, C_O> const& v2) {
		basic_matrix<decltype(v1[0][0] + v2[0][0]), std::max(R, R_O), std::max(C, C_O)> res{v1};
		return res += v2;
	}
	template<typename T, size_t R, size_t C, typename T_O, size_t R_O, size_t C_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator-(basic_matrix<T, R, C> const& v1, basic_matrix<T_O, R_O, C_O> const& v2) {
		basic_matrix<decltype(v1[0][0] - v2[0][0]), std::max(R, R_O), std::max(C, C_O)> res{v1};
		return res -= v2;
	}
	
	template<typename T, size_t R, size_t C, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator*(basic_matrix<T, R, C> const& v, T_O const& q) {
		basic_matrix<decltype(v[0][0] * q), R, C> res{v};
		return res *= q;
	}
	template<typename T, size_t R, size_t C, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator*(T_O const& q, basic_matrix<T, R, C> const& v) {
		basic_matrix<decltype(v[0][0] * q), R, C> res{v};
		return res *= q;
	}
	template<typename T, size_t R, size_t C, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator/(basic_matrix<T, R, C> const& v, T_O const& q) {
		basic_matrix<decltype(v[0][0] / q), R, C> res{v};
		return res /= q;
	}
	template<typename T, size_t R, size_t C, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator/(T_O const& q, basic_matrix<T, R, C> const& v) {
		basic_matrix<decltype(q / v[0][0]), R, C> res{v};
		for (size_t r = 0; r < R; r++)
			for (size_t c = 0; c < C; c++)
				data[r][c] = q / data[r][c];
		return res;
	}

	class matrix2f : public basic_matrix<float, 2u, 2u> { public: using basic_matrix::basic_matrix; };
	class matrix3f : public basic_matrix<float, 3u, 3u> { public: using basic_matrix::basic_matrix; };
	class matrix4f : public basic_matrix<float, 4u, 4u> { public: using basic_matrix::basic_matrix; };

	class matrix2d : public basic_matrix<double, 2u, 2u> { public: using basic_matrix::basic_matrix; };
	class matrix3d : public basic_matrix<double, 3u, 3u> { public: using basic_matrix::basic_matrix; };
	class matrix4d : public basic_matrix<double, 4u, 4u> { public: using basic_matrix::basic_matrix; };

	class matrix2b : public basic_matrix<uint8_t, 2u, 2u> { public: using basic_matrix::basic_matrix; };
	class matrix3b : public basic_matrix<uint8_t, 3u, 3u> { public: using basic_matrix::basic_matrix; };
	class matrix4b : public basic_matrix<uint8_t, 4u, 4u> { public: using basic_matrix::basic_matrix; };

	class matrix2u : public basic_matrix<uint32_t, 2u, 2u> { public: using basic_matrix::basic_matrix; };
	class matrix3u : public basic_matrix<uint32_t, 3u, 3u> { public: using basic_matrix::basic_matrix; };
	class matrix4u : public basic_matrix<uint32_t, 4u, 4u> { public: using basic_matrix::basic_matrix; };

	class matrix2i : public basic_matrix<int32_t, 2u, 2u> { public: using basic_matrix::basic_matrix; };
	class matrix3i : public basic_matrix<int32_t, 3u, 3u> { public: using basic_matrix::basic_matrix; };
	class matrix4i : public basic_matrix<int32_t, 4u, 4u> { public: using basic_matrix::basic_matrix; };

	class matrix : public matrix4f { public: using matrix4f::matrix4f; };
}