#pragma once
#include <initializer_list>
#include <algorithm>

#include "mml/exceptions.hpp"
DefineNewMMLException(VectorIndexOutOfBounds);

namespace mml {
	template<typename T, size_t S>
	class basic_vector {
	protected:
		T data[S];
	public:
		using value_type = T;
		static const size_t size_value = S;

		basic_vector() : data{T(0)} {}
		basic_vector(basic_vector<T, S> const& other) {
			std::copy(other.begin(), other.end(), data);
		}
		basic_vector(basic_vector<T, S>&& other) {
			std::move(other.begin(), other.end(), data);
		}
		template <typename... Tail>
		basic_vector(typename std::enable_if<sizeof...(Tail) + 1 <= S, T>::type head = T(0),
					 Tail... tail) : data{head, T(tail)...} {}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_vector(basic_vector<T_O, S_O> const& other, typename std::enable_if<(S_O <= S), void*>::type less = nullptr) : basic_vector() {
			std::copy(other.begin(), other.end(), data);
		}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_vector(basic_vector<T_O, S_O> &&other, typename std::enable_if<(S_O <= S), void*>::type less = nullptr) : basic_vector() {
			std::move(other.begin(), other.end(), data);
		}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		explicit basic_vector(basic_vector<T_O, S_O> const& other, typename std::enable_if<(S_O > S), void*>::type more = nullptr) {
			std::copy(other.begin(), other.begin() + S, data);
		}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		explicit basic_vector(basic_vector<T_O, S_O> &&other, typename std::enable_if<(S_O > S), void*>::type more = nullptr) {
			std::move(other.begin(), other.begin() + S, data);
		}
		basic_vector(std::initializer_list<T> const& inputs) : basic_vector() {
			if (inputs.size() > S)
				throw Exceptions::VectorIndexOutOfBounds("Too many inputs.");
			std::copy(inputs.begin(), inputs.end(), data);
			std::fill(data + inputs.size(), data + S, T(0));
		}
		basic_vector(std::initializer_list<T>&& inputs) : basic_vector() {
			if (inputs.size() > S)
				throw Exceptions::VectorIndexOutOfBounds("Too many inputs.");
			std::move(inputs.begin(), inputs.end(), data);
			std::fill(data + inputs.size(), data + S, T(0));
		}
		basic_vector<T, S> const& operator=(basic_vector<T, S> const& other) {
			std::copy(other.begin(), other.end(), data);
			return *this;
		}
		basic_vector<T, S> const& operator=(basic_vector<T, S>&& other) {
			std::move(other.begin(), other.end(), data);
			return *this;
		}

		T const& operator[](size_t index) const {
			if (index >= S)
				throw Exceptions::VectorIndexOutOfBounds();
			return data[index];
		}
		T& operator[](size_t index) {
			if (index >= S)
				throw Exceptions::VectorIndexOutOfBounds();
			return data[index];
		}
		T const& at(size_t index) const {
			return operator[](index);
		}
		T& at(size_t index) {
			return operator[](index);
		}

		size_t size() const {
			return S;
		}
		T const* begin() const {
			return data;
		}
		T* begin() {
			return data;
		}
		T const* end() const {
			return data + S;
		}
		T* end() {
			return data + S;
		}
		bool empty() const {
			for (size_t i = 0; i < S; i++)
				if (data[i] != T(0))
					return false;
			return true;
		}
		void clear() {
			for (size_t i = 0; i < S; i++)
				data[i] = T(0);
		}

