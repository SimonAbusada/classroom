
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main(int argc, char *argv[]) {
	setlocale(LC_ALL,"");
	printf("Gracias a locale.h puede poner palabras acentuadas como... Mexico :')");
	return 0;
}
