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
 *
 * TODO: index_end = find_token_end_(string, index, nested_count&)\n"
 * TODO: index = string_rskip_whitespace_(string, index)\n"
 * TODO: element = convent_token_to_variable_(token, string)\n"
 *
 * Text
 * blah blah blah
 *
 * Variable
 * Use: {{foo$}}
 * Set: {{foo$ the value}}
 *
 * Command
 * No Param: {{foo}}
 * No Param: {{foo the content}}
 * W/ Param: {{foo(a = thing 1, b = thing 2)}}
 * W/ Param: {{foo(a = thing 1, b = thing 2) the content}}
 *
 * Array
 * Use: {{foo@1}}
 * Set: {{foo@ [ thing 1, thing 2 ] }}
 *
 * Map
 * Use: {{foo#abc}}
 * Set: {{foo# ( abc = thing 1, xyz = thing 2 ) }}
 *
 * -------------------------------------------------------------------------
 * Implement ztext::eval(ZText*, Element*, bool = true)
 * - Remove ZText* from Element objects
 * add element_eval_variable()
 * Test - Subcase - Variable With Data
 */


/******************************************************************************
 * Includes
 */

// C++
#include <functional>
#include <source_location>
#include <string>
#include <stack>
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
	X(Error_None                               ,  0 , "No Error"               ) \
	X(Error_Invalid_Parameter                  ,  1 , "A parameter is invalid" ) \
	X(Error_Element_In_Use                     ,  2 , "The requested Element is in use by another ZText object" ) \
	X(Error_Element_Type_Not_Text              ,  3 , "The expected Element type is text" ) \
	X(Error_Element_Type_Not_Variable          ,  4 , "The expected Element type is a variable" ) \
	X(Error_Parser_Token_Invalid               ,  5 , "The Parser found an invalid token" ) \
	X(Error_Parser_Token_Name_Invalid          ,  6 , "The Parser found an invalid token name" ) \
	X(Error_Parser_No_Text_Found               ,  7 , "The Parser was not able to find any text" ) \
	X(Error_Parser_Token_End_Marker_Missing    ,  8 , "The Parser was not able to find the token end marker '}}'" ) \
	X(Error_Parser_Token_Name_Missing          ,  9 , "The Parser was not able to find the token name"        ) \
	X(Error_Parser_Token_Identifier_Invalid    , 10 , "The Parser found an invalid token identifier" ) \
	X(Error_Parser_Variable_Content_Invalid    , 11 , "The Parser found an invalid content used with a variable" ) \
	X(Error_Parser_Token_Begin_Marker_Missing  , 12 , "The Parser encountered a token end marker '}}' without a preceding begin marker '{{'" )\


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


	// --- ZText --- //
	[[nodiscard]] ZText*          create() noexcept;
	[[]]          void            destroy(ZText*&) noexcept;
	[[]]          void            clear(ZText*) noexcept;
	[[]]          void            clear_commands(ZText*) noexcept;
	[[]]          void            clear_variables(ZText*) noexcept;

	// --- Evaluation --- //
	[[nodiscard]] std::string     eval(ZText*, Element*, bool = true) noexcept;

	// --- Parse --- //
	[[]]          std::error_code parse(const std::string&, Element*&) noexcept;

	// --- Element --- //
	[[]]          std::error_code element_append(Element*, Element*) noexcept;
	[[]]          std::error_code element_insert(Element*, Element*) noexcept;
	[[]]          Element*        element_destroy(Element*&) noexcept;
	[[]]          void            element_remove(Element*) noexcept;
	[[nodiscard]] Element*        element_next(Element*) noexcept;
	[[nodiscard]] Element*        element_prev(Element*) noexcept;
	[[nodiscard]] Element*        element_find_head(Element*) noexcept;
	[[nodiscard]] Element*        element_find_tail(Element*) noexcept;

	//[[nodiscard]] std::error_code command_child() const noexcept;
	//[[]]          std::error_code command_child_set(Elment*) noexcept;

	[[nodiscard]] Element*        element_text_create(const std::string&) noexcept;
	[[]]          std::error_code element_text_set(Element*, const std::string&) noexcept;

	[[nodiscard]] Element*        element_variable_create(const std::string&) noexcept;
	[[]]          std::error_code element_variable_set(Element*, const std::string&) noexcept;
	[[]]          std::error_code element_variable_set(Element*, const std::string&, size_t, size_t) noexcept;



	// --- Debugging --- //
	#ifdef ZTEXT_DEBUG_ENABLED
	[[]]          void            print(const Element*, const bool = false) noexcept;
	#endif

	// --- Deprecated --- //
	//[[]]          Element*        element_append_command(Element*, const std::string) noexcept;
	//[[]]          Element*        element_append_text(Element*, const std::string) noexcept;
	//[[]]          Element*        element_append_variable(Element*, const std::string) noexcept;

	//[[]]          void            command_set(ZText*, const std::string, const ztext::CommandLambda) noexcept;
	//[[]]          void            command_destroy(ZText*, const std::string) noexcept;
	//[[nodiscard]] Element*        command_content(Element*) noexcept;
	//[[]]          void            command_content_set(Element*, Element*) noexcept;
	//[[nodiscard]] MapStringString command_property(Element*) noexcept;
	//[[]]          void            command_property_set(Element*, const std::string, const std::string) noexcept;

	//[[]]          std::error_code variable_set(ZText*, const std::string, const std::string) noexcept;
	//[[]]          void            variable_destroy(ZText*, const std::string) noexcept;

	//#ifdef ZTEXT_DEBUG_ENABLED
	//[[]]          void            debug_element(Element*) noexcept;
	//[[]]          void            debug_command(ZText*) noexcept;
	//[[]]          void            debug_variable(ZText*) noexcept;
	//#endif
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
	{	Text
	,	Variable
	,	Command
	};


	struct Element
	{
		Element*        next     = nullptr;
		Element*        prev     = nullptr;
		Element*        child    = nullptr;
		Element*        parent   = nullptr;
		MapStringString property = {};
		std::string     text     = {};
		Type            type     = Type::Text;
	};

	using MapStringCommand = std::unordered_map<std::string, CommandLambda>;

	struct ZText
	{
		MapStringString  variable = {};
		MapStringCommand command  = {};
	};
}

// }}}
// {{{ Private: Constants

namespace
{
	constexpr char Token_Begin  = '{';
	constexpr char Token_End    = '}';
	constexpr char Token_Escape = '\\';

	constexpr char Identifier_Command  = '(';
	constexpr char Identifier_Variable = '$';
	constexpr char Identifier_Array    = '@';
	constexpr char Identifier_Map      = '#';

	constexpr char Dataset_Array_Begin = '[';
	constexpr char Dataset_Array_End   = ']';
	constexpr char Dataset_Map_Begin   = '(';
	constexpr char Dataset_Map_End     = ')';
}

// }}}
// {{{ Private: Datatypes

namespace
{
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
}

// }}}
// {{{ Private: Debugging

namespace
{
	[[maybe_unused]]
	void debug(const Token& token
		, const std::string_view& string
		) noexcept
	{
		size_t len = string.size();

		printf("--- Token ---\n");
		printf("token     : %lu,%lu '%s'\n"
			, token.begin
			, token.end
			, (token.begin >= token.end || token.begin >= len || token.end >= len) ? "" : std::string(string.substr(token.begin, (token.end - token.begin + 1))).c_str()
			);
		printf("name      : %lu,%lu '%s'\n"
			, token.name_begin
			, token.name_end
			, (token.name_begin >= token.name_end || token.name_begin >= len || token.name_end >= len) ? "" : std::string(string.substr(token.name_begin, (token.name_end - token.name_begin + 1))).c_str()
			);
		printf("identifier: %lu '%c'\n"
			, token.identifier
			, (token.identifier == 0 || token.identifier >= len || token.identifier >= len) ? '\0' : string[token.identifier]
			);
		printf("assignment: %lu '%c'\n"
			, token.assignment
			, (token.assignment == 0 || token.assignment >= len || token.assignment >= len) ? '\0' : string[token.assignment]
			);
		printf("content   : %lu,%lu '%s'\n"
			, token.content_begin
			, token.content_end
			, (token.content_begin >= token.content_end || token.content_begin >= len || token.content_end >= len) ? "" : std::string(string.substr(token.content_begin, (token.content_end - token.content_begin + 1))).c_str()
			);
	}
}

// }}}
// {{{ Private: Evaluation: Variable

namespace
{
	std::string element_eval_variable_(ztext::ZText* ztext
		, const ztext::Element* element
		) noexcept
	{
printf("\n%s\n", __FUNCTION__);
print(element, true);

		if(ztext->variable.contains(element->text) == false)
		{
			ztext->variable[element->text] = "";
		}

		if(element->child == nullptr)
		{
			std::string retval = ztext->variable[element->text];
			return retval;
		}

		std::string retval = eval(ztext, element->child);
		ztext->variable[element->text] = retval;

		return retval;
	}
}

// }}}
// {{{ Private: Element Utilities

namespace
{
	inline void element_init_(ztext::Element* element
		) noexcept
	{
		element->next     = nullptr;
		element->prev     = nullptr;
		element->child    = nullptr;
		element->parent   = nullptr;
		element->property = {};
		element->text     = {};
		element->type     = ztext::Type::Text;
	}
}

// }}}
// {{{ Private: String Utils

namespace
{
	std::string string_clean_escapes_(std::string string
		) noexcept
	{
		size_t index = 0;

		while(index < string.size())
		{
			if(string[index] != Token_Escape)
			{
				index++;

				continue;
			}

			if((index + 2) < string.size())
			{
				if(string[index + 1] == Token_Begin
					&& string[index + 2] == Token_Begin
					)
				{
					string.erase(index, 1);
				}
				else if(string[index + 1] == Token_End
					&& string[index + 2] == Token_End
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


	std::string string_substr_(const std::string& string
		, size_t begin
		, size_t end
		) noexcept
	{
		return string.substr(begin, (end - begin + 1));
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


	std::string to_string_(const ztext::Type type
		) noexcept
	{
		switch(type)
		{
			case ztext::Type::Variable: return "variable";
			case ztext::Type::Text:     return "text";
			case ztext::Type::Command:  return "command";
		}

		return {};
	}
}

// }}}
// {{{ Private: Parse

namespace
{
	std::error_code parse_(const std::string&, size_t&, size_t&, ztext::Element*&) noexcept;
	std::error_code parse_text_(const std::string&, size_t&, size_t&, ztext::Element*&) noexcept;
	std::error_code parse_token_(const std::string&, size_t&, size_t&, ztext::Element*&) noexcept;
	std::error_code parse_token_identifier_(Token&, const std::string&) noexcept;
	std::error_code parse_token_name_(Token&, const std::string&) noexcept;
	std::error_code parse_token_variable_(Token&, const std::string&) noexcept;


	// {{{ Private: Parse: Text

	std::error_code parse_text_(const std::string& string
		, size_t&          begin
		, size_t&          end
		, ztext::Element*& element
		) noexcept
	{
		size_t index = begin;

		while(index <= end)
		{
			if(string[index] == Token_Begin)
			{
				if((index + 1) <= end
					&& string[index - 1] != Token_Escape
					&& string[index + 1] == Token_Begin
					)
				{
					index--;
					break;
				}
			}

			if(string[index] == Token_End)
			{
				if((index + 1) <= end
					&& string[index - 1] != Token_Escape
					&& string[index + 1] == Token_End
					)
				{
					begin = index;
					return ztext::Error_Parser_Token_Begin_Marker_Missing;
				}
			}

			index++;
		}

		std::string text = string_substr_(string, begin, index);
		text = string_trim_(text);
		text = string_clean_whitespace_(text);

		begin = index + 1;

		if(text.empty() == true)
		{
			return ztext::Error_Parser_No_Text_Found;
		}

		element = ztext::element_text_create(text);

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token

	inline bool is_valid_token_name_character_(const unsigned char c
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


	std::error_code parse_token_name_(Token& token
		, const std::string& string
		) noexcept
	{
printf("\n%s\n", __FUNCTION__);
debug(token, string);
		size_t index = string_skip_whitespace_(string, token.begin + 2);
		token.name_begin = index;

		if(string[index] == Identifier_Variable
			|| string[index] == Identifier_Command
			|| string[index] == Token_End
			)
		{
			return ztext::Error_Parser_Token_Name_Missing;
		}

		// BUG: Loop until one of the following is found
		//	- an invalid character
		//	- a token identifier
		//	- a token begin/end
		//	- white space
		while(is_valid_token_name_character_(string[index]) == true)
		{
			index++;
		}

		if(token.name_begin == index)
		{
			return ztext::Error_Parser_Token_Name_Invalid;
		}

		token.name_end = index - 1;

debug(token, string);

		return ztext::Error_None;
	}


	std::error_code parse_token_identifier_(Token& token
		, const std::string& string
		) noexcept
	{
printf("\n%s\n", __FUNCTION__);
debug(token, string);

		size_t index = string_skip_whitespace_(string, token.name_end + 1);
		token.identifier = index;

		if(string[index] != Identifier_Variable
			)
		{
			return ztext::Error_Parser_Token_Identifier_Invalid;
		}

debug(token, string);
		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token: Variable

	std::error_code parse_token_variable_(Token& token
		, const std::string& string
		) noexcept
	{
printf("\n%s\n", __FUNCTION__);
debug(token, string);

		size_t index = string_skip_whitespace_(string, token.identifier + 1);
printf("%lu %c\n", index, string[index]);

		if(string[index] == Token_End)
		{
			return ztext::Error_None;
		}

		token.content_begin = index;
		token.content_end   = token.end - 2;

debug(token, string);

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token

	std::error_code parse_token_(const std::string& string
		, size_t&            string_begin
		, size_t&            string_end
		, ztext::Element*&   element
		) noexcept
	{
printf("%s\n", __FUNCTION__);

		std::error_code error       = {};
		size_t          index_begin = string_begin + 2;
		size_t          index_end   = index_begin;
		size_t          depth       = 0;

		while(index_end <= string_end)
		{
			if(string[index_end] == Token_Begin)
			{
				if((index_end + 1) <= string_end
					&& string[index_end - 1] != Token_Escape
					&& string[index_end + 1] == Token_Begin
					)
				{
					depth++;
				}
			}

			if(string[index_end] == Token_End)
			{
				if((index_end + 1) <= string_end
					&& string[index_end - 1] != Token_Escape
					&& string[index_end + 1] == Token_End
					&& depth == 0
					)
				{
					index_end++;
					break;
				}
			}

			index_end++;
		}

		if(index_end > string_end)
		{
			string_begin = index_begin;
			return ztext::Error_Parser_Token_End_Marker_Missing;
		}

		Token token;
		token.begin = string_begin;
		token.end   = index_end;

		error = parse_token_name_(token, string);

		if(error != ztext::Error_None)
		{
			string_begin = token.name_begin;
			return error;
		}

		error = parse_token_identifier_(token, string);

		if(error != ztext::Error_None)
		{
			string_begin = token.identifier;
			return error;
		}

		if(string[token.identifier] == Identifier_Variable)
		{
			parse_token_variable_(token, string);
			element = ztext::element_variable_create(
				string_substr_(string, token.name_begin, token.name_end)
				);

			if(token.content_begin != 0)
			{
				error = ztext::element_variable_set(element
					, string_substr_(string
						, token.content_begin
						, token.content_end
						)
					);
			}
		}



string_begin = string_end + 1;
#if 0
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
#endif

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse

	std::error_code parse_(const std::string& string
		, size_t&          index_begin
		, size_t&          index_end
		, ztext::Element*& element_head
		) noexcept
	{
printf("%s\n", __FUNCTION__);
		element_head = nullptr;
		ztext::Element* element_tail = nullptr;

		while(index_begin <= index_end)
		{
			ztext::Element* element = nullptr;
			std::error_code error   = ztext::Error_None;

			if((index_begin + 1) <= index_end
				&& string[index_begin + 0] == Token_Begin
				&& string[index_begin + 1] == Token_Begin
				)
			{
				error = parse_token_(string, index_begin, index_end, element);
			}
			else
			{
				error = parse_text_(string, index_begin, index_end, element);

				if(error == ztext::Error_Parser_No_Text_Found)
				{
					continue;
				}
			}

			if(error != ztext::Error_None)
			{
				while(element_head != nullptr)
				{
					element_head = ztext::element_destroy(element_head);
				}

				return error;
			}

			if(element_head == nullptr)
			{
				element_head = element;
			}
			else
			{
				ztext::element_append(element_tail, element);
			}

			element_tail = element;
		}

		return ztext::Error_None;
	}

	// }}}

}

// }}}
// {{{ Private: Error

namespace
{

	void report_error(const std::error_code& error
		, const std::string& string
		, const size_t       index_begin
		) noexcept
	{
		size_t line_count = 1;
		size_t line_start = 0;
		size_t line_end   = 0;
		size_t index = 0;

		while(index < string.size() && index < index_begin)
		{
			if(string[index] == '\n')
			{
				line_count++;
				line_start = index + 1;
			}

			index++;
		}

		while(line_start < string.size() && std::isspace(string[line_start]))
		{
			line_start++;
		}

		line_end = line_start;
		while(line_end < string.size() && string[line_end] != '\n')
		{
			line_end++;
		}

		printf("Line: %ld, Char: %ld, Error: %s\n", line_count, index_begin, error.message().c_str());
		printf("%s\n", string.substr(line_start, line_end - line_start).c_str());
		printf("%*s\n", int(index_begin - line_start + 1), "^");
	}
}

// }}}
// {{{ ZText

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
	if(ztext == nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
		#endif
	}

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


void ztext::clear(ztext::ZText* ztext
	) noexcept
{
	if(ztext == nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
		#endif
	}

	ztext::clear_commands(ztext);
	ztext::clear_variables(ztext);
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
// TODO Add a Variable Element
// TODO Add a Command Element
TEST_CASE("clear")
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
}
#endif // }}}


void ztext::clear_commands(ztext::ZText* ztext
	) noexcept
{
	if(ztext == nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
		#endif
	}

	ztext->command = {};
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("clear/command")
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
}
#endif // }}}


void ztext::clear_variables(ztext::ZText* ztext
	) noexcept
{
	if(ztext == nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
		#endif
	}

	ztext->variable = {};
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("clear/variable")
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
}
#endif // }}}

// }}}
// {{{ Evaluation

std::string ztext::eval(ztext::ZText* ztext
	, ztext::Element* element
	, bool            to_end
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'ztext' can not be null"
			<< '\n';
	}

	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	std::string retval;

	while(true)
	{
		switch(element->type)
		{
			case ztext::Type::Text:
				retval += string_clean_escapes_(element->text);
				break;

			case ztext::Type::Variable:
				retval += element_eval_variable_(ztext, element);
				break;

			case ztext::Type::Command:
				ZTEXT_ERROR << "Not Implemented\n";
				//retval += element_eval_command_(ztext, element);
				break;
		}

		if(to_end == false
			|| element->next == nullptr
			)
		{
			break;
		}

		element = element->next;
	}

	return retval;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("eval/text")
{
	ztext::ZText* zt = ztext::create();

	SUBCASE("single")
	{
		ztext::Element* text = ztext::element_text_create("text");
		CHECK(ztext::eval(zt, text, false) == "text");
		ztext::element_destroy(text);
	}

	SUBCASE("many")
	{
		ztext::Element* hello = ztext::element_text_create("Hello");
		ztext::Element* comma = ztext::element_text_create(", ");
		ztext::Element* world = ztext::element_text_create("World");

		ztext::element_append(hello, comma);
		ztext::element_append(comma, world);
		CHECK(ztext::eval(zt, hello, true) == "Hello, World");

		ztext::element_destroy(hello);
		ztext::element_destroy(comma);
		ztext::element_destroy(world);
	}

	destroy(zt);
}
#endif // }}}


// }}}
// {{{ Parse

std::error_code ztext::parse(const std::string& string
	, ztext::Element*& element
	) noexcept
{
	std::error_code error = {};

	if(string.empty() == true)
	{
		element = element_text_create("");
	}
	else
	{
		size_t index_begin = 0;
		size_t index_end   = string.size() - 1;
		error = parse_(string, index_begin, index_end, element);

		if(error == ztext::Error_Parser_No_Text_Found)
		{
			element = ztext::element_text_create("");
			error = Error_None;
		}

		if(error != Error_None)
		{
			while(element != nullptr)
			{
				element = ztext::element_destroy(element);
			}

			/*
			if(error == ztext::Error_Parser_Token_End_Marker_Missing
				|| error == ztext::Error_Parser_Token_Name_Invalid
				|| error == ztext::Error_Parser_Token_Name_Missing
				|| error == ztext::Error_Parser_Token_Identifier_Invalid
				|| error == ztext::Error_Parser_Variable_Content_Invalid
				|| error == ztext::Error_Parser_Token_Begin_Marker_Missing
				)
			*/
			{
				report_error(error, string, index_begin);
			}

			return error;
		}

		if(element == nullptr)
		{
			element = ztext::element_text_create("");
		}
	}

	return Error_None;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{ parse/text
TEST_CASE("parse/text")
{
	ztext::ZText*   zt      = ztext::create();
	ztext::Element* element = nullptr;
	std::error_code error   = {};

	SUBCASE("Invaild Data")
	{
		error = ztext::parse("foo }} bar", element);
		CHECK(error == ztext::Error_Parser_Token_Begin_Marker_Missing);
	}

	SUBCASE("Pure White-Space")
	{
		const std::string empty    = "";
		const std::string newlines = "\n\n\n";
		const std::string spaces   = "   ";
		const std::string tabs     = "		";

		// -------------------------------------- //

		ztext::clear(zt);
		error = ztext::parse(empty, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == "");

		// -------------------------------------- //

		ztext::clear(zt);
		error = ztext::parse(newlines, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == "");

		// -------------------------------------- //

		ztext::clear(zt);
		error = ztext::parse(spaces, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == "");

		// -------------------------------------- //

		ztext::clear(zt);
		error = ztext::parse(tabs, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == "");
	}

	SUBCASE("Simple Text")
	{
		std::string text = "X";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == text);
	}

	SUBCASE("Leading White-Space")
	{
		std::string text = "X";

		error = ztext::parse(" 	 " + text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == text);
	}

	SUBCASE("Trailing White-Space")
	{
		std::string text = "X";

		error = ztext::parse(text + " 	 	", element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == text);
	}

	SUBCASE("Leading and Trailing White-Space")
	{
		std::string text = "X";

		error = ztext::parse("	" + text + "       ", element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == text);
	}

	SUBCASE("Clean White-Space")
	{
		std::string text = "X	Y  Z";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "X Y Z");
	}

	SUBCASE("Multi-Line")
	{
		std::string text = " \
			X            \
			Y            \
			Z            \
			";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "X Y Z");
	}

	SUBCASE("Before Token")
	{
		std::string text = "foo {{token$}}";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "foo");
	}

	SUBCASE("Escaped Token")
	{
		std::string text = "\\{{token\\}}";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "{{token}}");
	}

	SUBCASE("Embedded Escaped Token")
	{
		std::string text = "foo \\{{token\\}} bar";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "foo {{token}} bar");
	}

	destroy(zt);
}
#endif // }}}
#if 0//def ZTEXT_IMPLEMENTATION_TEST // {{{ parse/variable
TEST_CASE("parse/variable")
{
	ztext::ZText*   zt      = ztext::create();
	ztext::Element* element = nullptr;
	std::error_code error   = {};

	SUBCASE("Invaild Data")
	{
		error = ztext::parse("{{", element);
		CHECK(error == ztext::Error_Parser_Token_End_Marker_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{var$", element);
		CHECK(error == ztext::Error_Parser_Token_End_Marker_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{var$\\}}", element);
		CHECK(error == ztext::Error_Parser_Token_End_Marker_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{$}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{*$}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Invalid);
	}

	SUBCASE("Variable")
	{
		ztext::clear(zt);

		error = ztext::parse("{{var$}}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element       != nullptr);
		CHECK(element->prev == nullptr);
		CHECK(element->type == ztext::Type::Variable);
		CHECK(element->text == "var");

		CHECK(ztext::eval(zt, element) == "");

		ztext::element_destroy(element);
	}

	SUBCASE("Variable With White-Space")
	{
		ztext::clear(zt);

		error = ztext::parse(" {{ var $ }} ", element);
		CHECK(error == ztext::Error_None);

		CHECK(element       != nullptr);
		CHECK(element->type == ztext::Type::Variable);
		CHECK(element->text == "var");

		CHECK(ztext::eval(zt, element) == "");

		ztext::element_destroy(element);
	}

	SUBCASE("Variable With Data")
	{
		ztext::clear(zt);

		error = ztext::parse("{{var$foo}}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element       != nullptr);
		CHECK(element->type == ztext::Type::Variable);
		CHECK(element->text == "var");

		CHECK(ztext::eval(zt, element) == "foo");

		ztext::element_destroy(element);
	}

	SUBCASE("Variable With Data and White-Space")
	{
		error = ztext::parse(" {{ var $ foo }} ", element);
		CHECK(error == ztext::Error_None);

		CHECK(element       != nullptr);
		CHECK(element->type == ztext::Type::Variable);
		CHECK(element->text == "var");

		CHECK(ztext::eval(zt, element) == "foo");

		ztext::element_destroy(element);
	}

	SUBCASE("Variable With Cleaned Data")
	{
		error = ztext::parse(" {{ var$  \
			foo		\
			\\{{123\\}}	\
			bar		\
			}} ", element);
		CHECK(error == ztext::Error_None);

		CHECK(element       != nullptr);
		CHECK(element->type == ztext::Type::Variable);
		CHECK(element->text == "var");

		CHECK(ztext::eval(zt, element) == "foo {{123}} bar");

		ztext::element_destroy(element);
	}

	SUBCASE("Variable With Nested Variables")
	{
		ztext::Element* var = nullptr;
		error = ztext::parse("{{var$ abc}}", var);
		CHECK(error == ztext::Error_None);

		CHECK(var       != nullptr);
		CHECK(var->type == ztext::Type::Variable);
		CHECK(var->text == "var");

printf("--------------------------------------------------------------------------------\n");
		error = ztext::parse("{{foo$ {{var$}} }}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element       != nullptr);
		CHECK(element->type == ztext::Type::Variable);
		CHECK(element->text == "foo");

		ztext::element_append(var, element);

		CHECK(ztext::eval(zt, var) == "abc abc");

		ztext::element_destroy(var);
		ztext::element_destroy(element);
	}

	SUBCASE("Variable With Nested Variables Recursive")
	{
	}

	destroy(zt);
}
#endif // }}}

// }}}
// {{{ Element

std::error_code ztext::element_append(ztext::Element* position
	, ztext::Element* element
	) noexcept
{
	if(position == nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Invalid Parameter: 'position' can not be NULL."
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

	if(element->prev != nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Element In-Use: 'element' is already linked."
			<< '\n';
		#endif
		return Error_Element_In_Use;
	}

	Element* tail = element;
	while(true)
	{
		tail->parent = position->parent;

		if(tail->next == nullptr)
		{
			break;
		}

		tail = tail->next;
	}

	element->prev  = position;
	tail->next     = position->next;
	position->next = element;

	if(tail->next != nullptr)
	{
		tail->next->prev = tail;
	}

	return Error_None;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/append")
{
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");

	ztext::element_append(foo, bar);
	CHECK(ztext::element_next(foo) == bar);
	CHECK(ztext::element_next(bar) == nullptr);
	CHECK(ztext::element_prev(bar) == foo);
	CHECK(ztext::element_prev(foo) == nullptr);

	ztext::Element* abc = ztext::element_text_create("abc");
	ztext::Element* xyz = ztext::element_text_create("xyz");

	ztext::element_append(abc, xyz);
	ztext::element_append(foo, abc);

	CHECK(ztext::element_next(foo) == abc);
	CHECK(ztext::element_next(abc) == xyz);
	CHECK(ztext::element_next(xyz) == bar);
	CHECK(ztext::element_next(bar) == nullptr);
	CHECK(ztext::element_prev(bar) == xyz);
	CHECK(ztext::element_prev(xyz) == abc);
	CHECK(ztext::element_prev(abc) == foo);
	CHECK(ztext::element_prev(foo) == nullptr);
}
#endif // }}}


std::error_code ztext::element_insert(ztext::Element* position
	, ztext::Element* element
	) noexcept
{
	if(position == nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Invalid Parameter: 'position' can not be NULL."
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

	if(element->prev != nullptr)
	{
		#if ZTEXT_DEBUG_ENABLED
		ZTEXT_ERROR
			<< "Element In-Use: 'element' is already linked."
			<< '\n';
		#endif
		return Error_Element_In_Use;
	}

	Element* tail = element;
	while(true)
	{
		tail->parent = position->parent;

		if(tail->next == nullptr)
		{
			break;
		}

		tail = tail->next;
	}

	element->prev  = position->prev;
	tail->next     = position;
	position->prev = tail;

	if(element->prev != nullptr)
	{
		element->prev->next = element;
	}

	return Error_None;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/insert")
{
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");

	ztext::element_insert(bar, foo);
	CHECK(ztext::element_next(foo) == bar);
	CHECK(ztext::element_next(bar) == nullptr);
	CHECK(ztext::element_prev(bar) == foo);
	CHECK(ztext::element_prev(foo) == nullptr);

	ztext::Element* abc = ztext::element_text_create("abc");
	ztext::Element* xyz = ztext::element_text_create("xyz");

	ztext::element_insert(xyz, abc);
	ztext::element_insert(bar, abc);

	CHECK(ztext::element_next(foo) == abc);
	CHECK(ztext::element_next(abc) == xyz);
	CHECK(ztext::element_next(xyz) == bar);
	CHECK(ztext::element_next(bar) == nullptr);
	CHECK(ztext::element_prev(bar) == xyz);
	CHECK(ztext::element_prev(xyz) == abc);
	CHECK(ztext::element_prev(abc) == foo);
	CHECK(ztext::element_prev(foo) == nullptr);
}
#endif // }}}


ztext::Element* ztext::element_destroy(ztext::Element*& element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	std::stack<ztext::Element*> stack = {};

	element_remove(element);

	if(element->child != nullptr)
	{
		stack.push(element->child);
	}

	ztext::Element* retval = element->next;

	element_init_(element);
	delete element;
	element = nullptr;

	while(stack.empty() == false)
	{
		element = stack.top();
		stack.pop();

		while(element != nullptr)
		{
			if(element->child != nullptr)
			{
				stack.push(element->child);
			}

			ztext::Element* temp = element->next;

			element_init_(element);
			delete element;

			element = temp;
		}
	}

	return retval;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/destroy")
{
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");
	ztext::Element* xyz = ztext::element_text_create("xyz");

	ztext::element_append(foo, xyz);
	ztext::element_append(xyz, bar);

	// foo -- xyz -- bar

	ztext::element_destroy(xyz);
	CHECK(xyz == nullptr);
	CHECK(ztext::element_next(foo) == bar);
	CHECK(ztext::element_prev(bar) == foo);

	ztext::element_destroy(bar);
	CHECK(bar == nullptr);
	CHECK(ztext::element_next(foo) == nullptr);

	ztext::element_destroy(foo);
	CHECK(foo == nullptr);
}
#endif // }}}


void ztext::element_remove(ztext::Element* element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	if(element->parent != nullptr)
	{
		if(element->parent->child == element)
		{
			element->parent->child = element->next;
		}
	}

	if(element->next != nullptr)
	{
		element->next->prev = element->prev;
	}

	if(element->prev != nullptr)
	{
		element->prev->next = element->next;
	}

	element->next   = nullptr;
	element->prev   = nullptr;
	element->parent = nullptr;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/remove")
{
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");
	ztext::Element* xyz = ztext::element_text_create("xyz");

	ztext::element_append(foo, xyz);
	ztext::element_append(xyz, bar);

	ztext::element_remove(xyz);
	CHECK(ztext::element_next(foo)  == bar);
	CHECK(ztext::element_prev(bar)  == foo);
	CHECK(ztext::element_next(xyz) == nullptr);
	CHECK(ztext::element_prev(xyz) == nullptr);

	CHECK(xyz->next == nullptr);
	CHECK(xyz->prev == nullptr);
	CHECK(xyz->text == "xyz");

	ztext::element_destroy(foo);
	ztext::element_destroy(bar);
	ztext::element_destroy(xyz);
}
#endif // }}}


ztext::Element* ztext::element_next(ztext::Element* element
		) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	return element->next;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/next")
{
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");

	ztext::element_append(foo, bar);

	ztext::Element* element = foo;

	element = ztext::element_next(element);
	CHECK(element == bar);

	element = ztext::element_next(element);
	CHECK(element == nullptr);

	ztext::element_destroy(foo);
	ztext::element_destroy(bar);
}
#endif // }}}


ztext::Element* ztext::element_prev(ztext::Element* element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	return element->prev;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/prev")
{
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");

	ztext::element_append(foo, bar);

	ztext::Element* element = bar;

	element = ztext::element_prev(element);
	CHECK(element == foo);

	element = ztext::element_prev(element);
	CHECK(element == nullptr);

	ztext::element_destroy(foo);
	ztext::element_destroy(bar);
}
#endif // }}}


ztext::Element* ztext::element_find_head(ztext::Element* element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	while(element->prev != nullptr)
	{
		element = element->prev;
	}

	return element;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/find/head")
{
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");
	ztext::Element* xyz = ztext::element_text_create("xyz");

	ztext::element_append(foo, xyz);
	ztext::element_append(xyz, bar);

	ztext::Element* element = nullptr;
	element = ztext::element_find_head(xyz);;
	CHECK(element == foo);

	element = ztext::element_find_head(bar);;
	CHECK(element == foo);

	ztext::element_destroy(foo);
	ztext::element_destroy(bar);
	ztext::element_destroy(xyz);
}
#endif // }}}

ztext::Element* ztext::element_find_tail(ztext::Element* element
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	while(element->next != nullptr)
	{
		element = element->next;
	}

	return element;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/find/tail")
{
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");
	ztext::Element* xyz = ztext::element_text_create("xyz");

	ztext::element_append(foo, xyz);
	ztext::element_append(xyz, bar);

	ztext::Element* element = nullptr;
	element = ztext::element_find_tail(xyz);;
	CHECK(element == bar);

	element = ztext::element_find_tail(foo);;
	CHECK(element == bar);

	ztext::element_destroy(foo);
	ztext::element_destroy(bar);
	ztext::element_destroy(xyz);
}
#endif // }}}

// }}}
// {{{ Element: Text

ztext::Element* ztext::element_text_create(const std::string& string
	) noexcept
{
	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Text;
	element->text = string;

	return element;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/text/create")
{
	ztext::Element* text = ztext::element_text_create("text");

	CHECK(text->text == "text");

	ztext::element_destroy(text);
}
#endif // }}}


std::error_code ztext::element_text_set(Element* element
	, const std::string& text
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}
	#endif

	if(element->type != ztext::Type::Text)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' must be of type 'text'"
			<< '\n';

		return Error_Element_Type_Not_Text;
	}

	element->text = text;

	return Error_None;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/text/set")
{
	ztext::Element* element = nullptr;
	std::error_code error = {};

	error = ztext::element_text_set(element, "aaa");
	CHECK(error == ztext::Error_Invalid_Parameter);

	element = ztext::element_variable_create("var");
	error = ztext::element_text_set(element, "bbb");
	CHECK(error == ztext::Error_Element_Type_Not_Text);
	ztext::element_destroy(element);

	element = ztext::element_text_create("ccc");
	ztext::element_text_set(element, "ddd");
	CHECK(element->text == "ddd");
	ztext::element_destroy(element);
}
#endif // }}}

// }}}
// {{{ Element: Variable

ztext::Element* ztext::element_variable_create(const std::string& name
	) noexcept
{
	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Variable;
	element->text = name;

	return element;
}


std::error_code ztext::element_variable_set(Element* element
	, const std::string& string
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}
	#endif

	#if ZTEXT_DEBUG_ENABLED
	if(element->type != ztext::Type::Variable)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' must be of type 'variable'"
			<< '\n';

		return Error_Element_Type_Not_Variable;
	}
	#endif

	std::error_code error = {};

	error = element_variable_set(element, string, 0, string.size() - 1);

	return error;
}


std::error_code ztext::element_variable_set(Element* element
	, const std::string& string
	, size_t             begin
	, size_t             end
	) noexcept
{
printf("%s\n", __FUNCTION__);
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}
	#endif

	#if ZTEXT_DEBUG_ENABLED
	if(element->type != ztext::Type::Variable)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' must be of type 'variable'"
			<< '\n';

		return Error_Element_Type_Not_Variable;
	}
	#endif

	std::error_code error = {};
	Element*        child = nullptr;

	error = parse_(string, begin, end, child);

	if(error != ztext::Error_None)
	{
		while(child != nullptr)
		{
			child = ztext::element_destroy(child);
		}

		return error;
	}

	while(element->child != nullptr)
	{
		element->child = ztext::element_destroy(element->child);
	}

	element->child = child;

	return ztext::Error_None;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/variable/set")
{
	ztext::ZText*   zt   = ztext::create();

	destroy(zt);
}
#endif // }}}

// }}}
// {{{ Debugging

void print_(const ztext::Element* element
	, const bool children
	, int        level
	) noexcept
{
	if(element == nullptr)
	{
		printf("%*selement: 0x%08lx <- 0x%08lx -> 0x%08lx ^ 0x%08lx %s (%s)\n"
			, (level * 3)
			, ""
			, (uint64_t)0
			, (uint64_t)0
			, (uint64_t)0
			, (uint64_t)0
			, ""
			, ""
			);
		return;
	}

	printf("%*selement: 0x%08lx <- 0x%08lx -> 0x%08lx ^ 0x%08lx %s (%s)\n"
		, (level * 3)
		, ""
		, (uint64_t)element->prev
		, (uint64_t)element
		, (uint64_t)element->next
		, (uint64_t)element->parent
		, to_string_(element->type).c_str()
		, element->text.c_str()
		);

	if(children == true)
	{
		element = element->child;

		while(element != nullptr)
		{
			print_(element, children, level + 1);

			element = element->next;
		}
	}
}

void ztext::print(const ztext::Element* element
	, const bool children
	) noexcept
{
	print_(element, children, 0);
}

// }}}


// {{{ // --- Deprecated --- //
#if 0
namespace ztext
{
	// {{{ Parse

#if 0
#endif


#if 0
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
#endif

	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{ parse/text
#if 0
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

		destroy(zt);
	}
#endif
	#endif // }}}
	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{ parse/variable
#if 0
	TEST_CASE("parse/variable")
	{
		ztext::ZText*   zt    = create();
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

		SUBCASE("Variable With Nested Variables")
		{
		}

		SUBCASE("Variable With Nested Variables Recursive")
		{
		}

		destroy(zt);
	}
#endif
	#endif // }}}

	// }}}
	// {{{ Element

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


	/*
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
	*/


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



	/*
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
	*/


	/*
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
	*/


	/*
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
	*/


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

#endif
// }}}

#endif // }}}
