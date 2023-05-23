len=`wc --bytes $1 | cut -f1 -d\ `
hex=`printf "%08x" $len`
ofs=$(((256+2)*1024*1024-4-len))
ofshex=`printf "%08x" $ofs`
printf "\x${hex:6:2}\x${hex:4:2}\x${hex:2:2}\x${hex:0:2}" >> test.bin
