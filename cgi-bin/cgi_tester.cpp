#include <iostream>
#include <ostream>
#include <string>
#include <sstream>

int main(int ac, char **av, char **env) {
	(void)ac;
	(void)av;
	if (!env || !env[0]) {
		std::cerr << "500";
		exit(42);
	}
	std::cout << std::endl;

	std::cout << "<html>" << std::endl
		<< "<head>" << std::endl
	 	<< "	<link rel=\"stylesheet\" href=\"get_no_mod.sh/neonstyle.css\">"
			<< std::endl
	 	<< "</head>" << std::endl
	 	<< "<body>" << std::endl
	 	<< "	<div class=\"login-box\">" << std::endl
	 	<< "		<h2>Cgi data</h2>" << std::endl
	 	<< "			<a href=\"http://localhost:4567/cgi-bin/cgi_tester\">"
			<< std::endl
	 	<< "				<span></span>" << std::endl
	 	<< "				<span></span>" << std::endl
	 	<< "				<span></span>" << std::endl
	 	<< "				<span></span>" << std::endl
	 	<< "			</a>" << std::endl;

	//<div cgi-keyword="Hello world">how are you doing</div>
	std::string key;
	std::string value;
	for (size_t i = 0; env[i]; i++) {
		std::istringstream stream(env[i]);

		std::getline(stream, key, '=');
		std::getline(stream, value);

		if (value.empty()) {
			std::cout << "			<div tabindex=\"0\" id=\"parent\" cgi-keyword=\""
				<< key << "\"> <div id=\"child\"> Empty</div></div>" << std::endl;
		}
		else {
			std::cout << "			<div tabindex=\"0\" id=\"parent\" cgi-keyword=\""
				<< key << "\"> <div id=\"child\">" << value << "</div></div>" << std::endl;
		}
	}
	std::cout << "	</div>" << std::endl
		<< "</body>" << std::endl
		<< "</html>";

	return 0;
}
