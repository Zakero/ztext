/******************************************************************************
 * Copyright 2021-2022 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/******************************************************************************
 * Includes
 */

// C++
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// POSIX


/******************************************************************************
 * Macros
 */

// {{{ Macros

#ifndef ZTEXT_ERROR_MESSAGES_ENABLED
#define ZTEXT_ERROR_MESSAGES_ENABLED 0
#endif

#define ZTEXT_ERROR_MESSAGE \
	if(ZTEXT_ERROR_MESSAGES_ENABLED == 0) {} \
	else std::cerr << std::source_location::current().function_name() << " - "

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
	X(Error_None                                       ,  0 , "No Error"               ) \
	X(Error_Invalid_Parameter                          ,  1 , "A parameter is invalid" ) \
	X(Error_Element_In_Use                             ,  2 , "The requested Element is in use by another ZText object" ) \
	X(Error_Element_Type_Not_Array                     ,  3 , "The expected Element type is array" ) \
	X(Error_Element_Type_Not_Command                   ,  4 , "The expected Element type is command" ) \
	X(Error_Element_Type_Not_Map                       ,  5 , "The expected Element type is map" ) \
	X(Error_Element_Type_Not_Text                      ,  6 , "The expected Element type is text" ) \
	X(Error_Element_Type_Not_Variable                  ,  7 , "The expected Element type is a variable" ) \
	X(Error_Parser_Token_Name_Invalid                  ,  8 , "The Parser found an invalid token name" ) \
	X(Error_Parser_No_Text_Found                       ,  9 , "The Parser was not able to find any text" ) \
	X(Error_Parser_Token_End_Marker_Missing            , 10 , "The Parser was not able to find the token end marker '}}'" ) \
	X(Error_Parser_Token_Name_Missing                  , 11 , "The Parser was not able to find the token name"        ) \
	X(Error_Parser_Token_Identifier_Invalid            , 12 , "The Parser found an invalid token type" ) \
	X(Error_Parser_Token_Begin_Marker_Missing          , 13 , "The Parser encountered a token end marker '}}' without a preceding begin marker '{{'" )\
	X(Error_Parser_Command_Property_End_Marker_Missing , 14 , "The Parser was not able to find the command property end marker ')'" ) \
	X(Error_Parser_Map_Begin_Marker_Missing            , 15 , "The Parser was not able to find the map begin marker '('" ) \
	X(Error_Parser_Map_End_Marker_Missing              , 16 , "The Parser was not able to find the map end marker ')'" ) \
	X(Error_Parser_Map_Key_Value_Pair_Missing          , 17 , "The Parser was not able to find the map's key/value pair" ) \
	X(Error_Parser_Map_Key_Missing                     , 18 , "The Parser was not able to find the key of the map's key/value pair" ) \
	X(Error_Parser_Map_Value_Missing                   , 19 , "The Parser was not able to find the value of the map's key/value pair" ) \
	X(Error_Parser_Map_Index_Invalid                   , 20 , "The Parser encounterd an invalid map index" ) \
	X(Error_Parser_Map_Contains_Invalid_Data           , 21 , "The Parser encounterd an invalid data in the map token" ) \
	X(Error_Map_Contains_Invalid_Name                  , 22 , "The Map contains an invalid name" ) \
	X(Error_Map_Contains_Invalid_Element               , 23 , "The Map contains an invalid element" ) \
	X(Error_Array_Contains_Invalid_Element             , 24 , "The Array contains an invalid element" ) \
	X(Error_Parser_Array_Begin_Marker_Missing          , 25 , "The Parser was not able to find the array begin marker '['" ) \
	X(Error_Parser_Array_End_Marker_Missing            , 26 , "The Parser was not able to find the array end marker ']'" ) \
	X(Error_Parser_Array_Contains_Invalid_Data         , 27 , "The Parser encounterd an invalid data in the array token" ) \
	X(Error_Parser_Array_Value_Missing                 , 28 , "The Parser was not able to find the value in the array" ) \


#define ZTextCommandLambda(...) [__VA_ARGS__](ztext::ZText* ztext, ztext::Element* element) -> std::string

// }}}


/******************************************************************************
 * ZText
 */

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

	using CommandLambda    = std::function<std::string(ZText*, Element*)>;
	using MapStringElement = std::unordered_map<std::string, Element*>;
	using MapStringString  = std::unordered_map<std::string, std::string>;
	using VectorElement    = std::vector<Element*>;
	using VectorString     = std::vector<std::string>;

	// --- ZText --- //
	[[nodiscard]] ZText*           create() noexcept;
	[[]]          void             destroy(ZText*&) noexcept;
	[[]]          void             clear(ZText*) noexcept;

	[[nodiscard]] Element*         array(ZText*, std::string, size_t) noexcept;
	[[]]          void             array_clear(ZText*) noexcept;
	[[]]          void             array_erase(ZText*, std::string) noexcept;
	[[nodiscard]] VectorString     array_list(ZText*) noexcept;
	[[]]          void             array_set(ZText*, std::string, VectorElement, bool = false) noexcept;

	[[]]          void             command_clear(ZText*) noexcept;
	[[]]          void             command_erase(ZText*, std::string) noexcept;
	[[]]          void             command_set(ZText*, std::string, ztext::CommandLambda) noexcept;

	[[nodiscard]] Element*         map(ZText*, std::string, std::string) noexcept;
	[[]]          void             map_clear(ZText*) noexcept;
	[[]]          void             map_erase(ZText*, std::string) noexcept;
	[[nodiscard]] VectorString     map_list(ZText*) noexcept;
	[[nodiscard]] VectorString     map_list(ZText*, std::string) noexcept;
	[[]]          void             map_set(ZText*, std::string, MapStringElement, bool = false) noexcept;

	[[nodiscard]] Element*         variable(ZText*, std::string) noexcept;
	[[]]          void             variable_clear(ZText*) noexcept;
	[[]]          void             variable_erase(ZText*, std::string) noexcept;
	[[nodiscard]] VectorString     variable_list(ZText*) noexcept;
	[[]]          void             variable_set(ZText*, std::string, Element*, bool = false) noexcept;

	// --- Evaluation --- //
	[[nodiscard]] std::string      eval(ZText*, Element*, bool = true) noexcept;

	// --- Parse --- //
	[[]]          std::error_code  parse(const std::string&, Element*&) noexcept;
	[[]]          std::error_code  parse(const std::string&, size_t, size_t, Element*&) noexcept;
	[[]]          std::error_code  parse(const std::string&, ztext::MapStringString&) noexcept;
	[[]]          std::error_code  parse(const std::string&, size_t, size_t, ztext::MapStringString&) noexcept;

	// --- Element --- //
	[[]]          std::error_code  element_append(Element*, Element*) noexcept;
	[[]]          std::error_code  element_insert(Element*, Element*) noexcept;
	[[]]          Element*         element_destroy(Element*&) noexcept;
	[[]]          void             element_destroy_all(Element*&) noexcept;
	[[]]          void             element_remove(Element*) noexcept;
	[[nodiscard]] Element*         element_next(Element*) noexcept;
	[[nodiscard]] Element*         element_prev(Element*) noexcept;
	[[nodiscard]] Element*         element_find_head(Element*) noexcept;
	[[nodiscard]] Element*         element_find_tail(Element*) noexcept;

	[[nodiscard]] Element*         element_array_create(const std::string&) noexcept;
	[[]]          std::error_code  element_array_set(Element*, VectorElement) noexcept;
	[[]]          std::error_code  element_array_index_set(Element*, size_t) noexcept;

	[[nodiscard]] Element*         element_command_create(const std::string&) noexcept;
	[[nodiscard]] Element*         element_command_content(Element*) noexcept;
	[[]]          std::error_code  element_command_content_set(Element*, Element*) noexcept;
	[[nodiscard]] MapStringString& element_command_property(Element*) noexcept;
	[[]]          void             element_command_property_set(Element*, MapStringString) noexcept;

	[[nodiscard]] Element*         element_map_create(const std::string&) noexcept;
	[[]]          std::error_code  element_map_set(Element*, MapStringElement) noexcept;
	[[]]          std::error_code  element_map_key_set(Element*, std::string) noexcept;

	[[nodiscard]] Element*         element_text_create(const std::string&) noexcept;
	[[]]          std::error_code  element_text_set(Element*, const std::string&) noexcept;

	[[nodiscard]] Element*         element_variable_create(const std::string&) noexcept;
	[[]]          std::error_code  element_variable_set(Element*, Element*) noexcept;
}

#ifdef ZTEXT_IMPLEMENTATION // {{{

// {{{ Includes

#include <source_location>
#include <stack>

// }}}
// {{{ ErrorCategory_

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
	// Token Types
	enum class Type : uint8_t
	{	Array
	,	Command
	,	Map
	,	Text
	,	Variable
	};

	// Element
	// The core structure that defines the data relationships.
	//
	// TODO: Find a way to merge "map" and "property".
	// TODO: Should property values be "pre-parsed" ?
	struct Element
	{
		Element*         next     = nullptr;
		Element*         prev     = nullptr;
		Element*         child    = nullptr;
		Element*         parent   = nullptr;
		MapStringElement map      = {};         // Storage for the Map Token
		VectorElement    array    = {};         // Storage for the Array Token
		MapStringString  property = {};         // Storage for the Command Token
		std::string      text     = {};         // Text or the Token Name
		Type             type     = Type::Text;
	};

	using MapStringBool       = std::unordered_map<std::string, bool>;
	using MapStringCommand    = std::unordered_map<std::string, CommandLambda>;
	using MapMapStringElement = std::unordered_map<std::string, MapStringElement>;
	using MapVectorElement    = std::unordered_map<std::string, VectorElement>;

	struct ZText
	{
		MapVectorElement    array             = {}; // A collection of arrays
		MapStringBool       array_readonly    = {}; // Determines which arrays are read-only
		MapStringCommand    command           = {}; // All of the added commands
		MapMapStringElement map               = {}; // A collection of maps
		MapStringBool       map_readonly      = {}; // Determines which maps are read-only
		MapStringElement    variable          = {}; // A collection of variables
		MapStringBool       variable_readonly = {}; // Determines which variables are read-only
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

	constexpr char Dataset_Array_Begin     = '[';
	constexpr char Dataset_Array_End       = ']';
	constexpr char Dataset_Array_Separator = ',';
	constexpr char Dataset_Map_Begin       = '(';
	constexpr char Dataset_Map_End         = ')';
	constexpr char Dataset_Map_Assignment  = '=';
	constexpr char Dataset_Map_Separator   = ',';
}

