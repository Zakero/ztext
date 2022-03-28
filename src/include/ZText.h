/******************************************************************************
 * Copyright 2021-2022 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/*
 * TODO: Recursive variables
 *       VAR: foo = "Hello, World!"
 *       VAR: bar = "{{$foo}}"
 *       eval {{$bar}} = "Hello, World!"
 *
 * TODO: Compond variables
 *       VAR: foo = "Hello, World!"
 *       VAR: bar = "f"
 *       evel {{${{$bar}}oo}} = "Hello, World!"
 */


/******************************************************************************
 * Includes
 */

// C++
#include <functional>
#include <source_location>
#include <string>
#include <unordered_map>

// POSIX


/******************************************************************************
 * Macros
 */

// {{{ Macros

#define ZTEXT_ERROR std::cerr \
	<< std::source_location::current().function_name() \
	<< " - "

/**
 * \internal
 *
 * \brief Error_Table
 *
 * An X-Macro table of error codes. The columns are:
 * -# __ErrorName__<br>
 *    The error name will be accessible as zakero::Network::_ErrorName_
 *    <br><br>
 * -# __ErrorValue__<br>
 *    The integer value of the error and will be available as `int` values in 
 *    the zakero::Network class.
 *    <br><br>
 * -# __ErrorMessage__<br>
 *    The text that will be used by `std::error_code.message()`
 */
#define ZTEXT__ERROR_DATA \
	X(Error_None                            ,  0 , "No Error"               ) \
	X(Error_Invalid_Parameter               ,  1 , "A parameter is invalid" ) \
	X(Error_Element_In_Use                  ,  2 , "The requested Element is in use by another ZText object" ) \
	X(Error_Parser_Invalid_Token_Name       ,  3 , "The Parser found an invalid token name" ) \
	X(Error_Parser_No_Text_Found            ,  4 , "The Parser was not able to find any text" ) \
	X(Error_Parser_Token_End_Marker_Missing ,  5 , "The Parser was not able to find the token closer '}}'" ) \
	X(Error_Parser_Token_Name_Missing       ,  6 , "The Parser was not able to find the token name"        ) \
	X(Error_Parser_Invalid_Token_Identifier ,  7 , "The Parser found an invalid token indentifier" ) \


// }}}


namespace ztext
{
	// {{{ Error

	class ErrorCategory_
		: public std::error_category
	{
		public:
			constexpr ErrorCategory_() noexcept {};

			[[nodiscard]] const char* name() const noexcept final override;
			[[nodiscard]] std::string message(int condition) const noexcept final override;
	};

	extern ErrorCategory_ ErrorCategory;

	#define X(name_, val_, mesg_) \
	const std::error_code name_(val_, ErrorCategory);
	ZTEXT__ERROR_DATA
	#undef X

	// }}}

	struct ZText;
	struct Element;

	using CommandLambda = std::function<std::string(ZText*, Element*)>;
	using MapStringString = std::unordered_map<std::string, std::string>;


	// --- Utility --- //
	[[nodiscard]] ZText*          create() noexcept;
	[[]]          void            destroy(ZText*&) noexcept;
	[[]]          void            clear(ZText*) noexcept;
	[[]]          void            clear_commands(ZText*) noexcept;
	[[]]          void            clear_elements(ZText*) noexcept;
	[[]]          void            clear_variables(ZText*) noexcept;
	[[]]          std::error_code parse(ZText*, const std::string&) noexcept;
	[[nodiscard]] Element*        root_element(ZText*) noexcept;

	// --- Element --- //
	[[]]          std::error_code element_append(ZText*, Element*, Element*) noexcept;
	[[]]          Element*        element_destroy(Element*&) noexcept;
	[[]]          void            element_remove(Element*) noexcept;
	[[nodiscard]] Element*        element_next(Element*) noexcept;
	[[nodiscard]] Element*        element_prev(Element*) noexcept;

	[[nodiscard]] std::string     element_eval(Element*) noexcept;

	// --- Element: Text --- //
	[[nodiscard]] Element*        element_text_create(const std::string&) noexcept;

	// --- Debugging --- //
	#ifdef ZTEXT_DEBUG_ENABLED
	[[]]          void            print(const Element*, const bool = false) noexcept;
	#endif

	// --- Deprecated --- //
	[[]]          Element*        element_append_command(Element*, const std::string) noexcept;
	[[]]          Element*        element_append_text(Element*, const std::string) noexcept;
	[[]]          Element*        element_append_variable(Element*, const std::string) noexcept;

	[[]]          void            command_set(ZText*, const std::string, const ztext::CommandLambda) noexcept;
	[[]]          void            command_destroy(ZText*, const std::string) noexcept;
	[[nodiscard]] Element*        command_content(Element*) noexcept;
	[[]]          void            command_content_set(Element*, Element*) noexcept;
	[[nodiscard]] MapStringString command_property(Element*) noexcept;
	[[]]          void            command_property_set(Element*, const std::string, const std::string) noexcept;

	[[]]          void            variable_set(ZText*, const std::string, const std::string) noexcept;
	[[]]          void            variable_destroy(ZText*, const std::string) noexcept;

	#ifdef ZTEXT_DEBUG_ENABLED
	[[]]          void            debug_element(Element*) noexcept;
	[[]]          void            debug_command(ZText*) noexcept;
	[[]]          void            debug_variable(ZText*) noexcept;
	#endif
}

#ifdef ZTEXT_IMPLEMENTATION // {{{

