#pragma once

#include <iostream>

const int MAX_VECTOR_SIZE = 100000000;
const int MAX_MATRIX_SIZE = 10000;

// Шаблон вектора
template <class ValType>
class TVector
{
protected:
	int Size;       // размер вектора
	int StartIndex; // индекс первого элемента вектора
	ValType* pVector;
public:
	TVector(int s = 10, int si = 0);
	TVector(const TVector& v);                // конструктор копирования
	~TVector();
	int GetSize() { return Size; } // размер вектора
	int GetStartIndex() { return StartIndex; } // индекс первого элемента
	ValType& operator[](int pos);             // доступ
	bool operator==(const TVector& v) const;  // сравнение
	bool operator!=(const TVector& v) const;  // сравнение
	TVector& operator=(const TVector& v);     // присваивание

	// скалярные операции
	TVector  operator+(const ValType& val);   // прибавить скаляр
	TVector  operator-(const ValType& val);   // вычесть скаляр
	TVector  operator*(const ValType& val);   // умножить на скаляр

	// векторные операции
	TVector  operator+(const TVector& v);     // сложение
	TVector  operator-(const TVector& v);     // вычитание
	ValType  operator*(const TVector& v);     // скалярное произведение

	// ввод-вывод

	friend std::istream& operator>>(std::istream& in, TVector& v) {
		TVector<ValType>back(v);
		try {
			for (size_t i = 0; i < v.GetSize() - v.GetStartIndex(); ++i) {
				if (!(in >> v.pVector[i])) {
					in.clear();
					in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					throw std::invalid_argument("Incorrect input");
				}
			}
		}
		catch (...) {
			v = back;
			throw;
		}

		return in;
	};
	friend std::ostream& operator<<(std::ostream& out, const TVector& v) {
		ValType zero = {};

		for (size_t i = 0; i < v.StartIndex; ++i) {
			out << zero << " ";
		}

		bool m = true;
		for (size_t i = 0; i < v.Size - v.StartIndex; ++i) {
			if (!m) {
				out << " ";
			}
			out << v.pVector[i];
			m = false;
		}

		return out;
	};
};

// Верхнетреугольная матрица
template <class ValType>
class TMatrix : public TVector<TVector<ValType>>
{
public:
	TMatrix(int s = 10);
	TMatrix(const TMatrix& mt);                    // копирование
	TMatrix(const TVector<TVector<ValType>>& mt);  // преобразование типа
	bool operator==(const TMatrix& mt) const;      // сравнение
	bool operator!=(const TMatrix& mt) const;      // сравнение
	TMatrix& operator= (const TMatrix& mt);        // присваивание
	TMatrix  operator+ (const TMatrix& mt);        // сложение
	TMatrix  operator- (const TMatrix& mt);        // вычитание

	// ввод / вывод
	friend std::istream& operator>>(std::istream& in, TMatrix& mt) {
		TMatrix<ValType>mat_copy(mt);

		for (size_t i = 0; i < mt.Size; ++i) {
			try {
				in >> mt.pVector[i];
			}
			catch (...) {
				mt = mat_copy;
				throw std::invalid_argument("Incorrect input");
			}

		}

		return in;
	};
	friend std::ostream& operator<<(std::ostream& out, const TMatrix& mt) {
		for (size_t i = 0; i < mt.Size; ++i) {
			out << mt.pVector[i] << "\n";
		}
		return out;
	};
}; 

//////////класс TVector

template<class ValType> //конструктор
TVector<ValType>::TVector(int s, int si) : Size(s), StartIndex(si)
{
	if (s <= 0 || s > MAX_VECTOR_SIZE) {
		throw std::invalid_argument("Error. Incorrect size");
	}

	if (si < 0 || si >= s) {
		throw std::invalid_argument("Error. Incorrect start_index");
	}

	this->pVector = new ValType[s - si]();
}

template<class ValType> //конструктор копирования
TVector<ValType>::TVector(const TVector& v)
{
	this->Size = v.Size;
	this->StartIndex = v.StartIndex;
	this->pVector = new ValType[v.Size - v.StartIndex];

	for (size_t i = 0; i < v.Size - v.StartIndex; ++i) {
		this->pVector[i] = v.pVector[i];
	}
}

template<class ValType>
TVector<ValType>::~TVector() //деструктор
{
	delete[] pVector;
	this->pVector = nullptr;
}

template<class ValType>
ValType& TVector<ValType>::operator[](int pos)//доступ
{
	if (pos < this->StartIndex || pos >= this->Size) {
		throw std::out_of_range("Incorrect index");
	}

	return this->pVector[pos - this->StartIndex];
}

