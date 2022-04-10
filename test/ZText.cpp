/*
g++ -std=c++20 -Wall -Werror -o ZText ZText.cpp && ./ZText
rm -f vgcore.*; g++ -std=c++20 -Wall -Werror -g -o ZText ZText.cpp && valgrind --leak-check=full ./ZText
 */
#define DOCTEST_CONFIG_IMPLEMENT
#include "./doctest.h"

#define ZTEXT_ERROR_CHECKS_ENABLED 1
#define ZTEXT_ERROR_MESSAGES_ENABLED 1
#define ZTEXT_IMPLEMENTATION
#define ZTEXT_IMPLEMENTATION_TEST
#include "../src/include/ZText.h"
#include <stdio.h>
#include <sstream>


#if 0
void manual_tests()
{
	printf("ztext::ZText Size: %ld\n", sizeof(ztext::ZText));
	printf("ztext::Element Size: %ld\n", sizeof(ztext::Element));
	ztext::ZText* ztext = ztext::create();

	ztext::variable_set(ztext, "a"   , "Hello");
	ztext::variable_set(ztext, "42"  , "answer");
	ztext::variable_set(ztext, "foo" , "");
	printf("\n");
	ztext::debug_variable(ztext);
	ztext::variable_destroy(ztext, "42");
	ztext::variable_destroy(ztext, "foo");
	printf("\n");
	ztext::debug_variable(ztext);

	ztext::Element* root_element = ztext::root_element(ztext);
	ztext::Element* element = nullptr;;
	element = ztext::element_append_variable(root_element, "a");
	printf("var: %s\n", ztext::element_eval(element).c_str());
	element = ztext::element_append_text(element, "foo");
	printf("txt: %s\n", ztext::element_eval(element).c_str());

	element = ztext::element_next(root_element);
	element = ztext::element_destroy(element);
	printf("ele: %s\n", ztext::element_eval(element).c_str());
	printf("\n");

	ztext::command_set(ztext
		, "section"
		, [](ztext::ZText*, ztext::Element* element)
		{
			ztext::MapStringString prop = ztext::command_property(element);

			std::stringstream ss = {};
			char buf[81] = { 0 };

			std::string section_name = {};
			if(prop.contains("title") == true)
			{
				section_name = prop["title"];
			}

			std::string line = "--------------------------------------------------------------------------------";
			size_t lead_len = 3;
			size_t len = line.size();

			ss << '\n';
			sprintf(buf, "%.*s", (int)lead_len, line.c_str());
			ss << buf;
			len -= lead_len;
			if(section_name.empty() == false)
			{
				ss << ' ' << section_name << ' ';

				len -= (2 + section_name.size());
			}

			sprintf(buf, "%.*s", (int)len, line.c_str());
			ss << buf << '\n';

			ztext::Element* e = nullptr;
			e = ztext::command_content(element);
			if(e != nullptr)
			{
				while(e)
				{
					ss << ztext::element_eval(e);
					e = ztext::element_next(e);
					if(e != nullptr)
					{
						ss << ' ';
					}
				}

				ss << '\n';
			}

			ss << line << '\n';
			ss << '\n';

			return ss.str();
		});
	ztext::Element* command = nullptr;
	command = ztext::element_append_command(element, "section");
	ztext::command_property_set(command, "title", "Foo");
	element = ztext::command_content(command);
	element = ztext::element_append_variable(element, "greet");
	element = ztext::element_append_variable(element, "thing");
	element = ztext::element_append_text(element, "\n");
	element = ztext::element_append_text(element, "This is a bunch of text.");
	element = ztext::element_append_text(element, "Some more text.");
	element = ztext::element_append_text(element, "And yet even more text.");

	ztext::variable_set(ztext, "greet", "Hello");
	ztext::variable_set(ztext, "thing", "World");
	printf("%s\n", ztext::element_eval(command).c_str());
	printf("\n");
	printf("\n");

	//std::string text = "{{$test Hello maybe?}}";
	std::string text = "{{test}}";
	//std::string text = "{{test content}}";
	//std::string text = "{{test()}}";
	//std::string text = "{{test() content}}";
	//std::string text = "{{test(abc=123) content}}";
	//std::string text = "{{test(abc=123,def=456) content}}";
	ztext::parse(ztext, text);

	ztext::destroy(ztext);
}
#endif


int main(int argc, char** argv)
{
	doctest::Context context;

	// --- Configuration: Defaults --- //
	context.setOption("order-by" , "name");
	context.setOption("rand-seed", "0"   );

	// --- Configuration: Parse Command-Line --- //
	context.applyCommandLine(argc, argv);

	// --- Configuration: Overrides --- //

	// --- Run Tests --- //
	int result = context.run();

	// --- Get Results --- ///
	if(context.shouldExit())
	{
		return result;
	}

	// --- Misc Stuff --- ///
	//manual_tests();

	// --- Done --- ///
	return result;
}