		template<typename = typename std::enable_if<S >= 0 && S <= 4>::type> T const& x() const { return data[0]; }
		template<typename = typename std::enable_if<S >= 1 && S <= 4>::type> T const& y() const { return data[1]; }
		template<typename = typename std::enable_if<S >= 2 && S <= 4>::type> T const& z() const { return data[2]; }
		template<typename = typename std::enable_if<S >= 3 && S <= 4>::type> T const& w() const { return data[3]; }
		template<typename = typename std::enable_if<S >= 0 && S <= 4>::type> void x(T const& value) { data[0] = value; }
		template<typename = typename std::enable_if<S >= 1 && S <= 4>::type> void y(T const& value) { data[0] = value; }
		template<typename = typename std::enable_if<S >= 2 && S <= 4>::type> void z(T const& value) { data[0] = value; }
		template<typename = typename std::enable_if<S >= 3 && S <= 4>::type> void w(T const& value) { data[0] = value; }
		
		T length() const {
			T sum = T(0);
			for (size_t i = 0; i < S; i++)
				sum += data[i] * data[i];
			return std::sqrt(sum);
		}
		template<typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
		void normalize() {
			auto l = length();
			for (size_t i = 0; i < S; i++)
				data[i] /= l;
		}

		template<typename..., typename T_O = T>
		typename std::enable_if<std::is_floating_point<T_O>::value, basic_vector<T_O, S>>::type const normalized() const {
			basic_vector<T_O, S> ret(*this);
			ret.normalize();
			return ret;
		}
		template<typename..., typename T_O = T>
		typename std::enable_if<!std::is_floating_point<T_O>::value, basic_vector<float, S>>::type const normalized() const {
			basic_vector<float, S> ret(*this);
			ret.normalize();
			return ret;
		}

		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(S_O <= S)>::type>
		basic_vector<T, S>& operator+=(basic_vector<T_O, S_O> const& other) {
			for (size_t i = 0; i < std::min(S, S_O); i++)
				data[i] += T(other[i]);
			return *this;
		}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(S_O <= S)>::type>
		basic_vector<T, S>& operator-=(basic_vector<T_O, S_O> const& other) {
			for (size_t i = 0; i < std::min(S, S_O); i++)
				data[i] -= T(other[i]);
			return *this;
		}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(S_O <= S)>::type>
		basic_vector<T, S>& operator*=(basic_vector<T_O, S_O> const& other) {
			for (size_t i = 0; i < std::min(S, S_O); i++)
				data[i] *= T(other[i]);
			return *this;
		}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(S_O <= S)>::type>
		basic_vector<T, S>& operator/=(basic_vector<T_O, S_O> const& other) {
			for (size_t i = 0; i < std::min(S, S_O); i++)
				data[i] /= T(other[i]);
			return *this;
		}

		template<typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_vector<T, S>& operator*=(T_O const& q) {
			for (size_t i = 0; i < S; i++)
				data[i] *= T(q);
			return *this;
		}
		template<typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_vector<T, S>& operator/=(T_O const& q) {
			for (size_t i = 0; i < S; i++)
				data[i] /= T(q);
			return *this;
		}

		basic_vector<T, S> const operator-() const {
			basic_vector<T, S> res;
			for (size_t i = 0; i < S; i++)
				res.data[i] = -data[i];
			return res;
		}
	};

