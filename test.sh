#! /bin/sh

EXIT_CODE=0
for file in samples/*.class; do
	./out/rum $file > /dev/null
	if [ $? -eq 0 ]; then
		echo ✅ $file
	else
		echo ❌ $file
		EXIT_CODE=1
	fi
done;

exit $EXIT_CODE