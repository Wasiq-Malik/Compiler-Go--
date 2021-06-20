#include <iostream>
#include <streambuf>
#include <fstream>
#include <vector>
#include <string>
#include <ctype.h>
#include <iterator>
#include <unordered_map>
#include <map>

int *ds = new int[1000];

void initialise_ds(unordered_map<string, tuple<string, int, int>> &symbol_table)
{
    unordered_map<string, tuple<string, int, int>>::iterator itr;
    for (itr = symbols.begin(); itr != symbols.end(); ++itr)
    {
        int address = get<1>(itr->second);
        int init_value = get<2>(itr->second);

        ds[address / 4] = init_value;
    }
}

void execute_machine_code(vector<tuple<int, int, int, int>> &quad, unordered_map<string, tuple<string, int, int>> &symbol_table)
{
    initialise_ds(symbol_table);

    for (int pc = 0; pc < quad.size(); pc++)
    {

        int s1, s2, d;
        switch (get<0>(quad[pc]))
        {
        case 1:
            s1 = get<1>(quad[pc]) / 4;
            d = get<2>(quad[pc]) / 4;
            ds[d] = ds[s1];

            break;
        case 2:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]) / 4;
            ds[d] = ds[s1] + ds[s2];

            break;
        case 3:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]) / 4;
            ds[d] = ds[s1] - ds[s2];

            break;
        case 4:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]) / 4;
            ds[d] = ds[s1] * ds[s2];

            break;
        case 5:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]) / 4;
            ds[d] = ds[s1] / ds[s2];

            break;
        case 6:
            d = get<1>(quad[pc]) / 4;
            cout << " [in] ";
            cin >> ds[d];

            break;
        case 7:
            d = get<1>(quad[pc]) / 4;
            cout << " [out] " << ds[d];

            break;
        case 8:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]);

            if (ds[s1] == ds[s2])
                pc = d - 2;
            break;
        case 9:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]);

            if (ds[s1] >= ds[s2])
                pc = d - 2;
            break;
        case 10:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]);

            if (ds[s1] <= ds[s2])
                pc = d - 2;
            break;
        case 11:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]);

            if (ds[s1] > ds[s2])
                pc = d - 2;

            break;
        case 12:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]);

            if (ds[s1] < ds[s2])
                pc = d - 2;
            break;
        case 13:
            d = get<1>(quad[pc]);
            pc = d - 2;
            break;
        case 14:
            cout  << endl;
            break;
        case 15:
            s1 = get<1>(quad[pc]) / 4;
            s2 = get<2>(quad[pc]) / 4;
            d = get<3>(quad[pc]);

            if (ds[s1] != ds[s2])
                pc = d - 2;
            break;
        }
    }
}