	template<typename T, size_t S>
	class basic_homogeneous_vector : public basic_vector<T, S + 1> {
	protected:
		using basic_vector<T, S + 1>::data;
	public:
		template<typename... Tail>
		basic_homogeneous_vector(typename std::enable_if<sizeof...(Tail) + 1 <= S, T>::type head = T(0),
										 Tail... tail) : basic_vector<T, S + 1>(head, tail...) { data[S] = T(1); }
		template<typename... Tail>
		basic_homogeneous_vector(typename std::enable_if<sizeof...(Tail) == S, T>::type head = T(0),
										 Tail... tail) : basic_vector<T, S + 1>(head, tail...) {}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_homogeneous_vector(basic_vector<T_O, S_O> const& other, typename std::enable_if<(S_O <= S + 1), void*>::type less = nullptr)
			: basic_vector<T, S + 1>(other) { data[S] = T(1); }
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		basic_homogeneous_vector(basic_vector<T_O, S_O> &&other, typename std::enable_if<(S_O <= S + 1), void*>::type less = nullptr)
			: basic_vector<T, S + 1>(other) { data[S] = T(1); }
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		explicit basic_homogeneous_vector(basic_vector<T_O, S_O> const& other, typename std::enable_if<(S_O > S + 1), void*>::type more = nullptr)
			: basic_vector<T, S + 1>(other) {}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
		explicit basic_homogeneous_vector(basic_vector<T_O, S_O> &&other, typename std::enable_if<(S_O > S + 1), void*>::type more = nullptr)
			: basic_vector<T, S + 1>(other) {}
		basic_homogeneous_vector(T* inputs) : basic_vector<T, S + 1>(inputs) { data[S] = T(1); }
		basic_homogeneous_vector(std::initializer_list<T> const& inputs) : basic_vector<T, S + 1>(inputs) {
			data[S] = T(1);
		}
		basic_homogeneous_vector(std::initializer_list<T>&& inputs) : basic_vector<T, S + 1>(inputs) {
			data[S] = T(1);
		}
	};

	template<typename T, size_t S, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(S_O == S)>::type>
	bool operator==(basic_vector<T, S> const& v1, basic_vector<T_O, S_O> const& v2) {
		for (size_t i = 0; i < S; i++)
			if (v1[i] != v2[i])
				return false;
		return true;
	}
	template<typename T, size_t S, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<(S_O == S)>::type>
	bool operator!=(basic_vector<T, S> const& v1, basic_vector<T_O, S_O> const& v2) {
		return !(v1 == v2);
	}

	template<typename T, size_t S, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto operator+(basic_vector<T, S> const& v1, basic_vector<T_O, S_O> const& v2) {
		basic_vector<decltype(v1[0] + v2[0]), std::max(S, S_O)> res{v1};
		return res += v2;
	}
	template<typename T, size_t S, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto operator-(basic_vector<T, S> const& v1, basic_vector<T_O, S_O> const& v2) {
		basic_vector<decltype(v1[0] - v2[0]), std::max(S, S_O)> res{v1};
		return res -= v2;
	}
	template<typename T, size_t S, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto operator*(basic_vector<T, S> const& v1, basic_vector<T_O, S_O> const& v2) {
		basic_vector<decltype(v1[0] * v2[0]), std::max(S, S_O)> res{v1};
		return res *= v2;
	}
	template<typename T, size_t S, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto operator/(basic_vector<T, S> const& v1, basic_vector<T_O, S_O> const& v2) {
		basic_vector<decltype(v1[0] / v2[0]), std::max(S, S_O)> res{v1};
		return res /= v2;
	}

	template<typename T, size_t S, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator*(basic_vector<T, S> const& v, T_O const& q) {
		basic_vector<decltype(v[0] * q), S> res{v};
		return res *= q;
	}
	template<typename T, size_t S, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator*(T_O const& q, basic_vector<T, S> const& v) {
		basic_vector<decltype(v[0] * q), S> res{v};
		return res *= q;
	}
	template<typename T, size_t S, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator/(basic_vector<T, S> const& v, T_O const& q) {
		basic_vector<decltype(v[0] / q), S> res{v};
		return res /= q;
	}
	template<typename T, size_t S, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	auto const operator/(T_O const& q, basic_vector<T, S> const& v) {
		basic_vector<decltype(q / v[0]), S> res{v};
		for (auto &it : res)
			it = q / it;
		return res;
	}

	template<typename T, size_t S, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	T const operator%(basic_vector<T, S> const& v1, basic_vector<T_O, S_O> const& v2) {
		T res = T(0);
		for (int i = 0; i < std::min(S, S_O); i++)
			res += v1[i] * v2[i];
		return res;
	}
	template<typename T, size_t S, typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	T const dot(basic_vector<T, S> const& v1, basic_vector<T_O, S_O> const& v2) {
		return v1 % v2;
	}

