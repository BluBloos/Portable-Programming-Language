mkdir bin

pushd bin

g++ -g ../src/ppl.cpp -I ../src/ -o ppl
g++ -g ../src/tests.cpp -I  ../src/ -o tests

popd