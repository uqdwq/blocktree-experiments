##! /bin/bash
#pushd . > /dev/null
#SCRIPT_PATH="${BASH_SOURCE[0]}";
#while([ -h "${SCRIPT_PATH}" ]); do
#    cd "`dirname "${SCRIPT_PATH}"`"
#    SCRIPT_PATH="$(readlink "`basename "${SCRIPT_PATH}"`")";
#done
#cd "`dirname "${SCRIPT_PATH}"`" > /dev/null
#SCRIPT_PATH="`pwd`";
#popd  > /dev/null
echo id,text_id,textsize,treesize,constr_time,peak_mem,access_avg,constr_time_rs,treesize_rs,rank_avg,s,t,l,result
#./out/build/BlockTreesExperiments -p "32MiB" ./data/cere
./out/build/BlockTreesExperiments -p "16MiB" ./data/coreutils
#./out/build/BlockTreesExperiments -p "32MiB" ./data/dblp.xml
#./out/build/BlockTreesExperiments -p "32MiB" ./data/einstein.en.txt
#./out/build/BlockTreesExperiments -p "1MiB" ./data/english
#./out/build/BlockTreesExperiments -p "100MiB" ./data/Escherichia_Coli
#./out/build/BlockTreesExperiments -p "100MiB" ./data/influenza
#./out/build/BlockTreesExperiments -p "32MiB" ./data/kernel
#./out/build/BlockTreesExperiments -p "32MiB" ./data/para
#./out/build/BlockTreesExperiments -p "32MiB" ./data/pitches
#./out/build/BlockTreesExperiments -p "32MiB" ./data/sources
#./out/build/BlockTreesExperiments -p "32MiB" ./data/world_leaders
