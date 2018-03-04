#pragma once
#include <vector>

template <class T>
class Array3d {
	int nx;
	int ny;
	int nz;

	T* tmp1;//структура выделения памяти
	T** tmp2;
	T*** data;

public:

	void Initialize(int nx, int ny, int nz);
	Array3d();
	Array3d(int nx, int ny, int nz);
	T& operator()(int i, int j, int k);
	T& operator[](int index);
	T operator()(int index) const;
	T* getArray1d();

	int gnx() const { return nx; };
	int gny() const { return ny; };
	int gnz() const { return nz; };
	int size() const { return nx*ny*nz; };

	void Clear();
};

template<class T>
inline void Array3d<T>::Initialize(int _nx, int _ny, int _nz)
{
	nx = _nx; ny = _ny; nz = _nz;

	tmp1 = new T[nx*ny*nz];
	tmp2 = new T*[nx*ny];
	data = new T**[nx];

	for (int i = 0; i < nx*ny; i++)
		tmp2[i] = &(tmp1[i*nz]);
	for (int i = 0; i < nx; i++)
		data[i] = &(tmp2[i*ny]);
}

template<class T>
inline Array3d<T>::Array3d()
{
	tmp1 = 0;
	tmp2 = 0; 
	data = 0;
}

template<class T>
inline Array3d<T>::Array3d(int _nx, int _ny, int _nz)
{
	Initialize(_nx, _ny, _nz);
}

template<class T>
inline T & Array3d<T>::operator()(int i, int j, int k)
{
	return data[i][j][k];
}

template<class T>
inline T & Array3d<T>::operator[](int index)
{
	return tmp1[index];
}

template<class T>
inline T Array3d<T>::operator()(int index) const
{
	return tmp1[index];
}

template<class T>
inline T * Array3d<T>::getArray1d()
{
	return tmp1;
}

template<class T>
inline void Array3d<T>::Clear()
{
	delete[] data;
	delete[] tmp2;
	delete[] tmp1;
}
