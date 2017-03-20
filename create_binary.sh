hg clone https://bitbucket.org/alanmi/abc
cp fim/*.c abc/src/base/main/
cp fim/*.h abc/src/base/main/
rm fim/main.o
rm fim/fim_generate_final_faults.o
rm fim/fim_misc.o
rm fim/fim_ntk_circuits_generator.o
rm fim/fim_obvious_pairs.o
rm fim/fim_pregroup_faults_generator.o
rm fim/fim_read_and_preprocess.o
rm fim/fim_write_result_in_file.o
cd abc
make clean
make libabc.a OPTFLAGS="-g -O3" RELEASE=1
cp libabc.a ../fim
gcc -Wall -O3 -c src/base/main/main.c -o ../fim/main.o -fopenmp -Wno-unused-function -Wno-write-strings -Wno-sign-compare -DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -DABC_USE_CUDD=1 -DABC_USE_READLINE -DABC_USE_PTHREADS -Wno-unused-but-set-variable -Isrc
gcc -Wall -O3 -c src/base/main/fim_generate_final_faults.c -o ../fim/fim_generate_final_faults.o -fopenmp -Wno-unused-function -Wno-write-strings -Wno-sign-compare -DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -DABC_USE_CUDD=1 -DABC_USE_READLINE -DABC_USE_PTHREADS -Wno-unused-but-set-variable -Isrc 
gcc -Wall -O3 -c src/base/main/fim_misc.c -o ../fim/fim_misc.o -fopenmp -Wno-unused-function -Wno-write-strings -Wno-sign-compare -DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -DABC_USE_CUDD=1 -DABC_USE_READLINE -DABC_USE_PTHREADS -Wno-unused-but-set-variable -Isrc x
gcc -Wall -O3 -c src/base/main/fim_ntk_circuits_generator.c -o ../fim/fim_ntk_circuits_generator.o -fopenmp -Wno-unused-function -Wno-write-strings -Wno-sign-compare -DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -DABC_USE_CUDD=1 -DABC_USE_READLINE -DABC_USE_PTHREADS -Wno-unused-but-set-variable -Isrc 
gcc -Wall -O3 -c src/base/main/fim_obvious_pairs.c -o ../fim/fim_obvious_pairs.o -fopenmp -Wno-unused-function -Wno-write-strings -Wno-sign-compare -DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -DABC_USE_CUDD=1 -DABC_USE_READLINE -DABC_USE_PTHREADS -Wno-unused-but-set-variable -Isrc 
gcc -Wall -O3 -c src/base/main/fim_pregroup_faults_generator.c -o ../fim/fim_pregroup_faults_generator.o -fopenmp -Wno-unused-function -Wno-write-strings -Wno-sign-compare -DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -DABC_USE_CUDD=1 -DABC_USE_READLINE -DABC_USE_PTHREADS -Wno-unused-but-set-variable -Isrc 
gcc -Wall -O3 -c src/base/main/fim_read_and_preprocess.c -o ../fim/fim_read_and_preprocess.o -fopenmp -Wno-unused-function -Wno-write-strings -Wno-sign-compare -DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -DABC_USE_CUDD=1 -DABC_USE_READLINE -DABC_USE_PTHREADS -Wno-unused-but-set-variable -Isrc 
gcc -Wall -O3 -c src/base/main/fim_write_result_in_file.c -o ../fim/fim_write_result_in_file.o -fopenmp -Wno-unused-function -Wno-write-strings -Wno-sign-compare -DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -DABC_USE_CUDD=1 -DABC_USE_READLINE -DABC_USE_PTHREADS -Wno-unused-but-set-variable -Isrc 
cd ../fim
rm ./equiv.exe
gcc -O3 -fopenmp -o equiv.exe fim_generate_final_faults.o fim_misc.o fim_ntk_circuits_generator.o fim_obvious_pairs.o fim_pregroup_faults_generator.o fim_read_and_preprocess.o fim_write_result_in_file.o main.o libabc.a -lm -ldl -rdynamic -lreadline -ltermcap -lpthread -lrt
cp ./equiv.exe ../
