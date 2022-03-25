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
	X(Error_None              ,  0 , "No Error"               ) \
	X(Error_Invalid_Parameter ,  1 , "A parameter is invalid" ) \


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


	[[nodiscard]] ZText*          create() noexcept;
	[[]]          void            destroy(ZText*&) noexcept;
	[[]]          void            clear(ZText*) noexcept;
	[[]]          void            clear_commands(ZText*) noexcept;
	[[]]          void            clear_elements(ZText*) noexcept;
	[[]]          void            clear_variables(ZText*) noexcept;
	[[]]          std::error_code parse(ZText*, const std::string) noexcept;

	[[nodiscard]] Element*        root_element(ZText*) noexcept;

	[[]]          Element*        element_append_command(Element*, const std::string) noexcept;
	[[]]          Element*        element_append_text(Element*, const std::string) noexcept;
	[[]]          Element*        element_append_variable(Element*, const std::string) noexcept;
	[[]]          Element*        element_destroy(Element*&) noexcept;
	[[nodiscard]] std::string     element_eval(Element*) noexcept;
	[[nodiscard]] Element*        element_next(Element*) noexcept;
	[[nodiscard]] Element*        element_prev(Element*) noexcept;

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

namespace ztext
{
	// {{{ Error

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
	ErrorCategory_ ErrorCategory;

	// }}}

	enum class Type : uint8_t
	{	Root
	,	Text
	,	Variable
	,	Command
	} type;


	struct Element
	{
		ZText*          ztext;
		Element*        next     = nullptr;
		Element*        prev     = nullptr;
		Element*        child    = nullptr;
		MapStringString property = {};
		std::string     text     = {};
		Type            type     = Type::Text;
	};


	struct ZText
	{
		MapStringString                                variable     = {};
		std::unordered_map<std::string, CommandLambda> command      = {};
		Element                                        root_element = {};
	};


	// {{{ Private

	ztext::Element* find_tail_(ztext::Element* element
		) noexcept
	{
		while(element->next != nullptr)
		{
			element = element->next;
		}

		return element;
	}


	void element_append_(Element* element_prev
		, Element* element
		) noexcept
	{
		Element* element_next = element_prev->next;

		element->next = element_next;
		element->prev = element_prev;

		element_prev->next = element;

		if(element_next)
		{
			element_next->prev = element;
		}
	}


	ztext::Element* element_create_(ztext::ZText* ztext
		, ztext::Type type
		) noexcept
	{
		ztext::Element* element = new ztext::Element;
		element->ztext = ztext;
		element->type     = type;

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


	std::string string_clean_whitespace(std::string string
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

	// }}}
	// {{{ Util
	// {{{ Util: create/destroy

	ZText* create() noexcept
	{
		ZText* ztext = new ZText;

		ztext->root_element.ztext = ztext;
		ztext->root_element.type  = Type::Root;

		return ztext;
	};

	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
	TEST_CASE("create")
	{
		ZText* ztext = create();

		CHECK(ztext                     != nullptr);
		CHECK(ztext->root_element.ztext == ztext);
		CHECK(ztext->root_element.type  == Type::Root);

		destroy(ztext);
	}
	#endif // }}}


	void destroy(ZText*& ztext
		) noexcept
	{
		clear(ztext);

		delete ztext;
		ztext = nullptr;
	}

	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
	TEST_CASE("destroy")
	{
		ZText* ztext = create();
		destroy(ztext);

		CHECK(ztext == nullptr);
	}
	#endif // }}}

	// }}}
	// {{{ Util: clear

	void clear(ZText* ztext
		) noexcept
	{
		if(ztext == nullptr)
		{
			// Error
		}

		clear_commands(ztext);
		clear_elements(ztext);
		clear_variables(ztext);
	}

	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
	TEST_CASE("clear")
	{
		ZText*   ztext   = create();
		Element* element = element_append_text(root_element(ztext)
			, "text");
		variable_set(ztext, "foo", "bar");
		command_set(ztext, "cmd", [](ZText*, Element*){return std::string();});

		CHECK(ztext                           != nullptr);
		CHECK(element                         != nullptr);
		CHECK(ztext->root_element.next        == element);
		CHECK(ztext->variable.contains("foo") == true);
		CHECK(ztext->command.contains("cmd")  == true);

		clear(ztext);

		CHECK(ztext                           != nullptr);
		CHECK(ztext->root_element.next        == nullptr);
		CHECK(ztext->variable.contains("foo") == false);
		CHECK(ztext->command.contains("cmd")  == false);

		destroy(ztext);
	}
	#endif // }}}


	void clear_commands(ZText* ztext
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
		ZText* ztext = create();
		command_set(ztext, "cmd", [](ZText*, Element*){return std::string();});

		CHECK(ztext                          != nullptr);
		CHECK(ztext->command.contains("cmd") == true);

		clear(ztext);

		CHECK(ztext->command.contains("cmd") == false);

		destroy(ztext);
	}
	#endif // }}}


	void clear_elements(ZText* ztext
		) noexcept
	{
		if(ztext == nullptr)
		{
			// Error
		}

		Element* element = ztext->root_element.next;
		while(element != nullptr)
		{
			element = element_destroy(element);
		}
	}

	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
	TEST_CASE("clear/element")
	{
		ZText*   ztext   = create();
		Element* element = element_append_text(root_element(ztext)
			, "text");

		CHECK(ztext                    != nullptr);
		CHECK(element                  != nullptr);
		CHECK(ztext->root_element.next == element);

		clear(ztext);

		CHECK(ztext->root_element.next == nullptr);

		destroy(ztext);
	}
	#endif // }}}


	void clear_variables(ZText* ztext
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
		ZText* ztext = create();
		variable_set(ztext, "foo", "bar");

		CHECK(ztext                           != nullptr);
		CHECK(ztext->variable.contains("foo") == true);

		clear(ztext);

		CHECK(ztext->variable.contains("foo") == false);

		destroy(ztext);
	}
	#endif // }}}

	// }}}
	// }}}
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
		size_t indentifier    = 0;
		size_t assignment     = 0;
		bool   is_valid       = false;
	};


	Element* parse_text_(ztext::ZText* ztext
		, const std::string& string
		, size_t&            index_begin
		, size_t&            index_end
		) noexcept
	{
		const size_t string_len = string.size();

		if(index_begin >= string_len)
		{
			return nullptr;
		}

		index_end = index_begin + 1;

		while(index_end < string_len)
		{
			if(string[index_end] == '{')
			{
				break;
			}

			index_end++;
		}

		std::string text = string.substr(index_begin, index_end);
		text = string_trim_(text);
		text = string_clean_whitespace(text);

		if(text.empty() == true)
		{
			return nullptr;
		}

		Element* element = element_create_(ztext, Type::Text);
		element->text = text;

		return element;
	}


	std::error_code parse(ZText* ztext
		, const std::string string
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

		Element* tail        = find_tail_(&ztext->root_element);
		Element* element     = nullptr;
		size_t   index_begin = 0;
		size_t   index_end   = 0;

		element = parse_text_(ztext, string, index_begin, index_end);

		if(element == nullptr)
		{
			return Error_None;
		}

		element_append_(tail, element);
		tail = element;


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

		destroy(zt);
	}
	#endif // }}}

	// }}}
	// {{{ Element

	std::string element_eval(Element* element
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
			case Type::Root:     return {};
			case Type::Text:     return element->text;
			case Type::Variable: ZTEXT_ERROR << "Not Implemented\n"; break;//return eval_variable(element);
			case Type::Command:  ZTEXT_ERROR << "Not Implemented\n"; break;//return eval_command(element);
		}

		return {};
	}

