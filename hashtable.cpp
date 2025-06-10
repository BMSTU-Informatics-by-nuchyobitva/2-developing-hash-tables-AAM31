#include <iostream>
#include <string>

// ��� ���������� ����, ��� ��� ����� ����� ���
const unsigned int prime_for_hash = 0x01000193;
unsigned int hash_value;

// ���������� � ���������� �������� ������ �������, 
static bool is_prime(unsigned int n) {
    if (n % 2 == 0 || n % 3 == 0) return false; // ������������� ��� ��������� �����
    for (unsigned int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}
static unsigned int next_prime(unsigned int n) {
    if (n <= 41) return 41;// ��������� ����� 41)
    if (n % 2 == 0) ++n;
    while (!is_prime(n)) n += 2; 
    return n;
}

class Person {
public:
    std::string surname;
    int year;
    Person(const std::string& n = "", int y = 0) : surname(n), year(y) {} // ���������� ��� ������� ����������
};

enum State { fr, oc, de }; //free, occupied, deletedd
class Hash_table {
private:
    struct Hash_table_elem {
        Person person;
        State state;
        Hash_table_elem() : state(fr) {} // ���������� ��� ������� ����������
    };

    Hash_table_elem* table;
    unsigned int cap = 0;// capacity
    unsigned int con = 0; // congestion

    // ���-�������
    int hash(const Person& p) const { // https://ru.wikipedia.org/wiki/FNV, �� ����������
        hash_value = 2166136261u;
        hash_value ^= static_cast<unsigned int>(p.year);


        for (char c : p.surname) {
            hash_value ^= (static_cast<unsigned int>(c) * prime_for_hash);
            hash_value = (hash_value << 13) | (hash_value >> (32 - 13)); 
            hash_value += (hash_value >> 7) & (p.year | hash_value);
        }
        return hash_value % cap;
    }
    void resize() { // ��� ������������ ������ insert
        unsigned int newcap = next_prime(2 * cap + 1); // ��� ������ ����������� ����� �� ������ ��� � 2 ����
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
    Hash_table(unsigned int size = 41) : cap(next_prime(size)), con(0) { //�������������� ������ ���-������� � 41 ��������� �������
        table = new Hash_table_elem[cap];
    }

    ~Hash_table() { // ���������� ��� �� ��� ������� ����������, ��...
        delete[] table;
    }

    void insert(const std::string& surname, int year) {
        Person newPerson = *(new Person(surname, year));
        if (con / cap > 0.75) { // ??? ����������� �������� 75%, �� ���� ������ ������������ ��, �������� 95%, �� ������� ���
            resize();
        }
        int i = hash(newPerson);
        int start_i = i;

        while(true) {
            if (table[i].state == oc) { // ������ occupied
                if (table[i].person.surname == newPerson.surname && table[i].person.year == newPerson.year) {
                    return; // ����� ��������
                }
            }
            else { // ������ free � deleted
                table[i].person = newPerson; 
                table[i].state = oc;
                ++con;
                return;
            }

            i = (i + 1) % cap;
            if (i == start_i) break; // ��������� ����, ���� ������ ���� ����
        }
    }

    Person* search(const std::string& surname, int year) { // O(n)
        unsigned int i = hash(Person(surname, year));
        unsigned int start_i = i;

        while(true) {
            if (table[i].state == oc) { // ������ occupied
                if (table[i].person.surname == surname && table[i].person.year == year) {              
                    return &table[i].person;
                }
            }
            else if (table[i].state == fr) { // ������ free
                break;
            }

            i = (i + 1) % cap;
            if (i == start_i) break; // ��������� ����, ���� ������ ���� ����
        }
        return nullptr;
    }

    bool remove(const std::string& surname, int year) {
        unsigned int i = hash(Person(surname, year));
        unsigned int start_i = i;

        while(true) {
            if (table[i].state == oc) { // ������ occupied
                if (table[i].person.surname == surname && table[i].person.year == year) {
                    table[i].state = de;
                    return true;
                }
            }
            else if (table[i].state == fr) { // ������ free
                return false;
            }

            i = (i + 1) % cap;
            if (i == start_i) break; // ��������� ����, ���� ������ ���� ����
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
    Hash_table ht(42); // ��������� ������ ��� ����� 41

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