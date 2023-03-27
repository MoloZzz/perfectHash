#include <iostream>
#include <random>
#include <stdio.h>
#include <algorithm>

using namespace std;


int gcd(int numer, int denom) {

    while (numer != denom) {
        if (numer > denom) {
            numer = numer - denom;
        }
        else {
            denom = denom - numer;
        }
    }

    return numer;
}

class rational
{
    int numer;
    int denom;

public:
    rational() {
        numer = 0;
        denom = 1;
    }
    rational(int numer_new) {
        numer = numer_new;
        denom = 1;
    }
    rational(int numer_new, int denom_new) {
        if (!numer_new) {
            numer = 0;
            denom = 1;
        }
        else {
            int sign = 1;
            if (numer_new < 0) { sign = -sign; numer_new = -numer_new; };
            if (denom_new < 0) { sign = -sign; denom_new = -denom_new; };

            int gsd_new = gcd(numer_new, denom_new);
            //cout << numer_new << ':' << denom_new << " gsd: " << gsd_new << endl;
            if (gsd_new) {
                numer = sign * numer_new / gsd_new;
                denom = denom_new / gsd_new;
            }
            else {
                numer = sign * numer_new;
                denom = denom_new;
            }
        }
    }
    rational(int numer_new, int denom_new, int nooptimize) {
        numer = numer_new;
        denom = denom_new;
    }
    bool is1() { return numer && numer == denom; };
    bool is0() { return !numer; };
    rational operator+(rational);
    rational operator-(rational);
    rational operator *(rational);
    rational operator /(rational);
    bool operator ==(rational);
    bool operator <(rational);
    int get_numer() { return numer; };
    int get_denom() { return denom; };
};

rational rational ::operator+(rational c1)
{
    if (denom == 1 && c1.denom == 1) {
        return rational(numer + c1.numer, denom, 0);
    }
    else {
        return rational((numer * c1.denom) + (c1.numer * denom), denom * c1.denom);
    }
}

rational rational ::operator -(rational c1)
{
    if (denom == 1 && c1.denom == 1) {
        return rational(numer - c1.numer, denom, 0);
    }
    else {
        return rational((numer * c1.denom) - (c1.numer * denom), denom * c1.denom);
    }
}

rational rational ::operator *(rational c1)
{
    if (denom == 1 && c1.denom == 1) {
        return rational(numer * c1.numer, denom, 0);
    }
    else {
        return rational(numer * c1.numer, denom * c1.denom);
    }
}

bool rational ::operator ==(rational c1) {
    return (numer == c1.numer) && (denom == c1.denom);
}

bool rational ::operator <(rational c1) {
    return (double(numer) / double(denom) < double(c1.numer) / double(c1.denom));
}

rational rational :: operator /(rational c1)
{
    return rational(numer * c1.denom, c1.numer * denom);
}

static default_random_engine generator;
static uniform_int_distribution<int> distribution_numer(-100, 100);
static uniform_int_distribution<int> distribution_denom(1, 100);

//returns semi-random rational number, between -1000 and 1000
rational rand_rational() {
    return rational(distribution_numer(generator), distribution_denom(generator));
}

std::ostream& operator<<(std::ostream& s, rational& r) {
    if (r.get_denom() == 1) {
        return s << r.get_numer();
    }
    else {
        //return s << "(" << r.get_numer() << "/" << r.get_denom() << ")";
        return s << r.get_numer() << "/" << r.get_denom();
    }
}

struct hash_table {
    unsigned int size;
    unsigned int a;
    unsigned int b;
    rational* data;
};

int hash_main(rational number, int p, int m) {
    return (abs((number.get_numer() * number.get_denom())) % p) % m;
}

int hash_second(rational number, int a, int b, int p, int m) {
    int hash = ((abs((number.get_numer() * number.get_denom()) * a + b)) % p) % m;
    return hash;
}

static const unsigned int prime_for_m[] = { 1,1,5,11,19,29,37,53,67,83,101,137,149,173,199,233,269,293,331,373,409,449,491,541,587,631,691,733,797,853,907,967 };

int prime_second_for_n(unsigned int m) {
    if (m < 32) {
        return prime_for_m[m];
    }
    // no need to implement a generic prime finder - it's just a demo lab
    return 11093;
}

void print_second_table(hash_table* input_hashtable) {
    int size2 = input_hashtable->size * input_hashtable->size;
    cout << "|" << size2 << "|" << input_hashtable->a << "|" << input_hashtable->b << "|";
    for (int i = 0; i < size2; i++) {
        cout << input_hashtable->data[i];
    }
}

