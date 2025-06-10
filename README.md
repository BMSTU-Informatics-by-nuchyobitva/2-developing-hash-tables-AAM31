# Семинар 2
Отчёт о проделанной работе:
- [ ] Скриншот из [симулятора кастомных хэш-функций](https://iswsa.acm.org/mphf/openDSAPerfectHashAnimation/perfectHashAV.html):
      
![image](https://github.com/user-attachments/assets/1d46a8f0-1374-43ac-ab82-3d8638d14cac)

Создаем структуру Person:
```
class Person {
public:
    std::string surname;
    int year;
    Person(const std::string& n = "", int y = 0) : surname(n), year(y) {} // компилятор сам создаст деструктор
};
```

Достался вариант с линейным пробированием, то есть каждый новый элемент, получившийся в результает хэширования с уже существующим хэшем, "уедет" вниз по таблице. Для корректной работы поиска будем хранить данные об удаленных ячейках, создаем enum State.

Сам класс Hash_table напоминает vector и будет выглядеть так:
```
enum State { fr, oc, de }; //free, occupied, deletedd
class Hash_table {
private:
    struct Hash_table_elem {
        Person person; // класс Person с фамилией, годом и конструктором
        State state; // наш enum
        Hash_table_elem() : state(fr) {} // компилятор сам создаст деструктор
    };

    Hash_table_elem* table;
    unsigned int cap = 0;// capacity
    unsigned int con = 0; // congestion
public:
    Hash_table(unsigned int size = 41) : cap(next_prime(size)), con(0) { //инициализируем пустую хеш-таблицу с 41 свободной ячейкой
        table = new Hash_table_elem[cap];
    }

    ~Hash_table() { // компилятор мог бы сам создать деструктор, но...
        delete[] table;
    }
}
```
Также стоит организовать функцию insert(), а еще и resize(), которая будет переформировывать и увеличивать вместимоть нашей таблички в случае ее переполнения, поэтому следует создать дополнительные функции для нахождения простых чисел...

Функции search(), remove(), print() делаем функциями-членами, также как и функцию хэширования...

Хэш-функцию делаю не свою, но модифицирую [эту](https://ru.wikipedia.org/wiki/FNV):
```
const unsigned int prime_for_hash = 0x01000193;
unsigned int hash_value;
int hash(const Person& p) const { 
	hash_value = 2166136261u;
	hash_value ^= static_cast<unsigned int>(p.year);

	for (char c : p.surname) {
		hash_value ^= (static_cast<unsigned int>(c) * prime_for_hash);
    		hash_value = (hash_value << 13) | (hash_value >> (32 - 13)); 
    		hash_value += (hash_value >> 7) & (p.year | hash_value);
	}
	return hash_value % cap;
}
```

