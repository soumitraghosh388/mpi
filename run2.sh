
#!/bin/bash
#mpic++ 2.cpp
mpic++ 2.c
for i in {1..12}
do
    for j in {1..7}
    do
        #mpirun  --use-hwthread-cpus --oversubscribe -np $i ./a.out < ../test_cases/2/$j.in > $j.out
        #cp ../test_cases/2/$j.in 2_input.in
	mpirun  -np $i ./a.out < ../test_cases/2/$j.in > $j.out
        echo $i, $j
	diff $j.out ../test_cases/2/$j.out >> $j.diff
    done
done
