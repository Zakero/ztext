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
 * TODO: variable_set(ZText* ztext, string name, Element* node, bool read_only)
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
 *
 * Rename ZTEXT_DEBUG_ENABLED to ZTEXT_ERROR_CHECKS_ENABLED
 * Add ZTEXT_ERROR_MESSAGES_ENABLED
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
	X(Error_Parser_Recursive_Dependencies      , 13 , "The Parser has detected a recursive dependeny of data" )\


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

	using CommandLambda   = std::function<std::string(ZText*, Element*)>;
	using MapStringString = std::unordered_map<std::string, std::string>;
	using VectorString    = std::vector<std::string>;


	// --- ZText --- //
	[[nodiscard]] ZText*          create() noexcept;
	[[]]          void            destroy(ZText*&) noexcept;
	[[]]          void            cache_clear(ZText*) noexcept;
	[[]]          void            cache_variable_clear_all(ZText*) noexcept;
	[[]]          VectorString    cache_variable_list(ZText*) noexcept;
	//[[]]          std::string     cache_variable_eval(ZText* ztext, const std::string& name) noexcept;
	//[[]]          void            cache_variable_set(ZText* ztext, const std::string& name, Element* element_chain, bool read_only = false) noexcept;

	// --- Evaluation --- //
	[[nodiscard]] std::string     eval(ZText*, Element*, bool = true) noexcept;

	// --- Parse --- //
	[[]]          std::error_code parse(const std::string&, Element*&) noexcept;

	// --- Element --- //
	[[]]          std::error_code element_append(Element*, Element*) noexcept;
	[[]]          std::error_code element_insert(Element*, Element*) noexcept;
	[[]]          Element*        element_destroy(Element*&) noexcept;
	[[]]          void            element_destroy_all(Element*&) noexcept;
	[[]]          void            element_remove(Element*) noexcept;
	[[nodiscard]] Element*        element_next(Element*) noexcept;
	[[nodiscard]] Element*        element_prev(Element*) noexcept;
	[[nodiscard]] Element*        element_find_head(Element*) noexcept;
	[[nodiscard]] Element*        element_find_tail(Element*) noexcept;

	//[[]]          void            command_create(ZText*, const std::string&, const ztext::CommandLambda) noexcept;
	//[[]]          void            command_destroy(ZText*, const std::string&) noexcept;
	//[[]]          void            element_command_create(const std::string&) noexcept;
	//[[nodiscard]] Element*        element_command_content(Element*) noexcept;
	//[[]]          std::error_code element_command_content_set(Element*, Element*) noexcept;
	//[[nodiscard]] MapStringString element_command_property(Element*) noexcept;
	//[[]]          void            element_command_property_set(Element*, std::string, const std::string) noexcept;

	[[nodiscard]] Element*        element_text_create(const std::string&) noexcept;
	[[]]          std::error_code element_text_set(Element*, const std::string&) noexcept;

	[[nodiscard]] Element*        element_variable_create(const std::string&) noexcept;
	[[]]          std::error_code element_variable_set(Element*, const std::string&) noexcept;
	[[]]          std::error_code element_variable_set(Element*, const std::string&, size_t, size_t) noexcept;
	[[]]          std::error_code element_variable_set(Element*, Element*) noexcept;

	// --- Debugging --- //
	#ifdef ZTEXT_DEBUG_ENABLED
	[[]]          void            print(const Element*, const bool = false) noexcept;
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
	using MapStringElement = std::unordered_map<std::string, Element*>;

	struct ZText
	{
		MapStringElement variable = {};
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
// {{{ Private: Element Utilities

namespace
{
	void            element_init_(ztext::Element*) noexcept;
	ztext::Element* element_copy_(ztext::Element*) noexcept;
	ztext::Element* element_copy_all_(ztext::Element*) noexcept;

	ztext::Element* element_copy_(ztext::Element* element
		) noexcept
	{
		ztext::Element* retval = new ztext::Element;
		element_init_(retval);
		
		retval->property = element->property;
		retval->text     = element->text;
		retval->type     = element->type;

		if(element->child != nullptr)
		{
			retval->child = element_copy_all_(element->child);
		}

		return retval;
	}


	ztext::Element* element_copy_all_(ztext::Element* element
		) noexcept
	{
		ztext::Element* retval = nullptr;
		ztext::Element* tail   = nullptr;

		while(element != nullptr)
		{
			ztext::Element* tmp = element_copy_(element);

			if(retval == nullptr)
			{
				[[unlikely]];
				retval = tmp;
				tail   = tmp;
			}
			else
			{
				element_append(tail, tmp);
				tail = tmp;
			}

			element = element->next;
		}
printf("%s\n", __FUNCTION__);
ztext::print(retval, true);

		return retval;
	}


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
// {{{ Private: Evaluation: Variable

namespace
{
	std::string element_eval_variable_(ztext::ZText* ztext
		, const ztext::Element* element
		) noexcept
	{
printf("\n%s\n", __FUNCTION__);
print(element, true);

		if(element->child != nullptr)
		{
			std::string retval = eval(ztext, element->child);

			if(ztext->variable.contains(element->text) == true)
			{
				element_destroy_all(ztext->variable[element->text]);
			}

			ztext::Element* content = element_copy_all_(element->child);
			ztext->variable[element->text] = content;

			return retval;
		}

		if(ztext->variable.contains(element->text) == true)
		{
			std::string retval = eval(ztext, ztext->variable[element->text]);

			return retval;
		}

		return "";
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


	// Remove
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


	size_t string_skip_whitespace_leading_(const std::string& string
		, size_t index
		) noexcept
	{
		while(index < string.size()
			&& std::isspace(static_cast<unsigned char>(string[index])) != 0
			)
		{
			index++;
		}

		return index;
	}


	size_t string_skip_whitespace_trailing_(const std::string& string
		, size_t index
		) noexcept
	{
		while(index > 0
			&& std::isspace(static_cast<unsigned char>(string[index])) != 0
			)
		{
			index--;
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


	////std::string string_trim_(std::string string
	//void string_trim_(const std::string& string
	//	, size_t& start
	//	, size_t& end
	//	) noexcept
	//{
	//	size_t len   = string.size();
	//	start = 0;
	//	end   = len - 1;

	//	while(start < len
	//		&& std::isspace(static_cast<unsigned char>(string[start])) != 0
	//		)
	//	{
	//		start++;
	//	}

	//	while(end > 0
	//		&& std::isspace(static_cast<unsigned char>(string[end])) != 0
	//		)
	//	{
	//		end--;
	//	}

	//	//return string.substr(start, end - start + 1);
	//}


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
printf("Element Chain\n");
ztext::Element* e = element_head;
while(e != nullptr)
{
	print(e, true);
	e = e->next;
}
		}

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Text

	std::error_code parse_text_(const std::string& string
		, size_t&          begin
		, size_t&          end
		, ztext::Element*& element
		) noexcept
	{
printf("%s\n", __FUNCTION__);
printf("%s\n", string.c_str());
printf("%lu %lu '%s'\n", begin, end, string.substr(begin, (end - begin + 1)).c_str());
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

		index--;
		std::string text = string_substr_(string, begin, index);
		text = string_clean_whitespace_(text);

		begin = index + 1;

		if(text.empty() == true)
		{
			return ztext::Error_Parser_No_Text_Found;
		}

		element = ztext::element_text_create(text);
print(element, true);

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
					)
				{
					if(depth == 0)
					{
						index_end++;
						break;
					}

					depth--;
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
print(element, true);

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

		string_begin = index_end + 1;

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token Name

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

	// }}}
	// {{{ Private: Parse: Token Identifier

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

		//size_t index = string_skip_whitespace_(string, token.identifier + 1);
		size_t index = string_skip_whitespace_leading_(string, token.identifier + 1);
printf("%lu %c\n", index, string[index]);

		if(string[index] == Token_End)
		{
			return ztext::Error_None;
		}

		token.content_begin = index;
		//token.content_end   = token.end - 2;
		token.content_end   = string_skip_whitespace_trailing_(string, token.end - 2);

debug(token, string);

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

	ztext::cache_clear(ztext);

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


void ztext::cache_clear(ztext::ZText* ztext
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

	ztext::cache_variable_clear_all(ztext);
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("cache/clear")
{
	ztext::ZText* zt = ztext::create();

	ztext::Element* var = ztext::element_variable_create("name");
	ztext::element_variable_set(var, "The Foo");

	std::string name = ztext::eval(zt, var);

	ztext::VectorString list = ztext::cache_variable_list(zt);
	CHECK(list.size() == 1);

	ztext::element_destroy_all(var);
	ztext::cache_clear(zt);
	list = ztext::cache_variable_list(zt);
	CHECK(list.empty() == true);

	destroy(zt);
}
#endif // }}}


void ztext::cache_variable_clear_all(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	for(auto& [name, element] : ztext->variable)
	{
		ztext::element_destroy_all(element);
	}

	ztext->variable.clear();
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("cache/variable/clear/all")
{
	ztext::ZText* zt = ztext::create();

	ztext::Element* var = ztext::element_variable_create("name");
	ztext::element_variable_set(var, "The Foo");

	std::string name = ztext::eval(zt, var);

	ztext::VectorString list = ztext::cache_variable_list(zt);
	CHECK(list.size() == 1);

	ztext::cache_variable_clear_all(zt);
	list = ztext::cache_variable_list(zt);
	CHECK(list.empty() == true);

	ztext::element_destroy(var);

	destroy(zt);
}
#endif // }}}


ztext::VectorString ztext::cache_variable_list(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_DEBUG_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	VectorString retval;
	for(auto& [name, element] : ztext->variable)
	{
		retval.push_back(name);
	}

	return retval;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("cache/variable/list")
{
	ztext::ZText* zt = ztext::create();

	ztext::Element* var = ztext::element_variable_create("name");
	ztext::element_variable_set(var, "The Foo");

	std::string name = ztext::eval(zt, var);

	ztext::VectorString list = ztext::cache_variable_list(zt);
	CHECK(list.size() == 1);
	CHECK(list[0]     == "name");

	ztext::element_destroy(var);
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

		//string_trim_(string, index_begin, index_end);

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

		ztext::cache_clear(zt);
		error = ztext::parse(empty, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == "");
		ztext::element_destroy(element);

		// -------------------------------------- //

		ztext::cache_clear(zt);
		error = ztext::parse(newlines, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == " ");
		ztext::element_destroy(element);

		// -------------------------------------- //

		ztext::cache_clear(zt);
		error = ztext::parse(spaces, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == " ");
		ztext::element_destroy(element);

		// -------------------------------------- //

		ztext::cache_clear(zt);
		error = ztext::parse(tabs, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == " ");
		ztext::element_destroy(element);
	}

	SUBCASE("Simple Text")
	{
		std::string text = "X";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == text);
		ztext::element_destroy(element);
	}

	SUBCASE("Leading White-Space")
	{
		std::string text = " X";

		error = ztext::parse(" 	 " + text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == text);
		ztext::element_destroy(element);
	}

	SUBCASE("Trailing White-Space")
	{
		std::string text = "X ";

		error = ztext::parse(text + " 	 	", element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == text);
		ztext::element_destroy(element);
	}

	SUBCASE("Leading and Trailing White-Space")
	{
		std::string text = " X ";

		error = ztext::parse("	" + text + "       ", element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == text);
		ztext::element_destroy(element);
	}

	SUBCASE("Clean White-Space")
	{
		std::string text = "X	Y  Z";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "X Y Z");
		ztext::element_destroy(element);
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
		
		CHECK(ztext::eval(zt, element) == " X Y Z ");
		ztext::element_destroy(element);
	}

	SUBCASE("Before Token")
	{
		std::string text = "foo {{bar$}}";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "foo ");

		while(element != nullptr)
		{
			element = ztext::element_destroy(element);
		}
	}

	SUBCASE("Escaped Token")
	{
		std::string text = "\\{{token\\}}";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "{{token}}");
		ztext::element_destroy(element);
	}

	SUBCASE("Embedded Escaped Token")
	{
		std::string text = "foo \\{{token\\}} bar";

		error = ztext::parse(text, element);
		CHECK(error == ztext::Error_None);
		
		CHECK(ztext::eval(zt, element) == "foo {{token}} bar");
		ztext::element_destroy(element);
	}

	destroy(zt);
}
#endif // }}}
#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{ parse/variable
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
		ztext::cache_clear(zt);

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
		ztext::cache_clear(zt);

		error = ztext::parse("{{ var $ }}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element       != nullptr);
		CHECK(element->type == ztext::Type::Variable);
		CHECK(element->text == "var");

		CHECK(ztext::eval(zt, element) == "");

		ztext::element_destroy(element);
	}

	SUBCASE("Variable With Data")
	{
		ztext::cache_clear(zt);

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
		error = ztext::parse("{{ var $ foo }}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element       != nullptr);
		CHECK(element->type == ztext::Type::Variable);
		CHECK(element->text == "var");

		CHECK(ztext::eval(zt, element) == "foo");

		ztext::element_destroy(element);
	}

	SUBCASE("Variable With Cleaned Data")
	{
		error = ztext::parse("{{ var$  \
			foo		\
			\\{{123\\}}	\
			bar		\
			}}", element);
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
		ztext::Element* foo = nullptr;
		ztext::Element* bar = nullptr;

		error = ztext::parse("{{var$ abc}}"      , var);
		error = ztext::parse("{{foo$ {{var$}} }}", foo);
		error = ztext::parse("{{bar$ {{foo$}} }}", bar);

		ztext::element_append(var, foo);
		ztext::element_append(foo, bar);

		CHECK(ztext::eval(zt, var) == "abcabcabc");

		ztext::print(var, true);
		ztext::element_destroy(var);
		ztext::element_destroy(foo);
		ztext::element_destroy(bar);

		error = ztext::parse("{{var$ xyz}}-{{foo$!{{var$}}!}}-{{bar$?{{foo$}}?}}", var);
		CHECK(ztext::eval(zt, var) == "xyz-!xyz!-?!xyz!?");
		ztext::element_destroy_all(var);
	}

	SUBCASE("Variable Recursive")
	{
		ztext::Element* var = nullptr;
		error = ztext::parse("{{foo$ {{bar$ {{foo$}} }} }}", var);
		CHECK(error == ztext::Error_None);

		CHECK(ztext::eval(zt, var) == "");

		ztext::element_destroy_all(var);
	}

	SUBCASE("Variable Reuse")
	{
		ztext::Element* doc = nullptr;
		error = ztext::parse("{{ name$ Billy Bob }} lives at {{ place$ {{name$}}'s House }}. \
			{{ name$ Johnny Ray }} lives at {{ place$ }}."
			, doc);
		CHECK(error == ztext::Error_None);

		CHECK(ztext::eval(zt, doc) == "Billy Bob lives at Billy Bob's House. Johnny Ray lives at Johnny Ray's House.");

		ztext::element_destroy_all(doc);
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

	ztext::element_destroy(foo);
	ztext::element_destroy(bar);
	ztext::element_destroy(abc);
	ztext::element_destroy(xyz);
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

	ztext::element_destroy(foo);
	ztext::element_destroy(bar);
	ztext::element_destroy(abc);
	ztext::element_destroy(xyz);
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

	ztext::Element* retval = element->next;

	std::stack<ztext::Element*> stack = {};

	element_remove(element);

	if(element->child != nullptr)
	{
		stack.push(element->child);
	}

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


void ztext::element_destroy_all(ztext::Element*& element
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

	while(element != nullptr)
	{
		element = ztext::element_destroy(element);
	}
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/destroy/all")
{
	// Must be run in Valgrind to check for memory leaks
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");
	ztext::Element* xyz = ztext::element_text_create("xyz");

	ztext::element_append(foo, xyz);
	ztext::element_append(xyz, bar);

	ztext::element_destroy_all(foo);
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

ztext::Element* ztext::element_text_create(const std::string& text
	) noexcept
{
	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Text;
	element->text = text;

	return element;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/text/create")
{
	ztext::ZText* zt = ztext::create();

	SUBCASE("Simple")
	{
		ztext::Element* text = ztext::element_text_create("text");
		CHECK(ztext::eval(zt, text) == "text");
		ztext::element_destroy(text);
	}

	SUBCASE("White-Space")
	{
		ztext::Element* text = ztext::element_text_create("   text   ");
		CHECK(ztext::eval(zt, text) == "   text   ");
		ztext::element_destroy(text);
	}

	ztext::destroy(zt);
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

	SUBCASE("Invalid")
	{
		std::error_code error = {};

		error = ztext::element_text_set(element, "aaa");
		CHECK(error == ztext::Error_Invalid_Parameter);

		element = ztext::element_variable_create("var");
		error = ztext::element_text_set(element, "bbb");
		CHECK(error == ztext::Error_Element_Type_Not_Text);
		ztext::element_destroy(element);
	}

	ztext::ZText* zt = ztext::create();

	SUBCASE("Simple")
	{
		element = ztext::element_text_create("ccc");
		ztext::element_text_set(element, "ddd");
		CHECK(ztext::eval(zt, element) == "ddd");
		ztext::element_destroy(element);
	}

	SUBCASE("White-Space")
	{
		element = ztext::element_text_create("ccc");
		ztext::element_text_set(element, "   d   d   d   ");
		CHECK(ztext::eval(zt, element) == "   d   d   d   ");
		ztext::element_destroy(element);
	}

	ztext::destroy(zt);
}
#endif // }}}

// }}}
// {{{ Element: Variable

ztext::Element* ztext::element_variable_create(const std::string& name
	) noexcept
{
	for(size_t index = 0; index < name.size(); index++)
	{
		if(is_valid_token_name_character_(name[index]) == false)
		{
			return nullptr;
		}
	}

	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Variable;
	element->text = name;

	return element;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/variable/create")
{
	ztext::ZText* zt = ztext::create();

	ztext::Element* var = ztext::element_variable_create("v{r");
	CHECK(var == nullptr);

	var = ztext::element_variable_create("var");
	CHECK(ztext::eval(zt, var) == "");
	ztext::element_destroy(var);

	ztext::destroy(zt);
}
#endif // }}}

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
printf("- %lu %c\n", begin, string[begin]);
printf("- %lu %c\n", end  , string[end]);
	#if ZTEXT_DEBUG_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}

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
		ztext::element_destroy_all(child);

		return error;
	}

	error = ztext::element_variable_set(element, child);

	element->child = child;

	return error;
}


std::error_code ztext::element_variable_set(Element* element
	, Element* content
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

	while(element->child != nullptr)
	{
		element->child = ztext::element_destroy(element->child);
	}

	element->child = content;

	return ztext::Error_None;
}

#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
TEST_CASE("element/variable/set")
{
	ztext::ZText*   zt    = ztext::create();
	ztext::Element* var   = nullptr;
	std::error_code error = {};

	SUBCASE("Invalid")
	{
		error = ztext::element_variable_set(var, "foo");
		CHECK(error == ztext::Error_Invalid_Parameter);

		var = ztext::element_text_create("text");
		error = ztext::element_variable_set(var, "foo");
		CHECK(error == ztext::Error_Element_Type_Not_Variable);
		ztext::element_destroy(var);
	}

	SUBCASE("String")
	{
		var = ztext::element_variable_create("var");
		
		error = ztext::element_variable_set(var, "abcdef");
		CHECK(error == ztext::Error_None);
		CHECK(ztext::eval(zt, var) == "abcdef");

		error = ztext::element_variable_set(var, "abcdef", 1, 4);
		CHECK(error == ztext::Error_None);
		CHECK(ztext::eval(zt, var) == "bcde");

		ztext::element_destroy(var);
	}

	SUBCASE("Element")
	{
		var = ztext::element_variable_create("var");
		
		error = ztext::element_variable_set(var, nullptr);
		CHECK(error == ztext::Error_None);
		CHECK(ztext::eval(zt, var) == "");

		std::string text = "   foo   ";
		error = ztext::element_variable_set(var
			, ztext::element_text_create(text)
			);
		CHECK(error == ztext::Error_None);
		CHECK(ztext::eval(zt, var) == text);

		ztext::element_destroy(var);
	}

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

#endif // }}}
