#include <iostream>

void get_big_data(char *str1, char *str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int len = len1 + len2;

	
	int *res = (int *)calloc(sizeof(int), len);
	int i, j;

	//ÀÛ³Ë
	for (i = len1 - 1; i >= 0; i--)
	{
		for (j = len2 - 1; j >= 0; j--)
		{
			res[i + j + 1] += (str1[i] - '0') * (str2[j] - '0');
		}
	}
	//½øÎ»
	for (i = len - 1; i >= 0; i--)
	{
		res[i - 1] += res[i] / 10;
		res[i] %= 10;
	}
	char *value = (char *)calloc(sizeof(char), len + 1);
	i = 0;
	while (res[i] == 0)
		i++;
	for (j = 0; i < len; i++, j++)
	{
		value[j] = res[i] + '0';
	}
	std::cout << str1 << " * " << str2 << " = " << value << std::endl;


}


int main()
{
	char str1[] = "11111173647327489732480987328409834748209876479283411";
	char str2[] = "5347692873654987632746792838647092638478";

	get_big_data(str1, str2);

	//unsigned long long int a = -1;
	//std::cout << "unsigned long long int : " << a << std::endl;


	system("pause");

	return 0;
}