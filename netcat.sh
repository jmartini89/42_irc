if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	nc -v -C bendermac 8080
elif [[ "$OSTYPE" == "darwin"* ]]; then
	nc -v -c 127.0.0.1 8080
else
	echo wut
fi