template<class ValType>
bool TVector<ValType>::operator==(const TVector& v) const //сравнение
{
	if (this->Size != v.Size) {
		return false;
	}

	int min_start_index = (this->StartIndex <= v.StartIndex ? this->StartIndex : v.StartIndex);
	int max_start_index = (this->StartIndex >= v.StartIndex ? this->StartIndex : v.StartIndex);

	ValType zero = {};

	if (min_start_index == this->StartIndex) {
		for (size_t i = min_start_index; i < max_start_index; ++i) {
			if (this->pVector[i - min_start_index] != zero) {
				return false;
			}
		}
	}
	else {
		for (size_t i = min_start_index; i < max_start_index; ++i) {
			if (v.pVector[i - min_start_index] != zero) {
				return false;
			}
		}
	}

	for (size_t i = max_start_index; i < this->Size; ++i) {
		if (this->pVector[i - this->StartIndex] != v.pVector[i - v.StartIndex]) {
			return false;
		}
	}

	return true;
}

template<class ValType>
bool TVector<ValType>::operator!=(const TVector& v) const //сравнение
{
	return !(*this == v);
}

template<class ValType>
TVector<ValType>& TVector<ValType>::operator=(const TVector& v) //присваивание
{
	if (this == &v) {
		return *this;
	}
	if (this->Size != v.Size) {
		ValType* p = new ValType[v.Size - v.StartIndex];
		delete[] this->pVector;
		this->pVector = p;
		p = nullptr;
		this->Size = v.Size;
	}
	this->StartIndex = v.StartIndex;

	for (size_t i = 0; i < v.Size - v.StartIndex; ++i) {
		this->pVector[i] = v.pVector[i];
	}
	return *this;
}

template<class ValType>
TVector<ValType> TVector<ValType>::operator+(const ValType& val) //прибавить скаляр
{
	TVector<ValType> tvec(this->Size, this->StartIndex);
	for (size_t i = 0; i < this->Size - this->StartIndex; ++i) {
		tvec.pVector[i] = this->pVector[i] + val;
	}
	return tvec;
}

template<class ValType>
TVector<ValType> TVector<ValType>::operator-(const ValType& val) //вычесть скаляр
{
	TVector<ValType> tvec(this->Size, this->StartIndex);
	for (size_t i = 0; i < this->Size - this->StartIndex; ++i) {
		tvec.pVector[i] = this->pVector[i] - val;
	}
	return tvec;
}

template<class ValType>
TVector<ValType> TVector<ValType>::operator*(const ValType& val) //умножить на скаляр
{
	TVector<ValType> tvec(this->Size, this->StartIndex);
	for (size_t i = 0; i < this->Size - this->StartIndex; ++i) {
		tvec.pVector[i] = this->pVector[i] * val;
	}
	return tvec;
}

template<class ValType>
TVector<ValType> TVector<ValType>::operator+(const TVector& v) //сложение
{
	if (this->Size != v.Size) {
		throw std::runtime_error("The vectors have different sizes");
	}

	int min_start_index = (this->StartIndex <= v.StartIndex ? this->StartIndex : v.StartIndex);

	TVector<ValType> tvec(this->Size, min_start_index);

	if (min_start_index == this->StartIndex) {
		for (size_t i = min_start_index; i < v.StartIndex; ++i) {
			tvec[i] = (*this)[i];
		}
		for (size_t i = v.StartIndex; i < this->Size; ++i) {
			tvec[i] = (*this)[i] + v.pVector[i - v.StartIndex];
		}
	}
	else {
		for (size_t i = min_start_index; i < this->StartIndex; ++i) {
			tvec[i] = v.pVector[i - v.StartIndex];
		}
		for (size_t i = this->StartIndex; i < this->Size; ++i) {
			tvec[i] = (*this)[i] + v.pVector[i - v.StartIndex];
		}
	}

	return tvec;
}

template<class ValType>
TVector<ValType> TVector<ValType>::operator-(const TVector& v) //вычитание
{
	if (this->Size != v.Size) {
		throw std::runtime_error("The vectors have different sizes");
	}

	int min_start_index = (this->StartIndex <= v.StartIndex ? this->StartIndex : v.StartIndex);

	TVector<ValType> tvec(this->Size, min_start_index);

	if (min_start_index == this->StartIndex) {
		for (size_t i = min_start_index; i < v.StartIndex; ++i) { //до максимального стартового индекса просто копирую
			tvec[i] = (*this)[i];
		}
		for (size_t i = v.StartIndex; i < this->Size; ++i) { //с наибольшего стартового индекса делаю вычитание
			tvec[i] = (*this)[i] - v.pVector[i - v.StartIndex];
		}
	}
	else {

		for (size_t i = this->StartIndex; i < this->Size; ++i) { //копирую значения от максимального стартового индекса
			tvec[i] = (*this)[i];
		}

		int size = this->Size - min_start_index;
		for (size_t i = 0; i < size; ++i) {  //проходим по всему доступному массиву и делаем вычитание
			tvec.pVector[i] = tvec.pVector[i] - v.pVector[i];
		}
	}

	return tvec;
}

