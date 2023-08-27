/*
 * Copyright (c) 2023-Present, Japroz Singh Saini <japrozsaini@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// method_info access_flags mask table
#define METHOD_INFO_ACC_PUBLIC 0x0001
#define METHOD_INFO_ACC_PRIVATE 0x0002
#define METHOD_INFO_ACC_PROTECTED 0x0004
#define METHOD_INFO_ACC_STATIC 0x0008
#define METHOD_INFO_ACC_FINAL 0x0010
#define METHOD_INFO_ACC_SYNCHRONIZED 0x0020
#define METHOD_INFO_ACC_BRIDGE 0x0040
#define METHOD_INFO_ACC_VARARGS 0x0080
#define METHOD_INFO_ACC_NATIVE 0x0100
#define METHOD_INFO_ACC_ABSTRACT 0x0400
#define METHOD_INFO_ACC_STRICT 0x0800
#define METHOD_INFO_ACC_SYNTHETIC 0x1000

// class file access_flags mask table
#define ACC_PUBLIC 0x0001
#define ACC_FINAL 0x0010
#define ACC_SUPER 0x0020
#define ACC_INTERFACE 0x0200
#define ACC_ABSTRACT 0x0400
#define ACC_SYNTHETIC 0x1000
#define ACC_ANNOTATION 0x2000
#define ACC_ENUM 0x4000

// field_info access_flags mask table
#define FIELD_INFO_ACC_PUBLIC 0x0001
#define FIELD_INFO_ACC_PRIVATE 0x0002
#define FIELD_INFO_ACC_PROTECTED 0x0004
#define FIELD_INFO_ACC_STATIC 0x0008
#define FIELD_INFO_ACC_FINAL 0x0010
#define FIELD_INFO_ACC_VOLATILE 0x0040
#define FIELD_INFO_ACC_TRANSIENT 0x0080
#define FIELD_INFO_ACC_SYNTHETIC 0x1000
#define FIELD_INFO_ACC_ENUM 0x4000

// helpful macros
#define READ_U2(var) fread(var, sizeof(unsigned short), 1, file)
#define SWAP(var) var = le_to_be(var) // swap the endian-ness of an unsigned short
#define ALLOC(type, count) (type *)malloc(sizeof(type) * (count))
#define PRINT_FLAG(str, flag) sprintf(str, "%s%s" flag, str, strlen(str) == 0 ? "" : ", ");

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

struct constant_interface_methodref_t
{
    unsigned short class_index;
    unsigned short name_and_type_index;
};

struct constant_invoke_dynamic_t
{
    unsigned short bootstrap_method_attr_index;
    unsigned short name_and_type_index;
};

struct constant_method_handle_t
{
    uint8_t reference_kind;
    unsigned short reference_index;
};

struct constant_float_t
{
    uint32_t bytes;
};

struct constant_integer_t
{
    uint32_t bytes;
};

struct constant_double_t
{
    uint32_t high_bytes;
    uint32_t low_bytes;
};

struct constant_long_t
{
    uint32_t high_bytes;
    uint32_t low_bytes;
};

struct constant_method_type_t
{
    unsigned short descriptor_index;
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
        struct constant_interface_methodref_t constant_interface_methodref;
        struct constant_invoke_dynamic_t constant_invoke_dynamic;
        struct constant_method_handle_t constant_method_handle;
        struct constant_float_t constant_float;
        struct constant_integer_t constant_integer;
        struct constant_double_t constant_double;
        struct constant_long_t constant_long;
        struct constant_method_type_t constant_method_type;
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

const char *get_tag_name(uint8_t tag)
{
    switch (tag)
    {
    case CONSTANT_Class:
        return "CONSTANT_Class";
    case CONSTANT_Fieldref:
        return "CONSTANT_Fieldref";
    case CONSTANT_Methodref:
        return "CONSTANT_Methodref";
    case CONSTANT_InterfaceMethodref:
        return "CONSTANT_InterfaceMethodref";
    case CONSTANT_String:
        return "CONSTANT_String";
    case CONSTANT_Integer:
        return "CONSTANT_Integer";
    case CONSTANT_Float:
        return "CONSTANT_Float";
    case CONSTANT_Long:
        return "CONSTANT_Long";
    case CONSTANT_Double:
        return "CONSTANT_Double";
    case CONSTANT_NameAndType:
        return "CONSTANT_NameAndType";
    case CONSTANT_Utf8:
        return "CONSTANT_Utf8";
    case CONSTANT_MethodHandle:
        return "CONSTANT_MethodHandle";
    case CONSTANT_MethodType:
        return "CONSTANT_MethodType";
    case CONSTANT_InvokeDynamic:
        return "CONSTANT_InvokeDynamic";
    default:
        return "<unknown tag>";
    }
}

char *get_access_flags(unsigned short flag)
{
    char *ret = malloc(sizeof(char) * 256);
    if (flag & ACC_PUBLIC)
    {
        PRINT_FLAG(ret, "ACC_PUBLIC");
    }
    if (flag & ACC_FINAL)
    {
        PRINT_FLAG(ret, "ACC_FINAL");
    }
    if (flag & ACC_SUPER)
    {
        PRINT_FLAG(ret, "ACC_SUPER");
    }
    if (flag & ACC_INTERFACE)
    {
        PRINT_FLAG(ret, "ACC_INTERFACE");
    }
    if (flag & ACC_ABSTRACT)
    {
        PRINT_FLAG(ret, "ACC_ABSTRACT");
    }
    if (flag & ACC_SYNTHETIC)
    {
        PRINT_FLAG(ret, "ACC_SYNTHETIC");
    }
    if (flag & ACC_ANNOTATION)
    {
        PRINT_FLAG(ret, "ACC_ANNOTATION");
    }
    if (flag & ACC_ENUM)
    {
        PRINT_FLAG(ret, "ACC_ENUM");
    }
    return strlen(ret) == 0 ? "<unknown flag>" : ret;
}

char *get_field_info_access_flags(unsigned short flag)
{
    char *ret = malloc(sizeof(char) * 256);

    if (flag & FIELD_INFO_ACC_PUBLIC)
    {
        PRINT_FLAG(ret, "ACC_PUBLIC");
    }
    if (flag & FIELD_INFO_ACC_PRIVATE)
    {
        PRINT_FLAG(ret, "ACC_PRIVATE");
    }
    if (flag & FIELD_INFO_ACC_PROTECTED)
    {
        PRINT_FLAG(ret, "ACC_PROTECTED");
    }
    if (flag & FIELD_INFO_ACC_STATIC)
    {
        PRINT_FLAG(ret, "ACC_STATIC");
    }
    if (flag & FIELD_INFO_ACC_FINAL)
    {
        PRINT_FLAG(ret, "ACC_FINAL");
    }
    if (flag & FIELD_INFO_ACC_VOLATILE)
    {
        PRINT_FLAG(ret, "ACC_VOLATILE");
    }
    if (flag & FIELD_INFO_ACC_TRANSIENT)
    {
        PRINT_FLAG(ret, "ACC_TRANSIENT");
    }
    if (flag & FIELD_INFO_ACC_SYNTHETIC)
    {
        PRINT_FLAG(ret, "ACC_SYNTHETIC");
    }
    if (flag & FIELD_INFO_ACC_ENUM)
    {
        PRINT_FLAG(ret, "ACC_ENUM");
    }
    return strlen(ret) == 0 ? "<unknown flag>" : ret;
}

char *get_method_info_access_flags(unsigned short flag)
{
    char *ret = malloc(sizeof(char) * 256);

    if (flag & METHOD_INFO_ACC_PUBLIC)
    {
        PRINT_FLAG(ret, "ACC_PUBLIC");
    }
    if (flag & METHOD_INFO_ACC_PRIVATE)
    {
        PRINT_FLAG(ret, "ACC_PRIVATE");
    }
    if (flag & METHOD_INFO_ACC_PROTECTED)
    {
        PRINT_FLAG(ret, "ACC_PROTECTED");
    }
    if (flag & METHOD_INFO_ACC_STATIC)
    {
        PRINT_FLAG(ret, "ACC_STATIC");
    }
    if (flag & METHOD_INFO_ACC_FINAL)
    {
        PRINT_FLAG(ret, "ACC_FINAL");
    }
    if (flag & METHOD_INFO_ACC_SYNCHRONIZED)
    {
        PRINT_FLAG(ret, "ACC_SYNCHRONIZED");
    }
    if (flag & METHOD_INFO_ACC_BRIDGE)
    {
        PRINT_FLAG(ret, "ACC_BRIDGE");
    }
    if (flag & METHOD_INFO_ACC_VARARGS)
    {
        PRINT_FLAG(ret, "ACC_VARARGS");
    }
    if (flag & METHOD_INFO_ACC_NATIVE)
    {
        PRINT_FLAG(ret, "ACC_NATIVE");
    }
    if (flag & METHOD_INFO_ACC_ABSTRACT)
    {
        PRINT_FLAG(ret, "ACC_ABSTRACT");
    }
    if (flag & METHOD_INFO_ACC_STRICT)
    {
        PRINT_FLAG(ret, "ACC_STRICT");
    }
    if (flag & METHOD_INFO_ACC_SYNTHETIC)
    {
        PRINT_FLAG(ret, "ACC_SYNTHETIC");
    }
    return strlen(ret) == 0 ? "<unknown flag>" : ret;
}

void cleanup(void)
{
    for (int i = 0; i < class.constant_pool_count - 1; i++)
    {
        if (class.constant_pool[i].tag == CONSTANT_Utf8)
        {
            free(class.constant_pool[i].constant_utf8.bytes);
        }
    }
    free(class.constant_pool);
    free(class.interfaces);
    for (int i = 0; i < class.fields_count; i++)
    {
        for (int j = 0; j < class.fields[i].attributes_count; j++)
        {
            free(class.fields[i].attributes[j].info);
        }
        free(class.fields[i].attributes);
    }
    free(class.fields);

    for (int i = 0; i < class.methods_count; i++)
    {
        for (int j = 0; j < class.methods[i].attributes_count; j++)
        {
            free(class.methods[i].attributes[j].info);
        }
        free(class.methods[i].attributes);
    }
    free(class.methods);
    for (int i = 0; i < class.attribute_count; i++)
    {
        free(class.attributes[i].info);
    }
    free(class.attributes);
}

void pretty_print(void)
{
    printf("magic                 : 0x%x\n"
           "minor                 : %d\n"
           "major                 : %d\n"
           "constant_pool_count   : %d\n",
           class.magic, class.minor, class.major, class.constant_pool_count);
    printf("attribute_count       : %d\n", class.attribute_count);
    printf("access_flags          : 0x%x – %s\n"
           "this_class            : %d\n"
           "super_class           : %d\n"
           "interfaces_count      : %d\n",
           class.access_flags, get_access_flags(class.access_flags),
           class.this_class, class.super_class, class.interfaces_count);
    printf("fields_count          : %d\n", class.fields_count);
    printf("methods_count         : %d\n", class.methods_count);
    printf("interfaces           -> %s", class.interfaces_count == 0 ? "[]\n" : "\n");
    printf("constant_pool        -> %s", class.constant_pool_count == 0 ? "[]\n" : "\n");
    for (size_t i = 0; i < class.constant_pool_count - 1; i++)
    {
        printf("\ttag                   : %s\n", get_tag_name(class.constant_pool[i].tag));
        if (class.constant_pool[i].tag == CONSTANT_Methodref)
        {
            printf("\tclass_index           : %d\n", class.constant_pool[i].constant_methodref.class_index);
            printf("\tname_and_type_index   : %d\n", class.constant_pool[i].constant_methodref.name_and_type_index);
        }
        else if (class.constant_pool[i].tag == CONSTANT_Utf8)
        {
            printf("\tlength                : %d\n", class.constant_pool[i].constant_utf8.length);
            printf("\tbytes                 : \"%s\"\n", class.constant_pool[i].constant_utf8.bytes);
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
        else if (class.constant_pool[i].tag == CONSTANT_InterfaceMethodref)
        {
            printf("\tclass_index          : %d\n", class.constant_pool[i].constant_interface_methodref.class_index);
            printf("\tname_and_type_index  : %d\n", class.constant_pool[i].constant_interface_methodref.name_and_type_index);
        }
        else if (class.constant_pool[i].tag == CONSTANT_InvokeDynamic)
        {
            printf("\tbootstrap_method_attr_index : %d\n", class.constant_pool[i].constant_invoke_dynamic.bootstrap_method_attr_index);
            printf("\tname_and_type_index.        : %d\n", class.constant_pool[i].constant_invoke_dynamic.name_and_type_index);
        }
        else if (class.constant_pool[i].tag == CONSTANT_MethodHandle)
        {
            printf("\treference_kind       : %d\n", class.constant_pool[i].constant_method_handle.reference_kind);
            printf("\treference_index      : %d\n", class.constant_pool[i].constant_method_handle.reference_index);
        }
        else if (class.constant_pool[i].tag == CONSTANT_Float)
        {
            printf("\tbytes                : %d\n", class.constant_pool[i].constant_float.bytes);
        }
        else if (class.constant_pool[i].tag == CONSTANT_Integer)
        {
            printf("\tbytes                : %d\n", class.constant_pool[i].constant_integer.bytes);
        }
        else if (class.constant_pool[i].tag == CONSTANT_Double)
        {
            printf("\thigh_bytes           : 0x%.8x\n", class.constant_pool[i].constant_double.high_bytes);
            printf("\tlow_bytes            : 0x%.8x\n", class.constant_pool[i].constant_double.low_bytes);
        }
        else if (class.constant_pool[i].tag == CONSTANT_Long)
        {
            printf("\thigh_bytes           : 0x%.8x\n", class.constant_pool[i].constant_long.high_bytes);
            printf("\tlow_bytes            : 0x%.8x\n", class.constant_pool[i].constant_long.low_bytes);
        }
        else if (class.constant_pool[i].tag == CONSTANT_MethodType)
        {
            printf("\tdescriptor_index     : %d\n", class.constant_pool[i].constant_method_type.descriptor_index);
        }
        else
        {
            printf("\tunknown tag value '%d'\n", class.constant_pool[i].tag);
        }
        printf("\t-------------------------\n");
    }
    for (size_t i = 0; i < class.interfaces_count; i++)
    {
        printf("interface count %zu    : %d\n", i, class.interfaces[i]);
    }
    printf("fields                -> %s", class.fields_count == 0 ? "[]\n" : "\n");
    for (size_t i = 0; i < class.fields_count; i++)
    {
        printf("\taccess_flags          : 0x%.4x – %s\n"
               "\tname_index            : %d\n"
               "\tdescriptor_index      : %d\n"
               "\tattributes_count      : %d\n",
               class.fields[i].access_flags, get_field_info_access_flags(class.fields[i].access_flags), class.fields[i].name_index, class.fields[i].descriptor_index, class.fields[i].attributes_count);
        printf("\tattributes            -> %s", class.fields[i].attributes_count == 0 ? "[]\n" : "\n");
        for (size_t k = 0; k < class.fields[i].attributes_count; k++)
        {
            printf("\t\tattribute_name_index    : %d\n"
                   "\t\tattribute_length        : %d\n"
                   "\t\tinfo                    : \"%s\"\n",
                   class.fields[i].attributes[k].attribute_name_index, class.fields[i].attributes[k].attribute_length, class.fields[i].attributes[k].info);
        }
        printf("\t\t----------------------------\n");
    }
    printf("methods               -> %s", class.methods_count == 0 ? "[]\n" : "\n");
    for (size_t i = 0; i < class.methods_count; i++)
    {
        printf("\taccess_flags          : 0x%.4x – %s\n"
               "\tname_index            : %d\n"
               "\tdescriptor_index      : %d\n"
               "\tattributes_count      : %d\n",
               class.methods[i].access_flags, get_method_info_access_flags(class.methods[i].access_flags), class.methods[i].name_index, class.methods[i].descriptor_index, class.methods[i].attributes_count);
        printf("\tattributes            -> %s", class.methods[i].attributes_count == 0 ? "[]\n" : "\n");
        for (size_t k = 0; k < class.methods[i].attributes_count; k++)
        {
            printf("\t\tattribute_name_index    : %d\n"
                   "\t\tattribute_length        : %d\n"
                   "\t\tinfo                    : \"%s\"\n",
                   class.methods[i].attributes[k].attribute_name_index, class.methods[i].attributes[k].attribute_length, class.methods[i].attributes[k].info);
        }
        printf("\t\t----------------------------\n");
    }
    printf("attributes            -> %s", class.attribute_count == 0 ? "[]\n" : "\n");
    for (size_t i = 0; i < class.attribute_count; i++)
    {
        printf("\tattribute_name_index    : %d\n"
               "\tattribute_length        : %d\n"
               "\tinfo                    : \"%s\"\n",
               class.attributes[i].attribute_name_index, class.attributes[i].attribute_length, class.attributes[i].info);
        printf("\t----------------------------\n");
    }
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

        printf("%zu : %s\n", i, get_tag_name(tag));
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

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_methodref = {
                    .class_index = class_index,
                    .name_and_type_index = name_and_type_index}};
            class.constant_pool[i] = cp_info;
            break;
        }
        case CONSTANT_InterfaceMethodref:
        {
            unsigned short class_index, name_and_type_index;
            READ_U2(&class_index);
            READ_U2(&name_and_type_index);

            SWAP(class_index);
            SWAP(name_and_type_index);

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_interface_methodref = {
                    .class_index = class_index,
                    .name_and_type_index = name_and_type_index}};
            class.constant_pool[i] = cp_info;
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
            uint32_t bytes;
            fread(&bytes, sizeof(uint32_t), 1, file);
            bytes = __builtin_bswap32(bytes);

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_integer = {
                    .bytes = bytes}};

            class.constant_pool[i] = cp_info;
            break;
        }
        case CONSTANT_Float:
        {
            uint32_t bytes;
            fread(&bytes, sizeof(uint32_t), 1, file);
            bytes = __builtin_bswap32(bytes);

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_float = {
                    .bytes = bytes}};

            class.constant_pool[i] = cp_info;
            break;
        }
        case CONSTANT_Long:
        {
            uint32_t high_bytes, low_bytes;
            fread(&high_bytes, sizeof(uint32_t), 1, file);
            fread(&low_bytes, sizeof(uint32_t), 1, file);
            high_bytes = __builtin_bswap32(high_bytes);
            low_bytes = __builtin_bswap32(low_bytes);

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_long = {
                    .high_bytes = high_bytes,
                    .low_bytes = low_bytes}};
            class.constant_pool[i] = cp_info;
            i++;
            break;
        }
        case CONSTANT_Double:
        {
            uint32_t high_bytes, low_bytes;
            fread(&high_bytes, sizeof(uint32_t), 1, file);
            fread(&low_bytes, sizeof(uint32_t), 1, file);
            high_bytes = __builtin_bswap32(high_bytes);
            low_bytes = __builtin_bswap32(low_bytes);

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_double = {
                    .high_bytes = high_bytes,
                    .low_bytes = low_bytes}};

            class.constant_pool[i] = cp_info;
            i++;
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
            uint8_t reference_kind;
            unsigned short reference_index;

            fread(&reference_kind, sizeof(uint8_t), 1, file);
            READ_U2(&reference_index);
            SWAP(reference_index);

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_method_handle = {
                    .reference_kind = reference_kind,
                    .reference_index = reference_index}};

            class.constant_pool[i] = cp_info;
            break;
        }
        case CONSTANT_MethodType:
        {
            unsigned short descriptor_index;
            READ_U2(&descriptor_index);
            SWAP(descriptor_index);

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_method_type = {
                    .descriptor_index = descriptor_index}};

            class.constant_pool[i] = cp_info;
            break;
        }
        case CONSTANT_InvokeDynamic:
        {
            unsigned short bootstrap_method_attr_index, name_and_type_index;
            READ_U2(&bootstrap_method_attr_index);
            READ_U2(&name_and_type_index);

            SWAP(bootstrap_method_attr_index);
            SWAP(name_and_type_index);

            struct cp_info_t cp_info = {
                .tag = tag,
                .constant_invoke_dynamic = {
                    .bootstrap_method_attr_index = bootstrap_method_attr_index,
                    .name_and_type_index = name_and_type_index}};
            class.constant_pool[i] = cp_info;
            break;
        }
        default:
            printf("Unexpected tag with value %d\n", tag);
            exit(EXIT_FAILURE);
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
    class.fields = ALLOC(struct field_info_t, class.fields_count);
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

        struct field_info_t field_info = {
            .access_flags = access_flags,
            .name_index = name_index,
            .descriptor_index = descriptor_index,
            .attributes_count = attributes_count,
            .attributes = attributes};

        class.fields[i] = field_info;
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

        struct method_info_t method = {
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
        READ_U2(&name_index);
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
        printf("[-] couldn't open file '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    // parse the file and map it into the `class_t` struct
    parse_file(file);
    pretty_print();
    cleanup();
    fclose(file);

    return EXIT_SUCCESS;
}
