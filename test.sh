#!/bin/bash
if [ $# -lt 1 ];then 					#pocet cisel bud zadam nebo 16 :)
    numbers=16;
else
    numbers=$1;
fi;
mpic++ --prefix /usr/local/share/OpenMPI -o pms pms.cpp				#preklad cpp zdrojaku
# if-vstup, of-vystup, bs-velikost znaku, count-pocet znaku, status-netiskne info, 3 radek, zbytek vystup STDERR pryc
dd if=/dev/urandom bs=1 count=$numbers of=numbers status=noxfer > /dev/null 2>&1    #vyrobeni souboru s random cisly
numbers=`echo "scale=2 ; l($numbers) / l(2)" | bc -l`					#vypocet poctu procesoru
#echo $numbers
numbers=`echo "(($numbers+0.5)/1)+1" | bc`						      #zaokrouhleni na cela cisla +1
#echo $numbers
#mpirun --prefix /usr/local/share/OpenMPI -np $numbers xterm -e gdb pms			#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $numbers ./pms			#spusteni
rm -f pms numbers					                                    #uklid
