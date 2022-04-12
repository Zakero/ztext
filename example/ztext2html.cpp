/*
g++ -std=c++20 -Wall -Werror -o ztext2html ztext2html.cpp && ./ztext2html sample.ztext
g++ -std=c++20 -Wall -Werror -fno-exceptions -Os -o ztext2html ztext2html.cpp && strip ztext2html && ls -l ztext2html && ./ztext2html sample.ztext | tee sample.md
 */

#define ZTEXT_ERROR_CHECKS_ENABLED 1
#define ZTEXT_ERROR_MESSAGES_ENABLED 1
#define ZTEXT_IMPLEMENTATION
#include "../src/include/ZText.h"
#include <stdio.h>
#include <fstream>
#include <sstream>

void add_html_commands(ztext::ZText* ztext)
{
	ztext::command_set(ztext
		, "title"
		, ZTextCommandLambda()
		{
			ztext::Element* content = ztext::element_command_content(element);

			std::string retval = "<h1>";
			retval += ztext::eval(ztext, content);
			retval += "</h1>\n";

			return retval;
		});

	ztext::command_set(ztext
		, "section"
		, ZTextCommandLambda()
		{
			ztext::Element* content = ztext::element_command_content(element);

			ztext::MapStringString& parameter = ztext::element_command_property(element);

			std::string retval = "<p>";
			if(parameter.contains("title") == true)
			{
				retval += "<b>" + parameter["title"] + "</b><br/>\n";
			}

			retval += ztext::eval(ztext, content);
			retval += "</p>\n";

			return retval;
		});

	ztext::command_set(ztext
		, "b"
		, ZTextCommandLambda()
		{
			ztext::Element* content = ztext::element_command_content(element);

			std::string retval = "<b>";
			retval += ztext::eval(ztext, content);
			retval += "</b>";

			return retval;
		});

	ztext::command_set(ztext
		, "i"
		, ZTextCommandLambda()
		{
			ztext::Element* content = ztext::element_command_content(element);

			std::string retval = "<i>";
			retval += ztext::eval(ztext, content);
			retval += "</i>";

			return retval;
		});
}


int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		printf("Usage: %s INPUT_FILE > OUTPUT_FILE\n", argv[0]);
		return 1;
	}

	std::stringstream file;
	{
		std::ifstream ifs(argv[1]);
		file << ifs.rdbuf();
	}

	ztext::ZText* ztext = ztext::create();
	add_html_commands(ztext);

	std::error_code error = {};
	ztext::Element* head  = nullptr;
	error = ztext::parse(file.str(), head);

	if(error)
	{
		ztext::destroy(ztext);
		printf("Error(%d): %s\n", error.value(), error.message().c_str());
		return 2;
	}

	std::string html = ztext::eval(ztext, head);

	printf("<html><body>\n");
	printf("%s\n", html.c_str());
	printf("</body></html>\n");

	ztext::destroy(ztext);
	return 0;
}
