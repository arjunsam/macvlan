macvlan: macvlan.o
	gcc -o macvlan macvlan.o
macvlan.o: 
	gcc -c macvlan.c

clean:
	rm macvlan.o
	rm macvlan
