mkdir bin

pushd bin

g++ -g ../src/ppl.cpp -I ../src/ -o ppl.exe

popd