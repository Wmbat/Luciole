echo Building Luciole in Debug mode with Tests.

mkdir build -p

git submodule update --init --recursive

cmake . -B build -DCMAKE_BUILD_TYPE=Debug -Dtest=ON
