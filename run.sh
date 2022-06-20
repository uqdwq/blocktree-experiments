#! /bin/bash
pushd . > /dev/null
SCRIPT_PATH="${BASH_SOURCE[0]}";
while([ -h "${SCRIPT_PATH}" ]); do
    cd "`dirname "${SCRIPT_PATH}"`"
    SCRIPT_PATH="$(readlink "`basename "${SCRIPT_PATH}"`")";
done
cd "`dirname "${SCRIPT_PATH}"`" > /dev/null
SCRIPT_PATH="`pwd`";
popd  > /dev/null
code="$(date +%Y%m%d%H%M%S)"
mkdir bench/run$code
# bench my own with s tau and leaf for different sizes
sB="1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536"
Field_Separator=$IFS
tau="2,4,8,16"
leaf="4,8,16,32,64"
IFS=,

touch bench/run$code/cmpr.data
sizeD="1,2,4,8,16,32,64,128"
sizeE="1,2,4,8,16,32,64,128,256"
#sizeD="1,2,4,8,16,32,64,128"
factor="MiB"
for s in $sizeD; do
    echo "======${s}MB=====" >> bench/run$code/cmpr.data
    "$SCRIPT_PATH"/out/build/BlockTreesExperiments -p "${s}${factor}" -s 1 -t 2 -l 32 -m 3   "$SCRIPT_PATH"/data/dna.200MB >>  bench/run$code/cmpr.data
done
echo "======${SCRIPT_PATH}/data/dna.200MB=====-s 1 -t 2 -l 32" >> bench/run$code/cmpr.data
for s in $sizeD; do
    echo "======${s}MB=====" >> bench/run$code/cmpr.data
    echo "======${s}MB====="
    "$SCRIPT_PATH"/out/build/BlockTreesExperiments -p "${s}${factor}" -s 1 -t 2 -l 32 -m 1   "$SCRIPT_PATH"/data/dna.200MB >>  bench/run$code/cmpr.data
    "$SCRIPT_PATH"/out/build/BlockTreesExperiments -p "${s}${factor}" -s 1 -t 2 -l 32 -m 2   "$SCRIPT_PATH"/data/dna.200MB >>  bench/run$code/cmpr.data
done
echo "======${SCRIPT_PATH}/data/english.1024MB=====-s 1 -t 2 -l 32" >> bench/run$code/cmpr.data

#sizeE="1,2,4,8"
for s in $sizeE; do
    echo "======${s}MB=====" >> bench/run$code/cmpr.data
    "$SCRIPT_PATH"/out/build/BlockTreesExperiments -p "${s}${factor}" -s 1 -t 2 -l 32 -m 1   "$SCRIPT_PATH"/data/english.1024MB >>  bench/run$code/cmpr.data
    "$SCRIPT_PATH"/out/build/BlockTreesExperiments -p "${s}${factor}" -s 1 -t 2 -l 32 -m 2   "$SCRIPT_PATH"/data/english.1024MB >>  bench/run$code/cmpr.data   
done

# bench mine vs given for different sizes;
touch bench/run$code/stl.csv
# echo "size,s,tau,leafsize,time,endmem,peakmem" >> bench/run$code/stl.csv
# for s in $sB
# do
#     for t in 2 4 8 16 32 64
#     do
#         for l in 4 8 16 32 64
#         do
#             "$SCRIPT_PATH"/out/build/BlockTreesExperiments -p "10${factor}" -s "${s}" -t "${t}" -l "${l}" -m 0   "$SCRIPT_PATH"/data/data/dna.200MB >>  bench/run$code/stl.csv
#         done
#     done
# done