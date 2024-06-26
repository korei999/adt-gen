#!/bin/sh
set -e
set -x

release()
{
    rm -rf build
    if cmake -GNinja -S . -B build/ -DCMAKE_BUILD_TYPE=Release
    then
        cmake --build build/ -j -v
    fi
}

default()
{
    rm -rf build
    if cmake -GNinja -S . -B build/ -DCMAKE_BUILD_TYPE=RelWithDebInfo
    then
        cmake --build build/ -j -v
    fi
}

debug()
{
    rm -rf build
    if CC=clang CXX=clang++ CC_LD=mold CXX_LD=mold cmake -G "Ninja" -S . -B build/ -DCMAKE_BUILD_TYPE=Debug
    then
        cmake --build build/ -j -v
    fi
}

asan()
{
    rm -rf build
    if CC=clang CXX=clang++ CC_LD=mold CXX_LD=mold cmake -G "Ninja" -S . -B build/ -DCMAKE_BUILD_TYPE=Asan
    then
        cmake --build build/ -j -v
    fi
}

build()
{
    cmake --build build/ -j -v
}

run()
{
    BIN=test2

    if cmake --build build/ -j -v
    then
        echo ""
        # ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=leaks.txt ./build/$BIN "$@" # 2> /tmp/$BIN-dbg.txt
        # ASAN_OPTIONS=halt_on_error=0 ./build/$BIN "$@" # 2> /tmp/$BIN-dbg.txt
        ./build/$BIN "$@" # 2> /tmp/$BIN-dbg.txt
    fi
}

_install()
{
    cmake --install build/
}

_uninstall()
{
    sudo xargs rm < ./build/install_manifest.txt
}

cd $(dirname $0)

case "$1" in
    default) default ;;
    run) run "$@" ;;
    debug) debug ;;
    asan) asan ;;
    release) release ;;
    install) _install ;;
    uninstall) _uninstall ;;
    *) build ;;
esac
