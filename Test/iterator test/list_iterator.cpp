#include <list>
#include <iostream>
using namespace std;

int main(void) {
  list<int> lst;
  for(int n = 1; n <= 10; n++)
    lst.push_back(n);

  cout << "Before reference" << endl;

  for(list<int>::iterator it = lst.begin(); it != lst.end(); it++)
    cout << *it << " ";
  cout << endl;


  int *p;
  cout << "after reference" << endl;
  for(list<int>::iterator it = lst.begin(); it != lst.end(); it++) {
    p = &*it;
    *p *= 10;
  }


  for(list<int>::iterator it = lst.begin(); it != lst.end(); it++)
    cout << *it << " ";
  cout << endl;
  return 0;
}