	template<typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	basic_vector<T, 3> const operator^(basic_vector<T, 3> const& v1, basic_vector<T_O, 3> const& v2) {
		return basic_vector<decltype(v1[0] * v2[0]), 3>(v1[1] * v2[2] - v2[1] * v1[2],
																v1[2] * v2[0] - v2[2] * v1[0],
																v1[0] * v2[1] - v2[0] * v1[1]);
	}
	template<typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	basic_vector<T, 3> const cross(basic_vector<T, 3> const& v1, basic_vector<T_O, 3> const& v2) {
		return v1 ^ v2;
	}

	template<typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	basic_vector<T, 3> const operator^(basic_homogeneous_vector<T, 3> const& v1, basic_vector<T_O, 3> const& v2) {
		return basic_vector<T, 3>(v1) ^ v2;
	}
	template<typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	basic_vector<T, 3> const operator^(basic_vector<T, 3> const& v1, basic_homogeneous_vector<T_O, 3> const& v2) {
		return v1 ^ basic_vector<T, 3>(v2);
	}
	template<typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	basic_vector<T, 3> const operator^(basic_homogeneous_vector<T, 3> const& v1, basic_homogeneous_vector<T_O, 3> const& v2) {
		return basic_vector<T, 3>(v1) ^ basic_vector<T, 3>(v2);
	}

	template<typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	basic_vector<T, 3> const cross(basic_homogeneous_vector<T, 3> const& v1, basic_vector<T_O, 3> const& v2) {
		return basic_vector<T, 3>(v1) ^ v2;
	}
	template<typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	basic_vector<T, 3> const cross(basic_vector<T, 3> const& v1, basic_homogeneous_vector<T_O, 3> const& v2) {
		return v1 ^ basic_vector<T, 3>(v2);
	}
	template<typename T, typename T_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type>
	basic_vector<T, 3> const cross(basic_homogeneous_vector<T, 3> const& v1, basic_homogeneous_vector<T_O, 3> const& v2) {
		return basic_vector<T, 3>(v1) ^ basic_vector<T, 3>(v2);
	}

	class vector2f : public basic_vector<float, 2u> { public: using basic_vector::basic_vector; };
	class vector3f : public basic_vector<float, 3u> { public: using basic_vector::basic_vector; };
	class vector4f : public basic_vector<float, 4u> { public: using basic_vector::basic_vector; };

	class vector2d : public basic_vector<double, 2u> { public: using basic_vector::basic_vector; };
	class vector3d : public basic_vector<double, 3u> { public: using basic_vector::basic_vector; };
	class vector4d : public basic_vector<double, 4u> { public: using basic_vector::basic_vector; };

	class vector2b : public basic_vector<uint8_t, 2u> { public: using basic_vector::basic_vector; };
	class vector3b : public basic_vector<uint8_t, 3u> { public: using basic_vector::basic_vector; };
	class vector4b : public basic_vector<uint8_t, 4u> { public: using basic_vector::basic_vector; };

	class vector2u : public basic_vector<uint32_t, 2u> { public: using basic_vector::basic_vector; };
	class vector3u : public basic_vector<uint32_t, 3u> { public: using basic_vector::basic_vector; };
	class vector4u : public basic_vector<uint32_t, 4u> { public: using basic_vector::basic_vector; };

	class vector2i : public basic_vector<int32_t, 2u> { public: using basic_vector::basic_vector; };
	class vector3i : public basic_vector<int32_t, 3u> { public: using basic_vector::basic_vector; };
	class vector4i : public basic_vector<int32_t, 4u> { public: using basic_vector::basic_vector; };

	class vector : public vector3f { public: using vector3f::vector3f; };
	class vectorH : public basic_homogeneous_vector<float, 3u> { public: using basic_homogeneous_vector::basic_homogeneous_vector; };
}