	Element* element_next(Element* element
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
		ztext::ZText*   zt      = create();
		ztext::Element* root    = ztext::root_element(zt);
		ztext::Element* text    = element_append_text(root, "foo");
		ztext::Element* element = element_next(root);

		CHECK(element == text);

		destroy(zt);
	}
	#endif // }}}

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


	Element* parse_variable(ZText* ztext
		, const std::string& string
		, size_t&            index
		) noexcept
	{
		std::string name = {};

		size_t start = index + 3;
		while(index < string.size())
		{
			if(isspace(string[index]))
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
			, Type::Variable
			);
		element->text = name;

		if(string[index] == '}')
		{
			index += 2;
			return element;
		}

		while(isspace(string[index]))
		{
			index++;
		}

		start = index;
		std::string value = {};

		while(index < string.size())
		{
			if(string[index] == '}')
			{
				if((index + 1) < string.size()
					&& string[index - 1] != '\\'
					&& string[index + 1] == '}'
					)
				{
					value = string.substr(start, index - start);
					break;
				}
			}

			index++;
		}

		if(value.empty())
		{
			// error
			delete element;
			element = nullptr;

			return nullptr;
		}

		variable_set(ztext, name, value);

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


	Element* root_element(ZText* ztext
		) noexcept
	{
		if(ztext == nullptr)
		{
			// Error
		}

		return &ztext->root_element;
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

		element_append_(element, new_element);

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

		element_append_(element, new_element);

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

		element_append_(element, new_element);

		return new_element;
	}


	Element* element_destroy(Element*& element
		) noexcept
	{
		if(element == nullptr)
		{
			// error
		}

		if(element->type == Type::Root)
		{
			// error
		}

		Element* element_prev = element->prev;
		Element* element_next = element->next;

		if(element_prev)
		{
			element_prev->next = element_next;
		}

		if(element_next)
		{
			element_next->prev = element_prev;
		}

		while(element->child != nullptr)
		{
			element->child = element_destroy(element->child);
		}

		element->ztext = nullptr;
		element->next     = nullptr;
		element->prev     = nullptr;
		element->child    = nullptr;
		element->property = {};
		element->text     = {};
		element->type     = Type::Text;

		delete element;
		element = nullptr;

		return element_next;
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


	Element* element_prev(Element* element
		) noexcept
	{
		if(element == nullptr)
		{
			// error
		}

		return element->prev;
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
