#! /bin/sh

for file in samples/*.class; do
	./out/rum $file > /dev/null
	if [ $? -eq 0 ]; then
		echo ✅ $file
	else
		echo ❌ $file
	fi
done;