
#!/bin/bash
mpic++ 1B.c
for i in {1..12}
do
    for j in {1..5}
    do
        #mpirun  --use-hwthread-cpus --oversubscribe -np $i ./a.out < ../test_cases/1B/$j.in > $j.out
	echo $i, $j
        mpirun  -np $i ./a.out < ../test_cases/1B/$j.in > $j.out
        diff $j.out ../test_cases/1B/$j.out > $j.diff
    done
done
