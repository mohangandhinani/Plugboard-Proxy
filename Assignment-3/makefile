make_rule1: pbproxy.c
	gcc pbproxy.c client.c server.c -o pbproxy -lcrypto

debug:
	gcc -g pbproxy.c client.c server.c -o pbproxy -lcrypto


clean:
	rm -rf pbproxy