// {{{ Error

namespace ztext
{
	/**
	 * \class ErrorCategary_
	 *
	 * \brief Error categories.
	 *
	 * This class holds all the error categories for the error codes. The 
	 * data in this class is built from the ZTEXT__ERROR_DATA macro.
	 */


	/**
	 * \fn ErrorCategory_::ErrorCategory_()
	 *
	 * \brief Constructor
	 */


	/**
	 * \brief The name of the error category.
	 * 
	 * \return A C-Style string.
	 */
	const char* ErrorCategory_::name() const noexcept
	{
		return "ztext";
	}


	/**
	 * \brief A description message.
	 *
	 * \return The message.
	 */
	std::string ErrorCategory_::message(int condition ///< The error code.
		) const noexcept
	{
		switch(condition)
		{
			#define X(name_, val_, mesg_) \
			case val_: return mesg_;
				ZTEXT__ERROR_DATA
			#undef X
		}

		return "Unknown error condition";
	}


	/**
	 * \brief A single instance.
	 *
	 * This one instance will be used by all error codes.
	 */
	__attribute__((visibility ("hidden")))
	ErrorCategory_ ErrorCategory;

}

// }}}
// {{{ Datatypes

namespace ztext
{
	enum class Type : uint8_t
	{	Root
	,	Text
	,	Variable
	,	Command
	};


	struct Element
	{
		ZText*          ztext    = nullptr;
		Element*        next     = nullptr;
		Element*        prev     = nullptr;
		Element*        child    = nullptr;
		MapStringString property = {};
		std::string     text     = {};
		Type            type     = Type::Text;
	};


	struct ZText
	{
		Element*                                       root     = nullptr;
		MapStringString                                variable = {};
		std::unordered_map<std::string, CommandLambda> command  = {};
	};
}

// }}}
// {{{ Private

namespace
{
	ztext::Element* find_tail_(ztext::Element* element
		) noexcept
	{
		while(element->next != nullptr)
		{
			element = element->next;
		}

		return element;
	}

	std::string to_string(const ztext::Type type
		) noexcept
	{
		switch(type)
		{
			case ztext::Type::Root:     return "root";
			case ztext::Type::Variable: return "variable";
			case ztext::Type::Text:     return "text";
			case ztext::Type::Command:  return "command";
		}

		return {};
	}
}

// }}}
// {{{ Utility
// {{{ Utility: create/destroy

ztext::ZText* ztext::create() noexcept
{
	ztext::ZText* ztext = new ztext::ZText;

	return ztext;
};

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("create")
{
	ztext::ZText* zt = ztext::create();

	CHECK(zt != nullptr);

	ztext::destroy(zt);
}
#endif // }}}


