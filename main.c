/*
 * Copyright (c) 2023-Present, Japroz Singh Saini <japrozsaini@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// cp_info_tag values
#define CONSTANT_Class 7
#define CONSTANT_Fieldref 9
#define CONSTANT_Methodref 10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_String 8
#define CONSTANT_Integer 3
#define CONSTANT_Float 4
#define CONSTANT_Long 5
#define CONSTANT_Double 6
#define CONSTANT_NameAndType 12
#define CONSTANT_Utf8 1
#define CONSTANT_MethodHandle 15
#define CONSTANT_MethodType 16
#define CONSTANT_InvokeDynamic 18

// access_flags mask table
#define ACC_PUBLIC 0x0001
#define ACC_PRIVATE 0x0002
#define ACC_PROTECTED 0x0004
#define ACC_STATIC 0x0008
#define ACC_FINAL 0x0010
#define ACC_SYNCHRONIZED 0x0020
#define ACC_BRIDGE 0x0040
#define ACC_VARARGS 0x0080
#define ACC_NATIVE 0x0100
#define ACC_ABSTRACT 0x0400
#define ACC_STRICT 0x0800
#define ACC_SYNTHETIC 0x1000

// helpful macros
#define READ_U2(var) fread(var, sizeof(unsigned short), 1, file)
#define SWAP(var) var = le_to_be(var) // swap the endian-ness of an unsigned short
#define ALLOC(type, count) (type *)malloc(sizeof(type) * (count))

struct constant_methodref_t
{
	unsigned short class_index;
	unsigned short name_and_type_index;
};

struct constant_utf8_t
{
	unsigned short length;
	uint8_t *bytes;
};

struct constant_class_t
{
	unsigned short name_index;
};

struct constant_name_and_type_info_t
{
	unsigned short name_index;
	unsigned short descriptor_index;
};

struct constant_fieldref_t
{
	unsigned short class_index;
	unsigned short name_and_type_index;
};

struct constant_string_t
{
	unsigned short string_index;
};

struct cp_info_t
{
	uint8_t tag;
	union
	{
		struct constant_methodref_t constant_methodref;
		struct constant_utf8_t constant_utf8;
		struct constant_class_t constant_class;
		struct constant_name_and_type_info_t constant_name_and_type_info;
		struct constant_fieldref_t constant_fieldref;
		struct constant_string_t constant_string;
	};
};

struct attribute_info_t
{
	unsigned short attribute_name_index;
	uint32_t attribute_length;
	uint8_t *info; // `attribute_length` number of elements
};

struct field_info_t
{
	unsigned short access_flags;
	unsigned short name_index;
	unsigned short descriptor_index;
	unsigned short attributes_count;
	struct attribute_info_t *attributes; // `attributes_count` number of elements
};

struct method_info_t
{
	unsigned short access_flags;
	unsigned short name_index;
	unsigned short descriptor_index;
	unsigned short attributes_count;
	struct attribute_info_t *attributes; // `attributes_count` number of elements
};

struct class_t
{
	uint32_t magic;
	unsigned short minor;
	unsigned short major;
	unsigned short constant_pool_count;
	struct cp_info_t *constant_pool; // `constant_pool_count` -1 number of elements
	unsigned short access_flags;
	unsigned short this_class;
	unsigned short super_class;
	unsigned short interfaces_count;
	unsigned short *interfaces; // `interface_count` number of elements
	unsigned short fields_count;
	struct field_info_t *fields; // `fields_count` number of elements
	unsigned short methods_count;
	struct method_info_t *methods; // `methods_count` number of elements
	unsigned short attribute_count;
	struct attribute_info_t *attributes; // `attribute_count` number of elements
};

struct class_t class; // the main class struct

void pretty_print()
{
	printf("magic                 : %x\n"
		   "minor                 : %d\n"
		   "major                 : %d\n"
		   "constant_pool_count   : %d\n"
		   "constant_pool         ->\n",
		   class.magic, class.minor, class.major, class.constant_pool_count);
	for (size_t i = 0; i < class.constant_pool_count - 1; i++)
	{
		printf("\ttag                   : %d\n", class.constant_pool[i].tag);
		if (class.constant_pool[i].tag == CONSTANT_Methodref)
		{
			printf("\tclass_index           : %d\n", class.constant_pool[i].constant_methodref.class_index);
			printf("\tname_and_type_index   : %d\n", class.constant_pool[i].constant_methodref.name_and_type_index);
		}
		else if (class.constant_pool[i].tag == CONSTANT_Utf8)
		{
			printf("\tlength                : %d\n", class.constant_pool[i].constant_utf8.length);
			printf("\tbytes                 : %s\n", class.constant_pool[i].constant_utf8.bytes);
		}
		else if (class.constant_pool[i].tag == CONSTANT_Class)
		{
			printf("\tname_index            : %d\n", class.constant_pool[i].constant_class.name_index);
		}
		else if (class.constant_pool[i].tag == CONSTANT_NameAndType)
		{
			printf("\tname_index            : %d\n", class.constant_pool[i].constant_name_and_type_info.name_index);
			printf("\tdescriptor_index      : %d\n", class.constant_pool[i].constant_name_and_type_info.descriptor_index);
		}
		else if (class.constant_pool[i].tag == CONSTANT_Fieldref)
		{
			printf("\tclass_index          : %d\n", class.constant_pool[i].constant_fieldref.class_index);
			printf("\tname_and_type_index  : %d\n", class.constant_pool[i].constant_fieldref.name_and_type_index);
		}
		else if (class.constant_pool[i].tag == CONSTANT_String)
		{
			printf("\tstring_index         : %d\n", class.constant_pool[i].constant_string.string_index);
		}
		else
		{
			printf("\tunknown tag value '%d'\n", class.constant_pool[i].tag);
		}
		printf("\t-------------------------\n");
	}
	printf("access_flags          : %d\n"
		   "this_class            : %d\n"
		   "super_class           : %d\n"
		   "interfaces_count      : %d\n"
		   "interfaces            ->\n",
		   class.access_flags, class.this_class, class.super_class, class.interfaces_count);
	for (size_t i = 0; i < class.interfaces_count; i++)
	{
		printf("interface count %zu    : %d\n", i, class.interfaces[i]);
	}
	printf("fields_count          : %d\n", class.fields_count);
	// TODO: Display fields
	for (size_t i = 0; i < class.fields_count; i++)
	{
	}
	printf("fields_count          : %d\n", class.fields_count);
}

unsigned short le_to_be(unsigned short bytes)
{
	unsigned short result = ((bytes & 0xFF) << 8) | ((bytes & 0xFF00) >> 8);
	return result;
}

void parse_file(FILE *file)
{
	// read headers from file
	fread(&class.magic, sizeof(uint32_t), 1, file);
	READ_U2(&class.minor);
	READ_U2(&class.major);
	READ_U2(&class.constant_pool_count);
	SWAP(class.constant_pool_count);

	class.constant_pool = ALLOC(struct cp_info_t, class.constant_pool_count - 1);
	for (size_t i = 0; i < class.constant_pool_count - 1; i++)
	{
		uint8_t tag;
		fread(&tag, sizeof(uint8_t), 1, file);
		switch (tag)
		{
		case CONSTANT_Class:
		{
			unsigned short name_index;
			READ_U2(&name_index);
			SWAP(name_index);

			struct cp_info_t cp_info = {
				.tag = tag,
				.constant_class = {
					.name_index = name_index}};

			class.constant_pool[i] = cp_info;
			break;
		}
		case CONSTANT_Fieldref:
		{
			unsigned short class_index, name_and_type_index;
			READ_U2(&class_index);
			READ_U2(&name_and_type_index);

			SWAP(class_index);
			SWAP(name_and_type_index);

			struct cp_info_t cp_info = {
				.tag = tag,
				.constant_fieldref = {
					.class_index = class_index,
					.name_and_type_index = name_and_type_index}};
			class.constant_pool[i] = cp_info;
			break;
		}
		case CONSTANT_Methodref:
		{
			unsigned short class_index, name_and_type_index;
			READ_U2(&class_index);
			READ_U2(&name_and_type_index);

			SWAP(class_index);
			SWAP(name_and_type_index);

			struct cp_info_t cp_info =
				{
					.tag = tag,
					.constant_methodref = {
						.class_index = class_index,
						.name_and_type_index = name_and_type_index}};
			class.constant_pool[i] = cp_info;
			break;
		}
		case CONSTANT_InterfaceMethodref:
		{
			printf("'CONSTANT_InterfaceMethodref' tag not yet implemented!\n");
			break;
		}
		case CONSTANT_String:
		{
			unsigned short string_index;
			READ_U2(&string_index);
			SWAP(string_index);

			struct cp_info_t cp_info = {
				.tag = tag,
				.constant_string = {
					.string_index = string_index}};
			class.constant_pool[i] = cp_info;
			break;
		}
		case CONSTANT_Integer:
		{
			printf("'CONSTANT_Integer' tag not yet implemented!\n");
			break;
		}
		case CONSTANT_Float:
		{
			printf("'CONSTANT_Float' tag not yet implemented!\n");
			break;
		}
		case CONSTANT_Long:
		{
			printf("'CONSTANT_Long' tag not yet implemented!\n");
			break;
		}
		case CONSTANT_Double:
		{
			printf("'CONSTANT_Double' tag not yet implemented!\n");
			break;
		}
		case CONSTANT_NameAndType:
		{
			unsigned short name_index, descriptor_index;
			READ_U2(&name_index);
			READ_U2(&descriptor_index);

			SWAP(name_index);
			SWAP(descriptor_index);

			struct cp_info_t cp_info = {
				.tag = tag,
				.constant_name_and_type_info = {
					.name_index = name_index,
					.descriptor_index = descriptor_index}};

			class.constant_pool[i] = cp_info;
			break;
		}
		case CONSTANT_Utf8:
		{
			unsigned short length;
			READ_U2(&length);
			SWAP(length);

			uint8_t *bytes = ALLOC(uint8_t, length);
			fread(bytes, sizeof(uint8_t), length, file);
			struct cp_info_t cp_info = {
				.tag = tag,
				.constant_utf8 = {
					.length = length,
					.bytes = bytes}};

			class.constant_pool[i] = cp_info;
			break;
		}
		case CONSTANT_MethodHandle:
		{
			printf("'CONSTANT_MethodHandle' tag not yet implemented!\n");
			break;
		}
		case CONSTANT_MethodType:
		{
			printf("'CONSTANT_MethodType' tag not yet implemented!\n");
			break;
		}
		case CONSTANT_InvokeDynamic:
		{
			printf("'CONSTANT_InvokeDynamic' tag not yet implemented!\n");
			break;
		}
		default:
			printf("Unexpected tag with value %d\n", tag);
		}
	}
	READ_U2(&class.access_flags);
	READ_U2(&class.this_class);
	READ_U2(&class.super_class);
	READ_U2(&class.interfaces_count);
	SWAP(class.interfaces_count);

	class.interfaces = ALLOC(unsigned short, class.interfaces_count);

	for (size_t i = 0; i < class.interfaces_count; i++)
	{
		unsigned short interface;
		READ_U2(&interface);
		class.interfaces[i] = interface;
	}

	READ_U2(&class.fields_count);
	SWAP(class.fields_count);
	class.fields_count = le_to_be(class.fields_count);
	for (size_t i = 0; i < class.fields_count; i++)
	{
		unsigned short access_flags, name_index, descriptor_index, attributes_count;
		READ_U2(&access_flags);
		READ_U2(&name_index);
		READ_U2(&descriptor_index);
		READ_U2(&attributes_count);

		SWAP(access_flags);
		SWAP(name_index);
		SWAP(descriptor_index);
		SWAP(attributes_count);
		struct attribute_info_t *attributes = ALLOC(struct attribute_info_t, attributes_count);
		for (size_t k = 0; k < attributes_count; k++)
		{
			unsigned short name_index;
			uint32_t attribute_length;
			READ_U2(&name_index);
			fread(&attribute_length, sizeof(uint32_t), 1, file);

			name_index = le_to_be(name_index);
			attribute_length = __builtin_bswap32(attribute_length);
			unsigned char *bytes = (unsigned char *)malloc(sizeof(unsigned char) * attribute_length);
			fread(bytes, sizeof(unsigned char), attribute_length, file);

			struct attribute_info_t attribute = {
				.attribute_name_index = name_index,
				.attribute_length = attribute_length,
				.info = bytes};

			attributes[k] = attribute;
		}

		// TODO: complete this by using a class file of a real java codebase file
		printf("access_flags     :: %x, %d\n", access_flags, access_flags);
		printf("name_index       :: %x, %d\n", name_index, name_index);
		printf("descriptor_index :: %x, %d\n", descriptor_index, descriptor_index);
		printf("attribute_count  :: %x, %d\n", attributes_count, attributes_count);
	}

	READ_U2(&class.methods_count);
	SWAP(class.methods_count);
	class.methods = ALLOC(struct method_info_t, class.methods_count);
	for (size_t i = 0; i < class.methods_count; i++)
	{
		unsigned short access_flags, name_index, descriptor_index, attributes_count;
		READ_U2(&access_flags);
		READ_U2(&name_index);
		READ_U2(&descriptor_index);
		READ_U2(&attributes_count);

		SWAP(access_flags);
		SWAP(name_index);
		SWAP(descriptor_index);
		SWAP(attributes_count);
		struct attribute_info_t *attributes = ALLOC(struct attribute_info_t, attributes_count);

		for (size_t k = 0; k < attributes_count; k++)
		{
			unsigned short name_index;
			uint32_t attribute_length;
			READ_U2(&name_index);
			fread(&attribute_length, sizeof(uint32_t), 1, file);

			name_index = le_to_be(name_index);
			attribute_length = __builtin_bswap32(attribute_length);
			unsigned char *bytes = ALLOC(unsigned char, attribute_length);
			fread(bytes, sizeof(unsigned char), attribute_length, file);

			struct attribute_info_t attribute = {
				.attribute_name_index = name_index,
				.attribute_length = attribute_length,
				.info = bytes};

			attributes[k] = attribute;
		}

		struct method_info_t method =
			{
				.access_flags = access_flags,
				.name_index = name_index,
				.descriptor_index = descriptor_index,
				.attributes_count = attributes_count,
				.attributes = attributes};

		class.methods[i] = method;
	}

	READ_U2(&class.attribute_count);
	SWAP(class.attribute_count);
	class.attributes = ALLOC(struct attribute_info_t, class.attribute_count);
	for (size_t i = 0; i < class.attribute_count; i++)
	{
		unsigned short name_index;
		uint32_t attribute_length;
		fread(&name_index, sizeof(unsigned short), 1, file);
		fread(&attribute_length, sizeof(uint32_t), 1, file);

		SWAP(name_index);
		attribute_length = __builtin_bswap32(attribute_length);
		unsigned char *bytes = ALLOC(unsigned char, attribute_length);
		fread(bytes, sizeof(unsigned char), attribute_length, file);

		struct attribute_info_t attribute = {
			.attribute_name_index = name_index,
			.attribute_length = attribute_length,
			.info = bytes};

		class.attributes[i] = attribute;
	}

	// convert them from little-endian to big-endian
	class.magic = __builtin_bswap32(class.magic);
	SWAP(class.minor);
	SWAP(class.major);
	SWAP(class.access_flags);
	SWAP(class.this_class);
	SWAP(class.super_class);
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage : %s <filename>\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE *file = fopen(argv[1], "rb");
	if (file == NULL)
	{
		printf("[-] couldn't open file 'Main.class'\n");
		return EXIT_FAILURE;
	}

	// parse the file and map it into the `class_t` struct
	parse_file(file);
	pretty_print();

	// printf("magic       		: %x\n", class.magic);
	// printf("minor       		: %x, %d\n", class.minor, class.minor);
	// printf("major       		: %x, %d\n", class.major, class.major);
	// printf("constant_pool_count	: %x, %d\n", class.constant_pool_count, class.constant_pool_count);
	// printf("access_flags   		: %x, %d\n", class.access_flags, class.access_flags);
	// printf("this_class     		: %x, %d\n", class.this_class, class.this_class);
	// printf("super_class   		: %x, %d\n", class.super_class, class.super_class);
	// printf("interfaces_count	: %x, %d\n", class.interfaces_count, class.interfaces_count);

	return EXIT_SUCCESS;
}
