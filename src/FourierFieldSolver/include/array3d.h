#pragma once
#include <vector>

template <class T>
class Array3d {
    int nx;
    int ny;
    int nz;

    T* tmp1;  //структура выделения памяти
    T** tmp2;
    T*** data;

    void AllocMem();
    void SetPointersInNull() {
        tmp1 = 0;
        tmp2 = 0;
        data = 0;
    }

public:

    void Initialize(int nx, int ny, int nz);
    Array3d();
    Array3d(const Array3d& arr);
    Array3d(int nx, int ny, int nz);
    ~Array3d() { Clear(); }

    friend int operator==(const Array3d<T>& arr1, const Array3d<T>& arr2) {
        if (arr1.nx != arr2.nx || arr1.ny != arr2.ny || arr1.nz != arr2.nz) return 0;
        for (int i = 0; i < arr1.nx; i++)
            for (int j = 0; j < arr1.ny; j++)
                for (int k = 0; k < arr1.nz; k++)
                    if (arr1.data[i][j][k] != arr2.data[i][j][k]) return 0;
        return 1;
    };
    friend int operator!=(const Array3d<T>& arr1, const Array3d<T>& arr2) {
        return !(arr1 == arr2);
    };
    T& operator()(int i, int j, int k);
    T& operator[](int index);
    T operator()(int index) const;
    Array3d& operator=(const Array3d& arr);

    T* getArray1d();

    int gnx() const { return nx; };
    int gny() const { return ny; };
    int gnz() const { return nz; };
    int size() const { return nx*ny*nz; };

    void Clear();
};

template<class T>
inline void Array3d<T>::AllocMem()
{
    tmp1 = new T[nx*ny*nz];
    tmp2 = new T*[nx*ny];
    data = new T**[nx];

    for (int i = 0; i < nx*ny; i++)
        tmp2[i] = &(tmp1[i*nz]);
    for (int i = 0; i < nx; i++)
        data[i] = &(tmp2[i*ny]);
}

template<class T>
inline void Array3d<T>::Initialize(int _nx, int _ny, int _nz)
{
    Clear();

    nx = _nx; ny = _ny; nz = _nz;

    AllocMem();
}

template<class T>
inline Array3d<T>::Array3d()
{
    SetPointersInNull();
}

template<class T>
inline Array3d<T>::Array3d(const Array3d & arr)
{
    nx = arr.nx; ny = arr.ny; nz = arr.nz;
    SetPointersInNull();

    AllocMem();

    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            for (int k = 0; k < nz; k++)
                (*this)(i, j, k) = arr.data[i][j][k];
}

template<class T>
inline Array3d<T>::Array3d(int _nx, int _ny, int _nz)
{
    SetPointersInNull();
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
inline Array3d<T> & Array3d<T>::operator=(const Array3d & arr)
{
    Initialize(arr.nx, arr.ny, arr.nz);

    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            for (int k = 0; k < nz; k++)
                (*this)(i, j, k) = arr.data[i][j][k];

    return *this;
}

template<class T>
inline T * Array3d<T>::getArray1d()
{
    return tmp1;
}

template<class T>
inline void Array3d<T>::Clear()
{
    if (data) delete[] data;
    if (tmp2) delete[] tmp2;
    if (tmp1) delete[] tmp1;
    SetPointersInNull();
}
