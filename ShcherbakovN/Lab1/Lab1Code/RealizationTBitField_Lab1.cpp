#include <../../../../../include/Lab1/tbitfield.h>

TBitField::TBitField(int len)
{
	if (len <= 0)
	{
		throw std::out_of_range("The length of the bit field must be greater than 0"); //Бросаем исключение.
	}

	BitLen = len; //Длина битового поля.
	unsigned int bit_telem = std::numeric_limits<TELEM>::digits; //Столько бит занимает TELEM.(Аналогично sizeof(TELEM)*8)
	MemLen = (BitLen + (bit_telem - 1)) / bit_telem; //Количество элементов типа TELEM в pMem для представления битового поля.
	pMem = new TELEM[MemLen]; //Память для битового поля.
	memset(pMem, 0, sizeof(TELEM)*MemLen); //Инициализируем битовое поле (все байты, в количестве sizeof(TELEM)*MemLen) нулями.
}

TBitField::TBitField(const TBitField& bf)
{
	BitLen = bf.BitLen;
	MemLen = bf.MemLen;
	pMem = new TELEM[MemLen];
	for (int i = 0; i < MemLen; ++i) //Копируем битовое поле bf в новый объект класса.
	{
		pMem[i] = bf.pMem[i];
	}
}

TBitField::~TBitField()
{
	delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const
{
	if (n >= BitLen || n < 0)
	{
		throw std::out_of_range("Incorrect bit number"); //Бросаем исключение.
	}

	return (MemLen - 1) -(n / std::numeric_limits<TELEM>::digits); //Вернёт индекс в массиве pMem, где n-ый бит. std::numeric_limits<TELEM>::digits вернёт количество бит типа TELEM.
	//(n / std::numeric_limits<TELEM>::digits) - номер элемента pMem, если бы биты шли слева направо. (MemLen - 1) -(n / std::numeric_limits<TELEM>::digits) - это уже номер элемента pMem, когда биты идут справа налево.
	//Пример: Всего бит 64 (63..32, 31..0), n = 45, вернётся 45 / 32 = 1, 2-1-1=0 (если TELEm = unsigned int). То есть 45 бит находится в pMem[0].
}

TELEM TBitField::GetMemMask(const int n) const //Здесь создаём маску для конкретных sizeof(TELEM) (Если unsigned int, то 4) байт, в которых находится n-ый бит.
{
	if (n >= BitLen || n < 0)
	{
		throw std::out_of_range("Incorrect bit number"); //Бросаем исключение.
	}

	unsigned int bit_telem = std::numeric_limits<TELEM>::digits; //Столько бит занимает TELEM.(Анналогично sizeof(TELEM)*8)
	TELEM mask = 1 << (n % bit_telem); //Смещаем единицу влево на (n % bit_telem) бит, формируя маску.
	return mask;
}

int TBitField::GetLength(void) const
{
	return BitLen;
}

void TBitField::SetBit(const int n)
{
	if (n >= BitLen || n < 0)
	{
		throw std::out_of_range("Incorrect bit number"); //Бросаем исключение.
	}

	pMem[GetMemIndex(n)] |= GetMemMask(n); //Изменяем значение n-ого бита на единицу. | - побитовое или.
	//pMem[GetMemIndex(n)] = pMem[GetMemIndex(n)] | GetMemMask(n); - в развёрнутом виде.
}

void TBitField::ClrBit(const int n)
{
	if (n >= BitLen || n < 0)
	{
		throw std::out_of_range("Incorrect bit number"); //Бросаем исключение.
	}

	pMem[GetMemIndex(n)] &= ~(GetMemMask(n)); //Изменяем значение n-ого бита на нуль (1 & 0 = 0, 0 & 0 = 0). & - побитовое И. ~ - инверсия битов(побитовое отрицание).
	//pMem[GetMemIndex(n)] = pMem[GetMemIndex(n)] & ~(GetMemMask(n)); - в развёрнутом виде.
}

int  TBitField::GetBit(const int n) const
{
	if (n >= BitLen || n < 0)
	{
		throw std::out_of_range("Incorrect bit number"); //Бросаем исключение.
	}

	return (pMem[GetMemIndex(n)] & GetMemMask(n)) == 0 ? 0 : 1; //Тернаный оператор условия. & - побитовое И. 0 & 1 = 0. 1 & 1 = 1.
}

int TBitField::operator==(const TBitField& bf) const
{
	if (BitLen != bf.BitLen) //Проверяем на несовпадение длины битовых полей.
	{
		return 0;
	}

	for (int i = 0; i < MemLen; ++i) //Проверяем битовые поля по значению элементов pMem.
	{
		if (pMem[i] != bf.pMem[i])
		{
			return 0;
		}
	}

	return 1;
}

int TBitField::operator!=(const TBitField& bf) const
{
	return (*this == bf) ? 0 : 1;//*this == bf (перегруженный оператор == для битовых полей, в нём уже есть проверка на BitLen != bf.BitLen). Если вернёт 1, то вернётся 0. Если вернётся 0, то блок условия не выполнится, вернётся 1.
}

TBitField& TBitField::operator=(const TBitField& bf)
{
	if (this == &bf) //Проверка на то, что передаём то же самое битовое поле.
	{
		return *this;
	}

	delete[] pMem; //Очищаем старую память исходного битового поля.

	BitLen = bf.BitLen;
	MemLen = bf.MemLen;
	pMem = new TELEM[MemLen];
	for (int i = 0; i < MemLen; ++i) //Копируем битовое поле bf в другое (исходное) битовое поле.
	{
		pMem[i] = bf.pMem[i];
	}

	return *this; //Возвращаем ссылку на объект.
}

TBitField TBitField::operator|(const TBitField& bf)
{
	int MaxBitLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
	TBitField res(MaxBitLen); //Создаём итоговое битовое поле размером, равным размеру самого длинного битового поля из сравниваемых.

	const TBitField& LongBF = (BitLen >= bf.BitLen) ? *this : bf; //Узнаём более длинное битовое поле.
	const TBitField& ShortBF = (BitLen < bf.BitLen) ? *this : bf; //Узнаём более короткое битовое поле.

	int i;
	for (i = 0; i < (LongBF.MemLen - ShortBF.MemLen); ++i)
	{
		res.pMem[i] = LongBF.pMem[i]; //Переписываем байты, которых нет в меньшем битовом поле, в итоговое битовое поле.
	}
	int j = 0;
	for (i, j; i < LongBF.MemLen; ++i, ++j)
	{
		res.pMem[i] = LongBF.pMem[i] | ShortBF.pMem[j]; //Сравниваем побитовым ИЛИ биты двух битовых полей.
	}
	return res;
}

TBitField TBitField::operator&(const TBitField& bf)
{
	int MaxBitLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
	TBitField res(MaxBitLen); //Создаём итоговое битовое поле размером, равным размеру самого длинного битового поля из сравниваемых.

	const TBitField& LongBF = (BitLen >= bf.BitLen) ? *this : bf; //Узнаём более длинное битовое поле.
	const TBitField& ShortBF = (BitLen < bf.BitLen) ? *this : bf; //Узнаём более короткое битовое поле.

	int i;
	for (i = 0; i < (LongBF.MemLen - ShortBF.MemLen); ++i)
	{
		res.pMem[i] = LongBF.pMem[i]; //Переписываем байты, которых нет в меньшем битовом поле, в итоговое битовое поле.
	}
	int j = 0;
	if (i < LongBF.MemLen)
	{
		int elem_longBF = LongBF.MemLen - 1 - i; //Номер элемента в pMem более длинного битового поля, который сравнивается с "неполным" элементом (у которого меньше битов, чем должно быть) более короткого битового поля.
		int elem_shortBF = ShortBF.MemLen - 1; //Номер элемента в pMem более короткого битового поля, который является "неполным" элементом. (Например (std::numeric_limits<TELEM>::digits = 32), если элемент состоит из 32 - полный, а если элемент состоит из 14 бит - неполный).
		int bit_longBF = -1, bit_shortBF = -1;
		int ind_bit_longBF = -1, ind_bit_shortBF = -1;
		const int bit_in_elem = std::numeric_limits<TELEM>::digits; //std::numeric_limits<TELEM>::digits (Анналогично sizeof(TELEM)*8).
		for (int k = 0; k < bit_in_elem && ind_bit_longBF < MaxBitLen - 1; ++k) //k - смещение по битам в элементе. //(ind_bit_longBF < MaxBitLen - 1) - проверяем, не пытаемся ли мы обратиться к несуществующему биту.
		{
			ind_bit_longBF = (bit_in_elem * elem_longBF) + k; //Номер соотвествующего бита в более длинном битовом поле.
			ind_bit_shortBF = (bit_in_elem * elem_shortBF) + k; //Номер соотвествующего бита в более коротком битовом поле.
			bit_longBF = LongBF.GetBit(ind_bit_longBF); //Получаем биты в элементе более длинного битового поля, который сравнивается с "неполным" элементом более короткого битового поля.
			if (ind_bit_shortBF >= ShortBF.GetLength()) //Проверка на то, что обращаемся к битам с номерами, которых нет в битовом поле. (Например, в битовом поле 10 бит, а мы обратимся к 10 по номеру биту(номера начинаются с 0), который являлся бы 11 битом)
			{
				bit_shortBF = 1; //Обращаем в единицу, чтобы при побитовом И скопировать биты из более длинного битового поля, а не поменять их.
			}
			else
			{
				bit_shortBF = ShortBF.GetBit(ind_bit_shortBF); //Получаем биты в элементе более короткого битового поля ("неполный" элемент).
			}
			if ((bit_longBF & bit_shortBF) == 0) //Побитовое И.
			{
				res.ClrBit(ind_bit_longBF);
			}
			else
			{
				res.SetBit(ind_bit_longBF);
			}
		}
		i++;
		j++;
	}
	for (i, j; i < LongBF.MemLen; ++i, ++j)
	{
		res.pMem[i] = LongBF.pMem[i] & ShortBF.pMem[j]; //Сравниваем побитовым И биты двух битовых полей.
	}
	return res;
}

TBitField TBitField::operator~(void)
{
	TBitField res(BitLen); //Создаём итоговое битовое поле, размером, равным размеру исходного битового поля.
	for (int i = MemLen - 1; i > 0; --i)
	{
		res.pMem[i] = ~(pMem[i]);
	}
	const int bit_in_elem = std::numeric_limits<TELEM>::digits; //std::numeric_limits<TELEM>::digits (Аналогично sizeof(TELEM)*8).
	int bit_in_BF = -1, ind_bit_in_BF = -1;
	for (int k=0; k < bit_in_elem && ind_bit_in_BF < BitLen - 1; ++k) //Инвертируем самые старшие биты(инвертируем "неполный" элемент в pMem). (ind_bit_in_BF < BitLen - 1) - проверяем, не пытаемся ли мы обратиться к несуществующему биту.
	{
		ind_bit_in_BF = (bit_in_elem * (MemLen - 1)) + k;
		bit_in_BF = GetBit(ind_bit_in_BF);
		if (bit_in_BF)
		{
			res.ClrBit(ind_bit_in_BF);
		}
		else
		{
			res.SetBit(ind_bit_in_BF);
		}
	}
	return res;
}

//Формат ввода:********, где * - 0 или 1.
std::istream& operator>>(std::istream& istr, TBitField& bf) //Если передаём битовое поле больше, чем размер поля bf, то запишутся только первые (младшие) биты. Если передаём битовое поле меньше, чем размер поля bf, то запишутся все биты, но страшие биты в bf будут равны 0. При равенстве - всё записывается.
{
	for (int i = 0; i < bf.GetLength(); ++i) //Очищаем битовое поле от старых значений.
	{
		bf.ClrBit(i);
	}

	std::string str; //Создаём буфер для ввода из консоли (строку).
	istr >> str; //Ввод из консоли.
	int str_len = str.length(), str_bit = -1;
	for (int i = 0; i < str_len; ++i) //Проверка на то, что введено НЕ 0 или НЕ 1.
	{
		str_bit = str[i] - '0'; //Если str[i] == '0', то str[i] вернёт 48. Если str[i] == '1', то str[i] вернёт 49. '0' == 48.
		if(!(str_bit == 0 || str_bit == 1))
		{
			throw std::invalid_argument("A non-bit field has been introduced");
		}
	}
	str_bit = -1;
	for (int i = 0, j = str_len - 1; i < bf.BitLen && j >= 0; ++i, --j) //Записываем биты в битовое поле, начиная с конца буфера (строки) и с конца битового поля.
	{
		str_bit = str[j] - '0'; //Если str[i] == '0', то str[i] вернёт 48. Если str[i] == '1', то str[i] вернёт 49. '0' == 48.
		if (str_bit)
		{
			bf.SetBit(i);
		}
		else
		{
			bf.ClrBit(i);
		}
	}
	return istr;
}
std::ostream& operator<<(std::ostream& ostr, const TBitField& bf)
{
	for (int i = bf.BitLen - 1; i >= 0; --i)
	{
		ostr << bf.GetBit(i);
	}
	return ostr;
}