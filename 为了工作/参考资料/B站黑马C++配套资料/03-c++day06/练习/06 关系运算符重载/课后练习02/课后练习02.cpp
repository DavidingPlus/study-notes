#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector> 
using namespace std;
int main()
{
	vector<int>  A(4);
	vector<int>::iterator pi;
	pi = A.begin();  
	*pi++ = 51; 
	*pi++ = 61; 
	*pi++ = 51; 
	*pi++ = 81;
	cout << *(A.begin() + 1) << endl;;
	cout << *find(A.begin(), A.end(), 51) << endl;
	cout << count(A.begin(), A.end(), 51) << endl;
	reverse(A.begin(), A.end());
	for (pi = A.begin(); pi != A.end(); pi++)  cout << *pi << " ";
	cout << endl;
	sort(A.begin(), A.end());
	for (pi = A.begin(); pi != A.end(); pi++)  cout << *pi << " ";
	cout << endl;
	return 0;
}