void ztext::destroy(ztext::ZText*& ztext
	) noexcept
{
	ztext::clear(ztext);

	delete ztext;
	ztext = nullptr;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("destroy")
{
	ztext::ZText* zt = ztext::create();
	ztext::destroy(zt);

	CHECK(zt == nullptr);
}
#endif // }}}

// }}}
// {{{ Utility: clear

void ztext::clear(ztext::ZText* ztext
	) noexcept
{
	if(ztext == nullptr)
	{
		// Error
	}

	ztext::clear_commands(ztext);
	ztext::clear_elements(ztext);
	ztext::clear_variables(ztext);
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("clear")
{
	ztext::ZText*   zt      = ztext::create();
	ztext::Element* element = ztext::element_text_create("text");
	ztext::variable_set(zt, "foo", "bar");
	ztext::command_set(zt, "cmd", [](ztext::ZText*, ztext::Element*){return std::string();});

	ztext::element_append(zt, nullptr, element);

	CHECK(zt                           != nullptr);
	CHECK(element                      != nullptr);
	CHECK(zt->root                     == element);
	CHECK(zt->variable.contains("foo") == true);
	CHECK(zt->command.contains("cmd")  == true);

	clear(zt);

	CHECK(zt                           != nullptr);
	CHECK(zt->root                     == nullptr);
	CHECK(zt->variable.contains("foo") == false);
	CHECK(zt->command.contains("cmd")  == false);

	destroy(zt);
}
#endif // }}}


void ztext::clear_commands(ztext::ZText* ztext
	) noexcept
{
	if(ztext == nullptr)
	{
		// Error
	}

	ztext->command = {};
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("clear/command")
{
	ztext::ZText* zt = ztext::create();
	ztext::command_set(zt, "cmd", [](ztext::ZText*, ztext::Element*){return std::string();});

	ztext::Element* element = ztext::element_text_create("text");
	ztext::element_append(zt, nullptr, element);

	CHECK(zt                          != nullptr);
	CHECK(zt->command.contains("cmd") == true);
	CHECK(zt->root                    == element);

	ztext::clear_commands(zt);

	CHECK(zt->command.contains("cmd") == false);
	CHECK(zt->root                    == element);

	destroy(zt);
}
#endif // }}}


void ztext::clear_elements(ztext::ZText* ztext
	) noexcept
{
	if(ztext == nullptr)
	{
		// Error
	}

	ztext::Element* element = ztext->root;
	while(element != nullptr)
	{
		element = ztext::element_destroy(element);
	}

	ztext->root = nullptr;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("clear/element")
{
	ztext::ZText*   zt      = ztext::create();
	ztext::command_set(zt, "cmd", [](ztext::ZText*, ztext::Element*){return std::string();});

	ztext::Element* element = ztext::element_text_create("text");
	ztext::element_append(zt, nullptr, element);

	CHECK(zt       != nullptr);
	CHECK(element  != nullptr);
	CHECK(zt->root == element);

	clear_elements(zt);

	CHECK(zt->command.contains("cmd") == true);
	CHECK(zt->root                    == nullptr);

	destroy(zt);
}
#endif // }}}


void ztext::clear_variables(ztext::ZText* ztext
	) noexcept
{
	if(ztext == nullptr)
	{
		// Error
	}

	ztext->variable = {};
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("clear/variable")
{
	ztext::ZText* zt = ztext::create();
	ztext::variable_set(zt, "foo", "bar");

	ztext::Element* element = ztext::element_text_create("text");
	ztext::element_append(zt, nullptr, element);

	CHECK(zt                           != nullptr);
	CHECK(zt->variable.contains("foo") == true);
	CHECK(zt->root                     == element);

	clear_variables(zt);

	CHECK(zt->variable.contains("foo") == false);
	CHECK(zt->root                     == element);

	destroy(zt);
}
#endif // }}}

// }}}
// {{{ Utility: parse


// }}}
// {{{ Utility: root_element

ztext::Element* ztext::root_element(ztext::ZText* ztext
	) noexcept
{
	if(ztext == nullptr)
	{
		// Error
	}

	return ztext->root;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("root_element")
{
	ztext::ZText*   zt   = ztext::create();
	ztext::Element* foo  = ztext::element_text_create("foo");
	ztext::Element* bar  = ztext::element_text_create("bar");
	ztext::Element* root = ztext::root_element(zt);

	CHECK(root == nullptr);

	ztext::element_append(zt, nullptr, foo);
	root = ztext::root_element(zt);
	CHECK(root == foo);
	CHECK(ztext::element_eval(root) == "foo");

	ztext::element_append(zt, nullptr, bar);
	root = ztext::root_element(zt);
	CHECK(root == bar);
	CHECK(ztext::element_eval(root) == "bar");

	destroy(zt);
}
#endif // }}}

// }}}
// }}}
// {{{ Element

std::error_code ztext::element_append(ztext::ZText* ztext
	, ztext::Element* position
	, ztext::Element* element
	) noexcept
{
	if(ztext == nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
		#endif
		return Error_Invalid_Parameter;
	}

	if(element == nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be NULL."
			<< '\n';
		#endif
		return Error_Invalid_Parameter;
	}

	if(element->ztext != nullptr
		&& element->ztext != ztext
		)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Element In-Use: 'element' is being used by another ZText."
			<< '\n';
		#endif
		return Error_Element_In_Use;
	}

	element->ztext = ztext;

	if(position == nullptr)
	{
		if(ztext->root != nullptr)
		{
			element->next     = ztext->root;
			ztext->root->prev = element;
		}

		ztext->root = element;

		return Error_None;
	}

	element->prev  = position;
	element->next  = position->next;
	position->next = element;

	if(element->next != nullptr)
	{
		element->next->prev = element;
	}

	return Error_None;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/append")
{
	ztext::ZText*   zt   = ztext::create();
	ztext::Element* foo  = ztext::element_text_create("foo");
	ztext::Element* bar  = ztext::element_text_create("bar");

	ztext::element_append(zt, nullptr, foo);
	ztext::element_append(zt, foo    , bar);

	CHECK(ztext::element_next(foo) == bar);
	CHECK(ztext::element_next(bar) == nullptr);

	CHECK(ztext::element_prev(bar) == foo);
	CHECK(ztext::element_prev(foo) == nullptr);

	ztext::destroy(zt);
}
#endif // }}}


ztext::Element* ztext::element_destroy(ztext::Element*& element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR << "Parameter 'element' can not be null\n";
	}
	#endif

	element_remove(element);

	while(element->child != nullptr)
	{
		element->child = element_destroy(element->child);
	}

	ztext::Element* retval = element->next;

	element->ztext    = nullptr;
	element->next     = nullptr;
	element->prev     = nullptr;
	element->child    = nullptr;
	element->property = {};
	element->text     = {};
	element->type     = Type::Text;

	delete element;
	element = nullptr;

	return retval;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/destroy")
{
	ztext::ZText*   zt   = ztext::create();
	ztext::Element* foo  = ztext::element_text_create("foo");
	ztext::Element* bar  = ztext::element_text_create("bar");
	ztext::Element* test = ztext::element_text_create("test");

	ztext::element_append(zt, nullptr, foo);
	ztext::element_append(zt, foo    , test);
	ztext::element_append(zt, test   , bar);

	CHECK(ztext::element_next(foo) == test);
	CHECK(ztext::element_prev(bar) == test);

	ztext::element_destroy(test);
	CHECK(ztext::element_next(foo)  == bar);
	CHECK(ztext::element_prev(bar)  == foo);

	CHECK(test == nullptr);

	ztext::destroy(zt);
}
#endif // }}}


void ztext::element_remove(ztext::Element* element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR << "Parameter 'element' can not be null\n";
	}
	#endif

	if(element->next != nullptr)
	{
		element->next->prev = element->prev;
	}

	if(element->prev != nullptr)
	{
		element->prev->next = element->next;
	}

	element->next  = nullptr;
	element->prev  = nullptr;
	element->ztext = nullptr;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/remove")
{
	ztext::ZText*   zt   = ztext::create();
	ztext::Element* foo  = ztext::element_text_create("foo");
	ztext::Element* bar  = ztext::element_text_create("bar");
	ztext::Element* test = ztext::element_text_create("test");

	ztext::element_append(zt, nullptr, foo);
	ztext::element_append(zt, foo    , test);
	ztext::element_append(zt, test   , bar);

	CHECK(ztext::element_next(foo) == test);
	CHECK(ztext::element_prev(bar) == test);

	ztext::element_remove(test);
	CHECK(ztext::element_next(foo)  == bar);
	CHECK(ztext::element_prev(bar)  == foo);
	CHECK(ztext::element_next(test) == nullptr);
	CHECK(ztext::element_prev(test) == nullptr);

	CHECK(test->next  == nullptr);
	CHECK(test->prev  == nullptr);
	CHECK(test->ztext == nullptr);

	ztext::destroy(zt);
}
#endif // }}}


ztext::Element* ztext::element_next(ztext::Element* element
		) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR << "Parameter 'element' can not be null\n";
	}
	#endif

	return element->next;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/next")
{
	ztext::ZText*   zt  = ztext::create();
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");

	ztext::element_append(zt, nullptr, foo);
	ztext::element_append(zt, foo    , bar);

	ztext::Element* element = ztext::root_element(zt);
	CHECK(element == foo);

	element = ztext::element_next(element);
	CHECK(element == bar);

	element = ztext::element_next(element);
	CHECK(element == nullptr);

	destroy(zt);
}
#endif // }}}


ztext::Element* ztext::element_prev(ztext::Element* element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR << "Parameter 'element' can not be null\n";
	}
	#endif

	return element->prev;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/prev")
{
	ztext::ZText*   zt  = ztext::create();
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");

	ztext::element_append(zt, nullptr, foo);
	ztext::element_append(zt, foo    , bar);

	ztext::Element* element = bar;

	element = ztext::element_prev(element);
	CHECK(element == foo);

	element = ztext::element_prev(element);
	CHECK(element == nullptr);

	destroy(zt);
}
#endif // }}}


std::string ztext::element_eval(ztext::Element* element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR << "Parameter 'element' can not be null\n";
	}
	#endif

	switch(element->type)
	{
		case ztext::Type::Root:     return {};
		case ztext::Type::Text:     return element->text;
		//case ztext::Type::Variable: return element_eval_variable_(element);
		case ztext::Type::Variable: ZTEXT_ERROR << "Not Implemented\n"; break;//return eval_command(element);
		case ztext::Type::Command:  ZTEXT_ERROR << "Not Implemented\n"; break;//return eval_command(element);
	}

	return {};
}

// {{{ Element: Text

ztext::Element* ztext::element_text_create(const std::string& string
	) noexcept
{
	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Text;
	element->text = string;

	return element;
}

// }}}
// }}}
// {{{ Debugging

void ztext::print(const ztext::Element* element
	, const bool to_end
	) noexcept
{
	const Element* e = element;

	do
	{
		printf("element: %p\n", (void*)e);
		printf("\tztext: %p\n", (void*)e->ztext);
		printf("\t next: %p\n", (void*)e->next);
		printf("\t prev: %p\n", (void*)e->prev);
		printf("\t type: %s\n", to_string(e->type).c_str());

		e = e->next;
	} while(e != nullptr && to_end == true);
}

// }}}


// --- Deprecated --- //
namespace ztext
{

	ztext::Element* element_create_(ztext::ZText* ztext
		, ztext::Type type
		) noexcept
	{
		ztext::Element* element = new ztext::Element;
		element->ztext = ztext;
		element->type  = type;

		return element;
	}


	std::string string_trim_(std::string string
		) noexcept
	{
		size_t len   = string.size();
		size_t start = 0;

		while(start < len
			&& std::isspace(static_cast<unsigned char>(string[start])) != 0
			)
		{
			start++;
		}

		size_t end = len - 1;

		while(end > 0
			&& std::isspace(static_cast<unsigned char>(string[end])) != 0
			)
		{
			end--;
		}

		return string.substr(start, end - start + 1);
	}


	std::string string_clean_escapes_(std::string string
		) noexcept
	{
		size_t index = 0;

		while(index < string.size())
		{
			if(string[index] != '\\')
			{
				index++;

				continue;
			}

			if((index + 2) < string.size())
			{
				if(string[index + 1] == '{'
					&& string[index + 2] == '{'
					)
				{
					string.erase(index, 1);
				}
				else if(string[index + 1] == '}'
					&& string[index + 2] == '}'
					)
				{
					string.erase(index, 1);
				}
			}

			index += 2;
		}

		return string;
	}


	std::string string_clean_whitespace_(std::string string
		) noexcept
	{
		size_t index_1 = 0;
		size_t index_2 = 0;

		while(index_1 < string.size())
		{
			if(std::isspace(static_cast<unsigned char>(string[index_1])) == 0)
			{
				index_1++;
				continue;
			}

			index_2 = index_1 + 1;

			while(std::isspace(static_cast<unsigned char>(string[index_2])) != 0)
			{
				index_2++;
			}

			string.replace(index_1, index_2 - index_1, " ");

			index_1++;
		}

		return string;
	}


	size_t string_skip_whitespace_(const std::string& string
		, size_t index
		) noexcept
	{
		while(index < string.size())
		{
			if(std::isspace(static_cast<unsigned char>(string[index])) == 0)
			{
				break;
			}

			index++;
		}

		return index;
	}


	bool validate_token_name_character_(const unsigned char c
		) noexcept
	{
		if(std::isalnum(c) != 0
			|| c == '-'
			|| c == '_'
			)
		{
			return true;
		}

		return false;
	}

	// {{{ Parse

	constexpr char Identifier_Command  = '(';
	constexpr char Identifier_Variable = '$';
	constexpr char Assignment          = '=';

	struct Token
	{
		size_t begin          = 0;
		size_t end            = 0;
		size_t name_begin     = 0;
		size_t name_end       = 0;
		size_t property_begin = 0;
		size_t property_end   = 0;
		size_t content_begin  = 0;
		size_t content_end    = 0;
		size_t identifier     = 0;
		size_t assignment     = 0;
		bool   is_valid       = false;
	};


	std::error_code parse_text_(ztext::ZText* ztext
		, const std::string& string
		, size_t&            index_begin
		, size_t&            index_end
		, Element*&          element
		) noexcept
	{
		const size_t string_len = string.size();
printf("--- 3.0 --- len: %lu\n", string_len);

		index_end = index_begin + 1;

printf("--- 3.1 --- %lu\n", index_end);
		while(index_end < string_len)
		{
			if(string[index_end] == '{')
			{
				if(string[index_end - 1] != '\\'
					&& (index_end + 1) < string_len
					&& string[index_end + 1] == '{'
					)
				{
					break;
				}
			}

			index_end++;
		}

printf("--- 3.2 --- %lu\n", index_end);
		std::string text = string.substr(index_begin, index_end);
printf("--- 3.3 --- '%s'\n", text.c_str());
		text = string_trim_(text);
printf("--- 3.4 --- '%s'\n", text.c_str());
		text = string_clean_whitespace_(text);
printf("--- 3.5 --- '%s'\n", text.c_str());
		text = string_clean_escapes_(text);
printf("--- 3.6 --- '%s'\n", text.c_str());

		if(text.empty() == true)
		{
			element = nullptr;

			return Error_Parser_No_Text_Found;
		}

		element = element_create_(ztext, Type::Text);
		element->text = text;

		return Error_None;
	}


	void debug_token_(const std::string_view& string
		, const Token& token
		) noexcept
	{
		size_t len = string.size();

		printf("--- Token ---\n");
		printf("token     : %lu,%lu (%s)\n"
			, token.begin
			, token.end
			, (token.begin >= token.end || token.begin >= len || token.end >= len) ? "" : std::string(string.substr(token.begin, (token.end - token.begin + 1))).c_str()
			);
		printf("name      : %lu,%lu (%s)\n"
			, token.name_begin
			, token.name_end
			, (token.name_begin >= token.name_end || token.name_begin >= len || token.name_end >= len) ? "" : std::string(string.substr(token.name_begin, (token.name_end - token.name_begin + 1))).c_str()
			);
		printf("identifier: %lu (%c)\n"
			, token.identifier
			, (token.identifier == 0 || token.identifier >= len || token.identifier >= len) ? '\0' : string[token.identifier]
			);
	}


	std::error_code parse_token_name_(const std::string& string
		, Token& token
		) noexcept
	{
		size_t index = string_skip_whitespace_(string, token.begin + 2);

		if(string[index] == '$'
			|| string[index] == '('
			|| string[index] == '}'
			)
		{
			return Error_Parser_Token_Name_Missing;
		}

		token.name_begin = index;

		while(validate_token_name_character_(string[index]) == true)
		{
			index++;
		}

		if(token.name_begin == index)
		{
			return Error_Parser_Invalid_Token_Name;
		}

		token.name_end = index - 1;

		return Error_None;
	}


	std::error_code parse_token_identifier_(const std::string& string
		, Token& token
		) noexcept
	{
		debug_token_(string, token);

		size_t index = string_skip_whitespace_(string, token.name_end + 1);

		if(string[index] == '$')
		{
			token.identifier = index;

			return Error_None;
		}

		return Error_Parser_Invalid_Token_Identifier;
	}

	
	std::error_code parse_token_variable_(ZText* ztext
		, const std::string& string
		, Token&             token
		, Element*&          element
		) noexcept
	{
printf("--- %s ---\n", __FUNCTION__);
debug_token_(string, token);
		size_t index = 0;

		const std::string name = string.substr(token.name_begin, token.name_end - token.name_begin + 1);

		// Is variable being set?
		index = token.identifier + 1;
		index = string_skip_whitespace_(string, index);

		std::string value = "";
		Element* content = nullptr;
		if(string[index] == '=')
		{
			index = string_skip_whitespace_(string, index + 1);
			content = element_create_(ztext, Type::Text);
			content->text = string.substr(index, token.end - index - 1);
			content->text = string_trim_(content->text);
			content->text = string_clean_whitespace_(content->text);
			//content->text = string_clean_escapes_(content->text);

printf("content:\n");
debug_element(content);
		}

		element = element_create_(ztext, Type::Variable);
		element->text  = name;
		element->child = content;
printf("element:\n");
debug_element(element);

		return Error_None;
	}


	std::error_code parse_token_(ztext::ZText* ztext
		, const std::string& string
		, size_t&            index_begin
		, size_t&            index_end
		, Element*&          element
		) noexcept
	{
		element = nullptr;

		Token token;
		token.begin = index_begin;
		index_begin += 2;
		index_end = index_begin;

		// BUG - This will not work with nested tokens
		while(index_end < string.size())
		{
			if(string[index_end] == '}'
				&& string[index_end - 1] != '\\'
				&& string[index_end + 1] == '}'
				)
			{
				token.end = index_end + 1;
				break;
			}

			index_end++;
		}

printf("--- 2.0 --- %lu >= %lu\n", index_end , string.size());
		if(index_end >= string.size())
		{
			return Error_Parser_Token_End_Marker_Missing;
		}

printf("--- 3.0 ---\n");
		std::error_code error = parse_token_name_(string, token);
		if(error != Error_None)
		{
			return error;
		}

printf("--- 4.0 ---\n");

		error = parse_token_identifier_(string, token);
		if(error != Error_None)
		{
			return error;
		}

printf("--- 5.0 ---\n");
		if(string[token.identifier] == '$')
		{
			error = parse_token_variable_(ztext, string, token, element);
		}

		return Error_None;
	}


	std::error_code parse(ZText* ztext
		, const std::string& string
		) noexcept
	{
		if(ztext == nullptr)
		{
			#if ZTEXT_DEBUG_ENABLED
			ZTEXT_ERROR << "Parameter 'ztext' can not be null\n";
			#endif

			return Error_Invalid_Parameter;
		}

		if(string.empty() == true)
		{
			#if ZTEXT_DEBUG_ENABLED
			ZTEXT_ERROR << "Parameter 'string' can not be empty\n";
			#endif

			return Error_Invalid_Parameter;
		}

		Element*        tail        = find_tail_(ztext->root);
		Element*        element     = nullptr;
		size_t          index_begin = string_skip_whitespace_(string, 0);
		size_t          index_end   = 0;
		std::error_code error       = {};

		while(index_begin < string.size())
		{
printf("--- 1 --- %c %lu\n", string[index_begin], index_begin);
			if((index_begin + 1) < string.size()
				&& string[index_begin + 0] == '{'
				&& string[index_begin + 1] == '{'
				)
			{
printf("--- 2 --- %c %lu\n", string[index_begin], index_begin);
				error = parse_token_(ztext, string, index_begin, index_end, element);
			}
			else
			{
printf("--- 3 --- %c %lu\n", string[index_begin], index_begin);
				error = parse_text_(ztext, string, index_begin, index_end, element);
			}

printf("--- 4 --- %p\n", (void*)element);
			if(element == nullptr)
			{
				return error;
			}

			element_append(ztext, tail, element);
			tail = element;
			element = nullptr;
			index_begin = string_skip_whitespace_(string, index_end);
		}


#if 0
		Type     token_type = Type::Text;

		if(string[0] == '{' && string[1] == '{')
		{
			token_type = parse_token_type(string, index);

			if(token_type == Type::Variable)
			{
				element = parse_variable(ztext, string, index);
			}

			if(token_type == Type::Command)
			{
				element = parse_command(ztext, string, index);
			}

			if(element)
			{
				debug_element(element);
			}
		}
#endif

		return Error_None;
	}

	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{ parse/text
	TEST_CASE("parse/text")
	{
		ztext::ZText*   zt    = create();
#if 0
		ztext::Element* root  = ztext::root_element(zt);
		std::error_code error = {};

		SUBCASE("Invaild Data")
		{
			error = ztext::parse(nullptr, "foo");
			CHECK(error == Error_Invalid_Parameter);

			error = ztext::parse(zt, "");
			CHECK(error == Error_Invalid_Parameter);
		}

		SUBCASE("Pure White-Space")
		{
			ztext::Element* element = nullptr;

			std::string newlines = "\n\n\n";
			std::string spaces   = "   ";
			std::string tabs     = "		";

			// -------------------------------------- //

			error = ztext::parse(zt, newlines);
			CHECK(error == Error_None);
			
			element = ztext::element_next(root);
			CHECK(element == nullptr);

			// -------------------------------------- //

			error = ztext::parse(zt, spaces);
			CHECK(error == Error_None);
			
			element = ztext::element_next(root);
			CHECK(element == nullptr);

			// -------------------------------------- //

			error = ztext::parse(zt, tabs);
			CHECK(error == Error_None);
			
			element = ztext::element_next(root);
			CHECK(element == nullptr);
		}

		SUBCASE("Simple Text")
		{
			std::string text = "X";

			error = ztext::parse(zt, text);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == text);
		}

		SUBCASE("Leading White-Space")
		{
			std::string text = "X";
			std::string text_complex = " " + text;

			error = ztext::parse(zt, text_complex);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == text);
		}

		SUBCASE("Trailing White-Space")
		{
			std::string text = "X";
			std::string text_complex = text + " ";

			error = ztext::parse(zt, text_complex);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == text);
		}

		SUBCASE("Leading and Trailing White-Space")
		{
			std::string text = "X";
			std::string text_complex = "	" + text + "	";

			error = ztext::parse(zt, text_complex);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == text);
		}

		SUBCASE("Clean White-Space")
		{
			std::string text = "X	Y  Z";

			error = ztext::parse(zt, text);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == "X Y Z");
		}

		SUBCASE("Multi-Line")
		{
			std::string text = " \
				X            \
				Y            \
				Z            \
				";

			error = ztext::parse(zt, text);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == "X Y Z");
		}

		SUBCASE("Before Token")
		{
			std::string text = "foo {{token$}}";

			error = ztext::parse(zt, text);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == "foo");
		}

		SUBCASE("Escaped Token")
		{
			std::string text = "\\{{token\\}}";

			error = ztext::parse(zt, text);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == "{{token}}");
		}

		SUBCASE("Embedded Escaped Token")
		{
			std::string text = "foo \\{{token\\}} bar";

			error = ztext::parse(zt, text);
			CHECK(error == Error_None);
			
			ztext::Element* element = ztext::element_next(root);
			CHECK(ztext::element_eval(element) == "foo {{token}} bar");
		}