// }}}
// {{{ Private: Datatypes

namespace
{
	// The Token Definition
	//
	// Used exclusively and extensively by the parser.
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
		size_t type_index     = 0;
		char   type           = 0;
	};
}

// }}}
// {{{ Private: Debugging

#if ZTEXT_DEBUGGING_ENABLED
namespace
{
	// Convert the Element Type to a string.
	// Useful for debugging.
	std::string to_string_(const ztext::Type type
		) noexcept
	{
		switch(type)
		{
			case ztext::Type::Array:    return "array";
			case ztext::Type::Command:  return "command";
			case ztext::Type::Map:      return "map";
			case ztext::Type::Text:     return "text";
			case ztext::Type::Variable: return "variable";
		}

		return {};
	}


	// Print information about an element and optionally, its children.
	void debug_(const ztext::Element* element
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

		for(const auto& [key, value] : element->property)
		{
			printf("%*sproperty: %s=%s\n"
				, ((level + 1) * 3)
				, ""
				, key.c_str()
				, value.c_str()
				);
		}

		if(children == true)
		{
			element = element->child;

			while(element != nullptr)
			{
				debug_(element, children, level + 1);

				element = element->next;
			}
		}
	}

	// Print information about an element and optionally, its children.
	[[maybe_unused]]
	void debug_(const ztext::Element* element
		, const bool children = false
		) noexcept
	{
		debug_(element, children, 0);
	}

	// Print information about a Token
	[[maybe_unused]]
	void debug_(const Token& token
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
		printf("type_index: %lu '%c'\n"
			, token.type_index
			, (token.type_index == 0 || token.type_index >= len || token.type_index >= len) ? '\0' : string[token.type_index]
			);
		printf("type      : '%c'\n"
			, token.type
			);
		printf("property  : %lu,%lu '%s'\n"
			, token.property_begin
			, token.property_end
			, (token.property_begin >= token.property_end || token.property_begin >= len || token.property_end >= len) ? "" : std::string(string.substr(token.property_begin, (token.property_end - token.property_begin + 1))).c_str()
			);
		printf("content   : %lu,%lu '%s'\n"
			, token.content_begin
			, token.content_end
			, (token.content_begin >= token.content_end || token.content_begin >= len || token.content_end >= len) ? "" : std::string(string.substr(token.content_begin, (token.content_end - token.content_begin + 1))).c_str()
			);
	}
}
#endif

// }}}
// {{{ Private: Element Utilities

namespace
{
	ztext::Element* element_copy_(ztext::Element*) noexcept;
	ztext::Element* element_copy_all_(ztext::Element*) noexcept;
	void            element_init_(ztext::Element*) noexcept;


	// Creates a copy of an Element.
	// All the of Element's children will be copied as well.
	// The Element connections are NOT carried over to the copy.
	ztext::Element* element_copy_(ztext::Element* element
		) noexcept
	{
		ztext::Element* retval = new ztext::Element;
		
		retval->property = element->property;
		retval->text     = element->text;
		retval->type     = element->type;

		if(element->child != nullptr)
		{
			retval->child = element_copy_all_(element->child);
		}

		return retval;
	}


	// Creates a copy of all the Elements starting with element all the way to the tail.
	// All the of Element children will be copied as well.
	// And all the copies will be connected in the same order as the original elements.
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

		return retval;
	}


	// Initialize an Element to a clean slate.
	inline void element_init_(ztext::Element* element
		) noexcept
	{
		element->next     = nullptr;
		element->prev     = nullptr;
		element->child    = nullptr;
		element->parent   = nullptr;
		element->map      = {};
		element->array    = {};
		element->property = {};
		element->text     = {};
		element->type     = ztext::Type::Text;
	}
}

// }}}
// {{{ Private: Array Utilities

namespace
{
	// Make a deep copy of an array.
	ztext::VectorElement array_copy_(ztext::VectorElement& array_orig
		) noexcept
	{
		ztext::VectorElement array_copy = {};

		for(auto& element : array_orig)
		{
			array_copy.push_back(element_copy_all_(element));
		}

		return array_copy;
	}


	// Destroy the entire contents of an array.
	void array_destroy_(ztext::VectorElement& array
		) noexcept
	{
		for(auto& element : array)
		{
			ztext::element_destroy_all(element);
		}

		array.clear();
	}
}

// }}}
// {{{ Private: Map Utilities

namespace
{
	// Make a deep copy of a map.
	ztext::MapStringElement map_copy_(ztext::MapStringElement& map_orig
		) noexcept
	{
		ztext::MapStringElement map_copy = {};

		for(auto& [ name, element ] : map_orig)
		{
			map_copy[name] = element_copy_all_(element);
		}

		return map_copy;
	}


	// Destroy the entire contents of a map.
	void map_destroy_(ztext::MapStringElement& map
		) noexcept
	{
		for(auto& [ name, element ] : map)
		{
			ztext::element_destroy_all(element);
		}

		map.clear();
	}
}

// }}}
// {{{ Private: Evaluation: Array

namespace
{
	std::string element_eval_array_(ztext::ZText* ztext
		, ztext::Element* element
		) noexcept
	{
		std::string key   = element->property[""];
		size_t      index = 0;

		if(key.empty() == true)
		{
			index = element->array.size();
		}
		else
		{
			index = std::stoul(key);
		}

		if(element->array.empty() == true)
		{
			ztext::Element* e = ztext::array(ztext, element->text, index);
			std::string retval = ztext::eval(ztext, e);

			return retval;
		}

		if(ztext->array_readonly.contains(element->text) == true
			&& ztext->array_readonly[element->text] == true
			)
		{
			ztext::Element* e = ztext::array(ztext, element->text, index);
			std::string retval = ztext::eval(ztext, e);

			return retval;
		}

		std::string retval = "";

		if(index < element->array.size())
		{
			retval = ztext::eval(ztext, element->array[index]);
		}

		ztext::array_set(ztext
			, element->text
			, array_copy_(element->array)
			, false
			);

		return retval;
	}
}

// }}}
// {{{ Private: Evaluation: Command

namespace
{
	std::string element_eval_command_(ztext::ZText* ztext
		, ztext::Element* element
		) noexcept
	{
		if(ztext->command.contains(element->text) == false)
		{
			ZTEXT_ERROR_MESSAGE
				<< "Error: Command '"
				<< element->text
				<< "' not found."
				<< '\n';

			return "";
		}

		std::string retval = ztext->command[element->text](ztext, element);

		return retval;
	}
}

// }}}
// {{{ Private: Evaluation: Map

namespace
{
	std::string element_eval_map_(ztext::ZText* ztext
		, ztext::Element* element
		) noexcept
	{
		std::string key = element->property[""];

		if(element->map.empty() == true)
		{
			ztext::Element* e = ztext::map(ztext, element->text, key);
			std::string retval = ztext::eval(ztext, e);

			return retval;
		}

		if(ztext->map_readonly.contains(element->text) == true
			&& ztext->map_readonly[element->text] == true
			)
		{
			ztext::Element* e = ztext::map(ztext, element->text, key);
			std::string retval = ztext::eval(ztext, e);

			return retval;
		}

		std::string retval = "";

		if(element->map.contains(key) == false)
		{
			retval = "";
		}
		else
		{
			retval = ztext::eval(ztext, element->map[key]);
		}

		ztext::map_set(ztext
			, element->text
			, map_copy_(element->map)
			, false
			);

		return retval;
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
		if(element->child == nullptr)
		{
			ztext::Element* e = ztext::variable(ztext, element->text);
			std::string retval = ztext::eval(ztext, e);

			return retval;
		}

		if(ztext->variable_readonly.contains(element->text) == true
			&& ztext->variable_readonly[element->text] == true
			)
		{
			ztext::Element* e = ztext::variable(ztext, element->text);
			std::string retval = ztext::eval(ztext, e);

			return retval;
		}

		std::string retval = ztext::eval(ztext, element->child);

		ztext::Element* content = element_copy_all_(element->child);
		ztext::variable_set(ztext, element->text, content, false);

		ztext->variable[element->text] = content;

		return retval;
	}
}

// }}}
// {{{ Private: String Utils

