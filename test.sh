#! /bin/sh

for file in samples/*; do
	./out/rum $file > /dev/null
	if [ $? -eq 0 ]; then
		echo ✅ $file
	else
	echo no
		echo ❌ $file
	fi
done;