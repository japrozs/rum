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

unsigned short le_to_be(unsigned short bytes)
{
	unsigned short result = ((bytes & 0xFF) << 8) | ((bytes & 0xFF00) >> 8);
	return result;
}

void parse_file(FILE *file)
{
	// read headers from file
	fread(&class.magic, sizeof(uint32_t), 1, file);
	fread(&class.minor, sizeof(unsigned short), 1, file);
	fread(&class.major, sizeof(unsigned short), 1, file);
	fread(&class.constant_pool_count, sizeof(unsigned short), 1, file);
	class.constant_pool_count = le_to_be(class.constant_pool_count);

	class.constant_pool = (struct cp_info_t *)malloc(sizeof(struct cp_info_t) * (class.constant_pool_count - 1));
	for (size_t i = 0; i < class.constant_pool_count - 1; i++)
	{
		uint8_t tag;
		fread(&tag, sizeof(uint8_t), 1, file);
		switch (tag)
		{
		case CONSTANT_Class:
		{
			unsigned short name_index;
			fread(&name_index, sizeof(unsigned short), 1, file);
			name_index = le_to_be(name_index);

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
			fread(&class_index, sizeof(unsigned short), 1, file);
			fread(&name_and_type_index, sizeof(unsigned short), 1, file);

			class_index = le_to_be(class_index);
			name_and_type_index = le_to_be(name_and_type_index);
			break;
		}
		case CONSTANT_Methodref:
		{
			unsigned short class_index, name_and_type_index;
			fread(&class_index, sizeof(unsigned short), 1, file);
			fread(&name_and_type_index, sizeof(unsigned short), 1, file);
			class_index = le_to_be(class_index);
			name_and_type_index = le_to_be(name_and_type_index);

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
			fread(&string_index, sizeof(unsigned short), 1, file);
			string_index = le_to_be(string_index);

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
			fread(&name_index, sizeof(unsigned short), 1, file);
			fread(&descriptor_index, sizeof(unsigned short), 1, file);

			name_index = le_to_be(name_index);
			descriptor_index = le_to_be(descriptor_index);

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
			fread(&length, sizeof(unsigned short), 1, file);
			length = le_to_be(length);

			uint8_t *bytes = (uint8_t *)malloc(sizeof(uint8_t) * length);
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
	fread(&class.access_flags, sizeof(unsigned short), 1, file);
	fread(&class.this_class, sizeof(unsigned short), 1, file);
	fread(&class.super_class, sizeof(unsigned short), 1, file);
	fread(&class.interfaces_count, sizeof(unsigned short), 1, file);
	class.interfaces_count = le_to_be(class.interfaces_count);

	class.interfaces = (unsigned short *)malloc(sizeof(unsigned short) * class.interfaces_count);
	for (size_t i = 0; i < class.interfaces_count; i++)
	{
		unsigned short interface;
		fread(&interface, sizeof(unsigned short), 1, file);
		class.interfaces[i] = interface;
	}

	fread(&class.fields_count, sizeof(unsigned short), 1, file);
	class.fields_count = le_to_be(class.fields_count);
	for (size_t i = 0; i < class.fields_count; i++)
	{
		unsigned short access_flags, name_index, descriptor_index, attributes_count;
		fread(&access_flags, sizeof(unsigned short), 1, file);
		fread(&name_index, sizeof(unsigned short), 1, file);
		fread(&descriptor_index, sizeof(unsigned short), 1, file);
		fread(&attributes_count, sizeof(unsigned short), 1, file);

		access_flags = le_to_be(access_flags);
		name_index = le_to_be(name_index);
		descriptor_index = le_to_be(descriptor_index);
		attributes_count = le_to_be(attributes_count);
		struct attribute_info_t *attributes = malloc(sizeof(struct attribute_info_t) * attributes_count);
		for (size_t k = 0; k < attributes_count; k++)
		{
			unsigned short name_index;
			uint32_t attribute_length;
			fread(&name_index, sizeof(unsigned short), 1, file);
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

	fread(&class.methods_count, sizeof(unsigned short), 1, file);
	class.methods_count = le_to_be(class.methods_count);
	class.methods = (struct method_info_t *)malloc(sizeof(struct method_info_t) * class.methods_count);
	for (size_t i = 0; i < class.methods_count; i++)
	{
		unsigned short access_flags, name_index, descriptor_index, attributes_count;
		fread(&access_flags, sizeof(unsigned short), 1, file);
		fread(&name_index, sizeof(unsigned short), 1, file);
		fread(&descriptor_index, sizeof(unsigned short), 1, file);
		fread(&attributes_count, sizeof(unsigned short), 1, file);

		access_flags = le_to_be(access_flags);
		name_index = le_to_be(name_index);
		descriptor_index = le_to_be(descriptor_index);
		attributes_count = le_to_be(attributes_count);
		struct attribute_info_t *attributes = (struct attribute_info_t *)malloc(sizeof(struct attribute_info_t) * attributes_count);

		for (size_t k = 0; k < attributes_count; k++)
		{
			unsigned short name_index;
			uint32_t attribute_length;
			fread(&name_index, sizeof(unsigned short), 1, file);
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

		struct method_info_t method =
			{
				.access_flags = access_flags,
				.name_index = name_index,
				.descriptor_index = descriptor_index,
				.attributes_count = attributes_count,
				.attributes = attributes};

		class.methods[i] = method;
	}

	fread(&class.attribute_count, sizeof(unsigned short), 1, file);
	class.attribute_count = le_to_be(class.attribute_count);
	class.attributes = (struct attribute_info_t *)malloc(sizeof(struct attribute_info_t) * class.attribute_count);
	for (size_t i = 0; i < class.attribute_count; i++)
	{
		unsigned short name_index;
		uint32_t attribute_length;
		fread(&name_index, sizeof(unsigned short), 1, file);
		fread(&attribute_length, sizeof(uint32_t), 1, file);

		name_index = le_to_be(name_index);
		attribute_length = __builtin_bswap32(attribute_length);
		unsigned char *bytes = (unsigned char *)malloc(sizeof(unsigned char) * attribute_length);
		fread(bytes, sizeof(unsigned char), attribute_length, file);

		struct attribute_info_t attribute = {
			.attribute_name_index = name_index,
			.attribute_length = attribute_length,
			.info = bytes};

		class.attributes[i] = attribute;
	}

	// convert them from little-endian to big-endian
	class.magic = __builtin_bswap32(class.magic);
	class.minor = le_to_be(class.minor);
	class.major = le_to_be(class.major);
	class.access_flags = le_to_be(class.access_flags);
	class.this_class = le_to_be(class.this_class);
	class.super_class = le_to_be(class.super_class);
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

	printf("magic       		: %x\n", class.magic);
	printf("minor       		: %x, %d\n", class.minor, class.minor);
	printf("major       		: %x, %d\n", class.major, class.major);
	printf("constant_pool_count	: %x, %d\n", class.constant_pool_count, class.constant_pool_count);
	printf("access_flags   		: %x, %d\n", class.access_flags, class.access_flags);
	printf("this_class     		: %x, %d\n", class.this_class, class.this_class);
	printf("super_class   		: %x, %d\n", class.super_class, class.super_class);
	printf("interfaces_count	: %x, %d\n", class.interfaces_count, class.interfaces_count);

	return EXIT_SUCCESS;
}