rational* get_from_ideal_hash_table(hash_table** main_hashtable, rational* key, int p, int m) {
    int this_hash = hash_main(*key, p, m);
    hash_table* second_table = main_hashtable[this_hash];

    if (second_table == nullptr) {
        return nullptr;
    }
    else {
        int secondhash = hash_second(*key, second_table->a, second_table->b, prime_second_for_n(second_table->size), second_table->size * second_table->size);

        return &(second_table->data[secondhash]);
    }
}

bool has_collisions(rational* numbers, int a, int b, int p, int m, int m2) {
    int* collisions = new int[m2];
    for (int i = 0; i < m2; i++) {
        collisions[i] = 0;
    }
    for (int i = 0; i < m; i++) {
        collisions[hash_second(numbers[i], a, b, p, m2)]++;
    }
    for (int i = 0; i < m2; i++) {
        if (collisions[i] > 1) {
            delete collisions;
            return 1;
        }
    }
    delete collisions;
    return 0;
}


void calculate_ideal_hash_table(hash_table* second_table, int p) {
    int m = second_table->size;
    int m2 = m * m;
    rational* numbers = second_table->data;
    int a;
    int b;
    bool found_perfect_table = 0;
    while (!found_perfect_table) {
        a = rand() % p;
        b = rand() % p;
        if (has_collisions(numbers, a, b, p, second_table->size, m2) == 1) {}
        else {
            found_perfect_table = 1;
        };
    }

    second_table->a = a;
    second_table->b = b;
}

int main() {
    srand(time(NULL));   // Initialization, should only be called once.

    const int DATA_TOTAL = 40;
    const int P_MAIN = 109;
    const int M_MAIN = 80;
    hash_table* main_hash_table[M_MAIN];

    rational DATA[DATA_TOTAL] =
    { \
  {1,46}, {26,68}, {40,54}, {23,62}, {96,29}, {71,16}, {83,85}, {86,68}, \
  {95,99}, {70,36}, {24,55}, {22,50}, {71,2}, {29,80}, {69,49}, {28,22}, \
  {91,59}, {52,68}, {10,29}, {69,51}, {39,17}, {92,29}, {14,40}, {61,1}, \
  {43,86}, {98,74}, {47,27}, {10,45}, {94,27}, {33,77}, {50,71}, {47,89}, \
  {3,90}, {51,14}, {16,66}, {54,86}, {77,47}, {41,12}, {19,74}, {19,39}, \
    };


    /* clean up main hash table */
    for (int i = 0; i < M_MAIN; i++) {
        main_hash_table[i] = nullptr;
    }

    int collisions_main[M_MAIN];
    for (int i = 0; i < M_MAIN; i++) {
        collisions_main[i] = 0;
    }
    for (int i = 0; i < DATA_TOTAL; i++) {
        collisions_main[hash_main(DATA[i], P_MAIN, M_MAIN)]++;
    }


    for (int i = 0; i < DATA_TOTAL; i++) {
        unsigned int current_hash = hash_main(DATA[i], P_MAIN, M_MAIN);
        if (1 == collisions_main[current_hash]) {
            main_hash_table[current_hash] = new hash_table{ 1, 0, 0, &DATA[i] };
        }
        else {
            if (main_hash_table[current_hash] == nullptr) {
                rational* new_array = new rational[collisions_main[current_hash]]{};
                main_hash_table[current_hash] = new hash_table{ 0, 0, 0, new_array };
            }
            hash_table* current_second_table = main_hash_table[current_hash];
            current_second_table->data[current_second_table->size] = DATA[i];
            current_second_table->size++;
        }
    }

    for (int i = 0; i < M_MAIN; i++) {
        hash_table* current_second_table = main_hash_table[i];
        if ((current_second_table != nullptr) && (current_second_table->size > 1)) {
            int second_table_p = prime_second_for_n(current_second_table->size);
            calculate_ideal_hash_table(current_second_table, second_table_p);
            int n2_size = current_second_table->size * current_second_table->size;
            rational* n2_array = new rational[n2_size]{};
            for (int j = 0; i < n2_size; i++) {
                n2_array[j] = { 0,0 };
            }
            for (int j = 0; j < current_second_table->size; j++) {
                int hash = hash_second(current_second_table->data[j], current_second_table->a, current_second_table->b, second_table_p, n2_size);
                n2_array[hash] = current_second_table->data[j];
                cout << "i=" << i << " j=" << j << " n=" << current_second_table->data[j] << endl;
            }
            delete current_second_table->data;
            current_second_table->data = n2_array;
        }
    }

    cout << "loop end";

    for (int i = 0; i < M_MAIN; i++) {
        cout << "loop";
        printf("%2d: -> ", i);
        if (main_hash_table[i] != nullptr) {
            print_second_table(main_hash_table[i]);
        }
        else {
            cout << "null";
        }
        cout << endl;
    }
    return 0;
}
