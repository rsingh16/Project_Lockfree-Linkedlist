#!/bin/bash

for i in 4 6 8
do
	echo Lock
	./mylocks 20000 $i
	echo LockFree
	./mylockfree 20000 $i
done
