#include <stdio.h>

void haut(char *);
void bas();

int main(int ac, char **av)
{
     printf("Content-Type: text/html;\n\n");
     haut("Ma page en C !");

     printf("Hello World !\n");
	 if (ac > 1)
	 	printf("My name is %s\n", av[1]);
	 else
		 printf("Sadly you didn't provide a name!\n");

     bas();
     return 0;
}

/* On sépare le squelette HTML du reste du code */
void haut(char *title) {
     printf("<html><head>");
     printf("\t\t<title>%s</title>", title);
     printf("\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\t</head>\n\t<body>");
}

void bas() {
     printf("\t</body>\n</html>");
}
