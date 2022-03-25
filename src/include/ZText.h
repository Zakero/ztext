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

	[[]]          void            debug_element(Element*) noexcept;
	[[]]          void            debug_command(ZText*) noexcept;
	[[]]          void            debug_variable(ZText*) noexcept;
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
		ZText*        z_script;
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


	ztext::Element* element_create(ztext::ZText* z_script
		, ztext::Type type
		) noexcept
	{
		ztext::Element* element = new ztext::Element;
		element->z_script = z_script;
		element->type     = type;

		return element;
	}


	ZText* create() noexcept
	{
		ZText* z_script = new ZText;

		z_script->root_element.z_script = z_script;
		z_script->root_element.type     = Type::Root;

		return z_script;
	};


	void destroy(ZText*& z_script
		) noexcept
	{
		clear(z_script);

		delete z_script;
		z_script = nullptr;
	}


	void clear(ZText* z_script
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		clear_commands(z_script);
		clear_elements(z_script);
		clear_variables(z_script);
	}


	void clear_commands(ZText* z_script
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		z_script->command = {};
	}


	void clear_elements(ZText* z_script
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		Element* element = z_script->root_element.next;
		while(element != nullptr)
		{
			element = element_destroy(element);
		}
	}


	void clear_variables(ZText* z_script
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		z_script->variable = {};
	}


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


	Element* parse_command(ZText* z_script
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

		Element* element = element_create(z_script
			, Type::Command
			);
		element->text = name;

		if(string[index] == '(')
		{
		}

		return element;
	}


	Element* parse_variable(ZText* z_script
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

		Element* element = element_create(z_script
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

		variable_set(z_script, name, value);

		return element;
	}


	void parse(ZText* z_script
		, const std::string string
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		if(string.empty() == true)
		{
			return;
		}

		size_t   index = 0;
		Type     token_type = Type::Text;
		Element* element = nullptr;

		if(string[0] == '{' && string[1] == '{')
		{
			token_type = parse_token_type(string, index);

			if(token_type == Type::Variable)
			{
				element = parse_variable(z_script, string, index);
			}

			if(token_type == Type::Command)
			{
				element = parse_command(z_script, string, index);
			}

			if(element)
			{
				debug_element(element);
			}
		}
	}


	void command_set(ZText* z_script
		, const std::string   name
		, const CommandLambda command
		) noexcept
	{
		if(z_script == nullptr)
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

		z_script->command[name] = command;
	}


	void command_destroy(ZText* z_script
		, const std::string name
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		if(name.empty())
		{
			// Error
		}

		if(z_script->command.contains(name) == false)
		{
			// Error
		}

		z_script->command.erase(name);
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
			command->child = element_create(command->z_script
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


	void variable_set(ZText* z_script
		, const std::string name
		, const std::string value
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		if(name.empty())
		{
			// Error
		}

		z_script->variable[name] = value;
	}


	void variable_destroy(ZText* z_script
		, const std::string    name
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		if(name.empty())
		{
			// Error
		}

		if(z_script->variable.contains(name) == false)
		{
			// Error
		}

		z_script->variable.erase(name);
	}


	Element* root_element(ZText* z_script
		) noexcept
	{
		if(z_script == nullptr)
		{
			// Error
		}

		return &z_script->root_element;
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

		Element* new_element = element_create(element->z_script
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

		Element* new_element = element_create(element->z_script
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

		Element* new_element = element_create(element->z_script
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

		element->z_script = nullptr;
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

		if(element->z_script->command.contains(name) == false)
		{
			return {};
		}

		std::string value = element->z_script->command[name](element->z_script, element);

		// Recursive Variables?

		return value;
	}


	std::string eval_variable(Element* element
		) noexcept
	{
		std::string name = element->text;

		if(element->z_script->variable.contains(name) == false)
		{
			return {};
		}

		std::string value = element->z_script->variable[name];

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


	void debug_element(Element* element
		) noexcept
	{
		#ifdef ZTEXT_DEBUG_ENABLED
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
		#else
		(void*)element;
		#endif
	}


	void debug_command(ZText* z_script
		) noexcept
	{
		#ifdef ZTEXT_DEBUG_ENABLED
		for(auto& [ name, value ] : z_script->command)
		{
			printf("%s()\n", name.c_str());
		}
		#else
		(void*)z_script;
		#endif
	}


	void debug_variable(ZText* z_script
		) noexcept
	{
		#ifdef ZTEXT_DEBUG_ENABLED
		size_t name_len = 0;

		for(auto& [ name, value ] : z_script->variable)
		{
			name_len = std::max(name_len, name.size());
		}

		for(auto& [ name, value ] : z_script->variable)
		{
			printf("%-*s: \"%s\"\n", (int)name_len, name.c_str(), value.c_str());
		}
		#else
		(void*)z_script;
		#endif
	}
}

#endif // }}}
