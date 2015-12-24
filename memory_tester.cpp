#include <iostream>
#include <exception>
#include <memory>
#include <list>
 
using namespace std;

int main()
{
    list<unique_ptr<char>> ptrs;
    try
    {
        while (true)
        {
            ptrs.emplace_back(new char[128]);
        }
    }
    catch (std::bad_alloc& e)
    {
        cout << "No memory\n";
    }
 
    size_t counter = 0;
    for (auto it = ptrs.begin(); it != ptrs.end();)
    {
        if (counter++ % 2 == 0)
        {
            ptrs.erase(it++);
        }
        else
        {
            ++it;
        }
    }
 
    try
    {
        auto ptr = unique_ptr<char>(new char[1024]);
    }
    catch (std::bad_alloc& e)
    {
        cout << "Still no memory\n";
 
        cout << "I want more memory!\n";
        for (size_t i = 0; i < 1024/128; ++i)
        {
            ptrs.pop_back(); // +128
        }
    }    
 
    try
    {
        auto ptr = unique_ptr<char>(new char[1024]);
 
        cout << "Uha-ha! I take a memory!\n";
 
    }
    catch (std::bad_alloc& e)
    {
        cout << "still no memory\n";
    }
 
    return 0;
}
