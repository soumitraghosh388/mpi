
#!/bin/bash
#mpic++ 3.cpp
mpic++ 3.c
for i in {1..12}
do
    for j in {1..6}
    do
	echo Process $i test $j
        #mpirun  --use-hwthread-cpus --oversubscribe -np $i ./a.out < ../test_cases/3/$j.in > $j.out
        #cp ../test_cases/3/$j.in 3_input.in
	mpirun  -np $i ./a.out < ../test_cases/3/$j.in > $j.out
	#echo $i >> $j.diff
	#sleep 2
        diff $j.out ../test_cases/3/$j.out >> $j.diff
    done
done