template<class ValType>
ValType TVector<ValType>::operator*(const TVector& v) //скалярное произведение
{
	if (this->Size != v.Size) {
		throw std::runtime_error("The vectors have different sizes"); 
	}

	ValType value = {};

	int max_start_index = (this->StartIndex >= v.StartIndex ? this->StartIndex : v.StartIndex);

	for (size_t i = max_start_index; i < this->Size; ++i) {
		value += (*this)[i] * v.pVector[i - v.StartIndex];
	}

	return value;
}

//класс TMatrix
template<class ValType>
TMatrix<ValType>::TMatrix(int s) :TVector<TVector<ValType>>(s, 0) {
	if (s <= 0 || s > MAX_MATRIX_SIZE) {
		throw std::invalid_argument("Invalid matrix size");
	}

	for (size_t i = 0; i < s; ++i) {
		this->pVector[i] = TVector<ValType>(s, i);
	}

}
template<class ValType>
TMatrix<ValType>::TMatrix(const TMatrix& mt) :TVector<TVector<ValType>>(mt.Size) {// копирование  

	for (size_t i = 0; i < mt.Size; ++i) {
		this->pVector[i] = mt.pVector[i];
	}
}
template<class ValType>
TMatrix<ValType>::TMatrix(const TVector<TVector<ValType>>& mt) {// преобразование типа 

	TVector<TVector<ValType>>& non_const_mt = const_cast<TVector<TVector<ValType>>&>(mt);

	int size = non_const_mt.GetSize();

	if (size > MAX_MATRIX_SIZE) {
		throw std::invalid_argument("Invalid matrix size");
	}

	if (non_const_mt.GetStartIndex() != 0) {
		throw std::invalid_argument("Error. The starting index of the vector of vectors should be 0");
	}

	for (int i = 0; i < size; ++i) {
		if (non_const_mt[i].GetStartIndex() != i) {
			throw std::runtime_error("Matching of the starting indexes");
		}
	}

	TVector<ValType>* pMat = new TVector<ValType>[size];
	this->pVector = pMat;
	pMat = nullptr;
	this->Size = size;

	for (size_t i = 0; i < size; ++i) {
		this->pVector[i] = non_const_mt[i];
	}

}
template<class ValType>
bool TMatrix<ValType>::operator==(const TMatrix& mt) const
{
	if (this->Size != mt.Size) {
		return false;
	}

	for (size_t i = 0; i < this->Size; ++i) {
		if (this->pVector[i] != mt.pVector[i]) {
			return false;
		}
	}

	return true;
}

template<class ValType>
bool TMatrix<ValType>::operator!=(const TMatrix& mt) const //сравнение
{
	return !((*this) == mt);
}

template<class ValType>
TMatrix<ValType>& TMatrix<ValType>::operator=(const TMatrix& mt)//присваивание
{
	if (this == &mt) {
		return *this;
	}

	if (this->Size != mt.Size) {
		TVector<ValType>* pMat = new TVector<ValType>[mt.Size];
		delete[] this->pVector;
		this->pVector = pMat;
		pMat = nullptr;
		this->Size = mt.Size;
	}

	for (size_t i = 0; i < mt.Size; ++i) {
		this->pVector[i] = mt.pVector[i];
	}

	return *this;
}

template<class ValType>
TMatrix<ValType> TMatrix<ValType>::operator+(const TMatrix& mt)//сложение
{
	if (this->Size != mt.Size) {
		throw std::runtime_error("Different sizes of matrices");
	}

	TMatrix<ValType>mat(this->Size);
	for (size_t i = 0; i < this->Size; ++i) {
		mat.pVector[i] = this->pVector[i] + mt.pVector[i];
	}

	return mat;
}

template<class ValType>
TMatrix<ValType> TMatrix<ValType>::operator-(const TMatrix& mt)//вычитание
{
	if (this->Size != mt.Size) {
		throw std::runtime_error("Different sizes of matrices");
	}

	TMatrix<ValType>mat(this->Size);
	for (size_t i = 0; i < this->Size; ++i) {
		mat.pVector[i] = this->pVector[i] - mt.pVector[i];
	}

	return mat;
}

