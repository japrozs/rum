# Rum

Rum is a somewhat-spec compliant parser for the Java Class File Format that is capable of parsing a Java Class file generate by a simple hello world program.


> `PLEASE DON'T READ THE CODE IF YOU VALUE YOUR MENTAL HEALTH. THE CODE IS EXTREMELY EXTREMELY BAD, ALTHOUGH FUNCTIONAL ;)`

## Usage
To build `rum`, run the following commands:
```bash
git clone https://github.com/japrozs/rum.git
cd rum
./make.sh
./out/rum <class file>
```

to run test, on the [sample files](./samples), run the following commands:
```bash
./make.sh
./test.sh
```

running `rum` on a [hello-world program](./samples/Main.java) yields the following results
```sh
magic                 : cafebabe
minor                 : 0
major                 : 61
constant_pool_count   : 29
attribute_count       : 1
access_flags          : 33
this_class            : 21
super_class           : 2
interfaces_count      : 0
fields_count          : 0
methods_count         : 2
interfaces           -> []
constant_pool        -> 
	tag                   : CONSTANT_Methodref
	class_index           : 2
	name_and_type_index   : 3
	-------------------------
	tag                   : CONSTANT_Class
	name_index            : 4
	-------------------------
	tag                   : CONSTANT_NameAndType
	name_index            : 5
	descriptor_index      : 6
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 16
	bytes                 : "java/lang/Object<init>"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 6
	bytes                 : "<init>"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 3
	bytes                 : "()V"
	-------------------------
	tag                   : CONSTANT_Fieldref
	class_index          : 8
	name_and_type_index  : 9
	-------------------------
	tag                   : CONSTANT_Class
	name_index            : 10
	-------------------------
	tag                   : CONSTANT_NameAndType
	name_index            : 11
	descriptor_index      : 12
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 16
	bytes                 : "java/lang/Systemout"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 3
	bytes                 : "out"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 21
	bytes                 : "Ljava/io/PrintStream;"
	-------------------------
	tag                   : CONSTANT_String
	string_index         : 14
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 16
	bytes                 : "hello, universe!java/io/PrintStream"
	-------------------------
	tag                   : CONSTANT_Methodref
	class_index           : 16
	name_and_type_index   : 17
	-------------------------
	tag                   : CONSTANT_Class
	name_index            : 18
	-------------------------
	tag                   : CONSTANT_NameAndType
	name_index            : 19
	descriptor_index      : 20
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 19
	bytes                 : "java/io/PrintStream"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 7
	bytes                 : "println"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 21
	bytes                 : "(Ljava/lang/String;)V"
	-------------------------
	tag                   : CONSTANT_Class
	name_index            : 22
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 4
	bytes                 : "Main"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 4
	bytes                 : "Code"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 15
	bytes                 : "LineNumberTable"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 4
	bytes                 : "main"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 22
	bytes                 : "([Ljava/lang/String;)V"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 10
	bytes                 : "SourceFile"
	-------------------------
	tag                   : CONSTANT_Utf8
	length                : 9
	bytes                 : "Main.java"
	-------------------------
fields                -> []
methods               -> 
	access_flags          : ACC_PUBLIC
	name_index            : 5
	descriptor_index      : 6
	attributes_count      : 1
	attributes            -> 
		attribute_name_index    : 23
		attribute_length        : 29
		info                    : ""
		----------------------------
	access_flags          : ACC_PUBLIC, ACC_STATIC
	name_index            : 25
	descriptor_index      : 26
	attributes_count      : 1
	attributes            -> 
		attribute_name_index    : 23
		attribute_length        : 37
		info                    : ""
		----------------------------
attributes            -> 
	attribute_name_index    : 27
	attribute_length        : 2
	info                    : ""
	----------------------------
```

## Credits
All the sample Java files for testing purposes have been copied directly from [William Fiset's DSA repository](https://github.com/williamfiset/DEPRECATED-data-structures)

## Reference
[`Java Class File Format`](https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html)