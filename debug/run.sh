cd ../
if make
then
	cd debug/
	./run-qemu.sh &
	pidQemu=$!
	./run-gdb.sh
	kill $pidQemu
	killall qemu-system-arm
else
	print 'Compilation fault';
fi