#endif

		destroy(zt);
	}
	#endif // }}}
	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{ parse/variable
	TEST_CASE("parse/variable")
	{
		ztext::ZText*   zt    = create();
#if 0
		ztext::Element* root  = ztext::root_element(zt);
		std::error_code error = {};

		SUBCASE("Invaild Data")
		{
			error = ztext::parse(zt, "{{");
			CHECK(error == Error_Parser_Token_End_Marker_Missing);

			// -------------------------------------- //

			error = ztext::parse(zt, "{{var$");
			CHECK(error == Error_Parser_Token_End_Marker_Missing);

			// -------------------------------------- //

			error = ztext::parse(zt, "{{var$\\}}");
			CHECK(error == Error_Parser_Token_End_Marker_Missing);

			// -------------------------------------- //

			error = ztext::parse(zt, "{{}}");
			CHECK(error == Error_Parser_Token_Name_Missing);

			// -------------------------------------- //

			error = ztext::parse(zt, "{{$}}");
			CHECK(error == Error_Parser_Token_Name_Missing);

			// -------------------------------------- //

			error = ztext::parse(zt, "{{*$}}");
			CHECK(error == Error_Parser_Invalid_Token_Name);
		}

		SUBCASE("Variable")
		{
			error = ztext::parse(zt, "{{var$}}");
			CHECK(error == Error_None);

			ztext::Element* element = ztext::element_next(root);

			CHECK(element       != nullptr);
			CHECK(element->type == ztext::Type::Variable);
			CHECK(element->text == "var");
			CHECK(ztext::element_eval(element) == "");
		}

		SUBCASE("Variable With White-Space")
		{
			error = ztext::parse(zt, " {{ var $ }} ");
			CHECK(error == Error_None);

			ztext::Element* element = ztext::element_next(root);

			CHECK(element       != nullptr);
			CHECK(element->type == ztext::Type::Variable);
			CHECK(element->text == "var");
			CHECK(ztext::element_eval(element) == "");
		}

		SUBCASE("Variable With Data")
		{
			error = ztext::parse(zt, "{{var$=foo}}");
			CHECK(error == Error_None);

			ztext::Element* element = ztext::element_next(root);

			CHECK(element       != nullptr);
			CHECK(element->type == ztext::Type::Variable);
			CHECK(element->text == "var");
			CHECK(ztext::element_eval(element) == "foo");
		}

		SUBCASE("Variable With Data and White-Space")
		{
			error = ztext::parse(zt, " {{ var $ = foo }} ");
			CHECK(error == Error_None);

			ztext::Element* element = ztext::element_next(root);

			CHECK(element       != nullptr);
			CHECK(element->type == ztext::Type::Variable);
			CHECK(element->text == "var");
			CHECK(ztext::element_eval(element) == "foo");
		}

		SUBCASE("Variable With Cleaned Data")
		{
			error = ztext::parse(zt, " {{ var$ = \
				foo	\
				bar	\
				}} ");
			CHECK(error == Error_None);

			ztext::Element* element = ztext::element_next(root);

			CHECK(element       != nullptr);
			CHECK(element->type == ztext::Type::Variable);
			CHECK(element->text == "var");
			CHECK(ztext::element_eval(element) == "foo bar");
		}

#endif
		SUBCASE("Variable With Nested Variables")
		{
		}

		SUBCASE("Variable With Nested Variables Recursive")
		{
		}

		destroy(zt);
	}
	#endif // }}}

	// }}}
	// {{{ Element

	std::string element_eval_variable_(Element* element
		) noexcept
	{
printf("--- %s ---\n", __FUNCTION__);
//debug_token_(string, token);
debug_element(element);

		if(element->ztext->variable.contains(element->text) == false)
		{
			element->ztext->variable[element->text] = "";
		}

		if(element->child != nullptr)
		{
			std::string value = element_eval(element->child);
			element->ztext->variable[element->text] = value;
		}

		std::string string = element->ztext->variable[element->text];

		return string;
	}
	// }}}

	Type parse_token_type(const std::string& string
		, const size_t start
		) noexcept
	{
		size_t index = start + 2;
		if(string[index] == '$')
		{
			return Type::Variable;
		}

		return Type::Command;
	}


	Element* parse_command(ZText* ztext
		, const std::string& string
		, size_t&            index
		) noexcept
	{
		std::string name = {};

		size_t start = index + 2;
		while(index < string.size())
		{
			if(isspace(string[index]))
			{
				name = string.substr(start, index - start);
				break;
			}

			if(string[index] == '(')
			{
				name = string.substr(start, index - start);
				break;
			}

			if(string[index] == '}')
			{
				if((index + 1) < string.size()
					&& string[index - 1] != '\\'
					&& string[index + 1] == '}'
					)
				{
					name = string.substr(start, index - start);
					break;
				}
			}

			index++;
		}

		if(name.empty() == true)
		{
			// Error

			return nullptr;
		}

		Element* element = element_create_(ztext
			, Type::Command
			);
		element->text = name;

		if(string[index] == '(')
		{
		}

		return element;
	}


	void command_set(ZText* ztext
		, const std::string   name
		, const CommandLambda command
		) noexcept
	{
		if(ztext == nullptr)
		{
			// Error
		}

		if(name.empty())
		{
			// Error
		}

		if(command == nullptr)
		{
			// Error
		}

		ztext->command[name] = command;
	}


	void command_destroy(ZText* ztext
		, const std::string name
		) noexcept
	{
		if(ztext == nullptr)
		{
			// Error
		}

		if(name.empty())
		{
			// Error
		}

		if(ztext->command.contains(name) == false)
		{
			// Error
		}

		ztext->command.erase(name);
	}


	Element* command_content(Element* command
		) noexcept
	{
		if(command == nullptr)
		{
			// Error
		}

		if(command->type != Type::Command)
		{
			// Error
		}

		if(command->child == nullptr)
		{
			command->child = element_create_(command->ztext
				, Type::Text
				);
		}

		return command->child;
	}


	void command_content_set(Element* command
		, Element* content
		) noexcept
	{
		if(command == nullptr)
		{
			// Error
		}

		if(command->type != Type::Command)
		{
			// Error
		}

		if(content == nullptr)
		{
			// Error
		}

		while(command->child != nullptr)
		{
			command->child = element_destroy(command->child);
		}

		command->child = content;
	}


	void command_property_set(Element* command
		, const std::string name
		, const std::string value
		) noexcept
	{
		if(command == nullptr)
		{
			// Error
		}

		if(command->type != Type::Command)
		{
			// Error
		}

		if(name.empty())
		{
			// Error
		}

		command->property[name] = value;
	}


	MapStringString command_property(Element* command
		) noexcept
	{
		if(command == nullptr)
		{
			// Error
		}

		if(command->type != Type::Command)
		{
			// Error
		}

		return command->property;
	}


	void variable_set(ZText* ztext
		, const std::string name
		, const std::string value
		) noexcept
	{
		if(ztext == nullptr)
		{
			// Error
		}

		if(name.empty())
		{
			// Error
		}

		ztext->variable[name] = value;
	}


	void variable_destroy(ZText* ztext
		, const std::string    name
		) noexcept
	{
		if(ztext == nullptr)
		{
			// Error
		}

		if(name.empty())
		{
			// Error
		}

		if(ztext->variable.contains(name) == false)
		{
			// Error
		}

		ztext->variable.erase(name);
	}



	Element* element_append_command(Element* element
		, const std::string command_name
		) noexcept
	{
		if(element == nullptr)
		{
			// error
		}

		Element* new_element = element_create_(element->ztext
			, Type::Command
			);
		new_element->text = command_name;

		element_append(element->ztext, element, new_element);

		return new_element;
	}


	Element* element_append_text(Element* element
		, const std::string           text
		) noexcept
	{
		if(element == nullptr)
		{
			// error
		}

		Element* new_element = element_create_(element->ztext
			, Type::Text
			);
		new_element->text = text;

		element_append(element->ztext, element, new_element);

		return new_element;
	}


	Element* element_append_variable(Element* element
		, const std::string               variable_name
		) noexcept
	{
		if(element == nullptr)
		{
			// error
		}

		Element* new_element = element_create_(element->ztext
			, Type::Variable
			);
		new_element->text = variable_name;

		element_append(element->ztext, element, new_element);

		return new_element;
	}


	std::string eval_command(Element* element
		) noexcept
	{
		std::string name = element->text;

		if(element->ztext->command.contains(name) == false)
		{
			return {};
		}

		std::string value = element->ztext->command[name](element->ztext, element);

		// Recursive Variables?

		return value;
	}


	std::string eval_variable(Element* element
		) noexcept
	{
		std::string name = element->text;

		if(element->ztext->variable.contains(name) == false)
		{
			return {};
		}

		std::string value = element->ztext->variable[name];

		// Recursive Variables?

		return value;
	}


	// {{{ debug
	#ifdef ZTEXT_DEBUG_ENABLED

	void debug_element(Element* element
		) noexcept
	{
		printf("id  : %p\n", element);
		printf("prev: %p\n", element->prev);
		printf("next: %p\n", element->next);


		size_t name_len = 0;

		if(element->type == Type::Command)
		{
			for(const auto& [ name, value ] : element->property)
			{
				name_len = std::max(name_len, name.size());
			}
		}

		switch(element->type)
		{
			case Type::Root:
				printf("type: %d (Root)\n", (int)element->type);
				break;
			case Type::Variable:
				printf("type: %d (Variable)\n", (int)element->type);
				printf("variable: %s\n", element->text.c_str());
				break;
			case Type::Text:
				printf("type: %d (Text)\n", (int)element->type);
				printf("text: %s\n", element->text.c_str());
				break;
			case Type::Command:
				printf("type: %d (Command)\n", (int)element->type);
				printf("command: %s\n", element->text.c_str());
				for(const auto& [name, value] : element->property)
				{
					printf("\t%-*s: \"%s\"\n", (int)name_len, name.c_str(), value.c_str());
				}
				break;
		}
	}


	void debug_command(ZText* ztext
		) noexcept
	{
		for(auto& [ name, value ] : ztext->command)
		{
			printf("%s()\n", name.c_str());
		}
	}


	void debug_variable(ZText* ztext
		) noexcept
	{
		size_t name_len = 0;

		for(auto& [ name, value ] : ztext->variable)
		{
			name_len = std::max(name_len, name.size());
		}

		for(auto& [ name, value ] : ztext->variable)
		{
			printf("%-*s: \"%s\"\n", (int)name_len, name.c_str(), value.c_str());
		}
	}

	#endif
	// }}}
}

#endif // }}}
