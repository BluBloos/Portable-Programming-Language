mkdir bin

pushd bin

g++ -g -O0 ../src/ppl.cpp -I ../src/ -o ppl
g++ -g ../src/tests.cpp -I  ../src/ -o tests

popd