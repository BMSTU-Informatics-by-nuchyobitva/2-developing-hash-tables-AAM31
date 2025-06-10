#include <iostream>
#include <string>

// дл€ вычислени€ хэша, так что пусть живут тут
const unsigned int prime_for_hash = 0x01000193;
unsigned int hash_value;

// подготочка к увеличению рабочего объема таблицы, 
static bool is_prime(unsigned int n) {
    if (n % 2 == 0 || n % 3 == 0) return false; // микропроверка дл€ ускорени€ цикла
    for (unsigned int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}
static unsigned int next_prime(unsigned int n) {
    if (n <= 41) return 41;// начальный объем 41)
    if (n % 2 == 0) ++n;
    while (!is_prime(n)) n += 2; 
    return n;
}

class Person {
public:
    std::string surname;
    int year;
    Person(const std::string& n = "", int y = 0) : surname(n), year(y) {} // компил€тор сам создаст деструктор
};

enum State { fr, oc, de }; //free, occupied, deletedd
class Hash_table {
private:
    struct Hash_table_elem {
        Person person;
        State state;
        Hash_table_elem() : state(fr) {} // компил€тор сам создаст деструктор
    };

    Hash_table_elem* table;
    unsigned int cap = 0;// capacity
    unsigned int con = 0; // congestion

    // хеш-функци€
    int hash(const Person& p) const { // https://ru.wikipedia.org/wiki/FNV, но »«ћ≈Ќ≈ЌЌџ…
        hash_value = 2166136261u;
        hash_value ^= static_cast<unsigned int>(p.year);


        for (char c : p.surname) {
            hash_value ^= (static_cast<unsigned int>(c) * prime_for_hash);
            hash_value = (hash_value << 13) | (hash_value >> (32 - 13)); 
            hash_value += (hash_value >> 7) & (p.year | hash_value);
        }
        return hash_value % cap;
    }
    void resize() { // дл€ корректности работы insert
        unsigned int newcap = next_prime(2 * cap + 1); // нет смысла увеличивать объем не больше чем в 2 раза
        Hash_table_elem* newtable = new Hash_table_elem[newcap];
        unsigned int oldcap = cap;
        cap = newcap;
        unsigned int newcon = 0;

        for (unsigned int i = 0; i < oldcap; ++i) {
            if (table[i].state == oc) {
                Person& p = table[i].person;
                unsigned int i = hash(p);
                while (newtable[i].state == oc) {
                    i = (i + 1) % cap;
                }
                newtable[i].person = p;
                newtable[i].state = oc;
                ++newcon;
            }
        }
        delete[] table;
        table = newtable;
        con = newcon;
    }
public:
    Hash_table(unsigned int size = 41) : cap(next_prime(size)), con(0) { //инициализируем пустую хеш-таблицу с 41 свободной €чейкой
        table = new Hash_table_elem[cap];
    }

    ~Hash_table() { // компил€тор мог бы сам создать деструктор, но...
        delete[] table;
    }

    void insert(const std::string& surname, int year) {
        Person newPerson = *(new Person(surname, year));
        if (con / cap > 0.75) { // ??? коэффициент загрузки 75%, не вижу смысла неувеличени€ до, например 95%, но хороший тон
            resize();
        }
        int i = hash(newPerson);
        int start_i = i;

        while(true) {
            if (table[i].state == oc) { // случай occupied
                if (table[i].person.surname == newPerson.surname && table[i].person.year == newPerson.year) {
                    return; // нашли дубликат
                }
            }
            else { // случай free и deleted
                table[i].person = newPerson; 
                table[i].state = oc;
                ++con;
                return;
            }

            i = (i + 1) % cap;
            if (i == start_i) break; // завершаем цикл, если прошли весь круг
        }
    }

    Person* search(const std::string& surname, int year) { // O(n)
        unsigned int i = hash(Person(surname, year));
        unsigned int start_i = i;

        while(true) {
            if (table[i].state == oc) { // случай occupied
                if (table[i].person.surname == surname && table[i].person.year == year) {              
                    return &table[i].person;
                }
            }
            else if (table[i].state == fr) { // случай free
                break;
            }

            i = (i + 1) % cap;
            if (i == start_i) break; // завершаем цикл, если прошли весь круг
        }
        return nullptr;
    }

    bool remove(const std::string& surname, int year) {
        unsigned int i = hash(Person(surname, year));
        unsigned int start_i = i;

        while(true) {
            if (table[i].state == oc) { // случай occupied
                if (table[i].person.surname == surname && table[i].person.year == year) {
                    table[i].state = de;
                    return true;
                }
            }
            else if (table[i].state == fr) { // случай free
                return false;
            }

            i = (i + 1) % cap;
            if (i == start_i) break; // завершаем цикл, если прошли весь круг
        }
        return false;
    }

    void print() const {
        std::cout << "table contains " << con << " out of " << cap << std::endl;
        for (unsigned int i = 0; i < cap; ++i) {
            if (table[i].state == oc) {
                std::cout <<"key " << i << ": " << table[i].person.surname << ", " << table[i].person.year << std::endl;
            }
            else if (table[i].state == de) {
                std::cout << "key " << i << ": was once deleted, previous:  " << table[i].person.surname << ", " << table[i].person.year << "; now free" << std::endl;
            }
        }
    }
};

int main() {
    Hash_table ht(42); // начальный размер все равно 41

    ht.insert("Ivanov", 1990);
    ht.insert("Petrov", 1985);
    ht.insert("Sidorov", 2000);
    ht.insert("Sadorov", 2000);
    ht.insert("Sidoroff", 2000);
    ht.insert("Sidoroff", 2001);

    ht.remove("Petrov", 1985);
    ht.print();


    ht.remove("Petrov", 1985);
    ht.remove("Sadorov", 2000);
    ht.remove("Sidorov", 2000);
    ht.remove("Ivanov", 1990);
    ht.insert("Kizilov", 1999);
    ht.print(); 

    ht.insert("Kozlov", 1995);
    ht.print();
    
    std::cout << "address is " << ht.search("Sidoroff", 2001) << std::endl;

    return 0;
}