namespace
{
	std::string escape_sequence_remove_(std::string string
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
					index++;
				}
				else if(string[index + 1] == Token_End
					&& string[index + 2] == Token_End
					)
				{
					string.erase(index, 1);
					index++;
				}
			}

			index++;
		}

		return string;
	}


	size_t find_char_(const std::string& string
		, unsigned char ch
		, size_t        begin
		, size_t        end
		) noexcept
	{
		size_t index = begin;
		while(index <= end)
		{
			if(string[index] == ch
				&& string[index - 1] != Token_Escape
				)
			{
				break;
			}

			index++;
		}

		return index;
	}


	size_t find_char_end_(const std::string& string
		, unsigned char char_begin
		, unsigned char char_end
		, size_t index_begin
		, size_t index_end
		) noexcept
	{
		size_t index = index_begin + 1;
		size_t depth = 0;

		while(index <= index_end)
		{
			if(string[index] == char_begin
				&& string[index - 1] != Token_Escape
				)
			{
				depth++;
			}

			if(string[index] == char_end
				&& string[index - 1] != Token_Escape
				)
			{
				if(depth == 0)
				{
					break;
				}

				depth--;
			}

			index++;
		}

		return index;
	}


	std::string whitespace_clean_(std::string string
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


	size_t whitespace_skip_leading_(const std::string& string
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


	size_t whitespace_skip_trailing_(const std::string& string
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


	inline std::string substr_(const std::string& string
		, size_t begin
		, size_t end
		) noexcept
	{
		return string.substr(begin, (end - begin + 1));
	}
}

// }}}
// {{{ Private: Parse

namespace
{
	std::error_code parse_(const std::string&, size_t&, size_t&, ztext::Element*&) noexcept;
	std::error_code parse_(const std::string&, ztext::VectorString&) noexcept;
	std::error_code parse_(const std::string&, size_t, size_t, ztext::VectorString&) noexcept;
	std::error_code parse_key_value_(const std::string&, size_t, size_t, std::string&, std::string&) noexcept;
	std::error_code parse_text_(const std::string&, size_t&, size_t&, ztext::Element*&) noexcept;
	std::error_code parse_token_(const std::string&, size_t&, size_t&, ztext::Element*&) noexcept;
	std::error_code parse_token_array_(Token&, const std::string&) noexcept;
	std::error_code parse_token_map_(Token&, const std::string&) noexcept;
	std::error_code parse_token_identifier_(Token&, const std::string&) noexcept;
	std::error_code parse_token_name_(Token&, const std::string&) noexcept;
	std::error_code parse_token_command_(Token&, const std::string&) noexcept;
	std::error_code parse_token_variable_(Token&, const std::string&) noexcept;


	// {{{ Private: Parse

	std::error_code parse_(const std::string& string
		, size_t&          index_begin
		, size_t&          index_end
		, ztext::Element*& element_head
		) noexcept
	{
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
	// {{{ Private: Parse: VectorString

	[[maybe_unused]]
	std::error_code parse_(const std::string& string
		, ztext::VectorString& array
		) noexcept
	{
		array.clear();

		#if ZTEXT_ERROR_CHECKS_ENABLED
		if(string.empty() == true)
		{
			return ztext::Error_Parser_No_Text_Found;
		}
		#endif

		size_t begin = whitespace_skip_leading_(string, 0);
		size_t end   = whitespace_skip_trailing_(string, string.size() - 1);

		#if ZTEXT_ERROR_CHECKS_ENABLED
		if(begin > end)
		{
			return ztext::Error_Parser_Array_Begin_Marker_Missing;
		}

		if(string[begin] != Dataset_Array_Begin)
		{
			return ztext::Error_Parser_Array_Begin_Marker_Missing;
		}

		if(string[end] != Dataset_Array_End)
		{
			return ztext::Error_Parser_Array_End_Marker_Missing;
		}
		#endif

		return parse_(string, begin, end, array);
	}


	std::error_code parse_(const std::string& string
		, size_t               begin
		, size_t               end
		, ztext::VectorString& array
		) noexcept
	{
		array.clear();

		#if ZTEXT_ERROR_CHECKS_ENABLED
		if(string.empty() == true)
		{
			return ztext::Error_Parser_No_Text_Found;
		}
		#endif

		begin = whitespace_skip_leading_(string, begin);
		end   = whitespace_skip_trailing_(string, end);

		if((begin + 1) == end)
		{
			return ztext::Error_None;
		}

		#if ZTEXT_ERROR_CHECKS_ENABLED
		if(begin > end)
		{
			return ztext::Error_Parser_Array_Begin_Marker_Missing;
		}

		if(string[begin] != Dataset_Array_Begin)
		{
			return ztext::Error_Parser_Array_Begin_Marker_Missing;
		}

		if(string[end] != Dataset_Array_End)
		{
			return ztext::Error_Parser_Array_End_Marker_Missing;
		}
		#endif

		size_t kv_begin = begin;
		size_t kv_end   = 0;

		while(kv_begin < end)
		{
			kv_end = find_char_(string, Dataset_Array_Separator, kv_begin + 1, end);
			if(kv_end > end)
			{
				kv_end = end;
			}

			if(kv_begin + 1 >= kv_end)
			{
				return ztext::Error_Parser_Array_Value_Missing;
			}

			std::string value = substr_(string
				, whitespace_skip_leading_(string, kv_begin + 1)
				, whitespace_skip_trailing_(string, kv_end - 1)
				);
			value = whitespace_clean_(value);

			array.push_back(value);

			kv_begin = kv_end;
		}

		return ztext::Error_None;
	}


	#ifdef ZTEXT_IMPLEMENTATION_TEST
	TEST_CASE("/parse/array/") // {{{
	{
		SUBCASE("Invalid")
		{
			ztext::VectorString test  = { "foo", "bar" };
			std::error_code     error = {};

			error = parse_("", test);
			CHECK(error == ztext::Error_Parser_No_Text_Found);
			CHECK(test.empty() == true);

			error = parse_("]", test);
			CHECK(error == ztext::Error_Parser_Array_Begin_Marker_Missing);

			error = parse_("[", test);
			CHECK(error == ztext::Error_Parser_Array_End_Marker_Missing);

			error = parse_("[,]", test);
			CHECK(error == ztext::Error_Parser_Array_Value_Missing);

			error = parse_("[,foo]", test);
			CHECK(error == ztext::Error_Parser_Array_Value_Missing);

			error = parse_("[foo,]", test);
			CHECK(error == ztext::Error_Parser_Array_Value_Missing);
		}

		SUBCASE("Empty")
		{
			ztext::VectorString test  = {};
			std::error_code     error = {};

			error = parse_("[]", test);
			CHECK(error == ztext::Error_None);
			CHECK(test.empty() == true);
		}

		SUBCASE("Whitespace")
		{
			ztext::VectorString control =
			{	"foo"
			,	"abc"
			,	"123"
			};

			ztext::VectorString test;
			std::string string = "[ \
				foo		\
				,abc, \
				123\
				]";
			std::error_code error = parse_(string, test);
			CHECK(error == ztext::Error_None);

			CHECK(control.size() == test.size());

			for(size_t i = 0; i < control.size(); i++)
			{
				CHECK(control[i] == test[i]);
			}
		}
	} // }}}
	#endif

	// }}}
	// {{{ Private: Parse: Key/Value

	std::error_code parse_key_value_(const std::string& string
		, size_t       begin
		, size_t       end
		, std::string& key
		, std::string& value
		) noexcept
	{
		size_t index = find_char_(string, Dataset_Map_Assignment, begin, end);

		#if ZTEXT_ERROR_CHECKS_ENABLED
		if(index == begin)
		{
			return ztext::Error_Parser_Map_Key_Missing;
		}

		if(index > end)
		{
			return ztext::Error_Parser_Map_Key_Value_Pair_Missing;
		}
		#endif

		size_t key_begin   = begin + 1;
		size_t key_end     = index - 1;
		size_t value_begin = index + 1;
		size_t value_end   = end   - 1;

		key_begin = whitespace_skip_leading_(string, key_begin);
		key_end   = whitespace_skip_trailing_(string, key_end);

		value_begin = whitespace_skip_leading_(string, value_begin);
		value_end   = whitespace_skip_trailing_(string, value_end);

		#if ZTEXT_ERROR_CHECKS_ENABLED
		if(key_begin > key_end)
		{
			return ztext::Error_Parser_Map_Key_Missing;
		}

		if(value_begin > value_end)
		{
			return ztext::Error_Parser_Map_Value_Missing;
		}
		#endif

		key = substr_(string, key_begin, key_end);
		key = whitespace_clean_(key);

		value = substr_(string, value_begin, value_end);
		value = whitespace_clean_(value);

		#if ZTEXT_ERROR_CHECKS_ENABLED
		if(key.empty() == true)
		{
			return ztext::Error_Parser_Map_Key_Missing;
		}

		if(value.empty() == true)
		{
			return ztext::Error_Parser_Map_Value_Missing;
		}
		#endif

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
		std::string text = substr_(string, begin, index);
		text = whitespace_clean_(text);

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

	std::error_code parse_token_(const std::string& string
		, size_t&          begin
		, size_t&          end
		, ztext::Element*& element
		) noexcept
	{
		std::error_code error       = {};
		size_t          index_begin = begin + 2;
		size_t          index_end   = index_begin;
		size_t          depth       = 0;

		// --- Find Token End --- //

		while(index_end <= end)
		{
			if(string[index_end] == Token_Begin
				&& (index_end + 1) <= end
				&& string[index_end + 1] == Token_Begin
				&& string[index_end - 1] != Token_Escape
				)
			{
				depth++;
				index_end++;
			}

			if(string[index_end] == Token_End
				&& (index_end + 1) <= end
				&& string[index_end + 1] == Token_End
				&& string[index_end - 1] != Token_Escape
				)
			{
				if(depth == 0)
				{
					index_end++;
					break;
				}

				depth--;
				index_end++;
			}

			index_end++;
		}

		if(index_end > end)
		{
			begin = index_begin;
			return ztext::Error_Parser_Token_End_Marker_Missing;
		}

		// --- --------- --- //

		Token token;
		token.begin = begin;
		token.end   = index_end;

		// --- Token Name --- //

		error = parse_token_name_(token, string);

		if(error != ztext::Error_None)
		{
			begin = token.name_begin;
			return error;
		}

		// --- Token Identifier --- //

		error = parse_token_identifier_(token, string);

		if(error != ztext::Error_None)
		{
			begin = token.type_index;
			return error;
		}

		// --- Token: Array --- //

		if(token.type == Identifier_Array)
		{
			parse_token_array_(token, string);

			element = ztext::element_array_create(
				substr_(string, token.name_begin, token.name_end)
				);

			if(token.content_begin != 0)
			{
				std::string index = substr_(string, token.content_begin, token.content_end);

				element->property[""] = index;
			}

			if(token.property_begin != 0)
			{
				ztext::VectorString raw_data;
				parse_(string
					, token.property_begin
					, token.property_end
					, raw_data
					);
				for(auto& raw_element : raw_data)
				{
					ztext::Element* e = nullptr;
					error = ztext::parse(raw_element, e);
					if(error)
					{
						return error;
					}

					element->array.push_back(e);
				}
			}
		}

		// --- Token: Command --- //

		if(token.type == Identifier_Command)
		{
			parse_token_command_(token, string);

			element = ztext::element_command_create(
				substr_(string, token.name_begin, token.name_end)
				);

			if(token.property_begin != 0)
			{
				ztext::MapStringString map;
				ztext::parse(string
					, token.property_begin
					, token.property_end
					, map
					);
				ztext::element_command_property_set(element, map);
			}

			if(token.content_begin != 0)
			{
				ztext::Element* content = nullptr;
				error = ztext::parse(substr_(string, token.content_begin, token.content_end)
					, content
					);

				if(error)
				{
					return error;
				}

				error = ztext::element_command_content_set(element
					, content
					);
			}
		}

		// --- Token: Map --- //

		if(token.type == Identifier_Map)
		{
			parse_token_map_(token, string);

			element = ztext::element_map_create(
				substr_(string, token.name_begin, token.name_end)
				);

			if(token.content_begin != 0)
			{
				std::string index = substr_(string, token.content_begin, token.content_end);

				element->property[""] = index;
			}

			if(token.property_begin != 0)
			{
				ztext::MapStringString raw_data;
				ztext::parse(string
					, token.property_begin
					, token.property_end
					, raw_data
					);
				for(auto& [ key, raw_element ] : raw_data)
				{
					error = ztext::parse(raw_element
						, element->map[key]
						);
					if(error)
					{
						return error;
					}
				}
			}
		}

		// --- Token: Variable --- //

		if(token.type == Identifier_Variable)
		{
			parse_token_variable_(token, string);

			element = ztext::element_variable_create(
				substr_(string, token.name_begin, token.name_end)
				);

			if(token.content_begin != 0)
			{
				ztext::Element* content = nullptr;
				error = ztext::parse(substr_(string, token.content_begin, token.content_end)
					, content
					);

				if(error)
				{
					return error;
				}

				error = ztext::element_variable_set(element
					, content
					);
			}
		}

		// --- Done --- //

		begin = index_end + 1;

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token Name

	inline bool is_valid_token_name_character_(const unsigned char c
		) noexcept
	{
		if(std::isalnum(c) != 0
			|| c == '_'
			)
		{
			return true;
		}

		return false;
	}


	inline bool token_name_is_valid(const std::string& string
		) noexcept
	{
		for(const unsigned char c : string)
		{
			if(is_valid_token_name_character_(c) == false)
			{
				return false;
			}
		}

		return true;
	}


	std::error_code parse_token_name_(Token& token
		, const std::string& string
		) noexcept
	{
		size_t index = whitespace_skip_leading_(string, token.begin + 2);
		token.name_begin = index;

		if(string[index] == Identifier_Variable
			|| string[index] == Identifier_Array
			|| string[index] == Identifier_Command
			|| string[index] == Identifier_Map
			|| string[index] == Token_End
			)
		{
			return ztext::Error_Parser_Token_Name_Missing;
		}

		while(is_valid_token_name_character_(string[index]) == true)
		{
			index++;
		}

		if(token.name_begin == index)
		{
			return ztext::Error_Parser_Token_Name_Invalid;
		}

		token.name_end = index - 1;

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token Identifier

	std::error_code parse_token_identifier_(Token& token
		, const std::string& string
		) noexcept
	{
		size_t index = whitespace_skip_leading_(string, token.name_end + 1);
		token.type_index = index;

		if(string[index] == Identifier_Array)
		{
			token.type = Identifier_Array;
			return ztext::Error_None;
		}

		if(string[index] == Identifier_Map)
		{
			token.type = Identifier_Map;
			return ztext::Error_None;
		}

		if(string[index] == Identifier_Variable)
		{
			token.type = Identifier_Variable;
			return ztext::Error_None;
		}

		token.type = Identifier_Command;
		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token: Array

	std::error_code parse_token_array_(Token& token
		, const std::string& string
		) noexcept
	{
		size_t index = whitespace_skip_leading_(string, token.type_index + 1);

		if(string[index] == Token_End)
		{
			return ztext::Error_None;
		}

		if(string[index] != Dataset_Array_Begin)
		{
			if(is_valid_token_name_character_(string[index]) == false)
			{
				return ztext::Error_Parser_Map_Index_Invalid;
			}

			token.content_begin = index;
			index++;

			while(index < token.end)
			{
				if(is_valid_token_name_character_(string[index]) == false)
				{
					break;
				}

				index++;
			}

			token.content_end = index - 1;
			index = whitespace_skip_leading_(string, index);
		}

		if(string[index] == Dataset_Array_Begin)
		{
			token.property_begin = index;

			index = find_char_end_(string
				, Dataset_Array_Begin
				, Dataset_Array_End
				, token.property_begin
				, token.end
				);

			if(index > token.end)
			{
				return ztext::Error_Parser_Array_End_Marker_Missing;
			}

			token.property_end = index;

			index++;
		}

		index = whitespace_skip_leading_(string, index);

		if(string[index] != Token_End)
		{
			return ztext::Error_Parser_Array_Contains_Invalid_Data;
		}

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token: Map

	std::error_code parse_token_map_(Token& token
		, const std::string& string
		) noexcept
	{
		size_t index = whitespace_skip_leading_(string, token.type_index + 1);

		if(string[index] == Token_End)
		{
			return ztext::Error_None;
		}

		if(string[index] != Dataset_Map_Begin)
		{
			if(is_valid_token_name_character_(string[index]) == false)
			{
				return ztext::Error_Parser_Map_Index_Invalid;
			}

			token.content_begin = index;
			index++;

			while(index < token.end)
			{
				if(is_valid_token_name_character_(string[index]) == false)
				{
					break;
				}

				index++;
			}

			token.content_end = index - 1;
			index = whitespace_skip_leading_(string, index);
		}

		if(string[index] == Dataset_Map_Begin)
		{
			token.property_begin = index;

			index = find_char_end_(string
				, Dataset_Map_Begin
				, Dataset_Map_End
				, token.property_begin
				, token.end
				);

			if(index > token.end)
			{
				return ztext::Error_Parser_Map_End_Marker_Missing;
			}

			token.property_end = index;

			index++;
		}

		index = whitespace_skip_leading_(string, index);

		if(string[index] != Token_End)
		{
			return ztext::Error_Parser_Map_Contains_Invalid_Data;
		}

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token: Command

	std::error_code parse_token_command_(Token& token
		, const std::string& string
		) noexcept
	{
		if(string[token.type_index] == Token_End)
		{
			return ztext::Error_None;
		}

		size_t index = token.type_index;

		if(string[index] == Dataset_Map_Begin)
		{
			token.property_begin = token.type_index;

			index = find_char_end_(string
				, Dataset_Map_Begin
				, Dataset_Map_End
				, token.property_begin
				, token.end
				);

			if(index > token.end)
			{
				return ztext::Error_Parser_Command_Property_End_Marker_Missing;
			}

			token.property_end = index;

			index++;
		}

		index = whitespace_skip_leading_(string, index);

		if(string[index] != Token_End)
		{
			token.content_begin = index;
			token.content_end   = whitespace_skip_trailing_(string, token.end - 2);
		}

		return ztext::Error_None;
	}

	// }}}
	// {{{ Private: Parse: Token: Variable

	std::error_code parse_token_variable_(Token& token
		, const std::string& string
		) noexcept
	{
		size_t index = whitespace_skip_leading_(string, token.type_index + 1);

		if(string[index] == Token_End)
		{
			return ztext::Error_None;
		}

		token.content_begin = index;
		token.content_end   = whitespace_skip_trailing_(string, token.end - 2);

		return ztext::Error_None;
	}

	// }}}
}

// }}}
// {{{ Private: Error

namespace
{
	void report_error_(const std::error_code& error
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

// Create a new instance of a ZText object.
ztext::ZText* ztext::create() noexcept
{
	ztext::ZText* ztext = new ztext::ZText;

	return ztext;
};


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/create/") // {{{
{
	ztext::ZText* zt = ztext::create();

	CHECK(zt != nullptr);

	ztext::destroy(zt);
} // }}}
#endif


// Destroy an instance of a ZText object and its contents.
void ztext::destroy(ztext::ZText*& ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	ztext::clear(ztext);

	delete ztext;
	ztext = nullptr;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/destroy/") // {{{
{
	ztext::ZText* zt = ztext::create();
	ztext::destroy(zt);

	CHECK(zt == nullptr);
} // }}}
#endif


// Clear the contents of a ZText object.
void ztext::clear(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	ztext::array_clear(ztext);
	ztext::command_clear(ztext);
	ztext::map_clear(ztext);
	ztext::variable_clear(ztext);
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/clear/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::Element* var = ztext::element_variable_create("name");
	ztext::element_variable_set(var
		, ztext::element_text_create("The Foo")
		);

	std::string name = ztext::eval(zt, var);

	ztext::VectorString list = ztext::variable_list(zt);
	CHECK(list.size() == 1);

	ztext::element_destroy_all(var);
	ztext::clear(zt);
	list = ztext::variable_list(zt);
	CHECK(list.empty() == true);

	destroy(zt);
} // }}}
#endif

// }}}
// {{{ ZText: Array

// Access the array data in the ZText object.
ztext::Element* ztext::array(ztext::ZText* ztext
	, std::string name
	, size_t      index
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->array.contains(name) == false)
	{
		return nullptr;
	}

	if(index >= ztext->array[name].size())
	{
		return nullptr;
	}

	ztext::Element* element = ztext->array[name][index];

	return element;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/array/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Clear all array data from the ZText object.
void ztext::array_clear(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	for(auto& [name, array] : ztext->array)
	{
		array_destroy_(array);
	}

	ztext->array.clear();
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/array/clear/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Erase the contents of a single array in the ZText object.
void ztext::array_erase(ztext::ZText* ztext
	, std::string name
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->array.contains(name) == false)
	{
		return;
	}

	array_destroy_(ztext->array[name]);

	ztext->array.erase(name);
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/array/erase/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Get a list of all the arrays in the ZText object.
ztext::VectorString ztext::array_list(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	VectorString retval;
	for(auto& [name, vector] : ztext->array)
	{
		retval.push_back(name);
	}

	return retval;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/array/list/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Set the contents of an array in the ZText object.
// If the array does not exist, it will be created.
// If the array does exists, the contents will be destroyed.
// A deep copy of the "array" will be made and stored in the ZText object.
void ztext::array_set(ztext::ZText* ztext
	, std::string          name
	, ztext::VectorElement array
	, bool                 read_only
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}

	if(array.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'array' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->array.contains(name) == true)
	{
		array_destroy_(ztext->array[name]);
	}

	ztext->array[name] = array;
	ztext->array_readonly[name] = read_only;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/array/set/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif

// }}}
// {{{ ZText: Command

// Remove all the commands from the ZText object.
void ztext::command_clear(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	ztext->command.clear();
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/command/clear/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Remove a command from the ZText object.
void ztext::command_erase(ztext::ZText* ztext
	, std::string name
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}

	if(token_name_is_valid(name) == false)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' is not valid."
			<< '\n';
	}
	#endif

	if(ztext->command.contains(name) == false)
	{
		return;
	}

	ztext->command.erase(name);
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/command/erase/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Add or replace a command in the ZText object.
void ztext::command_set(ztext::ZText* ztext
	, std::string          name
	, ztext::CommandLambda lambda
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}

	if(token_name_is_valid(name) == false)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' is not valid."
			<< '\n';
	}

	if(lambda == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'lambda' can not be NULL."
			<< '\n';
	}
	#endif

	ztext->command[name] = lambda;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/command/set/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif

// }}}
// {{{ ZText: Map

// Access the contents of a map in the ZText object.
ztext::Element* ztext::map(ztext::ZText* ztext
	, std::string name
	, std::string key
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}

	if(key.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'key' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->map.contains(name) == false)
	{
		return nullptr;
	}

	if(ztext->map[name].contains(key) == false)
	{
		return nullptr;
	}

	ztext::Element* element = ztext->map[name][key];

	return element;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/map/eval/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Clear all the map data in the ZText object.
void ztext::map_clear(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	for(auto& [name, map] : ztext->map)
	{
		map_destroy_(map);
	}

	ztext->map.clear();
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/map/clear/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Remove a single map from the ZText object.
void ztext::map_erase(ztext::ZText* ztext
	, std::string name
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->map.contains(name) == false)
	{
		return;
	}

	map_destroy_(ztext->map[name]);

	ztext->map.erase(name);
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/map/erase/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Get a list of all the maps in the ZText object.
ztext::VectorString ztext::map_list(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}
	#endif

	VectorString retval;
	for(auto& [name, map] : ztext->map)
	{
		retval.push_back(name);
	}

	return retval;
}


// Get a list of all the keys of the specified map in the ZText object.
ztext::VectorString ztext::map_list(ztext::ZText* ztext
	, std::string name
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->map.contains(name) == false)
	{
		return {};
	}

	VectorString retval;
	for(auto& [key, value] : ztext->map[name])
	{
		retval.push_back(key);
	}

	return retval;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/map/list/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Set the contents of a map in the ZText object.
// If the map does not exist, it will be created.
// If the map does exists, the contents will be destroyed.
// A deep copy of the "map" will be made and stored in the ZText object.
void ztext::map_set(ztext::ZText* ztext
	, std::string             name
	, ztext::MapStringElement map
	, bool                    read_only
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}

	if(map.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'map' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->map.contains(name) == true)
	{
		map_destroy_(ztext->map[name]);
	}

	ztext->map[name] = map;
	ztext->map_readonly[name] = read_only;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/map/set/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif

// }}}
// {{{ ZText: Variable

// Get the contents of a variable in the ZText object.
ztext::Element* ztext::variable(ztext::ZText* ztext
	, std::string name
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->variable.contains(name) == false)
	{
		return nullptr;
	}

	ztext::Element* element = ztext->variable[name];

	return element;
}


// Remove all variables from the ZText object.
void ztext::variable_clear(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/variable/clear/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::Element* var = ztext::element_variable_create("name");
	ztext::element_variable_set(var
		, ztext::element_text_create("The Foo")
		);

	std::string name = ztext::eval(zt, var);

	ztext::VectorString list = ztext::variable_list(zt);
	CHECK(list.size() == 1);

	ztext::variable_clear(zt);
	list = ztext::variable_list(zt);
	CHECK(list.empty() == true);

	ztext::element_destroy(var);

	destroy(zt);
} // }}}
#endif


// Remove a single variable from the ZText object.
void ztext::variable_erase(ztext::ZText* ztext
	, std::string name
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be NULL."
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty."
			<< '\n';
	}
	#endif

	if(ztext->variable.contains(name) == false)
	{
		return;
	}

	ztext::element_destroy_all(ztext->variable[name]);

	ztext->variable.erase(name);
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/variable/erase/") // {{{
{
	ztext::ZText* zt = ztext::create();

	destroy(zt);
} // }}}
#endif


// Get a list of all variables in the ZText object.
ztext::VectorString ztext::variable_list(ztext::ZText* ztext
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/variable/list/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::Element* var = ztext::element_variable_create("name");
	ztext::element_variable_set(var
		, ztext::element_text_create("The Foo")
		);

	std::string name = ztext::eval(zt, var);

	ztext::VectorString list = ztext::variable_list(zt);
	CHECK(list.size() == 1);
	CHECK(list[0]     == "name");

	ztext::element_destroy(var);
	destroy(zt);
} // }}}
#endif


// Set the value of a variable in the ZText object.
void ztext::variable_set(ztext::ZText* ztext
	, std::string     name
	, ztext::Element* element
	, bool            read_only
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be null"
			<< '\n';
	}

	if(name.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'name' can not be empty"
			<< '\n';
	}

	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	if(ztext->variable.contains(name) == true)
	{
		ztext::element_destroy_all(ztext->variable[name]);
	}

	ztext->variable[name] = element;
	ztext->variable_readonly[name] = read_only;
}

// }}}
// {{{ Evaluation

// The provide element will be evaluated and the resulting string will be returned.
std::string ztext::eval(ztext::ZText* ztext
	, ztext::Element* element
	, bool            to_end
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(ztext == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'ztext' can not be null"
			<< '\n';
	}
	#endif

	if(element == nullptr)
	{
		return "";
	}

	std::string retval;

	while(true)
	{
		switch(element->type)
		{
			case ztext::Type::Text:
				retval += escape_sequence_remove_(element->text);
				break;

			case ztext::Type::Variable:
				retval += element_eval_variable_(ztext, element);
				break;

			case ztext::Type::Command:
				retval += element_eval_command_(ztext, element);
				break;

			case ztext::Type::Map:
				retval += element_eval_map_(ztext, element);
				break;

			case ztext::Type::Array:
				retval += element_eval_array_(ztext, element);
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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/eval/command/") // {{{
{
	ztext::ZText* zt = ztext::create();
	ztext::command_set(zt
		, "cmd"
		, ZTextCommandLambda(/* ztext, element */)
		{
			std::string     retval  = {};
			ztext::Element* content = nullptr;

			content = ztext::element_command_content(element);

			if(content != nullptr)
			{
				retval += ztext::eval(ztext, content);
			}

			retval += "X";

			return retval;
		});

	SUBCASE("single")
	{
		ztext::Element* element = ztext::element_command_create("cmd");
		CHECK(ztext::eval(zt, element, false) == "X");
		ztext::element_destroy(element);
	}

	SUBCASE("many")
	{
		ztext::Element* foo = ztext::element_command_create("cmd");
		ztext::Element* bar = ztext::element_command_create("cmd");
		ztext::Element* zig = ztext::element_command_create("cmd");

		ztext::element_append(foo, bar);
		ztext::element_append(bar, zig);
		CHECK(ztext::eval(zt, foo, true) == "XXX");

		ztext::element_destroy(foo);
		ztext::element_destroy(bar);
		ztext::element_destroy(zig);
	}

	SUBCASE("deep")
	{
		ztext::Element* foo = ztext::element_command_create("cmd");
		ztext::Element* bar = ztext::element_command_create("cmd");
		ztext::Element* zig = ztext::element_command_create("cmd");

		ztext::element_command_content_set(foo, bar);
		ztext::element_command_content_set(bar, zig);
		CHECK(ztext::eval(zt, foo, true) == "XXX");

		ztext::element_destroy(foo);
	}

	destroy(zt);
} // }}}
TEST_CASE("/eval/text/") // {{{
{
	ztext::ZText* zt = ztext::create();

	SUBCASE("single")
	{
		ztext::Element* text = ztext::element_text_create("text");
		CHECK(ztext::eval(zt, text, false) == "text");
		ztext::element_destroy(text);

		text = ztext::element_text_create("\\{{text\\}}");
		CHECK(ztext::eval(zt, text, false) == "{{text}}");
		ztext::element_destroy(text);

		text = ztext::element_text_create("{{text}}");
		CHECK(ztext::eval(zt, text, false) == "{{text}}");
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
} // }}}
TEST_CASE("/eval/variable/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::variable_set(zt, "var", ztext::element_text_create("X"));

	SUBCASE("single")
	{
		ztext::Element* element = ztext::element_variable_create("var");
		CHECK(ztext::eval(zt, element, false) == "X");
		ztext::element_destroy(element);
	}

	SUBCASE("many")
	{
		ztext::Element* foo = ztext::element_variable_create("var");
		ztext::Element* bar = ztext::element_variable_create("var");
		ztext::Element* zig = ztext::element_variable_create("var");

		ztext::element_append(foo, bar);
		ztext::element_append(bar, zig);
		CHECK(ztext::eval(zt, foo, true) == "XXX");

		ztext::element_destroy(foo);
		ztext::element_destroy(bar);
		ztext::element_destroy(zig);
	}

	destroy(zt);
} // }}}
#endif

// }}}
// {{{ Parse

// The provided string will be converted into a heirarchy of elements.
// The head of the heirarchy will be stored in element.
std::error_code ztext::parse(const std::string& string
	, ztext::Element*& element
	) noexcept
{
	if(string.empty() == true)
	{
		element = element_text_create("");

		return Error_None;
	}

	size_t begin = 0;
	size_t end   = string.size() - 1;

	return ztext::parse(string, begin, end, element);
}


// A section of the provided string will be converted into a heirarchy of elements.
// The head of the heirarchy will be stored in element.
std::error_code ztext::parse(const std::string& string
	, size_t           begin
	, size_t           end
	, ztext::Element*& element
	) noexcept
{
	if(string.empty() == true)
	{
		element = element_text_create("");

		return Error_None;
	}

	std::error_code error = parse_(string, begin, end, element);

	if(error == ztext::Error_Parser_No_Text_Found)
	{
		element = ztext::element_text_create("");

		return Error_None;
	}

	if(error != Error_None)
	{
		ztext::element_destroy_all(element);

		report_error_(error, string, begin);

		return error;
	}

	if(element == nullptr)
	{
		element = ztext::element_text_create("");
	}

	return Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/parse/element/array/") // {{{
{
	ztext::ZText*   zt      = ztext::create();
	ztext::Element* element = nullptr;
	std::error_code error   = {};

	SUBCASE("Invaild")
	{
		error = ztext::parse("{{array@", element);
		CHECK(error == ztext::Error_Parser_Token_End_Marker_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{array@\\}}", element);
		CHECK(error == ztext::Error_Parser_Token_End_Marker_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{@}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{*@}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Invalid);
	}

	SUBCASE("No Data")
	{
		ztext::clear(zt);

		error = ztext::parse("{{array@}}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element != nullptr);
		CHECK(ztext::eval(zt, element) == "");

		ztext::element_destroy(element);
	}

	SUBCASE("Set Data Then Lookup")
	{
		ztext::clear(zt);

		error = ztext::parse("{{array@[foo,bar,abc]}}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element != nullptr);
		CHECK(ztext::eval(zt, element) == "");

		ztext::Element* data;
		error = ztext::parse("{{array@1}}", data);
		CHECK(error == ztext::Error_None);
		CHECK(ztext::eval(zt, data) == "bar");

		ztext::element_destroy(data);
		ztext::element_destroy(element);
		ztext::clear(zt);
	}

	SUBCASE("Set And Lookup Data")
	{
		ztext::clear(zt);

		error = ztext::parse("{{array@2[foo,bar,abc]}}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element != nullptr);
		CHECK(ztext::eval(zt, element) == "abc");

		ztext::element_destroy(element);
	}

	destroy(zt);
} // }}}
TEST_CASE("/parse/element/command/") // {{{
{
	ztext::ZText*   zt      = ztext::create();
	ztext::Element* element = nullptr;
	std::error_code error   = {};

	SUBCASE("Simple")
	{
		ztext::command_set(zt
			, "cmd"
			, ZTextCommandLambda()
			{
				return "text";
			});

		SUBCASE("Property:None,Content:None")
		{
			error = ztext::parse("{{cmd}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "text");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:None,Content:Unused")
		{
			error = ztext::parse("{{cmd foo bar}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "text");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:Unused,Content:None")
		{
			error = ztext::parse("{{cmd(key=value)}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "text");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:Unused,Content:Unused")
		{
			error = ztext::parse("{{cmd(key=value)foo bar}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "text");
			ztext::element_destroy_all(element);
		}
	}

	SUBCASE("Content")
	{
		ztext::command_set(zt
			, "cmd"
			, ZTextCommandLambda()
			{
				ztext::Element* content
					= ztext::element_command_content(element);

				std::string str = "";
				if(content != nullptr)
				{
					str = ztext::eval(ztext, content);
				}

				std::string retval = "--" + str + "--";

				return retval;
			});

		SUBCASE("Property:None,Content:None")
		{
			error = ztext::parse("{{cmd}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "----");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:None,Content:Used")
		{
			error = ztext::parse("{{cmd foo bar}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "--foo bar--");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:Unused,Content:None")
		{
			error = ztext::parse("{{cmd(key=value)}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "----");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:Unused,Content:Used")
		{
			error = ztext::parse("{{cmd(key=value) foo bar }}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "--foo bar--");
			ztext::element_destroy_all(element);
		}
	}

	SUBCASE("Property")
	{
		ztext::command_set(zt
			, "cmd"
			, ZTextCommandLambda()
			{
				ztext::MapStringString property
					= ztext::element_command_property(element);

				std::string retval = "";
				for(auto& [ key, value ] : property)
				{
					retval += '|' + key + ':' + value + '|';
				}

				return retval;
			});

		SUBCASE("Property:None,Content:None")
		{
			error = ztext::parse("{{cmd}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:None,Content:Unused")
		{
			error = ztext::parse("{{cmd foo bar}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:Used,Content:None")
		{
			error = ztext::parse("{{cmd(foo=bar)}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "|foo:bar|");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:Used,Content:Unused")
		{
			error = ztext::parse("{{cmd(foo=bar) blah blah}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "|foo:bar|");
			ztext::element_destroy_all(element);
		}
	}

	SUBCASE("Complex")
	{
		ztext::command_set(zt
			, "cmd"
			, ZTextCommandLambda(/* ztext, element */)
			{
				ztext::MapStringString property
					= ztext::element_command_property(element);

				ztext::Element* content
					= ztext::element_command_content(element);

				std::string retval = "";

				for(auto& [ key, value ] : property)
				{
					retval += '|' + key + ':' + value + '|';
				}

				std::string str = "";
				if(content != nullptr)
				{
					str = ztext::eval(ztext, content);
				}

				retval += "--" + str + "--";

				return retval;
			});

		SUBCASE("Property:None,Content:None")
		{
			error = ztext::parse("{{cmd}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "----");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:None,Content:Used")
		{
			error = ztext::parse("{{cmd foo bar}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "--foo bar--");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:Used,Content:None")
		{
			error = ztext::parse("{{cmd(abc=xyz)}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "|abc:xyz|----");
			ztext::element_destroy_all(element);
		}

		SUBCASE("Property:Used,Content:Used")
		{
			error = ztext::parse("{{cmd(abc=xyz) foo bar}}", element);
			CHECK(error == ztext::Error_None);
			CHECK(ztext::eval(zt, element) == "|abc:xyz|--foo bar--");
			ztext::element_destroy_all(element);
		}
	}

	ztext::destroy(zt);
} // }}}
TEST_CASE("/parse/element/map/") // {{{
{
	ztext::ZText*   zt      = ztext::create();
	ztext::Element* element = nullptr;
	std::error_code error   = {};

	SUBCASE("Invaild")
	{
		error = ztext::parse("{{map#", element);
		CHECK(error == ztext::Error_Parser_Token_End_Marker_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{map#\\}}", element);
		CHECK(error == ztext::Error_Parser_Token_End_Marker_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{#}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Missing);

		// -------------------------------------- //

		error = ztext::parse("{{*#}}", element);
		CHECK(error == ztext::Error_Parser_Token_Name_Invalid);
	}

	SUBCASE("No Data")
	{
		ztext::clear(zt);

		error = ztext::parse("{{map#}}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element != nullptr);
		CHECK(ztext::eval(zt, element) == "");

		ztext::element_destroy(element);
	}

	SUBCASE("Set Data Then Lookup")
	{
		ztext::clear(zt);

		error = ztext::parse("{{map#(foo=bar,abc=xyz)}}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element != nullptr);
		CHECK(ztext::eval(zt, element) == "");

		ztext::Element* data;
		error = ztext::parse("{{map#foo}}", data);
		CHECK(error == ztext::Error_None);
		CHECK(ztext::eval(zt, data) == "bar");

		ztext::element_destroy(data);
		ztext::element_destroy(element);
		ztext::clear(zt);
	}

	SUBCASE("Set And Lookup Data")
	{
		ztext::clear(zt);

		error = ztext::parse("{{map#abc(foo=bar,abc=xyz)}}", element);
		CHECK(error == ztext::Error_None);

		CHECK(element != nullptr);
		CHECK(ztext::eval(zt, element) == "xyz");

		ztext::element_destroy(element);
	}

	destroy(zt);
} // }}}
TEST_CASE("/parse/element/text/") // {{{
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
		ztext::element_destroy(element);

		// -------------------------------------- //

		ztext::clear(zt);
		error = ztext::parse(newlines, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == " ");
		ztext::element_destroy(element);

		// -------------------------------------- //

		ztext::clear(zt);
		error = ztext::parse(spaces, element);
		CHECK(error   == ztext::Error_None);
		CHECK(element != nullptr);
		
		CHECK(ztext::eval(zt, element) == " ");
		ztext::element_destroy(element);

		// -------------------------------------- //

		ztext::clear(zt);
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
} // }}}
TEST_CASE("/parse/element/variable/") // {{{
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

		ztext::element_destroy(var);
		ztext::element_destroy(foo);
		ztext::element_destroy(bar);

		error = ztext::parse("{{var$ xyz}} {{foo$|{{var$}}|}} {{bar$-{{foo$}}-}}", var);
		CHECK(ztext::eval(zt, var) == "xyz |xyz| -|xyz|-");
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
} // }}}
#endif


// The string will be parsed and stored in the provided map.
// The first and last characters of the string must be the Map beginning and ending characters.
std::error_code ztext::parse(const std::string& string
	, ztext::MapStringString& map
	) noexcept
{
	map.clear();

	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(string.empty() == true)
	{
		return Error_Parser_No_Text_Found;
	}
	#endif

	size_t begin = whitespace_skip_leading_(string, 0);
	size_t end   = whitespace_skip_trailing_(string, string.size() - 1);

	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(begin > end)
	{
		return Error_Parser_Map_Begin_Marker_Missing;
	}

	if(string[begin] != Dataset_Map_Begin)
	{
		return Error_Parser_Map_Begin_Marker_Missing;
	}

	if(string[end] != Dataset_Map_End)
	{
		return Error_Parser_Map_End_Marker_Missing;
	}
	#endif

	return ztext::parse(string, begin, end, map);
}


// A section of the string will be parsed and stored in the provided map.
// The first and last characters of the parsed string must be the Map beginning and ending characters.
std::error_code ztext::parse(const std::string& string
	, size_t                  begin
	, size_t                  end
	, ztext::MapStringString& map
	) noexcept
{
	map.clear();

	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(string.empty() == true)
	{
		return Error_Parser_No_Text_Found;
	}
	#endif

	begin = whitespace_skip_leading_(string, begin);
	end   = whitespace_skip_trailing_(string, end);

	if(whitespace_skip_leading_(string, begin + 1) == end)
	{
		return Error_None;
	}

	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(begin > end)
	{
		return Error_Parser_Map_Begin_Marker_Missing;
	}

	if(string[begin] != Dataset_Map_Begin)
	{
		return Error_Parser_Map_Begin_Marker_Missing;
	}

	if(string[end] != Dataset_Map_End)
	{
		return Error_Parser_Map_End_Marker_Missing;
	}
	#endif

	size_t kv_begin = begin;
	size_t kv_end   = 0;

	while(kv_begin < end)
	{
		kv_end = find_char_(string, Dataset_Map_Separator, kv_begin + 1, end);
		if(kv_end > end)
		{
			kv_end = end;
		}

		std::string     key;
		std::string     value;
		std::error_code error;

		error = parse_key_value_(string, kv_begin, kv_end, key, value);
		if(error)
		{
			return error;
		}

		map[key] = value;

		kv_begin = kv_end;
	}

	return Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/parse/map/") // {{{
{
	SUBCASE("Invalid")
	{
		ztext::MapStringString test  = {{"foo", "bar"}};
		std::error_code        error = {};

		error = ztext::parse("", test);
		CHECK(error == ztext::Error_Parser_No_Text_Found);
		CHECK(test.empty() == true);

		error = ztext::parse(")", test);
		CHECK(error == ztext::Error_Parser_Map_Begin_Marker_Missing);

		error = ztext::parse("(", test);
		CHECK(error == ztext::Error_Parser_Map_End_Marker_Missing);

		error = ztext::parse("(,)", test);
		CHECK(error == ztext::Error_Parser_Map_Key_Value_Pair_Missing);

		error = ztext::parse("(foo=)", test);
		CHECK(error == ztext::Error_Parser_Map_Value_Missing);

		error = ztext::parse("(foo   =    )", test);
		CHECK(error == ztext::Error_Parser_Map_Value_Missing);

		error = ztext::parse("(foo=,)", test);
		CHECK(error == ztext::Error_Parser_Map_Value_Missing);

		error = ztext::parse("(a=b,foo=,)", test);
		CHECK(error == ztext::Error_Parser_Map_Value_Missing);

		error = ztext::parse("(foo=,a=b)", test);
		CHECK(error == ztext::Error_Parser_Map_Value_Missing);

		error = ztext::parse("(=bar)", test);
		CHECK(error == ztext::Error_Parser_Map_Key_Missing);

		error = ztext::parse("(=bar,)", test);
		CHECK(error == ztext::Error_Parser_Map_Key_Missing);

		error = ztext::parse("(a=b,=bar,)", test);
		CHECK(error == ztext::Error_Parser_Map_Key_Missing);

		error = ztext::parse("(a=b,   =   bar,)", test);
		CHECK(error == ztext::Error_Parser_Map_Key_Missing);

		error = ztext::parse("(=bar,a=b)", test);
		CHECK(error == ztext::Error_Parser_Map_Key_Missing);
	}

	SUBCASE("Empty")
	{
		ztext::MapStringString test  = {};
		std::error_code        error = {};

		error = ztext::parse("()", test);
		CHECK(error == ztext::Error_None);
		CHECK(test.empty() == true);
	}

	SUBCASE("Whitespace")
	{
		ztext::MapStringString control =
		{	{	"foo", "bar" }
		,	{	"abc", "xyz" }
		,	{	"123", "456" }
		};

		ztext::MapStringString test;
		std::string string = "( \
			foo		=  	bar \
			,abc=xyz, \
			123\
			=\
			456\
			)";
		std::error_code error = ztext::parse(string, test);
		CHECK(error == ztext::Error_None);

		CHECK(control.size() == test.size());

		for(const auto& [key, value] : test)
		{
			CHECK(control.contains(key) == true);
			CHECK(control[key] == value);
		}
	}
} // }}}
#endif

// }}}
// {{{ Element

// The provided elememnt will be placed after the specified position.
// If the element is the head of a chain, the entire chain will be added.
std::error_code ztext::element_append(ztext::Element* position
	, ztext::Element* element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(position == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'position' can not be NULL."
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be NULL."
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->prev != nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Element In-Use: 'element' is already linked."
			<< '\n';

		return Error_Element_In_Use;
	}
	#endif

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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/append/") // {{{
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
} // }}}
#endif


// The provided elememnt will be placed before the specified position.
// If the element is the head of a chain, the entire chain will be added.
std::error_code ztext::element_insert(ztext::Element* position
	, ztext::Element* element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(position == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'position' can not be NULL."
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be NULL."
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->prev != nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Element In-Use: 'element' is already linked."
			<< '\n';

		return Error_Element_In_Use;
	}
	#endif

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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/insert/") // {{{
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
} // }}}
#endif


// The specified element will be destroyed.
// If the element is a part of a chain, the chain will be updated as needed.
ztext::Element* ztext::element_destroy(ztext::Element*& element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
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

	array_destroy_(element->array);
	map_destroy_(element->map);

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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/destroy/") // {{{
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
} // }}}
#endif


// The specified element will be destroyed.
// If the element is a part of a chain, all the elements from element to the tail will be destroyed.
void ztext::element_destroy_all(ztext::Element*& element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	while(element != nullptr)
	{
		element = ztext::element_destroy(element);
	}
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/destroy/all/") // {{{
{
	// Must be run in Valgrind to check for memory leaks
	ztext::Element* foo = ztext::element_text_create("foo");
	ztext::Element* bar = ztext::element_text_create("bar");
	ztext::Element* xyz = ztext::element_text_create("xyz");

	ztext::element_append(foo, xyz);
	ztext::element_append(xyz, bar);

	ztext::element_destroy_all(foo);
	CHECK(foo == nullptr);
} // }}}
#endif


// Remove the specified element from the element chain.
void ztext::element_remove(ztext::Element* element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/remove/") // {{{
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
} // }}}
#endif


// Get the next element in the chain.
ztext::Element* ztext::element_next(ztext::Element* element
		) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	return element->next;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/next/") // {{{
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
} // }}}
#endif


// Get the previous element in the chain.
ztext::Element* ztext::element_prev(ztext::Element* element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}
	#endif

	return element->prev;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/prev/") // {{{
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
} // }}}
#endif


// Find the first element (ie: the head) of the chain.
ztext::Element* ztext::element_find_head(ztext::Element* element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/find/head/") // {{{
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
} // }}}
#endif


// Find the last element (ie: the tail) of the chain.
ztext::Element* ztext::element_find_tail(ztext::Element* element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/find/tail/") // {{{
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
} // }}}
#endif

// }}}
// {{{ Element: Array

// Create an Array Element.
ztext::Element* ztext::element_array_create(const std::string& name
	) noexcept
{
	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Array;
	element->text = name;

	return element;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/array/create/") // {{{
{
	ztext::ZText* zt = ztext::create();

	SUBCASE("Simple")
	{
		ztext::Element* array = ztext::element_array_create("map");
		CHECK(ztext::eval(zt, array) == "");
		ztext::element_destroy(array);
	}

	ztext::destroy(zt);
} // }}}
#endif


// Set the value of the array in the element.
// When this element is evaluated by a ZText object, the Array data will be stored in that ZText object.
std::error_code ztext::element_array_set(Element* element
	, VectorElement array
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->type != ztext::Type::Array)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'array'"
			<< '\n';

		return Error_Element_Type_Not_Array;
	}

	if(array.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'array' can not be empty"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	for(const auto& element : array)
	{
		if(element == nullptr)
		{
			ZTEXT_ERROR_MESSAGE
				<< "Invalid Parameter: 'array' contains an invalid element"
				<< '\n';

			return Error_Array_Contains_Invalid_Element;
		}
	}
	#endif

	array_destroy_(element->array);
	element->array = array;

	return Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/array/set/") // {{{
{
	SUBCASE("Invalid")
	{
		std::error_code error = {};

		error = ztext::element_array_set(nullptr, {});
		CHECK(error == ztext::Error_Invalid_Parameter);

		ztext::Element* element = nullptr;

		element = ztext::element_text_create("text");
		error = ztext::element_array_set(element, {});
		CHECK(error == ztext::Error_Element_Type_Not_Array);
		ztext::element_destroy(element);

		element = ztext::element_array_create("array_element");
		error = ztext::element_array_set(element, {});
		CHECK(error == ztext::Error_Invalid_Parameter);

		ztext::VectorElement bad_data = { nullptr };
		error = ztext::element_array_set(element, bad_data);
		CHECK(error == ztext::Error_Array_Contains_Invalid_Element);

		element_destroy(element);
	}

	SUBCASE("Valid")
	{
		ztext::ZText* zt = ztext::create();
		ztext::variable_set(zt, "var", ztext::element_text_create("abc"));

		ztext::VectorElement array_data =
		{	ztext::element_text_create("hello")
		,	ztext::element_variable_create("var")
		};

		ztext::Element* element = ztext::element_array_create("array_element");
		ztext::element_array_set(element, array_data);

		CHECK(ztext::eval(zt, element) == "");

		ztext::element_destroy(element);
		ztext::destroy(zt);
	}
} // }}}
#endif


// Set the Array index to use when accessing the Array data.
std::error_code ztext::element_array_index_set(ztext::Element* element
	, size_t index
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->type != ztext::Type::Array)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'array'"
			<< '\n';

		return Error_Element_Type_Not_Array;
	}
	#endif

	element->property[""] = std::to_string(index);

	return ztext::Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/array/index/set/") // {{{
{
	SUBCASE("Invalid")
	{
		ztext::Element* element = nullptr;
		std::error_code error   = {};

		error = ztext::element_array_index_set(nullptr, 0);
		CHECK(error == ztext::Error_Invalid_Parameter);

		element = ztext::element_text_create("text");
		error = ztext::element_array_index_set(element, 0);
		CHECK(error == ztext::Error_Element_Type_Not_Array);
		ztext::element_destroy(element);
	}

	SUBCASE("Valid")
	{
		ztext::ZText* zt = ztext::create();
		ztext::variable_set(zt
			, "var"
			, ztext::element_text_create("abc")
			);

		ztext::VectorElement array_data =
		{	ztext::element_text_create("hello")
		,	ztext::element_variable_create("var")
		};

		ztext::Element* element = ztext::element_array_create("array_element");
		ztext::element_array_set(element, array_data);

		ztext::element_array_index_set(element, 999999);
		CHECK(ztext::eval(zt, element) == "");

		ztext::element_array_index_set(element, 0);
		CHECK(ztext::eval(zt, element) == "hello");

		ztext::element_array_index_set(element, 1);
		CHECK(ztext::eval(zt, element) == "abc");

		ztext::element_destroy(element);
		ztext::destroy(zt);
	}
} // }}}
#endif

// }}}
// {{{ Element: Command

// Create a Command Element
// When this element is evaluated, the ZText object will provide this element to the Command Lambda.
ztext::Element* ztext::element_command_create(const std::string& name
	) noexcept
{
	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Command;
	element->text = name;

	return element;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/command/create/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::destroy(zt);
} // }}}
#endif

// Access the content of the Command Element.
// The returned element maybe the head of an element chain.
ztext::Element* ztext::element_command_content(Element* element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}

	if(element->type != ztext::Type::Command)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'command'"
			<< '\n';
	}
	#endif

	return element->child;
}


// Set the content of a Command Element.
std::error_code ztext::element_command_content_set(Element* element
	, Element* content
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->type != ztext::Type::Command)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'command'"
			<< '\n';

		return Error_Element_Type_Not_Command;
	}

	if(content->parent != nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'content' is being used by another element"
			<< '\n';

		return Error_Element_In_Use;
	}
	#endif

	if(element->child != nullptr)
	{
		element_destroy_all(element->child);
	}

	Element* e = content;
	while(e != nullptr)
	{
		e->parent = element;
		e = e->next;
	}

	element->child = content;

	return Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/command/content/set/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::destroy(zt);
} // }}}
#endif


// Access the Command Properties of this element.
ztext::MapStringString& ztext::element_command_property(Element* element
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';
	}

	if(element->type != ztext::Type::Command)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'command'"
			<< '\n';
	}
	#endif

	return element->property;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/command/property/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::destroy(zt);
} // }}}
#endif


// Set the Command properties for the specified element.
void ztext::element_command_property_set(Element* element
	, MapStringString property
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		//return Error_Invalid_Parameter;
	}

	if(element->type != ztext::Type::Command)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'command'"
			<< '\n';

		//return Error_Element_Type_Not_Command;
	}
	#endif

	element->property = property;

	//return Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/command/property/set/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::destroy(zt);
} // }}}
#endif

// }}}
// {{{ Element: Map

// Create a Map Element.
ztext::Element* ztext::element_map_create(const std::string& name
	) noexcept
{
	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Map;
	element->text = name;

	return element;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/map/create/") // {{{
{
	ztext::ZText* zt = ztext::create();

	SUBCASE("Simple")
	{
		ztext::Element* map = ztext::element_map_create("map");
		CHECK(ztext::eval(zt, map) == "");
		ztext::element_destroy(map);
	}

	ztext::destroy(zt);
} // }}}
#endif


// Set the value of the map in the element.
// When this element is evaluated by a ZText object, the contents of the map will be stored in that ZText object.
std::error_code ztext::element_map_set(Element* element
	, MapStringElement map
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->type != ztext::Type::Map)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'map'"
			<< '\n';

		return Error_Element_Type_Not_Map;
	}

	if(map.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'map' can not be empty"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	for(const auto& [ name, element ] : map)
	{
		if(name.empty() == true)
		{
			ZTEXT_ERROR_MESSAGE
				<< "Invalid Parameter: 'map' contains an invalid name"
				<< '\n';

			return Error_Map_Contains_Invalid_Name;
		}

		if(element == nullptr)
		{
			ZTEXT_ERROR_MESSAGE
				<< "Invalid Parameter: 'map' contains an invalid element"
				<< '\n';

			return Error_Map_Contains_Invalid_Element;
		}
	}
	#endif

	map_destroy_(element->map);
	element->map = map;

	return Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/map/set/") // {{{
{
	SUBCASE("Invalid")
	{
		ztext::Element* element = nullptr;
		std::error_code error   = {};

		error = ztext::element_map_set(nullptr, {});
		CHECK(error == ztext::Error_Invalid_Parameter);

		element = ztext::element_text_create("text");
		error = ztext::element_map_set(element, {});
		CHECK(error == ztext::Error_Element_Type_Not_Map);
		ztext::element_destroy(element);

		element = ztext::element_map_create("map_element");
		error = ztext::element_map_set(element, {});
		CHECK(error == ztext::Error_Invalid_Parameter);

		ztext::MapStringElement bad_data = { { "", nullptr } };
		error = ztext::element_map_set(element, bad_data);
		CHECK(error == ztext::Error_Map_Contains_Invalid_Name);

		bad_data = { { "Valid Name", nullptr } };
		error = ztext::element_map_set(element, bad_data);
		CHECK(error == ztext::Error_Map_Contains_Invalid_Element);

		element_destroy(element);
	}

	SUBCASE("Valid")
	{
		ztext::ZText* zt = ztext::create();
		ztext::variable_set(zt, "var", ztext::element_text_create("abc"));

		ztext::MapStringElement map_data =
		{	{ "foo", ztext::element_text_create("hello") }
		,	{ "bar", ztext::element_variable_create("var") }
		};

		ztext::Element* element = ztext::element_map_create("map_element");
		ztext::element_map_set(element, map_data);

		CHECK(ztext::eval(zt, element) == "");

		ztext::element_destroy(element);
		ztext::destroy(zt);
	}
} // }}}
#endif


// Set the key to use when this Element is evaluated.
std::error_code ztext::element_map_key_set(ztext::Element* element
	, std::string index
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->type != ztext::Type::Map)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'map'"
			<< '\n';

		return Error_Element_Type_Not_Map;
	}

	if(index.empty() == true)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'index' can not be empty"
			<< '\n';

		return Error_Invalid_Parameter;
	}
	#endif

	element->property[""] = index;

	return ztext::Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/map/key/set/") // {{{
{
	SUBCASE("Invalid")
	{
		ztext::Element* element = nullptr;
		std::error_code error   = {};

		error = ztext::element_map_key_set(nullptr, "");
		CHECK(error == ztext::Error_Invalid_Parameter);

		element = ztext::element_text_create("text");
		error = ztext::element_map_key_set(element, "index");
		CHECK(error == ztext::Error_Element_Type_Not_Map);
		ztext::element_destroy(element);

		element = ztext::element_map_create("map_element");
		error = ztext::element_map_key_set(element, "");
		CHECK(error == ztext::Error_Invalid_Parameter);
		element_destroy(element);
	}

	SUBCASE("Valid")
	{
		ztext::ZText* zt = ztext::create();
		ztext::variable_set(zt
			, "var"
			, ztext::element_text_create("abc")
			);

		ztext::MapStringElement map_data =
		{	{ "foo", ztext::element_text_create("hello") }
		,	{ "bar", ztext::element_variable_create("var") }
		};

		ztext::Element* element = ztext::element_map_create("map_element");
		ztext::element_map_set(element, map_data);

		ztext::element_map_key_set(element, "does not exist");
		CHECK(ztext::eval(zt, element) == "");

		ztext::element_map_key_set(element, "foo");
		CHECK(ztext::eval(zt, element) == "hello");

		ztext::element_map_key_set(element, "bar");
		CHECK(ztext::eval(zt, element) == "abc");

		ztext::element_destroy(element);
		ztext::destroy(zt);
	}
} // }}}
#endif

// }}}
// {{{ Element: Text

// Create a Text Element
ztext::Element* ztext::element_text_create(const std::string& text
	) noexcept
{
	ztext::Element* element = new ztext::Element;

	element->type = ztext::Type::Text;
	element->text = text;

	return element;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/text/create/") // {{{
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
} // }}}
#endif


// Set the contents of the Text Element.
std::error_code ztext::element_text_set(Element* element
	, const std::string& text
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->type != ztext::Type::Text)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'text'"
			<< '\n';

		return Error_Element_Type_Not_Text;
	}
	#endif

	element->text = text;

	return Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/text/set/") // {{{
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
} // }}}
#endif

// }}}
// {{{ Element: Variable

// Create a Variable Element
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


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/variable/create/") // {{{
{
	ztext::ZText* zt = ztext::create();

	ztext::Element* var = ztext::element_variable_create("v{r");
	CHECK(var == nullptr);

	var = ztext::element_variable_create("var");
	CHECK(ztext::eval(zt, var) == "");
	ztext::element_destroy(var);

	ztext::destroy(zt);
} // }}}
#endif


// Set the value of the variable in the element.
// When this element is evaluated by a ZText object, the contents of the variable will be stored in that ZText object.
std::error_code ztext::element_variable_set(Element* element
	, Element* content
	) noexcept
{
	#if ZTEXT_ERROR_CHECKS_ENABLED
	if(element == nullptr)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' can not be null"
			<< '\n';

		return Error_Invalid_Parameter;
	}

	if(element->type != ztext::Type::Variable)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'element' must be of type 'variable'"
			<< '\n';

		return Error_Element_Type_Not_Variable;
	}

	if(content != nullptr
		&& content->parent != nullptr
		)
	{
		ZTEXT_ERROR_MESSAGE
			<< "Invalid Parameter: 'content' is being used"
			<< '\n';

		return Error_Element_In_Use;
	}
	#endif

	while(element->child != nullptr)
	{
		element->child = ztext::element_destroy(element->child);
	}

	element->child = content;

	while(content != nullptr)
	{
		content->parent = element;
		content = content->next;
	}

	return ztext::Error_None;
}


#ifdef ZTEXT_IMPLEMENTATION_TEST
TEST_CASE("/element/variable/set/") // {{{
{
	ztext::ZText*   zt    = ztext::create();
	ztext::Element* var   = nullptr;
	std::error_code error = {};

	SUBCASE("Invalid")
	{
		ztext::Element* content = ztext::element_text_create("foo");

		error = ztext::element_variable_set(var, content);
		CHECK(error == ztext::Error_Invalid_Parameter);

		var = ztext::element_text_create("text");
		error = ztext::element_variable_set(var, content);
		CHECK(error == ztext::Error_Element_Type_Not_Variable);

		ztext::element_destroy(var);
		ztext::element_destroy(content);
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

	SUBCASE("read-only")
	{
		ztext::Element* cache = nullptr;
		var = ztext::element_variable_create("var");

		// --- ------ --- //

		ztext::element_variable_set(var, ztext::element_text_create("aaa"));
		CHECK(ztext::eval(zt, var) == "aaa");
		cache = ztext::variable(zt, "var");
		CHECK(ztext::eval(zt, cache) == "aaa");

		// --- ------ --- //

		ztext::variable_set(zt, "var", ztext::element_text_create("bbb"));
		CHECK(ztext::eval(zt, var) == "aaa");

		cache = ztext::variable(zt, "var");
		CHECK(ztext::eval(zt, cache) == "aaa");

		// --- ------ --- //

		ztext::variable_set(zt, "var", ztext::element_text_create("ccc"), true);
		CHECK(ztext::eval(zt, var) == "ccc");

		cache = ztext::variable(zt, "var");
		CHECK(ztext::eval(zt, cache) == "ccc");

		// --- ------ --- //

		ztext::variable_set(zt, "var", ztext::element_text_create("ddd"), false);
		CHECK(ztext::eval(zt, var) == "aaa");

		cache = ztext::variable(zt, "var");
		CHECK(ztext::eval(zt, cache) == "aaa");

		// --- ------ --- //

		ztext::element_destroy(var);
	}

	destroy(zt);
} // }}}
#endif

// }}}

#endif // }}}
