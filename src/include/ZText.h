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

#include <functional>
#include <string>
#include <unordered_map>

namespace ztext
{
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
	[[]]          void            parse(ZText*, const std::string) noexcept;

	[[nodiscard]] Element*        root_element(ZText*) noexcept;

	[[nodiscard]] Element*        element_append_command(Element*, const std::string) noexcept;
	[[nodiscard]] Element*        element_append_text(Element*, const std::string) noexcept;
	[[nodiscard]] Element*        element_append_variable(Element*, const std::string) noexcept;
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

	ztext::Element* element_create_(ztext::ZText* ztext
		, ztext::Type type
		) noexcept
	{
		ztext::Element* element = new ztext::Element;
		element->ztext = ztext;
		element->type     = type;

		return element;
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


	void parse(ZText* ztext
		, const std::string string
		) noexcept
	{
		if(ztext == nullptr)
		{
			// Error
		}

		if(string.empty() == true)
		{
			return;
		}

#if 0
		size_t   index = 0;
		Type     token_type = Type::Text;
		Element* element = nullptr;

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
	}

	#ifdef ZTEXT_IMPLEMENTATION_TEST // {{{
	TEST_CASE("parse/text")
	{
		ZText* ztext = create();
		std::string text = "Some random text.";

		parse(ztext, text);

		Element* element = root_element(ztext);

		CHECK(element != nullptr);
		CHECK(element_eval(element) == text);

		destroy(ztext);
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



	void element_append(Element* element_prev
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

		element_append(element, new_element);

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

		element_append(element, new_element);

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

		element_append(element, new_element);

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


	std::string element_eval(Element* element
		) noexcept
	{
		if(element == nullptr)
		{
			// error
		}

		switch(element->type)
		{
			case Type::Root:     return {};
			case Type::Text:     return element->text;
			case Type::Variable: return eval_variable(element);
			case Type::Command:  return eval_command(element);
		}

		return {};
	}


	Element* element_next(Element* element
		) noexcept
	{
		if(element == nullptr)
		{
			// error
		}

		return element->next;
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
